/*
  ==============================================================================

    GainReductionMeter.h


  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include "GUI/MeterBallistics.h"
#include <JuceHeader.h>


//==============================================================================

class GainReductionMeter
    : public juce::Component
    , public juce::SettableTooltipClient
{
public:
    explicit GainReductionMeter(GuiResources &resources);
    ~GainReductionMeter() override = default;

    void paint(juce::Graphics &) override;
    void resized() override;

    void Update();

private:
    GuiResources &resources;

    MeterBallistics level { 5.0f, 400.0f, 0.0f };
    
    float  lastPaintedDb { 0.0f };  // displayed value as of the most recent repaint
    double lastUpdateMs  { 0.0  };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainReductionMeter)
};
