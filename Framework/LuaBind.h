#ifndef _LUABIND_H
#define _LUABIND_H

#include "sol.hpp"


//================================= Typedefs ===================================
typedef sol::state LuaState;
typedef sol::table LuaTable;

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



#endif
