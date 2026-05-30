/*
  ==============================================================================

 TweakDynamics.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>

//==============================================================================

class TweakDynamics
    : public juce::Component
{
public:
    TweakDynamics(GuiResources &resources);
    ~TweakDynamics();
    
    void paint(juce::Graphics &) override;
    void resized() override;
    
private:
};
