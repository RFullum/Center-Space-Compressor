/*
  ==============================================================================

    GuiResources.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <functional>
#include "ColorPalette.h"

class CenterSpaceAudioProcessor;

//==============================================================================

struct GuiResources
{
    juce::AudioProcessorValueTreeState *apvts     = nullptr;
    const Palette::Theme               &theme;
    juce::LookAndFeel                  *csLAndF   = nullptr;
    CenterSpaceAudioProcessor          *processor = nullptr;

    std::function<bool()>     getTooltipsEnabled;
    std::function<void(bool)> setTooltipsEnabled;
    std::function<void()>     refreshTooltipWindow;
};
