//==============================================================================

#pragma once

#include "sigslot/signal.hpp"

template <typename Lockable, typename ...T>
using Signal_Base = sigslot::signal_base<Lockable,T...>;

/**
 * Specialization of signal_base to be used in single threaded contexts.
 * Slot connection, disconnection and signal emission are not thread-safe.
 * The performance improvement over the thread-safe variant is not impressive,
 * so this is not very useful.
 */
template <typename ...T>
using Signal_ST = sigslot::signal_st<T...>;

/**
 * Specialization of signal_base to be used in multi-threaded contexts.
 * Slot connection, disconnection and signal emission are thread-safe.
 *
 * Recursive signal emission and emission cycles are supported too.
 */
template <typename ...T>
using Signal = sigslot::signal_base<std::recursive_mutex, T...>;

using SignalConnection       = sigslot::connection;
using SignalScopedConnection = sigslot::scoped_connection;
using SignalObserver         = sigslot::observer;

/// @brief helper for connections group handling
class SignalConnectionGroup
{
public:
    ~SignalConnectionGroup()
    {
        DisconnectAll();
    }

    template <typename... Args>
    void AddConnections(Args&&... args)
    {
        (connections.push_back(std::forward<Args>(args)), ...);
    }

    void DisconnectAll()
    {
        connections.clear();
    }

    std::vector<SignalScopedConnection> &GetConnections() 
    {
        return connections;
    }

private:
    std::vector<SignalScopedConnection> connections;
};

/// @brief RAII helper for blocking signals.
template <typename SignalType> struct SignalBlocker
{
    SignalBlocker(SignalType& s, bool emitOnRelease = false)
        : sig(s), emit(emitOnRelease)
    {
        sig.block();
    }

    ~SignalBlocker() { release(); }

    SignalBlocker& operator=(SignalBlocker&& other)
    {
        if (this == &other)
            return *this;

        release();
        std::swap(sig, other.sig);
        return *this;
    }

private:
    SignalBlocker(SignalBlocker&) = default;
    SignalBlocker& operator=(SignalBlocker&) = default;

    void release()
    {
        sig.unblock();

        if (emit)
            sig();
    }

    SignalType& sig;
    const bool  emit;
};
