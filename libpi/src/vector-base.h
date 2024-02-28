#ifndef __VECTOR_BASE_SET_H__
#define __VECTOR_BASE_SET_H__
#include "libc/bit-support.h"
#include "asm-helpers.h"
#include "rpi.h"
// #   define is_aligned(_p,_n) (((unsigned)(_p))%(_n) == 0)

/*
 * vector base address register:
 *   3-121 --- let's us control where the exception jump table is!
 *
 * defines: 
 *  - vector_base_set  
 *  - vector_base_get
 */

cp_asm_set(vector_base_asm, p15, 0, c12, c0, 0)
cp_asm_get(vector_base_asm, p15, 0, c12, c0, 0)

// return the current value vector base is set to.
static inline void *vector_base_get(void) {
    // todo("implement using inline assembly to get the vec base reg");   
    return (void*)vector_base_asm_get(); 
}

// check that not null and alignment is good.
static inline int vector_base_chk(void *vector_base) {
    // todo("check that not null and alignment is correct.");
    // return 1;
    if (vector_base) {
        // if (is_aligned(vector_base, 32)) {
        if ((((unsigned)(vector_base))%(32) == 0)) {
            return 1;
        }
    }
    return 0;
}

// set vector base: must not have been set already.
static inline void vector_base_set(void *vec) {
    if(!vector_base_chk(vec))
        panic("illegal vector base %p\n", vec);

    void *v = vector_base_get();
    // if already set to the same vector, just return.
    if(v == vec)
        return;

    if(v) 
        panic("vector base register already set=%p\n", v);

    // todo("set vector base here.");
    vector_base_asm_set((uint32_t)vec);

    // make sure it equals <vec>
    v = vector_base_get();
    if(v != vec)
        panic("set vector=%p, but have %p\n", vec, v);
}

// set vector base to <vec> and return old value: could have
// been previously set (i.e., non-null).
static inline void *
vector_base_reset(void *vec) {
    void *old_vec = 0;

    if(!vector_base_chk(vec))
        panic("illegal vector base %p\n", vec);

    // todo("validate <vec> and set as vector base\n");
    old_vec = vector_base_get();
    vector_base_asm_set((uint32_t)vec);

    assert(vector_base_get() == vec);
    return old_vec;
}
#endif
