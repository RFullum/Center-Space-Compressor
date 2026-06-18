/*
  ==============================================================================

    CenterSpaceHelpers.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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

    static const juce::FontOptions SelectorFontOptions { juce::FontOptions("Helvetica", 11.0f, juce::Font::plain) };

}   // namespace CenterSpace
