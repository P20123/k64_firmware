#include <stdint.h>
#include <drivers/kinetis/ftm.h>


typedef struct {
    uint8_t ftmnum;

} ftm_config_t;

typedef struct {
    uint8_t channel;
    uint8_t duty_cycle;
    uint32_t port_clock_gate_base;
    uint32_t port_clock_gate_mask;
    uint32_t pcr;

} ftm_ch_config_t;


int ftm_init() {
    /* enable clock to the FTM module */

    /* disable write protection */

    /* initialize the count to zero */

    /* set the counter initial value to zero */

    /* ftm mod */


}
