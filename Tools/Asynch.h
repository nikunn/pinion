#ifndef _ASYNCH_LINUX_H
#define _ASYNCH_LINUX_H

#include "Framework/Types.h"

class AsynchLinux
{
public :  //======================== Public ======================
  static int asynchRead();
  static int asynchWrite();

  static int asynchOpen(const std::string& device_name, const long baud);
  static void asynchClose(const int handle);

  static void asynchBaud(const int handle, const long baud);

private : //======================= Private ======================
  static int bytesAvailable(const int handle);
};


#endif
