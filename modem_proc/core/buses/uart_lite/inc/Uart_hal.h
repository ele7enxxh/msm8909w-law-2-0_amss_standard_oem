#ifndef UART_HAL_H
#define UART_HAL_H
/*==================================================================================================

FILE: Uart_hal.h

DESCRIPTION: Defines the HAL layer definitions and functions for the UART driver.

                           Copyright (c) 2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/

/*==================================================================================================
Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/uart_lite/inc/Uart_hal.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/25/13   VV      Included Get SNAP, Enable interrupt and get MISR APIs.
08/28/13   VV      Changing header Uart_comdefs.h to Uart_ComDefs.h
06/24/13   VV      Initial revision.

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "HALhwio.h"
#include "HALcomdef.h"
#include "Uart_ComDefs.h"
#include "Uart_hwio.h"

/*==================================================================================================
                                               MACROS
==================================================================================================*/

#define REG_IN(base, hwiosym) HWIO_##hwiosym##_IN(base)

#define REG_OUT(base, hwiosym, value) HWIO_##hwiosym##_OUT(base,value)

/*==================================================================================================
                                               CONSTANTS
==================================================================================================*/

#define TX_FIFO_RATIO   0.5
#define RX_XFER_SIZE   4096

/* Command Register */
#define UARTDM_CR_ENA_RX         0x00000001   // Enable Receiver
#define UARTDM_CR_DIS_RX         0x00000002   // Disable Receiver
#define UARTDM_CR_ENA_TX         0x00000004   // Enable Transmitter
#define UARTDM_CR_DIS_TX         0x00000008   // Disable Transmitter
#define UARTDM_CR_RESET_RX       0x00000010   // Reset Receiver
#define UARTDM_CR_RESET_TX       0x00000020   // Reset Transmitter
#define UARTDM_CR_RESET_ERR      0x00000030   // Reset Error Status
#define UARTDM_CR_STA_BRK        0x00000050   // Start Break
#define UARTDM_CR_STO_BRK        0x00000060   // Stop Break
#define UARTDM_CR_RESET_STALE    0x00000080   // Clears the stale interrupt
#define UARTDM_CR_SET_RFR        0x000000D0   // Set RFR
#define UARTDM_CR_RESET_RFR      0x000000E0   // Reset RFR
#define UARTDM_CR_ENA_CR_PROT    0x00000100   // Enable CR protection
#define UARTDM_CR_DIS_CR_PROT    0x00000200   // Disable CR protection
#define UARTDM_CR_ENA_STALE_EVT  0x00000500   // Enables Stale Event
#define UARTDM_CR_DIS_STALE_EVT  0x00000600   // Disables Stale Event

/* Interrupt Status Register */
#define UARTDM_ISR_TX_DONE    HWIO_UART_DM_ISR_TX_DONE_BMSK         // TX Done
#define UARTDM_ISR_TX_ERROR   HWIO_UART_DM_ISR_TX_ERROR_BMSK        // TX Error
#define UARTDM_ISR_TX_READY   HWIO_UART_DM_ISR_TX_READY_BMSK        // TX Ready
#define UARTDM_ISR_CUR_CTS    HWIO_UART_DM_ISR_CURRENT_CTS_BMSK     // Current CTS
#define UARTDM_ISR_DELTA_CTS  HWIO_UART_DM_ISR_DELTA_CTS_BMSK       // Delta CTS
#define UARTDM_ISR_RXLEV      HWIO_UART_DM_ISR_RXLEV_BMSK           // RX Level exceeded
#define UARTDM_ISR_RXSTALE    HWIO_UART_DM_ISR_RXSTALE_BMSK         // Stale RX character occurred
#define UARTDM_ISR_RXBREAK    HWIO_UART_DM_ISR_RXBREAK_CHANGE_BMSK  // RX Break occurred
#define UARTDM_ISR_RXHUNT     HWIO_UART_DM_ISR_RXHUNT_BMSK          // RX Hunt character received
#define UARTDM_ISR_TXLEV      HWIO_UART_DM_ISR_TXLEV_BMSK           // TX Level or below met

/* Status Register */
#define UARTDM_SR_RXBREAK_START  HWIO_UART_DM_SR_RX_BREAK_START_LAST_BMSK   // RX Break start
#define UARTDM_SR_RXHUNT         HWIO_UART_DM_SR_HUNT_CHAR_BMSK   // Hunt Character Received
#define UARTDM_SR_RXBREAK        HWIO_UART_DM_SR_RX_BREAK_BMSK   // Break Received
#define UARTDM_SR_PF_ERR         HWIO_UART_DM_SR_PAR_FRAME_ERR_BMSK   // Parity of Framing Error
#define UARTDM_SR_OVR_ERR        HWIO_UART_DM_SR_UART_OVERRUN_BMSK   // Overrun Error
#define UARTDM_SR_TXEMT          HWIO_UART_DM_SR_TXEMT_BMSK   // Transmitter Empty
#define UARTDM_SR_TXRDY          HWIO_UART_DM_SR_TXRDY_BMSK   // Transmitter Ready
#define UARTDM_SR_RXFULL         HWIO_UART_DM_SR_RXFULL_BMSK   // Receiver Full
#define UARTDM_SR_RXRDY          HWIO_UART_DM_SR_RXRDY_BMSK   // Receiver Ready

/* RX FIFO State Register */
#define UARTDM_RXFS_FIFO_STATE_MSB_MASK   HWIO_UART_DM_RXFS_RX_FIFO_STATE_MSB_BMSK
#define UARTDM_RXFS_FIFO_STATE_MSB_SHIFT  HWIO_UART_DM_RXFS_RX_FIFO_STATE_MSB_SHFT
#define UARTDM_RXFS_FIFO_STATE_LSB_MASK   HWIO_UART_DM_RXFS_RX_FIFO_STATE_LSB_BMSK
#define UARTDM_RXFS_FIFO_STATE_LSB_SHIFT  HWIO_UART_DM_RXFS_RX_FIFO_STATE_LSB_SHFT

/* TX FIFO State Register */
#define UARTDM_TXFS_FIFO_STATE_MSB_MASK   HWIO_UART_DM_TXFS_TX_FIFO_STATE_MSB_BMSK
#define UARTDM_TXFS_FIFO_STATE_MSB_SHIFT  HWIO_UART_DM_TXFS_TX_FIFO_STATE_MSB_SHFT
#define UARTDM_TXFS_FIFO_STATE_LSB_MASK   HWIO_UART_DM_TXFS_TX_FIFO_STATE_LSB_BMSK
#define UARTDM_TXFS_FIFO_STATE_LSB_SHIFT  HWIO_UART_DM_TXFS_TX_FIFO_STATE_LSB_SHFT


/* MR1 */
#define UARTDM_MR1_AUTO_RFR_LEVEL1  HWIO_UART_DM_MR1_AUTO_RFR_LEVEL1_BMSK
#define UARTDM_MR1_RX_RDY_CTL       HWIO_UART_DM_MR1_RX_RDY_CTL_BMSK
#define UARTDM_MR1_CTS_CTL          HWIO_UART_DM_MR1_CTS_CTL_BMSK
#define UARTDM_MR1_AUTO_RFR_LEVEL0  HWIO_UART_DM_MR1_AUTO_RFR_LEVEL0_BMSK


/* MR2 Character Formats - Parity*/
#define UARTDM_NO_PARITY            HWIO_UART_DM_MR2_PARITY_MODE_NO_PARITY_FVAL
#define UARTDM_ODD_PARITY           HWIO_UART_DM_MR2_PARITY_MODE_ODD_PARITY_FVAL
#define UARTDM_EVEN_PARITY          HWIO_UART_DM_MR2_PARITY_MODE_EVEN_PARITY_FVAL
#define UARTDM_SPACE_PARITY         HWIO_UART_DM_MR2_PARITY_MODE_SPACE_PARITY_FVAL
#define UARTDM_PARITY_SHFT          HWIO_UART_DM_MR2_PARITY_MODE_SHFT
#define UARTDM_PARITY_BMSK          HWIO_UART_DM_MR2_PARITY_MODE_BMSK

/* MR2 Character Formats - Stop bit length*/
#define UARTDM_0_563_STOP_BIT       HWIO_UART_DM_MR2_STOP_BIT_LEN_ENUM_0_563_FVAL
#define UARTDM_1_STOP_BIT           HWIO_UART_DM_MR2_STOP_BIT_LEN_ENUM_1_000_BIT_TIME_FVAL
#define UARTDM_1_563_STOP_BIT       HWIO_UART_DM_MR2_STOP_BIT_LEN_ENUM_1_563_FVAL
#define UARTDM_2_STOP_BIT           HWIO_UART_DM_MR2_STOP_BIT_LEN_ENUM_2_000_BIT_TIMES_FVAL
#define UARTDM_STOP_BIT_SHFT        HWIO_UART_DM_MR2_STOP_BIT_LEN_SHFT
#define UARTDM_STOP_BIT_BMSK        HWIO_UART_DM_MR2_STOP_BIT_LEN_BMSK

/* MR2 Character Formats - Bits per character*/
#define UARTDM_5_BITS_PER_CHAR      HWIO_UART_DM_MR2_BITS_PER_CHAR_ENUM_5_BITS_FVAL
#define UARTDM_6_BITS_PER_CHAR      HWIO_UART_DM_MR2_BITS_PER_CHAR_ENUM_6_BITS_FVAL
#define UARTDM_7_BITS_PER_CHAR      HWIO_UART_DM_MR2_BITS_PER_CHAR_ENUM_7_BITS_FVAL
#define UARTDM_8_BITS_PER_CHAR      HWIO_UART_DM_MR2_BITS_PER_CHAR_ENUM_8_BITS_FVAL
#define UARTDM_BITS_PER_CHAR_SHFT   HWIO_UART_DM_MR2_BITS_PER_CHAR_SHFT
#define UARTDM_BITS_PER_CHAR_BMSK   HWIO_UART_DM_MR2_BITS_PER_CHAR_BMSK

#define UARTDM_MR2_LOOPBACK         HWIO_UART_DM_MR2_LOOPBACK_BMSK

/* DMA Enable Register */
#define UARTDM_DMEN_RX_SC_ENABLE    HWIO_UART_DM_DMEN_RX_SC_ENABLE_BMSK
#define UARTDM_DMEN_TX_SC_ENABLE    HWIO_UART_DM_DMEN_TX_SC_ENABLE_BMSK
#define UARTDM_DMEN_RX_BAM_ENABLE   HWIO_UART_DM_DMEN_RX_BAM_ENABLE_BMSK
#define UARTDM_DMEN_TX_BAM_ENABLE   HWIO_UART_DM_DMEN_TX_BAM_ENABLE_BMSK

/* Interrupt Programming Register */
#define UARTDM_IPR_STALE_TIMEOUT_MSB  HWIO_UART_DM_IPR_STALE_TIMEOUT_MSB_BMSK
#define UARTDM_IPR_STALE_TIMEOUT_LSB  HWIO_UART_DM_IPR_STALE_TIMEOUT_LSB_BMSK

/* IRDA Register */
#define UARTDM_IRDA_IRDA_EN  HWIO_UART_DM_IRDA_IRDA_EN_BMSK

/* Generics Register */
#define UARTDM_GENERIC_RAM_ADDR_WIDTH   HWIO_UART_DM_GENERICS_GENERIC_RAM_ADDR_WIDTH_BMSK


void    HALuart_EnableInterrupts(uint32 base, uint32 flags);
void    HALuart_DisableInterrupts(uint32 base, uint32 flags);
uint32  HALuart_GetDmaRxSnapChars(uint32 base);
uint32  HALuart_GetFifoSize(uint32 base);
uint32  HALuart_GetInterruptStatus(uint32 base);
uint32  HALuart_GetMaskedInterruptStatus(uint32 base);
uint32  HALuart_GetStatus(uint32 base);
uint32  HALuart_GetTxFifoSize(uint32 base);
uint32  HALuart_ReadRxFifo(uint32 base,char* read_buf,uint32 bytes_to_read);
void    HALuart_ResetErrorStatus(uint32 base);
void    HALuart_ResetReceiver(uint32 base);
void    HALuart_ResetStaleInterrupt(uint32 base);
void    HALuart_ResetTransmitter(uint32 base);
void    HALuart_SetAutoInboundFlowCtl(uint32 base, uint32 enabled);
void    HALuart_SetCharFormat(uint32 base,uint32 char_len,uint32 stop_bit,uint32 parity);
void    HALuart_SetCmdSynchronization(uint32 base, uint32 enabled);
void    HALuart_SetDivider(uint32 base, uint32 divider);
void    HALuart_SetDmaEnable(uint32 base, uint32 enable);
void    HALuart_SetIrdaEnable(uint32 base, uint32 enabled);
void    HALuart_SetOutboundFlowCtl(uint32 base, uint32 enabled);
void    HALuart_SetLoopback(uint32 base, uint32 enabled);
void    HALuart_SetRxEnable(uint32 base, uint32 enabled);
void    HALuart_SetRxSCEnable(uint32 base, uint32 enabled);
void    HALuart_SetRxWatermark(uint32 base, uint32 num_bytes);
void    HALuart_SetStaleEvent(uint32 base, uint32 enabled);
void    HALuart_SetStaleTimeout(uint32 base, uint32 stale_timeout);
void    HALuart_SetTxEnable(uint32 base, uint32 enabled);
void    HALuart_SetTxFifoSize(uint32 base, uint32 num_bytes);
void    HALuart_SetTxSCEnable(uint32 base, uint32 enabled);
void    HALuart_SetTxWatermark(uint32 base, uint32 num_bytes);
void    HALuart_SetupDmaRx(uint32 base, uint32 num_chars);
uint32  HALuart_WriteTxFifo(uint32 base, char *ptr, uint32 num_bytes);

#endif
