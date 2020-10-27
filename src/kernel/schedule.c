#include <stddef.h>
#include <stdint.h>
#include <kernel/schedule.h>
#include <kernel/process.h>
#include <kernel/private/static_memory.h>


int scheduler_init(proc_table_t *process_table, pcb_t **proc_list) {
    int status = 0;
    process_table->proc_list = proc_list;
    process_table->curr_proc = *process_table->proc_list;
    process_table->next_pid = 1;
    process_table->num_processes = 0;
    return status;
}

int schedule_process(proc_table_t *process_table, pcb_t *proc) {
    int status = 0;
    if(process_table->num_processes == KERNEL_PTAB_ENTRIES_MAX) {
        status = -1;
        goto done;
    }
    // determine next pid - very easy since we can never remove a process
    // in the current implementation.
    proc->pid = process_table->next_pid;
    process_table->next_pid++;
    process_table->proc_list[process_table->num_processes] = proc;
    process_table->num_processes++;
done:
    return status;
}

// This is just a round-robin scheduler.  Not very efficient, but it will do.
pcb_t *scheduler_main(proc_table_t *process_table) {
    if(process_table->curr_proc - process_table->proc_list[0] ==
            (process_table->num_processes - 1)) {
        process_table->curr_proc = process_table->proc_list[0];
    }
    else {
        process_table->curr_proc++;
    }
    // configure MCM and MPU for memory protection
    MCM->PID = process_table->curr_proc->pid;
    return process_table->curr_proc;
}
