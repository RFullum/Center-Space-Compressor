/*
  ==============================================================================

    TitleFooter.cpp
    Author:  Robert Fullum

  ==============================================================================
*/

#include "TitleFooter.h"


//==============================================================================

TitleFooter::TitleFooter(GuiResources &resources)
: resources(resources)
{
    setOpaque(false);
}

TitleFooter::~TitleFooter() {}

void TitleFooter::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();
    bounds.reduce(5, 0);
    g.setColour(resources.theme.textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica"
                                           , 9.0f
                                           , juce::Font::plain)));
    auto versionArea = bounds.removeFromLeft(30);
    auto urlArea     = bounds.removeFromRight(150);
    g.drawText("v2.0", versionArea, juce::Justification::centredLeft);
    g.drawText("FULLUMMUSIC.COM", urlArea, juce::Justification::centredRight);
}
