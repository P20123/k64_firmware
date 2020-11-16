#include <MK64F12.h>
#include <drivers/devices/status_leds.h>

/*************
 * FUNCTIONS
 *************/
int status_leds_init() {
    /* enable clock for PORTC */
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

    /* set the port mux to be alt 1 */
    PORTC->PCR[16] |= PORT_PCR_MUX(1);
    PORTC->PCR[17] |= PORT_PCR_MUX(1);

    /* set the GPIO to be outputs */
    GPIOC->PDDR |= (1 << 16);
    GPIOC->PDDR |= (1 << 17);

    return 0;
}

