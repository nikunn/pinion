#include "Framework/LuaBind.h"
#include "Framework/Logger.h"
#include "Sensor/GPS.h"

//================================== Adafruit_GPS ==============================
// Constructor
Adafruit_GPS::Adafruit_GPS(const LuaTable& cfg) : AsynchSensor(cfg)
{
  // Get baud
  _baud = cfg.get<long>("start_baud");

  // Get rate
  _rate = cfg.get<int>("rate");

  // Check that the rate value is known
  if (_rate != 1 && _rate != 5 && _rate != 10)
  {
    FATAL_PF("GPS Cannot parse the rate:%u (1/5/10)", _rate);
  }
}

bool Adafruit_GPS::init()
{
  // Define a Command packet we will use later
  UartPacket cmd;

  // Change the baud rate to 57600
  cmd = UartPacket(GPS_PMTK_BAUD_57600, 251);
  changeBaud(cmd, 57600);

  // Change the output to contain some NMEA sentence type
  cmd = UartPacket(GPS_PMTK_OUTPUT_RMC, 314);
  write(cmd);

  // Change the output rate
  switch (_rate)
  {
    case 1:
      cmd = UartPacket(GPS_PMTK_UPDATE_1HZ, 220); break;
    case 5:
      cmd = UartPacket(GPS_PMTK_UPDATE_5HZ, 220); break;
    case 10:
      cmd = UartPacket(GPS_PMTK_UPDATE_10HZ, 220); break;
    default:
      ERROR_LG("GPS rate is unknown");
  }

  // Send the command to change the output rate
  write(cmd);

  // Initialization went fine
  return true;
}

// Received an event, a full packet
void Adafruit_GPS::onEvent(const UartPacket& packet)
{
  // Get the message
  const std::string& message = packet.message;

  // Log the received message
  INFO_PF("%s", message.c_str());

  // Check if this is a ack and call onAck
  if (isAck(packet))
  {
    onAck(packet);
  }
  // Then this is a message containing GPS info
  else
  {
    // Parse the new message
    _gps_parser.parse(message, _gps_info);

    /*
    INFO_PF("GPS position signal:%u latitude:%f longitude:%f speed:%f direction:%f",
      _gps_info.signal(), _gps_info.latitude(), _gps_info.longitude(),
      _gps_info.speed(), _gps_info.direction());
    */
  }
}

// Check whether a packet is an ack
bool Adafruit_GPS::isAck(const UartPacket& packet)
{
  // Get the message
  const std::string& message = packet.message;

  if (message.rfind(std::string("PMTK001")) != std::string::npos
      && message.length() == GPS_ACK_LEN)
  {
    return true;
  }
  else
  {
    return false;
  }
}

// Action when receiving ack
void Adafruit_GPS::onAck(const UartPacket& packet)
{
  // Get the message
  const std::string& message = packet.message;

  // Get the command id
  int cmd_id = std::atoi(message.substr(9, 3).c_str());
  
  // Get the command flag
  int flag = std::atoi(message.substr(13, 1).c_str());

  // Log the received message
  INFO_PF("GPS received an ack commad_id:%u flag:%u", cmd_id, flag);
}
