#include <stdio.h>
#include <unistd.h>

#include "Acquisition/Daq.h"
#include "Acquisition/LabJackDaq.cpp"

#include "Sensor/Sensor.h"
//#include "Sensor/TMP006.h"
//#include "Sensor/LSM303.h"
//#include "Sensor/MAG303.h"
//#include "Sensor/L3GD20.h"
#include "Sensor/GPS.cpp"

#include "Framework/LuaBind.h"
#include "Framework/Logger.h"
#include "Framework/Factory.h"
#include "Framework/Universe.h"


//=================================== Main =====================================
int main()
{
  // Register all the classes
  Factory::registerClass("LabJack", new CreatorType<LabJackDaq>);
  Factory::registerClass("GPS", new CreatorType<Adafruit_GPS>);

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


  LabJackDaq* labd = static_cast<LabJackDaq*>(Factory::get("LBJ"));

  Adafruit_GPS* gps = static_cast<Adafruit_GPS*>(Factory::get("GPS"));

  /*
  labd->open();

  gps->stop();
  gps->start();
  gps->init();

  long counter = 0;

  while (true)
  {
    gps->get();
    usleep(1000);
    counter++;
  }
  */
}
