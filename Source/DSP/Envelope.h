/*
  ==============================================================================

    Envelope.h
    Created: 17 Nov 2020 2:53:33pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once


class Envelope
{
public:
    Envelope();

    void  SetSampleRate(float SR);
    float GetSampleRate();
    void  SetAttackTime(float atk);
    void  SetReleaseTime(float rls);

    // peakMode: 0 = peak detection, 1 = RMS detection. Loaded once per
    // block by the caller; this method does not touch atomics per sample.
    float Process(float inputVal, int peakMode);

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
