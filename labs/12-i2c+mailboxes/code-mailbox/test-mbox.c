#include "rpi.h"
#include "mbox.h"

uint32_t rpi_temp_get(void) ;

#include "cycle-count.h"

// compute cycles per second using
//  - cycle_cnt_read();
//  - timer_get_usec();
unsigned cyc_per_sec(void) {
    // todo("implement this!\n");
    // unsigned start = timer_get_usec();
    unsigned start_cycles = cycle_cnt_read();
    delay_ms(1000);
    unsigned end_cycles = cycle_cnt_read();
    // unsigned end = timer_get_usec();
    unsigned cycles = end_cycles - start_cycles;
    // unsigned time = end - start;
    return cycles / 1000;
}
// 

void notmain(void) { 
    output("mailbox serial number = %llx\n", rpi_get_serialnum());
    // todo("implement the rest");
    // while (GET32(MBOX_STATUS) & MAILBOX_FULL)
    // ;

    // We can then send by writing the address
    // of the buffer to `MBOX_WRITE` bitwise-or'd with the channel we
    // send on (the document states this should be 8).  

    output("mailbox revision number = %x\n", rpi_get_revision());
    output("mailbox model number = %x\n", rpi_get_model());

    uint32_t size = rpi_get_memsize();
    output("mailbox physical mem: size=%d (%dMB)\n", 
            size, 
            size/(1024*1024));

    // print as fahrenheit
    unsigned x = rpi_temp_get();

    // convert <x> to C and F
    // unsigned C = 0, F = 0;
    unsigned C = (x) / 1000;
    unsigned F = (C * 9/5) + 32;
    // mailbox temp = 0x8dde, C=0 F=0
    // 30 90-100
    output("mailbox temp = %x, C=%d F=%d\n", x, C, F); 

    // todo("do overclocking!\n");
}
