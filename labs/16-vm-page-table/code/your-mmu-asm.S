@ Must carefully read B2 of the armv6 manual.  You cannot rely on "well it
@ worked on my test case": mistakes cause stale data, which may or may not
@ show up in your test case (likely not) despite being wildly broken.
@
@ Note: Rd is allegedly a read-only parameter to these instructions, but
@ the manual says SBZ (should be zero).  I think (and the linux source seems
@ to think) that this means we have to explicitly set it to 0.  Not setting
@ Rd=0 is an easy mistake to make.  Perhaps is worth building in (Linux does
@ not, but) --- the cost of an extra mov is negligible given our other 
@ overheads.
@
@ Alot of people put these instructions in inline assembly, wrapped up by 
@ function calls (e.g., cs107e's MMU code).  This practice is dangerous. 
@ For example, if you are flushing caches, you cannot be sure that the 
@ function return, etc does not then reload them, esp w.r.t. the BTB.  I 
@ think the only safe approach is to put directly in assembly so you are 
@ guaranteed no branch, indirect jump, load/store happens in between.
@
@ A lot of MMU code out there appears pretty broken b/c they don't do enough
@ flushing operations and/or rely on gcc code generation to not do the above.

#include "rpi-asm.h"
#include "armv6-coprocessor-asm.h"


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ many page numbers are in 
@    armv6-coprocessor-asm.h
@ and the other armv6-*.h files

// just read the cache type: use C code to figure out if it is unified
// b6-13, b6-14
MK_FN(get_cache_type)
    asm_not_implemented()
    bx lr

MK_FN(cp15_ctrl_reg1_rd)
    @ mov r1, #0
    CONTROL_REG1_RD(r0)
    @ flush after change to cp15 reg
    @ All changes to CP14 and CP15 registers are only guaranteed to be visible to subsequent instructions after the execution of a PrefetchFlush operation, or the taking of an exception, or the return from an exception.
    @ FLUSH_BTB(r1)
    @ DSB(r1)
    @ says you need to "flush_btb, dsb, prefetch flush" but I think you only need the prefetch flush.
    PREFETCH_FLUSH(r1)
    bx lr

MK_FN(cp15_ctrl_reg1_wr)
    mov r1, #0
    CONTROL_REG1_WR(r0)
    PREFETCH_FLUSH(r1)
    bx lr

// void reset(void)
// clear the dcache, icache, tlbs
MK_FN(mmu_reset)
    @ b staff_mmu_reset
    mov r2, #0
    INV_TLB(r2)
    INV_DCACHE(r2)
    INV_ICACHE(r2)
    @ FLUSH_BTB(r2)
    DSB(r2)
    PREFETCH_FLUSH(r2)
    bx lr

// void cp15_domain_ctrl_wr(uint32_t dom_reg);
//  need to do cp15 sync: flush_btb, dsb, prefetch flush
MK_FN(cp15_domain_ctrl_wr)
    @ b staff_cp15_domain_ctrl_wr
    mov r2, #0
    DOMAIN_CTRL_WR(r0)
    PREFETCH_FLUSH(r2)
    bx lr

// void cp15_set_procid_ttbr0(uint32_t proc_and_asid, fld_t *pt);
// sequence from b2-25
MK_FN(cp15_set_procid_ttbr0)
    @ b staff_cp15_set_procid_ttbr0

    mov r2, #0
    DSB(r2)
    ASID_SET(r2)
    PREFETCH_FLUSH(r2)
    @ no loads and stores
    @ so we don't need DSB before the second ASID

    TTBR_BASE_CTRL_WR(r2)
    TTBR0_SET(r1)
    TTBR1_SET(r2)
    PREFETCH_FLUSH(r2)
    FLUSH_BTB(r2)
    PREFETCH_FLUSH(r2)

    @ DSB(r2)
    @ This ensures that all accesses are related to the correct context ID.
    ASID_SET(r0)
    PREFETCH_FLUSH(r2)
    bx lr

// void mmu_disable_set_asm(cp15_ctrl_reg1_t c);
MK_FN(mmu_disable_set_asm)
    @ b staff_mmu_disable_set_asm

    @ note: this piece of code is mapped into the sample place in both
    @ MMU-enabled and MMU-disabled so BTB and Prefetch entries work identically
    @ to both MMU-enable and disable.   Without this requirement I believe is
    @ very hard to get right (perhaps impossible?) since we cannot flush
    @ the BTB and prefetch and switch MMU on/off atomically.  thus, would
    @ flush, but instructions could get fetched before we could disable,
    @ defeating the purpose.

    mov r2, #0
    CLEAN_INV_DCACHE(r2)
    DSB(r2)
    PREFETCH_FLUSH(r2)

    CONTROL_REG1_WR(r0)
    PREFETCH_FLUSH(r2)

    INV_ICACHE(r2)
    DSB(r2)
    @ don't need a prefetch flush for DSB
    @ PREFETCH_FLUSH(r2)

    FLUSH_BTB(r2)
    PREFETCH_FLUSH(r2)
    bx lr

// void mmu_enable_set_asm(cp15_ctrl_reg1_t c);
MK_FN(mmu_enable_set_asm)
    @ b staff_mmu_enable_set_asm
    mov r2, #0
    CONTROL_REG1_RD(r1)
    @ PREFETCH_FLUSH(r2)
    bic r1, r1, #0x1000  @ disable icache bit 12 turn off
    CONTROL_REG1_WR(r1)
    PREFETCH_FLUSH(r2)
    @ disable icache bit 12 turn off
    @ bit12 control register
    @ invalidate
    @ turn on
    @ bic instruction
    INV_ICACHE(r2)
    DSB(r2)
    PREFETCH_FLUSH(r2)

    CONTROL_REG1_WR(r0)
    PREFETCH_FLUSH(r2)


    FLUSH_BTB(r2)
    PREFETCH_FLUSH(r2)
    @ bic r0, r0, #0x1000  @ enable icache bit 12 turn on
    @ orr r0, r0, #0x1000  @ enable icache bit 12 turn on
    bx lr

// B-23
MK_FN(mmu_sync_pte_mods)
    @ b staff_mmu_sync_pte_mods
    mov r2, #0
    CLEAN_INV_DCACHE(r2)
    INV_ICACHE(r2)
    DSB(r2)
    INV_TLB(r2)
    FLUSH_BTB(r2)
    DSB(r2)
    PREFETCH_FLUSH(r2)
    bx lr


@ @ Must carefully read B2 of the armv6 manual.  You cannot rely on "well it
@ @ worked on my test case": mistakes cause stale data, which may or may not
@ @ show up in your test case (likely not) despite being wildly broken.
@ @
@ @ Note: Rd is allegedly a read-only parameter to these instructions, but
@ @ the manual says SBZ (should be zero).  I think (and the linux source seems
@ @ to think) that this means we have to explicitly set it to 0.  Not setting
@ @ Rd=0 is an easy mistake to make.  Perhaps is worth building in (Linux does
@ @ not, but) --- the cost of an extra mov is negligible given our other 
@ @ overheads.
@ @
@ @ Alot of people put these instructions in inline assembly, wrapped up by 
@ @ function calls (e.g., cs107e's MMU code).  This practice is dangerous. 
@ @ For example, if you are flushing caches, you cannot be sure that the 
@ @ function return, etc does not then reload them, esp w.r.t. the BTB.  I 
@ @ think the only safe approach is to put directly in assembly so you are 
@ @ guaranteed no branch, indirect jump, load/store happens in between.
@ @
@ @ A lot of MMU code out there appears pretty broken b/c they don't do enough
@ @ flushing operations and/or rely on gcc code generation to not do the above.

@ #include "rpi-asm.h"
@ #include "armv6-coprocessor-asm.h"


@ @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ @ many page numbers are in 
@ @    armv6-coprocessor-asm.h
@ @ and the other armv6-*.h files

@ // just read the cache type: use C code to figure out if it is unified
@ // b6-13, b6-14
@ MK_FN(get_cache_type)
@     asm_not_implemented()
@     bx lr

@ MK_FN(cp15_ctrl_reg1_rd)
@     @ mov r1, #0
@     CONTROL_REG1_RD(r0)
@     @ flush after change to cp15 reg
@     @ All changes to CP14 and CP15 registers are only guaranteed to be visible to subsequent instructions after the execution of a PrefetchFlush operation, or the taking of an exception, or the return from an exception.
@     @ FLUSH_BTB(r1)
@     @ DSB(r1)
@     @ says you need to "flush_btb, dsb, prefetch flush" but I think you only need the prefetch flush.
@     PREFETCH_FLUSH(r1)
@     bx lr

@ MK_FN(cp15_ctrl_reg1_wr)
@     mov r1, #0
@     CONTROL_REG1_WR(r0)
@     PREFETCH_FLUSH(r1)
@     bx lr

@ // void reset(void)
@ // clear the dcache, icache, tlbs
@ MK_FN(mmu_reset)
@     @ b staff_mmu_reset
@     @ asm_not_implemented()
@     @ Invalidate all caches (ITLB, DTLB, data cache, instruction cache). 
@     @ Do not clean the data cache since it will potentially write garbage back to memory)
@     INV_TLB(r0)
@     INV_DCACHE(r0)
@     INV_ICACHE(r0)
@     @ ensures completion of the ICache invalidation
@     @ FLUSH_BTB(r0)
@     DSB(r0)
@     @ r0 is already void here
@     @ coherent
@     PREFETCH_FLUSH(r0)
@     bx lr

@ // void cp15_domain_ctrl_wr(uint32_t dom_reg);
@ //  need to do cp15 sync: flush_btb, dsb, prefetch flush
@ MK_FN(cp15_domain_ctrl_wr)
@     @ b staff_cp15_domain_ctrl_wr
@     mov r1, #0
@     DOMAIN_CTRL_WR(r0)
@     @ flush after change to cp15 reg
@     @ All changes to CP14 and CP15 registers are only guaranteed to be visible to subsequent instructions after the execution of a PrefetchFlush operation, or the taking of an exception, or the return from an exception.
@     @ FLUSH_BTB(r1)
@     @ DSB(r1)
@     @ says you need to "flush_btb, dsb, prefetch flush" but I think you only need the prefetch flush.
@     PREFETCH_FLUSH(r1)
@     @ asm_not_implemented()
@     bx lr

@ @ MK_FN(cp15_domain_ctrl_rd)
@ @     mov r1, #0
@ @     DOMAIN_CTRL_RD(r0)
@ @     PREFETCH_FLUSH(r1)
@ @     bx lr
@ // void cp15_set_procid_ttbr0(uint32_t proc_and_asid, fld_t *pt);
@ // sequence from b2-25
@ MK_FN(cp15_set_procid_ttbr0)
@     @ b staff_cp15_set_procid_ttbr0
@     @ ASID Change ASID to 0
@     mov r2, #0
@     @ dsb 3-129
@     @ You must ensure that software performs a Data Synchronization Barrier operation before changes to this register.
@     DSB(r2)
@     ASID_SET(r2)
@     PREFETCH_FLUSH(r2)

@     @ Change Translation Table Base Register
@     TTBR0_SET(r1)
@     PREFETCH_FLUSH(r2)
@     TTBR1_SET(r2)
@     PREFETCH_FLUSH(r2)
@     TTBR_BASE_CTRL_WR(r2)
@     PREFETCH_FLUSH(r2)

@     @ Change ASID to new value
@     @ DSB(r2)
@     ASID_SET(r0)
@     PREFETCH_FLUSH(r2)

@     @ flush BTB
@     FLUSH_BTB(r2)
@     PREFETCH_FLUSH(r2)
@     @ asm_not_implemented()
@     bx lr

@ // void mmu_disable_set_asm(cp15_ctrl_reg1_t c);
@ MK_FN(mmu_disable_set_asm)
@     @ b staff_mmu_disable_set_asm

@     @ note: this piece of code is mapped into the sample place in both
@     @ MMU-enabled and MMU-disabled so BTB and Prefetch entries work identically
@     @ to both MMU-enable and disable.   Without this requirement I believe is
@     @ very hard to get right (perhaps impossible?) since we cannot flush
@     @ the BTB and prefetch and switch MMU on/off atomically.  thus, would
@     @ flush, but instructions could get fetched before we could disable,
@     @ defeating the purpose.

@     @ asm_not_implemented()
@     @ before disabling the MMU you'll need to clean the data cache or you'll lose dirty entries.
@     mov r1, #0
@     CLEAN_INV_DCACHE(r1)
@     @ ensures visibility of the data cleaned from the D Cache
@     DSB(r1)
@     PREFETCH_FLUSH(r1)

@     CONTROL_REG1_WR(r0)
@     PREFETCH_FLUSH(r1)

@     @ the test assumes you did
@     INV_ICACHE(r1)
@     DSB(r1)
@     PREFETCH_FLUSH(r1)

@     FLUSH_BTB(r1)
@     PREFETCH_FLUSH(r1)
@     bx lr

    

@ // void mmu_enable_set_asm(cp15_ctrl_reg1_t c);
@ MK_FN(mmu_enable_set_asm)
@     @ b staff_mmu_enable_set_asm
@     mov r2, #0
@     CONTROL_REG1_RD(r1)
@     PREFETCH_FLUSH(r2)
@     bic r1, r1, #0x1000  @ disable icache bit 12 turn off
@     CONTROL_REG1_WR(r1)
@     PREFETCH_FLUSH(r2)
@     @ disable icache bit 12 turn off
@     @ bit12 control register
@     @ invalidate
@     @ turn on
@     @ bic instruction
@     INV_ICACHE(r2)
@     DSB(r2)
@     PREFETCH_FLUSH(r2)

@     CONTROL_REG1_WR(r0)
@     PREFETCH_FLUSH(r2)


@     FLUSH_BTB(r2)
@     PREFETCH_FLUSH(r2)
@     @ bic r0, r0, #0x1000  @ enable icache bit 12 turn on
@     @ orr r0, r0, #0x1000  @ enable icache bit 12 turn on
@     bx lr
@     @ b staff_mmu_enable_set_asm
    
@     @ mov r1, #0

@     @ @ turn MMU on
@     @ CONTROL_REG1_WR(r0)
@     @ PREFETCH_FLUSH(r1)

@     @ @ the test assumes you did
@     @ INV_ICACHE(r1)
@     @ DSB(r1)
@     @ PREFETCH_FLUSH(r1)

    

@     @ FLUSH_BTB(r1)
@     @ PREFETCH_FLUSH(r1)

@     @ bx lr

@ @ void mmu_sync_pte_mods(void);
@ MK_FN(mmu_sync_pte_mods)
@     @ b staff_mmu_sync_pte_mods
@     @ be conservative and invalidate the icache too

@     @ STR rx, [Page table entry] ; 
@     @ Clean line [Page table entry]
@     @ DSB ; ensures visibility of the data cleaned from the D Cache
@     @ Invalidate TLB entry by MVA [page address]
@     @ Invalidate BTB
@     @ DSB ; ensure completion of the Invalidate TLB
@     @ PrefetchFlush
@     INV_ICACHE(r0)
@     DSB(r0)
@     PREFETCH_FLUSH(r0)

@     CLEAN_INV_DCACHE(r0)
@     DSB(r0)
@     PREFETCH_FLUSH(r0)

@     INV_TLB(r0)
@     DSB(r0)
@     PREFETCH_FLUSH(r0)

@     FLUSH_BTB(r0)
@     @ need to flush the BTB on almost all VM changes.
@     PREFETCH_FLUSH(r0)
@     bx lr


@ @ @ Must carefully read B2 of the armv6 manual.  You cannot rely on "well it
@ @ @ worked on my test case": mistakes cause stale data, which may or may not
@ @ @ show up in your test case (likely not) despite being wildly broken.
@ @ @
@ @ @ Note: Rd is allegedly a read-only parameter to these instructions, but
@ @ @ the manual says SBZ (should be zero).  I think (and the linux source seems
@ @ @ to think) that this means we have to explicitly set it to 0.  Not setting
@ @ @ Rd=0 is an easy mistake to make.  Perhaps is worth building in (Linux does
@ @ @ not, but) --- the cost of an extra mov is negligible given our other 
@ @ @ overheads.
@ @ @
@ @ @ Alot of people put these instructions in inline assembly, wrapped up by 
@ @ @ function calls (e.g., cs107e's MMU code).  This practice is dangerous. 
@ @ @ For example, if you are flushing caches, you cannot be sure that the 
@ @ @ function return, etc does not then reload them, esp w.r.t. the BTB.  I 
@ @ @ think the only safe approach is to put directly in assembly so you are 
@ @ @ guaranteed no branch, indirect jump, load/store happens in between.
@ @ @
@ @ @ A lot of MMU code out there appears pretty broken b/c they don't do enough
@ @ @ flushing operations and/or rely on gcc code generation to not do the above.

@ @ #include "rpi-asm.h"
@ @ #include "armv6-coprocessor-asm.h"


@ @ @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ @ @ many page numbers are in 
@ @ @    armv6-coprocessor-asm.h
@ @ @ and the other armv6-*.h files

@ @ // just read the cache type: use C code to figure out if it is unified
@ @ // b6-13, b6-14
@ @ MK_FN(get_cache_type)
@ @     asm_not_implemented()
@ @     bx lr

@ @ // void reset(void)
@ @ // clear the dcache, icache, tlbs
@ @ MK_FN(mmu_reset)
@ @     b staff_mmu_reset

@ @     asm_not_implemented()
@ @     bx lr

@ @ // void cp15_domain_ctrl_wr(uint32_t dom_reg);
@ @ //  need to do cp15 sync: flush_btb, dsb, prefetch flush
@ @ MK_FN(cp15_domain_ctrl_wr)
@ @     b staff_cp15_domain_ctrl_wr

@ @     asm_not_implemented()
@ @     bx lr

@ @ // void cp15_set_procid_ttbr0(uint32_t proc_and_asid, fld_t *pt);
@ @ // sequence from b2-25
@ @ MK_FN(cp15_set_procid_ttbr0)
@ @     b staff_cp15_set_procid_ttbr0

@ @     asm_not_implemented()
@ @     bx lr

@ @ // void mmu_disable_set_asm(cp15_ctrl_reg1_t c);
@ @ MK_FN(mmu_disable_set_asm)
@ @     b staff_mmu_disable_set_asm

@ @     @ note: this piece of code is mapped into the sample place in both
@ @     @ MMU-enabled and MMU-disabled so BTB and Prefetch entries work identically
@ @     @ to both MMU-enable and disable.   Without this requirement I believe is
@ @     @ very hard to get right (perhaps impossible?) since we cannot flush
@ @     @ the BTB and prefetch and switch MMU on/off atomically.  thus, would
@ @     @ flush, but instructions could get fetched before we could disable,
@ @     @ defeating the purpose.

@ @     asm_not_implemented()
@ @     bx lr

@ @ // void mmu_enable_set_asm(cp15_ctrl_reg1_t c);
@ @ MK_FN(mmu_enable_set_asm)
@ @     b staff_mmu_enable_set_asm

@ @     asm_not_implemented()
@ @     bx lr

@ @ MK_FN(mmu_sync_pte_mods)
@ @     b staff_mmu_sync_pte_mods

@ @     asm_not_implemented()
@ @     bx lr
