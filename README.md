# Galaxy Tape 2.0

Galaxy Tape is a 64-bit stereo/mono VST3 tape-echo and reverb effect. Version
2.0 keeps the supplied 773 x 384 reference-panel artwork and adds a polished,
scalable hardware-style interface, engraved knob indicators, refined
display type, a Loops information and licensing panel, and production-ready
factory presets.

## About Loops

[Loops](https://loops.co.ke) helps producers discover curated sample packs and
sounds through previews and production metadata. Its catalog covers genres
including house, hip-hop, techno, Afrobeats, and cinematic music. Loops also
supports sample creators and music production workflows. Check the terms for
the individual pack before using its samples commercially. Learn more at
[About Loops](https://loops.co.ke/about), or use the plugin's **INFO** button
for company, project, and license details.

## Install on Windows

Download [Galaxy-Tape-v2.0.0-Setup.exe](https://github.com/joe-calih/Galaxy-Tape/releases/download/v2.0.0/Galaxy-Tape-v2.0.0-Setup.exe)
from the [GitHub Releases](https://github.com/joe-calih/Galaxy-Tape/releases)
page. Run the installer; it places the 64-bit VST3 in the shared Windows VST3
directory (`Common Files\VST3\Galaxy Tape.vst3`). At the end of installation,
choose **Visit Loops** to open [loops.co.ke](https://loops.co.ke). Restart FL
Studio and rescan plugins if the effect is not immediately listed.

The plugin has a fixed aspect ratio and a **70%-150% size menu**, in 10%
increments. Choose the size from the top-right button. **INFO** opens About,
Loops, licensing, and source-code options.

## Controls

- **Input Vol / Output Vol:** input and final output gain (0-2x).
- **Head Select:** selects combinations of the three echo heads (1-7); zero
  suppresses the echo heads and leaves the reverb available. Factory settings
  select Head 1 to match the supplied reference. The three head
  buttons independently enable/disable their corresponding heads.
- **Echo Rate:** delay time from 40 to 1600 ms. With Tempo Sync on, the nearest
  1/16, 1/8, 1/4, 1/2, or whole-note value is used from the host tempo.
- **Feedback:** repeat level. **Tape Loop / Splice** holds the existing buffer
  with near-unity feedback and stops feeding new input into it.
- **Bass / Treble:** low- and high-band tone controls for the repeats.
- **Echo Pan / Reverb Pan:** stereo placement of the echo and reverb returns.
- **Echo Vol / Reverb Vol:** independent effect-return levels.
- **Input Send:** controls whether new input is sent to the tape delay;
  existing repeats continue to decay when it is off.
- **Tape stock:** click the small tape selector to cycle New, Used, and Old.
  Older stock adds saturation, high-frequency loss, and more wow.
- **Wet Solo:** removes the dry input from the output. **Power** bypasses the
  effect.
- **A/B, Copy, Paste:** compare two parameter states or copy/paste a state.
  Settings continue to use normal host automation and plugin state recall.

The head windows show each head's actual delay time in milliseconds: head 1
uses the full rate, head 2 uses 2/3, and head 3 uses 1/2. Disabled heads keep
their readout but dim it. Tempo Sync snaps the master rate to the nearest
supported note value from the active host tempo and updates the head readouts.
The output meter's analog needle follows the measured output peak; the red
input lamp illuminates on peaks near full scale.

### Factory presets

The preset menu has a starting sound for vocals, drums, bass, guitar, keys, dub,
atmospheres, and low-color tape-bus use: **Default**, **Vocal - Tight Slap**,
**Vocal - Wide Double**, **Vocal - Warm Plate Echo**, **Drums - Tight Room**,
**Drums - Snare Throw**, **Bass - Round Tape**, **Guitar - Rock Slap**,
**Keys - Stereo Quarter**, **Keys - Dark Echo**, **Dub - Live Feedback**,
**Ambient - Long Orbit**, and **Studio - Tape Bus**. These are starting points,
not channel-strip or mastering presets; the plugin is a tape echo and reverb,
not a compressor, equalizer, or licensed SSL emulation.

## Hosts and CPU

The installer contains a native x64 VST3, suitable for 64-bit FL Studio and
other DAWs that host VST3 plug-ins on Windows. The same parameter set is
automatable and state-recallable in the host; tempo-synced presets read the
host transport tempo. Mono and stereo layouts are supported. The plugin does
not require MIDI, network access, or runtime downloads. JUCE LTO/release
optimizations are enabled, and the reverb configuration is only updated when
the tape-stock mode changes.

The displayed panel artwork is a 773 x 384 raster image; the artwork, title
type, controls, and meter scale together. The original interface size is 100%.

## Build from source

The GitHub Actions workflow builds the 64-bit VST3 and packages a Windows
installer on every push. Tag a version (for example `v2.0.0`) to publish a
downloadable GitHub Release. A local Windows build needs CMake 3.22+, Visual
Studio 2022 C++ tools, and Inno Setup 6. CMake fetches JUCE 8.0.6 at configure
time.

```powershell
cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target GalaxyTape_VST3 --parallel 2
```

GitHub Actions builds the VST3 and versioned installer on each push and
publishes the installer on tags named `v*` (for example `v2.0.0`). See
[CHANGELOG.md](CHANGELOG.md) for release history and
[the workflow](.github/workflows/windows-installer.yml) for the exact build,
packaging, and release steps.

Galaxy Tape source code is licensed under **AGPL-3.0-or-later**. JUCE 8.0.6
is fetched at build time under its AGPLv3 terms; redistribution requires
providing the corresponding source. The supplied interface art is included
with this project under the stated project license. Read [LICENSE](LICENSE)
and [JUCE licensing](https://juce.com/legal/) before redistributing.
