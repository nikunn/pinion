#include "Framework/LuaBind.h"
#include "Framework/Logger.h"
#include "Sensor/GPS.h"

//================================== Adafruit_GPS ==============================
// Constructor
Adafruit_GPS::Adafruit_GPS(const LuaTable& cfg) : AsynchSensor(cfg)
{
  // Get baud
  _baud = cfg.get<uint32_t>("start_baud");

  // Get rate
  _rate = cfg.get<int>("rate");

  // Check that the rate value is known
  if (_rate != 1 && _rate != 5 && _rate != 10)
  {
    FATAL_PF("GPS Cannot parse the rate:%u (1/5/10)", _rate);
  }

  // Number of packet before non ack
  _max_packet_wait = 10;

  // Begin and end character of all packet
  _begin_char = '$';
  _end_char = '\n';

  // Reset the asynch communication variables
  reset();
}

bool Adafruit_GPS::init()
{
  // Define a Command packet we will use later
  CommandPacket cmd;

  // Change the baud rate to 57600
  cmd = CommandPacket(GPS_PMTK_BAUD_57600, 251);
  setBaud(cmd, 57600);

  // Change the output to contain some NMEA sentence type
  cmd = CommandPacket(GPS_PMTK_OUTPUT_RMC, 314);
  command(cmd);

  // Change the output rate
  switch (_rate)
  {
    case 1:
      cmd = CommandPacket(GPS_PMTK_UPDATE_1HZ, 220); break;
    case 5:
      cmd = CommandPacket(GPS_PMTK_UPDATE_5HZ, 220); break;
    case 10:
      cmd = CommandPacket(GPS_PMTK_UPDATE_10HZ, 220); break;
    default:
      ERROR_LG("GPS rate is unknown");
  }

  // Send the command to change the output rate
  command(cmd);

  // Initialization went fine
  return true;
}

void Adafruit_GPS::command(const CommandPacket& cmd)
{
  // Add this command in the vector of pending command
  _command_pending.push_back(std::make_pair(cmd, packetId()));

  // Send the command to the sensor
  write(cmd);
}

void Adafruit_GPS::onPacket(const std::string& packet)
{
  // Log the received packet
  INFO_PF("%s", packet.c_str());

  // Check if this is a ack and call onAck
  if (packet.rfind(std::string("PMTK001")) != std::string::npos && packet.length() == GPS_ACK_LEN)
  {
    onAck(packet);
  }
  // Then this is a packet containing GPS info
  else
  {
    // Parse the new packet
    _gps_parser.parse(packet, _gps_info);

    /*
    INFO_PF("GPS position signal:%u latitude:%f longitude:%f speed:%f direction:%f",
      _gps_info.signal(), _gps_info.latitude(), _gps_info.longitude(),
      _gps_info.speed(), _gps_info.direction());
    */
  }

  // Get the last packet id
  long last_id = packetId();

  // Initialize iterator
  std::vector<std::pair<CommandPacket, long> >::iterator it = _command_pending.begin();

  // Go through the vector
  for ( ; it != _command_pending.end(); )
  {
    // Get the command packet
    const CommandPacket& cmd = (*it).first;

    if ((*it).second < last_id - _max_packet_wait)
    {
      // Call onNonAck
      onNonAck(cmd);

      // Erase this pending command
      it = _command_pending.erase(it);
    }
    else { ++it; }
  }
}

void Adafruit_GPS::onAck(const std::string& packet)
{
  // Get the command id
  int id = std::atoi(packet.substr(9, 3).c_str());
  
  // Get the command flag
  int flag = std::atoi(packet.substr(13, 1).c_str());

  for(std::vector<std::pair<CommandPacket, long> >::iterator it = _command_pending.begin();
    it != _command_pending.end(); ++it)
  {
    // Get the command packet
    const CommandPacket& cmd = (*it).first;

    // Check the command id
    if (cmd.id == id)
    {
      // If the command was not successful call onNonAck
      if (flag == !3)
      {
        onNonAck(cmd);
      }

      // Erase this command as we received an ack
      _command_pending.erase(it);

      // Break the for loop
      break;
    }
  }
}

// Called when no ack were received after a command
void Adafruit_GPS::onNonAck(const CommandPacket& cmd)
{
  WARNING_PF("Command non ack %u", cmd.id); 
}

// Get data from the sensor and process it
void Adafruit_GPS::get()
{
  read();
}
