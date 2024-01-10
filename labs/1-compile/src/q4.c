// Same as (3) but for load?
#include <sys/types.h>
// If register r1 holds a memory address (a pointer), what 
// instruction do you use to store an 8-bit integer to that location?
void foo(){
    int8_t* x = 7;
    printf("load 8-bit\n");
    int8_t b = *x;
}
// Answer: ldr, ldrb is for loading the 8-bit int into int8_t instead of loading the 8-bit int from the location
// 00000000 <foo>:
//    0:   e92d4800        push    {fp, lr}
//    4:   e28db004        add     fp, sp, #4, 0
//    8:   e24dd008        sub     sp, sp, #8, 0
//    c:   e3a03007        mov     r3, #7, 0
//   10:   e50b3008        str     r3, [fp, #-8]
//   14:   e59f001c        ldr     r0, [pc, #28]   ; 38 <foo+0x38>
//   18:   ebfffffe        bl      0 <puts>
//   1c:   e51b3008        ldr     r3, [fp, #-8]
//   20:   e5d33000        ldrb    r3, [r3]
//   24:   e54b3009        strb    r3, [fp, #-9]
//   28:   e1a00000        nop                     ; (mov r0, r0)
//   2c:   e24bd004        sub     sp, fp, #4, 0
//   30:   e8bd4800        pop     {fp, lr}
//   34:   e12fff1e        bx      lr
//   38:   00000000        .word   0x00000000