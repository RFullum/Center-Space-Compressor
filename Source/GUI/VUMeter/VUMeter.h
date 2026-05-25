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

    float heightMult    = 0.0f;
    bool  levelClipping = false;

    float SR              = 44100.0f;
    float decayRateRise   = 0.0005f;
    float decayRateFall   = 0.001f;
    float decayFactorRise = decayRateRise * SR;     // 22.05 @ 44.1k
    float decayFactorFall = decayRateFall * SR;     // 44.1  @ 44.1k

private:
    float meterLevel = 0.0f;

    juce::Colour clipBackRed;
    juce::Colour clippingRed    { (juce::uint8)255, (juce::uint8)10, (juce::uint8)27, (juce::uint8)255 };
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
