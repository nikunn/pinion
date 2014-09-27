#ifndef _WIRE_H
#define _WIRE_H

#include "Framework/Accessible.h"
#include "Framework/LuaBind.h"

//=================================== Wire =====================================
class Wire : public Accessible
{
public:

  int handle() const { return _handle; }

protected:

  int _handle;
};


//================================== I2CWire ===================================
class I2cWire : public Wire
{
public :
  I2cWire(const LuaTable& cfg);
  ~I2cWire();

  int handle() const { return _handle; }
  int dataLine() const { return _data_line; }
  int clockLine() const { return _clock_line; }

private :
  int _data_line;
  int _clock_line;

  int _handle;
  int _bus;
};


//================================== SPIWire ===================================
class SpiWire : public Wire
{
public :
  SpiWire(const LuaTable& cfg);

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


//================================ AsynchWire ==================================
class AsynchWire : public Wire
{
public :
  AsynchWire(const LuaTable& cfg);

  int transmitLine() const { return _transmit_line; }
  int receiveLine() const { return _receive_line; }

private :
  int _transmit_line;
  int _receive_line;
};



#endif
