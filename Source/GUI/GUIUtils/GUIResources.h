/*
  ==============================================================================

    GuiResources.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"

//==============================================================================

struct GuiResources
{
    juce::AudioProcessorValueTreeState *apvts  = nullptr;
    const Palette::Theme               &theme;
    juce::LookAndFeel                  *csLAndF = nullptr;
};
