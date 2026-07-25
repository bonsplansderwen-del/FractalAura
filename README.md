# Fractal Aura - VST3 Audio Plugin

A spectral granular synthesizer plugin built with JUCE. FractalAura combines granular synthesis with FFT-based spectral processing and chaotic LFO modulation for experimental sound design.

## Features

- **Granular Engine**: Customizable grain size, density, and pitch with random pan and reverse playback
- **Spectral Processor**: Real-time FFT pitch shifting with spectral freezing and tilting
- **Chaos LFO**: Lorenz attractor-based modulation for organic, evolving sounds
- **Native macOS AU/VST3 Support**: High-performance audio processing with SIMD optimizations

## Requirements

- macOS 10.13+
- Xcode with Command Line Tools
- CMake 3.22+
- Git

## Quick Start

### 1. Clone Repository with Submodules

```bash
git clone https://github.com/bonsplansderwen-del/FractalAura.git
cd FractalAura
git submodule update --init --recursive
```

This clones JUCE 9.0.0 as a submodule.

### 2. Build on macOS

```bash
# Create build directory
mkdir build
cd build

# Configure with Xcode generator (Release build)
cmake .. -G Xcode -DCMAKE_BUILD_TYPE=Release

# Build the plugin
cmake --build . --config Release
```

### 3. Install Plugin

The built plugins are located at:
- **VST3**: `build/FractalAura_artefacts/Release/VST3/FractalAura.vst3`
- **AU**: `build/FractalAura_artefacts/Release/AU/FractalAura.component`

**Install to system directories:**

```bash
# VST3
cp -r build/FractalAura_artefacts/Release/VST3/FractalAura.vst3 \
  /Library/Audio/Plug-Ins/VST3/

# AU
cp -r build/FractalAura_artefacts/Release/AU/FractalAura.component \
  /Library/Audio/Plug-Ins/Components/
```

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Grain Size | 10-500 ms | 100 ms | Duration of each grain envelope |
| Density | 1-50 | 15 | Grains spawned per second |
| Pitch Shift | -24 to +24 semitones | 0 | Coarse pitch shift |
| Chaos Rate | 0.01-10 Hz | 1 Hz | LFO modulation speed |
| Freeze | Toggle | Off | Lock spectral magnitudes |
| Spectral Tilt | -1 to +1 | 0 | Frequency-dependent gain (-1=bass boost, +1=treble boost) |
| Mix | 0-1 | 0.5 | Dry/wet balance |

## Audio Processing Pipeline

```
Input Audio
    ↓
[Granular Engine] ← processes with pitch modulation from Chaos LFO
    ↓
[Spectral Processor] ← FFT pitch shift + freeze + tilt
    ↓
[Mix (equal-power crossfade)]
    ↓
Output Audio
```

## Architecture

### Source Structure

```
Source/
├── PluginProcessor.cpp/h       # Main plugin class & parameter tree
├── AudioEngine/
│   ├── GranularEngine.h        # Grain synthesis with ring buffer
│   ├── SpectralProcessor.cpp/h # FFT pitch shifting & phase vocoder
│   └── ChaosLFO.h              # Lorenz attractor modulation
└── GUI/
    └── MainEditor.cpp/h        # JUCE UI with animated visualization
```

### Key Algorithms

- **Phase Vocoder**: FFT-based pitch shifting with phase unwrapping (SpectralProcessor)
- **Granular Synthesis**: 4-second ring buffer with max 64 concurrent grains
- **Spectral Freeze**: Captures frozen magnitude/phase, applies randomized phase drift
- **Chaos Modulation**: Lorenz equations for organic pitch wobble

## Troubleshooting

### Build Fails: "Cannot find juce_add_plugin"

Ensure JUCE submodule is initialized:

```bash
git submodule update --init --recursive
```

### Plugin Not Found in DAW

Check install location:

```bash
ls -la /Library/Audio/Plug-Ins/VST3/
ls -la /Library/Audio/Plug-Ins/Components/
```

Restart your DAW if the plugin was just installed.

### Code Signing Issues (Xcode)

The CMakeLists.txt disables code signing. If you need signing:

```bash
cmake .. -G Xcode -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="Developer ID Application"
```

## Development

### Clean Rebuild

```bash
cd build
rm -rf *
cmake .. -G Xcode -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### Use Ninja for Faster Builds

```bash
brew install ninja
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

## License

MIT

## Credits

Built with [JUCE Framework](https://juce.com/)
