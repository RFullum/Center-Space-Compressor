/*
  ==============================================================================

    Envelope.cpp
    Created: 12 Dec 2020 2:15:13pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "Envelope.h"

#include <JuceHeader.h>
#include <cmath>


//==============================================================================

Envelope::Envelope() {}

void Envelope::SetSampleRate(float SR)
{
    sampleRate = SR;
    expFactor  = -2.0f * juce::MathConstants<float>::pi * 1000.0f / sampleRate;
}

float Envelope::GetSampleRate()
{
    return sampleRate;
}

void Envelope::SetAttackTime(float atk)
{
    attack = atk;
    cteAT  = CalcCte(attack);
}

void Envelope::SetReleaseTime(float rls)
{
    release = rls;
    cteRL   = CalcCte(release);
}

float Envelope::Process(float inputVal, int peakMode)
{
    if (peakMode == 1)
        inVal = inputVal * inputVal;
    else
        inVal = std::abs(inputVal);

    float cte    = (inVal > prevVal) ? cteAT : cteRL;
    float result = inVal + cte * (prevVal - inVal);

    // update previous value with current values
    prevVal = result;

    if (peakMode == 1)
        return std::sqrt(result);

    return result;
}

/// Calculates the curve attack/release curve
float Envelope::CalcCte(float timeMs)
{
    return (timeMs < 1.0e-3f) ? 0.0f : std::exp(expFactor / timeMs);
}
