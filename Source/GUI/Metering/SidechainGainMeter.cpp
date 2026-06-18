/*
  ==============================================================================

    SidechainGainMeter.cpp

  ==============================================================================
*/

#include "SidechainGainMeter.h"
#include "GUI/MeterScaling.h"
#include "PluginProcessor.h"

#include <cmath>


//==============================================================================

namespace
{
    // dB range comes from MeterScaling — shared with stereo-field meter and
    // the dB MeterScale labels so all three stay aligned.

    constexpr float floorDb = -120.0f;

    constexpr float slotCornerRadius = 2.0f;
    constexpr float peakTickHeight   = 2.0f;

    constexpr float repaintThresholdDb = 0.1f;
}


//==============================================================================

SidechainGainMeter::SidechainGainMeter(GuiResources &res)
: resources(res)
{
    setOpaque(false);
    lastUpdateMs = juce::Time::getMillisecondCounterHiRes();
}

void SidechainGainMeter::resized() {}

void SidechainGainMeter::Update()
{
    auto *processor = resources.processor;
    if (processor == nullptr)
        return;

    const double nowMs  = juce::Time::getMillisecondCounterHiRes();
    const float  dtSecs = (float) juce::jmax(0.001, (nowMs - lastUpdateMs) / 1000.0);
    lastUpdateMs = nowMs;

    const float targetDb = juce::Decibels::gainToDecibels(processor->sideChainLevel.load(), floorDb);

    const float currentDb  = level.Advance(targetDb, dtSecs);
    const float peakHoldDb = peak.Advance(currentDb, dtSecs);

    if (std::abs(currentDb  - lastPaintedDb) > repaintThresholdDb
        || std::abs(peakHoldDb - lastPaintedPeak) > repaintThresholdDb)
    {
        repaint();
        lastPaintedDb   = currentDb;
        lastPaintedPeak = peakHoldDb;
    }
}

void SidechainGainMeter::paint(juce::Graphics &g)
{
    const auto bounds = getLocalBounds().toFloat();// .reduced(slotInsetX, slotInsetY);

    g.setColour(resources.theme.structure);
    g.fillRoundedRectangle(bounds, slotCornerRadius);

    const float fillTopY = MeterScaling::levelDbToY(level.GetCurrentDb(), bounds.getY(), bounds.getBottom());
    auto fill = juce::Rectangle<float>(bounds.getX()
                                       , fillTopY
                                       , bounds.getWidth()
                                       , bounds.getBottom() - fillTopY);
    g.setColour(resources.theme.secondaryAccent);
    g.fillRoundedRectangle(fill, slotCornerRadius);

    if (peak.GetPeakDb() > floorDb + 1.0f)
    {
        const float peakY = MeterScaling::levelDbToY(peak.GetPeakDb(), bounds.getY(), bounds.getBottom());
        const auto  tick  = juce::Rectangle<float>(bounds.getX()
                                                   , peakY - (peakTickHeight * 0.5f)
                                                   , bounds.getWidth()
                                                   , peakTickHeight);
        g.setColour(resources.theme.textPrimary);
        g.fillRect(tick);
    }
}
