//#include <sys/types.h>
//#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
//#include <stdio.h>
//#include <stdlib.h>

#include "Include/I2cTools/include/linux/i2c-dev.h"
#include "Framework/Logger.h"
#include "Tools/I2C.h"

#define I2C_RESTART  1<<8    // Repeated start
#define I2C_READ     2<<8    // Read a byte

// Definition to match easily the LSB of the adress
#define WRITING 0
#define READING 1

// Length in number of character of the I2C device name
// Example: "/dev/i2c-1" + the terminating 0
#define I2C_DEVICE_NAME_LEN 11

// Opens an I2C device. Example of bus number 1 "/dev/i2c-1"
int I2cLinux::i2cOpen(int bus)
{
  // Define the device name
  char device_name[I2C_DEVICE_NAME_LEN];

  // Check the bus number
  if(bus < 0 || bus > 9) { FATAL_PF("I2C Bus number not reconized: %u", bus); }

  // Create the device name from the bus number
  snprintf(device_name, I2C_DEVICE_NAME_LEN, "/dev/i2c-%u", bus);

  // Open connection to device
  int handle = open(device_name, O_RDWR);

  // Check
  if (handle < 0)
  {
    FATAL_PF("Could not open connection to I2C device %s", device_name);
  }

  // Check the I2C functionality of this device
  i2cCheck(handle, device_name);

  // Return the handle
  return handle;
}

void I2cLinux::i2cCheck(int handle, char* device_name)
{
  // Define variable for check
  unsigned long funcs;

  // Perform check of I2C functionality
  if(ioctl(handle, I2C_FUNCS, &funcs) < 0)
  {
    FATAL_PF("Could not perform I2C check on device %s", device_name);
  }

  // Abort if device does not have I2C functionality
  if (!(funcs & I2C_FUNC_I2C))
  {
    FATAL_PF("I2C device %s does not have I2C functionality", device_name);
  }
}

// Close an I2C device
void I2cLinux::i2cClose(int handle)
{
  // Open connection to device
  int err = close(handle);

  // Check
  if (err < 0) { ERROR_PF("Could not close connection to I2C on handle %u", handle); }
}

// Create a sequence to read some bytes and execute the communication
// Example of such sequence {0x70, 0x8a, I2C_RESTART, 0x71, I2C_READ};
int I2cLinux::i2cCom(const int handle, const byte address, const byte regis, 
                     const bool write, byte* data, const int bytes_num)
{
  // Create slave adress plus read/write bit for convenience
  byte addr_read = address | 1;
  byte addr_write =address & ~1;

  // Define the sequence length
  int sequence_len = bytes_num + 4;
  
  // Initialize the sequence
  uint16_t* sequence = new uint16_t[sequence_len];

  // The first byte of the sequence is the slave write address
  sequence[0] = addr_write;

  // The second byte is the register pointer that we will read/write data from later
  sequence[1] = regis;

  // The third byte is a restart
  sequence[2] = I2C_RESTART;

  // The fourth byte is the slave read/write address
  sequence[3] = write ? addr_write : addr_read;

  // Fill the rest of the sequence
  for (int i = 0; i < bytes_num; i++)
  {
    // The rest of the sequence is made of the data we want to write
    if (write)
    {
      sequence[3 + i] = data[i];
    }
    // The rest of the sequence is made of as many I2C_READ as bytes_num
    else
    {
      sequence[3 + i] = I2C_READ;
    }
  }

  // Execute the I2C communication with this sequence
  int result = sendSequence(handle, sequence, sequence_len, write ? NULL : data);

  // delete the sequence
  delete sequence;
  
  // Return result
  return result;
}

// Count the number of segments in an I2C sequence befor allocating the message buffer
int I2cLinux::countSegments(uint16_t *sequence, int sequence_len)
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

/*
  Sends a command/data sequence that can include restarts, writes and reads.

  sequence is the I2C operation sequence that should be performed. 
  reads. Note that the sequence is composed of uint16_t, not byte.
  This is because we have to support out-of-band
  signalling of I2C_RESTART and I2C_READ operations, while still passing through 8-bit data.

  there is an upper limit on the number of segments (restarts): no more than 42.
*/
int I2cLinux::sendSequence(int handle, uint16_t* sequence, int sequence_len, byte* rcv_data)
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

  /* msg_buf needs to hold all *bytes written* in the entire sequence.
   * Since it is difficult to estimate that number without processing the sequence,
   * we make an upper-bound guess: sequence_len. Yes, this is inefficient, but
     optimizing this doesn't seem to be worth the effort. */

  byte* msg_buf = new byte[sequence_len];
  byte* msg_cur_buf_ptr = msg_buf;
  byte* msg_cur_buf_base;
  int msg_cur_buf_size;

  // The I2C adress
  byte address;

  // Whether we are writing or reading
  byte rw;

  // Get the address, it is always the first byte
  address = sequence[0];

  // The LSB of the address is the read write bit
  rw = address & 1;

  // Initialize the buffer
  msg_cur_buf_size = 0;
  msg_cur_buf_base = msg_cur_buf_ptr;

  // Initialize a counter to go through the sequence
  int i = 1;

  // Go through the sequence to fill the I2C structures
  while(i < sequence_len)
  {
    if(sequence[i] != I2C_RESTART)
    {
      // If we are writing, the only thing in the sequence are bytes to be written
      if(rw == WRITING) { *msg_cur_buf_ptr++ = (byte)(sequence[i]); }

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
      if(rw == READING) { rcv_data += msg_cur_buf_size; }

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

  // 
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
