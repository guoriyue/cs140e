#include "rpi.h"
#include "asm-helpers.h"
#include "cpsr-util.h"
#include "vector-base.h"
void switchto_anymode_asm(uint32_t regs[17]);



int do_syscall(uint32_t regs[17]) {
    int sysno = regs[0];
    trace("in syscall: sysno=%d\n", sysno);

    for(unsigned i = 0; i < 17; i++) {
        if(regs[i])
            trace("reg[%d]=%x\n", i, regs[i]);
    }

    assert(sysno == 0);
    clean_reboot();
}



void swi_fn(void);
void nop_10(void);
void mov_ident(void);

void notmain(void) {
    extern uint32_t swi_test_handlers[];
    vector_base_set(swi_test_handlers);
    // output("about to check that swi test works\n");

    uint32_t regs[17];
    for(unsigned i = 0; i < 15; i++)
        regs[i] = i;
    regs[15] = (uint32_t)swi_fn;
    regs[16] = SUPER_MODE;
    trace("about to jump to pc=[%x] with cpsr=%x\n",
            regs[15], regs[16]);
    switchto_anymode_asm(regs);
    // trace("from USER_MODE to SYS_MODE\n");
    // regs[16] = SYS_MODE;
    // switchto_anymode_asm(regs);
    // regs[16] = SUPER_MODE;
    // trace("from SYS_MODE to SUPER_MODE\n");
    // switchto_anymode_asm(regs);
    // regs[16] = USER_MODE;
    // trace("from SUPER_MODE to USER_MODE\n");
    // switchto_anymode_asm(regs);
}
