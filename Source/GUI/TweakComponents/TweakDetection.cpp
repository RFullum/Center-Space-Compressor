/*
  ==============================================================================

 TweakDetection.cpp
    Author:  Robert Fullum

  ==============================================================================
*/

#include "TweakDetection.h"


//==============================================================================

TweakDetection::TweakDetection(GuiResources &resources)
{
    setOpaque(false);
}

TweakDetection::~TweakDetection() {}

void TweakDetection::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::hotpink);
}

void TweakDetection::resized()
{
    
}

