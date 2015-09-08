// Inspired from Adafruit drivers https://github.com/adafruit
#ifndef __ADXL345_H__
#define __ADXL345_H__

#include "Sensor/Sensor.h"

// This is an SPI Accelerometer, The maximum SPI clock speed is 5 MHz

// Device Id
#define ADXL345_DEVICE_ID 0xE5

// I2C address
#define ADXL345_I2CADDR 0x53

// Resolution multiplier 4mg per lsb
#define ADXL345_RESOLUTION_MULTIPLIER (0.004)

namespace pno
{
// Registers
enum ADXL345_REGISTER
{
  ADXL345_REGISTER_DEVICE_ID       = 0x00,
  ADXL345_REGISTER_OUT_X_L_A       = 0x32,
  ADXL345_REGISTER_OUT_X_H_A       = 0x33,
  ADXL345_REGISTER_OUT_Y_L_A       = 0x34,
  ADXL345_REGISTER_OUT_Y_H_A       = 0x35,
  ADXL345_REGISTER_OUT_Z_L_A       = 0x36,
  ADXL345_REGISTER_OUT_Z_H_A       = 0x37,
  ADXL345_REGISTER_DATA_RATE       = 0x2C,
  ADXL345_REGISTER_DATA_RANGE      = 0x31,
};

// Datarate
// Use of the 3200 Hz and 1600 Hz output data rates is only recommended with SPI communication rates greater than or equal to 2 MHz. The 800 Hz output data rate is recommended only for communication speeds greater than or equal to 400 kHz, and the remaining data rates scale proportionally. For example, the minimum recommended communication speed for a 200 Hz output data rate is 100 kHz.
enum ADXL345_DATARATE
{
  ADXL345_DATARATE_3200_HZ    = 0b1111, // 1600Hz Bandwidth
  ADXL345_DATARATE_1600_HZ    = 0b1110, //  800Hz Bandwidth
  ADXL345_DATARATE_800_HZ     = 0b1101, //  400Hz Bandwidth
  ADXL345_DATARATE_400_HZ     = 0b1100, //  200Hz Bandwidth
  ADXL345_DATARATE_200_HZ     = 0b1011, //  100Hz Bandwidth
  ADXL345_DATARATE_100_HZ     = 0b1010, //   50Hz Bandwidth
  ADXL345_DATARATE_50_HZ      = 0b1001, //   25Hz Bandwidth
  ADXL345_DATARATE_25_HZ      = 0b1000, // 12.5Hz Bandwidth
  ADXL345_DATARATE_12_5_HZ    = 0b0111, // 6.25Hz Bandwidth
  ADXL345_DATARATE_6_25HZ     = 0b0110, // 3.13Hz Bandwidth
  ADXL345_DATARATE_3_13_HZ    = 0b0101, // 1.56Hz Bandwidth
  ADXL345_DATARATE_1_56_HZ    = 0b0100, // 0.78Hz Bandwidth
  ADXL345_DATARATE_0_78_HZ    = 0b0011, // 0.39Hz Bandwidth
  ADXL345_DATARATE_0_39_HZ    = 0b0010, // 0.20Hz Bandwidth
  ADXL345_DATARATE_0_20_HZ    = 0b0001, // 0.10Hz Bandwidth
  ADXL345_DATARATE_0_10_HZ    = 0b0000  // 0.05Hz Bandwidth
};

// Datarange
enum ADXL345_DATARANGE
{
  ADXL345_DATARANGE_16_G      = 0b11,   // +/- 16g
  ADXL345_DATARANGE_8_G       = 0b10,   // +/- 8g
  ADXL345_DATARANGE_4_G       = 0b01,   // +/- 4g
  ADXL345_DATARANGE_2_G       = 0b00    // +/- 2g (default value)
};

//=============================== AccelADXL345 =============================
class AccelADXL345 : public SpiSensor
{
public:

  AccelADXL345(const LuaTable&);
  bool init();
  void get();

private:

  // Set range in g
  void setRange(ADXL345_DATARANGE range);

  // Last read accelerometer data
  SensorVector _data;

  // Axis of gravity
  AXIS _axis;
};


}
#endif
