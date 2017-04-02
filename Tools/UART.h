#ifndef _ASYNCH_LINUX_H
#define _ASYNCH_LINUX_H

#include <string>
#include <termios.h>
#include <sys/signal.h>

#include "Framework/Types.h"
#include "Framework/Dispatcher.h"

// Define the max buffer size and packet size
#define UART_PACKET_SIZE 248


namespace pno
{
//================================ UartPacket ==================================
class UartPacket
{
public :
  UartPacket() {};
  UartPacket(const char* msg, long no = 0) : id(no), message(msg) {};
  UartPacket(const char* msg, int len, long no = 0)
  {
    id = no;
    message = std::string(msg, len);
  };

  long id;
  std::string message;
};

//================================ UartLinux ===================================
class UartLinux
{
public :  //======================== Public ======================

  static void uartInit(const int handle, const long baud);

  static void uartWrite(const int handle, const UartPacket&);
  static UartPacket uartRead(const int handle);

  static void changeBaud(const int handle, const long baud);
  static speed_t longToBaud(const long baud);

private : //======================= Private ======================

  static void uartConfig(const int handle);
  static void applyConfig(const int handle, const termios& config);
};


}
#endif
