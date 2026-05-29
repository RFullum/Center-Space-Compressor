#pragma once

#include "Signal.h"

/// @brief Helper which can hold Property(s). It will subscribe to the
/// Property(s) on_change signal and provides it's own blanket blockable
/// OnChange signal.
class HoldableOnChange
{
public:
    Signal_ST<> OnChange;

    /// @brief Adds one or more Property types to the OnChange signal.
    template <typename... PropertyTypes> void Add(PropertyTypes&... properties)
    {
        (subscribe(properties), ...);
    }

    /// @brief Keep this SignalBlocker alive while you make changes that you
    /// don't want to be emitted. Once the connection_blocker is deleted, it
    /// will unblock and emit.
    SignalBlocker<Signal_ST<>> blocker() { return { OnChange, true }; }

private:
    template <typename Type> void subscribe(Type& property)
    {
        connections.push_back(
            property.on_change.connect_scoped([this](auto) { OnChange(); }));
    }

    std::vector<sigslot::scoped_connection> connections;
};
