#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <nongnu/unistd.h>
#include <kernel/kernel_ftab.h>
#include <environment.h>
#include <drivers/uart.h>
#include <tinyxpc/proto_msg.h>
#include <util/debug.h>
#include <wchar.h>

static int stdout;

static bool txpc_ready;

typedef struct {
    uint8_t crc_bytes;
    uint32_t crc_polyn;
    uint16_t version;
} txpc_router_state_t;

int txpc_accumulate_bytes(int fd) {

}

void txpc_send_msg(int fd, uint8_t to, uint8_t from, char *body, size_t size) {
    if(!txpc_ready) return;
    txpc_msg_t msg = {
        .hdr ={
            .type = TXPC_FRAME_TYPE_MSG,
            .to = to,
            .from = from,
            .size = size & 0xff
        }
    };
    write(fd, &msg, sizeof(txpc_msg_t));
    write(fd, body, size & 0xff);
}


/**
 * Create a debug message from a format string, it's unique identifier, and
 * argument list.
 *
 *                       DO NOT CALL THIS FUNCTION DIRECTLY
 *
 * Seriously you will make a huge headache for yourself. Use the os_log macro.
 */
int vos_log(char *fmt, uint16_t id, ...) {
    va_list argv;
    char *currptr = fmt;
    char *endptr = fmt;
    void *currarg = NULL;
    char msgbuf[255];
    char *msgbuf_write;
    int curr_arg_bytes = 0;

    // put the ID into the message
    ((uint16_t*)msgbuf)[0] = id;
    msgbuf_write = msgbuf + 2;
    // handle the arg list
    va_start(argv, id);
    while(*currptr != '\0') {
        if(*currptr == '%') {
            endptr = currptr;
            curr_arg_bytes = yield_fmt_arg_bytes(currptr, &endptr);
            if(endptr == currptr) {
                // no bytes processed, error here
            }
            currptr = endptr + 1;
            // handle the argument
            switch(curr_arg_bytes) {
                case 0:break;

                case 1:
                    // va_arg promotes char to int
                    currarg = (void*)va_arg(argv, int);
                break;

                case 2:
                    // va_arg promotes short to int
                    currarg = (void*)va_arg(argv, int);
                break;

                case 4:
                    currarg = (void*)va_arg(argv, uint32_t);
                break;

                case 8:
                    currarg = (void*)va_arg(argv, uint64_t);
                break;
                
                case -1:
                    // call strlen on ptr
                    currarg = (void*)va_arg(argv, void *);
                    curr_arg_bytes = strlen(currarg);
                break;

                case -2:
                    // NOTE: we do not support this right now.
                    // call wcslen on ptr
                    currarg = (void*)va_arg(argv, void *);
                    /*curr_arg_bytes = wcslen(currarg);*/
                break;

                default: break;
            }
            memcpy(msgbuf_write, currarg, curr_arg_bytes);
            msgbuf_write += curr_arg_bytes;
            if(msgbuf_write - msgbuf > 255) {
                // too big for a TXPC_FRAME_TYPE_MSG, will need STREAM
                // messages for this debug log
            }
        }
        else {
            currptr++;
        }
    }
    va_end(argv);
    txpc_send_msg(stdout, 1, 1, msgbuf, msgbuf_write - msgbuf);
}

char test_fn(int n) {
    static char x = 'a';
    if(n == 0) {
        x = 'b';
    }
    else if(n == 1){
        x = 'c';
    }

    return x;
}


int main(void) {
    ftab_init();
    SystemCoreClockUpdate();
    uart0_conf.input_clock_rate = SystemCoreClock;
    txpc_ready = false;
    stdout = uart_init(uart0_conf); 
    txpc_msg_t msg = {0};
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORTB->PCR[21] |= PORT_PCR_MUX(1);
    PORTB->PCR[22] |= PORT_PCR_MUX(1);
    GPIOB->PDDR |= (1 << 21);
    GPIOB->PDDR |= (1 << 22);
    GPIOB->PSOR |= (1 << 21);
    GPIOB->PCOR |= (1 << 22);

    // 0xAD is our phony "ready to transmit" message
    /*
     *char buf[255];
     *while(1) {
     *    read(stdout, buf, 255);
     *    write(stdout, buf, strlen(buf));
     *}
     */
    char x = test_fn(10);
    write(stdout, &x, 1);
    x = test_fn(0);
    write(stdout, &x, 1);
    x = test_fn(1);
    write(stdout, &x, 1);
    while(msg.hdr.type != 0xAD) {
        int bytes = 0;
        while(bytes < sizeof(txpc_msg_t)) {
            // yikes on reading from stdout
            bytes += read(stdout, (char*)&msg + bytes, sizeof(txpc_msg_t) - bytes);
        }
    }
    GPIOB->PCOR |= (1 << 21);
    GPIOB->PSOR |= (1 << 22);
    os_log("hello %s\n", "world");
    /*txpc_send_msg(stdout, 1, 1, "hello world\r\n", sizeof("hello world\r\n"));*/
    for(;;) {
        os_log("this is a %s message, just for %cun\n", "world", 'f');
    }
    return 0;
}
