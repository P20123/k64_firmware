#include <environment.h>
#include <nongnu/unistd.h>
#include <stdint.h>
#include <multitasking.h>
#include <tinyxpc/tinyxpc.h>
#include <tinyxpc/xpc_relay.h>
#include <programs/xpc_relay_event_loop.h>
#include <programs/xpc_spammer.h>
#include <drivers/devices/status_leds.h>

pcb_t xpc_spammer_app;
uint32_t xpc_spammer_stack[XPC_SPAMMER_STACK_SIZE];

int xpc_spammer_main(void) {
    xpc_yield_until_ready();
    for(;;) {
        int status = TXPC_STATUS_BAD_STATE;
        while(status != TXPC_STATUS_DONE) {
            RED_LED_ON();
            status = xpc_send_msg(comms_relay, 1, 1, "hello world\r\n", 13);
            yield();
        }
        RED_LED_OFF();
    }
    return 0;
}
