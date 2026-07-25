#include "SpectralProcessor.h"
#include <cmath>

namespace AuraDSP
{
    SpectralProcessor::SpectralProcessor() { fftBuffer.resize(fftSize * 2, 0.0f); }

    void SpectralProcessor::prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        const size_t numChannels = spec.numChannels;
        inputFifo.assign(numChannels, std::vector<float>(fftSize, 0.0f));
        outputFifo.assign(numChannels, std::vector<float>(fftSize * 2, 0.0f));
        fifoIndex.assign(numChannels, 0);

        const size_t numBins = (fftSize / 2) + 1;
        frozenMagnitudes.assign(numChannels, std::vector<float>(numBins, 0.0f));
        frozenPhases.assign(numChannels, std::vector<float>(numBins, 0.0f));
        previousInputPhases.assign(numChannels, std::vector<float>(numBins, 0.0f));
        previousOutputPhases.assign(numChannels, std::vector<float>(numBins, 0.0f));
        reset();
    }

    void SpectralProcessor::reset()
    {
        for (auto& ch : inputFifo) std::fill(ch.begin(), ch.end(), 0.0f);
        for (auto& ch : outputFifo) std::fill(ch.begin(), ch.end(), 0.0f);
        for (auto& ch : previousInputPhases) std::fill(ch.begin(), ch.end(), 0.0f);
        for (auto& ch : previousOutputPhases) std::fill(ch.begin(), ch.end(), 0.0f);
        std::fill(fifoIndex.begin(), fifoIndex.end(), 0);
    }

    void SpectralProcessor::process(juce::AudioBuffer<float>& buffer, float pitchSemitones, bool freeze, float spectralTilt)
    {
        const int numSamples = buffer.getNumSamples();
        const int numChannels = std::min(static_cast<int>(buffer.getNumChannels()), static_cast<int>(inputFifo.size()));
        const float pitchRatio = std::pow(2.0f, pitchSemitones / 12.0f);

        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            for (int s = 0; s < numSamples; ++s)
            {
                inputFifo[ch][fifoIndex[ch]] = channelData[s];
                channelData[s] = outputFifo[ch][fifoIndex[ch]];
                outputFifo[ch][fifoIndex[ch]] = 0.0f;
                fifoIndex[ch]++;

                if (fifoIndex[ch] >= hopSize)
                {
                    fifoIndex[ch] = 0;
                    std::copy(inputFifo[ch].begin(), inputFifo[ch].end(), fftBuffer.begin());
                    window.multiplyWithWindowingTable(fftBuffer.data(), fftSize);
                    forwardFFT.performRealOnlyForwardTransform(fftBuffer.data());

                    const int numBins = (fftSize / 2) + 1;
                    std::vector<std::complex<float>> spectrum(numBins);
                    spectrum[0] = {fftBuffer[0], 0.0f};
                    spectrum[numBins - 1] = {fftBuffer[1], 0.0f};
                    for (int k = 1; k < numBins - 1; ++k)
                        spectrum[k] = {fftBuffer[2 * k], fftBuffer[2 * k + 1]};

                    processSpectrum(spectrum, ch, pitchRatio, freeze, spectralTilt);

                    fftBuffer[0] = spectrum[0].real();
                    fftBuffer[1] = spectrum[numBins - 1].real();
                    for (int k = 1; k < numBins - 1; ++k)
                    {
                        fftBuffer[2 * k] = spectrum[k].real();
                        fftBuffer[2 * k + 1] = spectrum[k].imag();
                    }

                    inverseFFT.performRealOnlyInverseTransform(fftBuffer.data());
                    window.multiplyWithWindowingTable(fftBuffer.data(), fftSize);

                    const float scale = 2.0f / (3.0f * static_cast<float>(fftSize));
                    for (int i = 0; i < fftSize; ++i)
                        outputFifo[ch][i] += fftBuffer[i] * scale;

                    std::move(inputFifo[ch].begin() + hopSize, inputFifo[ch].end(), inputFifo[ch].begin());
                    std::fill(inputFifo[ch].end() - hopSize, inputFifo[ch].end(), 0.0f);
                }
            }
        }
    }

    void SpectralProcessor::processSpectrum(std::vector<std::complex<float>>& spectrum, int ch, float pitchRatio, bool freeze, float tilt)
    {
        const int numBins = static_cast<int>(spectrum.size());
        const float expectedPhaseDiff = 2.0f * juce::MathConstants<float>::pi * static_cast<float>(hopSize) / static_cast<float>(fftSize);

        std::vector<float> currentMagnitudes(numBins, 0.0f), currentPhases(numBins, 0.0f);
        for (int k = 0; k < numBins; ++k)
        {
            currentMagnitudes[k] = std::abs(spectrum[k]);
            currentPhases[k]     = std::arg(spectrum[k]);
        }

        if (freeze)
        {
            if (frozenMagnitudes[ch][0] == 0.0f && frozenMagnitudes[ch][numBins / 2] == 0.0f)
            {
                frozenMagnitudes[ch] = currentMagnitudes;
                frozenPhases[ch]     = currentPhases;
            }
            for (int k = 0; k < numBins; ++k)
            {
                currentMagnitudes[k] = frozenMagnitudes[ch][k];
                currentPhases[k] += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
            }
        }
        else
        {
            frozenMagnitudes[ch][0] = 0.0f;
            frozenMagnitudes[ch][numBins / 2] = 0.0f;
        }

        if (std::abs(tilt) > 0.01f)
        {
            for (int k = 0; k < numBins; ++k)
            {
                float normFreq = static_cast<float>(k) / numBins;
                float gainLinear = std::pow(10.0f, (tilt * (normFreq - 0.5f) * 24.0f) / 20.0f);
                currentMagnitudes[k] *= gainLinear;
            }
        }

        std::vector<float> shiftedMagnitudes(numBins, 0.0f), shiftedPhases(numBins, 0.0f);
        for (int k = 0; k < numBins; ++k)
        {
            float phaseDiff = currentPhases[k] - previousInputPhases[ch][k];
            previousInputPhases[ch][k] = currentPhases[k];

            phaseDiff -= k * expectedPhaseDiff;
            while (phaseDiff > juce::MathConstants<float>::pi) phaseDiff -= juce::MathConstants<float>::twoPi;
            while (phaseDiff < -juce::MathConstants<float>::pi) phaseDiff += juce::MathConstants<float>::twoPi;

            float trueFreq = (k * expectedPhaseDiff + phaseDiff) / hopSize;
            int targetBin = static_cast<int>(std::round(k * pitchRatio));
            
            if (targetBin < numBins)
            {
                shiftedMagnitudes[targetBin] += currentMagnitudes[k];
                shiftedPhases[targetBin] = previousOutputPhases[ch][targetBin] + (trueFreq * pitchRatio * hopSize);
                previousOutputPhases[ch][targetBin] = shiftedPhases[targetBin];
            }
        }

        for (int k = 0; k < numBins; ++k)
            spectrum[k] = std::polar(shiftedMagnitudes[k], shiftedPhases[k]);
    }
}
