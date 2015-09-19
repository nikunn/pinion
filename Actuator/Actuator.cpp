#include "Framework/Logger.h"
#include "Acquisition/Daq.h"
#include "Actuator/Actuator.h"


namespace pno
{
//=================================== Actuator =================================
Actuator::Actuator(const LuaTable& cfg) : Accessible(cfg)
{
  // Set Data Acquisition device
  std::string daq_name = cfg.get<std::string>("daq");
  _daq = static_cast<DaqDevice*>(Factory::get(daq_name));
}

//=================================== Pulser ===================================
Pulser::Pulser(const LuaTable& cfg) : Actuator(cfg)
{
  // Set pin path
  _pin = cfg.get<std::string>("pin") + "/";

  // Get the polarity
  _config.polarity = cfg.get<bool>("polarity");

  // Get frequency in Hz
  const float frequency = cfg.get<float>("frequency");

  // Compute the period in nanosecond
  _config.period = 1.0f / frequency * 1e9 + 0.5f;

  // Get duty cycle percentage
  const float duty_pct = cfg.get<float>("duty_cycle");

  // Check duty cycle is a percentage
  if (duty_pct < 0 || duty_pct > 1)
  {
    FATAL_PF("Duty cycle is not a percentage for pin %s", _pin.c_str());
  }

  // Compute the duty cycle in nanosecond
  _config.duty = duty_pct * _config.period;

  // Initialize the PWM pin
  daq().pwmInit(_pin, _config);
}

// Start the pulser
void Pulser::start() const
{
  daq().pwmSetStatus(_pin, true);
}

// Stop the pulser
void Pulser::stop() const
{
  daq().pwmSetStatus(_pin, false);
}

// Get the status
bool Pulser::status() const
{
  return daq().pwmGetStatus(_pin);
}


}
