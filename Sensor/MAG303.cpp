#include "MAG303.h"

// Instantiates a new Adafruit_MAG303 class
Adafruit_MAG303::Adafruit_MAG303(const LuaTable& cfg)
{
  // Set the sensor I2C address
  _address = MAG303_I2CADDR;

  // Set Data Acquisition device
  std::string daq_name = cfg.get<std::string>("device");
  _daq = static_cast<DaqDevice*>(Factory::get(daq_name));

  // Set Wire
  std::string wire_name = cfg.get<std::string>("wire");
  _wire = static_cast<I2cWire*>(Factory::get(wire_name));

  // Get the sensor gain
  float gain = cfg.get<float>("gain");

  // Set the sensor gravity axis
  if (gain == 1.3f) { _gain = MAG303_GAIN_1_3; }
  else if (gain == 1.9f) { _gain = MAG303_GAIN_1_9; }
  else if (gain == 2.5f) { _gain = MAG303_GAIN_2_5; }
  else if (gain == 4.0f) { _gain = MAG303_GAIN_4_0; }
  else if (gain == 4.7f) { _gain = MAG303_GAIN_4_7; }
  else if (gain == 5.6f) { _gain = MAG303_GAIN_5_6; }
  else if (gain == 8.1f) { _gain = MAG303_GAIN_8_1; }
  else
  {
    FATAL_PF("MAG303 Cannot parse the gain:%f (1.3/1.9/2.5/4.0/4.7/5.6/8.1)", gain);
  }
}

// Initialize the magnetometer
bool Adafruit_MAG303::init()
{
  // Enable the magnetometer
  byte config = 0x0;
  write(MAG303_REGISTER_MR_REG_M, &config);

  // MAG303 has no WHOAMI register so read CRA_REG_M to check
  // the default value (0b00010000/0x10)
  byte reg1;
  read(MAG303_REGISTER_CRA_REG_M, &reg1);

  // Check it is what we expect
  if (reg1 != 0x10) { ERROR_LG("MAG303 cannot write/read register 1"); return false; }
  
  // Set the gain to a known level
  if (!setGain()) { ERROR_LG("MAG303 cannot set the gain"); return false; };

  // Initialization went fine
  return true;
}

// Sets the magnetometer's gain
bool Adafruit_MAG303::setGain()
{
  // Write the desired gain to the sensor
  write(MAG303_REGISTER_CRB_REG_M, (byte*)&_gain);
  
  // Read the data we have just written
  byte regm;
  read(MAG303_REGISTER_CRB_REG_M, &regm);

  // Check it is what we expect
  if (regm != _gain) { return false; }
  
  // Set the constant
  switch(_gain)
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
  INFO_PF("MAG303 x:%f y:%f z:%f", _data.x, _data.y, _data.z);
}
