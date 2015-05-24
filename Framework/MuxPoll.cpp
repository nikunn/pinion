#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include "Framework/Logger.h"
#include "Framework/StopWatch.h"
#include "Framework/MuxPoll.h"

//================================== MuxPoll ===================================

// Return the epoll instance handle
int& MuxPoll::getHandle()
{
  // Initialize the epoll instance handle
  static int _epoll_handle = -1;

  // Return
  return _epoll_handle;
}

// Boolean defining if we should be listening
bool& MuxPoll::getStatus()
{
  // Declaration of static boolean is running
  static bool _status = false;

  // Return running flag
  return _status;
}

// Initialize the epoll instance
void MuxPoll::initialize()
{
  // Initialize the epoll instance handle
  getHandle() = epoll_create(MUX_POLL_MONITOR_NB);

  // Check for error
  if (getHandle() == -1)
  {
    FATAL_LG("Cannot create an instance of epoll");
  }
  else
  {
    // A bit of log
    INFO_PF("Created an instance of epoll with file descriptor %u", getHandle());
  }

  // Change the running status to 'true'
  getStatus() = true;
}

// Listen in loop
void MuxPoll::start()
{
  // Check the status
  while(getStatus())
  {
    // Listen for events
    _listen();
  }
}

// Close the epoll instance
void MuxPoll::stop()
{
  // Change the runnig status to 'false'
  getStatus() = false;
}

// Add the given file descriptor to list of interest
void MuxPoll::add(const int fd)
{
  // Initialize the epoll event structure
  struct epoll_event ev;

  // Populate information to be given back on ready
  ev.data.fd = fd;

  // Specify the set of events we are interested in
  ev.events = EPOLLIN;

  // add this handle to the list of interest
  if (epoll_ctl(getHandle(), EPOLL_CTL_ADD, fd, &ev) == -1)
  {
    // A bit of log
    ERROR_PF("Cannot add handle %u to epoll", fd);
  }
  else
  {
    // A bit of log
    INFO_PF("Adding file descriptor %u to epoll instance's files of interest", fd);
  }
}

// Function called when we received a new signal
void MuxPoll::_listen()
{
  // Allocate an array to store the events
  struct epoll_event ev_list[MUX_POLL_MAX_EVENTS];

  // Listen for new events
  const int ready_nb = epoll_wait(getHandle(), ev_list, MUX_POLL_MAX_EVENTS, -1);

  // Check number of events
  if (ready_nb == -1)
  {
    // A bit of log
    INFO_PF("Epoll %u listen did not yield to any events", getHandle());

    // Exit here
    return;
  }

  // Go through each event
  for (int i = 0; i < ready_nb; i++)
  {
    // Check if we have some input data
    if (ev_list[i].events & EPOLLIN)
    {
      // Dispatch this event to the listeners
      PollDispatcher::dispatch(ev_list[i].data.fd, PollEvent());
    }
  }
}
