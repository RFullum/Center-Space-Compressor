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

        // Knee = 6 / 12 dB, input exactly at threshold (overshoot = 0):
        //   x = kneeDb/2, gainDb = slope * (kneeDb/2)^2 / (2*kneeDb) = slope * kneeDb / 8
        for (float kneeDb : { 6.0f, 12.0f })
        {
            const float slope    = (1.0f / ratio) - 1.0f;
            const float expected = juce::Decibels::decibelsToGain(slope * kneeDb / 8.0f);
            const float actual   = SoftKneeCompressorGain(thresholdAmp, thresholdDb, ratio, kneeDb);
            EXPECT_NEAR(actual, expected, 1e-5f);
        }

        // For each non-zero knee, sweep the five characteristic input levels and
        // verify the closed-form piecewise gainDb. Levels (relative to threshold):
        //   -kneeDb        : well below knee (gain = 0 dB)
        //   -kneeDb/2      : lower knee boundary (gain = 0 dB)
        //   0              : knee midpoint (gain = slope * kneeDb / 8)
        //   +kneeDb/2      : upper knee boundary (gain = slope * kneeDb/2)
        //   +kneeDb        : well above knee (gain = slope * kneeDb)
        for (float kneeDb : { 6.0f, 12.0f, 24.0f })
        {
            const float slope = (1.0f / ratio) - 1.0f;
            struct Case { float overshootDb; float expectedGainDb; };
            const Case cases[] = {
                { -kneeDb,         0.0f                          },
                { -kneeDb * 0.5f,  0.0f                          },
                {  0.0f,           slope * kneeDb / 8.0f         },
                {  kneeDb * 0.5f,  slope * kneeDb * 0.5f         },
                {  kneeDb,         slope * kneeDb                },
            };
            for (const auto &c : cases)
            {
                const float envAmp   = juce::Decibels::decibelsToGain(thresholdDb + c.overshootDb);
                const float expected = juce::Decibels::decibelsToGain(c.expectedGainDb);
                const float actual   = SoftKneeCompressorGain(envAmp, thresholdDb, ratio, kneeDb);
                EXPECT_NEAR(actual, expected, 1e-4f);
            }
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
    // Input/output stereo-type selectors. Mirrors the conditional encode/decode
    // in processBlock: when inputIsMS, ch0 is already mid and ch1 is side;
    // when outputIsMS, mid/side pass through without the 0.5 decode factor.
    // Helper: returns {chan0Out, chan1Out} for unit gain (no compression).
    static std::pair<float, float> StereoTypeProcess(bool inputIsMS, bool outputIsMS, float ch0In, float ch1In)
    {
        float mid;
        float side;
        if (inputIsMS)
        {
            mid  = ch0In;
            side = ch1In;
        }
        else
        {
            mid  = ch0In + ch1In;
            side = ch0In - ch1In;
        }

        if (outputIsMS)
            return { mid, side };

        return { (mid + side) * 0.5f, (mid - side) * 0.5f };
    }

    void TestStereoTypeSelectors()
    {
        std::cout << "[test] I/O stereo-type selectors\n";

        const float L = 0.7f;
        const float R = -0.3f;

        // LR in, LR out: round-trip back to original.
        {
            const auto [c0, c1] = StereoTypeProcess(false, false, L, R);
            EXPECT_NEAR(c0, L, 1e-6f);
            EXPECT_NEAR(c1, R, 1e-6f);
        }

        // LR in, MS out: ch0 = L+R, ch1 = L-R.
        {
            const auto [c0, c1] = StereoTypeProcess(false, true, L, R);
            EXPECT_NEAR(c0, L + R, 1e-6f);
            EXPECT_NEAR(c1, L - R, 1e-6f);
        }

        // MS in, LR out: ch0 = (M+S)*0.5, ch1 = (M-S)*0.5.
        {
            const float M = 1.0f;
            const float S = 0.4f;
            const auto [c0, c1] = StereoTypeProcess(true, false, M, S);
            EXPECT_NEAR(c0, (M + S) * 0.5f, 1e-6f);
            EXPECT_NEAR(c1, (M - S) * 0.5f, 1e-6f);
        }

        // MS in, MS out: passthrough (no encode, no decode, no compensation).
        {
            const float M = 1.0f;
            const float S = 0.4f;
            const auto [c0, c1] = StereoTypeProcess(true, true, M, S);
            EXPECT_NEAR(c0, M, 1e-6f);
            EXPECT_NEAR(c1, S, 1e-6f);
        }
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


    //==============================================================================
    // End-to-end signal-chain gain-reduction check.
    //
    // Simulates the full processBlock path with fixed parameters:
    //   - L = R = 0.5 (correlated mono content, 0 dBFS mid after encode)
    //   - Steady SC tone at -10 dBFS, threshold = -30 dB, ratio = 20:1, knee = 0,
    //     attack = 1 ms, release = 50 ms => 20 dB of overshoot, very aggressive.
    //
    // After enough samples for the BallisticsFilter to converge to steady state,
    // we read the envelope, push it through the same soft-knee formula the
    // processor uses, and verify:
    //   1. envelope dB ~ SC level dB
    //   2. compGain matches the closed-form expectation
    //   3. outMidLevel (RMS(mid * compGain) * 0.5) is at least 18 dB below inMidLevel
    //   4. With the current VUMeter floor (-60 dB), the bar height drops by
    //      a clearly visible amount (>= 0.25 of full scale).
    //
    // Prints every intermediate dB value so a human reading the test output can
    // verify the meter-tap points match the screenshot complaint.
    void TestSignalChainGainReduction()
    {
        std::cout << "[test] full signal-chain GR (fixed params)\n";

        constexpr double sampleRate = 48000.0;
        constexpr int    totalSamples = 24000;   // 0.5 s -- way past convergence for 1 ms attack

        // Fixed parameters mirroring "side-chain 20 dB over threshold, aggressive comp"
        constexpr float scLevelLin   = 0.31622776f;   // -10 dBFS
        constexpr float thresholdDb  = -30.0f;
        constexpr float ratio        = 20.0f;
        constexpr float kneeDb       = 0.0f;
        constexpr float attackMs     = 1.0f;
        constexpr float releaseMs    = 50.0f;

        // Mid input. L = R = 0.5 -> mid = L+R = 1.0 (0 dBFS), side = 0.
        constexpr float L = 0.5f;
        constexpr float R = 0.5f;
        const     float mid  = L + R;
        const     float side = L - R;

        // Run the envelope follower to steady state on a DC SC level (the static
        // curve only cares about the level, not waveform).
        juce::dsp::ProcessSpec spec;
        spec.sampleRate       = sampleRate;
        spec.maximumBlockSize = 512;
        spec.numChannels      = 1;

        juce::dsp::BallisticsFilter<float> envelope;
        envelope.prepare(spec);
        envelope.setAttackTime (attackMs);
        envelope.setReleaseTime(releaseMs);
        envelope.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::peak);

        // Push 0.5 s of steady SC samples; capture the final envelope value as
        // steady-state.
        float env = 0.0f;
        for (int n = 0; n < totalSamples; ++n)
            env = envelope.processSample(0, scLevelLin);

        const float envDb = juce::Decibels::gainToDecibels(env);

        // Closed-form static-curve gain (matches processBlock).
        const float compGain   = SoftKneeCompressorGain(env, thresholdDb, ratio, kneeDb);
        const float compGainDb = juce::Decibels::gainToDecibels(compGain);

        // Mirror the processor's metering: inMidBuffer holds `mid`, scaled by 0.5
        // at read time; outMidBuffer holds `mid * compGain`, scaled by 0.5 at
        // read time (RMS of a steady value is just the value).
        const float inMidLevel  = std::abs(mid)              * 0.5f;
        const float outMidLevel = std::abs(mid * compGain)   * 0.5f;

        const float inMidDb  = juce::Decibels::gainToDecibels(inMidLevel);
        const float outMidDb = juce::Decibels::gainToDecibels(outMidLevel);
        const float observedGrDb = inMidDb - outMidDb;

        // VU meter mapping currently in VUMeter.cpp:63 (floor = -60 dB).
        const auto MeterBar = [](float linear)
        {
            return juce::jlimit(0.0f, 1.0f,
                                juce::jmap(juce::Decibels::gainToDecibels(linear),
                                           -60.0f, 0.0f, 0.0f, 1.0f));
        };

        const float inMidBar  = MeterBar(inMidLevel);
        const float outMidBar = MeterBar(outMidLevel);

        std::cout << "    SC steady-state env: "    << envDb       << " dB (expected ~-10)\n";
        std::cout << "    compGain:            "    << compGainDb  << " dB\n";
        std::cout << "    inMidLevel:          "    << inMidLevel  << " (" << inMidDb  << " dB)\n";
        std::cout << "    outMidLevel:         "    << outMidLevel << " (" << outMidDb << " dB)\n";
        std::cout << "    observed GR:         "    << observedGrDb << " dB\n";
        std::cout << "    inMid meter bar:     "    << inMidBar    << " (0-1)\n";
        std::cout << "    outMid meter bar:    "    << outMidBar   << " (0-1)\n";
        std::cout << "    bar drop:            "    << (inMidBar - outMidBar) << "\n";

        // 1. Envelope should be ~-10 dB (well within 0.5 dB after 0.5 s of convergence).
        EXPECT_NEAR(envDb, -10.0f, 0.5f);

        // 2. Closed-form: overshoot = +20 dB, slope = 1/20 - 1 = -0.95,
        //    gainDb = -0.95 * 20 = -19 dB.
        EXPECT_NEAR(compGainDb, -19.0f, 0.5f);

        // 3. outMidLevel should be ~19 dB below inMidLevel (driven by compGain).
        EXPECT_NEAR(observedGrDb, 19.0f, 0.5f);

        // 4. With -60 dB meter floor: inMidBar is at -6 dB -> ~0.90; outMidBar
        //    is at -25 dB -> ~0.58. A drop of >= 0.25 is visually obvious.
        if (! ((inMidBar - outMidBar) >= 0.25f))
        {
            std::cerr << "FAIL: meter bar drop too small to be visible: "
                      << (inMidBar - outMidBar) << " (expected >= 0.25)\n";
            ++failureCount;
        }
    }


    //==============================================================================
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

        const auto fullRange  = FocusToScCutoffs(FocusPreset::FullRange);
        const auto vocal      = FocusToScCutoffs(FocusPreset::Vocal);
        const auto kick       = FocusToScCutoffs(FocusPreset::Kick);
        const auto transients = FocusToScCutoffs(FocusPreset::Transients);
        const auto high       = FocusToScCutoffs(FocusPreset::High);

        EXPECT_NEAR(fullRange.hpfHz,      20.0f, 1e-5f);
        EXPECT_NEAR(fullRange.lpfHz,   20000.0f, 1e-5f);
        EXPECT_NEAR(vocal.hpfHz,         250.0f, 1e-5f);
        EXPECT_NEAR(vocal.lpfHz,        3000.0f, 1e-5f);
        EXPECT_NEAR(kick.hpfHz,           40.0f, 1e-5f);
        EXPECT_NEAR(kick.lpfHz,           90.0f, 1e-5f);
        EXPECT_NEAR(transients.hpfHz,    400.0f, 1e-5f);
        EXPECT_NEAR(transients.lpfHz,  15000.0f, 1e-5f);
        EXPECT_NEAR(high.hpfHz,          800.0f, 1e-5f);
        EXPECT_NEAR(high.lpfHz,        20000.0f, 1e-5f);
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
    TestStereoTypeSelectors();
    TestBallisticsAttackTime();
    TestSignalChainGainReduction();
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
