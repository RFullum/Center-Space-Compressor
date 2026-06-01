/*
  ==============================================================================

    SidechainGainMeter.h

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>


//==============================================================================

class SidechainGainMeter
    : public juce::Component
{
public:
    explicit SidechainGainMeter(GuiResources &resources);
    ~SidechainGainMeter() override = default;

    void paint(juce::Graphics &) override;
    void resized() override;

    void Update();

private:
    void  AdvanceLevel(float targetDb, float dtSeconds);
    void  AdvancePeakHold(float dtSeconds);

    GuiResources &resources;

    float currentDb        { -120.0f };  // smoothed level
    float peakHoldDb       { -120.0f };  // peak-hold display value
    float peakHoldTimer    { 0.0f    };  // seconds remaining at current peak before decay starts
    float lastPaintedDb    { -120.0f };  // displayed level as of last repaint
    float lastPaintedPeak  { -120.0f };  // displayed peak as of last repaint

    double lastUpdateMs { 0.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SidechainGainMeter)
};
