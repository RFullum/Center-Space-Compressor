/*
  ==============================================================================

    TitleHeader.h
    Created: 4 Feb 2021 11:47:30am
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>


//==============================================================================

class TitleHeader
    : public juce::Component
{
public:
    TitleHeader(GuiResources &resources);
    ~TitleHeader() override;

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    static constexpr int numGlyphs    = 9;
    static constexpr int glyphW       = 4;
    static constexpr int numGlyphGaps = numGlyphs - 1;
    static constexpr int glyphGap     = 3;
    static constexpr int glyphAreaW   = (glyphW * numGlyphs) + (glyphGap * numGlyphGaps) + glyphW; // center glyph double wide
    
    GuiResources &resources;
    
    juce::Rectangle<int> fullumMusicArea;
    juce::Rectangle<int> centerArea;
    juce::Rectangle<int> spaceArea;
    juce::Rectangle<int> glyphArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleHeader)
};

