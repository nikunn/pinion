#include <fcntl.h>

#include "Framework/Logger.h"
#include "Framework/Factory.h"
#include "Acquisition/Daq.h"
#include "Acquisition/Wire.h"


namespace pno
{
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
  // Open the connection to this I2C bus
  _handle = daq().openFile(device(), O_RDWR);

  // Initialize the connection
  daq().i2cInit(*this);

  // A bit of log
  INFO_PF("Created I2C wire \"%s\", device:%s handle:%u",
    cfg.get<std::string>("name").c_str(), device().c_str(), _handle);
}

// Destructor
I2cWire::~I2cWire()
{
  // Close connection to I2C bus
  daq().closeFile(handle());
}


//================================== SPIWire ===================================
SpiWire::SpiWire(const LuaTable& cfg) : Wire(cfg)
{
  // Get speed
  _speed = cfg.get<uint32_t>("speed");

  // Get mode
  _mode = cfg.get<uint8_t>("mode");

  // Get number of bits per word
  _word_bits = cfg.get<uint8_t>("word_bits");

  // Open the connection to this I2C bus
  _handle = daq().openFile(device(), O_RDWR);

  // Initialize this connection
  daq().spiInit(*this);

  // A bit of log
  INFO_PF("Created SPI wire \"%s\", device:%s handle:%u, mode:%u, speed:%u wordBits:%u",
    cfg.get<std::string>("name").c_str(), device().c_str(), _handle, mode(), speed(), wordBits());
}

// Destructor
SpiWire::~SpiWire()
{
  // Close connection to SPI bus
  daq().closeFile(handle());
}


//================================= UartWire ===================================
UartWire::UartWire(const LuaTable& cfg) : Wire(cfg)
{
  // Get the default baud
  _default_baud = cfg.get<long>("default_baud");

  // Get the communication type
  std::string com_type_str = cfg.get<std::string>("com_type");

  // Open the connection to this serial device
  _handle = daq().openFile(device(), O_RDWR | O_NOCTTY | O_NONBLOCK);

  // Initialize the connection
  daq().uartInit(*this);

  // A bit of log
  INFO_PF("Created UART wire \"%s\", device:%s handle:%u",
    cfg.get<std::string>("name").c_str(), device().c_str(), _handle);
}

// Destructor
UartWire::~UartWire()
{
  // Close connection to UART
  daq().closeFile(handle());
}


}
