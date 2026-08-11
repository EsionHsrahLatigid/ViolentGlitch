# ViolentGlitch

ViolentGlitch is a destructive digital glitch distortion effect built with JUCE. It combines bit crushing, sample-and-hold rate destruction, random polarity flips, digital folding, and hard clipping for unstable machine-breakup textures.

Start with low monitoring volume. This plug-in is intentionally aggressive and can create sharp transients.

## Features

- Mono or stereo audio input with matching output
- Extreme bit-depth reduction
- Sample-rate destruction using a held-sample stage
- Random sign inversion, amplitude bursts, folding, and full-scale spikes
- Dry/wet mix and APVTS parameter/state persistence
- Repository-local VST3, AU, and Standalone artifact staging

## Build

Requirements:

- CMake 3.22+
- A C++17 compiler
- Ninja, Xcode, or another CMake generator
- Network access for the initial JUCE fetch, or a JUCE checkout at `JUCE/`

JUCE is pinned to 8.0.15 when downloaded by CMake.

```sh
cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/release --target ViolentGlitch_Artifacts
ctest --test-dir build/release --output-on-failure
```

Final products are staged under:

```text
artifacts/Release/
├── AU/ViolentGlitch.component          # macOS only
├── Standalone/ViolentGlitch.app        # macOS
└── VST3/ViolentGlitch.vst3
```

Override the staging root with `-DVIOLENTGLITCH_ARTIFACT_DIR=/absolute/path`.

## Plug-in identity

- Product: `ViolentGlitch`
- Vendor/company: `EsionHsrahLatigid`
- JUCE manufacturer code: `EHL_`
- JUCE plug-in code: `Vglt`
- Bundle identifier: `jp.ehl.violentglitch`
- Required format: VST3
- Additional formats: AU on Apple platforms and Standalone

## License

This repository's source code is licensed under the MIT License. See [LICENSE](LICENSE).

## JUCE licensing

JUCE 8 modules are dual-licensed under AGPLv3 and the commercial JUCE license. Review the [JUCE 8.0.15 license](https://github.com/juce-framework/JUCE/blob/8.0.15/LICENSE.md) and choose a compatible licensing path before distributing plug-in binaries. This repository does not vendor JUCE or grant a JUCE commercial license.
