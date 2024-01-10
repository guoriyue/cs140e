// What does an infinite loop look like? Why?

int main(){
    while(1){
        printf("Hello World!\n");
    }
    return 0;
}

// Answer: Line 10 is an infinite loop because it is a branch to line 8
// 00000000 <main>:
//    0:   e92d4800        push    {fp, lr}
//    4:   e28db004        add     fp, sp, #4, 0
//    8:   e59f0004        ldr     r0, [pc, #4]    ; 14 <main+0x14>
//    c:   ebfffffe        bl      0 <puts>
//   10:   eafffffc        b       8 <main+0x8>
//   14:   00000000        .word   0x00000000