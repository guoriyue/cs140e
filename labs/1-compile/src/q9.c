// How do you call a routine whose address 
// is in a register rather than a constant? 
// (You'll use this in the threads lab.)

int my_routine(int a, int b) {
    return a + b;
}

int main() {
    // save to a register
    int (*routine_to_a_register)(int, int) = &my_routine;
    int result = routine_to_a_register(2, 3);

    // save to a constant
    int (*const routine_to_a_constant)(int, int) = &my_routine;
    result = routine_to_a_constant(4, 5);
    return 0;
}

// 00000000 <my_routine>:
//    0:   e52db004        push    {fp}            ; (str fp, [sp, #-4]!)
//    4:   e28db000        add     fp, sp, #0, 0
//    8:   e24dd00c        sub     sp, sp, #12, 0
//    c:   e50b0008        str     r0, [fp, #-8]
//   10:   e50b100c        str     r1, [fp, #-12]
//   14:   e51b2008        ldr     r2, [fp, #-8]
//   18:   e51b300c        ldr     r3, [fp, #-12]
//   1c:   e0823003        add     r3, r2, r3
//   20:   e1a00003        mov     r0, r3
//   24:   e28bd000        add     sp, fp, #0, 0
//   28:   e49db004        pop     {fp}            ; (ldr fp, [sp], #4)
//   2c:   e12fff1e        bx      lr

// 00000030 <main>:
//   30:   e92d4800        push    {fp, lr}
//   34:   e28db004        add     fp, sp, #4, 0
//   38:   e24dd010        sub     sp, sp, #16, 0
//   3c:   e59f304c        ldr     r3, [pc, #76]   ; 90 <main+0x60>
//   40:   e50b3008        str     r3, [fp, #-8]
//   44:   e51b3008        ldr     r3, [fp, #-8]
//   48:   e3a01003        mov     r1, #3, 0
//   4c:   e3a00002        mov     r0, #2, 0
//   50:   e1a0e00f        mov     lr, pc
//   54:   e12fff13        bx      r3
//   58:   e50b000c        str     r0, [fp, #-12]
//   5c:   e59f302c        ldr     r3, [pc, #44]   ; 90 <main+0x60>
//   60:   e50b3010        str     r3, [fp, #-16]
//   64:   e51b3010        ldr     r3, [fp, #-16]
//   68:   e3a01005        mov     r1, #5, 0
//   6c:   e3a00004        mov     r0, #4, 0
//   70:   e1a0e00f        mov     lr, pc
//   74:   e12fff13        bx      r3
//   78:   e50b000c        str     r0, [fp, #-12]
//   7c:   e3a03000        mov     r3, #0, 0
//   80:   e1a00003        mov     r0, r3
//   84:   e24bd004        sub     sp, fp, #4, 0
//   88:   e8bd4800        pop     {fp, lr}
//   8c:   e12fff1e        bx      lr
//   90:   00000000        .word   0x00000000