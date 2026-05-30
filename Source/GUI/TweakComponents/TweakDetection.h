/*
  ==============================================================================

    TitleFooter.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>

//==============================================================================

class TweakDetection
    : public juce::Component
{
public:
    TweakDetection(GuiResources &resources);
    ~TweakDetection();
    
    void paint(juce::Graphics &) override;
    void resized() override;
    
private:
    
};
