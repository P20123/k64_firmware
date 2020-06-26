/*
 * File:        uart.c
 * Purpose:     Provide UART routines for serial IO
 *
 * Notes:		
 *
 */
#include <MK64F12.h>
#include <drivers/uart.h>
#include <kernel/kernel_ftab.h>

// this file targets only MK64F12, so we can statically alloc 6 uart_contexts.
char txq_headers[6][18];
char rxq_headers[6][18];
char txqs[6][255];
char rxqs[6][255];
// for the interrupt handler - each one gets accessed by the isr when an
// appropriate interrupt triggers for that UART.
uart_context contexts[6] = {
    {UART0, txq_headers[0], rxq_headers[0]},
    {UART1, txq_headers[1], rxq_headers[1]},
    {UART2, txq_headers[2], rxq_headers[2]},
    {UART3, txq_headers[3], rxq_headers[3]},
    {UART4, txq_headers[4], rxq_headers[4]},
    {UART5, txq_headers[5], rxq_headers[5]}
};

const void * uart_contexts_address = &contexts[0];

// prototypes
unsigned int uart_write(uart_context *context, void *buf, unsigned int bytes);
unsigned int uart_read(uart_context *context, void *buf, unsigned int bytes);
unsigned int uart_close(uart_context *context);

// uart assembly routines
char getchar(char *rxq);

int uart_init(uart_config conf) {
    //define variables for baud rate and baud rate fine adjust
    uint16_t ubd, brfa;

    //Enable clock for UART
    *(conf.uart_clock_gate_base) |= conf.uart_clock_gate_mask;
    // Enable PORTA clock
    *(conf.port_clock_gate_base) |= conf.port_clock_gate_mask;

     

    // Configure the output registers
    // FIXME move this to a separate function w/ separate configs.
    *(conf.tx_pcr) = conf.tx_alt << PORT_PCR_MUX_SHIFT;
    *(conf.rx_pcr) = conf.rx_alt << PORT_PCR_MUX_SHIFT;
    if(conf.rt_alt < 255) {
        *(conf.rt_pcr) = conf.rt_alt << PORT_PCR_MUX_SHIFT;
    }

    if(conf.ct_alt < 255) {
        *(conf.ct_pcr) = conf.ct_alt << PORT_PCR_MUX_SHIFT;
    }


    /*Configure the UART for establishing serial communication*/

    //Disable transmitter and receiver until proper settings are chosen for the UART module
    conf.uart_base->C2 &= ~UART_C2_RE_MASK;
    conf.uart_base->C2 &= ~UART_C2_TE_MASK;


    //Select default transmission/reception settings for serial communication of UART by clearing the control register 1
    conf.uart_base->C1 = 0;


    //UART Baud rate is calculated by: baud rate = UART module clock / (16 × (SBR[12:0] + BRFD))
    //13 bits of SBR are shared by the 8 bits of UART3_BDL and the lower 5 bits of UART3_BDH 
    //BRFD is dependent on BRFA, refer Table 52-234 in K64 reference manual
    //BRFA is defined by the lower 4 bits of control register, UART0_C4 

    //calculate baud rate settings: ubd = UART module clock/16* baud rate
    ubd = (uint16_t)((conf.input_clock_rate)/(conf.baud * 16));

    //clear SBR bits of BDH
    conf.uart_base->BDH &= ~UART_BDH_SBR_MASK;
      

    //distribute this ubd in BDH and BDL
    conf.uart_base->BDH |= (UART_BDH_SBR_MASK & (ubd >> 8));
    conf.uart_base->BDL = ubd;


    //BRFD = (1/32)*BRFA 
    //make the baud rate closer to the desired value by using BRFA
    brfa = (((conf.input_clock_rate*32)/(conf.baud * 16)) - (ubd * 32));

    //write the value of brfa in UART0_C4
    conf.uart_base->C4 |= (UART_C4_BRFA_MASK & brfa);
        
    // insert this config into the ftab and return its id
    // aaaaa this is gross, but it IS a hal after all.
    // FIXME YOU IDIOT, JUST PASS THE INDEX IN THE CONFIG STRUCT
    int which = (conf.uart_base == UART0)? 0:
                (conf.uart_base == UART1)? 1:
                (conf.uart_base == UART2)? 2:
                (conf.uart_base == UART3)? 3:
                (conf.uart_base == UART4)? 4:5;

    init_queue(&txqs[which], &txq_headers[which], 255);
    init_queue(&rxqs[which], &rxq_headers[which], 255);
    //Enable transmitter and receiver of UART (and interrupts)
    if(conf.configure_interrupts > 0) {
        asm("cpsid i");
        // set RWFIFO[RXWATER] such that interrupts are actually generated
        conf.uart_base->RWFIFO = 1;
        // set TWFIFO[TXWATER] such that interrupts are actually generated
        /*
         *conf.uart_base->TWFIFO = 1;
         */

        // turn on rx, tx, rx interrupt, tx interrupt
        conf.uart_base->C2 |= UART_C2_RE_MASK | UART_C2_TE_MASK | UART_C2_RIE_MASK | UART_C2_TIE_MASK;

        /*
         *NVIC_EnableIRQ(conf.irqn);
         *NVIC->IP[conf.irqn] = conf.priority;
         */
        // generate DMA requests instead of interrupts
        conf.uart_base->C5 |= UART_C5_TDMAS_MASK;
        // enable tx fifo
        conf.uart_base->PFIFO |= UART_PFIFO_TXFE_MASK;
        asm("cpsie i");
    }
    else {
        conf.uart_base->C2 |= UART_C2_RE_MASK | UART_C2_TE_MASK;
    }

            
    return ftab_open((ftab_entry_t){
            .context = (void*)&contexts[which],
            .write = uart_write,
            .read = uart_read,
            .close = uart_close,
        }
    );
}

unsigned int uart_write(uart_context *context, void *buf, unsigned int bytes) {
    int buf_init = buf;
    while(*((char*)buf++) && bytes >= 0) {
        putchar(*(((char*)buf)-1), context->txq, context->uart_base);
        bytes--;
    }
    return buf - buf_init;
}

unsigned int uart_read(uart_context *context, void *buf, unsigned int bytes) {
    int bytes_read = 0;
    if(bytes == 0) {
        bytes_read = available(context->rxq);
    }
    else {
        while(bytes_read < bytes && available(context->rxq) > 0) {
            ((char*)buf)[bytes_read] = getchar(context->rxq);
            bytes_read++;
        }
        ((char*)buf)[bytes_read] = 0;
    }
    return bytes_read;

    // feof not supported w/ current getchar, since it loops waiting for input.
    // we should fix that so it is non-blocking...
    /*
     *int bytes_read = 0;
     *if(available(context->rxq) >= bytes) {
     *    for(int i = 0; i < bytes; i++) {
     *        ((char*)buf)[i] = getchar(context->rxq);
     *    }
     *    bytes_read = bytes;
     *}
     *return bytes_read;
     */
}


// FIXME deinit uart, turn off interrupts? Data not available in context atm...
unsigned int uart_close(uart_context *context) {}
