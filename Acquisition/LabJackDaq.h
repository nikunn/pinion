#ifndef _LABJACK_DAQ_H
#define _LABJACK_DAQ_H

#include "Daq.h"

#define LABJACK_USB_PACKET_SIZE 32
#define LABJACK_I2C_PACKET_SIZE 32

#define LABJACK_INIT_ERR_ADDR -1

//=========================== Forward Declarations =============================

class I2cWire;
class UartWire;
class I2cSensor;
class UartSensor;
class UartPacket;

//================================ LabJackDaq =================================
class LabJackDaq : public DaqDevice
{
public :

  LabJackDaq(const LuaTable&);
  ~LabJackDaq();

          //======================== Device ======================
  void deviceOpen();
  void deviceClose();
  void deviceInfo();

  int handle() const { return _handle; }
  static void errorCheck(int, const char*);

          //================= I2C Communication ==================
  int i2cOpen(const I2cWire&);
  void i2cClose(const I2cWire&);
  void i2cSet(const I2cSensor&);
  void i2cRead(const I2cSensor&, const byte regis, byte* data, const int bytes_num = 1);
  void i2cWrite(const I2cSensor&, const byte regis, const byte* data, const int bytes_num = 1);

          //================== Uart Communication ================
  int uartOpen(const UartWire&);
  void uartClose(const UartWire&);
  void changeBaud(const UartSensor&);
  void uartWrite(const UartSensor&, const UartPacket&);
  void uartRead(const UartSensor&, byte* data, int& bytes_num);

private:

  // Handle to this device
  int _handle;

  // Current I2C configuration
  const I2cWire* _i2c_wire;
};


#endif
