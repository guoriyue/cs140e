// complete working example of trivial vm:
//  1. identity map the kernel (code, heap, kernel stack, 
//     exception stack, device memory)
//  2. turn VM on/off making sure we can print.
//  3. with VM on, write to an unmapped address and make sure we 
//     get a fault.
#include "rpi.h"
#include "pinned-vm.h"
#include "mmu.h"       

// from last lab.
#include "switchto.h"
#include "full-except.h"
#include "procmap.h"

enum { OneMB = 1024*1024};

// used to store the illegal address we will write.
static volatile uint32_t illegal_addr;

// a trivial fault handler that checks that we got the fault
// we expected.
static void fault_handler(regs_t *r) {
    printk("in fault handler\n");
    uint32_t fault_addr;
    // n the fault handler, prints the pc, 
    // the ARMv6 "reason" for the fault (using the dfsr), 
    // re-enables the domain permissions, and returns.

    printk("pc=%x\n", r->regs[15]);
    // b4-44
    asm volatile("MRC p15, 0, %0, c6, c0, 0" : "=r" (fault_addr));
    uint32_t dfsr;
    asm volatile("MRC p15, 0, %0, c5, c0, 0" : "=r" (dfsr));
    printk("fault addr=%x, dfsr=%x\n", fault_addr, dfsr);

    // re-enable the domain permissions.
    uint32_t dacs = DOM_client << 1*2 | DOM_client << 2*2;
    staff_domain_access_ctrl_set(dacs);

    

    // // make sure we faulted on the address that should be accessed.
    // if(fault_addr != illegal_addr)
    //     panic("illegal fault!  expected %x, got %x\n",
    //         illegal_addr, fault_addr);
    // else
    //     trace("SUCCESS!: got a fault on address=%x\n", fault_addr);

    // // done with test.
    // trace("all done: going to reboot\n");
    // clean_reboot();
}


void notmain(void) { 
    assert(!mmu_is_enabled());

    // // compute all the domain permissions.
    // uint32_t d = dom_perm(p->dom_ids, DOM_client);

    // pin_mmu_init(d);
    // procmap_pin(p);

    // pin_debug("about to turn on mmu\n");

    // // setup.
    // pin_mmu_switch(0,1);
    // pin_mmu_enable();

    // assert(mmu_is_enabled());
    // pin_debug("enabled!\n");

    // // can only check this after MMU is on.
    // pin_debug("going to check entries are pinned\n");
    // for(unsigned i = 0; i < p->n; i++)
    //     pin_check_exists(p->map[i].addr);


    // A domain fault. Write a single test that:

    // Tags the heap with its own domain id d.
    // Removes permissions for d, does a load using GET32, and gets the fault.
    // In the fault handler, prints the pc, the ARMv6 "reason" for the fault (using the dfsr), re-enables the domain permissions, and returns.
    // Do (2) and (3) for store. Use (PUT32) so you can check the pc.
    // Do (2) and (3) for a jump. You'll have to write the instruction for bx lr to a heap location and jump to it. Note: for this you'll need to also install a prefetch abort handler (just like we did last lab).


    // ******************************************************
    // 1. one-time initialization.
    //   - create an empty page table (to catch errors).
    //   - initialize all the vm hardware
    //   - compute permissions for no-user-access.


    // map the heap: for lab cksums must be at 0x100000.
    kmalloc_init_set_start((void*)OneMB, OneMB);

    // if we are correct this will never get accessed.
    // since all valid entries are pinned.
    void *null_pt = kmalloc_aligned(4096*4, 1<<14);
    assert((uint32_t)null_pt % (1<<14) == 0);

    // initialize everything, after bootup.
    // <mmu.h>
    staff_mmu_init();

    // definitions in <pinned-vm.h>
    //
    // see 3-151 for table, or B4-9
    //    given: APX = 0, AP = 1
    //    the bits are: APX << 2 | AP
    uint32_t AXP = 0;
    uint32_t AP = 1;
    uint32_t no_user = AXP << 2 | 1; // no access user (privileged only)
    assert(perm_rw_priv == no_user);

    // current index into the 8 pinned entries in tlb.
    unsigned idx = 0;

    // armv6 has 16 different domains with their own privileges.
    // just pick one for the kernel.
    enum { 
        dom_kern = 1, // domain id for kernel
        dom_heap = 2, // domain id for heap
    };          

    // ******************************************************
    // 2. setup device memory.
    // 
    // permissions: kernel domain, no user access, 
    // memory rules: strongly ordered, not shared.
    pin_t dev  = pin_mk_global(dom_kern, no_user, MEM_device);
    
    // map all device memory to itself.  ("identity map")
    pin_mmu_sec(idx++, 0x20000000, 0x20000000, dev);   // tlb 0
    pin_mmu_sec(idx++, 0x20100000, 0x20100000, dev);   // tlb 1
    pin_mmu_sec(idx++, 0x20200000, 0x20200000, dev);   // tlb 2
    // vm map

    // ******************************************************
    // 3. setup kernel memory: 

    // protection: same as device.
    // memory rules: uncached access.
    pin_t kern = pin_mk_global(dom_kern, no_user, MEM_uncached);

    // Q1: if you uncomment this, what happens / why?
    // kern = pin_mk_global(dom_kern, perm_ro_priv, MEM_uncached);
    // hangs? why?
    // because it's perm_ro_priv, read only, can not write

    // map first two MB for the kernel (1: code, 2: heap).
    //
    // NOTE: obviously it would be better to not have 0 (null) 
    // mapped, but our code starts at 0x8000 and we are using
    // 1mb sections.  you can fix this as an extension.  
    // very useful!
    pin_mmu_sec(idx++, 0, 0, kern);                    // tlb 3
    pin_t heap = pin_mk_global(dom_heap, no_user, MEM_uncached);
    pin_mmu_sec(idx++, OneMB, OneMB, heap);            // tlb 4

    // now map kernel stack (or nothing will work)
    uint32_t kern_stack = STACK_ADDR-OneMB;
    pin_mmu_sec(idx++, kern_stack, kern_stack, kern);   // tlb 5
    uint32_t except_stack = INT_STACK_ADDR-OneMB;

    // Q2: if you comment this out, what happens?
    pin_mmu_sec(idx++, except_stack, except_stack, kern);
    // no exception stack, will hang
    // cannot jump to fault_handler and returns

    // ******************************************************
    // 4. setup vm hardware.
    //  - page table, asid, pid.
    //  - domain permissions.

    // b4-42: give permissions for all domains.

    // Q3: if you set this to ~0, what happens w.r.t. Q1?
    uint32_t dacs = DOM_client << dom_kern*2;
    staff_domain_access_ctrl_set(dacs);
     
    // staff_domain_access_ctrl_set(~0); 
    // works fine
    // 11, Accesses are not checked against the access permission bits in the TLB entry, so a permission fault cannot be generated

    // set address space id, page table, and pid.
    // note:
    //  - pid never matters, it's just to help the os.
    //  - asid doesn't matter for this test b/c all entries 
    //    are global
    //  - the page table is empty (since pinning) and is
    //    just to catch errors.
    enum { ASID = 1, PID = 128 };
    staff_mmu_set_ctx(PID, ASID,null_pt);

    // if you want to see the lockdown entries.
    // lockdown_print_entries("about to turn on first time");

    // ******************************************************
    // 5. turn it on/off, checking that it worked.
    // trace("about to enable\n");
    // for(int i = 0; i < 10; i++) {
    //     staff_mmu_enable();

    //     if(mmu_is_enabled())
    //         trace("MMU ON: hello from virtual memory!  cnt=%d\n", i);
    //     else
    //         panic("MMU is not on?\n");

    //     staff_mmu_disable();
    //     assert(!mmu_is_enabled());
    //     trace("MMU is off!\n");
    // }

    // ******************************************************
    // 6. setup exception handling and make sure we get a fault.

    // just like last lab.  setup a data abort handler.
    full_except_install(0);
    full_except_set_data_abort(fault_handler);
    full_except_set_prefetch(fault_handler);


    staff_mmu_enable();
    illegal_addr = OneMB + 0x1000;
    trace("remove permissions for d: got %x\n", GET32(illegal_addr));

    staff_domain_access_ctrl_set(0b01<<1*2);
    trace("try PUT32\n");
    PUT32(illegal_addr, 0xdeadbeef);

    


    uint32_t bx_lr = 0xe12fff1e;

    // PUT32(illegal_addr, bx_lr);
    *(uint32_t*)illegal_addr = bx_lr;
    trace("try jump\n");
    // can also cast to a function pointer and call it
    staff_domain_access_ctrl_set(0b01<<1*2);
    BRANCHTO(illegal_addr);
    // asm volatile("bx %0" : : "r" (illegal_addr));
    
    // Do (2) and (3) for a jump. 
    // You'll have to write the instruction for bx lr to a heap 
    // location and jump to it. Note: for this you'll need to 
    // also install a prefetch abort handler (just like we did last lab).
    // asm volatile("mov r0, #0xdeadbeef");


    // Removes permissions for d, does a load using GET32, and gets the fault.

    // // the address we will write to (2MB) we know this is not mapped.
    // illegal_addr = OneMB + OneMB;

    // // this <PUT32> should "work" since vm is off.
    // assert(!mmu_is_enabled());
    // PUT32(illegal_addr, 0xdeadbeef);
    // trace("we wrote without vm: got %x\n", GET32(illegal_addr));
    // assert(GET32(illegal_addr) == 0xdeadbeef);

    // // this should fault.
    // staff_mmu_enable();
    // assert(mmu_is_enabled());
    // PUT32(illegal_addr, 0xdeadbeef);
    // panic("should not reach here\n");
}
