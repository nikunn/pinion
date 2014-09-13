#include "Wire.h"

#include "Framework/LuaBind.h"
#include "Framework/Logger.h"

//================================== I2CWire ===================================
I2cWire::I2cWire(const LuaTable& cfg)
{
  // Set the data line
  _data_line = cfg.get<int>("data_line");

  // Set the clock line
  _clock_line = cfg.get<int>("clock_line");

  // A bit of log
  INFO_PF("Creating I2C wire \"%s\", data:%u clock:%u",
    cfg.get<std::string>("name").c_str(), _data_line, _clock_line);
}


//================================== SPIWire ===================================
SpiWire::SpiWire(const LuaTable& cfg)
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
  INFO_PF("Creating SPI wire \"%s\", clock:%u mosi:%u miso:%u slave:%u",
    cfg.get<std::string>("name").c_str(), _clock_line, _mosi_line, _miso_line, _slave_line);
}


//================================ AsynchWire ==================================
AsynchWire::AsynchWire(const LuaTable& cfg)
{
  // Set the transmit line
  _transmit_line = cfg.get<int>("transmit_line");

  // Set the Receive line
  _receive_line = cfg.get<int>("receive_line");

  // A bit of log
  INFO_PF("Creating Asynch wire \"%s\", transmit:%u receive:%u",
    cfg.get<std::string>("name").c_str(), _transmit_line, _receive_line);
}

