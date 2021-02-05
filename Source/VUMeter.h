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
    
    void vuMeterLevel(float level, float sampleRate);
    
private:
    /// Multiplies meterHeight by mult (mult is typically gain level 0-1)
    void heightMultiplier(float mult);
    
    // Rectangle
    Rectangle<int> meterBack;
    Rectangle<int> meterLight;
    Rectangle<int> clipBack;
    
    // Member Variables
    float meterLevel;
    bool levelClipping;
    
    float heightMult;
    float SR;
    float decayRateRise;        // = 0.0005f;
    float decayRateFall;        // = 0.001f; // in ms... in Seconds?
    float decayFactorRise;
    float decayFactorFall;      // for N in value *= (1 - 1/N), in samples
    
    // Colors 
    Colour brightGreen = Colour((uint8)89, (uint8)255, (uint8)0, (uint8)255);
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VUMeter)
};


//================================================================================

class ReduceMeter : public VUMeter
{
public:
    void resized() override;
};
