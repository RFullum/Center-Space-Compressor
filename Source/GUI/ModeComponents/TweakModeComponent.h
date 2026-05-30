/*
  ==============================================================================

    TweakModeComponent.h

    Quick-and-dirty container for the new v2.0 Tweak-mode controls:
    SC HPF, SC LPF, Knee, Style, Lookahead.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "OtherLookAndFeel.h"

// TODO: NIX
//==============================================================================

class TweakModeComponent
    : public juce::Component
{
public:
    TweakModeComponent(juce::AudioProcessorValueTreeState &apvts
                       , OtherLookAndFeel                 &dialLookAndFeel
                       , BoxLookAndFeel                   &comboLookAndFeel);
    ~TweakModeComponent() override;

    void paint(juce::Graphics &) override;
    void resized() override;

    void SetKneeVisible(bool shouldBeVisible);

private:
    OtherLookAndFeel &dialLnF;
    BoxLookAndFeel   &boxLnF;

    juce::Slider   scHpfSlider;
    juce::Slider   scLpfSlider;
    juce::Slider   kneeSlider;
    juce::ComboBox styleBox;
    juce::ComboBox lookaheadBox;

    juce::Label scHpfLabel;
    juce::Label scLpfLabel;
    juce::Label kneeLabel;
    juce::Label styleLabel;
    juce::Label lookaheadLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   scHpfAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   scLpfAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   kneeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> styleAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lookaheadAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TweakModeComponent)
};
