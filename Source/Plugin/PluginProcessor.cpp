/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================

CenterSpaceAudioProcessor::CenterSpaceAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: juce::AudioProcessor(BusesProperties()
                      #if ! JucePlugin_IsMidiEffect
                       #if ! JucePlugin_IsSynth
                        .withInput ("Input",     juce::AudioChannelSet::stereo(), true)
                       #endif
                        .withOutput("Output",    juce::AudioChannelSet::stereo(), true)
                        .withInput ("Sidechain", juce::AudioChannelSet::stereo(), true)
                      #endif
                       )
,
#endif
  parameters(*this, nullptr, "ParameterTree", {
      std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"inGain",     1}, "Input Gain dB",           juce::NormalisableRange<float>(-100.0f, 12.0f, 0.01f, 4.0f, false),  0.0f, "dB")
    , std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"sideInGain", 1}, "Sidechain Input Gain dB", juce::NormalisableRange<float>(-100.0f, 12.0f, 0.01f, 4.0f, false),  0.0f, "dB")
    , std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"attack",     1}, "Attack ms",               juce::NormalisableRange<float>(0.01f, 2000.0f, 0.01f, 0.15f, false), 0.2f, "ms")
    , std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"release",    1}, "Release ms",              juce::NormalisableRange<float>(1.0f, 2000.0f, 0.01f, 0.15f, false),  2.0f, "ms")
    , std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"threshold",  1}, "Threshold dB",            juce::NormalisableRange<float>(-100.0f, 12.0f, 0.01f, 4.0f, false),  0.0f, "dB")
    , std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"ratio",      1}, "Ratio",                   juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f, 0.4f, false),      1.0f, ":1")
    , std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"outGain",    1}, "Output Gain dB",          juce::NormalisableRange<float>(-100.0f, 12.0f, 0.01f, 4.0f, false),  0.0f, "dB")
    , std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"peakRMS",   1}, "Peak/RMS",                juce::StringArray({"Peak", "RMS"}), 0)
  })
{
    inputGainParam       = parameters.getRawParameterValue("inGain");
    sidechainInGainParam = parameters.getRawParameterValue("sideInGain");
    attackParam          = parameters.getRawParameterValue("attack");
    releaseParam         = parameters.getRawParameterValue("release");
    thresholdParam       = parameters.getRawParameterValue("threshold");
    ratioParam           = parameters.getRawParameterValue("ratio");
    outputGainParam      = parameters.getRawParameterValue("outGain");
    peakRMSChoice        = parameters.getRawParameterValue("peakRMS");
}

CenterSpaceAudioProcessor::~CenterSpaceAudioProcessor() {}


const juce::String CenterSpaceAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CenterSpaceAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CenterSpaceAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CenterSpaceAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CenterSpaceAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CenterSpaceAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CenterSpaceAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CenterSpaceAudioProcessor::setCurrentProgram(int /*index*/) {}

const juce::String CenterSpaceAudioProcessor::getProgramName(int /*index*/)
{
    return {};
}

void CenterSpaceAudioProcessor::changeProgramName(int index, const juce::String &/*newName*/) {}


void CenterSpaceAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    env.SetSampleRate (sampleRate);
    env.SetAttackTime (*attackParam);
    env.SetReleaseTime(*releaseParam);

    inLeftBuffer.setSize    (1, samplesPerBlock, false, true, false);
    inMidBuffer.setSize     (1, samplesPerBlock, false, true, false);
    inRightBuffer.setSize   (1, samplesPerBlock, false, true, false);
    inSideBuffer.setSize    (1, samplesPerBlock, false, true, false);
    sidechainBuffer.setSize (1, samplesPerBlock, false, true, false);
    outMidBuffer.setSize    (1, samplesPerBlock, false, true, false);
}

void CenterSpaceAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CenterSpaceAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    const auto &mainOutput = layouts.getMainOutputChannelSet();
    const auto &mainInput  = layouts.getMainInputChannelSet();

    // the sidechain can take any layout, the main bus needs to be the same on the input and output
    return mainInput == mainOutput && ! mainInput.isDisabled();
}
#endif

void CenterSpaceAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &/*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    inLeftBuffer.clear();
    inMidBuffer.clear();
    inRightBuffer.clear();
    inSideBuffer.clear();
    sidechainBuffer.clear();
    outMidBuffer.clear();

    const int   numSamples = buffer.getNumSamples();
    const int   peakMode   = (int)peakRMSChoice->load();

    const float inGainAmp   = decibels.decibelsToGain(inputGainParam->load());
    const float outGainAmp  = decibels.decibelsToGain(outputGainParam->load());
    const float sideGainAmp = decibels.decibelsToGain(sidechainInGainParam->load());

    const float thresholdAmp     = decibels.decibelsToGain(thresholdParam->load());
    const float thresholdInverse = 1.0f / thresholdAmp;
    const float ratio            = 1.0f / ratioParam->load();

    const float gainCompensation = 0.5f;
    const float outScale         = outGainAmp * gainCompensation;

    const double currentSR = getSampleRate();

    if (env.GetSampleRate() != currentSR)
        env.SetSampleRate((float)currentSR);

    env.SetAttackTime (attackParam->load());
    env.SetReleaseTime(releaseParam->load());

    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    auto mainInputOutput = getBusBuffer(buffer, true, 0);
    auto sideChainInput  = getBusBuffer(buffer, true, 1);

    float *leftChannel  = mainInputOutput.getWritePointer(0);
    float *rightChannel = (mainInputOutput.getNumChannels() > 1)
                              ? mainInputOutput.getWritePointer(1)
                              : leftChannel;

    const int           numSCChannels = sideChainInput.getNumChannels();
    const float        *scReadPointers[2] = { nullptr, nullptr };
    const int           clampedSCChannels = juce::jmin(2, numSCChannels);

    for (int j = 0; j < clampedSCChannels; ++j)
        scReadPointers[j] = sideChainInput.getReadPointer(j);

    const float scChannelScale = (numSCChannels < 1)
                                     ? 1.0f
                                     : (1.0f / (float)numSCChannels);

    // --- Sample loop ---
    for (int i = 0; i < numSamples; ++i)
    {
        // Encode Main Stereo to MS
        const float mid  = (leftChannel[i] + rightChannel[i]) * inGainAmp;
        const float side = (leftChannel[i] - rightChannel[i]) * inGainAmp;

        // Input Metering
        inLeftBuffer.addSample (0, i, leftChannel[i]  * inGainAmp);
        inMidBuffer.addSample  (0, i, mid);
        inRightBuffer.addSample(0, i, rightChannel[i] * inGainAmp);
        inSideBuffer.addSample (0, i, side);

        // Mono the sidechain
        float monoSidechainSample = 0.0f;

        for (int j = 0; j < clampedSCChannels; ++j)
            monoSidechainSample += scReadPointers[j][i];

        monoSidechainSample *= scChannelScale * sideGainAmp;

        // Sidechain Metering
        sidechainBuffer.addSample(0, i, monoSidechainSample);

        // Run sidechain through the envelope
        const float envVal = env.Process(monoSidechainSample, peakMode);

        // Compressor gain
        const float compGain = (envVal < thresholdAmp)
                                   ? 1.0f
                                   : std::pow(envVal * thresholdInverse, ratio - 1.0f);

        const float midComped = mid * compGain;

        // Output Metering
        outMidBuffer.addSample(0, i, midComped);

        // Encode Main MS to Stereo
        leftChannel[i]  = (midComped + side) * outScale;
        rightChannel[i] = (midComped - side) * outScale;
    }

    if (peakMode == 1)
    {
        inLeftLevel  = inLeftBuffer.getRMSLevel (0, 0, numSamples);
        inMidLevel   = inMidBuffer.getRMSLevel  (0, 0, numSamples) * 0.5f;
        inRightLevel = inRightBuffer.getRMSLevel(0, 0, numSamples);
        inSideLevel  = inSideBuffer.getRMSLevel (0, 0, numSamples);

        sideChainLevel = sidechainBuffer.getRMSLevel(0, 0, numSamples);

        outLeftLevel  = buffer.getRMSLevel      (0, 0, numSamples);
        outMidLevel   = outMidBuffer.getRMSLevel(0, 0, numSamples) * 0.5f;
        outRightLevel = buffer.getRMSLevel      (1, 0, numSamples);

        gainReduction = inMidLevel - outMidLevel;
        outMidLevel  *= outGainAmp;
    }
    else
    {
        inLeftLevel  = inLeftBuffer.getMagnitude (0, numSamples);
        inMidLevel   = inMidBuffer.getMagnitude  (0, numSamples) * 0.5f;
        inRightLevel = inRightBuffer.getMagnitude(0, numSamples);
        inSideLevel  = inSideBuffer.getMagnitude (0, numSamples);

        sideChainLevel = sidechainBuffer.getMagnitude(0, numSamples);

        outLeftLevel  = buffer.getMagnitude      (0, 0, numSamples);
        outMidLevel   = outMidBuffer.getMagnitude(0, numSamples) * 0.5f;
        outRightLevel = buffer.getMagnitude      (1, 0, numSamples);

        gainReduction = inMidLevel - outMidLevel;
        outMidLevel  *= outGainAmp;
    }
}

bool CenterSpaceAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor *CenterSpaceAudioProcessor::createEditor()
{
    return new CenterSpaceAudioProcessorEditor(*this);
}

void CenterSpaceAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CenterSpaceAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new CenterSpaceAudioProcessor();
}
