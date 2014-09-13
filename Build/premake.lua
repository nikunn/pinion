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
  includedirs { "../", "/usr/include/lua5.2/", "../Include/Sol/", "../Include/Log/src/" }
  targetdir "lib/"


----------------------------------- Framework ----------------------------------
project "framework"
  kind "SharedLib"
  links { "external" }
  files { "../Framework/Factory.cpp", "../Framework/Universe.cpp" }
  includedirs { "../", "/usr/include/lua5.2/", "../Include/Sol/" }
  targetdir "lib/"


----------------------------------- Framework ----------------------------------
project "sensor"
  kind "SharedLib"
  links { "external" }
  files { "../Sensor/All.cpp" }
  includedirs { "../", "/usr/include/lua5.2/", "../Include/Sol/" }
  targetdir "lib/"


---------------------------------- Acquisition ---------------------------------
project "daq"
  kind "ConsoleApp"
  files { "main.cpp" }
  links { "external", "framework", "sensor", "LabJackM" }
  includedirs { "../", "/usr/include/lua5.2/", "../Include/Sol/" }
  linkoptions { "-Wl,-rpath,./lib/" }

-------------------------------------- End -------------------------------------
