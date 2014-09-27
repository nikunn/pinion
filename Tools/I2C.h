#ifndef _I2C_LINUX_H
#define _I2C_LINUX_H

#include "Framework/Types.h"

class I2cLinux
{
public :  //======================== Public ======================
  static int i2cCom(const int handle, const byte address, const byte regis, 
                    const bool write, byte* data, const int bytes_num);

  static int i2cOpen(int bus);
  static void i2cClose(int handle); 
  static void i2cCheck(int handle, char* device_name);

private : //======================= Private ======================
  static int countSegments(uint16_t *sequence, int sequence_len);
  static int sendSequence(int handle, uint16_t* sequence, int sequence_len, byte* rcv_data);
};


#endif
