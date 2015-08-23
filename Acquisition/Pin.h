#ifndef _PIN_H
#define _PIN_H

#include "Framework/Accessible.h"
#include "Framework/LuaBind.h"


namespace pno
{
//=========================== Forward Declarations =============================

class DaqDevice;


//=================================== Gpio =====================================
class Gpio
{
public :
  Gpio(const LuaTable&);

  int handle() { return _handle; }

private :
  std::string _pin;
  int _handle;
};


}
#endif
