/*
  ==============================================================================

    Metering.h
    Author:  Robert Fullum

    Host component for the central metering area. Owns the 60 Hz timer that
    drives every child meter's Update(), and lays them out within the editor's
    centre column. Future siblings: SidechainGainMeter, GainReductionMeter.

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>

class StereoFieldMeter;


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

    std::unique_ptr<StereoFieldMeter> stereoFieldMeter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Metering)
};
