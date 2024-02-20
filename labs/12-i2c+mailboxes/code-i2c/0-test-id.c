// dumb initial test that you can read the device address.
#include "rpi.h"
#include "i2c.h"


void i2c_init(void);

// shortest will be 130 for i2c accel.
void i2c_init_clk_div(unsigned clk_div);

// can call N times, will only initialize once (the first time)
void i2c_init_once(void);

// write <nbytes> of <datea> to i2c device address <addr>
int i2c_write(unsigned addr, uint8_t data[], unsigned nbytes);
// read <nbytes> of <datea> from i2c device address <addr>
int i2c_read(unsigned addr, uint8_t data[], unsigned nbytes);

// read a single device register <reg> from i2c device 
// <addr> and return the result.
static uint8_t i2c_get_reg(uint8_t addr, uint8_t reg) {
    i2c_write_my(addr, &reg, 1);

    uint8_t v;
    i2c_read_my(addr,  &v, 1);
    return v;
}

void notmain(void) {
    delay_ms(100);   // allow time for i2c/device to boot up.
    i2c_init_my();
    delay_ms(100);   // allow time for i2c/dev to settle after init.

    // from application note.
    uint8_t dev_addr = 0b1101000;

    enum {
        WHO_AM_I_REG      = 0x75,
        WHO_AM_I_VAL = 0x68,
    };

    uint8_t v = i2c_get_reg(dev_addr, WHO_AM_I_REG);
    if(v != WHO_AM_I_VAL)
        panic("Initial probe failed: expected %b (%x), have %b (%x)\n",
            WHO_AM_I_VAL, WHO_AM_I_VAL, v, v);
    printk("SUCCESS: mpu-6050 acknowledged our ping: WHO_AM_I=%b!!\n", v);
}
