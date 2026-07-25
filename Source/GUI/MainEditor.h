#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../PluginProcessor.h"

class FractalAuraEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    FractalAuraEditor(FractalAuraAudioProcessor&);
    ~FractalAuraEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    FractalAuraAudioProcessor& audioProcessor;
    float phase { 0.0f };

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    juce::Slider grainSizeSlider, densitySlider, pitchSlider, mixSlider, tiltSlider;
    std::unique_ptr<Attachment> grainSizeAttach, densityAttach, pitchAttach, mixAttach, tiltAttach;
    
    juce::ToggleButton freezeButton{"Freeze Spectrum"};
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> freezeAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FractalAuraEditor)
};
