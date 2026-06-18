/*
  ==============================================================================

    TitleHeader.h
    Created: 4 Feb 2021 11:47:30am
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include "ABButton.h"
#include <JuceHeader.h>

class Selector;
class PatchControls;

//==============================================================================

class TitleHeader
    : public  juce::Component
    , private juce::Timer
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
    
    // Overlay for tooltip & right-click
    class TitleArea
        : public  juce::Component
        , public  juce::SettableTooltipClient
    {
    public:
        explicit TitleArea(std::function<void()> rightClickHandler);
        void mouseDown(const juce::MouseEvent &e) override;

    private:
        std::function<void()> onRightClick;
    };

    void timerCallback() override;
    
    void ShowOptionsMenu();
    juce::String PatchIdentity() const;

    GuiResources &resources;

    std::unique_ptr<Selector>       uiModeSelector;
    std::unique_ptr<ABButton>       abButton;
    std::unique_ptr<PatchControls>  patchControls;
    std::unique_ptr<TitleArea>      titleArea;

    juce::Rectangle<int> fullumMusicArea;
    juce::Rectangle<int> centerArea;
    juce::Rectangle<int> spaceArea;
    juce::Rectangle<int> glyphArea;
    
    juce::String lastPatchIdentity;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleHeader)
};

