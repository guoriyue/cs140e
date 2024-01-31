// engler, cs140e: your code to find the tty-usb device on your laptop.
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "libunix.h"

#define _SVID_SOURCE
#include <dirent.h>
static const char *ttyusb_prefixes[] = {
    "ttyUSB",	// linux
    "ttyACM",   // linux
    "cu.SLAB_USB", // mac os
    "cu.usbserial", // mac os
    // if your system uses another name, add it.
	0
};

static int filter(const struct dirent *d) {
    // scan through the prefixes, returning 1 when you find a match.
    // 0 if there is no match.
    // unimplemented();
    for (int i = 0; ttyusb_prefixes[i] != 0; i++) {
        if (strstr(d->d_name, ttyusb_prefixes[i]) != 0) {
            return 1;
        }
    }
    return 0;
}

// find the TTY-usb device (if any) by using <scandir> to search for
// a device with a prefix given by <ttyusb_prefixes> in /dev
// returns:
//  - device name.
// error: panic's if 0 or more than 1 devices.
char *find_ttyusb(void) {
    // use <alphasort> in <scandir>
    // return a malloc'd name so doesn't corrupt.
    // unimplemented();
    struct dirent **namelist;
    int n = scandir("/dev", &namelist, filter, alphasort);
    if (n == 0) {
        panic("no ttyusb devices found");
    }
    if (n > 1) {
        panic("more than one ttyusb device found");
    }
    char *name = malloc(strlen(namelist[0]->d_name) + 1);
    strcpy(name, namelist[0]->d_name);
    return name;
}

int compare_mtime(const void *a, const void *b) {
    struct stat stat_a, stat_b;
    stat(((struct dirent *)a)->d_name, &stat_a);
    stat(((struct dirent *)b)->d_name, &stat_b);

    if (stat_a.st_mtime < stat_b.st_mtime) return 1;
    if (stat_a.st_mtime > stat_b.st_mtime) return -1;
    return 0;
}


// return the most recently mounted ttyusb (the one
// mounted last).  use the modification time 
// returned by state.
char *find_ttyusb_last(void) {
    // unimplemented();
    struct dirent **namelist;
    int n = scandir("/dev", &namelist, filter, alphasort);
    if (n == 0) {
        panic("no ttyusb devices found");
    }
    if (n > 1) {
        panic("more than one ttyusb device found");
    }
    qsort(namelist, n, sizeof(struct dirent *), compare_mtime);
    char *name = malloc(strlen(namelist[n-1]->d_name) + 1);
    strcpy(name, namelist[n-1]->d_name);
    return name;
}

// return the oldest mounted ttyusb (the one mounted
// "first") --- use the modification returned by
// stat()
char *find_ttyusb_first(void) {
    // unimplemented();
    struct dirent **namelist;
    int n = scandir("/dev", &namelist, filter, alphasort);
    if (n == 0) {
        panic("no ttyusb devices found");
    }
    if (n > 1) {
        panic("more than one ttyusb device found");
    }
    qsort(namelist, n, sizeof(struct dirent *), compare_mtime);
    char *name = malloc(strlen(namelist[0]->d_name) + 1);
    strcpy(name, namelist[0]->d_name);
    return name;
}
