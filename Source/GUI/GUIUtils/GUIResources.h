/*
  ==============================================================================

    GuiResources.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"

class CenterSpaceAudioProcessor;

//==============================================================================

struct GuiResources
{
    juce::AudioProcessorValueTreeState *apvts     = nullptr;
    const Palette::Theme               &theme;
    juce::LookAndFeel                  *csLAndF   = nullptr;
    CenterSpaceAudioProcessor          *processor = nullptr;
};
