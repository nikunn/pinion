#ifndef _MUX_POLL_H
#define _MUX_POLL_H

#define MUX_POLL_MONITOR_NB 8
#define MUX_POLL_MAX_EVENTS 8

#include "Framework/Dispatcher.h"


namespace pno
{
//======================= Poll Dispatch Listener Event =========================
class PollEvent : Event
{
public :
  PollEvent() {};
};

using PollListener = Listener<PollEvent>;
using PollDispatcher = Dispatcher<PollEvent>;


//================================== MuxPoll ===================================
class MuxPoll
{
public:

  //============================ public functions ============================

  // Start an instance of epoll
  static void initialize();

  // Listen in loop while status is set to running
  static void start();

  // Stop listening
  static void stop();

  // Add a watcher for some handle
  static void add(const int handle, const uint32_t event_mask);

private:

  //=========================== private functions ============================

  // Listen for events
  static void _listen();

private:

  //============================ private members =============================

  // Boolean defining if we should be listening
  static bool& getStatus();

  // Get handle to epoll instance
  static int& getHandle();
};


}
#endif
