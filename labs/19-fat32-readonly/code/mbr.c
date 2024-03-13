#include "rpi.h"
#include "pi-sd.h"
#include "mbr.h"

mbr_t *mbr_read() {
  // Be sure to call pi_sd_init() before calling this function!

  pi_sd_init();

  // TODO: Read the MBR into a heap-allocated buffer.  Use `pi_sd_read` or
  // `pi_sec_read` to read 1 sector from LBA 0 into memory.
  // unimplemented();
  // // allocate <nsec> worth of space, read in from SD card, return pointer.
  // // your kmalloc better work!
  // void *pi_sec_read(uint32_t lba, uint32_t nsec) {
  // uint8_t buf[512];
  void * buf = pi_sec_read(0, 1);
  // pi_sec_read(0, 1, (uint8_t *)0x8000);

  // TODO: Verify that the MBR is valid. (see mbr_check)
  // unimplemented();
  // mbr_check((mbr_t *)0x8000);
  mbr_check((mbr_t *)buf);

  // TODO: Return the MBR.
  // unimplemented();
  // return (mbr_t *)0x8000;
  return (mbr_t *)buf;
}
