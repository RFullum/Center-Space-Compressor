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
    , public juce::SettableTooltipClient
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
        float currentDb     = -120.0f;  // last smoothed value, dB
        float lastPaintedDb = -120.0f;  // value as of the most recent repaint
    };

    void AdvancePoint(SamplePoint &point, float targetDb, float dtSeconds);

    void BuildTopPath(juce::Path                 &path
                      , const SamplePoint        &l
                      , const SamplePoint        &c
                      , const SamplePoint        &r
                      , juce::Rectangle<float>    plotBounds) const;
    void BuildCurvePath(juce::Path               &path
                        , const SamplePoint      &l
                        , const SamplePoint      &c
                        , const SamplePoint      &r
                        , juce::Rectangle<float>  plotBounds) const;

    void DrawGrid(juce::Graphics &g, juce::Rectangle<float> plotBounds) const;

    GuiResources &resources;

    SamplePoint inL, inC, inR;
    SamplePoint outL, outC, outR;
    float       grDb             { 0.0f };
    float       grDbLastPainted  { 0.0f };

    double lastUpdateMs { 0.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoFieldMeter)
};
