#include "rpi.h"
#include "sw-uart.h"
#include "cycle-count.h"
#include "cycle-util.h"
#include "wait-routines.h"
#include <stdarg.h>

// do this first: used timed_write to cleanup.
//  recall: time to write each bit (0 or 1) is in <uart->usec_per_bit>
void sw_uart_put8(sw_uart_t *uart, uint8_t c) {
    // use local variables to minimize any loads or stores
    int tx = uart->tx;
    uint32_t n = uart->cycle_per_bit,
             u = n,
             s = cycle_cnt_read();

    // use wait_ncycles_exact
    // todo("implement this code\n");
   
    // sw_uart_get8_timeout(uart, 1000);
    gpio_write(tx, 0);
    wait_ncycles_exact(s, u);
    u += n;
    for(int i = 0; i < 8; i++) {
        gpio_write(tx, c & 1);
        c >>= 1;
        wait_ncycles_exact(s, u);
        u += n;
    }
    gpio_write(tx, 1);
    wait_ncycles_exact(s, u);
    u += n;
}

// do this second: you can type in pi-cat to send stuff.
//      EASY BUG: if you are reading input, but you do not get here in 
//      time it will disappear.
int sw_uart_get8_timeout(sw_uart_t *uart, uint32_t timeout_usec) {
    unsigned rx = uart->rx;

    // right away (used to be return never).
    while(!wait_until_usec(rx, 0, timeout_usec))
        return -1;

    // return -1;
    // todo("implement this code\n");
    // return gpio_read(uart->rx);
    // int read = get32((volatile unsigned*) uart->rx);
    // return read;
    // return 99999;

    uint32_t n = uart->cycle_per_bit,
             u = n,
             s = cycle_cnt_read();
    int result = 0;
    for(int i = 0; i < 8; i++) {
        int read = gpio_read(rx);
        result |= (read << i);
        wait_ncycles_exact(s, u);
        u += n;
    }
    return result;
}

// finish implementing this routine.  
sw_uart_t sw_uart_init_helper(unsigned tx, unsigned rx,
        unsigned baud, 
        unsigned cyc_per_bit,
        unsigned usec_per_bit) 
{
    assert(tx && tx<31);
    assert(rx && rx<31);
    assert(cyc_per_bit && cyc_per_bit > usec_per_bit);
    assert(usec_per_bit);

    // make sure it makes sense.
    unsigned mhz = 700 * 1000 * 1000;
    unsigned derived = cyc_per_bit * baud;
    if(! ((mhz - baud) <= derived && derived <= (mhz + baud)) )
        panic("too much diff: cyc_per_bit = %d * baud = %d\n", 
            cyc_per_bit, cyc_per_bit * baud);

    // todo("setup rx,tx and initial state of tx pin.");
    // 20xx
    // unsigned int mask;
    // volatile unsigned int* AUX_ENABLES = (volatile unsigned int*)0x20215004;
    // mask = 0b001;
    // *AUX_ENABLES = *AUX_ENABLES | mask;

    // volatile unsigned int* AUX_MU_IER_REG = (volatile unsigned int*)0x20215044;
    // mask = 0b11;
    // *AUX_MU_IER_REG = *AUX_MU_IER_REG & ~mask;

    // volatile unsigned int* AUX_MU_CNTL_REG = (volatile unsigned int*)0x20215060;
    // mask = 0b11;
    // *AUX_MU_CNTL_REG = *AUX_MU_CNTL_REG | mask;


    // volatile unsigned int* AUX_MU_LCR_REG = (volatile unsigned int*)0x2021504C;
    // mask = 0b1;
    // *AUX_MU_LCR_REG = *AUX_MU_LCR_REG | mask;

    // volatile unsigned int* AUX_MU_IO_REG = (volatile unsigned int*)0x20215040;
    
    asm volatile("" ::: "memory");

    // tx input 
    // rx output 
    // gpio_set_function(tx, GPIO_FUNC_ALT5);
    // gpio_set_function(rx, GPIO_FUNC_ALT5);
    gpio_set_output(tx);
    gpio_set_input(rx);
    gpio_write(tx, 1);
    // gpio_write(rx, 1);

    return (sw_uart_t) { 
            .tx = tx, 
            .rx = rx, 
            .baud = baud, 
            .cycle_per_bit = cyc_per_bit ,
            .usec_per_bit = usec_per_bit 
    };
}
