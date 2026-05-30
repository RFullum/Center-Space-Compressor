/*
  ==============================================================================

 VibeDetection.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>

//==============================================================================

class VibeDetection
    : public juce::Component
{
public:
    VibeDetection(GuiResources &resources);
    ~VibeDetection();
    
    void paint(juce::Graphics &) override;
    void resized() override;
    
private:
};
