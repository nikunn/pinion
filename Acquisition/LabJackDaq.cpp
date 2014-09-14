#include "LabJackM.h"

#include "Framework/Logger.h"
#include "Acquisition/LabJackDaq.h"
#include "Acquisition/Wire.cpp"
#include "Sensor/Sensor.h"

//=============================== LabJackDaq ================================
// Constructor
LabJackDaq::LabJackDaq(const LuaTable&)
{
  _i2c_wire = NULL;
};

// Destructor
LabJackDaq::~LabJackDaq()
{
  // Close the lab jack device
  close();
};

// Open connection to labjack
void LabJackDaq::open()
{
  // Define Error.
  int err;

  // Open first found LabJackDaq.
  err = LJM_OpenS("LJM_dtANY", "USB", "LJM_idANY", &_handle);
  errorCheck(err, "LJM_Open");
}

// Close connection to labjack
void LabJackDaq::close()
{
  // Define Error.
  int err;

  // Close.
  err = LJM_Close(handle());
  errorCheck(err, "LJM_Close");
}

// Print some informations about this labjack device
void LabJackDaq::info()
{

  int err;
  int portOrPipe, ipAddress, serialNumber, packetMaxBytes;
  int deviceType = LJM_dtANY;
  int connectionType = LJM_ctANY;
  char string[LJM_STRING_ALLOCATION_SIZE];

  // Get device name.
  err = LJM_eReadNameString(handle(), "DEVICE_NAME_DEFAULT", string);
  if (err == LJME_NOERROR) { INFO_PF("DEVICE_NAME_DEFAULT: %s\n", string); }
  else { FATAL_LG("This device does not have a name\n"); }

  // Get device info.
  err = LJM_GetHandleInfo(handle(), &deviceType, &connectionType, &serialNumber, &ipAddress,
      &portOrPipe, &packetMaxBytes);
  errorCheck(err, "LJM_GetHandleInfo");

  // Print device info.
  INFO_LG("The maximum number of bytes in one packet is %d bytes");
}

void LabJackDaq::errorCheck(int err, const char * formattedDescription)
{
  // Define the string that contains warnings/errors
  char errName[LJM_MAX_NAME_SIZE];

  // Check if this is a warning
  if (err >= LJME_WARNINGS_BEGIN && err <= LJME_WARNINGS_END)
  {
    // Get the string associated with this warning
    LJM_ErrorToString(err, errName);

    // Print a warning
    WARNING_PF("LabJack: \"%s\" (warning code: %d)", errName, err);
  }
  // Check if this is an error
  else if (err != LJME_NOERROR)
  {
    // Get the string associated with this error
    LJM_ErrorToString(err, errName);

    // Close connection to LabJack
    LJM_CloseAll();

    // Print a fatal and exit
    FATAL_PF("LabJack: \"%s\" (error code: %d)", errName, err);
  }
}

//=============================== I2c Communication ============================

// Initialize the I2C communication for this sensor (has to be done for each sensor each use).
void LabJackDaq::i2cSet(const I2cSensor& sensor)
{
  // Get the wire for this sensor
  const I2cWire* wire = static_cast<const I2cWire*>(sensor.wire());

  // Define some variable
  int err;

  // Check if we need to set the I2C configuration for this wire
  if (_i2c_wire != wire)
  {
    // Get the I2C wire line
    int data_line = wire->dataLine();
    int clock_line = wire->clockLine();

    // Configure the I2C data line, SDA pin number
    err = LJM_eWriteName(handle(), "I2C_SDA_DIONUM", data_line);
    errorCheck(err, "LJM_eWriteName (I2C_SDA_DIONUM)");

    // Configure the I2C clock line, SCL pin number
    err = LJM_eWriteName(handle(), "I2C_SCL_DIONUM", clock_line);
    errorCheck(err, "LJM_eWriteName (I2C_SCL_DIONUM)");

    // Speed throttle is inversely proportional to clock frequency. 0 = max
    err = LJM_eWriteName(handle(), "I2C_SPEED_THROTTLE", 0);
    errorCheck(err, "LJM_eWriteName (I2C_SPEED_THROTTLE)");

    // A bit of log
    INFO_PF("Setting LabJack:%u I2C with data:%u clock:%u",
      handle(), data_line, clock_line);
  }

  // Options bits:
  // bit0: Reset the I2C bus.
  err = LJM_eWriteName(handle(), "I2C_OPTIONS", 0);
  errorCheck(err, "LJM_eWriteName (I2C_OPTIONS)");

  // Configure the I2C address of Slave
  err = LJM_eWriteName(handle(), "I2C_SLAVE_ADDRESS", sensor.address());
  errorCheck(err, "LJM_eWriteName (I2C_SLAVE_ADDRESS)");

  // Set the I2C wire
  _i2c_wire = wire;
}

// Read data in I2C
void LabJackDaq::i2cRead(const I2cSensor& sensor, const byte regis, byte* data, const int bytes_num)
{
  // Set the configuration for communicating with this sensor
  i2cSet(sensor);

  // Define some variable
  int err, i;
  int errorAddress = INITIAL_ERR_ADDRESS;

  // Set the number of bytes to transmit
  err = LJM_eWriteName(handle(), "I2C_NUM_BYTES_TX", 1);
  errorCheck(err, "LJM_eWriteName (I2C_NUM_BYTES_TX)");

  // Set the number of bytes to receive
  err = LJM_eWriteName(handle(), "I2C_NUM_BYTES_RX", bytes_num);
  errorCheck(err, "LJM_eWriteName (I2C_NUM_BYTES_RX)");

  // Set the adress to read, from the memory pointer
  double regis_array[1];
  regis_array[0] = (double)regis;
  
  // Prepare the communication to write the address we will read from
  err = LJM_eWriteNameArray(handle(), "I2C_WRITE_DATA", 1, regis_array, &errorAddress);
  errorCheck(err, "LJM_eNames (I2C_WRITE_DATA)");

  // Do the I2C communications.
  err = LJM_eWriteName(handle(), "I2C_GO", 1);
  errorCheck(err, "LJM_eWriteName (I2C_GO)");

  // RX bytes will go here
  double data_rx[I2C_PACKET_MAX_SIZE];

  // Initialize the array of data to be read
  for (i = 0; i < bytes_num; i++) { data_rx[i] = 0; }

  // Get the data back from device to user space
  err = LJM_eReadNameArray(handle(), "I2C_READ_DATA", bytes_num, data_rx, &errorAddress);
  errorCheck(err, "LJM_eNames (I2C_READ_DATA)");

  // Populate the array of data
  for (i = 0; i < bytes_num; i++) { data[i] = (byte) data_rx[i]; }
}

// Write data in I2C
void LabJackDaq::i2cWrite(const I2cSensor& sensor, const byte regis, byte* data, int bytes_num)
{
  // Set the configuration for communicating with this sensor
  i2cSet(sensor);

  // Write EEPROM bytes in the user memory area, using the page write technique. Note
  // that page writes are limited to 16 bytes max, and must be aligned with the 16-byte
  // page intervals. For instance, if you start writing at address 14, you can only write
  // two bytes because byte 16 is the start of a new page.

  // We need to put the register we are going to write to at the begining of the data
  bytes_num += 1;

  // Initialize some variables
  int err, i;
  int errorAddress = INITIAL_ERR_ADDRESS;

  // Set the number of bytes to transmit
  err = LJM_eWriteName(handle(), "I2C_NUM_BYTES_TX", bytes_num);
  errorCheck(err, "LJM_eWriteName (I2C_NUM_BYTES_TX)");

  // Set the number of bytes to receive
  err = LJM_eWriteName(handle(), "I2C_NUM_BYTES_RX", 0);
  errorCheck(err, "LJM_eWriteName (I2C_NUM_BYTES_RX)");

  // TX/RX bytes will go here
  double data_tx[I2C_PACKET_MAX_SIZE];
  
  // Set the adress to write to, the memory pointer, first thing to write
  data_tx[0] = (double)regis;

  // Populate the array with data to write. Careful on the index as the first one is the register!
  for (i = 1; i < bytes_num; i++) { data_tx[i] = (double)(data[i-1]); }

  // Prepare the communication, put the data in device
  err = LJM_eWriteNameArray(handle(), "I2C_WRITE_DATA", bytes_num, data_tx, &errorAddress);
  errorCheck(err, "LJM_eNames (I2C_WRITE_DATA)");

  // Do the I2C communications.
  err = LJM_eWriteName(handle(), "I2C_GO", 1);
  errorCheck(err, "LJM_eWriteName (I2C_GO)");
}


//============================== Asynch Communication ==========================

// Initialize the Asynch communication for this sensor
void LabJackDaq::asynchSet(const AsynchSensor& sensor)
{
  // Get the Asynch wire for this sensor
  const AsynchWire* wire = sensor.wire();

  // Define some variable
  int err;

  // Configure the asynch receiving line
  err = LJM_eWriteName(handle(), "ASYNCH_TX_DIONUM", wire->transmitLine());
  errorCheck(err, "LJM_eWriteName (ASYNCH_TX_DIONUM)");

  // Configure the asynch transmitting line
  err = LJM_eWriteName(handle(), "ASYNCH_RX_DIONUM", wire->receiveLine());
  errorCheck(err, "LJM_eWriteName (ASYNCH_RX_DIONUM)");

  // Configure the number of data bits
  err = LJM_eWriteName(handle(), "ASYNCH_NUM_BITS", 8);
  errorCheck(err, "LJM_eWriteName (ASYNCH_NUM_BITS)");

  // Configure the receiving buffer size
  err = LJM_eWriteName(handle(), "ASYNCH_RX_BUFFER_SIZE_BYTES", ASYNCH_BUFFER_SIZE);
  errorCheck(err, "LJM_eWriteName (ASYNCH_RX_BUFFER_SIZE_BYTES)");

  // Configure the baud rate
  err = LJM_eWriteName(handle(), "ASYNCH_BAUD", sensor.baud());
  errorCheck(err, "LJM_eWriteName (ASYNCH_BAUD)");
}

// Turn on Asynch. Configures timing hardware, DIO lines and allocates the receiving buffer
void LabJackDaq::asynchStart()
{
  // Define some variable
  int err;

  // Enable Asynch communication
  err = LJM_eWriteName(handle(), "ASYNCH_ENABLE", 1);
  errorCheck(err, "LJM_eWriteName (ASYNCH_ENABLE)");
}

// Turn off asynch
void LabJackDaq::asynchStop()
{
  // Define some variable
  int err;

  // Disable Asynch communication
  err = LJM_eWriteName(handle(), "ASYNCH_ENABLE", 0);
  errorCheck(err, "LJM_eWriteName (ASYNCH_ENABLE)");
}

// Read data in asynch
void LabJackDaq::asynchRead(byte* data, int& bytes_read)
{
  // Initialize the number of byte read to 0
  bytes_read = 0;

  // Define some variable
  int err, i;
  int errorAddress = INITIAL_ERR_ADDRESS;

  // Get the number of bytes that have been received
  double bytes_buffer;
  err = LJM_eReadName(handle(), "ASYNCH_NUM_BYTES_RX", &bytes_buffer);
  errorCheck(err, "LJM_eWriteName (ASYNCH_NUM_BYTES_RX)");

  // Get the number of bytes that have been received
  double bytes_buffer_check;
  err = LJM_eReadName(handle(), "ASYNCH_NUM_BYTES_RX", &bytes_buffer_check);
  errorCheck(err, "LJM_eWriteName (ASYNCH_NUM_BYTES_RX)");

  // Check if data is currently being written to the device
  if ((int) bytes_buffer != (int) bytes_buffer_check)
  {
    // No need to go further as we will mess the data
    return;
  }

  // Read the bytes by small chunk of USB_PACKET_SIZE
  while (bytes_buffer > 0)
  {
    // Initiliaze number of byte to read from device
    int bytes_num = std::min((int) bytes_buffer, USB_PACKET_SIZE);

    // RX bytes will go here
    double data_rx[USB_PACKET_SIZE];

    // Initialize the array of data to be read
    for (i = 0; i < USB_PACKET_SIZE; i++) { data_rx[i] = 0; }

    // Get the data back from device
    err = LJM_eReadNameArray(handle(), "ASYNCH_DATA_RX", bytes_num, data_rx, &errorAddress);
    errorCheck(err, "LJM_eNames (ASYNCH_READ_DATA)");

    // Check we are not exceeding the buffer size
    if (bytes_read + bytes_num >= ASYNCH_BUFFER_SIZE)
    {
      // Print an error log
      ERROR_LG("LabJack Asynch data is exceeding the buffer");

      return;
    }
    else
    {
      // Go through the read data
      for (i = 0; i < bytes_num; i++)
      {
        // Copy the data from the buffer to the results
        data[bytes_read] = data_rx[i];

        // Increment the number of bytes read
        bytes_read += 1;
      }

      // Get the number of byte to be read for next iteration
      err = LJM_eReadName(handle(), "ASYNCH_NUM_BYTES_RX", &bytes_buffer);
      errorCheck(err, "LJM_eWriteName (ASYNCH_NUM_BYTES_RX)");
    }
  }
}

// Write data to asynch
void LabJackDaq::asynchWrite(const CommandPacket& cmd)
{
  // Get the string to pass to the sensor
  const std::string& cmd_str = cmd.command;

  // Some log
  DEBUG_PF("command: %s\n", cmd_str.c_str());

  // Define some variable
  int err, i;
  int errorAddress = INITIAL_ERR_ADDRESS;

  // Set the number of bytes to transmit
  err = LJM_eWriteName(handle(), "ASYNCH_NUM_BYTES_TX", cmd_str.size());
  errorCheck(err, "LJM_eWriteName (ASYNCH_NUM_BYTES_TX)");

  // TX bytes will go here
  double data_tx[USB_PACKET_SIZE];

  // Initalize the number of bytes to write and already written
  int bytes_command = cmd_str.size();
  int bytes_written = 0;

  // Loop to write the data in the device
  while (bytes_written < bytes_command)
  {
    // Compute number of bytes to write
    int bytes_towrite = std::min(bytes_command - bytes_written, USB_PACKET_SIZE);

    // Initialize the array of data to be read
    for (i = 0; i < bytes_towrite; i++)
    {
      // Input the current char as double in the array to write
      data_tx[i] = (double) ((cmd_str.c_str())[bytes_written + i]);
    }

    // Write the data to device
    err = LJM_eWriteNameArray(handle(), "ASYNCH_DATA_TX", bytes_towrite, data_tx, &errorAddress);
    errorCheck(err, "LJM_eNames (ASYNCH_WRITE_DATA)");

    // Update the number of bytes already written to the device
    bytes_written += bytes_towrite;
  }

  // Write to device the number of bytes that are to be transmitted
  double bytes_buffer;
  err = LJM_eReadName(handle(), "ASYNCH_NUM_BYTES_TX", &bytes_buffer);
  errorCheck(err, "LJM_eWriteName (ASYNCH_NUM_BYTES_TX)");

  // Asynch communication to write the data from the device to the sensor
  err = LJM_eWriteName(handle(), "ASYNCH_TX_GO", 1);
  errorCheck(err, "LJM_eWriteName (ASYNCH_TX_GO)");
}
