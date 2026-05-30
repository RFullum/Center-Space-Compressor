/*
  ==============================================================================

 VibeDetection.h
    Author:  Robert Fullum

  ==============================================================================
*/

#include "VibeDetection.h"


//==============================================================================

VibeDetection::VibeDetection(GuiResources &resources)
{
    setOpaque(false);
}

VibeDetection::~VibeDetection() {}

void VibeDetection::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::honeydew);
}

void VibeDetection::resized()
{
    
}
