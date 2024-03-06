#include "rpi-asm.h"

@ all of these are called from <switchto.h>: look there for more
@ help in understanding if need to.

@ drop in your code from last lab to get sp and lr.
@
@ void priv_get_sp_lr_asm(uint32_t mode, uint32_t *sp, uint32_t *lr);
@   - assumes caller is at privileged mode [we could check this]
@   - mode in r0
@   - sp pointer in r1
@   - lr pointer in r2
@
@ MK_FN(priv_get_sp_lr_asm)
@     b staff_priv_get_sp_lr_asm
@     asm_not_reached();

MK_FN(priv_get_sp_lr_asm)
    @ asm_todo("implement this");
    mrs r3, cpsr
    @ cps #SYS_MODE
    msr cpsr, r0

    @ USER And SYSTEM share the same sp/lr/cpsr
    @ mov r1, sp
    @ mov r2, lr
    stm r1, {sp}
    stm r2, {lr}

    @ cps #SUPER_MODE
    msr cpsr, r3
    bx lr



@ drop in your code from lab 13 [in 1-rfe-asm.S]
@
@ void switchto_user_asm(regs_t *r)  __attribute__((noreturn));
@  - <r> is a 17-entry register entry in r0.
@
@ switch to user mode loading r0-r15, and cpsr from <r>
@ MK_FN(switchto_user_asm)
@     b staff_switchto_user_asm
@     asm_not_reached();
MK_FN(switchto_user_asm)
    @ asm_todo("implement this");
    mov sp, r0
    
    add r1, sp, #60  @ 15
    @ add r2, sp, #64  @ 16

    @ ldr lr, {r1}
    mov sp, r1
    
    ldm r0, {r0-r12,sp,lr}^
    @ cps #USER_MODE

    rfe sp
    @ bx lr
    asm_not_reached();

@ drop in your code from lab 13
@ 
@ void switchto_priv_asm(regs_t *r)  __attribute__((noreturn));
@ MK_FN(switchto_priv_asm)
@    b staff_switchto_priv_asm
@    asm_not_reached();

MK_FN(switchto_priv_asm)
    ldr r1, [r0, #64]  @ 16
    msr cpsr_cxsf, r1
    ldm r0, {r0-r15}
    rfe sp
    asm_not_reached();
    @ add r1, r0, #60  @ 15
    @ mov sp, r1
    @ add r2, r0, #64  @ 16
    @ ldr r1, [r2]
    @ // load value from a priviledged mode
    @ @ ldm r0, {r0-r12,sp,lr}^ is only for user mode
    
    @ mrs r3, cpsr @ 0x20000190
    
    @ msr cpsr_cxsf, r1
    @ mov r1, sp
    @ mov r2, lr
    @ msr cpsr_cxsf, r3

    @ sub sp, sp, #12 @ to sp place
    @ mov sp, r1
    @ add sp, sp, #4 @ lr
    @ mov sp, r2
    @ @ sub sp, sp, #4
    @ @ add sp, sp, #8
    @ mov sp, r0
    @ @ldm r0, {r0-r12} @ 0xc070

    @ @ b tmp

    @ rfe sp
    @ asm_not_reached();
    @ @ sub sp, sp, #64

    @ mov sp, r0
    @ stm sp, {r0-r12,sp,lr} @ pc, cpsr
    @ add sp, sp, #56
    @ stm sp, {lr}
    @ add sp, sp, #64
    @ mrs r2, cpsr
    @ stm sp, {r2}
    @ sub sp, sp, #64

    @ str sp, [r0]
    @ mov sp, r1

    @ ldm sp, {r0-r12,sp,lr}
    @ add sp, sp, #56
    @ rfe sp

@ you'll have to write this, but it should mirror your other save
@ routines.    
@
@ void cswitchto_priv_asm(regs_t *old, regs_t *next);
@ 
@ voluntary context switch: save the current registers in <old>
@ and call the right switchto on <next>
MK_FN(cswitchto_priv_asm)
    @ initially just forward to ours.
    @ b staff_cswitchto_priv_asm
    @ asm_not_reached();
    @ to implement:
    @ 1. save all the callee regs (just like in your threads library)
    @    into the 17-entry register array we have been using.  note
    @    these have to be at the right offsets.
    @    e.g.:
    @       str r4,  [r0, #(4*4)]  @ r4
    @       str r5,  [r0, #(5*4)]  @ r5
    @       ... etc ...
    @   don't forget the cpsr!
    
    str r4,  [r0, #(4*4)]  @ r4
    str r5,  [r0, #(5*4)]  @ r5
    str r6,  [r0, #(6*4)]  @ r6
    str r7,  [r0, #(7*4)]  @ r7
    str r8,  [r0, #(8*4)]  @ r8
    str r9,  [r0, #(9*4)]  @ r9
    str r10, [r0, #(10*4)] @ r10
    str r11, [r0, #(11*4)] @ r11
    str r12, [r0, #(12*4)] @ 12
    str sp, [r0, #(13*4)] @ sp
    str lr, [r0, #(14*4)] @ lr
    str lr, [r0, #(15*4)] @ pc

    mrs r3, cpsr
    str r3, [r0, #(16*4)] @ cpsr

    @ ldr r3, [r0, #64]  @ 16
    @ str r3, [r0, #(16*4)] @ lr

    mov r0, r1

    @ 2. just call the right switch routine: note, we should
    @    *not* reach this location when switching back!
    bl switchto_priv_asm
    asm_not_reached();

@ you'll have to write this, but it should mirror your other save
@ routines.    
@
@ void cswitchto_user_asm(regs_t *old, regs_t *next);
@ 
@ voluntary context switch: save the current registers in <old>
@ and call the right switchto on <next>
MK_FN(cswitchto_user_asm)
    @ initially just forward to ours.
    @ b staff_cswitchto_priv_asm
    @ asm_not_reached();
    @ to implement:
    @ str r0,  [r0, #(0*4)]  @ r0
    @ str r1,  [r0, #(1*4)]  @ r4
    @ str r2,  [r0, #(2*4)]  @ r2
    @ str r3,  [r0, #(3*4)]  @ r3
    @ why we don't need to keep r0-r3?
    str r4,  [r0, #(4*4)]  @ r4
    str r5,  [r0, #(5*4)]  @ r5
    str r6,  [r0, #(6*4)]  @ r6
    str r7,  [r0, #(7*4)]  @ r7
    str r8,  [r0, #(8*4)]  @ r8
    str r9,  [r0, #(9*4)]  @ r9
    str r10, [r0, #(10*4)] @ r10
    str r11, [r0, #(11*4)] @ r11
    str r12, [r0, #(12*4)] @ 12
    str sp, [r0, #(13*4)] @ sp
    str lr, [r0, #(14*4)] @ lr
    str lr, [r0, #(15*4)] @ pc

    mrs r3, cpsr
    str r3, [r0, #(16*4)] @ cpsr

    @ ldr r3, [r0, #64]  @ 16
    @ str r3, [r0, #(16*4)] @ lr

    mov r0, r1
    @ asm_todo("save the registers: identical to <switchto_priv_asm>")

    bl switchto_user_asm
    asm_not_reached();


@ #include "rpi-asm.h"

@ @ all of these are called from <switchto.h>: look there for more
@ @ help in understanding if need to.

@ @ drop in your code from last lab to get sp and lr.
@ @
@ @ void priv_get_sp_lr_asm(uint32_t mode, uint32_t *sp, uint32_t *lr);
@ @   - assumes caller is at privileged mode [we could check this]
@ @   - mode in r0
@ @   - sp pointer in r1
@ @   - lr pointer in r2
@ @
@ MK_FN(priv_get_sp_lr_asm)
@     b staff_priv_get_sp_lr_asm
@     asm_not_reached();


@ @ drop in your code from lab 13 [in 1-rfe-asm.S]
@ @
@ @ void switchto_user_asm(regs_t *r)  __attribute__((noreturn));
@ @  - <r> is a 17-entry register entry in r0.
@ @
@ @ switch to user mode loading r0-r15, and cpsr from <r>
@ MK_FN(switchto_user_asm)
@     b staff_switchto_user_asm
@     asm_not_reached();

@ @ drop in your code from lab 13
@ @ 
@ @ void switchto_priv_asm(regs_t *r)  __attribute__((noreturn));
@ MK_FN(switchto_priv_asm)
@     b staff_switchto_priv_asm
@     asm_not_reached();

@ @ you'll have to write this, but it should mirror your other save
@ @ routines.    
@ @
@ @ void cswitchto_priv_asm(regs_t *old, regs_t *next);
@ @ 
@ @ voluntary context switch: save the current registers in <old>
@ @ and call the right switchto on <next>
@ MK_FN(cswitchto_priv_asm)
@     @ initially just forward to ours.
@     b staff_cswitchto_priv_asm

@     @ to implement:
@     @ 1. save all the callee regs (just like in your threads library)
@     @    into the 17-entry register array we have been using.  note
@     @    these have to be at the right offsets.
@     @    e.g.:
@     @       str r4,  [r0, #(4*4)]  @ r4
@     @       str r5,  [r0, #(5*4)]  @ r5
@     @       ... etc ...
@     @   don't forget the cpsr!


@     @ 2. just call the right switch routine: note, we should
@     @    *not* reach this location when switching back!
@     bl switchto_priv_asm
@     asm_not_reached();

@ @ you'll have to write this, but it should mirror your other save
@ @ routines.    
@ @
@ @ void cswitchto_user_asm(regs_t *old, regs_t *next);
@ @ 
@ @ voluntary context switch: save the current registers in <old>
@ @ and call the right switchto on <next>
@ MK_FN(cswitchto_user_asm)
@     @ initially just forward to ours.
@     b staff_cswitchto_priv_asm

@     asm_todo("save the registers: identical to <switchto_priv_asm>")
@     bl switchto_user_asm
@     asm_not_reached();
