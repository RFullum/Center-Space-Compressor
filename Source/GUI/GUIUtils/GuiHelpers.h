/*
  ==============================================================================

    GuiHelpers.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

namespace CenterSpace
{

    enum class UIModeType
    {
        Vibe = 0,
        Tweak
    };

    enum class StereoType
    {
        LeftRight = 0,
        MidSide
    };


    // Styles a slider with our textbox layout (right of horizontal, below others).
    inline void SetupSlider(juce::Component             *parent
                            , juce::Slider              &slider
                            , juce::Slider::SliderStyle  style
                            , juce::Colour               fillColor
                            , juce::Colour               thumbColor
                            , juce::Colour               textColor)
    {
        slider.setSliderStyle(style);

        const auto tbPos = (style == juce::Slider::SliderStyle::LinearHorizontal)
                              ? juce::Slider::TextBoxRight
                              : juce::Slider::TextBoxBelow;

        // 48px right fits "100 ms"-"999 ms"; 56px below fits "-XX.X dB".
        const int tbW = (tbPos == juce::Slider::TextBoxRight) ? 48 : 56;
        const int tbH = 14;

        slider.setTextBoxStyle(tbPos, false, tbW, tbH);
        slider.setColour      (juce::Slider::textBoxOutlineColourId,    juce::Colours::transparentBlack);
        slider.setColour      (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        slider.setColour      (juce::Slider::textBoxTextColourId,       textColor);

        if (style == juce::Slider::SliderStyle::LinearHorizontal
            || style == juce::Slider::SliderStyle::LinearVertical)
            slider.setColour(juce::Slider::trackColourId, fillColor);
        else if (style == juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag)
            slider.setColour(juce::Slider::rotarySliderFillColourId, fillColor);

        slider.setColour(juce::Slider::thumbColourId, thumbColor);

        parent->addAndMakeVisible(slider);
    }

    inline void SetupLabel(juce::Component              *parent
                           , juce::Label                &label
                           , juce::String                labelText
                           , juce::Colour                color
                           , float                       fontSize
                           , juce::Font::FontStyleFlags  flag = juce::Font::FontStyleFlags::plain
                           , juce::Justification         just = juce::Justification::centred)
    {
        label.setFont             (juce::FontOptions("helvetica", fontSize, 1));
        label.setText             (labelText, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour           (juce::Label::textColourId, color);
        label.setJustificationType(just);

        parent->addAndMakeVisible(label);
    }

}   // namespace CenterSpace
