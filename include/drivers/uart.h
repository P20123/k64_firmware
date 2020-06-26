#pragma once
#include <MK64F12.h>
// fun fact: we don't actually use DMA.  It's single-character interrupts.
//#define FEATURE_UART_DMA // only targeting k64 right now, so yes.

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


/**
 * Software Queue written for CMPE-250.  These are the fields in a C struct.
 */
typedef struct {
    char *in_ptr;
    char *out_ptr;
    char *buf_start;
    char *buf_past;
    char buf_size;
    char num_enqd;
} queue_t;

typedef struct {
    UART_Type *uart_base;
#ifndef FEATURE_UART_DMA
    queue_t *txq;
    queue_t *rxq;
#endif
} uart_context;

/*
 * Configure a UART for communication
 */
int uart_init(uart_config conf);

// ignore this for now, this is for a better hal at some point
#ifdef MCU_MK64F12
#endif
