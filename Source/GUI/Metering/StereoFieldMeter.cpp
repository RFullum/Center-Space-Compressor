/*
  ==============================================================================

    StereoFieldMeter.cpp

  ==============================================================================
*/

#include "StereoFieldMeter.h"
#include "MeterScaling.h"
#include "PluginProcessor.h"

#include <cmath>


//==============================================================================

namespace
{
    // dB range and tick positions come from MeterScaling — single source of
    // truth shared with SC meter, GR meter, and MeterScale labels.

    constexpr float floorDb = -120.0f;   // input atomics below this are treated as silence

    // StereoFieldMeter smoothness
    constexpr float riseTimeMs = 80.0f;
    constexpr float fallTimeMs = 400.0f;

    constexpr float inputFillAlpha   = 0.18f;
    constexpr float outputFillAlpha  = 0.32f;
    constexpr float strokeWidth      = 1.5f;
    constexpr float gridStrokeWidth  = 0.5f;

    constexpr float repaintThresholdDb = 0.1f;

}   // namespace


//==============================================================================

StereoFieldMeter::StereoFieldMeter(GuiResources &res)
: resources(res)
{
    setOpaque(false);
    lastUpdateMs = juce::Time::getMillisecondCounterHiRes();
}

void StereoFieldMeter::resized() {}

void StereoFieldMeter::Update()
{
    auto *processor = resources.processor;
    if (processor == nullptr)
        return;

    const double nowMs   = juce::Time::getMillisecondCounterHiRes();
    const float  dtSecs  = (float) juce::jmax(0.001, (nowMs - lastUpdateMs) / 1000.0);
    lastUpdateMs = nowMs;

    auto linearToDb = [] (float linear)
    {
        return juce::Decibels::gainToDecibels(linear, floorDb);
    };

    const float tInL  = linearToDb(processor->inLevelChan0 .load());
    const float tInC  = linearToDb(processor->inMidLevel   .load());
    const float tInR  = linearToDb(processor->inLevelChan1 .load());
    const float tOutL = linearToDb(processor->outLevelChan0.load());
    const float tOutC = linearToDb(processor->outMidLevel  .load());
    const float tOutR = linearToDb(processor->outLevelChan1.load());

    AdvancePoint(inL,  tInL,  dtSecs);
    AdvancePoint(inC,  tInC,  dtSecs);
    AdvancePoint(inR,  tInR,  dtSecs);
    AdvancePoint(outL, tOutL, dtSecs);
    AdvancePoint(outC, tOutC, dtSecs);
    AdvancePoint(outR, tOutR, dtSecs);

    grDb = processor->gainReduction.load();

    const bool moved = std::abs(inL .currentDb - inL .lastPaintedDb) > repaintThresholdDb
                    || std::abs(inC .currentDb - inC .lastPaintedDb) > repaintThresholdDb
                    || std::abs(inR .currentDb - inR .lastPaintedDb) > repaintThresholdDb
                    || std::abs(outL.currentDb - outL.lastPaintedDb) > repaintThresholdDb
                    || std::abs(outC.currentDb - outC.lastPaintedDb) > repaintThresholdDb
                    || std::abs(outR.currentDb - outR.lastPaintedDb) > repaintThresholdDb
                    || std::abs(grDb           - grDbLastPainted)    > repaintThresholdDb;

    if (moved)
    {
        repaint();
        inL .lastPaintedDb = inL .currentDb;
        inC .lastPaintedDb = inC .currentDb;
        inR .lastPaintedDb = inR .currentDb;
        outL.lastPaintedDb = outL.currentDb;
        outC.lastPaintedDb = outC.currentDb;
        outR.lastPaintedDb = outR.currentDb;
        grDbLastPainted    = grDb;
    }
}

void StereoFieldMeter::AdvancePoint(SamplePoint &point, float targetDb, float dtSeconds)
{
    const bool  rising = (targetDb > point.currentDb);
    const float tauMs  = rising ? riseTimeMs : fallTimeMs;
    const float tauSec = tauMs * 0.001f;

    // Standard one-pole exponential smoother: alpha = 1 - exp(-dt / tau)
    const float alpha = 1.0f - std::exp(-dtSeconds / tauSec);

    point.currentDb += alpha * (targetDb - point.currentDb);
}

void StereoFieldMeter::BuildTopPath(juce::Path               &path
                                    , const SamplePoint      &l
                                    , const SamplePoint      &c
                                    , const SamplePoint      &r
                                    , juce::Rectangle<float>  plotBounds) const
{
    const float left   = plotBounds.getX();
    const float right  = plotBounds.getRight();
    const float centre = plotBounds.getCentreX();
    const float top    = plotBounds.getY();
    const float bottom = plotBounds.getBottom();

    const float yL = MeterScaling::levelDbToY(l.currentDb, top, bottom);
    const float yC = MeterScaling::levelDbToY(c.currentDb, top, bottom);
    const float yR = MeterScaling::levelDbToY(r.currentDb, top, bottom);

    const float midLC = (left + centre) * 0.5f;
    const float midCR = (centre + right) * 0.5f;

    path.startNewSubPath(left, yL);
    path.cubicTo(midLC, yL, midLC, yC, centre, yC);
    path.cubicTo(midCR, yC, midCR, yR, right,  yR);
}

void StereoFieldMeter::BuildCurvePath(juce::Path               &path
                                      , const SamplePoint      &l
                                      , const SamplePoint      &c
                                      , const SamplePoint      &r
                                      , juce::Rectangle<float>  plotBounds) const
{
    BuildTopPath(path, l, c, r, plotBounds);
    path.lineTo(plotBounds.getRight(), plotBounds.getBottom());
    path.lineTo(plotBounds.getX(),     plotBounds.getBottom());
    path.closeSubPath();
}

void StereoFieldMeter::paint(juce::Graphics &g)
{
    const auto plot = getLocalBounds().toFloat();

    g.setColour(resources.theme.structure.withAlpha(0.35f));
    g.fillRoundedRectangle(plot, 4.0f);

    DrawGrid(g, plot);

    // Input curve (drawn first, drawn under)
    {
        juce::Path inputPath;
        BuildCurvePath(inputPath, inL, inC, inR, plot);

        g.setColour(resources.theme.textSecondary.withAlpha(inputFillAlpha));
        g.fillPath(inputPath);

        juce::Path inputTop;
        BuildTopPath(inputTop, inL, inC, inR, plot);
        g.setColour(resources.theme.textSecondary);
        g.strokePath(inputTop, juce::PathStrokeType(strokeWidth));
    }

    // Output curve (drawn on top)
    {
        juce::Path outputPath;
        BuildCurvePath(outputPath, outL, outC, outR, plot);

        g.setColour(resources.theme.primaryAccent.withAlpha(outputFillAlpha));
        g.fillPath(outputPath);

        juce::Path outputTop;
        BuildTopPath(outputTop, outL, outC, outR, plot);
        g.setColour(resources.theme.primaryAccent);
        g.strokePath(outputTop, juce::PathStrokeType(strokeWidth));
    }
}


void StereoFieldMeter::DrawGrid(juce::Graphics &g, juce::Rectangle<float> plot) const
{
    g.setColour(resources.theme.structure.brighter(0.2f));

    for (float db : MeterScaling::levelTicksDb)
    {
        const float y = MeterScaling::levelDbToY(db, plot.getY(), plot.getBottom());
        g.drawHorizontalLine((int) std::round(y), plot.getX(), plot.getRight());
    }

    // Vertical centre line — the C column. Light dashed.
    const float xC = plot.getCentreX();
    juce::Path dashed;
    dashed.startNewSubPath(xC, plot.getY());
    dashed.lineTo         (xC, plot.getBottom());
    const float dashes[] = { 3.0f, 3.0f };
    juce::PathStrokeType(gridStrokeWidth).createDashedStroke(dashed, dashed, dashes, 2);
    g.strokePath(dashed, juce::PathStrokeType(gridStrokeWidth));
}
