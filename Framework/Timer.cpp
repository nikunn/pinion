#include <thread>

#include "Framework/Timer.h"
#include "Framework/Logger.h"

//=================================== Timer ====================================

// Timer constructor
Timer::Timer()
{
  // Initialize the running flag to false
  getStatus() = false;

  // Bit of log
  INFO_PF("Timer clock has a resolution of %f", Clock::resolution());
}

// Return static container of all channels
bool& Timer::getStatus()
{
  // Declaration of static boolean is running
  static bool _status;

  // Return running flag
  return _status;
}

// Return static container of all channels
Timer::ChannelContainer& Timer::getChannels()
{
  // Declaration of static class container
  static ChannelContainer _channels;

  // Return static class container
  return _channels;
}

// Return static container of callbacks
Timer::CallBackContainer& Timer::getCallBacks()
{
  // Declaration of static class container
  static CallBackContainer _callbacks;

  // Return static class container
  return _callbacks;
}

// Add a channel to the timer with a given period
int Timer::add_channel(long period)
{
  // Check if this period is already registered
  for (Channel& channel : getChannels())
  {
    if (channel.period == period)
    {
      // Timer fot his period has already been registered, return the associated id
      INFO_PF("Timer channel with %u ms period already registered", period);

      // Return the associated id
      return channel.id;
    }
  }

  // Check if the timer is not currently running
  if (getStatus())
  {
    // Dump  a log
    ERROR_LG("Timer cannot add new channel in timer, currently running");

    // Return negative result
    return -1;
  }
  else
  {
    // Dump  a log
    INFO_PF("Timer, add new channel to timer with %u ms period", period);

    // Get the new id
    int id = getChannels().size() + 1;

    // Add a new channel to this timer
    getChannels().push_back(Channel(id, period));

    // Return new channel id
    return id;
  }
}

void Timer::stop()
{
  // A bit of log
  INFO_LG("Timer Stop");

  // Change the running flag to false
  getStatus() = false;
}

void Timer::_initialize()
{
  // Change the running flag to true
  getStatus() = true;

  // Get the current time
  TimePoint current_time = Clock::now();

  // Go through each channel
  for (Channel& channel : getChannels())
  {
    // Compute this channel next callback time
    TimePoint callback_time = current_time + MilliSeconds(channel.period);

    // Insert this callback in the container
    getCallBacks().insert(CallBack(channel.id, callback_time));
  }
}

void Timer::start()
{
  // A bit of log
  INFO_LG("Timer Start");

  // Call the initialization
  _initialize();

  // Main timer loop
  while(getStatus())
  {
    // Check that there is a next callback
    if (getCallBacks().size() > 0)
    {
      // Wait until the next callback
      _wait();

      // Process the callback
      _callback();
    }
    else
    {
      // Change the running flag to false
      getStatus() = false;
    }
  }
}

// Wait function
void Timer::_wait()
{
  // Get the next callback information
  const auto callback = getCallBacks().cbegin();

  // Get the callback time
  TimePoint callback_time = callback->time;

  // Get the current time
  TimePoint current_time = Clock::now();

  // Compute the difference of current and callback time
  TimeSpan diff = current_time - callback_time;

  // Check that the callback is in the future
  if (callback_time > current_time)
  {
    // Sleep until the next callback
    std::this_thread::sleep_until(callback_time);
  }
  else if (diff > MilliSeconds(1))
  {
    WARNING_PF("Timer is running late by %u ms", diff.as<MilliSeconds>());
  }
}

// Process callback and call dispatcher
void Timer::_callback()
{
  // Define a vector containing all new callbacks
  std::vector<CallBack> new_callbacks;

  // Get the current callback information
  auto callback = getCallBacks().begin();

  // Get the callback time
  TimePoint callback_time = callback->time;

  // Get the current time
  TimePoint current_time = Clock::now();

  // Create a timer event for this callback
  const TimerEvent evt;

  // Go through each channel associated with this callback
  auto range = getCallBacks().equal_range(*callback);
  for (auto it = range.first; it != range.second; ++it)
  {
    // Get the channel id
    int id = callback->id;

    // Call the dispatcher for this channel
    TimerDispatcher::dispatch(id, evt);

    // Get the period for this channel
    long period = getChannels()[id - 1].period;

    // Compute this channel next callback time
    TimePoint next_time = current_time + MilliSeconds(period);

    // Push this future callback in the vector
    new_callbacks.push_back(CallBack(id, next_time));
  }

  // Erase already processed callbacks
  getCallBacks().erase(*callback);

  // Push all future callback to the callback container
  for (CallBack cb : new_callbacks)
  {
    // Insert the next callback for this channel
    getCallBacks().insert(cb);
  }
}

//=============================== TimerListener ================================
// Constructor.
TimerListener::TimerListener(const LuaTable& cfg)
{
  // Get the period from the config.
  long period = cfg.get<long>("period_ms");

  // Add a new channel with the given period.
  int channel = Timer::add_channel(period);

  // Register to this channel.
  TimerDispatcher::registerListener(channel, static_cast<Listener<TimerEvent>*>(this));
}
