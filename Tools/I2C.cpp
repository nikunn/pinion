#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#include "Include/I2cTools/include/linux/i2c-dev.h"
#include "Framework/Logger.h"
#include "Framework/StopWatch.h"
#include "Tools/I2C.h"

#define I2C_RESTART  1<<8    // Repeated start
#define I2C_READ     2<<8    // Read a byte

// Define reading and writing bit for convenience
#define I2C_WRITING_BIT 0
#define I2C_READING_BIT 1

// Opens an I2C device. Example for bus number 1 "/dev/i2c-1"
int I2cLinux::i2cOpen(const std::string& device_name)
{
  // Open connection to device
  int handle = open(device_name.c_str(), O_RDWR);

  // Check
  if (handle < 0)
  {
    FATAL_PF("Could not open connection to I2C device %s", device_name.c_str());
  }

  // Define variable for check
  unsigned long funcs;

  // Perform check of I2C functionality
  if(ioctl(handle, I2C_FUNCS, &funcs) < 0)
  {
    FATAL_PF("Could not perform I2C check on device %s", device_name.c_str());
  }

  // Abort if device does not have I2C functionality
  if (!(funcs & I2C_FUNC_I2C))
  {
    FATAL_PF("I2C device %s does not have I2C functionality", device_name.c_str());
  }
  // Return the handle
  return handle;
}

// Close an I2C device
void I2cLinux::i2cClose(const int handle)
{
  // Open connection to device
  int err = close(handle);

  // Check
  if (err < 0) { ERROR_PF("Could not close connection to I2C on handle %u", handle); }
}

// Create a sequence to read some bytes and execute the communication
// Example of such sequence {0x70, 0x8a, I2C_RESTART, 0x71, I2C_READ};
int I2cLinux::i2cRead(const int handle, const byte address, const byte regis,
                      byte* data, const int bytes_num)
{
  // Benchmark I2C read actions.
  BENCH_SCOPE("I2C READ");

  // Create slave adress adding read/write bit from sensor 7bit address
  byte addr_read = (address << 1) | I2C_READING_BIT;
  byte addr_write = (address << 1) & ~I2C_READING_BIT;

  // Define the sequence length
  int sequence_len = bytes_num + 4;

  // Initialize the sequence
  uint16_t* sequence = new uint16_t[sequence_len];

  // The first byte of the sequence is the slave write address as we will write register
  sequence[0] = addr_write;

  // The second byte is the register pointer that we will read data from
  sequence[1] = regis;

  // The third byte is a restart, start a new segment to read the data
  sequence[2] = I2C_RESTART;

  // The fourth byte is the slave read/write address
  sequence[3] = addr_read;

  // Fill the rest of the sequence
  for (int i = 0; i < bytes_num; i++) { sequence[4 + i] = I2C_READ; }

  // Execute the I2C communication with this sequence
  int result = sendSequence(handle, sequence, sequence_len, data);

  // delete the sequence
  delete sequence;

  // Return result
  return result;
}

// Create a sequence to read some bytes and execute the communication
// For write the sequence is only one segment
int I2cLinux::i2cWrite(const int handle, const byte address, const byte regis,
                       const byte* data, const int bytes_num)
{
  // Benchmark I2C write actions.
  BENCH_SCOPE("I2C WRITE");

  // Create slave write adress adding write bit from sensor 7bit address
  byte addr_write = (address << 1) & ~1;

  // Define the sequence length
  int sequence_len = bytes_num + 2;

  // Initialize the sequence
  uint16_t* sequence = new uint16_t[sequence_len];

  // The first byte of the sequence is the slave write address
  sequence[0] = addr_write;

  // The second byte is the register pointer that we will write data to
  sequence[1] = regis;

  // The rest of the sequence is made of the data we want to write
  for (int i = 0; i < bytes_num; i++) { sequence[2 + i] = data[i]; }

  // Execute the I2C communication with this sequence
  int result = sendSequence(handle, sequence, sequence_len, NULL);

  // delete the sequence
  delete sequence;

  // Return result
  return result;
}

// Count the number of segments in an I2C sequence before allocating the message buffer
// There is an upper limit on the number of segments (restarts): no more than 42.
int I2cLinux::countSegments(const uint16_t *sequence, const int sequence_len)
{
  // Initialize counter, there is always at least one segment
  int segments_num = 1;

  // Go throught the sequence
  for(int i = 1; i < sequence_len; i++)
  {
    // Count the number of I2C restart which mark a segment
    if(sequence[i] == I2C_RESTART) { segments_num++; }
  }

  // Return the number of segments
  return segments_num;
}

// Sends a command/data sequence that can include restarts, writes and reads.
// Note that the sequence is composed of uint16_t, not byte.
int I2cLinux::sendSequence(const int handle, const uint16_t* sequence, const int sequence_len, byte* rcv_data)
{
  // Initialize the message sequence
  i2c_rdwr_ioctl_data message_seq;

  // Count the number of segments in the seuqnce
  int segments_num = countSegments(sequence, sequence_len);

  // Initialize the result
  int result = -1;

  // Few checks on sequence length
  if(sequence_len < 2 || segments_num > I2C_RDRW_IOCTL_MAX_MSGS)
  {
    // Dump an error log
    ERROR_LG("I2C communication failure, wrong sequence/segment length")

    // Exit, return failure code
    return result;
  }

  // Initialize an array of I2C messages
  i2c_msg* messages = new i2c_msg[segments_num];
  i2c_msg* current_message = messages;

  // msg_buf needs to hold all *bytes written* in the entire sequence.
  byte* msg_buf = new byte[sequence_len];
  byte* msg_cur_buf_ptr = msg_buf;

  // Get the address, it is always the first byte
  byte address = sequence[0];

  // The LSB of the address is the read write bit
  byte rw = address & 1;

  // Initialize the buffer
  int msg_cur_buf_size = 0;
  byte* msg_cur_buf_base = msg_cur_buf_ptr;

  // Initialize a counter to go through the sequence
  int i = 1;

  // Go through the sequence to fill the I2C structures
  while(i < sequence_len)
  {
    if(sequence[i] != I2C_RESTART)
    {
      // If we are writing, the only thing in the sequence are bytes to be written
      if(rw == I2C_WRITING_BIT) { *msg_cur_buf_ptr++ = (byte)(sequence[i]); }

      // For reads, there is nothing to be done.
      msg_cur_buf_size++;
    }

    // Check if we have a complete segment
    if(sequence[i] == I2C_RESTART || i == sequence_len - 1)
    {
      // Fill out the message structure with this complete segment
      current_message->addr = address >> 1; //Linux uses 7-bit addresses
      current_message->flags = rw ? I2C_M_RD : 0;
      current_message->len = msg_cur_buf_size;

      // Buf needs to point to either the buffer that will receive data
      // or buffer that holds bytes to be written
      current_message->buf = (char*) (rw ? rcv_data : msg_cur_buf_base);

      // Increment the current message pointer
      current_message++;

      // If this is a read, increment
      if(rw == I2C_READING_BIT) { rcv_data += msg_cur_buf_size; }

      // Check if there is more transaction
      if(i < sequence_len - 2)
      {
        // Get the address, it is always the first byte
        address = sequence[++i];

        // The LSB of the address is the read write bit
        rw = address & 1;

        // Initialize the buffer
        msg_cur_buf_size = 0;
        msg_cur_buf_base = msg_cur_buf_ptr;
      }
    }

    // Increment the counter
    i++;
  }

  // Populate the message sequence structure
  message_seq.msgs = messages;
  message_seq.nmsgs = segments_num;

  // Execute the I2C communication
  result = ioctl(handle, I2C_RDWR, (unsigned long)(&message_seq));

  // Free the messages and buffer memory
  delete messages;
  delete msg_buf;

  // Return the result
  return result;
}
