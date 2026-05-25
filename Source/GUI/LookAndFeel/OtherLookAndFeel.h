/*
  ==============================================================================

    OtherLookAndFeel.h
    Created: 4 Feb 2021 10:59:27am
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


//==============================================================================

class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OtherLookAndFeel();
    ~OtherLookAndFeel();

    void drawRotarySlider(juce::Graphics &g
                          , int x
                          , int y
                          , int width
                          , int height
                          , float sliderPos
                          , float rotaryStartAngle
                          , float rotaryEndAngle
                          , juce::Slider &slider) override;

    void drawLinearSlider(juce::Graphics &g
                          , int x
                          , int y
                          , int width
                          , int height
                          , float sliderPos
                          , float minSliderPos
                          , float maxSliderPos
                          , const juce::Slider::SliderStyle style
                          , juce::Slider &slider) override;

    juce::Font getLabelFont(juce::Label &label) override;

    void SetDialColor(juce::Colour &dialC);
    void SetTickColor(juce::Colour &tickC);
    void SetBackColor(juce::Colour &backC);

private:
    juce::Colour dialColor;
    juce::Colour tickColor;
    juce::Colour backColor;
};


//==============================================================================

class BoxLookAndFeel : public juce::LookAndFeel_V4
{
public:
    BoxLookAndFeel();
    ~BoxLookAndFeel();

    void       drawComboBox(juce::Graphics &g, int width, int height, bool, int, int, int, int, juce::ComboBox &box) override;
    juce::Font getComboBoxFont(juce::ComboBox &box) override;

    void SetOutlineColor(juce::Colour &outline);

private:
    juce::Colour outlineColor;
};
