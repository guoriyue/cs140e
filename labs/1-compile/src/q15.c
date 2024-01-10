#include <sys/types.h>
void PUT8(void *addr, unsigned val){
    int8_t* x = addr;
    *x = val;
}