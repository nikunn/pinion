#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Framework/Logger.h"
#include "Tools/Uart.h"


//================================== UartCom ===================================
UartCom::Type UartCom::parse(const std::string& com)
{
  if (com == "serial") { return UartCom::SERIAL; }
  else if(com == "asynch") { return UartCom::ASYNCH; }
  else { FATAL_PF("The UART communication type %s is not reconized", com.c_str()); }
}


//================================= UartLinux ==================================
int UartLinux::uartOpen(const std::string& device_name, const long baud, const UartCom::Type com)
{
  // Open the UART device
  int handle = open(device_name.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

  // Check the result
  if(handle < 0)
  {
    FATAL_PF("Could not open connection to UART device %s", device_name.c_str());
  }

  // Check if it really is a UART device
  if(!isatty(handle))
  {
    FATAL_PF("Device %s is not a UART device", device_name.c_str());
  }

  // Apply the right configuration for this communication type
  switch (com)
  {
    case UartCom::ASYNCH: asynchConfig(handle); break ;
    case UartCom::SERIAL: serialConfig(handle); break ;

    default:
      FATAL_LG("The chosen UART communication type is not reconized");
  }

  // Change the baud rate
  changeBaud(handle, baud);

  // Return the handle
  return handle;
}

// Apply a termios configuration to an UART handle
void UartLinux::applyConfig(const int handle, const termios& config)
{
  // Flush
  tcflush(handle, TCIFLUSH);

  // Finally, apply the configuration
  if(tcsetattr(handle, TCSANOW, &config) < 0)
  {
    FATAL_PF("Could not set the UART configuration for handle %u", handle);
  }
}

// Configure the given handle device in serial mode
void UartLinux::serialConfig(const int handle)
{
}

// Configure the given handle device in asynchronous mode
void UartLinux::asynchConfig(const int handle)
{
  // Definition of signal action
  struct sigaction saio;

  // Configure the signal handler before making the device asynchronous
  saio.sa_sigaction = UartLinux::onEvent;
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
}

// Close a UART device
void UartLinux::uartClose(const int handle)
{
  // A bit of log
  INFO_PF("Closing UART connection on handle %u", handle );

  // Close the connection to the given handle
  if (close(handle) < 0)
  {
    ERROR_PF("Could not close connection to UART device on handle %u", handle);
  }
}

// Write to UART
void UartLinux::uartWrite(const int handle, const UartPacket& packet)
{
  // Get the packet content
  const std::string& message = packet.message;

  // Write the packet content to the UART
  int bytes_write = write(handle, message.c_str(), message.length());
}

// Read from UART
void UartLinux::uartRead(const int handle, byte* data, int& bytes_num)
{
}

// Function called when we received a new packet
void UartLinux::onEvent(int signo, siginfo_t* info, void*)
{
  // Define the buffer we will read data to
  char buff[UART_PACKET_SIZE];

  // Get the handle from the signal info
  const int handle = info->si_fd;

  // Read the buffer
  int bytes_read = read(handle, buff, UART_PACKET_SIZE);

  // Check the number of bytes read
  if (bytes_read < 0 || bytes_read > UART_PACKET_SIZE)
  {
    // Print an error log
    ERROR_PF("Could not read UART buffer on handle %u", handle);

    // Exit the function
    return;
  }

  // Make the buffer null terminated
  buff[bytes_read]=0;

  // Get the packetId
  long id = 0;//getPacketId(handle);

  // Create a packet from this buffer and id
  const UartPacket evt(buff, id);

  // Dispatch this event to the listeners
  AsynchDispatcher::dispatch(handle, evt);
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

// Function returning the number of byte in the UART buffer
int UartLinux::bytesAvailable(const int handle)
{
  // Inititialize the number of available bytes
  int result;

  // Check for errors
  if (ioctl(handle, FIONREAD, &result) < 0)
  {
    ERROR_PF("Could not get the number of available byte on handle %u", handle);
  }

  // Return the result
  return result ;
}

