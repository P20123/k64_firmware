/**
 * This is the entry point for system calls, using the ARM SVC mechanism.
 * The entry point must be written in assembly in order to recover the SVC
 * immediate argument, as well as set up the SVC arguments for the dispatcher.
 * The SVC dispatcher can take up to 4 arguments in registers r0-r3, the last of
 * which is used as a pointer to a memory location where the service called will
 * write its result or return value, for services which return data.
 *
 * Services may be called via this mechanism from either kernel or user threads,
 * but NOT exception threads. Using the SVC mechanism from an exception will
 * cause an ARM UsageFault. Exceptions which need to call a kernel service
 * will have to call it directly on ARM systems.
 */
.syntax unified
.text

.extern svcall_main

.global SVC_Handler
.thumb_func
SVC_Handler:
    // determine which stack pointer the context was pushed to
    tst lr, #4
    ite eq
    mrseq r0, msp
    mrsne r0, psp
    // retrieve the link address (read: return address)
    ldr r1, [r0, #24]
    // two bytes back from that address is the instruction which caused the
    // SVC, the last byte of which is the SVC immediate argument
    ldrsb r1, [r1, #-2]
    // r0 -> appropriate sp (for args)
    // r1 -> svc number
    b svcall_main
