#include <environment.h>
#include <nongnu/unistd.h>
#include <stdint.h>
#include <multitasking.h>
#include <tinyxpc/tinyxpc.h>
#include <tinyxpc/xpc_relay.h>
#include <programs/xpc_relay_event_loop.h>
#include <drivers/devices/status_leds.h>
#include <stdbool.h>

volatile uart_relay_ctx_t uart0_io_ctx;
volatile xpc_relay_state_t uart0_relay;
xpc_relay_state_t *comms_relay = &uart0_relay;

pcb_t xpc_relay_event_loop_app;
uint32_t xpc_relay_event_loop_stack[XPC_RELAY_STACK_SIZE];

bool ready = false;

void xpc_yield_until_ready() {
    if(!ready) yield();
}



void try_io_op() {
    if(uart0_io_ctx.notify_on_read && read(uart0_fileno, NULL, 0) > 0) {
        xpc_rd_op_continue(&uart0_relay);
    }
    if(uart0_io_ctx.notify_on_write) {
        xpc_wr_op_continue(&uart0_relay);
    }
}

int uart_relay_write(void *io_ctx, char **buffer, int offset, size_t bytes_max) {
    uart_relay_ctx_t *ctx = (uart_relay_ctx_t*)io_ctx;
    return write(ctx->fd, *buffer + offset, bytes_max);
}

int uart_relay_read(void *io_ctx, char **buffer, int offset, size_t bytes_max) {
    uart_relay_ctx_t *ctx = (uart_relay_ctx_t*)io_ctx;
    int bytes = 0;
    // read into the specified location if read_buf is set, otherwise store it
    // in our own context.
    if(*buffer == NULL) {
        bytes = read(ctx->fd, ctx->read_buf + offset, bytes_max);
        // tell the relay where we read into
        *buffer = (char*)ctx->read_buf;
    }
    else {
        bytes = read(ctx->fd, *buffer + offset, bytes_max);
    }
    if(bytes < 0) {
        bytes = 0;
    }
    return bytes > 0 ? bytes:0;
}

void uart_relay_io_reset(void *io_ctx, int which, size_t bytes) {
    uart_relay_ctx_t *ctx = (uart_relay_ctx_t*)io_ctx;
    if(which) {
        /*ctx->read_offset = 0;*/
    }
    else {
        /*ctx->write_offset = 0;*/
    }
}

void uart_relay_io_notify(void *io_ctx, int which, bool enable) {
    uart_relay_ctx_t *ctx = (uart_relay_ctx_t*)io_ctx;
    if(which) {
        ctx->notify_on_read = enable;
    }
    else {
        ctx->notify_on_write = enable;
    }
}

bool xpc_dispatch_msg(void *msg_ctx, txpc_hdr_t *msg_hdr, char *payload) {
    bool status = false;

done:
    return status;
}


int xpc_relay_event_loop_main(void) {
    ready = false;
    xpc_relay_config(
        &uart0_relay,
        &uart0_io_ctx,
        NULL,
        NULL,
        uart_relay_write,
        uart_relay_read,
        uart_relay_io_reset,
        uart_relay_io_notify,
        xpc_dispatch_msg,
        NULL, NULL
    );
    uart0_io_ctx.notify_on_write = true;
    uart0_io_ctx.fd = uart0_fileno;

    // require buffer reset on both ends to synchronize relays.
    RED_LED_ON();
    int status = TXPC_STATUS_BAD_STATE;
    while(status != TXPC_STATUS_DONE) {
        status = xpc_relay_send_reset(&uart0_relay);
        try_io_op();
        yield();
    }
    // delay until reset is received
    while(uart0_relay.inflight_wr_op.op == TXPC_OP_RESET){
        try_io_op();
        yield();
    }
    RED_LED_OFF();
    ready = true;

    for(;;) {
        try_io_op();
        yield();
    }
    return 0;
}
