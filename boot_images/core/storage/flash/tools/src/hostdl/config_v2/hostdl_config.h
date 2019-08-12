#ifndef HOSTDL_CONFIG_H
#define HOSTDL_CONFIG_H

/*==================================================================
 *
 * FILE:        hostdl_config.h
 *
 * SERVICES:    Target Specific definition
 *
 * DESCRIPTION:
 *    This header file contains hardware specific definitions which
 *    must be ported to each MSM
 *
 * Copyright (c) 2009-2012, 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/config_v2/hostdl_config.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/30/14     sb      Watchdog changes for 9x45
 * 10/19/12     sv      Watchdog changes for 9x25
 * 09/22/11     sv      Watchdog changes for 9x15
 * 08/16/11     jz      Support ehostdl on 9x15
 * 10/01/10     sv      Add support to disable watchdog
 * 10/26/09     eo      Cleanup hwio UART register mapping
 * 08/22/09     op      Add UART for 8K
 * 02/27/09     jz      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

//--------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------
#ifdef TARGET_ID_9K
  #define UART_REG_MAP(uart_regsym) UART2_UART_##uart_regsym
#elif defined(TARGET_ID_8K) 
  #define UART_REG_MAP(uart_regsym) UART_##uart_regsym
#else 
  /* All other targets such as T_QSC6695 */
  #define UART_REG_MAP(uart_regsym) UART2_##uart_regsym
#endif

/* Use UART2 */
#define UART_REG_SYM(hwio_regsym)  UART_REG_MAP(hwio_regsym)

/*      Reset Break             */
#define UART_CR_RESET_BREAK         (0x04 << \
        HWIO_SHFT(UART_REG_MAP(CR),CHANNEL_COMMAND))

/*      Stop Break             */
#define UART_CR_STOP_BREAK           (0x06 << \
        HWIO_SHFT(UART_REG_MAP(CR),CHANNEL_COMMAND))

/*      Set Ready for Receive             */
#define UART_CR_SET_RFR              (0x0D << \
        HWIO_SHFT(UART_REG_MAP(CR),CHANNEL_COMMAND))

/* defines needed for ap_uart.c */

/*      Break received                */
#define UART_SR_BREAK_RXD           (0x01 << \
        HWIO_SHFT(UART_REG_MAP(SR),RX_BREAK))

/*      Parity or Framing error       */
#define UART_SR_PF_ERR              (0x01 << \
        HWIO_SHFT(UART_REG_MAP(SR),PAR_FRAME_ERR))

/*      Overrun error                 */
#define UART_SR_OVR_ERR             (0x01 << \
        HWIO_SHFT(UART_REG_MAP(SR),UART_OVERRUN))

/*      8 bits per character          */
#define UART_MR2_8BPC               (0x03 << \
        HWIO_SHFT(UART_REG_MAP(MR2),BITS_PER_CHAR))

/*      1 stop bit                    */
#define UART_MR2_1SB                (0x01 << \
        HWIO_SHFT(UART_REG_MAP(MR2),STOP_BIT_LEN))

/*      Reset error status            */
#define UART_CR_RESET_ERR           (0x03 << \
        HWIO_SHFT(UART_REG_MAP(CR),CHANNEL_COMMAND))

/*      Reset transmitter             */
#define UART_CR_RESET_TX            (0x02 << \
        HWIO_SHFT(UART_REG_MAP(CR),CHANNEL_COMMAND))

/*      Reset receiver                */
#define UART_CR_RESET_RX            (0x01 << \
        HWIO_SHFT(UART_REG_MAP(CR),CHANNEL_COMMAND))

/*      Enable receiver               */
#define UART_CR_RX_ENA              (0x01 << \
        HWIO_SHFT(UART_REG_MAP(CR),UART_RX_EN))

/*      Enable transmitter            */
#define UART_CR_TX_ENA              (0x01 << \
        HWIO_SHFT(UART_REG_MAP(CR),UART_TX_EN))

/* Selection of which receiver status bits we consider to be errors. */
#define  UART_SR_RX_ERROR  (UART_SR_BREAK_RXD |       \
                            UART_SR_PF_ERR    |       \
                            UART_SR_OVR_ERR )

/* Watchdog defines */
#define HWIO_MPM2_WDOG_BITE_VAL_REG_ADDR   0x004AA010
/* Each tick is 31.25 uSec. Configuring a total of 5.12 Sec(0x4000 ticks) for 
 * watchdog timeout */
#define HOSTDL_WDOG_BITE_REG_VAL           0x4000
#define HWIO_MPM2_WDOG_CTL_REG_ADDR        0x004AA004
#define HOSTDL_WDOG_CTL_REG_VAL            0x1

/*---------------------------------------------------------------------------
                    MACRO KICK_WATCHDOG

This macro resets the watchdog timer circuit.
---------------------------------------------------------------------------*/
#if !defined(FLASH_WATCHDOG_DISABLED)
  #define KICK_WATCHDOG() \
     *((volatile uint32 *)FLASH_WATCHDOG_RESET_ADDR) = 1;
#else
  #define KICK_WATCHDOG()
#endif

#ifdef FLASH_ENABLE_WDOG_CFG
  #define ENABLE_WATCHDOG() \
    *(volatile uint32 *)HWIO_MPM2_WDOG_BITE_VAL_REG_ADDR = \
      HOSTDL_WDOG_BITE_REG_VAL; \
    *(volatile uint32 *)HWIO_MPM2_WDOG_CTL_REG_ADDR = HOSTDL_WDOG_CTL_REG_VAL;
#else
  #define ENABLE_WATCHDOG()
#endif

#endif /* HOSTDL_CONFIG_H */
