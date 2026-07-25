# Development Roadmap

## Current Version: 1.0.0

### Features
- ✅ Granular synthesis (64 voices, 4-sec buffer)
- ✅ FFT pitch shifting with phase vocoder
- ✅ Chaos LFO modulation
- ✅ Spectral freeze & tilt
- ✅ VST3 & AU support (macOS)
- ✅ Animated UI

---

## Planned Features (v1.1+)

### Audio Processing
- [ ] **Multi-tap delay** on granular output for spatial effects
- [ ] **Convolver** for impulse response effects
- [ ] **Formant preservation** during pitch shifting
- [ ] **Polyphonic grain modulation** (per-grain LFO)
- [ ] **Grain recording mode** (record user input → granulate)

### UI/UX
- [ ] **Waveform display** showing input audio
- [ ] **Spectrum analyzer** visualization
- [ ] **Preset system** (save/load plugin state)
- [ ] **Drag-to-draw envelope** for custom grain shapes
- [ ] **Keyboard shortcuts** for parameter tweaks

### Architecture
- [ ] **Windows & Linux support** (via JUCE cross-platform)
- [ ] **Standalone application** version
- [ ] **VST2 support** (if JUCE allows)
- [ ] **MIDI control** mapping
- [ ] **Undo/Redo** for parameter changes

### Performance
- [ ] **Simd optimization** for FFT bins
- [ ] **GPU acceleration** (Metal on macOS)
- [ ] **CPU profiling** and optimization
- [ ] **Thread pool** for grain processing

### Testing & Quality
- [ ] **Unit tests** for audio engines
- [ ] **Integration tests** with JUCE framework
- [ ] **Regression tests** for audio output
- [ ] **Benchmarks** for CPU usage

---

## Known Issues

- [ ] High CPU usage at high grain density (>40 grains)
- [ ] Spectral freeze can cause audible artifacts on transients
- [ ] Phase vocoder has slight latency (FFT hop size)
- [ ] No anti-aliasing on grain pitch shifting

---

## Version History

### v1.0.0 (Current) - Jul 2026
- Initial release
- Spectral granular synthesis
- Chaos modulation
- macOS VST3/AU support

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for development setup and guidelines.
