/*
  ==============================================================================

    ABCompareManager.cpp

  ==============================================================================
*/

#include "ABCompareManager.h"


//==============================================================================

ABCompareManager::ABCompareManager(juce::AudioProcessorValueTreeState &apvtsToManage)
: apvts(apvtsToManage)
{}

void ABCompareManager::Init()
{
    slotA = SnapshotCurrent();
    slotB = {}; // empty until first toggle
    currentSlot = Slot::A;
}

void ABCompareManager::Toggle()
{
    const auto target = (currentSlot == Slot::A) ? Slot::B : Slot::A;
    SwitchTo(target);
}

void ABCompareManager::SwitchTo(Slot target)
{
    if (target == currentSlot)
        return;

    const auto departingSnapshot = SnapshotCurrent();
    if (currentSlot == Slot::A)
        slotA = departingSnapshot;
    else
        slotB = departingSnapshot;

    auto &targetTree = (target == Slot::A) ? slotA : slotB;
    if (! targetTree.isValid() || targetTree.getNumChildren() == 0)
        targetTree = departingSnapshot;

    ApplySnapshot(targetTree);
    currentSlot = target;
}

void ABCompareManager::ResetToLiveState()
{
    slotA = SnapshotCurrent();
    slotB = {};
    currentSlot = Slot::A;
}

juce::ValueTree ABCompareManager::SerializeState() const
{
    const bool aValid = slotA.isValid() && (slotA.getNumChildren() > 0);
    const bool bValid = slotB.isValid() && (slotB.getNumChildren() > 0);

    if (! aValid && ! bValid)
        return {};

    juce::ValueTree root { juce::Identifier(stateTagName) };
    root.setProperty(currentSlotPropID
                     , (currentSlot == Slot::A) ? "A" : "B"
                     , nullptr);

    if (aValid)
    {
        juce::ValueTree wrap { juce::Identifier(slotATagName) };
        wrap.appendChild(slotA.createCopy(), nullptr);
        root.appendChild(wrap, nullptr);
    }

    if (bValid)
    {
        juce::ValueTree wrap { juce::Identifier(slotBTagName) };
        wrap.appendChild(slotB.createCopy(), nullptr);
        root.appendChild(wrap, nullptr);
    }

    return root;
}

void ABCompareManager::RestoreState(const juce::ValueTree &tree)
{
    if (! tree.hasType(juce::Identifier(stateTagName)))
        return;

    slotA = {};
    slotB = {};

    if (auto wrapA = tree.getChildWithName(juce::Identifier(slotATagName)); wrapA.isValid() && (wrapA.getNumChildren() > 0))
        slotA = wrapA.getChild(0).createCopy();

    if (auto wrapB = tree.getChildWithName(juce::Identifier(slotBTagName)); wrapB.isValid() && (wrapB.getNumChildren() > 0))
        slotB = wrapB.getChild(0).createCopy();

    const auto slotName = tree.getProperty(currentSlotPropID).toString();
    currentSlot = (slotName == "B") ? Slot::B : Slot::A;
}

bool ABCompareManager::IsExcludedFromSnapshot(const juce::String &paramID) noexcept
{
    return paramID == "bypass";
}

juce::ValueTree ABCompareManager::SnapshotCurrent() const
{
    auto state = apvts.copyState();

    for (int i = state.getNumChildren() - 1; i >= 0; --i)
    {
        const auto child = state.getChild(i);
        const auto id    = child.getProperty("id").toString();

        if (IsExcludedFromSnapshot(id))
            state.removeChild(i, nullptr);
    }

    return state;
}

void ABCompareManager::ApplySnapshot(const juce::ValueTree &snapshot)
{
    if (! snapshot.isValid())
        return;

    for (int i = 0; i < snapshot.getNumChildren(); ++i)
    {
        const auto child = snapshot.getChild(i);
        const auto id    = child.getProperty("id").toString();

        if (IsExcludedFromSnapshot(id))
            continue;

        if (auto *param = apvts.getParameter(id))
        {
            // PARAM children store raw values; APVTS expects normalised.
            const float rawValue = (float) child.getProperty("value");
            param->setValueNotifyingHost(param->convertTo0to1(rawValue));
        }
    }
}
