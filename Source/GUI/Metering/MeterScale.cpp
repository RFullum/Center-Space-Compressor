/*
  ==============================================================================

    MeterScale.cpp

  ==============================================================================
*/

#include "MeterScale.h"
#include "MeterScaling.h"


//==============================================================================

namespace
{
    static constexpr float labelFontPx     = 10.0f;
    static constexpr float labelSlotHeight = 14.0f;
}


//==============================================================================

MeterScale::MeterScale(GuiResources          &res
                       , Type                 t
                       , juce::Justification  just)
: resources(res)
, type(t)
, textJustification(just)
{
    setOpaque(false);
}

void MeterScale::SetJustification(juce::Justification j)
{
    if (j != textJustification)
    {
        textJustification = j;
        repaint();
    }
}

void MeterScale::paint(juce::Graphics &g)
{
    g.setColour(resources.theme.textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica", labelFontPx, juce::Font::plain)));

    const auto bounds = getLocalBounds().toFloat();

    // The dB → Y mapping uses an inset rectangle equal to the sibling meters'
    // bounds (i.e. our own bounds minus the label-clearance padding above and
    // below). Labels can extend into the padding without clipping, while the
    // 0 dB / -36 dB ticks still land exactly on the meter's top / bottom edge.
    const float top    = bounds.getY()      + (float) labelPad;
    const float bottom = bounds.getBottom() - (float) labelPad;

    auto drawTick = [&] (float db, float y)
    {
        const auto label = juce::String((int) std::round(db));
        const auto slot  = juce::Rectangle<float>(bounds.getX()
                                                  , y - (labelSlotHeight * 0.5f)
                                                  , bounds.getWidth()
                                                  , labelSlotHeight);
        g.drawText(label, slot, textJustification);
    };

    if (type == Type::Level)
    {
        for (float db : MeterScaling::levelTicksDb)
            drawTick(db, MeterScaling::levelDbToY(db, top, bottom));
    }
    else
    {
        for (float db : MeterScaling::grTicksDb)
            drawTick(db, MeterScaling::grDbToY(db, top, bottom));
    }
}
