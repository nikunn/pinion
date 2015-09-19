#ifndef _ACCESSIBLE_H
#define _ACCESSIBLE_H


#include "Framework/LuaBind.h"


namespace pno
{
//================================ Accessible ==================================

// A base class for factory constructible and accessible objects
class Accessible
{
public:
  Accessible(const LuaTable& cfg) : _name(cfg.get<std::string>("name")) {};

  const std::string& name() const { return _name; }

private:
  std::string _name;
};


}
#endif
