#include <unistd.h>
#include <sys/epoll.h>

#include "Framework/Logger.h"
#include "Acquisition/Daq.h"
#include "Acquisition/Wire.h"
#include "Sensor/Sensor.h"


namespace pno
{
//==================================== Sensor ==================================
Sensor::Sensor(const LuaTable& cfg)
{
  // Set Data Acquisition device
  std::string daq_name = cfg.get<std::string>("daq");
  _daq = static_cast<DaqDevice*>(Factory::get(daq_name));
}


//=================================== I2cSensor ================================
I2cSensor::I2cSensor(const LuaTable& cfg) : Sensor(cfg), TimerListener(cfg)
{
  // Set Wire
  std::string wire_name = cfg.get<std::string>("wire");
  _wire = static_cast<I2cWire*>(Factory::get(wire_name));
}

// Read sensor data from acquisition device
void I2cSensor::read(const byte regis, byte* data, const int bytes_num)
{
  daq().i2cRead(*this, regis, data, bytes_num);
}

// Write data to sensor
void I2cSensor::write(const byte regis, byte* data, const int bytes_num)
{
  daq().i2cWrite(*this, regis, data, bytes_num);
}


//=================================== UartSensor ===============================
UartSensor::UartSensor(const LuaTable& cfg) : Sensor(cfg)
{
  // Set Wire
  std::string wire_name = cfg.get<std::string>("wire");
  _wire = static_cast<UartWire*>(Factory::get(wire_name));

  // Get the handle
  int handle = _wire->handle();

  // Add this device to the Poll Multiplexer
  MuxPoll::add(handle, EPOLLIN);

  // Register to this device
  PollDispatcher::registerListener(handle, static_cast<PollListener*>(this));
}

// Change Baud rate
void UartSensor::changeBaud(const UartPacket& cmd, const long baud)
{
  // Send the message to change the baud to the sensor
  daq().uartWrite(*this, cmd.message.c_str());

  // Sleep a bit.
  sleep(1);

  // Change the baud
  _baud = baud;

  // Change the baud
  daq().changeBaud(*this);
}

// Write a message to sensor
void UartSensor::write(const UartPacket& pkt)
{
  daq().uartWrite(*this, pkt.message.c_str());
}


//================================== DigitalSensor =============================
//Constructor
DigitalSensor::DigitalSensor(const LuaTable& cfg) : Sensor(cfg)
{
  // Set device path
  _device = cfg.get<std::string>("device");

  // Get edge
  std::string edge = cfg.get<std::string>("edge");

  // Check edge value
  if (edge != "rising" && edge != "falling" && edge != "both")
  {
    FATAL_PF("Edge is undefined: %s", edge.c_str());
  }

  // Get the handle
  int handle = daq().openFile(_device);

  // Add this device to the Poll Multiplexer
  MuxPoll::add(handle, EPOLLET);

  // Register to this device
  PollDispatcher::registerListener(handle, static_cast<PollListener*>(this));
}

//===================================== Counter ================================
// Constructor
Counter::Counter(const LuaTable& cfg) : DigitalSensor(cfg)
{
  // Initialize counter
  _count = 0;
}

// On event callback
void Counter::onEvent(const PollEvent& evt)
{
  _count++;
}


}
