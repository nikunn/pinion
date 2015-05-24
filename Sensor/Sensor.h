#ifndef _SENSOR_H
#define _SENSOR_H

#define GRAVITY 9.80665F


#include "Framework/Types.h"
#include "Framework/Factory.h"
#include "Framework/MuxPoll.h"
#include "Framework/MuxTimer.h"
#include "Framework/MuxSignal.h"
#include "Tools/Uart.h"


//=========================== Forward Declarations =============================

class I2cWire;
class UartWire;
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

public :  //======================== Public ======================

  Sensor(const LuaTable&);
  DaqDevice& daq() const { return *_daq; }

private : //======================= Private ======================

  DaqDevice* _daq;
};


//=================================== I2cSensor ================================
class I2cSensor : public Sensor, public TimerListener
{
public :  //======================== Public ======================

  I2cSensor(const LuaTable&);

  const byte address() const { return _address; }
  const I2cWire* wire() const { return _wire; }

  void read(const byte regis, byte* data, const int bytes_num = 1);
  void write(const byte regis, byte* data, const int bytes_num = 1);

  virtual void get() = 0;
  void onEvent(const TimerEvent&) { get(); }

protected : //===================== Protected ====================

  byte _address;

private : //======================= Private ======================

  I2cWire* _wire;
};


//=================================== UartSensor ===============================
class UartSensor : public Sensor
{
public :  //======================== Public ======================

  UartSensor(const LuaTable&);

          //======================= Accessor =====================
  long baud() const { return _baud; }
  void changeBaud(const UartPacket& cmd, const long baud);

  const UartWire* wire() const { return _wire; }

          //==================== Communication ===================
  void write(const UartPacket&);
  virtual void read() {};

protected : //===================== Protected ====================

  long _baud;

private : //======================= Private ======================
  UartWire* _wire;
};


//================================== SerialSensor ==============================
class SerialSensor : public UartSensor
{
public :  //======================== Public ======================

  SerialSensor(const LuaTable&);

          //==================== Communication ===================
  void reset();
  void read();

  void startPacket();
  void endPacket();
  virtual void onPacket(const UartPacket&) = 0;

protected : //===================== Protected ====================

  long _baud;

  char _begin_char;
  char _stop_char;
  char _end_char;

  char _last_packet[UART_PACKET_SIZE];
  char _current_packet[UART_PACKET_SIZE];
  char* _current_char;
};


//================================== SignalSensor ==============================
class SignalSensor : public UartSensor, public SignalListener
{
public :  //======================== Public ======================

  SignalSensor(const LuaTable&);
};


//=================================== PollSensor ===============================
class PollSensor : public UartSensor, public PollListener
{
public :  //======================== Public ======================

  PollSensor(const LuaTable&);
};


#endif
