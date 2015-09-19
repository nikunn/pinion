#include <sys/mman.h>

#include "Framework/Logger.h"
#include "Framework/Factory.h"
#include "Tools/IO.h"
#include "Acquisition/Daq.h"
#include "Acquisition/Pin.h"


namespace pno
{
//=================================== Gpio =====================================
Pin::Pin(const LuaTable& cfg)
{
  // Get the pin path
  _pin = cfg.get<std::string>("pin");
}

//=================================== Gpio =====================================
Gpio::Gpio(const LuaTable& cfg) : Pin(cfg)
{
  // Get the direction configuation
  std::string direction = cfg.get<std::string>("direction");

  // Check direction value
  if (direction != "in" && direction != "out")
  {
    FATAL_PF("GPIO %s config, direction is undefined: %s", pin().c_str(), direction.c_str());
  }

  // Set the direction
  if (IoLinux::write(pin() + GPIO_DIRECTION, direction) < 0)
  {
    FATAL_PF("Cannot set the direction %s for GPIO %s", direction.c_str(), pin().c_str());
  }

  // Get the edge configuration
  std::string edge = cfg.get<std::string>("edge");

  // Check edge value
  if (edge != "rising" && edge != "falling" && edge != "both" && edge != "none")
  {
    FATAL_PF("GPIO %s config, edge is undefined: %s", pin().c_str(), edge.c_str());
  }

  // Set the edge
  if (IoLinux::write(pin() + GPIO_EDGE, edge) < 0)
  {
    FATAL_PF("Cannot set the edge %s for GPIO %s", edge.c_str(), pin().c_str());
  }

  // A bit of log
  INFO_PF("GPIO initialization %s, direction:%s, edge:%s"
    , pin().c_str(), direction.c_str(), edge.c_str());
}

//================================= GpioFile ===================================
GpioFile::GpioFile(const LuaTable& cfg) : Gpio(cfg)
{
  // Get the handle on the pin value
  _handle = IoLinux::openFile(pin() + "/" + GPIO_VALUE);

  // A bit of log
  INFO_PF("GPIO file %s handle %u", pin().c_str(), _handle);
}

// Destructor
GpioFile::~GpioFile()
{
  // Close value file
  IoLinux::closeFile(handle());
}

//================================== GpioBank ==================================
GpioBank::GpioBank(const LuaTable& cfg) : Accessible(cfg)
{
  // Get file
  std::string file = cfg.get<std::string>("file");

  // Get address
  uintptr_t address = cfg.get<uintptr_t>("address");

  // Open file
  int gpio_handle = IoLinux::openFile(file, O_RDWR | O_SYNC);

  // Map memory
  void* _map_base =
    mmap(0, GPIO_MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, gpio_handle, address);


  // Check the mapping is done
  if(_map_base == MAP_FAILED)
  {
    FATAL_PF("Could not map address %X", address);
  }

  // Compute the virtual address
  _virt_addr = (uintptr_t) _map_base;

  // A bit of log
  INFO_PF("%s created, address %X memory mapped at %X", name().c_str(), address, _virt_addr);

  // Close file
  IoLinux::closeFile(gpio_handle);
};

// Destructor
GpioBank::~GpioBank()
{
  // Unmap the previously mapped memory
  if(munmap(_map_base, GPIO_MAP_SIZE) == -1)
  {
    ERROR_PF("Cannot unmap memory %X", _map_base);
  }
}

// Read Gpio
bool GpioBank::read(const uint32_t mask) const
{
  // Compute the register address to access for read
  uintptr_t read_addr = _virt_addr + GPIO_READ_REG;

  // Read the value
  uint32_t val = *((uint32_t *) read_addr);

  // Apply the mask
  return (val & mask) != 0;
}

// Write GPIO
void GpioBank::write(const uint32_t mask, const bool val) const
{
  // Compute the register address to access for write
  uintptr_t write_addr = _virt_addr + (val ? GPIO_WRITE_HIGH_REG : GPIO_WRITE_LOW_REG);

  // Write the data
  *((uint32_t *) write_addr) = mask;
}

//================================== GpioMem ===================================
GpioMem::GpioMem(const LuaTable& cfg) : Accessible(cfg), Gpio(cfg)
{
  // Set Bank
  std::string bank = cfg.get<std::string>("bank");
  _bank = static_cast<GpioBank*>(Factory::get(bank));

  // Get GPIO number on this bank
  _gpio = cfg.get<uint8_t>("gpio");

  // Compute the mask
  _mask = 1 << _gpio;
};


}
