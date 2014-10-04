#ifndef _WIRE_H
#define _WIRE_H

#include "Framework/Accessible.h"
#include "Framework/LuaBind.h"
#include "Tools/I2C.h"
#include "Tools/Uart.h"

//=========================== Forward Declarations =============================

class DaqDevice;


//=================================== Wire =====================================
class Wire : public Accessible
{
public :
  Wire(const LuaTable&);

  int handle() const { return _handle; }

  DaqDevice& daq() const { return *_daq; }
  const std::string& device() const { return _device; }

protected :
  int _handle;

private :
  DaqDevice* _daq;
  std::string _device;
};


//================================== I2CWire ===================================
class I2cWire : public Wire
{
public :
  I2cWire(const LuaTable&);
  ~I2cWire();

  int dataLine() const { return _data_line; }
  int clockLine() const { return _clock_line; }

private :
  int _data_line;
  int _clock_line;
};


//================================== SPIWire ===================================
class SpiWire : public Wire
{
public :
  SpiWire(const LuaTable&);

  int clockLine() const { return _clock_line; }
  int mosiLine() const { return _mosi_line; }
  int misoLine() const { return _miso_line; }
  int slaveLine() const { return _slave_line; }

private :
  int _clock_line;
  int _mosi_line;
  int _miso_line;
  int _slave_line;
};


//================================= UartWire ===================================
class UartWire : public Wire
{
public :
  UartWire(const LuaTable&);
  ~UartWire();

  int transmitLine() const { return _transmit_line; }
  int receiveLine() const { return _receive_line; }

  UartCom::Type comType() const { return _com_type; }
  long defaultBaud() const { return _default_baud; }

private :
  UartCom::Type _com_type;
  int _transmit_line;
  int _receive_line;

  long _default_baud;
};



#endif
