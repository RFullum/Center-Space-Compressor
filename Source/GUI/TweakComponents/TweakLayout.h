/*
  ==============================================================================

    TweakLayout.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

class TweakLayout
    : public juce::Component
{
public:
    TweakLayout();
    ~TweakLayout();
    
    void resized() override;
    
protected:
    juce::Rectangle<int> slider1Area;
    juce::Rectangle<int> slider2Area;
    juce::Rectangle<int> slider3Area;
    juce::Rectangle<int> slider4Area;
    juce::Rectangle<int> sliderLabel1Area;
    juce::Rectangle<int> sliderLabel2Area;
    juce::Rectangle<int> sliderLabel3Area;
    juce::Rectangle<int> sliderLabel4Area;
    
    juce::Rectangle<int> slot1Area;
    juce::Rectangle<int> slot2Area;
    juce::Rectangle<int> slot1LabelArea;
    juce::Rectangle<int> slot2LabelArea;
};
