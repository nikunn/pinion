#include "MAG303.h"

// Instantiates a new Adafruit_MAG303 class
Adafruit_MAG303::Adafruit_MAG303(int handle, int data_line, int clock_line, byte address)
{
  _handle = handle;
  _address = address;
  _data_line = data_line;
  _clock_line = clock_line;
}

// Initialize the magnetometer
bool Adafruit_MAG303::init(MAG303_GAIN gain)
{
  // Enable the magnetometer
  byte config = 0x0;
  write(MAG303_REGISTER_MR_REG_M, &config);

  // MAG303 has no WHOAMI register so read CRA_REG_M to check
  // the default value (0b00010000/0x10)
  byte reg1;
  read(MAG303_REGISTER_CRA_REG_M, &reg1);

  // Check it is what we expect
  if (reg1 != 0x10) { return false; }
  
  // Set the gain to a known level
  if (!setGain(gain)) { return false; };

  // Initialization went fine
  return true;
}

// Sets the magnetometer's gain
bool Adafruit_MAG303::setGain(MAG303_GAIN gain)
{
  // Write the desired gain to the sensor
  write(MAG303_REGISTER_CRB_REG_M, (byte*)&gain);
  
  // Read the data we have just written
  byte regm;
  read(MAG303_REGISTER_CRB_REG_M, &regm);

  // Check it is what we expect
  if (regm != gain) { return false; }
  
  // Set the private member to gain value
  _gain = gain;
 
  // Set the constant
  switch(gain)
  {
    case MAG303_GAIN_1_3 :
      _gauss_lsb_xy = 1100;
      _gauss_lsb_z  = 980;
      break;
    case MAG303_GAIN_1_9:
      _gauss_lsb_xy = 855;
      _gauss_lsb_z  = 760;
      break;
    case MAG303_GAIN_2_5:
      _gauss_lsb_xy = 670;
      _gauss_lsb_z  = 600;
      break;
    case MAG303_GAIN_4_0:
      _gauss_lsb_xy = 450;
      _gauss_lsb_z  = 400;
      break;
    case MAG303_GAIN_4_7:
      _gauss_lsb_xy = 400;
      _gauss_lsb_z  = 255;
      break;
    case MAG303_GAIN_5_6:
      _gauss_lsb_xy = 330;
      _gauss_lsb_z  = 295;
      break;
    case MAG303_GAIN_8_1:
      _gauss_lsb_xy = 230;
      _gauss_lsb_z  = 205;
      break;
  }

  // Settings went fine
  return true;
}

// Reads the raw data from the sensor
void Adafruit_MAG303::get()
{
  // Read the magnetometer
  byte magnet[6];
  read(MAG303_REGISTER_OUT_X_H_M, &magnet[0], 6);

  // Note high before low, and Z before Y
  byte xhi = magnet[0];
  byte xlo = magnet[1];
  byte zhi = magnet[2];
  byte zlo = magnet[3];
  byte yhi = magnet[4];
  byte ylo = magnet[5];
  
  // Shift values to create properly formed integer (low byte first)
  _data.x = int8To16(xhi, xlo);
  _data.y = int8To16(yhi, ylo);
  _data.z = int8To16(zhi, zlo);

  _data.x *=  GAUSS_TO_MICROTESLA / _gauss_lsb_xy;
  _data.y *=  GAUSS_TO_MICROTESLA / _gauss_lsb_xy;
  _data.z *=  GAUSS_TO_MICROTESLA / _gauss_lsb_z;

  // Some log if debug
  #ifdef _DEBUG_SENSOR_
  printf("MAG303 x:%f y:%f z:%f\n", _data.x, _data.y, _data.z);
  #endif
}
