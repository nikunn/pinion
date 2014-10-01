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

// Posix compliant source
#define _POSIX_SOURCE 1

// Initialize the handle
int AsynchLinux::handle = 0;

//================================ AsynchLinux =================================
// Opens the given serial device in asynchronous mode
int AsynchLinux::asynchOpen(const std::string& device_name, const long baud)
{
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
  saio.sa_handler = AsynchLinux::onCallback;
  sigemptyset(&(saio.sa_mask));
  saio.sa_flags = 0;
  saio.sa_restorer = NULL;
  sigaction(SIGIO, &saio, NULL);

  // Allow the process to receive SIGIO
  fcntl(handle, F_SETOWN, getpid());

  // Make the file descriptor asynchronous (the manual page says only
  // O_APPEND and O_NONBLOCK, will work with F_SETFL...)
  fcntl(handle, F_SETFL, FASYNC);

  // Get the configuration
  if(tcgetattr(handle, &config) < 0)
  {
    FATAL_PF("Could not get the serial configuration for device %s", device_name.c_str());
  }

  // Set port settings for canonical input processing
  config.c_cflag = CRTSCTS | CS8 | CLOCAL | CREAD;
  config.c_iflag = IGNPAR | ICRNL;
  config.c_oflag = 0;
  config.c_lflag = ICANON;
  config.c_cc[VMIN]=1;
  config.c_cc[VTIME]=0;

  // Define the baud speed
  speed_t baud_speed;

  switch (baud)
  {
    case     50: baud_speed =     B50 ; break ;
    case     75: baud_speed =     B75 ; break ;
    case    110: baud_speed =    B110 ; break ;
    case    134: baud_speed =    B134 ; break ;
    case    150: baud_speed =    B150 ; break ;
    case    200: baud_speed =    B200 ; break ;
    case    300: baud_speed =    B300 ; break ;
    case    600: baud_speed =    B600 ; break ;
    case   1200: baud_speed =   B1200 ; break ;
    case   1800: baud_speed =   B1800 ; break ;
    case   2400: baud_speed =   B2400 ; break ;
    case   4800: baud_speed =   B4800 ; break ;
    case   9600: baud_speed =   B9600 ; break ;
    case  19200: baud_speed =  B19200 ; break ;
    case  38400: baud_speed =  B38400 ; break ;
    case  57600: baud_speed =  B57600 ; break ;
    case 115200: baud_speed = B115200 ; break ;
    case 230400: baud_speed = B230400 ; break ;

    default:
      FATAL_PF("The serial baud rate is not compatible for device %s", device_name.c_str());
  }

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
void AsynchLinux::onCallback(int status)
{
  // Define the buffer we will read data to
  char buff[LINUX_ASYNCH_PACKET_SIZE];

  // Read the serial buffer
  int result = read(AsynchLinux::handle, buff, LINUX_ASYNCH_PACKET_SIZE);

  // Override the last character to null
  buff[result]=0;

  // Print the buffer
  INFO_PF("Asynch buffer:%s num:%u\n", buff, result);
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

