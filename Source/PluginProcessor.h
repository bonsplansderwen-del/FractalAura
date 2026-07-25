#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "AudioEngine/GranularEngine.h"
#include "AudioEngine/SpectralProcessor.h"
#include "AudioEngine/ChaosLFO.h"

class FractalAuraAudioProcessor : public juce::AudioProcessor
{
public:
    FractalAuraAudioProcessor();
    ~FractalAuraAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    
    const juce::String getName() const override { return "Fractal Aura"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    AuraDSP::GranularEngine granularEngine;
    AuraDSP::SpectralProcessor spectralProcessor;
    AuraDSP::ChaosLFO chaosLFO;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FractalAuraAudioProcessor)
};
