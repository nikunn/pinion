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
#if defined __DEVICE_LAB__
#include "Acquisition/LabJackDaq.cpp"
#elif defined __DEVICE_UXB__
#include "Acquisition/LinuxBoardDaq.cpp"
#endif

#include "Sensor/All.h"

//=================================== Main =====================================
int main()
{
  // Start an instance of epoll
  MuxPoll::initialize();

  // Register all the classes in the factory
  #if defined __DEVICE_LAB__
  Factory::registerClass("LabJack", new CreatorType<LabJackDaq>);
  #elif defined __DEVICE_UXB__
  Factory::registerClass("LinuxBoard", new CreatorType<LinuxBoardDaq>);
  #endif
  Factory::registerClass("UartWire", new CreatorType<UartWire>);
  Factory::registerClass("I2cWire", new CreatorType<I2cWire>);
  Factory::registerClass("GPS", new CreatorType<GpsFGP>);
  Factory::registerClass("LSM303", new CreatorType<AccelLSM303>);
  Factory::registerClass("MAG303", new CreatorType<MagnetoMAG303>);
  Factory::registerClass("L3GD20", new CreatorType<GyroL3GD20>);

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

  #if defined __DEVICE_LAB__
  LabJackDaq* daq = static_cast<LabJackDaq*>(Factory::get("LAB"));
  #elif defined __DEVICE_RPI__
  LinuxBoardDaq* daq = static_cast<LinuxBoardDaq*>(Factory::get("UXB"));
  #endif

  GpsFGP* gps = static_cast<GpsFGP*>(Factory::get("GPS"));
  gps->init();

  AccelLSM303* accel = static_cast<AccelLSM303*>(Factory::get("Accel"));
  MagnetoMAG303* magneto = static_cast<MagnetoMAG303*>(Factory::get("Magneto"));
  GyroL3GD20* gyro = static_cast<GyroL3GD20*>(Factory::get("Gyro"));

  if (gyro->init() && accel->init() && magneto->init())
  {
    std::thread timer_thread(MuxTimer::start);
    timer_thread.detach();
  }
  else { INFO_LG("I2C sensor init failed"); }

  std::thread poll_thread(MuxPoll::start);
  poll_thread.detach();

  sleep(20);
  return 0;
}
