#include <MK64F12.h>
#include <stdint.h>
#include <stdbool.h>
#include <drivers/kinetis/ftm.h>

/*************
 * VARIABLES
 *************/
static FTM_Type *ftm_bases[] = FTM_BASE_PTRS;

/*************
 * FUNCTIONS
 *************/
int ftm_init(ftm_config_t config, uint32_t clk_f_hz) {
    uint32_t count_clk;
    uint16_t modval;

    /* get the base pointer */
    FTM_Type *base = ftm_bases[config.ftm_num];

    /* enable clock to the FTM module */
    *(config.ftm_clock_gate_base) |= config.ftm_clock_gate_mask;

    /* disable write protection */
    base->MODE |= FTM_MODE_WPDIS_MASK;

    /* initialize the count to zero */
    base->CNT = 0;

    /* set the counter initial value */
    base->CNTIN = config.cnt_init;

    /* set the ftm mod value to ensure the right PWM freq */
    count_clk = clk_f_hz >> config.prescalar;
    modval = (count_clk / config.freq) + (config.cnt_init - 1);
    base->MOD = modval;

    /* prescalar and clock source */
    base->SC = FTM_SC_PS(config.prescalar) | FTM_SC_CLKS(config.clksrc);

    return 0;
}

int ftm_ch_epwm_init(ftm_ch_config_t ch_config) {
    /* get the base pointer */
    FTM_Type *base = ftm_bases[ch_config.ftm_num];

    /* enable the channel port clocks */
    *(ch_config.port_clock_gate_base) |= ch_config.port_clock_gate_mask;

    /* set the channel port mux and enable high drive strength */
    *(ch_config.pcr) = PORT_PCR_MUX(ch_config.alt) | PORT_PCR_DSE_MASK;

    /* configure for edge aligned, high true pulse PWM */
    base->CONTROLS[ch_config.channel].CnSC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    base->CONTROLS[ch_config.channel].CnSC &= ~FTM_CnSC_ELSA_MASK;

    return 0;
}

int ftm_ch_CnV_set(uint8_t ftm_num, uint8_t ch_num, uint16_t value) {
    /* get the base pointer */
    FTM_Type *base = ftm_bases[ftm_num];

    /* set the CnV */
    base->CONTROLS[ch_num].CnV = value;
    
    return 0;
}

int ftm_ch_duty_set(uint8_t ftm_num, uint8_t ch_num, uint8_t duty) {
    /* get the base pointer */
    FTM_Type *base = ftm_bases[ftm_num];

    /* set the CnV */
    base->CONTROLS[ch_num].CnV = (base->MOD / 100) * duty;

    return 0;
}
