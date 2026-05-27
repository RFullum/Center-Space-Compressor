/*
  ==============================================================================

    TestMain.cpp

    Plain console-app DSP test harness. No DAW, no GUI, no real-time audio I/O.
    Constructs DSP objects, feeds them sample data synchronously, and asserts
    numerical behavior. Run via `cmake --build --preset=macos-debug --target
    centerspace_tests && build/macos/...`, or via `ctest --preset=macos`.

    Add tests by writing a free function and calling it from main(). Use the
    EXPECT_NEAR macro for floating-point comparisons.

  ==============================================================================
*/

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>

#include "Derivations.h"

#include <cmath>
#include <iostream>


//==============================================================================

namespace
{
    int failureCount = 0;

    #define EXPECT_NEAR(actual, expected, tol)                                          \
        do                                                                              \
        {                                                                               \
            const float testActual   = (float)(actual);                                 \
            const float testExpected = (float)(expected);                               \
            if (std::abs(testActual - testExpected) > (tol))                            \
            {                                                                           \
                std::cerr << "FAIL: " << #actual << " (" << testActual << ") != "       \
                          << #expected << " (" << testExpected << "), tol=" << (tol)    \
                          << "  [" << __FILE__ << ":" << __LINE__ << "]\n";             \
                ++failureCount;                                                         \
            }                                                                           \
        } while (0)


    //==============================================================================
    // Compressor static-curve formula (mirrors the inline calc in
    // PluginProcessor::processBlock). When Phase 3 step 12 extracts the
    // compressor into a class, this test should call that class directly
    // instead of re-implementing the formula.
    float CompressorGain(float input, float thresholdAmp, float userRatio)
    {
        if (input < thresholdAmp)
            return 1.0f;

        const float ratioRecip = 1.0f / userRatio;
        return std::pow(input / thresholdAmp, ratioRecip - 1.0f);
    }

    //==============================================================================
    // dB-domain soft-knee static curve (mirrors the per-sample formula in
    // PluginProcessor::processBlock).
    float SoftKneeCompressorGain(float envAmp, float thresholdDb, float userRatio, float kneeDb)
    {
        const float safeEnvVal = juce::jmax(envAmp, 1e-9f);
        const float envDb      = 20.0f * std::log10(safeEnvVal);
        const float overshoot  = envDb - thresholdDb;
        const float slope      = (1.0f / userRatio) - 1.0f;

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
        return juce::Decibels::decibelsToGain(gainDb);
    }


    void TestSoftKneeStaticCurve()
    {
        std::cout << "[test] soft-knee dB-domain static curve\n";

        const float thresholdDb = -6.0206f;   // 0.5 linear
        const float ratio       = 4.0f;
        const float thresholdAmp = 0.5f;

        // Knee = 0 should match the old hard-knee formula across input levels.
        for (float in : { 0.1f, 0.25f, 0.5f, 0.707f, 1.0f, 1.5f, 2.0f })
        {
            const float hardKnee = CompressorGain(in, thresholdAmp, ratio);
            const float softKnee = SoftKneeCompressorGain(in, thresholdDb, ratio, 0.0f);
            EXPECT_NEAR(softKnee, hardKnee, 1e-4f);
        }

        // Knee = 6 dB, input exactly at threshold (overshoot = 0):
        //   x = kneeDb/2, gainDb = slope * (kneeDb/2)^2 / (2*kneeDb) = slope * kneeDb / 8
        {
            const float kneeDb   = 6.0f;
            const float slope    = (1.0f / ratio) - 1.0f;
            const float expected = juce::Decibels::decibelsToGain(slope * kneeDb / 8.0f);
            const float actual   = SoftKneeCompressorGain(thresholdAmp, thresholdDb, ratio, kneeDb);
            EXPECT_NEAR(actual, expected, 1e-5f);
        }

        // Knee = 24 dB: monotonic decreasing across overshoot sweep, and
        // continuous at the ±kneeDb/2 boundaries.
        {
            const float kneeDb = 24.0f;
            float prevGain = 1.0f + 1e-3f;   // small slack to allow first sample == 1.0
            for (float overshootDb = -20.0f; overshootDb <= 20.0f; overshootDb += 1.0f)
            {
                const float envAmp = juce::Decibels::decibelsToGain(thresholdDb + overshootDb);
                const float g = SoftKneeCompressorGain(envAmp, thresholdDb, ratio, kneeDb);
                if (g > prevGain + 1e-5f)
                {
                    std::cerr << "FAIL: soft-knee not monotonic at overshoot=" << overshootDb
                              << " (g=" << g << ", prev=" << prevGain << ")\n";
                    ++failureCount;
                }
                prevGain = g;
            }

            // Continuity at the lower knee boundary (overshoot = -kneeDb/2).
            const float belowAmp = juce::Decibels::decibelsToGain(thresholdDb - kneeDb * 0.5f - 1e-4f);
            const float atAmp    = juce::Decibels::decibelsToGain(thresholdDb - kneeDb * 0.5f + 1e-4f);
            EXPECT_NEAR(SoftKneeCompressorGain(belowAmp, thresholdDb, ratio, kneeDb),
                        SoftKneeCompressorGain(atAmp,    thresholdDb, ratio, kneeDb), 1e-3f);

            // Continuity at the upper knee boundary (overshoot = +kneeDb/2).
            const float justInsideAmp = juce::Decibels::decibelsToGain(thresholdDb + kneeDb * 0.5f - 1e-4f);
            const float justOutsideAmp = juce::Decibels::decibelsToGain(thresholdDb + kneeDb * 0.5f + 1e-4f);
            EXPECT_NEAR(SoftKneeCompressorGain(justInsideAmp,  thresholdDb, ratio, kneeDb),
                        SoftKneeCompressorGain(justOutsideAmp, thresholdDb, ratio, kneeDb), 1e-3f);
        }

        // envVal = 0 must not produce NaN/Inf thanks to the 1e-9f floor.
        {
            const float g = SoftKneeCompressorGain(0.0f, thresholdDb, ratio, 12.0f);
            if (! std::isfinite(g))
            {
                std::cerr << "FAIL: SoftKneeCompressorGain(0) produced non-finite value " << g << "\n";
                ++failureCount;
            }
        }
    }


    //==============================================================================
    void TestCompressorStaticCurve()
    {
        std::cout << "[test] compressor static curve\n";

        const float thresholdAmp = 0.5f;   // ~-6 dBFS
        const float ratio        = 4.0f;

        // Input at threshold => exactly 1.0 gain (no compression).
        EXPECT_NEAR(CompressorGain(thresholdAmp, thresholdAmp, ratio), 1.0f, 1e-5f);

        // Input below threshold => unity gain.
        EXPECT_NEAR(CompressorGain(0.1f, thresholdAmp, ratio), 1.0f, 1e-5f);

        // 0 dBFS in (1.0), threshold 0.5 linear (~-6.0206 dB), 4:1 ratio.
        //   input above threshold = 6.0206 dB
        //   output above threshold = 6.0206 / 4 = 1.5052 dB
        //   GR = 6.0206 - 1.5052 = 4.5155 dB
        // (Using exact linear values rather than the -6 dB shorthand keeps
        //  the expectation precise.)
        const float gain     = CompressorGain(1.0f, thresholdAmp, ratio);
        const float expected = std::pow(2.0f, -0.75f);   // 2^(1/ratio - 1) for input/thr = 2
        EXPECT_NEAR(gain, expected, 1e-5f);

        // 1:1 ratio => no compression even when above threshold.
        EXPECT_NEAR(CompressorGain(1.0f, thresholdAmp, 1.0f), 1.0f, 1e-5f);
    }


    //==============================================================================
    // Mid/Side encode-decode should round-trip exactly with unity gain and the
    // 0.5 gain compensation factor used in processBlock.
    void TestMidSideRoundTrip()
    {
        std::cout << "[test] M/S round-trip\n";

        const float L = 0.7f;
        const float R = -0.3f;

        const float mid  = L + R;
        const float side = L - R;

        const float Lout = (mid + side) * 0.5f;
        const float Rout = (mid - side) * 0.5f;

        EXPECT_NEAR(Lout, L, 1e-6f);
        EXPECT_NEAR(Rout, R, 1e-6f);
    }


    //==============================================================================
    // juce::dsp::BallisticsFilter attack-time sanity check.
    //
    // JUCE's smoothing coefficient is exp(-2*pi*1000 / (sr * attackTimeMs)),
    // so after attackTime ms of step input the envelope reaches
    // 1 - exp(-2*pi) ≈ 99.81% of the target (not 63%).
    //
    // We sanity-check two things:
    //   1. A short attack (10 ms) reaches ~1.0 after 10 ms.
    //   2. A long attack (100 ms) is still well short of 1.0 after 10 ms.
    void TestBallisticsAttackTime()
    {
        std::cout << "[test] BallisticsFilter attack time\n";

        juce::dsp::ProcessSpec spec;
        spec.sampleRate       = 48000.0;
        spec.maximumBlockSize = 512;
        spec.numChannels      = 1;

        const int samples10ms = 480;

        // Fast attack (10 ms): after 10 ms, should be ~1.0.
        {
            juce::dsp::BallisticsFilter<float> filter;
            filter.prepare(spec);
            filter.setAttackTime (10.0f);
            filter.setReleaseTime(100.0f);
            filter.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::peak);

            float lastVal = 0.0f;
            for (int n = 0; n < samples10ms; ++n)
                lastVal = filter.processSample(0, 1.0f);

            EXPECT_NEAR(lastVal, 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi), 0.01f);
        }

        // Slow attack (100 ms): after only 10 ms, envelope should still be
        // well under 0.5 — well short of fully attacking.
        {
            juce::dsp::BallisticsFilter<float> filter;
            filter.prepare(spec);
            filter.setAttackTime (100.0f);
            filter.setReleaseTime(500.0f);
            filter.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::peak);

            float lastVal = 0.0f;
            for (int n = 0; n < samples10ms; ++n)
                lastVal = filter.processSample(0, 1.0f);

            if (! (lastVal < 0.5f))
            {
                std::cerr << "FAIL: slow attack reached " << lastVal
                          << " after 10 ms (expected < 0.5)\n";
                ++failureCount;
            }
        }
    }


    //==============================================================================
    // Measures the steady-state RMS of a StateVariableTPTFilter driven by a sine
    // tone, in dB relative to the unfiltered sine RMS (0.7071...). Discards the
    // first half of the buffer so the filter has time to settle before we measure.
    float MeasureFilterRmsDb(juce::dsp::StateVariableTPTFilter<float> &filter
                             , double sampleRate
                             , float toneHz
                             , int totalSamples)
    {
        const float twoPiOverSr = juce::MathConstants<float>::twoPi / (float)sampleRate;
        const int   measureStart = totalSamples / 2;

        double sumSq        = 0.0;
        int    measureCount = 0;

        for (int n = 0; n < totalSamples; ++n)
        {
            const float input  = std::sin(twoPiOverSr * toneHz * (float)n);
            const float output = filter.processSample(0, input);

            if (n >= measureStart)
            {
                sumSq += (double)output * (double)output;
                ++measureCount;
            }
        }

        const float rms = (float)std::sqrt(sumSq / (double)measureCount);
        return juce::Decibels::gainToDecibels(rms / 0.70710678f);
    }


    void TestScFilterResponse()
    {
        std::cout << "[test] SC HPF/LPF response\n";

        constexpr double sampleRate   = 48000.0;
        constexpr int    totalSamples = 24000;   // 0.5 s

        juce::dsp::ProcessSpec spec;
        spec.sampleRate       = sampleRate;
        spec.maximumBlockSize = 512;
        spec.numChannels      = 1;

        // HPF at 200 Hz, sine at 50 Hz (2 oct below): expect ~24 dB attenuation.
        {
            juce::dsp::StateVariableTPTFilter<float> hpf;
            hpf.prepare(spec);
            hpf.setType(juce::dsp::StateVariableTPTFilterType::highpass);
            hpf.setCutoffFrequency(200.0f);
            hpf.reset();

            const float attenDb = MeasureFilterRmsDb(hpf, sampleRate, 50.0f, totalSamples);
            EXPECT_NEAR(attenDb, -24.0f, 3.0f);
        }

        // HPF at 200 Hz, sine at 2 kHz (well above cutoff): near pass-band (0 dB).
        {
            juce::dsp::StateVariableTPTFilter<float> hpf;
            hpf.prepare(spec);
            hpf.setType(juce::dsp::StateVariableTPTFilterType::highpass);
            hpf.setCutoffFrequency(200.0f);
            hpf.reset();

            const float passDb = MeasureFilterRmsDb(hpf, sampleRate, 2000.0f, totalSamples);
            EXPECT_NEAR(passDb, 0.0f, 1.0f);
        }

        // LPF at 1 kHz, sine at 4 kHz (2 oct above): expect ~24 dB attenuation.
        {
            juce::dsp::StateVariableTPTFilter<float> lpf;
            lpf.prepare(spec);
            lpf.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
            lpf.setCutoffFrequency(1000.0f);
            lpf.reset();

            const float attenDb = MeasureFilterRmsDb(lpf, sampleRate, 4000.0f, totalSamples);
            EXPECT_NEAR(attenDb, -24.0f, 3.0f);
        }

        // LPF at 1 kHz, sine at 100 Hz (well below cutoff): near pass-band (0 dB).
        {
            juce::dsp::StateVariableTPTFilter<float> lpf;
            lpf.prepare(spec);
            lpf.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
            lpf.setCutoffFrequency(1000.0f);
            lpf.reset();

            const float passDb = MeasureFilterRmsDb(lpf, sampleRate, 100.0f, totalSamples);
            EXPECT_NEAR(passDb, 0.0f, 1.0f);
        }
    }
}


//==============================================================================
// Vibe/Tweak effective-value derivation layer. See Source/DSP/Derivations.h
// and CLAUDE.md "Vibe-mode macro derivations".

namespace
{
    void TestCompressMacro()
    {
        using namespace CenterSpace::Derivations;
        std::cout << "[test] compress macro\n";

        EXPECT_NEAR(CompressToSideInGainDb(0.0f),   0.0f, 1e-5f);
        EXPECT_NEAR(CompressToSideInGainDb(0.5f),   3.0f, 1e-5f);
        EXPECT_NEAR(CompressToSideInGainDb(1.0f),   9.0f, 1e-5f);
        EXPECT_NEAR(CompressToSideInGainDb(0.25f),  1.5f, 1e-5f);   // midpoint of 0→0.5 segment
        EXPECT_NEAR(CompressToSideInGainDb(0.75f),  6.0f, 1e-5f);   // midpoint of 0.5→1 segment

        EXPECT_NEAR(CompressToThresholdDb(0.0f),    0.0f,   1e-5f);
        EXPECT_NEAR(CompressToThresholdDb(0.5f),  -12.0f,   1e-5f);
        EXPECT_NEAR(CompressToThresholdDb(1.0f),  -30.0f,   1e-5f);
        EXPECT_NEAR(CompressToThresholdDb(0.25f),  -6.0f,   1e-5f);
        EXPECT_NEAR(CompressToThresholdDb(0.75f), -21.0f,   1e-5f);

        EXPECT_NEAR(CompressToRatio(0.0f),  1.0f,  1e-5f);
        EXPECT_NEAR(CompressToRatio(0.5f),  3.0f,  1e-5f);
        EXPECT_NEAR(CompressToRatio(1.0f), 10.0f,  1e-5f);
        EXPECT_NEAR(CompressToRatio(0.25f), 2.0f,  1e-5f);
        EXPECT_NEAR(CompressToRatio(0.75f), 6.5f,  1e-5f);
    }


    void TestReactMacro()
    {
        using namespace CenterSpace::Derivations;
        std::cout << "[test] react macro\n";

        const auto cleanAtk  = GetAttackRangeForFeel (Feel::Clean);
        const auto cleanRel  = GetReleaseRangeForFeel(Feel::Clean);
        const auto smoothAtk = GetAttackRangeForFeel (Feel::Smooth);
        const auto smoothRel = GetReleaseRangeForFeel(Feel::Smooth);

        EXPECT_NEAR(cleanAtk.minMs,   0.5f,  1e-5f);
        EXPECT_NEAR(cleanAtk.maxMs,  30.0f,  1e-5f);
        EXPECT_NEAR(cleanRel.minMs,  30.0f,  1e-5f);
        EXPECT_NEAR(cleanRel.maxMs, 500.0f,  1e-5f);

        EXPECT_NEAR(smoothAtk.minMs,    5.0f,   1e-5f);
        EXPECT_NEAR(smoothAtk.maxMs,  100.0f,   1e-5f);
        EXPECT_NEAR(smoothRel.minMs,  100.0f,   1e-5f);
        EXPECT_NEAR(smoothRel.maxMs, 1000.0f,   1e-5f);

        // react = 0 → min of range, react = 1 → max of range.
        EXPECT_NEAR(ReactToMs(0.0f, smoothAtk),    5.0f, 1e-5f);
        EXPECT_NEAR(ReactToMs(1.0f, smoothAtk),  100.0f, 1e-5f);
        EXPECT_NEAR(ReactToMs(0.0f, smoothRel),  100.0f, 1e-5f);
        EXPECT_NEAR(ReactToMs(1.0f, smoothRel), 1000.0f, 1e-5f);
        EXPECT_NEAR(ReactToMs(0.5f, cleanAtk),   15.25f, 1e-4f);
    }


    void TestFeelBakes()
    {
        using namespace CenterSpace::Derivations;
        std::cout << "[test] feel bakes\n";

        // Clean
        if (FeelToStyle   (Feel::Clean) != Style::ModernVCA) { std::cerr << "FAIL: Clean style\n";       ++failureCount; }
        if (FeelToPeakMode(Feel::Clean) != PeakMode::Peak)   { std::cerr << "FAIL: Clean peakMode\n";    ++failureCount; }
        EXPECT_NEAR(FeelToKneeDb         (Feel::Clean), 0.0f, 1e-5f);
        EXPECT_NEAR(FeelToLookaheadOnMs  (Feel::Clean), 1.0f, 1e-5f);

        // Smooth
        if (FeelToStyle   (Feel::Smooth) != Style::Opto)    { std::cerr << "FAIL: Smooth style\n";       ++failureCount; }
        if (FeelToPeakMode(Feel::Smooth) != PeakMode::RMS)  { std::cerr << "FAIL: Smooth peakMode\n";    ++failureCount; }
        EXPECT_NEAR(FeelToKneeDb         (Feel::Smooth), 12.0f, 1e-5f);
        EXPECT_NEAR(FeelToLookaheadOnMs  (Feel::Smooth),  4.0f, 1e-5f);
    }


    void TestFocusPresets()
    {
        using namespace CenterSpace::Derivations;
        std::cout << "[test] focus presets\n";

        const auto fullRange    = FocusToScCutoffs(FocusPreset::FullRange);
        const auto kickThump    = FocusToScCutoffs(FocusPreset::KickThump);
        const auto vocalClarity = FocusToScCutoffs(FocusPreset::VocalClarity);
        const auto hatsRange    = FocusToScCutoffs(FocusPreset::HatsRange);

        EXPECT_NEAR(fullRange.hpfHz,       20.0f, 1e-5f);
        EXPECT_NEAR(fullRange.lpfHz,    20000.0f, 1e-5f);
        EXPECT_NEAR(kickThump.hpfHz,       40.0f, 1e-5f);
        EXPECT_NEAR(kickThump.lpfHz,       90.0f, 1e-5f);
        EXPECT_NEAR(vocalClarity.hpfHz,  1000.0f, 1e-5f);
        EXPECT_NEAR(vocalClarity.lpfHz,  3000.0f, 1e-5f);
        EXPECT_NEAR(hatsRange.hpfHz,      350.0f, 1e-5f);
        EXPECT_NEAR(hatsRange.lpfHz,    20000.0f, 1e-5f);
    }


    void TestStyleOverride()
    {
        using namespace CenterSpace::Derivations;
        std::cout << "[test] style override (Opto forces RMS + 12 dB knee)\n";

        // Modern VCA: pass user values through.
        if (ApplyStyleOverrideToPeakMode(Style::ModernVCA, PeakMode::Peak) != PeakMode::Peak)
        { std::cerr << "FAIL: VCA passthrough Peak\n"; ++failureCount; }
        if (ApplyStyleOverrideToPeakMode(Style::ModernVCA, PeakMode::RMS) != PeakMode::RMS)
        { std::cerr << "FAIL: VCA passthrough RMS\n";  ++failureCount; }
        EXPECT_NEAR(ApplyStyleOverrideToKneeDb(Style::ModernVCA,  0.0f),  0.0f, 1e-5f);
        EXPECT_NEAR(ApplyStyleOverrideToKneeDb(Style::ModernVCA, 24.0f), 24.0f, 1e-5f);

        // Opto: force RMS + 12 dB knee regardless of user value.
        if (ApplyStyleOverrideToPeakMode(Style::Opto, PeakMode::Peak) != PeakMode::RMS)
        { std::cerr << "FAIL: Opto did not force RMS from Peak\n"; ++failureCount; }
        if (ApplyStyleOverrideToPeakMode(Style::Opto, PeakMode::RMS) != PeakMode::RMS)
        { std::cerr << "FAIL: Opto RMS\n"; ++failureCount; }
        EXPECT_NEAR(ApplyStyleOverrideToKneeDb(Style::Opto,  0.0f), 12.0f, 1e-5f);
        EXPECT_NEAR(ApplyStyleOverrideToKneeDb(Style::Opto, 24.0f), 12.0f, 1e-5f);
    }


    void TestLookaheadChoice()
    {
        using namespace CenterSpace::Derivations;
        std::cout << "[test] lookahead choice to ms / samples\n";

        EXPECT_NEAR(LookaheadChoiceToMs(LookaheadChoice::Ms0),   0.0f, 1e-5f);
        EXPECT_NEAR(LookaheadChoiceToMs(LookaheadChoice::Ms1),   1.0f, 1e-5f);
        EXPECT_NEAR(LookaheadChoiceToMs(LookaheadChoice::Ms4),   4.0f, 1e-5f);
        EXPECT_NEAR(LookaheadChoiceToMs(LookaheadChoice::Ms10), 10.0f, 1e-5f);

        // 4 ms @ 48 kHz = 192 samples; 10 ms @ 44.1 kHz = 441; 0 ms = 0.
        if (MsToSamples(4.0f,  48000.0) != 192) { std::cerr << "FAIL: 4 ms @ 48k\n";  ++failureCount; }
        if (MsToSamples(10.0f, 44100.0) != 441) { std::cerr << "FAIL: 10 ms @ 44.1k\n"; ++failureCount; }
        if (MsToSamples(0.0f,  48000.0) != 0)   { std::cerr << "FAIL: 0 ms\n";        ++failureCount; }

        // Full grid: every lookahead choice at every common sample rate. This is
        // the sample count the processor will pass to setLatencySamples().
        struct Case { LookaheadChoice choice; double sr; int expected; };
        const Case cases[] = {
            { LookaheadChoice::Ms0,   44100.0, 0    },
            { LookaheadChoice::Ms1,   44100.0, 44   },
            { LookaheadChoice::Ms4,   44100.0, 176  },
            { LookaheadChoice::Ms10,  44100.0, 441  },
            { LookaheadChoice::Ms0,   48000.0, 0    },
            { LookaheadChoice::Ms1,   48000.0, 48   },
            { LookaheadChoice::Ms4,   48000.0, 192  },
            { LookaheadChoice::Ms10,  48000.0, 480  },
            { LookaheadChoice::Ms0,   96000.0, 0    },
            { LookaheadChoice::Ms1,   96000.0, 96   },
            { LookaheadChoice::Ms4,   96000.0, 384  },
            { LookaheadChoice::Ms10,  96000.0, 960  },
        };
        for (const auto &c : cases)
        {
            const int actual = MsToSamples(LookaheadChoiceToMs(c.choice), c.sr);
            if (actual != c.expected)
            {
                std::cerr << "FAIL: lookahead choice " << (int)c.choice
                          << " @ " << c.sr << " Hz -> " << actual
                          << ", expected " << c.expected << "\n";
                ++failureCount;
            }
        }
    }
}


//==============================================================================

int main()
{
    std::cout << "CenterSpace test harness\n";

    TestCompressorStaticCurve();
    TestSoftKneeStaticCurve();
    TestMidSideRoundTrip();
    TestBallisticsAttackTime();
    TestScFilterResponse();
    TestCompressMacro();
    TestReactMacro();
    TestFeelBakes();
    TestFocusPresets();
    TestStyleOverride();
    TestLookaheadChoice();

    if (failureCount == 0)
    {
        std::cout << "All tests passed.\n";
        return 0;
    }

    std::cerr << failureCount << " test(s) failed.\n";
    return 1;
}
