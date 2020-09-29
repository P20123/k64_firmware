#include <nongnu/unistd.h>
#include <kernel/kernel_ftab.h>
#include <environment.h>
#include <drivers/uart.h>
#include <stdio.h>
/*
 *#include <alibc/extensions/array.h>
 *#include <alibc/extensions/array_iterator.h>
 */
#include <string.h>

char a = 'a';

int main(void) {
    ftab_init();
    SystemCoreClockUpdate();
    uart0_conf.input_clock_rate = SystemCoreClock;
    int uart0_fd = uart_init(uart0_conf);

    /*
     *array_t *uut = create_array(10, sizeof(int));
     *for(int i = 0; i < 10; i++) {
     *    array_append(uut, i);
     *}
     *iter_context *iter = create_array_iterator(uut);
     *char buf[32];
     *while(iter_okay(iter) == ITER_CONTINUE || iter_okay(iter) == ITER_READY) {
     *    sprintf(buf, "%i\n", *(int*)iter_next(iter));
     *    write(uart0_fd, buf, strlen(buf));
     *}
     */
    char buf[11];
    memcpy(buf, "hello world", 11);
    write(uart0_fd, buf, 11);
    write(uart0_fd, &a, 1);
    a = 'b';
    write(uart0_fd, &a, 1);
    for(;;);
    return 0;
}
