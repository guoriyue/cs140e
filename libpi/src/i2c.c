/*
 * simplified i2c implementation --- no dma, no interrupts.  the latter
 * probably should get added.  the pi's we use can only access i2c1
 * so we hardwire everything in.
 *
 * datasheet starts at p28 in the broadcom pdf.
 *
 */
#include "rpi.h"
#include "libc/helper-macros.h"
#include "i2c.h"

typedef struct {
	uint32_t control; // "C" register, p29
	uint32_t status;  // "S" register, p31

#	define check_dlen(x) assert(((x) >> 16) == 0)
	uint32_t dlen; 	// p32. number of bytes to xmit, recv
					// reading from dlen when TA=1
					// or DONE=1 returns bytes still
					// to recv/xmit.  
					// reading when TA=0 and DONE=0
					// returns the last DLEN written.
					// can be left over multiple pkts.

    // Today address's should be 7 bits.
#	define check_dev_addr(x) assert(((x) >> 7) == 0)
	uint32_t 	dev_addr;   // "A" register, p 33, device addr 

	uint32_t fifo;  // p33: only use the lower 8 bits.
#	define check_clock_div(x) assert(((x) >> 16) == 0)
	uint32_t clock_div;     // p34
	// we aren't going to use this: fun to mess w/ tho.
	uint32_t clock_delay;   // p34
	uint32_t clock_stretch_timeout;     // broken on pi.
} RPI_i2c_t;

// offsets from table "i2c address map" p 28
_Static_assert(offsetof(RPI_i2c_t, control) == 0, "wrong offset");
_Static_assert(offsetof(RPI_i2c_t, status) == 0x4, "wrong offset");
_Static_assert(offsetof(RPI_i2c_t, dlen) == 0x8, "wrong offset");
_Static_assert(offsetof(RPI_i2c_t, dev_addr) == 0xc, "wrong offset");
_Static_assert(offsetof(RPI_i2c_t, fifo) == 0x10, "wrong offset");
_Static_assert(offsetof(RPI_i2c_t, clock_div) == 0x14, "wrong offset");
_Static_assert(offsetof(RPI_i2c_t, clock_delay) == 0x18, "wrong offset");

/*
 * There are three BSC masters inside BCM. The register addresses starts from
 *	 BSC0: 0x7E20_5000 (0x20205000)
 *	 BSC1: 0x7E80_4000
 *	 BSC2 : 0x7E80_5000 (0x20805000)
 * the PI can only use BSC1.
 */
static volatile RPI_i2c_t *i2c = (void*)0x20804000; 	// BSC1

// extend so this can fail.
int i2c_write(unsigned addr, uint8_t data[], unsigned nbytes) {
    // todo("implement");
	// return 1;
	// similar to i2c_read
	// unsigned status = ;
	while (i2c->status & 0x1) {
		// delay_ms(1);
	}

	while (!(i2c->status & (1 << 6))) {
		// delay_ms(1);
	}

	// unsigned status = i2c->status;
	// printk("status: %x\n", status);
	// printk("i2c->status & (1 << 6) is %x\n", status & (1 << 6));
	// printk("i2c->status & (1 << 8) is %x\n", status & (1 << 8));
	// printk("i2c->status & (1 << 9) is %x\n", status & (1 << 9));
	if ((i2c->status & (1 << 6) && ((i2c->status & (1 << 8)) == 0) && ((i2c->status & (1 << 9)) == 0))) {
		
		// PUT32(i2c->status, 1 << 1);
		// PUT32(i2c->dev_addr, addr);
		i2c->status |= (1 << 1);
		i2c->dev_addr = addr;
		i2c->dlen = nbytes;
		// unsigned control = GET32(i2c->control);
		// control = control | (1 << 7);
		// control = control & ~(1);
		// PUT32(i2c->control, control);
		// i2c->control &= ~(1);
		i2c->control = (i2c->control | (1 << 7)) & (~(1));
		

		// while(!(i2c->status & 0x1)) {
		// 	delay_ms(1);
		// }
		while(!(((!(i2c->status & 0x1)) & (i2c->status & 0b10)) ^ ((i2c->status & 0x1) & (!(i2c->status & 0b10))))) {
			// delay_ms(1);
		}

		// 2. Read the bytes: you'll have to check that there is a byte available
		// 	each time.

		while (nbytes > 0) {
			// check 4 to see if the fifo is full when writing
			while (!(i2c->status & (1 << 4))) {
				// delay_ms(1);
			}
			i2c->fifo = *data;
			data++;
			nbytes--;
		}

		// 3. Do the end of a transfer: use status to wait for `DONE` (p32).  Then
		// 	check that TA is 0, and there were no errors.

		while (!(i2c->status & (1 << 1))) {
			// delay_us(1);
		}

		// status = i2c->status;
		if ((i2c->status & (1 << 1)) && ((i2c->status & (1)) == 0) && ((i2c->status & (1 << 9)) == 0) && ((i2c->status & (1 << 8)) == 0)) {
			return 1;
		} else {
			// return 0;
			panic("status is not correct\n");
		}
	} else {
		panic("status is not correct\n");
	}
}

// extend so it returns failure.
int i2c_read(unsigned addr, uint8_t data[], unsigned nbytes) {
    // todo("implement");
	// return 1;
	// Before starting: wait until transfer is not active.

	// Then: check in status that: fifo is empty 6, there was no clock
	// stretch timeout 8 and there were no errors 9.  We shouldn't see any of
	// these today.

	// Clear the DONE field in status since it appears it can still be
	// set from a previous invocation.

	// Set the device address and length.

	// Set the control reg to read and start transfer.

	// Wait until the transfer has started.

	// while (i2c->status & 0x1) {
	// 	delay_ms(1);
	// }

	while (i2c->status & 0x1) {
		// delay_ms(1);
	}

	while (!(i2c->status & (1 << 6))) {
		// delay_ms(1);
	}
	
	if ((i2c->status & (1 << 6) && ((i2c->status & (1 << 8)) == 0) && ((i2c->status & (1 << 9)) == 0))) {
		// printk("i2c->status is %x\n", i2c->status);
		i2c->status |= (1 << 1);
		// printk("i2c->status is %x\n", i2c->status);
		i2c->dev_addr = addr;
		i2c->dlen = nbytes;
		i2c->control = i2c->control | (1 << 7) | (1);
		
		// printk("i2c->status is %x\n", i2c->status);
		// Wait until the transfer has started
		while(!(((!(i2c->status & 0x1)) & (i2c->status & 0b10)) ^ ((i2c->status & 0x1) & (!(i2c->status & 0b10))))) {
			// delay_ms(1);
		}

		// 2. Read the bytes: you'll have to check that there is a byte available
		// 	each time.
		// printk("i2c->control is %x\n", i2c->control);
		// printk("n bytes is %d\n", nbytes);
		while (nbytes > 0) {
			// printk("i2c->control is %x\n", i2c->control);
			while (!(i2c->status & (1 << 5))) {
				// delay_ms(1);
			}
			*data = i2c->fifo;
			data++;
			nbytes--;
		}

		// 3. Do the end of a transfer: use status to wait for `DONE` (p32).  Then
		// 	check that TA is 0, and there were no errors.

		while (!(i2c->status & (1 << 1))) {
			// delay_ms(1);
		}

		if ((i2c->status & (1 << 1)) && ((i2c->status & (1)) == 0) && ((i2c->status & (1 << 9)) == 0) && ((i2c->status & (1 << 8)) == 0)) {
			return 1;
		} else {
			// return 0;
			panic("status is not correct\n");
		}
	} else {
		// return 0;
		panic("status is not correct\n");
	}
}

void i2c_init(void) {
    // todo("setup GPIO, setup i2c, sanity check results");
	// serup GPIO
	unsigned alt0 = 4;
	// sda1 gpio2, scl1 gpio3
	gpio_set_function(2, alt0);
	gpio_set_function(3, alt0);

	// PUT32(i2c->control, 0x0 | (1 << 7) | (1 << 15) | (0b11 << 4));
	// PUT32(i2c->clock_div, 0x5dc); // 100KHz
	// i2c->control = 0x0 | (1 << 7) | (1 << 15) | (0b11 << 4); 7 init transfer
	i2c->control = 0x0 | (1 << 15) | (0b11 << 4);
	i2c->clock_div = 0x5dc; // 100KHz
	i2c->status = 0;
}

// shortest will be 130 for i2c accel.
void i2c_init_clk_div(unsigned clk_div) {
    // todo("same as init but set the clock divider");
	i2c->clock_div = 0x5dc; // 100KHz
}
