// Inspired from Adafruit drivers https://github.com/adafruit
#ifndef __LSM303_H__
#define __LSM303_H__

#include "Sensor/Sensor.h"

// This is an Accelerometer

// I2C address
#define LSM303_I2CADDR 0x19


namespace pno
{
// Registers
enum LSM303_REGISTER
{
  LSM303_REGISTER_CTRL_REG1_A     = 0x20,
  LSM303_REGISTER_CTRL_REG2_A     = 0x21,
  LSM303_REGISTER_CTRL_REG3_A     = 0x22,
  LSM303_REGISTER_CTRL_REG4_A     = 0x23,
  LSM303_REGISTER_CTRL_REG5_A     = 0x24,
  LSM303_REGISTER_CTRL_REG6_A     = 0x25,
  LSM303_REGISTER_REFERENCE_A     = 0x26,
  LSM303_REGISTER_STATUS_REG_A    = 0x27,
  LSM303_REGISTER_OUT_X_L_A       = 0x28,
  LSM303_REGISTER_OUT_X_H_A       = 0x29,
  LSM303_REGISTER_OUT_Y_L_A       = 0x2A,
  LSM303_REGISTER_OUT_Y_H_A       = 0x2B,
  LSM303_REGISTER_OUT_Z_L_A       = 0x2C,
  LSM303_REGISTER_OUT_Z_H_A       = 0x2D,
  LSM303_REGISTER_FIFO_CTRL_REG_A = 0x2E,
  LSM303_REGISTER_FIFO_SRC_REG_A  = 0x2F,
  LSM303_REGISTER_INT1_CFG_A      = 0x30,
  LSM303_REGISTER_INT1_SOURCE_A   = 0x31,
  LSM303_REGISTER_INT1_THS_A      = 0x32,
  LSM303_REGISTER_INT1_DURATION_A = 0x33,
  LSM303_REGISTER_INT2_CFG_A      = 0x34,
  LSM303_REGISTER_INT2_SOURCE_A   = 0x35,
  LSM303_REGISTER_INT2_THS_A      = 0x36,
  LSM303_REGISTER_INT2_DURATION_A = 0x37,
  LSM303_REGISTER_CLICK_CFG_A     = 0x38,
  LSM303_REGISTER_CLICK_SRC_A     = 0x39,
  LSM303_REGISTER_CLICK_THS_A     = 0x3A,
  LSM303_REGISTER_TIME_LIMIT_A    = 0x3B,
  LSM303_REGISTER_TIME_LATENCY_A  = 0x3C,
  LSM303_REGISTER_TIME_WINDOW_A   = 0x3D
};

//================================ AccelLSM303 =============================
class AccelLSM303 : public I2cSensor
{
public:

  AccelLSM303(const LuaTable&);
  bool init();
  void get();

private:

  // Last read accelerometer data
  SensorVector _data;
};


}
#endif
