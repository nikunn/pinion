#ifndef _MUX_TIMER_H
#define _MUX_TIMER_H

#include <set>
#include <vector>

#include "Framework/Clock.h"
#include "Framework/LuaBind.h"
#include "Framework/Dispatcher.h"


namespace pno
{
//======================= Timer Dispatch Listener Event ========================
// Timer Event class.
class TimerEvent : Event
{
public :
  TimerEvent() {};
};

// Forward declaration of class timer.
class MuxTimer;

// Timer Dispatcher class.
using TimerDispatcher = Dispatcher<TimerEvent>;

// Timer Listener class.
class TimerListener : public Listener<TimerEvent>
{
  public:

  // Forbid the use of default constructor.
  TimerListener() = delete;

  // Constructor based on a config block.
  TimerListener(const LuaTable&);
};


//================================== MuxTimer ==================================
class MuxTimer
{
private:

  //=============================== Clock ================================

  using Clock = ClockGen<std::chrono::steady_clock>;
  using TimePoint = Clock::TimePoint;
  using TimeSpan = Clock::TimeSpan;

public:

  //=============================== Channel ==============================

  struct Channel
  {
    Channel(int id, long period) : id(id), period(period) {};

    int id;
    long period;
  };

  //============================== CallBack ==============================

  // CallBack, contains the information about the callback
  struct CallBack
  {
    CallBack(int id, TimePoint time) : id(id), time(time) {};

    friend bool operator< (CallBack const& lhs, CallBack const& rhs)
    {
      return lhs.time < rhs.time;
    }

    friend bool operator== (CallBack const& lhs, CallBack const& rhs)
    {
      return lhs.time == rhs.time;
    }

    int id;
    TimePoint time;
  };

  //============================== Typedefs ==============================

  // Define the container holding the all channel information
  using ChannelContainer = std::vector<Channel>;

  // Define the container holding the callback time and channel id
  using CallBackContainer = std::multiset<CallBack>;

public:

  //============================ public functions ============================

  // Constructor
  MuxTimer();

  // Define a () operator to start a timer in a new thread
  void operator() () { start(); }

  // Add a timer with a given period in milliseconds and return channel id
  static int add_channel(long period);

  // Start the timer
  static void start();

  // Stop the timer
  static void stop();

private:

  //=========================== private functions ============================

  // Initialize the timer
  static void _initialize();

  // wait for next callback
  static void _wait();

  // Process the callback
  static void _callback();


private:

  //============================ private members =============================


  // Boolean defining if timer is currently running
  static bool& getStatus();

  // Container holding all channel informations
  static ChannelContainer& getChannels();

  // Container holding the future callback time for each period
  static CallBackContainer& getCallBacks();
};


}
#endif
