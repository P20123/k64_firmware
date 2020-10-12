#include <environment.h>
#include <nongnu/unistd.h>
#include <string.h>
#include <drivers/kinetis/uart.h>
#include <stdint.h>

typedef struct {
    uint32_t stack_top;
    uint32_t stack_lim;
    int (*main)(void);
    uint8_t pid;
} pcb_t;

int enter_process(pcb_t *which) {
    // configure mpu
    // configure mcm
    MCM->PID = which->pid;
    enter_process_asm(&which->stack_top, which->main);
    // implicitly return contents of r0 (eabi calling conv, return code in r0)
}

int greeter_app_test_sp(int stack_frames) {
    // rough recursive function
    char buf[20];
    buf[5] = 10;
    return (stack_frames)? greeter_app_test_sp(stack_frames-1):0;
}

int greeter_app_main(void) {
    char *msgs[] = {"entering greeter app", "greeter recursion test success"};
    write(0, msgs[0], strlen(msgs[0]));
    greeter_app_test_sp(20);
    write(0, msgs[1], strlen(msgs[1]));
    return 0;
}

pcb_t greeter_proc;

char greeter_stack[200];

int main(void) {
    uart0_conf.input_clock_rate = SystemCoreClock;
    int stdout = uart_init(uart0_conf);
    write(stdout, "started\r\n", 9);
    
    greeter_proc.main = greeter_app_main;
    // process is directly underneath main stack
    greeter_proc.stack_lim = greeter_stack + sizeof(greeter_stack); 
    greeter_proc.stack_top = greeter_stack;
    SIM->SCGC7 |= SIM_SCGC7_MPU_MASK;
    enter_process(&greeter_proc);
    return 0;
}
