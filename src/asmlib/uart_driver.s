//****************************************************************
// Serial driver on uart0
// interrupt service routine, specifically
//****************************************************************
            //Include files
#include <drivers/uart_constants.h>
//****************************************************************
//EQUates:

//****************************************************************
.syntax unified
.text
.global putchar
.thumb_func
putchar://     proc {r1-r14}, {}
// put char from r0 onto the TxQueue
// inputs             : r0: char
//                    : r1: which queue
//                    : r2: which uart
// outputs            : none
// modified registers : none
            push {r1, lr}
pc_loop:    cpsid i
            bl   enqueue
            cpsie i
            bcs  pc_loop
            //enable interrupt
            movs r1, #(UART_C2_TIE_MASK | UART_C2_RIE_MASK | UART_C2_RE_MASK | UART_C2_TE_MASK)
            strb r1, [r2, #UART_C2_OFFSET]
            pop  {r1, pc}
            //endp

.global getchar
.thumb_func
getchar://     proc {r1-r14}, {}
// place on r0 a char from the RxQueue
// inputs             : r0: which queue
// outputs            : r0: char
// modified registers : r0
            push {r1, lr}
gc_loop:    cpsid i
            movs r1, r0
            bl   dequeue
            cpsie i
            bcs  gc_loop
            pop  {r1, pc}
            //endp


//.thumb_func
.global UART0_RX_TX_IRQHandler
.global UART1_RX_TX_IRQHandler
.global UART2_RX_TX_IRQHandler
.global UART3_RX_TX_IRQHandler
.global UART4_RX_TX_IRQHandler
.global UART5_RX_TX_IRQHandler
uart_isr:   //proc {r0-r14}, {}
// interrupt service routine for uart0.  handles rx and tx requests.
// inputs             : none
// outputs            : none
// modified registers : none
// register uses:
// r0: uart base
// r1: uart mask
// r2: scratch
// r3: context entry
// r4: uart base
// CONTEXT SIZE: 12 bytes
.thumb_func
UART0_RX_TX_IRQHandler:
			push {r4, lr}
            cpsid i         //mask interrupts
            movs  r0, #0
            b uart_isr.join    
.thumb_func
UART1_RX_TX_IRQHandler:
			push {r4, lr}
            cpsid i         //mask interrupts
            movs r0, #12
.thumb_func
UART2_RX_TX_IRQHandler:
            b uart_isr.join    
			push {r4, lr}
            cpsid i         //mask interrupts
            movs r0, #24
            b uart_isr.join    
.thumb_func
UART3_RX_TX_IRQHandler:
			push {r4, lr}
            cpsid i         //mask interrupts
            movs r0, #36
            b uart_isr.join    
.thumb_func
UART4_RX_TX_IRQHandler:
			push {r4, lr}
            cpsid i         //mask interrupts
            movs r0, #48
            b uart_isr.join    
.thumb_func
UART5_RX_TX_IRQHandler:
			push {r4, lr}
            cpsid i         //mask interrupts
            movs r0, #60
uart_isr.join:
            ldr r3, =uart_contexts_address
            ldr r3, [r3, #0]
            ldr r4, [r3, r0] // the uart base address
            movs  r1, #UART_C2_TIE_MASK
            ldrb r2, [r4, #UART_C2_OFFSET]
            ands r2, r2, r1
            cmp  r2, #0
            beq  not_tx
            ldrb r2, [r4, #UART_S1_OFFSET]
            movs r1, #UART_S1_TDRE_MASK
            ands r2, r2, r1
            cmp  r2, #0 // I really think this is unnecessary...
            beq  not_tx
            // interrupt is not a false alarm
            adds r2, r3, r0
            ldr  r1, [r2, #4] // offset for txq_header in uart_context
            bl   dequeue
			bcs  clear_txirq
            //write to uart
            strb r0, [r4, #UART_D_OFFSET]
            b    isr_done

clear_txirq: 
            movs r1, #(UART_C2_TE_MASK | UART_C2_RIE_MASK | UART_C2_RE_MASK)
            strb r1, [r4, #UART_C2_OFFSET]
            b    isr_done

not_tx:
            ldrb r2, [r4, #UART_S1_OFFSET]
            movs r1, #UART_S1_RDRF_MASK
            ands r2, r2, r1
            cmp  r2, #0
            beq  isr_done
            adds r2, r3, r0 // uart_contexts[r0].rxq_header
            ldr  r1, [r2, #8] // offset for rxq_header in uart_context
            ldrb r0, [r4, #UART_D_OFFSET]
            bl   enqueue

isr_done:   cpsie i
			pop  {r4, pc}
            //endp
.end
