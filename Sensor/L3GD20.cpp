#include "L3GD20.h"

// Instantiates a new Adafruit_L3GD20 class
Adafruit_L3GD20::Adafruit_L3GD20(int handle, int data_line, int clock_line, byte address)
{
  _handle = handle;
  _address = address;
  _data_line = data_line;
  _clock_line = clock_line;
}

// Initialize
bool Adafruit_L3GD20::init(L3GD20_RANGE range)
{
  // Set the range
  _range = range;

  // Check we are properly connected by comparing the WHOAMI
  byte whoami;
  read(L3GD20_REGISTER_WHO_AM_I, &whoami);

  // Check WHOAMI value
  if (whoami != L3GD20_ID) { return false; }

  // Enable all 3 channels
  byte config = 0x0F;
  write(L3GD20_REGISTER_CTRL_REG1, &config);

  // Read the data we have just written
  byte reg1;
  read(L3GD20_REGISTER_CTRL_REG1, &reg1);

  // Check it is what we expect
  if (reg1 != config) { return false; }

  // Change the resolution
  switch(_range)
  {
    case L3GD20_RANGE_250DPS:
      config = 0x00;
      write(L3GD20_REGISTER_CTRL_REG4, &config);
      break;
    case L3GD20_RANGE_500DPS:
      config = 0x10;
      write(L3GD20_REGISTER_CTRL_REG4, &config);
      break;
    case L3GD20_RANGE_2000DPS:
      config = 0x20;
      write(L3GD20_REGISTER_CTRL_REG4, &config);
      break;
  }

  // Read the data we have just written
  byte reg4;
  read(L3GD20_REGISTER_CTRL_REG4, &reg4);

  // Check it is what we expect
  if (reg4 != config) { return false; }

  // Initialization went fine
  return true;
}

// Return the sensitivity depending on the range
float Adafruit_L3GD20::sensitivity(L3GD20_RANGE range)
{
  switch(range)
  {
    case L3GD20_RANGE_250DPS:
      return L3GD20_SENSITIVITY_250DPS;
    case L3GD20_RANGE_500DPS:
      return L3GD20_SENSITIVITY_500DPS;
    case L3GD20_RANGE_2000DPS:
      return L3GD20_SENSITIVITY_2000DPS;
  }
}

// Get data from the sensor and process it
void Adafruit_L3GD20::get()
{
  // Read the magnetometer
  byte data[6];

  // The MSb=1 (0x80) enables address auto-increment to allow multiple read/write.
  read((L3GD20_REGISTER_OUT_X_L | 0x80), &data[0], 6);

  // Get the lo and hi value for x,y and z
  byte xlo = data[0];
  byte xhi = data[1];
  byte ylo = data[2];
  byte yhi = data[3];
  byte zlo = data[4];
  byte zhi = data[5];

  // Get the sensitivity
  float factor = sensitivity(_range);
  
  // Convert the value
  _data.x = factor * int8To16(xhi, xlo);
  _data.y = factor * int8To16(yhi, ylo);
  _data.z = factor * int8To16(zhi, zlo);

  // Some log if debug
  #ifdef _DEBUG_SENSOR_
  printf("L3GD20 x:%f y:%f z:%f\n", _data.x, _data.y, _data.z);
  #endif
}
