#pragma once
/**
 * This file contains all the ALTs for the K64 UART pins, and definitions which
 * are GNU assembly compatible.  Generated from Freescale C headers.
 */
/*
 *
 *UART0 PINS:
 *CTS PTD5, ALT 3 | PTB3, ALT 3  | PTA16, ALT 3 | PTA0, ALT 2 
 *RTS PTD4, ALT 3 | PTB2, ALT 3  | PTA17, ALT 3 | PTA3, ALT 2
 *RXD PTD6, ALT 3 | PTB16, ALT 3 | PTA15, ALT 3 | PTA1, ALT 2
 *TXD PTD7, ALT 3 | PTB17, ALT 3 | PTA14, ALT 3 | PTA2, ALT 2
 *
 *UART1 PINS:
 *CTS PTC2, ALT 3 | PTE2, ALT 3
 *RTS PTC1, ALT 3 | PTE3, ALT 3
 *RXD PTC3, ALT 3 | PTE1, ALT 3
 *TXD PTC4, ALT 3 | PTE0, ALT 3
 *
 *UART2 PINS:
 *CTS PTD5, ALT 3
 *RTS PTD0, ALT 3
 *RXD PTD2, ALT 3
 *TXD PTD3, ALT 3
 *
 *UART3 PINS:
 *CTS PTC19, ALT 3 | PTB9, ALT 3  | PTE6, ALT 3 | PTB11, ALT 2
 *RTS PTC18, ALT 3 | PTB8, ALT 3  | PTE7, ALT 3 | PTB12, ALT 2
 *RXD PTC16, ALT 3 | PTB10, ALT 3 | PTE5, ALT 3
 *TXD PTC17, ALT 3 | PTB11, ALT 3 | PTE4, ALT 3
 *
 *UART4 PINS:
 *CTS PTC13, ALT 3 | PTE26, ALT 3
 *RTS PTC12, ALT 3 | PTE27, ALT 3
 *RXD PTC14, ALT 3 | PTE25, ALT 3
 *TXD PTC15, ALT 3 | PTE24, ALT 3
 *
 *UART5 PINS:
 *CTS PTD11, ALT 3 | PTE10, ALT 3
 *RTS PTD10, ALT 3 | PTE11, ALT 3
 *RXD PTD8, ALT 3  | PTE9, ALT 3
 *TXD PTD9, ALT 3  | PTE8, ALT 3
 */
#define UART_BDH_OFFSET 0
#define UART_BDL_OFFSET 1
#define UART_C1_OFFSET 2
#define UART_C2_OFFSET 3
#define UART_S1_OFFSET 4
#define UART_S2_OFFSET 5
#define UART_C3_OFFSET 6
#define UART_D_OFFSET 7
#define UART_MA1_OFFSET 8
#define UART_MA2_OFFSET 9
#define UART_C4_OFFSET 10
#define UART_C5_OFFSET 11
#define UART_ED_OFFSET 12
#define UART_MODEM_OFFSET 13
#define UART_IR_OFFSET 14
#define UART_PFIFO_OFFSET 16
#define UART_CFIFO_OFFSET 17
#define UART_SFIFO_OFFSET 18
#define UART_TWFIFO_OFFSET 19
#define UART_TCFIFO_OFFSET 20
#define UART_RWFIFO_OFFSET 21
#define UART_RCFIFO_OFFSET 22
#define UART_C7816_OFFSET 23
#define UART_IE7816_OFFSET 24
#define UART_IS7816_OFFSET 25
#define UART_WP7816T0_OFFSET 26
#define UART_WP7816T1_OFFSET 27
#define UART_WN7816_OFFSET 28
#define UART_WF7816_OFFSET 29
#define UART_ET7816_OFFSET 30
#define UART_TL7816_OFFSET 31

// UART 0
#define UART0_BASE 0x4006a000
#define UART1_BASE 0x4006b000
#define UART2_BASE 0x4006c000
#define UART3_BASE 0x4006d000
#define UART4_BASE 0x400ea000
#define UART5_BASE 0x400eb000

#define UART0_BDH (UART0_BASE + UART_BDH_OFFSET)
#define UART0_BDL (UART0_BASE + UART_BDL_OFFSET)
#define UART0_C1 (UART0_BASE + UART_C1_OFFSET)
#define UART0_C2 (UART0_BASE + UART_C2_OFFSET)
#define UART0_S1 (UART0_BASE + UART_S1_OFFSET)
#define UART0_S2 (UART0_BASE + UART_S2_OFFSET)
#define UART0_C3 (UART0_BASE + UART_C3_OFFSET)
#define UART0_D (UART0_BASE + UART_D_OFFSET)
#define UART0_MA1 (UART0_BASE + UART_MA1_OFFSET)
#define UART0_MA2 (UART0_BASE + UART_MA2_OFFSET)
#define UART0_C4 (UART0_BASE + UART_C4_OFFSET)
#define UART0_C5 (UART0_BASE + UART_C5_OFFSET)
#define UART0_ED (UART0_BASE + UART_ED_OFFSET)
#define UART0_MODEM (UART0_BASE + UART_MODEM_OFFSET)
#define UART0_IR (UART0_BASE + UART_IR_OFFSET)
#define UART0_PFIFO (UART0_BASE + UART_PFIFO_OFFSET)
#define UART0_CFIFO (UART0_BASE + UART_CFIFO_OFFSET)
#define UART0_SFIFO (UART0_BASE + UART_SFIFO_OFFSET)
#define UART0_TWFIFO (UART0_BASE + UART_TWFIFO_OFFSET)
#define UART0_TCFIFO (UART0_BASE + UART_TCFIFO_OFFSET)
#define UART0_RWFIFO (UART0_BASE + UART_RWFIFO_OFFSET)
#define UART0_RCFIFO (UART0_BASE + UART_RCFIFO_OFFSET)
#define UART0_C7816 (UART0_BASE + UART_C7816_OFFSET)
#define UART0_IE7816 (UART0_BASE + UART_IE7816_OFFSET)
#define UART0_IS7816 (UART0_BASE + UART_IS7816_OFFSET)
#define UART0_WP7816T0 (UART0_BASE + UART_WP7816T0_OFFSET)
#define UART0_WP7816T1 (UART0_BASE + UART_WP7816T1_OFFSET)
#define UART0_WN7816 (UART0_BASE + UART_WN7816_OFFSET)
#define UART0_WF7816 (UART0_BASE + UART_WF7816_OFFSET)
#define UART0_ET7816 (UART0_BASE + UART_ET7816_OFFSET)



#define UART1_BDH (UART1_BASE + UART_BDH_OFFSET)
#define UART1_BDL (UART1_BASE + UART_BDL_OFFSET)
#define UART1_C1 (UART1_BASE + UART_C1_OFFSET)
#define UART1_C2 (UART1_BASE + UART_C2_OFFSET)
#define UART1_S1 (UART1_BASE + UART_S1_OFFSET)
#define UART1_S2 (UART1_BASE + UART_S2_OFFSET)
#define UART1_C3 (UART1_BASE + UART_C3_OFFSET)
#define UART1_D (UART1_BASE + UART_D_OFFSET)
#define UART1_MA1 (UART1_BASE + UART_MA1_OFFSET)
#define UART1_MA2 (UART1_BASE + UART_MA2_OFFSET)
#define UART1_C4 (UART1_BASE + UART_C4_OFFSET)
#define UART1_C5 (UART1_BASE + UART_C5_OFFSET)
#define UART1_ED (UART1_BASE + UART_ED_OFFSET)
#define UART1_MODEM (UART1_BASE + UART_MODEM_OFFSET)
#define UART1_IR (UART1_BASE + UART_IR_OFFSET)
#define UART1_PFIFO (UART1_BASE + UART_PFIFO_OFFSET)
#define UART1_CFIFO (UART1_BASE + UART_CFIFO_OFFSET)
#define UART1_SFIFO (UART1_BASE + UART_SFIFO_OFFSET)
#define UART1_TWFIFO (UART1_BASE + UART_TWFIFO_OFFSET)
#define UART1_TCFIFO (UART1_BASE + UART_TCFIFO_OFFSET)
#define UART1_RWFIFO (UART1_BASE + UART_RWFIFO_OFFSET)
#define UART1_RCFIFO (UART1_BASE + UART_RCFIFO_OFFSET)
#define UART1_C7816 (UART1_BASE + UART_C7816_OFFSET)
#define UART1_IE7816 (UART1_BASE + UART_IE7816_OFFSET)
#define UART1_IS7816 (UART1_BASE + UART_IS7816_OFFSET)
#define UART1_WP7816T0 (UART1_BASE + UART_WP7816T0_OFFSET)
#define UART1_WP7816T1 (UART1_BASE + UART_WP7816T1_OFFSET)
#define UART1_WN7816 (UART1_BASE + UART_WN7816_OFFSET)
#define UART1_WF7816 (UART1_BASE + UART_WF7816_OFFSET)
#define UART1_ET7816 (UART1_BASE + UART_ET7816_OFFSET)


#define UART2_BDH (UART2_BASE + UART_BDH_OFFSET)
#define UART2_BDL (UART2_BASE + UART_BDL_OFFSET)
#define UART2_C1 (UART2_BASE + UART_C1_OFFSET)
#define UART2_C2 (UART2_BASE + UART_C2_OFFSET)
#define UART2_S1 (UART2_BASE + UART_S1_OFFSET)
#define UART2_S2 (UART2_BASE + UART_S2_OFFSET)
#define UART2_C3 (UART2_BASE + UART_C3_OFFSET)
#define UART2_D (UART2_BASE + UART_D_OFFSET)
#define UART2_MA1 (UART2_BASE + UART_MA1_OFFSET)
#define UART2_MA2 (UART2_BASE + UART_MA2_OFFSET)
#define UART2_C4 (UART2_BASE + UART_C4_OFFSET)
#define UART2_C5 (UART2_BASE + UART_C5_OFFSET)
#define UART2_ED (UART2_BASE + UART_ED_OFFSET)
#define UART2_MODEM (UART2_BASE + UART_MODEM_OFFSET)
#define UART2_IR (UART2_BASE + UART_IR_OFFSET)
#define UART2_PFIFO (UART2_BASE + UART_PFIFO_OFFSET)
#define UART2_CFIFO (UART2_BASE + UART_CFIFO_OFFSET)
#define UART2_SFIFO (UART2_BASE + UART_SFIFO_OFFSET)
#define UART2_TWFIFO (UART2_BASE + UART_TWFIFO_OFFSET)
#define UART2_TCFIFO (UART2_BASE + UART_TCFIFO_OFFSET)
#define UART2_RWFIFO (UART2_BASE + UART_RWFIFO_OFFSET)
#define UART2_RCFIFO (UART2_BASE + UART_RCFIFO_OFFSET)
#define UART2_C7816 (UART2_BASE + UART_C7816_OFFSET)
#define UART2_IE7816 (UART2_BASE + UART_IE7816_OFFSET)
#define UART2_IS7816 (UART2_BASE + UART_IS7816_OFFSET)
#define UART2_WP7816T0 (UART2_BASE + UART_WP7816T0_OFFSET)
#define UART2_WP7816T1 (UART2_BASE + UART_WP7816T1_OFFSET)
#define UART2_WN7816 (UART2_BASE + UART_WN7816_OFFSET)
#define UART2_WF7816 (UART2_BASE + UART_WF7816_OFFSET)
#define UART2_ET7816 (UART2_BASE + UART_ET7816_OFFSET)


#define UART3_BDH (UART3_BASE + UART_BDH_OFFSET)
#define UART3_BDL (UART3_BASE + UART_BDL_OFFSET)
#define UART3_C1 (UART3_BASE + UART_C1_OFFSET)
#define UART3_C2 (UART3_BASE + UART_C2_OFFSET)
#define UART3_S1 (UART3_BASE + UART_S1_OFFSET)
#define UART3_S2 (UART3_BASE + UART_S2_OFFSET)
#define UART3_C3 (UART3_BASE + UART_C3_OFFSET)
#define UART3_D (UART3_BASE + UART_D_OFFSET)
#define UART3_MA1 (UART3_BASE + UART_MA1_OFFSET)
#define UART3_MA2 (UART3_BASE + UART_MA2_OFFSET)
#define UART3_C4 (UART3_BASE + UART_C4_OFFSET)
#define UART3_C5 (UART3_BASE + UART_C5_OFFSET)
#define UART3_ED (UART3_BASE + UART_ED_OFFSET)
#define UART3_MODEM (UART3_BASE + UART_MODEM_OFFSET)
#define UART3_IR (UART3_BASE + UART_IR_OFFSET)
#define UART3_PFIFO (UART3_BASE + UART_PFIFO_OFFSET)
#define UART3_CFIFO (UART3_BASE + UART_CFIFO_OFFSET)
#define UART3_SFIFO (UART3_BASE + UART_SFIFO_OFFSET)
#define UART3_TWFIFO (UART3_BASE + UART_TWFIFO_OFFSET)
#define UART3_TCFIFO (UART3_BASE + UART_TCFIFO_OFFSET)
#define UART3_RWFIFO (UART3_BASE + UART_RWFIFO_OFFSET)
#define UART3_RCFIFO (UART3_BASE + UART_RCFIFO_OFFSET)
#define UART3_C7816 (UART3_BASE + UART_C7816_OFFSET)
#define UART3_IE7816 (UART3_BASE + UART_IE7816_OFFSET)
#define UART3_IS7816 (UART3_BASE + UART_IS7816_OFFSET)
#define UART3_WP7816T0 (UART3_BASE + UART_WP7816T0_OFFSET)
#define UART3_WP7816T1 (UART3_BASE + UART_WP7816T1_OFFSET)
#define UART3_WN7816 (UART3_BASE + UART_WN7816_OFFSET)
#define UART3_WF7816 (UART3_BASE + UART_WF7816_OFFSET)
#define UART3_ET7816 (UART3_BASE + UART_ET7816_OFFSET)


#define UART4_BDH (UART4_BASE + UART_BDH_OFFSET)
#define UART4_BDL (UART4_BASE + UART_BDL_OFFSET)
#define UART4_C1 (UART4_BASE + UART_C1_OFFSET)
#define UART4_C2 (UART4_BASE + UART_C2_OFFSET)
#define UART4_S1 (UART4_BASE + UART_S1_OFFSET)
#define UART4_S2 (UART4_BASE + UART_S2_OFFSET)
#define UART4_C3 (UART4_BASE + UART_C3_OFFSET)
#define UART4_D (UART4_BASE + UART_D_OFFSET)
#define UART4_MA1 (UART4_BASE + UART_MA1_OFFSET)
#define UART4_MA2 (UART4_BASE + UART_MA2_OFFSET)
#define UART4_C4 (UART4_BASE + UART_C4_OFFSET)
#define UART4_C5 (UART4_BASE + UART_C5_OFFSET)
#define UART4_ED (UART4_BASE + UART_ED_OFFSET)
#define UART4_MODEM (UART4_BASE + UART_MODEM_OFFSET)
#define UART4_IR (UART4_BASE + UART_IR_OFFSET)
#define UART4_PFIFO (UART4_BASE + UART_PFIFO_OFFSET)
#define UART4_CFIFO (UART4_BASE + UART_CFIFO_OFFSET)
#define UART4_SFIFO (UART4_BASE + UART_SFIFO_OFFSET)
#define UART4_TWFIFO (UART4_BASE + UART_TWFIFO_OFFSET)
#define UART4_TCFIFO (UART4_BASE + UART_TCFIFO_OFFSET)
#define UART4_RWFIFO (UART4_BASE + UART_RWFIFO_OFFSET)
#define UART4_RCFIFO (UART4_BASE + UART_RCFIFO_OFFSET)
#define UART4_C7816 (UART4_BASE + UART_C7816_OFFSET)
#define UART4_IE7816 (UART4_BASE + UART_IE7816_OFFSET)
#define UART4_IS7816 (UART4_BASE + UART_IS7816_OFFSET)
#define UART4_WP7816T0 (UART4_BASE + UART_WP7816T0_OFFSET)
#define UART4_WP7816T1 (UART4_BASE + UART_WP7816T1_OFFSET)
#define UART4_WN7816 (UART4_BASE + UART_WN7816_OFFSET)
#define UART4_WF7816 (UART4_BASE + UART_WF7816_OFFSET)
#define UART4_ET7816 (UART4_BASE + UART_ET7816_OFFSET)


#define UART5_BDH (UART5_BASE + UART_BDH_OFFSET)
#define UART5_BDL (UART5_BASE + UART_BDL_OFFSET)
#define UART5_C1 (UART5_BASE + UART_C1_OFFSET)
#define UART5_C2 (UART5_BASE + UART_C2_OFFSET)
#define UART5_S1 (UART5_BASE + UART_S1_OFFSET)
#define UART5_S2 (UART5_BASE + UART_S2_OFFSET)
#define UART5_C3 (UART5_BASE + UART_C3_OFFSET)
#define UART5_D (UART5_BASE + UART_D_OFFSET)
#define UART5_MA1 (UART5_BASE + UART_MA1_OFFSET)
#define UART5_MA2 (UART5_BASE + UART_MA2_OFFSET)
#define UART5_C4 (UART5_BASE + UART_C4_OFFSET)
#define UART5_C5 (UART5_BASE + UART_C5_OFFSET)
#define UART5_ED (UART5_BASE + UART_ED_OFFSET)
#define UART5_MODEM (UART5_BASE + UART_MODEM_OFFSET)
#define UART5_IR (UART5_BASE + UART_IR_OFFSET)
#define UART5_PFIFO (UART5_BASE + UART_PFIFO_OFFSET)
#define UART5_CFIFO (UART5_BASE + UART_CFIFO_OFFSET)
#define UART5_SFIFO (UART5_BASE + UART_SFIFO_OFFSET)
#define UART5_TWFIFO (UART5_BASE + UART_TWFIFO_OFFSET)
#define UART5_TCFIFO (UART5_BASE + UART_TCFIFO_OFFSET)
#define UART5_RWFIFO (UART5_BASE + UART_RWFIFO_OFFSET)
#define UART5_RCFIFO (UART5_BASE + UART_RCFIFO_OFFSET)
#define UART5_C7816 (UART5_BASE + UART_C7816_OFFSET)
#define UART5_IE7816 (UART5_BASE + UART_IE7816_OFFSET)
#define UART5_IS7816 (UART5_BASE + UART_IS7816_OFFSET)
#define UART5_WP7816T0 (UART5_BASE + UART_WP7816T0_OFFSET)
#define UART5_WP7816T1 (UART5_BASE + UART_WP7816T1_OFFSET)
#define UART5_WN7816 (UART5_BASE + UART_WN7816_OFFSET)
#define UART5_WF7816 (UART5_BASE + UART_WF7816_OFFSET)
#define UART5_ET7816 (UART5_BASE + UART_ET7816_OFFSET)


/* BDH Bit Fields */
#define UART_BDH_SBR_MASK                        0x1Fu
#define UART_BDH_SBR_SHIFT                       0
#define UART_BDH_SBNS_MASK                       0x20u
#define UART_BDH_SBNS_SHIFT                      5
#define UART_BDH_RXEDGIE_MASK                    0x40u
#define UART_BDH_RXEDGIE_SHIFT                   6
#define UART_BDH_LBKDIE_MASK                     0x80u
#define UART_BDH_LBKDIE_SHIFT                    7
/* BDL Bit Fields */
#define UART_BDL_SBR_MASK                        0xFFu
#define UART_BDL_SBR_SHIFT                       0
/* C1 Bit Fields */
#define UART_C1_PT_MASK                          0x1u
#define UART_C1_PT_SHIFT                         0
#define UART_C1_PE_MASK                          0x2u
#define UART_C1_PE_SHIFT                         1
#define UART_C1_ILT_MASK                         0x4u
#define UART_C1_ILT_SHIFT                        2
#define UART_C1_WAKE_MASK                        0x8u
#define UART_C1_WAKE_SHIFT                       3
#define UART_C1_M_MASK                           0x10u
#define UART_C1_M_SHIFT                          4
#define UART_C1_RSRC_MASK                        0x20u
#define UART_C1_RSRC_SHIFT                       5
#define UART_C1_UARTSWAI_MASK                    0x40u
#define UART_C1_UARTSWAI_SHIFT                   6
#define UART_C1_LOOPS_MASK                       0x80u
#define UART_C1_LOOPS_SHIFT                      7
/* C2 Bit Fields */
#define UART_C2_SBK_MASK                         0x1u
#define UART_C2_SBK_SHIFT                        0
#define UART_C2_RWU_MASK                         0x2u
#define UART_C2_RWU_SHIFT                        1
#define UART_C2_RE_MASK                          0x4u
#define UART_C2_RE_SHIFT                         2
#define UART_C2_TE_MASK                          0x8u
#define UART_C2_TE_SHIFT                         3
#define UART_C2_ILIE_MASK                        0x10u
#define UART_C2_ILIE_SHIFT                       4
#define UART_C2_RIE_MASK                         0x20u
#define UART_C2_RIE_SHIFT                        5
#define UART_C2_TCIE_MASK                        0x40u
#define UART_C2_TCIE_SHIFT                       6
#define UART_C2_TIE_MASK                         0x80u
#define UART_C2_TIE_SHIFT                        7
/* S1 Bit Fields */
#define UART_S1_PF_MASK                          0x1u
#define UART_S1_PF_SHIFT                         0
#define UART_S1_FE_MASK                          0x2u
#define UART_S1_FE_SHIFT                         1
#define UART_S1_NF_MASK                          0x4u
#define UART_S1_NF_SHIFT                         2
#define UART_S1_OR_MASK                          0x8u
#define UART_S1_OR_SHIFT                         3
#define UART_S1_IDLE_MASK                        0x10u
#define UART_S1_IDLE_SHIFT                       4
#define UART_S1_RDRF_MASK                        0x20u
#define UART_S1_RDRF_SHIFT                       5
#define UART_S1_TC_MASK                          0x40u
#define UART_S1_TC_SHIFT                         6
#define UART_S1_TDRE_MASK                        0x80u
#define UART_S1_TDRE_SHIFT                       7
/* S2 Bit Fields */
#define UART_S2_RAF_MASK                         0x1u
#define UART_S2_RAF_SHIFT                        0
#define UART_S2_LBKDE_MASK                       0x2u
#define UART_S2_LBKDE_SHIFT                      1
#define UART_S2_BRK13_MASK                       0x4u
#define UART_S2_BRK13_SHIFT                      2
#define UART_S2_RWUID_MASK                       0x8u
#define UART_S2_RWUID_SHIFT                      3
#define UART_S2_RXINV_MASK                       0x10u
#define UART_S2_RXINV_SHIFT                      4
#define UART_S2_MSBF_MASK                        0x20u
#define UART_S2_MSBF_SHIFT                       5
#define UART_S2_RXEDGIF_MASK                     0x40u
#define UART_S2_RXEDGIF_SHIFT                    6
#define UART_S2_LBKDIF_MASK                      0x80u
#define UART_S2_LBKDIF_SHIFT                     7
/* C3 Bit Fields */
#define UART_C3_PEIE_MASK                        0x1u
#define UART_C3_PEIE_SHIFT                       0
#define UART_C3_FEIE_MASK                        0x2u
#define UART_C3_FEIE_SHIFT                       1
#define UART_C3_NEIE_MASK                        0x4u
#define UART_C3_NEIE_SHIFT                       2
#define UART_C3_ORIE_MASK                        0x8u
#define UART_C3_ORIE_SHIFT                       3
#define UART_C3_TXINV_MASK                       0x10u
#define UART_C3_TXINV_SHIFT                      4
#define UART_C3_TXDIR_MASK                       0x20u
#define UART_C3_TXDIR_SHIFT                      5
#define UART_C3_T8_MASK                          0x40u
#define UART_C3_T8_SHIFT                         6
#define UART_C3_R8_MASK                          0x80u
#define UART_C3_R8_SHIFT                         7
/* D Bit Fields */
#define UART_D_RT_MASK                           0xFFu
#define UART_D_RT_SHIFT                          0
/* MA1 Bit Fields */
#define UART_MA1_MA_MASK                         0xFFu
#define UART_MA1_MA_SHIFT                        0
/* MA2 Bit Fields */
#define UART_MA2_MA_MASK                         0xFFu
#define UART_MA2_MA_SHIFT                        0
/* C4 Bit Fields */
#define UART_C4_BRFA_MASK                        0x1Fu
#define UART_C4_BRFA_SHIFT                       0
#define UART_C4_M10_MASK                         0x20u
#define UART_C4_M10_SHIFT                        5
#define UART_C4_MAEN2_MASK                       0x40u
#define UART_C4_MAEN2_SHIFT                      6
#define UART_C4_MAEN1_MASK                       0x80u
#define UART_C4_MAEN1_SHIFT                      7
/* C5 Bit Fields */
#define UART_C5_LBKDDMAS_MASK                    0x8u
#define UART_C5_LBKDDMAS_SHIFT                   3
#define UART_C5_ILDMAS_MASK                      0x10u
#define UART_C5_ILDMAS_SHIFT                     4
#define UART_C5_RDMAS_MASK                       0x20u
#define UART_C5_RDMAS_SHIFT                      5
#define UART_C5_TCDMAS_MASK                      0x40u
#define UART_C5_TCDMAS_SHIFT                     6
#define UART_C5_TDMAS_MASK                       0x80u
#define UART_C5_TDMAS_SHIFT                      7
/* ED Bit Fields */
#define UART_ED_PARITYE_MASK                     0x40u
#define UART_ED_PARITYE_SHIFT                    6
#define UART_ED_NOISY_MASK                       0x80u
#define UART_ED_NOISY_SHIFT                      7
/* MODEM Bit Fields */
#define UART_MODEM_TXCTSE_MASK                   0x1u
#define UART_MODEM_TXCTSE_SHIFT                  0
#define UART_MODEM_TXRTSE_MASK                   0x2u
#define UART_MODEM_TXRTSE_SHIFT                  1
#define UART_MODEM_TXRTSPOL_MASK                 0x4u
#define UART_MODEM_TXRTSPOL_SHIFT                2
#define UART_MODEM_RXRTSE_MASK                   0x8u
#define UART_MODEM_RXRTSE_SHIFT                  3
/* IR Bit Fields */
#define UART_IR_TNP_MASK                         0x3u
#define UART_IR_TNP_SHIFT                        0
#define UART_IR_IREN_MASK                        0x4u
#define UART_IR_IREN_SHIFT                       2
/* PFIFO Bit Fields */
#define UART_PFIFO_RXFIFOSIZE_MASK               0x7u
#define UART_PFIFO_RXFIFOSIZE_SHIFT              0
#define UART_PFIFO_RXFE_MASK                     0x8u
#define UART_PFIFO_RXFE_SHIFT                    3
#define UART_PFIFO_TXFIFOSIZE_MASK               0x70u
#define UART_PFIFO_TXFIFOSIZE_SHIFT              4
#define UART_PFIFO_TXFE_MASK                     0x80u
#define UART_PFIFO_TXFE_SHIFT                    7
/* CFIFO Bit Fields */
#define UART_CFIFO_RXUFE_MASK                    0x1u
#define UART_CFIFO_RXUFE_SHIFT                   0
#define UART_CFIFO_TXOFE_MASK                    0x2u
#define UART_CFIFO_TXOFE_SHIFT                   1
#define UART_CFIFO_RXOFE_MASK                    0x4u
#define UART_CFIFO_RXOFE_SHIFT                   2
#define UART_CFIFO_RXFLUSH_MASK                  0x40u
#define UART_CFIFO_RXFLUSH_SHIFT                 6
#define UART_CFIFO_TXFLUSH_MASK                  0x80u
#define UART_CFIFO_TXFLUSH_SHIFT                 7
/* SFIFO Bit Fields */
#define UART_SFIFO_RXUF_MASK                     0x1u
#define UART_SFIFO_RXUF_SHIFT                    0
#define UART_SFIFO_TXOF_MASK                     0x2u
#define UART_SFIFO_TXOF_SHIFT                    1
#define UART_SFIFO_RXOF_MASK                     0x4u
#define UART_SFIFO_RXOF_SHIFT                    2
#define UART_SFIFO_RXEMPT_MASK                   0x40u
#define UART_SFIFO_RXEMPT_SHIFT                  6
#define UART_SFIFO_TXEMPT_MASK                   0x80u
#define UART_SFIFO_TXEMPT_SHIFT                  7
/* TWFIFO Bit Fields */
#define UART_TWFIFO_TXWATER_MASK                 0xFFu
#define UART_TWFIFO_TXWATER_SHIFT                0
/* TCFIFO Bit Fields */
#define UART_TCFIFO_TXCOUNT_MASK                 0xFFu
#define UART_TCFIFO_TXCOUNT_SHIFT                0
/* RWFIFO Bit Fields */
#define UART_RWFIFO_RXWATER_MASK                 0xFFu
#define UART_RWFIFO_RXWATER_SHIFT                0
/* RCFIFO Bit Fields */
#define UART_RCFIFO_RXCOUNT_MASK                 0xFFu
#define UART_RCFIFO_RXCOUNT_SHIFT                0
/* C7816 Bit Fields */
#define UART_C7816_ISO_7816E_MASK                0x1u
#define UART_C7816_ISO_7816E_SHIFT               0
#define UART_C7816_TTYPE_MASK                    0x2u
#define UART_C7816_TTYPE_SHIFT                   1
#define UART_C7816_INIT_MASK                     0x4u
#define UART_C7816_INIT_SHIFT                    2
#define UART_C7816_ANACK_MASK                    0x8u
#define UART_C7816_ANACK_SHIFT                   3
#define UART_C7816_ONACK_MASK                    0x10u
#define UART_C7816_ONACK_SHIFT                   4
/* IE7816 Bit Fields */
#define UART_IE7816_RXTE_MASK                    0x1u
#define UART_IE7816_RXTE_SHIFT                   0
#define UART_IE7816_TXTE_MASK                    0x2u
#define UART_IE7816_TXTE_SHIFT                   1
#define UART_IE7816_GTVE_MASK                    0x4u
#define UART_IE7816_GTVE_SHIFT                   2
#define UART_IE7816_INITDE_MASK                  0x10u
#define UART_IE7816_INITDE_SHIFT                 4
#define UART_IE7816_BWTE_MASK                    0x20u
#define UART_IE7816_BWTE_SHIFT                   5
#define UART_IE7816_CWTE_MASK                    0x40u
#define UART_IE7816_CWTE_SHIFT                   6
#define UART_IE7816_WTE_MASK                     0x80u
#define UART_IE7816_WTE_SHIFT                    7
/* IS7816 Bit Fields */
#define UART_IS7816_RXT_MASK                     0x1u
#define UART_IS7816_RXT_SHIFT                    0
#define UART_IS7816_TXT_MASK                     0x2u
#define UART_IS7816_TXT_SHIFT                    1
#define UART_IS7816_GTV_MASK                     0x4u
#define UART_IS7816_GTV_SHIFT                    2
#define UART_IS7816_INITD_MASK                   0x10u
#define UART_IS7816_INITD_SHIFT                  4
#define UART_IS7816_BWT_MASK                     0x20u
#define UART_IS7816_BWT_SHIFT                    5
#define UART_IS7816_CWT_MASK                     0x40u
#define UART_IS7816_CWT_SHIFT                    6
#define UART_IS7816_WT_MASK                      0x80u
#define UART_IS7816_WT_SHIFT                     7
/* WP7816_T_TYPE0 Bit Fields */
#define UART_WP7816_T_TYPE0_WI_MASK              0xFFu
#define UART_WP7816_T_TYPE0_WI_SHIFT             0
/* WP7816_T_TYPE1 Bit Fields */
#define UART_WP7816_T_TYPE1_BWI_MASK             0xFu
#define UART_WP7816_T_TYPE1_BWI_SHIFT            0
#define UART_WP7816_T_TYPE1_CWI_MASK             0xF0u
#define UART_WP7816_T_TYPE1_CWI_SHIFT            4
/* WN7816 Bit Fields */
#define UART_WN7816_GTN_MASK                     0xFFu
#define UART_WN7816_GTN_SHIFT                    0
/* WF7816 Bit Fields */
#define UART_WF7816_GTFD_MASK                    0xFFu
#define UART_WF7816_GTFD_SHIFT                   0
/* ET7816 Bit Fields */
#define UART_ET7816_RXTHRESHOLD_MASK             0xFu
#define UART_ET7816_RXTHRESHOLD_SHIFT            0
#define UART_ET7816_TXTHRESHOLD_MASK             0xF0u
#define UART_ET7816_TXTHRESHOLD_SHIFT            4
/* TL7816 Bit Fields */
#define UART_TL7816_TLEN_MASK                    0xFFu
#define UART_TL7816_TLEN_SHIFT                   0
