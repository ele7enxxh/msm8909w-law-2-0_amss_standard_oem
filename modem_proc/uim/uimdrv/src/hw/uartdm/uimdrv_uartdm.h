#ifndef UIMDRV_UARTDM_H
#define UIMDRV_UARTDM_H
/*============================================================================
  FILE:         uimdrv_uartdm.h

  OVERVIEW:     File conatins the inclusions and definitions needed by uartdm

  DEPENDENCIES: N/A

                Copyright (c) 2012 - 2014 QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.  Please
use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/hw/uartdm/uimdrv_uartdm.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       -----------------------------------------------------------
09/16/15    sam        UART 1_4_2 changes
09/18/14    akv        UIMDRV UT enhancements
07/17/14    kr         removed compilation errors
05/30/14    akv        UART_DM register logging
02/20/14    ak         Maximize the UART clock frequency by setting it directly
01/29/14    akv        RX ISR refactoring changes
11/27/13    akv        Initialization of hw enumeration based on dal chip family
11/13/13    akv        Program BCR Reg to enable all features for UARTDM 1.4.0
10/08/13    nmb        HW header file cleanup
09/19/13    akv        Reference CLK changes to support 4.8MHz
09/16/13    yk         Removed the code for old targets and for old features
09/09/13    akv        Split UARTDM reset into multiple CR commands
08/22/13    ak         Remove unnecessary macros
07/12/13    nmb        Global Reorganization
07/12/13    akv        HW Enumeration - modifying target specific hardware values 
                       and organizing them in value-key pairs
04/04/13    ak         Correction in the bit definition of
                       MSMU_MR2_RX_ERROR_ZERO_CHAR_OFF
02/22/13    js         8x26 bring up changes
02/08/13    akv        Power optimization fixes
01/16/13    akv        Debounce timer set to 3 sclks
12/05/12    js         UIM Parallel Processing changes
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "comdef.h"
#include "uimdrv_main.h"

#define UIM_UART_DM_HW_VERSION_1_4_2    0x10040002

#define CSR_VALUE MSMU_CSR_16BITS_BIT

/* default BADR value of 0x40 words */
#define UIM_DEFAULT_BADR_VAL            0x40
/* Optimized the stale timeout value to 2 (i.e about 2 character times
   or ~20 etus for speed enhancement. */
#define MSMU_IPR_DEF_STALE_TIMEOUT      0x02

/* Max number of SR check for CR write */
#define UIM_MAX_NUM_SR_CHECK            0x5

#define SET      1
#define CLR      0

/* Uart configuration values used by the pmic */
#define UIM_SET_MODE_18                 0x00001000
#define UIM_CLR_MODE_18                 0xFFFFEFFF

/* Status Register */
#define MSMU_SR_CMD_IN_PROG  0x1000       /* Command in progress */
#define MSMU_SR_RXHUNT    0x80         /* Hunt Character Received          */
#define MSMU_SR_RXBREAK   0x40         /* Break Received                   */
#define MSMU_SR_PF_ERR    0x20         /* Parity or Framing Error          */
#define MSMU_SR_OVR_ERR   0x10         /* Overrun Error                    */
#define MSMU_SR_TXEMT     0x08         /* Transmitter Empty                */
#define MSMU_SR_TXRDY     0x04         /* Transmitter Ready                */
#define MSMU_SR_RXFULL    0x02         /* Receiver Full                    */
#define MSMU_SR_RXRDY     0x01         /* Receiver Ready                   */

        /* Interrupt Mask Register */
#define MSMU_IMR_WWT         0x10000      /* Work waiting time expiry IRQ     */
#define MSMU_IMR_TX_COMP     0x08000     /* last byte transfer in a Transfer */
#define MSMU_IMR_RX_RD_ERROR 0x04000      /* Read from a empty FIFO           */
#define MSMU_IMR_TX_WR_ERROR 0x02000      /* TX FIFO full write error         */
#define MSMU_IMR_PAR_ERR     0x01000      /* Parity frame error IRQ           */
#define MSMU_IMR_TX_DONE     0x00200      /* Only for T=1 Protocol            */
#define MSMU_IMR_TX_ERROR    0x00100      /* Parity error on transmission     */
#define MSMU_IMR_TX_READY    0x00080      /* TX FIFO Empty Or NO_CHARS FOR TX */
#define MSMU_IMR_CUR_CTS     0x00040      /* Current CTS                      */
#define MSMU_IMR_DELTA_CTS   0x00020      /* Delta CTS                        */
#define MSMU_IMR_RXLEV       0x00010      /* RX Level exceeded                */
#define MSMU_IMR_RXSTALE     0x00008      /* Stale RX character occurred      */
#define MSMU_IMR_RXBREAK     0x00004      /* RX Break occurred                */
#define MSMU_IMR_RXHUNT      0x00002      /* RX Hunt character received       */
#define MSMU_IMR_TXLEV       0x00001      /* TX Level or below met            */
#define MSMU_IMR_MASK_ALL    0x00000      /* Mask all interrupts              */


        /* Interrupt Status Register */
#define MSMU_ISR_WWT         0x10000      /* Work waiting time expiry IRQ     */
#define MSMU_ISR_TX_COMP     0x08000      /* last byte transfer in a Transfer */
#define MSMU_ISR_RX_RD_ERROR 0x04000      /* Read from a empty FIFO           */
#define MSMU_ISR_TX_WR_ERROR 0x02000      /* TX FIFO full write error         */
#define MSMU_ISR_PAR_ERR     0x01000      /* Parity frame error IRQ           */
#define MSMU_ISR_TX_DONE     0x00200      /* Only for T=1 Protocol            */
#define MSMU_ISR_TX_ERROR    0x00100      /* Parity error on transmission     */
#define MSMU_ISR_TX_READY    0x00080      /* TX FIFO Empty Or NO_CHARS FOR TX */
#define MSMU_ISR_CUR_CTS     0x00040      /* Current CTS                      */
#define MSMU_ISR_DELTA_CTS   0x00020      /* Delta CTS                        */
#define MSMU_ISR_RXLEV       0x00010      /* RX Level exceeded                */
#define MSMU_ISR_RXSTALE     0x00008      /* Stale RX character occurred      */
#define MSMU_ISR_RXBREAK     0x00004      /* RX Break occurred                */
#define MSMU_ISR_RXHUNT      0x00002      /* RX Hunt character received       */
#define MSMU_ISR_TXLEV       0x00001      /* TX Level or below met            */


#define MSMU_MR1_DISABLE_RFR_CTS 0x00  /* Disable RFR and CTS control      */


        /* Mode Register 2 */
#define MSMU_MR2_RX_ERROR_CHAR_OFF 0x200   /* RX Error char off bit       */
#define MSMU_MR2_RX_ERROR_ZERO_CHAR_OFF 0x100   /* RX Error char off bit       */
#define MSMU_MR2_LOOPBACK          0x80    /* Channel Mode                */
#define MSMU_MR2_ERRMODE_CHAR      0x00    /* Error Mode - Character mode */
#define MSMU_MR2_ERRMODE_BLOCK     0x40    /* Error Mode - Block mode     */
#define MSMU_MR2_5BPC              0x00    /* 5 Bits per character        */
#define MSMU_MR2_6BPC              0x10    /* 6 Bits per character        */
#define MSMU_MR2_7BPC              0x20    /* 7 Bits per character        */
#define MSMU_MR2_8BPC              0x30    /* 8 Bits per character        */
#define MSMU_MR2_1SB               0x04    /* 1 Stop Bit                  */
#define MSMU_MR2_2SB               0x0C    /* 2 Stop Bits                 */
#define MSMU_MR2_OPAR              0x01    /* Odd Parity                  */
#define MSMU_MR2_EPAR              0x02    /* Even Parity                 */
#define MSMU_MR2_SPAR              0x03    /* Space Parity                */

        /* Clock Select Register */
#define MSMU_CSR_2BITS_BIT         0xAA     /* 2 bits/bit in sample mode         */
#define MSMU_CSR_4BITS_BIT         0xCC     /* 4 bits/bit in sample mode         */
#define MSMU_CSR_8BITS_BIT         0xEE     /* 8 bits/bit in sample mode         */
#define MSMU_CSR_16BITS_BIT        0xFF     /* baud rate determined by M/N clock
                                                                                                 16 bits per bit in sample mode    */
#define MSMU_CSR_TX_ONLY_MASK      0x0F    /* Mask to change only TX CSR       */
#define MSMU_CSR_RX_ONLY_MASK      0xF0    /* Mask to change only RX CSR       */

        /* Command Register */
#define MSMU_CR_RESET_RX         0x010     /* Reset Receiver                   */
#define MSMU_CR_RESET_TX         0x020     /* Reset Transmitter                */
#define MSMU_CR_RESET_ERR        0x030     /* Reset Error Status               */
#define MSMU_CR_RESET_BRK_INT    0x040     /* Reset Break Change Interrupt     */
#define MSMU_CR_STA_BRK          0x050     /* Start Break                      */
#define MSMU_CR_STO_BRK          0x060     /* Stop Break                       */
#define MSMU_CR_CLR_DCTS         0x070     /* Clear CTS Change (delta)         */
#define MSMU_CR_CLEAR_STALE_EVT  0x080     /* Clear State Event                */
#define MSMU_CR_SAMP_MODE        0x090     /* Sample Data Mode                 */
#define MSMU_CR_TEST_PARITY      0x0A0     /* Test Parity                      */
#define MSMU_CR_TEST_FRAME       0x0B0     /* Test Frame                       */
#define MSMU_CR_RESET_SAMPLE     0x0C0     /* Reset Sample Data Mode           */
#define MSMU_CR_SET_RFR          0x0D0     /* Set RFR                          */
#define MSMU_CR_RESET_RFR        0x0E0     /* Reset RFR                        */
#define MSMU_CR_UART_RESET_INT   0x0F0     /* Reset UART                       */
#define MSMU_CR_CLR_TX_ERROR     0x800     /* Clear TX_ERROR status            */
#define MSMU_CR_RESET_PARERR_INT 0x840     /* reset parity error interrupt     */
#define MSMU_CR_WWT_CNTR_EN      0x870     /* WWT counter enable               */
#define MSMU_CR_WWT_CNTR_DIS     0x880     /* WWT counter disable              */
#define MSMU_CR_CLR_TX_WR_ERR    0x890     /* clear TX write error IRQ         */
#define MSMU_CR_CLR_RX_RD_ERR    0x8A0     /* RX read error IRQ                */
#define MSMU_CR_CLR_TX_COMP      0x8B0     /* clear TX COMP IRQ                */
#define MSMU_CR_CLR_WWT_INT      0x8C0     /* clear WWT IRQ                    */
#define MSMU_CR_DIS_CR_PROTECT   0x200     /* Disable CR protection            */
#define MSMU_CR_CLR_TX_READY     0x300     /* Clears the TX Ready Event        */
#define MSMU_CR_FORCE_STALE_EVT  0x400     /* Force a Stale Event: FLUSH RX Pkr*/
#define MSMU_CR_ENA_STALE_EVT    0x500     /* Enable Stale Event               */
#define MSMU_CR_DIS_TX           0x08      /* Disable Transmitter              */
#define MSMU_CR_ENA_TX           0x04      /* Enable Transmitter               */
#define MSMU_CR_DIS_RX           0x02      /* Disable Receiver                 */
#define MSMU_CR_ENA_RX           0x01      /* Enable Receiver                  */

#define MSMU_BCR_UART_1_4_0            0x37      /* Enable all features for UART1.4.0*/
#define MSMU_BCR_NEGLET_NO_CR_PROT     0x40      /* Enable NEW_CMD_NEGLET_NO_CR_PROT */

#define MSMU_WWT_CYC_REENA_RX                0x2000000      /* Enable WWT auto reenable         */

#define UART_NUM_TX_CHARS_MASK               0xFFFFFF

#define UART_REG_LOG_MAX_ENTRY               200

#ifndef FEATURE_UIM_TEST_FRAMEWORK
/*
 * MACROS FOR UART REGISTERS ON UIM1
*/
#define UIM1_UART_MR1            MSS_UIM0_UART_DM_MR1             /* mode #1 */
#define UIM1_UART_MR2            MSS_UIM0_UART_DM_MR2             /* mode #2 */
#define UIM1_UART_CSR            MSS_UIM0_UART_DM_CSR             /* clock selection */
#define UIM1_UART_SR             MSS_UIM0_UART_DM_SR              /* status */
#define UIM1_UART_CR             MSS_UIM0_UART_DM_CR              /* command */
#define UIM1_UART_MISR           MSS_UIM0_UART_DM_MISR            /* masked interrupt status */
#define UIM1_UART_IMR            MSS_UIM0_UART_DM_IMR             /* interrupt mask */
#define UIM1_UART_ISR            MSS_UIM0_UART_DM_ISR             /* interrupt status */
#define UIM1_UART_IPR            MSS_UIM0_UART_DM_IPR             /* interrupt programming */
#define UIM1_UART_TFWR           MSS_UIM0_UART_DM_TFWR            /* transmit FIFO watermark */
#define UIM1_UART_RFWR           MSS_UIM0_UART_DM_RFWR            /* receive FIFO watermark */

#define UIM1_UART_RX_INIT        MSS_UIM0_UART_DM_DMRX            /* the number of chars */
#define UIM1_UART_IRDA           MSS_UIM0_UART_DM_IRDA            /* for the IRDA function */
#define UIM1_UART_RX_TOTAL_SNAP  MSS_UIM0_UART_DM_RX_TOTAL_SNAP   /* the number of chars received */
#define UIM1_UART_DMEM           MSS_UIM0_UART_DM_DMEN            /* enable DM for RX/TX*/
#define UIM1_UART_NUM_TX_CHARS   MSS_UIM0_UART_DM_NO_CHARS_FOR_TX /* the total number of chars for TX */
#define UIM1_UART_BADR           MSS_UIM0_UART_DM_BADR            /* RX FIFO base address */

#define UIM1_UART_TXFS           MSS_UIM0_UART_DM_TXFS            /* TX FIFO status */
#define UIM1_UART_RXFS           MSS_UIM0_UART_DM_RXFS            /* RX FIFO status */

#define UIM1_UART_TF             MSS_UIM0_UART_DM_TF              /* TX FIFO */
#define UIM1_UART_RF             MSS_UIM0_UART_DM_RF              /* RX FIFO */

#define UIM1_UART_USIM_CFG       MSS_UIM0_UART_DM_SIM_CFG         /* SIM configuration */

#define UIM1_UART_DM_UIM_CFG     MSS_UIM0_UART_DM_UIM_CFG         /* UIM Controller Configuration */

#define UIM1_UART_DM_UIM_CMD            MSS_UIM0_UART_DM_UIM_CMD       /* Command register for UIM Controller */
#define UIM1_UART_DM_IO_STATUS          MSS_UIM0_UART_DM_UIM_IO_STATUS /* Card presence and UIM controller activity status register */
#define UIM1_UART_DM_UIM_CNTRL_MISR     MSS_UIM0_UART_DM_UIM_IRQ_MISR  /* Interrupt status register for UIM controller */
#define UIM1_UART_DM_UIMCNTRL_CLR       MSS_UIM0_UART_DM_UIM_IRQ_CLR   /* Interrupt clear register for UIM controller */
#define UIM1_UART_DM_UIMCNTRL_IMR       MSS_UIM0_UART_DM_UIM_IRQ_IMR   /* Interrupt Mask register for UIM controller */
#define UIM1_UART_DM_UIMCNTRL_CLR_SET   MSS_UIM0_UART_DM_UIM_IRQ_CLR_SET /* Interrupt clear register for UIM controller. read-modify-write */
#define UIM1_UART_DM_UIMCNTRL_IMR_SET   MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET /* Interrupt Mask register for UIM controller. read-modify-write */

#define UIM1_UART_DM_WWT_TIMEOUT MSS_UIM0_UART_DM_WWT_TIMEOUT     /* WWT timeout value programming register */
#define UIM1_UART_DM_CLK_CTRL    MSS_UIM0_UART_DM_CLK_CTRL        /* Clock control register */
#define UIM1_UART_DM_BCR         MSS_UIM0_UART_DM_BCR             /* Backward compatibility register */
#define UIM1_UART_DM_DMRX_DBG    MSS_UIM0_UART_DM_DMRX_DBG        /* Last write to UART_DM_DMRX register */
#define UIM1_UART_DM_HW_VERSION  MSS_UIM0_UART_DM_HW_VERSION      /* UART_DM revision */
#define UIM1_UART_DM_GENERICS    MSS_UIM0_UART_DM_GENERICS        /* UART_DM generics */

/*
 * MACROS FOR UART REGISTERS ON UIM2
*/
#define UIM2_UART_MR1            MSS_UIM1_UART_DM_MR1             /* mode #1 */
#define UIM2_UART_MR2            MSS_UIM1_UART_DM_MR2             /* mode #2 */
#define UIM2_UART_CSR            MSS_UIM1_UART_DM_CSR             /* clock selection */
#define UIM2_UART_SR             MSS_UIM1_UART_DM_SR              /* status */
#define UIM2_UART_CR             MSS_UIM1_UART_DM_CR              /* command */
#define UIM2_UART_MISR           MSS_UIM1_UART_DM_MISR            /* masked interrupt status */
#define UIM2_UART_IMR            MSS_UIM1_UART_DM_IMR             /* interrupt mask */
#define UIM2_UART_ISR            MSS_UIM1_UART_DM_ISR             /* interrupt status */
#define UIM2_UART_IPR            MSS_UIM1_UART_DM_IPR             /* interrupt programming */
#define UIM2_UART_TFWR           MSS_UIM1_UART_DM_TFWR            /* transmit FIFO watermark */
#define UIM2_UART_RFWR           MSS_UIM1_UART_DM_RFWR            /* receive FIFO watermark */

#define UIM2_UART_RX_INIT        MSS_UIM1_UART_DM_DMRX            /* the number of chars */
#define UIM2_UART_IRDA           MSS_UIM1_UART_DM_IRDA            /* for the IRDA function */
#define UIM2_UART_RX_TOTAL_SNAP  MSS_UIM1_UART_DM_RX_TOTAL_SNAP   /* the number of chars received */
#define UIM2_UART_DMEM           MSS_UIM1_UART_DM_DMEN            /* enable DM for RX/TX*/
#define UIM2_UART_NUM_TX_CHARS   MSS_UIM1_UART_DM_NO_CHARS_FOR_TX /* the total number of chars for TX */
#define UIM2_UART_BADR           MSS_UIM1_UART_DM_BADR            /* RX FIFO base address */

#define UIM2_UART_TXFS           MSS_UIM1_UART_DM_TXFS            /* TX FIFO status */
#define UIM2_UART_RXFS           MSS_UIM1_UART_DM_RXFS            /* RX FIFO status */

#define UIM2_UART_TF             MSS_UIM1_UART_DM_TF              /* TX FIFO */
#define UIM2_UART_RF             MSS_UIM1_UART_DM_RF              /* RX FIFO */

#define UIM2_UART_USIM_CFG       MSS_UIM1_UART_DM_SIM_CFG         /* SIM configuration */
#define UIM2_UART_DM_UIM_CFG     MSS_UIM1_UART_DM_UIM_CFG         /* UIM Controller Configuration */

#define UIM2_UART_DM_UIM_CMD             MSS_UIM1_UART_DM_UIM_CMD      /* Command register for UIM Controller */
#define UIM2_UART_DM_IO_STATUS          MSS_UIM1_UART_DM_UIM_IO_STATUS /* Card presence and UIM controller activity status register */
#define UIM2_UART_DM_UIM_CNTRL_MISR     MSS_UIM1_UART_DM_UIM_IRQ_MISR  /* Interrupt status register for UIM controller */
#define UIM2_UART_DM_UIMCNTRL_CLR       MSS_UIM1_UART_DM_UIM_IRQ_CLR   /* Interrupt clear register for UIM controller */
#define UIM2_UART_DM_UIMCNTRL_IMR       MSS_UIM1_UART_DM_UIM_IRQ_IMR   /* Interrupt Mask register for UIM controller */
#define UIM2_UART_DM_UIMCNTRL_CLR_SET   MSS_UIM1_UART_DM_UIM_IRQ_CLR_SET /* Interrupt clear register for UIM controller. read-modify-write */
#define UIM2_UART_DM_UIMCNTRL_IMR_SET   MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET /* Interrupt Mask register for UIM controller. read-modify-write */

#define UIM2_UART_DM_WWT_TIMEOUT MSS_UIM1_UART_DM_WWT_TIMEOUT     /* WWT timeout value programming register */
#define UIM2_UART_DM_CLK_CTRL    MSS_UIM1_UART_DM_CLK_CTRL        /* Clock control register */
#define UIM2_UART_DM_BCR         MSS_UIM1_UART_DM_BCR             /* Backward compatibility register */
#define UIM2_UART_DM_DMRX_DBG    MSS_UIM1_UART_DM_DMRX_DBG        /* Last write to UART_DM_DMRX register */
#define UIM2_UART_DM_HW_VERSION  MSS_UIM1_UART_DM_HW_VERSION      /* UART_DM revision */
#define UIM2_UART_DM_GENERICS    MSS_UIM1_UART_DM_GENERICS        /* UART_DM generics */

/*
 * MACROS FOR UART REGISTERS ON UIM3
*/
#define UIM3_UART_MR1            MSS_UIM2_UART_DM_MR1             /* mode #1 */
#define UIM3_UART_MR2            MSS_UIM2_UART_DM_MR2             /* mode #2 */
#define UIM3_UART_CSR            MSS_UIM2_UART_DM_CSR             /* clock selection */
#define UIM3_UART_SR             MSS_UIM2_UART_DM_SR              /* status */
#define UIM3_UART_CR             MSS_UIM2_UART_DM_CR              /* command */
#define UIM3_UART_MISR           MSS_UIM2_UART_DM_MISR            /* masked interrupt status */
#define UIM3_UART_IMR            MSS_UIM2_UART_DM_IMR             /* interrupt mask */
#define UIM3_UART_ISR            MSS_UIM2_UART_DM_ISR             /* interrupt status */
#define UIM3_UART_IPR            MSS_UIM2_UART_DM_IPR             /* interrupt programming */
#define UIM3_UART_TFWR           MSS_UIM2_UART_DM_TFWR            /* transmit FIFO watermark */
#define UIM3_UART_RFWR           MSS_UIM2_UART_DM_RFWR            /* receive FIFO watermark */

#define UIM3_UART_RX_INIT        MSS_UIM2_UART_DM_DMRX            /* the number of chars */
#define UIM3_UART_IRDA           MSS_UIM2_UART_DM_IRDA            /* for the IRDA function */
#define UIM3_UART_RX_TOTAL_SNAP  MSS_UIM2_UART_DM_RX_TOTAL_SNAP   /* the number of chars received */
#define UIM3_UART_DMEM           MSS_UIM2_UART_DM_DMEN            /* enable DM for RX/TX*/
#define UIM3_UART_NUM_TX_CHARS   MSS_UIM2_UART_DM_NO_CHARS_FOR_TX /* the total number of chars for TX */
#define UIM3_UART_BADR           MSS_UIM2_UART_DM_BADR            /* RX FIFO base address */

#define UIM3_UART_TXFS           MSS_UIM2_UART_DM_TXFS            /* TX FIFO status */
#define UIM3_UART_RXFS           MSS_UIM2_UART_DM_RXFS            /* RX FIFO status */

#define UIM3_UART_TF             MSS_UIM2_UART_DM_TF              /* TX FIFO */
#define UIM3_UART_RF             MSS_UIM2_UART_DM_RF              /* RX FIFO */

#define UIM3_UART_USIM_CFG       MSS_UIM2_UART_DM_SIM_CFG         /* SIM configuration */
#define UIM3_UART_DM_UIM_CFG     MSS_UIM2_UART_DM_UIM_CFG         /* UIM Controller Configuration */

#define UIM3_UART_DM_UIM_CMD            MSS_UIM2_UART_DM_UIM_CMD         /* Command register for UIM Controller */
#define UIM3_UART_DM_IO_STATUS          MSS_UIM2_UART_DM_UIM_IO_STATUS   /* Card presence and UIM controller activity status register */
#define UIM3_UART_DM_UIM_CNTRL_MISR     MSS_UIM2_UART_DM_UIM_IRQ_MISR    /* Interrupt status register for UIM controller */
#define UIM3_UART_DM_UIMCNTRL_CLR       MSS_UIM2_UART_DM_UIM_IRQ_CLR     /* Interrupt clear register for UIM controller */
#define UIM3_UART_DM_UIMCNTRL_IMR       MSS_UIM2_UART_DM_UIM_IRQ_IMR     /* Interrupt Mask register for UIM controller */
#define UIM3_UART_DM_UIMCNTRL_CLR_SET   MSS_UIM2_UART_DM_UIM_IRQ_CLR_SET /* Interrupt clear register for UIM controller. read-modify-write */
#define UIM3_UART_DM_UIMCNTRL_IMR_SET   MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET /* Interrupt Mask register for UIM controller. read-modify-write */

#define UIM3_UART_DM_WWT_TIMEOUT MSS_UIM2_UART_DM_WWT_TIMEOUT     /* WWT timeout value programming register */
#define UIM3_UART_DM_CLK_CTRL    MSS_UIM2_UART_DM_CLK_CTRL        /* Clock control register */
#define UIM3_UART_DM_BCR         MSS_UIM2_UART_DM_BCR             /* Backward compatibility register */
#define UIM3_UART_DM_DMRX_DBG    MSS_UIM2_UART_DM_DMRX_DBG        /* Last write to UART_DM_DMRX register */
#define UIM3_UART_DM_HW_VERSION  MSS_UIM2_UART_DM_HW_VERSION      /* UART_DM revision */
#define UIM3_UART_DM_GENERICS    MSS_UIM2_UART_DM_GENERICS        /* UART_DM generics */

/* MASKS AND SHIFTS FOR SIM CONFIGURATION REGISTER */
#define UART_DEFAULT_SIM_CFG_MASK_RX_VAL     (1<<HWIO_MSS_UIM0_UART_DM_SIM_CFG_MASK_RX_SHFT)
#define UART_SIM_CFG_WH__SWAP_D_MASK         HWIO_FMSK(UIM1_UART_USIM_CFG, SWAP_D)
#define UART_SIM_CFG_WH__INV_D_MASK          HWIO_FMSK(UIM1_UART_USIM_CFG, INV_D)
#define UART_SIM_CFG_TX_RX_MODE_MASK        (HWIO_FMSK(UIM1_UART_USIM_CFG, UIM_TX_MODE) | \
                                                 HWIO_FMSK(UIM1_UART_USIM_CFG, UIM_RX_MODE))
#define UART_SIM_CFG__STOP_BIT_LEN_MASK      HWIO_FMSK(UIM1_UART_USIM_CFG, SIM_STOP_BIT_LEN)
#define UART_SIM_CFG__STOP_BIT_LEN_SHFT      HWIO_MSS_UIM0_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_SHFT
#define UIM_DEFAULT_STOP_BIT_VALUE          (UIM_DEFLT_GUARDTIME_BITS<<UART_SIM_CFG__STOP_BIT_LEN_SHFT)
#define UART_SIM_CFG__CLK_MASK               HWIO_MSS_UIM0_UART_DM_SIM_CFG_SIM_CLK_ON_BMSK | \
                                                 HWIO_MSS_UIM0_UART_DM_SIM_CFG_SIM_CLK_TD8_SEL_BMSK | \
                                                 HWIO_MSS_UIM0_UART_DM_SIM_CFG_SIM_CLK_STOP_HIGH_BMSK
#define UART_SIM_CFG__SIM_CLK_STOP_HI_MASK   HWIO_FMSK(UIM1_UART_USIM_CFG,SIM_CLK_STOP_HIGH)
#define UART_SIM_CFG__SIM_CLK_ON_MASK        HWIO_FMSK(UIM1_UART_USIM_CFG,SIM_CLK_ON)
#define UART_SIM_CFG__SIM_CLK_TD8_SEL_MASK   HWIO_FMSK(UIM1_UART_USIM_CFG,SIM_CLK_TD8_SEL)
#define UART_SIM_CFG__MASK_RX                HWIO_FMSK(UIM1_UART_USIM_CFG,MASK_RX)
#define UART_SIM_CFG__MASK                  (HWIO_FMSK(UIM1_UART_USIM_CFG,MASK_RX) | \
                                                 HWIO_FMSK(UIM1_UART_USIM_CFG,SWAP_D)  | \
                                                 HWIO_FMSK(UIM1_UART_USIM_CFG,INV_D)   | \
                                                 HWIO_FMSK(UIM1_UART_USIM_CFG,SIM_SEL))
#define UART_SIM_CFG__SIM_SEL_MASK           HWIO_FMSK(UIM1_UART_USIM_CFG,SIM_SEL)
#define UART_SIM_CFG__SIM_CLK_OFF            0
#define UART_SIM_CFG__SIM_CLK_TD4_SEL_MASK   0

/* MASKS AND SHIFTS FOR FIFO REGISTERS */
#define UART_DM_RXFS__RXFIFO_STATE_LSB_MASK  HWIO_FMSK(UIM1_UART_RXFS, RX_FIFO_STATE_LSB)
#define UART_DM_RXFS__RX_BUFFER_STATE_MASK   HWIO_FMSK(UIM1_UART_RXFS, RX_BUFFER_STATE)
#define UART_DM_RXFS__RXFIFO_STATE_LSB_SHFT  HWIO_SHFT(UIM1_UART_RXFS, RX_FIFO_STATE_LSB)
#define UART_DM_RXFS__RX_BUFFER_STATE_SHFT   HWIO_SHFT(UIM1_UART_RXFS, RX_BUFFER_STATE)

#define UART_DM_TXFS__TXFIFO_STATE_LSB_MASK  HWIO_FMSK(UIM1_UART_TXFS, TX_FIFO_STATE_LSB)
#define UART_DM_TXFS__TX_BUFFER_STATE_MASK   HWIO_FMSK(UIM1_UART_TXFS, TX_BUFFER_STATE)
#define UART_DM_TXFS__TXFIFO_STATE_LSB_SHFT  HWIO_SHFT(UIM1_UART_TXFS, TX_FIFO_STATE_LSB)
#define UART_DM_TXFS__TX_BUFFER_STATE_SHFT   HWIO_SHFT(UIM1_UART_TXFS, TX_BUFFER_STATE)

typedef enum
{
  UART_DM_MR1                             = 0x0000 ,
  UART_DM_MR2                             = 0x0004 ,
  UART_DM_CSR_SR_DEPRECATED               = 0x0008 ,
  UART_DM_CR_MISR_DEPRECATED              = 0x0010 ,
  UART_DM_IMR_ISR_DEPRECATED              = 0x0014 ,
  UART_DM_IPR                             = 0x0018 ,
  UART_DM_TFWR                            = 0x001C ,
  UART_DM_RFWR                            = 0x0020 ,
  UART_DM_HCR                             = 0x0024 ,
  UART_DM_DMRX                            = 0x0034 ,
  UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED   = 0x0038 ,
  UART_DM_DMEN                            = 0x003C ,
  UART_DM_NO_CHARS_FOR_TX                 = 0x0040 ,
  UART_DM_BADR                            = 0x0044 ,
  UART_DM_TESTSL                          = 0x0048 ,
  UART_DM_TXFS                            = 0x004C ,
  UART_DM_RXFS                            = 0x0050 ,
  UART_DM_MISR_MODE                       = 0x0060 ,
  UART_DM_MISR_RESET                      = 0x0064 ,
  UART_DM_MISR_EXPORT                     = 0x0068 ,
  UART_DM_MISR_VAL                        = 0x006C ,
  UART_DM_TF_RF_DEPRECATED                = 0x0070 ,
  UART_DM_TF_RF_2_DEPRECATED              = 0x0074 ,
  UART_DM_TF_RF_3_DEPRECATED              = 0x0078 ,
  UART_DM_TF_RF_4_DEPRECATED              = 0x007C ,
  UART_DM_SIM_CFG                         = 0x0080 ,
  UART_DM_TEST_WR_ADDR                    = 0x0084 ,
  UART_DM_TEST_WR_DATA                    = 0x0088 ,
  UART_DM_TEST_RD_ADDR                    = 0x008C ,
  UART_DM_TEST_RD_DATA                    = 0x0090 ,
  UART_DM_CSR                             = 0x00A0 ,
  UART_DM_SR                              = 0x00A4 ,
  UART_DM_CR                              = 0x00A8 ,
  UART_DM_MISR                            = 0x00AC ,
  UART_DM_IMR                             = 0x00B0 ,
  UART_DM_ISR                             = 0x00B4 ,
  UART_DM_IRDA                            = 0x00B8 ,
  UART_DM_RX_TOTAL_SNAP                   = 0x00BC ,
  UART_DM_WWT_TIMEOUT                     = 0x00C0 ,
  UART_DM_CLK_CTRL                        = 0x00C4 ,
  UART_DM_BCR                             = 0x00C8 ,
  UART_DM_RX_TRANS_CTRL                   = 0x00CC ,
  UART_DM_DMRX_DBG                        = 0x00D0 ,
  UART_DM_FSM_STATUS                      = 0x00D4 ,
  UART_DM_HW_VERSION                      = 0x00D8 ,
  UART_DM_GENERICS                        = 0x00DC ,
  UART_DM_TF                              = 0x0100 ,
  UART_DM_TF_2                            = 0x0104 ,
  UART_DM_TF_3                            = 0x0108 ,
  UART_DM_TF_4                            = 0x010C ,
  UART_DM_TF_5                            = 0x0110 ,
  UART_DM_TF_6                            = 0x0114 ,
  UART_DM_TF_7                            = 0x0118 ,
  UART_DM_TF_8                            = 0x011C ,
  UART_DM_TF_9                            = 0x0120 ,
  UART_DM_TF_10                           = 0x0124 ,
  UART_DM_TF_11                           = 0x0128 ,
  UART_DM_TF_12                           = 0x012C ,
  UART_DM_TF_13                           = 0x0130 ,
  UART_DM_TF_14                           = 0x0134 ,
  UART_DM_TF_15                           = 0x0138 ,
  UART_DM_TF_16                           = 0x013C ,
  UART_DM_RF                              = 0x0140 ,
  UART_DM_RF_2                            = 0x0144 ,
  UART_DM_RF_3                            = 0x0148 ,
  UART_DM_RF_4                            = 0x014C ,
  UART_DM_RF_5                            = 0x0150 ,
  UART_DM_RF_6                            = 0x0154 ,
  UART_DM_RF_7                            = 0x0158 ,
  UART_DM_RF_8                            = 0x015C ,
  UART_DM_RF_9                            = 0x0160 ,
  UART_DM_RF_10                           = 0x0164 ,
  UART_DM_RF_11                           = 0x0168 ,
  UART_DM_RF_12                           = 0x016C ,
  UART_DM_RF_13                           = 0x0170 ,
  UART_DM_RF_14                           = 0x0174 ,
  UART_DM_RF_15                           = 0x0178 ,
  UART_DM_RF_16                           = 0x017C ,
  UART_DM_UIM_CFG                         = 0x0180 ,
  UART_DM_UIM_CMD                         = 0x0184 ,
  UART_DM_UIM_IO_STATUS                   = 0x0188 ,
  UART_DM_UIM_IRQ_ISR                     = 0x018C ,
  UART_DM_UIM_IRQ_MISR                    = 0x0190 ,
  UART_DM_UIM_IRQ_CLR                     = 0x0194 ,
  UART_DM_UIM_IRQ_IMR                     = 0x0198 ,
  UART_DM_UIM_IRQ_IMR_SET                 = 0x019C ,
  UART_DM_UIM_IRQ_IMR_CLR                 = 0x01A0
}uartdm_register_map_type;

#define VALIDATE_UIM_UART_START_ADDRESS(uim_instance)       \
  if (0x00 == uim_hw_if.uart[uim_instance].start_addr )     \
  {                                                         \
    return;                                                 \
  }
#define VALIDATE_UIM_UART_START_ADDRESS_ELSE_RETURN(uim_instance, value_to_return)   \
  if (0x00 == uim_hw_if.uart[uim_instance].start_addr )                              \
  {                                                                                  \
    return value_to_return;                                                          \
  }
#define UIM_HWIO_IN(addr)  (*((volatile uint32 *) (addr)))
#define UIM_HWIO_OUT(addr, val)  (*((volatile uint32 *) (addr)) = ((uint32) (val)))

#else
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

typedef enum {
    UIM_REG_WRITE = 0x0 ,
    UIM_REG_READ  = 0x1
}uartdm_register_log_action_type;

typedef struct{
  uartdm_register_map_type         reg_name;   /* Name of the register */
  uartdm_register_log_action_type  action;     /* Write or Read */
  dword                            value;      /* Value */
}uartdm_trans_log_type;

typedef struct{
  uint32                     active_index;
  uartdm_trans_log_type      trans_log[UART_REG_LOG_MAX_ENTRY];
}uim_reg_log_type;


/* Bits in the UART_DM_UIM_CFG register */
#define UIM_EVENT_DEBOUNCE_TIME         0x00000003
#define UIM_CARD_EVENTS_EN              0x00000040
#define UIM_REMOVED_TRIGGER_EN          0x00000100
#define UIM_BATT_ALARM_TRIGGER_EN       0x00000200
#define UIM_PMIC_ALARM_EN               0x00000400
#define UIM_MODE_18                     0x00001000

#define UIM_PRESENT_POLARITY_BIT        0x00000020
#define UIM_SET_PRESENT_POLARITY_BIT    0x00000020
#define UIM_CLR_PRESENT_POLARITY_BIT    0xFFFFFFDF


#define UART_IRDA_DISABLE          0x00 /* Disable UART IRDA transceiver */

/*
 * DEFINITIONS FOR UARTDM CLK CNTRL register
 */
#define UIM_CLK_CTRL__RX_ENABLE_CGC_OPT   0x20 /* clock gating for RX enable */
#define UIM_CLK_CTRL__TX_ENABLE_CGC_OPT   0x10 /* clock gating for TX enable */
#define UIM_CLK_CTRL__DISABLE_CGC         0x00 /* disable clock gating */

/* Table holding the default UARTDM instance number for different UIM instances. */
extern const uim_uartdm_num_table_type   uim_uartdm_num_table[];

 /* Per slot macros */
  /* Program the UART command register and update the shadow value */

  #define UIM_PRG_CR_SLOT(uim_ptr, cr_val)                    prgCR(uim_ptr->id, cr_val);

  #define UIM_PRG_MR1_SLOT( uim_ptr, mr1_val )                prgMR1(uim_ptr->id, mr1_val);

  #define UIM_PRG_MR2_SLOT( uim_ptr, mr2_val )                prgMR2(uim_ptr->id, mr2_val);

  #define UIM_PRG_UART_CSR_SLOT( uim_ptr, csr_val )           prgCSR(uim_ptr->id, csr_val);

  #define UIM_READ_STATUS_SLOT( uim_ptr )                     readStatus(uim_ptr->id)

  #define UIM_RESET_RX_SLOT( uim_ptr )                        prgCR(uim_ptr->id, MSMU_CR_RESET_RX)

  #define UIM_RESET_TX_SLOT( uim_ptr )                        prgCR(uim_ptr->id, MSMU_CR_RESET_TX)

  #define UIM_RESET_ERR_SLOT( uim_ptr )                       prgCR(uim_ptr->id, MSMU_CR_RESET_ERR)

  #define UIM_RESET_BREAK_SLOT( uim_ptr )                     prgCR(uim_ptr->id, MSMU_CR_RESET_BRK_INT)

  #define UIM_START_BREAK_SLOT( uim_ptr )                     prgCR(uim_ptr->id, MSMU_CR_STA_BRK)

  #define UIM_STOP_BREAK_SLOT( uim_ptr )                      prgCR(uim_ptr->id, MSMU_CR_STO_BRK)

  #define UIM_RESET_TX_ERROR_SLOT( uim_ptr )                  prgCR(uim_ptr->id, MSMU_CR_CLR_TX_ERROR)

  #define UIM_SAMPLE_MODE_SLOT( uim_ptr )                     prgCR(uim_ptr->id, MSMU_CR_SAMP_MODE)

  #define UIM_DISABLE_UART_TX_SLOT( uim_ptr )                 prgCR(uim_ptr->id, MSMU_CR_DIS_TX)

  #define UIM_ENABLE_UART_TX_SLOT( uim_ptr )                  prgCR(uim_ptr->id, MSMU_CR_ENA_TX)

  #define UIM_DISABLE_UART_RX_SLOT( uim_ptr )                 prgCR(uim_ptr->id, MSMU_CR_DIS_RX)

  #define UIM_ENABLE_UART_RX_SLOT( uim_ptr )                  prgCR(uim_ptr->id, MSMU_CR_ENA_RX)

  #define UIM_RESET_SAMPLE_MODE_SLOT( uim_ptr )               prgCR(uim_ptr->id, MSMU_CR_RESET_SAMPLE)

  #define UIM_ENABLE_RX_STALE_INT_SLOT( uim_ptr )             prgCR(uim_ptr->id, MSMU_CR_ENA_STALE_EVT)

  #define UIM_CLEAR_RX_STALE_INT_SLOT( uim_ptr )              prgCR(uim_ptr->id, MSMU_CR_CLEAR_STALE_EVT)

  #define UIM_FORCE_RX_STALE_INT_SLOT( uim_ptr )              prgCR(uim_ptr->id, MSMU_CR_FORCE_STALE_EVT)

  #define UIM_CLEAR_TX_READY_INT_SLOT( uim_ptr )              prgCR(uim_ptr->id, MSMU_CR_CLR_TX_READY)

  #define UIM_CLEAR_TX_COMP_INT_SLOT( uim_ptr )               prgCR(uim_ptr->id, MSMU_CR_CLR_TX_COMP)

  #define UIM_DISABLE_CR_PROTECTION_SLOT( uim_ptr )           prgCR(uim_ptr->id, MSMU_CR_DIS_CR_PROTECT)

  #define UIM_RESET_UART_DM_SLOT( uim_ptr )                   prgCR(uim_ptr->id, MSMU_CR_CLR_TX_READY);   \
                                                              prgCR(uim_ptr->id, MSMU_CR_DIS_TX);         \
                                                              prgCR(uim_ptr->id, MSMU_CR_DIS_RX)

  #define UIM_READ_MISR_SLOT( uim_ptr )                       readMISR(uim_ptr->id)

  #define UIM_PRG_IMR_SLOT( uim_ptr, imr_val )                prgIMR(uim_ptr->id, imr_val)

  #define UIM_READ_ISR_SLOT( uim_ptr )                        readISR(uim_ptr->id)

  #define UIM_PRG_IPR_SLOT( uim_ptr, ipr_val )                prgIPR(uim_ptr->id, ipr_val)

  #define UIM_TX_WATERLEVEL_SLOT( uim_ptr, tfwr_val )         prgTxWaterlevel(uim_ptr->id, tfwr_val)

  #define UIM_RX_WATERLEVEL_SLOT( uim_ptr, rfwr_val )         prgRxWaterlevel(uim_ptr->id, rfwr_val)

  #define UIM_DM_RX_INIT_SLOT( uim_ptr, rx_chars )            prgRXInit(uim_ptr->id, rx_chars)

  #define UIM_GET_DYNAMIC_SIZE_IN_RX_SLOT( uim_ptr )          readRxInit(uim_ptr->id)

  #define UIM_DISABLE_IRDA_SLOT( uim_ptr )                    disableIRDA(uim_ptr->id)

  #define UIM_GET_NUM_BYTES_IN_RX_SLOT( uim_ptr )             getNumBytesInRX(uim_ptr->id)

  #define UIM_ENABLE_RX_CHARMODE_UARTDM_SLOT( uim_ptr )       enableRxCharMode(uim_ptr->id)

  #define UIM_DISABLE_DATA_MOVER_SLOT( uim_ptr )

  #define UIM_RESET_PARITY_SLOT(uim_ptr)

  #define UIM_NUM_CHARS_TO_SEND_SLOT( uim_ptr, tx_num )       numCharToSend(uim_ptr->id, tx_num)

  #define UIM_GET_NUM_CHARS_TO_SEND_SLOT( uim_ptr )           (getnumCharToSend(uim_ptr->id) & UART_NUM_TX_CHARS_MASK)

  #define UIM_PRG_UART_BADR_SLOT(uim_ptr, badr_val)           prgBADR(uim_ptr->id, badr_val)

  #define UIM_GET_NUM_BYTES_WAITING_IN_TX_SLOT( uim_ptr )     getNumBytesWaitingInTx(uim_ptr->id)

  #define UIM_GET_NUM_BYTES_WAITING_IN_RX_SLOT( uim_ptr )     getNumBytesWaitingInRx(uim_ptr->id)

  #define UIM_SEND_TX_FIFO_SLOT( uim_ptr, tx_value )          sendWord(uim_ptr->id, tx_value)

  #define UIM_GET_RX_WORD_SLOT( uim_ptr )                     getWord(uim_ptr->id)

  /* should be removed with legacy uart removal */
  #define UIM_GET_RX_BYTE_SLOT( uim_ptr )                     (getWord(uim_ptr->id) & 0xFF)


  #define UIM_UART_CNFG_SLOT( uim_ptr, cnfg_mask, cnfg_val )  prgSIMCFG(uim_ptr->id, cnfg_mask, cnfg_val)

  #define UIM_PROGRAM_INVERSE_CONVENTION_SLOT( uim_ptr )                                                 \
          prgSIMCFG ( uim_ptr->id, UART_SIM_CFG_WH__SWAP_D_MASK |                                        \
                                         UART_SIM_CFG_WH__INV_D_MASK,                                    \
                                         UART_SIM_CFG_WH__SWAP_D_MASK |                                  \
                                         UART_SIM_CFG_WH__INV_D_MASK );                                  \
          UIM_PRG_MR2_SLOT ( uim_ptr, MSMU_MR2_8BPC | MSMU_MR2_2SB | MSMU_MR2_OPAR |                     \
                              MSMU_MR2_RX_ERROR_ZERO_CHAR_OFF);                                          \
          uim_ptr->state.convention = UIM_INVERSE_CONVENTION

#define UIM_PROGRAM_DIRECT_CONVENTION_SLOT( uim_ptr )                                                    \
          prgSIMCFG ( uim_ptr->id, UART_SIM_CFG_WH__SWAP_D_MASK |                                        \
                                         UART_SIM_CFG_WH__INV_D_MASK,                                    \
                                         UART_SIM_CFG__SIM_CLK_OFF );                                    \
          UIM_PRG_MR2_SLOT ( uim_ptr, MSMU_MR2_8BPC | MSMU_MR2_2SB | MSMU_MR2_EPAR |                     \
                             MSMU_MR2_RX_ERROR_ZERO_CHAR_OFF);                                           \
          uim_ptr->state.convention = UIM_DIRECT_CONVENTION


/* UARTDM configuration functions */
void initialize_uartdm(uim_instance_enum_type uim_instance);

void prgMR1(uim_instance_enum_type uim_instance, dword val);

    /*
      UART_DM_MR2, Access: Read\Write
      This register is used to configure the UART operation mode which contains
      information about the bits_per_char, stop_bit_len, error mode, parity mode
      and setting of dropping of Parity or framing error character or Zero
      character into the RX FIFO.
    */
void  prgMR2(uim_instance_enum_type uim_instance, dword val);
    /*
      UART_DM_CSR, Access: Write
      UART clock selection register. This register is used in conjunction with
      the UART M/N counter registers to determine the bit rate for the
      transmitter and receiver.
      The transmitter and receiver can be clocked at different rates
    */
void  prgCSR(uim_instance_enum_type uim_instance, dword val);

    /*
      UART_DM_SR, Access: Read
      UART status registers. This register is used to obtain the current State
      (RX Ready, RX Full, TX Ready, TX empty, Overrun, Parity error, Rx break,
      Hunt Char) of the UART subsystem. This register is updated asynchronously.
    */
dword readStatus(uim_instance_enum_type uim_instance);

    /*
      UART_DM_CR, Access: Write
      UART command registers.This register is used to issue specific commands
      (such as reset RX\TX\error\steal interrupt etc) to the UART subsystem.
      This register is updated asynchronously,
      register enables the IRDA function,
      Note:
        Do not reset the transmitter and disable it at the same time.
        Do not reset the receiver and disable it at the same time.
    */
void prgCR(uim_instance_enum_type uim_instance, dword  val);


    /*
     UART_DM_ISR, Access: Read
     UART interrupt status register. This register provides the current status
     of the possible interrupt conditions (PAR_ERR, RX break/lev/stale/hunt ,
     Tx Done\Error\Ready\Lev etc). If one of these bits is set (1), and the
     corresponding bit in the UART_DM_IMR register is set (1), an interrupt
     is generated (with the exception of bit 6, CURRENT_CTS).
     If the corresponding bit in the UART_DM_IMR register is clear (0), setting
     one of these bits has no effect on the UART interrupt request signal.
     This register is updated asynchronously
    */
dword readISR(uim_instance_enum_type uim_instance);

    /*
      UART_DM_IMR, Access: Write
      UART interrupt mask register. This register is used to enable the
      corresponding functions in the UART_DM_ISR register
    */
void  prgIMR(uim_instance_enum_type uim_instance, dword val);

    /*
      UART_DM_MISR, Access: Read
      Masked interrupt status register. A read from this register returns the
      AND of the UART_DM_ISR and the UART_DM_IMR registers.
      This register is updated asynchronously
    */
dword readMISR(uim_instance_enum_type uim_instance);

    /*
      UART_DM_IPR Access: Read\Write
      UART interrupt programming register, This register is used to configure
      stale_timeout info.
    */
void  prgIPR(uim_instance_enum_type uim_instance, dword val);

    /*
      UART_DM_TFWR Access: Read\Write
      This register is used to configure transmit Watermarks info
    */
void  prgTxWaterlevel(uim_instance_enum_type uim_instance, dword val);

    /*
      UART_DM_RFWR Access: Read\Write
      This register is used to configure receive Watermarks info
    */
void  prgRxWaterlevel(uim_instance_enum_type uim_instance, dword val);

    /*
      UART_DM_DMRX Access: Read\Write
      This register is used to configure number of chars to be received in
      new Rx transfer initialization
    */
void  prgRXInit(uim_instance_enum_type uim_instance, dword val);
dword readRxInit(uim_instance_enum_type uim_instance);

    /*
      UART_DM_IRDA, Access: Write
      This register enables the IRDA function,
      Currently, it is not require for UIM communication
    */
void disableIRDA(uim_instance_enum_type uim_instance);

    /*
      UART_DM_RX_TOTAL_SNAP Access: Read
      It holds the number of characters received since the end of last Rx transfer.
      Rx transfer ends when one of the conditions is met:
       1. the number of characters which were received since the end of the
          previous transfer equals the value which was written to DMRX at
          Transfer initialization.
       2. A stale event occurred
    */
dword getNumBytesInRX(uim_instance_enum_type uim_instance);

    /*
      UART_DM_NO_CHARS_FOR_TX Access: Read\Write
      This register contains total number of characters for transmission.
      It is used by the transmitter to calculate how many characters to
      transmit in the last word
    */
void  numCharToSend(uim_instance_enum_type uim_instance, dword val);
dword getnumCharToSend(uim_instance_enum_type uim_instance);

    /*
      UART_DM_BADR Access: Read\Write
      This register is used to configure the base address of RX FIFO
    */
void prgBADR(uim_instance_enum_type uim_instance, dword val);

    /*
      UART_DM_TXFS Access: Read
      This register contains number of bytes waiting in TX FIFO
    */
uint32 getNumBytesWaitingInTx(uim_instance_enum_type uim_instance);

    /*
      UART_DM_RXFS Access: Read
      This register contains number of bytes waiting in RX FIFO
    */
uint32 getNumBytesWaitingInRx(uim_instance_enum_type uim_instance);

    /*
      UART_DM_TF Access: Write
      This register is used to send word to TX FIFO
    */
void sendWord(uim_instance_enum_type uim_instance, dword val);

    /*
      UART_DM_RF Access: Read
      This register is used to receive word from RX FIFO, After the read is
      completed, the FIFO read pointer is updated to make the next character
      available.
    */
dword getWord(uim_instance_enum_type uim_instance);

    /*
      UART_DM_SIM_CFG Access: Read\Write
      UART_SIM_CFG register is responsible for configuring SIM/UIM  mode such
      as direct\indirect convention, Clock Stop High or Low, UIM TX\RX mode etc..
    */
void prgSIMCFG(uim_instance_enum_type uim_instance, dword cnfg_mask, dword cnfg_val);
void prgUIMCFG(uim_instance_enum_type uim_instance, uint32 uim_cfg_reg_val, boolean mask_required, uint32 mask_value, boolean set_clr);


/* Enable RX Char mode */
void enableRxCharMode(uim_instance_enum_type uim_instance);

void uim_uartdm_uim_controller_config(uim_instance_global_type *uim_ptr, boolean hotswap_enabled);
void uim_uartdm_uim_controller_mask_carddet_int(uim_instance_global_type *uim_ptr);
void uim_uartdm_uim_controller_clear_carddet_int(uim_instance_global_type *uim_ptr);
void uim_uartdm_uim_controller_init_hw_deact(uim_instance_enum_type uim_instance);
void uim_uartdm_uim_controller_recover(uim_instance_global_type *uim_ptr);
void uim_uartdm_set_wwt_val(uim_instance_global_type *uim_ptr, uint32 wwt_val);
void uim_uartdm_enable_wwt_counter(uim_instance_global_type *uim_ptr);
void uim_uartdm_disable_wwt_counter(uim_instance_global_type *uim_ptr);
void uim_uartdm_clear_wwt_int_status(uim_instance_global_type *uim_ptr);
void uim_set_hotswap_polarity(uim_instance_enum_type uim_instance, DALGpioValueType value);
uim_card_swap_enum_type uim_uartdm_uim_controller_card_presence(uim_instance_global_type *uim_ptr);
void uim_uartdm_uim_controller_card_ins_int(uim_instance_enum_type uim_instance, DALGpioValueType uim_ins_level);
void uim_uartdm_uim_controller_card_rem_int(uim_instance_enum_type uim_instance, DALGpioValueType uim_ins_level);
int32 uim_bytes_waiting_in_rxfifo(uim_instance_global_type *uim_ptr);
int32 uim_bytes_waiting_in_txfifo(uim_instance_global_type *uim_ptr);
void UIM_SET_UARTDM_ACTIVE_MODE(uim_instance_global_type *uim_ptr);
void UIM_SET_UARTDM_INACTIVE_MODE(uim_instance_global_type *uim_ptr);
void UIM_SET_UARTDM_BCR_REG_PER_VER(uim_instance_global_type *uim_ptr);
void uim_prg_uart_csr_by_div_factor(uint32 bit_clk_div_factor, 
                                    uim_instance_enum_type uim_instance);
#endif /* UIMDRV_UARTDM_H */
