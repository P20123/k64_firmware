#include <stdint.h>
#include <kernel/process.h>
#include "blink_demo.h"
#include <MK64F12.h>
#include <multitasking.h>
#define abs(x) (((x) < 0) ? -(x):(x))
pcb_t blink_demo_app;

uint32_t blink_demo_stack[BLINK_DEMO_STACK_WORDS];


int blink_demo_main(void) {
    SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

    PORTB->PCR[21] |= PORT_PCR_MUX(1);
    PORTB->PCR[22] |= PORT_PCR_MUX(1);
    GPIOB->PDDR |= (1 << 21);
    GPIOB->PDDR |= (1 << 22);
    GPIOB->PSOR |= (1 << 21);
    GPIOB->PCOR |= (1 << 22);

    // LPO 1kHz input for lptmr
    LPTMR0->PSR |= (1 << LPTMR_PSR_PCS_SHIFT);
    // bypass prescaler
    LPTMR0->PSR |= (1 << LPTMR_PSR_PBYP_SHIFT);

    // free-running mode
    LPTMR0->CSR |= (1 << LPTMR_CSR_TFC_SHIFT);
    // enable lptmr as a 16-second period counter without interrupts.
    LPTMR0->CSR |= (1 << LPTMR_CSR_TEN_SHIFT);

    // page 1104 of the ref. manual.  write before read... why?
    LPTMR0->CNR = 0;
    uint16_t time = LPTMR0->CNR;
    uint16_t prev_time = time;
    int color = 0;

    for(;;) {
        while(abs(time - prev_time) < 1000) {
            LPTMR0->CNR = 0;
            time = LPTMR0->CNR;
            yield();
        }
        prev_time = time;
        if(color) {
            GPIOB->PSOR |= (1 << 21);
            GPIOB->PCOR |= (1 << 22);
        }
        else {
            GPIOB->PCOR |= (1 << 21);
            GPIOB->PSOR |= (1 << 22);
        }
        color ^= 1;
    }
    return 0;
}
