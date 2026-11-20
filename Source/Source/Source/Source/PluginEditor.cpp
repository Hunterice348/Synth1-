#include "PluginProcessor.h"
#include "PluginEditor.h"

HuntericeAbyssAudioProcessorEditor::HuntericeAbyssAudioProcessorEditor(
    HuntericeAbyssAudioProcessor& p,
    juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(640, 420);
    setResizable(true, true);
    setResizeLimits(480, 320, 1280, 840);

    titleLabel.setText("HUNTERICE  ABYSS", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(22.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    auto setupRotary = [](juce::Slider& s, const juce::String& name) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
        s.setName(name);
    };

    setupRotary(masterVolSlider, "MASTER");
    setupRotary(oscLevelSlider, "OSC");
    setupRotary(attackSlider, "ATTACK");
    setupRotary(decaySlider, "DECAY");
    setupRotary(sustainSlider, "SUSTAIN");
    setupRotary(releaseSlider, "RELEASE");

    masterVolAttachment.reset(new SliderAttachment(vts, "masterVol", masterVolSlider));
    oscLevelAttachment.reset(new SliderAttachment(vts, "oscLevel", oscLevelSlider));
    attackAttachment.reset(new SliderAttachment(vts, "attack", attackSlider));
    decayAttachment.reset(new SliderAttachment(vts, "decay", decaySlider));
    sustainAttachment.reset(new SliderAttachment(vts, "sustain", sustainSlider));
    releaseAttachment.reset(new SliderAttachment(vts, "release", releaseSlider));

    addAndMakeVisible(masterVolSlider);
    addAndMakeVisible(oscLevelSlider);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(releaseSlider);
}

HuntericeAbyssAudioProcessorEditor::~HuntericeAbyssAudioProcessorEditor() {}

void HuntericeAbyssAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(25, 25, 28));

    g.setColour(juce::Colour::fromRGB(139, 0, 0));
    g.fillRect(0, 0, getWidth(), 3);

    g.setColour(juce::Colour::fromRGB(18, 18, 20));
    g.fillRect(0, getHeight() - 40, getWidth(), 40);

    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("HUNTERICE ABYSS  v1.0  |  VST3  |  Windows x64",
               10, getHeight() - 40, getWidth() - 20, 40,
               juce::Justification::centredLeft);
}

void HuntericeAbyssAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(16);
    titleLabel.setBounds(area.removeFromTop(40));
    area.removeFromTop(10);

    auto knobsArea = area.removeFromTop(260);
    int knobWidth = knobsArea.getWidth() / 6;

    masterVolSlider.setBounds(knobsArea.removeFromLeft(knobWidth));
    oscLevelSlider.setBounds(knobsArea.removeFromLeft(knobWidth));
    attackSlider.setBounds(knobsArea.removeFromLeft(knobWidth));
    decaySlider.setBounds(knobsArea.removeFromLeft(knobWidth));
    sustainSlider.setBounds(knobsArea.removeFromLeft(knobWidth));
    releaseSlider.setBounds(knobsArea.removeFromLeft(knobWidth));
}
