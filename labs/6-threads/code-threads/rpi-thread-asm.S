/*
 * implement these.  ordered in difficulty.
 */
#include "rpi-asm.h"

@ return the current stack pointer.
MK_FN(rpi_get_sp)
    asm_todo("implement this")
    bx lr

MK_FN(rpi_cswitch)
    push {r4-r11,lr}
    str sp, [r0]
    @ ldr sp, [r1] @ [] is defererencing need to understand pointer and scalar
    mov sp, r1
    pop {r4-r11, lr}
    bx lr
    

@ [Make sure you can answer: why do we need to do this?]
@
@ use this to setup each thread for the first time.
@ setup the stack so that when cswitch runs it will:
@	- load address of <rpi_init_trampoline> into LR
@	- <code> into r1, 
@	- <arg> into r0
@ 
MK_FN(rpi_init_trampoline)
    @ asm_todo("implement this")
    mov r0, r4
    mov r1, r5
    blx r1
    bl rpi_exit
    @ bx lr

