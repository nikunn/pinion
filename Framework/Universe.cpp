#include "Framework/Logger.h"
#include "Framework/Factory.h"
#include "Framework/Universe.h"

//================================= Universe ===================================

// Create all object in universe
void Universe::createUniverse(const LuaState& lua_state)
{
  // Get the universe
  LuaTable universe = lua_state.get<LuaTable>("Universe");

  // Get the catalog
  LuaTable catalog = lua_state.get<LuaTable>("Catalog");

  // Go through the universe and get tables of object to create
  for (int univ_index = 1; univ_index < universe.size() + 1; univ_index++)
  {
    // Get the object table
    LuaTable obj_table = universe.get<LuaTable>(univ_index);

    // Go through the universe and get tables of object to create
    for (int obj_index = 1; obj_index < obj_table.size() + 1; obj_index++)
    {
      // Get the object name
      std::string obj_name = obj_table.get<std::string>(obj_index);

      // Get the configuration
      LuaTable config = catalog.get<LuaTable>(obj_name);

      // Create Object
      Accessible* object = Factory::create(config);

      // Log the creation of object
      if (object)
      {
        INFO_PF("Creation of %s successful", obj_name.c_str());
      }
      else
      {
        FATAL_PF("Creation of %s failed", obj_name.c_str());
      }
    }
  }
}
