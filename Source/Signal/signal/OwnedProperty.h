//==============================================================================

#pragma once

#include "signal/Property.h"

//==============================================================================
// A Property with ownership that can only be changed by its Owner class
// but if you have an OwnedProperty in your hand, you can connect to its on_change / will_change signals
// as well as read its value.
//
// When you declare an OwnedProperty in your own class, you have to specify the
// Owner template parameter which is the ONE class that is allowed to set it.
//
// However, when passing references to a OwnedProperty around
// you can drop the Owned parameter, and degrade to a simpler OwnedProperty<Type> &


template <typename Lockable, typename T, typename Owner = void>
class OwnedProperty_Base;

//==============================================================================

template <typename Lockable, typename T>
class OwnedProperty_Base<Lockable, T, void>
    : private Property_Base<Lockable, T>
{
public:
    OwnedProperty_Base() = default;

    OwnedProperty_Base(T const& val)
    : Property_Base<Lockable, T>(val)
    {
    }

    using Property_Base<Lockable, T>::on_change;
    using Property_Base<Lockable, T>::will_change;
    using Property_Base<Lockable, T>::get;

protected:
    using Property_Base<Lockable, T>::set;
    using Property_Base<Lockable, T>::setwithnoemit;
    OwnedProperty_Base<Lockable, T> &operator=(T const& rhs)
    {
        set(rhs);
        return *this;
    }
};

//==============================================================================

template <typename Lockable, typename T, typename Owner>
class OwnedProperty_Base final
    : public OwnedProperty_Base<Lockable, T, void>
{
    using OwnedProperty_Base<Lockable, T, void>::set;
    using OwnedProperty_Base<Lockable, T, void>::operator=;
    friend Owner;
};

//==============================================================================

template <typename T, class Owner = void>
using OwnedProperty_ST = OwnedProperty_Base<sigslot::detail::null_mutex, T, Owner>;

template <typename T, class Owner = void>
using OwnedProperty = OwnedProperty_Base<std::recursive_mutex, T, Owner>;