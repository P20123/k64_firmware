#include <kernel/private/static_memory.h>
/**
 * Process Scheduler
 */
pcb_t *process_list[KERNEL_PTAB_ENTRIES_MAX];
proc_table_t process_table;
