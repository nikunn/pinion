#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Framework/Signal.h"
#include "Framework/Logger.h"
#include "Framework/StopWatch.h"

//================================= SignalMux ==================================

// Configure the given handle device in asynchronous mode
void SignalMux::add(const int handle)
{
  // Definition of signal action
  struct sigaction saio;

  // Configure the signal handler before making the device asynchronous
  saio.sa_sigaction = onEvent;
  sigemptyset(&(saio.sa_mask));
  saio.sa_flags = 0;
  saio.sa_restorer = NULL;

  // Tells Sigaction to add sig_info_t *info and void *context to the callback signature
  saio.sa_flags = SA_SIGINFO;

  // Call to sigaction to allow the callback on IO signal
  if(sigaction(SIGIO, &saio, NULL) < 0)
  {
    FATAL_PF("Could not register action to handle %u IO signals", handle);
  }

  // Allow the process to receive SIGIO (socket owner)
  if(fcntl(handle, F_SETOWN, getpid()) < 0)
  {
    FATAL_PF("Could not make process be socket owner for handle %u IO signals", handle);
  }

  // Make Linux populate si_fd and si_band in the siginfo struct
  if(fcntl(handle, F_SETSIG, SIGIO) < 0)
  {
    FATAL_PF("Could not make the handle %u callback informations available", handle);
  }

  // Make the file descriptor asynchronous (the manual page says only
  // O_APPEND and O_NONBLOCK, will work with F_SETFL...)
  if(fcntl(handle, F_SETFL, FASYNC) < 0)
  {
    FATAL_PF("Could not make the handle %u asynchronous", handle);
  }
}

// Function called when we received a new signal
void SignalMux::onEvent(int signo, siginfo_t* info, void*)
{
  // Benchmark UART event actions.
  BENCH_SCOPE("SIGNAL EVENT");

  // Dispatch this event to the listeners
  SignalDispatcher::dispatch(info->si_fd, SignalEvent(info->si_signo, info->si_code, info->si_band));
}
