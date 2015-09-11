#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "Framework/Logger.h"
#include "Framework/StopWatch.h"
#include "Acquisition/Wire.h"
#include "Tools/SPI.h"


namespace pno
{
//================================= SpiLinux ===================================
// Initialize an SPI device
void SpiLinux::spiInit(SpiWire& wire)
{
  // Get handle
  const int handle = wire.handle();

  // Set mode
  if (ioctl(handle, SPI_IOC_WR_MODE, &wire.mode()) < 0)
  {
    FATAL_PF("Could not set SPI mode for handle %u", handle);
  }

  // Get the mode
  if (ioctl(handle, SPI_IOC_RD_MODE, &wire.mode()) < 0)
  {
    FATAL_PF("Could not get SPI mode for handle %u", handle);
  }

  // Set speed
  if (ioctl(handle, SPI_IOC_WR_MAX_SPEED_HZ, &wire.speed()) < 0)
  {
    FATAL_PF("Could not set SPI speed for handle %u", handle);
  }

  // Get the speed
  if (ioctl(handle, SPI_IOC_RD_MAX_SPEED_HZ, &wire.speed()) < 0)
  {
    FATAL_PF("Could not get SPI speed for handle %u", handle);
  }

  // Set the number of bits per word
  if (ioctl(handle, SPI_IOC_WR_BITS_PER_WORD, &wire.wordBits()) < 0)
  {
    FATAL_PF("Could not get SPI bits per word for handle %u", handle);
  }

  // Set the number of bits per word
  if (ioctl(handle, SPI_IOC_RD_BITS_PER_WORD, &wire.wordBits()) < 0)
  {
    FATAL_PF("Could not get SPI bits per word for handle %u", handle);
  }
}

// Transfer data, read and write operations
// Both read and write buffer has to be of same length
int SpiLinux::spiTransfer(const SpiWire& wire, const byte* wdata, byte* rdata, const int bytes_num)
{
  // Benchmark SPI write actions.
  BENCH_SCOPE("SPI TRANSFER");

  // Initialize the SPI transfer data structure
  struct spi_ioc_transfer txinfo;
  txinfo.tx_buf = (__u64) wdata;
  txinfo.rx_buf = (__u64) rdata;
  txinfo.len = bytes_num;
  txinfo.delay_usecs = 0;
  txinfo.speed_hz = wire.speed();
  txinfo.bits_per_word = wire.wordBits();

  // Execute the transfer
  if (ioctl(wire.handle(), SPI_IOC_MESSAGE(1), &txinfo) < 0)
  {
    FATAL_PF("Could not transfer SPI data to device handle %u", wire.handle());
  }

  // Return 
  return bytes_num;
}


}
