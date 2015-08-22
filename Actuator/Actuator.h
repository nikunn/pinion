#ifndef _ACTUATOR_H
#define _ACTUATOR_H

#include "Framework/Types.h"
#include "Framework/Factory.h"


namespace pno
{
//=========================== Forward Declarations =============================

class DaqDevice;


//=================================== Actuator =================================
class Actuator : public Accessible
{
public :  //======================== Public ======================

  Actuator(const LuaTable&);
  DaqDevice& daq() const { return *_daq; }

private : //======================= Private ======================

  DaqDevice* _daq;
};


//=================================== Pulser ===================================
class Pulser : public Actuator
{
public :  //======================== Public ======================

  Pulser(const LuaTable&);
  ~Pulser() { stop(); }

  void start() const;
  void stop() const;
  bool status() const;


private : //======================= Private ======================

  // Path to pin
  std::string _pin;

  // PWM configuration
  pwmConfig _config;
};


}
#endif
