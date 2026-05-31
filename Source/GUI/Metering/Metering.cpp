/*
  ==============================================================================

    Metering.cpp
    Author:  Robert Fullum

  ==============================================================================
*/

#include "Metering.h"

#include "StereoFieldMeter.h"

//==============================================================================

Metering::Metering(GuiResources &res)
: resources(res)
, stereoFieldMeter(std::make_unique<StereoFieldMeter>(res))
{
    setOpaque(false);

    addAndMakeVisible(stereoFieldMeter.get());
    
    startTimerHz(60);
}

Metering::~Metering()
{
    stopTimer();
}

void Metering::paint(juce::Graphics &) {}

void Metering::resized()
{
    auto bounds = getLocalBounds();

    // TODO: Update when SC and GR meters implemented
    stereoFieldMeter->setBounds(bounds);
}

void Metering::timerCallback()
{
    stereoFieldMeter->Update();
}
