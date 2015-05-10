#ifndef _ASYNCH_LINUX_H
#define _ASYNCH_LINUX_H

#include <string>
#include <termios.h>
#include <sys/signal.h>

#include "Framework/Types.h"
#include "Framework/Dispatcher.h"

// Define the max buffer size and packet size
#define UART_BUFFER_SIZE 2048 // have to be smaller than 2048 for LabJack use
#define UART_PACKET_SIZE 248

//================================ UartPacket ==================================
class UartPacket
{
public :
  UartPacket() {};
  UartPacket(const char* msg, long no = 0) : id(no), message(msg) {};

  long id;
  std::string message;
};

//================================ UartLinux ===================================
class UartLinux
{
public :  //======================== Public ======================

  static int uartOpen(const std::string& device_name, const long baud);
  static void uartClose(const int handle);

  static void uartWrite(const int handle, const UartPacket&);
  static UartPacket uartRead(const int handle);

  static void changeBaud(const int handle, const long baud);
  static speed_t longToBaud(const long baud);

private : //======================= Private ======================

  static void uartConfig(const int handle);
  static void applyConfig(const int handle, const termios& config);
};


#endif
