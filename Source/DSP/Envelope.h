/*
  ==============================================================================

    Envelope.h
    Created: 17 Nov 2020 2:53:33pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <atomic>


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

    float sampleRate = 44100.0f;
    float attack     = 0.1f;
    float release    = 0.2f;
    float expFactor  = -0.142f;
    float cteAT      = 0.0f;
    float cteRL      = 0.0f;
    float inVal      = 0.0f;
    float prevVal    = 0.0f;
};
