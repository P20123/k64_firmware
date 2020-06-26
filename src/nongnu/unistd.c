#include <nongnu/unistd.h>
#include <kernel/kernel_ftab.h>
/**
 * UNIX read/write byte streams
 */

ssize_t read(int fildes, void *buf, size_t nbyte) {
    if(fildes > FDT_SIZE) {
        return -1;
    }
    ftab_entry_t entry = file_descriptor_table.entries[fildes];
    return entry.read(entry.context, buf, nbyte);
}

ssize_t write(int fildes, const void *buf, size_t nbyte) {
    if(fildes > FDT_SIZE) {
        return -1;
    }
    ftab_entry_t entry = file_descriptor_table.entries[fildes];
    return entry.write(entry.context, buf, nbyte);
}
