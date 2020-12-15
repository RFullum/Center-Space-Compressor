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
    VUMeter(float xPosition, float yPosition, float meterWidth_, float meterHeight_, float sampleRate_);
    ~VUMeter() override;
    
    

    void paint (juce::Graphics&) override;
    void resized() override;
    
    /// Sets SR with sampleRate_
    void setSampleRate(float sampleRate_);
    
    /// Sets xPos and yPos to arguments
    void setMeterPosition(float xPosition, float yPosition);
    
    /// Sets meterWidth to newWidth
    void setMeterWidth(float newWidth);
    
    /// Sets meterHeight to new value
    void setMeterHeight(float newHeight);
    
    /// Returns meterWidth
    float getMeterWidth();
    
    /// Returns meterHeight
    float getMeterHeight();
    
    /// Returns meter's current samplerate
    float getSampleRate();
    
    /**
    Processes meter. Takes the level you want to meter in amplitude 0-1 for inVal, and samplerate.
    Updates SR if necessary. Determines height of meter via heightMultiplier( ), then runs resized( ) and repaint( )
    */
    void meterProcess(float inValue, float sampleRate_);
    

private:
    /// Multiplies meterHeight by mult (mult is typically gain level 0-1)
    void heightMultiplier(float mult);
    
    // Rectangle
    Rectangle<float> meterLevel;
    
    // Dimensions
    float SR;
    float xPos; // { getWidth()  / 2.0f };
    float yPos; // { getHeight() / 2.0f };
    float meterWidth; // { 50.0f };
    float meterHeight; // { 120.0f };
    float heightMult;
    float currentValue;
    float decayRate = 0.007f; // in ms
    float decayFactor;      // for N in value *= (1 - 1/N), in samples
    
    
    // Colors 
    Colour brightGreen = Colour((uint8)89, (uint8)255, (uint8)0, (uint8)255);
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VUMeter)
};
