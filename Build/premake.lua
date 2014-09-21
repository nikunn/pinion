------------------------------------ Option ------------------------------------
newoption {
  trigger = "device",
  value = "Device",
  description = "Choose a device for data acquisition",
  allowed =
  {
    { "lab",  "LabJack" },
    { "rpi",  "RaspberryPi" },
    { "bbb",  "BeagleBone Black" }
  }
}

-- Define device compilation flag for all devices
local device_def = {}
device_def["lab"] = "__DEVICE_LAB__"
device_def["rpi"] = "__DEVICE_RPI__"
device_def["bbb"] = "__DEVICE_BBB__"

local device_lib = {}
device_lib["lab"] = "LabJackM"
device_lib["rpi"] = ""
device_lib["bbb"] = ""

-- Set default device to LabJack
if not _OPTIONS["device"] then
  _OPTIONS["device"] = "lab"
end

-- Get the data acquisition device
local device = _OPTIONS["device"]

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
  links { "external", "nmea", "framework", "sensor", device_lib[device] }
  includedirs { "../", "/usr/include/lua5.2/", "../Include/Sol/", "../Include/Nmea/include/" }
  defines { device_def[device] }
  linkoptions { "-Wl,-rpath,./lib/" }

-------------------------------------- End -------------------------------------
