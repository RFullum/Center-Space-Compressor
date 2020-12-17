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
VUMeter::VUMeter() : SR(44100.0f), xPos( getWidth() / 2.0f ), yPos( getHeight() / 2.0f ),
                        meterWidth(50.0f), meterHeight(120.0f), heightMult(0.0f),
                        currentValue(0.0f), decayRateRise(0.0005f), decayRateFall(0.001f),
                        decayFactorRise(decayRateRise * SR), decayFactorFall(decayRateFall * SR)
{
}

VUMeter::VUMeter(float xPosition, float yPosition, float meterWidth_, float meterHeight_, float sampleRate_) :
                    SR(sampleRate_), xPos(xPosition), yPos(yPosition), meterWidth(meterWidth_), meterHeight(meterHeight_),
                    heightMult(0.0f), currentValue(0.0f), decayRateRise(0.0005f), decayRateFall(0.001f),
                    decayFactorRise(decayRateRise * SR), decayFactorFall(decayRateFall * SR)
{
}

VUMeter::~VUMeter()
{
}


//===========================================================================

/// Sets SR with sampleRate_
void VUMeter::setSampleRate(float sampleRate_)
{
    SR = sampleRate_;
}

/// Sets xPos and yPos to arguments
void VUMeter::setMeterPosition(float xPosition, float yPosition)
{
    xPos = xPosition;
    yPos = yPosition;
}

/// Sets meterWidth to newWidth
void VUMeter::setMeterWidth(float newWidth)
{
    meterWidth = newWidth;
}

/// Sets meterHeight to newHeight
void VUMeter::setMeterHeight(float newHeight)
{
    meterHeight = newHeight;
}


/// Returns meterWidth
float VUMeter::getMeterWidth()
{
    return meterWidth;
}

/// Returns meterHeight
float VUMeter::getMeterHeight()
{
    return meterHeight;
}

/// Returns meter's current samplerate
float VUMeter::getSampleRate()
{
    return SR;
}

/**
 Sets heightMult to mult if amplitude is increasing, or tails off the heightMult by decayFactor formula if decreasing.
 */
void VUMeter::heightMultiplier(float mult)
{
    //heightMult = mult;
    //heightMult = (mult > heightMult) ? mult : heightMult * (1.0f - (1.0f / decayFactor) );
    
    if (mult > heightMult)
        heightMult = mult * (1.0f - (1.0f / decayFactorRise) );
    else
        heightMult *= 1.0f - (1.0f / decayFactorFall);
}

/**
Processes meter. Takes the level you want to meter in amplitude 0-1 for inVal, and samplerate.
Updates SR if necessary. Determines height of meter via heightMultiplier( ), then runs resized( ) and repaint( )
*/
void VUMeter::meterProcess(float inValue, float sampleRate_)
{
    SR = (SR != sampleRate_) ? sampleRate_ : SR;
    heightMultiplier(inValue);
    resized();
    repaint();
}


void VUMeter::paint (juce::Graphics& g)
{
    g.setColour(brightGreen);
    g.fillRect(meterLevel);
    //g.drawRect(meterLevel);
    
}

void VUMeter::resized()
{
    meterLevel.setBounds( xPos, yPos + meterHeight, meterWidth, -meterHeight * heightMult );
}

//===========================================================================
