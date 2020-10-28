#pragma once
#include <drivers/kinetis/uart.h>
/**
 * The real thing is in src/kernel/environment.c
 */
extern uart_config uart0_conf;
extern uart_config uart3_conf;
extern int uart0_fileno;
