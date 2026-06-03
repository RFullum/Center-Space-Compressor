# Center Space

[![Latest release](https://img.shields.io/github/v/release/RFullum/Center-Space-Compressor?label=download&style=for-the-badge)](https://github.com/RFullum/Center-Space-Compressor/releases/latest)

A stereo sidechain compressor that only compresses the **center** of the stereo field.

If no signal hits the sidechain, the compressor does nothing — but the M/S encode/decode and input/output gain still run, so you can also use Center Space as a stereo-type converter on its own.

![Vibe mode](docs/screenshots/vibe-mode.png)

---

## Features

- **Two UI modes**:
  - **Vibe** — macro controls (`Compress`, `React`, `Focus`, `Feel`, `Lookahead On/Off`). Quick to dial in.
  - **Tweak** — full sidechain compressor with Threshold, Ratio, Knee, Attack, Release, SC HPF/LPF, SC Gain, Peak/RMS detection, Style, and Lookahead time.

  ![Tweak mode](docs/screenshots/tweak-mode.png)
- **9 Focus presets** for the sidechain filter pair (Full Range, Reduce Bass, Vocal, Kick, Bass, Transients, Low, Mid, High).
- **Two compressor styles** — Modern VCA (clean) and Opto (smooth, soft-knee, RMS).
- **Lookahead** — 0, 1, 4, or 10 ms. Reports latency to the host.
- **M/S input + output conversion** — feed L/R or already-encoded M/S; output as either.
- **Patch system** — factory presets included, user patches save to `~/Library/Application Support/FullumMusic/Center Space/Patches/` as `.cspatch` files. Browse with a grid-style popup picker.

  ![Patch popup](docs/screenshots/patch-popup.png)
- **A/B compare** — two independent snapshot slots with implicit-copy semantics.
- **Custom metering** — sidechain level (with peak-hold), gain reduction, and a stereo-field meter that shows input and output at the L/C/R points of the field.
- **Tooltips on every control**, toggleable via the title-bar right-click menu.

  ![Tooltip](docs/screenshots/tooltips.png)

  ![Options menu](docs/screenshots/options-menu.png)

---

## System Requirements

- macOS 11 (Big Sur) or later
- Universal binary (Apple Silicon + Intel)
- Formats: **VST3**, **AU**

Windows and Linux presets exist in `CMakePresets.json` but haven't been exercised.

---

## Download

Grab the latest macOS installer from the [Releases page](https://github.com/RFullum/Center-Space-Compressor/releases/latest).

The `.pkg` installs VST3 and AU plug-ins. Signed and notarized for macOS.

---

## Where files land after install

- VST3 → `/Library/Audio/Plug-Ins/VST3/Center Space.vst3`
- AU → `/Library/Audio/Plug-Ins/Components/Center Space.component`

User patches live at:
`~/Library/Application Support/FullumMusic/Center Space/Patches/`

Factory patches travel inside the plugin bundle and are read-only.

---

## Building from Source

Requires CMake ≥ 3.22 and a C++20 compiler. JUCE 8 is vendored as a git submodule.

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

For development iteration, use `--preset=macos-debug` instead.

Build a specific format:

```sh
cmake --build --preset=macos-release --target CenterSpace_VST3
cmake --build --preset=macos-release --target CenterSpace_AU
cmake --build --preset=macos-release --target CenterSpace_Standalone
```

---

## License

See [LICENSE.txt](LICENSE.txt). Third-party component notices are in [THIRD_PARTY_NOTICES.txt](THIRD_PARTY_NOTICES.txt).

---

## Credits

Center Space is by **Robert Fullum** / [FullumMusic](https://www.fullummusic.com).

Built with [JUCE 8](https://juce.com). Includes the [sigslot](https://github.com/palacaze/sigslot) signal/slot library (MIT).
