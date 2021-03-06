/**
 * PendSV_Handler is the entry point for the scheduler.  The actual scheduling
 * algorithm takes place in scheduler_main().  The entry and exit for the
 * scheduler must be written in assembly, because of the system-specific stack
 * exchanges that must take place.
 */
.syntax unified
.text

.extern scheduler_main
.extern process_table

.global PendSV_Handler
.thumb_func
PendSV_Handler:
// current register layout:
// r12 = process_table ptr
// r0 = curr_proc ptr
// r1 = prev. thread frame ptr
// r2 = scratch
    cpsid i
    /** save context of process **/
    /** The kernel is theoretically preemptable **/
    ldr r12, =process_table

    // determine if prev. fp is psp or msp
    // we can only get here from kernel thread or process thread, because pendsv
    // is the lowest priority exception (only options are 0x9 or 0xD).
    ands r0, lr, #0xF // keep only the lower 4 bits
    cmp r0, #0x9
    itee ne
    mrsne r1, psp
    mrseq r1, msp
    beq from_kernel_proc

    /** switching from a user process **/
    // registers r0-r3 are already on the process stack
    // save registers r4-r11 before process sw.
    stmdb r1!, {r4-r11}
    // if the process was using the fpu, also push the high registers.
    tst lr, #0x10
    it eq
    vstmdbeq r1!, {s16-s31}

    // save the psp to the pcb
    ldr r0, [r12, #4] // r0 has curr_proc (pointer)
    ldr r0, [r0, #0]
    str r1, [r0, #0] // save psp (points to END of stack AFTER context push)

    // save the exc_return value to the pcb
    strh lr, [r0, #16]

    b do_process_switch

from_kernel_proc:
    /** switching from a kernel process **/
    // There are no kernel processes, so this is NOT IMPLEMENTED.
    // This is passthrough for the first context switch away from kernel_main.

do_process_switch:
    // r0 has the process_table pointer
    movs r0, r12
    // switch to a new process here
    stmdb sp!, {lr}
    bl scheduler_main
    ldmia sp!, {lr}
    // r0 has ptr to the target pcb

    // load psp from target pcb
    ldr r1, [r0, #0]
    // load exc_return_lsb from target pcb
    ldrh r2, [r0, #16]
    // unstack the fpu context, if it was in use.
    tst r2, #0x10
    it eq
    vldmiaeq r1!, {s16-s31}

    // restore r4-r11 from the process stack
    ldmia r1!, {r4-r11}
    
    // restore the psp, as if no context switch happened (no extra frame)
    msr psp, r1
    // exception return with adjusted sp and CONTROL, core takes over from here.
    // core sets CONTROL on exception exit based on PC value.
    // load lr for exc_return
    bics lr, lr, #255
    orrs lr, lr, r2
    cpsie i
    bx lr
