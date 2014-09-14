#include "Tools/Nmea.h"

//================================= GpsParser ==================================
// Constructor
GpsParser::GpsParser()
{
  // Initialize the parser
  nmea_parser_init(&_parser);
}

// Destructor
GpsParser::~GpsParser()
{
  // Destroy parser
  nmea_parser_destroy(&_parser);
}

void GpsParser::parse(const std::string& packet, GpsInfo& info)
{
  // Create a new GPS info
  GpsInfo last;

  // Reset the info
  last.reset();

  // Append return and end of line character
  std::string message = (packet.substr(0, packet.length() - 1)).append("\r\n");

  // Parse the NMEA packet
  nmea_parse(&_parser, message.c_str(), message.length(), &last.info());

  // Check if the packet is valid
  if (last.signal() >= 2)
  {
    // Push the last to become the current GPS info
    info = last;
  }
}


//================================== GpsInfo ===================================
// Constructor
GpsInfo::GpsInfo()
{
  // Reset the GPS info
  reset();
}

// Destructor
GpsInfo::~GpsInfo()
{
}

void GpsInfo::reset()
{
  // Reset the GPS info
  nmea_zero_INFO(&_info);
}
