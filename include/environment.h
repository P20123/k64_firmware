#pragma once
#include <drivers/kinetis/uart.h>
#include <drivers/kinetis/i2c.h>
#include <drivers/kinetis/ftm.h>
/**
 * The real thing is in src/kernel/environment.c
 */
extern uart_config uart0_conf;
extern uart_config uart3_conf;
extern i2c_config_t i2c0_conf;
extern ftm_config_t ftm3_conf;
extern ftm_ch_config_t ftm3_ch5_conf;
extern ftm_ch_config_t ftm3_ch4_conf;
extern ftm_config_t ftm0_conf;
extern ftm_ch_config_t ftm0_ch0_conf;
extern ftm_ch_config_t ftm0_ch1_conf;
extern int uart0_fileno;
