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
VUMeter::VUMeter() :  heightMult(0.0f), levelClipping(false), SR(44100.0f),
                      decayRateRise(0.0005f), decayRateFall(0.001f),
                      decayFactorRise(decayRateRise * SR), decayFactorFall(decayRateFall * SR),
                      meterLevel(0.0f),
                      clippingRed(Colour( (uint8)255, (uint8)10, (uint8)27, (uint8)255 ) )
{
}


VUMeter::~VUMeter()
{
}


//===========================================================================

void VUMeter::vuMeterLevel(float level, float sampleRate)
{
    // Convert to dB to get proper response curve. Normalize for resize() bounds
    float multiplier = jmap( Decibels::gainToDecibels(level), -100.0f, 0.0f, 0.0f, 1.0f );
    
    // limit values: cap at 1.0f
    if (multiplier > 1.0f)
        multiplier = 1.0f;
    
    // If sample rate changes, update SR and decay factors
    if (SR != sampleRate)
    {
        SR = sampleRate;
        decayFactorRise = decayRateRise * SR;
        decayFactorFall = decayRateFall * SR;
    }
    
    heightMultiplier(multiplier);
    
    levelClipping = (multiplier < 1.0f) ? false : true;
    
    resized();
    repaint();
}


/**
 Sets heightMult to mult if amplitude is increasing, or tails off the heightMult by decayFactor formula if decreasing.
 */
void VUMeter::heightMultiplier(float mult)
{   
    if (mult > heightMult)
        heightMult = mult * ( 1.0f - (1.0f / decayFactorRise) );
    else
        heightMult *= 1.0f - (1.0f / decayFactorFall);
    
    if (mult == 1.0f)
        heightMult = 1.0f;
}

void VUMeter::paint (juce::Graphics& g)
{
    // Clipping? Bright red, else dark red
    if (levelClipping)
        g.setColour ( clippingRed );
    else
        g.setColour ( clipBackRed );
    
    g.fillRect ( clipBack );
    
    // Level meter background
    g.setColour ( levelBackGreen );
    g.fillRect  ( meterBack );
    
    // Level meter VU
    g.setColour ( levelGreen );
    g.fillRect  ( meterLight );
}

void VUMeter::resized()
{
    int reducer    = 2;
    auto totalArea = getLocalBounds();
    
    // Create Clipping Light Area
    Rectangle<int> reducedArea = totalArea.reduced         ( reducer );
    Rectangle<int> clipArea    = reducedArea.removeFromTop ( reducedArea.getHeight() * 0.2f );
    
    clipBack.setBounds( clipArea.getX(), clipArea.getY(), clipArea.getWidth(), clipArea.getHeight() );
    
    // Create Level Meter Area
    Rectangle<int> vuMeterArea = reducedArea;
    
    meterBack.setBounds  ( vuMeterArea.getX(), vuMeterArea.getY(), vuMeterArea.getWidth(), vuMeterArea.getHeight() );
    meterLight.setBounds ( vuMeterArea.getX(), vuMeterArea.getY() + vuMeterArea.getHeight(),
                           vuMeterArea.getWidth(), -vuMeterArea.getHeight() * heightMult );
}


/// Sets the colors of the level and clipping meter
void VUMeter::setColors(Colour& levelColor, Colour& clipColor)
{
    clipBackRed    = clipColor.darker().darker();
    //clippingRed    = clipColor.brighter();
    levelBackGreen = levelColor.darker().darker();
    levelGreen     = levelColor.brighter().brighter();
}

//===========================================================================
//===========================================================================
//===========================================================================


ReduceMeter::ReduceMeter()  {}
ReduceMeter::~ReduceMeter() {}

void ReduceMeter::vuMeterLevel(float level, float sampleRate)
{
    // Limit value: clip at 1.0f
    float multiplier = (level < 1.0f) ? level : 1.0f;
    
    // If sample rate changes, update SR and decay factors
    if (SR != sampleRate)
    {
        SR = sampleRate;
        decayFactorRise = decayRateRise * SR;
        decayFactorFall = decayRateFall * SR;
    }
    
    heightMultiplier(multiplier);
    
    levelClipping = false;
    
    resized();
    repaint();
}


void ReduceMeter::resized()
{
    //meterLevel.setBounds( xPos, yPos, meterWidth, meterHeight * heightMult );
    int reducer    = 2;
    auto totalArea = getLocalBounds();
    
    // Create Clipping Light Area
    Rectangle<int> reducedArea = totalArea.reduced         ( reducer );
    Rectangle<int> clipArea    = reducedArea.removeFromTop ( reducedArea.getHeight() * 0.2f );
    
    clipBack.setBounds( clipArea.getX(), clipArea.getY(), clipArea.getWidth(), clipArea.getHeight() );
    
    // Create Gain Reduction Meter area
    Rectangle<int> reductionMeterArea = reducedArea;
    
    meterBack.setBounds  ( reductionMeterArea.getX(), reductionMeterArea.getY(),
                           reductionMeterArea.getWidth(), reductionMeterArea.getHeight() );
    meterLight.setBounds ( reductionMeterArea.getX(), reductionMeterArea.getY(),
                           reductionMeterArea.getWidth(), reductionMeterArea.getHeight() * heightMult );
    
}
