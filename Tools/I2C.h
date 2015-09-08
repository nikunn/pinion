// This is an altered version of https://github.com/jwr/lsquaredc

#ifndef _I2C_LINUX_H
#define _I2C_LINUX_H

#include "Framework/Types.h"


namespace pno
{
class I2cLinux
{
public :  //======================== Public ======================
  static void i2cInit(const int handle);
  static int i2cRead(const int handle, const byte address, const byte regis, byte* data, const int bytes_num);
  static int i2cWrite(const int handle, const byte address, const byte regis, const byte* data, const int bytes_num);

private : //======================= Private ======================
  static int countSegments(const uint16_t *sequence, const int sequence_len);
  static int sendSequence(const int handle, const uint16_t* sequence, const int sequence_len, byte* rcv_data);
};


}
#endif
