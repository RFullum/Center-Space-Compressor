/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Derivations.h"


//==============================================================================

namespace
{
    // Vibe = minimal UI controls; Tweak = all UI param controls
    static constexpr int uiModeVibe  = 0;
    static constexpr int uiModeTweak = 1;
}

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
      // Shared
      std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"uiMode",     1}, "UI Mode",                 juce::StringArray({"Vibe", "Tweak"}), 0)
    , std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"inputType",  1}, "Input Type",              juce::StringArray({"LR", "M/S"}), 0)
    , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"inGain",     1}, "Input Gain dB",           juce::NormalisableRange<float>(-100.0f, 12.0f, 0.01f, 4.0f, false),    0.0f, "dB")
    , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"outGain",    1}, "Output Gain dB",          juce::NormalisableRange<float>(-100.0f, 12.0f, 0.01f, 4.0f, false),    0.0f, "dB")
    , std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"outputType", 1}, "Output Type",             juce::StringArray({"LR", "M/S"}), 0)
    , std::make_unique<juce::AudioParameterBool>  (juce::ParameterID{"bypass",     1}, "Bypass",                  false)

      // Tweak-mode primitives
    , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"sideInGain", 1}, "Sidechain Input Gain dB", juce::NormalisableRange<float>(-100.0f, 12.0f, 0.01f, 4.0f, false),    0.0f, "dB")
    , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"scHpfHz",    1}, "SC HPF Hz",               juce::NormalisableRange<float>(20.0f, 2000.0f, 0.01f, 0.3f, false),    20.0f, "Hz")
    , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"scLpfHz",    1}, "SC LPF Hz",               juce::NormalisableRange<float>(200.0f, 20000.0f, 0.01f, 0.3f, false),  20000.0f, "Hz")
    , std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"peakRMS",    1}, "Peak/RMS",                juce::StringArray({"Peak", "RMS"}), 0)
    , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"attack",     1}, "Attack ms",               juce::NormalisableRange<float>(0.01f, 2000.0f, 0.01f, 0.15f, false),   10.0f, "ms")
    , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"release",    1}, "Release ms",              juce::NormalisableRange<float>(1.0f, 2000.0f, 0.01f, 0.15f, false),    100.0f, "ms")
    , std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"style",      1}, "Style",                   juce::StringArray({"Modern VCA", "Opto"}), 0)
    , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"threshold",  1}, "Threshold dB",            juce::NormalisableRange<float>(-100.0f, 12.0f, 0.01f, 4.0f, false),    0.0f, "dB")
    , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"ratio",      1}, "Ratio",                   juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f, 0.4f, false),        1.0f, ":1")
    , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"knee",       1}, "Knee dB",                 juce::NormalisableRange<float>(0.0f, 24.0f, 0.01f),                    0.0f, "dB")
    , std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"lookahead",  1}, "Lookahead",               juce::StringArray({"0 ms", "1 ms", "4 ms", "10 ms"}), 0)

      // Vibe-mode macros
    , std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"feel",       1}, "Feel",                    juce::StringArray({"Clean", "Smooth"}), 0)
    , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"compress",   1}, "Compress",                juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),                    0.0f)
    , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"react",      1}, "React",                   juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),                    0.5f)
    , std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"focus",      1}, "Focus",                   juce::StringArray({"Full Range"
                                                                                                                                     , "Reduce Bass"
                                                                                                                                     , "Transient Focus"
                                                                                                                                     , "Lows"
                                                                                                                                     , "Low Mid"
                                                                                                                                     , "High Mid"
                                                                                                                                     , "High"
                                                                                                                                     , "Vocal Body"
                                                                                                                                     , "Vocal Clarity"
                                                                                                                                     , "Kick Thump"
                                                                                                                                     , "Kick Smack"
                                                                                                                                     , "Snare Thump"
                                                                                                                                     , "Snare Smack"
                                                                                                                                     , "Bass Body"
                                                                                                                                     , "Hats Range"}), 0)
    , std::make_unique<juce::AudioParameterBool>  (juce::ParameterID{"lookaheadOnOff", 1}, "Lookahead On/Off",    false)
  })
{
    // Shared
    uiModeChoice         = parameters.getRawParameterValue("uiMode");
    inputTypeChoice      = parameters.getRawParameterValue("inputType");
    inputGainParam       = parameters.getRawParameterValue("inGain");
    outputGainParam      = parameters.getRawParameterValue("outGain");
    outputTypeChoice     = parameters.getRawParameterValue("outputType");
    bypassParam          = dynamic_cast<juce::AudioParameterBool *>(parameters.getParameter("bypass"));

    // Tweak-mode direct params
    sidechainInGainParam = parameters.getRawParameterValue("sideInGain");
    scHpfHzParam         = parameters.getRawParameterValue("scHpfHz");
    scLpfHzParam         = parameters.getRawParameterValue("scLpfHz");
    peakRMSChoice        = parameters.getRawParameterValue("peakRMS");
    attackParam          = parameters.getRawParameterValue("attack");
    releaseParam         = parameters.getRawParameterValue("release");
    styleChoice          = parameters.getRawParameterValue("style");
    thresholdParam       = parameters.getRawParameterValue("threshold");
    ratioParam           = parameters.getRawParameterValue("ratio");
    kneeParam            = parameters.getRawParameterValue("knee");
    lookaheadChoice      = parameters.getRawParameterValue("lookahead");

    // Vibe-mode macro params
    feelChoice           = parameters.getRawParameterValue("feel");
    compressParam        = parameters.getRawParameterValue("compress");
    reactParam           = parameters.getRawParameterValue("react");
    focusChoice          = parameters.getRawParameterValue("focus");
    lookaheadOnOffParam  = dynamic_cast<juce::AudioParameterBool *>(parameters.getParameter("lookaheadOnOff"));
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
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = (juce::uint32)samplesPerBlock;
    spec.numChannels      = 1;

    envelope.prepare(spec);
    envelope.setAttackTime (GetEffectiveAttackMs());
    envelope.setReleaseTime(GetEffectiveReleaseMs());

    scHpf.prepare(spec);
    scHpf.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    scHpf.reset();

    scLpf.prepare(spec);
    scLpf.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    scLpf.reset();

    inLeftBuffer.setSize    (1, samplesPerBlock, false, true, false);
    inMidBuffer.setSize     (1, samplesPerBlock, false, true, false);
    inRightBuffer.setSize   (1, samplesPerBlock, false, true, false);
    inSideBuffer.setSize    (1, samplesPerBlock, false, true, false);
    sidechainBuffer.setSize (1, samplesPerBlock, false, true, false);
    outMidBuffer.setSize    (1, samplesPerBlock, false, true, false);

    static constexpr double rampSec = 0.02;
    inGainSmoothed.reset          (sampleRate, rampSec);
    outGainSmoothed.reset         (sampleRate, rampSec);
    sideGainSmoothed.reset        (sampleRate, rampSec);
    thresholdSmoothed.reset       (sampleRate, rampSec);
    ratioReciprocalSmoothed.reset (sampleRate, rampSec);
    scHpfSmoothed.reset           (sampleRate, rampSec);
    scLpfSmoothed.reset           (sampleRate, rampSec);

    inGainSmoothed.setCurrentAndTargetValue         (decibels.decibelsToGain(inputGainParam->load()));
    outGainSmoothed.setCurrentAndTargetValue        (decibels.decibelsToGain(outputGainParam->load()));
    sideGainSmoothed.setCurrentAndTargetValue       (decibels.decibelsToGain(GetEffectiveSideInGainDb()));
    thresholdSmoothed.setCurrentAndTargetValue      (decibels.decibelsToGain(GetEffectiveThresholdDb()));
    ratioReciprocalSmoothed.setCurrentAndTargetValue(1.0f / GetEffectiveRatio());
    scHpfSmoothed.setCurrentAndTargetValue          (GetEffectiveScHpfHz());
    scLpfSmoothed.setCurrentAndTargetValue          (GetEffectiveScLpfHz());
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

    const int numSamples = buffer.getNumSamples();
    const int peakMode   = GetEffectivePeakMode();

    inGainSmoothed.setTargetValue          (decibels.decibelsToGain(inputGainParam->load()));
    outGainSmoothed.setTargetValue         (decibels.decibelsToGain(outputGainParam->load()));
    sideGainSmoothed.setTargetValue        (decibels.decibelsToGain(GetEffectiveSideInGainDb()));
    thresholdSmoothed.setTargetValue       (decibels.decibelsToGain(GetEffectiveThresholdDb()));
    ratioReciprocalSmoothed.setTargetValue (1.0f / GetEffectiveRatio());
    scHpfSmoothed.setTargetValue           (GetEffectiveScHpfHz());
    scLpfSmoothed.setTargetValue           (GetEffectiveScLpfHz());

    envelope.setAttackTime (GetEffectiveAttackMs());
    envelope.setReleaseTime(GetEffectiveReleaseMs());
    envelope.setLevelCalculationType(peakMode == 1
                                         ? juce::dsp::BallisticsFilterLevelCalculationType::RMS
                                         : juce::dsp::BallisticsFilterLevelCalculationType::peak);

    constexpr float gainCompensation = 0.5f;    // M/S decode factor

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

    // Track the lowest compGain (= max gain reduction) seen this block
    float minCompGain = 1.0f;

    // --- Sample loop ---
    for (int i = 0; i < numSamples; ++i)
    {
        const float inGainAmp    = inGainSmoothed.getNextValue();
        const float sideGainAmp  = sideGainSmoothed.getNextValue();
        const float outGainAmp   = outGainSmoothed.getNextValue();
        const float thresholdAmp = thresholdSmoothed.getNextValue();
        const float ratioRecip   = ratioReciprocalSmoothed.getNextValue();
        const float outScale     = outGainAmp * gainCompensation;

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

        // SC filters 
        scHpf.setCutoffFrequency(scHpfSmoothed.getNextValue());
        scLpf.setCutoffFrequency(scLpfSmoothed.getNextValue());
        monoSidechainSample = scHpf.processSample(0, monoSidechainSample);
        monoSidechainSample = scLpf.processSample(0, monoSidechainSample);

        sidechainBuffer.addSample(0, i, monoSidechainSample);

        const float envVal = envelope.processSample(0, monoSidechainSample);

        // Compressor gain
        const float compGain = (envVal < thresholdAmp)
                                   ? 1.0f
                                   : std::pow(envVal / thresholdAmp, ratioRecip - 1.0f);

        minCompGain = juce::jmin(minCompGain, compGain);

        const float midComped = mid * compGain;

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
    }

    outMidLevel = outMidLevel.load() * outGainSmoothed.getCurrentValue();

    // GR meter: convert the block's deepest compressor gain to dB and
    // normalise to 0-1 against a fixed full-scale (24 dB). 1.0 == full meter.
    constexpr float meterFullScaleDb = 24.0f;
    const float     grDb             = -juce::Decibels::gainToDecibels(minCompGain);
    gainReduction                    = juce::jlimit(0.0f, 1.0f, grDb / meterFullScaleDb);
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


float CenterSpaceAudioProcessor::GetEffectiveSideInGainDb() const
{
    if ((int)uiModeChoice->load() == uiModeTweak)
        return sidechainInGainParam->load();

    return CenterSpace::Derivations::CompressToSideInGainDb(compressParam->load());
}

float CenterSpaceAudioProcessor::GetEffectiveScHpfHz() const
{
    if ((int)uiModeChoice->load() == uiModeTweak)
        return scHpfHzParam->load();

    const auto preset = (CenterSpace::Derivations::FocusPreset)(int)focusChoice->load();
    return CenterSpace::Derivations::FocusToScCutoffs(preset).hpfHz;
}

float CenterSpaceAudioProcessor::GetEffectiveScLpfHz() const
{
    if ((int)uiModeChoice->load() == uiModeTweak)
        return scLpfHzParam->load();

    const auto preset = (CenterSpace::Derivations::FocusPreset)(int)focusChoice->load();
    return CenterSpace::Derivations::FocusToScCutoffs(preset).lpfHz;
}

int CenterSpaceAudioProcessor::GetEffectivePeakMode() const
{
    using namespace CenterSpace::Derivations;

    if ((int)uiModeChoice->load() == uiModeTweak)
    {
        const auto style    = (Style)(int)styleChoice->load();
        const auto userMode = (PeakMode)(int)peakRMSChoice->load();
        return (int)ApplyStyleOverrideToPeakMode(style, userMode);
    }

    const auto feel = (Feel)(int)feelChoice->load();
    return (int)FeelToPeakMode(feel);
}

float CenterSpaceAudioProcessor::GetEffectiveAttackMs() const
{
    using namespace CenterSpace::Derivations;

    if ((int)uiModeChoice->load() == uiModeTweak)
        return attackParam->load();

    const auto feel = (Feel)(int)feelChoice->load();
    return ReactToMs(reactParam->load(), GetAttackRangeForFeel(feel));
}

float CenterSpaceAudioProcessor::GetEffectiveReleaseMs() const
{
    using namespace CenterSpace::Derivations;

    if ((int)uiModeChoice->load() == uiModeTweak)
        return releaseParam->load();

    const auto feel = (Feel)(int)feelChoice->load();
    return ReactToMs(reactParam->load(), GetReleaseRangeForFeel(feel));
}

int CenterSpaceAudioProcessor::GetEffectiveStyle() const
{
    using namespace CenterSpace::Derivations;

    if ((int)uiModeChoice->load() == uiModeTweak)
        return (int)styleChoice->load();

    const auto feel = (Feel)(int)feelChoice->load();
    return (int)FeelToStyle(feel);
}

float CenterSpaceAudioProcessor::GetEffectiveThresholdDb() const
{
    if ((int)uiModeChoice->load() == uiModeTweak)
        return thresholdParam->load();

    return CenterSpace::Derivations::CompressToThresholdDb(compressParam->load());
}

float CenterSpaceAudioProcessor::GetEffectiveRatio() const
{
    if ((int)uiModeChoice->load() == uiModeTweak)
        return ratioParam->load();

    return CenterSpace::Derivations::CompressToRatio(compressParam->load());
}

float CenterSpaceAudioProcessor::GetEffectiveKneeDb() const
{
    using namespace CenterSpace::Derivations;

    if ((int)uiModeChoice->load() == uiModeTweak)
    {
        const auto style = (Style)(int)styleChoice->load();
        return ApplyStyleOverrideToKneeDb(style, kneeParam->load());
    }

    const auto feel = (Feel)(int)feelChoice->load();
    return FeelToKneeDb(feel);
}

int CenterSpaceAudioProcessor::GetEffectiveLookaheadSamples() const
{
    using namespace CenterSpace::Derivations;

    float ms = 0.0f;

    if ((int)uiModeChoice->load() == uiModeTweak)
    {
        ms = LookaheadChoiceToMs((LookaheadChoice)(int)lookaheadChoice->load());
    }
    else
    {
        if (lookaheadOnOffParam->get())
            ms = FeelToLookaheadOnMs((Feel)(int)feelChoice->load());
    }

    return MsToSamples(ms, currentSampleRate);
}
