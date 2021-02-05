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
VUMeter::VUMeter() :  meterLevel(0.0f), levelClipping(false), heightMult(0.0f),
                      SR(44100.0f), decayRateRise(0.0005f), decayRateFall(0.001f),
                      decayFactorRise(decayRateRise * SR), decayFactorFall(decayRateFall * SR)
{
}


VUMeter::~VUMeter()
{
}


//===========================================================================

void VUMeter::vuMeterLevel(float level, float sampleRate)
{
    float multiplier = (level < 1.0f) ? level : 1.0f;
    
    // If sample rate changes, update SR and decay factors
    if (SR != sampleRate)
    {
        SR = sampleRate;
        decayFactorRise = decayRateRise * SR;
        decayFactorFall = decayRateFall * SR;
    }
    
    heightMultiplier(multiplier);
    
    levelClipping = (level < 1.0f) ? false : true;
    
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
    Colour clipBackRed    = Colour( (uint8)97,  (uint8)9,   (uint8)5 );
    Colour clippingRed    = Colour( (uint8)255, (uint8)52,  (uint8)41 );
    Colour levelBackGreen = Colour( (uint8)10,  (uint8)87,  (uint8)9 );
    Colour levelGreen     = Colour( (uint8)27,  (uint8)255, (uint8)23 );
    
    
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
    int reducer = 2;
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

//===========================================================================


void ReduceMeter::resized()
{
    //meterLevel.setBounds( xPos, yPos, meterWidth, meterHeight * heightMult );
}
