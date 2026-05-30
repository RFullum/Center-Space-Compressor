/*
  ==============================================================================

    TitleFooter.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>

class DetectionSelector;
class LookaheadTweakSelector;

//==============================================================================

class TweakDetection
    : public juce::Component
{
public:
    TweakDetection(GuiResources &resources);
    ~TweakDetection();
    
    void resized() override;
    
private:
    juce::Slider scGainSlider;
    juce::Slider threshSlider;
    juce::Slider scHpfSlider;
    juce::Slider scLpfSlider;
    
    juce::Label scGainLabel;
    juce::Label threshLabel;
    juce::Label scHpfLabel;
    juce::Label scLpfLabel;
    juce::Label detectionLabel;
    juce::Label laLabel;
    
    std::unique_ptr<DetectionSelector>      detectionSelector;
    std::unique_ptr<LookaheadTweakSelector> laSelector;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> scGainSliderAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> threshSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> scHpfAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> scLpfAttachment;
};
