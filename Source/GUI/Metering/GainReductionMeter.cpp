/*
  ==============================================================================

    GainReductionMeter.cpp

  ==============================================================================
*/

#include "GainReductionMeter.h"
#include "MeterScaling.h"
#include "PluginProcessor.h"

#include <cmath>


//==============================================================================
// Tuning constants
// GR axis bounds (0 → 24 dB of reduction) live in MeterScaling so every meter
// + the scale-label component agree on placement.

namespace
{
    constexpr float attackTimeMs  = 5.0f;
    constexpr float releaseTimeMs = 400.0f;

    constexpr float slotCornerRadius = 2.0f;
    constexpr float slotInsetX       = 4.0f;
    constexpr float slotInsetY       = 4.0f;

    constexpr float repaintThresholdDb = 0.1f;

    // Snap to avoid asymptotic never-getting there
    constexpr float snapToTargetDb = 0.05f;
}


//==============================================================================

GainReductionMeter::GainReductionMeter(GuiResources &res)
: resources(res)
{
    setOpaque(false);
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

    AdvanceLevel(targetDb, dtSecs);

    // Snap to target when very close — otherwise the smoother asymptotes at
    // a non-zero plateau and the meter never empties.
    bool snapped = false;
    
    if (!juce::approximatelyEqual(currentDb, targetDb) && std::abs(currentDb - targetDb) < snapToTargetDb)
    {
        currentDb = targetDb;
        snapped   = true;
    }

    if (snapped || std::abs(currentDb - lastPaintedDb) > repaintThresholdDb)
    {
        repaint();
        lastPaintedDb = currentDb;
    }
}

void GainReductionMeter::AdvanceLevel(float targetDb, float dtSeconds)
{
    const bool  attacking = (targetDb > currentDb);
    const float tauSec    = (attacking ? attackTimeMs : releaseTimeMs) * 0.001f;
    const float alpha     = 1.0f - std::exp(-dtSeconds / tauSec);

    currentDb += alpha * (targetDb - currentDb);
}

void GainReductionMeter::paint(juce::Graphics &g)
{
    const auto bounds = getLocalBounds().toFloat(); // .reduced(slotInsetX, slotInsetY);

    g.setColour(resources.theme.structure);
    g.fillRoundedRectangle(bounds, slotCornerRadius);

    // GR fills top-down: the bar's bottom edge tracks the dB-mapped Y position.
    const float fillBottomY = MeterScaling::grDbToY(currentDb, bounds.getY(), bounds.getBottom());
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
