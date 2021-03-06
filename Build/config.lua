#!/usr/bin/lua

-- Definiton of data acquisition device
local daq_device = "UXB"

----------------------------------- Universe -----------------------------------
-- List of all object to be created, in this order
Universe =
{
  -- Data acquisition device
  {
    daq_device,
  },

  -- List of wires connecting the sensors
  --[[
  {
    -- Uart wire connecting GPS
    "UartWire",

    -- I2C wire connecting sensors
    "I2cWire",

    -- SPI wire connecting sensors
    "SpiWire",
  },
  ]]--

  -- List of actuators plugged to data acquisition devices
  {
    -- PWM
    -- "Pulser",
  },

  -- List of GPIO
  {
    -- GPIO bank 1
    "GpioBank1",

    -- GPIO
    "Gpio44",
    "Gpio61",
  },

  -- List of sensors plugged to data acquisition devices
  {
    -- Counter
    -- "Counter",

    -- GPS Sensor
    -- "GPS",

    -- Accelerometer
    -- "AccelADXL",

    -- Gyroscope
    --"Gyro",

    -- Magnetometer
    -- "Magneto",
  },
}

------------------------------------ Catalog -----------------------------------
Catalog =
{
    ------------------------------ Acquisition -----------------------------
  UXB =
  {
    name = "UXB",
    class = "LinuxBoard",
  },

    ---------------------------------- Wire --------------------------------
  UartWire =
  {
    name = "UartWire",
    class = "UartWire",
    daq = daq_device,
    device = "/dev/ttyO1",
    com_type = "asynch", -- serial or asynch
    default_baud = 9600,
  },

  I2cWire =
  {
    name = "I2cWire",
    class = "I2cWire",
    daq = daq_device,
    device = "/dev/i2c-1",
  },

  SpiWire =
  {
    name = "SpiWire",
    class = "SpiWire",
    mode = 3,
    word_bits = 8,
    speed = 500000,
    daq = daq_device,
    device = "/dev/spidev1.0",
  },

    ------------------------------- Actuators ------------------------------
  Pulser =
  {
    name = "Pulser",
    class = "Pulser",
    daq = daq_device,
    pin = "/sys/devices/ocp.3/pwm_test_P9_14.12", -- path to device
    polarity = true, -- true is High
    frequency = 1000, -- in Hz
    duty_cycle = 0.5, -- in Percent
  },

    ------------------------------- GPIO Bank ------------------------------
  GpioBank0 =
  {
    name = "GpioBank0",
    class = "GpioBank",
    file = "/dev/mem",
    address = 0x44E07000,
  },

  GpioBank1 =
  {
    name = "GpioBank1",
    class = "GpioBank",
    file = "/dev/mem",
    address = 0x4804C000,
  },

  GpioBank2 =
  {
    name = "GpioBank2",
    class = "GpioBank",
    file = "/dev/mem",
    address = 0x481AC000,
  },

  GpioBank3 =
  {
    name = "GpioBank3",
    class = "GpioBank",
    file = "/dev/mem",
    address = 0x481AE000,
  },

    --------------------------------- GPIO ---------------------------------
  -- GPIO 44 P8_12 gpio1[12] (pull-down)
  Gpio44 =
  {
    name = "Gpio44",
    class = "GpioMem",
    pin = "/sys/class/gpio/gpio44/",
    direction = "out",
    edge = "none",
    bank = "GpioBank1",
    gpio = 12,
  },

  -- GPIO 61 P8_26 gpio1[29] (pull down)
  Gpio61 =
  {
    name = "Gpio61",
    class = "GpioMem",
    pin = "/sys/class/gpio/gpio61/",
    direction = "out",
    edge = "none",
    bank = "GpioBank1",
    gpio = 29,
  },

    -------------------------------- Sensors -------------------------------
  Counter =
  {
    name = "Counter",
    class = "Counter",
    daq = daq_device,
    pin = "/sys/class/gpio/gpio115/",
    direction = "in",
    edge = "rising",
  },

  GPS =
  {
    name = "GPS",
    class = "GPS",
    daq = daq_device,
    wire = "UartWire",
    start_baud = 9600,
    rate = 10, -- in Hz, (1/5/10)
  },

  AccelLSM =
  {
    name = "AccelLSM",
    class = "LSM303",
    daq = daq_device,
    wire = "I2cWire",
    period_ms = 400, -- in Millisecond
  },

  AccelADXL =
  {
    name = "AccelADXL",
    class = "ADXL345",
    daq = daq_device,
    wire = "SpiWire",
    period_ms = 400, -- in Millisecond
  },

  Magneto =
  {
    name = "Magneto",
    class = "MAG303",
    daq = daq_device,
    wire = "I2cWire",
    gain = 1.3, -- in something, (1.3/1.9/2.5/4.0/4.7/5.6/8.1)
    period_ms = 200, -- in Millisecond
  },

  Gyro =
  {
    name = "Gyro",
    class = "L3GD20",
    daq = daq_device,
    wire = "SpiWire",
    range = 250, -- in degree per second, (250/500/2000)
    period_ms = 100, -- in Millisecond
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
    level = "GLOBAL",

    configuration =
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
    level = "DEBUG",

    configuration =
    {
    },
  },
}

-------------------------------------- End -------------------------------------
