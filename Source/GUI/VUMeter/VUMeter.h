/*
  ==============================================================================

    VUMeter.h
    Created: 14 Dec 2020 4:14:17pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


//==============================================================================

class VUMeter
    : public juce::Component
{
public:
    VUMeter();
    ~VUMeter() override;

    void paint(juce::Graphics &) override;
    void resized() override;

    virtual void VuMeterLevel(float level, float sampleRate);

    void SetColors(juce::Colour &levelColor, juce::Colour &clipColor, juce::Colour &backingGrey);

protected:
    void HeightMultiplier(float mult);

    juce::Rectangle<int> meterBack;
    juce::Rectangle<int> meterLight;
    juce::Rectangle<int> clipBack;

    float heightMult;
    bool  levelClipping;

    float SR;
    float decayRateRise;
    float decayRateFall;
    float decayFactorRise;
    float decayFactorFall;

private:
    float meterLevel;

    juce::Colour clipBackRed;
    juce::Colour clippingRed;
    juce::Colour levelBackGreen;
    juce::Colour levelGreen;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VUMeter)
};


//==============================================================================

class ReduceMeter
    : public VUMeter
{
public:
    ReduceMeter();
    ~ReduceMeter();

    void resized() override;

    void VuMeterLevel(float level, float sampleRate) override;
};
