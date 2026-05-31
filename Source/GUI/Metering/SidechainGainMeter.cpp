/*
  ==============================================================================

    SidechainGainMeter.cpp

  ==============================================================================
*/

#include "SidechainGainMeter.h"
#include "PluginProcessor.h"

#include <cmath>


//==============================================================================

namespace
{
    constexpr float minDb   = -36.0f;
    constexpr float maxDb   =   3.0f;
    constexpr float floorDb = -120.0f;

    constexpr float riseTimeMs = 20.0f;
    constexpr float fallTimeMs = 250.0f;

    constexpr float peakHoldSeconds = 1.5f;    // peak hold
    constexpr float peakDecayDbPerS = 12.0f;   // peak decay

    constexpr float slotCornerRadius = 2.0f;
    constexpr float slotInsetX       = 4.0f;   // horizontal padding inside the component bounds
    constexpr float slotInsetY       = 4.0f;
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

    const float priorCurrent = currentDb;
    const float priorPeak    = peakHoldDb;

    AdvanceLevel   (targetDb, dtSecs);
    AdvancePeakHold(dtSecs);

    if (std::abs(currentDb  - priorCurrent) > repaintThresholdDb
        || std::abs(peakHoldDb - priorPeak)  > repaintThresholdDb)
    {
        repaint();
    }
}

void SidechainGainMeter::AdvanceLevel(float targetDb, float dtSeconds)
{
    const bool  rising = (targetDb > currentDb);
    const float tauSec = (rising ? riseTimeMs : fallTimeMs) * 0.001f;
    const float alpha  = 1.0f - std::exp(-dtSeconds / tauSec);

    currentDb += alpha * (targetDb - currentDb);
}

void SidechainGainMeter::AdvancePeakHold(float dtSeconds)
{
    if (currentDb > peakHoldDb)
    {
        peakHoldDb    = currentDb;
        peakHoldTimer = peakHoldSeconds;
        return;
    }

    if (peakHoldTimer > 0.0f)
    {
        peakHoldTimer = juce::jmax(0.0f, peakHoldTimer - dtSeconds);
        return;
    }

    peakHoldDb -= peakDecayDbPerS * dtSeconds;
    peakHoldDb  = juce::jmax(peakHoldDb, floorDb);
}

float SidechainGainMeter::DbToY(float db, float top, float bottom) const
{
    const float clamped = juce::jlimit(minDb, maxDb, db);
    const float t       = (clamped - minDb) / (maxDb - minDb);
    return juce::jmap(t, 0.0f, 1.0f, bottom, top);
}

void SidechainGainMeter::paint(juce::Graphics &g)
{
    const auto bounds = getLocalBounds().toFloat().reduced(slotInsetX, slotInsetY);

    g.setColour(resources.theme.structure);
    g.fillRoundedRectangle(bounds, slotCornerRadius);

    const float fillTopY = DbToY(currentDb, bounds.getY(), bounds.getBottom());
    auto fill = juce::Rectangle<float>(bounds.getX()
                                       , fillTopY
                                       , bounds.getWidth()
                                       , bounds.getBottom() - fillTopY);
    g.setColour(resources.theme.secondaryAccent);
    g.fillRoundedRectangle(fill, slotCornerRadius);

    if (peakHoldDb > floorDb + 1.0f)
    {
        const float peakY = DbToY(peakHoldDb, bounds.getY(), bounds.getBottom());
        const auto  tick  = juce::Rectangle<float>(bounds.getX()
                                                   , peakY - (peakTickHeight * 0.5f)
                                                   , bounds.getWidth()
                                                   , peakTickHeight);
        g.setColour(resources.theme.textPrimary);
        g.fillRect(tick);
    }
}
