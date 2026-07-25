#include "MainEditor.h"

FractalAuraEditor::FractalAuraEditor(FractalAuraAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(900, 500);

    auto setupSlider = [this](juce::Slider& s, const juce::String& paramID, std::unique_ptr<Attachment>& attach)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(s);
        attach = std::make_unique<Attachment>(audioProcessor.apvts, paramID, s);
    };

    setupSlider(grainSizeSlider, "grainSize", grainSizeAttach);
    setupSlider(densitySlider, "density", densityAttach);
    setupSlider(pitchSlider, "pitchShift", pitchAttach);
    setupSlider(tiltSlider, "spectralTilt", tiltAttach);
    setupSlider(mixSlider, "mix", mixAttach);

    addAndMakeVisible(freezeButton);
    freezeButton.setColour(juce::ToggleButton::textColourId, juce::Colour(0xff00F5FF));
    freezeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "spectralFreeze", freezeButton);

    startTimerHz(60);
}

FractalAuraEditor::~FractalAuraEditor() { stopTimer(); }

void FractalAuraEditor::timerCallback()
{
    phase += 0.03f;
    repaint();
}

void FractalAuraEditor::paint(juce::Graphics& g)
{
    juce::ColourGradient bgGrad(juce::Colour(0xff090514), getLocalBounds().getCentre().toFloat(),
                                juce::Colour(0xff020105), juce::Point<float>(0, 0), true);
    g.setGradientFill(bgGrad);
    g.fillAll();

    auto area = getLocalBounds().toFloat().reduced(60.0f);
    juce::Path auraRing;
    int points = 12;
    float radius = 180.0f + std::sin(phase) * 15.0f;
    juce::Point<float> center = area.getCentre();

    for (int i = 0; i < points; ++i)
    {
        float angle = i * juce::MathConstants<float>::twoPi / points + phase * 0.15f;
        float x = center.x + radius * std::cos(angle);
        float y = center.y + radius * std::sin(angle);
        if (i == 0) auraRing.startNewSubPath(x, y);
        else auraRing.lineTo(x, y);
    }
    auraRing.closeSubPath();

    g.setColour(juce::Colour(0xff00F5FF).withAlpha(0.2f));
    g.strokePath(auraRing, juce::PathStrokeType(2.5f));

    g.setColour(juce::Colour(0xff8A2BE2));
    g.setFont(juce::FontOptions("Helvetica", 32.0f, juce::Font::bold));
    g.drawText("F R A C T A L  A U R A", getLocalBounds().removeFromTop(80), juce::Justification::centred);
}

void FractalAuraEditor::resized()
{
    auto bounds = getLocalBounds().reduced(40);
    bounds.removeFromTop(60);
    
    auto topRow = bounds.removeFromTop(200);
    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::FlexDirection::row;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    flexBox.items.add(juce::FlexItem(grainSizeSlider).withFlex(1));
    flexBox.items.add(juce::FlexItem(densitySlider).withFlex(1));
    flexBox.items.add(juce::FlexItem(pitchSlider).withFlex(1));
    flexBox.items.add(juce::FlexItem(tiltSlider).withFlex(1));
    flexBox.items.add(juce::FlexItem(mixSlider).withFlex(1));
    flexBox.performLayout(topRow);

    freezeButton.setBounds(bounds.getCentreX() - 60, bounds.getBottom() - 40, 120, 30);
}
