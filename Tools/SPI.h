#ifndef _SPI_LINUX_H
#define _SPI_LINUX_H

#include "Framework/Types.h"


namespace pno
{
//=========================== Forward Declarations =============================
class SpiWire;

//================================= SpiLinux ===================================
class SpiLinux
{
public :  //======================== Public ======================
  static void spiInit(SpiWire& wire);
  static int spiTransfer(const SpiWire& wire, const byte* wdata, byte* rdata, const int bytes_num);
};


}
#endif
