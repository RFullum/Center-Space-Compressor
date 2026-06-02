/*
  ==============================================================================

    VibeLayout.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

class VibeLayout
    : public juce::Component
{
public:
    VibeLayout();
    ~VibeLayout();
    
    void resized() override;
    
protected:
    juce::Rectangle<int> sliderLabelArea;
    juce::Rectangle<int> sliderArea;
    juce::Rectangle<int> slot1LabelArea;
    juce::Rectangle<int> slot1Area;
    juce::Rectangle<int> slot2LabelArea;
    juce::Rectangle<int> slot2Area;
};
