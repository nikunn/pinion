#ifndef _UNIVERSE_H
#define _UNIVERSE_H

#include "Framework/LuaBind.h"


namespace pno
{
//================================= Universe ===================================

// Class to manage the universe
class Universe
{
public:
  static void createUniverse(const LuaState&);
};


}
#endif
