# Galaxy Tape

Galaxy Tape is a 64-bit stereo VST3 tape-echo and reverb effect for FL Studio
and other VST3 hosts. Its fixed-size 773 x 384 interface uses the supplied
reference image as the panel artwork, with live controls drawn over the knobs,
switches, and meter.

## Install on Windows

Download and run **Galaxy-Tape-Setup.exe** from the repository's
[Releases](https://github.com/joe-calih/Galaxy-Tape/releases) page. The
installer places the VST3 in the shared Windows VST3 directory
(`Common Files\VST3\Galaxy Tape.vst3`). Restart FL Studio and rescan plugins.

## Controls

- **Input Vol / Output Vol:** input and final output gain (0-2x).
- **Head Select:** selects combinations of the three echo heads (1-7); zero
  suppresses the echo heads and leaves the reverb available. The three head
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
- **Tape stock:** click the small tape selector to cycle Splice, New, Used, and
  Old. Older stock adds saturation, high-frequency loss, and more wow.
- **Wet Solo:** removes the dry input from the output. **Power** bypasses the
  effect.
- **A/B, Copy, Paste:** compare two parameter states or copy/paste a state.

Factory presets: **Default**, **Slapback**, **Galactic Wash**, **Old Orbit**, and
**Dub Loop**.

## Build from source

The GitHub Actions workflow builds the 64-bit VST3 and packages a Windows
installer on every push. Tag a version (for example `v1.0.0`) to publish a
downloadable GitHub Release. A local Windows build needs CMake 3.22+, Visual
Studio 2022 C++ tools, and Inno Setup 6. CMake fetches JUCE 8.0.6 at configure
time.

```powershell
cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target GalaxyTape_VST3 --parallel 2
```

The plugin source is licensed under **AGPL-3.0-or-later**. JUCE is fetched at
build time under its AGPLv3 terms; distributing the installer requires making
the corresponding source available to recipients. See the
[GNU AGPL](https://www.gnu.org/licenses/agpl-3.0.html) and
[JUCE licensing](https://juce.com/legal/).
