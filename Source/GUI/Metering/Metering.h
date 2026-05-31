/*
  ==============================================================================

    Metering.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>

class StereoFieldMeter;
class SidechainGainMeter;
class GainReductionMeter;


//==============================================================================

class Metering
    : public  juce::Component
    , private juce::Timer
{
public:
    explicit Metering(GuiResources &resources);
    ~Metering() override;

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    void timerCallback() override;

    GuiResources &resources;

    std::unique_ptr<SidechainGainMeter> sidechainGainMeter;
    std::unique_ptr<GainReductionMeter> gainReductionMeter;
    std::unique_ptr<StereoFieldMeter>   stereoFieldMeter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Metering)
};
