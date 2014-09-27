#ifndef _RASPBERRYPI_DAQ_H
#define _RASPBERRYPI_DAQ_H

#include "Acquisition/Daq.h"


//=========================== Forward Declarations =============================

class I2cWire;
class AsynchSensor;

//================================ RaspberryPiDaq =================================
class RaspberryPiDaq : public DaqDevice
{
public :  //======================== Public ======================

  RaspberryPiDaq(const LuaTable&);
  ~RaspberryPiDaq();

          //======================== Device ======================
  void open();
  void close();
  void info();

          //================= I2C Communication ==================
  void i2cRead(const I2cSensor&, const byte regis, byte* data, const int bytes_num = 1);
  void i2cWrite(const I2cSensor&, const byte regis, const byte* data, const int bytes_num = 1);

          //================= Asynch Communication ===============
  void asynchStart();
  void asynchStop();
  void asynchSet(const AsynchSensor&);
  void asynchRead(byte* data, int& bytes_num);
  void asynchWrite(const CommandPacket&);
};


#endif
