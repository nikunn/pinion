#ifndef _LUABIND_H
#define _LUABIND_H

#include "sol.hpp"


namespace pno
{
//================================= Typedefs ===================================
using LuaState = sol::state;
using LuaTable = sol::table;

//================================= LuaScript ==================================
class LuaScript
{
public :

  //============================ public functions ============================
  LuaScript(const std::string&);
  ~LuaScript() {};

  const LuaState& get() const { return _script; };

private :

  //=========================== private members ============================
  LuaState _script;
};


}
#endif
