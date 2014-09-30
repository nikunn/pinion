#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>

//================================== Typedefs ==================================
typedef uint8_t byte;

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

#endif
