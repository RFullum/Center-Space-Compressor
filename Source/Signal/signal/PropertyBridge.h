//==============================================================================

#pragma once

#include "signal/Property.h"

template <typename TYPE_A, typename TYPE_B>
struct Converter
{
    static TYPE_B AToB(const TYPE_A &a) { return (TYPE_B)(a); }
    static TYPE_A BToA(const TYPE_B &b) { return (TYPE_A)(b); }
};

// Base class for Property comms bridging
template <typename PROPERTY_T, typename COMMS_T = PROPERTY_T>
class PropertyBridge
{
public:
    using PROPERTY_TO_COMMS = std::function<COMMS_T(PROPERTY_T)>;
    using COMMS_TO_PROPERTY = std::function<PROPERTY_T(COMMS_T)>;

    PropertyBridge(Property_ST<PROPERTY_T> &prop)
    : property(prop)
    {
        connection = property.on_change.connect(&PropertyBridge::OnPropertyChanged, this);
    }

    virtual ~PropertyBridge()
    {
        connection.disconnect();
    }

    // Transmits a change from our property
    // to be propagated by the custom comms scheme
    // ie: property being changed from UI
    virtual void Transmit(PROPERTY_T value) = 0;

    // Receives a change from the custom comms scheme
    // Updates our property blocking calling comms back
    void Receive(COMMS_T value)
    {
        connection.block();
        property = CommsToProperty(value);
        connection.unblock();
    }

    void Touch() { Transmit(property.get()); }

    PROPERTY_TO_COMMS PropertyToComms;
    COMMS_TO_PROPERTY CommsToProperty;

protected:
    Property_ST<PROPERTY_T> &property;

    SignalScopedConnection connection;

private:
    void OnPropertyChanged(PROPERTY_T value) { Transmit(value); }
};
