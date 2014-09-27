#ifndef _DAQ_H
#define _DAQ_H

#include "Framework/Types.h"
#include "Framework/Accessible.h"
#include "Framework/LuaBind.h"


//=========================== Forward Declarations =============================

class I2cSensor;
class AsynchSensor;
class CommandPacket;


//================================= DaqDevice ==================================
class DaqDevice : public Accessible
{
public :  //======================== Public ======================

          //======================== Device ======================
  virtual void open() = 0;
  virtual void close() = 0;
  virtual void info() = 0;

          //================= I2C Communication ==================
  virtual void i2cSet(const I2cSensor&) {};
  virtual void i2cRead(const I2cSensor&, const byte regis, byte* data, const int bytes_num = 1) = 0;
  virtual void i2cWrite(const I2cSensor&, const byte regis, const byte* data, const int bytes_num = 1) = 0;

          //================= Asynch Communication ===============
  virtual void asynchStart() = 0;
  virtual void asynchStop() = 0;
  virtual void asynchSet(const AsynchSensor&) = 0;
  virtual void asynchRead(byte* data, int& bytes_num) = 0;
  virtual void asynchWrite(const CommandPacket&) = 0;
};


#endif
