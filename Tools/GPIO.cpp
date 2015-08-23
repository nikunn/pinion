#include "Framework/Logger.h"
#include "Tools/GPIO.h"
#include "Tools/IO.h"


namespace pno
{
// Initialize GPIO pin
int GpioLinux::gpioInit(const std::string& pin, const GpioConfig& gpio)
{
  // Set the direction
  if (IoLinux::write(pin + GPIO_DIRECTION, gpio.direction) < 0)
  {
    FATAL_PF("Cannot set the direction %s for GPIO %s", gpio.direction.c_str(), pin.c_str());
  }

  // Set the edge
  if (IoLinux::write(pin + GPIO_EDGE, gpio.edge) < 0)
  {
    FATAL_PF("Cannot set the edge %s for GPIO %s", gpio.edge.c_str(), pin.c_str());
  }

  // Get the handle on the pin value
  int handle = IoLinux::openFile(pin + "/" + GPIO_VALUE);

  // A bit of log
  INFO_PF("GPIO initialization %s, handle:%u, direction:%s, edge:%s"
    , pin.c_str(), handle, gpio.direction.c_str(), gpio.edge.c_str());

  // Return handle
  return handle;
}


}
