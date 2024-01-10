// /* foo.c start */
// static int lock = 0, cnt = 0;

// void bar(void);

// void foo(int *p) {              
//     cnt++;                      // line 1
//     lock = 1;                   // line 2
//     bar();                      // line 3
//     lock = 0;                   // line 4
//     return;                
// }
// // Line 2 and 4 can be removed but line 1 cannot be removed?
// /* foo.c end */

// void foo(int *p, int *q) {
//     // *q = 1; // remove this line because finally *q = 3
//     *p = 2;
//     *q = 3;
//     return;
// }


// #include <stdlib.h>
// int main(void) {
//     int *p = malloc(4);
//     *p = 10;
//     return 0;
// }
// // gcc can remove all lines in this function and only return 0 because nothing is used
// for garbage collection because we always need a value for r0
// 0:   e3a00000        mov     r0, #0, 0