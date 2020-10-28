#pragma once
#include <kernel/process.h>

/**
 * Process Scheduler
 */
#ifndef KERNEL_PTAB_ENTRIES_MAX
#define KERNEL_PTAB_ENTRIES_MAX 8
#endif
extern pcb_t *process_list[];
extern proc_table_t process_table;
