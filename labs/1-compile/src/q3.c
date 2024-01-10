#include <sys/types.h>
// If register r1 holds a memory address (a pointer), what 
// instruction do you use to store an 8-bit integer to that location?
#include <sys/types.h>
void foo(){
    int8_t a = 7;
    int8_t* x = a;
}
// Answer: str
// strb is for storing the 8 bit int into register r3, and it is -5 instead of -8 because it is only 8 bits
// but finally we use str to save the 8 bit int into the location
// 00000000 <foo>:
//    0:   e52db004        push    {fp}            ; (str fp, [sp, #-4]!)
//    4:   e28db000        add     fp, sp, #0, 0
//    8:   e24dd00c        sub     sp, sp, #12, 0
//    c:   e3a03007        mov     r3, #7, 0
//   10:   e54b3005        strb    r3, [fp, #-5]
//   14:   e15b30d5        ldrsb   r3, [fp, #-5]
//   18:   e50b300c        str     r3, [fp, #-12]
//   1c:   e1a00000        nop                     ; (mov r0, r0)
//   20:   e28bd000        add     sp, fp, #0, 0
//   24:   e49db004        pop     {fp}            ; (ldr fp, [sp], #4)
//   28:   e12fff1e        bx      lr