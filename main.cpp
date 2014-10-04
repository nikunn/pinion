#include <stdio.h>
#include <unistd.h>

#include "Framework/LuaBind.h"
#include "Framework/Logger.h"
#include "Framework/Factory.h"
#include "Framework/Universe.h"

// Include the data acquisition device file
#if defined __DEVICE_LAB__
#include "Acquisition/LabJackDaq.cpp"
#elif defined __DEVICE_RPI__
#include "Acquisition/RaspberryPiDaq.cpp"
#elif defined __DEVICE_BBB__
#include "Acquisition/BeagleBoneDaq.cpp"
#endif

#include "Sensor/All.h"


//=================================== Main =====================================
int main()
{
  // Register all the classes in the factory
  #if defined __DEVICE_LAB__
  Factory::registerClass("LabJack", new CreatorType<LabJackDaq>);
  #elif defined __DEVICE_RPI__
  Factory::registerClass("RaspberryPi", new CreatorType<RaspberryPiDaq>);
  #elif defined __DEVICE_BBB__
  Factory::registerClass("BeagleBone", new CreatorType<BeagleBoneDaq>);
  #endif
  Factory::registerClass("UartWire", new CreatorType<UartWire>);
  Factory::registerClass("I2cWire", new CreatorType<I2cWire>);
  Factory::registerClass("GPS", new CreatorType<Adafruit_GPS>);
  Factory::registerClass("LSM303", new CreatorType<Adafruit_LSM303>);
  Factory::registerClass("MAG303", new CreatorType<Adafruit_MAG303>);
  Factory::registerClass("L3GD20", new CreatorType<Adafruit_L3GD20>);

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
  RaspberryPiDaq* daq = static_cast<RaspberryPiDaq*>(Factory::get("RPI"));
  #elif defined __DEVICE_BBB__
  BeagleBone* daq = static_cast<BeagleBone*>(Factory::get("BBB"));
  #endif

  //Adafruit_LSM303* accel = static_cast<Adafruit_LSM303*>(Factory::get("Accel"));
  //Adafruit_MAG303* magneto = static_cast<Adafruit_MAG303*>(Factory::get("Magneto"));
  //Adafruit_L3GD20* gyro = static_cast<Adafruit_L3GD20*>(Factory::get("Gyro"));

  /*
  if (gyro->init())
  {
    INFO_LG("Gyro init ok");

    while (true)
    {
      gyro->get();
      usleep(300000);
      //accel->get();
      //usleep(100000);
      //magneto->get();
      //usleep(100000);
    }
  }
  else { INFO_LG("Gyro init failed"); }
  */

  Adafruit_GPS* gps = static_cast<Adafruit_GPS*>(Factory::get("GPS"));
  // gps->init();
  while (true)
  {
    usleep(10000);
  }
  //if (gps->init()) { INFO_LG("GPS init ok"); }
}
