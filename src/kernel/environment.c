#include <environment.h>
#include <drivers/kinetis/uart.h>
#include <drivers/kinetis/i2c.h>
#include <drivers/kinetis/ftm.h>

// UART 0 CONFIGURATION (OpenSDA UART <-> USB)
uart_config uart0_conf = {
    .uart_base = UART0,
    .rx_pcr = &(PORTB->PCR[16]),
    .tx_pcr = &(PORTB->PCR[17]),
    .rt_pcr = 0,
    .ct_pcr = 0,
    .rx_alt = 3,
    .tx_alt = 3,
    .rt_alt = 255,
    .ct_alt = 255,
    .baud = 115200,
    .input_clock_rate = 120000000,
    .uart_clock_gate_base = &(SIM->SCGC4),
    .port_clock_gate_base = &(SIM->SCGC5),
    .uart_clock_gate_mask = SIM_SCGC4_UART0_MASK,
    .port_clock_gate_mask = SIM_SCGC5_PORTB_MASK,
    .configure_interrupts = 1,
    .irqn = UART0_RX_TX_IRQn,
    .priority = 0,
    .rwfifo_sz = 0,
    .twfifo_sz = 8
};

// UART 3 CONFIGURATION (HM-10 BLE UART)
uart_config uart3_conf = {
    .uart_base = UART3,
    .rx_pcr = &(PORTC->PCR[16]),
    .tx_pcr = &(PORTC->PCR[17]),
    .rt_pcr = 0,
    .ct_pcr = 0,
    .rx_alt = 3,
    .tx_alt = 3,
    .rt_alt = 255,
    .ct_alt = 255,
    .baud = 9600,
    .input_clock_rate = 60000000,
    .uart_clock_gate_base = &(SIM->SCGC4),
    .port_clock_gate_base = &(SIM->SCGC5),
    .uart_clock_gate_mask = SIM_SCGC4_UART3_MASK,
    .port_clock_gate_mask = SIM_SCGC5_PORTC_MASK,
    .configure_interrupts = 1,
    .irqn = UART3_RX_TX_IRQn,
    .priority = 1,
    .rwfifo_sz = 0,
    .twfifo_sz = 0
};

i2c_config_t i2c0_conf = {
    .i2c_num = 0,
    .baud = 90000,
    .stophold = true,
    .interrupt = true,
    .filter_clocks = 0,
    .priority = 0
};

ftm_config_t ftm3_conf = {
    .ftm_num = 3u,
    .ftm_clock_gate_base = &(SIM->SCGC3),
    .ftm_clock_gate_mask = SIM_SCGC3_FTM3_MASK,
    .freq = 50u,
    .prescalar = 5u,
    .clksrc = 1u,
    .cnt_init = 0u
};

ftm_config_t ftm0_conf = {
    .ftm_num = 0u,
    .ftm_clock_gate_base = &(SIM->SCGC6),
    .ftm_clock_gate_mask = SIM_SCGC6_FTM0_MASK,
    .freq = 400u,
    .prescalar = 2u,
    .clksrc = 1u,
    .cnt_init = 0u
};

int uart0_fileno;
