#ifndef _NMEA_H
#define _NMEA_H

#include <nmea/nmea.h>
#include <string>


namespace pno
{
//================================== GpsInfo ===================================
class GpsInfo
{
public:

  //============================ public functions ============================
  GpsInfo();
  ~GpsInfo();

  void reset();

  nmeaINFO& info() { return _info; }

  int signal() { return _info.sig; }
  double latitude() { return _info.lat; }
  double longitude() { return _info.lon; }
  double direction() { return _info.direction; }
  double speed() { return _info.speed; }

private:

  //=========================== private members ============================
  nmeaINFO _info;
};

//================================= GpsParser ==================================
class GpsParser
{
public:

  //============================ public functions ============================
  GpsParser();
  ~GpsParser();

  void parse(const std::string&, GpsInfo&);

private:

  //=========================== private members ============================
  nmeaPARSER _parser;
};


}
#endif
