/*
  ==============================================================================

    StereoFieldMeter.h

    Visualize Stereo Field levels: Overlapping meters:
    - Input Levels and Output Levels
    - Levels measured at three points: Hard L, Center, Hard R.
    - Curve drawn across three levels

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>


//==============================================================================

class StereoFieldMeter
    : public juce::Component
{
public:
    explicit StereoFieldMeter(GuiResources &resources);
    ~StereoFieldMeter() override = default;

    void paint(juce::Graphics &) override;
    void resized() override;

    void Update();

private:
    struct SamplePoint
    {
        float currentDb = -120.0f;  // last smoothed value, dB
    };

    float DbToY(float db, float plotTop, float plotBottom) const;
    void AdvancePoint(SamplePoint &point, float targetDb, float dtSeconds);

    void BuildCurvePath(juce::Path               &path
                        , const SamplePoint      &l
                        , const SamplePoint      &c
                        , const SamplePoint      &r
                        , juce::Rectangle<float>  plotBounds) const;

    void DrawGrid(juce::Graphics &g, juce::Rectangle<float> plotBounds) const;
    void DrawAxisLabels(juce::Graphics &g, juce::Rectangle<float> plotBounds) const;
    void DrawGrOverlay(juce::Graphics &g, juce::Rectangle<float> plotBounds) const;

    GuiResources &resources;

    SamplePoint inL, inC, inR;
    SamplePoint outL, outC, outR;
    float       grDb { 0.0f };

    double lastUpdateMs { 0.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoFieldMeter)
};
