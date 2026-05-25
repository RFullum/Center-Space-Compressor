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
  DSP/                        Envelope (compressor envelope follower)
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

## Signal flow (one-paragraph summary)

`processBlock` receives a stereo bus + a sidechain bus. Stereo input is encoded to Mid/Side. The sidechain is mono-summed, run through the `Envelope` ballistics follower, compared to threshold, and a per-sample gain reduction is computed (peak-or-RMS, attack/release smoothed). That gain is applied to the **mid** channel only; the side channel is untouched. M/S is decoded back to stereo and written out. Result: the center collapses under sidechain transients while the stereo width stays at full level.

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

### Initializer lists

Comma-first, leading colon on its own line, aligned:

```cpp
MyClass::MyClass()
: memberA(0)
, memberB(1)
, memberC(2)
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

1. **Phase 1 — Toolchain & baseline.** Done. C++20, JUCE 8.0.13, CMake migration, `juce::` qualification, Projucer removed.
2. **Phase 2 — Project hygiene.** In progress.
   - Step 6 (class rename): done.
   - Step 7 (folder reorg): done.
   - Step 8 (style pass): this is the doc that drives it.
   - Step 9 (LICENSE + README polish): pending.
3. **Phase 3 — DSP rework.** Pending; needs a brainstorm checkpoint to lock the compressor feature set first.
4. **Phase 4 — GUI redesign** to match Dirty Little Bass Synth aesthetic.
5. **Phase 5 — Release prep.** DAW verification, code-sign + notarize, v2.0 tag.

---

## Common gotchas

- **AU + Logic Pro / GarageBand requires `juce::ParameterID{id, versionHint}` with a non-zero hint** — see the APVTS pattern above. Plain string-IDs trigger an assert in Debug builds.
- **`Builds/` directory regeneration** — if Xcode has the old Projucer-generated `Builds/MacOSX/...xcodeproj` cached in any window, closing and reopening at the new `build/macos/CenterSpace.xcodeproj` is required. Don't open the old path; it'll resurrect.
- **`COPY_PLUGIN_AFTER_BUILD` overwrites the installed plugin** — Debug builds overwrite Release in `~/Library/Audio/Plug-Ins/`. Be aware if A/B-ing in a DAW.
