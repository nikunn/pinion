#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>

#define GRAVITY 9.80665F


namespace pno
{
//================================== Typedefs ==================================
using byte = uint8_t;

// Convert two byte to 16 bit int
static int16_t int8To16(byte* ent)
{
  return ((int16_t)ent[0] << 8) | ent[1];
}

// Convert two byte to 16 bit int
static int16_t int8To16(byte hi, byte lo)
{
  return ((int16_t)hi << 8) | lo;
}


//=============================== PwmConfig ================================
// Configuration for PWM
struct PwmConfig
{
  bool polarity;
  unsigned long period;
  unsigned long duty;
};


//============================== GpioConfig ================================
// Configuration for GPIO
struct GpioConfig
{
  GpioConfig(const std::string& direction, const std::string edge)
    : direction(direction), edge(edge)
  {
    // Check direction value
    if (direction != "in" && direction != "out")
    {
      FATAL_PF("GPIO config, direction is undefined: %s", direction.c_str());
    }

    // Check edge value
    if (edge != "rising" && edge != "falling" && edge != "both" && edge != "none")
    {
      FATAL_PF("GPIO config, edge is undefined: %s", edge.c_str());
    }
  }

  std::string direction;
  std::string edge;
};


//================================== AXIS ==================================
// Enum containing the 3 axis
enum AXIS
{
  AXIS_X,
  AXIS_Y,
  AXIS_Z,
};


//============================== SensorVector ==============================
// Struct contaning 3 dimensional vector
struct SensorVector
{
  float x;
  float y;
  float z;
};


//=============================== EulerAngle ===============================
// Struct Containg the three Euler angle
struct EulerAngle
{
  // Rotation around the longitudinal axis (the plane body, 'X axis').
  // Roll is positive and increasing when moving downward. -90°<=roll<=90
  float roll;

  // Rotation around the lateral axis (the wing span, 'Y axis').
  // Pitch is positive and increasing when moving upwards. -180°<=pitch<=180
  float pitch;

  // Angle between the longitudinal axis (the plane body) and magnetic north.
  // Measured clockwise when viewing from the top of the device. 0-359
  float heading;
};


}
#endif
