#ifndef _WIRE_H
#define _WIRE_H

#include "Framework/Accessible.h"
#include "Framework/LuaBind.h"
#include "Framework/Types.h"
#include "Tools/I2C.h"
#include "Tools/SPI.h"
#include "Tools/UART.h"


namespace pno
{
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
};


//================================== SPIWire ===================================
class SpiWire : public Wire
{
public :
  SpiWire(const LuaTable&);
  ~SpiWire();

  uint8_t& mode() { return _mode; }
  uint32_t& speed() { return _speed; }
  uint8_t& wordBits() { return _word_bits; }

  const uint32_t& speed() const { return _speed; }
  const uint8_t& wordBits() const { return _word_bits; }

private :
  uint8_t _mode;
  uint32_t _speed;
  uint8_t _word_bits;
};


//================================= UartWire ===================================
class UartWire : public Wire
{
public :
  UartWire(const LuaTable&);
  ~UartWire();

  long defaultBaud() const { return _default_baud; }

private :
  long _default_baud;
};


}
#endif
