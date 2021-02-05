/*
  ==============================================================================

    OtherLookAndFeel.h
    Created: 4 Feb 2021 10:59:27am
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


class OtherLookAndFeel : public LookAndFeel_V4
{
public:
    /// Constructor
    OtherLookAndFeel();
    
    /// Draws rotary slider as round knob
    void drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, Slider &slider) override;
    
    /// Draws linear slider with a square thumb, only as wide as the track
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                           float sliderPos,
                                           float minSliderPos,
                                           float maxSliderPos,
                                           const Slider::SliderStyle style, Slider& slider) override;
    
    /// Sets the color of the dial (knob)
    void setDialColor(Colour dialC);
    
    /// Sets the colorof the tick on the dial
    void setTickColor(Colour tickC);
    
private:
    Colour dialColor;
    Colour tickColor;
};
