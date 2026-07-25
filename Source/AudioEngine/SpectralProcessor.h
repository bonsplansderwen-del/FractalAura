#pragma once
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <complex>

namespace AuraDSP
{
    class SpectralProcessor
    {
    public:
        static constexpr int fftOrder = 10;
        static constexpr int fftSize = 1 << fftOrder;
        static constexpr int hopSize = fftSize / 4;

        SpectralProcessor();
        ~SpectralProcessor() = default;

        void prepare(const juce::dsp::ProcessSpec& spec);
        void reset();
        void process(juce::AudioBuffer<float>& buffer, float pitchSemitones, bool freeze, float spectralTilt);

    private:
        void processSpectrum(std::vector<std::complex<float>>& spectrum, int channel, float pitchRatio, bool freeze, float tilt);

        juce::dsp::FFT forwardFFT { fftOrder };
        juce::dsp::FFT inverseFFT { fftOrder };
        juce::dsp::WindowingFunction<float> window { fftSize, juce::dsp::WindowingFunction<float>::hann };
        double sampleRate { 44100.0 };

        std::vector<std::vector<float>> inputFifo, outputFifo;
        std::vector<int> fifoIndex;
        std::vector<std::vector<float>> frozenMagnitudes, frozenPhases;
        std::vector<std::vector<float>> previousInputPhases, previousOutputPhases;
        std::vector<float> fftBuffer;
    };
}
