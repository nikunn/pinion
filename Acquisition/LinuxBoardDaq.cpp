#include "Framework/Logger.h"
#include "Tools/IO.h"
#include "Tools/PWM.h"
#include "Tools/GPIO.h"
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
int LinuxBoardDaq::openFile(const std::string& file)
{
  return IoLinux::openFile(file);
}

//===================================== GPIO ===================================
// Initialize GPIO pin
int LinuxBoardDaq::gpioInit(const std::string& pin, const GpioConfig& gpio)
{
  return GpioLinux::gpioInit(pin, gpio);
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

//=============================== I2C Communication ============================
// Open I2C device
int LinuxBoardDaq::i2cOpen(const I2cWire& wire)
{
  return I2cLinux::i2cOpen(wire.device());
}

// Close I2C device
void LinuxBoardDaq::i2cClose(const I2cWire& wire)
{
  I2cLinux::i2cClose(wire.handle());
}

// Read data in I2C
void LinuxBoardDaq::i2cRead(const I2cSensor& sensor, const byte regis,
                             byte* data, const int bytes_num)
{
  // Execute the I2C communication
  int result = I2cLinux::i2cRead(sensor.wire()->handle(), sensor.address(), regis, data, bytes_num);
}

// Write data in I2C
void LinuxBoardDaq::i2cWrite(const I2cSensor& sensor, const byte regis,
                              const byte* data, const int bytes_num)
{
  // Execute the I2C communication
  int result = I2cLinux::i2cWrite(sensor.wire()->handle(), sensor.address(), regis, data, bytes_num);
}


//=============================== UART Communication ===========================
// Open UART device
int LinuxBoardDaq::uartOpen(const UartWire& wire)
{
  return UartLinux::uartOpen(wire.device(), wire.defaultBaud());
}

// Close UART device
void LinuxBoardDaq::uartClose(const UartWire& wire)
{
  UartLinux::uartClose(wire.handle());
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
