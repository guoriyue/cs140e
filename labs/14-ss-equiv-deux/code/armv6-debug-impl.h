#ifndef __ARMV6_DEBUG_IMPL_H__
#define __ARMV6_DEBUG_IMPL_H__

/*************************************************************************
 * all the different assembly routines.
 */
#include "asm-helpers.h"

#if 0
// all we need for IMB at the moment: prefetch flush.
static inline void prefetch_flush(void) {
    unsigned r = 0;
    asm volatile ("mcr p15, 0, %0, c7, c5, 4" :: "r" (r));
}
#endif

// turn <x> into a string
#define MK_STR(x) #x

// define a general co-processor inline assembly routine to set the value.
// from manual: must prefetch-flush after each set.
#define coproc_mk_set(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)       \
    static inline void c ## coproc ## _ ## fn_name ## _set(uint32_t v) {                    \
        asm volatile ("mcr " MK_STR(coproc) ", "                        \
                             MK_STR(opcode_1) ", "                      \
                             "%0, "                                     \
                            MK_STR(Crn) ", "                            \
                            MK_STR(Crm) ", "                            \
                            MK_STR(opcode_2) :: "r" (v));               \
        prefetch_flush();                                               \
    }

#define coproc_mk_get(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)       \
    static inline uint32_t c ## coproc ## _ ## fn_name ## _get(void) {                      \
        uint32_t ret=0;                                                   \
        asm volatile ("mrc " MK_STR(coproc) ", "                        \
                             MK_STR(opcode_1) ", "                      \
                             "%0, "                                     \
                            MK_STR(Crn) ", "                            \
                            MK_STR(Crm) ", "                            \
                            MK_STR(opcode_2) : "=r" (ret));             \
        return ret;                                                     \
    }


// make both get and set methods.
#define coproc_mk(fn, coproc, opcode_1, Crn, Crm, opcode_2)     \
    coproc_mk_set(fn, coproc, opcode_1, Crn, Crm, opcode_2)        \
    coproc_mk_get(fn, coproc, opcode_1, Crn, Crm, opcode_2) 

// produces p14_brv_get and p14_brv_set
// coproc_mk(brv, p14, 0, c0, crm, op2)

/*******************************************************************************
 * debug support.
 */
#include "libc/helper-macros.h"     // to check the debug layout.
#include "libc/bit-support.h"           // bit_* and bits_* routines.


// 13-5
struct debug_id {
                                // lower bit pos : upper bit pos [inclusive]
                                // see 0-example-debug.c for how to use macros
                                // to check bitposition and size.  very very easy
                                // to mess up: you should always do.
    uint32_t    revision:4,     // 0:3  revision number
                variant:4,      // 4:7  major revision number
                :4,             // 8:11
                debug_rev:4,   // 12:15
                debug_ver:4,    // 16:19
                context:4,      // 20:23
                brp:4,          // 24:27 --- number of breakpoint register
                                //           pairs+1
                wrp:4          // 28:31 --- number of watchpoint pairs.
        ;
};


// Get the debug id register
static inline uint32_t cp14_debug_id_get(void) {
    // the documents seem to imply the general purpose register 
    // SBZ ("should be zero") so we clear it first.
    uint32_t ret = 0;

    asm volatile ("mrc p14, 0, %0, c0, c0, 0" : "=r"(ret));
    return ret;
}

// This macro invocation creates a routine called cp14_debug_id_macro
// that is equivalant to <cp14_debug_id_get>
//
// you can see this by adding "-E" to the gcc compile line and inspecting
// the output.
coproc_mk_get(debug_id_macro, p14, 0, c0, c0, 0)

// enable the debug coproc
static inline void cp14_enable(void);

// get the cp14 status register.
static inline uint32_t cp14_status_get(void);
// set the cp14 status register.
static inline void cp14_status_set(uint32_t status);

#if 0
static inline uint32_t cp15_dfsr_get(void);
static inline uint32_t cp15_ifar_get(void);
static inline uint32_t cp15_ifsr_get(void);
static inline uint32_t cp14_dscr_get(void);
#endif

//**********************************************************************
// all your code should go here.  implementation of the debug interface.

// example of how to define get and set for status registers
coproc_mk(status, p14, 0, c0, c1, 0)

// you'll need to define these and a bunch of other routines.
// static inline uint32_t cp15_dfsr_get(void) { todo("implement"); }
// static inline uint32_t cp15_ifar_get(void) { todo("implement"); }
// static inline uint32_t cp15_ifsr_get(void) { todo("implement"); }
// static inline uint32_t cp14_dscr_get(void) { todo("implement"); }
// static inline uint32_t cp14_wcr0_set(uint32_t r) { todo("implement"); }

// static inline void cp14_wvr0_set(uint32_t r) { todo("implement"); }
// static inline void cp14_bcr0_set(uint32_t r) { todo("implement"); }
// static inline void cp14_bvr0_set(uint32_t r) { todo("implement"); }

// fn_name, coproc, opcode_1, Crn, Crm, opcode_2
coproc_mk(bcr0, p14, 0, c0, c0, 0b101)
coproc_mk(bvr0, p14, 0, c0, c0, 0b100)
coproc_mk(dscr, p14, 0, c0, c1, 0)
coproc_mk(wcr0, p14, 0, c0, c0, 7)
coproc_mk(wvr0, p14, 0, c0, c0, 6)
coproc_mk(wfar, p14, 0, c0, c6, 0)

coproc_mk(dfsr, p15, 0, c5, c0, 0)
coproc_mk(ifsr, p15, 0, c5, c0, 1)
coproc_mk(ifar, p15, 0, c6, c0, 2)
// return 1 if enabled, 0 otherwise.  
//    - we wind up reading the status register a bunch:
//      could return its value instead of 1 (since is 
//      non-zero).
static inline int cp14_is_enabled(void) {
    // unimplemented();
    return ((cp14_dscr_get() >> 14) & 0b11) == 0b10;
}

// enable debug coprocessor 
static inline void cp14_enable(void) {
    // if it's already enabled, just return?
    if(cp14_is_enabled())
        return;
        // panic("already enabled\n");

    // for the core to take a debug exception, monitor debug mode has to be both 
    // selected and enabled --- bit 14 clear and bit 15 set.
    // unimplemented();
    // coproc_mk_set(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)
    int r = cp14_dscr_get();
    r &= ~(0b11 << 14);
    r |= 0b10 << 14;
    cp14_dscr_set(r);


    assert(cp14_is_enabled());
}

// disable debug coprocessor
static inline void cp14_disable(void) {
    if(!cp14_is_enabled())
        return;

    // unimplemented();
    // cp14_dscr_set(0);
    int r = cp14_dscr_get();
    r &= ~(0b11 << 14);
    cp14_dscr_set(r);

    assert(!cp14_is_enabled());
}


static inline int cp14_bcr0_is_enabled(void) {
    // unimplemented();
    return cp14_bcr0_get() & 1;
}
static inline void cp14_bcr0_enable(void) {
    // unimplemented();
    uint32_t r = cp14_bcr0_get();
    cp14_bcr0_set(r | 1);
}
static inline void cp14_bcr0_disable(void) {
    // unimplemented();
    uint32_t r = cp14_bcr0_get();
    cp14_bcr0_set(r & ~1);
}

// was this a brkpt fault?
static inline int was_brkpt_fault(void) {
    // use IFSR and then DSCR
    // unimplemented();
    int fault = cp15_ifsr_get();
    if ((fault & 0b1111) == 0b0010) {
        if (((cp14_dscr_get() >> 2) & 0b1111) == 0b0001) {
            // a breakpoint occurred
            return 1;
        }
    }
    return 0;
}

// was watchpoint debug fault caused by a load?
static inline int datafault_from_ld(void) {
    return bit_isset(cp15_dfsr_get(), 11) == 0;
}
// ...  by a store?
static inline int datafault_from_st(void) {
    return !datafault_from_ld();
}


// 13-33: tabl 13-23
static inline int was_watchpt_fault(void) {
    // use DFSR then DSCR
    // unimplemented();
    if ((cp15_dfsr_get() & 0b1111) == 0b0010) {
        if (((cp14_dscr_get() >> 2) & 0b1111) == 0b0010) {
            // a watchpoint occurred
            return 1;
        }
    }
    return 0;
}

static inline int cp14_wcr0_is_enabled(void) {
    // unimplemented();
    return cp14_wcr0_get() & 1;
}
static inline void cp14_wcr0_enable(void) {
    // unimplemented();
    uint32_t r = cp14_wcr0_get();
    cp14_wcr0_set(r | 1);
}
static inline void cp14_wcr0_disable(void) {
    // unimplemented();
    uint32_t r = cp14_wcr0_get();
    cp14_wcr0_set(r & ~1);
}

// Get watchpoint fault using WFAR
static inline uint32_t watchpt_fault_pc(void) {
    // unimplemented();
    // why?
    return cp14_wfar_get() - 8;
}

#endif
