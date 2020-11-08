#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
// It would be nice to abstract this, but we can't do that right now.
#include <MK64F12.h>
/**
 * Process Control Block
 * The PCB contains the minimal set of information that the kernel needs
 * in order to handle context switching.
 *
 * Note that this is Cortex-M4 specific, and other platforms will need
 * a different set of information.
 */
typedef struct {
    uint32_t stack_ptr;
    uint32_t stack_top;
    uint32_t stack_lim;
    int (*main)(void);
    uint16_t exc_return_lsb;
    uint8_t pid;
    bool valid;
} pcb_t;

/**
 * Process Table
 * The Process Table maintains the kernel's list of open processes.  The process
 * scheduler uses this table to swap out processes and maintain their state.
 */
typedef struct {
    pcb_t **proc_list;
    pcb_t **curr_proc;
    int next_pid;
    int num_processes;
} proc_table_t;

/**
 * Initialize a new process without scheduling it for execution.
 * @param proc pointer to a process control block to be initialized
 * @param main the entry point of the new process.
 * @param stack_start pointer to a word-aligned buffer for the process stack.
 * @param stack_size number of bytes in the process stack.
 * @param uses_fp whether the process will use the floating point unit or not.
 * @param kernel_model whether the process will run in kernel or in user mode.
 * @return 0 on success, -1 on failure.
 */
int process_init(
    pcb_t *proc, int (*main)(void),
    uint32_t *stack_start, size_t stack_size,
    bool uses_fp, bool kernel_mode
);

/**
 * Exit point for any process.  This function is what process entry points will
 * return to when they exit.
 */
void process_stop(void);

/**
 * Obtain the process identifier of the process which made the active syscall
 */
int get_syscaller_pid(void);

/**
 * Obtain the process identifier of the currently active process
 */
int get_active_pid(void);
