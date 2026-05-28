/*
  ==============================================================================

    VUMeter.cpp
    Created: 14 Dec 2020 4:14:17pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VUMeter.h"


//==============================================================================

VUMeter::VUMeter()  {}
VUMeter::~VUMeter() {}

void VUMeter::paint(juce::Graphics &g)
{
    // Clipping? Bright red, else dark red
    if (levelClipping)
        g.setColour(clippingRed);
    else
        g.setColour(clipBackRed);

    g.fillRect(clipBack);

    // Level meter background
    g.setColour(levelBackGreen);
    g.fillRect(meterBack);

    // Level meter VU
    g.setColour(levelGreen);
    g.fillRect(meterLight);
}

void VUMeter::resized()
{
    int  reducer = 2;
    auto bounds  = getLocalBounds();

    // Create Clipping Light Area
    auto reducedArea = bounds.reduced(reducer);
    auto clipArea    = reducedArea.removeFromTop(reducedArea.proportionOfHeight(0.2f));

    clipBack.setBounds(clipArea.getX(), clipArea.getY(), clipArea.getWidth(), clipArea.getHeight());

    // Create Level Meter Area
    auto vuMeterArea = reducedArea;

    meterBack.setBounds(vuMeterArea.getX(), vuMeterArea.getY(), vuMeterArea.getWidth(), vuMeterArea.getHeight());
    meterLight.setBounds(vuMeterArea.getX()
                         , vuMeterArea.getY() + vuMeterArea.getHeight()
                         , vuMeterArea.getWidth()
                         , -vuMeterArea.proportionOfHeight(heightMult));
    
}

void VUMeter::VuMeterLevel(float level, float sampleRate)
{
    // Convert to dB to get proper response curve. Normalize for resize() bounds
    float multiplier = juce::jmap(juce::Decibels::gainToDecibels(level), -60.0f, 0.0f, 0.0f, 1.0f);

    // limit values: cap at 1.0f
    if (multiplier > 1.0f)
        multiplier = 1.0f;

    // If sample rate changes, update SR and decay factors
    if (!juce::approximatelyEqual(SR, sampleRate))
    {
        SR              = sampleRate;
        decayFactorRise = decayRateRise * SR;
        decayFactorFall = decayRateFall * SR;
    }

    HeightMultiplier(multiplier);

    levelClipping = (multiplier < 1.0f) ? false : true;

    resized();
    repaint();
}

void VUMeter::SetColors(juce::Colour &levelColor, juce::Colour &clipColor, juce::Colour &backingGrey)
{
    clipBackRed    = clipColor.darker().darker();
    levelBackGreen = backingGrey;
    levelGreen     = levelColor.brighter().brighter();
}

void VUMeter::HeightMultiplier(float mult)
{
    if (mult > heightMult)
        heightMult = mult * (1.0f - (1.0f / decayFactorRise));
    else
        heightMult *= 1.0f - (1.0f / decayFactorFall);

    if (mult == 1.0f)
        heightMult = 1.0f;
}


//==============================================================================

ReduceMeter::ReduceMeter()  {}
ReduceMeter::~ReduceMeter() {}

void ReduceMeter::resized()
{
    int  reducer = 2;
    auto bounds  = getLocalBounds();

    // Create Clipping Light Area
    auto reducedArea = bounds.reduced(reducer);
    auto clipArea    = reducedArea.removeFromTop(reducedArea.proportionOfHeight(0.2f));

    clipBack.setBounds(clipArea.getX(), clipArea.getY(), clipArea.getWidth(), clipArea.getHeight());

    // Create Gain Reduction Meter area
    juce::Rectangle<int> reductionMeterArea = reducedArea;

    meterBack.setBounds(reductionMeterArea.getX()
                        , reductionMeterArea.getY()
                        , reductionMeterArea.getWidth()
                        , reductionMeterArea.getHeight());
    meterLight.setBounds(reductionMeterArea.getX()
                         , reductionMeterArea.getY()
                         , reductionMeterArea.getWidth()
                         , reductionMeterArea.proportionOfHeight(heightMult));
}

void ReduceMeter::VuMeterLevel(float level, float sampleRate)
{
    // Limit value: clip at 1.0f
    float multiplier = (level < 1.0f) ? level : 1.0f;

    // If sample rate changes, update SR and decay factors
    if (!juce::approximatelyEqual(SR, sampleRate))
    {
        SR              = sampleRate;
        decayFactorRise = decayRateRise * SR;
        decayFactorFall = decayRateFall * SR;
    }

    HeightMultiplier(multiplier);

    levelClipping = false;

    resized();
    repaint();
}
