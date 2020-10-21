#pragma once
#include <stdint.h>
typedef struct {
    /*
     *struct {
     *    uint32_t r4, r5, r6, r7, r8, r9, r10;
     *    union {
     *        uint32_t r11, fp;
     *    };
     *    uint32_t r12;
     *    union {
     *        uint32_t r13, sp;
     *    };
     *    union {
     *        uint32_t r14, lr;
     *    };
     *    union {
     *        uint32_t r15, pc;
     *    };
     *    uint32_t apsr;
     *} registers;
     */
    uint32_t stack_ptr;
    uint32_t stack_top;
    uint32_t stack_lim;
    int (*main)(void);
    uint16_t exc_return_lsb;
    uint8_t pid;
    bool valid;
} pcb_t;

typedef struct {
    pcb_t *proc_tab;
    pcb_t *curr_proc;
    int proc_tab_count;
} proc_table_t;
