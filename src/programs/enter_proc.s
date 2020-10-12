.syntax unified
.text
.thumb_func
.global enter_process_asm
enter_process_asm:
// args:
//   r0: address of stack top address
//   r1: process entry point address
// returns:
//   process return code in r0.

    push {r2, lr}
    // use psp
    movs r2, #1
    mrs r2, control
    isb
    // load psp = process stack top
    ldr sp, [r0, #0]
    // use psp and set unprivileged mode
    movs r2, #3
    mrs r2, control
    isb
    // jump to process main
    bx r1;
    // return - r0 is the return value of the process.
    pop {r2, pc}
