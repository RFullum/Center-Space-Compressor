/*
  ==============================================================================

    PatchSelectionPopup.h

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include <JuceHeader.h>
#include <functional>

class PatchManager;


//==============================================================================

class PatchSelectionPopup
{
public:
    PatchSelectionPopup(GuiResources &resources, PatchManager &patchManager);
    ~PatchSelectionPopup() = default;

    /// Show the popup centered over the target component's top-level parent.
    /// `onFileChosen` is invoked exactly once if the user picks a patch.
    /// Dismissal (Esc, click outside) does not invoke the callback.
    void Show(juce::Component *targetComponent,
              std::function<void(const juce::File &)> onFileChosen);

private:
    GuiResources &resources;
    PatchManager &patchManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchSelectionPopup)
};
