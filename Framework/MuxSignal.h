#ifndef _MUX_SIGNAL_H
#define _MUX_SIGNAL_H

// Posix compliant source
#define _POSIX_SOURCE 1

#include <sys/signal.h>

#include "Framework/Dispatcher.h"


namespace pno
{
//====================== Sugnal Dispatch Listener Event ========================
class SignalEvent : Event
{
public :
  SignalEvent() {};
  SignalEvent(int no, int code, long band) : no(no), code(code), band(band) {};

  int no;
  int code;
  long band;
};

using SignalListener = Listener<SignalEvent>;
using SignalDispatcher = Dispatcher<SignalEvent>;


//================================= MuxSignal ==================================
class MuxSignal
{
public:

  //============================ public functions ============================

  // Add a callback for some handle events
  static void add(const int handle);

private:

  //=========================== private functions ============================

  // Callback for signals
  static void onEvent(int signo, siginfo_t* info, void*);
};


}
#endif
