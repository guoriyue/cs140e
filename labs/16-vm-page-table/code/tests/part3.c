#include "rpi.h"
#include "libc/bit-support.h"

// has useful enums and helpers.
#include "vector-base.h"
#include "mmu.h"
#include "procmap.h"

// from last lab.
#include "switchto.h"
#include "full-except.h"

#define MOD 10005
// coproc
// chapter 3
// dcache 
// icache
// Data Cache Lockdown Register
// Instruction Cache Lockdown Register
// cp_asm_set(lockdown_index, p15, 5, c15, c4, 2)
// fn_name, coproc, opcode_1, Crn, Crm, opcode_2
cp_asm_set(data_cache_lockdown, p15, 0, c9, c0, 1)
cp_asm_set(instruction_cache_lockdown, p15, 0, c9, c0, 0)
cp_asm_get(data_cache_lockdown, p15, 0, c9, c0, 1)
cp_asm_get(instruction_cache_lockdown, p15, 0, c9, c0, 0)
cp_asm_set(cache_type, p15, 0, c0, c0, 1)
cp_asm_get(cache_type, p15, 0, c0, c0, 1)
// MRC p15, 0, <Rd>, c1, c0, 0 MCR p15, 0, <Rd>, c1, c0, 0

cp_asm_set(ctrl_reg, p15, 0, c1, c0, 0)
cp_asm_get(ctrl_reg, p15, 0, c1, c0, 0)
// MCR p15,0,<Rd>,c7,c10,4
cp_asm_set(sync_barrier, p15, 0, c7, c10, 4)
void notmain(void) { 
    int mat_a[1024 * 128];
    int mat_b[128 * 16];
    int mat_c[1024 * 16];
    for (int i = 0; i < 1024 * 128; i++) {
        mat_a[i] = i % MOD;
    }
    for (int i = 0; i < 128 * 16; i++) {
        mat_b[i] = i % MOD;
    }
    // memset(mat_c, 0, sizeof(mat_c));
    for (int i = 0; i < 1024 * 16; i++) {
        mat_c[i] = 0;
    }
    // baseline of matrix multiplication
    uint32_t start = timer_get_usec();
    for (int i = 0; i < 1024; i++) {
        for (int j = 0; j < 16; j++) {
            for (int k = 0; k < 128; k++) {
                mat_c[i * 16 + j] += mat_a[i * 128 + k] * mat_b[k * 16 + j];
            }
        }
    }
    sync_barrier_set(0);
    uint32_t end = timer_get_usec();
    printk("%d usec\n", end - start);
    // baseline: 164895 usec
    

    // uint32_t dcl = data_cache_lockdown_get();
    // printk("data cache lockdown: %x\n", dcl);
    // dcl |= 0x0f;
    // data_cache_lockdown_set(dcl);
    // uint32_t icl = instruction_cache_lockdown_get();
    // printk("instruction cache lockdown: %x\n", icl);
    // icl |= 0b0100;
    // instruction_cache_lockdown_set(icl);
    // uint32_t ct = cache_type_get();
    // printk("cache type: %x\n", ct);
    uint32_t control_reg = ctrl_reg_get();
    // printk("control reg: %x\n", control_reg);
    control_reg |= 0b1 << 12;
    // printk("control reg: %x\n", control_reg);
    ctrl_reg_set(control_reg);
    // instruction cache 12
    // 
    // cache type: 0x1d152152
    // 0001 1101 0001 0101 0010 0001 0101 0010
    // 1KB cache
    // 1024 Byte = 2048 numbers
    // ct &= 0b0000 << 18; // 0101 originally 16KB cache
    // ct |= 0b0111 << 18; // 64KB cache


    start = timer_get_usec();
    for (int i = 0; i < 1024; i++) {
        for (int j = 0; j < 16; j++) {
            for (int k = 0; k < 128; k++) {
                mat_c[i * 16 + j] += mat_a[i * 128 + k] * mat_b[k * 16 + j];
            }
        }
    }
    sync_barrier_set(0);
    end = timer_get_usec();
    printk("%d usec\n", end - start);
    // get data cache
    
    // data cache lockdown: 0xfffffff0
    // instruction cache lockdown: 0xfffffff0
    printk("SUCCESS!\n");
}