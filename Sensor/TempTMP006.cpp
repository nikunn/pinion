#include<cmath>

#include "Framework/Logger.h"
#include "Sensor/TempTMP006.h"

//================================== TempTMP006 ================================
TempTMP006::TempTMP006(const LuaTable& cfg) : I2cSensor(cfg)
{
  // Set the sensor I2C address
  _address = TMP006_I2CADDR;
}


bool TempTMP006::init()
{
  // Define the smaple rate
  uint16_t samplerate = TMP006_CONFIG_16SAMPLE;

  // Define a configuration
  // 0111 1001 1000 0000
  uint16_t config = TMP006_CONFIG_MODEON | TMP006_CONFIG_DRDYEN | samplerate;

  // Update the sensor with this configuration
  write(TMP006_REGISTER_CONFIG, (byte*)&config, 2);

  // Read the config back to check
  byte check[2];
  read(TMP006_REGISTER_CONFIG, &check[0], 2);

  INFO_PF("config:%u check:%u\n", config, int8To16(&check[0]));

  // Perform check on the config
  if (int8To16(&check[0]) != config) { return false; }

  // Perform check on Manufacturer Id
  byte tmid[2];
  read(TMP006_REGISTER_MANID, &tmid[0], 2);
  int16_t mid = int8To16(&tmid[0]);
  if (mid != 0x5449) return false;

  // Perform check on Device Id
  byte tdid[2];
  read(TMP006_REGISTER_DEVID, &tdid[0], 2);
  int16_t did = int8To16(&tdid[0]);
  if (did != 0x67) return false;

  // Initialization went fine
  return true;
}

//////////////////////////////////////////////////////

void TempTMP006::get()
{
  // Get object temperature
  _data = readObjTempC();

  // Get ambiant temperature
  _ambiant = readDieTempC();

  // Some log if debug
  INFO_PF("TMP006 obj:%f amb:%f\n", _data, _ambiant);
}

float TempTMP006::readDieTempC(void)
{
   float Tdie = readRawDieTemperature();
   Tdie *= 0.03125; // convert to celsius
   return Tdie;
}

float TempTMP006::readObjTempC(void)
{
   float Tdie = readRawDieTemperature();
   float Vobj = readRawVoltage();
   Vobj *= 156.25;  // 156.25 nV per LSB
   Vobj /= 1000; // nV -> uV
   Vobj /= 1000; // uV -> mV
   Vobj /= 1000; // mV -> V
   Tdie *= 0.03125; // convert to celsius
   Tdie += 273.15; // convert to kelvin

   float tdie_tref = Tdie - TMP006_TREF;
   float S = (1 + TMP006_A1*tdie_tref + 
                     TMP006_A2*tdie_tref*tdie_tref);
   S *= TMP006_S0;
   S /= 10000000;
   S /= 10000000;
   
   float Vos = TMP006_B0 + TMP006_B1*tdie_tref + TMP006_B2*tdie_tref*tdie_tref;
   float fVobj = (Vobj - Vos) + TMP006_C2*(Vobj-Vos)*(Vobj-Vos);
   float Tobj = std::sqrt(std::sqrt(Tdie * Tdie * Tdie * Tdie + fVobj/S));
   
   Tobj -= 273.15; // Kelvin -> *C
   return Tobj;
}


int16_t TempTMP006::readRawDieTemperature(void)
{
  // Read the ambiant temperature
  byte traw[2];
  read(TMP006_REGISTER_TAMB, &traw[0], 2);
  int16_t raw =int8To16(&traw[0]);

  // Shift the to LSB as per datasheet
  raw >>= 2;

  return raw;
}

int16_t TempTMP006::readRawVoltage(void)
{
  // Read the Object temperature
  byte traw[2];
  read(TMP006_REGISTER_VOBJ, &traw[0], 2);
  int16_t raw =int8To16(&traw[0]);

  return raw; 
}
