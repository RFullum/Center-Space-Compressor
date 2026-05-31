/*
  ==============================================================================

    Metering.cpp
    Author:  Robert Fullum

  ==============================================================================
*/

#include "Metering.h"

#include "SidechainGainMeter.h"
#include "GainReductionMeter.h"
#include "StereoFieldMeter.h"

//==============================================================================

namespace
{
    constexpr int barWidth = 28;
    constexpr int barGap   = 2;
}

Metering::Metering(GuiResources &res)
: resources(res)
, sidechainGainMeter(std::make_unique<SidechainGainMeter>(res))
, gainReductionMeter(std::make_unique<GainReductionMeter>(res))
, stereoFieldMeter  (std::make_unique<StereoFieldMeter>  (res))
{
    setOpaque(false);

    addAndMakeVisible(sidechainGainMeter.get());
    addAndMakeVisible(gainReductionMeter.get());
    addAndMakeVisible(stereoFieldMeter  .get());

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
    sidechainGainMeter->setBounds(bounds.removeFromLeft(barWidth));
    bounds.removeFromLeft(barGap);
    gainReductionMeter->setBounds(bounds.removeFromLeft(barWidth));
    bounds.removeFromLeft(barGap);
    stereoFieldMeter  ->setBounds(bounds);
}

void Metering::timerCallback()
{
    sidechainGainMeter->Update();
    gainReductionMeter->Update();
    stereoFieldMeter  ->Update();
}
