------------------------------------ Option ------------------------------------
newoption {
  trigger = "device",
  value = "Device",
  description = "Choose a device for data acquisition",
  allowed =
  {
    { "lab",  "LabJack" },
    { "uxb",  "LinuxBoard" },
  }
}

-- Define device compilation flag for all devices
local device_def = {}
device_def["lab"] = "__DEVICE_LAB__"
device_def["uxb"] = "__DEVICE_UXB__"

local device_lib = {}
device_lib["lab"] = "LabJackM"
device_lib["uxb"] = nil

-- Set default device to LabJack
if not _OPTIONS["device"] then
  _OPTIONS["device"] = "uxb"
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
  buildoptions "-std=c++0x"

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
  files { "Src/Framework/LuaBind.cpp", "Src/Framework/Logger.cpp" }
  includedirs { "Src/", "/usr/include/lua5.2/", "Src/Include/Sol/", "Src/Include/Log/src/", "Src/Include/Nmea/include/" }
  targetdir "lib/"


------------------------------------- Nmea -------------------------------------
-- Compile some external library
project "nmea"
  kind "SharedLib"
  files { "Src/Tools/Nmea.cpp", "Src/Include/Nmea/src/*.c" }
  includedirs { "Src/", "Src/Include/Nmea/include/" }
  buildoptions "-w"
  targetdir "lib/"


----------------------------------- Framework ----------------------------------
project "framework"
  kind "SharedLib"
  links { "external" }
  files { "Src/Framework/Factory.cpp", "Src/Framework/Universe.cpp", "Src/Framework/StopWatch.cpp", "Src/Framework/Mux*.cpp" }
  includedirs { "Src/", "/usr/include/lua5.2/", "Src/Include/Sol/" }
  targetdir "lib/"


----------------------------------- Actuator -----------------------------------
project "actuator"
  kind "SharedLib"
  links { "external", "nmea" }
  files { "Src/Actuator/Actuator.cpp" }
  includedirs { "Src/", "/usr/include/lua5.2/", "Src/Include/Sol/" }
  targetdir "lib/"


------------------------------------ Sensor ------------------------------------
project "sensor"
  kind "SharedLib"
  links { "external", "nmea" }
  files { "Src/Sensor/All.cpp" }
  includedirs { "Src/", "/usr/include/lua5.2/", "Src/Include/Sol/", "Src/Include/Nmea/include/" }
  targetdir "lib/"


---------------------------------- Acquisition ---------------------------------
project "daq"
  kind "ConsoleApp"
  files { "Src/Tools/I2C.cpp", "Src/Tools/Uart.cpp", "Src/Tools/PWM.cpp", "Src/Tools/IO.cpp",
          "Src/Acquisition/Wire.cpp", "Src/main.cpp" }
  links { "external", "nmea", "framework", "actuator", "sensor", "lua5.2", "pthread" }
  if device_lib[device] then links { device_lib[device] } end
  includedirs { "Src/", "/usr/include/lua5.2/", "Src/Include/Sol/", "Src/Include/Nmea/include/" }
  defines { device_def[device] }
  linkoptions { "-Wl,-rpath,./lib/" }

-------------------------------------- End -------------------------------------
