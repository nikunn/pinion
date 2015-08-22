#ifndef _IO_LINUX_H
#define _IO_LINUX_H


namespace pno
{
//================================== IoLinux ===================================
class IoLinux
{
public :  //======================== Public ======================
  // Helper to open
  static int openFile(const std::string& file);

  // Helper to write
  static int write(const std::string& file, const std::string& str);
  static int write(const std::string& file, unsigned long value);

  // Helper to read
  static std::string read(const std::string& file);
};


}
#endif
