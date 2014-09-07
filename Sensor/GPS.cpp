#include "Sensor/GPS.h"
#include "Sensor/Sensor.cpp" // SALE

#include "Framework/LuaBind.h"
#include "Framework/Factory.h"

//================================== Adafruit_GPS ==============================
// Constructor
Adafruit_GPS::Adafruit_GPS(const LuaTable& cfg)
{
  // Get baud
  _baud = cfg.get<uint32_t>("start_baud");

  // Set Data Acquisition device
  std::string daq_name = cfg.get<std::string>("device");
  _daq = static_cast<DaqDevice*>(Factory::get(daq_name));

  // Set Wire
  std::string wire_name = cfg.get<std::string>("wire");
  _wire = static_cast<AsynchWire*>(Factory::get(wire_name));

  // Number of packet before non ack
  _max_packet_wait = 10;

  // Begin and end character of all packet
  _begin_char = '$';
  _end_char = '\n';
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

  // Change the output rate to 10HZ
  cmd = CommandPacket(GPS_PMTK_UPDATE_10HZ, 220);
  command(cmd);

  // Initialization went fine
  return true;
}

void Adafruit_GPS::setBaud(const CommandPacket& cmd, uint32_t baud)
{
  // Send the command to change the sensor baud rate
  command(cmd);

  // Update the sensor baud rate value
  _baud = baud;
}

void Adafruit_GPS::command(const CommandPacket& cmd)
{
  // Add this command in the vector of pending command
  _command_pending.push_back(std::make_pair(cmd, packetId()));

  // Send the command to the sensor
  _daq->asynchWrite(cmd);

  // Call the get function as we may received the ack quickly
  get();
}

void Adafruit_GPS::onPacket(const std::string& packet)
{
  #ifdef _DEBUG_SENSOR_
  // Log the received packet
  printf("%s\n", packet.c_str());
  #endif

  // Check if this is a ack and call onAck
  if (packet.rfind(std::string("PMTK001")) != std::string::npos && packet.length() == GPS_ACK_LEN)
  {
    onAck(packet);
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
  printf("non ack %u\n", cmd.id); 
}

// Get data from the sensor and process it
void Adafruit_GPS::get()
{
  read();
}
