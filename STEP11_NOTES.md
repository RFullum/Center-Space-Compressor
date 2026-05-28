# Step 11 — DAW verification notes

Working scratchpad for Phase 3 Step 11 (end-to-end DAW verification). **Phase 3 complete.**

---

## Status

| # | Item | Status | Host(s) | Notes |
|---|---|---|---|---|
| 1 | Plugin scans & instantiates | PASS | Ableton Live | |
| 2 | GUI opens at correct size, all controls visible | PASS | Ableton Live | |
| 3 | All 22 params in automation panel | PASS | Ableton Live | |
| 4 | State survives session save/reopen | PASS | Ableton Live | |
| 5 | uiMode switch (Vibe↔Tweak) toggles component set | PASS | Ableton Live | |
| 6 | Vibe ↔ Tweak param independence | PASS | Ableton Live | |
| 7 | Style switch hides/shows Knee + Peak/RMS | PASS | Ableton Live | |
| 8 | Audio behavior — Modern VCA + Peak, + RMS, Opto | PASS | Ableton Live | |
| 9 | Bypass via host: clean toggle, no clicks, no PDC drift | PASS | Ableton Live | |
| 10 | I/O type selectors — M/S in + M/S out passthrough | PASS | Ableton Live | |
| 11 | Vibe-mode audio behavior (compress, react, feel, focus) | PASS | Ableton Live | param response confirmed; slider *feel* deferred to Phase 5 |
| 11a | Feel of Compress/React linear sweeps + skews | DEFERRED to Phase 5 | | Needs real-world mixing context — see CLAUDE.md Phase 5 note |
| 12 | SC HPF/LPF audibly affect detection | PASS | Ableton Live | |
| 13 | Lookahead reports latency (4 ms) | PASS | Ableton Live | |
| 14 | No SC routed: no compression, gain still applies | PASS | Ableton Live | |
| 15 | Style switch under heavy GR — no NaN/clicks | PASS | Ableton Live | |
| 16 | Load old v1 patches — new params default | N/A | | No v1 patches available to test |

---

## Issues found and resolved during Step 11

### 1. VUMeter output Mid doesn't visibly reduce under compression (FLAGGED FOR PHASE 4)

**Symptom:** With aggressive comp, the GR meter shows immense reduction but the output Mid bar barely drops. Signal flow verified correct via `TestSignalChainGainReduction` (19 dB GR → 32% bar drop). Purely a display-scaling concern. Floor tightened from -100 dB to -60 dB in Phase 3. Final pro-comp range to be decided alongside the new meter visuals in Phase 4. See METER_DESIGN_NOTES.md.

### 2. Per-block envelope clicks during compression (FIXED)

**Symptom:** ~7000 sample-level discontinuities in an 8-second wet capture, periodic at the host's 64-sample block size. Spike size proportional to current GR depth.

**Root cause:** `juce::dsp::BallisticsFilter::setLevelCalculationType()` calls `reset()` internally ([juce_BallisticsFilter.cpp:60-65](JUCE/modules/juce_dsp/processors/juce_BallisticsFilter.cpp)). `processBlock` was calling it every block, zeroing envelope state at every block boundary. Each block, the envelope briefly reported ~0, compGain jumped back toward 1.0, the audio briefly became un-compressed (= the click), then the envelope caught up.

**Fix:** Cached `lastAppliedPeakMode` / `lastAppliedAttackMs` / `lastAppliedReleaseMs` in PluginProcessor; only call the JUCE setters when the value actually changes. Sentinels reset in `prepareToPlay` so sample-rate changes still propagate. Gotcha entry added to CLAUDE.md.

---

## Phase 3 sign-off

All blocking checklist items PASS. Phase 3 is **done**. Outstanding deferrals are all Phase 4 (GUI) or Phase 5 (release polish) territory:

- Meter display range — Phase 4 GUI redesign
- Stereo-field visualization concept — Phase 4 (see METER_DESIGN_NOTES.md)
- Slider skew tuning — Phase 5 release prep with realistic source material
