#include "Framework/LuaBind.h"
#include "Framework/Logger.h"
#include "Sensor/AccelLSM303.h"


namespace pno
{
//================================ AccelLSM303 =============================
// Instantiates a new AccelLSM303 class
AccelLSM303::AccelLSM303(const LuaTable& cfg) : I2cSensor(cfg)
{
  // Set the sensor I2C address
  _address = LSM303_I2CADDR;
}

// Initialize Accelerometer
bool AccelLSM303::init()
{
  // Enable the accelerometer (100Hz)
  // 0x57 = 0101(Normal 100Hz) 0(Normal Power) 111(Enable ZYX)
  byte config = 0x57;
  write(LSM303_REGISTER_CTRL_REG1_A, &config);

  // LSM303 has no WHOAMI register so read CTRL_REG1_A back to check
  byte reg1;
  read(LSM303_REGISTER_CTRL_REG1_A, &reg1);

  // Check it has the correct value
  if (reg1 != config) { ERROR_LG("LSM303 cannot write/read register 1"); return false; }  
  
  // Set the min max and the high resolution mode
  // 0x8 = 0(Block Data) 0(Endian) 00(2G MinMax) 1(HighResolution) 00(Nothing) 0(SPI) 
  config = 0x8;
  write(LSM303_REGISTER_CTRL_REG4_A, &config);

  // Read the data we have just written
  byte reg4;
  read(LSM303_REGISTER_CTRL_REG4_A, &reg4);

  // Check it is what we expect
  if (reg4 != config) { ERROR_LG("LSM303 cannot write/read register 4"); return false; }

  // Initialization went fine
  return true;
}

// Reads the raw data from the sensor
void AccelLSM303::get()
{
  // Read the accelerometer
  byte data[6];

  // The MSb=1 enables address auto-increment to allow multiple read/write.
  read((LSM303_REGISTER_OUT_X_L_A | 0x80), &data[0], 6);

  // Get the lo and hi value for x,y and z
  byte xlo = data[0];
  byte xhi = data[1];
  byte ylo = data[2];
  byte yhi = data[3];
  byte zlo = data[4];
  byte zhi = data[5];
  
  // Shift values to create properly formed integer (low byte first)
  _data.x = int8To16(xhi, xlo) >> 4;
  _data.y = int8To16(yhi, ylo) >> 4;
  _data.z = int8To16(zhi, zlo) >> 4;

  // Convert the value to g and adjust the axis of gravity
  _data.x = _data.x / 1000;
  _data.y = _data.y / 1000;
  _data.z = _data.z / 1000;

  // Some log if debug
  INFO_PF("LSM303 x:%f y:%f z:%f", _data.x, _data.y, _data.z);
}


}
