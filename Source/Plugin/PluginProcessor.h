/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


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

    std::atomic<float> inMidLevel     { 0.0f };
    std::atomic<float> inLeftLevel    { 0.0f };
    std::atomic<float> inRightLevel   { 0.0f };
    std::atomic<float> inSideLevel    { 0.0f };
    std::atomic<float> sideChainLevel { 0.0f };
    std::atomic<float> outLeftLevel   { 0.0f };
    std::atomic<float> outMidLevel    { 0.0f };
    std::atomic<float> outRightLevel  { 0.0f };
    std::atomic<float> gainReduction  { 0.0f };

private:
    // Effective-value derivation layer.
    // Tweak (full control) mode: returns the matching primitive directly
    // (with the Opto style override applied to peakMode and knee);
    // Vibe mode (minimal control): derives from the macros
    float GetEffectiveSideInGainDb()     const;
    float GetEffectiveScHpfHz()          const;
    float GetEffectiveScLpfHz()          const;
    int   GetEffectivePeakMode()         const;
    float GetEffectiveAttackMs()         const;
    float GetEffectiveReleaseMs()        const;
    int   GetEffectiveStyle()            const;
    float GetEffectiveThresholdDb()      const;
    float GetEffectiveRatio()            const;
    float GetEffectiveKneeDb()           const;
    int   GetEffectiveLookaheadSamples() const;

    double currentSampleRate = 44100.0;

    // Shared
    std::atomic<float>       *uiModeChoice         = nullptr;
    std::atomic<float>       *inputTypeChoice      = nullptr;
    std::atomic<float>       *inputGainParam       = nullptr;
    std::atomic<float>       *outputGainParam      = nullptr;
    std::atomic<float>       *outputTypeChoice     = nullptr;
    juce::AudioParameterBool *bypassParam          = nullptr;

    // Tweak-mode direct params
    std::atomic<float> *sidechainInGainParam  = nullptr;
    std::atomic<float> *scHpfHzParam          = nullptr;
    std::atomic<float> *scLpfHzParam          = nullptr;
    std::atomic<float> *peakRMSChoice         = nullptr;
    std::atomic<float> *attackParam           = nullptr;
    std::atomic<float> *releaseParam          = nullptr;
    std::atomic<float> *styleChoice           = nullptr;
    std::atomic<float> *thresholdParam        = nullptr;
    std::atomic<float> *ratioParam            = nullptr;
    std::atomic<float> *kneeParam             = nullptr;
    std::atomic<float> *lookaheadChoice       = nullptr;

    // Vibe-mode macros
    std::atomic<float>       *feelChoice          = nullptr;
    std::atomic<float>       *compressParam       = nullptr;
    std::atomic<float>       *reactParam          = nullptr;
    std::atomic<float>       *focusChoice         = nullptr;
    juce::AudioParameterBool *lookaheadOnOffParam = nullptr;

    juce::dsp::BallisticsFilter<float>        envelope;
    juce::dsp::StateVariableTPTFilter<float>  scHpf;
    juce::dsp::StateVariableTPTFilter<float>  scLpf;

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
    juce::SmoothedValue<float> thresholdSmoothed;     // dB
    juce::SmoothedValue<float> ratioReciprocalSmoothed;
    juce::SmoothedValue<float> kneeSmoothed;          // dB
    juce::SmoothedValue<float> scHpfSmoothed;
    juce::SmoothedValue<float> scLpfSmoothed;

    static juce::Decibels decibels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CenterSpaceAudioProcessor)
};
