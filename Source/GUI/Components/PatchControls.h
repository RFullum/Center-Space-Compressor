/*
  ==============================================================================

    PatchControls.h

    Two-row patch + A/B widget for the title header:
      Top:    [<]  patch name  [>]
      Bottom: INIT  SAVE  SAVE AS  DELETE  A/B

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include "ABCompareManager.h"
#include <JuceHeader.h>

class PatchManager;


//==============================================================================

class PatchControls
    : public  juce::Component
    , private juce::Timer
{
public:
    PatchControls(GuiResources       &resources
                  , PatchManager     &patchManager
                  , ABCompareManager &abCompareManager);
    ~PatchControls() override;

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    class ABButton
        : public juce::Button
    {
    public:
        ABButton(GuiResources &resources, ABCompareManager &abCompareManager);
        ~ABButton() override = default;

        void paintButton(juce::Graphics &, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    private:
        GuiResources     &resources;
        ABCompareManager &abCompareManager;
    };

    void timerCallback() override;

    void HandlePrev();
    void HandleNext();
    void HandlePatchNameClicked();
    void HandleInit();
    void HandleSave();
    void HandleSaveAs();
    void HandleDelete();
    void HandleAB();

    void RefreshDisplay();
    void ShowPatchPopupMenu();

    GuiResources     &resources;
    PatchManager     &patchManager;
    ABCompareManager &abCompareManager;

    juce::TextButton prevButton;
    juce::TextButton nextButton;
    juce::TextButton patchNameButton;
    juce::TextButton initButton;
    juce::TextButton saveButton;
    juce::TextButton saveAsButton;
    juce::TextButton deleteButton;
    ABButton         abButton;

    // Async dialog ownership — re-used so successive Save As / Delete prompts
    // don't leak.
    std::unique_ptr<juce::AlertWindow> alertWindow;

    juce::String           lastDisplayedName;
    ABCompareManager::Slot lastDisplayedSlot { ABCompareManager::Slot::A };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchControls)
};
