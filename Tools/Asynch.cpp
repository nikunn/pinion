#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "Framework/Logger.h"
#include "Tools/Asynch.h"

int AsynchLinux::asynchOpen(const std::string& device_name, const long baud)
{
  // Define the connection option
  termios config;

  // Open the serial device
  int handle = open(device_name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

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

  // Get the configuration
  if(tcgetattr(handle, &config) < 0)
  {
    FATAL_PF("Could not get the serial configuration for device %s", device_name.c_str());
  }

  // Input flags - Turn off input processing
  // convert break to null byte, no CR to NL translation,
  // no NL to CR translation, don't mark parity errors or breaks
  // no input parity check, don't strip high bit off,
  // no XON/XOFF software flow control
  config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);

  // Output flags - Turn off output processing
  // no CR to NL translation, no NL to CR-NL translation,
  // no NL to CR translation, no column 0 CR suppression,
  // no Ctrl-D suppression, no fill characters, no case mapping,
  // no local output processing
  // config.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
  //                     ONOCR | ONOEOT| OFILL | OLCUC | OPOST);
  config.c_oflag &= ~OPOST ;

  // No line processing:
  // echo off, echo newline off, canonical mode off, 
  // extended input processing off, signal chars off
  config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

  // Turn off character processing
  // clear current char size mask, no parity checking, one stop bit
  // no output processing, force 8 bit input
  config.c_cflag &= ~(CSIZE | PARENB | CSTOPB);
  config.c_cflag |= CS8;

  // One input byte is enough to return from read()
  // Inter-character timer off
  config.c_cc[VMIN]  = 0;
  config.c_cc[VTIME] = 0;

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

  // Finally, apply the configuration
  if(tcsetattr(handle, TCSAFLUSH, &config) < 0)
  {
    FATAL_PF("Could not set the serial configuration for device %s", device_name.c_str());
  }

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

int bytesAvailable(const int handle)
{
  // Inititialize the number of available bytes
  int result;

  // Request 
  int err = ioctl(handle, FIONREAD, &result);

  // Check for errors
  if (err < 0) { ERROR_PF("Could not get the number of available byte on handle %u", handle); }

  // Return the result
  return result ;
}

