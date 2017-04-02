#include "Framework/LuaBind.h"
#include "Framework/Logger.h"
#include "Sensor/ParticlePMS7003.h"


namespace pno
{
//================================ ParticlePMS7003 =============================
// Constructor
ParticlePMS7003::ParticlePMS7003(const LuaTable& cfg) : UartSensor(cfg)
{
  // Get baud
  _baud = cfg.get<long>("baud");

  // Initialize partial message index
  _index = 0;

  // Partial message being populated flag
  _partial = false;

  // Initialize buffer
  for (int i = 0; i < PMS7003_BUFFER_LEN; i++) { _buffer[i] = 0; }
}

// Received an event, a full packetSignalEvent
void ParticlePMS7003::onEvent(const PollEvent& evt)
{
  // Get the message
  const UartPacket& pkt = UartLinux::uartRead(wire()->handle());

  // Get the message
  const std::string& message = pkt.message;

  // Get the length of the message
  const int msg_len = message.length();

  // Get the pointer to the first character of the message
  const byte* msg_bytes = (byte*) message.c_str();

  // Go through the message byte by byte
  for (int i = 0; i < msg_len; i++)
  {
    // Check if we have a partial message being populated
    if (_partial)
    {
      // Check if it is the correct second start character or any other character
      if (_index != 1 || (_index == 1 && msg_bytes[i] == PMS7003_START_CHAR_2))
      {
        // Put character in buffer
        _buffer[_index] = msg_bytes[i];

        // Increment message and Callback on the end of message if needed
        if (++_index == PMS7003_MESSAGE_LEN) { onMessage(); }
      }
      // There is a mistake we reset the buffer
      else { resetBuffer(); }
    }
    // We do not have a partial message yet, looking for the begining
    else
    {
      // Check if it could be begining of message
      if (msg_bytes[i] == PMS7003_START_CHAR_1)
      {
        // Initialize new message
        _buffer[0] = msg_bytes[i];
        _partial = true;
        _index = 1;
      }
    }
  }
}

// Reset the buffer
void ParticlePMS7003::resetBuffer()
{
  _index = 0;
  _partial = false;
}

void ParticlePMS7003::onMessage()
{
  // Initialize check sum
  uint16_t chk_sum = 0;

  // Compute the check sum
  for (int i = 0; i < PMS7003_CHK_INDEX; i++) { chk_sum += _buffer[i]; }

  // Parse useful data
  _pm1 = uint8To16(&_buffer[PMS7003_PM1_INDEX]);
  _pm2 = uint8To16(&_buffer[PMS7003_PM2_INDEX]);
  _pm10 = uint8To16(&_buffer[PMS7003_PM10_INDEX]);
  uint16_t chk_byte = uint8To16(&_buffer[PMS7003_CHK_INDEX]);

  // Perform check on the data integrity
  //if (chk_sum == chk_byte)
  {
    INFO_PF("message pm1:%u pm2.5:%u pm10:%u chk:%u/%u", _pm1, _pm2, _pm10, chk_sum, chk_byte);
  }

  // Reset the buffer
  resetBuffer();
}


}
