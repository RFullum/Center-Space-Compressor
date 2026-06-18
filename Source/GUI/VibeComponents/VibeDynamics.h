/*
  ==============================================================================

 VibeDynamics.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include "VibeLayout.h"
#include <JuceHeader.h>

class Selector;

//==============================================================================

class VibeDynamics
    : public VibeLayout
{
public:
    VibeDynamics(GuiResources &resources);
    ~VibeDynamics();
    
    void resized() override;
    
private:
    juce::Slider reactSlider;
    juce::Label  reactLabel;
    juce::Label  feelLabel;
    
    std::unique_ptr<Selector> feelSelector;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reactAttachment;
};
