#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Framework/Logger.h"
#include "Tools/IO.h"


namespace pno
{
//================================== IoLinux ===================================
// Open a file
int IoLinux::openFile(const std::string& file)
{
  // Open connection to device
  int handle = open(file.c_str(), O_RDONLY);

  // Check
  if (handle < 0)
  {
    FATAL_PF("Could not open file %s", file.c_str());
  }

  // Return handle
  return handle;
}

// Write to file
int IoLinux::write(const std::string& file, const std::string& str)
{
  // Define file to write to
  std::ofstream fs;
  fs.open(file.c_str());

  // Check if file is open
  if (!fs.is_open())
  {
    // A bit of log
    ERROR_PF("Cannot open file %s for write", file.c_str());

    // Return error code
    return -1;
  }

  // Append value
  fs << str.c_str();

  // Close file
  fs.close();

  // Return
  return 0;
}

// Write to file
int IoLinux::write(const std::string& file, unsigned long value)
{
  return write(file, std::to_string(value));
}

// Read from file
std::string IoLinux::read(const std::string& file)
{
  // Define file to read from
  std::ifstream fs;
  fs.open(file.c_str());

  // Check if file is open
  if (!fs.is_open()) 
  {
    // Dump an error log
    ERROR_PF("Cannot read from file %s", file.c_str());
  }

  // Define input
  std::string input;

  // Read from file
  getline(fs, input);

  // Close file
  fs.close();

  // Return result
  return input;
}


}
