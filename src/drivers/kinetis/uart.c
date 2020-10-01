#include <MK64F12.h>
#include <drivers/uart.h>
#include <kernel/kernel_ftab.h>
#include <queue/queue.h>
#include <stdbool.h>


/**
 * UNIX-like File IO
 *     state and prototypes
 */
/**
 * Function implementing write system call for UART
 * @param uart_context uart context to write to. Configured by uart_init.
 * @param buf buffer to write
 * @param bytes number of bytes to write
 * @return number of bytes actually written. Number of bytes left in buffer
 * if bytes == 0.
 */
unsigned int uart_write(uart_context *context, char *buf, unsigned int bytes);

/**
 * Function implementing write system call for UART
 * @param uart_context uart context to read from. Configured by uart_init.
 * @param buf buffer to read from
 * @param bytes number of bytes to read
 * @return number of bytes actually read. Number of bytes available
 * if bytes == 0.
 */
unsigned int uart_read(uart_context *context, char *buf, unsigned int bytes);

/**
 * Not implemented. UARTs are not de-configured.
 */
unsigned int uart_close(uart_context *context);
/* End UNIX state and prototypes */

/**
 * Interrupt Service Routines
 *     state and prototypes
 */
/**
 * Generic ISR for UART RX/TX interrupts
 */
void uart_isr(void);

// currently active uart
static volatile int which_uart;
// true if any uart is currently being processed by uart_isr
static volatile bool isr_active;

queue_t txq_headers[6];
queue_t rxq_headers[6];
volatile char txqs[6][255];
volatile char rxqs[6][255];
uart_context contexts[6] = {
    {UART0, &txq_headers[0], &rxq_headers[0]},
    {UART1, &txq_headers[1], &rxq_headers[1]},
    {UART2, &txq_headers[2], &rxq_headers[2]},
    {UART3, &txq_headers[3], &rxq_headers[3]},
    {UART4, &txq_headers[4], &rxq_headers[4]},
    {UART5, &txq_headers[5], &rxq_headers[5]}
};
/* End ISR state and prototypes */


int uart_init(uart_config conf) {
    //define variables for baud rate and baud rate fine adjust
    uint16_t ubd, brfa;

    // Enable clocks
    *(conf.uart_clock_gate_base) |= conf.uart_clock_gate_mask;
    *(conf.port_clock_gate_base) |= conf.port_clock_gate_mask;

     
    // Set port muxes
    *(conf.tx_pcr) = conf.tx_alt << PORT_PCR_MUX_SHIFT;
    *(conf.rx_pcr) = conf.rx_alt << PORT_PCR_MUX_SHIFT;
    if(conf.rt_alt < 255) {
        *(conf.rt_pcr) = conf.rt_alt << PORT_PCR_MUX_SHIFT;
    }

    if(conf.ct_alt < 255) {
        *(conf.ct_pcr) = conf.ct_alt << PORT_PCR_MUX_SHIFT;
    }

    // Disable RX/TX until after configuration.
    conf.uart_base->C2 &= ~UART_C2_RE_MASK;
    conf.uart_base->C2 &= ~UART_C2_TE_MASK;

    conf.uart_base->C1 = 0;


    // baud rate = UART module clock / (16 × (SBR[12:0] + BRFD))
    // BRFD is dependent on BRFA, refer to Table 52-234 in K64 reference manual
    // BRFA is defined by the lower 4 bits of control register, UART0_C4 

    //calculate baud rate settings: ubd = UART module clock/16* baud rate
    ubd = (uint16_t)((conf.input_clock_rate)/(conf.baud * 16));

    conf.uart_base->BDH &= ~UART_BDH_SBR_MASK;
      
    //distribute this ubd in BDH and BDL
    conf.uart_base->BDH |= (UART_BDH_SBR_MASK & (ubd >> 8));
    conf.uart_base->BDL = ubd;


    //BRFD = (1/32)*BRFA 
    //make the baud rate closer to the desired value by using BRFA
    brfa = (((conf.input_clock_rate*32)/(conf.baud * 16)) - (ubd * 32));

    conf.uart_base->C4 |= (UART_C4_BRFA_MASK & brfa);
        
    // Configure a new file-table entry and necessary state
    int which = (conf.uart_base == UART0)? 0:
                (conf.uart_base == UART1)? 1:
                (conf.uart_base == UART2)? 2:
                (conf.uart_base == UART3)? 3:
                (conf.uart_base == UART4)? 4:5;

    queue_init(&txq_headers[which], &txqs[which], 255);
    queue_init(&rxq_headers[which], &rxqs[which], 255);

    // make isr available
    which_uart = 0;
    isr_active = false;

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
        conf.uart_base->C2 |= UART_C2_RE_MASK | UART_C2_TE_MASK
            | UART_C2_RIE_MASK | UART_C2_TIE_MASK;

        NVIC_EnableIRQ(conf.irqn);
        NVIC->IP[conf.irqn] = conf.priority;
        // generate DMA requests instead of interrupts
        /*conf.uart_base->C5 |= UART_C5_TDMAS_MASK;*/
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

unsigned int uart_write(uart_context *context, char *buf, unsigned int bytes) {
    unsigned int bytes_written = 0;
    if(bytes == 0) {
        bytes_written = context->txq->cap - context->txq->size;
        goto done;
    }
    do {
        queue_push(context->txq, buf[bytes_written]);
        bytes_written += (context->txq->op_ok) ? 1:0;
    } while(context->txq->op_ok && bytes_written < bytes);
    if(bytes_written > 0) context->uart_base->C2 |= UART_C2_TIE_MASK;
done:
    return bytes_written;
}


unsigned int uart_read(uart_context *context, char *buf, unsigned int bytes) {
    unsigned int bytes_read = 0;
    if(bytes == 0) {
        bytes_read = context->rxq->size;
        goto done;
    }
    do {
        char c = queue_pop(context->rxq);
        if(context->rxq->op_ok) {
            buf[bytes_read] = c;
        }
        bytes_read += (context->rxq->op_ok) ? 1:0;
    } while(context->rxq->op_ok && bytes_read < bytes);
done:
    return bytes_read;
}


unsigned int uart_close(uart_context *context) { return 0; }


void uart_isr(void){
    if((contexts[which_uart].uart_base->C2 & UART_C2_TIE_MASK)
            && (contexts[which_uart].uart_base->S1 & UART_S1_TDRE_MASK)) {
        // tx int
        char c = queue_pop(contexts[which_uart].txq);
        if(!contexts[which_uart].txq->op_ok) {
            contexts[which_uart].uart_base->C2 &= ~UART_C2_TIE_MASK;
        }
        else {
            contexts[which_uart].uart_base->D = c;
        }
    }
    else if(contexts[which_uart].uart_base->S1 & UART_S1_RDRF_MASK) {
        // rx int
        queue_push(contexts[which_uart].rxq, contexts[which_uart].uart_base->D);
    }
}

/**
 * ISR stubs
 */
void UART0_RX_TX_IRQHandler(void) {
    if(!isr_active) {
        isr_active = true;
        which_uart = 0;
        uart_isr();
        isr_active = false;
    }
}


void UART1_RX_TX_IRQHandler(void) {
    if(!isr_active) {
        isr_active = true;
        which_uart = 1;
        uart_isr();
        isr_active = false;
    }
}


void UART2_RX_TX_IRQHandler(void) {
    if(!isr_active) {
        isr_active = true;
        which_uart = 2;
        uart_isr();
        isr_active = false;
    }
}


void UART3_RX_TX_IRQHandler(void) {
    if(!isr_active) {
        isr_active = true;
        which_uart = 3;
        uart_isr();
        isr_active = false;
    }
}


void UART4_RX_TX_IRQHandler(void) {
    if(!isr_active) {
        isr_active = true;
        which_uart = 4;
        uart_isr();
        isr_active = false;
    }
}


void UART5_RX_TX_IRQHandler(void) {
    if(!isr_active) {
        isr_active = true;
        which_uart = 6;
        uart_isr();
        isr_active = false;
    }
}
