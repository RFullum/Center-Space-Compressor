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

    void  SetSampleRate(float SR);
    float GetSampleRate();
    void  SetAttackTime(float atk);
    void  SetReleaseTime(float rls);

    float Process(float inputVal, std::atomic<float> *peakRMSMode);

private:
    float CalcCte(float timeMs);

    float sampleRate;
    float attack;
    float release;
    float expFactor;
    float cteAT;
    float cteRL;
    float inVal;
    float prevVal;
};
