#include <unistd.h>

#include "Framework/Logger.h"
#include "Acquisition/Daq.h"
#include "Acquisition/Wire.h"
#include "Sensor/Sensor.h"

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
}

// Change Baud rate
void UartSensor::changeBaud(const UartPacket& cmd, const long baud)
{
  // Send the message to change the baud to the sensor
  daq().uartWrite(*this, cmd.message.c_str());

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


//================================== SerialSensor ==============================
SerialSensor::SerialSensor(const LuaTable& cfg) : UartSensor(cfg)
{
  // Reset the serial communication variables
  reset();
}

// Reset the uart variables
void SerialSensor::reset()
{
  // Reset the two packets buffer
  for (int i = 0; i < UART_PACKET_SIZE; i++) { _last_packet[i] = 0; _current_packet[i] = 0; }

  // Reset the pointer of current char
  _current_char = &_current_packet[0];
}

// Start a new packet
void SerialSensor::startPacket()
{
  // Reset the buffer
  for (int i = 0; i < UART_PACKET_SIZE; i++) { _current_packet[i] = 0; }

  // Point the current character to the begininng of the buffer
  _current_char = &_current_packet[0];
}

// End of packet jobs
void SerialSensor::endPacket()
{
  // Move the current packet the the last packet
  for (int i = 0; i < UART_PACKET_SIZE; i++)
  {
    _last_packet[i] = _current_packet[i];
  }

  // Call the sensor as new packet is ready
  onPacket(UartPacket(_last_packet));
}

// Read the device buffer
void SerialSensor::read()
{
  // Allocate a read buffer
  byte data[UART_BUFFER_SIZE];

  // Initailize the number of byte read
  int byte_read = 0;

  // Read the buffer from the device
  daq().uartRead(*this, &data[0], byte_read);

  // Go through the read data
  for (int i = 0; i < byte_read; i++)
  {
    // Found begining of new packet
    if ((char) data[i] == _begin_char)
    {
      startPacket();
    }
    // Found end of packet
    else if ((char) data[i] == _end_char)
    {
      // End the current packet
      endPacket();

      // Start a new packet
      startPacket();

      // No need to continue with this end of packet character
      continue;
    }

    // Check that the buffer is not full
    if (std::greater<char*>()(_current_char,
          &_current_packet[0] + sizeof(char*) * (UART_PACKET_SIZE - 1)))
    {
      ERROR_LG("Serial sensor over the limit of the packet size buffer");
    }
    else
    {
      // append the read character to the end of the current packet buffer
      *_current_char = (char) data[i];

      // Increment the current character pointer
      _current_char++;
    }
  }
}


//================================== AsynchSensor ==============================
//Constructor
AsynchSensor::AsynchSensor(const LuaTable& cfg) : UartSensor(cfg)
{
  // Get the handle
  int handle = wire()->handle();

  // Register to this handle
  AsynchDispatcher::registerListener(handle, static_cast<AsynchListener*>(this));
}
