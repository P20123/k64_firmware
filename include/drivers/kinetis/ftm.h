#pragma once
#include <MK64F12.h>
#include <stdint.h>
#include <stdbool.h>

/*********
 * TYPES
 *********/
typedef struct {
    uint8_t ftm_num;                /* ftm num to configure */
    uint32_t *ftm_clock_gate_base;
    uint32_t ftm_clock_gate_mask;
    uint32_t freq;                  /* Desired FTM Counter Frequency */
    uint8_t prescalar;              /* 0-7 Divide by 2^PS */
    uint8_t clksrc;                 /* 0-3 No CLK, SysCLK, FFCLK, EXCLK */
    uint16_t cnt_init;              /* uint16 to initialize cnt */
} ftm_config_t;

typedef struct {
    uint8_t ftm_num;                /* ftm num where to channel resides */
    uint8_t channel;                /* ftm channel to configure */
    uint32_t *port_clock_gate_base;
    uint32_t port_clock_gate_mask;
    uint32_t *pcr;                  /* address to the PCR */
    uint32_t alt;                   /* pin mux alt */
} ftm_ch_config_t;

/**************
 * PROTOTYPES
 **************/

/**
 * Initializes the FTM peripheral.
 * @param config A configuration struct for the FTM peripheral.
 * @param clk_f_hz The frequency of the clock configured for the FTM (before PS).
 * @return 0
 */
int ftm_init(ftm_config_t config, uint32_t clk_f_hz);

/**
 * Initializes a specific channel for an FTM to edge aligned PWM mode.
 * @param ch_config A configuration struct for the FTM channel.
 * @return 0
 */
int ftm_ch_epwm_init(ftm_ch_config_t ch_config);

/**
 * Direct access to the CnV register of a specfic FTM channel.
 * @param ftm_num The number of the FTM peripheral.
 * @param ch_num The channel number to be configured.
 * @param value The value to configure the CnV register.
 * @return 0
 */
int ftm_ch_CnV_set(uint8_t ftm_num, uint8_t ch_num, uint16_t value);

/**
 * Abstracted access to the CnV register of a specfic FTM channel.
 * @param ftm_num The number of the FTM peripheral.
 * @param ch_num The channel number to be configured.
 * @param duty Duty cycle to set the FTM channel to.
 * @return 0
 */
int ftm_ch_duty_set(uint8_t ftm_num, uint8_t ch_num, uint8_t duty);
