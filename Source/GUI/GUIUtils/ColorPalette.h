/*
  ==============================================================================

    ColorPalette.h
    Created: 29 May 2026 11:39:53am
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GUI/ColorPalette.h"

//==============================================================================

namespace Palette
{

    inline const Theme DefaultTheme
    {
        juce::Colour(0xFF0D0E12),   // background:      not-quite-black
        juce::Colour(0xFF1A1C24),   // structure:       dark blue-grey
        juce::Colour(0xFFFF6B00),   // primaryAccent:   blaze orange
        juce::Colour(0xFF00C8B4),   // secondaryAccent: teal
        juce::Colour(0xFFD0D8E8),   // textPrimary:     blue-tinted off-white
        juce::Colour(0xFF5A6480),   // textSecondary:   blue-gray
        juce::Colour(0xFFFF2D78),   // pinkAccent:      hot pink
        juce::Colour(0xFFFF6B00)    // orangeAccent:    blaze orange
    };

}   // namespace Palette
