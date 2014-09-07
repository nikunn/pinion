#include "Framework/Logger.h"

#include "Sensor/Sensor.h"

//==================================== Sensor ==================================

// Convert two byte to 16 bit int
/*static*/ int16_t Sensor::int8To16(byte* ent)
{
  return ((int16_t)ent[0] << 8) | ent[1];
}

// Convert two byte to 16 bit int
/*static*/ int16_t Sensor::int8To16(byte hi, byte lo)
{
  return ((int16_t)hi << 8) | lo;
}


//================================== AsynchSensor ==============================

// Set Baud rate
void AsynchSensor::setBaud(const CommandPacket& cmd, uint32_t baud_rate)
{
  // Send the command to change the baud to the sensor
  _daq->asynchWrite(cmd);

  // Stop the communication
  _daq->asynchStop();

  // Change the baud
  _baud = baud_rate;

  // Start the communication
  _daq->asynchStart();
}

// Start the Asynchronous communication
void AsynchSensor::start()
{
  // Reset the two packets buffer
  for (int i = 0; i < ASYNCH_PACKET_MAX_SIZE; i++) { _last_packet[i] = 0; _current_packet[i] = 0; }

  // Reset the pointer of current char
  _current_char = &_current_packet[0];

  // Reset the packet id
  _packet_id = 0;

  // Set the Asynch communication for this sensor
  _daq->asynchSet(*this);

  // Start the communication
  _daq->asynchStart();
}

// Stop the Asynchronous communication
void AsynchSensor::stop()
{
  // Stop the communication
  _daq->asynchStop();
}

// Start a new packet
void AsynchSensor::startPacket()
{
  // Reset the buffer
  for (int i = 0; i < ASYNCH_PACKET_MAX_SIZE; i++) { _current_packet[i] = 0; }

  // Point the current character to the begininng of the buffer
  _current_char = &_current_packet[0];
}

// End of packet jobs
void AsynchSensor::endPacket()
{
  // Move the current packet the the last packet
  for (int i = 0; i < ASYNCH_PACKET_MAX_SIZE; i++)
  {
    _last_packet[i] = _current_packet[i];
  }

  // Update the packet id
  _packet_id++;

  // Call the sensor as new packet is ready
  onPacket(std::string(_last_packet));
}

// Read the device buffer
void AsynchSensor::read()
{
  // Allocate a read buffer
  byte data[ASYNCH_BUFFER_SIZE];

  // Initailize the number of byte read
  int byte_read = 0;

  // Read the buffer from the device
  _daq->asynchRead(&data[0], byte_read);

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
          &_current_packet[0] + sizeof(char*) * (ASYNCH_PACKET_MAX_SIZE - 1)))
    {
      ERROR_LG("Asynch sensor over the limit of the packet size buffer");
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
