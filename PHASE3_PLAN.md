# Phase 3 (Bucket B) implementation plan

Each step below is sized to fit one Claude Code session comfortably. Run them in order. Each one assumes:

- The implementing session reads **CLAUDE.md** first for code style, signal flow, parameter inventory, macro derivations, and implementation invariants.
- After each step: build + tests must be green before the user commits and moves on.
- Build verification: `cmake --build --preset=macos-release --target CenterSpace_All 2>&1 | grep -E "error:|\*\* BUILD"`
- Test harness: `./build/macos/centerspace_tests_artefacts/Debug/centerspace_tests` or `cd build/macos && ctest -C Debug --output-on-failure`

---

## Step 1 — Add v2.0 APVTS parameter inventory

**Goal:** Add all 22 v2.0 APVTS parameters to `CenterSpaceAudioProcessor`. No DSP changes — params exist but aren't yet read by `processBlock`. Wire raw-pointer access for all new params. Build green; old behavior preserved.

**Prompt:**
> Read CLAUDE.md, especially the "v2.0 parameter inventory" section.
>
> Add all 22 APVTS parameters listed under "Shared", "Tweak-mode primitives", and "Vibe-mode macros" to `CenterSpaceAudioProcessor`'s `parameters` constructor in `Source/Plugin/PluginProcessor.cpp`. The existing v1 params (`inGain`, `sideInGain`, `attack`, `release`, `threshold`, `ratio`, `outGain`, `peakRMS`) are already present — keep them, update defaults per the inventory (attack default = 10.0, release default = 100.0). Add the 14 new params:
>
> Shared: `uiMode`, `inputType`, `outputType`, `bypass`. (`inGain` and `outGain` already exist.)
> Tweak primitives (new): `scHpfHz`, `scLpfHz`, `style`, `knee`, `lookahead`.
> Vibe macros (new): `feel`, `compress`, `react`, `focus`, `lookaheadOnOff`.
>
> All new params use `versionHint = 1`. For `focus`, use a `juce::StringArray` of the 15 preset names from CLAUDE.md.
>
> In `PluginProcessor.h`, add raw-pointer members (`std::atomic<float> *` for floats and choices, `juce::AudioParameterBool *` for `bypass`) initialised to `nullptr`. In the constructor body, assign each via `parameters.getRawParameterValue(...)` (or the bool's equivalent for `bypass`).
>
> Do **not** modify `processBlock`. The new params should be reachable but unused — `processBlock` keeps using the existing primitives. The point of this step is to land the param structure cleanly without behavioral changes.
>
> Verify with: full release build green, test harness green. Note the new defaults will alter test expectations for attack/release if anything reads them — leave the test harness as is unless it actually fails.

---

## Step 2 — Atomic metering members

**Goal:** Convert the cross-thread level/GR float members to `std::atomic<float>`. Update editor reads to use `.load()`. Trivial change, sets up safer multithreaded behavior before further refactoring.

**Prompt:**
> Read CLAUDE.md, especially the "Audio thread / message thread split" and "Implementation invariants" sections.
>
> In `Source/Plugin/PluginProcessor.h`, change the public level members to atomics:
> ```cpp
> std::atomic<float> inMidLevel     { 0.0f };
> std::atomic<float> inLeftLevel    { 0.0f };
> std::atomic<float> inRightLevel   { 0.0f };
> std::atomic<float> inSideLevel    { 0.0f };
> std::atomic<float> sideChainLevel { 0.0f };
> std::atomic<float> outLeftLevel   { 0.0f };
> std::atomic<float> outMidLevel    { 0.0f };
> std::atomic<float> outRightLevel  { 0.0f };
> std::atomic<float> gainReduction  { 0.0f };
> ```
>
> In `Source/Plugin/PluginProcessor.cpp` `processBlock`, the post-loop assignments (e.g. `inLeftLevel = inLeftBuffer.getRMSLevel(...)`) become atomic stores — they work syntactically because `std::atomic<float>` has an `operator=` for the underlying type. No semantic change needed.
>
> In `Source/Plugin/PluginEditor.cpp` `timerCallback`, the reads (e.g. `audioProcessor.inLeftLevel`) need `.load()` calls. Update each call site. Use `std::memory_order_relaxed` if you want to be explicit; the default sequential ordering is also fine here since contention is one-way.
>
> Verify: build green, plugin loads in Standalone or DAW without crash, meters still update.

---

## Step 3 — Effective-value derivation layer

**Goal:** Build the mode-aware translation layer between APVTS params and the DSP. This is the architectural core of Vibe/Tweak independence. The `SmoothedValue` chain in `processBlock` will read from these derivation functions instead of the params directly.

**Prompt:**
> Read CLAUDE.md, especially the "Vibe / Tweak modes" and "Vibe-mode macro derivations" sections. This step is the structural heart of mode independence — get it right.
>
> Add a set of `private` methods to `CenterSpaceAudioProcessor` that return the **effective** DSP value for each driver-of-DSP parameter, branching on `uiMode`:
>
> ```cpp
> float       GetEffectiveSideInGainDb() const;
> float       GetEffectiveScHpfHz() const;
> float       GetEffectiveScLpfHz() const;
> int         GetEffectivePeakMode() const;      // 0 = Peak, 1 = RMS
> float       GetEffectiveAttackMs() const;
> float       GetEffectiveReleaseMs() const;
> int         GetEffectiveStyle() const;         // 0 = Modern VCA, 1 = Opto
> float       GetEffectiveThresholdDb() const;
> float       GetEffectiveRatio() const;
> float       GetEffectiveKneeDb() const;
> int         GetEffectiveLookaheadSamples() const;   // 0 / 1ms / 4ms / 10ms × sample rate
> ```
>
> When `uiMode == Tweak`, each returns the corresponding Tweak primitive directly (with appropriate atomic `.load()` and unit conversions).
>
> When `uiMode == Vibe`, derive from the macros per the rules in CLAUDE.md's "Vibe-mode macro derivations" section:
> - `feel` → bakes style, peakRMS, knee, lookahead-on value, and attack/release ranges
> - `compress` (0–1) → sideInGain (0 → +9 dB linear interp), threshold (0 → -30 dB linear interp), ratio (1 → 10:1 linear interp)
> - `react` (0–1) → linear interp of attack and release within Feel's range
> - `focus` choice → HPF/LPF pair via lookup table (use the 15 entries from CLAUDE.md)
> - `lookaheadOnOff` → 0 ms or Feel-dependent value (Clean: 1 ms, Smooth: 4 ms)
>
> Style-conditional rules ALSO apply when `uiMode == Tweak`:
> - If `style == Opto`: `GetEffectivePeakMode()` returns 1 (RMS) regardless of `peakRMS` value, and `GetEffectiveKneeDb()` returns 12.0 regardless of `knee` value.
> - If `style == Modern VCA`: pass through user values.
>
> Do **not** call these from `processBlock` yet. Just define them. Add a small set of test-harness assertions in `Tests/TestMain.cpp` that verify derivation math at a few control points (e.g. `compress=0.5` → expected sideInGain/threshold/ratio; Feel=Smooth + react=0 → expected attack/release; focus presets each return the right HPF/LPF; Opto forces RMS+12 dB knee regardless of param values).
>
> Verify: build green, test harness green including new assertions.

---

## Step 4 — Wire `processBlock` to derivation layer + add SC HPF/LPF

**Goal:** Replace the direct param reads in `processBlock` with calls to the derivation functions. Add the two `juce::dsp::StateVariableTPTFilter<float>` instances for SC HPF and LPF. Smooth their cutoffs. Insert in the SC signal chain before the envelope follower.

**Prompt:**
> Read CLAUDE.md, especially the "v2.0 signal flow" and "Implementation invariants" sections.
>
> Two related changes in this step:
>
> **A. Re-wire `processBlock` to read from `GetEffective*()` functions** (defined in Step 3). For every per-block param read (the lines that call `setTargetValue(...)` on the SmoothedValues), replace the direct atomic `.load()` with the corresponding `GetEffective*()` call. Example:
>
> ```cpp
> // before
> sideGainSmoothed.setTargetValue(decibels.decibelsToGain(sidechainInGainParam->load()));
> // after
> sideGainSmoothed.setTargetValue(decibels.decibelsToGain(GetEffectiveSideInGainDb()));
> ```
>
> Same for `inGainSmoothed`, `outGainSmoothed`, `thresholdSmoothed`, `ratioReciprocalSmoothed`. Update peakMode load too. The `BallisticsFilter::setAttackTime/setReleaseTime/setLevelCalculationType` calls use the effective values.
>
> After this, the DSP behavior should be **identical to current** when `uiMode == Tweak` and all values match. When `uiMode == Vibe`, derivations kick in.
>
> **B. Add SC HPF + LPF.** Add two `juce::dsp::StateVariableTPTFilter<float>` members for the SC HPF and LPF. Add two `juce::SmoothedValue<float>` members for the cutoff frequencies. In `prepareToPlay`, call `prepare(spec)` on each filter (mono ProcessSpec, channel count 1) and reset the smoothed values to the effective starting cutoffs.
>
> Configure the filters: HPF as `juce::dsp::StateVariableTPTFilterType::highpass`, LPF as `lowpass`. Resonance left at default (no Q control exposed).
>
> In `processBlock` after the per-block setup, set target cutoffs from `GetEffectiveScHpfHz()` and `GetEffectiveScLpfHz()`. Inside the sample loop, before passing `monoSidechainSample` to the envelope: update filter cutoffs from the smoothed values (`setCutoffFrequency(scHpfSmoothed.getNextValue())`), then `monoSidechainSample = scHpf.processSample(0, monoSidechainSample); monoSidechainSample = scLpf.processSample(0, monoSidechainSample);`
>
> Add a test-harness assertion: SC filters should attenuate frequencies outside their pass band by the expected amount (test with sine input at known frequencies, e.g. HPF at 200 Hz should attenuate 50 Hz by roughly 12 dB/oct × 2 octaves = 24 dB).
>
> Verify: build green, test harness green, plugin still loads and behaves like Tweak mode does today when uiMode == Tweak.

---

## Step 5 — Soft-knee dB-domain compressor formula

**Goal:** Replace the linear-domain `pow(envVal / threshold, ratioRecip - 1)` formula with the dB-domain knee-aware formula. Includes the `envVal` floor clamp.

**Prompt:**
> Read CLAUDE.md, especially the "Vibe-mode macro derivations" and "Implementation invariants" sections.
>
> Current static-curve math in `processBlock`:
> ```cpp
> const float compGain = (envVal < thresholdAmp)
>                            ? 1.0f
>                            : std::pow(envVal / thresholdAmp, ratioRecip - 1.0f);
> ```
>
> Replace with the textbook dB-domain soft-knee formula. Add a `kneeSmoothed` `SmoothedValue<float>` member (initialised and target-set per block from `GetEffectiveKneeDb()`).
>
> Per-sample:
> ```cpp
> const float safeEnvVal = juce::jmax(envVal, 1e-9f);   // mandatory: log10(0) = -inf
> const float envDb      = 20.0f * std::log10(safeEnvVal);
> const float threshDb   = 20.0f * std::log10(thresholdAmp);   // or pass dB directly
> const float kneeDb     = kneeSmoothed.getNextValue();
> const float overshoot  = envDb - threshDb;
> const float slope      = ratioRecip - 1.0f;   // negative for ratio > 1
>
> float gainDb;
> if (kneeDb <= 0.0f)
> {
>     gainDb = (overshoot <= 0.0f) ? 0.0f : slope * overshoot;
> }
> else if (overshoot <= -kneeDb * 0.5f)
> {
>     gainDb = 0.0f;
> }
> else if (overshoot >= kneeDb * 0.5f)
> {
>     gainDb = slope * overshoot;
> }
> else
> {
>     const float x = overshoot + kneeDb * 0.5f;
>     gainDb = slope * x * x / (2.0f * kneeDb);
> }
> const float compGain = juce::Decibels::decibelsToGain(gainDb);
> ```
>
> Notes:
> - Smooth the `thresholdDb` either by smoothing the dB value directly (preferred — avoids log10 per sample), or compute `threshDb` from `thresholdSmoothed.getNextValue()` once per sample. Pick one; the dB-direct path is faster.
> - Consider whether you want to smooth `kneeDb` in dB-domain too (you do; transitions between Modern's 0 dB knee and Opto's 12 dB knee need to ramp).
> - The min-comp-gain tracking for the GR meter (`minCompGain = juce::jmin(minCompGain, compGain);`) still works the same way.
>
> Add test-harness assertions:
> - Knee = 0: matches the old hard-knee formula at multiple input levels.
> - Knee = 6 dB: at threshold, gainDb is roughly slope × (kneeDb/2)² / (2 × kneeDb) = slope × kneeDb/8 (the curve's midpoint).
> - Knee = 24 dB: smooth transition is monotonic and continuous.
>
> Verify: build green, test harness green.

---

## Step 6 — Lookahead implementation

**Goal:** Add the audio-path delay line for L+R input (pre-encode). Report latency to the host. Handle lookahead choice changes mid-session.

**Prompt:**
> Read CLAUDE.md, especially the "v2.0 signal flow" (steps 1–2) and "Implementation invariants" sections.
>
> Add a stereo delay buffer for the L and R input channels. Max delay = 10 ms × max-supported sample rate (e.g. 192 kHz → 1920 samples; allocate 2048 for safety).
>
> Use either `juce::dsp::DelayLine<float>` with two channels, or a `juce::AudioBuffer<float>` used as a circular buffer. The DelayLine approach is simpler.
>
> In `prepareToPlay`:
> - Resize/prepare the delay line for the current sample rate and max delay.
> - Compute the current effective lookahead in samples via `GetEffectiveLookaheadSamples()`. Call `setLatencySamples(currentLookaheadSamples)` so the host knows about PDC.
>
> In `processBlock`:
> - At the top, before any DSP: get effective lookahead in samples. If it differs from the current value, update the delay line's delay amount and call `setLatencySamples(...)` again. (This can glitch briefly; acceptable per the spec.)
> - **Critical:** delay both L and R **before** M/S encoding. So your sample loop should: read `L_in = leftChannel[i]; R_in = rightChannel[i]`, push these into the delay line, pop the delayed L_delayed/R_delayed, then encode mid/side from those delayed values, not from the raw inputs.
> - The sidechain path remains undelayed.
>
> Add a test-harness assertion: with a known sample rate and a known lookahead choice, `setLatencySamples` returns the expected sample count. Use `processor.getLatencySamples()` to check.
>
> Verify: build green, test harness green. In a DAW (manual test), insert the plugin and set lookahead to 4 ms — confirm the host reports latency for that track and PDC compensates other tracks.

---

## Step 7 — Bypass via getBypassParameter()

**Goal:** Wire `bypass` param as the host's bypass driver. Skip compression DSP when bypassed but maintain delay state so PDC remains correct.

**Prompt:**
> Read CLAUDE.md, especially the "Implementation invariants" section.
>
> Override `juce::AudioProcessor::getBypassParameter()` to return the bypass param:
> ```cpp
> juce::AudioProcessorParameter *getBypassParameter() const override
> {
>     return parameters.getParameter("bypass");
> }
> ```
>
> Add `bypassParam` as a raw pointer member (`juce::AudioParameterBool *`, init nullptr in header, assigned in constructor body via `dynamic_cast<juce::AudioParameterBool *>(parameters.getParameter("bypass"))`).
>
> In `processBlock`, at the top after the buffer clears and per-block-setup:
> ```cpp
> const bool bypassed = bypassParam->get();
> ```
>
> If bypassed:
> - Still push input samples through the lookahead delay (so latency reporting stays accurate).
> - Skip M/S encode, compression DSP, M/S decode, gain stages.
> - The delayed L/R samples just pass through to the output channels.
> - Keep `SmoothedValue` `setTargetValue` calls active so leaving bypass doesn't cause a target jump from stale values.
> - Skip metering updates (or zero them out) — the user expects "plugin is doing nothing."
> - Early-return from `processBlock` after writing output.
>
> If not bypassed: existing path.
>
> Manual DAW test: insert plugin, hit host's bypass button, audio should pass through unchanged with no glitching. Toggle on/off rapidly — no clicks. Set lookahead to 4 ms and bypass mid-playback — other tracks stay aligned (test by having a different unbypassed plugin with delay compensation on a parallel track).
>
> Verify: build green, test harness green.

---

## Step 8 — Input / Output stereo type selectors

**Goal:** Implement `inputType` and `outputType` behavior so the plugin can act as an L/R ↔ M/S converter and skip its own encode/decode when needed.

**Prompt:**
> Read CLAUDE.md, especially the "v2.0 signal flow" steps 1 and 7.
>
> Add `inputTypeParam` and `outputTypeParam` as raw atomic pointer members (or use `getRawParameterValue` results stored in existing pattern).
>
> In `processBlock`, the M/S encode (currently `mid = L+R; side = L-R`) becomes conditional:
> ```cpp
> const bool inputIsMS  = ((int)inputTypeParam->load() == 1);   // 0 = LR, 1 = M/S
> const bool outputIsMS = ((int)outputTypeParam->load() == 1);
>
> // In sample loop, after lookahead delay:
> float mid, side;
> if (inputIsMS)
> {
>     mid  = leftDelayed  * inGainAmp;   // channel 0 is M
>     side = rightDelayed * inGainAmp;   // channel 1 is S
> }
> else
> {
>     mid  = (leftDelayed + rightDelayed) * inGainAmp;
>     side = (leftDelayed - rightDelayed) * inGainAmp;
> }
> ```
>
> Output side mirrors:
> ```cpp
> if (outputIsMS)
> {
>     leftChannel[i]  = midComped       * outGainAmp;
>     rightChannel[i] = side            * outGainAmp;
> }
> else
> {
>     leftChannel[i]  = (midComped + side) * outScale;   // outScale = outGainAmp * 0.5
>     rightChannel[i] = (midComped - side) * outScale;
> }
> ```
>
> Note that `outScale` (with the 0.5 gain compensation) is only used when decoding back to L/R. When passing M/S through, no gain compensation — just `outGainAmp`.
>
> Add test-harness assertions:
> - LR in, M/S out: stereo input produces (L+R) on channel 0, (L-R) on channel 1.
> - M/S in, LR out: feeding M and S directly outputs (M+S)*0.5, (M-S)*0.5.
> - M/S in, M/S out: passthrough with gain applied (no encode/decode).
> - LR in, LR out: existing round-trip behavior.
>
> Verify: build green, test harness green.

---

## Step 9 — Test harness expansion

**Goal:** Backfill any test gaps. By now the harness should cover: knee math, dB-domain static curve, SC filter response, lookahead latency reporting, Vibe macro derivations (Compress curve, React mapping, Feel bakes, Focus presets), and I/O type round-trips.

**Prompt:**
> Read CLAUDE.md.
>
> Audit `Tests/TestMain.cpp` for coverage gaps relative to the v2.0 feature set. Each of the following should have at least one assertion:
>
> - **Knee math** at multiple knee widths (0, 6, 12, 24 dB) and multiple input levels (well below, at, above, well above threshold). Includes the `envVal` floor clamp behavior (input of 0 should not produce NaN or infinity).
> - **SC filter response** — input sine at known frequency, verify attenuation matches expected 12 dB/oct rolloff.
> - **Lookahead latency reporting** — for each of the 4 choices (0, 1, 4, 10 ms), verify `processor.getLatencySamples()` returns the expected count at multiple sample rates (44.1k, 48k, 96k).
> - **Vibe macro derivations** — at minimum:
>   - `compress = 0, 0.5, 1.0` produces the expected sideInGain/threshold/ratio.
>   - `feel = Smooth` forces effective style = Opto, peakRMS = RMS, knee = 12 dB regardless of Tweak param values.
>   - `react = 0` and `react = 1` produce min and max of Feel's attack/release range.
>   - Each `focus` preset returns the expected HPF/LPF pair.
> - **Style-conditional behavior in Tweak mode** — when `style = Opto`, effective peakRMS is RMS and effective knee is 12 dB regardless of user-set values.
> - **I/O stereo type round-trips** — LR↔LR, LR↔MS, MS↔LR, MS↔MS all behave correctly.
>
> Add any missing assertions. Refactor the test file if it's getting unwieldy — split into per-area files (e.g. `Tests/KneeTests.cpp`, `Tests/MacroTests.cpp`) and update `CMakeLists.txt` to include them. Keep the `main()` entry simple.
>
> Verify: build green, ctest green.

---

## Step 10 — Quick-and-dirty GUI for new controls

**Goal:** Make every new parameter accessible from the plugin window so the user can test in a DAW. Aesthetic polish is Phase 4 — this is just functional UI.

**Prompt:**
> Read CLAUDE.md, especially the "Style-conditional UI visibility" section.
>
> The current `PluginEditor` lays out a stereo compressor UI for the v1 param set. We need to add UI for the new params:
>
> Tweak mode primitives (new sliders/comboboxes to add):
> - `scHpfHz` and `scLpfHz` — float sliders, Hz, log skew
> - `knee` — float slider, dB. **Visible only when `style == Modern VCA`.**
> - `style` — combo box / two-button selector (Modern VCA / Opto)
> - `lookahead` — choice control (0 / 1 / 4 / 10 ms). Buttons or combo box, your call.
> - `peakRMS` already exists — keep it but make it **visible only when `style == Modern VCA`**.
>
> Shared (new):
> - `uiMode` — Vibe / Tweak toggle (button or combo). Drives which mode-specific component is visible.
> - `inputType` and `outputType` — combo boxes (LR / M/S). Place them in non-prominent positions per Topic 6.
>
> Vibe mode macros (new — visible only when `uiMode == Vibe`):
> - `feel` — Clean / Smooth selector.
> - `compress` — float slider 0–1.
> - `react` — float slider 0–1.
> - `focus` — combo box with the 15 preset names.
> - `lookaheadOnOff` — toggle button.
>
> Tweak-mode controls (visible only when `uiMode == Tweak`): the existing 7 sliders + peakRMS combo, plus the 5 new Tweak primitives added above.
>
> **Implementation pattern:** create two `juce::Component` subclasses (`TweakModeComponent`, `VibeModeComponent`) that own the mode-specific controls. The editor instantiates both, adds both as children, and toggles `setVisible(true/false)` based on `uiMode`. Shared controls stay on the main editor.
>
> Style-conditional visibility (Knee and peakRMS hidden in Opto) is implemented by registering an APVTS parameter listener on `style` that calls `setVisible` on those two controls.
>
> Layout: this is throwaway. Just squeeze the new controls into the existing window using `getLocalBounds().removeFromX(...)` or a `juce::FlexBox`. No need for it to look good. Phase 4 will redesign everything.
>
> For each new control, create the corresponding `juce::AudioProcessorValueTreeState::SliderAttachment` / `ComboBoxAttachment` / `ButtonAttachment` so the UI binds to APVTS automatically.
>
> Manual test in DAW: insert plugin, confirm every parameter has a control, automation lists show all 22 params, switching uiMode swaps the visible component set, switching style in Tweak mode shows/hides knee + peakRMS.
>
> Verify: build green, plugin loads in Standalone + Ableton + AU host.

---

## Step 11 — End-to-end DAW verification

**Goal:** Make sure the whole thing works in real hosts before we declare Phase 3 done.

**Prompt (mostly for the user, but Claude can assist with bug hunts):**
> This step is hands-on verification in a DAW. Use Ableton Live as the primary host; ideally also test in Logic Pro and Reaper. Checklist:
>
> **General**
> - Plugin scans and instantiates without errors in each host.
> - GUI window opens at correct size, all controls visible.
> - All 22 params appear in the host's automation panel.
> - Patch state survives session save/reopen (the existing `getStateInformation` / `setStateInformation` machinery).
>
> **Modes**
> - Switching uiMode toggles visible component set without crash or glitch.
> - Adjusting Vibe macros does not modify the Tweak primitives, and vice versa.
> - Style switching in Tweak mode hides/shows Knee and Peak/RMS controls.
>
> **Audio**
> - Modern VCA + Peak: behaves like a clean fast comp.
> - Modern VCA + RMS: smoother, slower-feeling detection.
> - Opto: noticeably softer, with the wider baked knee.
> - SC HPF/LPF cutoffs change detection behavior audibly.
> - Lookahead reports latency to host (check host's "track delay" indicator with lookahead = 4 ms).
> - Bypass via host button: clean toggle, no clicks, no PDC drift.
> - I/O type selectors: with M/S input and M/S output, the plugin passes M and S as channels 0 and 1.
>
> **Vibe mode**
> - Compress slider produces expected behavior at 0 / 0.5 / 1.
> - React slider audibly speeds up / slows down the comp.
> - Feel = Smooth produces opto-like behavior.
> - Focus presets each give different detection character.
>
> **Edge cases**
> - No SC routed: plugin does no compression but still applies in/out gain. (Topic 11 spec.)
> - Switching styles under heavy GR: small audible artifact is acceptable; no NaN/clicks/explosion.
> - Loading old v1 patches: should mostly work for existing param names; new params get defaults.
>
> If bugs found, file as TODOs and open a fresh Claude Code session to fix one at a time. Reference this plan and CLAUDE.md.
>
> Phase 3 is done when this checklist is green.

---

## After Phase 3

- **Phase 4 (GUI redesign):** Vibe/Tweak full UI design, mid meter scaling, SC silence indicator, patch system (`FullumMusicModules::PatchManager`), A/B compare (`FullumMusicModules::ABCompareManager`), bypass UI fold-in.
- **Phase 5 (release prep):** LICENSE, README polish, screenshots, code-sign + notarize, v2.0 tag.

---

## How to use this plan

1. Open a new Claude Code session in the project root.
2. Copy the prompt for the next step into the session.
3. Let it work. Watch for build/test failures.
4. Commit when green. Move to the next step.

If a step turns out larger than expected, split it. If a step's prompt missed something, update this file and CLAUDE.md before the next session.
