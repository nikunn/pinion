// Inspired from Adafruit drivers https://github.com/adafruit
#ifndef __MAG303_H__
#define __MAG303_H__

#include "Sensor/Sensor.h"

// This is a Magnetometer

// I2C address
#define MAG303_I2CADDR 0x1E

// Gauss to microTesla factor
#define GAUSS_TO_MICROTESLA       (100)

// Registers
enum MAG303_REGISTER
{
  MAG303_REGISTER_CRA_REG_M    = 0x00,
  MAG303_REGISTER_CRB_REG_M    = 0x01,
  MAG303_REGISTER_MR_REG_M     = 0x02,
  MAG303_REGISTER_OUT_X_H_M    = 0x03,
  MAG303_REGISTER_OUT_X_L_M    = 0x04,
  MAG303_REGISTER_OUT_Z_H_M    = 0x05,
  MAG303_REGISTER_OUT_Z_L_M    = 0x06,
  MAG303_REGISTER_OUT_Y_H_M    = 0x07,
  MAG303_REGISTER_OUT_Y_L_M    = 0x08,
  MAG303_REGISTER_SR_REG_Mg    = 0x09,
  MAG303_REGISTER_IRA_REG_M    = 0x0A,
  MAG303_REGISTER_IRB_REG_M    = 0x0B,
  MAG303_REGISTER_IRC_REG_M    = 0x0C,
  MAG303_REGISTER_TEMP_OUT_H_M = 0x31,
  MAG303_REGISTER_TEMP_OUT_L_M = 0x32
};

// Gain
enum MAG303_GAIN
{
  MAG303_GAIN_1_3 = 0x20,  // +/- 1.3
  MAG303_GAIN_1_9 = 0x40,  // +/- 1.9
  MAG303_GAIN_2_5 = 0x60,  // +/- 2.5
  MAG303_GAIN_4_0 = 0x80,  // +/- 4.0
  MAG303_GAIN_4_7 = 0xA0,  // +/- 4.7
  MAG303_GAIN_5_6 = 0xC0,  // +/- 5.6
  MAG303_GAIN_8_1 = 0xE0   // +/- 8.1
};

//================================ Adafruit_MAG303 =============================
class Adafruit_MAG303 : public I2cSensor
{
public:

  Adafruit_MAG303(const LuaTable&);

  bool init();
  bool setGain();
  void get();

private:

  MAG303_GAIN _gain;
  float _gauss_lsb_xy;
  float _gauss_lsb_z;
  SensorVector _data;
};

#endif
