/*
  ==============================================================================

    SidechainGainMeter.h

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include "GUI/MeterBallistics.h"
#include <JuceHeader.h>


//==============================================================================

class SidechainGainMeter
    : public juce::Component
    , public juce::SettableTooltipClient
{
public:
    explicit SidechainGainMeter(GuiResources &resources);
    ~SidechainGainMeter() override = default;

    void paint(juce::Graphics &) override;
    void resized() override;

    void Update();

private:
    GuiResources &resources;
    
    MeterBallistics level { 20.0f, 250.0f, -120.0f }; 
    MeterPeakHold   peak  { 1.5f,  12.0f, -120.0f };

    float lastPaintedDb    { -120.0f };  // displayed level as of last repaint
    float lastPaintedPeak  { -120.0f };  // displayed peak as of last repaint

    double lastUpdateMs { 0.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SidechainGainMeter)
};
