#include <MK64F12.h>

/**********
 * MACROS
 **********/
#define GREEN_LED_ON()  GPIOC->PSOR |= (1 << 17)
#define GREEN_LED_OFF() GPIOC->PCOR |= (1 << 17)
#define RED_LED_ON()    GPIOC->PSOR |= (1 << 16)
#define RED_LED_OFF()   GPIOC->PCOR |= (1 << 16)

/**************
 * PROTOTYPES
 **************/

/**
 * Enables the Status LEDs located on PTC16, PTC17
 * @return 0
 */
int status_leds_init();
