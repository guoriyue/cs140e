// Load/store 32-bit?
#include <sys/types.h>
void foo(int32_t a){
    int32_t* x = a;
    printf("load 32-bit\n");
    int32_t b = *x;
}

// Answer: ldr and str
// 00000000 <foo>:
//    0:   e92d4800        push    {fp, lr}
//    4:   e28db004        add     fp, sp, #4, 0
//    8:   e24dd010        sub     sp, sp, #16, 0
//    c:   e50b0010        str     r0, [fp, #-16]
//   10:   e51b3010        ldr     r3, [fp, #-16]
//   14:   e50b3008        str     r3, [fp, #-8]
//   18:   e59f001c        ldr     r0, [pc, #28]   ; 3c <foo+0x3c>
//   1c:   ebfffffe        bl      0 <puts>
//   20:   e51b3008        ldr     r3, [fp, #-8]
//   24:   e5933000        ldr     r3, [r3]
//   28:   e50b300c        str     r3, [fp, #-12]
//   2c:   e1a00000        nop                     ; (mov r0, r0)
//   30:   e24bd004        sub     sp, fp, #4, 0
//   34:   e8bd4800        pop     {fp, lr}
//   38:   e12fff1e        bx      lr
//   3c:   00000000        .word   0x00000000