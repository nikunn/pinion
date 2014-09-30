#ifndef _DAQ_H
#define _DAQ_H

#include "Framework/Types.h"
#include "Framework/Accessible.h"
#include "Framework/LuaBind.h"


//=========================== Forward Declarations =============================

class I2cWire;
class AsynchWire;
class I2cSensor;
class AsynchSensor;


//================================= DaqDevice ==================================
class DaqDevice : public Accessible
{
public :  //======================== Public ======================

          //======================== Device ======================
  virtual void deviceOpen() = 0;
  virtual void deviceClose() = 0;
  virtual void deviceInfo() = 0;

          //================= I2C Communication ==================
  virtual int i2cOpen(const I2cWire&) = 0;
  virtual void i2cClose(const I2cWire&) = 0;

  virtual void i2cRead(const I2cSensor&, const byte regis, byte* data, const int bytes_num = 1) = 0;
  virtual void i2cWrite(const I2cSensor&, const byte regis, const byte* data, const int bytes_num = 1) = 0;

          //================= Asynch Communication ===============
  virtual int asynchOpen(const AsynchWire&) = 0;
  virtual void asynchClose(const AsynchWire&) = 0;

  virtual void asynchBaud(const AsynchSensor&) = 0;
  virtual void asynchRead(const AsynchSensor&, byte* data, int& bytes_num) = 0;
  virtual void asynchWrite(const AsynchSensor&, const std::string&) = 0;
};


#endif
