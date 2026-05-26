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
}


//==============================================================================

int main()
{
    std::cout << "CenterSpace test harness\n";

    TestCompressorStaticCurve();
    TestMidSideRoundTrip();
    TestBallisticsAttackTime();

    if (failureCount == 0)
    {
        std::cout << "All tests passed.\n";
        return 0;
    }

    std::cerr << failureCount << " test(s) failed.\n";
    return 1;
}
