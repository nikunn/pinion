#include "Framework/Logger.h"
#include "Tools/IO.h"
#include "Tools/PWM.h"
#include "Tools/I2C.h"
#include "Tools/UART.h"
#include "Acquisition/LinuxBoardDaq.h"
#include "Acquisition/Wire.h"
#include "Sensor/Sensor.h"


namespace pno
{
//=============================== LinuxBoardDaq ================================
//====================================== IO ====================================
// Open file
int LinuxBoardDaq::openFile(const std::string& file, const int option)
{
  return IoLinux::openFile(file, option);
}

// Close handle
void LinuxBoardDaq::closeFile(const int handle)
{
  return IoLinux::closeFile(handle);
}

//====================================== PWM ===================================
// Initialize PWM pin
void LinuxBoardDaq::pwmInit(const std::string& pin, const PwmConfig& pwm)
{
  PwmLinux::pwmInit(pin, pwm);
}

// Change status of PWM
void LinuxBoardDaq::pwmSetStatus(const std::string& pin, const bool status)
{
  PwmLinux::pwmSetStatus(pin, status);
}

// Get current status of PWM
bool LinuxBoardDaq::pwmGetStatus(const std::string& pin)
{
  return PwmLinux::pwmGetStatus(pin);
}

//=============================== SPI Communication ============================
// Initialize SPI device
void LinuxBoardDaq::spiInit(SpiWire& wire)
{
  SpiLinux::spiInit(wire);
}

// Transfer data in SPI
void LinuxBoardDaq::spiTransfer(const SpiWire& wire, const byte* wdata, byte* rdata, const int bytes_num)
{
  SpiLinux::spiTransfer(wire, wdata, rdata, bytes_num);
}

//=============================== I2C Communication ============================
// Initialize I2C device
void LinuxBoardDaq::i2cInit(const I2cWire& wire)
{
  I2cLinux::i2cInit(wire.handle());
}

// Read data in I2C
void LinuxBoardDaq::i2cRead(const I2cSensor& sensor, const byte regis, byte* data, const int bytes_num)
{
  int result = I2cLinux::i2cRead(sensor.wire()->handle(), sensor.address(), regis, data, bytes_num);
}

// Write data in I2C
void LinuxBoardDaq::i2cWrite(const I2cSensor& sensor, const byte regis, const byte* data, const int bytes_num)
{
  int result = I2cLinux::i2cWrite(sensor.wire()->handle(), sensor.address(), regis, data, bytes_num);
}


//=============================== UART Communication ===========================
// Initialize UART device
void LinuxBoardDaq::uartInit(const UartWire& wire)
{
  UartLinux::uartInit(wire.handle(), wire.defaultBaud());
}

// Change the baud of UART device
void LinuxBoardDaq::changeBaud(const UartSensor& sensor)
{
  UartLinux::changeBaud(sensor.wire()->handle(), sensor.baud());
}

// Read data from serial
void LinuxBoardDaq::uartRead(const UartSensor& sensor, byte* data, int& bytes_num)
{
}

// Write data to serial
void LinuxBoardDaq::uartWrite(const UartSensor& sensor, const UartPacket& packet)
{
  UartLinux::uartWrite(sensor.wire()->handle(), packet);
}

}
