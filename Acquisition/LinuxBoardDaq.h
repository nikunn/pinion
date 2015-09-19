#ifndef _LINUXBOARD_DAQ_H
#define _LINUXBOARD_DAQ_H

#include "Acquisition/Daq.h"


namespace pno
{
//=========================== Forward Declarations =============================

class I2cWire;
class UartWire;
class I2cSensor;
class UartSensor;
class UartPacket;


//=============================== LinuxBoardDaq ================================
class LinuxBoardDaq : public DaqDevice
{
public :  //======================== Public ======================

  LinuxBoardDaq(const LuaTable& cfg) : DaqDevice(cfg) {};
  ~LinuxBoardDaq() {};

          //======================== IO ==========================
  int openFile(const std::string& file, const int option);
  void closeFile(const int handle);

          //======================== PWM =========================
  void pwmInit(const std::string& pin, const PwmConfig& pwm);
  void pwmSetStatus(const std::string& pin, const bool status);
  bool pwmGetStatus(const std::string& pin);

          //================= SPI Communication ==================
  void spiInit(SpiWire&);
  void spiTransfer(const SpiWire& wire, const byte* wdata, byte* rdata, const int bytes_num);

          //================= I2C Communication ==================
  void i2cInit(const I2cWire&);
  void i2cRead(const I2cSensor&, const byte regis, byte* data, const int bytes_num = 1);
  void i2cWrite(const I2cSensor&, const byte regis, const byte* data, const int bytes_num = 1);

          //================== Uart Communication ================
  void uartInit(const UartWire&);
  void changeBaud(const UartSensor&);
  void uartWrite(const UartSensor&, const UartPacket&);
  void uartRead(const UartSensor&, byte* data, int& bytes_num);
};


}
#endif
