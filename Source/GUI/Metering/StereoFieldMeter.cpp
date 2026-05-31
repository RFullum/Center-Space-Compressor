/*
  ==============================================================================

    StereoFieldMeter.cpp

  ==============================================================================
*/

#include "StereoFieldMeter.h"
#include "PluginProcessor.h"

#include <cmath>


//==============================================================================

namespace
{

    // dB axis bounds
    constexpr float minDb   = -36.0f;
    constexpr float maxDb   =   3.0f;
    constexpr float floorDb = -120.0f;   // input atomics below this are treated as silence

    // Per-point smoothing time constants (asymmetric: fast rise, slow fall)
    constexpr float riseTimeMs = 20.0f;
    constexpr float fallTimeMs = 250.0f;

    // Paint
    constexpr float plotInsetX     = 24.0f;   // room for dB labels on the left
    constexpr float plotInsetTop   = 18.0f;   // room for L / C / R labels
    constexpr float plotInsetBot   = 18.0f;   // room for dB axis label

    constexpr float inputFillAlpha   = 0.18f;
    constexpr float outputFillAlpha  = 0.32f;
    constexpr float strokeWidth      = 1.5f;
    constexpr float gridStrokeWidth  = 0.5f;

    constexpr float grOverlayThresholdDb = 0.5f;
    constexpr float axisLabelFontPx      = 10.0f;
    constexpr float grOverlayFontPx      = 12.0f;

    // Gridline dB values (in addition to the boundary 0 / maxDb / minDb)
    constexpr float gridDb[] = { 0.0f, -6.0f, -12.0f, -24.0f };

    // Repaint only when something moved at least this much — saves redraws on
    // near-silent material.
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

    const float priorInL  = inL .currentDb;
    const float priorInC  = inC .currentDb;
    const float priorInR  = inR .currentDb;
    const float priorOutL = outL.currentDb;
    const float priorOutC = outC.currentDb;
    const float priorOutR = outR.currentDb;
    const float priorGr   = grDb;

    AdvancePoint(inL,  tInL,  dtSecs);
    AdvancePoint(inC,  tInC,  dtSecs);
    AdvancePoint(inR,  tInR,  dtSecs);
    AdvancePoint(outL, tOutL, dtSecs);
    AdvancePoint(outC, tOutC, dtSecs);
    AdvancePoint(outR, tOutR, dtSecs);

    grDb = processor->gainReduction.load();

    const bool moved = std::abs(inL .currentDb - priorInL ) > repaintThresholdDb
                    || std::abs(inC .currentDb - priorInC ) > repaintThresholdDb
                    || std::abs(inR .currentDb - priorInR ) > repaintThresholdDb
                    || std::abs(outL.currentDb - priorOutL) > repaintThresholdDb
                    || std::abs(outC.currentDb - priorOutC) > repaintThresholdDb
                    || std::abs(outR.currentDb - priorOutR) > repaintThresholdDb
                    || std::abs(grDb           - priorGr  ) > repaintThresholdDb;

    if (moved)
        repaint();
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

float StereoFieldMeter::DbToY(float db, float plotTop, float plotBottom) const
{
    const float clamped = juce::jlimit(minDb, maxDb, db);
    const float t       = (clamped - minDb) / (maxDb - minDb);     // 0 at bottom, 1 at top
    return juce::jmap(t, 0.0f, 1.0f, plotBottom, plotTop);
}

void StereoFieldMeter::BuildCurvePath(juce::Path               &path
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

    const float yL = DbToY(l.currentDb, top, bottom);
    const float yC = DbToY(c.currentDb, top, bottom);
    const float yR = DbToY(r.currentDb, top, bottom);

    path.startNewSubPath(left,   yL);
    path.lineTo         (centre, yC);
    path.lineTo         (right,  yR);
    path.lineTo         (right,  bottom);
    path.lineTo         (left,   bottom);
    path.closeSubPath();
}

void StereoFieldMeter::paint(juce::Graphics &g)
{
    const auto fullBounds = getLocalBounds().toFloat();
    auto plot = fullBounds.reduced(plotInsetX, 0.0f);
    plot.removeFromTop   (plotInsetTop);
    plot.removeFromBottom(plotInsetBot);

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
        inputTop.startNewSubPath(plot.getX(),         DbToY(inL.currentDb, plot.getY(), plot.getBottom()));
        inputTop.lineTo         (plot.getCentreX(),   DbToY(inC.currentDb, plot.getY(), plot.getBottom()));
        inputTop.lineTo         (plot.getRight(),     DbToY(inR.currentDb, plot.getY(), plot.getBottom()));
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
        outputTop.startNewSubPath(plot.getX(),       DbToY(outL.currentDb, plot.getY(), plot.getBottom()));
        outputTop.lineTo         (plot.getCentreX(), DbToY(outC.currentDb, plot.getY(), plot.getBottom()));
        outputTop.lineTo         (plot.getRight(),   DbToY(outR.currentDb, plot.getY(), plot.getBottom()));
        g.setColour(resources.theme.primaryAccent);
        g.strokePath(outputTop, juce::PathStrokeType(strokeWidth));
    }

    DrawAxisLabels(g, plot);
    DrawGrOverlay (g, plot);
}

void StereoFieldMeter::DrawGrid(juce::Graphics &g, juce::Rectangle<float> plot) const
{
    g.setColour(resources.theme.structure.brighter(0.2f));

    for (float db : gridDb)
    {
        const float y = DbToY(db, plot.getY(), plot.getBottom());
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

void StereoFieldMeter::DrawAxisLabels(juce::Graphics &g, juce::Rectangle<float> plot) const
{
    g.setFont(juce::Font(juce::FontOptions("Helvetica", axisLabelFontPx, juce::Font::plain)));
    g.setColour(resources.theme.textSecondary);

    // dB ticks on the left
    auto drawDbTick = [&] (float db)
    {
        const float y = DbToY(db, plot.getY(), plot.getBottom());
        const auto text = juce::String(db, db == 0.0f ? 0 : 0) + " dB";
        const auto area = juce::Rectangle<float>(0.0f, y - 8.0f, plot.getX() - 4.0f, 16.0f);
        g.drawText(text, area, juce::Justification::centredRight);
    };

    drawDbTick(0.0f);
    drawDbTick(-6.0f);
    drawDbTick(-12.0f);
    drawDbTick(-24.0f);
    drawDbTick(minDb);

    // L / C / R labels along the top
    const float topY = plot.getY() - plotInsetTop;
    const float labelH = plotInsetTop;

    g.drawText("L", juce::Rectangle<float>(plot.getX() - 8.0f,       topY, 16.0f, labelH), juce::Justification::centred);
    g.drawText("C", juce::Rectangle<float>(plot.getCentreX() - 8.0f, topY, 16.0f, labelH), juce::Justification::centred);
    g.drawText("R", juce::Rectangle<float>(plot.getRight()  - 8.0f,  topY, 16.0f, labelH), juce::Justification::centred);
}

void StereoFieldMeter::DrawGrOverlay(juce::Graphics &g, juce::Rectangle<float> plot) const
{
    if (grDb < grOverlayThresholdDb)
        return;

    const float xC      = plot.getCentreX();
    const float yInC    = DbToY(inC .currentDb, plot.getY(), plot.getBottom());
    const float yOutC   = DbToY(outC.currentDb, plot.getY(), plot.getBottom());
    const float midY    = (yInC + yOutC) * 0.5f;

    const auto label = juce::String("GR ") + juce::String(grDb, 1) + " dB";

    g.setFont(juce::Font(juce::FontOptions("Helvetica", grOverlayFontPx, juce::Font::bold)));
    g.setColour(resources.theme.primaryAccent);

    const auto area = juce::Rectangle<float>(xC - 60.0f, midY - 10.0f, 120.0f, 20.0f);
    g.drawText(label, area, juce::Justification::centred);
}
