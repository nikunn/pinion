#ifndef _SENSOR_H
#define _SENSOR_H


#include "Framework/Types.h"
#include "Framework/Factory.h"
#include "Framework/MuxPoll.h"
#include "Framework/MuxTimer.h"
#include "Framework/MuxSignal.h"
#include "Tools/UART.h"


namespace pno
{
//=========================== Forward Declarations =============================

class I2cWire;
class UartWire;
class DaqDevice;


//==================================== Sensor ==================================
class Sensor : public Accessible
{
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
class UartSensor : public Sensor, public PollListener
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


//================================== DigitalSensor =============================
class DigitalSensor : public Sensor, public PollListener
{
public :  //======================== Public ======================

  DigitalSensor(const LuaTable&);

  void onEvent(const PollEvent&) {};

private : //======================= Private ======================

  std::string _device;
  int _handle;
};


//===================================== Counter ================================
class Counter: public DigitalSensor
{
public :  //======================== Public ======================

  Counter(const LuaTable&);

  long long get() { return _count; }
  void onEvent(const PollEvent&);

private : //======================= Private ======================

  long long _count;
};


}
#endif
