#ifndef _GPS_H
#define _GPS_H

#include<vector>

#include "Tools/Nmea.h"
#include "Tools/Uart.h"
#include "Sensor/Sensor.h"

// Update rate command
#define GPS_PMTK_UPDATE_1HZ  "$PMTK220,1000*1F\r\n"
#define GPS_PMTK_UPDATE_5HZ  "$PMTK220,200*2C\r\n"
#define GPS_PMTK_UPDATE_10HZ "$PMTK220,100*2F\r\n"

// Baud rate command
#define GPS_PMTK_BAUD_57600 "$PMTK251,57600*2C\r\n"
#define GPS_PMTK_BAUD_38400 "$PMTK251,38400*27\r\n"
#define GPS_PMTK_BAUD_19200 "$PMTK251,19200*22\r\n"
#define GPS_PMTK_BAUD_14400 "$PMTK251,14400*29\r\n"
#define GPS_PMTK_BAUD_9600  "$PMTK251,9600*17\r\n"

// Sentence command
#define GPS_PMTK_OUTPUT_ALL    "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
#define GPS_PMTK_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
#define GPS_PMTK_OUTPUT_RMC    "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n"
#define GPS_PMTK_OUTPUT_OFF    "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"

// Length of ack packet (for a quick check)
#define GPS_ACK_LEN 18

//================================== Adafruit_GPS ==============================
class Adafruit_GPS : public AsynchSensor
{
public:

  Adafruit_GPS(const LuaTable&);

  bool init();

  void onEvent(const UartPacket&);

  bool isAck(const UartPacket&);
  void onAck(const UartPacket&);

private:

  int _rate;

  GpsParser _gps_parser;
  GpsInfo _gps_info;
};

#endif
