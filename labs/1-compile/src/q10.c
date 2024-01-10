// Implement a an assembly routine unsigned GET32(void *addr) 
// that does a 32-bit load of the memory pointed to by address 
// addr and returns it. Put your implementation in a mem-op.S assembly 
// file and make sure it compiles with your arm-none-eabi-gcc.
unsigned GET32(void *addr){
    unsigned* x = addr;
    unsigned b = *x;
    return b;
}
