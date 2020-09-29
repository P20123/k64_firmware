#include <stdint.h>
#include <nongnu/unistd.h>
#include <kernel/kernel_ftab.h>
#include <environment.h>
#include <drivers/uart.h>
#include <string.h>
#include <MK64F12.h>
#include <stdbool.h>

#define DBG_LOG(x) write(uart0_fd, x, strlen(x));

enum {
    SDHC_ADMA2_DESC_VALID = (1 << 0),
    SDHC_ADMA2_DESC_END = (1 << 1),
    SDHC_ADMA2_DESC_INT = (1 << 2),
    SDHC_ADMA2_DESC_ACT1 = (1 << 4),
    SDHC_ADMA2_DESC_ACT2 = (1 << 5),
    SDHC_ADMA2_NOOP = SDHC_ADMA2_DESC_VALID,
    SDHC_ADMA2_XFER = SDHC_ADMA2_DESC_VALID | SDHC_ADMA2_DESC_ACT2,
    SDHC_ADMA2_LINK = SDHC_ADMA2_XFER | SDHC_ADMA2_DESC_ACT1
};

typedef struct {
    struct {
        uint16_t attrs;
        uint16_t length;
    } ctrl; 
    uint32_t *address;
} sdhc_adma2_desc_t;


static bool cc_flag, ctoe_flag;

static int uart0_fd;

// read buffer location for one sdhc block.
// 512 bytes, aligned to 4-byte words for ADMA2
static uint32_t sdhc_rd_buf[128];

// transfer control descriptor for SDHC ADMA2
static sdhc_adma2_desc_t rd_descs[] = {
    // perform the actual transfer into / out of the buffer
    {.ctrl = {.attrs = SDHC_ADMA2_XFER | SDHC_ADMA2_DESC_END, .length = 512}, .address = sdhc_rd_buf},
};

void init_sdhc() {
    // setup clocks
    SIM->SCGC3 |= SIM_SCGC3_SDHC_MASK;

    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    // connect SDHC pins to PORTE maps that bring them out to the onboard
    // SD card
    PORTE->PCR[0] |= (4 << PORT_PCR_MUX_SHIFT); // DAT 1
    PORTE->PCR[1] |= (4 << PORT_PCR_MUX_SHIFT); // DAT 0
    PORTE->PCR[2] |= (4 << PORT_PCR_MUX_SHIFT); // SDCLK
    PORTE->PCR[3] |= (4 << PORT_PCR_MUX_SHIFT); // CMD
    PORTE->PCR[4] |= (4 << PORT_PCR_MUX_SHIFT); // DAT 3
    PORTE->PCR[5] |= (4 << PORT_PCR_MUX_SHIFT); // DAT 2
    // not sure if this is necessary?
    PORTE->PCR[6] = (1 << PORT_PCR_MUX_SHIFT); // Digital IO for card sw.

    // reset sdhc
    SDHC->SYSCTL |= SDHC_SYSCTL_RSTA_MASK | SDHC_SYSCTL_RSTC_MASK | SDHC_SYSCTL_RSTD_MASK;

    // set watermark levels to zero (disabling them)
    SDHC->WML &= ~(SDHC_WML_RDWML_MASK | SDHC_WML_WRWML_MASK);

    // turn off DAT3 card detection
    SDHC->PROCTL &= ~(SDHC_PROCTL_D3CD_MASK);

    // disable all automatic clock gating
    SDHC->SYSCTL |= SDHC_SYSCTL_PEREN_MASK | SDHC_SYSCTL_HCKEN_MASK | SDHC_SYSCTL_IPGEN_MASK;

    // enable interrupt generation
    SDHC->IRQSIGEN = 0xffffffff;

    // enable event generation
    SDHC->IRQSTATEN = 0xffffffff;
    SDHC->IRQSTATEN &= ~(SDHC_IRQSTATEN_CINSEN_MASK);
    SDHC->IRQSTATEN &= ~(SDHC_IRQSTATEN_CRMSEN_MASK);

    // allow card insert event
    /*
     *SDHC->IRQSTATEN |= (SDHC_IRQSTATEN_CINSEN_MASK);
     */

    // generate an interrupt for card insertion and removal event
    /*
     *SDHC->IRQSIGEN |= (SDHC_IRQSIGEN_CINSIEN_MASK | SDHC_IRQSIGEN_CRMIEN_MASK);
     */

    // Enable interrupts
    NVIC_EnableIRQ(SDHC_IRQn);

}

void sdhc_initcard() {
    SDHC->SYSCTL |= SDHC_SYSCTL_INITA_MASK;
    // wait for SDHC to unset this flag, indicating 80 clocks have passed
    while(SDHC->SYSCTL & SDHC_SYSCTL_INITA_MASK);
/*
 *
 *    // sdhc driven from core clock by default, which is 120 MHz,
 *    // so this is 375 kHz, close enough to 400 kHz "boot clk" for SD spec
 *    SDHC->SYSCTL |= (0x20 << SDHC_SYSCTL_SDCLKFS_SHIFT) | (5 << SDHC_SYSCTL_DVS_SHIFT);
 *    // wait for command inhibit status to go away
 *    while(SDHC-> PRSSTAT & (SDHC_PRSSTAT_CIHB_MASK | SDHC_PRSSTAT_CDIHB_MASK));
 *    // start 80-clock initialization
 *    SDHC->SYSCTL |= SDHC_SYSCTL_INITA_MASK;
 */
}

#define STRINGIFY(x) #x
void SDHC_IRQHandler(void) {
    if(SDHC->IRQSTAT & SDHC_IRQSTAT_DMAE_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_DMAE_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_DMAE_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_AC12E_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_AC12E_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_AC12E_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_DEBE_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_DEBE_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_DCE_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_DCE_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_DCE_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_DTOE_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_DTOE_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_DTOE_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_CIE_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_CIE_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_CIE_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_CEBE_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_CEBE_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_CEBE_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_CCE_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_CCE_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_CCE_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_CTOE_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_CTOE_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_CTOE_MASK)" interrupt\r\n");
        ctoe_flag = true;
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_CINT_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_CINT_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_CINT_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_CRM_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_CRM_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_CRM_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_CINS_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_CINS_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_CINS_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_BRR_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_BRR_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_BRR_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_BWR_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_BWR_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_BWR_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_BGE_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_BGE_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_BGE_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_TC_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_TC_MASK;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_TC_MASK)" interrupt\r\n");
    }

    if(SDHC->IRQSTAT & SDHC_IRQSTAT_CC_MASK) {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_CC_MASK;
        cc_flag = true;
        DBG_LOG("got "STRINGIFY(SDHC_IRQSTAT_CC_MASK)" interrupt\r\n");
    }
/*
 *
 *    // card insertion interrupt
 *    // this seems to always happen, regardless of card present status?
 *    if(SDHC->IRQSTAT & SDHC_IRQSTAT_CINS_MASK) {
 *        SDHC->IRQSTAT |= SDHC_IRQSTAT_CINS_MASK;
 *        // disable and clear card insert event
 *        SDHC->IRQSTATEN &= ~(SDHC_IRQSTATEN_CINSEN_MASK);
 *        // enable card removal event
 *        SDHC->IRQSTATEN |= SDHC_IRQSTATEN_CRMSEN_MASK;
 *        [>DBG_LOG("got card insertion event\r\n");<]
 *    }
 *    // card removal interrupt
 *    else if(SDHC->IRQSTAT & SDHC_IRQSTAT_CRM_MASK) {
 *        // disable and clear card removal event
 *        SDHC->IRQSTATEN &= ~(SDHC_IRQSTATEN_CRMSEN_MASK);
 *        // enable card insertion event
 *        SDHC->IRQSTATEN |= SDHC_IRQSTATEN_CINSEN_MASK;
 *        [>DBG_LOG("got card removal event\r\n");<]
 *    }
 *    else {
 *        [>DBG_LOG("got an unhandled sdhc interrupt\r\n");<]
 *    }
 */
}

void sdhc_configure_xfer(int blockcount, uint32_t arg, int index, int flags) {
    /*SDHC->BLKATTR = (blockcount << SDHC_BLKATTR_BLKCNT_SHIFT) | 512;*/
    SDHC->CMDARG = arg;
    SDHC->XFERTYP = (index << SDHC_XFERTYP_CMDINX_SHIFT) |
        (flags & (SDHC_XFERTYP_DMAEN_MASK | SDHC_XFERTYP_MSBSEL_MASK
                | SDHC_XFERTYP_DPSEL_MASK | SDHC_XFERTYP_CMDTYP_MASK
                | SDHC_XFERTYP_BCEN_MASK | SDHC_XFERTYP_CICEN_MASK
                | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP_MASK
                | SDHC_XFERTYP_DTDSEL_MASK | SDHC_XFERTYP_AC12EN_MASK));
    // wait for command inhibit status to go away
    while(SDHC->PRSSTAT & (SDHC_PRSSTAT_CIHB_MASK | SDHC_PRSSTAT_CDIHB_MASK));
}

int main(void) {
    ftab_init();
    SystemCoreClockUpdate();
    uart0_conf.input_clock_rate = SystemCoreClock;

    uart0_fd = uart_init(uart0_conf);
    DBG_LOG("UART0 config complete\r\n");
    init_sdhc();
    DBG_LOG("SDHC config complete\r\n");
    sdhc_initcard();
    DBG_LOG("card init complete\r\n");
    // do a cmd0 to go idle
    sdhc_configure_xfer(0, 0, 0, 0);
    // wait for command complete
    while(!cc_flag && !ctoe_flag);
    cc_flag = false;
    ctoe_flag = false;
    DBG_LOG("cmd0 done\r\n");

    // try cmd8 for sd(hc) init
    sdhc_configure_xfer(0, 0x1AA, 8, (2 << SDHC_XFERTYP_RSPTYP_SHIFT));
    while(!cc_flag);
    cc_flag = false;
    // for some reason the response to cmd8 is in the arg...
    if(SDHC->CMDARG == 0x1AA) {
        DBG_LOG("sd(hc) detected\r\n");
        // set cmd55 to set application-specific commands...
        sdhc_configure_xfer(0, 0, 55, (2 << SDHC_XFERTYP_RSPTYP_SHIFT));
        while(!cc_flag);
        cc_flag = false;
        if(SDHC->CMDRSP[0] == 0x05) goto sdsc;

        // set APP_OP_COND to get sd operating conditions
        sdhc_configure_xfer(0, 0x40000000, 41, (2 << SDHC_XFERTYP_RSPTYP_SHIFT));
        while(!cc_flag);
        cc_flag = false;
        // operating conds in cmdard...?
        uint32_t op_cond = SDHC->CMDRSP[0];
    }
    else {
sdsc:
        DBG_LOG("sdsc detected\r\n");
        // send CMD1
        sdhc_configure_xfer(0, 0, 1, (2 << SDHC_XFERTYP_RSPTYP_SHIFT) | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_CICEN_MASK);
        while(!cc_flag && !ctoe_flag);
        /*
         *if(ctoe_flag) {
         *    sdhc_configure_xfer(0, 0, 1, (1 << SDHC_XFERTYP_RSPTYP_SHIFT));
         *}
         */
        // only one retry...
        /*while(!cc_flag);*/
        ctoe_flag = false;
        cc_flag = false;
    }
    // set block length to 512 (why is this even needed?)
    /*
     *sdhc_configure_xfer(0, 512, 16, (2 << SDHC_XFERTYP_RSPTYP_SHIFT) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK);
     *while(!cc_flag);
     *cc_flag = false;
     */
    DBG_LOG("init done\r\n");


    // read the first block (512 bytes, protective mbr)
    // select ADMA2
    SDHC->PROCTL |= (2 << SDHC_PROCTL_DMAS_SHIFT);
    // disable simple dma
    SDHC->DSADDR = 0;
    // set descriptor table location
    SDHC->ADSADDR = (uint32_t)rd_descs;
    sdhc_configure_xfer(1, 0, 17, SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_AC12EN_MASK | (1 << SDHC_XFERTYP_RSPTYP_SHIFT) | SDHC_XFERTYP_DMAEN_MASK | SDHC_XFERTYP_DPSEL_MASK | SDHC_XFERTYP_DTDSEL_MASK /* read */ | SDHC_XFERTYP_BCEN_MASK);
    while(!cc_flag);
    cc_flag = false;
    ctoe_flag = false;
    DBG_LOG("tests done\r\n");
    for(;;);
    return 0;
}
