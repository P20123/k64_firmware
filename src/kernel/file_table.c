#include <kernel/file.h>
#include <stdint.h>
/**
 * Functions and macros relating to the kernel's file descriptor table.
 */

#define EMPTY_ENTRY (ftab_entry_t){\
    .context = 0,\
    .read = 0,\
    .write = 0,\
    .close = 0,\
    .locks = {.read_pid = 0, .write_pid = 0}\
}

#define IS_EMPTY_ENTRY(e) (((uint32_t)e.context | (uint32_t)e.read | (uint32_t)e.write | (uint32_t)e.close) == 0)

ftab_t file_descriptor_table;
void *fdt_address = &file_descriptor_table;

void ftab_init() {
    file_descriptor_table.next_fd = 0;
    for(int i = 0; i < FDT_SIZE; i++) {
        file_descriptor_table.entries[i] = EMPTY_ENTRY;
    }
}

int ftab_open(ftab_entry_t entry) {
    // fill the next open fd
    if(file_descriptor_table.next_fd > FDT_SIZE) {
        // table is full
        return -1;
    }
    int new_fd = file_descriptor_table.next_fd;
    // insert the entry
    file_descriptor_table.entries[new_fd] = entry;
    // update next_fd - start at new_fd + 1 for efficiency
    for(int i = new_fd + 1; i < FDT_SIZE || i == new_fd; i++) {
        if(IS_EMPTY_ENTRY(file_descriptor_table.entries[i])) {
            file_descriptor_table.next_fd = i;
            break;
        }
        if(i == FDT_SIZE - 1) {
            i = 0;
        }
    }
    // return fd of entry
    return new_fd;
}

int ftab_close(int fd) {
    if(fd >= FDT_SIZE) {
        return 1;
    }
    file_descriptor_table.entries[fd] = EMPTY_ENTRY;

    // reset, use lowest fd available to enspeeden future searches.
    if(fd < file_descriptor_table.next_fd) {
        file_descriptor_table.next_fd = fd;
    }
    return 0;
}
