#include <stdint.h>
#include <kernel/process.h>
#include "print_demo.h"
#include <nongnu/unistd.h>
#include <string.h>
#include <environment.h>
pcb_t print_demo_app;
uint32_t print_demo_stack[PRINT_DEMO_STACK_WORDS];


int print_demo_main(void) {
    int bytes = 0;
    char buf[255];
    for(;;) {
        bytes = read(uart0_fileno, buf, 255);
        if(bytes > 0) {
            while((bytes -= write(uart0_fileno, buf, bytes)));
            memset(buf, 0, 255);
            bytes = 0;
        }
    }
}
