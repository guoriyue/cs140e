/*
 * Implement the following routines to set GPIO pins to input or output,
 * and to read (input) and write (output) them.
 *
 * DO NOT USE loads and stores directly: only use GET32 and PUT32
 * to read and write memory.  Use the minimal number of such calls.
 *
 * See rpi.h in this directory for the definitions.
 */
#include "rpi.h"

// see broadcomm documents for magic addresses.
enum {
    GPIO_BASE = 0x20200000,
    gpio_set0  = (GPIO_BASE + 0x1C),
    gpio_clr0  = (GPIO_BASE + 0x28),
    gpio_lev0  = (GPIO_BASE + 0x34)
};

//
// Part 1 implement gpio_set_on, gpio_set_off, gpio_set_output
//

// set <pin> to be an output pin.
//
// note: fsel0, fsel1, fsel2 are contiguous in memory, so you
// can (and should) use array calculations!
void gpio_set_output(unsigned pin) {
  gpio_set_function(pin, GPIO_FUNC_OUTPUT);
  // if(pin >= 32)
  //   return;
  // if(pin >= 32 && pin != 47)
  //   return ;

  // // implement this
  // // use <gpio_fsel0>
  // if(pin <= 9){
  //   volatile unsigned gpio_fsel0 = (volatile unsigned ) GPIO_BASE;
  //   unsigned val = GET32(gpio_fsel0);
  //   int mask = 0b00000111;
  //   int shift = pin * 3;
  //   // *gpio_fsel0 = (*gpio_fsel0 & ~(mask << shift)) | (1 << shift);
  //   PUT32(gpio_fsel0, (val & ~(mask << shift)) | (1 << shift));
  // }else if (pin <= 19){
  //   volatile unsigned gpio_fsel1 = (volatile unsigned) (GPIO_BASE + 0x04);
  //   unsigned val = GET32(gpio_fsel1);
  //   int mask = 0b00000111;
  //   int shift = (pin - 10) * 3;
  //   // *gpio_fsel1 = (*gpio_fsel1 & ~(mask << shift)) | (1 << shift);
  //   PUT32(gpio_fsel1, (val & ~(mask << shift)) | (1 << shift));
  // }else if (pin <= 29){
  //   volatile unsigned gpio_fsel2 = (volatile unsigned) (GPIO_BASE + 0x08);
  //   unsigned val = GET32(gpio_fsel2);
  //   int mask = 0b00000111;
  //   int shift = (pin - 20) * 3;
  //   // *gpio_fsel2 = (*gpio_fsel2 & ~(mask << shift)) | (1 << shift);
  //   PUT32(gpio_fsel2, (val & ~(mask << shift)) | (1 << shift));
  // }else if (pin<32){
  //   volatile unsigned gpio_fsel3 = (volatile unsigned) (GPIO_BASE + 0x0c);
  //   unsigned val = GET32(gpio_fsel3);
  //   int mask = 0b00000111;
  //   int shift = (pin - 30) * 3;
  //   // *gpio_fsel2 = (*gpio_fsel2 & ~(mask << shift)) | (1 << shift);
  //   PUT32(gpio_fsel3, (val & ~(mask << shift)) | (1 << shift));
  // }
}

// set GPIO <pin> on.
void gpio_set_on(unsigned pin) {
  if(pin >= 32 && pin != 47)
    return ;
  // implement this
  // use <gpio_set0>
  // volatile unsigned *gpio_set0_ = (volatile unsigned *)gpio_set0;
  // *gpio_set0_ = (1 << pin);
  if (pin == 47) {
    volatile unsigned gpio_set1 = (volatile unsigned) (gpio_set0 + 0x04);
    PUT32(gpio_set1, (1 << (pin - 32)));
    return;
  }
  PUT32(gpio_set0, (1 << pin));
}

// set GPIO <pin> off
void gpio_set_off(unsigned pin) {
  if(pin >= 32 && pin != 47)
    return ;
  // implement this
  // use <gpio_clr0>
  // volatile unsigned *gpio_clr0_ = (volatile unsigned *)gpio_clr0;
  // unsigned val = GET32(*gpio_clr0_);
  if (pin == 47) {
    volatile unsigned gpio_clr1 = (volatile unsigned) (gpio_clr0 + 0x04);
    PUT32(gpio_clr1, (1 << (pin - 32)));
    return;
  }
  PUT32(gpio_clr0, (1 << pin));
  // *gpio_clr0_ = (1 << pin);
}

// set <pin> to <v> (v \in {0,1})
void gpio_write(unsigned pin, unsigned v) {
    if(v)
        gpio_set_on(pin);
    else
        gpio_set_off(pin);
}

//
// Part 2: implement gpio_set_input and gpio_read
//

// set <pin> to input.
void gpio_set_input(unsigned pin) {
  
  gpio_set_function(pin, GPIO_FUNC_INPUT);
  // // implement.
  // if(pin >= 32)
  //   return;

  // if(pin >= 32 && pin != 47)
  //   return;

  // if(pin <= 9){
  //   volatile unsigned gpio_fsel0 = (volatile unsigned ) GPIO_BASE;
  //   unsigned val = GET32(gpio_fsel0);
  //   int mask = 0b00000111;
  //   int shift = pin * 3;
  //   PUT32(gpio_fsel0, (val & ~(mask << shift)));
  // }else if (pin <= 19){
  //   volatile unsigned gpio_fsel1 = (volatile unsigned) (GPIO_BASE + 0x04);
  //   unsigned val = GET32(gpio_fsel1);
  //   int mask = 0b00000111;
  //   int shift = (pin - 10) * 3;
  //   // *gpio_fsel1 = (*gpio_fsel1 & ~(mask << shift));
  //   PUT32(gpio_fsel1, (val & ~(mask << shift)));
  // }else if (pin <= 29){
  //   volatile unsigned gpio_fsel2 = (volatile unsigned) (GPIO_BASE + 0x08);
  //   unsigned val = GET32(gpio_fsel2);
  //   int mask = 0b00000111;
  //   int shift = (pin - 20) * 3;
  //   // *gpio_fsel2 = (*gpio_fsel2 & ~(mask << shift));
  //   PUT32(gpio_fsel2, (val & ~(mask << shift)));
  // }else if (pin<32){
  //   volatile unsigned gpio_fsel3 = (volatile unsigned) (GPIO_BASE + 0x0c);
  //   unsigned val = GET32(gpio_fsel3);
  //   int mask = 0b00000111;
  //   int shift = (pin - 30) * 3;
  //   // *gpio_fsel2 = (*gpio_fsel2 & ~(mask << shift));
  //   PUT32(gpio_fsel3, (val & ~(mask << shift)));
  
  // }
}

// return the value of <pin>
int gpio_read(unsigned pin) {
  // if(pin >= 32)
  //   return 0;
  
  if(pin >= 32)
    return -1;

  unsigned v = 0;
  // implement.
  // volatile unsigned *gpio_lev0_ = (volatile unsigned *)gpio_lev0;
  int all_values = GET32(gpio_lev0);
  v = (all_values >> pin) & 0x1;
  // return v;
  return DEV_VAL32(v);

  // volatile unsigned *gpio_clr0_ = (volatile unsigned *)gpio_clr0;
  // *gpio_clr0_ = (1 << pin);

}

void gpio_set_function(unsigned pin, gpio_func_t function){
  // printk works now

  if (function<0 || function > 7){
    return;
  }

  if(pin >= 32 && pin != 47)
    return ;

  // implement this
  // use <gpio_fsel0>
  if(pin <= 9){
    volatile unsigned gpio_fsel0 = (volatile unsigned ) GPIO_BASE;
    unsigned val = GET32(gpio_fsel0);
    int mask = 0b00000111;
    int shift = pin * 3;
    // *gpio_fsel0 = (*gpio_fsel0 & ~(mask << shift)) | (1 << shift);
    PUT32(gpio_fsel0, (val & ~(mask << shift)) | (function << shift));
  }else if (pin <= 19){
    volatile unsigned gpio_fsel1 = (volatile unsigned) (GPIO_BASE + 0x04);
    unsigned val = GET32(gpio_fsel1);
    int mask = 0b00000111;
    int shift = (pin - 10) * 3;
    // *gpio_fsel1 = (*gpio_fsel1 & ~(mask << shift)) | (1 << shift);
    PUT32(gpio_fsel1, (val & ~(mask << shift)) | (function << shift));
  }else if (pin <= 29){
    volatile unsigned gpio_fsel2 = (volatile unsigned) (GPIO_BASE + 0x08);
    unsigned val = GET32(gpio_fsel2);
    int mask = 0b00000111;
    int shift = (pin - 20) * 3;
    // *gpio_fsel2 = (*gpio_fsel2 & ~(mask << shift)) | (1 << shift);
    PUT32(gpio_fsel2, (val & ~(mask << shift)) | (function << shift));
  }else if (pin<32){
    volatile unsigned gpio_fsel3 = (volatile unsigned) (GPIO_BASE + 0x0c);
    unsigned val = GET32(gpio_fsel3);
    int mask = 0b00000111;
    int shift = (pin - 30) * 3;
    // *gpio_fsel2 = (*gpio_fsel2 & ~(mask << shift)) | (1 << shift);
    PUT32(gpio_fsel3, (val & ~(mask << shift)) | (function << shift));
  }else if (pin == 47){
    volatile unsigned gpio_fsel4 = (volatile unsigned) (GPIO_BASE + 0x10);
    unsigned val = GET32(gpio_fsel4);
    int mask = 0b00000111;
    int shift = (pin - 40) * 3;
    // *gpio_fsel2 = (*gpio_fsel2 & ~(mask << shift)) | (1 << shift);
    PUT32(gpio_fsel4, (val & ~(mask << shift)) | (function << shift));
  }
  
  return ;
}