#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Framework/Logger.h"
#include "Framework/StopWatch.h"
#include "Tools/UART.h"


namespace pno
{
//================================= UartLinux ==================================
void UartLinux::uartInit(const int handle, const long baud)
{
  // Check if it really is a UART device
  if(!isatty(handle))
  {
    FATAL_PF("Device %u is not a UART device", handle);
  }

  // Apply the UART configuration for this device
  uartConfig(handle);

  // Change the baud rate
  changeBaud(handle, baud);
}

// Apply a termios configuration to an UART handle
void UartLinux::applyConfig(const int handle, const termios& config)
{
  // Flush the input queue
  tcflush(handle, TCIFLUSH);

  // Finally, apply the configuration
  if(tcsetattr(handle, TCSANOW, &config) < 0)
  {
    FATAL_PF("Could not set the UART configuration for handle %u", handle);
  }
}

// Configure the given handle device
void UartLinux::uartConfig(const int handle)
{
  // Define the UART option
  termios config;

  // Get the configuration
  if(tcgetattr(handle, &config) < 0)
  {
    FATAL_PF("Could not get the serial configuration for handle %u", handle);
  }

  // Set port settings for canonical input processing
  config.c_cflag = CRTSCTS | CS8 | CLOCAL | CREAD;
  config.c_iflag = IGNPAR | ICRNL | IGNCR;
  config.c_oflag = 0;
  config.c_lflag = ICANON;
  config.c_cc[VMIN]=1;
  config.c_cc[VTIME]=0;

  // Apply configuration
  applyConfig(handle, config);
}

// Change the baud rate of a serial device
void UartLinux::changeBaud(const int handle, const long baud)
{
  // A bit of log
  INFO_PF("Changing to baud %u for the UART on handle %u", baud, handle );

  // Define the UART option
  termios config;

  // Get the configuration
  if(tcgetattr(handle, &config) < 0)
  {
    FATAL_PF("Could not get the UART configuration for handle %u", handle);
  }

  // Define the baud speed
  speed_t baud_speed = longToBaud(baud);

  // Baud rate
  if(cfsetispeed(&config, baud_speed) < 0 || cfsetospeed(&config, baud_speed) < 0)
  {
    FATAL_PF("Could not set the UART baud rate for handle %u", handle);
  }

  // Apply configuration
  applyConfig(handle, config);

  // Flush the input and output queue
  tcflush(handle, TCIOFLUSH);
}

// Write to UART
void UartLinux::uartWrite(const int handle, const UartPacket& pkt)
{
  // Benchmark UART write actions.
  BENCH_SCOPE("UART WRITE");

  // Get the message
  const std::string& message = pkt.message;

  // Write the packet content to the UART
  int bytes_write = write(handle, message.c_str(), message.length());
}

// Read from UART
UartPacket UartLinux::uartRead(const int handle)
{
  // Benchmark UART event actions.
  BENCH_SCOPE("UART READ");

  // Define the buffer we will read data to
  char buff[UART_PACKET_SIZE];

  // Read the buffer
  int bytes_read = read(handle, buff, UART_PACKET_SIZE);

  // Check the number of bytes read
  if (bytes_read < 0 || bytes_read > UART_PACKET_SIZE)
  {
    // Print an error log
    ERROR_PF("Could not read UART buffer on handle %u", handle);
  }

  // Make the buffer null terminated
  buff[bytes_read]=0;

  // Return the result
  return UartPacket(buff, 0);
}

// Get the baud rate in termios format from long
speed_t UartLinux::longToBaud(const long baud)
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
      FATAL_PF("The UART baud rate %u is not compatible", baud);
  }
}


}
