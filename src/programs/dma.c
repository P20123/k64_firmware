#include <stdint.h>
#include <nongnu/unistd.h>
#include <kernel/kernel_ftab.h>
#include <environment.h>
#include <drivers/uart.h>
/*
 *#include <alibc/extensions/array.h>
 *#include <alibc/extensions/array_iterator.h>
 */
#include <string.h>

#define DBG_LOG(x) write(uart0_fd, x, strlen(x));

int sdhc_send_cmd(int cmd_idx, uint32_t cmd_arg) {
    uint32_t cmd = (cmd_idx & 0x3f) << SDHC_XFERTYP_CMDINX_SHIFT;
    // CMDTYP, DPSEL, CICEN, CCCEN, RSTTYP, DTDSEL need to be set here?
    // it has to do with cmd_idx apparently.
}

static char test_string[] = "not a very long string, but it'll do for now.\r\n";


int init_dma(char *src_addr) {
    // route clocks to DMA an DMAMUX
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
    SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;

    // configure dma

    // round robin enabled
    DMA0->CR |= DMA_CR_ERCA_MASK;

    // configure priorities

    // enable error interrupt
    DMA0->EEI |= DMA_INT_INT1_MASK;

    // zero out the TCD
    memset(DMA0->TCD + 1, 0, 32);

    // configure transfer control descriptors
    DMA0->TCD[1].SADDR = src_addr;
    // 1-byte incr. for each source addr.
    DMA0->TCD[1].SOFF = 1;
    // do not change source addr. ptr. on end of last major iter.
    DMA0->TCD[1].SLAST = 0;
    DMA0->TCD[1].ATTR = 0;
    DMA0->TCD[1].NBYTES_MLNO = 8;

    DMA0->TCD[1].DADDR = &(UART0->D);
    // 0 byte incr. for each destination write
    DMA0->TCD[1].DOFF = 0;
    // no scatter/gather, do not change dest. addr. ptr. on end of last major iter.
    DMA0->TCD[1].DLAST_SGA = 0;

    // Enable an interrupt when the major loop xfer is done
    DMA0->TCD[1].CSR = DMA_CSR_INTMAJOR_MASK;
    // set the number of major loops to complete.
    DMA0->TCD[1].BITER_ELINKNO = 1;
    DMA0->TCD[1].CITER_ELINKNO = 1;

    // enable requests from the DMAMUX'd peripherals
    DMA0->ERQ |= DMA_ERQ_ERQ1_MASK;


    // select appropriate source peripherals
    /*
     *DMAMUX->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK | 2; // uart 0 rx
     */
    DMAMUX->CHCFG[1] |= DMAMUX_CHCFG_ENBL_MASK | 3; // uart 0 tx
}

// replace this with dma chaining, have another channel fix the value in
// NBYTES, I think.
void DMA1_IRQHandler(void) {
    // clear interrupt req
    DMA0->CINT |= 1;
    // clear done flag
    DMA0->CDNE |= 1;
    // reset the number of bytes to transfer (repeat it)
    /*
     *DMA0->TCD[1].SLAST = -1; //-sizeof(test_string);
     *DMA0->TCD[1].NBYTES_MLNO = 1; //sizeof(test_string);
     */
}


void DMA_Error_IRQHandler(void) {
}

void UART0_ERR_IRQHandler(void) {
}

int main(void) {
    ftab_init();
    SystemCoreClockUpdate();
    uart0_conf.input_clock_rate = SystemCoreClock;

    char buf[255];
    memcpy(buf, test_string, sizeof(test_string));

    int uart0_fd = uart_init(uart0_conf);
    init_dma(buf);
    NVIC_EnableIRQ(DMA1_IRQn);
    NVIC_EnableIRQ(DMA_Error_IRQn);
    NVIC_EnableIRQ(UART0_ERR_IRQn);
    asm("cpsie i");
    /*
     *while(1)
     */
        /*
         *DMA0->TCD[1].CSR |= 1;
         */
    /*
     *DBG_LOG("UART DMA");
     */

    /*
     *DBG_LOG("Initializing SDHC");
     *SIM->SCGC3 |= SIM_SCGC3_SDHC_MASK;
     */

    for(;;);
    return 0;
}
