#include "Framework/Logger.h"
#include "Framework/Factory.h"
#include "Acquisition/Daq.h"
#include "Acquisition/Pin.h"


namespace pno
{
//=================================== Gpio =====================================
Gpio::Gpio(const LuaTable& cfg)
{
  // Set Data Acquisition device
  std::string daq_name = cfg.get<std::string>("daq");
  DaqDevice* daq = static_cast<DaqDevice*>(Factory::get(daq_name));

  // Get the gpio pin
  _pin = cfg.get<std::string>("pin") + "/";

  // Create GPIO configuration
  GpioConfig gpio(cfg.get<std::string>("direction"), cfg.get<std::string>("edge"));

  // Initialize the GPIO
  _handle = daq->gpioInit(_pin, gpio);
}


}
