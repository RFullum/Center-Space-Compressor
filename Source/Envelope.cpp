/*
  ==============================================================================

    Envelope.cpp
    Created: 12 Dec 2020 2:15:13pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "Envelope.h"


Envelope::Envelope() : sampleRate(44100.0f), attack(0.1f), release(0.2f), inVal(0.0f), prevVal(0.0f)
{
}

/// Sets sampleRate
void Envelope::setSampleRate(float SR)
{
    sampleRate = SR;
    expFactor  = -2.0f * MathConstants<float>::pi * 1000.0f / sampleRate;
}

/// returns sampleRate
float Envelope::getSampleRate()
{
    return sampleRate;
}

/// Sets attack time in ms
void Envelope::setAttackTime(float atk)
{
    attack = atk;
    cteAT  = calcCte(attack);
}

/// Set release time in ms
void Envelope::setReleaseTime(float rls)
{
    release = rls;
    cteRL   = calcCte(release);
}

/**
 Takes absolute value of input sample, determines if it's increasing or decreasing,
 and appies the attack and release accordingly.
 */
float Envelope::process(float inputVal, std::atomic<float>* peakRMSMode)
{
    if (*peakRMSMode == 1)
        inVal = inputVal * inputVal;
    else
        inVal = std::abs(inputVal);

    float cte    = (inVal > prevVal) ? cteAT : cteRL;
    float result = inVal + cte * (prevVal - inVal);
    
    // update previous value with current values
    prevVal = result;
    
    if (*peakRMSMode == 1)
        return std::sqrt(result);
    
    return result;
}

//================================================================================

/// Calculates the curve attack/release curve
float Envelope::calcCte(float timeMs)
{
    return ( timeMs < (1.0e-3) ) ? 0 : ( std::exp(expFactor / timeMs) );
}
