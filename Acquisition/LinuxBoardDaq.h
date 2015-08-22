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

  LinuxBoardDaq(const LuaTable&) {};
  ~LinuxBoardDaq() {};

          //======================== IO ==========================
  int openFile(const std::string& file);

          //======================== PWM =========================
  void pwmInit(const std::string& pin, const pwmConfig& pwm);
  void pwmSetStatus(const std::string& pin, const bool status);
  bool pwmGetStatus(const std::string& pin);

          //================= I2C Communication ==================
  int i2cOpen(const I2cWire&);
  void i2cClose(const I2cWire&);
  void i2cRead(const I2cSensor&, const byte regis, byte* data, const int bytes_num = 1);
  void i2cWrite(const I2cSensor&, const byte regis, const byte* data, const int bytes_num = 1);

          //================== Uart Communication ================
  int uartOpen(const UartWire&);
  void uartClose(const UartWire&);
  void changeBaud(const UartSensor&);
  void uartWrite(const UartSensor&, const UartPacket&);
  void uartRead(const UartSensor&, byte* data, int& bytes_num);
};


}
#endif
