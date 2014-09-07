#include "easylogging++.h"

#include "Framework/Logger.h"

// Initialize the logger, should be called only once
_INITIALIZE_EASYLOGGINGPP

//=============================================================================\'
#define LOGGER_IMPLEMENT(TYPE)                                                 \
void Logger::print_##TYPE(const char* message)                                 \
{                                                                              \
  LOG(TYPE) << message;                                                        \
}                                                                              \
//=============================================================================\'


//================================== Logger ====================================

// Implement the interface logging functions
LOGGER_IMPLEMENT(INFO);
LOGGER_IMPLEMENT(DEBUG);
LOGGER_IMPLEMENT(WARNING);
LOGGER_IMPLEMENT(ERROR);
LOGGER_IMPLEMENT(FATAL);


// Configure the default logger
void Logger::configure(const LuaTable& lua_config)
{
  // Create a new configuration
  el::Configurations log_config;

  // Reset the new configuration to default
  log_config.setToDefault();

  // Go through each level of logging
  
  for (int level_ind = 1; level_ind < lua_config.size() + 1; level_ind++)
  {
    // Get the table defining this level
    LuaTable level_config = lua_config.get<LuaTable>(level_ind);

    // Get the level name as string
    std::string level_name = level_config.get<std::string>("level");

    // Get the corresponding enum for this level
    el::Level level = el::LevelHelper::convertFromString(level_name.c_str());

    // Check if level has been successfully parsed
    if (level != el::Level::Unknown)
    {
      // Get the table with the configuration details
      LuaTable configuration = level_config.get<LuaTable>("configuration");

      // Go through the configuration for this level
      for (int field_index = 1; field_index < configuration.size() + 1; field_index++)
      {
        // Get the table with the field details
        LuaTable field_config = configuration.get<LuaTable>(field_index);

        // Get the field name
        std::string field_name = field_config.get<std::string>("field");

        // Get the corresponding enum for this field
        el::ConfigurationType field =
          el::ConfigurationTypeHelper::convertFromString(field_name.c_str());

        // Check if field has been successfully parsed
        if (field != el::ConfigurationType::Unknown)
        {
          // Get the value for this field
          std::string value = field_config.get<std::string>("value");

          // Set this field/value to the current configuration
          log_config.set(level, field, value);
        }
      }
    }
  }

  // Apply this configuration to the default logger
  el::Loggers::reconfigureLogger("default", log_config);
}
