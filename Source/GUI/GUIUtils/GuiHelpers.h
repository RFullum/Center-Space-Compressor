/*
  ==============================================================================

    GuiHelpers.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <cmath>

//==============================================================================

namespace CenterSpace
{

    enum class UIModeType
    {
        Vibe = 0,
        Tweak
    };

    enum class StyleType
    {
        VCA = 0,
        Opto
    };

    enum class StereoType
    {
        LeftRight = 0,
        MidSide
    };

    enum class FeelType
    {
        Clean = 0,
        Smooth
    };

    enum class DetectionType
    {
        Peak = 0,
        RMS
    };

    enum class LookaheadMsType
    {
        Zero = 0,
        One,
        Four,
        Ten
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

    // Slider value-label formatters. Wire each slider with one of these:
    //   slider.textFromValueFunction = CenterSpace::SliderText::Db;
    // Tiered precision keeps labels readable across each parameter's range.
    namespace SliderText
    {
        inline juce::String Db(double v)
        {
            return juce::String(v, 1) + " dB";
        }

        inline juce::String Hz(double v)
        {
            if (v >= 10000.0)
                return juce::String(v / 1000.0, 1) + " kHz";    // 20.0 kHz
            if (v >= 1000.0)
                return juce::String(v / 1000.0, 2) + " kHz";    // 1.50 kHz
            return juce::String((int) std::round(v)) + " Hz";   // 90 Hz
        }

        inline juce::String Ms(double v)
        {
            if (v >= 1000.0)
                return juce::String(v / 1000.0, 2) + " s";      // 1.50 s
            if (v >= 100.0)
                return juce::String((int) std::round(v)) + " ms"; // 250 ms
            if (v >= 10.0)
                return juce::String(v, 1) + " ms";              // 12.5 ms
            return juce::String(v, 2) + " ms";                  // 0.05 ms
        }

        inline juce::String Ratio(double v)
        {
            return juce::String(v, 1) + ":1";                   // 4.0:1
        }
    }

    inline void SetTip(juce::SettableTooltipClient &control, const juce::String &text)
    {
        control.setTooltip(text);
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
