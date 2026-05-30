/*
  ==============================================================================

 VibeDynamics.cpp
    Author:  Robert Fullum

  ==============================================================================
*/

#include "VibeDynamics.h"


//==============================================================================

VibeDynamics::VibeDynamics(GuiResources &resources)
{
    setOpaque(false);
}

VibeDynamics::~VibeDynamics() {}

void VibeDynamics::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::honeydew);
}

void VibeDynamics::resized()
{
    
}
