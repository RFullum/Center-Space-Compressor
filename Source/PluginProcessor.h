/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Envelope.h"

//==============================================================================
/**
*/
class CenterDuckComp2AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    CenterDuckComp2AudioProcessor();
    ~CenterDuckComp2AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // Parameters
    AudioProcessorValueTreeState parameters;
    
    float inMidLevel;//     = 0.0f;
    float inLeftLevel;//    = 0.0f;
    float inRightLevel;//   = 0.0f;
    float inSideLevel;//    = 0.0f;
    float sideChainLevel;// = 0.0f;
    float outLeftLevel;//   = 0.0f;
    float outMidLevel;//    = 0.0f;
    float outRightLevel;//  = 0.0f;
    float gainReduction;//  = 0.0f;

private:
    // Parameters
    std::atomic<float>* inputGainParam;
    std::atomic<float>* sidechainInGainParam;
    std::atomic<float>* attackParam;
    std::atomic<float>* releaseParam;
    std::atomic<float>* thresholdParam;
    std::atomic<float>* ratioParam;
    std::atomic<float>* outputGainParam;
    
    std::atomic<float>* peakRMSChoice;
    
    // Envelope Instance
    Envelope env;
    
    // Decibels for conversions
    static Decibels decibels;
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CenterDuckComp2AudioProcessor)
};
