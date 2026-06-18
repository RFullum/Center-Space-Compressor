/*
  ==============================================================================

    ABButton.cpp
    Author:  Robert Fullum

  ==============================================================================
*/

#include "ABButton.h"

//==============================================================================

ABButton::ABButton(const Palette::Theme &theme, ABCompareManager &abCompare)
: juce::Button("")
, theme(theme)
, abCompare(abCompare)
{
    setOpaque(false);
    setTooltip("Toggle between A/B compare slots. Each slot is an independent settings snapshot.");
    onClick = [&] { abCompare.Toggle(); repaint(); };
}

ABButton::~ABButton() {}

void ABButton::paintButton(juce::Graphics &g, bool highlighted, bool down)
{
    const auto bounds = getLocalBounds().toFloat();

    auto frameColour = theme.textSecondary;
    if (highlighted) frameColour = theme.textPrimary;
    if (down)        frameColour = theme.primaryAccent;

    g.setColour(frameColour);
    g.drawRoundedRectangle(bounds.reduced(0.5f), 3.0f, 1.0f);

    const bool aActive        = (abCompare.GetCurrentSlot() == ABCompareManager::Slot::A);
    const auto activeColour   = theme.primaryAccent;
    const auto inactiveColour = theme.textPrimary;

    juce::AttributedString text;
    text.setFont(juce::Font(juce::FontOptions("Helvetica", 13.0f, juce::Font::bold)));
    text.setJustification(juce::Justification::centred);
    text.append("A", aActive ? activeColour : inactiveColour);
    text.append("/", theme.textSecondary);
    text.append("B", aActive ? inactiveColour : activeColour);
    text.draw(g, bounds);
}
