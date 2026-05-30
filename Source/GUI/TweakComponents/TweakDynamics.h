/*
  ==============================================================================

 TweakDynamics.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>

class StyleSelector;

//==============================================================================

class TweakDynamics
    : public juce::Component
{
public:
    TweakDynamics(GuiResources &resources);
    ~TweakDynamics();
    
    void resized() override;
    
private:
    juce::Slider ratioSlider;
    juce::Slider kneeSlider;
    juce::Slider atkSlider;
    juce::Slider relSlider;
    
    juce::Label ratioLabel;
    juce::Label kneeLabel;
    juce::Label atkLabel;
    juce::Label relLabel;
    juce::Label styleLabel;
    
    std::unique_ptr<StyleSelector> styleSelector;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> kneeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> atkSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> relSliderAttachment;
};
