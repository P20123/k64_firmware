#include <stdint.h>
#include <stdbool.h>
#include <environment.h>
#include <drivers/kinetis/uart.h>
#include <nongnu/unistd.h>
#include <MK64F12.h>

void init_pit(int period_us) {
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    PIT->MCR |= PIT_MCR_MDIS_MASK;
    PIT->MCR |= PIT_MCR_FRZ_MASK;

    PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK;
    // input is 60 MHz
    PIT->CHANNEL[0].LDVAL = period_us*((SystemCoreClock)/1000000);

    PIT->MCR &= ~PIT_MCR_MDIS_MASK;
    NVIC_EnableIRQ(PIT0_IRQn);
    NVIC->IP[PIT0_IRQn] = 0;
}


volatile int color;
void PIT0_IRQHandler(void) {
    asm("cpsid i");
    PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
    // delay to simulate nasty ISR
    color ^= 1;
    for(volatile int i = 0; i < 1000; i++);
    asm("cpsie i");
}

int main(void) {
    int bytes = 0;
    int stdout;
    char buf[255];
    uart0_conf.input_clock_rate = SystemCoreClock;
    stdout = uart_init(uart0_conf);
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORTB->PCR[21] |= PORT_PCR_MUX(1);
    PORTB->PCR[22] |= PORT_PCR_MUX(1);
    GPIOB->PDDR |= (1 << 21);
    GPIOB->PDDR |= (1 << 22);
    GPIOB->PSOR |= (1 << 21);
    GPIOB->PCOR |= (1 << 22);
    color = 0;
    // 504 kHz, long-running interrupt (this is exactly tuned to fill the CPU
    // at 20.97 MHz)
    init_pit(504);

    while(1) {
        bytes = read(stdout, buf, 255);
        if(bytes > 0) {
            while((bytes -= write(stdout, buf, bytes)));
            memset(buf, 0, 255);
            bytes = 0;
        }
        if(color) {
            GPIOB->PSOR |= (1 << 21);
            GPIOB->PCOR |= (1 << 22);
        }
        else {
            GPIOB->PCOR |= (1 << 21);
            GPIOB->PSOR |= (1 << 22);
        }
    }
    /*while((bytes = write(stdout, "hello world\r\n", 13)) < 13);*/
    return 0;
}
