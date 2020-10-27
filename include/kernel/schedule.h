#pragma once
#include <kernel/process.h>
/**
 * Initialize the process table and any state needed for the process scheduler.
 * @param process_table pointer to the process table to be initialized.
 * @param proc_list pointer to array of process control block pointers
 * @return 0 on success, -1 on failure.
 */
int scheduler_init(proc_table_t *process_table, pcb_t **proc_list);

/**
 * Add an initialized process to the process queue.
 * The process will run at the next free time slot not used by another process.
 * @param process_table the process table in which to add the process
 * @param proc the process control block describing the process state
 * @return 0 on success, -1 on failure.
 */
int schedule_process(proc_table_t *process_table, pcb_t *proc);

/**
 * Scheduling algorithm implementation.
 * Do not call this function directly.  It is called by the scheduler entry
 * point function, which is platform-specific.  The scheduler entry function
 * may be called on an interrupt, or whenever a process should yield and this
 * function will determine the next process to run.
 * @param process_table the active process table.
 * @return pointer to next process to run.
 */
pcb_t *scheduler_main(proc_table_t *process_table);
