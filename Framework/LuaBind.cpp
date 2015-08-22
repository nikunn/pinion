#include "Framework/LuaBind.h"


namespace pno
{
//================================= LuaScript ==================================

// Constructor
LuaScript::LuaScript(const std::string& filename)
{
  // Add the basic library, by default libraries are not opened SALE
  _script.open_libraries(sol::lib::base);

  // Load the file
  _script.open_file(filename);
}


}
