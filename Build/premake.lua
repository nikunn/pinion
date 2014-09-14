----------------------------------- Solution -----------------------------------
solution "daq"
  -- Defines the available configurations
  configurations { "Release", "Debug" }
  -- All code is in C++
  language "C++"
  -- All code is to be compiled in c++11
  buildoptions "-std=c++11"

  configuration "Release"
    defines { "NDEBUG" }
    flags { "Optimize" }

  configuration "Debug"
    defines { "DEBUG" }
    flags { "Symbols" }



----------------------------------- External -----------------------------------
-- Compile some external library
project "external"
  kind "SharedLib"
  links { "lua5.2" }
  files { "../Framework/LuaBind.cpp", "../Framework/Logger.cpp" }
  includedirs { "../", "/usr/include/lua5.2/", "../Include/Sol/", "../Include/Log/src/", "../Include/Nmea/include/" }
  targetdir "lib/"


------------------------------------- Nmea -------------------------------------
-- Compile some external library
project "nmea"
  kind "SharedLib"
  files { "../Tools/Nmea.cpp", "../Include/Nmea/src/*.c" }
  includedirs { "../", "../Include/Nmea/include/" }
  buildoptions "-w"
  targetdir "lib/"


----------------------------------- Framework ----------------------------------
project "framework"
  kind "SharedLib"
  links { "external" }
  files { "../Framework/Factory.cpp", "../Framework/Universe.cpp" }
  includedirs { "../", "/usr/include/lua5.2/", "../Include/Sol/" }
  targetdir "lib/"


------------------------------------ Sensor ------------------------------------
project "sensor"
  kind "SharedLib"
  links { "external", "nmea" }
  files { "../Sensor/All.cpp" }
  includedirs { "../", "/usr/include/lua5.2/", "../Include/Sol/", "../Include/Nmea/include/" }
  targetdir "lib/"


---------------------------------- Acquisition ---------------------------------
project "daq"
  kind "ConsoleApp"
  files { "main.cpp" }
  links { "external", "nmea", "framework", "sensor", "LabJackM" }
  includedirs { "../", "/usr/include/lua5.2/", "../Include/Sol/", "../Include/Nmea/include/" }
  linkoptions { "-Wl,-rpath,./lib/" }

-------------------------------------- End -------------------------------------
