// Similarly, write an assembly routine 
// void PUT32(void *addr, unsigned val) that 
// stores the 32-bit quantity val into the memory 
// pointed to by address addr. 
// Also put this in mem-op.S and make sure it compiles.

void PUT32(void *addr, unsigned val){
    unsigned* x = addr;
    *x = val;
}