#pragma once
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <random>

namespace AuraDSP
{
    struct Grain
    {
        bool active { false };
        float position { 0.0f }, duration { 0.1f }, playhead { 0.0f };
        float pitchRatio { 1.0f }, panL { 0.707f }, panR { 0.707f };
        bool isReverse { false };
    };

    class GranularEngine
    {
    public:
        GranularEngine() = default;

        void prepare(const juce::dsp::ProcessSpec& spec)
        {
            sampleRate = spec.sampleRate;
            bufferLength = static_cast<int>(sampleRate * 4.0);
            ringBuffer.setSize(spec.numChannels, bufferLength);
            ringBuffer.clear();
            grains.resize(maxGrains);
            writePos = 0;
        }

        void process(const juce::AudioBuffer<float>& input, juce::AudioBuffer<float>& output, 
                     float grainSizeMs, float density, float pitchShift, float reverseProbability)
        {
            const int numSamples = input.getNumSamples();
            const int numChannels = input.getNumChannels();

            for (int ch = 0; ch < numChannels; ++ch)
            {
                const float* inData = input.getReadPointer(ch);
                float* ringData = ringBuffer.getWritePointer(ch);
                for (int s = 0; s < numSamples; ++s)
                    ringData[(writePos + s) % bufferLength] = inData[s];
            }

            spawnTimer += numSamples;
            int spawnInterval = static_cast<int>((sampleRate / std::max(0.1f, density)));
            if (spawnTimer >= spawnInterval)
            {
                spawnTimer = 0;
                triggerGrain(grainSizeMs, pitchShift, reverseProbability);
            }

            output.clear();
            for (auto& grain : grains)
            {
                if (!grain.active) continue;
                for (int s = 0; s < numSamples; ++s)
                {
                    float window = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * grain.playhead / grain.duration));
                    int readIdx = static_cast<int>(grain.position) % bufferLength;
                    if (readIdx < 0) readIdx += bufferLength;

                    float sL = ringBuffer.getSample(0, readIdx) * window * grain.panL;
                    float sR = ringBuffer.getSample(numChannels > 1 ? 1 : 0, readIdx) * window * grain.panR;

                    output.addSample(0, s, sL);
                    if (output.getNumChannels() > 1) output.addSample(1, s, sR);

                    grain.playhead += 1.0f / sampleRate;
                    grain.position += grain.isReverse ? -grain.pitchRatio : grain.pitchRatio;

                    if (grain.playhead >= grain.duration) { grain.active = false; break; }
                }
            }
            writePos = (writePos + numSamples) % bufferLength;
        }

    private:
        void triggerGrain(float sizeMs, float pitchShift, float revProb)
        {
            for (auto& g : grains)
            {
                if (!g.active)
                {
                    g.active = true;
                    g.duration = std::max(0.005f, sizeMs * 0.001f);
                    g.playhead = 0.0f;
                    g.pitchRatio = std::pow(2.0f, pitchShift / 12.0f);
                    float offsetSec = dist(rng) * 2.0f;
                    g.position = static_cast<float>(writePos) - (offsetSec * sampleRate);
                    float pan = dist(rng);
                    g.panL = std::cos(pan * juce::MathConstants<float>::halfPi);
                    g.panR = std::sin(pan * juce::MathConstants<float>::halfPi);
                    g.isReverse = dist(rng) < revProb;
                    break;
                }
            }
        }

        static constexpr size_t maxGrains = 64;
        std::vector<Grain> grains;
        juce::AudioBuffer<float> ringBuffer;
        int bufferLength { 0 }, writePos { 0 }, spawnTimer { 0 };
        double sampleRate { 44100.0 };
        std::mt19937 rng { std::random_device{}() };
        std::uniform_real_distribution<float> dist { 0.0f, 1.0f };
    };
}
