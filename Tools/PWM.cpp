#include "Framework/Logger.h"
#include "Tools/PWM.h"
#include "Tools/IO.h"


namespace pno
{
// Initialize PWM pin
void PwmLinux::pwmInit(const std::string pin, const pwmConfig& pwm)
{
  // Set the polarity
  if (IoLinux::write(pin + PWM_POLARITY, pwm.polarity) < 0)
  {
    FATAL_PF("Cannot set the polarity %u for PWM %s", pwm.polarity, pin.c_str());
  }

  // Set the period
  if (IoLinux::write(pin + PWM_PERIOD, pwm.period) < 0)
  {
    FATAL_PF("Cannot set the period %u for PWM %s", pwm.period, pin.c_str());
  }

  // Set the duty cycle
  if (IoLinux::write(pin + PWM_DUTY, pwm.duty) < 0)
  {
    FATAL_PF("Cannot set the duty cycle %u for PWM %s", pwm.duty, pin.c_str());
  }

  // A bit of log
  INFO_PF("PWM initialization %s, polarity:%u, period:%u, duty:%u"
    , pin.c_str(), pwm.polarity, pwm.period, pwm.duty);
}

// Change status of PWM
void PwmLinux::pwmSetStatus(const std::string& pin, const bool status)
{
  // Change the status of PWM and check
  if (IoLinux::write(pin + PWM_RUN, status ? 1 : 0) < 0)
  {
    // Dump an error log
    ERROR_PF("Cannot change the status for PWM %s", pin.c_str());
  }
}

// Get current status of PWM
bool PwmLinux::pwmGetStatus(const std::string& pin)
{
  // Read the current status
  std::string running = IoLinux::read(pin + PWM_RUN);

  // Return result
  return (running == "1");
}


}
