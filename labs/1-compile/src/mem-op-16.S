push    {fp}          
add     fp, sp, #0, 0
sub     sp, sp, #20, 0
str     r0, [fp, #-16]
ldr     r3, [fp, #-16]
str     r3, [fp, #-8]
ldr     r3, [fp, #-8]
ldrh    r3, [r3]
strh    r3, [fp, #-10]
ldrsh   r3, [fp, #-10]
mov     r0, r3
add     sp, fp, #0, 0
pop     {fp}           
bx      lr