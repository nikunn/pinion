#!/usr/bin/lua

----------------------------------- Universe -----------------------------------
-- List of all object to be created, in this order
Universe =
{
  -- List of data acquisition devices
  {
    -- LabJack Device
    "LBJ",

    -- Arduino Device
    --"ARD1",

    -- Raspberry Device
    --"RSB1",
  },

  -- List of wires connecting the sensors
  {
    -- Asynch wire connecting GPS
    "AsynchWire",
  },

  -- List of sensors plugged to data acquisition devices
  {
    -- GPS Sensor
    "GPS",

    -- Accelerometer
    --"Accel",

    -- Gyroscope
    --"Gyro",

    -- Magnetometer
    --"Magneto",
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

    ---------------------------------- Wire --------------------------------
  AsynchWire = 
  {
    name="AsynchWire",
    class="AsynchWire",
    transmit_line = 1,
    receive_line = 0,
  },

    -------------------------------- Sensors -------------------------------
  GPS =
  {
    device = "LBJ",
    name="GPS",
    class="GPS",
    wire = "AsynchWire",
    start_baud=9600,
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
