//==============================================================================

#pragma once

#include "signal/Signal.h"

// A Property is a encapsulates a value and may inform
// you on any changes applied to this value.
template <typename Lockable, typename T>
class Property_Base 
{
 public:

  typedef T ValueType;
  using lock_type = std::unique_lock<Lockable>;

  // Properties for built-in types are automatically
  // initialized to 0. See template spesialisations
  // at the bottom of this file
  Property_Base()
  {
  }

  Property_Base(T const& val)
  : value(val) 
  {
  }

  Property_Base(T&& val)
  : value(std::move(val))
  {
  }

  Property_Base(Property_Base<Lockable, T> const& toCopy)
  :value(toCopy.value)
  {
  }

  Property_Base(Property_Base<Lockable, T>&& toCopy)
  : value(std::move(toCopy.value))
  {
  }

  // sets the Property to a new value. 
  // will_change and on_change will be emitted if the value changes.
  void set(T const& valueToSet)
  {
      std::scoped_lock sl(lockable);

      if (valueToSet != value)
      {
          will_change(valueToSet);
          value = valueToSet;
          on_change(value);
      }
  }

  // sets the Property to a new value. 
  // will_change and on_change will not be emitted
  void setwithnoemit(T const& valueToSet) 
  {
      std::scoped_lock sl(lockable);
      value = valueToSet;
  }

  // sets the Property to a given value. 
  // will_change and on_change will always be emitted.
  void setandemit(T const& valueToSet)
  {
      std::scoped_lock sl(lockable);
      will_change(valueToSet);
      value = valueToSet;
      on_change(value);
  }

  // emits WillChange and OnChange
  // even if the value did not change
  void touch() 
  {
    will_change(value);
    on_change  (value);
  }

  // returns the internal value
  T const& get() const 
  { 
      return value;
  }

  // if there are any Properties connected to this Property,
  // they won't be notified of any further changes
  void disconnect_all() 
  {
      on_change  .disconnect_all();
      will_change.disconnect_all();
  }

  // assigns the value of another Property
  Property_Base<Lockable, T>& operator=(Property_Base<Lockable, T> const& rhs)
  {
    set(rhs.value);
    return *this;
  }

  // assigns a new value to this Property
  Property_Base<Lockable, T>& operator=(T const& rhs)
  {
    set(rhs);
    return *this;
  }

  // compares the values of two Properties
  bool operator==(Property_Base<Lockable, T> const& rhs) const
  {
    return Property_Base<Lockable, T>::get() == rhs.get();
  }

  bool operator!=(Property_Base<Lockable, T> const& rhs) const
  {
    return Property_Base<Lockable, T>::get() != rhs.get();
  }

  // compares the values of the Property to another value
  bool operator==(T const& rhs) const { return Property_Base<Lockable, T>::get() == rhs; }
  bool operator!=(T const& rhs) const { return Property_Base<Lockable, T>::get() != rhs; }

  // returns the value of this Property
  operator const ValueType &()  const { return Property_Base<Lockable, T>::get(); }
  const ValueType &operator->() const { return Property_Base<Lockable, T>::get(); }
  ValueType const& operator()() const { return Property_Base<Lockable, T>::get(); }

  Signal_Base<Lockable,T> on_change;
  Signal_Base<Lockable,T> will_change;

 private:
  Lockable  lockable;
  ValueType value;
};

/**
 * Specialization of Property2_Base to be used in single threaded contexts.
 * Slot connection, disconnection and signal emission are not thread-safe.
 * The performance improvement over the thread-safe variant is not impressive,
 * so this is not very useful.
 */
template <typename... T>
using Property_ST = Property_Base<sigslot::detail::null_mutex, T...>;

/**
 * Specialization of Property_Base to be used in multi-threaded contexts.
 * Slot connection, disconnection and signal emission are thread-safe.
 *
 * Recursive signal emission and emission cycles are supported too.
 */
template <typename... T>
using Property = Property_Base<std::recursive_mutex, T...>;

// Chains two properties
template <typename LockableSrc, typename LockableDst, typename T>
void chain(  Property_Base<LockableSrc, T>       &src
           , Property_Base<LockableDst, T>       &dst
           , std::vector<SignalScopedConnection> *connections = nullptr)
{
    dst = src;
    if (connections)
    {
        auto connection = dst.on_change.connect(&Property_Base<LockableSrc, T>::setandemit, &src);
        connections->emplace_back(std::move(connection));
        connection      = src.on_change.connect(&Property_Base<LockableDst, T>::set       , &dst);
        connections->emplace_back(std::move(connection));
    }
    else
    {
        dst.on_change.connect(&Property_Base<LockableSrc, T>::setandemit, &src);
        src.on_change.connect(&Property_Base<LockableDst, T>::set       , &dst);
    }
}

// Unchains two properties
template <typename LockableSrc, typename LockableDst, typename T>
void unchain(  Property_Base<LockableSrc, T> &src
             , Property_Base<LockableDst, T> &dst)
{
    dst.on_change.disconnect(&Property_Base<LockableSrc, T>::setandemit, &src);
    src.on_change.disconnect(&Property_Base<LockableDst, T>::set       , &dst);
}

// Connects dst property with src
template <typename Lockable, typename T>
void connectfrom(  Property_Base<Lockable, T>   &src
	             , Property_Base<Lockable, T> &dst)
{
	dst = src;
	src.on_change.connect(&Property_Base<Lockable, T>::setandemit, &dst);
}

// Connects dst property with src
template <typename Lockable, typename T>
SignalScopedConnection connectfromscoped(  Property_Base<Lockable, T>& src
                                         , Property_Base<Lockable, T>& dst)
{
    dst = src;
    return src.on_change.connect(&Property_Base<Lockable, T>::setandemit, &dst);
}

// Disconnects dst property with src
template <typename Lockable, typename T>
void disconnectfrom(  Property_Base<Lockable, T> &src
	                , Property_Base<Lockable, T> &dst)
{
	src.on_change.disconnect(&Property_Base<Lockable, T>::setandemit, &dst);
}