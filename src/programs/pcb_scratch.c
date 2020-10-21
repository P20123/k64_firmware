#include <environment.h>
#include <nongnu/unistd.h>
#include <string.h>
#include <drivers/kinetis/uart.h>
#include <stdint.h>
#include <kernel/process.h>

proc_table_t process_table;
pcb_t process_list[2];

/**
 * This just fakes an exception entry / context switch, to simplify the
 * scheduler assembly exit.  Note that all processes are user-mode, currently.
 */
void proc_init_cm4(proc_table_t *proc_tab, int which, bool uses_fp) {
    pcb_t *proc = proc_tab->proc_tab + which;
    int framesize = 0x20;
    uint32_t spmask = 0;
    uint32_t *fp = 0;
    bool forcealign = SCB->CCR & SCB_CCR_STKALIGN_Msk;
    if(uses_fp) {
        framesize = 0x68;
        forcealign = true;
    }
    // 8-byte alignment?
    spmask = forcealign? ~(1 << 3):0;
    proc->stack_ptr = (proc->stack_ptr - framesize) & spmask;
    // general-purpose registers
    *(uint32_t*)((uint8_t*)proc->stack_ptr) = 0;
    *(uint32_t*)((uint8_t*)proc->stack_ptr + 0x4) = 1;
    *(uint32_t*)((uint8_t*)proc->stack_ptr + 0x8) = 2;
    *(uint32_t*)((uint8_t*)proc->stack_ptr + 0xC) = 3;
    *(uint32_t*)((uint8_t*)proc->stack_ptr + 0x10) = 12;
    *(uint32_t*)((uint8_t*)proc->stack_ptr + 0x14) = 0; // FIXME LR
    *(uint32_t*)((uint8_t*)proc->stack_ptr + 0x18) = (uint32_t)proc->main; // pc "return address"
    *(uint32_t*)((uint8_t*)proc->stack_ptr + 0x1C) = (1 << 24) | ((proc->stack_ptr & (forcealign ? 1:0)) << 8); // xpsr
    // also move the stack ptr down to make room for r4-r11, as if this process
    // has already been started and has had r4-r11 pushed
    proc->stack_ptr -= 0x20;
    // do the same for the floating point registers, if they are in use.
    if(uses_fp) {
        proc->stack_ptr -= 0x20;
    }
    // also initialize the exc_return_lsb based on the FP state
    /*
     *if(uses_fp) {
     *    proc->exc_return_lsb = 0xFD;
     *}
     *else {
     *    proc->exc_return_lsb = 0xED;
     *}
     */
}


void init_pit(int period_us) {
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    PIT->MCR |= PIT_MCR_MDIS_MASK;
    PIT->MCR |= PIT_MCR_FRZ_MASK;

    PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK;
    // input is 20.97 MHz
    PIT->CHANNEL[0].LDVAL = period_us*((SystemCoreClock)/1000000);

    PIT->MCR &= ~PIT_MCR_MDIS_MASK;
    NVIC_EnableIRQ(PIT0_IRQn);
    NVIC->IP[PIT0_IRQn] = 0;
}

#define yield() do_pendsvcall()
void do_pendsvcall(void) {
    SCB->ICSR |= (1 << SCB_ICSR_PENDSVSET_Pos);
    asm("dsb");
    asm("isb");
}

void PIT0_IRQHandler(void) {
    PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
    do_pendsvcall();
}


/* guaranteeing 4-byte alignment */
uint32_t app1_stack[500];
uint32_t app2_stack[500];

extern int app1_main(void);
extern int app2_main(void);

int main(void) {
    uart0_conf.input_clock_rate = SystemCoreClock;
    int stdout = uart_init(uart0_conf);
    write(stdout, "started\r\n", 9);
    
    process_list[0].main = app1_main;
    process_list[0].stack_lim = (uint32_t)app1_stack;
    process_list[0].stack_top = (uint32_t)(app1_stack + 500);
    process_list[0].stack_ptr = process_list[0].stack_top;
    process_list[0].pid = 1;
    process_list[0].exc_return_lsb = 0xFD;

    process_list[1].main = app2_main;
    process_list[1].stack_lim = (uint32_t)app2_stack;
    process_list[1].stack_top = (uint32_t)(app2_stack + 500);
    process_list[1].stack_ptr = process_list[1].stack_top;
    process_list[1].pid = 2;
    process_list[1].exc_return_lsb = 0xFD;

    process_table.proc_tab = process_list;
    process_table.proc_tab_count = 2;
    process_table.curr_proc = process_list;

    /*SIM->SCGC7 |= SIM_SCGC7_MPU_MASK;*/
    NVIC_EnableIRQ(PendSV_IRQn);
    NVIC_SetPriority(PendSV_IRQn, 16);

    proc_init_cm4(&process_table, 0, false);
    proc_init_cm4(&process_table, 1, false);
    // start pit interrupt w/ 10ms period
    init_pit(1000);
    /*
     *asm("svc #0");
     *asm("svc #1");
     */
    do_pendsvcall();
    return 0;
}


void svcall_main(uint32_t *args, char svnum) {
    switch(svnum) {
        case 0:
            write(0, "zero\r\n", 6);
        break;
        case 1:
            write(0, "one\r\n", 5);
        break;
        default:
        break;
    }
}

pcb_t *scheduler_main(void) {
    // process switch here
    if(process_table.curr_proc == &process_list[0]) {
        process_table.curr_proc = &process_list[1];
    }
    else {
        process_table.curr_proc = &process_list[0];
    }
    // mcm/mpu config here
    MCM->PID = process_table.curr_proc->pid;
    return process_table.curr_proc;
}
