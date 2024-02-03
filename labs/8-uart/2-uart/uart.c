// implement:
//  void uart_init(void)
//
//  int uart_can_get8(void);
//  int uart_get8(void);
//
//  int uart_can_put8(void);
//  void uart_put8(uint8_t c);
//
//  int uart_tx_is_empty(void) {
//
// see that hello world works.
//
//
#include "rpi.h"

// called first to setup uart to 8n1 115200  baud,
// no interrupts.
//  - you will need memory barriers, use <dev_barrier()>
//
//  later: should add an init that takes a baud rate.

volatile unsigned int* AUX_ENABLES = (volatile unsigned int*) 0x20215004;
volatile unsigned int* AUX_MU_CNTL_REG = (volatile unsigned int*) 0x20215060;
volatile unsigned int* AUX_MU_LCR_REG = (volatile unsigned int*) 0x2021504c;
volatile unsigned int* AUX_MU_MCR_REG = (volatile unsigned int*) 0x20215050;

volatile unsigned int* AUX_MU_IER_REG = (volatile unsigned int*) 0x20215044;
volatile unsigned int* AUX_MU_IIR_REG = (volatile unsigned int*) 0x20215048;
volatile unsigned int* AUX_MU_BAUD_REG = (volatile unsigned int*) 0x20215068;
volatile unsigned int* AUX_MU_IO_REG = (volatile unsigned int*) 0x20215040;
volatile unsigned int* AUX_MU_LSR_REG = (volatile unsigned int*) 0x20215054;

void uart_init(void) {
    // You need to turn on the UART in AUX. Make sure you read-modify-write --- don't kill the SPIm enables.
    // Immediately disable tx/rx (you don't want to send garbage).
    // Figure out which registers you can ignore (e.g., IO, p 11). Many devices have many registers you can skip.
    // Find and clear all parts of its state (e.g., FIFO queues) since we are not absolutely positive they do not hold garbage.
    // Disable interrupts.
    // Configure: 115200 Baud, 8 bits, 1 start bit, 1 stop bit. No flow control.
    // Enable tx/rx. It should be working!
    // uart_disable();
    uint8_t enables_mask = 0b1;
    uint8_t enables_state = get32(AUX_ENABLES);
    enables_state = enables_state | enables_mask;
    put32(AUX_ENABLES, enables_state);
    // disable tx/rx. No flow control.
    put32(AUX_MU_CNTL_REG, 0b00000000);
    // Find and clear all parts of its state
    put32(AUX_MU_IIR_REG, 0b00000110);
    // Disable interrupts.
    put32(AUX_MU_IER_REG, 0b00000000);
    // Configure: 115200 Baud, 8 bits, 1 start bit, 1 stop bit.
    put32(AUX_MU_BAUD_REG, 270);
    put32(AUX_MU_LCR_REG, 0b00000011);
    dev_barrier();
    gpio_set_function(14, GPIO_FUNC_ALT5);
    gpio_set_function(15, GPIO_FUNC_ALT5);
    // enable tx/rx.
    put32(AUX_MU_CNTL_REG, 0b00000011);
}

// disable the uart.
void uart_disable(void) {
    uart_flush_tx();
    uint8_t enables_mask = 0b0;
    uint8_t enables_state = get32(AUX_ENABLES);
    enables_state = enables_state & enables_mask;
    put32(AUX_ENABLES, enables_state);
}


// returns one byte from the rx queue, if needed
// blocks until there is one.
int uart_get8(void) {
	// return 0;
    while(1) {
        if(get32(AUX_MU_LSR_REG) & 0b1) {
            return get32(AUX_MU_IO_REG) & 0xff;
        }
    }
}

// 1 = space to put at least one byte, 0 otherwise.
int uart_can_put8(void) {
    // return 0;
    if(get32(AUX_MU_LSR_REG) & 0b100000) {
        return 1;
    }
    return 0;
}

// put one byte on the tx qqueue, if needed, blocks
// until TX has space.
// returns < 0 on error.
int uart_put8(uint8_t c) {
    // return 0;
    while(1) {
        if(get32(AUX_MU_LSR_REG) & 0b100000) {
            put32(AUX_MU_IO_REG, c);
            return 0;
        }
    }
}

// simple wrapper routines useful later.

// 1 = at least one byte on rx queue, 0 otherwise
int uart_has_data(void) {
    // todo("must implement\n");
    if(get32(AUX_MU_LSR_REG) & 0b1) {
        return 1;
    }
    return 0;
}

// return -1 if no data, otherwise the byte.
int uart_get8_async(void) { 
    if(!uart_has_data())
        return -1;
    return uart_get8();
}

// 1 = tx queue empty, 0 = not empty.
int uart_tx_is_empty(void) {
    // unimplemented();
    if(get32(AUX_MU_LSR_REG) & 0b1000000) {
        return 1;
    }
    return 0;
}

// flush out all bytes in the uart --- we use this when 
// turning it off / on, etc.
void uart_flush_tx(void) {
    while(!uart_tx_is_empty())
        ;
}
