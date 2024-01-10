// What register holds the third pointer argument to a routine?
void foo(int* x, int* y, int* z){
    *z = *x + *y;
}

void caller(void)
{
	int x = 1;
    int y = 2;
    int z = 3;
    foo(&x, &y, &z);
}

// Answer: r2, because the third argument is fp - 16, which is put in r2 based on line 68
// 00000044 <caller>:
//   44:   e92d4800        push    {fp, lr}
//   48:   e28db004        add     fp, sp, #4, 0
//   4c:   e24dd010        sub     sp, sp, #16, 0
//   50:   e3a03001        mov     r3, #1, 0
//   54:   e50b3008        str     r3, [fp, #-8]
//   58:   e3a03002        mov     r3, #2, 0
//   5c:   e50b300c        str     r3, [fp, #-12]
//   60:   e3a03003        mov     r3, #3, 0
//   64:   e50b3010        str     r3, [fp, #-16]
//   68:   e24b2010        sub     r2, fp, #16, 0
//   6c:   e24b100c        sub     r1, fp, #12, 0
//   70:   e24b3008        sub     r3, fp, #8, 0
//   74:   e1a00003        mov     r0, r3
//   78:   ebfffffe        bl      0 <foo>
//   7c:   e1a00000        nop                     ; (mov r0, r0)
//   80:   e24bd004        sub     sp, fp, #4, 0
//   84:   e8bd4800        pop     {fp, lr}
//   88:   e12fff1e        bx      lr