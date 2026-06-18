/*
  ==============================================================================

    TitleHeader.cpp
    Created: 4 Feb 2021 11:47:30am
    Author:  Robert Fullum

  ==============================================================================
*/

#include "TitleHeader.h"

#include "Selector.h"
#include "GUI/PatchControls.h"
#include "PluginProcessor.h"

//==============================================================================

TitleHeader::TitleArea::TitleArea(std::function<void()> rightClickHandler)
: onRightClick(std::move(rightClickHandler))
{
    setOpaque(false);
    setInterceptsMouseClicks(true, false);
}

void TitleHeader::TitleArea::mouseDown(const juce::MouseEvent &e)
{
    if (e.mods.isPopupMenu() && onRightClick)
        onRightClick();
}

//==============================================================================

TitleHeader::TitleHeader(GuiResources &resources)
: resources(resources)
, uiModeSelector(std::make_unique<UIModeSelector>(resources, "uiMode"))
{
    setOpaque(false);

    addAndMakeVisible(uiModeSelector.get());
    uiModeSelector->SetTooltip("Vibe: Less detail, more feel. Tweak: Full control of every parameter.");

    titleArea = std::make_unique<TitleArea>([this] { ShowOptionsMenu(); });
    titleArea->setTooltip("Right-click for options menu.");
    addAndMakeVisible(titleArea.get());

    if (resources.processor != nullptr)
    {
        abButton      = std::make_unique<ABButton>     (resources.theme
                                                        , resources.processor->abCompareManager);
        patchControls = std::make_unique<PatchControls>(resources.theme
                                                        , resources.processor->patchManager
                                                        , abButton.get()
                                                        , resources.csLAndF);
        addAndMakeVisible(patchControls.get());
        lastPatchIdentity = PatchIdentity();
        patchControls->Update();
        startTimerHz(15);
    }
}

TitleHeader::~TitleHeader()
{
    stopTimer();
}

void TitleHeader::ShowOptionsMenu()
{
    if (!resources.getTooltipsEnabled || !resources.setTooltipsEnabled)
        return;

    const bool tipsOn = resources.getTooltipsEnabled();

    juce::PopupMenu menu;
    if (resources.csLAndF != nullptr)
        menu.setLookAndFeel(resources.csLAndF);

    menu.addItem(1, "Tooltips", true, tipsOn);

    auto getTips     = resources.getTooltipsEnabled;
    auto setTips     = resources.setTooltipsEnabled;
    auto refreshTips = resources.refreshTooltipWindow;

    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(titleArea.get()),
                       [getTips, setTips, refreshTips] (int chosen)
                       {
                           if (chosen != 1) return;
                           if (! getTips || ! setTips) return;

                           setTips(! getTips());
                           if (refreshTips)
                               refreshTips();
                       });
}


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

    if (titleArea != nullptr)
    {
        const int areaLeft  = centerArea.getX();
        const int areaRight = spaceArea.getRight();
        const int areaTop   = juce::jmin(centerArea.getY(), glyphArea.getY());
        const int areaBot   = juce::jmax(centerArea.getBottom(), glyphArea.getBottom());
        titleArea->setBounds(areaLeft, areaTop, areaRight - areaLeft, areaBot - areaTop);
    }
    
    auto patchingArea = bounds.removeFromRight(300);
    if (patchControls != nullptr)
        patchControls->setBounds(patchingArea.withSizeKeepingCentre(patchingArea.getWidth(), 60));

    bounds.removeFromRight(56);
    uiModeSelector->setBounds(bounds.removeFromRight(141).withSizeKeepingCentre(141, 35));
}

void TitleHeader::timerCallback()
{
    if (!patchControls)
        return;
    
    patchControls->Update();
    
    const auto identity = PatchIdentity();
    if (identity != lastPatchIdentity)
    {
        lastPatchIdentity = identity;
        resources.processor->abCompareManager.ResetToLiveState();
    }
}

juce::String TitleHeader::PatchIdentity() const
{
    jassert(resources.processor);
    auto &pm = *resources.processor;
    return pm.patchManager.GetCurrentPatchFile().getFullPathName()
            + "|"
            + pm.patchManager.GetCurrentPatchName()
            + "|"
            + juce::String((int) pm.patchManager.GetCurrentSource());
}

