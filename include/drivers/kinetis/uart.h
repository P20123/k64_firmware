#pragma once
#include <MK64F12.h>
#include <stdint.h>
#include <queue/queue.h>

/**
 * UART parameters required to initialize and route UART I/O.
 * Includes interrupt parameters, if enabled.
 */
typedef struct {
    UART_Type *uart_base;
    uint32_t *rx_pcr;
    uint32_t *tx_pcr;
    uint32_t *rt_pcr;
    uint32_t *ct_pcr;
    uint8_t rx_alt;
    uint8_t tx_alt;
    uint8_t rt_alt;
    uint8_t ct_alt;
    uint32_t baud;
    uint32_t input_clock_rate;
    uint32_t *uart_clock_gate_base;
    uint32_t *port_clock_gate_base;
    uint32_t uart_clock_gate_mask;
    uint32_t port_clock_gate_mask;
    uint8_t configure_interrupts;
    uint8_t irqn;
    uint8_t priority;
} uart_config;

typedef struct {
    UART_Type *uart_base;
    queue_t *txq;
    queue_t *rxq;
} uart_context;

/*
 * Configure a UART device
 * @param conf device configuration
 * @return file descriptor, or -1 on failure.
 */
int uart_init(uart_config conf);
