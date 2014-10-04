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

// Posix compliant source
#define _POSIX_SOURCE 1


//================================== UartCom ===================================
class UartCom
{
public :
  // Enum for the different type of uart communication
  enum Type
  {
    SERIAL,
    ASYNCH,
  };

public :
  static Type parse(const std::string&);
};

//====================== Asynch Dispatch Listener Event ========================
class UartPacket : Event
{
public :
  UartPacket() {};
  UartPacket(const char* msg, long no = 0) : id(no), message(msg) {};

  long id;
  std::string message;
};

typedef Listener<UartPacket> AsynchListener;
typedef Dispatcher<UartPacket> AsynchDispatcher;


//================================ UartLinux ===================================
class UartLinux
{
public :  //======================== Public ======================

  static int uartOpen(const std::string& device_name, const long baud, const UartCom::Type com);
  static void uartClose(const int handle);

  static void uartWrite(const int handle, const UartPacket&);
  static void uartRead(const int handle, byte* data, int& bytes_num);

  static void changeBaud(const int handle, const long baud);

  static void onEvent(int signo, siginfo_t* info, void* context);
  static speed_t longToBaud(const long baud);
  static int bytesAvailable(const int handle);

private : //======================= Private ======================

  static void applyConfig(const int handle, const termios& config);
  static void serialConfig(const int handle);
  static void asynchConfig(const int handle);
};


#endif
