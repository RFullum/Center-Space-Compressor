/*
  ==============================================================================

    Metering.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

class Metering
    : public juce::Component
{
public:
    Metering();
    ~Metering();
    
    void paint(juce::Graphics &) override;
    void resized() override;
};
