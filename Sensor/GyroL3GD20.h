// Inspired from Adafruit drivers https://github.com/adafruit
#ifndef __L3GD20_H__
#define __L3GD20_H__

#include "Sensor/Sensor.h"

// This is a Gyroscope

// I2C address
#define L3GD20_I2CADDR 0x6B

// Sensor Id WHOAMI
#define L3GD20_ID 0b11010100

// Sensitivity
#define L3GD20_SENSITIVITY_250DPS  0.00875F
#define L3GD20_SENSITIVITY_500DPS  0.0175F
#define L3GD20_SENSITIVITY_2000DPS 0.070F


namespace pno
{
// Registers
enum L3GD20_REGISTER
{
  L3GD20_REGISTER_WHO_AM_I            = 0x0F,
  L3GD20_REGISTER_CTRL_REG1           = 0x20,
  L3GD20_REGISTER_CTRL_REG2           = 0x21,
  L3GD20_REGISTER_CTRL_REG3           = 0x22,
  L3GD20_REGISTER_CTRL_REG4           = 0x23,
  L3GD20_REGISTER_CTRL_REG5           = 0x24,
  L3GD20_REGISTER_REFERENCE           = 0x25,
  L3GD20_REGISTER_OUT_TEMP            = 0x26,
  L3GD20_REGISTER_STATUS_REG          = 0x27,
  L3GD20_REGISTER_OUT_X_L             = 0x28,
  L3GD20_REGISTER_OUT_X_H             = 0x29,
  L3GD20_REGISTER_OUT_Y_L             = 0x2A,
  L3GD20_REGISTER_OUT_Y_H             = 0x2B,
  L3GD20_REGISTER_OUT_Z_L             = 0x2C,
  L3GD20_REGISTER_OUT_Z_H             = 0x2D,
  L3GD20_REGISTER_FIFO_CTRL_REG       = 0x2E,
  L3GD20_REGISTER_FIFO_SRC_REG        = 0x2F,
  L3GD20_REGISTER_INT1_CFG            = 0x30,
  L3GD20_REGISTER_INT1_SRC            = 0x31,
  L3GD20_REGISTER_TSH_XH              = 0x32,
  L3GD20_REGISTER_TSH_XL              = 0x33,
  L3GD20_REGISTER_TSH_YH              = 0x34,
  L3GD20_REGISTER_TSH_YL              = 0x35,
  L3GD20_REGISTER_TSH_ZH              = 0x36,
  L3GD20_REGISTER_TSH_ZL              = 0x37,
  L3GD20_REGISTER_INT1_DURATION       = 0x38 
};

// Range
enum L3GD20_RANGE
{
  L3GD20_RANGE_250DPS  = 250,
  L3GD20_RANGE_500DPS  = 500,
  L3GD20_RANGE_2000DPS = 2000
};

//=================================== GyroL3GD20 ===============================
class GyroL3GD20 : public I2cSensor
{
public:

  GyroL3GD20(const LuaTable&);

  bool init();
  float sensitivity(L3GD20_RANGE range);
  void get();

private:

  SensorVector _data;
  L3GD20_RANGE _range;
};


}
#endif
