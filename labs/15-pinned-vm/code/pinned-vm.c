// put your code here.
//
#include "rpi.h"
#include "libc/bit-support.h"

// has useful enums and helpers.
#include "vector-base.h"
#include "pinned-vm.h"
#include "mmu.h"
#include "procmap.h"

// generate the _get and _set methods.
// (see asm-helpers.h for the cp_asm macro 
// definition)
// arm1176.pdf: 3-149

// define the following routines.
// cp_asm_set(vector_base_asm, p15, 0, c12, c0, 0)
// cp_asm_get(vector_base_asm, p15, 0, c12, c0, 0)
cp_asm_set(lockdown_index, p15, 5, c15, c4, 2)
cp_asm_get(lockdown_index, p15, 5, c15, c4, 2)
cp_asm_set(lockdown_pa, p15, 5, c15, c6, 2)
cp_asm_get(lockdown_pa, p15, 5, c15, c6, 2)
cp_asm_set(lockdown_va, p15, 5, c15, c5, 2)
cp_asm_get(lockdown_va, p15, 5, c15, c5, 2)
cp_asm_set(lockdown_attr, p15, 5, c15, c7, 2)
cp_asm_get(lockdown_attr, p15, 5, c15, c7, 2)
#if 0
// arm1176.pdf: 3-149
void lockdown_index_set(uint32_t x);
uint32_t lockdown_index_get(void);

void lockdown_va_set(uint32_t x);
uint32_t lockdown_va_get(void);

void lockdown_pa_set(uint32_t x);
uint32_t lockdown_pa_get(void);

void lockdown_attr_set(uint32_t x);
uint32_t lockdown_attr_get(void);

void xlate_pa_set(uint32_t x);

// routines to manually check that a translation
// can succeed.  we use these to check that 
// pinned translations are in the TLB.
// see:
//    p 3-80---3-82 in arm1176.pdf

// translate for a privileged read access
void xlate_kern_rd_set(uint32_t x);

// translate for a priviledged write access
void xlate_kern_wr_set(uint32_t x);

// get physical address after manual translation
uint32_t xlate_pa_get(void);

#endif

// do a manual translation in tlb:
//   1. store result in <result>
//   2. return 1 if entry exists, 0 otherwise.
int tlb_contains_va(uint32_t *result, uint32_t va) {
    // 3-79
    assert(bits_get(va, 0,2) == 0);
    // printk("va=%x\n", va);
    // printk("lockdown_va_get()=%x\n", lockdown_va_get());
    // printk("result=%x\n", *result);
    return staff_tlb_contains_va(result, va);
    // looks up the virtual address
    // 1MB
    // 20 bits
    // 5
    // only 3 bits
    for (int i = 0; i < 8; i++) {
        lockdown_index_set(i);
        uint32_t base = lockdown_va_get();
        uint32_t base_mask = 0xFFF00000;
        if ((base & base_mask) == (va & base_mask)) {
            *result = va;
            return 1;
        }
    }
    return 0;
}

// map <va>-><pa> at TLB index <idx> with attributes <e>
void pin_mmu_sec(unsigned idx,  
                uint32_t va, 
                uint32_t pa,
                pin_t e) {

    // staff_pin_mmu_sec(idx, va, pa, e);
    // return;

    demand(idx < 8, lockdown index too large);
    // lower 20 bits should be 0.
    demand(bits_get(va, 0, 19) == 0, only handling 1MB sections);
    demand(bits_get(pa, 0, 19) == 0, only handling 1MB sections);

    if(va != pa)
        panic("for today's lab, va (%x) should equal pa (%x)\n",
                va,pa);

    debug("about to map %x->%x\n", va,pa);


    // these will hold the values you assign for the tlb entries.
    uint32_t x, va_ent, pa_ent, attr;

    lockdown_index_set(idx);

    // typedef struct {
    //     // for today we only handle 1MB sections.
    //     uint32_t G,         // is this a global entry?
    //             asid,      // don't think this should actually be in this.
    //             dom,       // domain id
    //             pagesize;  // can be 1MB or 16MB

    //     // permissions for needed to access page.
    //     //
    //     // see mem_perm_t above: is the bit merge of 
    //     // APX and AP so can or into AP position.
    //     mem_perm_t  AP_perm;

    //     // caching policy for this memory.
    //     // 
    //     // see mem_cache_t enum above.
    //     // see table on 6-16 b4-8/9
    //     // for today everything is uncacheable.
    //     mem_attr_t mem_attr;
    // } pin_t;
    // typedef enum {
    //     perm_rw_user = 0b011, // read-write user 
    //     perm_ro_user = 0b010, // read-only user
    //     perm_na_user = 0b001, // no access user

    //     // kernel only, user no access
    //     perm_ro_priv = 0b101,
    //     // perm_rw_priv = perm_na_user,
    //     perm_rw_priv = perm_na_user,
    //     perm_na_priv = 0b000,
    // } mem_perm_t;
    // typedef enum { 
    //     //                              TEX   C  B 
    //     // strongly ordered
    //     // not shared.
    //     MEM_device     =  TEX_C_B(    0b000,  0, 0),  
    //     // normal, non-cached
    //     MEM_uncached   =  TEX_C_B(    0b001,  0, 0),  

    //     // write back no alloc
    //     MEM_wb_noalloc =  TEX_C_B(    0b000,  1, 1),  
    //     // write through no alloc
    //     MEM_wt_noalloc =  TEX_C_B(    0b000,  1, 0),  

    //     // NOTE: missing a lot!
    // } mem_attr_t;


    // mem_attr_t 5 bits
    // mem_perm_t 3 bits

    // 149
    va_ent = va | (e.asid) | (e.G << 9);
    lockdown_va_set(va_ent);
    // 150, secure and non secure?
    pa_ent = pa | (e.pagesize << 6) | (e.AP_perm << 1) | 1;
    lockdown_pa_set(pa_ent);
    // valid and non valid
    attr = (e.dom << 7) | (e.mem_attr << 1);
    lockdown_attr_set(attr);

    // put your code here.
    // unimplemented();

// #if 0
    // put this back in when defined.
    if((x = lockdown_va_get()) != va_ent)
        panic("lockdown va: expected %x, have %x\n", va_ent,x);
    if((x = lockdown_pa_get()) != pa_ent)
        panic("lockdown pa: expected %x, have %x\n", pa_ent,x);
    if((x = lockdown_attr_get()) != attr)
        panic("lockdown attr: expected %x, have %x\n", attr,x);
// #endif
}

// check that <va> is pinned.  
void pin_check_exists(uint32_t va) {
    if(!mmu_is_enabled())
        panic("XXX: i think we can only check existence w/ mmu enabled\n");

    uint32_t r;
    if(tlb_contains_va(&r, va)) {
        pin_debug("success: TLB contains %x, returned %x\n", va, r);
        assert(va == r);
    } else
        panic("TLB should have %x: returned %x [reason=%b]\n", 
            va, r, bits_get(r,1,6));
}

void domain_access_ctrl_set(uint32_t d) {
    staff_domain_access_ctrl_set(d);
}

static void *null_pt = 0;

// turn the pinned MMU system on.
//    1. initialize the MMU (maybe not actually needed): clear TLB, caches
//       etc.  if you're obsessed with low line count this might not actually
//       be needed, but we don't risk it.
//    2. allocate a 2^14 aligned, 0-filled 4k page table so that any nonTLB
//       access gets a fault.
//    3. set the domain privileges (to DOM_client)
//    4. set the exception handler up using <vector_base_set>
//    5. turn the MMU on --- this can be much simpler than the normal
//       mmu procedure since it's never been on yet and we do not turn 
//       it off.
//    6. profit!

// fill this in based on the test code.
void pin_mmu_init(uint32_t domain_reg) {
    staff_pin_mmu_init(domain_reg);
    return;
}

void pin_mmu_switch(uint32_t pid, uint32_t asid) {
    assert(null_pt);
    staff_mmu_set_ctx(pid, asid, null_pt);
}

// void pin_mmu_on(procmap_t *p) {
//     staff_pin_mmu_on(p);
//     return;
// }