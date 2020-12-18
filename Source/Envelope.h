/*
  ==============================================================================

    Envelope.h
    Created: 17 Nov 2020 2:53:33pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


class Envelope
{
public:
    Envelope();
    
    /// Sets sampleRate
    void setSampleRate(float SR);
    
    /// returns sampleRate
    float getSampleRate();
    
    /// Sets attack time in ms
    void setAttackTime(float atk);
    
    /// Set release time in ms
    void setReleaseTime(float rls);
    
    /**
     Takes absolute value of input sample, determines if it's increasing or decreasing,
     and appies the attack and release accordingly. inputVal is the sample value.
     peakRMSMode is either 0 or 1 from the peakRMS combo box
     */
    float process(float inputVal, std::atomic<float>* peakRMSMode);
    
private:
    float calcCte(float timeMs);
    
    // Members
    float sampleRate;
    float attack;
    float release;
    float expFactor;
    float cteAT;
    float cteRL;
    float inVal;
    float prevVal;
};
