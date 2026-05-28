/*
  ==============================================================================

    VibeModeComponent.h

    Quick-and-dirty container for the v2.0 Vibe-mode macro controls:
    Feel, Compress, React, Focus, Lookahead On/Off.

  ==============================================================================
*/

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
    void SliderSetup(juce::Slider &slider, juce::LookAndFeel &lf);
    void LabelSetup (juce::Label &label, const juce::String &text);
    void ComboSetup (juce::ComboBox &box, const juce::StringArray &items);

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
