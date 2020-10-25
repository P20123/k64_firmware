#include <stddef.h>
#include <kernel/file.h>
#include <kernel/stream_io.h>
#include <MK64F12.h>
size_t kernel_write(int fd, char *buf, size_t bytes) {
    size_t r = 0;
    ftab_entry_t *entry = &file_descriptor_table.entries[fd];
    // FIXME ABSTRACT PID FETCH
    if(entry->locks.write_pid != 0 && MCM->PID != entry->locks.write_pid) {
        r = -1;
        goto done;
    }
    entry->locks.write_pid = MCM->PID;
    r = entry->write(entry->context, buf, bytes);
    if(r == bytes) {
        entry->locks.write_pid = 0;
    }
done:   
    return r;
}

size_t kernel_read(int fd, char *buf, size_t bytes) {
    size_t r = 0;
    ftab_entry_t *entry = &file_descriptor_table.entries[fd];
    // FIXME ABSTRACT PID FETCH
    if(entry->locks.read_pid != 0 && MCM->PID != entry->locks.read_pid) {
        r = -1;
        goto done;
    }
    entry->locks.read_pid = MCM->PID;
    r = entry->read(entry->context, buf, bytes);
    // this implies that reads are marshalled in order - one call to read
    // will return the specified number of bytes before unlocking the fd.
    // however, it also means that a request for a large number of bytes must
    // reduce the requested number as bytes come in, or the lock will never
    // be released.
    if(r == bytes) {
        entry->locks.read_pid = 0;
    }
done:   
    return r;
}
