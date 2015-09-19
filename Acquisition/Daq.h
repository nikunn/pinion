#ifndef _DAQ_H
#define _DAQ_H

#include "Framework/Types.h"
#include "Framework/Accessible.h"
#include "Framework/LuaBind.h"


namespace pno
{
//=========================== Forward Declarations =============================

class I2cWire;
class SpiWire;
class UartWire;
class I2cSensor;
class UartSensor;
class UartPacket;


//================================= DaqDevice ==================================
class DaqDevice : public Accessible
{
public :  //======================== Public ======================

  DaqDevice(const LuaTable& cfg) : Accessible(cfg) {};
  ~DaqDevice() {};

          //======================== IO ==========================
  virtual int openFile(const std::string& file, const int option) = 0;
  virtual void closeFile(const int handle) = 0;

          //======================== PWM =========================
  virtual void pwmInit(const std::string& pin, const PwmConfig& pwm) = 0;
  virtual void pwmSetStatus(const std::string& pin, const bool status) = 0;
  virtual bool pwmGetStatus(const std::string& pin) = 0;

          //================= SPI Communication ==================
  virtual void spiInit(SpiWire&) = 0;
  virtual void spiTransfer(const SpiWire& wire, const byte* wdata, byte* rdata, const int bytes_num) = 0;

          //================= I2C Communication ==================
  virtual void i2cInit(const I2cWire&) = 0;
  virtual void i2cRead(const I2cSensor&, const byte regis, byte* data, const int bytes_num = 1) = 0;
  virtual void i2cWrite(const I2cSensor&, const byte regis, const byte* data, const int bytes_num = 1) = 0;

          //================== Uart Communication ================
  virtual void uartInit(const UartWire&) = 0;
  virtual void changeBaud(const UartSensor&) = 0;
  virtual void uartWrite(const UartSensor&, const UartPacket&) = 0;
  virtual void uartRead(const UartSensor&, byte* data, int& bytes_num) = 0;
};


}
#endif
