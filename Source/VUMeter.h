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

class VUMeter  : public juce::Component
{
public:
    VUMeter();
    //VUMeter(float xPosition, float yPosition, float meterWidth_, float meterHeight_, float sampleRate_);
    ~VUMeter() override;
    
    

    void paint (juce::Graphics&) override;
    void resized() override;
    
    virtual void vuMeterLevel(float level, float sampleRate);
    
    /// Sets the colors of the level and clipping meter
    void setColors(Colour& levelColor, Colour& clipColor);
    
protected:
    // Rectangle
    Rectangle<int> meterBack;
    Rectangle<int> meterLight;
    Rectangle<int> clipBack;
    
    // Member Variables
    float heightMult;
    
    /// Multiplies meterHeight by mult (mult is typically gain level 0-1)
    void heightMultiplier(float mult);
    
    // Member Varialbes
    bool levelClipping;
    
    float SR;
    float decayRateRise;        // = 0.0005f;
    float decayRateFall;        // = 0.001f; // in ms... in Seconds?
    float decayFactorRise;
    float decayFactorFall;      // for N in value *= (1 - 1/N), in samples
    
private:
    // Member Variables
    float meterLevel;
    
    
    // Colors
    Colour clipBackRed;
    Colour clippingRed;
    Colour levelBackGreen;
    Colour levelGreen;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VUMeter)
};


//================================================================================

class ReduceMeter : public VUMeter
{
public:
    ReduceMeter();
    ~ReduceMeter();
    
    void vuMeterLevel(float level, float sampleRate) override;
    
    void resized() override;
};
