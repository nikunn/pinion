#include "Framework/LuaBind.h"
#include "Framework/Logger.h"
#include "Sensor/AccelADXL345.h"


namespace pno
{
//=============================== AccelADXL345 =============================
// Instantiates a new AccelADXL345 class
AccelADXL345::AccelADXL345(const LuaTable& cfg) : SpiSensor(cfg)
{
}

// Initialize Accelerometer
bool AccelADXL345::init()
{
  // Check we are properly connected by comparing the device id
  byte device_id;
  read(ADXL345_REGISTER_DEVICE_ID, &device_id);

  // Check WHOAMI value
  if (device_id != ADXL345_DEVICE_ID) { ERROR_LG("ADXL345 wrong device id"); return false; }

  // Enable Sensor
  byte config = 0x08;
  write(ADXL345_REGISTER_POWER_CTL, &config);

  // Initialize range
  ADXL345_DATARANGE data_range = ADXL345_DATARANGE_2_G;

  // Set range
  setFormat(data_range);

  // Set the data rate
  config = ADXL345_DATARATE_100_HZ;
  write(ADXL345_REGISTER_DATA_RATE, &config);

  // Initialization went fine
  return true;
}

void AccelADXL345::setFormat(ADXL345_DATARANGE range)
{
  // Read the data format
  byte format;
  read(ADXL345_REGISTER_DATA_RANGE, &format);

  // Reset the last four bits
  format &= ~0x0F;

  // Update the range
  format |= range;
  
  // Make sure that the FULL-RES bit is enabled
  format |= 0x08;
  
  // Write the register
  write(ADXL345_REGISTER_DATA_RANGE, &format);
}

// Reads the raw data from the sensor
void AccelADXL345::get()
{
  // Read the accelerometer
  byte data[6];

  // The MSb=1 enables address auto-increment to allow multiple read/write.
  read(ADXL345_REGISTER_OUT_X_L_A, &data[0], 6);

  // Get the lo and hi value for x,y and z
  byte xlo = data[0];
  byte xhi = data[1];
  byte ylo = data[2];
  byte yhi = data[3];
  byte zlo = data[4];
  byte zhi = data[5];
  
  // Shift values to create properly formed integer (low byte first)
  _data.x = int8To16(xhi, xlo);
  _data.y = int8To16(yhi, ylo);
  _data.z = int8To16(zhi, zlo);

  // Convert the value to g
  _data.x = _data.x * ADXL345_RESOLUTION_MULTIPLIER;
  _data.y = _data.y * ADXL345_RESOLUTION_MULTIPLIER;
  _data.z = _data.z * ADXL345_RESOLUTION_MULTIPLIER;

  // Some log if debug
  INFO_PF("ADXL345 x:%f y:%f z:%f", _data.x, _data.y, _data.z);
}


}
