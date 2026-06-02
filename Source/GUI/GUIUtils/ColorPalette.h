/*
  ==============================================================================

    ColorPalette.h
    Created: 29 May 2026 11:39:53am
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

namespace Palette
{

    struct Theme
    {
        juce::Colour background;
        juce::Colour structure;
        juce::Colour primaryAccent;
        juce::Colour secondaryAccent;
        juce::Colour textPrimary;
        juce::Colour textSecondary;
    };

    inline const Theme DefaultTheme
    {
        juce::Colour(0xFF0D0E12),   // background:      not-quite-black
        juce::Colour(0xFF1A1C24),   // structure:       dark blue-grey
        juce::Colour(0xFFFF6B00),   // primaryAccent:   blaze orange
        juce::Colour(0xFF00C8B4),   // secondaryAccent: teal
        juce::Colour(0xFFD0D8E8),   // textPrimary:     blue-tinted off-white
        juce::Colour(0xFF5A6480)    // textSecondary:   blue-gray
    };

}   // namespace Palette
