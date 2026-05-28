# Center Space — Working Notes

JUCE 8 / C++20 stereo sidechain compressor that only compresses the center (mid) of the stereo field. By Robert Fullum / FullumMusic.

Read this before doing anything in the repo.

---

## Build

CMake-based. Projucer is not used in this project.

```bash
# Configure (once):
cmake --preset=macos

# Build:
cmake --build --preset=macos-debug
cmake --build --preset=macos-release

# Specific target:
cmake --build --preset=macos-release --target CenterSpace_VST3
cmake --build --preset=macos-release --target CenterSpace_All
```

Successful builds auto-install:
- VST3 → `~/Library/Audio/Plug-Ins/VST3/Center Space.vst3`
- AU → `~/Library/Audio/Plug-Ins/Components/Center Space.component`
- Standalone .app stays in `build/macos/CenterSpace_artefacts/<Config>/Standalone/`

Don't push without explicit ask. The user manages branches.

---

## Project layout

```
Source/
  Plugin/                     PluginProcessor, PluginEditor
  DSP/                        Compressor (static curve + envelope + SC filters), CompressorTypes (enums)
  GUI/
    LookAndFeel/              OtherLookAndFeel, BoxLookAndFeel
    TitleHeader/              TitleHeader, TitleFooter
    VUMeter/                  VUMeter, ReduceMeter
Resources/                    Logo PNG (compiled in via juce_add_binary_data)
JUCE/                         JUCE submodule, pinned near 8.0.13
CMakeLists.txt
CMakePresets.json
```

Header search paths cover every `Source/*` subfolder, so `#include "Foo.h"` works from anywhere — no relative path prefixes needed in source files.

Directory names are **capitalized** (`Source/`, not `source/` or `sources/`). Singular `Source/`, never `Sources/`.

---

## Signal flow (v2.0)

`processBlock` receives a stereo bus + an external sidechain bus. The flow:

1. **Input stereo type check** — if `inputType == LR`, encode L/R → M/S; if `M/S`, treat channels as-is.
2. **Lookahead delay** (when active) — applied to **L and R before M/S encoding** so mid and side stay aligned.
3. **Input gain** scales the encoded mid (side is untouched).
4. **Sidechain path:**
   - External SC mono-summed and scaled by `sideInGain`.
   - **HPF → LPF** (detector-only; never enters audio output).
   - **`juce::dsp::BallisticsFilter`** envelope follower (peak or RMS).
5. **Static curve** (dB-domain, soft-knee aware) computes per-sample `compGain`.
6. **Mid × compGain.** Side passes through uncompressed.
7. **Output stereo type check** — if `outputType == LR`, decode M/S → L/R; if `M/S`, leave encoded.
8. **Output gain** applied last.

If no SC signal is present, the compressor does nothing. The plugin still performs I/O stereo-type conversion and applies in/out gain. No internal-SC fallback.

Result: the center collapses under sidechain transients while the stereo width stays at full level. With `inputType` / `outputType` set to M/S, the plugin can act as a stereo-type converter as a bonus.

---

## Code style — non-negotiable

Match these exactly when writing or editing code.

### Spacing & punctuation

- **No space between cast type and value:** `(float)x`, not `(float) x`. Same for `(int)`, `(size_t)`, `(juce::uint8)`. C-style cast in function signatures like `(int) override` is the exception.
- **Pointer/reference symbols stick to the variable name:** `juce::Slider &slider`, `std::atomic<float> *ptr`. Not `juce::Slider& slider`.
- **No space between function name and parens:** `getBounds()`, not `getBounds ()`.

### Braces & indentation

- **Allman braces** — opening `{` on its own line for functions, classes, control flow.
- **Tabs set to 4 spaces in Xcode.** Mix of tabs and spaces is acceptable when aligning — don't normalize indentation unless asked.
- One blank line between methods inside a class; two blank lines between unrelated classes in the same file.

### Member initialization

**Default values go on the declaration in the header**, not in the constructor's init list:

```cpp
// in the .h
private:
    float sampleRate = 44100.0f;
    int   blockSize  = 0;
    std::atomic<float> *gainParam = nullptr;
    juce::Colour bgColor { (juce::uint8)53, (juce::uint8)59, (juce::uint8)60, (juce::uint8)255 };
```

Reasons: cleaner default values stay next to the type, and you can't accidentally forget to initialize a new member when you add one — the header default catches it.

**The constructor's init list is only for things that *must* go there:** base-class initialisation, references, `const` members, and members whose initial value depends on a constructor argument.

When an init list is required, use comma-first, leading colon on its own line, aligned:

```cpp
MyClass::MyClass(int argA)
: baseClass(somethingNeededHere)
, memberThatDependsOnArg(argA * 2)
, memberRef(getSomeReference())
{}
```

### Wrapped function arguments

When wrapping call/declaration args, put the comma at the **start** of the next line, aligned with the first arg:

```cpp
CenterSpace::SetupSlider(this
                         , slider
                         , juce::Slider::SliderStyle::LinearVertical
                         , accent
                         , thumb
                         , txt);
```

### Naming

- **Project methods use `CapitalizedCamelCase`** to differentiate from JUCE: `Init`, `Update`, `SetupSlider`, `SetColors`, `Process`.
- **JUCE overrides keep JUCE casing:** `paint`, `resized`, `mouseDown`, `timerCallback`, `prepareToPlay`, `processBlock`.
- **Variables use bare `lowercaseCamelCase`.** No `m_` prefix, no trailing-`underscore_`.
- `auto bounds = getLocalBounds();` — never `area`, `totalArea`. (Derived rectangles can still be named for the region they represent: `titleHeaderArea`, `compressorArea` etc.)
- `#pragma once` for header guards. No `#ifndef`/`#define`/`#endif` guards.

### Method ordering inside classes

Within each visibility section: constructor/destructor first, then JUCE-inherited overrides, then project methods.

```cpp
public:
    MyClass();
    ~MyClass() override = default;

    // JUCE overrides first
    void paint(juce::Graphics &) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent &) override;

    // Our methods after
    void Init();
    void Update();
    void SetColors(...);

private:
    // Same rule (JUCE first, ours second) per section.
```

**The `.cpp` method definition order must match the header order.** When reordering one, reorder the other.

### Section dividers

Use thin comment dividers between top-of-file includes and the first class, between unrelated classes in the same file, and between major method groups:

```cpp
//==============================================================================
```

### CMake `target_sources` entries

For each file pair: **`.cpp` first, then `.h`**.

```cmake
target_sources(CenterSpace PRIVATE
    Source/Plugin/PluginProcessor.cpp
    Source/Plugin/PluginProcessor.h
    Source/Plugin/PluginEditor.cpp
    Source/Plugin/PluginEditor.h
    ...)
```

### Const-correctness

Decided as members are added — not blanket-applied. Don't const-decorate colour setters or other config-time methods unless the use case calls for it.

### `juce::` qualification

`addUsingNamespaceToJuceHeader` is off (we deleted `.jucer`, but the rule still applies). All JUCE types are fully qualified: `juce::Slider`, `juce::Colour`, `juce::AudioBuffer<float>`. No `using namespace juce` anywhere.

---

## Helper namespace

When utility helpers are added in Phase 3/4, put them under `namespace CenterSpace { ... }`. If a category of helpers needs its own namespace (e.g. param tagging, math utils), name it descriptively: `CenterSpaceUtils::`, `CenterSpaceParams::`, etc. Start with `CenterSpace::` and split as the need becomes obvious.

---

## Patterns to follow

### Adding a new APVTS parameter

Use `juce::ParameterID{"id", versionHint}` (not the bare string constructor) so Logic/GarageBand's parameter-version-hint assert stays quiet.

```cpp
std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{"newParam", 2},
    "New Param",
    juce::NormalisableRange<float>(0.0f, 1.0f),
    0.5f,
    "unit"),
```

**Version hint rules:**
- All v2.0 parameters use `versionHint = 1` and keep it forever.
- New parameters added later get the next-higher hint (`2`, `3`, …) tied to the release they shipped in.
- Never change an existing param's hint — that breaks automation in saved sessions.

### Audio thread / message thread split

The audio thread runs `processBlock` and everything reachable from it.

**Never on the audio thread:**
- Allocations (everything pre-allocated in `prepareToPlay`)
- File I/O
- Calls to `juce::AlertWindow` or other message-thread-only JUCE methods
- Mutex locks (use atomics)

**Use the dirty-flag pattern** when the audio thread needs to signal the UI: atomic flag flipped on the audio side, polled by the editor's 60 Hz timer on the message side.

### Editor's 60 Hz timer

`PluginEditor::timerCallback` drives all meter updates. Add new timer-driven UI work here. Don't spin up additional timers.

---

## v2.0 parameter inventory

22 APVTS entries total. All use `versionHint = 1`. Defaults shown.

### Shared (6) — visible in both Vibe and Tweak modes

| ID | Type | Range / Choices | Default |
|---|---|---|---|
| `uiMode` | Choice | `Vibe`, `Tweak` | `Vibe` |
| `inputType` | Choice | `LR`, `M/S` | `LR` |
| `inGain` | Float dB | -100 to +12 | 0.0 |
| `outGain` | Float dB | -100 to +12 | 0.0 |
| `outputType` | Choice | `LR`, `M/S` | `LR` |
| `bypass` | Bool | — | `false` (host-driven, no UI button) |

### Tweak-mode primitives (11) — DSP reads these directly when `uiMode == Tweak`

| ID | Type | Range / Choices | Default | Notes |
|---|---|---|---|---|
| `sideInGain` | Float dB | -100 to +12 | 0.0 | |
| `scHpfHz` | Float Hz | 20 to 2000 | 20.0 | 12 dB/oct, detector-only |
| `scLpfHz` | Float Hz | 200 to 20000 | 20000.0 | 12 dB/oct, detector-only |
| `peakRMS` | Choice | `Peak`, `RMS` | `Peak` | UI hidden when `style == Opto` (DSP forces RMS) |
| `attack` | Float ms | 0.01 to 2000 | 10.0 | |
| `release` | Float ms | 1 to 2000 | 100.0 | |
| `style` | Choice | `Modern VCA`, `Opto` | `Modern VCA` | |
| `threshold` | Float dB | -100 to +12 | 0.0 | |
| `ratio` | Float | 1 to 20 | 1.0 | 1.0 = no compression |
| `knee` | Float dB | 0 to 24 | 0.0 | UI hidden when `style == Opto` (DSP bakes 12 dB) |
| `lookahead` | Choice | `0 ms`, `1 ms`, `4 ms`, `10 ms` | `0 ms` | Reports latency to host |

### Vibe-mode macros (5) — DSP derives effective values from these when `uiMode == Vibe`

| ID | Type | Range / Choices | Default |
|---|---|---|---|
| `feel` | Choice | `Clean`, `Smooth` | `Clean` |
| `compress` | Float | 0.0 to 1.0 | 0.0 |
| `react` | Float | 0.0 to 1.0 | 0.5 |
| `focus` | Choice | 15 presets (see below) | `Full Range` |
| `lookaheadOnOff` | Bool | — | `false` |

---

## Vibe / Tweak modes — architecture

**Mode independence:** Vibe and Tweak each keep their own parameter values across mode switches. Switching Vibe→Tweak does **not** modify Tweak's primitives, and vice versa. Both halves persist in patches and A/B snapshots.

**Audio thread reads one set at a time:**
- `uiMode == Tweak`: DSP reads Tweak primitives directly.
- `uiMode == Vibe`: DSP derives effective values from Vibe macros (see derivation rules below).

The inactive set is stored but not read by the DSP.

**Mode-switch audio behavior:** the effective DSP values change instantly when `uiMode` flips. The existing `SmoothedValue` chain (~20 ms ramp) absorbs the discontinuity. No special crossfade logic needed.

**UI layout:**
- Shared params (top-of-list above) are visible in both modes, in the same positions.
- Mode-specific controls live on their own `juce::Component`, shown/hidden based on `uiMode`. Avoids per-control show/hide and resize churn.

---

## Vibe-mode macro derivations

When `uiMode == Vibe`, the DSP computes effective values from the macros using these rules:

### `feel` bakes multiple Tweak-equivalent values

| | Feel = Clean | Feel = Smooth |
|---|---|---|
| Effective style | Modern VCA | Opto |
| Effective `peakRMS` | Peak | RMS |
| Effective `knee` | 0 dB (hard) | 12 dB |
| Lookahead value when On | 1 ms | 4 ms |
| `react` attack range | 0.5 – 30 ms | 5 – 100 ms |
| `react` release range | 30 – 500 ms | 100 – 1000 ms |

### `compress` curve

Linear interpolation across three primitives:

| `compress` | effective sideInGain | effective threshold | effective ratio |
|---|---|---|---|
| 0.0 | 0 dB | 0 dB | 1.0 |
| 0.5 | +3 dB | -12 dB | 3:1 |
| 1.0 | +9 dB | -30 dB | 10:1 |

### `react` mapping

Linear, both attack and release ramp simultaneously within Feel's range:

| `react` | Attack | Release |
|---|---|---|
| 0.0 | min of Feel range | min of Feel range |
| 1.0 | max of Feel range | max of Feel range |

### `lookaheadOnOff`

- `false` → 0 ms (no lookahead, no reported latency)
- `true` → Feel-dependent: Clean = 1 ms, Smooth = 4 ms

### `focus` presets (HPF / LPF pairs)

| Preset | HPF | LPF |
|---|---|---|
| Full Range | 20 Hz | 20 kHz |
| Reduce Bass | 80 Hz | 20 kHz |
| Transient Focus | 120 Hz | 15 kHz |
| Lows | 40 Hz | 250 Hz |
| Low Mid | 250 Hz | 500 Hz |
| High Mid | 500 Hz | 2 kHz |
| High | 2 kHz | 20 kHz |
| Vocal Body | 250 Hz | 500 Hz |
| Vocal Clarity | 1 kHz | 3 kHz |
| Kick Thump | 40 Hz | 90 Hz |
| Kick Smack | 2.5 kHz | 4 kHz |
| Snare Thump | 100 Hz | 300 Hz |
| Snare Smack | 650 Hz | 20 kHz |
| Bass Body | 80 Hz | 350 Hz |
| Hats Range | 350 Hz | 20 kHz |

Some ranges overlap (e.g. Vocal Body == Low Mid). Intentional for now; refined after listening.

---

## Style-conditional UI visibility (Tweak mode only)

| Param | Visible when | Notes |
|---|---|---|
| `knee` | `style == Modern VCA` | DSP forces 12 dB in Opto regardless of param value |
| `peakRMS` | `style == Modern VCA` | DSP forces RMS in Opto regardless of param value |

UX rule: when a control is hidden, **its param value is preserved** — switching back to Modern restores the user's last-set knee/peak-RMS. Never write to a param just because its UI is hidden.

---

## Implementation invariants (Phase 3 punch list)

Things that must be true after Phase 3 implementation:

- **Lookahead delays L and R before M/S encoding.** Mid and side must be aligned in time when decoded. Delaying only the compressed mid breaks the M/S round-trip.
- **`envVal` clamped to `1e-9f` minimum** before `log10` in the static-curve math. `log10(0) = -infinity` cascades to NaN.
- **All cross-thread level/GR fields are `std::atomic<float>`** (`inLeftLevel`, `outMidLevel`, `gainReduction`, etc.).
- **`prepareToPlay` resets every stateful component on SR change:** BallisticsFilter, SC HPF/LPF, all `SmoothedValue`s, lookahead delay buffer.
- **All v2.0 params use `versionHint = 1`.** Never reassign hints.
- **Bypass param drives PDC correctly:** when bypassed, audio still passes through the lookahead delay so reported latency stays accurate.
- **SC mono-sum convention:** detector receives `(L + R) / numChannels`. Not max-of-L/R; not per-channel envelopes. Standard for compressors.
- **Style transitions:** small audio artifact on Modern↔Opto switch is acceptable. `SmoothedValue` chain handles the bulk; we don't add explicit crossfade logic.
- **Macros are real APVTS params, not UI-only.** Required for mode independence.

---

## Build verification

After any code change:

```bash
cmake --build --preset=macos-release --target CenterSpace_VST3 2>&1 | grep -E "error:|\*\* BUILD"
```

For full multi-format verification:

```bash
cmake --build --preset=macos-release --target CenterSpace_All 2>&1 | grep -E "error:|\*\* BUILD"
```

---

## Things to ask the user about, not assume

- Branch creation / switching
- Force pushes
- Anything that touches global git config
- Adding new APVTS params (param IDs, ranges, defaults are DSP-design decisions)
- UI layout / sizing changes (user iterates visually and gives explicit pixel directions)
- Whether something belongs in the DSP rework (Phase 3) vs. the GUI redesign (Phase 4)

---

## Phases (for context)

1. **Phase 1 — Toolchain & baseline.** ✅ Done. C++20, JUCE 8.0.13, CMake migration, `juce::` qualification, Projucer removed.
2. **Phase 2 — Project hygiene.** ✅ Done. Class rename, folder reorg, style pass complete. LICENSE + README polish deferred to Phase 5.
3. **Phase 3 — DSP rework.** 🟡 Bucket A done (header-side init, smart-pointer audit, include cleanup, atomic-load hoist, smoothed values, dB-based GR meter, BallisticsFilter swap, test harness). Bucket B brainstorm done. Implementation in progress — see "v2.0 parameter inventory" below for the locked spec.
4. **Phase 4 — GUI redesign** to match Dirty Little Bass Synth aesthetic. Includes Vibe/Tweak mode UI, patch system (`FullumMusicModules::PatchManager`), A/B compare (`FullumMusicModules::ABCompareManager`), SC silence indicator, mid meter scaling. **VUMeter dB range still feels too generous:** Phase 3 dropped the floor from -100 dB to -60 dB and the `TestSignalChainGainReduction` harness confirms the signal-chain math is correct (19 dB of real GR produces a 32-percentage-point bar drop), but in-DAW it still doesn't *read* as reduction unless threshold is pushed very low. Phase 4 should tighten the floor further (-48 dB or -36 dB) and likely add a dedicated mid-meter scale tied to the new visual design. This is purely a display-tuning issue — DSP and meter taps are verified correct.
5. **Phase 5 — Release prep.** LICENSE, README polish, screenshots, DAW verification, code-sign + notarize, v2.0 tag.

---

## Common gotchas

- **AU + Logic Pro / GarageBand requires `juce::ParameterID{id, versionHint}` with a non-zero hint** — see the APVTS pattern above. Plain string-IDs trigger an assert in Debug builds.
- **`Builds/` directory regeneration** — if Xcode has the old Projucer-generated `Builds/MacOSX/...xcodeproj` cached in any window, closing and reopening at the new `build/macos/CenterSpace.xcodeproj` is required. Don't open the old path; it'll resurrect.
- **`COPY_PLUGIN_AFTER_BUILD` overwrites the installed plugin** — Debug builds overwrite Release in `~/Library/Audio/Plug-Ins/`. Be aware if A/B-ing in a DAW.
- **Atomic-load pattern for APVTS param pointers:** `*atomicPtr` returns `std::atomic<float>&`, which can confuse template deduction (e.g. `juce::Decibels::gainToDecibels<T>`). Always use `atomicPtr->load()` when passing to templated functions.
- **`juce::dsp::BallisticsFilter` time-constant convention:** `setAttackTime(t)` means "reach ~99.8% of step input after `t` ms" (uses `exp(-2π/...)` internally), not the textbook "63% time." Same convention as the legacy hand-rolled `Envelope` class, so v1 attack/release values feel the same.
- **Sub-20 Hz HPF still passes some low-end:** a 12 dB/oct biquad at the 20 Hz floor attenuates 10 Hz by only ~12 dB. For "kick drums won't trigger" use cases, the user must raise the HPF to ~80–150 Hz. Document in user manual.
- **Out-of-phase SC sources cancel on mono-sum:** standard behavior across all SC-equipped compressors. Document in user manual.
- **Mid signal can exceed 0 dBFS internally:** `mid = L + R` with both at full scale = 2.0 linear. Decode's `× 0.5` brings it back. Phase 4 mid meter should scale by 0.5 for display.
- **`juce::dsp::BallisticsFilter::setLevelCalculationType` calls `reset()` internally** (see [juce_BallisticsFilter.cpp:60-65](JUCE/modules/juce_dsp/processors/juce_BallisticsFilter.cpp)). Calling it every `processBlock` zeros the envelope state at every block boundary, producing audible clicks during compression (compGain transiently jumps back to 1.0 each block). Always gate it behind a change check: only call when the mode actually changes. Cache the last-applied value and compare. Same hygiene is good for `setAttackTime`/`setReleaseTime` even though they don't reset state — saves an `exp()` per block.
