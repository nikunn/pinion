#include <unistd.h>
#include <sys/epoll.h>

#include "Framework/Logger.h"
#include "Acquisition/Wire.h"
#include "Acquisition/Daq.h"
#include "Sensor/Sensor.h"


namespace pno
{
//==================================== Sensor ==================================
Sensor::Sensor(const LuaTable& cfg) : Accessible(cfg)
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


//=================================== SpiSensor ================================
SpiSensor::SpiSensor(const LuaTable& cfg) : Sensor(cfg), TimerListener(cfg)
{
  // Set Wire
  std::string wire_name = cfg.get<std::string>("wire");
  _wire = static_cast<SpiWire*>(Factory::get(wire_name));
}

// Read sensor data from acquisition device
void SpiSensor::read(const byte regis, byte* data, const int bytes_num)
{
  // Copy register address
  byte address = regis;

  // Read mode
  address |= 0x80;

  // Add the multiple read bit if needed
  if (bytes_num > 1) { address |= 0x40; }

  // Initialize total length
  int length = bytes_num + 1;

  // Initialize the read and write data structure
  byte* wdata = new byte[length]();
  byte* rdata = new byte[length]();

  // Define the first data
  wdata[0] = address;

  // Transfer the data
  transfer(wdata, rdata, length);

  // Copy the read data to the argument array (first byte is skipped)
  for (int i = 0; i < bytes_num; i++) { data[i] = rdata[i + 1]; }

  // Delete the temporary arrays
  delete[] wdata;
  delete[] rdata;
}

// Write data to sensor
void SpiSensor::write(const byte regis, byte* data, const int bytes_num)
{
  // Initialize total length
  int length = bytes_num + 1;

  // Initialize the read data
  byte rdata = 0x00;

  // Initialize the write data structure
  byte* wdata = new byte[length]();

  // Input the register address as first byte to write
  wdata[0] = regis;

  // The rest of the sequence is made of the data we want to write
  for (int i = 0; i < bytes_num; i++) { wdata[1 + i] = data[i]; }

  // Transfer the data
  transfer(wdata, &rdata, length);

  // Delete the temporary write data
  delete[] wdata;
}

// Transfer data to sensor
void SpiSensor::transfer(const byte* wdata, byte* rdata, const int bytes_num)
{
  daq().spiTransfer(*wire(), wdata, rdata, bytes_num);
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
DigitalSensor::DigitalSensor(const LuaTable& cfg) : Sensor(cfg), GpioFile(cfg)
{
  // Add this device to the Poll Multiplexer
  MuxPoll::add(handle(), EPOLLET);

  // Register to this device
  PollDispatcher::registerListener(handle(), static_cast<PollListener*>(this));
}

//===================================== Counter ================================
// Constructor
Counter::Counter(const LuaTable& cfg) : DigitalSensor(cfg), _count(0) {}

// On event callback
void Counter::onEvent(const PollEvent& evt)
{
  _count++;
}


}
