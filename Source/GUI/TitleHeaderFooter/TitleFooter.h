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

class TitleFooter
    : public juce::Component
{
public:
    TitleFooter(GuiResources &resources);
    ~TitleFooter() override;
    
    void paint(juce::Graphics &) override;

private:
    GuiResources &resources;    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleFooter)
};
