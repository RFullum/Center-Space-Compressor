/*
  ==============================================================================

    CenterSpaceHelpers.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <cmath>

//==============================================================================

namespace CenterSpace
{

    enum class UIModeType
    {
        Vibe = 0,
        Tweak
    };

    enum class StyleType
    {
        VCA = 0,
        Opto
    };

    enum class StereoType
    {
        LeftRight = 0,
        MidSide
    };

    enum class FeelType
    {
        Clean = 0,
        Smooth
    };

    enum class DetectionType
    {
        Peak = 0,
        RMS
    };

    enum class LookaheadMsType
    {
        Zero = 0,
        One,
        Four,
        Ten
    };

    namespace SliderText
    {
        inline juce::String Db(double v)
        {
            return juce::String(v, 1) + " dB";
        }

        inline juce::String Hz(double v)
        {
            if (v >= 10000.0)
                return juce::String(v / 1000.0, 1) + " kHz";    // 20.0 kHz
            if (v >= 1000.0)
                return juce::String(v / 1000.0, 2) + " kHz";    // 1.50 kHz
            return juce::String((int) std::round(v)) + " Hz";   // 90 Hz
        }

        inline juce::String Ms(double v)
        {
            if (v >= 1000.0)
                return juce::String(v / 1000.0, 2) + " s";      // 1.50 s
            if (v >= 100.0)
                return juce::String((int) std::round(v)) + " ms"; // 250 ms
            if (v >= 10.0)
                return juce::String(v, 1) + " ms";              // 12.5 ms
            return juce::String(v, 2) + " ms";                  // 0.05 ms
        }

        inline juce::String Ratio(double v)
        {
            return juce::String(v, 1) + ":1";                   // 4.0:1
        }
    }   // namespace SliderText

}   // namespace CenterSpace
