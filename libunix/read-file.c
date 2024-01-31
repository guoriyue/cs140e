#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libunix.h"

// allocate buffer, read entire file into it, return it.   
// buffer is zero padded to a multiple of 4.
//
//  - <size> = exact nbytes of file.
//  - for allocation: round up allocated size to 4-byte multiple, pad
//    buffer with 0s. 
//
// fatal error: open/read of <name> fails.
//   - make sure to check all system calls for errors.
//   - make sure to close the file descriptor (this will
//     matter for later labs).
// 
void *read_file(unsigned *size, const char *name) {
    // How: 
    //    - use stat() to get the size of the file.
    //    - round up to a multiple of 4.
    //    - allocate a buffer
    //    - zero pads to a multiple of 4.
    //    - read entire file into buffer (read_exact())
    //    - fclose() the file descriptor
    //    - make sure any padding bytes have zeros.
    //    - return it.   
    // unimplemented();
    struct stat s;

    // printf("name: %s\n", name);
    
    if (stat(name, &s) < 0) {
        panic("stat");
    }
    // printf("s.st_size %d\n", s.st_size);
    // off_t off = s.st_size;
    // printf("off: %lld\n", off);
    *size = s.st_size;
    int padding = (4 - (*size % 4)) % 4;
    unsigned rounded_size = *size + padding;

    void *buf = malloc(rounded_size);
    int fd = open(name, O_RDONLY);
    if (*size == 0) {
        return buf;
    }
    if (read_exact(fd, buf, *size) < 0) {
        panic("read");
    }
    close(fd);
    for (unsigned i = *size; i < rounded_size; i++) {
        ((char *)buf)[i] = 0;
    }
    return buf;
}
