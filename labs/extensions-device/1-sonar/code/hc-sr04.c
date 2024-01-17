#include "rpi.h"
#include "hc-sr04.h"

// gpio_read(pin) until either:
//  1. gpio_read(pin) != v ==> return 1.
//  2. <timeout> microseconds have passed ==> return 0

// // in usec.  the lower 32-bits of the usec 
// // counter: if you investigate in the broadcom 
// // doc can see how to get the high 32-bits too.
// uint32_t timer_get_usec(void) {
//     dev_barrier();
//     uint32_t u = timer_get_usec_raw();
//     dev_barrier();
//     return u;
// }
static int read_while_eq(int pin, int v, unsigned timeout) {
    // unimplemented();
    // unsigned t1 = timer_get_usec();
    // // unsigned t2 = timer_get_usec();
    // // unsigned t3 = timer_get_usec();
    // int timeoutt=0;
    // for(int i = 0; i < 5; i++){
    //     while (gpio_read(pin) == v) {
    //         unsigned t2 = timer_get_usec();
    //         if ((t2 - t1) >= timeout)
    //             timeoutt++;
            
    //     }
    
    // }
    // if(timeoutt >= 3){
    //     return 0;
    // }
    
    // return 1;

	// wait for any previous pulse to end
    unsigned t1 = timer_get_usec();
	while (gpio_read(pin) == v){
        unsigned t2 = timer_get_usec();
        if ((t2 - t1) >= timeout)
            return 0;
    }
	// wait for the pulse to start
    while (gpio_read(pin) != v){
        unsigned t2 = timer_get_usec();
        if ((t2 - t1) >= timeout)
            return 0;
    }
	unsigned t3 = timer_get_usec();
	while (gpio_read(pin) == v){
        unsigned t2 = timer_get_usec();
        if ((t2 - t1) >= timeout)
            return 0;
    }
    unsigned t4 = timer_get_usec();
    return t4 - t3;
}

// initialize:
//  1. setup the <trigger> and <echo> GPIO pins.
// 	2. init the HC-SR04 (pay attention to time delays here)
// 
// Pay attention to the voltages on:
//    - Vcc
//    - Vout.
//
// Troubleshooting:
// 	1. there are conflicting accounts of what value voltage you
//	need for Vcc.
//	
// 	2. the initial 3 page data sheet you'll find sucks; look for
// 	a longer one. 
//
// The comments on the sparkfun product page might be helpful.
hc_sr04_t hc_sr04_init(unsigned trigger, unsigned echo) {
    hc_sr04_t h = { .trigger = trigger, .echo = echo };

    // gpio_set_input(trigger);
    // gpio_set_input(echo);

    // You only need to supply a short 10uS pulse to the trigger 
    // input to start the ranging, and then the module will send out an 8 cycle burst
    // of ultrasound at 40 kHz and raise its echo. 
    // The Echo is a distance object that is pulse width and the range in 
    // proportion .You can calculate the range through the time interval
    // between sending trigger signal and receiving echo signal. Formula: 
    // uS / 58 = centimeters or uS / 148 =inch; or: the range = high level 
    // time * velocity (340M/S) / 2; we suggest to use over 60ms measurement
    // cycle, in order to prevent trigger signal to the echo signal.

    // gpio_set_output(h.trigger);
    // // gpio_set_on(h.trigger);
    // // delay_us(10);
    // // gpio_set_off(h.trigger);
    // gpio_set_input(h.echo);
    return staff_hc_sr04_init(  trigger, echo);

    return h;
}

// get distance.
//	1. do a send (again, pay attention to any needed time 
// 	delays)
//
//	2. measure how long it takes and compute round trip
//	by converting that time to distance using the datasheet
// 	formula
//
// troubleshooting:
//  0. We don't have floating point or integer division.
//
//  1. The pulse can get lost!  Make sure you use the timeout read
//  routine you write.
// 
//	2. readings can be noisy --- you may need to require multiple
//	high (or low) readings before you decide to trust the 
// 	signal.
//
int hc_sr04_get_distance(hc_sr04_t h, unsigned timeout_usec) {
    // return staff_hc_sr04_get_distance(h,timeout_usec);

    gpio_set_off(h.trigger);
    delay_ms(2);
    gpio_set_on(h.trigger);
    delay_ms(10);
    gpio_set_off(h.trigger);


    

    // 1/40k*8 = 1/5000 = 200us
    // unsigned total_time = timeout_usec;

    unsigned t1 = timer_get_usec();
    unsigned result = read_while_eq(h.echo, 1, timeout_usec);
    if(result == 0){
        return -1;
    }else{
        unsigned t2 = timer_get_usec();
        return result/148;
    }

    

    // -1 = error.
    // return -1;
}
