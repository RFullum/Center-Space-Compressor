//==============================================================================

#pragma once

#include "signal/Property.h"

// Wraps a Property class, relying in process calls 
// to dispatch changed signals from the calling thread
template <typename T>
class Property_Reader
{
public:
    Property_Reader(Property_Base<sigslot::detail::null_mutex, T> &target)
    : cache(target)
    {
        connection = target.on_change.connect([&](T value)
        {
            cache.store(value);
            dirty.store(true);
        });
        
        dirty.store(true);
    }

    ~Property_Reader()
    {
        connection.disconnect();
    }

    void process()
    {
        if (dirty.exchange(false))
            on_change(cache.load());
    }

    T const get() const
    {
        return cache.load();
    }
    operator const T  &() const { return Property_Reader<T>::get(); }
    const T &operator->() const { return Property_Reader<T>::get(); }
    T const& operator()() const { return Property_Reader<T>::get(); }


    Signal_Base<sigslot::detail::null_mutex, T> on_change;

 private:
    std::atomic<T>    cache;
    std::atomic<bool> dirty{ false };

    SignalScopedConnection connection;
};
