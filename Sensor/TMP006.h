// Inspired from Adafruit drivers https://github.com/adafruit
#ifndef _TMP006_H
#define _TMP006_H

#include "Sensor/Sensor.h"

// This is a infrared temperature sensor

// I2C address
#define TMP006_I2CADDR 0x40

// Some constants
#define TMP006_B0 -0.0000294
#define TMP006_B1 -0.00000057
#define TMP006_B2 0.00000000463
#define TMP006_C2 13.4
#define TMP006_TREF 298.15
#define TMP006_A2 -0.00001678
#define TMP006_A1 0.00175
#define TMP006_S0 6.4

// Registers
enum TMP006_REGISTER
{
  TMP006_REGISTER_VOBJ   = 0x00,
  TMP006_REGISTER_TAMB   = 0x01,
  TMP006_REGISTER_CONFIG = 0x02,
  TMP006_REGISTER_MANID  = 0xFE, // Manufacturer Id
  TMP006_REGISTER_DEVID  = 0xFF, // Device Id
};

// Configuration
enum TMP006_CONFIG
{
  TMP006_CONFIG_RESET    = 0x8000,
  TMP006_CONFIG_MODEON   = 0x7000,
  TMP006_CONFIG_1SAMPLE  = 0x0000,
  TMP006_CONFIG_2SAMPLE  = 0x0200,
  TMP006_CONFIG_4SAMPLE  = 0x0400,
  TMP006_CONFIG_8SAMPLE  = 0x0600,
  TMP006_CONFIG_16SAMPLE = 0x0800,
  TMP006_CONFIG_DRDYEN   = 0x0100,
  TMP006_CONFIG_DRDY     = 0x0080,
};

//================================ Adafruit_TMP006 =============================
class Adafruit_TMP006 : public I2cSensor
{
public:

  Adafruit_TMP006(const LuaTable&);

  bool init();
  void get();

private:

  int16_t readRawDieTemperature(void);
  int16_t readRawVoltage(void);
  float readObjTempC(void);
  float readDieTempC(void);

private:

  //=========================== private members ============================
  float _data;
  float _ambiant;
};

#endif
