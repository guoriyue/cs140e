#include "rpi.h"
#include "trace.h"

void __wrap_notmain(void);
void __real_notmain(void);

void __wrap_notmain(void) {
    // implement this function!
    printk("about to call __real_notmain()\n");
    __real_notmain();
    clean_reboot();
}
