#include "Framework/Logger.h"
#include "Tools/I2C.h"
#include "Acquisition/RaspberryPiDaq.h"
#include "Acquisition/Wire.h"
#include "Sensor/Sensor.h"

//=============================== RaspberryPiDaq ================================
// Constructor
RaspberryPiDaq::RaspberryPiDaq(const LuaTable&)
{
};

// Destructor
RaspberryPiDaq::~RaspberryPiDaq()
{
};

// Open connection to labjack
void RaspberryPiDaq::open()
{
}

// Close connection to labjack
void RaspberryPiDaq::close()
{
}

// Print some informations about this labjack device
void RaspberryPiDaq::info()
{
}

//=============================== I2C Communication ============================

// Read data in I2C
void RaspberryPiDaq::i2cRead(const I2cSensor& sensor, const byte regis, byte* data, const int bytes_num)
{
  // Execute the I2C communication
  int result = I2cLinux::i2cRead(sensor.wire()->handle(), sensor.address(), regis, data, bytes_num);
}

// Write data in I2C
void RaspberryPiDaq::i2cWrite(const I2cSensor& sensor, const byte regis, const byte* data, const int bytes_num)
{
  // Execute the I2C communication
  int result = I2cLinux::i2cWrite(sensor.wire()->handle(), sensor.address(), regis, data, bytes_num);
}


//============================== Asynch Communication ==========================

// Initialize the Asynch communication for this sensor
void RaspberryPiDaq::asynchSet(const AsynchSensor& sensor)
{
}

// Turn on Asynch. Configures timing hardware, DIO lines and allocates the receiving buffer
void RaspberryPiDaq::asynchStart()
{
}

// Turn off asynch
void RaspberryPiDaq::asynchStop()
{
}

// Read data in asynch
void RaspberryPiDaq::asynchRead(byte* data, int& bytes_read)
{
}

// Write data to asynch
void RaspberryPiDaq::asynchWrite(const CommandPacket& cmd)
{
}
