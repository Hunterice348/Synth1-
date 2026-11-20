#include "PluginProcessor.h"
#include "PluginEditor.h"

HuntericeAbyssAudioProcessor::HuntericeAbyssAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

HuntericeAbyssAudioProcessor::~HuntericeAbyssAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
HuntericeAbyssAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"oscLevel", 1}, "Oscillator Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"masterVol", 1}, "Master Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"attack", 1}, "Attack",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), 0.01f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"decay", 1}, "Decay",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), 0.1f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"sustain", 1}, "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"release", 1}, "Release",
        juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.3f), 0.3f));

    return {params.begin(), params.end()};
}

void HuntericeAbyssAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    oscillator.initialise([](float x) {
        return x < 0.5f ? 2.0f * x - 1.0f : 2.0f * (1.0f - x);
    }, 128);
    oscillator.prepare(spec);
    oscillator.setFrequency(440.0f);

    masterGain.prepare(spec);
    masterGain.setGainLinear(0.8f);

    adsr.setSampleRate(sampleRate);

    juce::dsp::ADSR::Parameters p;
    p.attack = *apvts.getRawParameterValue("attack");
    p.decay = *apvts.getRawParameterValue("decay");
    p.sustain = *apvts.getRawParameterValue("sustain");
    p.release = *apvts.getRawParameterValue("release");
    adsr.setParameters(p);
}

void HuntericeAbyssAudioProcessor::releaseResources() {}

void HuntericeAbyssAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    juce::dsp::ADSR::Parameters p;
    p.attack = *apvts.getRawParameterValue("attack");
    p.decay = *apvts.getRawParameterValue("decay");
    p.sustain = *apvts.getRawParameterValue("sustain");
    p.release = *apvts.getRawParameterValue("release");
    adsr.setParameters(p);

    masterGain.setGainLinear(*apvts.getRawParameterValue("masterVol"));

    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            oscillator.setFrequency(msg.getFrequencyInHertz());
            adsr.noteOn();
        }
        else if (msg.isNoteOff())
        {
            adsr.noteOff();
        }
    }

    const float oscLevel = *apvts.getRawParameterValue("oscLevel");

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float env = adsr.getNextSample();
        float osc = oscillator.processSample(0.0f);
        float out = osc * env * oscLevel;

        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
            buffer.setSample(channel, sample, out);
    }

    juce::dsp::AudioBlock<float> block(buffer);
    masterGain.process(juce::dsp::ProcessContextReplacing<float>(block));

    midiMessages.clear();
}

juce::AudioProcessorEditor* HuntericeAbyssAudioProcessor::createEditor()
{
    return new HuntericeAbyssAudioProcessorEditor(*this, apvts);
}

void HuntericeAbyssAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void HuntericeAbyssAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HuntericeAbyssAudioProcessor();
}
