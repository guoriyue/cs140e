push    {fp}            
add     fp, sp, #0, 0
sub     sp, sp, #20, 0
str     r0, [fp, #-16]
str     r1, [fp, #-20]  
ldr     r3, [fp, #-16]
str     r3, [fp, #-8]
ldr     r3, [fp, #-8]
ldr     r2, [fp, #-20]  
str     r2, [r3]
nop                   
add     sp, fp, #0, 0
pop     {fp}           
bx      lr