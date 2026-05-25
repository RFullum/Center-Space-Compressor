# Center Space

Sidechain compressor that only compresses the center of the stereo field. Stereo width stays full volume. Only compresses when sidechain is present.

Builds as VST3 and AU on macOS.

## Building

Requires CMake ≥ 3.22 and a C++20 compiler. JUCE is vendored as a git submodule.

Clone with submodules:

```sh
git clone --recurse-submodules https://github.com/RFullum/Center-Space-Compressor.git
cd Center-Space-Compressor
```

If you already cloned without `--recurse-submodules`:

```sh
git submodule update --init --recursive
```

Configure and build (macOS):

```sh
cmake --preset=macos
cmake --build --preset=macos-release
```

Successful builds auto-install to:

- `~/Library/Audio/Plug-Ins/VST3/Center Space.vst3`
- `~/Library/Audio/Plug-Ins/Components/Center Space.component`

Standalone `.app` is left in `build/macos/CenterSpace_artefacts/Release/Standalone/`.

For development iteration, use `--preset=macos-debug` instead.

Windows and Linux presets are stubbed in `CMakePresets.json` but not yet exercised.
