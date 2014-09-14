#ifndef _LABJACK_DAQ_H
#define _LABJACK_DAQ_H

#include "Daq.h"

#define USB_PACKET_SIZE 32
#define I2C_PACKET_MAX_SIZE 32

#define INITIAL_ERR_ADDRESS -1

//=========================== Forward Declarations =============================

class I2cWire;
class AsynchSensor;

//================================ LabJackDaq =================================
class LabJackDaq : public DaqDevice
{
public :

  LabJackDaq(const LuaTable&);
  ~LabJackDaq();

          //======================== Device ======================
  void open();
  void close();
  void info();

  static void errorCheck(int, const char*);

          //================= I2C Communication ==================
  void i2cSet(const I2cSensor&);
  void i2cRead(const I2cSensor&, const byte regis, byte* data, const int bytes_num = 1);
  void i2cWrite(const I2cSensor&, const byte regis, byte* data, const int bytes_num = 1);

          //================= Asynch Communication ===============
  void asynchStart();
  void asynchStop();
  void asynchSet(const AsynchSensor&);
  void asynchRead(byte* data, int& bytes_num);
  void asynchWrite(const CommandPacket&);

private:

  //=========================== private members ============================
  // Current I2C configuration
  const I2cWire* _i2c_wire;
};


#endif
