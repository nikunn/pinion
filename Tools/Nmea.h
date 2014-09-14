#include <nmea/nmea.h>
#include <string>

//================================== GpsInfo ===================================
class GpsInfo
{
public:
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
  nmeaINFO _info;
};

//================================= GpsParser ==================================
class GpsParser
{
public:
  GpsParser();
  ~GpsParser();

  void parse(const std::string&, GpsInfo&);

private:
  nmeaPARSER _parser;
};
