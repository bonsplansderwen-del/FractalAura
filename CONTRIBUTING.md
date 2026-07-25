# Contributing to FractalAura

Thanks for your interest in contributing to FractalAura! This guide covers setup, development workflows, and best practices.

## Development Setup

### Prerequisites

- **macOS 10.13+** (primary target)
- **Xcode Command Line Tools**: `xcode-select --install`
- **CMake 3.22+**: `brew install cmake`
- **Ninja** (optional, faster builds): `brew install ninja`
- **Git**

### Clone and Prepare

```bash
# Clone with JUCE submodule
git clone https://github.com/bonsplansderwen-del/FractalAura.git
cd FractalAura
git submodule update --init --recursive
```

### Development Build

```bash
mkdir build-dev
cd build-dev

# Configure (Debug mode, faster iteration)
cmake .. -G Xcode -DCMAKE_BUILD_TYPE=Debug

# Or with Ninja for faster builds:
# cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build . --config Debug
```

### Installing for Testing

```bash
# After each build, reinstall the plugin
cp -r build-dev/FractalAura_artefacts/Debug/VST3/FractalAura.vst3 \
  /Library/Audio/Plug-Ins/VST3/

# Restart your DAW to load the new version
```

## Project Structure

```
FractalAura/
├── Source/
│   ├── PluginProcessor.cpp/h     # Main plugin class
│   ├── AudioEngine/
│   │   ├── GranularEngine.h      # Grain synthesis
│   │   ├── SpectralProcessor.cpp/h # FFT pitch shifting
│   │   └── ChaosLFO.h            # Lorenz modulation
│   └── GUI/
│       └── MainEditor.cpp/h      # JUCE UI
├── CMakeLists.txt                # Build configuration
├── JUCE/                         # Git submodule
└── .github/workflows/            # CI/CD
```

## Key Components

### 1. **GranularEngine** (Source/AudioEngine/GranularEngine.h)

Handles grain-based synthesis:
- Ring buffer: 4-second lookback window
- Max 64 concurrent grains
- Per-grain control: pitch, pan, reverse
- Hann window envelope

**To modify:**
- Change `maxGrains` constant for more/fewer concurrent voices
- Adjust `bufferLength` (in seconds) for longer/shorter lookback
- Tweak `triggerGrain()` for different grain randomization

### 2. **SpectralProcessor** (Source/AudioEngine/SpectralProcessor.cpp/h)

FFT-based pitch shifting with phase vocoder:
- 1024-point FFT (25% overlap)
- Phase unwrapping for transient preservation
- Spectral freeze captures magnitudes/phases
- Spectral tilt applies frequency-dependent gain

**To modify:**
- Change `fftOrder` for different FFT size (larger = more precision, more latency)
- Adjust `hopSize` ratio for overlap amount
- Customize phase vocoder algorithm in `processSpectrum()`

### 3. **ChaosLFO** (Source/AudioEngine/ChaosLFO.h)

Lorenz attractor-based modulation:
- Three coupled differential equations
- Chaotic yet deterministic behavior
- Scales output to ±1 range

**To modify:**
- Change `sigma`, `rho`, `beta` constants for different chaos character
- Adjust `dt` for integration speed
- Modify `0.05f` scaling factor for amplitude

### 4. **MainEditor** (Source/GUI/MainEditor.cpp/h)

JUCE-based UI:
- 5 rotary sliders + 1 toggle button
- Animated "aura ring" visualization
- Cyberpunk color scheme (purple/cyan)

**To modify:**
- Add sliders in `FractalAuraEditor::FractalAuraEditor()`
- Customize layout in `resized()`
- Change colors in `paint()`

## Making Changes

### Adding a New Parameter

1. **Add to parameter layout** (PluginProcessor.cpp):
   ```cpp
   params.push_back(std::make_unique<juce::AudioParameterFloat>(
       juce::ParameterID{"myParam", 1}, "My Parameter", minVal, maxVal, defaultVal));
   ```

2. **Use in processBlock()** (PluginProcessor.cpp):
   ```cpp
   const float myValue = apvts.getRawParameterValue("myParam")->load();
   ```

3. **Add UI control** (MainEditor.cpp):
   ```cpp
   juce::Slider mySlider;
   std::unique_ptr<Attachment> myAttach;
   // In constructor:
   setupSlider(mySlider, "myParam", myAttach);
   // In resized():
   mySlider.setBounds(...);
   ```

### Adding a New Audio Engine

1. Create header file: `Source/AudioEngine/MyEngine.h`
2. Create implementation if needed: `Source/AudioEngine/MyEngine.cpp`
3. Include in `PluginProcessor.h`
4. Instantiate in `FractalAuraAudioProcessor` class
5. Call in `processBlock()`
6. Add to `CMakeLists.txt` if it's a .cpp file

## Building for Release

### Local Release Build

```bash
mkdir build-release
cd build-release
cmake .. -G Xcode -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### GitHub Actions Release

1. Tag a commit:
   ```bash
   git tag v1.0.1
   git push origin v1.0.1
   ```

2. The workflow will automatically:
   - Build for x86_64 and arm64
   - Create a GitHub Release
   - Attach VST3 and AU binaries

## Testing

### Before Submitting PR

- [ ] Builds without errors (both Debug and Release)
- [ ] No compiler warnings
- [ ] Plugin loads in DAW (Logic Pro, Ableton, etc.)
- [ ] All parameters work correctly
- [ ] No audio glitches or crashes
- [ ] Code is formatted consistently

### Testing Checklist

```bash
# Clean rebuild
cd build-dev
rm -rf *
cmake .. -G Xcode -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug

# Test in DAW
cp -r FractalAura_artefacts/Debug/VST3/FractalAura.vst3 /Library/Audio/Plug-Ins/VST3/
```

## Code Style

- **C++ Standard**: C++20
- **Naming**: camelCase for variables/functions, PascalCase for classes
- **Formatting**: Consistent indentation (4 spaces)
- **Comments**: Explain *why*, not *what*

## Performance Considerations

- Use `juce::ScopedNoDenormals` in audio threads to avoid performance cliffs
- Allocate DSP buffers in `prepare()`, not in `processBlock()`
- Use SIMD where possible (JUCE handles this)
- Profile with Xcode Instruments if making major changes

## Troubleshooting

### Build Fails: "juce_add_plugin not found"

```bash
# Ensure submodule is initialized
git submodule update --init --recursive
```

### Plugin Won't Load in DAW

```bash
# Check install location
ls -la /Library/Audio/Plug-Ins/VST3/

# Restart DAW
# Check Console.app for error messages
```

### Audio Artifacts/Glitches

- Check ring buffer overflow in GranularEngine
- Verify FFT window overlap is correct
- Profile CPU usage in DAW
- Reduce grain count if needed

## Resources

- [JUCE Documentation](https://docs.juce.com/)
- [Phase Vocoder Theory](https://en.wikipedia.org/wiki/Phase_vocoder)
- [Granular Synthesis](https://en.wikipedia.org/wiki/Granular_synthesis)
- [Lorenz Attractor](https://en.wikipedia.org/wiki/Lorenz_system)

## Questions?

Open an issue on GitHub with:
- Clear description of problem/feature
- Steps to reproduce (if bug)
- Expected vs. actual behavior
- macOS version and DAW tested with

Happy hacking! 🚀
