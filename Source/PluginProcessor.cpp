/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CenterDuckComp2AudioProcessor::CenterDuckComp2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)
                       .withInput("Sidechain", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif

//
// ParameterFloats:
// id, description, min, max, default
// ~OR~
// id, description, normalisableRange(min, max, increment, skew, symmetric),
//                 default, param label, param category, string from value, string to value
//
// ParameterChoices:
// id, descript, choices (StringArray), default index of StringArray
//
parameters(*this, nullptr, "ParameterTree", {
    std::make_unique<AudioParameterFloat>("inGain", "Input Gain dB",
                                          NormalisableRange<float>(-100.0f, 12.0f, 0.01f, 4.0f, false), 0.0f, "dB"),
    std::make_unique<AudioParameterFloat>("sideInGain", "Sidechain Input Gain dB",
                                          NormalisableRange<float>(-100.0f, 12.0f, 0.01f, 4.0f, false), 0.0f, "dB"),
    std::make_unique<AudioParameterFloat>("attack", "Attack ms",
                                          NormalisableRange<float>(0.01f, 2000.0f, 0.01f, 0.15f, false), 0.2f, "ms"),
    std::make_unique<AudioParameterFloat>("release", "Release ms",
                                          NormalisableRange<float>(0.1f, 2000.0f, 0.01f, 0.15f, false), 2.0f, "ms"),
    std::make_unique<AudioParameterFloat>("threshold", "Threshold dB",
                                          NormalisableRange<float>(-100.0f, 12.0f, 0.01f, 4.0f, false), 0.0f, "dB"),
    std::make_unique<AudioParameterFloat>("ratio", "Ratio",
                                          NormalisableRange<float>(1.0f, 20.0f, 0.1f, 0.4f, false), 1.0f, ":1"),
    std::make_unique<AudioParameterFloat>("outGain", "Output Gain dB",
                                          NormalisableRange<float>(-100.0f, 12.0f, 0.01f, 4.0f, false), 0.0f, "dB"),
    std::make_unique<AudioParameterChoice>("peakRMS", "Peak/RMS", StringArray( {"Peak", "RMS"} ), 0)
})

{
    inputGainParam = parameters.getRawParameterValue("inGain");
    sidechainInGainParam = parameters.getRawParameterValue("sideInGain");
    attackParam = parameters.getRawParameterValue("attack");
    releaseParam = parameters.getRawParameterValue("release");
    thresholdParam = parameters.getRawParameterValue("threshold");
    ratioParam = parameters.getRawParameterValue("ratio");
    outputGainParam = parameters.getRawParameterValue("outGain");
    peakRMSChoice = parameters.getRawParameterValue("peakRMS");
}

CenterDuckComp2AudioProcessor::~CenterDuckComp2AudioProcessor()
{
}

//==============================================================================
const juce::String CenterDuckComp2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CenterDuckComp2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CenterDuckComp2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CenterDuckComp2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CenterDuckComp2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CenterDuckComp2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CenterDuckComp2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void CenterDuckComp2AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CenterDuckComp2AudioProcessor::getProgramName (int index)
{
    return {};
}

void CenterDuckComp2AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CenterDuckComp2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Initial setup for Envelope instance
    env.setSampleRate(sampleRate);
    env.setAttackTime(*attackParam);
    env.setReleaseTime(*releaseParam);
}

void CenterDuckComp2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CenterDuckComp2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& mainOutput = layouts.getMainOutputChannelSet();
    const auto& mainInput  = layouts.getMainInputChannelSet();
    
    // the sidechain can take any layout, the main bus needs to be the same on the input and output
    return mainInput == mainOutput && ! mainInput.isDisabled();

}
#endif

void CenterDuckComp2AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Gain Parameters
    float inGainDB = *inputGainParam;
    float outGainDB = *outputGainParam;
    float sideInGainDB = *sidechainInGainParam;
    
    float inGainAmp = decibels.decibelsToGain(inGainDB);
    float outGainAmp = decibels.decibelsToGain(outGainDB);
    float sideGainAmp = decibels.decibelsToGain(sideInGainDB);
    
    
    // Update Envelope Parameters if they've changed
    if (env.getSampleRate() != getSampleRate())
        env.setSampleRate( getSampleRate() );
    
    env.setAttackTime(*attackParam);
    env.setReleaseTime(*releaseParam);
    
    // Compression Parameters
    float thresholdDB = *thresholdParam;
    float thresholdAmp = decibels.decibelsToGain(thresholdDB);
    float thresholdInverse = 1.0f / thresholdAmp;
    float ratio = 1.0f / *ratioParam;
    
    // Channel setups
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear buffers
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // Separate Main IO from Sidechain
    auto mainInputOutput = getBusBuffer (buffer, true, 0);
    auto sideChainInput  = getBusBuffer (buffer, true, 1);
    
    // Buffer setup
    float* leftChannel = mainInputOutput.getWritePointer(0);
    float* rightChannel;
    
    if (mainInputOutput.getWritePointer(1))
        rightChannel = mainInputOutput.getWritePointer(1);
    else
        rightChannel = mainInputOutput.getWritePointer(0);
    

    // DSP!
    for (int i=0; i<buffer.getNumSamples(); i++)
    {
        // Encode Main Stereo to MS
        float mid = (leftChannel[i] + rightChannel[i]) * inGainAmp;
        float side = (leftChannel[i] - rightChannel[i]) * inGainAmp;
        
        inMidLevel = std::fabs(mid);
        
        // Mono the sidechain
        float monoSidechainSample = 0.0f;
        
        for (int j = 0; j < sideChainInput.getNumChannels(); ++j)
        {
            monoSidechainSample += sideChainInput.getWritePointer(j)[i];                // Combine all amplitudes
        }
        
        // Divide amplitudes by channel count without dividing by zero, then Mult by sidechain gain
        monoSidechainSample /= (sideChainInput.getNumChannels() < 1.0f) ? 1.0f : static_cast<float> (sideChainInput.getNumChannels());
        monoSidechainSample *= sideGainAmp;

        // Run sidechain values through the envelope
        float envVal = env.process(monoSidechainSample, peakRMSChoice);
        
        // Compressor gain
        float compGain = (envVal < thresholdAmp) ? 1.0f : std::pow(envVal * thresholdInverse, ratio - 1.0f);
        
        // Apply gain to mid channel
        float midComped = mid * compGain;
        

        // Encode Main MS to Stereo
        float gainCompensation = 0.5f;
        leftChannel[i] = ( (midComped + side) * outGainAmp ) * gainCompensation;
        rightChannel[i] = ( (midComped - side) * outGainAmp ) * gainCompensation;
        
        //outMeterVal = leftChannel[i];

    }   // DSP
    
    //outMeter.getCurrentLevel();
    
}

//==============================================================================
bool CenterDuckComp2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CenterDuckComp2AudioProcessor::createEditor()
{
    return new CenterDuckComp2AudioProcessorEditor (*this);
    //return new GenericAudioProcessorEditor (*this);
}

//==============================================================================
void CenterDuckComp2AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // getStateInformation
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void CenterDuckComp2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // setStateInformation
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (parameters.state.getType()))
        {
            parameters.replaceState (ValueTree::fromXml (*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CenterDuckComp2AudioProcessor();
}


//==============================================================================
//==============================================================================
//==============================================================================


