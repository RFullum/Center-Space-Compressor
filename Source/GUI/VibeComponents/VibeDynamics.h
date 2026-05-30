/*
  ==============================================================================

 VibeDynamics.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>

//==============================================================================

class VibeDynamics
    : public juce::Component
{
public:
    VibeDynamics(GuiResources &resources);
    ~VibeDynamics();
    
    void paint(juce::Graphics &) override;
    void resized() override;
    
private:
};
