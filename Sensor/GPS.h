#ifndef _GPS_H
#define _GPS_H

#include<vector>

#include "Sensor/Sensor.h"
#include "Tools/Nmea.h"

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

  void command(const CommandPacket&);
  bool init();
  void get();

  void onPacket(const std::string& packet);
  void onAck(const std::string& packet);
  void onNonAck(const CommandPacket& cmd);

private:

  int _max_packet_wait;
  std::vector<std::pair<CommandPacket, long> > _command_pending;

  int _rate;

  GpsParser _gps_parser;
  GpsInfo _gps_info;
};

#endif
