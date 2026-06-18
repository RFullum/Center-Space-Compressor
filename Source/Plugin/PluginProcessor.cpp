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
    static constexpr int   uiModeTweak           = 1;
    static constexpr float multiplicativeFloorDb = -200.0f;
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
    , std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"scLpfHz",    1}, "SC LPF Hz",               juce::NormalisableRange<float>(80.0f, 20000.0f, 0.01f, 0.3f, false),   20000.0f, "Hz")
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
                                                                                                                                     , "Vocal"
                                                                                                                                     , "Kick"
                                                                                                                                     , "Bass"
                                                                                                                                     , "Transients"
                                                                                                                                     , "Low"
                                                                                                                                     , "Mid"
                                                                                                                                     , "High"}), 0)
    , std::make_unique<juce::AudioParameterBool>  (juce::ParameterID{"lookaheadOnOff", 1}, "Lookahead On/Off",    false)
  })
//, patchManager(parameters)
, patchManager(parameters
               , PatchConfig{.fileExtension        = ".cspatch"
                            , .rootTagName         = "CSPatch"
                            , .pluginName          = "Center Space"
                            , .companyName         = "FullumMusic"
                            , .isExcludedFromPatch = [](const juce::String &id) { return id == "bypass"; } })
, abCompareManager(parameters)
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

    patchManager.Init();
    abCompareManager.Init();
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

void CenterSpaceAudioProcessor::changeProgramName(int /*index*/, const juce::String &/*newName*/) {}


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

    // Force re-application on next processBlock so the new sample rate's
    // coefficients are written even if param values haven't changed.
    lastAppliedAttackMs  = -1.0f;
    lastAppliedReleaseMs = -1.0f;
    lastAppliedPeakMode  = -1;

    scHpf.prepare(spec);
    scHpf.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    scHpf.reset();

    scLpf.prepare(spec);
    scLpf.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    scLpf.reset();

    juce::dsp::ProcessSpec stereoSpec;
    stereoSpec.sampleRate       = sampleRate;
    stereoSpec.maximumBlockSize = (juce::uint32)samplesPerBlock;
    stereoSpec.numChannels      = 2;

    lookaheadDelay.prepare(stereoSpec);
    lookaheadDelay.setMaximumDelayInSamples(maxLookaheadSamples);
    lookaheadDelay.reset();

    currentLookaheadSamples = juce::jlimit(0, maxLookaheadSamples, GetEffectiveLookaheadSamples());
    lookaheadDelay.setDelay((float)currentLookaheadSamples);
    setLatencySamples(currentLookaheadSamples);

    static constexpr double rampSec = 0.02;
    inGainSmoothed.reset          (sampleRate, rampSec);
    outGainSmoothed.reset         (sampleRate, rampSec);
    sideGainSmoothed.reset        (sampleRate, rampSec);
    thresholdSmoothed.reset       (sampleRate, rampSec);
    ratioReciprocalSmoothed.reset (sampleRate, rampSec);
    kneeSmoothed.reset            (sampleRate, rampSec);
    scHpfSmoothed.reset           (sampleRate, rampSec);
    scLpfSmoothed.reset           (sampleRate, rampSec);

    inGainSmoothed.setCurrentAndTargetValue         (juce::Decibels::decibelsToGain(inputGainParam->load(),       multiplicativeFloorDb));
    outGainSmoothed.setCurrentAndTargetValue        (juce::Decibels::decibelsToGain(outputGainParam->load(),      multiplicativeFloorDb));
    sideGainSmoothed.setCurrentAndTargetValue       (juce::Decibels::decibelsToGain(GetEffectiveSideInGainDb(),   multiplicativeFloorDb));
    thresholdSmoothed.setCurrentAndTargetValue      (GetEffectiveThresholdDb());
    ratioReciprocalSmoothed.setCurrentAndTargetValue(1.0f / GetEffectiveRatio());
    kneeSmoothed.setCurrentAndTargetValue           (GetEffectiveKneeDb());
    scHpfSmoothed.setCurrentAndTargetValue          (GetEffectiveScHpfHz());
    scLpfSmoothed.setCurrentAndTargetValue          (GetEffectiveScLpfHz());
}

void CenterSpaceAudioProcessor::releaseResources()
{
    envelope.reset();
    scHpf.reset();
    scLpf.reset();
    lookaheadDelay.reset();

    // Force coefficient re-application on next prepareToPlay/processBlock.
    lastAppliedPeakMode  = -1;
    lastAppliedAttackMs  = -1.0f;
    lastAppliedReleaseMs = -1.0f;
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

    const int numSamples = buffer.getNumSamples();
    const int peakMode   = GetEffectivePeakMode();

    inGainSmoothed         .setTargetValue(juce::Decibels::decibelsToGain(inputGainParam->load(),     multiplicativeFloorDb));
    outGainSmoothed        .setTargetValue(juce::Decibels::decibelsToGain(outputGainParam->load(),    multiplicativeFloorDb));
    sideGainSmoothed       .setTargetValue(juce::Decibels::decibelsToGain(GetEffectiveSideInGainDb(), multiplicativeFloorDb));
    thresholdSmoothed      .setTargetValue(GetEffectiveThresholdDb());
    ratioReciprocalSmoothed.setTargetValue(1.0f / GetEffectiveRatio());
    kneeSmoothed           .setTargetValue(GetEffectiveKneeDb());
    scHpfSmoothed          .setTargetValue(GetEffectiveScHpfHz());
    scLpfSmoothed          .setTargetValue(GetEffectiveScLpfHz());

    const int newLookaheadSamples = juce::jlimit(0, maxLookaheadSamples, GetEffectiveLookaheadSamples());
    if (newLookaheadSamples != currentLookaheadSamples)
    {
        lookaheadDelay.setDelay((float)newLookaheadSamples);
        setLatencySamples(newLookaheadSamples);
        currentLookaheadSamples = newLookaheadSamples;
    }

    // Only re-apply envelope settings when they actually change.
    const float effectiveAttackMs  = GetEffectiveAttackMs();
    const float effectiveReleaseMs = GetEffectiveReleaseMs();

    if (!juce::approximatelyEqual(effectiveAttackMs, lastAppliedAttackMs))
    {
        envelope.setAttackTime(effectiveAttackMs);
        lastAppliedAttackMs = effectiveAttackMs;
    }
    
    if (!juce::approximatelyEqual(effectiveReleaseMs, lastAppliedReleaseMs))
    {
        envelope.setReleaseTime(effectiveReleaseMs);
        lastAppliedReleaseMs = effectiveReleaseMs;
    }
    if (peakMode != lastAppliedPeakMode)
    {
        envelope.setLevelCalculationType(peakMode == 1
                                             ? juce::dsp::BallisticsFilterLevelCalculationType::RMS
                                             : juce::dsp::BallisticsFilterLevelCalculationType::peak);
        lastAppliedPeakMode = peakMode;
    }

    constexpr float gainCompensation = 0.5f;    // M/S decode factor

    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    auto mainInputOutput = getBusBuffer(buffer, true, 0);
    auto sideChainInput  = getBusBuffer(buffer, true, 1);

    float *chan0 = mainInputOutput.getWritePointer(0);
    float *chan1 = (mainInputOutput.getNumChannels() > 1)
                       ? mainInputOutput.getWritePointer(1)
                       : chan0;

    const bool inputIsMS  = ((int)inputTypeChoice->load()  == 1);
    const bool outputIsMS = ((int)outputTypeChoice->load() == 1);

    if (bypassParam->get())
    {
        // Keep the lookahead delay primed bypass toggles don't cause artefact.
        for (int i = 0; i < numSamples; ++i)
        {
            const float c0 = chan0[i];
            const float c1 = chan1[i];

            lookaheadDelay.pushSample(0, c0);
            lookaheadDelay.pushSample(1, c1);

            chan0[i] = lookaheadDelay.popSample(0);
            chan1[i] = lookaheadDelay.popSample(1);
        }

        inLevelChan0   = 0.0f;
        inMidLevel     = 0.0f;
        inLevelChan1   = 0.0f;
        sideChainLevel = 0.0f;
        outLevelChan0  = 0.0f;
        outMidLevel    = 0.0f;
        outLevelChan1  = 0.0f;
        gainReduction  = 0.0f;

        return;
    }

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
    
    // Metering Accumulators
    // maxAbs (peak); sumSq (RMS)
    float  maxAbsInChan0 = 0.0f, maxAbsInMid = 0.0f,  maxAbsInChan1 = 0.0f,  maxAbsInSide = 0.0f;
    float  maxAbsSC      = 0.0f, maxAbsOutMid = 0.0f, maxAbsOutChan0 = 0.0f, maxAbsOutChan1 = 0.0f;
    double sumSqInChan0  = 0.0,  sumSqInMid = 0.0,    sumSqInChan1 = 0.0,    sumSqInSide = 0.0;
    double sumSqSC       = 0.0,  sumSqOutMid = 0.0,   sumSqOutChan0 = 0.0,   sumSqOutChan1 = 0.0;

    // Compute SC filters at sub-block sizes; not per sample.
    constexpr int subBlockSize = 32;
    int           subBlockLeft = 0;

    // --- Sample loop ---
    for (int i = 0; i < numSamples; ++i)
    {
        if (subBlockLeft == 0)
        {
            scHpfSmoothed.skip(juce::jmin(subBlockSize, numSamples - i) - 1);
            scLpfSmoothed.skip(juce::jmin(subBlockSize, numSamples - i) - 1);
            scHpf.setCutoffFrequency(scHpfSmoothed.getNextValue());
            scLpf.setCutoffFrequency(scLpfSmoothed.getNextValue());
            subBlockLeft = juce::jmin(subBlockSize, numSamples - i);
        }
        --subBlockLeft;

        const float inGainAmp    = inGainSmoothed.getNextValue();
        const float sideGainAmp  = sideGainSmoothed.getNextValue();
        const float outGainAmp   = outGainSmoothed.getNextValue();
        const float threshDb     = thresholdSmoothed.getNextValue();
        const float ratioRecip   = ratioReciprocalSmoothed.getNextValue();
        const float kneeDb       = kneeSmoothed.getNextValue();
        const float outScale     = outGainAmp * gainCompensation;

        // Lookahead delay on the raw input channels so mid/side stay aligned
        // when M/S encoding happens after the delay.
        lookaheadDelay.pushSample(0, chan0[i]);
        lookaheadDelay.pushSample(1, chan1[i]);
        const float chan0In = lookaheadDelay.popSample(0);
        const float chan1In = lookaheadDelay.popSample(1);

        // Encode to M/S if the input is L/R; otherwise the channels are
        // already mid (ch0) and side (ch1).
        float mid;
        float side;
        if (inputIsMS)
        {
            mid  = chan0In * inGainAmp;
            side = chan1In * inGainAmp;
        }
        else
        {
            mid  = (chan0In + chan1In) * inGainAmp;
            side = (chan0In - chan1In) * inGainAmp;
        }

        // Input metering taps
        {
            const float inC0 = chan0In * inGainAmp;
            const float inC1 = chan1In * inGainAmp;
            maxAbsInChan0 = juce::jmax(maxAbsInChan0, std::abs(inC0));
            maxAbsInChan1 = juce::jmax(maxAbsInChan1, std::abs(inC1));
            maxAbsInMid   = juce::jmax(maxAbsInMid,   std::abs(mid));
            maxAbsInSide  = juce::jmax(maxAbsInSide,  std::abs(side));
            sumSqInChan0 += (double)inC0 * (double)inC0;
            sumSqInChan1 += (double)inC1 * (double)inC1;
            sumSqInMid   += (double)mid  * (double)mid;
            sumSqInSide  += (double)side * (double)side;
        }

        // Mono the sidechain
        float monoSidechainSample = 0.0f;

        for (int j = 0; j < clampedSCChannels; ++j)
            monoSidechainSample += scReadPointers[j][i];

        monoSidechainSample *= scChannelScale * sideGainAmp;

        // SC filters
        monoSidechainSample = scHpf.processSample(0, monoSidechainSample);
        monoSidechainSample = scLpf.processSample(0, monoSidechainSample);

        maxAbsSC  = juce::jmax(maxAbsSC, std::abs(monoSidechainSample));
        sumSqSC  += (double)monoSidechainSample * (double)monoSidechainSample;

        const float envVal = envelope.processSample(0, monoSidechainSample);

        // Compressor gain (dB, soft-knee, static curve)
        const float safeEnvVal = juce::jmax(envVal, 1e-9f);
        const float envDb      = 20.0f * std::log10(safeEnvVal);
        const float overshoot  = envDb - threshDb;
        const float slope      = ratioRecip - 1.0f;

        float gainDb;
        if (kneeDb <= 0.0f)
        {
            gainDb = (overshoot <= 0.0f) ? 0.0f : slope * overshoot;
        }
        else if (overshoot <= -kneeDb * 0.5f)
        {
            gainDb = 0.0f;
        }
        else if (overshoot >= kneeDb * 0.5f)
        {
            gainDb = slope * overshoot;
        }
        else
        {
            const float x = overshoot + kneeDb * 0.5f;
            gainDb = slope * x * x / (2.0f * kneeDb);
        }
        const float compGain = juce::Decibels::decibelsToGain(gainDb);

        minCompGain = juce::jmin(minCompGain, compGain);

        const float midComped = mid * compGain;

        // Output mid meter tap: post-gain, post-0.5 compensation
        const float outMidSample = midComped * outGainAmp * gainCompensation;
        maxAbsOutMid = juce::jmax(maxAbsOutMid, std::abs(outMidSample));
        sumSqOutMid += (double)outMidSample * (double)outMidSample;

        // Decode M/S back to L/R if the output is L/R; otherwise pass mid and
        // side through as ch0 and ch1 with no decode and no 0.5 compensation.
        if (outputIsMS)
        {
            chan0[i] = midComped * outGainAmp;
            chan1[i] = side      * outGainAmp;
        }
        else
        {
            chan0[i] = (midComped + side) * outScale;
            chan1[i] = (midComped - side) * outScale;
        }

        maxAbsOutChan0 = juce::jmax(maxAbsOutChan0, std::abs(chan0[i]));
        maxAbsOutChan1 = juce::jmax(maxAbsOutChan1, std::abs(chan1[i]));
        sumSqOutChan0 += (double)chan0[i] * (double)chan0[i];
        sumSqOutChan1 += (double)chan1[i] * (double)chan1[i];
    }

    // Publish meter values
    if (peakMode == 1)
    {
        const float invN = (numSamples > 0) ? 1.0f / (float)numSamples : 0.0f;
        inLevelChan0   = std::sqrt((float)(sumSqInChan0 * invN));
        inMidLevel     = std::sqrt((float)(sumSqInMid   * invN)) * 0.5f;
        inLevelChan1   = std::sqrt((float)(sumSqInChan1 * invN));
        sideChainLevel = std::sqrt((float)(sumSqSC      * invN));
        outLevelChan0  = std::sqrt((float)(sumSqOutChan0 * invN));
        outMidLevel    = std::sqrt((float)(sumSqOutMid   * invN));
        outLevelChan1  = std::sqrt((float)(sumSqOutChan1 * invN));
    }
    else
    {
        inLevelChan0   = maxAbsInChan0;
        inMidLevel     = maxAbsInMid * 0.5f;
        inLevelChan1   = maxAbsInChan1;
        sideChainLevel = maxAbsSC;
        outLevelChan0  = maxAbsOutChan0;
        outMidLevel    = maxAbsOutMid;
        outLevelChan1  = maxAbsOutChan1;
    }

    // GR meter: publish the block's deepest compressor reduction in dB
    gainReduction = juce::jmax(0.0f, -juce::Decibels::gainToDecibels(minCompGain));
}

juce::AudioProcessorParameter *CenterSpaceAudioProcessor::getBypassParameter() const
{
    return parameters.getParameter("bypass");
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
    state.setProperty("currentPatchPath"
                      , patchManager.GetCurrentPatchFile().getFullPathName()
                      , nullptr);
    state.removeChild(state.getChildWithName(juce::Identifier(ABCompareManager::stateTagName))
                      , nullptr);

    if (auto abState = abCompareManager.SerializeState(); abState.isValid())
        state.appendChild(abState, nullptr);

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CenterSpaceAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            const auto restored = juce::ValueTree::fromXml(*xmlState);
            parameters.replaceState(restored);

            const auto restoredPath = restored.getProperty("currentPatchPath").toString();
            if (restoredPath.isNotEmpty())
                patchManager.SetCurrentFromRestoredPath(juce::File(restoredPath));

            if (auto abState = restored.getChildWithName(juce::Identifier(ABCompareManager::stateTagName));
                abState.isValid())
            {
                abCompareManager.RestoreState(abState);
            }
        }
    }
}

// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new CenterSpaceAudioProcessor();
}


juce::PropertiesFile *CenterSpaceAudioProcessor::GetUserSettings() const
{
    if (userSettings == nullptr)
    {
        juce::PropertiesFile::Options opts;
        opts.applicationName     = "Center Space";
        opts.folderName          = "FullumMusic/Center Space";
        opts.filenameSuffix      = "settings";
        opts.osxLibrarySubFolder = "Application Support";
        userSettings = std::make_unique<juce::PropertiesFile>(opts);
    }
    return userSettings.get();
}

bool CenterSpaceAudioProcessor::GetTooltipsEnabled() const
{
    if (auto *settings = GetUserSettings())
        return settings->getBoolValue("tooltipsEnabled", true);
    return true;
}

void CenterSpaceAudioProcessor::SetTooltipsEnabled(bool enabled)
{
    if (auto *settings = GetUserSettings())
    {
        settings->setValue("tooltipsEnabled", enabled);
        settings->saveIfNeeded();
    }
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
