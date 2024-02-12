// very dumb, simple interface to wrap up watchpoints better.
// only handles a single watchpoint.
//
// You should be able to take most of the code from the 
// <1-watchpt-test.c> test case where you already did 
// all the needed operations.  This interface just packages 
// them up a bit.
//
// possible extensions:
//   - do as many as hardware supports, perhaps a handler for 
//     each one.
//   - make fast.
//   - support multiple independent watchpoints so you'd return
//     some kind of structure and pass it in as a parameter to
//     the routines.
#include "rpi.h"
#include <stdio.h>
#include "mini-watch.h"

// we have a single handler: so just use globals.
static watch_handler_t watchpt_handler = 0;
static void *watchpt_data = 0;

// is it a load fault?
static int mini_watch_load_fault(void) {
    // todo("implement");
    return datafault_from_ld();
}

// if we have a dataabort fault, call the watchpoint
// handler.
static void watchpt_fault(regs_t *r) {
    // watchpt handler.
    if(was_brkpt_fault())
        panic("should only get debug faults!\n");
    if(!was_watchpt_fault())
        panic("should only get watchpoint faults!\n");
    if(!watchpt_handler)
        panic("watchpoint fault without a fault handler\n");

    watch_fault_t w = {0};

    
    w.fault_pc = watchpt_fault_pc();
    w.is_load_p = datafault_from_ld();
    w.regs = r;
    // uint32_t addr = cp14_wvr0_get();
    
    // uint32_t wfar = cp14_wfar_get();
    unsigned wcr = cp14_wcr0_get();
    unsigned val = (wcr >> 5) & 0xF;
    unsigned shift = 0;
    while ((val & 0x1) != 1) {
        shift += 1;
        val = val >> 1;
    }

    unsigned wvr = cp14_wvr0_get();
    unsigned addr = wvr + shift;
    printk("shift=%d\n", shift);
    w.fault_addr = (void *)(addr);
    // printk("wfar=%p\n", wfar);
    // printk("addr=%p\n", addr);
    // printk("r[0]=%p\n", r->regs[0]);

    // uint32_t addr = cp14_wfar_get();
    // w.fault_addr = (void *)addr;
    // printk("r[0]=%p\n", r->regs[0]);
    printk("watchpt fault at addr %p, pc=%p\n", w.fault_addr, w.fault_pc);

    watchpt_handler(watchpt_data, &w);

    
    
    // in case they change the regs.
    switchto(w.regs);
}

// setup:
//   - exception handlers, 
//   - cp14, 
//   - setup the watchpoint handler
// (see: <1-watchpt-test.c>
void mini_watch_init(watch_handler_t h, void *data) {
    // todo("setup cp14 and the full exception routines");
    full_except_install(0);
    full_except_set_data_abort(watchpt_fault);
    cp14_enable();

    // just started, should not be enabled.
    assert(!cp14_bcr0_is_enabled());
    assert(!cp14_bcr0_is_enabled());

    watchpt_handler = h;
    watchpt_data = data;

}

// set a watch-point on <addr>.
void mini_watch_addr(void *addr) {
    // todo("watch <addr>");
    assert(!cp14_wcr0_is_enabled());
    // uint32_t b = 0b00000111111111; // 1 from 0-8

    uint32_t b = cp14_wcr0_get();
    uint32_t mask = 0b1 << 20 | 0b1111 << 5 | 0b11 << 14 | 0b11 << 21 | 0b111;
    unsigned shift = ((unsigned)addr % 4) + 5;
    uint32_t new_v = (1 << shift) | (0b111);
    b = (b & ~mask) | new_v;
    
    cp14_wcr0_set(b);
    cp14_wvr0_set((uint32_t)(addr));

    // trace("set watchpoint for addr %d\n", (uint32_t)addr);
    assert(cp14_wcr0_is_enabled());
}

// disable current watchpoint <addr>
void mini_watch_disable(void *addr) {
    // todo("implement");
    printk("Disabling watchpoint for addr %p\n", addr);
    cp14_wvr0_set((uint32_t)(addr));
    cp14_wcr0_disable();
}

// return 1 if enabled.
int mini_watch_enabled(void) {
    // todo("implement");
    return cp14_wcr0_is_enabled();
}

// called from exception handler: if the current 
// fault is a watchpoint, return 1
int mini_watch_is_fault(void) { 
    // todo("implement");
    return was_watchpt_fault();
}
