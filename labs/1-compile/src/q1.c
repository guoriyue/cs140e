// What register holds a pointer (not integer) return value?

int *fun(void);

int* caller(void)
{
	return fun();
}
// Answer: r0
// 00000000 <caller>:
//    0:   e92d4800        push    {fp, lr}
//    4:   e28db004        add     fp, sp, #4, 0
//    8:   ebfffffe        bl      0 <fun>
//    c:   e1a03000        mov     r3, r0
//   10:   e1a00003        mov     r0, r3
//   14:   e24bd004        sub     sp, fp, #4, 0
//   18:   e8bd4800        pop     {fp, lr}
//   1c:   e12fff1e        bx      lr