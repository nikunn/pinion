#ifndef _ASYNCH_LINUX_H
#define _ASYNCH_LINUX_H

#include <termios.h>

#include "Framework/Types.h"

// Define the max serial packet size
#define LINUX_ASYNCH_PACKET_SIZE 255

class AsynchLinux
{
public :  //======================== Public ======================
  static int asynchRead();
  static int asynchWrite();

  static int asynchOpen(const std::string& device_name, const long baud);
  static void asynchClose(const int handle);

  static void asynchBaud(const int handle, const long baud);

  static int handle;
private : //======================= Private ======================
  static void onCallback(int status);
  static speed_t longToBaud(const long baud);
  static int bytesAvailable(const int handle);
};


#endif
