/*
  ==============================================================================

    OtherLookAndFeel.h
    Created: 4 Feb 2021 10:59:27am
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
    /// Constructor & Destructor
    OtherLookAndFeel();
    ~OtherLookAndFeel();
    
    /// Draws rotary slider as round knob with outer ring and circular tick
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override;
    
    /// Draws linear slider with a square thumb, only as wide as the track
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos,
                                           float minSliderPos,
                                           float maxSliderPos,
                                           const juce::Slider::SliderStyle style, juce::Slider& slider) override;
    
    /// Sets the color of the dial (knob)
    void setDialColor(juce::Colour& dialC);
    
    /// Sets the color of the tick on the dial
    void setTickColor(juce::Colour& tickC);
    
    /// Sets the color of the outer ring
    void setBackColor(juce::Colour& backC);
    
    /// Sets font for Slider Text Box
    juce::Font getLabelFont (juce::Label& label) override;
    
private:
    juce::Colour dialColor;
    juce::Colour tickColor;
    juce::Colour backColor;
};



// ==================================================================================================
// ==================================================================================================


class BoxLookAndFeel : public juce::LookAndFeel_V4
{
public:
    BoxLookAndFeel();
    ~BoxLookAndFeel();
    
    /// Overrides ComboBox lookAndFeel: increases dropdown font size, arrow weight, outline weight, color to white
    void drawComboBox (juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box) override;
    
    /// Overrides to make minimum ComboBox Font size 18
    juce::Font getComboBoxFont (juce::ComboBox& box) override;
    
    void setOutlineColor(juce::Colour& outline);
private:
    juce::Colour outlineColor;
};
