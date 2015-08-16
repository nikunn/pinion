#ifndef _DAQ_H
#define _DAQ_H

#include "Framework/Types.h"
#include "Framework/Accessible.h"
#include "Framework/LuaBind.h"


//=========================== Forward Declarations =============================

class I2cWire;
class UartWire;
class I2cSensor;
class UartSensor;
class UartPacket;


//================================= DaqDevice ==================================
class DaqDevice : public Accessible
{
public :  //======================== Public ======================

          //======================== Device ======================
  virtual void deviceOpen() = 0;
  virtual void deviceClose() = 0;
  virtual void deviceInfo() = 0;

          //======================== PWM =========================
  virtual void pwmInit(const std::string pin, const pwmConfig& pwm) = 0;
  virtual void pwmSetStatus(const std::string& pin, const bool status) = 0;
  virtual bool pwmGetStatus(const std::string& pin) = 0;

          //================= I2C Communication ==================
  virtual int i2cOpen(const I2cWire&) = 0;
  virtual void i2cClose(const I2cWire&) = 0;

  virtual void i2cRead(const I2cSensor&, const byte regis, byte* data, const int bytes_num = 1) = 0;
  virtual void i2cWrite(const I2cSensor&, const byte regis, const byte* data, const int bytes_num = 1) = 0;

          //================== Uart Communication ================
  virtual int uartOpen(const UartWire&) = 0;
  virtual void uartClose(const UartWire&) = 0;
  virtual void changeBaud(const UartSensor&) = 0;
  virtual void uartWrite(const UartSensor&, const UartPacket&) = 0;
  virtual void uartRead(const UartSensor&, byte* data, int& bytes_num) = 0;
};


#endif
