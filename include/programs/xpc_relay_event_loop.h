#pragma once
#include <tinyxpc/tinyxpc.h>
#include <tinyxpc/xpc_relay.h>
#include <kernel/process.h>

/**
 * IPC interface-related items
 */

typedef struct {
    int fd;
    bool notify_on_read, notify_on_write;
    char read_buf[255];
} uart_relay_ctx_t;

extern volatile xpc_relay_state_t uart0_relay;
extern xpc_relay_state_t *comms_relay;

void xpc_yield_until_ready();

/**
 * Process-related items
 */
#define XPC_RELAY_STACK_SIZE 100

extern pcb_t xpc_relay_event_loop_app;
extern uint32_t xpc_relay_event_loop_stack[XPC_RELAY_STACK_SIZE];

int xpc_relay_event_loop_main(void);
