#include <stdint.h>
#include <nongnu/unistd.h>
#include <kernel/kernel_ftab.h>
#include <environment.h>
#include <stdbool.h>
#include <drivers/uart.h>
#include <drivers/i2c.h>
#include <string.h>
#include <MK64F12.h>

#define PRINT(x) write(uart0_fd, x, strlen(x));

int main(void) {
    ftab_init();
    SystemCoreClockUpdate();
    uart0_conf.input_clock_rate = SystemCoreClock;
    int uart0_fd = uart_init(uart0_conf);

    const uint8_t i2caddr = 0x68u;
    const uint8_t regaddr = 0x75u;

    uint16_t seq[] = {
        (i2caddr << 1),
        regaddr,
        I2C_RESTART,
        (i2caddr << 1) | 1,
        I2C_READ,
        I2C_READ
    };
    uint8_t data = 0;

    i2c_config_t config = {
        .i2c_num = 0,
        .baud = 9600,
        .stophold = false,
        .interrupt = true,
        .filter_clocks = 0
    };

    // initialize the i2c module
    init_m_i2c(config, 120000000);

    // enable clocks to the port
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    // configure the pins
    PORTE->PCR[24] |= PORT_PCR_MUX(5);
    PORTE->PCR[25] |= PORT_PCR_MUX(5);

    // send a sequence :-)
    i2c_send_seq(0, seq, 6, &data);

    for(;;);
    return 0;
}
