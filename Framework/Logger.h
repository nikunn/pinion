#ifndef _LOGGER_H
#define _LOGGER_H

#include "Framework/LuaBind.h"

// Useful macros for log
//=============================================================================\'
#define LOGGER_PRINTF(TYPE, ...)                                               \
  {                                                                            \
  char* buffer_logger;                                                                \
  asprintf(&buffer_logger, __VA_ARGS__);                                              \
  Logger::print_##TYPE(buffer_logger);                                                \
  }                                                                            \
//=============================================================================\'

// Logging in INFO
#define INFO_LG(MSG) Logger::print_INFO(MSG);
#define INFO_PF(...) LOGGER_PRINTF(INFO, __VA_ARGS__);

// Logging in DEBUG
#define DEBUG_LG(MSG) Logger::print_DEBUG(MSG);
#define DEBUG_PF(...) LOGGER_PRINTF(DEBUG, __VA_ARGS__);

// Logging in WARNING
#define WARNING_LG(MSG) Logger::print_WARNING(MSG);
#define WARNING_PF(...) LOGGER_PRINTF(WARNING, __VA_ARGS__);

// Logging in ERROR
#define ERROR_LG(MSG) Logger::print_ERROR(MSG);
#define ERROR_PF(...) LOGGER_PRINTF(ERROR, __VA_ARGS__);

// Logging in FATAL
#define FATAL_LG(MSG) Logger::print_FATAL(MSG);
#define FATAL_PF(...) LOGGER_PRINTF(FATAL, __VA_ARGS__);


//================================== Logger ====================================

class Logger
{
public:
  // List all the interface logging functions 
  static void print_INFO(const char*);
  static void print_DEBUG(const char*);
  static void print_WARNING(const char*);
  static void print_ERROR(const char*);
  static void print_FATAL(const char*);

  // Comfigure the logger from the global configuration file
  static void configure(const LuaTable&);
};



#endif
