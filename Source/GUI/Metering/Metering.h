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
class MeterScale;

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
    static constexpr int colorBlockSize = 10;
    
    void timerCallback() override;

    GuiResources &resources;

    std::unique_ptr<SidechainGainMeter> sidechainGainMeter;
    std::unique_ptr<GainReductionMeter> gainReductionMeter;
    std::unique_ptr<StereoFieldMeter>   stereoFieldMeter;
    std::unique_ptr<MeterScale>         meterScale;
    std::unique_ptr<MeterScale>         grMeterScale;
    
    juce::Rectangle<int> lArea;
    juce::Rectangle<int> cArea;
    juce::Rectangle<int> rArea;
    
    juce::Rectangle<int> grArea;
    juce::Rectangle<int> scArea;
    
    juce::Rectangle<int> inArea;
    juce::Rectangle<int> inTextArea;
    juce::Rectangle<int> outArea;
    juce::Rectangle<int> outTextArea;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Metering)
};
