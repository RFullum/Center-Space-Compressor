/*
  ==============================================================================

    PatchControls.cpp

  ==============================================================================
*/

#include "PatchControls.h"

#include "PatchManager.h"
#include "PluginProcessor.h"

//==============================================================================

PatchControls::ABButton::ABButton(GuiResources &res, ABCompareManager &ab)
: juce::Button("AB")
, resources(res)
, abCompareManager(ab)
{}

void PatchControls::ABButton::paintButton(juce::Graphics &g
                                          , bool shouldDrawButtonAsHighlighted
                                          , bool shouldDrawButtonAsDown)
{
    const auto bounds = getLocalBounds().toFloat();

    auto frameColour = resources.theme.textSecondary;
    if (shouldDrawButtonAsHighlighted) frameColour = resources.theme.textPrimary;
    if (shouldDrawButtonAsDown)        frameColour = resources.theme.primaryAccent;

    g.setColour(frameColour);
    g.drawRoundedRectangle(bounds.reduced(0.5f), 3.0f, 1.0f);

    const bool aActive        = (abCompareManager.GetCurrentSlot() == ABCompareManager::Slot::A);
    const auto activeColour   = resources.theme.primaryAccent;
    const auto inactiveColour = resources.theme.textPrimary;

    juce::AttributedString text;
    text.setFont(juce::Font(juce::FontOptions("Helvetica", 13.0f, juce::Font::bold)));
    text.setJustification(juce::Justification::centred);
    text.append("A", aActive ? activeColour : inactiveColour);
    text.append("/", resources.theme.textSecondary);
    text.append("B", aActive ? inactiveColour : activeColour);
    text.draw(g, bounds);
}

//==============================================================================

PatchControls::PatchControls(GuiResources       &res
                             , PatchManager     &pm
                             , ABCompareManager &ab)
: resources(res)
, patchManager(pm)
, abCompareManager(ab)
, abButton(res, ab)
, selectionPopup(res, pm)
{
    auto styleFrame = [this] (juce::TextButton &b, const juce::String &text)
    {
        b.setButtonText(text);
        b.setColour(juce::TextButton::buttonColourId,   juce::Colours::transparentBlack);
        b.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
        b.setColour(juce::TextButton::textColourOffId,  resources.theme.textPrimary);
        b.setColour(juce::TextButton::textColourOnId,   resources.theme.primaryAccent);
        b.setColour(juce::ComboBox::outlineColourId,    resources.theme.textSecondary);
        addAndMakeVisible(b);
    };

    styleFrame(prevButton,      "<");
    styleFrame(nextButton,      ">");
    styleFrame(patchNameButton, "Init");
    styleFrame(initButton,      "INIT");
    styleFrame(saveButton,      "SAVE");
    styleFrame(saveAsButton,    "SAVE AS");
    styleFrame(deleteButton,    "DELETE");

    addAndMakeVisible(abButton);

    prevButton     .setTooltip("Load the previous patch in the list.");
    nextButton     .setTooltip("Load the next patch in the list.");
    patchNameButton.setTooltip("Click to browse and pick a patch.");
    initButton     .setTooltip("Reset all parameters to their default values.");
    saveButton     .setTooltip("Save changes to the current patch.");
    saveAsButton   .setTooltip("Save the current settings as a new patch.");
    deleteButton   .setTooltip("Delete the current patch (user patches only).");
    abButton       .setTooltip("Toggle between A/B compare slots. Each slot is an independent settings snapshot.");

    prevButton     .onClick = [this] { HandlePrev(); };
    nextButton     .onClick = [this] { HandleNext(); };
    patchNameButton.onClick = [this] { HandlePatchNameClicked(); };
    initButton     .onClick = [this] { HandleInit(); };
    saveButton     .onClick = [this] { HandleSave(); };
    saveAsButton   .onClick = [this] { HandleSaveAs(); };
    deleteButton   .onClick = [this] { HandleDelete(); };
    abButton       .onClick = [this] { HandleAB(); };

    RefreshDisplay();
    startTimerHz(15);
}

PatchControls::~PatchControls()
{
    stopTimer();
}

void PatchControls::paint(juce::Graphics &) {}

void PatchControls::resized()
{
    auto bounds = getLocalBounds();

    constexpr int rowGap = 4;
    const int rowH = (bounds.getHeight() - rowGap) / 2;

    auto topRow    = bounds.removeFromTop(rowH);
    bounds.removeFromTop(rowGap);
    auto bottomRow = bounds;

    const int arrowW = 28;
    prevButton.setBounds(topRow.removeFromLeft(arrowW));
    topRow.removeFromLeft(4);
    nextButton.setBounds(topRow.removeFromRight(arrowW));
    topRow.removeFromRight(4);
    patchNameButton.setBounds(topRow);

    constexpr int numCells = 5;
    constexpr int cellGap  = 3;
    const int cellW = (bottomRow.getWidth() - (cellGap * (numCells - 1))) / numCells;

    auto take = [&] () -> juce::Rectangle<int>
    {
        auto r = bottomRow.removeFromLeft(cellW);
        bottomRow.removeFromLeft(cellGap);
        return r;
    };

    initButton  .setBounds(take());
    saveButton  .setBounds(take());
    saveAsButton.setBounds(take());
    deleteButton.setBounds(take());
    abButton    .setBounds(bottomRow);
}

void PatchControls::timerCallback()
{
    RefreshDisplay();
}

void PatchControls::RefreshDisplay()
{
    const auto name = patchManager.GetCurrentPatchName();
    const auto slot = abCompareManager.GetCurrentSlot();

    if (name != lastDisplayedName)
    {
        const auto displayed = patchManager.IsDirty()
                                ? (name + " *")
                                : name;
        patchNameButton.setButtonText(displayed);
        lastDisplayedName = name;
    }

    if (slot != lastDisplayedSlot)
    {
        abButton.repaint();
        lastDisplayedSlot = slot;
    }
}

//==============================================================================

void PatchControls::HandlePrev() { patchManager.StepPatch(-1); RefreshDisplay(); }
void PatchControls::HandleNext() { patchManager.StepPatch(+1); RefreshDisplay(); }

void PatchControls::HandlePatchNameClicked()
{
    ShowPatchPopupMenu();
}

void PatchControls::ShowPatchPopupMenu()
{
    selectionPopup.Show(&patchNameButton,
                        [this] (const juce::File &chosen)
                        {
                            if (patchManager.LoadPatch(chosen))
                                abCompareManager.ResetToLiveState();
        
                            RefreshDisplay();
                        });
}

void PatchControls::HandleInit()
{
    patchManager.LoadInit();
    abCompareManager.ResetToLiveState();
    RefreshDisplay();
}

void PatchControls::HandleSave()
{
    // SavePatch returns false for Init/Factory — fall through to Save As.
    if (patchManager.SavePatch())
    {
        RefreshDisplay();
        return;
    }

    HandleSaveAs();
}

void PatchControls::HandleSaveAs()
{
    alertWindow.reset(new juce::AlertWindow("Save Patch As",
                                            "Name for the new patch:",
                                            juce::MessageBoxIconType::NoIcon));
    alertWindow->setLookAndFeel(resources.csLAndF);

    alertWindow->addTextEditor("name", patchManager.GetCurrentPatchName(), {});
    alertWindow->addButton("Save",   1, juce::KeyPress(juce::KeyPress::returnKey));
    alertWindow->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    alertWindow->enterModalState(true,
        juce::ModalCallbackFunction::create([this] (int result)
        {
            if (alertWindow == nullptr) return;

            if (result == 1)
            {
                const auto requested = alertWindow->getTextEditorContents("name");
                patchManager.SavePatchAs(requested);
                RefreshDisplay();
            }

            alertWindow.reset();
        }), false);
}

void PatchControls::HandleDelete()
{
    if (! patchManager.IsCurrentPatchUserOwned())
        return;

    const auto file = patchManager.GetCurrentPatchFile();
    const auto name = patchManager.GetCurrentPatchName();

    alertWindow.reset(new juce::AlertWindow("Delete Patch",
                                            "Delete \"" + name + "\"? This cannot be undone.",
                                            juce::MessageBoxIconType::WarningIcon));
    alertWindow->setLookAndFeel(resources.csLAndF);
    alertWindow->addButton("Delete", 1, juce::KeyPress(juce::KeyPress::returnKey));
    alertWindow->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    alertWindow->enterModalState(true,
        juce::ModalCallbackFunction::create([this, file] (int result)
        {
            if (alertWindow == nullptr) return;

            if (result == 1)
            {
                patchManager.DeletePatch(file);
                abCompareManager.ResetToLiveState();
                RefreshDisplay();
            }

            alertWindow.reset();
        }), false);
}

void PatchControls::HandleAB()
{
    abCompareManager.Toggle();
    RefreshDisplay();
}
