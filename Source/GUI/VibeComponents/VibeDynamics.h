/*
  ==============================================================================

 VibeDynamics.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>

class FeelSelector;

//==============================================================================

class VibeDynamics
    : public juce::Component
{
public:
    VibeDynamics(GuiResources &resources);
    ~VibeDynamics();
    
    void resized() override;
    
private:
    juce::Slider reactSlider;
    juce::Label  reactLabel;
    juce::Label  feelLabel;
    
    std::unique_ptr<FeelSelector> feelSelector;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reactAttachment;
};
