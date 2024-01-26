// Q: what order does a push instruction write its registers?
#include "rpi.h"

enum { val1 = 0xdeadbeef, val2 = 0xFAF0FAF0 };

// you write this in <asm-check.S>
//
// should take a few lines:
//  use the "push" instruction to push <val1>  and <val2>
//  onto the memory pointed to by <addr>.
//
// returns the final address
uint32_t *push_two(uint32_t *addr, uint32_t val1, uint32_t val2);

void notmain() {
    uint32_t v[4] = { 1, 2, 3, 4 };

    uint32_t *res = push_two(&v[2], val1, val2);
    assert(res == &v[0]);

    // note this also shows you the order of writes.
    if(v[2] == val2 && v[1] == val1) {
        assert(v[3] == 4);
        assert(v[0] == 1);
        todo("what does this imply?\n");
    } else if(v[1] == val2 && v[0] == val1) {
        assert(v[3] == 4);
        assert(v[2] == 3);
        // push r1, r2
        // r1 = val1 r2 = val2
        // v[1] r2 v[0] r1
        // so actually first r2 in stack then r1 in stack
        // still r1 in lower address
        todo("what does this imply?\n");
    } else 
        panic("unexpected result\n");
}
