#ifndef _PIN_H
#define _PIN_H

// This is an altered version of Jan-Derk Bakker's http://www.lart.tudelft.nl

#include "Framework/Accessible.h"
#include "Framework/LuaBind.h"

// GPIO file
#define GPIO_DIRECTION   "direction"
#define GPIO_VALUE       "value"
#define GPIO_EDGE        "edge"

// Default map size
#define GPIO_MAP_SIZE 4096UL

// Define register for reading and writing
#define GPIO_READ_REG         0x13C
#define GPIO_WRITE_LOW_REG    0x190
#define GPIO_WRITE_HIGH_REG   0x194

namespace pno
{
//=========================== Forward Declarations =============================

class DaqDevice;


//==================================== Pin =====================================
class Pin
{
public :
  Pin(const LuaTable&);

  const std::string& pin() const { return _pin; }

private :
  std::string _pin;
};


//=================================== Gpio =====================================
class Gpio : public Pin
{
public :
  Gpio(const LuaTable&);
};


//================================= GpioFile ===================================
class GpioFile : public Gpio
{
public :
  GpioFile(const LuaTable&);
  ~GpioFile();

  int handle() { return _handle; }

private :
  int _handle;
};


//================================= GpioBank ===================================
class GpioBank : public Accessible
{
public :
  GpioBank(const LuaTable&);
  ~GpioBank();

  bool read(const uint32_t mask) const;
  void write(const uint32_t mask, const bool val) const;

private :
  void* _map_base;
  uintptr_t _virt_addr;
};


//================================== GpioMem ===================================
class GpioMem : public Accessible, public Gpio
{
public :
  GpioMem(const LuaTable&);

  GpioBank* bank() const { return _bank; }
  uint32_t mask() const { return _mask; }

  bool read() const { return _bank->read(_mask); }
  void write(const bool val) const { _bank->write(_mask, val); }

private :
  GpioBank* _bank;
  uint8_t _gpio;
  uint32_t _mask;
};


}
#endif
