/*
  ==============================================================================

    GainReductionMeter.cpp

  ==============================================================================
*/

#include "GainReductionMeter.h"
#include "GUI/MeterScaling.h"
#include "PluginProcessor.h"

#include <cmath>


//==============================================================================
// Tuning constants
// GR axis bounds (0 → 24 dB of reduction) live in MeterScaling so every meter
// + the scale-label component agree on placement.

namespace
{
    constexpr float slotCornerRadius = 2.0f;
    constexpr float slotInsetX       = 4.0f;
    constexpr float slotInsetY       = 4.0f;

    constexpr float repaintThresholdDb = 0.1f;
}


//==============================================================================

GainReductionMeter::GainReductionMeter(GuiResources &res)
: resources(res)
{
    setOpaque(false);
    level.SetSnapToTarget(0.05f); // avoids the asymptotic plateau that never empties
    lastUpdateMs = juce::Time::getMillisecondCounterHiRes();
}

void GainReductionMeter::resized() {}

void GainReductionMeter::Update()
{
    auto *processor = resources.processor;
    if (processor == nullptr)
        return;

    const double nowMs  = juce::Time::getMillisecondCounterHiRes();
    const float  dtSecs = (float) juce::jmax(0.001, (nowMs - lastUpdateMs) / 1000.0);
    lastUpdateMs = nowMs;

    const float targetDb = processor->gainReduction.load();   // already dB

    const float currentDb = level.Advance(targetDb, dtSecs);   // snap now happens inside Advance

    // Repaint on a meaningful move OR when the snap just landed us on target
    // (so the bar fully empties even when the final step is < threshold).
    if (std::abs(currentDb - lastPaintedDb) > repaintThresholdDb
        || (juce::approximatelyEqual(currentDb, targetDb)
            && !juce::approximatelyEqual(lastPaintedDb, currentDb)))
    {
        repaint();
        lastPaintedDb = currentDb;
    }
}

void GainReductionMeter::paint(juce::Graphics &g)
{
    const auto bounds = getLocalBounds().toFloat(); // .reduced(slotInsetX, slotInsetY);

    g.setColour(resources.theme.structure);
    g.fillRoundedRectangle(bounds, slotCornerRadius);

    // GR fills top-down: the bar's bottom edge tracks the dB-mapped Y position.
    const float fillBottomY = MeterScaling::grDbToY(level.GetCurrentDb(), bounds.getY(), bounds.getBottom());
    const float fillH       = fillBottomY - bounds.getY();

    if (fillH > 0.5f)
    {
        const auto fill = juce::Rectangle<float>(bounds.getX()
                                                 , bounds.getY()
                                                 , bounds.getWidth()
                                                 , fillH);
        g.setColour(resources.theme.primaryAccent);
        g.fillRoundedRectangle(fill, slotCornerRadius);
    }
}
