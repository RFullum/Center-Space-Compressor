/*
  ==============================================================================

    TitleHeader.cpp
    Created: 4 Feb 2021 11:47:30am
    Author:  Robert Fullum

  ==============================================================================
*/

#include "TitleHeader.h"


//==============================================================================

TitleHeader::TitleHeader(GuiResources &resources)
: resources(resources)
{
    setOpaque(false);
}

TitleHeader::~TitleHeader() {}


void TitleHeader::paint(juce::Graphics &g)
{
    
    
    g.setColour(resources.theme.textPrimary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica"
                                           , 14.0f
                                           , juce::Font::bold)));
    g.drawText("FULLUMMUSIC", fullumMusicArea, juce::Justification::centredRight);
    
    g.setColour(resources.theme.primaryAccent);
    g.setFont(juce::Font(juce::FontOptions("Helvetica"
                                           , 27.0f
                                           , juce::Font::bold)));
    g.drawText("CENTER", centerArea, juce::Justification::centred);
    g.drawText("SPACE",  spaceArea,  juce::Justification::centred);
    
    g.setColour(resources.theme.secondaryAccent);
    
    auto workingArea = glyphArea;
    for (int i = 0; i < numGlyphs; ++i)
    {
        const auto removeW = (i == 4)
                                ? glyphW * 2
                                : glyphW;
        auto area = workingArea.removeFromLeft(removeW).toFloat();
        
        switch (i)
        {
            case 1: [[fallthrough]];
            case 7: area.reduce(0, area.proportionOfHeight(0.1f)); break;
            case 2: [[fallthrough]];
            case 6: area.reduce(0, area.proportionOfHeight(0.2f)); break;
            case 3: [[fallthrough]];
            case 5: area.reduce(0, area.proportionOfHeight(0.3f)); break;
            case 4: area.reduce(0, area.proportionOfHeight(0.4f)); break;
            default: break;
        }
        
        g.fillRoundedRectangle(area, 2.0f);
        
        if (i < numGlyphGaps)
            workingArea.removeFromLeft(glyphGap);
    }
}

void TitleHeader::resized()
{
    auto bounds = getLocalBounds();
    bounds.reduce(10, 0);
    
    fullumMusicArea = bounds.removeFromRight(131);
    centerArea      = bounds.removeFromLeft(118);
    
    glyphArea       = bounds.removeFromLeft(glyphAreaW).reduced(0, 26);
    glyphArea.removeFromBottom(5);
    
    spaceArea       = bounds.removeFromLeft(97);
}

