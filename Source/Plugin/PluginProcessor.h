/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Envelope.h"


//==============================================================================

class CenterSpaceAudioProcessor
    : public juce::AudioProcessor
{
public:
    CenterSpaceAudioProcessor();
    ~CenterSpaceAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    juce::AudioProcessorEditor *createEditor() override;
    bool                        hasEditor() const override;

    const juce::String getName() const override;

    bool   acceptsMidi() const override;
    bool   producesMidi() const override;
    bool   isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int                getNumPrograms() override;
    int                getCurrentProgram() override;
    void               setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void               changeProgramName(int index, const juce::String &newName) override;

    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;

    float inMidLevel     = 0.0f;
    float inLeftLevel    = 0.0f;
    float inRightLevel   = 0.0f;
    float inSideLevel    = 0.0f;
    float sideChainLevel = 0.0f;
    float outLeftLevel   = 0.0f;
    float outMidLevel    = 0.0f;
    float outRightLevel  = 0.0f;
    float gainReduction  = 0.0f;

private:
    std::atomic<float> *inputGainParam        = nullptr;
    std::atomic<float> *sidechainInGainParam  = nullptr;
    std::atomic<float> *attackParam           = nullptr;
    std::atomic<float> *releaseParam          = nullptr;
    std::atomic<float> *thresholdParam        = nullptr;
    std::atomic<float> *ratioParam            = nullptr;
    std::atomic<float> *outputGainParam       = nullptr;

    std::atomic<float> *peakRMSChoice         = nullptr;

    Envelope env;

    // Pre-allocated metering buffers.
    juce::AudioBuffer<float> inLeftBuffer;
    juce::AudioBuffer<float> inMidBuffer;
    juce::AudioBuffer<float> inRightBuffer;
    juce::AudioBuffer<float> inSideBuffer;
    juce::AudioBuffer<float> sidechainBuffer;
    juce::AudioBuffer<float> outMidBuffer;

    juce::SmoothedValue<float> inGainSmoothed;
    juce::SmoothedValue<float> outGainSmoothed;
    juce::SmoothedValue<float> sideGainSmoothed;
    juce::SmoothedValue<float> thresholdSmoothed;
    juce::SmoothedValue<float> ratioReciprocalSmoothed;

    static juce::Decibels decibels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CenterSpaceAudioProcessor)
};
