#include "Framework/Logger.h"
#include "Framework/Factory.h"
#include "Acquisition/Daq.h"
#include "Acquisition/Wire.h"

//=================================== Wire =====================================
Wire::Wire(const LuaTable& cfg)
{
  // Set Data Acquisition device
  std::string daq_name = cfg.get<std::string>("daq");
  _daq = static_cast<DaqDevice*>(Factory::get(daq_name));

  // Get the device name
  _device = cfg.get<std::string>("device");
}

//================================== I2CWire ===================================
// Constructor
I2cWire::I2cWire(const LuaTable& cfg) : Wire(cfg)
{
  // Set the data line
  _data_line = cfg.get<int>("data_line");

  // Set the clock line
  _clock_line = cfg.get<int>("clock_line");

  // Open the connection to this I2C bus
  _handle = daq().i2cOpen(*this);

  // A bit of log
  INFO_PF("Created I2C wire \"%s\", data:%u clock:%u device:%s handle:%u",
    cfg.get<std::string>("name").c_str(), _data_line, _clock_line, device().c_str(), _handle);
}

// Destructor
I2cWire::~I2cWire()
{
  // Close connection to I2C bus
  daq().i2cClose(*this);
}


//================================== SPIWire ===================================
SpiWire::SpiWire(const LuaTable& cfg) : Wire(cfg)
{
  // Set the clock line
  _clock_line = cfg.get<int>("clock_line");

  // Set the mosi line
  _mosi_line = cfg.get<int>("mosi_line");

  // Set the miso line
  _miso_line = cfg.get<int>("miso_line");

  // Set the slave line
  _slave_line = cfg.get<int>("slave_line");

  // A bit of log
  INFO_PF("Created SPI wire \"%s\", clock:%u mosi:%u miso:%u slave:%u",
    cfg.get<std::string>("name").c_str(), _clock_line, _mosi_line, _miso_line, _slave_line);
}


//================================= UartWire ===================================
UartWire::UartWire(const LuaTable& cfg) : Wire(cfg)
{
  // Set the transmit line
  _transmit_line = cfg.get<int>("transmit_line");

  // Set the Receive line
  _receive_line = cfg.get<int>("receive_line");

  // Get the default baud
  _default_baud = cfg.get<long>("default_baud");

  // Get the communication type
  std::string com_type_str = cfg.get<std::string>("com_type");

  // Parse the communication type
  _com_type = UartCom::parse(com_type_str);

  // Open the connection to this serial device
  _handle = daq().uartOpen(*this);

  // A bit of log
  INFO_PF("Created UART wire \"%s\", transmit:%u receive:%u device:%s handle:%u",
    cfg.get<std::string>("name").c_str(), _transmit_line, _receive_line,
    device().c_str(), _handle);
}

// Destructor
UartWire::~UartWire()
{
  // Close connection to I2C bus
  daq().uartClose(*this);
}

