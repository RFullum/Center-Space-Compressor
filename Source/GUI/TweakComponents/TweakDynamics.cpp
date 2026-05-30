/*
  ==============================================================================

 TweakDynamics.h
    Author:  Robert Fullum

  ==============================================================================
*/

#include "TweakDynamics.h"

//==============================================================================


TweakDynamics::TweakDynamics(GuiResources &resources)
{
    setOpaque(false);
}

TweakDynamics::~TweakDynamics() {}

void TweakDynamics::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::hotpink);
}


void TweakDynamics::resized()
{
    
}
