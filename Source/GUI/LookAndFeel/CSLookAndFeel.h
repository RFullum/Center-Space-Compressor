/*
  ==============================================================================

    CSLookAndFeel.h
    Created: 29 May 2026 11:39:53am
    Author:  Robert Fullum

  ==============================================================================
*/


#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>

//==============================================================================

class CSLookAndFeel
    : public juce::LookAndFeel_V4
{
public:
    CSLookAndFeel(GuiResources &resources);
    
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height
                          , float sliderPos, float rotaryStartAngle, float rotaryEndAngle
                          , juce::Slider &slider) override;
    
    void SetTrackBackground(juce::Colour color);

private:
    GuiResources &resources;
    juce::Colour  trackBackground;
};
