/*
  ==============================================================================

 VibeDetection.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include "VibeLayout.h"
#include <JuceHeader.h>

class Selector;

//==============================================================================

class VibeDetection
    : public VibeLayout
{
public:
    VibeDetection(GuiResources &resources);
    ~VibeDetection();
    
    void resized() override;
    
private:
    juce::Slider   compressSlider;
    juce::Label    compressLabel;
    juce::Label    laLabel;
    juce::Label    focusLabel;
    juce::ComboBox focusBox;
    
    std::unique_ptr<Selector> laSelector;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   compressAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> focusAttachment;
};
