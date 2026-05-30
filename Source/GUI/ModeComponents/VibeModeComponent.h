/*
  ==============================================================================

    VibeModeComponent.h

    Quick-and-dirty container for the v2.0 Vibe-mode macro controls:
    Feel, Compress, React, Focus, Lookahead On/Off.

  ==============================================================================
*/
// TODO: NIX
#pragma once

#include <JuceHeader.h>
#include "OtherLookAndFeel.h"


//==============================================================================

class VibeModeComponent
    : public juce::Component
{
public:
    VibeModeComponent(juce::AudioProcessorValueTreeState &apvts
                      , OtherLookAndFeel                 &dialLookAndFeel
                      , BoxLookAndFeel                   &comboLookAndFeel);
    ~VibeModeComponent() override;

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    OtherLookAndFeel &dialLnF;
    BoxLookAndFeel   &boxLnF;

    juce::ComboBox     feelBox;
    juce::Slider       compressSlider;
    juce::Slider       reactSlider;
    juce::ComboBox     focusBox;
    juce::ToggleButton lookaheadOnOffButton;

    juce::Label feelLabel;
    juce::Label compressLabel;
    juce::Label reactLabel;
    juce::Label focusLabel;
    juce::Label lookaheadOnOffLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> feelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   compressAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   reactAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> focusAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>   lookaheadOnOffAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VibeModeComponent)
};
