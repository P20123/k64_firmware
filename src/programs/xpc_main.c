#include <stdint.h>
#include <stdbool.h>
#include <tinyxpc/tinyxpc.h>
#include <tinyxpc/xpc_relay.h>
#include <environment.h>
#include <drivers/uart.h>
#include <nongnu/unistd.h>
#include <util/debug.h>
#include <cwpack.h>
#include <MK64F12.h>

const int xpc_scratch_cap = 4096;
int xpc_scratch_bytes;
char xpc_scratch_region[4096];


typedef struct {
    int fd;
    bool notify_on_read, notify_on_write;
    char read_buf[255];
} uart_relay_ctx_t;

typedef struct {

} xpc_dispatch_ctx;

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

xpc_relay_state_t *xpc_relay_config(
    xpc_relay_state_t *target, void *io_ctx, void *msg_ctx, void *crc_ctx,
    io_wrap_fn *write, io_wrap_fn *read, io_reset_fn *reset,
    io_notify_config *io_notify, dispatch_fn *msg_handle_cb,
    crc_fn *crc, crc_polyn_config *crc_config
);

volatile uart_relay_ctx_t uart0_io_ctx;
volatile xpc_relay_state_t uart0_relay;
int stdout;

void init_pit(int period_us) {
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    PIT->MCR |= PIT_MCR_MDIS_MASK;
    PIT->MCR |= PIT_MCR_FRZ_MASK;

    PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK;
    // input is 60 MHz
    PIT->CHANNEL[0].LDVAL = period_us/((SystemCoreClock)/1000000);

    PIT->MCR &= ~PIT_MCR_MDIS_MASK;
    NVIC_EnableIRQ(PIT0_IRQn);
}

/*__attribute__((interrupt))*/
void PIT0_IRQHandler(void) {
    PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
    if(uart0_io_ctx.notify_on_read && read(stdout, NULL, 0) > 0) {
        xpc_rd_op_continue(&uart0_relay);
    }
    if(uart0_io_ctx.notify_on_write) {
        xpc_wr_op_continue(&uart0_relay);
    }
}
int main(void) {
    uart0_conf.input_clock_rate = SystemCoreClock;
    stdout = uart_init(uart0_conf);
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORTB->PCR[21] |= PORT_PCR_MUX(1);
    PORTB->PCR[22] |= PORT_PCR_MUX(1);
    GPIOB->PDDR |= (1 << 21);
    GPIOB->PDDR |= (1 << 22);
    GPIOB->PSOR |= (1 << 21);
    GPIOB->PCOR |= (1 << 22);
    xpc_scratch_bytes = 0;
    int bytes = 0;
    /*
     *char buf[255];
     *while(1) {
     *    bytes = read(stdout, buf, 255);
     *    if(bytes > 0) {
     *        while((bytes -= write(stdout, buf, bytes)));
     *        memset(buf, 0, 255);
     *        bytes = 0;
     *    }
     *}
     */
    /*while((bytes = write(stdout, "hello world\r\n", 13)) < 13);*/

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
    uart0_io_ctx.fd = stdout;
    // 10 ms period
    init_pit(10000);
    int status = TXPC_STATUS_BAD_STATE;
    while(status != TXPC_STATUS_DONE) {
        status = xpc_relay_send_reset(&uart0_relay);
    }
    while(xpc_send_msg(&uart0_relay, 0, 0xA, NULL, 0) != TXPC_STATUS_DONE);
    GPIOB->PCOR |= (1 << 21);
    GPIOB->PSOR |= (1 << 22);

    for(;;);
    return 0;
}
