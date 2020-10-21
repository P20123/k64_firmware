#include <nongnu/unistd.h>
#include <kernel/file.h>
#include <MK64F12.h>
/**
 * UNIX read/write byte streams
 */

extern void do_pendsvcall(void);

ssize_t read(int fildes, void *buf, size_t nbyte) {
    int r = 0;
    if(fildes > FDT_SIZE) {
        return -1;
    }
    ftab_entry_t *entry = &file_descriptor_table.entries[fildes];
    // FIXME ABSTRACT PID FETCH
    if(entry->pid_lock != 0 && MCM->PID != entry->pid_lock) {
        // FIXME YIELD
        do_pendsvcall();
        return -1;
    }
    entry->pid_lock = MCM->PID;
    r = entry->read(entry->context, buf, nbyte);
    entry->pid_lock = 0;
    return r;
}

ssize_t write(int fildes, const void *buf, size_t nbyte) {
    int r = 0;
    if(fildes > FDT_SIZE) {
        return -1;
    }
    ftab_entry_t *entry = &file_descriptor_table.entries[fildes];
    // FIXME ABSTRACT PID FETCH
    // if the entry is locked, and we are not the process doing the lock
    if(entry->pid_lock != 0 && MCM->PID != entry->pid_lock) {
        // FIXME YIELD
        do_pendsvcall();
        return -1;
    }
    entry->pid_lock = MCM->PID;
    r = entry->write(entry->context, buf, nbyte);
    entry->pid_lock = 0;
    return r;
}
