#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Framework/Logger.h"
#include "Tools/Asynch.h"

// Initialize the handle
int AsynchLinux::handle = 0;

//================================ AsynchLinux =================================
// Opens the given serial device in asynchronous mode
int AsynchLinux::asynchOpen(const std::string& device_name, const long baud)
{
  // Some logs
  INFO_PF("Opening serial device %s in asynch mode", device_name.c_str());

  // Define the connection option
  termios config;

  // Open the serial device
  int handle = open(device_name.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

  // Check the result
  if(handle < 0)
  {
    FATAL_PF("Could not open connection to serial device %s", device_name.c_str());
  }

  // Check if it really is a serial device
  if(!isatty(handle))
  {
    FATAL_PF("Device %s is not a serial device", device_name.c_str());
  }

  // Definition of signal action
  struct sigaction saio;

  // Configure the signal handler before making the device asynchronous
  saio.sa_sigaction = AsynchLinux::onCallback;
  sigemptyset(&(saio.sa_mask));
  saio.sa_flags = 0;
  saio.sa_restorer = NULL;

  // Tells Sigaction to add sig_info_t *info and void *context to the callback signature
  saio.sa_flags = SA_SIGINFO;

  // Call to sigaction to allow the callback on IO signal
  if(sigaction(SIGIO, &saio, NULL) < 0)
  {
    FATAL_PF("Could not register action to %s IO signals", device_name.c_str());
  }

  // Allow the process to receive SIGIO (socket owner)
  if(fcntl(handle, F_SETOWN, getpid()) < 0)
  {
    FATAL_PF("Could not make process be socket owner for %s IO signals", device_name.c_str());
  }

  // Make Linux populate si_fd and si_band in the siginfo struct
  if(fcntl(handle, F_SETSIG, SIGIO) < 0)
  {
    FATAL_PF("Could not make the device %s ", device_name.c_str());
  }

  // Make the file descriptor asynchronous (the manual page says only
  // O_APPEND and O_NONBLOCK, will work with F_SETFL...)
  if(fcntl(handle, F_SETFL, FASYNC) < 0)
  {
    FATAL_PF("Could not make the device %s handle asynchronous", device_name.c_str());
  }

  // Get the configuration
  if(tcgetattr(handle, &config) < 0)
  {
    FATAL_PF("Could not get the serial configuration for device %s", device_name.c_str());
  }

  // Set port settings for canonical input processing
  config.c_cflag = CRTSCTS | CS8 | CLOCAL | CREAD;
  config.c_iflag = IGNPAR | ICRNL | IGNCR;
  config.c_oflag = 0;
  config.c_lflag = ICANON;
  config.c_cc[VMIN]=1;
  config.c_cc[VTIME]=0;

  // Define the baud speed
  speed_t baud_speed = longToBaud(baud);

  // Baud rate
  if(cfsetispeed(&config, baud_speed) < 0 || cfsetospeed(&config, baud_speed) < 0)
  {
    FATAL_PF("Could not set the serial baud rate for device %s", device_name.c_str());
  }

  // Flush
  tcflush(handle, TCIFLUSH);

  // Finally, apply the configuration
  if(tcsetattr(handle, TCSANOW, &config) < 0)
  {
    FATAL_PF("Could not set the serial configuration for device %s", device_name.c_str());
  }

  // Put the static handle
  AsynchLinux::handle = handle;

  // Return the handle
  return handle;
}

// Close a serial device
void AsynchLinux::asynchClose(const int handle)
{
  // Close the connection to the given handle
  int err = close(handle);

  // Check
  if (err < 0) { ERROR_PF("Could not close connection to serial device on handle %u", handle); }
}

// Function called when we received a new packet
void AsynchLinux::onCallback(int signo, siginfo_t* info, void*)
{
  // Define the buffer we will read data to
  char buff[LINUX_ASYNCH_PACKET_SIZE];

  // Read the serial buffer
  int result = read(AsynchLinux::handle, buff, LINUX_ASYNCH_PACKET_SIZE);

  if (result > 3)
  {
    // Override the last character to null
    buff[result-2]=0;

    // Print the buffer
    INFO_PF("Asynch handle:%u buffer:%s", info->si_fd, buff);
  }
  else
  {
    INFO_LG("Incomplete packet");
  }
}

// Get the baud rate in termios format from long
speed_t AsynchLinux::longToBaud(const long baud)
{
  switch (baud)
  {
    case     50: return     B50 ; break ;
    case     75: return     B75 ; break ;
    case    110: return    B110 ; break ;
    case    134: return    B134 ; break ;
    case    150: return    B150 ; break ;
    case    200: return    B200 ; break ;
    case    300: return    B300 ; break ;
    case    600: return    B600 ; break ;
    case   1200: return   B1200 ; break ;
    case   1800: return   B1800 ; break ;
    case   2400: return   B2400 ; break ;
    case   4800: return   B4800 ; break ;
    case   9600: return   B9600 ; break ;
    case  19200: return  B19200 ; break ;
    case  38400: return  B38400 ; break ;
    case  57600: return  B57600 ; break ;
    case 115200: return B115200 ; break ;
    case 230400: return B230400 ; break ;

    default:
      FATAL_PF("The serial baud rate %u is not compatible", baud);
  }
}

// Function returning the number of byte in the serial buffer
int AsynchLinux::bytesAvailable(const int handle)
{
  // Inititialize the number of available bytes
  int result;

  // Request the number of bytes ready
  int err = ioctl(handle, FIONREAD, &result);

  // Check for errors
  if (err < 0) { ERROR_PF("Could not get the number of available byte on handle %u", handle); }

  // Return the result
  return result ;
}

