/*
  ==============================================================================

    CSLookAndFeel.h
    Author:  Robert Fullum

    Center Space's global LookAndFeel. Set default in editor.
 
  ==============================================================================
*/

#pragma once

#include "ColorPalette.h"
#include <JuceHeader.h>


//==============================================================================

class CSLookAndFeel
    : public juce::LookAndFeel_V4
{
public:
    explicit CSLookAndFeel(const Palette::Theme &themeRef);

    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height
                          , float sliderPos, float rotaryStartAngle, float rotaryEndAngle
                          , juce::Slider &slider) override;

    void SetTrackBackground(juce::Colour color);

    
    void drawComboBox(juce::Graphics &g, int width, int height
                      , bool isButtonDown
                      , int buttonX, int buttonY, int buttonW, int buttonH
                      , juce::ComboBox &box) override;
    juce::Font getComboBoxFont(juce::ComboBox &) override;
    void positionComboBoxText(juce::ComboBox &box, juce::Label &label) override;

    
    void drawPopupMenuBackground(juce::Graphics &g, int width, int height) override;
    void drawPopupMenuItem(juce::Graphics               &g
                           , const juce::Rectangle<int> &area
                           , bool                        isSeparator
                           , bool                        isActive
                           , bool                        isHighlighted
                           , bool                        isTicked
                           , bool                        hasSubMenu
                           , const juce::String         &text
                           , const juce::String         &shortcutKeyText
                           , const juce::Drawable       *icon
                           , const juce::Colour         *textColour) override;
    juce::Font getPopupMenuFont() override;
    void getIdealPopupMenuItemSize(const juce::String &text
                                   , bool              isSeparator
                                   , int               standardMenuItemHeight
                                   , int              &idealWidth
                                   , int              &idealHeight) override;

    
    void drawAlertBox(juce::Graphics               &g
                      , juce::AlertWindow          &alert
                      , const juce::Rectangle<int> &textArea
                      , juce::TextLayout           &textLayout) override;
    juce::Font getAlertWindowFont()        override;
    juce::Font getAlertWindowTitleFont()   override;
    juce::Font getAlertWindowMessageFont() override;

private:
    const Palette::Theme &theme;

    juce::Colour trackBackground = juce::Colours::transparentBlack;

    static constexpr float popupCornerRadius = 4.0f;
    static constexpr int   popupItemPaddingX = 12;
    static constexpr int   popupItemHeight   = 24;
    static constexpr int   popupSeparatorH   = 9;
};
