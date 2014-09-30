#ifndef _RASPBERRYPI_DAQ_H
#define _RASPBERRYPI_DAQ_H

#include "Acquisition/Daq.h"


//=========================== Forward Declarations =============================

class I2cWire;
class AsynchWire;
class I2cSensor;
class AsynchSensor;

//================================ RaspberryPiDaq =================================
class RaspberryPiDaq : public DaqDevice
{
public :  //======================== Public ======================

  RaspberryPiDaq(const LuaTable&);
  ~RaspberryPiDaq();

          //======================== Device ======================
  void deviceOpen();
  void deviceClose();
  void deviceInfo();

          //================= I2C Communication ==================
  int i2cOpen(const I2cWire&);
  void i2cClose(const I2cWire&);
  void i2cRead(const I2cSensor&, const byte regis, byte* data, const int bytes_num = 1);
  void i2cWrite(const I2cSensor&, const byte regis, const byte* data, const int bytes_num = 1);

          //================= Asynch Communication ===============
  int asynchOpen(const AsynchWire&);
  void asynchClose(const AsynchWire&);
  void asynchBaud(const AsynchSensor&);
  void asynchRead(const AsynchSensor&, byte* data, int& bytes_num);
  void asynchWrite(const AsynchSensor&, const std::string&);
};


#endif
