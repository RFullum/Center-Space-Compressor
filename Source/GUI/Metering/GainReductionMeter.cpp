/*
  ==============================================================================

    GainReductionMeter.cpp

  ==============================================================================
*/

#include "GainReductionMeter.h"
#include "PluginProcessor.h"

#include <cmath>


//==============================================================================
// Tuning constants — Step 5c lands here.

namespace
{
    // GR axis bounds, in dB-of-reduction (positive).
    constexpr float minGrDb       = 0.0f;    // bar empty at this value
    constexpr float maxGrDb       = 24.0f;   // bar full at this value
    constexpr float attackTimeMs  = 5.0f;
    constexpr float releaseTimeMs = 400.0f;

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
    lastUpdateMs = juce::Time::getMillisecondCounterHiRes();
}

void GainReductionMeter::resized() {}

//==============================================================================

void GainReductionMeter::Update()
{
    auto *processor = resources.processor;
    if (processor == nullptr)
        return;

    const double nowMs  = juce::Time::getMillisecondCounterHiRes();
    const float  dtSecs = (float) juce::jmax(0.001, (nowMs - lastUpdateMs) / 1000.0);
    lastUpdateMs = nowMs;

    const float targetDb = processor->gainReduction.load();   // already dB
    const float prior    = currentDb;

    AdvanceLevel(targetDb, dtSecs);

    if (std::abs(currentDb - prior) > repaintThresholdDb)
        repaint();
}

void GainReductionMeter::AdvanceLevel(float targetDb, float dtSeconds)
{
    const bool  attacking = (targetDb > currentDb);
    const float tauSec    = (attacking ? attackTimeMs : releaseTimeMs) * 0.001f;
    const float alpha     = 1.0f - std::exp(-dtSeconds / tauSec);

    currentDb += alpha * (targetDb - currentDb);
}

//==============================================================================

void GainReductionMeter::paint(juce::Graphics &g)
{
    const auto bounds = getLocalBounds().toFloat().reduced(slotInsetX, slotInsetY);

    g.setColour(resources.theme.structure);
    g.fillRoundedRectangle(bounds, slotCornerRadius);

    const float clamped = juce::jlimit(minGrDb, maxGrDb, currentDb);
    const float t       = (clamped - minGrDb) / (maxGrDb - minGrDb);   // 0..1
    const float fillH   = t * bounds.getHeight();

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
