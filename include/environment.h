#pragma once
#include <drivers/kinetis/uart.h>
#include <drivers/kinetis/i2c.h>
/**
 * The real thing is in src/kernel/environment.c
 */
extern uart_config uart0_conf;
extern uart_config uart3_conf;
extern i2c_config_t i2c0_conf;
extern int uart0_fileno;
