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
    /// Constructor & Destructor
    OtherLookAndFeel();
    ~OtherLookAndFeel();
    
    /// Draws rotary slider as round knob with outer ring and circular tick
    void drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, Slider &slider) override;
    
    /// Draws linear slider with a square thumb, only as wide as the track
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                           float sliderPos,
                                           float minSliderPos,
                                           float maxSliderPos,
                                           const Slider::SliderStyle style, Slider& slider) override;
    
    /// Sets the color of the dial (knob)
    void setDialColor(Colour& dialC);
    
    /// Sets the color of the tick on the dial
    void setTickColor(Colour& tickC);
    
    /// Sets the color of the outer ring
    void setBackColor(Colour& backC);
    
    /// Sets font for Slider Text Box
    Font getLabelFont (Label& label) override;
    
private:
    Colour dialColor;
    Colour tickColor;
    Colour backColor;
};



// ==================================================================================================
// ==================================================================================================


class BoxLookAndFeel : public LookAndFeel_V4
{
public:
    BoxLookAndFeel();
    ~BoxLookAndFeel();
    
    /// Overrides ComboBox lookAndFeel: increases dropdown font size, arrow weight, outline weight, color to white
    void drawComboBox (Graphics& g, int width, int height, bool, int, int, int, int, ComboBox& box) override;
    
    /// Overrides to make minimum ComboBox Font size 18
    Font getComboBoxFont (ComboBox& box) override;
private:
};
