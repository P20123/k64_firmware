#include <MK64F12.h>
#include <stdint.h>
#include <stdbool.h>
#include <drivers/kinetis/ftm.h>


typedef struct {
    uint8_t ftm_num;
    uint32_t ftm_clock_gate_base;
    uint32_t ftm_clock_gate_mask;
    uint32_t modvalue;
    int prescalar;                  /* 0-7 Divide by 2^PS */
    int clksrc;                     /* 0-3 No CLK, SysCLK, FFCLK, EXCLK */
} ftm_config_t;

typedef struct {
    uint8_t ftm_num;
    uint8_t channel;
    uint32_t port_clock_gate_base;
    uint32_t port_clock_gate_mask;
    uint32_t pcr;
    uint32_t alt;
} ftm_ch_config_t;

static FTM_Type *ftm_bases[] = FTM_BASE_PTRS;

int ftm_init(ftm_config_t *config) {
    /* get the base pointer */
    FTM_Type *base = ftm_bases[config.ftm_num];

    /* enable clock to the FTM module */
    *(config.ftm_clk_gate_base) |= config.ftm_clock_gate_mask;

    /* disable write protection */
    base->MODE |= FTM_MODE_WPDIS_MASK;

    /* initialize the count to zero */
    base->CNT = 0;

    /* set the counter initial value to zero */
    base->CNTIN = 0;

    /* ftm mod */
    base->MOD = config.modvalue;

    /* prescalar */
    base->SC = FTM_SC_PS(config.prescalar) | FTM_SC_CLKS(config.clksrc);
}

int ch_init(ftm_ch_config_t *ch_config) {
    /* get the base pointer */
    FTM_Type *base = ftm_bases[ch_config.ftm_num];

    /* enable the channel port clocks */
    *(ch_config.port_clock_gate_base) |= ch_config.port_clock_gate_mask;

    /* set the channel port mux */
    *(ch_config.pcr) = PORT_PCR_MUX(ch_config.alt);

    /* configure for edge aligned, high true pulse PWM */
    base->CONTROLS[ch_config.channel].CnSC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    base->CONTROLS[ch_config.channel].CnSC &= ~FTM_CnSC_ELSA_MASK;
}
