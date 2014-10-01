#include "Framework/Logger.h"
#include "Tools/I2C.h"
#include "Tools/Asynch.h"
#include "Acquisition/RaspberryPiDaq.h"
#include "Acquisition/Wire.h"
#include "Sensor/Sensor.h"

//============================== RaspberryPiDaq ================================
// Constructor
RaspberryPiDaq::RaspberryPiDaq(const LuaTable&)
{
};

// Destructor
RaspberryPiDaq::~RaspberryPiDaq()
{
};

// Open device
void RaspberryPiDaq::deviceOpen()
{
}

// Close device
void RaspberryPiDaq::deviceClose()
{
}

// Print some informations about this device
void RaspberryPiDaq::deviceInfo()
{
}

//=============================== I2C Communication ============================
// Open I2C device
int RaspberryPiDaq::i2cOpen(const I2cWire& wire)
{
  return I2cLinux::i2cOpen(wire.device());
}

// Close I2C device
void RaspberryPiDaq::i2cClose(const I2cWire& wire)
{
  I2cLinux::i2cClose(wire.handle());
}

// Read data in I2C
void RaspberryPiDaq::i2cRead(const I2cSensor& sensor, const byte regis,
                             byte* data, const int bytes_num)
{
  // Execute the I2C communication
  int result = I2cLinux::i2cRead(sensor.wire()->handle(), sensor.address(), regis, data, bytes_num);
}

// Write data in I2C
void RaspberryPiDaq::i2cWrite(const I2cSensor& sensor, const byte regis,
                              const byte* data, const int bytes_num)
{
  // Execute the I2C communication
  int result = I2cLinux::i2cWrite(sensor.wire()->handle(), sensor.address(), regis, data, bytes_num);
}


//============================== Asynch Communication ==========================

// Open serial device in asynch mode
int RaspberryPiDaq::asynchOpen(const AsynchWire& wire)
{
  return AsynchLinux::asynchOpen(wire.device(), wire.defaultBaud());
}

// Close asynch device
void RaspberryPiDaq::asynchClose(const AsynchWire& wire)
{
  AsynchLinux::asynchClose(wire.handle());
}

// Change the baud of asynch device
void RaspberryPiDaq::asynchBaud(const AsynchSensor& sensor)
{
}

// Read data from serial
void RaspberryPiDaq::asynchRead(const AsynchSensor& sensor, byte* data, int& bytes_num)
{
}

// Write data to serial
void RaspberryPiDaq::asynchWrite(const AsynchSensor& sensor, const std::string&)
{
}
