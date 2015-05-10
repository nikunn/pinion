#ifndef _DISPATCHER_H
#define _DISPATCHER_H

#include<map>


//================================== Event =====================================
class Event
{
};


//================================= Listener ===================================
template<typename EVT>
class Listener
{
  public:
    virtual ~Listener() {}
    virtual void onEvent(const EVT&) = 0;
};


//================================ Dispatcher ==================================
template<typename EVT>
class Dispatcher
{
public :  //======================= Typedefs =====================

  using ListenerContainer = std::multimap<int, Listener<EVT>*>;

public :  //======================== Public ======================

  static void dispatch(int channel, const EVT& event)
  {
    // Iterates over all listener that are registered to this channel
    auto range = getListenerTable().equal_range(channel);
    for (auto it = range.first; it != range.second; ++it)
    {
      // Call the on event method for this listener
      it->second->onEvent(event);
    }
  }

  static void registerListener(int channel, Listener<EVT>* listener)
  {
    // Register this listener for this channel
    if (!isRegistered(channel, listener))
    {
      getListenerTable().insert(std::make_pair(channel, listener));
    }
  }

private : //======================= Private ======================

  static ListenerContainer& getListenerTable()
  {
    // Declaration of static class container
    static ListenerContainer _listener_table;

    // Return static class container
    return _listener_table;
  }

  static bool isRegistered(int channel, const Listener<EVT>* listener)
  {
    // Iterates over all listener that are registered to this channel
    auto range = getListenerTable().equal_range(channel);
    for (auto it = range.first; it != range.second; ++it)
    {
      // Check if this listener is already registered on this channel
      if (it->second == listener) { return true; }
    }

    //
    return false;
  }
};


#endif

