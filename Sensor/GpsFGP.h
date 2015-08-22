#ifndef __FGP_H__
#define __FGP_H__

#include<vector>

#include "Tools/NMEA.h"
#include "Tools/UART.h"
#include "Sensor/Sensor.h"

// Update rate command
#define FGP_PMTK_UPDATE_1HZ  "$PMTK220,1000*1F\r\n"
#define FGP_PMTK_UPDATE_5HZ  "$PMTK220,200*2C\r\n"
#define FGP_PMTK_UPDATE_10HZ "$PMTK220,100*2F\r\n"

// Baud rate command
#define FGP_PMTK_BAUD_57600 "$PMTK251,57600*2C\r\n"
#define FGP_PMTK_BAUD_38400 "$PMTK251,38400*27\r\n"
#define FGP_PMTK_BAUD_19200 "$PMTK251,19200*22\r\n"
#define FGP_PMTK_BAUD_14400 "$PMTK251,14400*29\r\n"
#define FGP_PMTK_BAUD_9600  "$PMTK251,9600*17\r\n"

// Sentence command
#define FGP_PMTK_OUTPUT_ALL    "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
#define FGP_PMTK_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
#define FGP_PMTK_OUTPUT_RMC    "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n"
#define FGP_PMTK_OUTPUT_OFF    "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"

// Length of ack packet (for a quick check)
#define FGP_ACK_LEN 18


namespace pno
{
//===================================== GpsFGP =================================
class GpsFGP : public UartSensor
{
public:

  GpsFGP(const LuaTable&);

  bool init();

  void onEvent(const PollEvent&);

  bool isAck(const UartPacket&);
  void onAck(const UartPacket&);

private:

  int _rate;

  GpsParser _gps_parser;
  GpsInfo _gps_info;
};


}
#endif
