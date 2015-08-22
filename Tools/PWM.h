#ifndef _PWM_LINUX_H
#define _PWM_LINUX_H

#include "Framework/Types.h"

#define PWM_PERIOD "period"
#define PWM_DUTY "duty"
#define PWM_POLARITY "polarity"
#define PWM_RUN "run"

namespace pno
{
//================================== PwmLinux ==================================
class PwmLinux
{
public :  //======================== Public ======================
  static void pwmInit(const std::string pin, const pwmConfig& pwm);
  static void pwmSetStatus(const std::string& pin, const bool status);
  static bool pwmGetStatus(const std::string& pin);
};


}
#endif
