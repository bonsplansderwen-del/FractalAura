#include "PluginProcessor.h"
#include "GUI/MainEditor.h"

FractalAuraAudioProcessor::FractalAuraAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout FractalAuraAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"grainSize", 1}, "Grain Size", 10.0f, 500.0f, 100.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"density", 1}, "Density", 1.0f, 50.0f, 15.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"pitchShift", 1}, "Pitch Shift", -24.0f, 24.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"chaosRate", 1}, "Chaos Rate", 0.01f, 10.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"spectralFreeze", 1}, "Freeze", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"spectralTilt", 1}, "Spectral Tilt", -1.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"mix", 1}, "Dry/Wet Mix", 0.0f, 1.0f, 0.5f));

    return { params.begin(), params.end() };
}

void FractalAuraAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 };
    granularEngine.prepare(spec);
    spectralProcessor.prepare(spec);
    chaosLFO.setSpeed(*apvts.getRawParameterValue("chaosRate"));
}

void FractalAuraAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const float grainSize = apvts.getRawParameterValue("grainSize")->load();
    const float density   = apvts.getRawParameterValue("density")->load();
    const float pitch     = apvts.getRawParameterValue("pitchShift")->load();
    const float chaosRate = apvts.getRawParameterValue("chaosRate")->load();
    const float mix       = apvts.getRawParameterValue("mix")->load();
    const bool  freeze    = apvts.getRawParameterValue("spectralFreeze")->load() > 0.5f;
    const float tilt      = apvts.getRawParameterValue("spectralTilt")->load();

    chaosLFO.setSpeed(chaosRate);
    float modPitch = pitch + (chaosLFO.getNextSample() * 6.0f);

    juce::AudioBuffer<float> wetBuffer;
    wetBuffer.makeCopyOf(buffer);

    granularEngine.process(buffer, wetBuffer, grainSize, density, modPitch, 0.25f);
    spectralProcessor.process(wetBuffer, pitch, freeze, tilt);

    const float dryGain = std::cos(mix * juce::MathConstants<float>::halfPi);
    const float wetGain = std::sin(mix * juce::MathConstants<float>::halfPi);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        buffer.applyGain(ch, 0, buffer.getNumSamples(), dryGain);
        buffer.addFrom(ch, 0, wetBuffer, ch, 0, buffer.getNumSamples(), wetGain);
    }
}

juce::AudioProcessorEditor* FractalAuraAudioProcessor::createEditor()
{
    return new FractalAuraEditor(*this);
}

void FractalAuraAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FractalAuraAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new FractalAuraAudioProcessor(); }
