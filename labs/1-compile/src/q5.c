// Load/store 16-bit?
#include <sys/types.h>
void foo(int16_t a){
    int16_t* x = a;
    printf("load 16-bit\n");
    int16_t b = *x;
}
// Answer: ldr and str

// 00000000 <foo>:
//    0:   e92d4800        push    {fp, lr}
//    4:   e28db004        add     fp, sp, #4, 0
//    8:   e24dd010        sub     sp, sp, #16, 0
//    c:   e1a03000        mov     r3, r0
//   10:   e14b30be        strh    r3, [fp, #-14]
//   14:   e15b30fe        ldrsh   r3, [fp, #-14]
//   18:   e50b3008        str     r3, [fp, #-8]
//   1c:   e59f001c        ldr     r0, [pc, #28]   ; 40 <foo+0x40>
//   20:   ebfffffe        bl      0 <puts>
//   24:   e51b3008        ldr     r3, [fp, #-8]
//   28:   e1d330b0        ldrh    r3, [r3]
//   2c:   e14b30ba        strh    r3, [fp, #-10]
//   30:   e1a00000        nop                     ; (mov r0, r0)
//   34:   e24bd004        sub     sp, fp, #4, 0
//   38:   e8bd4800        pop     {fp, lr}
//   3c:   e12fff1e        bx      lr
//   40:   00000000        .word   0x00000000