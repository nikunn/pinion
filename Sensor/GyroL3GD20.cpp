#include "Framework/LuaBind.h"
#include "Framework/Logger.h"
#include "Sensor/GyroL3GD20.h"


namespace pno
{
//=================================== GyroL3GD20 ===============================
// Instantiates a new GyroL3GD20 class
GyroL3GD20::GyroL3GD20(const LuaTable& cfg) : SpiSensor(cfg)
{
  // Get the sensor range
  int range = cfg.get<int>("range");

  // Set the sensor gravity axis
  switch(range)
  {
    case 250:
      _range = L3GD20_RANGE_250DPS; break;
    case 500:
      _range = L3GD20_RANGE_500DPS; break;
    case 2000:
      _range = L3GD20_RANGE_2000DPS; break;
    default:
      FATAL_PF("L3GD20 Cannot parse the range:%u (250/500/2000)", range);
  }
}

// Initialize
bool GyroL3GD20::init()
{
  // Check we are properly connected by comparing the WHOAMI
  byte whoami;
  read(L3GD20_REGISTER_WHO_AM_I, &whoami);

  // Check WHOAMI value
  if (whoami != L3GD20_ID) { ERROR_LG("L3GD20 wrong whoami"); return false; }

  // Enable all 3 channels
  byte config = 0x0F;
  write(L3GD20_REGISTER_CTRL_REG1, &config);

  // Read the data we have just written
  byte reg1;
  read(L3GD20_REGISTER_CTRL_REG1, &reg1);

  // Check it is what we expect
  if (reg1 != config) { ERROR_LG("L3GD20 cannot write/read register 1"); return false; }

  // Change the resolution
  switch(_range)
  {
    case L3GD20_RANGE_250DPS:
      config = 0x00;
      write(L3GD20_REGISTER_CTRL_REG4, &config);
      _sensitivity = L3GD20_SENSITIVITY_250DPS;
      break;
    case L3GD20_RANGE_500DPS:
      config = 0x10;
      write(L3GD20_REGISTER_CTRL_REG4, &config);
      _sensitivity = L3GD20_SENSITIVITY_500DPS;
      break;
    case L3GD20_RANGE_2000DPS:
      config = 0x20;
      write(L3GD20_REGISTER_CTRL_REG4, &config);
      _sensitivity = L3GD20_SENSITIVITY_2000DPS;
      break;
  }

  // Read the data we have just written
  byte reg4;
  read(L3GD20_REGISTER_CTRL_REG4, &reg4);

  // Check it is what we expect
  if (reg4 != config) { ERROR_LG("L3GD20 cannot write/read register 4"); return false; }

  // Initialization went fine
  return true;
}

// Get data from the sensor and process it
void GyroL3GD20::get()
{
  // Read the gyroscope data
  byte data[6];

  // Read multiple register
  read(L3GD20_REGISTER_OUT_X_L, &data[0], 6);

  // Get the lo and hi value for x,y and z
  byte xlo = data[0];
  byte xhi = data[1];
  byte ylo = data[2];
  byte yhi = data[3];
  byte zlo = data[4];
  byte zhi = data[5];

  // Convert the value
  _data.x = _sensitivity * int8To16(xhi, xlo);
  _data.y = _sensitivity * int8To16(yhi, ylo);
  _data.z = _sensitivity * int8To16(zhi, zlo);

  // Some log
  INFO_PF("L3GD20 x:%f y:%f z:%f", _data.x, _data.y, _data.z);
}


}
