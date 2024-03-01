#include "rpi.h"
#include "pt-vm.h"
#include "helper-macros.h"
#include "procmap.h"

// turn this off if you don't want all the debug output.
enum { verbose_p = 1 };
enum { OneMB = 1024*1024 };

vm_pt_t *vm_pt_alloc(unsigned n) {
    demand(n == 4096, we only handling a fully-populated page table right now);

    vm_pt_t *pt = 0;
    unsigned nbytes = 4096 * sizeof *pt;

    // allocate pt with n entries.
    // pt = staff_vm_pt_alloc(n);
    pt = kmalloc(nbytes);
    assert(pt);
    memset(pt, 0, nbytes);

    demand(is_aligned_ptr(pt, 1<<14), must be 14-bit aligned!);
    return pt;
}

// allocate new page table and copy pt.
vm_pt_t *vm_dup(vm_pt_t *pt1) {
    vm_pt_t *pt2 = vm_pt_alloc(PT_LEVEL1_N);
    memcpy(pt2,pt1,PT_LEVEL1_N * sizeof *pt1);
    return pt2;
}

// same as pinned version: probably should check that
// the page table is set.
void vm_mmu_enable(void) {
    assert(!mmu_is_enabled());
    mmu_enable();
    assert(mmu_is_enabled());
}

// same as pinned version.
void vm_mmu_disable(void) {
    assert(mmu_is_enabled());
    mmu_disable();
    assert(!mmu_is_enabled());
}

// - set <pt,pid,asid> for an address space.
// - must be done before you switch into it!
// - mmu can be off or on.
void vm_mmu_switch(vm_pt_t *pt, uint32_t pid, uint32_t asid) {
    assert(pt);
    mmu_set_ctx(pid, asid, pt);
}

// just like pinned.
void vm_mmu_init(uint32_t domain_reg) {
    // initialize everything, after bootup.
    mmu_init();
    domain_access_ctrl_set(domain_reg);
}

// map the 1mb section starting at <va> to <pa>
// with memory attribute <attr>.
// map <va>-><pa> at TLB index <idx> with attributes <e>
vm_pte_t *
vm_map_sec(vm_pt_t *pt, uint32_t va, uint32_t pa, pin_t attr) 
{
    assert(aligned(va, OneMB));
    assert(aligned(pa, OneMB));

    // today we just use 1mb.
    assert(attr.pagesize == PAGE_1MB);

    unsigned index = va >> 20;
    assert(index < PT_LEVEL1_N);

    vm_pte_t *pte = 0;

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

    // typedef struct first_level_descriptor {
    //     unsigned
    //         tag:2,      // 0-1:2    should be 0b10
    //         B:1,        // 2:1      just like pinned.
    //         C:1,        // 3:1      just like pinned
    //         XN:1,       // 4:1      1 = execute never, 0 = can execute
    //                     // needs to have XP=1 in ctrl-1.
    //         domain:4,   // 5-8:4    b4-10: 0b11 = manager, 0b01 checked perms
    //         IMP:1,      // 9:1      should be set to 0 unless imp-defined 
    //                     //          functionality is needed.
    //         AP:2,       // 10-11:2  permissions, see b4-8/9
    //         TEX:3,      // 12-14:3
    //         APX:1,      // 15:1
    //         S:1,        // 16:1     set=0, deprecated.
    //         nG:1,       // 17:1     nG=0 ==> global mapping, =1 ==> process specific
    //         super:1,    // 18:1     selects between section (0) and supersection (1)
    //         _sbz1:1,    // 19:1     sbz
    //         sec_base_addr:12; // 20-31.  must be aligned.
    //         // tag,      // 0-1:2    should be 0b10
    //         // B,        // 2:1      just like pinned.
    //         // C,        // 3:1      just like pinned
    //         // XN,       // 4:1      1 = execute never, 0 = can execute
    //         //             // needs to have XP=1 in ctrl-1.

    //         // domain,   // 5-8:4    b4-10: 0b11 = manager, 0b01 checked perms
    //         // IMP,      // 9:1      should be set to 0 unless imp-defined 
    //         //             //          functionality is needed.

    //         // AP,       // 10-11:2  permissions, see b4-8/9
    //         // TEX,      // 12-14:3
    //         // APX,      // 15:1     
    //         // S,        // 16:1     set=0, deprecated.
    //         // nG,       // 17:1     nG=0 ==> global mapping, =1 ==> process specific
    //         // super,    // 18:1     selects between section (0) and supersection (1)
    //         // _sbz1,    // 19:1     sbz
    //         // sec_base_addr; // 20-31.  must be aligned.
    // } fld_t;
    
    return staff_vm_map_sec(pt,va,pa,attr);

    // lockdown_index_set(index);
    // uint32_t va_ent = va | (attr.asid) | (attr.G << 9);
    // lockdown_va_set(va_ent);
    // uint32_t pa_ent = pa | (attr.pagesize << 6) | (attr.AP_perm << 1) | 1;
    // lockdown_pa_set(pa_ent);
    // uint32_t attr_ent = (attr.dom << 7) | (attr.mem_attr << 1);
    // lockdown_attr_set(attr_ent);
    // va_ent = va | (e.asid) | (e.G << 9);
    // lockdown_va_set(va_ent);
    // // 150, secure and non secure?
    // pa_ent = pa | (e.pagesize << 6) | (e.AP_perm << 1) | 1;
    // lockdown_pa_set(pa_ent);
    // // valid and non valid
    // attr = (e.dom << 7) | (e.mem_attr << 1);
    // lockdown_attr_set(attr);

    // uint32_t va_entry = pa | attr.AP_perm | (attr.mem_attr << 2) | (attr.G << 17) | (attr.dom << 5) | (1 << 1);
    // pte = &pt[index];

    if(verbose_p)
        vm_pte_print(pt,pte);
    assert(pte);
    return pte;
}

// lookup 32-bit address va in pt and return the pte
// if it exists, 0 otherwise.
vm_pte_t * vm_lookup(vm_pt_t *pt, uint32_t va) {
    return staff_vm_lookup(pt,va);
}

// manually translate <va> in page table <pt>
// - if doesn't exist, returns 0.
// - if does exist returns the corresponding physical
//   address in <pa>
//
// NOTE: we can't just return the result b/c page 0 could be mapped.
vm_pte_t *vm_xlate(uint32_t *pa, vm_pt_t *pt, uint32_t va) {
    return staff_vm_xlate(pa,pt,va);
}

// compute the default attribute for each type.
static inline pin_t attr_mk(pr_ent_t *e) {
    switch(e->type) {
    case MEM_DEVICE: 
        return pin_mk_device(e->dom);
    // kernel: currently everything is uncached.
    case MEM_RW:
        return pin_mk_global(e->dom, perm_rw_priv, MEM_uncached);
   case MEM_RO: 
        panic("not handling\n");
   default: 
        panic("unknown type: %d\n", e->type);
    }
}

// setup the initial kernel mapping.  This will mirror
//  static inline void procmap_pin_on(procmap_t *p) 
// in <15-pinned-vm/code/procmap.h>  but will call
// your vm_ routines, not pinned routines.
//
// if <enable_p>=1, should enable the MMU.  make sure
// you setup the page table and asid. use  
// kern_asid, and kern_pid.
vm_pt_t *vm_map_kernel(procmap_t *p, int enable_p) {
    // the asid and pid we start with.  
    //    shouldn't matter since kernel is global.
    enum { kern_asid = 1, kern_pid = 0x140e };

    vm_pt_t *pt = 0;

    // return staff_vm_map_kernel(p,enable_p);
    uint32_t d = dom_perm(p->dom_ids, DOM_client);

    vm_mmu_init(d);
    pt = vm_pt_alloc(PT_LEVEL1_N);
    // procmap_pin(p);
    // for(unsigned i = 0; i < p->n; i++) {
    //     pr_ent_t *e = &p->map[i];
    //     demand(e->nbytes == MB, "nbytes=%d\n", e->nbytes);

    //     pin_t g;
    //     switch(e->type) {
    //     case MEM_DEVICE:
    //         g = pin_mk_device(e->dom);
    //         break;
    //     case MEM_RW:
    //         // currently everything is uncached.
    //         g = pin_mk_global(e->dom, perm_rw_priv, MEM_uncached);
    //         break;
    //     case MEM_RO: panic("not handling\n");
    //     default: panic("unknown type: %d\n", e->type);
    //     }
    //     pin_mmu_sec(i, e->addr, e->addr, g);
    // }
    for (int i = 0; i < p->n; i++) {
        pr_ent_t *e = &p->map[i];
        pin_t g = attr_mk(e);
        vm_map_sec(pt, e->addr, e->addr, g);
    }
    // procmap_pin(p);

    // printk("pin mmu switch\n");

    // setup.
    vm_mmu_switch(pt, kern_pid, kern_asid);
    // staff_pin_mmu_switch(0,1);
    // pin_mmu_enable();
    if (enable_p) {
        vm_mmu_enable();
    }
    // vm_mmu_enable();


    assert(pt);
    return pt;
}
