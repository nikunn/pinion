#ifndef _ASYNCH_LINUX_H
#define _ASYNCH_LINUX_H

// Define the max serial packet size
#define LINUX_ASYNCH_PACKET_SIZE 255

#include "Framework/Types.h"

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
  static int bytesAvailable(const int handle);
};


#endif


/*
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
*/
