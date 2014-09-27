#!/usr/bin/lua

----------------------------------- Universe -----------------------------------
-- List of all object to be created, in this order
Universe =
{
  -- List of data acquisition devices
  {
    -- LabJack Device
    --"LBJ",

    -- Raspberry Device
    "RPI",

    -- BeagleBone Device
    --"BBB",
  },

  -- List of wires connecting the sensors
  {
    -- Asynch wire connecting GPS
    "AsynchWire",

    -- I2C wire connecting sensors
    "I2cWire",
  },

  -- List of sensors plugged to data acquisition devices
  {
    -- GPS Sensor
    "GPS",

    -- Accelerometer
    "Accel",

    -- Gyroscope
    "Gyro",

    -- Magnetometer
    "Magneto",
  },
}

------------------------------------ Catalog -----------------------------------
Catalog =
{
    ------------------------------ Acquisition -----------------------------
  LBJ =
  {
    name = "LBJ",
    class = "LabJack",
  },

  RPI =
  {
    name = "RPI",
    class = "RaspberryPi",
  },

    ---------------------------------- Wire --------------------------------
  AsynchWire = 
  {
    name="AsynchWire",
    class="AsynchWire",
    transmit_line = 1,
    receive_line = 0,
  },

  I2cWire = 
  {
    name="I2cWire",
    class="I2cWire",
    bus = 1,
    data_line = 2,
    clock_line = 3,
  },

    -------------------------------- Sensors -------------------------------
  GPS =
  {
    device = "RPI",
    name="GPS",
    class="GPS",
    wire="AsynchWire",
    start_baud=9600,
    rate=10, -- in Hz, (1/5/10)
  },

  Accel =
  {
    device = "RPI",
    name="Accel",
    class="LSM303",
    wire="I2cWire",
    gravity_axis="Z",
  },

  Magneto =
  {
    device = "RPI",
    name="Magneto",
    class="MAG303",
    wire="I2cWire",
    gain=1.3, -- in something, (1.3/1.9/2.5/4.0/4.7/5.6/8.1)
  },

  Gyro =
  {
    device = "RPI",
    name="Gyro",
    class="L3GD20",
    wire="I2cWire",
    range=250, -- in degree per second, (250/500/2000)
  },
}

------------------------------------ Logger ------------------------------------
-- Define the configuration of the logger
-- Level are "GLOBAL" "DEBUG" "INFO" "WARNING" "ERROR" "FATAL" "VERBOSE" "TRACE"
Logger = 
{
  -- Values are always string
  {
    -- Level of logging
    level="GLOBAL",

    configuration=
    {
      -- Is this level enabled (bool)
      { field = "enabled" , value = "true" },

      -- Whether or not to write corresponding log to log file (bool)
      { field = "to_file" , value = "true" },

      -- Whether or not to write logs to standard output (bool)
      { field = "to_standard_output" , value = "true" },

      -- Determines format/pattern of logging for corresponding level (string)
      { field = "format" , value = "%datetime{%h:%m:%s.%g} %level - %msg" },

      -- Determines log file (full path) to write logs to for corresponding 
      -- level and logger (string)
      { field = "filename" , value = "logs/run.txt" },

      -- Specifies milliseconds width. Width can be within range (1-6) (uint)
      { field = "milliseconds_width" , value = "6" },

      -- bool   Determines whether or not performance tracking is enabled (bool)
      --{ field = "performance_tracking" , value = "" },

      -- If log file size of corresponding level is >= specified size,
      -- log file will be truncated (size_t)
      --{ field = "max_log_file_size" , value = "" },

      -- Specifies number of log entries to hold until we flush pending log data (size_t)
      --{ field = "log_flush_threshold" , value = "" },
    },
  },

  {
    level="DEBUG",

    configuration=
    {
    },
  },
}

-------------------------------------- End -------------------------------------
