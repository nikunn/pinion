#ifndef _GPIO_LINUX_H
#define _GPIO_LINUX_H

#include "Framework/Types.h"

#define GPIO_DIRECTION   "direction"
#define GPIO_VALUE       "value"
#define GPIO_EDGE        "edge"

namespace pno
{
//================================= GpioLinux ==================================
class GpioLinux
{
public :  //======================== Public ======================
  static int gpioInit(const std::string& pin, const GpioConfig& gpio);
};


}
#endif
