#ifndef __PMS7003_H__
#define __PMS7003_H__

#include<vector>

#include "Tools/UART.h"
#include "Sensor/Sensor.h"

// Start characters
#define PMS7003_START_CHAR_1 0x42
#define PMS7003_START_CHAR_2 0x4D

// Message and Buffer length
#define PMS7003_MESSAGE_LEN 32
#define PMS7003_BUFFER_LEN 48

// Particle concentration
#define PMS7003_LEN_INDEX 2
#define PMS7003_PM1_INDEX 10
#define PMS7003_PM2_INDEX 12
#define PMS7003_PM10_INDEX 14
#define PMS7003_CHK_INDEX 30

namespace pno
{
//================================ ParticlePMS7003 =============================
class ParticlePMS7003 : public UartSensor
{
public:

  ParticlePMS7003(const LuaTable&);

  void onEvent(const PollEvent&);

private:

  // Callback on end of message
  void onMessage();

  // Reset the buffer
  void resetBuffer();

  // Value for particule concentration
  uint16_t _pm1;
  uint16_t _pm2;
  uint16_t _pm10;

  // Partial message index
  int _index;

  // Partial message being populated flag
  bool _partial;

  // Buffer to aggregate partial messages
  byte _buffer[PMS7003_BUFFER_LEN];
};


}
#endif
