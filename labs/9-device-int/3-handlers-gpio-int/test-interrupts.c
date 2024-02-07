// test code for checking the interrupts.
#include "test-interrupts.h"

volatile int n_interrupt;

static interrupt_fn_t interrupt_fn;

void interrupt_vector(unsigned pc) {
    dev_barrier();
    n_interrupt++;

    if(!interrupt_fn(pc))
        panic("should have no other interrupts?\n");

    dev_barrier();
}

#include "vector-base.h"

// initialize all the interrupt stuff.  client passes in the 
// gpio int routine <fn>
//
// make sure you understand how this works.
void test_startup(init_fn_t init_fn, interrupt_fn_t int_fn) {
    output("\tImportant: must loop back (attach a jumper to) pins 20 & 21\n");
    output("\tImportant: must loop back (attach a jumper to) pins 20 & 21\n");
    output("\tImportant: must loop back (attach a jumper to) pins 20 & 21\n");

    // initialize.
    extern uint32_t interrupt_vec[];
    int_vec_init(interrupt_vec);

    gpio_set_output(out_pin);
    gpio_set_input(in_pin);

    init_fn();
    interrupt_fn = int_fn;

    // in case there was an event queued up.
    gpio_event_clear(in_pin);

    // start global interrupts.
    cpsr_int_enable();
}


/********************************************************************
 * falling edge.
 */

volatile int n_falling;

// check if there is an event, check if it was a falling edge.
int falling_handler(uint32_t pc) {
    // todo("implement this: return 0 if no rising int\n");
    if (gpio_event_detected(in_pin)) {
        if (gpio_read(in_pin) == 0) {
            n_falling++;
            gpio_event_clear(in_pin);
            return 1;
        }
    }
    return 0;
}

void falling_init(void) {
    gpio_write(out_pin, 1);
    gpio_int_falling_edge(in_pin);
}

/********************************************************************
 * rising edge.
 */

volatile int n_rising;

// check if there is an event, check if it was a rising edge.
int rising_handler(uint32_t pc) {
    // todo("implement this: return 0 if no rising int\n");
    if (gpio_event_detected(in_pin)) {
        if (gpio_read(in_pin)) {
            n_rising++;
            gpio_event_clear(in_pin);
            return 1;
        }
    }
    return 0;
}

void rising_init(void) {
    gpio_write(out_pin, 0);
    gpio_int_rising_edge(in_pin);
}

/********************************************************************
 * rising edge.
 */

#include "timer-interrupt.h"

void timer_test_init(void) {
    // turn on timer interrupts.
    timer_interrupt_init(0x4);
}

int timer_test_handler(uint32_t pc) {
    dev_barrier();
    // todo("implement this by stealing pieces from 5-interrupts/0-timer-int");
    // register names
    // check intrp on time and claen
    if (GET32(arm_timer_MaskedIRQ)) {
        n_interrupt++;
        PUT32(arm_timer_IRQClear, 1);
        return 1;
    }
    dev_barrier();
    return 0;
}