#include "rpi-asm.h"

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@ <0-rfe-example.c>

@ use rfe to load pc and cpsr from the 2 entry
@ array passed in r0.
MK_FN(rfe_asm)
    mov sp, r0
    rfe sp
    asm_not_reached();

@ a trampoline that loads the stack pointer
@ and calls <rfe_c_code>
MK_FN(rfe_trampoline_asm)
    mov sp, #INT_STACK_ADDR
    mov r0, sp
    bl rfe_c_code
    asm_not_reached();

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@ <1-rfe-blk.c>

@ tiny change: modify rfe_asm to deal with being 
@ passed a 17-entry register array
MK_FN(blk_rfe_asm)
    @ asm_todo("implement this");
    mov sp, r0
    
    add r1, sp, #60  @ 15
    add r2, sp, #64  @ 16

    @ ldr lr, {r1}
    mov lr, r1
    msr cpsr, r2
    mov sp, r1

    @ stm r1, {sp}

    rfe sp
    asm_not_reached();

@ do not need to modify: same as original, so just
@ call it.
MK_FN(blk_trampoline_asm)
    bl rfe_trampoline_asm
    asm_not_reached();


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@ <1-rfe-load-sp.c>

@ copy-paste <blk_rfe_asm> above and change it 
@ so it:
@  1. loads r0-r14 into user mode registers
@     (hint: look back at your <0-user-sp-lr> 
@     code)
@  2. does an rfe 
MK_FN(switchto_user_asm)
    @ asm_todo("implement this");
    mov sp, r0
    
    add r1, sp, #60  @ 15
    @ add r2, sp, #64  @ 16

    @ ldr lr, {r1}
    mov sp, r1

    ldm r0, {r0-r12,sp,lr}^

    rfe sp
    asm_not_reached();


@ so we don't get linker errors 
.weak rfe_c_code
rfe_c_code:
    asm_not_reached()


MK_FN(lr_get)
    mov r0, lr
    bx lr

MK_FN(sp_get)
    mov r0, sp
    bx lr

.weak do_another_mode
do_another_mode:
    asm_not_reached()

@ MK_FN(switchto_anymode_asm)
@     add sp, r0, #60

@     add r1, r0, #52 @ 13
@     add r2, r0, #56 @ 14

@     @ entry #16 
@     @ 64
@     @ [15, 3, 24, ... 0x8202 ,0x17]
@     @                  ^

@     mrs r3, cpsr
@     msr cpsr_c, r0
    
@     ldm r1, {sp}
@     ldm r2, {lr}
@     msr cpsr_c, r3

@     ldm r0, {r0-r12}
@     rfe sp
@     asm_not_reached();

MK_FN(switchto_anymode_asm)
    ldr r1, [r0, #64]  @ 16
    msr cpsr_cxsf, r1
    ldm r0, {r0-r15}
    rfe sp
    asm_not_reached();
    @ add r1, r0, #60  @ 15
    @ mov sp, r1
    @ add r2, r0, #64  @ 16
    @ ldr r1, [r2]
    // load value from a priviledged mode
    @ ldm r0, {r0-r12,sp,lr}^ is only for user mode
    
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