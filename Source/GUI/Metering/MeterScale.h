/*
  ==============================================================================

    MeterScale.h

    Vertical text scale for the metering area. Reusable for both the level
    scale (shared by SC bar + stereo-field meter, range -36 → +3 dB) and the
    GR scale (shared by GR bar only, range 0 → 24 dB of reduction). Tick
    values and dB → Y mapping come from MeterScaling — change them there and
    every meter + every MeterScale instance stays aligned.

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>


//==============================================================================

class MeterScale
    : public juce::Component
{
public:
    enum class Type
    {
        Level,   // -36 → +3 dB
        GR       //   0 → 24 dB of reduction
    };

    MeterScale(GuiResources          &resources
               , Type                type
               , juce::Justification textJustification = juce::Justification::centred);
    ~MeterScale() override = default;

    void paint(juce::Graphics &) override;

    void SetJustification(juce::Justification j);

    /// MeterScale's component bounds must extend this many pixels above AND
    /// below the sibling meters' bounds so labels at the extreme dB values
    /// (0 dB top, -36 dB bottom, 24 dB GR bottom) don't clip. The dB → Y
    /// mapping internally insets by this amount, so ticks still align with
    /// the meters' Y range.
    ///
    /// Use it from the parent layout:
    ///     auto meterRow = bounds.removeFromLeft(scaleW);
    ///     meterScale->setBounds(meterRow.expanded(0, MeterScale::labelPad));
    static constexpr int labelPad = 7;

private:
    GuiResources        &resources;
    Type                 type;
    juce::Justification  textJustification;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeterScale)
};
