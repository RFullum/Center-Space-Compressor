/*
  ==============================================================================

    ABCompareManager.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


//==============================================================================

/// Two-slot A/B comparison on top of the plugin's APVTS.
///
/// Behaviour:
///   - Toggle() snapshots the live APVTS state into the current slot, flips
///     the current-slot indicator, and applies the target slot's snapshot
///     to the APVTS (auto-seeding the target from the just-taken snapshot
///     if it was empty — so A and B start identical and diverge as edited).
///   - `bypass` is excluded from snapshots; host-driven, must not flip on
///     A/B switch.
///   - All operations are message-thread only.
///
/// State I/O: SerializeState / RestoreState round-trip both slots and the
/// current-slot indicator so session reload preserves the comparison.
class ABCompareManager
{
public:
    enum class Slot
    {
        A,
        B
    };

    explicit ABCompareManager(juce::AudioProcessorValueTreeState &apvtsToManage);

    void Init();
    Slot GetCurrentSlot() const noexcept { return currentSlot; }
    void Toggle();
    void SwitchTo(Slot target);
    void ResetToLiveState();
    juce::ValueTree SerializeState() const;
    void RestoreState(const juce::ValueTree &tree);

    static constexpr const char *stateTagName       = "ABCompareState";
    static constexpr const char *slotATagName       = "SlotA";
    static constexpr const char *slotBTagName       = "SlotB";
    static constexpr const char *currentSlotPropID  = "currentSlot";

private:
    static bool IsExcludedFromSnapshot(const juce::String &paramID) noexcept;

    juce::ValueTree SnapshotCurrent() const;
    void            ApplySnapshot(const juce::ValueTree &snapshot);

    juce::AudioProcessorValueTreeState &apvts;

    juce::ValueTree slotA;
    juce::ValueTree slotB;
    Slot            currentSlot { Slot::A };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ABCompareManager)
};
