#ifndef _SENSOR_H
#define _SENSOR_H

#define GRAVITY 9.80665F

#define ASYNCH_PACKET_MAX_SIZE 255
#define ASYNCH_BUFFER_SIZE 2048


#include "Framework/Types.h"
#include "Framework/Factory.h"


//=========================== Forward Declarations =============================

class I2cWire;
class AsynchWire;
class DaqDevice;


//==================================== Sensor ==================================
class Sensor : public Accessible
{

public :

  //================================== AXIS ==================================
  // Enum containing the 3 axis
  enum AXIS
  {
    AXIS_X,
    AXIS_Y,
    AXIS_Z,
  }; 

  //============================== SensorVector ==============================
  // Struct contaning 3 dimensional vector
  struct SensorVector
  {
    float x;
    float y;
    float z;
  };

  //=============================== EulerAngle ===============================
  // Struct Containg the three Euler angle
  struct EulerAngle
  {
    // Rotation around the longitudinal axis (the plane body, 'X axis').
    // Roll is positive and increasing when moving downward. -90°<=roll<=90°
    float roll;

    // Rotation around the lateral axis (the wing span, 'Y axis').
    // Pitch is positive and increasing when moving upwards. -180°<=pitch<=180°)
    float pitch;

    // Angle between the longitudinal axis (the plane body) and magnetic north.
    // Measured clockwise when viewing from the top of the device. 0-359°
    float heading;
  };

public :

  //============================ public functions ============================
  // Default Constructor Destructor
  Sensor() {};
  ~Sensor() {};

  const DaqDevice* daqDevice() const { return _daq; }

protected :

  //========================== protected functions ===========================
  static int16_t int8To16(byte*);
  static int16_t int8To16(byte, byte);

  DaqDevice* _daq;
};


//=================================== I2cSensor ================================
class I2cSensor : public Sensor
{
public :

  //============================ public functions ============================
  const I2cWire* wire() const { return _wire; }
  byte address() const { return _address; }

protected :

  //========================== protected members ===========================
  byte _address;
  I2cWire* _wire;
};


//============================== CommandPacket =============================
struct CommandPacket
{
  int id;
  std::string command;

  // Default constructor
  CommandPacket() {};
  CommandPacket(const char* cmd, int no) : command(cmd), id(no) {};
};


//================================== AsynchSensor ==============================
class AsynchSensor : public Sensor
{
public :
  //============================ public functions ============================

          //======================= Accessor =====================
  long baud() const { return _baud; }
  void setBaud(const CommandPacket& cmd, uint32_t baud);

  const AsynchWire* wire() const { return _wire; }

          //==================== Communication ===================
  void start();
  void stop();

  void read();

  void startPacket();
  void endPacket();
  long packetId() { return _packet_id; }
  virtual void onPacket(const std::string& packet) = 0;

protected :

  //========================== protected members ===========================

  uint32_t _baud;

  char _begin_char;
  char _stop_char;
  char _end_char;

  long _packet_id;

  char _last_packet[ASYNCH_PACKET_MAX_SIZE];
  char _current_packet[ASYNCH_PACKET_MAX_SIZE];
  char* _current_char;

  AsynchWire* _wire;
};


#endif
