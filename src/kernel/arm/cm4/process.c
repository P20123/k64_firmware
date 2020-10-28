#include <kernel/process.h>
#include <stdbool.h>
#include <multitasking.h>
/**
 * Fake an exception entry frame for a process, which makes starting a new
 * process identical to switching to it.  A context switch stack push is also
 * faked here, to make it even easier to start the process.
 *
 * The constants, as well as the source for the general flow of this function
 * is taken from the ARMv7-M Technical Reference Manual, Section B 1.5.6:
 * Exception Entry Behavior.
 * This function is intentionally structured to look exactly like the
 * pseudocode written in that section for the ExceptionEntry() function.
 *
 * @param proc PCB whose stack should be initialized.
 * @param uses_fp whether the process will use the floating point unit or not.
 * @param kernel_mode whether the process will run in kernel mode or not.
 * @return 0 on success, -1 on failure.
 */
int process_init(pcb_t *proc, int (*main)(void),
        uint32_t *stack_start, size_t stack_size,
        bool uses_fp, bool kernel_mode) {
    int framesize = 0x20;
    uint32_t spmask = 0;
    bool forcealign = SCB->CCR & SCB_CCR_STKALIGN_Msk;

    proc->main = main;
    proc->stack_lim = (uint32_t)stack_start;
    proc->stack_top = (uint32_t)(stack_start + stack_size);
    proc->stack_ptr = proc->stack_top;
    proc->pid = 1;

    if(uses_fp) {
        framesize = 0x68;
        forcealign = true;
    }
    // 8-byte alignment?
    spmask = forcealign? ~(1 << 2):~0;
    proc->stack_ptr = (proc->stack_ptr - framesize) & spmask;
    // general-purpose registers
    *(uint32_t*)((uint8_t*)proc->stack_ptr) = 0;
    *(uint32_t*)((uint8_t*)proc->stack_ptr + 0x4) = 1;
    *(uint32_t*)((uint8_t*)proc->stack_ptr + 0x8) = 2;
    *(uint32_t*)((uint8_t*)proc->stack_ptr + 0xC) = 3;
    *(uint32_t*)((uint8_t*)proc->stack_ptr + 0x10) = 12;
    *(uint32_t*)((uint8_t*)proc->stack_ptr + 0x14) = process_stop;
    // our entry point is the "return address" of the faked exception
    *(uint32_t*)((uint8_t*)proc->stack_ptr + 0x18) = (uint32_t)proc->main;
    // program status register (thumb mode ON, stack alignment mode)
    *(uint32_t*)((uint8_t*)proc->stack_ptr + 0x1C) =
        (1 << 24) | ((proc->stack_ptr & (forcealign ? 1:0)) << 9);

    // also move the stack ptr down to make room for r4-r11, as if this process
    // has already been started and has had r4-r11 pushed (this is for the
    // process switcher, not the CM4 exception entry procedure)
    proc->stack_ptr -= 0x20;
    // do the same for the floating point registers, if they are in use.
    // this makes room for s16-s31
    if(uses_fp) {
        proc->stack_ptr -= 0x40;
    }
    // also initialize the exc_return_lsb based on the FP and kernel state
    char proc_type = ((uses_fp? 1:0) << 1) | (kernel_mode? 1:0);
    switch(proc_type) {
        // user mode, no fp
        case 0:
            proc->exc_return_lsb = 0xFD;
        break;

        // kernel mode, no fp
        case 1:
            proc->exc_return_lsb = 0xF9;
        break;

        // user mode, with fp
        case 2:
            proc->exc_return_lsb = 0xED;
        break;

        // kernel mode, with fp
        case 3:
            proc->exc_return_lsb = 0xE9;
        break;

        default:
        break;
    }
}

// it would be nice to de-init and de-schedule processes, but that is currently
// NOT IMPLEMENTED.
__attribute__((noreturn))
void process_stop(void) {
    yield();
    asm("b .");
}

int get_syscaller_pid(void) {
    return (int)(MCM->PID & 0xff);
}

int get_active_pid(void) __attribute__((alias ("get_syscaller_pid")));
