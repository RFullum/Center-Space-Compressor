/*
  ==============================================================================

    GainReductionMeter.h


  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>


//==============================================================================

class GainReductionMeter
    : public juce::Component
{
public:
    explicit GainReductionMeter(GuiResources &resources);
    ~GainReductionMeter() override = default;

    void paint(juce::Graphics &) override;
    void resized() override;

    void Update();

private:
    void AdvanceLevel(float targetDb, float dtSeconds);

    GuiResources &resources;

    float  currentDb     { 0.0f };  // smoothed GR amount in dB (0 = no reduction)
    float  lastPaintedDb { 0.0f };  // displayed value as of the most recent repaint
    double lastUpdateMs  { 0.0  };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainReductionMeter)
};
