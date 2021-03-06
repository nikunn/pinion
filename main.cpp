#include <stdio.h>
#include <unistd.h>
#include <thread>

#include "Framework/LuaBind.h"
#include "Framework/Logger.h"
#include "Framework/Factory.h"
#include "Framework/Universe.h"
#include "Framework/StopWatch.h"
#include "Framework/MuxTimer.h"
#include "Framework/MuxPoll.h"

// Include the data acquisition device file
#include "Acquisition/LinuxBoardDaq.cpp"

#include "Sensor/All.h"
#include "Actuator/Actuator.h"

// Use 'pno' namespace
using namespace pno;

//=================================== Main =====================================
int main()
{
  //================================ Factory =================================
  // Register all the classes in the factory
  // Acquisition
  Factory::registerClass("LinuxBoard", new CreatorType<LinuxBoardDaq>);

  // Wire
  Factory::registerClass("I2cWire", new CreatorType<I2cWire>);
  Factory::registerClass("SpiWire", new CreatorType<SpiWire>);
  Factory::registerClass("UartWire", new CreatorType<UartWire>);

  // GPIO
  Factory::registerClass("GpioBank", new CreatorType<GpioBank>);
  Factory::registerClass("GpioMem", new CreatorType<GpioMem>);

  // Actuator
  Factory::registerClass("Pulser", new CreatorType<Pulser>);

  // Sensor
  Factory::registerClass("GPS", new CreatorType<GpsFGP>);
  Factory::registerClass("PMS7003", new CreatorType<ParticlePMS7003>);
  Factory::registerClass("LSM303", new CreatorType<AccelLSM303>);
  Factory::registerClass("ADXL345", new CreatorType<AccelADXL345>);
  Factory::registerClass("MAG303", new CreatorType<MagnetoMAG303>);
  Factory::registerClass("L3GD20", new CreatorType<GyroL3GD20>);
  Factory::registerClass("Counter", new CreatorType<Counter>);

  //================================= Init ===================================
  // Start an instance of epoll
  MuxPoll::initialize();

  // Read the configuration file
  LuaScript script("config.lua");

  // Create a lua state based on the configuration
  const LuaState& lua_state = script.get();

  // Get the logger configuration
  LuaTable logger_config = lua_state.get<LuaTable>("Logger");

  // Configure the default logger
  Logger::configure(logger_config);

  // Create the universe
  Universe::createUniverse(lua_state);

  // Get the data acquisition device
  LinuxBoardDaq* daq = static_cast<LinuxBoardDaq*>(Factory::get("UXB"));
  
  //================================= Main ===================================
  std::thread poll_thread(MuxPoll::start);
  poll_thread.detach();

  // ==================== PARTICLE ===================
  ParticlePMS7003* pcs = static_cast<ParticlePMS7003*>(Factory::get("PMS7003"));
  sleep(900000);

  // ==================== PULSER  ====================
  /*
  Pulser* pwm = static_cast<Pulser*>(Factory::get("Pulser"));
  Counter* cnt = static_cast<Counter*>(Factory::get("Counter"));

  std::thread poll_thread(MuxPoll::start);
  poll_thread.detach();

  sleep(1);
  pwm->start();
  sleep(1);
  pwm->stop();

  INFO_PF("Total count: %u", cnt->get());
  */

  // ==================== SENSOR  ====================
  /*
  // GpsFGP* gps = static_cast<GpsFGP*>(Factory::get("GPS"));
  // gps->init();

  // AccelLSM303* accel = static_cast<AccelLSM303*>(Factory::get("AccelLSM"));
  // AccelADXL345* accel = static_cast<AccelADXL345*>(Factory::get("AccelADXL"));
  // MagnetoMAG303* magneto = static_cast<MagnetoMAG303*>(Factory::get("Magneto"));
  GyroL3GD20* gyro = static_cast<GyroL3GD20*>(Factory::get("Gyro"));

  if (gyro->init())
  {
    std::thread timer_thread(MuxTimer::start);
    timer_thread.detach();
  }
  else { INFO_LG("sensor init failed"); }

  std::thread poll_thread(MuxPoll::start);
  poll_thread.detach();
  sleep(20);


  GpioMem* gpio_44 = static_cast<GpioMem*>(Factory::get("Gpio44"));
  GpioMem* gpio_61 = static_cast<GpioMem*>(Factory::get("Gpio61"));

  int counter = 0;
  bool status = true;

  while (counter < 100)
  {
    status = !status;
    gpio_44->write(status);

    counter++;
  }
  */

  return 0;
}
