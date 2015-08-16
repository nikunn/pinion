#ifndef _IO_LINUX_H
#define _IO_LINUX_H

class IoLinux
{
public :  //======================== Public ======================
  // Helper to write
  static int write(const std::string& file, const std::string& str);
  static int write(const std::string& file, unsigned long value);

  // Helper to read
  static std::string read(const std::string& file);
};



#endif
