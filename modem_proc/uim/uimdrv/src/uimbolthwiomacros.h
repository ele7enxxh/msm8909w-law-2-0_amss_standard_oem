#ifndef __UIMBOLTMACROS_H__
#define __UIMBOLTMACROS_H__
/*
===========================================================================
*/
/**
  @file UIMBOLTMACROS.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MSS_UIM0_UART_DM
    MSS_UIM1_UART_DM
    MSS_UIM2_UART_DM
    MSS_UIM3_UART_DM
    MSS_PERPH
    TLMM_CSR

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2013QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
  QUALCOMM Technologies Proprietary and Confidential.

  ===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimbolthwiomacros.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include "msmhwiobase.h"

#ifndef MSS_TOP_BASE
  #define MSS_TOP_BASE       MODEM_TOP_BASE
#endif

/*----------------------------------------------------------------------------
 * MODULE: MSS_UIM0_UART_DM
 *--------------------------------------------------------------------------*/

#define MSS_UIM0_UART_DM_REG_BASE                                                                               (MODEM_TOP_BASE      + 0x00190000)

#define HWIO_MSS_UIM0_UART_DM_MR1_ADDR                                                                          (MSS_UIM0_UART_DM_REG_BASE      + 0x00000000)
#define HWIO_MSS_UIM0_UART_DM_MR1_RMSK                                                                          0xffffffff
#define HWIO_MSS_UIM0_UART_DM_MR1_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_MR1_ADDR, HWIO_MSS_UIM0_UART_DM_MR1_RMSK)
#define HWIO_MSS_UIM0_UART_DM_MR1_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_MR1_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_MR1_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_MR1_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_MR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_MR1_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_MR1_IN)
#define HWIO_MSS_UIM0_UART_DM_MR1_AUTO_RFR_LEVEL1_BMSK                                                          0xffffff00
#define HWIO_MSS_UIM0_UART_DM_MR1_AUTO_RFR_LEVEL1_SHFT                                                                 0x8
#define HWIO_MSS_UIM0_UART_DM_MR1_RX_RDY_CTL_BMSK                                                                     0x80
#define HWIO_MSS_UIM0_UART_DM_MR1_RX_RDY_CTL_SHFT                                                                      0x7
#define HWIO_MSS_UIM0_UART_DM_MR1_CTS_CTL_BMSK                                                                        0x40
#define HWIO_MSS_UIM0_UART_DM_MR1_CTS_CTL_SHFT                                                                         0x6
#define HWIO_MSS_UIM0_UART_DM_MR1_AUTO_RFR_LEVEL0_BMSK                                                                0x3f
#define HWIO_MSS_UIM0_UART_DM_MR1_AUTO_RFR_LEVEL0_SHFT                                                                 0x0

#define HWIO_MSS_UIM0_UART_DM_MR2_ADDR                                                                          (MSS_UIM0_UART_DM_REG_BASE      + 0x00000004)
#define HWIO_MSS_UIM0_UART_DM_MR2_RMSK                                                                               0x7ff
#define HWIO_MSS_UIM0_UART_DM_MR2_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_MR2_ADDR, HWIO_MSS_UIM0_UART_DM_MR2_RMSK)
#define HWIO_MSS_UIM0_UART_DM_MR2_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_MR2_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_MR2_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_MR2_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_MR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_MR2_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_MR2_IN)
#define HWIO_MSS_UIM0_UART_DM_MR2_RFR_CTS_LOOPBACK_BMSK                                                              0x400
#define HWIO_MSS_UIM0_UART_DM_MR2_RFR_CTS_LOOPBACK_SHFT                                                                0xa
#define HWIO_MSS_UIM0_UART_DM_MR2_RX_ERROR_CHAR_OFF_BMSK                                                             0x200
#define HWIO_MSS_UIM0_UART_DM_MR2_RX_ERROR_CHAR_OFF_SHFT                                                               0x9
#define HWIO_MSS_UIM0_UART_DM_MR2_RX_BREAK_ZERO_CHAR_OFF_BMSK                                                        0x100
#define HWIO_MSS_UIM0_UART_DM_MR2_RX_BREAK_ZERO_CHAR_OFF_SHFT                                                          0x8
#define HWIO_MSS_UIM0_UART_DM_MR2_LOOPBACK_BMSK                                                                       0x80
#define HWIO_MSS_UIM0_UART_DM_MR2_LOOPBACK_SHFT                                                                        0x7
#define HWIO_MSS_UIM0_UART_DM_MR2_ERROR_MODE_BMSK                                                                     0x40
#define HWIO_MSS_UIM0_UART_DM_MR2_ERROR_MODE_SHFT                                                                      0x6
#define HWIO_MSS_UIM0_UART_DM_MR2_BITS_PER_CHAR_BMSK                                                                  0x30
#define HWIO_MSS_UIM0_UART_DM_MR2_BITS_PER_CHAR_SHFT                                                                   0x4
#define HWIO_MSS_UIM0_UART_DM_MR2_STOP_BIT_LEN_BMSK                                                                    0xc
#define HWIO_MSS_UIM0_UART_DM_MR2_STOP_BIT_LEN_SHFT                                                                    0x2
#define HWIO_MSS_UIM0_UART_DM_MR2_PARITY_MODE_BMSK                                                                     0x3
#define HWIO_MSS_UIM0_UART_DM_MR2_PARITY_MODE_SHFT                                                                     0x0

#define HWIO_MSS_UIM0_UART_DM_CSR_SR_DEPRECATED_ADDR                                                            (MSS_UIM0_UART_DM_REG_BASE      + 0x00000008)
#define HWIO_MSS_UIM0_UART_DM_CSR_SR_DEPRECATED_RMSK                                                            0xffffffff
#define HWIO_MSS_UIM0_UART_DM_CSR_SR_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_CSR_SR_DEPRECATED_ADDR, HWIO_MSS_UIM0_UART_DM_CSR_SR_DEPRECATED_RMSK)
#define HWIO_MSS_UIM0_UART_DM_CSR_SR_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_CSR_SR_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_CSR_SR_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_CSR_SR_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_CSR_SR_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_CSR_SR_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_CSR_SR_DEPRECATED_IN)
#define HWIO_MSS_UIM0_UART_DM_CSR_SR_DEPRECATED_UART_DM_CSR_SR_DEPRECATED_BMSK                                  0xffffffff
#define HWIO_MSS_UIM0_UART_DM_CSR_SR_DEPRECATED_UART_DM_CSR_SR_DEPRECATED_SHFT                                         0x0

#define HWIO_MSS_UIM0_UART_DM_CR_MISR_DEPRECATED_ADDR                                                           (MSS_UIM0_UART_DM_REG_BASE      + 0x00000010)
#define HWIO_MSS_UIM0_UART_DM_CR_MISR_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM0_UART_DM_CR_MISR_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_CR_MISR_DEPRECATED_ADDR, HWIO_MSS_UIM0_UART_DM_CR_MISR_DEPRECATED_RMSK)
#define HWIO_MSS_UIM0_UART_DM_CR_MISR_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_CR_MISR_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_CR_MISR_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_CR_MISR_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_CR_MISR_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_CR_MISR_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_CR_MISR_DEPRECATED_IN)
#define HWIO_MSS_UIM0_UART_DM_CR_MISR_DEPRECATED_UART_DM_CR_MISR_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_CR_MISR_DEPRECATED_UART_DM_CR_MISR_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM0_UART_DM_IMR_ISR_DEPRECATED_ADDR                                                           (MSS_UIM0_UART_DM_REG_BASE      + 0x00000014)
#define HWIO_MSS_UIM0_UART_DM_IMR_ISR_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM0_UART_DM_IMR_ISR_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_IMR_ISR_DEPRECATED_ADDR, HWIO_MSS_UIM0_UART_DM_IMR_ISR_DEPRECATED_RMSK)
#define HWIO_MSS_UIM0_UART_DM_IMR_ISR_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_IMR_ISR_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_IMR_ISR_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_IMR_ISR_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_IMR_ISR_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_IMR_ISR_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_IMR_ISR_DEPRECATED_IN)
#define HWIO_MSS_UIM0_UART_DM_IMR_ISR_DEPRECATED_UART_DM_IMR_ISR_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_IMR_ISR_DEPRECATED_UART_DM_IMR_ISR_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM0_UART_DM_IPR_ADDR                                                                          (MSS_UIM0_UART_DM_REG_BASE      + 0x00000018)
#define HWIO_MSS_UIM0_UART_DM_IPR_RMSK                                                                          0xffffffdf
#define HWIO_MSS_UIM0_UART_DM_IPR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_IPR_ADDR, HWIO_MSS_UIM0_UART_DM_IPR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_IPR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_IPR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_IPR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_IPR_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_IPR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_IPR_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_IPR_IN)
#define HWIO_MSS_UIM0_UART_DM_IPR_STALE_TIMEOUT_MSB_BMSK                                                        0xffffff80
#define HWIO_MSS_UIM0_UART_DM_IPR_STALE_TIMEOUT_MSB_SHFT                                                               0x7
#define HWIO_MSS_UIM0_UART_DM_IPR_SAMPLE_DATA_BMSK                                                                    0x40
#define HWIO_MSS_UIM0_UART_DM_IPR_SAMPLE_DATA_SHFT                                                                     0x6
#define HWIO_MSS_UIM0_UART_DM_IPR_STALE_TIMEOUT_LSB_BMSK                                                              0x1f
#define HWIO_MSS_UIM0_UART_DM_IPR_STALE_TIMEOUT_LSB_SHFT                                                               0x0

#define HWIO_MSS_UIM0_UART_DM_TFWR_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x0000001c)
#define HWIO_MSS_UIM0_UART_DM_TFWR_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TFWR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TFWR_ADDR, HWIO_MSS_UIM0_UART_DM_TFWR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_TFWR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TFWR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_TFWR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TFWR_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TFWR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_TFWR_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_TFWR_IN)
#define HWIO_MSS_UIM0_UART_DM_TFWR_TFW_BMSK                                                                     0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TFWR_TFW_SHFT                                                                            0x0

#define HWIO_MSS_UIM0_UART_DM_RFWR_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000020)
#define HWIO_MSS_UIM0_UART_DM_RFWR_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RFWR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RFWR_ADDR, HWIO_MSS_UIM0_UART_DM_RFWR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RFWR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RFWR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RFWR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_RFWR_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_RFWR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_RFWR_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_RFWR_IN)
#define HWIO_MSS_UIM0_UART_DM_RFWR_RFW_BMSK                                                                     0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RFWR_RFW_SHFT                                                                            0x0

#define HWIO_MSS_UIM0_UART_DM_HCR_ADDR                                                                          (MSS_UIM0_UART_DM_REG_BASE      + 0x00000024)
#define HWIO_MSS_UIM0_UART_DM_HCR_RMSK                                                                                0xff
#define HWIO_MSS_UIM0_UART_DM_HCR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_HCR_ADDR, HWIO_MSS_UIM0_UART_DM_HCR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_HCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_HCR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_HCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_HCR_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_HCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_HCR_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_HCR_IN)
#define HWIO_MSS_UIM0_UART_DM_HCR_DATA_BMSK                                                                           0xff
#define HWIO_MSS_UIM0_UART_DM_HCR_DATA_SHFT                                                                            0x0

#define HWIO_MSS_UIM0_UART_DM_DMRX_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000034)
#define HWIO_MSS_UIM0_UART_DM_DMRX_RMSK                                                                          0x1ffffff
#define HWIO_MSS_UIM0_UART_DM_DMRX_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_DMRX_ADDR, HWIO_MSS_UIM0_UART_DM_DMRX_RMSK)
#define HWIO_MSS_UIM0_UART_DM_DMRX_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_DMRX_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_DMRX_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_DMRX_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_DMRX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_DMRX_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_DMRX_IN)
#define HWIO_MSS_UIM0_UART_DM_DMRX_RX_DM_CRCI_CHARS_BMSK                                                         0x1ffffff
#define HWIO_MSS_UIM0_UART_DM_DMRX_RX_DM_CRCI_CHARS_SHFT                                                               0x0

#define HWIO_MSS_UIM0_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR                                                (MSS_UIM0_UART_DM_REG_BASE      + 0x00000038)
#define HWIO_MSS_UIM0_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_RMSK                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR, HWIO_MSS_UIM0_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_RMSK)
#define HWIO_MSS_UIM0_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_IN)
#define HWIO_MSS_UIM0_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_BMSK          0xffffffff
#define HWIO_MSS_UIM0_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_SHFT                 0x0

#define HWIO_MSS_UIM0_UART_DM_DMEN_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x0000003c)
#define HWIO_MSS_UIM0_UART_DM_DMEN_RMSK                                                                               0x3c
#define HWIO_MSS_UIM0_UART_DM_DMEN_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_DMEN_ADDR, HWIO_MSS_UIM0_UART_DM_DMEN_RMSK)
#define HWIO_MSS_UIM0_UART_DM_DMEN_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_DMEN_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_DMEN_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_DMEN_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_DMEN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_DMEN_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_DMEN_IN)
#define HWIO_MSS_UIM0_UART_DM_DMEN_RX_SC_ENABLE_BMSK                                                                  0x20
#define HWIO_MSS_UIM0_UART_DM_DMEN_RX_SC_ENABLE_SHFT                                                                   0x5
#define HWIO_MSS_UIM0_UART_DM_DMEN_TX_SC_ENABLE_BMSK                                                                  0x10
#define HWIO_MSS_UIM0_UART_DM_DMEN_TX_SC_ENABLE_SHFT                                                                   0x4
#define HWIO_MSS_UIM0_UART_DM_DMEN_RX_BAM_ENABLE_BMSK                                                                  0x8
#define HWIO_MSS_UIM0_UART_DM_DMEN_RX_BAM_ENABLE_SHFT                                                                  0x3
#define HWIO_MSS_UIM0_UART_DM_DMEN_TX_BAM_ENABLE_BMSK                                                                  0x4
#define HWIO_MSS_UIM0_UART_DM_DMEN_TX_BAM_ENABLE_SHFT                                                                  0x2

#define HWIO_MSS_UIM0_UART_DM_NO_CHARS_FOR_TX_ADDR                                                              (MSS_UIM0_UART_DM_REG_BASE      + 0x00000040)
#define HWIO_MSS_UIM0_UART_DM_NO_CHARS_FOR_TX_RMSK                                                                0xffffff
#define HWIO_MSS_UIM0_UART_DM_NO_CHARS_FOR_TX_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_NO_CHARS_FOR_TX_ADDR, HWIO_MSS_UIM0_UART_DM_NO_CHARS_FOR_TX_RMSK)
#define HWIO_MSS_UIM0_UART_DM_NO_CHARS_FOR_TX_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_NO_CHARS_FOR_TX_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_NO_CHARS_FOR_TX_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_NO_CHARS_FOR_TX_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_NO_CHARS_FOR_TX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_NO_CHARS_FOR_TX_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_NO_CHARS_FOR_TX_IN)
#define HWIO_MSS_UIM0_UART_DM_NO_CHARS_FOR_TX_TX_TOTAL_TRANS_LEN_BMSK                                             0xffffff
#define HWIO_MSS_UIM0_UART_DM_NO_CHARS_FOR_TX_TX_TOTAL_TRANS_LEN_SHFT                                                  0x0

#define HWIO_MSS_UIM0_UART_DM_BADR_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000044)
#define HWIO_MSS_UIM0_UART_DM_BADR_RMSK                                                                         0xfffffffc
#define HWIO_MSS_UIM0_UART_DM_BADR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_BADR_ADDR, HWIO_MSS_UIM0_UART_DM_BADR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_BADR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_BADR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_BADR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_BADR_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_BADR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_BADR_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_BADR_IN)
#define HWIO_MSS_UIM0_UART_DM_BADR_RX_BASE_ADDR_BMSK                                                            0xfffffffc
#define HWIO_MSS_UIM0_UART_DM_BADR_RX_BASE_ADDR_SHFT                                                                   0x2

#define HWIO_MSS_UIM0_UART_DM_TESTSL_ADDR                                                                       (MSS_UIM0_UART_DM_REG_BASE      + 0x00000048)
#define HWIO_MSS_UIM0_UART_DM_TESTSL_RMSK                                                                             0x1f
#define HWIO_MSS_UIM0_UART_DM_TESTSL_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TESTSL_ADDR, HWIO_MSS_UIM0_UART_DM_TESTSL_RMSK)
#define HWIO_MSS_UIM0_UART_DM_TESTSL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TESTSL_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_TESTSL_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TESTSL_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TESTSL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_TESTSL_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_TESTSL_IN)
#define HWIO_MSS_UIM0_UART_DM_TESTSL_TEST_EN_BMSK                                                                     0x10
#define HWIO_MSS_UIM0_UART_DM_TESTSL_TEST_EN_SHFT                                                                      0x4
#define HWIO_MSS_UIM0_UART_DM_TESTSL_TEST_SEL_BMSK                                                                     0xf
#define HWIO_MSS_UIM0_UART_DM_TESTSL_TEST_SEL_SHFT                                                                     0x0

#define HWIO_MSS_UIM0_UART_DM_TXFS_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x0000004c)
#define HWIO_MSS_UIM0_UART_DM_TXFS_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TXFS_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TXFS_ADDR, HWIO_MSS_UIM0_UART_DM_TXFS_RMSK)
#define HWIO_MSS_UIM0_UART_DM_TXFS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TXFS_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_TXFS_TX_FIFO_STATE_MSB_BMSK                                                       0xffffc000
#define HWIO_MSS_UIM0_UART_DM_TXFS_TX_FIFO_STATE_MSB_SHFT                                                              0xe
#define HWIO_MSS_UIM0_UART_DM_TXFS_TX_ASYNC_FIFO_STATE_BMSK                                                         0x3c00
#define HWIO_MSS_UIM0_UART_DM_TXFS_TX_ASYNC_FIFO_STATE_SHFT                                                            0xa
#define HWIO_MSS_UIM0_UART_DM_TXFS_TX_BUFFER_STATE_BMSK                                                              0x380
#define HWIO_MSS_UIM0_UART_DM_TXFS_TX_BUFFER_STATE_SHFT                                                                0x7
#define HWIO_MSS_UIM0_UART_DM_TXFS_TX_FIFO_STATE_LSB_BMSK                                                             0x7f
#define HWIO_MSS_UIM0_UART_DM_TXFS_TX_FIFO_STATE_LSB_SHFT                                                              0x0

#define HWIO_MSS_UIM0_UART_DM_RXFS_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000050)
#define HWIO_MSS_UIM0_UART_DM_RXFS_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RXFS_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RXFS_ADDR, HWIO_MSS_UIM0_UART_DM_RXFS_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RXFS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RXFS_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RXFS_RX_FIFO_STATE_MSB_BMSK                                                       0xffffc000
#define HWIO_MSS_UIM0_UART_DM_RXFS_RX_FIFO_STATE_MSB_SHFT                                                              0xe
#define HWIO_MSS_UIM0_UART_DM_RXFS_RX_ASYNC_FIFO_STATE_BMSK                                                         0x3c00
#define HWIO_MSS_UIM0_UART_DM_RXFS_RX_ASYNC_FIFO_STATE_SHFT                                                            0xa
#define HWIO_MSS_UIM0_UART_DM_RXFS_RX_BUFFER_STATE_BMSK                                                              0x380
#define HWIO_MSS_UIM0_UART_DM_RXFS_RX_BUFFER_STATE_SHFT                                                                0x7
#define HWIO_MSS_UIM0_UART_DM_RXFS_RX_FIFO_STATE_LSB_BMSK                                                             0x7f
#define HWIO_MSS_UIM0_UART_DM_RXFS_RX_FIFO_STATE_LSB_SHFT                                                              0x0

#define HWIO_MSS_UIM0_UART_DM_MISR_MODE_ADDR                                                                    (MSS_UIM0_UART_DM_REG_BASE      + 0x00000060)
#define HWIO_MSS_UIM0_UART_DM_MISR_MODE_RMSK                                                                           0x3
#define HWIO_MSS_UIM0_UART_DM_MISR_MODE_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_MISR_MODE_ADDR, HWIO_MSS_UIM0_UART_DM_MISR_MODE_RMSK)
#define HWIO_MSS_UIM0_UART_DM_MISR_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_MISR_MODE_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_MISR_MODE_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_MISR_MODE_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_MISR_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_MISR_MODE_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_MISR_MODE_IN)
#define HWIO_MSS_UIM0_UART_DM_MISR_MODE_MODE_BMSK                                                                      0x3
#define HWIO_MSS_UIM0_UART_DM_MISR_MODE_MODE_SHFT                                                                      0x0

#define HWIO_MSS_UIM0_UART_DM_MISR_RESET_ADDR                                                                   (MSS_UIM0_UART_DM_REG_BASE      + 0x00000064)
#define HWIO_MSS_UIM0_UART_DM_MISR_RESET_RMSK                                                                          0x1
#define HWIO_MSS_UIM0_UART_DM_MISR_RESET_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_MISR_RESET_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_MISR_RESET_RESET_BMSK                                                                    0x1
#define HWIO_MSS_UIM0_UART_DM_MISR_RESET_RESET_SHFT                                                                    0x0

#define HWIO_MSS_UIM0_UART_DM_MISR_EXPORT_ADDR                                                                  (MSS_UIM0_UART_DM_REG_BASE      + 0x00000068)
#define HWIO_MSS_UIM0_UART_DM_MISR_EXPORT_RMSK                                                                         0x1
#define HWIO_MSS_UIM0_UART_DM_MISR_EXPORT_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_MISR_EXPORT_ADDR, HWIO_MSS_UIM0_UART_DM_MISR_EXPORT_RMSK)
#define HWIO_MSS_UIM0_UART_DM_MISR_EXPORT_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_MISR_EXPORT_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_MISR_EXPORT_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_MISR_EXPORT_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_MISR_EXPORT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_MISR_EXPORT_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_MISR_EXPORT_IN)
#define HWIO_MSS_UIM0_UART_DM_MISR_EXPORT_EXPORT_BMSK                                                                  0x1
#define HWIO_MSS_UIM0_UART_DM_MISR_EXPORT_EXPORT_SHFT                                                                  0x0

#define HWIO_MSS_UIM0_UART_DM_MISR_VAL_ADDR                                                                     (MSS_UIM0_UART_DM_REG_BASE      + 0x0000006c)
#define HWIO_MSS_UIM0_UART_DM_MISR_VAL_RMSK                                                                          0x3ff
#define HWIO_MSS_UIM0_UART_DM_MISR_VAL_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_MISR_VAL_ADDR, HWIO_MSS_UIM0_UART_DM_MISR_VAL_RMSK)
#define HWIO_MSS_UIM0_UART_DM_MISR_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_MISR_VAL_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_MISR_VAL_VAL_BMSK                                                                      0x3ff
#define HWIO_MSS_UIM0_UART_DM_MISR_VAL_VAL_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_TF_RF_DEPRECATED_ADDR                                                             (MSS_UIM0_UART_DM_REG_BASE      + 0x00000070)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_DEPRECATED_RMSK                                                             0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_RF_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TF_RF_DEPRECATED_ADDR, HWIO_MSS_UIM0_UART_DM_TF_RF_DEPRECATED_RMSK)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TF_RF_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_RF_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_TF_RF_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_TF_RF_DEPRECATED_IN)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_DEPRECATED_UART_DM_TF_RF_DEPRECATED_BMSK                                    0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_RF_DEPRECATED_UART_DM_TF_RF_DEPRECATED_SHFT                                           0x0

#define HWIO_MSS_UIM0_UART_DM_TF_RF_2_DEPRECATED_ADDR                                                           (MSS_UIM0_UART_DM_REG_BASE      + 0x00000074)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_2_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_RF_2_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TF_RF_2_DEPRECATED_ADDR, HWIO_MSS_UIM0_UART_DM_TF_RF_2_DEPRECATED_RMSK)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_2_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TF_RF_2_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_2_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_RF_2_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_2_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_TF_RF_2_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_TF_RF_2_DEPRECATED_IN)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_2_DEPRECATED_UART_DM_TF_RF_2_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_RF_2_DEPRECATED_UART_DM_TF_RF_2_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM0_UART_DM_TF_RF_3_DEPRECATED_ADDR                                                           (MSS_UIM0_UART_DM_REG_BASE      + 0x00000078)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_3_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_RF_3_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TF_RF_3_DEPRECATED_ADDR, HWIO_MSS_UIM0_UART_DM_TF_RF_3_DEPRECATED_RMSK)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_3_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TF_RF_3_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_3_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_RF_3_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_3_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_TF_RF_3_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_TF_RF_3_DEPRECATED_IN)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_3_DEPRECATED_UART_DM_TF_RF_3_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_RF_3_DEPRECATED_UART_DM_TF_RF_3_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM0_UART_DM_TF_RF_4_DEPRECATED_ADDR                                                           (MSS_UIM0_UART_DM_REG_BASE      + 0x0000007c)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_4_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_RF_4_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TF_RF_4_DEPRECATED_ADDR, HWIO_MSS_UIM0_UART_DM_TF_RF_4_DEPRECATED_RMSK)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_4_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TF_RF_4_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_4_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_RF_4_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_4_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_TF_RF_4_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_TF_RF_4_DEPRECATED_IN)
#define HWIO_MSS_UIM0_UART_DM_TF_RF_4_DEPRECATED_UART_DM_TF_RF_4_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_RF_4_DEPRECATED_UART_DM_TF_RF_4_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_ADDR                                                                      (MSS_UIM0_UART_DM_REG_BASE      + 0x00000080)
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_RMSK                                                                         0x3ffef
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_SIM_CFG_ADDR, HWIO_MSS_UIM0_UART_DM_SIM_CFG_RMSK)
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_SIM_CFG_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_SIM_CFG_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_SIM_CFG_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_SIM_CFG_IN)
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_UIM_TX_MODE_BMSK                                                             0x20000
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_UIM_TX_MODE_SHFT                                                                0x11
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_UIM_RX_MODE_BMSK                                                             0x10000
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_UIM_RX_MODE_SHFT                                                                0x10
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_BMSK                                                         0xff00
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_SHFT                                                            0x8
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_SIM_CLK_ON_BMSK                                                                 0x80
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_SIM_CLK_ON_SHFT                                                                  0x7
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_SIM_CLK_TD8_SEL_BMSK                                                            0x40
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_SIM_CLK_TD8_SEL_SHFT                                                             0x6
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_SIM_CLK_STOP_HIGH_BMSK                                                          0x20
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_SIM_CLK_STOP_HIGH_SHFT                                                           0x5
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_MASK_RX_BMSK                                                                     0x8
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_MASK_RX_SHFT                                                                     0x3
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_SWAP_D_BMSK                                                                      0x4
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_SWAP_D_SHFT                                                                      0x2
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_INV_D_BMSK                                                                       0x2
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_INV_D_SHFT                                                                       0x1
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_SIM_SEL_BMSK                                                                     0x1
#define HWIO_MSS_UIM0_UART_DM_SIM_CFG_SIM_SEL_SHFT                                                                     0x0

#define HWIO_MSS_UIM0_UART_DM_TEST_WR_ADDR_ADDR                                                                 (MSS_UIM0_UART_DM_REG_BASE      + 0x00000084)
#define HWIO_MSS_UIM0_UART_DM_TEST_WR_ADDR_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TEST_WR_ADDR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TEST_WR_ADDR_ADDR, HWIO_MSS_UIM0_UART_DM_TEST_WR_ADDR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_TEST_WR_ADDR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TEST_WR_ADDR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_TEST_WR_ADDR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TEST_WR_ADDR_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TEST_WR_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_TEST_WR_ADDR_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_TEST_WR_ADDR_IN)
#define HWIO_MSS_UIM0_UART_DM_TEST_WR_ADDR_TEST_WR_ADDR_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TEST_WR_ADDR_TEST_WR_ADDR_SHFT                                                           0x0

#define HWIO_MSS_UIM0_UART_DM_TEST_WR_DATA_ADDR                                                                 (MSS_UIM0_UART_DM_REG_BASE      + 0x00000088)
#define HWIO_MSS_UIM0_UART_DM_TEST_WR_DATA_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TEST_WR_DATA_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TEST_WR_DATA_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TEST_WR_DATA_TEST_WR_DATA_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TEST_WR_DATA_TEST_WR_DATA_SHFT                                                           0x0

#define HWIO_MSS_UIM0_UART_DM_TEST_RD_ADDR_ADDR                                                                 (MSS_UIM0_UART_DM_REG_BASE      + 0x0000008c)
#define HWIO_MSS_UIM0_UART_DM_TEST_RD_ADDR_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TEST_RD_ADDR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TEST_RD_ADDR_ADDR, HWIO_MSS_UIM0_UART_DM_TEST_RD_ADDR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_TEST_RD_ADDR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TEST_RD_ADDR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_TEST_RD_ADDR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TEST_RD_ADDR_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TEST_RD_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_TEST_RD_ADDR_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_TEST_RD_ADDR_IN)
#define HWIO_MSS_UIM0_UART_DM_TEST_RD_ADDR_TEST_RD_ADDR_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TEST_RD_ADDR_TEST_RD_ADDR_SHFT                                                           0x0

#define HWIO_MSS_UIM0_UART_DM_TEST_RD_DATA_ADDR                                                                 (MSS_UIM0_UART_DM_REG_BASE      + 0x00000090)
#define HWIO_MSS_UIM0_UART_DM_TEST_RD_DATA_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TEST_RD_DATA_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TEST_RD_DATA_ADDR, HWIO_MSS_UIM0_UART_DM_TEST_RD_DATA_RMSK)
#define HWIO_MSS_UIM0_UART_DM_TEST_RD_DATA_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_TEST_RD_DATA_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_TEST_RD_DATA_TEST_RD_DATA_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TEST_RD_DATA_TEST_RD_DATA_SHFT                                                           0x0

#define HWIO_MSS_UIM0_UART_DM_CSR_ADDR                                                                          (MSS_UIM0_UART_DM_REG_BASE      + 0x000000a0)
#define HWIO_MSS_UIM0_UART_DM_CSR_RMSK                                                                                0xff
#define HWIO_MSS_UIM0_UART_DM_CSR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_CSR_ADDR, HWIO_MSS_UIM0_UART_DM_CSR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_CSR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_CSR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_CSR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_CSR_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_CSR_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_CSR_IN)
#define HWIO_MSS_UIM0_UART_DM_CSR_UART_RX_CLK_SEL_BMSK                                                                0xf0
#define HWIO_MSS_UIM0_UART_DM_CSR_UART_RX_CLK_SEL_SHFT                                                                 0x4
#define HWIO_MSS_UIM0_UART_DM_CSR_UART_TX_CLK_SEL_BMSK                                                                 0xf
#define HWIO_MSS_UIM0_UART_DM_CSR_UART_TX_CLK_SEL_SHFT                                                                 0x0

#define HWIO_MSS_UIM0_UART_DM_SR_ADDR                                                                           (MSS_UIM0_UART_DM_REG_BASE      + 0x000000a4)
#define HWIO_MSS_UIM0_UART_DM_SR_RMSK                                                                                0xfff
#define HWIO_MSS_UIM0_UART_DM_SR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_SR_ADDR, HWIO_MSS_UIM0_UART_DM_SR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_SR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_SR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_SR_TRANS_END_TRIGGER_BMSK                                                              0xc00
#define HWIO_MSS_UIM0_UART_DM_SR_TRANS_END_TRIGGER_SHFT                                                                0xa
#define HWIO_MSS_UIM0_UART_DM_SR_TRANS_ACTIVE_BMSK                                                                   0x200
#define HWIO_MSS_UIM0_UART_DM_SR_TRANS_ACTIVE_SHFT                                                                     0x9
#define HWIO_MSS_UIM0_UART_DM_SR_RX_BREAK_START_LAST_BMSK                                                            0x100
#define HWIO_MSS_UIM0_UART_DM_SR_RX_BREAK_START_LAST_SHFT                                                              0x8
#define HWIO_MSS_UIM0_UART_DM_SR_HUNT_CHAR_BMSK                                                                       0x80
#define HWIO_MSS_UIM0_UART_DM_SR_HUNT_CHAR_SHFT                                                                        0x7
#define HWIO_MSS_UIM0_UART_DM_SR_RX_BREAK_BMSK                                                                        0x40
#define HWIO_MSS_UIM0_UART_DM_SR_RX_BREAK_SHFT                                                                         0x6
#define HWIO_MSS_UIM0_UART_DM_SR_PAR_FRAME_ERR_BMSK                                                                   0x20
#define HWIO_MSS_UIM0_UART_DM_SR_PAR_FRAME_ERR_SHFT                                                                    0x5
#define HWIO_MSS_UIM0_UART_DM_SR_UART_OVERRUN_BMSK                                                                    0x10
#define HWIO_MSS_UIM0_UART_DM_SR_UART_OVERRUN_SHFT                                                                     0x4
#define HWIO_MSS_UIM0_UART_DM_SR_TXEMT_BMSK                                                                            0x8
#define HWIO_MSS_UIM0_UART_DM_SR_TXEMT_SHFT                                                                            0x3
#define HWIO_MSS_UIM0_UART_DM_SR_TXRDY_BMSK                                                                            0x4
#define HWIO_MSS_UIM0_UART_DM_SR_TXRDY_SHFT                                                                            0x2
#define HWIO_MSS_UIM0_UART_DM_SR_RXFULL_BMSK                                                                           0x2
#define HWIO_MSS_UIM0_UART_DM_SR_RXFULL_SHFT                                                                           0x1
#define HWIO_MSS_UIM0_UART_DM_SR_RXRDY_BMSK                                                                            0x1
#define HWIO_MSS_UIM0_UART_DM_SR_RXRDY_SHFT                                                                            0x0

#define HWIO_MSS_UIM0_UART_DM_CR_ADDR                                                                           (MSS_UIM0_UART_DM_REG_BASE      + 0x000000a8)
#define HWIO_MSS_UIM0_UART_DM_CR_RMSK                                                                                0xfff
#define HWIO_MSS_UIM0_UART_DM_CR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_CR_ADDR, HWIO_MSS_UIM0_UART_DM_CR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_CR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_CR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_CR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_CR_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_CR_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_CR_IN)
#define HWIO_MSS_UIM0_UART_DM_CR_CHANNEL_COMMAND_MSB_BMSK                                                            0x800
#define HWIO_MSS_UIM0_UART_DM_CR_CHANNEL_COMMAND_MSB_SHFT                                                              0xb
#define HWIO_MSS_UIM0_UART_DM_CR_GENERAL_COMMAND_BMSK                                                                0x700
#define HWIO_MSS_UIM0_UART_DM_CR_GENERAL_COMMAND_SHFT                                                                  0x8
#define HWIO_MSS_UIM0_UART_DM_CR_CHANNEL_COMMAND_LSB_BMSK                                                             0xf0
#define HWIO_MSS_UIM0_UART_DM_CR_CHANNEL_COMMAND_LSB_SHFT                                                              0x4
#define HWIO_MSS_UIM0_UART_DM_CR_UART_TX_DISABLE_BMSK                                                                  0x8
#define HWIO_MSS_UIM0_UART_DM_CR_UART_TX_DISABLE_SHFT                                                                  0x3
#define HWIO_MSS_UIM0_UART_DM_CR_UART_TX_EN_BMSK                                                                       0x4
#define HWIO_MSS_UIM0_UART_DM_CR_UART_TX_EN_SHFT                                                                       0x2
#define HWIO_MSS_UIM0_UART_DM_CR_UART_RX_DISABLE_BMSK                                                                  0x2
#define HWIO_MSS_UIM0_UART_DM_CR_UART_RX_DISABLE_SHFT                                                                  0x1
#define HWIO_MSS_UIM0_UART_DM_CR_UART_RX_EN_BMSK                                                                       0x1
#define HWIO_MSS_UIM0_UART_DM_CR_UART_RX_EN_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_MISR_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x000000ac)
#define HWIO_MSS_UIM0_UART_DM_MISR_RMSK                                                                            0x1ffff
#define HWIO_MSS_UIM0_UART_DM_MISR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_MISR_ADDR, HWIO_MSS_UIM0_UART_DM_MISR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_MISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_MISR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_MISR_UART_MISR_BMSK                                                                  0x1ffff
#define HWIO_MSS_UIM0_UART_DM_MISR_UART_MISR_SHFT                                                                      0x0

#define HWIO_MSS_UIM0_UART_DM_IMR_ADDR                                                                          (MSS_UIM0_UART_DM_REG_BASE      + 0x000000b0)
#define HWIO_MSS_UIM0_UART_DM_IMR_RMSK                                                                             0x1ffff
#define HWIO_MSS_UIM0_UART_DM_IMR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_IMR_ADDR, HWIO_MSS_UIM0_UART_DM_IMR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_IMR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_IMR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_IMR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_IMR_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_IMR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_IMR_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_IMR_IN)
#define HWIO_MSS_UIM0_UART_DM_IMR_WWT_IRQ_BMSK                                                                     0x10000
#define HWIO_MSS_UIM0_UART_DM_IMR_WWT_IRQ_SHFT                                                                        0x10
#define HWIO_MSS_UIM0_UART_DM_IMR_TXCOMP_IRQ_BMSK                                                                   0x8000
#define HWIO_MSS_UIM0_UART_DM_IMR_TXCOMP_IRQ_SHFT                                                                      0xf
#define HWIO_MSS_UIM0_UART_DM_IMR_RX_RD_ERROR_IRQ_BMSK                                                              0x4000
#define HWIO_MSS_UIM0_UART_DM_IMR_RX_RD_ERROR_IRQ_SHFT                                                                 0xe
#define HWIO_MSS_UIM0_UART_DM_IMR_TX_WR_ERROR_IRQ_BMSK                                                              0x2000
#define HWIO_MSS_UIM0_UART_DM_IMR_TX_WR_ERROR_IRQ_SHFT                                                                 0xd
#define HWIO_MSS_UIM0_UART_DM_IMR_PAR_FRAME_ERR_IRQ_BMSK                                                            0x1000
#define HWIO_MSS_UIM0_UART_DM_IMR_PAR_FRAME_ERR_IRQ_SHFT                                                               0xc
#define HWIO_MSS_UIM0_UART_DM_IMR_RXBREAK_END_BMSK                                                                   0x800
#define HWIO_MSS_UIM0_UART_DM_IMR_RXBREAK_END_SHFT                                                                     0xb
#define HWIO_MSS_UIM0_UART_DM_IMR_RXBREAK_START_BMSK                                                                 0x400
#define HWIO_MSS_UIM0_UART_DM_IMR_RXBREAK_START_SHFT                                                                   0xa
#define HWIO_MSS_UIM0_UART_DM_IMR_TX_DONE_BMSK                                                                       0x200
#define HWIO_MSS_UIM0_UART_DM_IMR_TX_DONE_SHFT                                                                         0x9
#define HWIO_MSS_UIM0_UART_DM_IMR_TX_ERROR_BMSK                                                                      0x100
#define HWIO_MSS_UIM0_UART_DM_IMR_TX_ERROR_SHFT                                                                        0x8
#define HWIO_MSS_UIM0_UART_DM_IMR_TX_READY_BMSK                                                                       0x80
#define HWIO_MSS_UIM0_UART_DM_IMR_TX_READY_SHFT                                                                        0x7
#define HWIO_MSS_UIM0_UART_DM_IMR_CURRENT_CTS_BMSK                                                                    0x40
#define HWIO_MSS_UIM0_UART_DM_IMR_CURRENT_CTS_SHFT                                                                     0x6
#define HWIO_MSS_UIM0_UART_DM_IMR_DELTA_CTS_BMSK                                                                      0x20
#define HWIO_MSS_UIM0_UART_DM_IMR_DELTA_CTS_SHFT                                                                       0x5
#define HWIO_MSS_UIM0_UART_DM_IMR_RXLEV_BMSK                                                                          0x10
#define HWIO_MSS_UIM0_UART_DM_IMR_RXLEV_SHFT                                                                           0x4
#define HWIO_MSS_UIM0_UART_DM_IMR_RXSTALE_BMSK                                                                         0x8
#define HWIO_MSS_UIM0_UART_DM_IMR_RXSTALE_SHFT                                                                         0x3
#define HWIO_MSS_UIM0_UART_DM_IMR_RXBREAK_CHANGE_BMSK                                                                  0x4
#define HWIO_MSS_UIM0_UART_DM_IMR_RXBREAK_CHANGE_SHFT                                                                  0x2
#define HWIO_MSS_UIM0_UART_DM_IMR_RXHUNT_BMSK                                                                          0x2
#define HWIO_MSS_UIM0_UART_DM_IMR_RXHUNT_SHFT                                                                          0x1
#define HWIO_MSS_UIM0_UART_DM_IMR_TXLEV_BMSK                                                                           0x1
#define HWIO_MSS_UIM0_UART_DM_IMR_TXLEV_SHFT                                                                           0x0

#define HWIO_MSS_UIM0_UART_DM_ISR_ADDR                                                                          (MSS_UIM0_UART_DM_REG_BASE      + 0x000000b4)
#define HWIO_MSS_UIM0_UART_DM_ISR_RMSK                                                                             0x1ffff
#define HWIO_MSS_UIM0_UART_DM_ISR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_ISR_ADDR, HWIO_MSS_UIM0_UART_DM_ISR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_ISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_ISR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_ISR_WWT_IRQ_BMSK                                                                     0x10000
#define HWIO_MSS_UIM0_UART_DM_ISR_WWT_IRQ_SHFT                                                                        0x10
#define HWIO_MSS_UIM0_UART_DM_ISR_TXCOMP_IRQ_BMSK                                                                   0x8000
#define HWIO_MSS_UIM0_UART_DM_ISR_TXCOMP_IRQ_SHFT                                                                      0xf
#define HWIO_MSS_UIM0_UART_DM_ISR_RX_RD_ERROR_IRQ_BMSK                                                              0x4000
#define HWIO_MSS_UIM0_UART_DM_ISR_RX_RD_ERROR_IRQ_SHFT                                                                 0xe
#define HWIO_MSS_UIM0_UART_DM_ISR_TX_WR_ERROR_IRQ_BMSK                                                              0x2000
#define HWIO_MSS_UIM0_UART_DM_ISR_TX_WR_ERROR_IRQ_SHFT                                                                 0xd
#define HWIO_MSS_UIM0_UART_DM_ISR_PAR_FRAME_ERR_IRQ_BMSK                                                            0x1000
#define HWIO_MSS_UIM0_UART_DM_ISR_PAR_FRAME_ERR_IRQ_SHFT                                                               0xc
#define HWIO_MSS_UIM0_UART_DM_ISR_RXBREAK_END_BMSK                                                                   0x800
#define HWIO_MSS_UIM0_UART_DM_ISR_RXBREAK_END_SHFT                                                                     0xb
#define HWIO_MSS_UIM0_UART_DM_ISR_RXBREAK_START_BMSK                                                                 0x400
#define HWIO_MSS_UIM0_UART_DM_ISR_RXBREAK_START_SHFT                                                                   0xa
#define HWIO_MSS_UIM0_UART_DM_ISR_TX_DONE_BMSK                                                                       0x200
#define HWIO_MSS_UIM0_UART_DM_ISR_TX_DONE_SHFT                                                                         0x9
#define HWIO_MSS_UIM0_UART_DM_ISR_TX_ERROR_BMSK                                                                      0x100
#define HWIO_MSS_UIM0_UART_DM_ISR_TX_ERROR_SHFT                                                                        0x8
#define HWIO_MSS_UIM0_UART_DM_ISR_TX_READY_BMSK                                                                       0x80
#define HWIO_MSS_UIM0_UART_DM_ISR_TX_READY_SHFT                                                                        0x7
#define HWIO_MSS_UIM0_UART_DM_ISR_CURRENT_CTS_BMSK                                                                    0x40
#define HWIO_MSS_UIM0_UART_DM_ISR_CURRENT_CTS_SHFT                                                                     0x6
#define HWIO_MSS_UIM0_UART_DM_ISR_DELTA_CTS_BMSK                                                                      0x20
#define HWIO_MSS_UIM0_UART_DM_ISR_DELTA_CTS_SHFT                                                                       0x5
#define HWIO_MSS_UIM0_UART_DM_ISR_RXLEV_BMSK                                                                          0x10
#define HWIO_MSS_UIM0_UART_DM_ISR_RXLEV_SHFT                                                                           0x4
#define HWIO_MSS_UIM0_UART_DM_ISR_RXSTALE_BMSK                                                                         0x8
#define HWIO_MSS_UIM0_UART_DM_ISR_RXSTALE_SHFT                                                                         0x3
#define HWIO_MSS_UIM0_UART_DM_ISR_RXBREAK_CHANGE_BMSK                                                                  0x4
#define HWIO_MSS_UIM0_UART_DM_ISR_RXBREAK_CHANGE_SHFT                                                                  0x2
#define HWIO_MSS_UIM0_UART_DM_ISR_RXHUNT_BMSK                                                                          0x2
#define HWIO_MSS_UIM0_UART_DM_ISR_RXHUNT_SHFT                                                                          0x1
#define HWIO_MSS_UIM0_UART_DM_ISR_TXLEV_BMSK                                                                           0x1
#define HWIO_MSS_UIM0_UART_DM_ISR_TXLEV_SHFT                                                                           0x0

#define HWIO_MSS_UIM0_UART_DM_IRDA_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x000000b8)
#define HWIO_MSS_UIM0_UART_DM_IRDA_RMSK                                                                               0x1f
#define HWIO_MSS_UIM0_UART_DM_IRDA_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_IRDA_ADDR, HWIO_MSS_UIM0_UART_DM_IRDA_RMSK)
#define HWIO_MSS_UIM0_UART_DM_IRDA_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_IRDA_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_IRDA_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_IRDA_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_IRDA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_IRDA_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_IRDA_IN)
#define HWIO_MSS_UIM0_UART_DM_IRDA_MEDIUM_RATE_EN_BMSK                                                                0x10
#define HWIO_MSS_UIM0_UART_DM_IRDA_MEDIUM_RATE_EN_SHFT                                                                 0x4
#define HWIO_MSS_UIM0_UART_DM_IRDA_IRDA_LOOPBACK_BMSK                                                                  0x8
#define HWIO_MSS_UIM0_UART_DM_IRDA_IRDA_LOOPBACK_SHFT                                                                  0x3
#define HWIO_MSS_UIM0_UART_DM_IRDA_INVERT_IRDA_TX_BMSK                                                                 0x4
#define HWIO_MSS_UIM0_UART_DM_IRDA_INVERT_IRDA_TX_SHFT                                                                 0x2
#define HWIO_MSS_UIM0_UART_DM_IRDA_INVERT_IRDA_RX_BMSK                                                                 0x2
#define HWIO_MSS_UIM0_UART_DM_IRDA_INVERT_IRDA_RX_SHFT                                                                 0x1
#define HWIO_MSS_UIM0_UART_DM_IRDA_IRDA_EN_BMSK                                                                        0x1
#define HWIO_MSS_UIM0_UART_DM_IRDA_IRDA_EN_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_RX_TOTAL_SNAP_ADDR                                                                (MSS_UIM0_UART_DM_REG_BASE      + 0x000000bc)
#define HWIO_MSS_UIM0_UART_DM_RX_TOTAL_SNAP_RMSK                                                                  0xffffff
#define HWIO_MSS_UIM0_UART_DM_RX_TOTAL_SNAP_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RX_TOTAL_SNAP_ADDR, HWIO_MSS_UIM0_UART_DM_RX_TOTAL_SNAP_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RX_TOTAL_SNAP_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RX_TOTAL_SNAP_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RX_TOTAL_SNAP_RX_TOTAL_BYTES_BMSK                                                   0xffffff
#define HWIO_MSS_UIM0_UART_DM_RX_TOTAL_SNAP_RX_TOTAL_BYTES_SHFT                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_WWT_TIMEOUT_ADDR                                                                  (MSS_UIM0_UART_DM_REG_BASE      + 0x000000c0)
#define HWIO_MSS_UIM0_UART_DM_WWT_TIMEOUT_RMSK                                                                   0x1ffffff
#define HWIO_MSS_UIM0_UART_DM_WWT_TIMEOUT_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_WWT_TIMEOUT_ADDR, HWIO_MSS_UIM0_UART_DM_WWT_TIMEOUT_RMSK)
#define HWIO_MSS_UIM0_UART_DM_WWT_TIMEOUT_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_WWT_TIMEOUT_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_WWT_TIMEOUT_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_WWT_TIMEOUT_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_WWT_TIMEOUT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_WWT_TIMEOUT_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_WWT_TIMEOUT_IN)
#define HWIO_MSS_UIM0_UART_DM_WWT_TIMEOUT_WWT_TIMEOUT_BMSK                                                       0x1ffffff
#define HWIO_MSS_UIM0_UART_DM_WWT_TIMEOUT_WWT_TIMEOUT_SHFT                                                             0x0

#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_ADDR                                                                     (MSS_UIM0_UART_DM_REG_BASE      + 0x000000c4)
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_RMSK                                                                       0xf06731
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_CLK_CTRL_ADDR, HWIO_MSS_UIM0_UART_DM_CLK_CTRL_RMSK)
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_CLK_CTRL_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_CLK_CTRL_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_CLK_CTRL_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_CLK_CTRL_IN)
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_UART_IRDA_CLK_CGC_OPEN_BMSK                                                0x800000
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_UART_IRDA_CLK_CGC_OPEN_SHFT                                                    0x17
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_UART_SIM_CLK_CGC_OPEN_BMSK                                                 0x400000
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_UART_SIM_CLK_CGC_OPEN_SHFT                                                     0x16
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_UART_RX_CLK_CGC_OPEN_BMSK                                                  0x200000
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_UART_RX_CLK_CGC_OPEN_SHFT                                                      0x15
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_UART_TX_CLK_CGC_OPEN_BMSK                                                  0x100000
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_UART_TX_CLK_CGC_OPEN_SHFT                                                      0x14
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_AHB_RX_BAM_CLK_CGC_OPEN_BMSK                                                 0x4000
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_AHB_RX_BAM_CLK_CGC_OPEN_SHFT                                                    0xe
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_AHB_TX_BAM_CLK_CGC_OPEN_BMSK                                                 0x2000
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_AHB_TX_BAM_CLK_CGC_OPEN_SHFT                                                    0xd
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_AHB_RX_CLK_CGC_OPEN_BMSK                                                      0x400
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_AHB_RX_CLK_CGC_OPEN_SHFT                                                        0xa
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_AHB_TX_CLK_CGC_OPEN_BMSK                                                      0x200
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_AHB_TX_CLK_CGC_OPEN_SHFT                                                        0x9
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_AHB_WR_CLK_CGC_OPEN_BMSK                                                      0x100
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_AHB_WR_CLK_CGC_OPEN_SHFT                                                        0x8
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_RX_ENABLE_CGC_OPT_BMSK                                                         0x20
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_RX_ENABLE_CGC_OPT_SHFT                                                          0x5
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_TX_ENABLE_CGC_OPT_BMSK                                                         0x10
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_TX_ENABLE_CGC_OPT_SHFT                                                          0x4
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_AHB_CLK_CGC_CLOSE_BMSK                                                          0x1
#define HWIO_MSS_UIM0_UART_DM_CLK_CTRL_AHB_CLK_CGC_CLOSE_SHFT                                                          0x0

#define HWIO_MSS_UIM0_UART_DM_BCR_ADDR                                                                          (MSS_UIM0_UART_DM_REG_BASE      + 0x000000c8)
#define HWIO_MSS_UIM0_UART_DM_BCR_RMSK                                                                                0x37
#define HWIO_MSS_UIM0_UART_DM_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_BCR_ADDR, HWIO_MSS_UIM0_UART_DM_BCR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_BCR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_BCR_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_BCR_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_BCR_IN)
#define HWIO_MSS_UIM0_UART_DM_BCR_RX_DMRX_1BYTE_RES_EN_BMSK                                                           0x20
#define HWIO_MSS_UIM0_UART_DM_BCR_RX_DMRX_1BYTE_RES_EN_SHFT                                                            0x5
#define HWIO_MSS_UIM0_UART_DM_BCR_RX_STALE_IRQ_DMRX_EQUAL_BMSK                                                        0x10
#define HWIO_MSS_UIM0_UART_DM_BCR_RX_STALE_IRQ_DMRX_EQUAL_SHFT                                                         0x4
#define HWIO_MSS_UIM0_UART_DM_BCR_RX_DMRX_LOW_EN_BMSK                                                                  0x4
#define HWIO_MSS_UIM0_UART_DM_BCR_RX_DMRX_LOW_EN_SHFT                                                                  0x2
#define HWIO_MSS_UIM0_UART_DM_BCR_STALE_IRQ_EMPTY_BMSK                                                                 0x2
#define HWIO_MSS_UIM0_UART_DM_BCR_STALE_IRQ_EMPTY_SHFT                                                                 0x1
#define HWIO_MSS_UIM0_UART_DM_BCR_TX_BREAK_DISABLE_BMSK                                                                0x1
#define HWIO_MSS_UIM0_UART_DM_BCR_TX_BREAK_DISABLE_SHFT                                                                0x0

#define HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_ADDR                                                                (MSS_UIM0_UART_DM_REG_BASE      + 0x000000cc)
#define HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_RMSK                                                                       0x7
#define HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_ADDR, HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_IN)
#define HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_RX_DMRX_CYCLIC_EN_BMSK                                                     0x4
#define HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_RX_DMRX_CYCLIC_EN_SHFT                                                     0x2
#define HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_RX_TRANS_AUTO_RE_ACTIVATE_BMSK                                             0x2
#define HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_RX_TRANS_AUTO_RE_ACTIVATE_SHFT                                             0x1
#define HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_RX_STALE_AUTO_RE_EN_BMSK                                                   0x1
#define HWIO_MSS_UIM0_UART_DM_RX_TRANS_CTRL_RX_STALE_AUTO_RE_EN_SHFT                                                   0x0

#define HWIO_MSS_UIM0_UART_DM_DMRX_DBG_ADDR                                                                     (MSS_UIM0_UART_DM_REG_BASE      + 0x000000d0)
#define HWIO_MSS_UIM0_UART_DM_DMRX_DBG_RMSK                                                                      0x1ffffff
#define HWIO_MSS_UIM0_UART_DM_DMRX_DBG_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_DMRX_DBG_ADDR, HWIO_MSS_UIM0_UART_DM_DMRX_DBG_RMSK)
#define HWIO_MSS_UIM0_UART_DM_DMRX_DBG_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_DMRX_DBG_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_DMRX_DBG_UART_DM_DMRX_VAL_BMSK                                                     0x1ffffff
#define HWIO_MSS_UIM0_UART_DM_DMRX_DBG_UART_DM_DMRX_VAL_SHFT                                                           0x0

#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_ADDR                                                                   (MSS_UIM0_UART_DM_REG_BASE      + 0x000000d4)
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_RMSK                                                                   0x37377771
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_FSM_STATUS_ADDR, HWIO_MSS_UIM0_UART_DM_FSM_STATUS_RMSK)
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_FSM_STATUS_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_TX_COMP_FSM_BMSK                                                       0x30000000
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_TX_COMP_FSM_SHFT                                                             0x1c
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_RX_PACK_FSM_BMSK                                                        0x7000000
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_RX_PACK_FSM_SHFT                                                             0x18
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_RX_TRANS_FSM_BMSK                                                        0x300000
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_RX_TRANS_FSM_SHFT                                                            0x14
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_TX_TRANS_FSM_BMSK                                                         0x70000
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_TX_TRANS_FSM_SHFT                                                            0x10
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_RX_PRO_TRANS_END_FSM_BMSK                                                  0x7000
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_RX_PRO_TRANS_END_FSM_SHFT                                                     0xc
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_RX_PRO_ACTIVE_FSM_BMSK                                                      0x700
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_RX_PRO_ACTIVE_FSM_SHFT                                                        0x8
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_TX_CON_TRANS_END_FSM_BMSK                                                    0x70
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_TX_CON_TRANS_END_FSM_SHFT                                                     0x4
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_RX_TRANSFER_ACTIVE_BMSK                                                       0x1
#define HWIO_MSS_UIM0_UART_DM_FSM_STATUS_RX_TRANSFER_ACTIVE_SHFT                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_HW_VERSION_ADDR                                                                   (MSS_UIM0_UART_DM_REG_BASE      + 0x000000d8)
#define HWIO_MSS_UIM0_UART_DM_HW_VERSION_RMSK                                                                   0xffffffff
#define HWIO_MSS_UIM0_UART_DM_HW_VERSION_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_HW_VERSION_ADDR, HWIO_MSS_UIM0_UART_DM_HW_VERSION_RMSK)
#define HWIO_MSS_UIM0_UART_DM_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_HW_VERSION_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_HW_VERSION_HW_VERSION_MAJOR_BMSK                                                  0xf0000000
#define HWIO_MSS_UIM0_UART_DM_HW_VERSION_HW_VERSION_MAJOR_SHFT                                                        0x1c
#define HWIO_MSS_UIM0_UART_DM_HW_VERSION_HW_VERSION_MINOR_BMSK                                                   0xfff0000
#define HWIO_MSS_UIM0_UART_DM_HW_VERSION_HW_VERSION_MINOR_SHFT                                                        0x10
#define HWIO_MSS_UIM0_UART_DM_HW_VERSION_HW_VERSION_STEP_BMSK                                                       0xffff
#define HWIO_MSS_UIM0_UART_DM_HW_VERSION_HW_VERSION_STEP_SHFT                                                          0x0

#define HWIO_MSS_UIM0_UART_DM_GENERICS_ADDR                                                                     (MSS_UIM0_UART_DM_REG_BASE      + 0x000000dc)
#define HWIO_MSS_UIM0_UART_DM_GENERICS_RMSK                                                                           0xff
#define HWIO_MSS_UIM0_UART_DM_GENERICS_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_GENERICS_ADDR, HWIO_MSS_UIM0_UART_DM_GENERICS_RMSK)
#define HWIO_MSS_UIM0_UART_DM_GENERICS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_GENERICS_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_GENERICS_GENERIC_BAM_IFC_BMSK                                                           0x80
#define HWIO_MSS_UIM0_UART_DM_GENERICS_GENERIC_BAM_IFC_SHFT                                                            0x7
#define HWIO_MSS_UIM0_UART_DM_GENERICS_GENERIC_DM_IFC_BMSK                                                            0x40
#define HWIO_MSS_UIM0_UART_DM_GENERICS_GENERIC_DM_IFC_SHFT                                                             0x6
#define HWIO_MSS_UIM0_UART_DM_GENERICS_GENERIC_IRDA_IFC_BMSK                                                          0x20
#define HWIO_MSS_UIM0_UART_DM_GENERICS_GENERIC_IRDA_IFC_SHFT                                                           0x5
#define HWIO_MSS_UIM0_UART_DM_GENERICS_GENERIC_SIM_GLUE_BMSK                                                          0x10
#define HWIO_MSS_UIM0_UART_DM_GENERICS_GENERIC_SIM_GLUE_SHFT                                                           0x4
#define HWIO_MSS_UIM0_UART_DM_GENERICS_GENERIC_RAM_ADDR_WIDTH_BMSK                                                     0xf
#define HWIO_MSS_UIM0_UART_DM_GENERICS_GENERIC_RAM_ADDR_WIDTH_SHFT                                                     0x0

#define HWIO_MSS_UIM0_UART_DM_TF_ADDR                                                                           (MSS_UIM0_UART_DM_REG_BASE      + 0x00000100)
#define HWIO_MSS_UIM0_UART_DM_TF_RMSK                                                                           0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_UART_TF_BMSK                                                                   0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_UART_TF_SHFT                                                                          0x0

#define HWIO_MSS_UIM0_UART_DM_TF_2_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000104)
#define HWIO_MSS_UIM0_UART_DM_TF_2_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_2_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_2_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_2_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_2_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_TF_3_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000108)
#define HWIO_MSS_UIM0_UART_DM_TF_3_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_3_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_3_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_3_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_3_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_TF_4_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x0000010c)
#define HWIO_MSS_UIM0_UART_DM_TF_4_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_4_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_4_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_4_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_4_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_TF_5_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000110)
#define HWIO_MSS_UIM0_UART_DM_TF_5_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_5_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_5_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_5_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_5_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_TF_6_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000114)
#define HWIO_MSS_UIM0_UART_DM_TF_6_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_6_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_6_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_6_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_6_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_TF_7_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000118)
#define HWIO_MSS_UIM0_UART_DM_TF_7_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_7_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_7_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_7_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_7_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_TF_8_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x0000011c)
#define HWIO_MSS_UIM0_UART_DM_TF_8_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_8_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_8_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_8_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_8_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_TF_9_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000120)
#define HWIO_MSS_UIM0_UART_DM_TF_9_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_9_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_9_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_9_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_9_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_TF_10_ADDR                                                                        (MSS_UIM0_UART_DM_REG_BASE      + 0x00000124)
#define HWIO_MSS_UIM0_UART_DM_TF_10_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_10_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_10_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_10_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_10_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_TF_11_ADDR                                                                        (MSS_UIM0_UART_DM_REG_BASE      + 0x00000128)
#define HWIO_MSS_UIM0_UART_DM_TF_11_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_11_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_11_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_11_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_11_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_TF_12_ADDR                                                                        (MSS_UIM0_UART_DM_REG_BASE      + 0x0000012c)
#define HWIO_MSS_UIM0_UART_DM_TF_12_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_12_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_12_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_12_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_12_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_TF_13_ADDR                                                                        (MSS_UIM0_UART_DM_REG_BASE      + 0x00000130)
#define HWIO_MSS_UIM0_UART_DM_TF_13_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_13_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_13_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_13_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_13_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_TF_14_ADDR                                                                        (MSS_UIM0_UART_DM_REG_BASE      + 0x00000134)
#define HWIO_MSS_UIM0_UART_DM_TF_14_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_14_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_14_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_14_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_14_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_TF_15_ADDR                                                                        (MSS_UIM0_UART_DM_REG_BASE      + 0x00000138)
#define HWIO_MSS_UIM0_UART_DM_TF_15_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_15_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_15_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_15_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_15_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_TF_16_ADDR                                                                        (MSS_UIM0_UART_DM_REG_BASE      + 0x0000013c)
#define HWIO_MSS_UIM0_UART_DM_TF_16_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_16_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_TF_16_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_TF_16_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_TF_16_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_RF_ADDR                                                                           (MSS_UIM0_UART_DM_REG_BASE      + 0x00000140)
#define HWIO_MSS_UIM0_UART_DM_RF_RMSK                                                                           0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_ADDR, HWIO_MSS_UIM0_UART_DM_RF_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_UART_RF_BMSK                                                                   0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_UART_RF_SHFT                                                                          0x0

#define HWIO_MSS_UIM0_UART_DM_RF_2_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000144)
#define HWIO_MSS_UIM0_UART_DM_RF_2_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_2_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_2_ADDR, HWIO_MSS_UIM0_UART_DM_RF_2_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_2_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_2_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_2_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_2_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_RF_3_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000148)
#define HWIO_MSS_UIM0_UART_DM_RF_3_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_3_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_3_ADDR, HWIO_MSS_UIM0_UART_DM_RF_3_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_3_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_3_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_3_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_3_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_RF_4_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x0000014c)
#define HWIO_MSS_UIM0_UART_DM_RF_4_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_4_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_4_ADDR, HWIO_MSS_UIM0_UART_DM_RF_4_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_4_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_4_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_4_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_4_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_RF_5_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000150)
#define HWIO_MSS_UIM0_UART_DM_RF_5_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_5_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_5_ADDR, HWIO_MSS_UIM0_UART_DM_RF_5_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_5_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_5_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_5_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_5_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_RF_6_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000154)
#define HWIO_MSS_UIM0_UART_DM_RF_6_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_6_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_6_ADDR, HWIO_MSS_UIM0_UART_DM_RF_6_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_6_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_6_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_6_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_6_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_RF_7_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000158)
#define HWIO_MSS_UIM0_UART_DM_RF_7_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_7_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_7_ADDR, HWIO_MSS_UIM0_UART_DM_RF_7_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_7_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_7_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_7_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_7_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_RF_8_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x0000015c)
#define HWIO_MSS_UIM0_UART_DM_RF_8_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_8_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_8_ADDR, HWIO_MSS_UIM0_UART_DM_RF_8_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_8_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_8_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_8_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_8_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_RF_9_ADDR                                                                         (MSS_UIM0_UART_DM_REG_BASE      + 0x00000160)
#define HWIO_MSS_UIM0_UART_DM_RF_9_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_9_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_9_ADDR, HWIO_MSS_UIM0_UART_DM_RF_9_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_9_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_9_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_9_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_9_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_RF_10_ADDR                                                                        (MSS_UIM0_UART_DM_REG_BASE      + 0x00000164)
#define HWIO_MSS_UIM0_UART_DM_RF_10_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_10_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_10_ADDR, HWIO_MSS_UIM0_UART_DM_RF_10_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_10_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_10_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_10_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_10_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_RF_11_ADDR                                                                        (MSS_UIM0_UART_DM_REG_BASE      + 0x00000168)
#define HWIO_MSS_UIM0_UART_DM_RF_11_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_11_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_11_ADDR, HWIO_MSS_UIM0_UART_DM_RF_11_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_11_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_11_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_11_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_11_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_RF_12_ADDR                                                                        (MSS_UIM0_UART_DM_REG_BASE      + 0x0000016c)
#define HWIO_MSS_UIM0_UART_DM_RF_12_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_12_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_12_ADDR, HWIO_MSS_UIM0_UART_DM_RF_12_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_12_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_12_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_12_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_12_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_RF_13_ADDR                                                                        (MSS_UIM0_UART_DM_REG_BASE      + 0x00000170)
#define HWIO_MSS_UIM0_UART_DM_RF_13_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_13_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_13_ADDR, HWIO_MSS_UIM0_UART_DM_RF_13_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_13_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_13_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_13_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_13_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_RF_14_ADDR                                                                        (MSS_UIM0_UART_DM_REG_BASE      + 0x00000174)
#define HWIO_MSS_UIM0_UART_DM_RF_14_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_14_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_14_ADDR, HWIO_MSS_UIM0_UART_DM_RF_14_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_14_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_14_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_14_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_14_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_RF_15_ADDR                                                                        (MSS_UIM0_UART_DM_REG_BASE      + 0x00000178)
#define HWIO_MSS_UIM0_UART_DM_RF_15_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_15_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_15_ADDR, HWIO_MSS_UIM0_UART_DM_RF_15_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_15_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_15_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_15_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_15_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_RF_16_ADDR                                                                        (MSS_UIM0_UART_DM_REG_BASE      + 0x0000017c)
#define HWIO_MSS_UIM0_UART_DM_RF_16_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_16_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_16_ADDR, HWIO_MSS_UIM0_UART_DM_RF_16_RMSK)
#define HWIO_MSS_UIM0_UART_DM_RF_16_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_RF_16_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_RF_16_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM0_UART_DM_RF_16_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_ADDR                                                                      (MSS_UIM0_UART_DM_REG_BASE      + 0x00000180)
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_RMSK                                                                          0xf77f
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_UIM_CFG_ADDR, HWIO_MSS_UIM0_UART_DM_UIM_CFG_RMSK)
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_UIM_CFG_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_UIM_CFG_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_UIM_CFG_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_UIM_CFG_IN)
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_BATT_ALARM_QUICK_DROP_EN_BMSK                                                 0x8000
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_BATT_ALARM_QUICK_DROP_EN_SHFT                                                    0xf
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_TESTBUS_EN_BMSK                                                               0x4000
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_TESTBUS_EN_SHFT                                                                  0xe
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_SW_RESET_BMSK                                                                 0x2000
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_SW_RESET_SHFT                                                                    0xd
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_MODE18_BMSK                                                                   0x1000
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_MODE18_SHFT                                                                      0xc
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_PMIC_ALARM_EN_BMSK                                                             0x400
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_PMIC_ALARM_EN_SHFT                                                               0xa
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_BATT_ALARM_TRIGGER_EN_BMSK                                                     0x200
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_BATT_ALARM_TRIGGER_EN_SHFT                                                       0x9
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_UIM_RMV_TRIGGER_EN_BMSK                                                        0x100
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_UIM_RMV_TRIGGER_EN_SHFT                                                          0x8
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_UIM_CARD_EVENTS_ENABLE_BMSK                                                     0x40
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_UIM_CARD_EVENTS_ENABLE_SHFT                                                      0x6
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_UIM_PRESENT_POLARITY_BMSK                                                       0x20
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_UIM_PRESENT_POLARITY_SHFT                                                        0x5
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_EVENT_DEBOUNCE_TIME_BMSK                                                        0x1f
#define HWIO_MSS_UIM0_UART_DM_UIM_CFG_EVENT_DEBOUNCE_TIME_SHFT                                                         0x0

#define HWIO_MSS_UIM0_UART_DM_UIM_CMD_ADDR                                                                      (MSS_UIM0_UART_DM_REG_BASE      + 0x00000184)
#define HWIO_MSS_UIM0_UART_DM_UIM_CMD_RMSK                                                                             0x3
#define HWIO_MSS_UIM0_UART_DM_UIM_CMD_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_UIM_CMD_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_UIM_CMD_RECOVER_FROM_HW_DEACTIVATION_BMSK                                                0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_CMD_RECOVER_FROM_HW_DEACTIVATION_SHFT                                                0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_CMD_INITIATE_HW_DEACTIVATION_BMSK                                                    0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_CMD_INITIATE_HW_DEACTIVATION_SHFT                                                    0x0

#define HWIO_MSS_UIM0_UART_DM_UIM_IO_STATUS_ADDR                                                                (MSS_UIM0_UART_DM_REG_BASE      + 0x00000188)
#define HWIO_MSS_UIM0_UART_DM_UIM_IO_STATUS_RMSK                                                                       0x7
#define HWIO_MSS_UIM0_UART_DM_UIM_IO_STATUS_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_UIM_IO_STATUS_ADDR, HWIO_MSS_UIM0_UART_DM_UIM_IO_STATUS_RMSK)
#define HWIO_MSS_UIM0_UART_DM_UIM_IO_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_UIM_IO_STATUS_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_UIM_IO_STATUS_UIM_IO_WRITE_IN_PROGRESS_BMSK                                              0x4
#define HWIO_MSS_UIM0_UART_DM_UIM_IO_STATUS_UIM_IO_WRITE_IN_PROGRESS_SHFT                                              0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_IO_STATUS_UIM_DEACTIVATION_STATUS_BMSK                                               0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_IO_STATUS_UIM_DEACTIVATION_STATUS_SHFT                                               0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_IO_STATUS_CARD_PRESENCE_BMSK                                                         0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_IO_STATUS_CARD_PRESENCE_SHFT                                                         0x0

#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_ADDR                                                                  (MSS_UIM0_UART_DM_REG_BASE      + 0x0000018c)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_RMSK                                                                        0x1f
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_ADDR, HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_UIM_IO_WRITE_DONE_BMSK                                                      0x10
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_UIM_IO_WRITE_DONE_SHFT                                                       0x4
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_HW_SEQUENCE_FINISH_BMSK                                                      0x8
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_HW_SEQUENCE_FINISH_SHFT                                                      0x3
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_BATT_ALARM_BMSK                                                              0x4
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_BATT_ALARM_SHFT                                                              0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_UIM_CARD_INSERTION_BMSK                                                      0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_UIM_CARD_INSERTION_SHFT                                                      0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_UIM_CARD_REMOVAL_BMSK                                                        0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_ISR_UIM_CARD_REMOVAL_SHFT                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_ADDR                                                                 (MSS_UIM0_UART_DM_REG_BASE      + 0x00000190)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_RMSK                                                                       0x1f
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_ADDR, HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_UIM_IO_WRITE_DONE_BMSK                                                     0x10
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_UIM_IO_WRITE_DONE_SHFT                                                      0x4
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_HW_SEQUENCE_FINISH_BMSK                                                     0x8
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_HW_SEQUENCE_FINISH_SHFT                                                     0x3
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_BATT_ALARM_BMSK                                                             0x4
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_BATT_ALARM_SHFT                                                             0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_UIM_CARD_INSERTION_BMSK                                                     0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_UIM_CARD_INSERTION_SHFT                                                     0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_UIM_CARD_REMOVAL_BMSK                                                       0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_MISR_UIM_CARD_REMOVAL_SHFT                                                       0x0

#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_CLR_ADDR                                                                  (MSS_UIM0_UART_DM_REG_BASE      + 0x00000194)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_CLR_RMSK                                                                        0x1f
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_CLR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_UIM_IRQ_CLR_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_CLR_UIM_IO_WRITE_DONE_BMSK                                                      0x10
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_CLR_UIM_IO_WRITE_DONE_SHFT                                                       0x4
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_CLR_HW_SEQUENCE_FINISH_BMSK                                                      0x8
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_CLR_HW_SEQUENCE_FINISH_SHFT                                                      0x3
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_CLR_BATT_ALARM_BMSK                                                              0x4
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_CLR_BATT_ALARM_SHFT                                                              0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_CLR_UIM_CARD_INSERTION_BMSK                                                      0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_CLR_UIM_CARD_INSERTION_SHFT                                                      0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_CLR_UIM_CARD_REMOVAL_BMSK                                                        0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_CLR_UIM_CARD_REMOVAL_SHFT                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_ADDR                                                                  (MSS_UIM0_UART_DM_REG_BASE      + 0x00000198)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_RMSK                                                                        0x1f
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_ADDR, HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_RMSK)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_ADDR, m)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_ADDR,m,v,HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_IN)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_UIM_IO_WRITE_DONE_BMSK                                                      0x10
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_UIM_IO_WRITE_DONE_SHFT                                                       0x4
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_HW_SEQUENCE_FINISH_BMSK                                                      0x8
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_HW_SEQUENCE_FINISH_SHFT                                                      0x3
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_BATT_ALARM_BMSK                                                              0x4
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_BATT_ALARM_SHFT                                                              0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_UIM_CARD_INSERTION_BMSK                                                      0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_UIM_CARD_INSERTION_SHFT                                                      0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_UIM_CARD_REMOVAL_BMSK                                                        0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_UIM_CARD_REMOVAL_SHFT                                                        0x0

#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET_ADDR                                                              (MSS_UIM0_UART_DM_REG_BASE      + 0x0000019c)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET_RMSK                                                                    0x1f
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET_UIM_IO_WRITE_DONE_BMSK                                                  0x10
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET_UIM_IO_WRITE_DONE_SHFT                                                   0x4
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET_HW_SEQUENCE_FINISH_BMSK                                                  0x8
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET_HW_SEQUENCE_FINISH_SHFT                                                  0x3
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET_BATT_ALARM_BMSK                                                          0x4
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET_BATT_ALARM_SHFT                                                          0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_INSERTION_BMSK                                                  0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_INSERTION_SHFT                                                  0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_REMOVAL_BMSK                                                    0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_REMOVAL_SHFT                                                    0x0

#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_CLR_ADDR                                                              (MSS_UIM0_UART_DM_REG_BASE      + 0x000001a0)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_CLR_RMSK                                                                    0x1f
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_CLR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_CLR_ADDR,v)
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_CLR_UIM_IO_WRITE_DONE_BMSK                                                  0x10
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_CLR_UIM_IO_WRITE_DONE_SHFT                                                   0x4
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_CLR_HW_SEQUENCE_FINISH_BMSK                                                  0x8
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_CLR_HW_SEQUENCE_FINISH_SHFT                                                  0x3
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_CLR_BATT_ALARM_BMSK                                                          0x4
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_CLR_BATT_ALARM_SHFT                                                          0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_INSERTION_BMSK                                                  0x2
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_INSERTION_SHFT                                                  0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_REMOVAL_BMSK                                                    0x1
#define HWIO_MSS_UIM0_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_REMOVAL_SHFT                                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: MSS_UIM1_UART_DM
 *--------------------------------------------------------------------------*/

#define MSS_UIM1_UART_DM_REG_BASE                                                                               (MODEM_TOP_BASE      + 0x00194000)

#define HWIO_MSS_UIM1_UART_DM_MR1_ADDR                                                                          (MSS_UIM1_UART_DM_REG_BASE      + 0x00000000)
#define HWIO_MSS_UIM1_UART_DM_MR1_RMSK                                                                          0xffffffff
#define HWIO_MSS_UIM1_UART_DM_MR1_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_MR1_ADDR, HWIO_MSS_UIM1_UART_DM_MR1_RMSK)
#define HWIO_MSS_UIM1_UART_DM_MR1_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_MR1_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_MR1_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_MR1_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_MR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_MR1_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_MR1_IN)
#define HWIO_MSS_UIM1_UART_DM_MR1_AUTO_RFR_LEVEL1_BMSK                                                          0xffffff00
#define HWIO_MSS_UIM1_UART_DM_MR1_AUTO_RFR_LEVEL1_SHFT                                                                 0x8
#define HWIO_MSS_UIM1_UART_DM_MR1_RX_RDY_CTL_BMSK                                                                     0x80
#define HWIO_MSS_UIM1_UART_DM_MR1_RX_RDY_CTL_SHFT                                                                      0x7
#define HWIO_MSS_UIM1_UART_DM_MR1_CTS_CTL_BMSK                                                                        0x40
#define HWIO_MSS_UIM1_UART_DM_MR1_CTS_CTL_SHFT                                                                         0x6
#define HWIO_MSS_UIM1_UART_DM_MR1_AUTO_RFR_LEVEL0_BMSK                                                                0x3f
#define HWIO_MSS_UIM1_UART_DM_MR1_AUTO_RFR_LEVEL0_SHFT                                                                 0x0

#define HWIO_MSS_UIM1_UART_DM_MR2_ADDR                                                                          (MSS_UIM1_UART_DM_REG_BASE      + 0x00000004)
#define HWIO_MSS_UIM1_UART_DM_MR2_RMSK                                                                               0x7ff
#define HWIO_MSS_UIM1_UART_DM_MR2_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_MR2_ADDR, HWIO_MSS_UIM1_UART_DM_MR2_RMSK)
#define HWIO_MSS_UIM1_UART_DM_MR2_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_MR2_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_MR2_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_MR2_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_MR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_MR2_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_MR2_IN)
#define HWIO_MSS_UIM1_UART_DM_MR2_RFR_CTS_LOOPBACK_BMSK                                                              0x400
#define HWIO_MSS_UIM1_UART_DM_MR2_RFR_CTS_LOOPBACK_SHFT                                                                0xa
#define HWIO_MSS_UIM1_UART_DM_MR2_RX_ERROR_CHAR_OFF_BMSK                                                             0x200
#define HWIO_MSS_UIM1_UART_DM_MR2_RX_ERROR_CHAR_OFF_SHFT                                                               0x9
#define HWIO_MSS_UIM1_UART_DM_MR2_RX_BREAK_ZERO_CHAR_OFF_BMSK                                                        0x100
#define HWIO_MSS_UIM1_UART_DM_MR2_RX_BREAK_ZERO_CHAR_OFF_SHFT                                                          0x8
#define HWIO_MSS_UIM1_UART_DM_MR2_LOOPBACK_BMSK                                                                       0x80
#define HWIO_MSS_UIM1_UART_DM_MR2_LOOPBACK_SHFT                                                                        0x7
#define HWIO_MSS_UIM1_UART_DM_MR2_ERROR_MODE_BMSK                                                                     0x40
#define HWIO_MSS_UIM1_UART_DM_MR2_ERROR_MODE_SHFT                                                                      0x6
#define HWIO_MSS_UIM1_UART_DM_MR2_BITS_PER_CHAR_BMSK                                                                  0x30
#define HWIO_MSS_UIM1_UART_DM_MR2_BITS_PER_CHAR_SHFT                                                                   0x4
#define HWIO_MSS_UIM1_UART_DM_MR2_STOP_BIT_LEN_BMSK                                                                    0xc
#define HWIO_MSS_UIM1_UART_DM_MR2_STOP_BIT_LEN_SHFT                                                                    0x2
#define HWIO_MSS_UIM1_UART_DM_MR2_PARITY_MODE_BMSK                                                                     0x3
#define HWIO_MSS_UIM1_UART_DM_MR2_PARITY_MODE_SHFT                                                                     0x0

#define HWIO_MSS_UIM1_UART_DM_CSR_SR_DEPRECATED_ADDR                                                            (MSS_UIM1_UART_DM_REG_BASE      + 0x00000008)
#define HWIO_MSS_UIM1_UART_DM_CSR_SR_DEPRECATED_RMSK                                                            0xffffffff
#define HWIO_MSS_UIM1_UART_DM_CSR_SR_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_CSR_SR_DEPRECATED_ADDR, HWIO_MSS_UIM1_UART_DM_CSR_SR_DEPRECATED_RMSK)
#define HWIO_MSS_UIM1_UART_DM_CSR_SR_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_CSR_SR_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_CSR_SR_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_CSR_SR_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_CSR_SR_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_CSR_SR_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_CSR_SR_DEPRECATED_IN)
#define HWIO_MSS_UIM1_UART_DM_CSR_SR_DEPRECATED_UART_DM_CSR_SR_DEPRECATED_BMSK                                  0xffffffff
#define HWIO_MSS_UIM1_UART_DM_CSR_SR_DEPRECATED_UART_DM_CSR_SR_DEPRECATED_SHFT                                         0x0

#define HWIO_MSS_UIM1_UART_DM_CR_MISR_DEPRECATED_ADDR                                                           (MSS_UIM1_UART_DM_REG_BASE      + 0x00000010)
#define HWIO_MSS_UIM1_UART_DM_CR_MISR_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM1_UART_DM_CR_MISR_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_CR_MISR_DEPRECATED_ADDR, HWIO_MSS_UIM1_UART_DM_CR_MISR_DEPRECATED_RMSK)
#define HWIO_MSS_UIM1_UART_DM_CR_MISR_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_CR_MISR_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_CR_MISR_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_CR_MISR_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_CR_MISR_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_CR_MISR_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_CR_MISR_DEPRECATED_IN)
#define HWIO_MSS_UIM1_UART_DM_CR_MISR_DEPRECATED_UART_DM_CR_MISR_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_CR_MISR_DEPRECATED_UART_DM_CR_MISR_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM1_UART_DM_IMR_ISR_DEPRECATED_ADDR                                                           (MSS_UIM1_UART_DM_REG_BASE      + 0x00000014)
#define HWIO_MSS_UIM1_UART_DM_IMR_ISR_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM1_UART_DM_IMR_ISR_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_IMR_ISR_DEPRECATED_ADDR, HWIO_MSS_UIM1_UART_DM_IMR_ISR_DEPRECATED_RMSK)
#define HWIO_MSS_UIM1_UART_DM_IMR_ISR_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_IMR_ISR_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_IMR_ISR_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_IMR_ISR_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_IMR_ISR_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_IMR_ISR_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_IMR_ISR_DEPRECATED_IN)
#define HWIO_MSS_UIM1_UART_DM_IMR_ISR_DEPRECATED_UART_DM_IMR_ISR_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_IMR_ISR_DEPRECATED_UART_DM_IMR_ISR_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM1_UART_DM_IPR_ADDR                                                                          (MSS_UIM1_UART_DM_REG_BASE      + 0x00000018)
#define HWIO_MSS_UIM1_UART_DM_IPR_RMSK                                                                          0xffffffdf
#define HWIO_MSS_UIM1_UART_DM_IPR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_IPR_ADDR, HWIO_MSS_UIM1_UART_DM_IPR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_IPR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_IPR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_IPR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_IPR_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_IPR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_IPR_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_IPR_IN)
#define HWIO_MSS_UIM1_UART_DM_IPR_STALE_TIMEOUT_MSB_BMSK                                                        0xffffff80
#define HWIO_MSS_UIM1_UART_DM_IPR_STALE_TIMEOUT_MSB_SHFT                                                               0x7
#define HWIO_MSS_UIM1_UART_DM_IPR_SAMPLE_DATA_BMSK                                                                    0x40
#define HWIO_MSS_UIM1_UART_DM_IPR_SAMPLE_DATA_SHFT                                                                     0x6
#define HWIO_MSS_UIM1_UART_DM_IPR_STALE_TIMEOUT_LSB_BMSK                                                              0x1f
#define HWIO_MSS_UIM1_UART_DM_IPR_STALE_TIMEOUT_LSB_SHFT                                                               0x0

#define HWIO_MSS_UIM1_UART_DM_TFWR_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x0000001c)
#define HWIO_MSS_UIM1_UART_DM_TFWR_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TFWR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TFWR_ADDR, HWIO_MSS_UIM1_UART_DM_TFWR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_TFWR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TFWR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_TFWR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TFWR_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TFWR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_TFWR_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_TFWR_IN)
#define HWIO_MSS_UIM1_UART_DM_TFWR_TFW_BMSK                                                                     0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TFWR_TFW_SHFT                                                                            0x0

#define HWIO_MSS_UIM1_UART_DM_RFWR_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000020)
#define HWIO_MSS_UIM1_UART_DM_RFWR_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RFWR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RFWR_ADDR, HWIO_MSS_UIM1_UART_DM_RFWR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RFWR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RFWR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RFWR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_RFWR_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_RFWR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_RFWR_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_RFWR_IN)
#define HWIO_MSS_UIM1_UART_DM_RFWR_RFW_BMSK                                                                     0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RFWR_RFW_SHFT                                                                            0x0

#define HWIO_MSS_UIM1_UART_DM_HCR_ADDR                                                                          (MSS_UIM1_UART_DM_REG_BASE      + 0x00000024)
#define HWIO_MSS_UIM1_UART_DM_HCR_RMSK                                                                                0xff
#define HWIO_MSS_UIM1_UART_DM_HCR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_HCR_ADDR, HWIO_MSS_UIM1_UART_DM_HCR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_HCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_HCR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_HCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_HCR_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_HCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_HCR_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_HCR_IN)
#define HWIO_MSS_UIM1_UART_DM_HCR_DATA_BMSK                                                                           0xff
#define HWIO_MSS_UIM1_UART_DM_HCR_DATA_SHFT                                                                            0x0

#define HWIO_MSS_UIM1_UART_DM_DMRX_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000034)
#define HWIO_MSS_UIM1_UART_DM_DMRX_RMSK                                                                          0x1ffffff
#define HWIO_MSS_UIM1_UART_DM_DMRX_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_DMRX_ADDR, HWIO_MSS_UIM1_UART_DM_DMRX_RMSK)
#define HWIO_MSS_UIM1_UART_DM_DMRX_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_DMRX_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_DMRX_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_DMRX_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_DMRX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_DMRX_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_DMRX_IN)
#define HWIO_MSS_UIM1_UART_DM_DMRX_RX_DM_CRCI_CHARS_BMSK                                                         0x1ffffff
#define HWIO_MSS_UIM1_UART_DM_DMRX_RX_DM_CRCI_CHARS_SHFT                                                               0x0

#define HWIO_MSS_UIM1_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR                                                (MSS_UIM1_UART_DM_REG_BASE      + 0x00000038)
#define HWIO_MSS_UIM1_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_RMSK                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR, HWIO_MSS_UIM1_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_RMSK)
#define HWIO_MSS_UIM1_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_IN)
#define HWIO_MSS_UIM1_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_BMSK          0xffffffff
#define HWIO_MSS_UIM1_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_SHFT                 0x0

#define HWIO_MSS_UIM1_UART_DM_DMEN_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x0000003c)
#define HWIO_MSS_UIM1_UART_DM_DMEN_RMSK                                                                               0x3c
#define HWIO_MSS_UIM1_UART_DM_DMEN_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_DMEN_ADDR, HWIO_MSS_UIM1_UART_DM_DMEN_RMSK)
#define HWIO_MSS_UIM1_UART_DM_DMEN_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_DMEN_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_DMEN_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_DMEN_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_DMEN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_DMEN_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_DMEN_IN)
#define HWIO_MSS_UIM1_UART_DM_DMEN_RX_SC_ENABLE_BMSK                                                                  0x20
#define HWIO_MSS_UIM1_UART_DM_DMEN_RX_SC_ENABLE_SHFT                                                                   0x5
#define HWIO_MSS_UIM1_UART_DM_DMEN_TX_SC_ENABLE_BMSK                                                                  0x10
#define HWIO_MSS_UIM1_UART_DM_DMEN_TX_SC_ENABLE_SHFT                                                                   0x4
#define HWIO_MSS_UIM1_UART_DM_DMEN_RX_BAM_ENABLE_BMSK                                                                  0x8
#define HWIO_MSS_UIM1_UART_DM_DMEN_RX_BAM_ENABLE_SHFT                                                                  0x3
#define HWIO_MSS_UIM1_UART_DM_DMEN_TX_BAM_ENABLE_BMSK                                                                  0x4
#define HWIO_MSS_UIM1_UART_DM_DMEN_TX_BAM_ENABLE_SHFT                                                                  0x2

#define HWIO_MSS_UIM1_UART_DM_NO_CHARS_FOR_TX_ADDR                                                              (MSS_UIM1_UART_DM_REG_BASE      + 0x00000040)
#define HWIO_MSS_UIM1_UART_DM_NO_CHARS_FOR_TX_RMSK                                                                0xffffff
#define HWIO_MSS_UIM1_UART_DM_NO_CHARS_FOR_TX_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_NO_CHARS_FOR_TX_ADDR, HWIO_MSS_UIM1_UART_DM_NO_CHARS_FOR_TX_RMSK)
#define HWIO_MSS_UIM1_UART_DM_NO_CHARS_FOR_TX_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_NO_CHARS_FOR_TX_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_NO_CHARS_FOR_TX_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_NO_CHARS_FOR_TX_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_NO_CHARS_FOR_TX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_NO_CHARS_FOR_TX_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_NO_CHARS_FOR_TX_IN)
#define HWIO_MSS_UIM1_UART_DM_NO_CHARS_FOR_TX_TX_TOTAL_TRANS_LEN_BMSK                                             0xffffff
#define HWIO_MSS_UIM1_UART_DM_NO_CHARS_FOR_TX_TX_TOTAL_TRANS_LEN_SHFT                                                  0x0

#define HWIO_MSS_UIM1_UART_DM_BADR_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000044)
#define HWIO_MSS_UIM1_UART_DM_BADR_RMSK                                                                         0xfffffffc
#define HWIO_MSS_UIM1_UART_DM_BADR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_BADR_ADDR, HWIO_MSS_UIM1_UART_DM_BADR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_BADR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_BADR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_BADR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_BADR_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_BADR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_BADR_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_BADR_IN)
#define HWIO_MSS_UIM1_UART_DM_BADR_RX_BASE_ADDR_BMSK                                                            0xfffffffc
#define HWIO_MSS_UIM1_UART_DM_BADR_RX_BASE_ADDR_SHFT                                                                   0x2

#define HWIO_MSS_UIM1_UART_DM_TESTSL_ADDR                                                                       (MSS_UIM1_UART_DM_REG_BASE      + 0x00000048)
#define HWIO_MSS_UIM1_UART_DM_TESTSL_RMSK                                                                             0x1f
#define HWIO_MSS_UIM1_UART_DM_TESTSL_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TESTSL_ADDR, HWIO_MSS_UIM1_UART_DM_TESTSL_RMSK)
#define HWIO_MSS_UIM1_UART_DM_TESTSL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TESTSL_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_TESTSL_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TESTSL_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TESTSL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_TESTSL_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_TESTSL_IN)
#define HWIO_MSS_UIM1_UART_DM_TESTSL_TEST_EN_BMSK                                                                     0x10
#define HWIO_MSS_UIM1_UART_DM_TESTSL_TEST_EN_SHFT                                                                      0x4
#define HWIO_MSS_UIM1_UART_DM_TESTSL_TEST_SEL_BMSK                                                                     0xf
#define HWIO_MSS_UIM1_UART_DM_TESTSL_TEST_SEL_SHFT                                                                     0x0

#define HWIO_MSS_UIM1_UART_DM_TXFS_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x0000004c)
#define HWIO_MSS_UIM1_UART_DM_TXFS_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TXFS_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TXFS_ADDR, HWIO_MSS_UIM1_UART_DM_TXFS_RMSK)
#define HWIO_MSS_UIM1_UART_DM_TXFS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TXFS_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_TXFS_TX_FIFO_STATE_MSB_BMSK                                                       0xffffc000
#define HWIO_MSS_UIM1_UART_DM_TXFS_TX_FIFO_STATE_MSB_SHFT                                                              0xe
#define HWIO_MSS_UIM1_UART_DM_TXFS_TX_ASYNC_FIFO_STATE_BMSK                                                         0x3c00
#define HWIO_MSS_UIM1_UART_DM_TXFS_TX_ASYNC_FIFO_STATE_SHFT                                                            0xa
#define HWIO_MSS_UIM1_UART_DM_TXFS_TX_BUFFER_STATE_BMSK                                                              0x380
#define HWIO_MSS_UIM1_UART_DM_TXFS_TX_BUFFER_STATE_SHFT                                                                0x7
#define HWIO_MSS_UIM1_UART_DM_TXFS_TX_FIFO_STATE_LSB_BMSK                                                             0x7f
#define HWIO_MSS_UIM1_UART_DM_TXFS_TX_FIFO_STATE_LSB_SHFT                                                              0x0

#define HWIO_MSS_UIM1_UART_DM_RXFS_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000050)
#define HWIO_MSS_UIM1_UART_DM_RXFS_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RXFS_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RXFS_ADDR, HWIO_MSS_UIM1_UART_DM_RXFS_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RXFS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RXFS_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RXFS_RX_FIFO_STATE_MSB_BMSK                                                       0xffffc000
#define HWIO_MSS_UIM1_UART_DM_RXFS_RX_FIFO_STATE_MSB_SHFT                                                              0xe
#define HWIO_MSS_UIM1_UART_DM_RXFS_RX_ASYNC_FIFO_STATE_BMSK                                                         0x3c00
#define HWIO_MSS_UIM1_UART_DM_RXFS_RX_ASYNC_FIFO_STATE_SHFT                                                            0xa
#define HWIO_MSS_UIM1_UART_DM_RXFS_RX_BUFFER_STATE_BMSK                                                              0x380
#define HWIO_MSS_UIM1_UART_DM_RXFS_RX_BUFFER_STATE_SHFT                                                                0x7
#define HWIO_MSS_UIM1_UART_DM_RXFS_RX_FIFO_STATE_LSB_BMSK                                                             0x7f
#define HWIO_MSS_UIM1_UART_DM_RXFS_RX_FIFO_STATE_LSB_SHFT                                                              0x0

#define HWIO_MSS_UIM1_UART_DM_MISR_MODE_ADDR                                                                    (MSS_UIM1_UART_DM_REG_BASE      + 0x00000060)
#define HWIO_MSS_UIM1_UART_DM_MISR_MODE_RMSK                                                                           0x3
#define HWIO_MSS_UIM1_UART_DM_MISR_MODE_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_MISR_MODE_ADDR, HWIO_MSS_UIM1_UART_DM_MISR_MODE_RMSK)
#define HWIO_MSS_UIM1_UART_DM_MISR_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_MISR_MODE_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_MISR_MODE_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_MISR_MODE_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_MISR_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_MISR_MODE_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_MISR_MODE_IN)
#define HWIO_MSS_UIM1_UART_DM_MISR_MODE_MODE_BMSK                                                                      0x3
#define HWIO_MSS_UIM1_UART_DM_MISR_MODE_MODE_SHFT                                                                      0x0

#define HWIO_MSS_UIM1_UART_DM_MISR_RESET_ADDR                                                                   (MSS_UIM1_UART_DM_REG_BASE      + 0x00000064)
#define HWIO_MSS_UIM1_UART_DM_MISR_RESET_RMSK                                                                          0x1
#define HWIO_MSS_UIM1_UART_DM_MISR_RESET_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_MISR_RESET_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_MISR_RESET_RESET_BMSK                                                                    0x1
#define HWIO_MSS_UIM1_UART_DM_MISR_RESET_RESET_SHFT                                                                    0x0

#define HWIO_MSS_UIM1_UART_DM_MISR_EXPORT_ADDR                                                                  (MSS_UIM1_UART_DM_REG_BASE      + 0x00000068)
#define HWIO_MSS_UIM1_UART_DM_MISR_EXPORT_RMSK                                                                         0x1
#define HWIO_MSS_UIM1_UART_DM_MISR_EXPORT_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_MISR_EXPORT_ADDR, HWIO_MSS_UIM1_UART_DM_MISR_EXPORT_RMSK)
#define HWIO_MSS_UIM1_UART_DM_MISR_EXPORT_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_MISR_EXPORT_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_MISR_EXPORT_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_MISR_EXPORT_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_MISR_EXPORT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_MISR_EXPORT_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_MISR_EXPORT_IN)
#define HWIO_MSS_UIM1_UART_DM_MISR_EXPORT_EXPORT_BMSK                                                                  0x1
#define HWIO_MSS_UIM1_UART_DM_MISR_EXPORT_EXPORT_SHFT                                                                  0x0

#define HWIO_MSS_UIM1_UART_DM_MISR_VAL_ADDR                                                                     (MSS_UIM1_UART_DM_REG_BASE      + 0x0000006c)
#define HWIO_MSS_UIM1_UART_DM_MISR_VAL_RMSK                                                                          0x3ff
#define HWIO_MSS_UIM1_UART_DM_MISR_VAL_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_MISR_VAL_ADDR, HWIO_MSS_UIM1_UART_DM_MISR_VAL_RMSK)
#define HWIO_MSS_UIM1_UART_DM_MISR_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_MISR_VAL_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_MISR_VAL_VAL_BMSK                                                                      0x3ff
#define HWIO_MSS_UIM1_UART_DM_MISR_VAL_VAL_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_TF_RF_DEPRECATED_ADDR                                                             (MSS_UIM1_UART_DM_REG_BASE      + 0x00000070)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_DEPRECATED_RMSK                                                             0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_RF_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TF_RF_DEPRECATED_ADDR, HWIO_MSS_UIM1_UART_DM_TF_RF_DEPRECATED_RMSK)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TF_RF_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_RF_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_TF_RF_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_TF_RF_DEPRECATED_IN)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_DEPRECATED_UART_DM_TF_RF_DEPRECATED_BMSK                                    0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_RF_DEPRECATED_UART_DM_TF_RF_DEPRECATED_SHFT                                           0x0

#define HWIO_MSS_UIM1_UART_DM_TF_RF_2_DEPRECATED_ADDR                                                           (MSS_UIM1_UART_DM_REG_BASE      + 0x00000074)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_2_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_RF_2_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TF_RF_2_DEPRECATED_ADDR, HWIO_MSS_UIM1_UART_DM_TF_RF_2_DEPRECATED_RMSK)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_2_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TF_RF_2_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_2_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_RF_2_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_2_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_TF_RF_2_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_TF_RF_2_DEPRECATED_IN)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_2_DEPRECATED_UART_DM_TF_RF_2_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_RF_2_DEPRECATED_UART_DM_TF_RF_2_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM1_UART_DM_TF_RF_3_DEPRECATED_ADDR                                                           (MSS_UIM1_UART_DM_REG_BASE      + 0x00000078)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_3_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_RF_3_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TF_RF_3_DEPRECATED_ADDR, HWIO_MSS_UIM1_UART_DM_TF_RF_3_DEPRECATED_RMSK)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_3_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TF_RF_3_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_3_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_RF_3_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_3_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_TF_RF_3_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_TF_RF_3_DEPRECATED_IN)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_3_DEPRECATED_UART_DM_TF_RF_3_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_RF_3_DEPRECATED_UART_DM_TF_RF_3_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM1_UART_DM_TF_RF_4_DEPRECATED_ADDR                                                           (MSS_UIM1_UART_DM_REG_BASE      + 0x0000007c)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_4_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_RF_4_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TF_RF_4_DEPRECATED_ADDR, HWIO_MSS_UIM1_UART_DM_TF_RF_4_DEPRECATED_RMSK)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_4_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TF_RF_4_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_4_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_RF_4_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_4_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_TF_RF_4_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_TF_RF_4_DEPRECATED_IN)
#define HWIO_MSS_UIM1_UART_DM_TF_RF_4_DEPRECATED_UART_DM_TF_RF_4_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_RF_4_DEPRECATED_UART_DM_TF_RF_4_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_ADDR                                                                      (MSS_UIM1_UART_DM_REG_BASE      + 0x00000080)
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_RMSK                                                                         0x3ffef
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_SIM_CFG_ADDR, HWIO_MSS_UIM1_UART_DM_SIM_CFG_RMSK)
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_SIM_CFG_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_SIM_CFG_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_SIM_CFG_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_SIM_CFG_IN)
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_UIM_TX_MODE_BMSK                                                             0x20000
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_UIM_TX_MODE_SHFT                                                                0x11
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_UIM_RX_MODE_BMSK                                                             0x10000
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_UIM_RX_MODE_SHFT                                                                0x10
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_BMSK                                                         0xff00
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_SHFT                                                            0x8
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_SIM_CLK_ON_BMSK                                                                 0x80
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_SIM_CLK_ON_SHFT                                                                  0x7
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_SIM_CLK_TD8_SEL_BMSK                                                            0x40
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_SIM_CLK_TD8_SEL_SHFT                                                             0x6
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_SIM_CLK_STOP_HIGH_BMSK                                                          0x20
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_SIM_CLK_STOP_HIGH_SHFT                                                           0x5
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_MASK_RX_BMSK                                                                     0x8
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_MASK_RX_SHFT                                                                     0x3
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_SWAP_D_BMSK                                                                      0x4
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_SWAP_D_SHFT                                                                      0x2
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_INV_D_BMSK                                                                       0x2
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_INV_D_SHFT                                                                       0x1
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_SIM_SEL_BMSK                                                                     0x1
#define HWIO_MSS_UIM1_UART_DM_SIM_CFG_SIM_SEL_SHFT                                                                     0x0

#define HWIO_MSS_UIM1_UART_DM_TEST_WR_ADDR_ADDR                                                                 (MSS_UIM1_UART_DM_REG_BASE      + 0x00000084)
#define HWIO_MSS_UIM1_UART_DM_TEST_WR_ADDR_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TEST_WR_ADDR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TEST_WR_ADDR_ADDR, HWIO_MSS_UIM1_UART_DM_TEST_WR_ADDR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_TEST_WR_ADDR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TEST_WR_ADDR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_TEST_WR_ADDR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TEST_WR_ADDR_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TEST_WR_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_TEST_WR_ADDR_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_TEST_WR_ADDR_IN)
#define HWIO_MSS_UIM1_UART_DM_TEST_WR_ADDR_TEST_WR_ADDR_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TEST_WR_ADDR_TEST_WR_ADDR_SHFT                                                           0x0

#define HWIO_MSS_UIM1_UART_DM_TEST_WR_DATA_ADDR                                                                 (MSS_UIM1_UART_DM_REG_BASE      + 0x00000088)
#define HWIO_MSS_UIM1_UART_DM_TEST_WR_DATA_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TEST_WR_DATA_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TEST_WR_DATA_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TEST_WR_DATA_TEST_WR_DATA_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TEST_WR_DATA_TEST_WR_DATA_SHFT                                                           0x0

#define HWIO_MSS_UIM1_UART_DM_TEST_RD_ADDR_ADDR                                                                 (MSS_UIM1_UART_DM_REG_BASE      + 0x0000008c)
#define HWIO_MSS_UIM1_UART_DM_TEST_RD_ADDR_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TEST_RD_ADDR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TEST_RD_ADDR_ADDR, HWIO_MSS_UIM1_UART_DM_TEST_RD_ADDR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_TEST_RD_ADDR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TEST_RD_ADDR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_TEST_RD_ADDR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TEST_RD_ADDR_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TEST_RD_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_TEST_RD_ADDR_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_TEST_RD_ADDR_IN)
#define HWIO_MSS_UIM1_UART_DM_TEST_RD_ADDR_TEST_RD_ADDR_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TEST_RD_ADDR_TEST_RD_ADDR_SHFT                                                           0x0

#define HWIO_MSS_UIM1_UART_DM_TEST_RD_DATA_ADDR                                                                 (MSS_UIM1_UART_DM_REG_BASE      + 0x00000090)
#define HWIO_MSS_UIM1_UART_DM_TEST_RD_DATA_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TEST_RD_DATA_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TEST_RD_DATA_ADDR, HWIO_MSS_UIM1_UART_DM_TEST_RD_DATA_RMSK)
#define HWIO_MSS_UIM1_UART_DM_TEST_RD_DATA_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_TEST_RD_DATA_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_TEST_RD_DATA_TEST_RD_DATA_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TEST_RD_DATA_TEST_RD_DATA_SHFT                                                           0x0

#define HWIO_MSS_UIM1_UART_DM_CSR_ADDR                                                                          (MSS_UIM1_UART_DM_REG_BASE      + 0x000000a0)
#define HWIO_MSS_UIM1_UART_DM_CSR_RMSK                                                                                0xff
#define HWIO_MSS_UIM1_UART_DM_CSR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_CSR_ADDR, HWIO_MSS_UIM1_UART_DM_CSR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_CSR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_CSR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_CSR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_CSR_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_CSR_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_CSR_IN)
#define HWIO_MSS_UIM1_UART_DM_CSR_UART_RX_CLK_SEL_BMSK                                                                0xf0
#define HWIO_MSS_UIM1_UART_DM_CSR_UART_RX_CLK_SEL_SHFT                                                                 0x4
#define HWIO_MSS_UIM1_UART_DM_CSR_UART_TX_CLK_SEL_BMSK                                                                 0xf
#define HWIO_MSS_UIM1_UART_DM_CSR_UART_TX_CLK_SEL_SHFT                                                                 0x0

#define HWIO_MSS_UIM1_UART_DM_SR_ADDR                                                                           (MSS_UIM1_UART_DM_REG_BASE      + 0x000000a4)
#define HWIO_MSS_UIM1_UART_DM_SR_RMSK                                                                                0xfff
#define HWIO_MSS_UIM1_UART_DM_SR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_SR_ADDR, HWIO_MSS_UIM1_UART_DM_SR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_SR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_SR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_SR_TRANS_END_TRIGGER_BMSK                                                              0xc00
#define HWIO_MSS_UIM1_UART_DM_SR_TRANS_END_TRIGGER_SHFT                                                                0xa
#define HWIO_MSS_UIM1_UART_DM_SR_TRANS_ACTIVE_BMSK                                                                   0x200
#define HWIO_MSS_UIM1_UART_DM_SR_TRANS_ACTIVE_SHFT                                                                     0x9
#define HWIO_MSS_UIM1_UART_DM_SR_RX_BREAK_START_LAST_BMSK                                                            0x100
#define HWIO_MSS_UIM1_UART_DM_SR_RX_BREAK_START_LAST_SHFT                                                              0x8
#define HWIO_MSS_UIM1_UART_DM_SR_HUNT_CHAR_BMSK                                                                       0x80
#define HWIO_MSS_UIM1_UART_DM_SR_HUNT_CHAR_SHFT                                                                        0x7
#define HWIO_MSS_UIM1_UART_DM_SR_RX_BREAK_BMSK                                                                        0x40
#define HWIO_MSS_UIM1_UART_DM_SR_RX_BREAK_SHFT                                                                         0x6
#define HWIO_MSS_UIM1_UART_DM_SR_PAR_FRAME_ERR_BMSK                                                                   0x20
#define HWIO_MSS_UIM1_UART_DM_SR_PAR_FRAME_ERR_SHFT                                                                    0x5
#define HWIO_MSS_UIM1_UART_DM_SR_UART_OVERRUN_BMSK                                                                    0x10
#define HWIO_MSS_UIM1_UART_DM_SR_UART_OVERRUN_SHFT                                                                     0x4
#define HWIO_MSS_UIM1_UART_DM_SR_TXEMT_BMSK                                                                            0x8
#define HWIO_MSS_UIM1_UART_DM_SR_TXEMT_SHFT                                                                            0x3
#define HWIO_MSS_UIM1_UART_DM_SR_TXRDY_BMSK                                                                            0x4
#define HWIO_MSS_UIM1_UART_DM_SR_TXRDY_SHFT                                                                            0x2
#define HWIO_MSS_UIM1_UART_DM_SR_RXFULL_BMSK                                                                           0x2
#define HWIO_MSS_UIM1_UART_DM_SR_RXFULL_SHFT                                                                           0x1
#define HWIO_MSS_UIM1_UART_DM_SR_RXRDY_BMSK                                                                            0x1
#define HWIO_MSS_UIM1_UART_DM_SR_RXRDY_SHFT                                                                            0x0

#define HWIO_MSS_UIM1_UART_DM_CR_ADDR                                                                           (MSS_UIM1_UART_DM_REG_BASE      + 0x000000a8)
#define HWIO_MSS_UIM1_UART_DM_CR_RMSK                                                                                0xfff
#define HWIO_MSS_UIM1_UART_DM_CR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_CR_ADDR, HWIO_MSS_UIM1_UART_DM_CR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_CR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_CR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_CR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_CR_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_CR_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_CR_IN)
#define HWIO_MSS_UIM1_UART_DM_CR_CHANNEL_COMMAND_MSB_BMSK                                                            0x800
#define HWIO_MSS_UIM1_UART_DM_CR_CHANNEL_COMMAND_MSB_SHFT                                                              0xb
#define HWIO_MSS_UIM1_UART_DM_CR_GENERAL_COMMAND_BMSK                                                                0x700
#define HWIO_MSS_UIM1_UART_DM_CR_GENERAL_COMMAND_SHFT                                                                  0x8
#define HWIO_MSS_UIM1_UART_DM_CR_CHANNEL_COMMAND_LSB_BMSK                                                             0xf0
#define HWIO_MSS_UIM1_UART_DM_CR_CHANNEL_COMMAND_LSB_SHFT                                                              0x4
#define HWIO_MSS_UIM1_UART_DM_CR_UART_TX_DISABLE_BMSK                                                                  0x8
#define HWIO_MSS_UIM1_UART_DM_CR_UART_TX_DISABLE_SHFT                                                                  0x3
#define HWIO_MSS_UIM1_UART_DM_CR_UART_TX_EN_BMSK                                                                       0x4
#define HWIO_MSS_UIM1_UART_DM_CR_UART_TX_EN_SHFT                                                                       0x2
#define HWIO_MSS_UIM1_UART_DM_CR_UART_RX_DISABLE_BMSK                                                                  0x2
#define HWIO_MSS_UIM1_UART_DM_CR_UART_RX_DISABLE_SHFT                                                                  0x1
#define HWIO_MSS_UIM1_UART_DM_CR_UART_RX_EN_BMSK                                                                       0x1
#define HWIO_MSS_UIM1_UART_DM_CR_UART_RX_EN_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_MISR_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x000000ac)
#define HWIO_MSS_UIM1_UART_DM_MISR_RMSK                                                                            0x1ffff
#define HWIO_MSS_UIM1_UART_DM_MISR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_MISR_ADDR, HWIO_MSS_UIM1_UART_DM_MISR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_MISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_MISR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_MISR_UART_MISR_BMSK                                                                  0x1ffff
#define HWIO_MSS_UIM1_UART_DM_MISR_UART_MISR_SHFT                                                                      0x0

#define HWIO_MSS_UIM1_UART_DM_IMR_ADDR                                                                          (MSS_UIM1_UART_DM_REG_BASE      + 0x000000b0)
#define HWIO_MSS_UIM1_UART_DM_IMR_RMSK                                                                             0x1ffff
#define HWIO_MSS_UIM1_UART_DM_IMR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_IMR_ADDR, HWIO_MSS_UIM1_UART_DM_IMR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_IMR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_IMR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_IMR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_IMR_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_IMR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_IMR_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_IMR_IN)
#define HWIO_MSS_UIM1_UART_DM_IMR_WWT_IRQ_BMSK                                                                     0x10000
#define HWIO_MSS_UIM1_UART_DM_IMR_WWT_IRQ_SHFT                                                                        0x10
#define HWIO_MSS_UIM1_UART_DM_IMR_TXCOMP_IRQ_BMSK                                                                   0x8000
#define HWIO_MSS_UIM1_UART_DM_IMR_TXCOMP_IRQ_SHFT                                                                      0xf
#define HWIO_MSS_UIM1_UART_DM_IMR_RX_RD_ERROR_IRQ_BMSK                                                              0x4000
#define HWIO_MSS_UIM1_UART_DM_IMR_RX_RD_ERROR_IRQ_SHFT                                                                 0xe
#define HWIO_MSS_UIM1_UART_DM_IMR_TX_WR_ERROR_IRQ_BMSK                                                              0x2000
#define HWIO_MSS_UIM1_UART_DM_IMR_TX_WR_ERROR_IRQ_SHFT                                                                 0xd
#define HWIO_MSS_UIM1_UART_DM_IMR_PAR_FRAME_ERR_IRQ_BMSK                                                            0x1000
#define HWIO_MSS_UIM1_UART_DM_IMR_PAR_FRAME_ERR_IRQ_SHFT                                                               0xc
#define HWIO_MSS_UIM1_UART_DM_IMR_RXBREAK_END_BMSK                                                                   0x800
#define HWIO_MSS_UIM1_UART_DM_IMR_RXBREAK_END_SHFT                                                                     0xb
#define HWIO_MSS_UIM1_UART_DM_IMR_RXBREAK_START_BMSK                                                                 0x400
#define HWIO_MSS_UIM1_UART_DM_IMR_RXBREAK_START_SHFT                                                                   0xa
#define HWIO_MSS_UIM1_UART_DM_IMR_TX_DONE_BMSK                                                                       0x200
#define HWIO_MSS_UIM1_UART_DM_IMR_TX_DONE_SHFT                                                                         0x9
#define HWIO_MSS_UIM1_UART_DM_IMR_TX_ERROR_BMSK                                                                      0x100
#define HWIO_MSS_UIM1_UART_DM_IMR_TX_ERROR_SHFT                                                                        0x8
#define HWIO_MSS_UIM1_UART_DM_IMR_TX_READY_BMSK                                                                       0x80
#define HWIO_MSS_UIM1_UART_DM_IMR_TX_READY_SHFT                                                                        0x7
#define HWIO_MSS_UIM1_UART_DM_IMR_CURRENT_CTS_BMSK                                                                    0x40
#define HWIO_MSS_UIM1_UART_DM_IMR_CURRENT_CTS_SHFT                                                                     0x6
#define HWIO_MSS_UIM1_UART_DM_IMR_DELTA_CTS_BMSK                                                                      0x20
#define HWIO_MSS_UIM1_UART_DM_IMR_DELTA_CTS_SHFT                                                                       0x5
#define HWIO_MSS_UIM1_UART_DM_IMR_RXLEV_BMSK                                                                          0x10
#define HWIO_MSS_UIM1_UART_DM_IMR_RXLEV_SHFT                                                                           0x4
#define HWIO_MSS_UIM1_UART_DM_IMR_RXSTALE_BMSK                                                                         0x8
#define HWIO_MSS_UIM1_UART_DM_IMR_RXSTALE_SHFT                                                                         0x3
#define HWIO_MSS_UIM1_UART_DM_IMR_RXBREAK_CHANGE_BMSK                                                                  0x4
#define HWIO_MSS_UIM1_UART_DM_IMR_RXBREAK_CHANGE_SHFT                                                                  0x2
#define HWIO_MSS_UIM1_UART_DM_IMR_RXHUNT_BMSK                                                                          0x2
#define HWIO_MSS_UIM1_UART_DM_IMR_RXHUNT_SHFT                                                                          0x1
#define HWIO_MSS_UIM1_UART_DM_IMR_TXLEV_BMSK                                                                           0x1
#define HWIO_MSS_UIM1_UART_DM_IMR_TXLEV_SHFT                                                                           0x0

#define HWIO_MSS_UIM1_UART_DM_ISR_ADDR                                                                          (MSS_UIM1_UART_DM_REG_BASE      + 0x000000b4)
#define HWIO_MSS_UIM1_UART_DM_ISR_RMSK                                                                             0x1ffff
#define HWIO_MSS_UIM1_UART_DM_ISR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_ISR_ADDR, HWIO_MSS_UIM1_UART_DM_ISR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_ISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_ISR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_ISR_WWT_IRQ_BMSK                                                                     0x10000
#define HWIO_MSS_UIM1_UART_DM_ISR_WWT_IRQ_SHFT                                                                        0x10
#define HWIO_MSS_UIM1_UART_DM_ISR_TXCOMP_IRQ_BMSK                                                                   0x8000
#define HWIO_MSS_UIM1_UART_DM_ISR_TXCOMP_IRQ_SHFT                                                                      0xf
#define HWIO_MSS_UIM1_UART_DM_ISR_RX_RD_ERROR_IRQ_BMSK                                                              0x4000
#define HWIO_MSS_UIM1_UART_DM_ISR_RX_RD_ERROR_IRQ_SHFT                                                                 0xe
#define HWIO_MSS_UIM1_UART_DM_ISR_TX_WR_ERROR_IRQ_BMSK                                                              0x2000
#define HWIO_MSS_UIM1_UART_DM_ISR_TX_WR_ERROR_IRQ_SHFT                                                                 0xd
#define HWIO_MSS_UIM1_UART_DM_ISR_PAR_FRAME_ERR_IRQ_BMSK                                                            0x1000
#define HWIO_MSS_UIM1_UART_DM_ISR_PAR_FRAME_ERR_IRQ_SHFT                                                               0xc
#define HWIO_MSS_UIM1_UART_DM_ISR_RXBREAK_END_BMSK                                                                   0x800
#define HWIO_MSS_UIM1_UART_DM_ISR_RXBREAK_END_SHFT                                                                     0xb
#define HWIO_MSS_UIM1_UART_DM_ISR_RXBREAK_START_BMSK                                                                 0x400
#define HWIO_MSS_UIM1_UART_DM_ISR_RXBREAK_START_SHFT                                                                   0xa
#define HWIO_MSS_UIM1_UART_DM_ISR_TX_DONE_BMSK                                                                       0x200
#define HWIO_MSS_UIM1_UART_DM_ISR_TX_DONE_SHFT                                                                         0x9
#define HWIO_MSS_UIM1_UART_DM_ISR_TX_ERROR_BMSK                                                                      0x100
#define HWIO_MSS_UIM1_UART_DM_ISR_TX_ERROR_SHFT                                                                        0x8
#define HWIO_MSS_UIM1_UART_DM_ISR_TX_READY_BMSK                                                                       0x80
#define HWIO_MSS_UIM1_UART_DM_ISR_TX_READY_SHFT                                                                        0x7
#define HWIO_MSS_UIM1_UART_DM_ISR_CURRENT_CTS_BMSK                                                                    0x40
#define HWIO_MSS_UIM1_UART_DM_ISR_CURRENT_CTS_SHFT                                                                     0x6
#define HWIO_MSS_UIM1_UART_DM_ISR_DELTA_CTS_BMSK                                                                      0x20
#define HWIO_MSS_UIM1_UART_DM_ISR_DELTA_CTS_SHFT                                                                       0x5
#define HWIO_MSS_UIM1_UART_DM_ISR_RXLEV_BMSK                                                                          0x10
#define HWIO_MSS_UIM1_UART_DM_ISR_RXLEV_SHFT                                                                           0x4
#define HWIO_MSS_UIM1_UART_DM_ISR_RXSTALE_BMSK                                                                         0x8
#define HWIO_MSS_UIM1_UART_DM_ISR_RXSTALE_SHFT                                                                         0x3
#define HWIO_MSS_UIM1_UART_DM_ISR_RXBREAK_CHANGE_BMSK                                                                  0x4
#define HWIO_MSS_UIM1_UART_DM_ISR_RXBREAK_CHANGE_SHFT                                                                  0x2
#define HWIO_MSS_UIM1_UART_DM_ISR_RXHUNT_BMSK                                                                          0x2
#define HWIO_MSS_UIM1_UART_DM_ISR_RXHUNT_SHFT                                                                          0x1
#define HWIO_MSS_UIM1_UART_DM_ISR_TXLEV_BMSK                                                                           0x1
#define HWIO_MSS_UIM1_UART_DM_ISR_TXLEV_SHFT                                                                           0x0

#define HWIO_MSS_UIM1_UART_DM_IRDA_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x000000b8)
#define HWIO_MSS_UIM1_UART_DM_IRDA_RMSK                                                                               0x1f
#define HWIO_MSS_UIM1_UART_DM_IRDA_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_IRDA_ADDR, HWIO_MSS_UIM1_UART_DM_IRDA_RMSK)
#define HWIO_MSS_UIM1_UART_DM_IRDA_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_IRDA_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_IRDA_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_IRDA_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_IRDA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_IRDA_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_IRDA_IN)
#define HWIO_MSS_UIM1_UART_DM_IRDA_MEDIUM_RATE_EN_BMSK                                                                0x10
#define HWIO_MSS_UIM1_UART_DM_IRDA_MEDIUM_RATE_EN_SHFT                                                                 0x4
#define HWIO_MSS_UIM1_UART_DM_IRDA_IRDA_LOOPBACK_BMSK                                                                  0x8
#define HWIO_MSS_UIM1_UART_DM_IRDA_IRDA_LOOPBACK_SHFT                                                                  0x3
#define HWIO_MSS_UIM1_UART_DM_IRDA_INVERT_IRDA_TX_BMSK                                                                 0x4
#define HWIO_MSS_UIM1_UART_DM_IRDA_INVERT_IRDA_TX_SHFT                                                                 0x2
#define HWIO_MSS_UIM1_UART_DM_IRDA_INVERT_IRDA_RX_BMSK                                                                 0x2
#define HWIO_MSS_UIM1_UART_DM_IRDA_INVERT_IRDA_RX_SHFT                                                                 0x1
#define HWIO_MSS_UIM1_UART_DM_IRDA_IRDA_EN_BMSK                                                                        0x1
#define HWIO_MSS_UIM1_UART_DM_IRDA_IRDA_EN_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_RX_TOTAL_SNAP_ADDR                                                                (MSS_UIM1_UART_DM_REG_BASE      + 0x000000bc)
#define HWIO_MSS_UIM1_UART_DM_RX_TOTAL_SNAP_RMSK                                                                  0xffffff
#define HWIO_MSS_UIM1_UART_DM_RX_TOTAL_SNAP_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RX_TOTAL_SNAP_ADDR, HWIO_MSS_UIM1_UART_DM_RX_TOTAL_SNAP_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RX_TOTAL_SNAP_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RX_TOTAL_SNAP_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RX_TOTAL_SNAP_RX_TOTAL_BYTES_BMSK                                                   0xffffff
#define HWIO_MSS_UIM1_UART_DM_RX_TOTAL_SNAP_RX_TOTAL_BYTES_SHFT                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_WWT_TIMEOUT_ADDR                                                                  (MSS_UIM1_UART_DM_REG_BASE      + 0x000000c0)
#define HWIO_MSS_UIM1_UART_DM_WWT_TIMEOUT_RMSK                                                                   0x1ffffff
#define HWIO_MSS_UIM1_UART_DM_WWT_TIMEOUT_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_WWT_TIMEOUT_ADDR, HWIO_MSS_UIM1_UART_DM_WWT_TIMEOUT_RMSK)
#define HWIO_MSS_UIM1_UART_DM_WWT_TIMEOUT_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_WWT_TIMEOUT_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_WWT_TIMEOUT_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_WWT_TIMEOUT_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_WWT_TIMEOUT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_WWT_TIMEOUT_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_WWT_TIMEOUT_IN)
#define HWIO_MSS_UIM1_UART_DM_WWT_TIMEOUT_WWT_TIMEOUT_BMSK                                                       0x1ffffff
#define HWIO_MSS_UIM1_UART_DM_WWT_TIMEOUT_WWT_TIMEOUT_SHFT                                                             0x0

#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_ADDR                                                                     (MSS_UIM1_UART_DM_REG_BASE      + 0x000000c4)
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_RMSK                                                                       0xf06731
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_CLK_CTRL_ADDR, HWIO_MSS_UIM1_UART_DM_CLK_CTRL_RMSK)
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_CLK_CTRL_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_CLK_CTRL_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_CLK_CTRL_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_CLK_CTRL_IN)
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_UART_IRDA_CLK_CGC_OPEN_BMSK                                                0x800000
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_UART_IRDA_CLK_CGC_OPEN_SHFT                                                    0x17
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_UART_SIM_CLK_CGC_OPEN_BMSK                                                 0x400000
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_UART_SIM_CLK_CGC_OPEN_SHFT                                                     0x16
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_UART_RX_CLK_CGC_OPEN_BMSK                                                  0x200000
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_UART_RX_CLK_CGC_OPEN_SHFT                                                      0x15
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_UART_TX_CLK_CGC_OPEN_BMSK                                                  0x100000
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_UART_TX_CLK_CGC_OPEN_SHFT                                                      0x14
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_AHB_RX_BAM_CLK_CGC_OPEN_BMSK                                                 0x4000
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_AHB_RX_BAM_CLK_CGC_OPEN_SHFT                                                    0xe
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_AHB_TX_BAM_CLK_CGC_OPEN_BMSK                                                 0x2000
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_AHB_TX_BAM_CLK_CGC_OPEN_SHFT                                                    0xd
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_AHB_RX_CLK_CGC_OPEN_BMSK                                                      0x400
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_AHB_RX_CLK_CGC_OPEN_SHFT                                                        0xa
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_AHB_TX_CLK_CGC_OPEN_BMSK                                                      0x200
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_AHB_TX_CLK_CGC_OPEN_SHFT                                                        0x9
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_AHB_WR_CLK_CGC_OPEN_BMSK                                                      0x100
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_AHB_WR_CLK_CGC_OPEN_SHFT                                                        0x8
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_RX_ENABLE_CGC_OPT_BMSK                                                         0x20
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_RX_ENABLE_CGC_OPT_SHFT                                                          0x5
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_TX_ENABLE_CGC_OPT_BMSK                                                         0x10
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_TX_ENABLE_CGC_OPT_SHFT                                                          0x4
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_AHB_CLK_CGC_CLOSE_BMSK                                                          0x1
#define HWIO_MSS_UIM1_UART_DM_CLK_CTRL_AHB_CLK_CGC_CLOSE_SHFT                                                          0x0

#define HWIO_MSS_UIM1_UART_DM_BCR_ADDR                                                                          (MSS_UIM1_UART_DM_REG_BASE      + 0x000000c8)
#define HWIO_MSS_UIM1_UART_DM_BCR_RMSK                                                                                0x37
#define HWIO_MSS_UIM1_UART_DM_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_BCR_ADDR, HWIO_MSS_UIM1_UART_DM_BCR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_BCR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_BCR_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_BCR_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_BCR_IN)
#define HWIO_MSS_UIM1_UART_DM_BCR_RX_DMRX_1BYTE_RES_EN_BMSK                                                           0x20
#define HWIO_MSS_UIM1_UART_DM_BCR_RX_DMRX_1BYTE_RES_EN_SHFT                                                            0x5
#define HWIO_MSS_UIM1_UART_DM_BCR_RX_STALE_IRQ_DMRX_EQUAL_BMSK                                                        0x10
#define HWIO_MSS_UIM1_UART_DM_BCR_RX_STALE_IRQ_DMRX_EQUAL_SHFT                                                         0x4
#define HWIO_MSS_UIM1_UART_DM_BCR_RX_DMRX_LOW_EN_BMSK                                                                  0x4
#define HWIO_MSS_UIM1_UART_DM_BCR_RX_DMRX_LOW_EN_SHFT                                                                  0x2
#define HWIO_MSS_UIM1_UART_DM_BCR_STALE_IRQ_EMPTY_BMSK                                                                 0x2
#define HWIO_MSS_UIM1_UART_DM_BCR_STALE_IRQ_EMPTY_SHFT                                                                 0x1
#define HWIO_MSS_UIM1_UART_DM_BCR_TX_BREAK_DISABLE_BMSK                                                                0x1
#define HWIO_MSS_UIM1_UART_DM_BCR_TX_BREAK_DISABLE_SHFT                                                                0x0

#define HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_ADDR                                                                (MSS_UIM1_UART_DM_REG_BASE      + 0x000000cc)
#define HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_RMSK                                                                       0x7
#define HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_ADDR, HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_IN)
#define HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_RX_DMRX_CYCLIC_EN_BMSK                                                     0x4
#define HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_RX_DMRX_CYCLIC_EN_SHFT                                                     0x2
#define HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_RX_TRANS_AUTO_RE_ACTIVATE_BMSK                                             0x2
#define HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_RX_TRANS_AUTO_RE_ACTIVATE_SHFT                                             0x1
#define HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_RX_STALE_AUTO_RE_EN_BMSK                                                   0x1
#define HWIO_MSS_UIM1_UART_DM_RX_TRANS_CTRL_RX_STALE_AUTO_RE_EN_SHFT                                                   0x0

#define HWIO_MSS_UIM1_UART_DM_DMRX_DBG_ADDR                                                                     (MSS_UIM1_UART_DM_REG_BASE      + 0x000000d0)
#define HWIO_MSS_UIM1_UART_DM_DMRX_DBG_RMSK                                                                      0x1ffffff
#define HWIO_MSS_UIM1_UART_DM_DMRX_DBG_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_DMRX_DBG_ADDR, HWIO_MSS_UIM1_UART_DM_DMRX_DBG_RMSK)
#define HWIO_MSS_UIM1_UART_DM_DMRX_DBG_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_DMRX_DBG_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_DMRX_DBG_UART_DM_DMRX_VAL_BMSK                                                     0x1ffffff
#define HWIO_MSS_UIM1_UART_DM_DMRX_DBG_UART_DM_DMRX_VAL_SHFT                                                           0x0

#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_ADDR                                                                   (MSS_UIM1_UART_DM_REG_BASE      + 0x000000d4)
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_RMSK                                                                   0x37377771
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_FSM_STATUS_ADDR, HWIO_MSS_UIM1_UART_DM_FSM_STATUS_RMSK)
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_FSM_STATUS_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_TX_COMP_FSM_BMSK                                                       0x30000000
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_TX_COMP_FSM_SHFT                                                             0x1c
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_RX_PACK_FSM_BMSK                                                        0x7000000
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_RX_PACK_FSM_SHFT                                                             0x18
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_RX_TRANS_FSM_BMSK                                                        0x300000
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_RX_TRANS_FSM_SHFT                                                            0x14
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_TX_TRANS_FSM_BMSK                                                         0x70000
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_TX_TRANS_FSM_SHFT                                                            0x10
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_RX_PRO_TRANS_END_FSM_BMSK                                                  0x7000
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_RX_PRO_TRANS_END_FSM_SHFT                                                     0xc
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_RX_PRO_ACTIVE_FSM_BMSK                                                      0x700
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_RX_PRO_ACTIVE_FSM_SHFT                                                        0x8
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_TX_CON_TRANS_END_FSM_BMSK                                                    0x70
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_TX_CON_TRANS_END_FSM_SHFT                                                     0x4
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_RX_TRANSFER_ACTIVE_BMSK                                                       0x1
#define HWIO_MSS_UIM1_UART_DM_FSM_STATUS_RX_TRANSFER_ACTIVE_SHFT                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_HW_VERSION_ADDR                                                                   (MSS_UIM1_UART_DM_REG_BASE      + 0x000000d8)
#define HWIO_MSS_UIM1_UART_DM_HW_VERSION_RMSK                                                                   0xffffffff
#define HWIO_MSS_UIM1_UART_DM_HW_VERSION_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_HW_VERSION_ADDR, HWIO_MSS_UIM1_UART_DM_HW_VERSION_RMSK)
#define HWIO_MSS_UIM1_UART_DM_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_HW_VERSION_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_HW_VERSION_HW_VERSION_MAJOR_BMSK                                                  0xf0000000
#define HWIO_MSS_UIM1_UART_DM_HW_VERSION_HW_VERSION_MAJOR_SHFT                                                        0x1c
#define HWIO_MSS_UIM1_UART_DM_HW_VERSION_HW_VERSION_MINOR_BMSK                                                   0xfff0000
#define HWIO_MSS_UIM1_UART_DM_HW_VERSION_HW_VERSION_MINOR_SHFT                                                        0x10
#define HWIO_MSS_UIM1_UART_DM_HW_VERSION_HW_VERSION_STEP_BMSK                                                       0xffff
#define HWIO_MSS_UIM1_UART_DM_HW_VERSION_HW_VERSION_STEP_SHFT                                                          0x0

#define HWIO_MSS_UIM1_UART_DM_GENERICS_ADDR                                                                     (MSS_UIM1_UART_DM_REG_BASE      + 0x000000dc)
#define HWIO_MSS_UIM1_UART_DM_GENERICS_RMSK                                                                           0xff
#define HWIO_MSS_UIM1_UART_DM_GENERICS_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_GENERICS_ADDR, HWIO_MSS_UIM1_UART_DM_GENERICS_RMSK)
#define HWIO_MSS_UIM1_UART_DM_GENERICS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_GENERICS_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_GENERICS_GENERIC_BAM_IFC_BMSK                                                           0x80
#define HWIO_MSS_UIM1_UART_DM_GENERICS_GENERIC_BAM_IFC_SHFT                                                            0x7
#define HWIO_MSS_UIM1_UART_DM_GENERICS_GENERIC_DM_IFC_BMSK                                                            0x40
#define HWIO_MSS_UIM1_UART_DM_GENERICS_GENERIC_DM_IFC_SHFT                                                             0x6
#define HWIO_MSS_UIM1_UART_DM_GENERICS_GENERIC_IRDA_IFC_BMSK                                                          0x20
#define HWIO_MSS_UIM1_UART_DM_GENERICS_GENERIC_IRDA_IFC_SHFT                                                           0x5
#define HWIO_MSS_UIM1_UART_DM_GENERICS_GENERIC_SIM_GLUE_BMSK                                                          0x10
#define HWIO_MSS_UIM1_UART_DM_GENERICS_GENERIC_SIM_GLUE_SHFT                                                           0x4
#define HWIO_MSS_UIM1_UART_DM_GENERICS_GENERIC_RAM_ADDR_WIDTH_BMSK                                                     0xf
#define HWIO_MSS_UIM1_UART_DM_GENERICS_GENERIC_RAM_ADDR_WIDTH_SHFT                                                     0x0

#define HWIO_MSS_UIM1_UART_DM_TF_ADDR                                                                           (MSS_UIM1_UART_DM_REG_BASE      + 0x00000100)
#define HWIO_MSS_UIM1_UART_DM_TF_RMSK                                                                           0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_UART_TF_BMSK                                                                   0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_UART_TF_SHFT                                                                          0x0

#define HWIO_MSS_UIM1_UART_DM_TF_2_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000104)
#define HWIO_MSS_UIM1_UART_DM_TF_2_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_2_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_2_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_2_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_2_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_TF_3_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000108)
#define HWIO_MSS_UIM1_UART_DM_TF_3_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_3_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_3_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_3_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_3_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_TF_4_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x0000010c)
#define HWIO_MSS_UIM1_UART_DM_TF_4_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_4_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_4_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_4_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_4_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_TF_5_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000110)
#define HWIO_MSS_UIM1_UART_DM_TF_5_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_5_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_5_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_5_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_5_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_TF_6_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000114)
#define HWIO_MSS_UIM1_UART_DM_TF_6_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_6_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_6_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_6_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_6_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_TF_7_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000118)
#define HWIO_MSS_UIM1_UART_DM_TF_7_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_7_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_7_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_7_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_7_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_TF_8_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x0000011c)
#define HWIO_MSS_UIM1_UART_DM_TF_8_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_8_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_8_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_8_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_8_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_TF_9_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000120)
#define HWIO_MSS_UIM1_UART_DM_TF_9_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_9_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_9_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_9_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_9_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_TF_10_ADDR                                                                        (MSS_UIM1_UART_DM_REG_BASE      + 0x00000124)
#define HWIO_MSS_UIM1_UART_DM_TF_10_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_10_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_10_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_10_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_10_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_TF_11_ADDR                                                                        (MSS_UIM1_UART_DM_REG_BASE      + 0x00000128)
#define HWIO_MSS_UIM1_UART_DM_TF_11_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_11_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_11_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_11_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_11_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_TF_12_ADDR                                                                        (MSS_UIM1_UART_DM_REG_BASE      + 0x0000012c)
#define HWIO_MSS_UIM1_UART_DM_TF_12_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_12_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_12_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_12_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_12_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_TF_13_ADDR                                                                        (MSS_UIM1_UART_DM_REG_BASE      + 0x00000130)
#define HWIO_MSS_UIM1_UART_DM_TF_13_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_13_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_13_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_13_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_13_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_TF_14_ADDR                                                                        (MSS_UIM1_UART_DM_REG_BASE      + 0x00000134)
#define HWIO_MSS_UIM1_UART_DM_TF_14_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_14_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_14_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_14_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_14_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_TF_15_ADDR                                                                        (MSS_UIM1_UART_DM_REG_BASE      + 0x00000138)
#define HWIO_MSS_UIM1_UART_DM_TF_15_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_15_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_15_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_15_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_15_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_TF_16_ADDR                                                                        (MSS_UIM1_UART_DM_REG_BASE      + 0x0000013c)
#define HWIO_MSS_UIM1_UART_DM_TF_16_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_16_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_TF_16_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_TF_16_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_TF_16_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_RF_ADDR                                                                           (MSS_UIM1_UART_DM_REG_BASE      + 0x00000140)
#define HWIO_MSS_UIM1_UART_DM_RF_RMSK                                                                           0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_ADDR, HWIO_MSS_UIM1_UART_DM_RF_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_UART_RF_BMSK                                                                   0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_UART_RF_SHFT                                                                          0x0

#define HWIO_MSS_UIM1_UART_DM_RF_2_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000144)
#define HWIO_MSS_UIM1_UART_DM_RF_2_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_2_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_2_ADDR, HWIO_MSS_UIM1_UART_DM_RF_2_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_2_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_2_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_2_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_2_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_RF_3_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000148)
#define HWIO_MSS_UIM1_UART_DM_RF_3_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_3_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_3_ADDR, HWIO_MSS_UIM1_UART_DM_RF_3_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_3_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_3_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_3_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_3_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_RF_4_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x0000014c)
#define HWIO_MSS_UIM1_UART_DM_RF_4_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_4_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_4_ADDR, HWIO_MSS_UIM1_UART_DM_RF_4_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_4_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_4_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_4_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_4_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_RF_5_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000150)
#define HWIO_MSS_UIM1_UART_DM_RF_5_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_5_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_5_ADDR, HWIO_MSS_UIM1_UART_DM_RF_5_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_5_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_5_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_5_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_5_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_RF_6_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000154)
#define HWIO_MSS_UIM1_UART_DM_RF_6_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_6_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_6_ADDR, HWIO_MSS_UIM1_UART_DM_RF_6_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_6_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_6_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_6_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_6_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_RF_7_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000158)
#define HWIO_MSS_UIM1_UART_DM_RF_7_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_7_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_7_ADDR, HWIO_MSS_UIM1_UART_DM_RF_7_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_7_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_7_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_7_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_7_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_RF_8_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x0000015c)
#define HWIO_MSS_UIM1_UART_DM_RF_8_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_8_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_8_ADDR, HWIO_MSS_UIM1_UART_DM_RF_8_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_8_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_8_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_8_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_8_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_RF_9_ADDR                                                                         (MSS_UIM1_UART_DM_REG_BASE      + 0x00000160)
#define HWIO_MSS_UIM1_UART_DM_RF_9_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_9_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_9_ADDR, HWIO_MSS_UIM1_UART_DM_RF_9_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_9_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_9_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_9_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_9_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_RF_10_ADDR                                                                        (MSS_UIM1_UART_DM_REG_BASE      + 0x00000164)
#define HWIO_MSS_UIM1_UART_DM_RF_10_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_10_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_10_ADDR, HWIO_MSS_UIM1_UART_DM_RF_10_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_10_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_10_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_10_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_10_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_RF_11_ADDR                                                                        (MSS_UIM1_UART_DM_REG_BASE      + 0x00000168)
#define HWIO_MSS_UIM1_UART_DM_RF_11_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_11_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_11_ADDR, HWIO_MSS_UIM1_UART_DM_RF_11_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_11_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_11_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_11_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_11_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_RF_12_ADDR                                                                        (MSS_UIM1_UART_DM_REG_BASE      + 0x0000016c)
#define HWIO_MSS_UIM1_UART_DM_RF_12_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_12_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_12_ADDR, HWIO_MSS_UIM1_UART_DM_RF_12_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_12_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_12_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_12_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_12_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_RF_13_ADDR                                                                        (MSS_UIM1_UART_DM_REG_BASE      + 0x00000170)
#define HWIO_MSS_UIM1_UART_DM_RF_13_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_13_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_13_ADDR, HWIO_MSS_UIM1_UART_DM_RF_13_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_13_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_13_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_13_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_13_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_RF_14_ADDR                                                                        (MSS_UIM1_UART_DM_REG_BASE      + 0x00000174)
#define HWIO_MSS_UIM1_UART_DM_RF_14_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_14_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_14_ADDR, HWIO_MSS_UIM1_UART_DM_RF_14_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_14_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_14_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_14_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_14_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_RF_15_ADDR                                                                        (MSS_UIM1_UART_DM_REG_BASE      + 0x00000178)
#define HWIO_MSS_UIM1_UART_DM_RF_15_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_15_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_15_ADDR, HWIO_MSS_UIM1_UART_DM_RF_15_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_15_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_15_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_15_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_15_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_RF_16_ADDR                                                                        (MSS_UIM1_UART_DM_REG_BASE      + 0x0000017c)
#define HWIO_MSS_UIM1_UART_DM_RF_16_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_16_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_16_ADDR, HWIO_MSS_UIM1_UART_DM_RF_16_RMSK)
#define HWIO_MSS_UIM1_UART_DM_RF_16_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_RF_16_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_RF_16_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM1_UART_DM_RF_16_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_ADDR                                                                      (MSS_UIM1_UART_DM_REG_BASE      + 0x00000180)
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_RMSK                                                                          0xf77f
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_UIM_CFG_ADDR, HWIO_MSS_UIM1_UART_DM_UIM_CFG_RMSK)
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_UIM_CFG_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_UIM_CFG_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_UIM_CFG_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_UIM_CFG_IN)
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_BATT_ALARM_QUICK_DROP_EN_BMSK                                                 0x8000
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_BATT_ALARM_QUICK_DROP_EN_SHFT                                                    0xf
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_TESTBUS_EN_BMSK                                                               0x4000
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_TESTBUS_EN_SHFT                                                                  0xe
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_SW_RESET_BMSK                                                                 0x2000
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_SW_RESET_SHFT                                                                    0xd
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_MODE18_BMSK                                                                   0x1000
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_MODE18_SHFT                                                                      0xc
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_PMIC_ALARM_EN_BMSK                                                             0x400
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_PMIC_ALARM_EN_SHFT                                                               0xa
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_BATT_ALARM_TRIGGER_EN_BMSK                                                     0x200
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_BATT_ALARM_TRIGGER_EN_SHFT                                                       0x9
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_UIM_RMV_TRIGGER_EN_BMSK                                                        0x100
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_UIM_RMV_TRIGGER_EN_SHFT                                                          0x8
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_UIM_CARD_EVENTS_ENABLE_BMSK                                                     0x40
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_UIM_CARD_EVENTS_ENABLE_SHFT                                                      0x6
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_UIM_PRESENT_POLARITY_BMSK                                                       0x20
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_UIM_PRESENT_POLARITY_SHFT                                                        0x5
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_EVENT_DEBOUNCE_TIME_BMSK                                                        0x1f
#define HWIO_MSS_UIM1_UART_DM_UIM_CFG_EVENT_DEBOUNCE_TIME_SHFT                                                         0x0

#define HWIO_MSS_UIM1_UART_DM_UIM_CMD_ADDR                                                                      (MSS_UIM1_UART_DM_REG_BASE      + 0x00000184)
#define HWIO_MSS_UIM1_UART_DM_UIM_CMD_RMSK                                                                             0x3
#define HWIO_MSS_UIM1_UART_DM_UIM_CMD_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_UIM_CMD_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_UIM_CMD_RECOVER_FROM_HW_DEACTIVATION_BMSK                                                0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_CMD_RECOVER_FROM_HW_DEACTIVATION_SHFT                                                0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_CMD_INITIATE_HW_DEACTIVATION_BMSK                                                    0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_CMD_INITIATE_HW_DEACTIVATION_SHFT                                                    0x0

#define HWIO_MSS_UIM1_UART_DM_UIM_IO_STATUS_ADDR                                                                (MSS_UIM1_UART_DM_REG_BASE      + 0x00000188)
#define HWIO_MSS_UIM1_UART_DM_UIM_IO_STATUS_RMSK                                                                       0x7
#define HWIO_MSS_UIM1_UART_DM_UIM_IO_STATUS_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_UIM_IO_STATUS_ADDR, HWIO_MSS_UIM1_UART_DM_UIM_IO_STATUS_RMSK)
#define HWIO_MSS_UIM1_UART_DM_UIM_IO_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_UIM_IO_STATUS_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_UIM_IO_STATUS_UIM_IO_WRITE_IN_PROGRESS_BMSK                                              0x4
#define HWIO_MSS_UIM1_UART_DM_UIM_IO_STATUS_UIM_IO_WRITE_IN_PROGRESS_SHFT                                              0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_IO_STATUS_UIM_DEACTIVATION_STATUS_BMSK                                               0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_IO_STATUS_UIM_DEACTIVATION_STATUS_SHFT                                               0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_IO_STATUS_CARD_PRESENCE_BMSK                                                         0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_IO_STATUS_CARD_PRESENCE_SHFT                                                         0x0

#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_ADDR                                                                  (MSS_UIM1_UART_DM_REG_BASE      + 0x0000018c)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_RMSK                                                                        0x1f
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_ADDR, HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_UIM_IO_WRITE_DONE_BMSK                                                      0x10
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_UIM_IO_WRITE_DONE_SHFT                                                       0x4
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_HW_SEQUENCE_FINISH_BMSK                                                      0x8
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_HW_SEQUENCE_FINISH_SHFT                                                      0x3
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_BATT_ALARM_BMSK                                                              0x4
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_BATT_ALARM_SHFT                                                              0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_UIM_CARD_INSERTION_BMSK                                                      0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_UIM_CARD_INSERTION_SHFT                                                      0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_UIM_CARD_REMOVAL_BMSK                                                        0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_ISR_UIM_CARD_REMOVAL_SHFT                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_ADDR                                                                 (MSS_UIM1_UART_DM_REG_BASE      + 0x00000190)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_RMSK                                                                       0x1f
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_ADDR, HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_UIM_IO_WRITE_DONE_BMSK                                                     0x10
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_UIM_IO_WRITE_DONE_SHFT                                                      0x4
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_HW_SEQUENCE_FINISH_BMSK                                                     0x8
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_HW_SEQUENCE_FINISH_SHFT                                                     0x3
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_BATT_ALARM_BMSK                                                             0x4
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_BATT_ALARM_SHFT                                                             0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_UIM_CARD_INSERTION_BMSK                                                     0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_UIM_CARD_INSERTION_SHFT                                                     0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_UIM_CARD_REMOVAL_BMSK                                                       0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_MISR_UIM_CARD_REMOVAL_SHFT                                                       0x0

#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_CLR_ADDR                                                                  (MSS_UIM1_UART_DM_REG_BASE      + 0x00000194)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_CLR_RMSK                                                                        0x1f
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_CLR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_UIM_IRQ_CLR_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_CLR_UIM_IO_WRITE_DONE_BMSK                                                      0x10
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_CLR_UIM_IO_WRITE_DONE_SHFT                                                       0x4
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_CLR_HW_SEQUENCE_FINISH_BMSK                                                      0x8
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_CLR_HW_SEQUENCE_FINISH_SHFT                                                      0x3
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_CLR_BATT_ALARM_BMSK                                                              0x4
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_CLR_BATT_ALARM_SHFT                                                              0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_CLR_UIM_CARD_INSERTION_BMSK                                                      0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_CLR_UIM_CARD_INSERTION_SHFT                                                      0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_CLR_UIM_CARD_REMOVAL_BMSK                                                        0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_CLR_UIM_CARD_REMOVAL_SHFT                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_ADDR                                                                  (MSS_UIM1_UART_DM_REG_BASE      + 0x00000198)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_RMSK                                                                        0x1f
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_ADDR, HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_RMSK)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_ADDR, m)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_ADDR,m,v,HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_IN)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_UIM_IO_WRITE_DONE_BMSK                                                      0x10
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_UIM_IO_WRITE_DONE_SHFT                                                       0x4
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_HW_SEQUENCE_FINISH_BMSK                                                      0x8
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_HW_SEQUENCE_FINISH_SHFT                                                      0x3
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_BATT_ALARM_BMSK                                                              0x4
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_BATT_ALARM_SHFT                                                              0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_UIM_CARD_INSERTION_BMSK                                                      0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_UIM_CARD_INSERTION_SHFT                                                      0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_UIM_CARD_REMOVAL_BMSK                                                        0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_UIM_CARD_REMOVAL_SHFT                                                        0x0

#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET_ADDR                                                              (MSS_UIM1_UART_DM_REG_BASE      + 0x0000019c)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET_RMSK                                                                    0x1f
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET_UIM_IO_WRITE_DONE_BMSK                                                  0x10
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET_UIM_IO_WRITE_DONE_SHFT                                                   0x4
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET_HW_SEQUENCE_FINISH_BMSK                                                  0x8
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET_HW_SEQUENCE_FINISH_SHFT                                                  0x3
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET_BATT_ALARM_BMSK                                                          0x4
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET_BATT_ALARM_SHFT                                                          0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_INSERTION_BMSK                                                  0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_INSERTION_SHFT                                                  0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_REMOVAL_BMSK                                                    0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_REMOVAL_SHFT                                                    0x0

#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_CLR_ADDR                                                              (MSS_UIM1_UART_DM_REG_BASE      + 0x000001a0)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_CLR_RMSK                                                                    0x1f
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_CLR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_CLR_ADDR,v)
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_CLR_UIM_IO_WRITE_DONE_BMSK                                                  0x10
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_CLR_UIM_IO_WRITE_DONE_SHFT                                                   0x4
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_CLR_HW_SEQUENCE_FINISH_BMSK                                                  0x8
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_CLR_HW_SEQUENCE_FINISH_SHFT                                                  0x3
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_CLR_BATT_ALARM_BMSK                                                          0x4
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_CLR_BATT_ALARM_SHFT                                                          0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_INSERTION_BMSK                                                  0x2
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_INSERTION_SHFT                                                  0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_REMOVAL_BMSK                                                    0x1
#define HWIO_MSS_UIM1_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_REMOVAL_SHFT                                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: MSS_UIM2_UART_DM
 *--------------------------------------------------------------------------*/

#define MSS_UIM2_UART_DM_REG_BASE                                                                               (MODEM_TOP_BASE      + 0x00198000)

#define HWIO_MSS_UIM2_UART_DM_MR1_ADDR                                                                          (MSS_UIM2_UART_DM_REG_BASE      + 0x00000000)
#define HWIO_MSS_UIM2_UART_DM_MR1_RMSK                                                                          0xffffffff
#define HWIO_MSS_UIM2_UART_DM_MR1_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_MR1_ADDR, HWIO_MSS_UIM2_UART_DM_MR1_RMSK)
#define HWIO_MSS_UIM2_UART_DM_MR1_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_MR1_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_MR1_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_MR1_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_MR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_MR1_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_MR1_IN)
#define HWIO_MSS_UIM2_UART_DM_MR1_AUTO_RFR_LEVEL1_BMSK                                                          0xffffff00
#define HWIO_MSS_UIM2_UART_DM_MR1_AUTO_RFR_LEVEL1_SHFT                                                                 0x8
#define HWIO_MSS_UIM2_UART_DM_MR1_RX_RDY_CTL_BMSK                                                                     0x80
#define HWIO_MSS_UIM2_UART_DM_MR1_RX_RDY_CTL_SHFT                                                                      0x7
#define HWIO_MSS_UIM2_UART_DM_MR1_CTS_CTL_BMSK                                                                        0x40
#define HWIO_MSS_UIM2_UART_DM_MR1_CTS_CTL_SHFT                                                                         0x6
#define HWIO_MSS_UIM2_UART_DM_MR1_AUTO_RFR_LEVEL0_BMSK                                                                0x3f
#define HWIO_MSS_UIM2_UART_DM_MR1_AUTO_RFR_LEVEL0_SHFT                                                                 0x0

#define HWIO_MSS_UIM2_UART_DM_MR2_ADDR                                                                          (MSS_UIM2_UART_DM_REG_BASE      + 0x00000004)
#define HWIO_MSS_UIM2_UART_DM_MR2_RMSK                                                                               0x7ff
#define HWIO_MSS_UIM2_UART_DM_MR2_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_MR2_ADDR, HWIO_MSS_UIM2_UART_DM_MR2_RMSK)
#define HWIO_MSS_UIM2_UART_DM_MR2_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_MR2_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_MR2_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_MR2_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_MR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_MR2_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_MR2_IN)
#define HWIO_MSS_UIM2_UART_DM_MR2_RFR_CTS_LOOPBACK_BMSK                                                              0x400
#define HWIO_MSS_UIM2_UART_DM_MR2_RFR_CTS_LOOPBACK_SHFT                                                                0xa
#define HWIO_MSS_UIM2_UART_DM_MR2_RX_ERROR_CHAR_OFF_BMSK                                                             0x200
#define HWIO_MSS_UIM2_UART_DM_MR2_RX_ERROR_CHAR_OFF_SHFT                                                               0x9
#define HWIO_MSS_UIM2_UART_DM_MR2_RX_BREAK_ZERO_CHAR_OFF_BMSK                                                        0x100
#define HWIO_MSS_UIM2_UART_DM_MR2_RX_BREAK_ZERO_CHAR_OFF_SHFT                                                          0x8
#define HWIO_MSS_UIM2_UART_DM_MR2_LOOPBACK_BMSK                                                                       0x80
#define HWIO_MSS_UIM2_UART_DM_MR2_LOOPBACK_SHFT                                                                        0x7
#define HWIO_MSS_UIM2_UART_DM_MR2_ERROR_MODE_BMSK                                                                     0x40
#define HWIO_MSS_UIM2_UART_DM_MR2_ERROR_MODE_SHFT                                                                      0x6
#define HWIO_MSS_UIM2_UART_DM_MR2_BITS_PER_CHAR_BMSK                                                                  0x30
#define HWIO_MSS_UIM2_UART_DM_MR2_BITS_PER_CHAR_SHFT                                                                   0x4
#define HWIO_MSS_UIM2_UART_DM_MR2_STOP_BIT_LEN_BMSK                                                                    0xc
#define HWIO_MSS_UIM2_UART_DM_MR2_STOP_BIT_LEN_SHFT                                                                    0x2
#define HWIO_MSS_UIM2_UART_DM_MR2_PARITY_MODE_BMSK                                                                     0x3
#define HWIO_MSS_UIM2_UART_DM_MR2_PARITY_MODE_SHFT                                                                     0x0

#define HWIO_MSS_UIM2_UART_DM_CSR_SR_DEPRECATED_ADDR                                                            (MSS_UIM2_UART_DM_REG_BASE      + 0x00000008)
#define HWIO_MSS_UIM2_UART_DM_CSR_SR_DEPRECATED_RMSK                                                            0xffffffff
#define HWIO_MSS_UIM2_UART_DM_CSR_SR_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_CSR_SR_DEPRECATED_ADDR, HWIO_MSS_UIM2_UART_DM_CSR_SR_DEPRECATED_RMSK)
#define HWIO_MSS_UIM2_UART_DM_CSR_SR_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_CSR_SR_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_CSR_SR_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_CSR_SR_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_CSR_SR_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_CSR_SR_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_CSR_SR_DEPRECATED_IN)
#define HWIO_MSS_UIM2_UART_DM_CSR_SR_DEPRECATED_UART_DM_CSR_SR_DEPRECATED_BMSK                                  0xffffffff
#define HWIO_MSS_UIM2_UART_DM_CSR_SR_DEPRECATED_UART_DM_CSR_SR_DEPRECATED_SHFT                                         0x0

#define HWIO_MSS_UIM2_UART_DM_CR_MISR_DEPRECATED_ADDR                                                           (MSS_UIM2_UART_DM_REG_BASE      + 0x00000010)
#define HWIO_MSS_UIM2_UART_DM_CR_MISR_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM2_UART_DM_CR_MISR_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_CR_MISR_DEPRECATED_ADDR, HWIO_MSS_UIM2_UART_DM_CR_MISR_DEPRECATED_RMSK)
#define HWIO_MSS_UIM2_UART_DM_CR_MISR_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_CR_MISR_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_CR_MISR_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_CR_MISR_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_CR_MISR_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_CR_MISR_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_CR_MISR_DEPRECATED_IN)
#define HWIO_MSS_UIM2_UART_DM_CR_MISR_DEPRECATED_UART_DM_CR_MISR_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_CR_MISR_DEPRECATED_UART_DM_CR_MISR_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM2_UART_DM_IMR_ISR_DEPRECATED_ADDR                                                           (MSS_UIM2_UART_DM_REG_BASE      + 0x00000014)
#define HWIO_MSS_UIM2_UART_DM_IMR_ISR_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM2_UART_DM_IMR_ISR_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_IMR_ISR_DEPRECATED_ADDR, HWIO_MSS_UIM2_UART_DM_IMR_ISR_DEPRECATED_RMSK)
#define HWIO_MSS_UIM2_UART_DM_IMR_ISR_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_IMR_ISR_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_IMR_ISR_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_IMR_ISR_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_IMR_ISR_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_IMR_ISR_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_IMR_ISR_DEPRECATED_IN)
#define HWIO_MSS_UIM2_UART_DM_IMR_ISR_DEPRECATED_UART_DM_IMR_ISR_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_IMR_ISR_DEPRECATED_UART_DM_IMR_ISR_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM2_UART_DM_IPR_ADDR                                                                          (MSS_UIM2_UART_DM_REG_BASE      + 0x00000018)
#define HWIO_MSS_UIM2_UART_DM_IPR_RMSK                                                                          0xffffffdf
#define HWIO_MSS_UIM2_UART_DM_IPR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_IPR_ADDR, HWIO_MSS_UIM2_UART_DM_IPR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_IPR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_IPR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_IPR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_IPR_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_IPR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_IPR_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_IPR_IN)
#define HWIO_MSS_UIM2_UART_DM_IPR_STALE_TIMEOUT_MSB_BMSK                                                        0xffffff80
#define HWIO_MSS_UIM2_UART_DM_IPR_STALE_TIMEOUT_MSB_SHFT                                                               0x7
#define HWIO_MSS_UIM2_UART_DM_IPR_SAMPLE_DATA_BMSK                                                                    0x40
#define HWIO_MSS_UIM2_UART_DM_IPR_SAMPLE_DATA_SHFT                                                                     0x6
#define HWIO_MSS_UIM2_UART_DM_IPR_STALE_TIMEOUT_LSB_BMSK                                                              0x1f
#define HWIO_MSS_UIM2_UART_DM_IPR_STALE_TIMEOUT_LSB_SHFT                                                               0x0

#define HWIO_MSS_UIM2_UART_DM_TFWR_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x0000001c)
#define HWIO_MSS_UIM2_UART_DM_TFWR_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TFWR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TFWR_ADDR, HWIO_MSS_UIM2_UART_DM_TFWR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_TFWR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TFWR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_TFWR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TFWR_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TFWR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_TFWR_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_TFWR_IN)
#define HWIO_MSS_UIM2_UART_DM_TFWR_TFW_BMSK                                                                     0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TFWR_TFW_SHFT                                                                            0x0

#define HWIO_MSS_UIM2_UART_DM_RFWR_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000020)
#define HWIO_MSS_UIM2_UART_DM_RFWR_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RFWR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RFWR_ADDR, HWIO_MSS_UIM2_UART_DM_RFWR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RFWR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RFWR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RFWR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_RFWR_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_RFWR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_RFWR_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_RFWR_IN)
#define HWIO_MSS_UIM2_UART_DM_RFWR_RFW_BMSK                                                                     0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RFWR_RFW_SHFT                                                                            0x0

#define HWIO_MSS_UIM2_UART_DM_HCR_ADDR                                                                          (MSS_UIM2_UART_DM_REG_BASE      + 0x00000024)
#define HWIO_MSS_UIM2_UART_DM_HCR_RMSK                                                                                0xff
#define HWIO_MSS_UIM2_UART_DM_HCR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_HCR_ADDR, HWIO_MSS_UIM2_UART_DM_HCR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_HCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_HCR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_HCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_HCR_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_HCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_HCR_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_HCR_IN)
#define HWIO_MSS_UIM2_UART_DM_HCR_DATA_BMSK                                                                           0xff
#define HWIO_MSS_UIM2_UART_DM_HCR_DATA_SHFT                                                                            0x0

#define HWIO_MSS_UIM2_UART_DM_DMRX_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000034)
#define HWIO_MSS_UIM2_UART_DM_DMRX_RMSK                                                                          0x1ffffff
#define HWIO_MSS_UIM2_UART_DM_DMRX_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_DMRX_ADDR, HWIO_MSS_UIM2_UART_DM_DMRX_RMSK)
#define HWIO_MSS_UIM2_UART_DM_DMRX_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_DMRX_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_DMRX_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_DMRX_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_DMRX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_DMRX_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_DMRX_IN)
#define HWIO_MSS_UIM2_UART_DM_DMRX_RX_DM_CRCI_CHARS_BMSK                                                         0x1ffffff
#define HWIO_MSS_UIM2_UART_DM_DMRX_RX_DM_CRCI_CHARS_SHFT                                                               0x0

#define HWIO_MSS_UIM2_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR                                                (MSS_UIM2_UART_DM_REG_BASE      + 0x00000038)
#define HWIO_MSS_UIM2_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_RMSK                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR, HWIO_MSS_UIM2_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_RMSK)
#define HWIO_MSS_UIM2_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_IN)
#define HWIO_MSS_UIM2_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_BMSK          0xffffffff
#define HWIO_MSS_UIM2_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_SHFT                 0x0

#define HWIO_MSS_UIM2_UART_DM_DMEN_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x0000003c)
#define HWIO_MSS_UIM2_UART_DM_DMEN_RMSK                                                                               0x3c
#define HWIO_MSS_UIM2_UART_DM_DMEN_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_DMEN_ADDR, HWIO_MSS_UIM2_UART_DM_DMEN_RMSK)
#define HWIO_MSS_UIM2_UART_DM_DMEN_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_DMEN_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_DMEN_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_DMEN_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_DMEN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_DMEN_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_DMEN_IN)
#define HWIO_MSS_UIM2_UART_DM_DMEN_RX_SC_ENABLE_BMSK                                                                  0x20
#define HWIO_MSS_UIM2_UART_DM_DMEN_RX_SC_ENABLE_SHFT                                                                   0x5
#define HWIO_MSS_UIM2_UART_DM_DMEN_TX_SC_ENABLE_BMSK                                                                  0x10
#define HWIO_MSS_UIM2_UART_DM_DMEN_TX_SC_ENABLE_SHFT                                                                   0x4
#define HWIO_MSS_UIM2_UART_DM_DMEN_RX_BAM_ENABLE_BMSK                                                                  0x8
#define HWIO_MSS_UIM2_UART_DM_DMEN_RX_BAM_ENABLE_SHFT                                                                  0x3
#define HWIO_MSS_UIM2_UART_DM_DMEN_TX_BAM_ENABLE_BMSK                                                                  0x4
#define HWIO_MSS_UIM2_UART_DM_DMEN_TX_BAM_ENABLE_SHFT                                                                  0x2

#define HWIO_MSS_UIM2_UART_DM_NO_CHARS_FOR_TX_ADDR                                                              (MSS_UIM2_UART_DM_REG_BASE      + 0x00000040)
#define HWIO_MSS_UIM2_UART_DM_NO_CHARS_FOR_TX_RMSK                                                                0xffffff
#define HWIO_MSS_UIM2_UART_DM_NO_CHARS_FOR_TX_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_NO_CHARS_FOR_TX_ADDR, HWIO_MSS_UIM2_UART_DM_NO_CHARS_FOR_TX_RMSK)
#define HWIO_MSS_UIM2_UART_DM_NO_CHARS_FOR_TX_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_NO_CHARS_FOR_TX_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_NO_CHARS_FOR_TX_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_NO_CHARS_FOR_TX_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_NO_CHARS_FOR_TX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_NO_CHARS_FOR_TX_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_NO_CHARS_FOR_TX_IN)
#define HWIO_MSS_UIM2_UART_DM_NO_CHARS_FOR_TX_TX_TOTAL_TRANS_LEN_BMSK                                             0xffffff
#define HWIO_MSS_UIM2_UART_DM_NO_CHARS_FOR_TX_TX_TOTAL_TRANS_LEN_SHFT                                                  0x0

#define HWIO_MSS_UIM2_UART_DM_BADR_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000044)
#define HWIO_MSS_UIM2_UART_DM_BADR_RMSK                                                                         0xfffffffc
#define HWIO_MSS_UIM2_UART_DM_BADR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_BADR_ADDR, HWIO_MSS_UIM2_UART_DM_BADR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_BADR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_BADR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_BADR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_BADR_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_BADR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_BADR_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_BADR_IN)
#define HWIO_MSS_UIM2_UART_DM_BADR_RX_BASE_ADDR_BMSK                                                            0xfffffffc
#define HWIO_MSS_UIM2_UART_DM_BADR_RX_BASE_ADDR_SHFT                                                                   0x2

#define HWIO_MSS_UIM2_UART_DM_TESTSL_ADDR                                                                       (MSS_UIM2_UART_DM_REG_BASE      + 0x00000048)
#define HWIO_MSS_UIM2_UART_DM_TESTSL_RMSK                                                                             0x1f
#define HWIO_MSS_UIM2_UART_DM_TESTSL_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TESTSL_ADDR, HWIO_MSS_UIM2_UART_DM_TESTSL_RMSK)
#define HWIO_MSS_UIM2_UART_DM_TESTSL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TESTSL_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_TESTSL_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TESTSL_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TESTSL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_TESTSL_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_TESTSL_IN)
#define HWIO_MSS_UIM2_UART_DM_TESTSL_TEST_EN_BMSK                                                                     0x10
#define HWIO_MSS_UIM2_UART_DM_TESTSL_TEST_EN_SHFT                                                                      0x4
#define HWIO_MSS_UIM2_UART_DM_TESTSL_TEST_SEL_BMSK                                                                     0xf
#define HWIO_MSS_UIM2_UART_DM_TESTSL_TEST_SEL_SHFT                                                                     0x0

#define HWIO_MSS_UIM2_UART_DM_TXFS_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x0000004c)
#define HWIO_MSS_UIM2_UART_DM_TXFS_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TXFS_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TXFS_ADDR, HWIO_MSS_UIM2_UART_DM_TXFS_RMSK)
#define HWIO_MSS_UIM2_UART_DM_TXFS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TXFS_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_TXFS_TX_FIFO_STATE_MSB_BMSK                                                       0xffffc000
#define HWIO_MSS_UIM2_UART_DM_TXFS_TX_FIFO_STATE_MSB_SHFT                                                              0xe
#define HWIO_MSS_UIM2_UART_DM_TXFS_TX_ASYNC_FIFO_STATE_BMSK                                                         0x3c00
#define HWIO_MSS_UIM2_UART_DM_TXFS_TX_ASYNC_FIFO_STATE_SHFT                                                            0xa
#define HWIO_MSS_UIM2_UART_DM_TXFS_TX_BUFFER_STATE_BMSK                                                              0x380
#define HWIO_MSS_UIM2_UART_DM_TXFS_TX_BUFFER_STATE_SHFT                                                                0x7
#define HWIO_MSS_UIM2_UART_DM_TXFS_TX_FIFO_STATE_LSB_BMSK                                                             0x7f
#define HWIO_MSS_UIM2_UART_DM_TXFS_TX_FIFO_STATE_LSB_SHFT                                                              0x0

#define HWIO_MSS_UIM2_UART_DM_RXFS_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000050)
#define HWIO_MSS_UIM2_UART_DM_RXFS_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RXFS_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RXFS_ADDR, HWIO_MSS_UIM2_UART_DM_RXFS_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RXFS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RXFS_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RXFS_RX_FIFO_STATE_MSB_BMSK                                                       0xffffc000
#define HWIO_MSS_UIM2_UART_DM_RXFS_RX_FIFO_STATE_MSB_SHFT                                                              0xe
#define HWIO_MSS_UIM2_UART_DM_RXFS_RX_ASYNC_FIFO_STATE_BMSK                                                         0x3c00
#define HWIO_MSS_UIM2_UART_DM_RXFS_RX_ASYNC_FIFO_STATE_SHFT                                                            0xa
#define HWIO_MSS_UIM2_UART_DM_RXFS_RX_BUFFER_STATE_BMSK                                                              0x380
#define HWIO_MSS_UIM2_UART_DM_RXFS_RX_BUFFER_STATE_SHFT                                                                0x7
#define HWIO_MSS_UIM2_UART_DM_RXFS_RX_FIFO_STATE_LSB_BMSK                                                             0x7f
#define HWIO_MSS_UIM2_UART_DM_RXFS_RX_FIFO_STATE_LSB_SHFT                                                              0x0

#define HWIO_MSS_UIM2_UART_DM_MISR_MODE_ADDR                                                                    (MSS_UIM2_UART_DM_REG_BASE      + 0x00000060)
#define HWIO_MSS_UIM2_UART_DM_MISR_MODE_RMSK                                                                           0x3
#define HWIO_MSS_UIM2_UART_DM_MISR_MODE_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_MISR_MODE_ADDR, HWIO_MSS_UIM2_UART_DM_MISR_MODE_RMSK)
#define HWIO_MSS_UIM2_UART_DM_MISR_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_MISR_MODE_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_MISR_MODE_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_MISR_MODE_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_MISR_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_MISR_MODE_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_MISR_MODE_IN)
#define HWIO_MSS_UIM2_UART_DM_MISR_MODE_MODE_BMSK                                                                      0x3
#define HWIO_MSS_UIM2_UART_DM_MISR_MODE_MODE_SHFT                                                                      0x0

#define HWIO_MSS_UIM2_UART_DM_MISR_RESET_ADDR                                                                   (MSS_UIM2_UART_DM_REG_BASE      + 0x00000064)
#define HWIO_MSS_UIM2_UART_DM_MISR_RESET_RMSK                                                                          0x1
#define HWIO_MSS_UIM2_UART_DM_MISR_RESET_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_MISR_RESET_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_MISR_RESET_RESET_BMSK                                                                    0x1
#define HWIO_MSS_UIM2_UART_DM_MISR_RESET_RESET_SHFT                                                                    0x0

#define HWIO_MSS_UIM2_UART_DM_MISR_EXPORT_ADDR                                                                  (MSS_UIM2_UART_DM_REG_BASE      + 0x00000068)
#define HWIO_MSS_UIM2_UART_DM_MISR_EXPORT_RMSK                                                                         0x1
#define HWIO_MSS_UIM2_UART_DM_MISR_EXPORT_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_MISR_EXPORT_ADDR, HWIO_MSS_UIM2_UART_DM_MISR_EXPORT_RMSK)
#define HWIO_MSS_UIM2_UART_DM_MISR_EXPORT_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_MISR_EXPORT_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_MISR_EXPORT_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_MISR_EXPORT_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_MISR_EXPORT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_MISR_EXPORT_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_MISR_EXPORT_IN)
#define HWIO_MSS_UIM2_UART_DM_MISR_EXPORT_EXPORT_BMSK                                                                  0x1
#define HWIO_MSS_UIM2_UART_DM_MISR_EXPORT_EXPORT_SHFT                                                                  0x0

#define HWIO_MSS_UIM2_UART_DM_MISR_VAL_ADDR                                                                     (MSS_UIM2_UART_DM_REG_BASE      + 0x0000006c)
#define HWIO_MSS_UIM2_UART_DM_MISR_VAL_RMSK                                                                          0x3ff
#define HWIO_MSS_UIM2_UART_DM_MISR_VAL_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_MISR_VAL_ADDR, HWIO_MSS_UIM2_UART_DM_MISR_VAL_RMSK)
#define HWIO_MSS_UIM2_UART_DM_MISR_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_MISR_VAL_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_MISR_VAL_VAL_BMSK                                                                      0x3ff
#define HWIO_MSS_UIM2_UART_DM_MISR_VAL_VAL_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_TF_RF_DEPRECATED_ADDR                                                             (MSS_UIM2_UART_DM_REG_BASE      + 0x00000070)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_DEPRECATED_RMSK                                                             0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_RF_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TF_RF_DEPRECATED_ADDR, HWIO_MSS_UIM2_UART_DM_TF_RF_DEPRECATED_RMSK)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TF_RF_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_RF_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_TF_RF_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_TF_RF_DEPRECATED_IN)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_DEPRECATED_UART_DM_TF_RF_DEPRECATED_BMSK                                    0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_RF_DEPRECATED_UART_DM_TF_RF_DEPRECATED_SHFT                                           0x0

#define HWIO_MSS_UIM2_UART_DM_TF_RF_2_DEPRECATED_ADDR                                                           (MSS_UIM2_UART_DM_REG_BASE      + 0x00000074)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_2_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_RF_2_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TF_RF_2_DEPRECATED_ADDR, HWIO_MSS_UIM2_UART_DM_TF_RF_2_DEPRECATED_RMSK)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_2_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TF_RF_2_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_2_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_RF_2_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_2_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_TF_RF_2_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_TF_RF_2_DEPRECATED_IN)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_2_DEPRECATED_UART_DM_TF_RF_2_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_RF_2_DEPRECATED_UART_DM_TF_RF_2_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM2_UART_DM_TF_RF_3_DEPRECATED_ADDR                                                           (MSS_UIM2_UART_DM_REG_BASE      + 0x00000078)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_3_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_RF_3_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TF_RF_3_DEPRECATED_ADDR, HWIO_MSS_UIM2_UART_DM_TF_RF_3_DEPRECATED_RMSK)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_3_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TF_RF_3_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_3_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_RF_3_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_3_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_TF_RF_3_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_TF_RF_3_DEPRECATED_IN)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_3_DEPRECATED_UART_DM_TF_RF_3_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_RF_3_DEPRECATED_UART_DM_TF_RF_3_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM2_UART_DM_TF_RF_4_DEPRECATED_ADDR                                                           (MSS_UIM2_UART_DM_REG_BASE      + 0x0000007c)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_4_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_RF_4_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TF_RF_4_DEPRECATED_ADDR, HWIO_MSS_UIM2_UART_DM_TF_RF_4_DEPRECATED_RMSK)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_4_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TF_RF_4_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_4_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_RF_4_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_4_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_TF_RF_4_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_TF_RF_4_DEPRECATED_IN)
#define HWIO_MSS_UIM2_UART_DM_TF_RF_4_DEPRECATED_UART_DM_TF_RF_4_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_RF_4_DEPRECATED_UART_DM_TF_RF_4_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_ADDR                                                                      (MSS_UIM2_UART_DM_REG_BASE      + 0x00000080)
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_RMSK                                                                         0x3ffef
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_SIM_CFG_ADDR, HWIO_MSS_UIM2_UART_DM_SIM_CFG_RMSK)
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_SIM_CFG_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_SIM_CFG_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_SIM_CFG_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_SIM_CFG_IN)
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_UIM_TX_MODE_BMSK                                                             0x20000
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_UIM_TX_MODE_SHFT                                                                0x11
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_UIM_RX_MODE_BMSK                                                             0x10000
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_UIM_RX_MODE_SHFT                                                                0x10
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_BMSK                                                         0xff00
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_SHFT                                                            0x8
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_SIM_CLK_ON_BMSK                                                                 0x80
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_SIM_CLK_ON_SHFT                                                                  0x7
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_SIM_CLK_TD8_SEL_BMSK                                                            0x40
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_SIM_CLK_TD8_SEL_SHFT                                                             0x6
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_SIM_CLK_STOP_HIGH_BMSK                                                          0x20
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_SIM_CLK_STOP_HIGH_SHFT                                                           0x5
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_MASK_RX_BMSK                                                                     0x8
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_MASK_RX_SHFT                                                                     0x3
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_SWAP_D_BMSK                                                                      0x4
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_SWAP_D_SHFT                                                                      0x2
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_INV_D_BMSK                                                                       0x2
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_INV_D_SHFT                                                                       0x1
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_SIM_SEL_BMSK                                                                     0x1
#define HWIO_MSS_UIM2_UART_DM_SIM_CFG_SIM_SEL_SHFT                                                                     0x0

#define HWIO_MSS_UIM2_UART_DM_TEST_WR_ADDR_ADDR                                                                 (MSS_UIM2_UART_DM_REG_BASE      + 0x00000084)
#define HWIO_MSS_UIM2_UART_DM_TEST_WR_ADDR_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TEST_WR_ADDR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TEST_WR_ADDR_ADDR, HWIO_MSS_UIM2_UART_DM_TEST_WR_ADDR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_TEST_WR_ADDR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TEST_WR_ADDR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_TEST_WR_ADDR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TEST_WR_ADDR_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TEST_WR_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_TEST_WR_ADDR_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_TEST_WR_ADDR_IN)
#define HWIO_MSS_UIM2_UART_DM_TEST_WR_ADDR_TEST_WR_ADDR_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TEST_WR_ADDR_TEST_WR_ADDR_SHFT                                                           0x0

#define HWIO_MSS_UIM2_UART_DM_TEST_WR_DATA_ADDR                                                                 (MSS_UIM2_UART_DM_REG_BASE      + 0x00000088)
#define HWIO_MSS_UIM2_UART_DM_TEST_WR_DATA_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TEST_WR_DATA_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TEST_WR_DATA_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TEST_WR_DATA_TEST_WR_DATA_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TEST_WR_DATA_TEST_WR_DATA_SHFT                                                           0x0

#define HWIO_MSS_UIM2_UART_DM_TEST_RD_ADDR_ADDR                                                                 (MSS_UIM2_UART_DM_REG_BASE      + 0x0000008c)
#define HWIO_MSS_UIM2_UART_DM_TEST_RD_ADDR_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TEST_RD_ADDR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TEST_RD_ADDR_ADDR, HWIO_MSS_UIM2_UART_DM_TEST_RD_ADDR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_TEST_RD_ADDR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TEST_RD_ADDR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_TEST_RD_ADDR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TEST_RD_ADDR_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TEST_RD_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_TEST_RD_ADDR_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_TEST_RD_ADDR_IN)
#define HWIO_MSS_UIM2_UART_DM_TEST_RD_ADDR_TEST_RD_ADDR_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TEST_RD_ADDR_TEST_RD_ADDR_SHFT                                                           0x0

#define HWIO_MSS_UIM2_UART_DM_TEST_RD_DATA_ADDR                                                                 (MSS_UIM2_UART_DM_REG_BASE      + 0x00000090)
#define HWIO_MSS_UIM2_UART_DM_TEST_RD_DATA_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TEST_RD_DATA_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TEST_RD_DATA_ADDR, HWIO_MSS_UIM2_UART_DM_TEST_RD_DATA_RMSK)
#define HWIO_MSS_UIM2_UART_DM_TEST_RD_DATA_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_TEST_RD_DATA_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_TEST_RD_DATA_TEST_RD_DATA_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TEST_RD_DATA_TEST_RD_DATA_SHFT                                                           0x0

#define HWIO_MSS_UIM2_UART_DM_CSR_ADDR                                                                          (MSS_UIM2_UART_DM_REG_BASE      + 0x000000a0)
#define HWIO_MSS_UIM2_UART_DM_CSR_RMSK                                                                                0xff
#define HWIO_MSS_UIM2_UART_DM_CSR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_CSR_ADDR, HWIO_MSS_UIM2_UART_DM_CSR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_CSR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_CSR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_CSR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_CSR_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_CSR_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_CSR_IN)
#define HWIO_MSS_UIM2_UART_DM_CSR_UART_RX_CLK_SEL_BMSK                                                                0xf0
#define HWIO_MSS_UIM2_UART_DM_CSR_UART_RX_CLK_SEL_SHFT                                                                 0x4
#define HWIO_MSS_UIM2_UART_DM_CSR_UART_TX_CLK_SEL_BMSK                                                                 0xf
#define HWIO_MSS_UIM2_UART_DM_CSR_UART_TX_CLK_SEL_SHFT                                                                 0x0

#define HWIO_MSS_UIM2_UART_DM_SR_ADDR                                                                           (MSS_UIM2_UART_DM_REG_BASE      + 0x000000a4)
#define HWIO_MSS_UIM2_UART_DM_SR_RMSK                                                                                0xfff
#define HWIO_MSS_UIM2_UART_DM_SR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_SR_ADDR, HWIO_MSS_UIM2_UART_DM_SR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_SR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_SR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_SR_TRANS_END_TRIGGER_BMSK                                                              0xc00
#define HWIO_MSS_UIM2_UART_DM_SR_TRANS_END_TRIGGER_SHFT                                                                0xa
#define HWIO_MSS_UIM2_UART_DM_SR_TRANS_ACTIVE_BMSK                                                                   0x200
#define HWIO_MSS_UIM2_UART_DM_SR_TRANS_ACTIVE_SHFT                                                                     0x9
#define HWIO_MSS_UIM2_UART_DM_SR_RX_BREAK_START_LAST_BMSK                                                            0x100
#define HWIO_MSS_UIM2_UART_DM_SR_RX_BREAK_START_LAST_SHFT                                                              0x8
#define HWIO_MSS_UIM2_UART_DM_SR_HUNT_CHAR_BMSK                                                                       0x80
#define HWIO_MSS_UIM2_UART_DM_SR_HUNT_CHAR_SHFT                                                                        0x7
#define HWIO_MSS_UIM2_UART_DM_SR_RX_BREAK_BMSK                                                                        0x40
#define HWIO_MSS_UIM2_UART_DM_SR_RX_BREAK_SHFT                                                                         0x6
#define HWIO_MSS_UIM2_UART_DM_SR_PAR_FRAME_ERR_BMSK                                                                   0x20
#define HWIO_MSS_UIM2_UART_DM_SR_PAR_FRAME_ERR_SHFT                                                                    0x5
#define HWIO_MSS_UIM2_UART_DM_SR_UART_OVERRUN_BMSK                                                                    0x10
#define HWIO_MSS_UIM2_UART_DM_SR_UART_OVERRUN_SHFT                                                                     0x4
#define HWIO_MSS_UIM2_UART_DM_SR_TXEMT_BMSK                                                                            0x8
#define HWIO_MSS_UIM2_UART_DM_SR_TXEMT_SHFT                                                                            0x3
#define HWIO_MSS_UIM2_UART_DM_SR_TXRDY_BMSK                                                                            0x4
#define HWIO_MSS_UIM2_UART_DM_SR_TXRDY_SHFT                                                                            0x2
#define HWIO_MSS_UIM2_UART_DM_SR_RXFULL_BMSK                                                                           0x2
#define HWIO_MSS_UIM2_UART_DM_SR_RXFULL_SHFT                                                                           0x1
#define HWIO_MSS_UIM2_UART_DM_SR_RXRDY_BMSK                                                                            0x1
#define HWIO_MSS_UIM2_UART_DM_SR_RXRDY_SHFT                                                                            0x0

#define HWIO_MSS_UIM2_UART_DM_CR_ADDR                                                                           (MSS_UIM2_UART_DM_REG_BASE      + 0x000000a8)
#define HWIO_MSS_UIM2_UART_DM_CR_RMSK                                                                                0xfff
#define HWIO_MSS_UIM2_UART_DM_CR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_CR_ADDR, HWIO_MSS_UIM2_UART_DM_CR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_CR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_CR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_CR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_CR_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_CR_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_CR_IN)
#define HWIO_MSS_UIM2_UART_DM_CR_CHANNEL_COMMAND_MSB_BMSK                                                            0x800
#define HWIO_MSS_UIM2_UART_DM_CR_CHANNEL_COMMAND_MSB_SHFT                                                              0xb
#define HWIO_MSS_UIM2_UART_DM_CR_GENERAL_COMMAND_BMSK                                                                0x700
#define HWIO_MSS_UIM2_UART_DM_CR_GENERAL_COMMAND_SHFT                                                                  0x8
#define HWIO_MSS_UIM2_UART_DM_CR_CHANNEL_COMMAND_LSB_BMSK                                                             0xf0
#define HWIO_MSS_UIM2_UART_DM_CR_CHANNEL_COMMAND_LSB_SHFT                                                              0x4
#define HWIO_MSS_UIM2_UART_DM_CR_UART_TX_DISABLE_BMSK                                                                  0x8
#define HWIO_MSS_UIM2_UART_DM_CR_UART_TX_DISABLE_SHFT                                                                  0x3
#define HWIO_MSS_UIM2_UART_DM_CR_UART_TX_EN_BMSK                                                                       0x4
#define HWIO_MSS_UIM2_UART_DM_CR_UART_TX_EN_SHFT                                                                       0x2
#define HWIO_MSS_UIM2_UART_DM_CR_UART_RX_DISABLE_BMSK                                                                  0x2
#define HWIO_MSS_UIM2_UART_DM_CR_UART_RX_DISABLE_SHFT                                                                  0x1
#define HWIO_MSS_UIM2_UART_DM_CR_UART_RX_EN_BMSK                                                                       0x1
#define HWIO_MSS_UIM2_UART_DM_CR_UART_RX_EN_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_MISR_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x000000ac)
#define HWIO_MSS_UIM2_UART_DM_MISR_RMSK                                                                            0x1ffff
#define HWIO_MSS_UIM2_UART_DM_MISR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_MISR_ADDR, HWIO_MSS_UIM2_UART_DM_MISR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_MISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_MISR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_MISR_UART_MISR_BMSK                                                                  0x1ffff
#define HWIO_MSS_UIM2_UART_DM_MISR_UART_MISR_SHFT                                                                      0x0

#define HWIO_MSS_UIM2_UART_DM_IMR_ADDR                                                                          (MSS_UIM2_UART_DM_REG_BASE      + 0x000000b0)
#define HWIO_MSS_UIM2_UART_DM_IMR_RMSK                                                                             0x1ffff
#define HWIO_MSS_UIM2_UART_DM_IMR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_IMR_ADDR, HWIO_MSS_UIM2_UART_DM_IMR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_IMR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_IMR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_IMR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_IMR_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_IMR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_IMR_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_IMR_IN)
#define HWIO_MSS_UIM2_UART_DM_IMR_WWT_IRQ_BMSK                                                                     0x10000
#define HWIO_MSS_UIM2_UART_DM_IMR_WWT_IRQ_SHFT                                                                        0x10
#define HWIO_MSS_UIM2_UART_DM_IMR_TXCOMP_IRQ_BMSK                                                                   0x8000
#define HWIO_MSS_UIM2_UART_DM_IMR_TXCOMP_IRQ_SHFT                                                                      0xf
#define HWIO_MSS_UIM2_UART_DM_IMR_RX_RD_ERROR_IRQ_BMSK                                                              0x4000
#define HWIO_MSS_UIM2_UART_DM_IMR_RX_RD_ERROR_IRQ_SHFT                                                                 0xe
#define HWIO_MSS_UIM2_UART_DM_IMR_TX_WR_ERROR_IRQ_BMSK                                                              0x2000
#define HWIO_MSS_UIM2_UART_DM_IMR_TX_WR_ERROR_IRQ_SHFT                                                                 0xd
#define HWIO_MSS_UIM2_UART_DM_IMR_PAR_FRAME_ERR_IRQ_BMSK                                                            0x1000
#define HWIO_MSS_UIM2_UART_DM_IMR_PAR_FRAME_ERR_IRQ_SHFT                                                               0xc
#define HWIO_MSS_UIM2_UART_DM_IMR_RXBREAK_END_BMSK                                                                   0x800
#define HWIO_MSS_UIM2_UART_DM_IMR_RXBREAK_END_SHFT                                                                     0xb
#define HWIO_MSS_UIM2_UART_DM_IMR_RXBREAK_START_BMSK                                                                 0x400
#define HWIO_MSS_UIM2_UART_DM_IMR_RXBREAK_START_SHFT                                                                   0xa
#define HWIO_MSS_UIM2_UART_DM_IMR_TX_DONE_BMSK                                                                       0x200
#define HWIO_MSS_UIM2_UART_DM_IMR_TX_DONE_SHFT                                                                         0x9
#define HWIO_MSS_UIM2_UART_DM_IMR_TX_ERROR_BMSK                                                                      0x100
#define HWIO_MSS_UIM2_UART_DM_IMR_TX_ERROR_SHFT                                                                        0x8
#define HWIO_MSS_UIM2_UART_DM_IMR_TX_READY_BMSK                                                                       0x80
#define HWIO_MSS_UIM2_UART_DM_IMR_TX_READY_SHFT                                                                        0x7
#define HWIO_MSS_UIM2_UART_DM_IMR_CURRENT_CTS_BMSK                                                                    0x40
#define HWIO_MSS_UIM2_UART_DM_IMR_CURRENT_CTS_SHFT                                                                     0x6
#define HWIO_MSS_UIM2_UART_DM_IMR_DELTA_CTS_BMSK                                                                      0x20
#define HWIO_MSS_UIM2_UART_DM_IMR_DELTA_CTS_SHFT                                                                       0x5
#define HWIO_MSS_UIM2_UART_DM_IMR_RXLEV_BMSK                                                                          0x10
#define HWIO_MSS_UIM2_UART_DM_IMR_RXLEV_SHFT                                                                           0x4
#define HWIO_MSS_UIM2_UART_DM_IMR_RXSTALE_BMSK                                                                         0x8
#define HWIO_MSS_UIM2_UART_DM_IMR_RXSTALE_SHFT                                                                         0x3
#define HWIO_MSS_UIM2_UART_DM_IMR_RXBREAK_CHANGE_BMSK                                                                  0x4
#define HWIO_MSS_UIM2_UART_DM_IMR_RXBREAK_CHANGE_SHFT                                                                  0x2
#define HWIO_MSS_UIM2_UART_DM_IMR_RXHUNT_BMSK                                                                          0x2
#define HWIO_MSS_UIM2_UART_DM_IMR_RXHUNT_SHFT                                                                          0x1
#define HWIO_MSS_UIM2_UART_DM_IMR_TXLEV_BMSK                                                                           0x1
#define HWIO_MSS_UIM2_UART_DM_IMR_TXLEV_SHFT                                                                           0x0

#define HWIO_MSS_UIM2_UART_DM_ISR_ADDR                                                                          (MSS_UIM2_UART_DM_REG_BASE      + 0x000000b4)
#define HWIO_MSS_UIM2_UART_DM_ISR_RMSK                                                                             0x1ffff
#define HWIO_MSS_UIM2_UART_DM_ISR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_ISR_ADDR, HWIO_MSS_UIM2_UART_DM_ISR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_ISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_ISR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_ISR_WWT_IRQ_BMSK                                                                     0x10000
#define HWIO_MSS_UIM2_UART_DM_ISR_WWT_IRQ_SHFT                                                                        0x10
#define HWIO_MSS_UIM2_UART_DM_ISR_TXCOMP_IRQ_BMSK                                                                   0x8000
#define HWIO_MSS_UIM2_UART_DM_ISR_TXCOMP_IRQ_SHFT                                                                      0xf
#define HWIO_MSS_UIM2_UART_DM_ISR_RX_RD_ERROR_IRQ_BMSK                                                              0x4000
#define HWIO_MSS_UIM2_UART_DM_ISR_RX_RD_ERROR_IRQ_SHFT                                                                 0xe
#define HWIO_MSS_UIM2_UART_DM_ISR_TX_WR_ERROR_IRQ_BMSK                                                              0x2000
#define HWIO_MSS_UIM2_UART_DM_ISR_TX_WR_ERROR_IRQ_SHFT                                                                 0xd
#define HWIO_MSS_UIM2_UART_DM_ISR_PAR_FRAME_ERR_IRQ_BMSK                                                            0x1000
#define HWIO_MSS_UIM2_UART_DM_ISR_PAR_FRAME_ERR_IRQ_SHFT                                                               0xc
#define HWIO_MSS_UIM2_UART_DM_ISR_RXBREAK_END_BMSK                                                                   0x800
#define HWIO_MSS_UIM2_UART_DM_ISR_RXBREAK_END_SHFT                                                                     0xb
#define HWIO_MSS_UIM2_UART_DM_ISR_RXBREAK_START_BMSK                                                                 0x400
#define HWIO_MSS_UIM2_UART_DM_ISR_RXBREAK_START_SHFT                                                                   0xa
#define HWIO_MSS_UIM2_UART_DM_ISR_TX_DONE_BMSK                                                                       0x200
#define HWIO_MSS_UIM2_UART_DM_ISR_TX_DONE_SHFT                                                                         0x9
#define HWIO_MSS_UIM2_UART_DM_ISR_TX_ERROR_BMSK                                                                      0x100
#define HWIO_MSS_UIM2_UART_DM_ISR_TX_ERROR_SHFT                                                                        0x8
#define HWIO_MSS_UIM2_UART_DM_ISR_TX_READY_BMSK                                                                       0x80
#define HWIO_MSS_UIM2_UART_DM_ISR_TX_READY_SHFT                                                                        0x7
#define HWIO_MSS_UIM2_UART_DM_ISR_CURRENT_CTS_BMSK                                                                    0x40
#define HWIO_MSS_UIM2_UART_DM_ISR_CURRENT_CTS_SHFT                                                                     0x6
#define HWIO_MSS_UIM2_UART_DM_ISR_DELTA_CTS_BMSK                                                                      0x20
#define HWIO_MSS_UIM2_UART_DM_ISR_DELTA_CTS_SHFT                                                                       0x5
#define HWIO_MSS_UIM2_UART_DM_ISR_RXLEV_BMSK                                                                          0x10
#define HWIO_MSS_UIM2_UART_DM_ISR_RXLEV_SHFT                                                                           0x4
#define HWIO_MSS_UIM2_UART_DM_ISR_RXSTALE_BMSK                                                                         0x8
#define HWIO_MSS_UIM2_UART_DM_ISR_RXSTALE_SHFT                                                                         0x3
#define HWIO_MSS_UIM2_UART_DM_ISR_RXBREAK_CHANGE_BMSK                                                                  0x4
#define HWIO_MSS_UIM2_UART_DM_ISR_RXBREAK_CHANGE_SHFT                                                                  0x2
#define HWIO_MSS_UIM2_UART_DM_ISR_RXHUNT_BMSK                                                                          0x2
#define HWIO_MSS_UIM2_UART_DM_ISR_RXHUNT_SHFT                                                                          0x1
#define HWIO_MSS_UIM2_UART_DM_ISR_TXLEV_BMSK                                                                           0x1
#define HWIO_MSS_UIM2_UART_DM_ISR_TXLEV_SHFT                                                                           0x0

#define HWIO_MSS_UIM2_UART_DM_IRDA_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x000000b8)
#define HWIO_MSS_UIM2_UART_DM_IRDA_RMSK                                                                               0x1f
#define HWIO_MSS_UIM2_UART_DM_IRDA_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_IRDA_ADDR, HWIO_MSS_UIM2_UART_DM_IRDA_RMSK)
#define HWIO_MSS_UIM2_UART_DM_IRDA_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_IRDA_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_IRDA_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_IRDA_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_IRDA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_IRDA_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_IRDA_IN)
#define HWIO_MSS_UIM2_UART_DM_IRDA_MEDIUM_RATE_EN_BMSK                                                                0x10
#define HWIO_MSS_UIM2_UART_DM_IRDA_MEDIUM_RATE_EN_SHFT                                                                 0x4
#define HWIO_MSS_UIM2_UART_DM_IRDA_IRDA_LOOPBACK_BMSK                                                                  0x8
#define HWIO_MSS_UIM2_UART_DM_IRDA_IRDA_LOOPBACK_SHFT                                                                  0x3
#define HWIO_MSS_UIM2_UART_DM_IRDA_INVERT_IRDA_TX_BMSK                                                                 0x4
#define HWIO_MSS_UIM2_UART_DM_IRDA_INVERT_IRDA_TX_SHFT                                                                 0x2
#define HWIO_MSS_UIM2_UART_DM_IRDA_INVERT_IRDA_RX_BMSK                                                                 0x2
#define HWIO_MSS_UIM2_UART_DM_IRDA_INVERT_IRDA_RX_SHFT                                                                 0x1
#define HWIO_MSS_UIM2_UART_DM_IRDA_IRDA_EN_BMSK                                                                        0x1
#define HWIO_MSS_UIM2_UART_DM_IRDA_IRDA_EN_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_RX_TOTAL_SNAP_ADDR                                                                (MSS_UIM2_UART_DM_REG_BASE      + 0x000000bc)
#define HWIO_MSS_UIM2_UART_DM_RX_TOTAL_SNAP_RMSK                                                                  0xffffff
#define HWIO_MSS_UIM2_UART_DM_RX_TOTAL_SNAP_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RX_TOTAL_SNAP_ADDR, HWIO_MSS_UIM2_UART_DM_RX_TOTAL_SNAP_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RX_TOTAL_SNAP_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RX_TOTAL_SNAP_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RX_TOTAL_SNAP_RX_TOTAL_BYTES_BMSK                                                   0xffffff
#define HWIO_MSS_UIM2_UART_DM_RX_TOTAL_SNAP_RX_TOTAL_BYTES_SHFT                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_WWT_TIMEOUT_ADDR                                                                  (MSS_UIM2_UART_DM_REG_BASE      + 0x000000c0)
#define HWIO_MSS_UIM2_UART_DM_WWT_TIMEOUT_RMSK                                                                   0x1ffffff
#define HWIO_MSS_UIM2_UART_DM_WWT_TIMEOUT_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_WWT_TIMEOUT_ADDR, HWIO_MSS_UIM2_UART_DM_WWT_TIMEOUT_RMSK)
#define HWIO_MSS_UIM2_UART_DM_WWT_TIMEOUT_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_WWT_TIMEOUT_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_WWT_TIMEOUT_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_WWT_TIMEOUT_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_WWT_TIMEOUT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_WWT_TIMEOUT_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_WWT_TIMEOUT_IN)
#define HWIO_MSS_UIM2_UART_DM_WWT_TIMEOUT_WWT_TIMEOUT_BMSK                                                       0x1ffffff
#define HWIO_MSS_UIM2_UART_DM_WWT_TIMEOUT_WWT_TIMEOUT_SHFT                                                             0x0

#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_ADDR                                                                     (MSS_UIM2_UART_DM_REG_BASE      + 0x000000c4)
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_RMSK                                                                       0xf06731
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_CLK_CTRL_ADDR, HWIO_MSS_UIM2_UART_DM_CLK_CTRL_RMSK)
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_CLK_CTRL_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_CLK_CTRL_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_CLK_CTRL_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_CLK_CTRL_IN)
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_UART_IRDA_CLK_CGC_OPEN_BMSK                                                0x800000
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_UART_IRDA_CLK_CGC_OPEN_SHFT                                                    0x17
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_UART_SIM_CLK_CGC_OPEN_BMSK                                                 0x400000
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_UART_SIM_CLK_CGC_OPEN_SHFT                                                     0x16
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_UART_RX_CLK_CGC_OPEN_BMSK                                                  0x200000
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_UART_RX_CLK_CGC_OPEN_SHFT                                                      0x15
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_UART_TX_CLK_CGC_OPEN_BMSK                                                  0x100000
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_UART_TX_CLK_CGC_OPEN_SHFT                                                      0x14
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_AHB_RX_BAM_CLK_CGC_OPEN_BMSK                                                 0x4000
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_AHB_RX_BAM_CLK_CGC_OPEN_SHFT                                                    0xe
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_AHB_TX_BAM_CLK_CGC_OPEN_BMSK                                                 0x2000
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_AHB_TX_BAM_CLK_CGC_OPEN_SHFT                                                    0xd
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_AHB_RX_CLK_CGC_OPEN_BMSK                                                      0x400
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_AHB_RX_CLK_CGC_OPEN_SHFT                                                        0xa
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_AHB_TX_CLK_CGC_OPEN_BMSK                                                      0x200
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_AHB_TX_CLK_CGC_OPEN_SHFT                                                        0x9
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_AHB_WR_CLK_CGC_OPEN_BMSK                                                      0x100
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_AHB_WR_CLK_CGC_OPEN_SHFT                                                        0x8
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_RX_ENABLE_CGC_OPT_BMSK                                                         0x20
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_RX_ENABLE_CGC_OPT_SHFT                                                          0x5
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_TX_ENABLE_CGC_OPT_BMSK                                                         0x10
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_TX_ENABLE_CGC_OPT_SHFT                                                          0x4
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_AHB_CLK_CGC_CLOSE_BMSK                                                          0x1
#define HWIO_MSS_UIM2_UART_DM_CLK_CTRL_AHB_CLK_CGC_CLOSE_SHFT                                                          0x0

#define HWIO_MSS_UIM2_UART_DM_BCR_ADDR                                                                          (MSS_UIM2_UART_DM_REG_BASE      + 0x000000c8)
#define HWIO_MSS_UIM2_UART_DM_BCR_RMSK                                                                                0x37
#define HWIO_MSS_UIM2_UART_DM_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_BCR_ADDR, HWIO_MSS_UIM2_UART_DM_BCR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_BCR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_BCR_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_BCR_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_BCR_IN)
#define HWIO_MSS_UIM2_UART_DM_BCR_RX_DMRX_1BYTE_RES_EN_BMSK                                                           0x20
#define HWIO_MSS_UIM2_UART_DM_BCR_RX_DMRX_1BYTE_RES_EN_SHFT                                                            0x5
#define HWIO_MSS_UIM2_UART_DM_BCR_RX_STALE_IRQ_DMRX_EQUAL_BMSK                                                        0x10
#define HWIO_MSS_UIM2_UART_DM_BCR_RX_STALE_IRQ_DMRX_EQUAL_SHFT                                                         0x4
#define HWIO_MSS_UIM2_UART_DM_BCR_RX_DMRX_LOW_EN_BMSK                                                                  0x4
#define HWIO_MSS_UIM2_UART_DM_BCR_RX_DMRX_LOW_EN_SHFT                                                                  0x2
#define HWIO_MSS_UIM2_UART_DM_BCR_STALE_IRQ_EMPTY_BMSK                                                                 0x2
#define HWIO_MSS_UIM2_UART_DM_BCR_STALE_IRQ_EMPTY_SHFT                                                                 0x1
#define HWIO_MSS_UIM2_UART_DM_BCR_TX_BREAK_DISABLE_BMSK                                                                0x1
#define HWIO_MSS_UIM2_UART_DM_BCR_TX_BREAK_DISABLE_SHFT                                                                0x0

#define HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_ADDR                                                                (MSS_UIM2_UART_DM_REG_BASE      + 0x000000cc)
#define HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_RMSK                                                                       0x7
#define HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_ADDR, HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_IN)
#define HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_RX_DMRX_CYCLIC_EN_BMSK                                                     0x4
#define HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_RX_DMRX_CYCLIC_EN_SHFT                                                     0x2
#define HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_RX_TRANS_AUTO_RE_ACTIVATE_BMSK                                             0x2
#define HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_RX_TRANS_AUTO_RE_ACTIVATE_SHFT                                             0x1
#define HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_RX_STALE_AUTO_RE_EN_BMSK                                                   0x1
#define HWIO_MSS_UIM2_UART_DM_RX_TRANS_CTRL_RX_STALE_AUTO_RE_EN_SHFT                                                   0x0

#define HWIO_MSS_UIM2_UART_DM_DMRX_DBG_ADDR                                                                     (MSS_UIM2_UART_DM_REG_BASE      + 0x000000d0)
#define HWIO_MSS_UIM2_UART_DM_DMRX_DBG_RMSK                                                                      0x1ffffff
#define HWIO_MSS_UIM2_UART_DM_DMRX_DBG_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_DMRX_DBG_ADDR, HWIO_MSS_UIM2_UART_DM_DMRX_DBG_RMSK)
#define HWIO_MSS_UIM2_UART_DM_DMRX_DBG_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_DMRX_DBG_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_DMRX_DBG_UART_DM_DMRX_VAL_BMSK                                                     0x1ffffff
#define HWIO_MSS_UIM2_UART_DM_DMRX_DBG_UART_DM_DMRX_VAL_SHFT                                                           0x0

#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_ADDR                                                                   (MSS_UIM2_UART_DM_REG_BASE      + 0x000000d4)
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_RMSK                                                                   0x37377771
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_FSM_STATUS_ADDR, HWIO_MSS_UIM2_UART_DM_FSM_STATUS_RMSK)
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_FSM_STATUS_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_TX_COMP_FSM_BMSK                                                       0x30000000
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_TX_COMP_FSM_SHFT                                                             0x1c
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_RX_PACK_FSM_BMSK                                                        0x7000000
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_RX_PACK_FSM_SHFT                                                             0x18
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_RX_TRANS_FSM_BMSK                                                        0x300000
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_RX_TRANS_FSM_SHFT                                                            0x14
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_TX_TRANS_FSM_BMSK                                                         0x70000
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_TX_TRANS_FSM_SHFT                                                            0x10
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_RX_PRO_TRANS_END_FSM_BMSK                                                  0x7000
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_RX_PRO_TRANS_END_FSM_SHFT                                                     0xc
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_RX_PRO_ACTIVE_FSM_BMSK                                                      0x700
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_RX_PRO_ACTIVE_FSM_SHFT                                                        0x8
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_TX_CON_TRANS_END_FSM_BMSK                                                    0x70
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_TX_CON_TRANS_END_FSM_SHFT                                                     0x4
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_RX_TRANSFER_ACTIVE_BMSK                                                       0x1
#define HWIO_MSS_UIM2_UART_DM_FSM_STATUS_RX_TRANSFER_ACTIVE_SHFT                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_HW_VERSION_ADDR                                                                   (MSS_UIM2_UART_DM_REG_BASE      + 0x000000d8)
#define HWIO_MSS_UIM2_UART_DM_HW_VERSION_RMSK                                                                   0xffffffff
#define HWIO_MSS_UIM2_UART_DM_HW_VERSION_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_HW_VERSION_ADDR, HWIO_MSS_UIM2_UART_DM_HW_VERSION_RMSK)
#define HWIO_MSS_UIM2_UART_DM_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_HW_VERSION_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_HW_VERSION_HW_VERSION_MAJOR_BMSK                                                  0xf0000000
#define HWIO_MSS_UIM2_UART_DM_HW_VERSION_HW_VERSION_MAJOR_SHFT                                                        0x1c
#define HWIO_MSS_UIM2_UART_DM_HW_VERSION_HW_VERSION_MINOR_BMSK                                                   0xfff0000
#define HWIO_MSS_UIM2_UART_DM_HW_VERSION_HW_VERSION_MINOR_SHFT                                                        0x10
#define HWIO_MSS_UIM2_UART_DM_HW_VERSION_HW_VERSION_STEP_BMSK                                                       0xffff
#define HWIO_MSS_UIM2_UART_DM_HW_VERSION_HW_VERSION_STEP_SHFT                                                          0x0

#define HWIO_MSS_UIM2_UART_DM_GENERICS_ADDR                                                                     (MSS_UIM2_UART_DM_REG_BASE      + 0x000000dc)
#define HWIO_MSS_UIM2_UART_DM_GENERICS_RMSK                                                                           0xff
#define HWIO_MSS_UIM2_UART_DM_GENERICS_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_GENERICS_ADDR, HWIO_MSS_UIM2_UART_DM_GENERICS_RMSK)
#define HWIO_MSS_UIM2_UART_DM_GENERICS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_GENERICS_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_GENERICS_GENERIC_BAM_IFC_BMSK                                                           0x80
#define HWIO_MSS_UIM2_UART_DM_GENERICS_GENERIC_BAM_IFC_SHFT                                                            0x7
#define HWIO_MSS_UIM2_UART_DM_GENERICS_GENERIC_DM_IFC_BMSK                                                            0x40
#define HWIO_MSS_UIM2_UART_DM_GENERICS_GENERIC_DM_IFC_SHFT                                                             0x6
#define HWIO_MSS_UIM2_UART_DM_GENERICS_GENERIC_IRDA_IFC_BMSK                                                          0x20
#define HWIO_MSS_UIM2_UART_DM_GENERICS_GENERIC_IRDA_IFC_SHFT                                                           0x5
#define HWIO_MSS_UIM2_UART_DM_GENERICS_GENERIC_SIM_GLUE_BMSK                                                          0x10
#define HWIO_MSS_UIM2_UART_DM_GENERICS_GENERIC_SIM_GLUE_SHFT                                                           0x4
#define HWIO_MSS_UIM2_UART_DM_GENERICS_GENERIC_RAM_ADDR_WIDTH_BMSK                                                     0xf
#define HWIO_MSS_UIM2_UART_DM_GENERICS_GENERIC_RAM_ADDR_WIDTH_SHFT                                                     0x0

#define HWIO_MSS_UIM2_UART_DM_TF_ADDR                                                                           (MSS_UIM2_UART_DM_REG_BASE      + 0x00000100)
#define HWIO_MSS_UIM2_UART_DM_TF_RMSK                                                                           0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_UART_TF_BMSK                                                                   0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_UART_TF_SHFT                                                                          0x0

#define HWIO_MSS_UIM2_UART_DM_TF_2_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000104)
#define HWIO_MSS_UIM2_UART_DM_TF_2_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_2_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_2_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_2_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_2_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_TF_3_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000108)
#define HWIO_MSS_UIM2_UART_DM_TF_3_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_3_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_3_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_3_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_3_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_TF_4_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x0000010c)
#define HWIO_MSS_UIM2_UART_DM_TF_4_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_4_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_4_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_4_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_4_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_TF_5_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000110)
#define HWIO_MSS_UIM2_UART_DM_TF_5_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_5_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_5_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_5_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_5_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_TF_6_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000114)
#define HWIO_MSS_UIM2_UART_DM_TF_6_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_6_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_6_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_6_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_6_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_TF_7_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000118)
#define HWIO_MSS_UIM2_UART_DM_TF_7_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_7_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_7_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_7_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_7_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_TF_8_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x0000011c)
#define HWIO_MSS_UIM2_UART_DM_TF_8_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_8_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_8_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_8_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_8_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_TF_9_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000120)
#define HWIO_MSS_UIM2_UART_DM_TF_9_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_9_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_9_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_9_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_9_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_TF_10_ADDR                                                                        (MSS_UIM2_UART_DM_REG_BASE      + 0x00000124)
#define HWIO_MSS_UIM2_UART_DM_TF_10_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_10_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_10_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_10_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_10_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_TF_11_ADDR                                                                        (MSS_UIM2_UART_DM_REG_BASE      + 0x00000128)
#define HWIO_MSS_UIM2_UART_DM_TF_11_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_11_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_11_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_11_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_11_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_TF_12_ADDR                                                                        (MSS_UIM2_UART_DM_REG_BASE      + 0x0000012c)
#define HWIO_MSS_UIM2_UART_DM_TF_12_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_12_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_12_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_12_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_12_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_TF_13_ADDR                                                                        (MSS_UIM2_UART_DM_REG_BASE      + 0x00000130)
#define HWIO_MSS_UIM2_UART_DM_TF_13_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_13_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_13_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_13_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_13_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_TF_14_ADDR                                                                        (MSS_UIM2_UART_DM_REG_BASE      + 0x00000134)
#define HWIO_MSS_UIM2_UART_DM_TF_14_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_14_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_14_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_14_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_14_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_TF_15_ADDR                                                                        (MSS_UIM2_UART_DM_REG_BASE      + 0x00000138)
#define HWIO_MSS_UIM2_UART_DM_TF_15_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_15_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_15_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_15_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_15_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_TF_16_ADDR                                                                        (MSS_UIM2_UART_DM_REG_BASE      + 0x0000013c)
#define HWIO_MSS_UIM2_UART_DM_TF_16_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_16_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_TF_16_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_TF_16_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_TF_16_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_RF_ADDR                                                                           (MSS_UIM2_UART_DM_REG_BASE      + 0x00000140)
#define HWIO_MSS_UIM2_UART_DM_RF_RMSK                                                                           0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_ADDR, HWIO_MSS_UIM2_UART_DM_RF_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_UART_RF_BMSK                                                                   0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_UART_RF_SHFT                                                                          0x0

#define HWIO_MSS_UIM2_UART_DM_RF_2_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000144)
#define HWIO_MSS_UIM2_UART_DM_RF_2_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_2_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_2_ADDR, HWIO_MSS_UIM2_UART_DM_RF_2_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_2_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_2_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_2_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_2_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_RF_3_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000148)
#define HWIO_MSS_UIM2_UART_DM_RF_3_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_3_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_3_ADDR, HWIO_MSS_UIM2_UART_DM_RF_3_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_3_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_3_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_3_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_3_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_RF_4_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x0000014c)
#define HWIO_MSS_UIM2_UART_DM_RF_4_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_4_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_4_ADDR, HWIO_MSS_UIM2_UART_DM_RF_4_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_4_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_4_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_4_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_4_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_RF_5_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000150)
#define HWIO_MSS_UIM2_UART_DM_RF_5_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_5_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_5_ADDR, HWIO_MSS_UIM2_UART_DM_RF_5_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_5_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_5_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_5_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_5_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_RF_6_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000154)
#define HWIO_MSS_UIM2_UART_DM_RF_6_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_6_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_6_ADDR, HWIO_MSS_UIM2_UART_DM_RF_6_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_6_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_6_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_6_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_6_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_RF_7_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000158)
#define HWIO_MSS_UIM2_UART_DM_RF_7_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_7_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_7_ADDR, HWIO_MSS_UIM2_UART_DM_RF_7_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_7_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_7_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_7_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_7_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_RF_8_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x0000015c)
#define HWIO_MSS_UIM2_UART_DM_RF_8_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_8_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_8_ADDR, HWIO_MSS_UIM2_UART_DM_RF_8_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_8_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_8_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_8_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_8_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_RF_9_ADDR                                                                         (MSS_UIM2_UART_DM_REG_BASE      + 0x00000160)
#define HWIO_MSS_UIM2_UART_DM_RF_9_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_9_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_9_ADDR, HWIO_MSS_UIM2_UART_DM_RF_9_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_9_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_9_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_9_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_9_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_RF_10_ADDR                                                                        (MSS_UIM2_UART_DM_REG_BASE      + 0x00000164)
#define HWIO_MSS_UIM2_UART_DM_RF_10_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_10_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_10_ADDR, HWIO_MSS_UIM2_UART_DM_RF_10_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_10_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_10_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_10_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_10_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_RF_11_ADDR                                                                        (MSS_UIM2_UART_DM_REG_BASE      + 0x00000168)
#define HWIO_MSS_UIM2_UART_DM_RF_11_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_11_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_11_ADDR, HWIO_MSS_UIM2_UART_DM_RF_11_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_11_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_11_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_11_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_11_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_RF_12_ADDR                                                                        (MSS_UIM2_UART_DM_REG_BASE      + 0x0000016c)
#define HWIO_MSS_UIM2_UART_DM_RF_12_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_12_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_12_ADDR, HWIO_MSS_UIM2_UART_DM_RF_12_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_12_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_12_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_12_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_12_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_RF_13_ADDR                                                                        (MSS_UIM2_UART_DM_REG_BASE      + 0x00000170)
#define HWIO_MSS_UIM2_UART_DM_RF_13_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_13_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_13_ADDR, HWIO_MSS_UIM2_UART_DM_RF_13_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_13_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_13_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_13_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_13_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_RF_14_ADDR                                                                        (MSS_UIM2_UART_DM_REG_BASE      + 0x00000174)
#define HWIO_MSS_UIM2_UART_DM_RF_14_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_14_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_14_ADDR, HWIO_MSS_UIM2_UART_DM_RF_14_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_14_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_14_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_14_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_14_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_RF_15_ADDR                                                                        (MSS_UIM2_UART_DM_REG_BASE      + 0x00000178)
#define HWIO_MSS_UIM2_UART_DM_RF_15_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_15_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_15_ADDR, HWIO_MSS_UIM2_UART_DM_RF_15_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_15_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_15_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_15_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_15_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_RF_16_ADDR                                                                        (MSS_UIM2_UART_DM_REG_BASE      + 0x0000017c)
#define HWIO_MSS_UIM2_UART_DM_RF_16_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_16_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_16_ADDR, HWIO_MSS_UIM2_UART_DM_RF_16_RMSK)
#define HWIO_MSS_UIM2_UART_DM_RF_16_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_RF_16_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_RF_16_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM2_UART_DM_RF_16_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_ADDR                                                                      (MSS_UIM2_UART_DM_REG_BASE      + 0x00000180)
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_RMSK                                                                          0xf77f
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_UIM_CFG_ADDR, HWIO_MSS_UIM2_UART_DM_UIM_CFG_RMSK)
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_UIM_CFG_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_UIM_CFG_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_UIM_CFG_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_UIM_CFG_IN)
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_BATT_ALARM_QUICK_DROP_EN_BMSK                                                 0x8000
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_BATT_ALARM_QUICK_DROP_EN_SHFT                                                    0xf
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_TESTBUS_EN_BMSK                                                               0x4000
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_TESTBUS_EN_SHFT                                                                  0xe
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_SW_RESET_BMSK                                                                 0x2000
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_SW_RESET_SHFT                                                                    0xd
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_MODE18_BMSK                                                                   0x1000
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_MODE18_SHFT                                                                      0xc
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_PMIC_ALARM_EN_BMSK                                                             0x400
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_PMIC_ALARM_EN_SHFT                                                               0xa
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_BATT_ALARM_TRIGGER_EN_BMSK                                                     0x200
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_BATT_ALARM_TRIGGER_EN_SHFT                                                       0x9
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_UIM_RMV_TRIGGER_EN_BMSK                                                        0x100
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_UIM_RMV_TRIGGER_EN_SHFT                                                          0x8
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_UIM_CARD_EVENTS_ENABLE_BMSK                                                     0x40
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_UIM_CARD_EVENTS_ENABLE_SHFT                                                      0x6
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_UIM_PRESENT_POLARITY_BMSK                                                       0x20
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_UIM_PRESENT_POLARITY_SHFT                                                        0x5
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_EVENT_DEBOUNCE_TIME_BMSK                                                        0x1f
#define HWIO_MSS_UIM2_UART_DM_UIM_CFG_EVENT_DEBOUNCE_TIME_SHFT                                                         0x0

#define HWIO_MSS_UIM2_UART_DM_UIM_CMD_ADDR                                                                      (MSS_UIM2_UART_DM_REG_BASE      + 0x00000184)
#define HWIO_MSS_UIM2_UART_DM_UIM_CMD_RMSK                                                                             0x3
#define HWIO_MSS_UIM2_UART_DM_UIM_CMD_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_UIM_CMD_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_UIM_CMD_RECOVER_FROM_HW_DEACTIVATION_BMSK                                                0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_CMD_RECOVER_FROM_HW_DEACTIVATION_SHFT                                                0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_CMD_INITIATE_HW_DEACTIVATION_BMSK                                                    0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_CMD_INITIATE_HW_DEACTIVATION_SHFT                                                    0x0

#define HWIO_MSS_UIM2_UART_DM_UIM_IO_STATUS_ADDR                                                                (MSS_UIM2_UART_DM_REG_BASE      + 0x00000188)
#define HWIO_MSS_UIM2_UART_DM_UIM_IO_STATUS_RMSK                                                                       0x7
#define HWIO_MSS_UIM2_UART_DM_UIM_IO_STATUS_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_UIM_IO_STATUS_ADDR, HWIO_MSS_UIM2_UART_DM_UIM_IO_STATUS_RMSK)
#define HWIO_MSS_UIM2_UART_DM_UIM_IO_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_UIM_IO_STATUS_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_UIM_IO_STATUS_UIM_IO_WRITE_IN_PROGRESS_BMSK                                              0x4
#define HWIO_MSS_UIM2_UART_DM_UIM_IO_STATUS_UIM_IO_WRITE_IN_PROGRESS_SHFT                                              0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_IO_STATUS_UIM_DEACTIVATION_STATUS_BMSK                                               0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_IO_STATUS_UIM_DEACTIVATION_STATUS_SHFT                                               0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_IO_STATUS_CARD_PRESENCE_BMSK                                                         0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_IO_STATUS_CARD_PRESENCE_SHFT                                                         0x0

#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_ADDR                                                                  (MSS_UIM2_UART_DM_REG_BASE      + 0x0000018c)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_RMSK                                                                        0x1f
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_ADDR, HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_UIM_IO_WRITE_DONE_BMSK                                                      0x10
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_UIM_IO_WRITE_DONE_SHFT                                                       0x4
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_HW_SEQUENCE_FINISH_BMSK                                                      0x8
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_HW_SEQUENCE_FINISH_SHFT                                                      0x3
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_BATT_ALARM_BMSK                                                              0x4
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_BATT_ALARM_SHFT                                                              0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_UIM_CARD_INSERTION_BMSK                                                      0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_UIM_CARD_INSERTION_SHFT                                                      0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_UIM_CARD_REMOVAL_BMSK                                                        0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_ISR_UIM_CARD_REMOVAL_SHFT                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_ADDR                                                                 (MSS_UIM2_UART_DM_REG_BASE      + 0x00000190)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_RMSK                                                                       0x1f
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_ADDR, HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_UIM_IO_WRITE_DONE_BMSK                                                     0x10
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_UIM_IO_WRITE_DONE_SHFT                                                      0x4
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_HW_SEQUENCE_FINISH_BMSK                                                     0x8
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_HW_SEQUENCE_FINISH_SHFT                                                     0x3
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_BATT_ALARM_BMSK                                                             0x4
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_BATT_ALARM_SHFT                                                             0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_UIM_CARD_INSERTION_BMSK                                                     0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_UIM_CARD_INSERTION_SHFT                                                     0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_UIM_CARD_REMOVAL_BMSK                                                       0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_MISR_UIM_CARD_REMOVAL_SHFT                                                       0x0

#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_CLR_ADDR                                                                  (MSS_UIM2_UART_DM_REG_BASE      + 0x00000194)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_CLR_RMSK                                                                        0x1f
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_CLR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_UIM_IRQ_CLR_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_CLR_UIM_IO_WRITE_DONE_BMSK                                                      0x10
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_CLR_UIM_IO_WRITE_DONE_SHFT                                                       0x4
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_CLR_HW_SEQUENCE_FINISH_BMSK                                                      0x8
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_CLR_HW_SEQUENCE_FINISH_SHFT                                                      0x3
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_CLR_BATT_ALARM_BMSK                                                              0x4
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_CLR_BATT_ALARM_SHFT                                                              0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_CLR_UIM_CARD_INSERTION_BMSK                                                      0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_CLR_UIM_CARD_INSERTION_SHFT                                                      0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_CLR_UIM_CARD_REMOVAL_BMSK                                                        0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_CLR_UIM_CARD_REMOVAL_SHFT                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_ADDR                                                                  (MSS_UIM2_UART_DM_REG_BASE      + 0x00000198)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_RMSK                                                                        0x1f
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_ADDR, HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_RMSK)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_ADDR, m)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_ADDR,m,v,HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_IN)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_UIM_IO_WRITE_DONE_BMSK                                                      0x10
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_UIM_IO_WRITE_DONE_SHFT                                                       0x4
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_HW_SEQUENCE_FINISH_BMSK                                                      0x8
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_HW_SEQUENCE_FINISH_SHFT                                                      0x3
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_BATT_ALARM_BMSK                                                              0x4
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_BATT_ALARM_SHFT                                                              0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_UIM_CARD_INSERTION_BMSK                                                      0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_UIM_CARD_INSERTION_SHFT                                                      0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_UIM_CARD_REMOVAL_BMSK                                                        0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_UIM_CARD_REMOVAL_SHFT                                                        0x0

#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET_ADDR                                                              (MSS_UIM2_UART_DM_REG_BASE      + 0x0000019c)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET_RMSK                                                                    0x1f
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET_UIM_IO_WRITE_DONE_BMSK                                                  0x10
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET_UIM_IO_WRITE_DONE_SHFT                                                   0x4
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET_HW_SEQUENCE_FINISH_BMSK                                                  0x8
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET_HW_SEQUENCE_FINISH_SHFT                                                  0x3
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET_BATT_ALARM_BMSK                                                          0x4
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET_BATT_ALARM_SHFT                                                          0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_INSERTION_BMSK                                                  0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_INSERTION_SHFT                                                  0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_REMOVAL_BMSK                                                    0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_REMOVAL_SHFT                                                    0x0

#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_CLR_ADDR                                                              (MSS_UIM2_UART_DM_REG_BASE      + 0x000001a0)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_CLR_RMSK                                                                    0x1f
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_CLR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_CLR_ADDR,v)
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_CLR_UIM_IO_WRITE_DONE_BMSK                                                  0x10
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_CLR_UIM_IO_WRITE_DONE_SHFT                                                   0x4
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_CLR_HW_SEQUENCE_FINISH_BMSK                                                  0x8
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_CLR_HW_SEQUENCE_FINISH_SHFT                                                  0x3
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_CLR_BATT_ALARM_BMSK                                                          0x4
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_CLR_BATT_ALARM_SHFT                                                          0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_INSERTION_BMSK                                                  0x2
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_INSERTION_SHFT                                                  0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_REMOVAL_BMSK                                                    0x1
#define HWIO_MSS_UIM2_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_REMOVAL_SHFT                                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: MSS_UIM3_UART_DM
 *--------------------------------------------------------------------------*/

#define MSS_UIM3_UART_DM_REG_BASE                                                                               (MODEM_TOP_BASE      + 0x0019c000)

#define HWIO_MSS_UIM3_UART_DM_MR1_ADDR                                                                          (MSS_UIM3_UART_DM_REG_BASE      + 0x00000000)
#define HWIO_MSS_UIM3_UART_DM_MR1_RMSK                                                                          0xffffffff
#define HWIO_MSS_UIM3_UART_DM_MR1_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_MR1_ADDR, HWIO_MSS_UIM3_UART_DM_MR1_RMSK)
#define HWIO_MSS_UIM3_UART_DM_MR1_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_MR1_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_MR1_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_MR1_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_MR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_MR1_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_MR1_IN)
#define HWIO_MSS_UIM3_UART_DM_MR1_AUTO_RFR_LEVEL1_BMSK                                                          0xffffff00
#define HWIO_MSS_UIM3_UART_DM_MR1_AUTO_RFR_LEVEL1_SHFT                                                                 0x8
#define HWIO_MSS_UIM3_UART_DM_MR1_RX_RDY_CTL_BMSK                                                                     0x80
#define HWIO_MSS_UIM3_UART_DM_MR1_RX_RDY_CTL_SHFT                                                                      0x7
#define HWIO_MSS_UIM3_UART_DM_MR1_CTS_CTL_BMSK                                                                        0x40
#define HWIO_MSS_UIM3_UART_DM_MR1_CTS_CTL_SHFT                                                                         0x6
#define HWIO_MSS_UIM3_UART_DM_MR1_AUTO_RFR_LEVEL0_BMSK                                                                0x3f
#define HWIO_MSS_UIM3_UART_DM_MR1_AUTO_RFR_LEVEL0_SHFT                                                                 0x0

#define HWIO_MSS_UIM3_UART_DM_MR2_ADDR                                                                          (MSS_UIM3_UART_DM_REG_BASE      + 0x00000004)
#define HWIO_MSS_UIM3_UART_DM_MR2_RMSK                                                                               0x7ff
#define HWIO_MSS_UIM3_UART_DM_MR2_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_MR2_ADDR, HWIO_MSS_UIM3_UART_DM_MR2_RMSK)
#define HWIO_MSS_UIM3_UART_DM_MR2_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_MR2_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_MR2_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_MR2_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_MR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_MR2_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_MR2_IN)
#define HWIO_MSS_UIM3_UART_DM_MR2_RFR_CTS_LOOPBACK_BMSK                                                              0x400
#define HWIO_MSS_UIM3_UART_DM_MR2_RFR_CTS_LOOPBACK_SHFT                                                                0xa
#define HWIO_MSS_UIM3_UART_DM_MR2_RX_ERROR_CHAR_OFF_BMSK                                                             0x200
#define HWIO_MSS_UIM3_UART_DM_MR2_RX_ERROR_CHAR_OFF_SHFT                                                               0x9
#define HWIO_MSS_UIM3_UART_DM_MR2_RX_BREAK_ZERO_CHAR_OFF_BMSK                                                        0x100
#define HWIO_MSS_UIM3_UART_DM_MR2_RX_BREAK_ZERO_CHAR_OFF_SHFT                                                          0x8
#define HWIO_MSS_UIM3_UART_DM_MR2_LOOPBACK_BMSK                                                                       0x80
#define HWIO_MSS_UIM3_UART_DM_MR2_LOOPBACK_SHFT                                                                        0x7
#define HWIO_MSS_UIM3_UART_DM_MR2_ERROR_MODE_BMSK                                                                     0x40
#define HWIO_MSS_UIM3_UART_DM_MR2_ERROR_MODE_SHFT                                                                      0x6
#define HWIO_MSS_UIM3_UART_DM_MR2_BITS_PER_CHAR_BMSK                                                                  0x30
#define HWIO_MSS_UIM3_UART_DM_MR2_BITS_PER_CHAR_SHFT                                                                   0x4
#define HWIO_MSS_UIM3_UART_DM_MR2_STOP_BIT_LEN_BMSK                                                                    0xc
#define HWIO_MSS_UIM3_UART_DM_MR2_STOP_BIT_LEN_SHFT                                                                    0x2
#define HWIO_MSS_UIM3_UART_DM_MR2_PARITY_MODE_BMSK                                                                     0x3
#define HWIO_MSS_UIM3_UART_DM_MR2_PARITY_MODE_SHFT                                                                     0x0

#define HWIO_MSS_UIM3_UART_DM_CSR_SR_DEPRECATED_ADDR                                                            (MSS_UIM3_UART_DM_REG_BASE      + 0x00000008)
#define HWIO_MSS_UIM3_UART_DM_CSR_SR_DEPRECATED_RMSK                                                            0xffffffff
#define HWIO_MSS_UIM3_UART_DM_CSR_SR_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_CSR_SR_DEPRECATED_ADDR, HWIO_MSS_UIM3_UART_DM_CSR_SR_DEPRECATED_RMSK)
#define HWIO_MSS_UIM3_UART_DM_CSR_SR_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_CSR_SR_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_CSR_SR_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_CSR_SR_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_CSR_SR_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_CSR_SR_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_CSR_SR_DEPRECATED_IN)
#define HWIO_MSS_UIM3_UART_DM_CSR_SR_DEPRECATED_UART_DM_CSR_SR_DEPRECATED_BMSK                                  0xffffffff
#define HWIO_MSS_UIM3_UART_DM_CSR_SR_DEPRECATED_UART_DM_CSR_SR_DEPRECATED_SHFT                                         0x0

#define HWIO_MSS_UIM3_UART_DM_CR_MISR_DEPRECATED_ADDR                                                           (MSS_UIM3_UART_DM_REG_BASE      + 0x00000010)
#define HWIO_MSS_UIM3_UART_DM_CR_MISR_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM3_UART_DM_CR_MISR_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_CR_MISR_DEPRECATED_ADDR, HWIO_MSS_UIM3_UART_DM_CR_MISR_DEPRECATED_RMSK)
#define HWIO_MSS_UIM3_UART_DM_CR_MISR_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_CR_MISR_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_CR_MISR_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_CR_MISR_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_CR_MISR_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_CR_MISR_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_CR_MISR_DEPRECATED_IN)
#define HWIO_MSS_UIM3_UART_DM_CR_MISR_DEPRECATED_UART_DM_CR_MISR_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_CR_MISR_DEPRECATED_UART_DM_CR_MISR_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM3_UART_DM_IMR_ISR_DEPRECATED_ADDR                                                           (MSS_UIM3_UART_DM_REG_BASE      + 0x00000014)
#define HWIO_MSS_UIM3_UART_DM_IMR_ISR_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM3_UART_DM_IMR_ISR_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_IMR_ISR_DEPRECATED_ADDR, HWIO_MSS_UIM3_UART_DM_IMR_ISR_DEPRECATED_RMSK)
#define HWIO_MSS_UIM3_UART_DM_IMR_ISR_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_IMR_ISR_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_IMR_ISR_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_IMR_ISR_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_IMR_ISR_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_IMR_ISR_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_IMR_ISR_DEPRECATED_IN)
#define HWIO_MSS_UIM3_UART_DM_IMR_ISR_DEPRECATED_UART_DM_IMR_ISR_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_IMR_ISR_DEPRECATED_UART_DM_IMR_ISR_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM3_UART_DM_IPR_ADDR                                                                          (MSS_UIM3_UART_DM_REG_BASE      + 0x00000018)
#define HWIO_MSS_UIM3_UART_DM_IPR_RMSK                                                                          0xffffffdf
#define HWIO_MSS_UIM3_UART_DM_IPR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_IPR_ADDR, HWIO_MSS_UIM3_UART_DM_IPR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_IPR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_IPR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_IPR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_IPR_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_IPR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_IPR_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_IPR_IN)
#define HWIO_MSS_UIM3_UART_DM_IPR_STALE_TIMEOUT_MSB_BMSK                                                        0xffffff80
#define HWIO_MSS_UIM3_UART_DM_IPR_STALE_TIMEOUT_MSB_SHFT                                                               0x7
#define HWIO_MSS_UIM3_UART_DM_IPR_SAMPLE_DATA_BMSK                                                                    0x40
#define HWIO_MSS_UIM3_UART_DM_IPR_SAMPLE_DATA_SHFT                                                                     0x6
#define HWIO_MSS_UIM3_UART_DM_IPR_STALE_TIMEOUT_LSB_BMSK                                                              0x1f
#define HWIO_MSS_UIM3_UART_DM_IPR_STALE_TIMEOUT_LSB_SHFT                                                               0x0

#define HWIO_MSS_UIM3_UART_DM_TFWR_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x0000001c)
#define HWIO_MSS_UIM3_UART_DM_TFWR_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TFWR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TFWR_ADDR, HWIO_MSS_UIM3_UART_DM_TFWR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_TFWR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TFWR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_TFWR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TFWR_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TFWR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_TFWR_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_TFWR_IN)
#define HWIO_MSS_UIM3_UART_DM_TFWR_TFW_BMSK                                                                     0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TFWR_TFW_SHFT                                                                            0x0

#define HWIO_MSS_UIM3_UART_DM_RFWR_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000020)
#define HWIO_MSS_UIM3_UART_DM_RFWR_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RFWR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RFWR_ADDR, HWIO_MSS_UIM3_UART_DM_RFWR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RFWR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RFWR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RFWR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_RFWR_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_RFWR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_RFWR_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_RFWR_IN)
#define HWIO_MSS_UIM3_UART_DM_RFWR_RFW_BMSK                                                                     0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RFWR_RFW_SHFT                                                                            0x0

#define HWIO_MSS_UIM3_UART_DM_HCR_ADDR                                                                          (MSS_UIM3_UART_DM_REG_BASE      + 0x00000024)
#define HWIO_MSS_UIM3_UART_DM_HCR_RMSK                                                                                0xff
#define HWIO_MSS_UIM3_UART_DM_HCR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_HCR_ADDR, HWIO_MSS_UIM3_UART_DM_HCR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_HCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_HCR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_HCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_HCR_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_HCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_HCR_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_HCR_IN)
#define HWIO_MSS_UIM3_UART_DM_HCR_DATA_BMSK                                                                           0xff
#define HWIO_MSS_UIM3_UART_DM_HCR_DATA_SHFT                                                                            0x0

#define HWIO_MSS_UIM3_UART_DM_DMRX_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000034)
#define HWIO_MSS_UIM3_UART_DM_DMRX_RMSK                                                                          0x1ffffff
#define HWIO_MSS_UIM3_UART_DM_DMRX_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_DMRX_ADDR, HWIO_MSS_UIM3_UART_DM_DMRX_RMSK)
#define HWIO_MSS_UIM3_UART_DM_DMRX_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_DMRX_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_DMRX_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_DMRX_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_DMRX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_DMRX_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_DMRX_IN)
#define HWIO_MSS_UIM3_UART_DM_DMRX_RX_DM_CRCI_CHARS_BMSK                                                         0x1ffffff
#define HWIO_MSS_UIM3_UART_DM_DMRX_RX_DM_CRCI_CHARS_SHFT                                                               0x0

#define HWIO_MSS_UIM3_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR                                                (MSS_UIM3_UART_DM_REG_BASE      + 0x00000038)
#define HWIO_MSS_UIM3_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_RMSK                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR, HWIO_MSS_UIM3_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_RMSK)
#define HWIO_MSS_UIM3_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_IN)
#define HWIO_MSS_UIM3_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_BMSK          0xffffffff
#define HWIO_MSS_UIM3_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_SHFT                 0x0

#define HWIO_MSS_UIM3_UART_DM_DMEN_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x0000003c)
#define HWIO_MSS_UIM3_UART_DM_DMEN_RMSK                                                                               0x3c
#define HWIO_MSS_UIM3_UART_DM_DMEN_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_DMEN_ADDR, HWIO_MSS_UIM3_UART_DM_DMEN_RMSK)
#define HWIO_MSS_UIM3_UART_DM_DMEN_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_DMEN_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_DMEN_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_DMEN_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_DMEN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_DMEN_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_DMEN_IN)
#define HWIO_MSS_UIM3_UART_DM_DMEN_RX_SC_ENABLE_BMSK                                                                  0x20
#define HWIO_MSS_UIM3_UART_DM_DMEN_RX_SC_ENABLE_SHFT                                                                   0x5
#define HWIO_MSS_UIM3_UART_DM_DMEN_TX_SC_ENABLE_BMSK                                                                  0x10
#define HWIO_MSS_UIM3_UART_DM_DMEN_TX_SC_ENABLE_SHFT                                                                   0x4
#define HWIO_MSS_UIM3_UART_DM_DMEN_RX_BAM_ENABLE_BMSK                                                                  0x8
#define HWIO_MSS_UIM3_UART_DM_DMEN_RX_BAM_ENABLE_SHFT                                                                  0x3
#define HWIO_MSS_UIM3_UART_DM_DMEN_TX_BAM_ENABLE_BMSK                                                                  0x4
#define HWIO_MSS_UIM3_UART_DM_DMEN_TX_BAM_ENABLE_SHFT                                                                  0x2

#define HWIO_MSS_UIM3_UART_DM_NO_CHARS_FOR_TX_ADDR                                                              (MSS_UIM3_UART_DM_REG_BASE      + 0x00000040)
#define HWIO_MSS_UIM3_UART_DM_NO_CHARS_FOR_TX_RMSK                                                                0xffffff
#define HWIO_MSS_UIM3_UART_DM_NO_CHARS_FOR_TX_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_NO_CHARS_FOR_TX_ADDR, HWIO_MSS_UIM3_UART_DM_NO_CHARS_FOR_TX_RMSK)
#define HWIO_MSS_UIM3_UART_DM_NO_CHARS_FOR_TX_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_NO_CHARS_FOR_TX_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_NO_CHARS_FOR_TX_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_NO_CHARS_FOR_TX_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_NO_CHARS_FOR_TX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_NO_CHARS_FOR_TX_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_NO_CHARS_FOR_TX_IN)
#define HWIO_MSS_UIM3_UART_DM_NO_CHARS_FOR_TX_TX_TOTAL_TRANS_LEN_BMSK                                             0xffffff
#define HWIO_MSS_UIM3_UART_DM_NO_CHARS_FOR_TX_TX_TOTAL_TRANS_LEN_SHFT                                                  0x0

#define HWIO_MSS_UIM3_UART_DM_BADR_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000044)
#define HWIO_MSS_UIM3_UART_DM_BADR_RMSK                                                                         0xfffffffc
#define HWIO_MSS_UIM3_UART_DM_BADR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_BADR_ADDR, HWIO_MSS_UIM3_UART_DM_BADR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_BADR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_BADR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_BADR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_BADR_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_BADR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_BADR_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_BADR_IN)
#define HWIO_MSS_UIM3_UART_DM_BADR_RX_BASE_ADDR_BMSK                                                            0xfffffffc
#define HWIO_MSS_UIM3_UART_DM_BADR_RX_BASE_ADDR_SHFT                                                                   0x2

#define HWIO_MSS_UIM3_UART_DM_TESTSL_ADDR                                                                       (MSS_UIM3_UART_DM_REG_BASE      + 0x00000048)
#define HWIO_MSS_UIM3_UART_DM_TESTSL_RMSK                                                                             0x1f
#define HWIO_MSS_UIM3_UART_DM_TESTSL_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TESTSL_ADDR, HWIO_MSS_UIM3_UART_DM_TESTSL_RMSK)
#define HWIO_MSS_UIM3_UART_DM_TESTSL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TESTSL_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_TESTSL_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TESTSL_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TESTSL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_TESTSL_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_TESTSL_IN)
#define HWIO_MSS_UIM3_UART_DM_TESTSL_TEST_EN_BMSK                                                                     0x10
#define HWIO_MSS_UIM3_UART_DM_TESTSL_TEST_EN_SHFT                                                                      0x4
#define HWIO_MSS_UIM3_UART_DM_TESTSL_TEST_SEL_BMSK                                                                     0xf
#define HWIO_MSS_UIM3_UART_DM_TESTSL_TEST_SEL_SHFT                                                                     0x0

#define HWIO_MSS_UIM3_UART_DM_TXFS_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x0000004c)
#define HWIO_MSS_UIM3_UART_DM_TXFS_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TXFS_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TXFS_ADDR, HWIO_MSS_UIM3_UART_DM_TXFS_RMSK)
#define HWIO_MSS_UIM3_UART_DM_TXFS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TXFS_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_TXFS_TX_FIFO_STATE_MSB_BMSK                                                       0xffffc000
#define HWIO_MSS_UIM3_UART_DM_TXFS_TX_FIFO_STATE_MSB_SHFT                                                              0xe
#define HWIO_MSS_UIM3_UART_DM_TXFS_TX_ASYNC_FIFO_STATE_BMSK                                                         0x3c00
#define HWIO_MSS_UIM3_UART_DM_TXFS_TX_ASYNC_FIFO_STATE_SHFT                                                            0xa
#define HWIO_MSS_UIM3_UART_DM_TXFS_TX_BUFFER_STATE_BMSK                                                              0x380
#define HWIO_MSS_UIM3_UART_DM_TXFS_TX_BUFFER_STATE_SHFT                                                                0x7
#define HWIO_MSS_UIM3_UART_DM_TXFS_TX_FIFO_STATE_LSB_BMSK                                                             0x7f
#define HWIO_MSS_UIM3_UART_DM_TXFS_TX_FIFO_STATE_LSB_SHFT                                                              0x0

#define HWIO_MSS_UIM3_UART_DM_RXFS_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000050)
#define HWIO_MSS_UIM3_UART_DM_RXFS_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RXFS_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RXFS_ADDR, HWIO_MSS_UIM3_UART_DM_RXFS_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RXFS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RXFS_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RXFS_RX_FIFO_STATE_MSB_BMSK                                                       0xffffc000
#define HWIO_MSS_UIM3_UART_DM_RXFS_RX_FIFO_STATE_MSB_SHFT                                                              0xe
#define HWIO_MSS_UIM3_UART_DM_RXFS_RX_ASYNC_FIFO_STATE_BMSK                                                         0x3c00
#define HWIO_MSS_UIM3_UART_DM_RXFS_RX_ASYNC_FIFO_STATE_SHFT                                                            0xa
#define HWIO_MSS_UIM3_UART_DM_RXFS_RX_BUFFER_STATE_BMSK                                                              0x380
#define HWIO_MSS_UIM3_UART_DM_RXFS_RX_BUFFER_STATE_SHFT                                                                0x7
#define HWIO_MSS_UIM3_UART_DM_RXFS_RX_FIFO_STATE_LSB_BMSK                                                             0x7f
#define HWIO_MSS_UIM3_UART_DM_RXFS_RX_FIFO_STATE_LSB_SHFT                                                              0x0

#define HWIO_MSS_UIM3_UART_DM_MISR_MODE_ADDR                                                                    (MSS_UIM3_UART_DM_REG_BASE      + 0x00000060)
#define HWIO_MSS_UIM3_UART_DM_MISR_MODE_RMSK                                                                           0x3
#define HWIO_MSS_UIM3_UART_DM_MISR_MODE_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_MISR_MODE_ADDR, HWIO_MSS_UIM3_UART_DM_MISR_MODE_RMSK)
#define HWIO_MSS_UIM3_UART_DM_MISR_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_MISR_MODE_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_MISR_MODE_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_MISR_MODE_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_MISR_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_MISR_MODE_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_MISR_MODE_IN)
#define HWIO_MSS_UIM3_UART_DM_MISR_MODE_MODE_BMSK                                                                      0x3
#define HWIO_MSS_UIM3_UART_DM_MISR_MODE_MODE_SHFT                                                                      0x0

#define HWIO_MSS_UIM3_UART_DM_MISR_RESET_ADDR                                                                   (MSS_UIM3_UART_DM_REG_BASE      + 0x00000064)
#define HWIO_MSS_UIM3_UART_DM_MISR_RESET_RMSK                                                                          0x1
#define HWIO_MSS_UIM3_UART_DM_MISR_RESET_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_MISR_RESET_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_MISR_RESET_RESET_BMSK                                                                    0x1
#define HWIO_MSS_UIM3_UART_DM_MISR_RESET_RESET_SHFT                                                                    0x0

#define HWIO_MSS_UIM3_UART_DM_MISR_EXPORT_ADDR                                                                  (MSS_UIM3_UART_DM_REG_BASE      + 0x00000068)
#define HWIO_MSS_UIM3_UART_DM_MISR_EXPORT_RMSK                                                                         0x1
#define HWIO_MSS_UIM3_UART_DM_MISR_EXPORT_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_MISR_EXPORT_ADDR, HWIO_MSS_UIM3_UART_DM_MISR_EXPORT_RMSK)
#define HWIO_MSS_UIM3_UART_DM_MISR_EXPORT_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_MISR_EXPORT_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_MISR_EXPORT_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_MISR_EXPORT_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_MISR_EXPORT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_MISR_EXPORT_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_MISR_EXPORT_IN)
#define HWIO_MSS_UIM3_UART_DM_MISR_EXPORT_EXPORT_BMSK                                                                  0x1
#define HWIO_MSS_UIM3_UART_DM_MISR_EXPORT_EXPORT_SHFT                                                                  0x0

#define HWIO_MSS_UIM3_UART_DM_MISR_VAL_ADDR                                                                     (MSS_UIM3_UART_DM_REG_BASE      + 0x0000006c)
#define HWIO_MSS_UIM3_UART_DM_MISR_VAL_RMSK                                                                          0x3ff
#define HWIO_MSS_UIM3_UART_DM_MISR_VAL_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_MISR_VAL_ADDR, HWIO_MSS_UIM3_UART_DM_MISR_VAL_RMSK)
#define HWIO_MSS_UIM3_UART_DM_MISR_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_MISR_VAL_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_MISR_VAL_VAL_BMSK                                                                      0x3ff
#define HWIO_MSS_UIM3_UART_DM_MISR_VAL_VAL_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_TF_RF_DEPRECATED_ADDR                                                             (MSS_UIM3_UART_DM_REG_BASE      + 0x00000070)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_DEPRECATED_RMSK                                                             0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_RF_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TF_RF_DEPRECATED_ADDR, HWIO_MSS_UIM3_UART_DM_TF_RF_DEPRECATED_RMSK)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TF_RF_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_RF_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_TF_RF_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_TF_RF_DEPRECATED_IN)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_DEPRECATED_UART_DM_TF_RF_DEPRECATED_BMSK                                    0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_RF_DEPRECATED_UART_DM_TF_RF_DEPRECATED_SHFT                                           0x0

#define HWIO_MSS_UIM3_UART_DM_TF_RF_2_DEPRECATED_ADDR                                                           (MSS_UIM3_UART_DM_REG_BASE      + 0x00000074)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_2_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_RF_2_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TF_RF_2_DEPRECATED_ADDR, HWIO_MSS_UIM3_UART_DM_TF_RF_2_DEPRECATED_RMSK)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_2_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TF_RF_2_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_2_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_RF_2_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_2_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_TF_RF_2_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_TF_RF_2_DEPRECATED_IN)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_2_DEPRECATED_UART_DM_TF_RF_2_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_RF_2_DEPRECATED_UART_DM_TF_RF_2_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM3_UART_DM_TF_RF_3_DEPRECATED_ADDR                                                           (MSS_UIM3_UART_DM_REG_BASE      + 0x00000078)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_3_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_RF_3_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TF_RF_3_DEPRECATED_ADDR, HWIO_MSS_UIM3_UART_DM_TF_RF_3_DEPRECATED_RMSK)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_3_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TF_RF_3_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_3_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_RF_3_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_3_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_TF_RF_3_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_TF_RF_3_DEPRECATED_IN)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_3_DEPRECATED_UART_DM_TF_RF_3_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_RF_3_DEPRECATED_UART_DM_TF_RF_3_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM3_UART_DM_TF_RF_4_DEPRECATED_ADDR                                                           (MSS_UIM3_UART_DM_REG_BASE      + 0x0000007c)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_4_DEPRECATED_RMSK                                                           0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_RF_4_DEPRECATED_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TF_RF_4_DEPRECATED_ADDR, HWIO_MSS_UIM3_UART_DM_TF_RF_4_DEPRECATED_RMSK)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_4_DEPRECATED_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TF_RF_4_DEPRECATED_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_4_DEPRECATED_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_RF_4_DEPRECATED_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_4_DEPRECATED_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_TF_RF_4_DEPRECATED_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_TF_RF_4_DEPRECATED_IN)
#define HWIO_MSS_UIM3_UART_DM_TF_RF_4_DEPRECATED_UART_DM_TF_RF_4_DEPRECATED_BMSK                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_RF_4_DEPRECATED_UART_DM_TF_RF_4_DEPRECATED_SHFT                                       0x0

#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_ADDR                                                                      (MSS_UIM3_UART_DM_REG_BASE      + 0x00000080)
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_RMSK                                                                         0x3ffef
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_SIM_CFG_ADDR, HWIO_MSS_UIM3_UART_DM_SIM_CFG_RMSK)
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_SIM_CFG_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_SIM_CFG_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_SIM_CFG_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_SIM_CFG_IN)
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_UIM_TX_MODE_BMSK                                                             0x20000
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_UIM_TX_MODE_SHFT                                                                0x11
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_UIM_RX_MODE_BMSK                                                             0x10000
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_UIM_RX_MODE_SHFT                                                                0x10
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_BMSK                                                         0xff00
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_SHFT                                                            0x8
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_SIM_CLK_ON_BMSK                                                                 0x80
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_SIM_CLK_ON_SHFT                                                                  0x7
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_SIM_CLK_TD8_SEL_BMSK                                                            0x40
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_SIM_CLK_TD8_SEL_SHFT                                                             0x6
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_SIM_CLK_STOP_HIGH_BMSK                                                          0x20
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_SIM_CLK_STOP_HIGH_SHFT                                                           0x5
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_MASK_RX_BMSK                                                                     0x8
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_MASK_RX_SHFT                                                                     0x3
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_SWAP_D_BMSK                                                                      0x4
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_SWAP_D_SHFT                                                                      0x2
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_INV_D_BMSK                                                                       0x2
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_INV_D_SHFT                                                                       0x1
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_SIM_SEL_BMSK                                                                     0x1
#define HWIO_MSS_UIM3_UART_DM_SIM_CFG_SIM_SEL_SHFT                                                                     0x0

#define HWIO_MSS_UIM3_UART_DM_TEST_WR_ADDR_ADDR                                                                 (MSS_UIM3_UART_DM_REG_BASE      + 0x00000084)
#define HWIO_MSS_UIM3_UART_DM_TEST_WR_ADDR_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TEST_WR_ADDR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TEST_WR_ADDR_ADDR, HWIO_MSS_UIM3_UART_DM_TEST_WR_ADDR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_TEST_WR_ADDR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TEST_WR_ADDR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_TEST_WR_ADDR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TEST_WR_ADDR_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TEST_WR_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_TEST_WR_ADDR_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_TEST_WR_ADDR_IN)
#define HWIO_MSS_UIM3_UART_DM_TEST_WR_ADDR_TEST_WR_ADDR_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TEST_WR_ADDR_TEST_WR_ADDR_SHFT                                                           0x0

#define HWIO_MSS_UIM3_UART_DM_TEST_WR_DATA_ADDR                                                                 (MSS_UIM3_UART_DM_REG_BASE      + 0x00000088)
#define HWIO_MSS_UIM3_UART_DM_TEST_WR_DATA_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TEST_WR_DATA_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TEST_WR_DATA_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TEST_WR_DATA_TEST_WR_DATA_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TEST_WR_DATA_TEST_WR_DATA_SHFT                                                           0x0

#define HWIO_MSS_UIM3_UART_DM_TEST_RD_ADDR_ADDR                                                                 (MSS_UIM3_UART_DM_REG_BASE      + 0x0000008c)
#define HWIO_MSS_UIM3_UART_DM_TEST_RD_ADDR_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TEST_RD_ADDR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TEST_RD_ADDR_ADDR, HWIO_MSS_UIM3_UART_DM_TEST_RD_ADDR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_TEST_RD_ADDR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TEST_RD_ADDR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_TEST_RD_ADDR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TEST_RD_ADDR_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TEST_RD_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_TEST_RD_ADDR_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_TEST_RD_ADDR_IN)
#define HWIO_MSS_UIM3_UART_DM_TEST_RD_ADDR_TEST_RD_ADDR_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TEST_RD_ADDR_TEST_RD_ADDR_SHFT                                                           0x0

#define HWIO_MSS_UIM3_UART_DM_TEST_RD_DATA_ADDR                                                                 (MSS_UIM3_UART_DM_REG_BASE      + 0x00000090)
#define HWIO_MSS_UIM3_UART_DM_TEST_RD_DATA_RMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TEST_RD_DATA_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TEST_RD_DATA_ADDR, HWIO_MSS_UIM3_UART_DM_TEST_RD_DATA_RMSK)
#define HWIO_MSS_UIM3_UART_DM_TEST_RD_DATA_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_TEST_RD_DATA_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_TEST_RD_DATA_TEST_RD_DATA_BMSK                                                    0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TEST_RD_DATA_TEST_RD_DATA_SHFT                                                           0x0

#define HWIO_MSS_UIM3_UART_DM_CSR_ADDR                                                                          (MSS_UIM3_UART_DM_REG_BASE      + 0x000000a0)
#define HWIO_MSS_UIM3_UART_DM_CSR_RMSK                                                                                0xff
#define HWIO_MSS_UIM3_UART_DM_CSR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_CSR_ADDR, HWIO_MSS_UIM3_UART_DM_CSR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_CSR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_CSR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_CSR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_CSR_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_CSR_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_CSR_IN)
#define HWIO_MSS_UIM3_UART_DM_CSR_UART_RX_CLK_SEL_BMSK                                                                0xf0
#define HWIO_MSS_UIM3_UART_DM_CSR_UART_RX_CLK_SEL_SHFT                                                                 0x4
#define HWIO_MSS_UIM3_UART_DM_CSR_UART_TX_CLK_SEL_BMSK                                                                 0xf
#define HWIO_MSS_UIM3_UART_DM_CSR_UART_TX_CLK_SEL_SHFT                                                                 0x0

#define HWIO_MSS_UIM3_UART_DM_SR_ADDR                                                                           (MSS_UIM3_UART_DM_REG_BASE      + 0x000000a4)
#define HWIO_MSS_UIM3_UART_DM_SR_RMSK                                                                                0xfff
#define HWIO_MSS_UIM3_UART_DM_SR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_SR_ADDR, HWIO_MSS_UIM3_UART_DM_SR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_SR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_SR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_SR_TRANS_END_TRIGGER_BMSK                                                              0xc00
#define HWIO_MSS_UIM3_UART_DM_SR_TRANS_END_TRIGGER_SHFT                                                                0xa
#define HWIO_MSS_UIM3_UART_DM_SR_TRANS_ACTIVE_BMSK                                                                   0x200
#define HWIO_MSS_UIM3_UART_DM_SR_TRANS_ACTIVE_SHFT                                                                     0x9
#define HWIO_MSS_UIM3_UART_DM_SR_RX_BREAK_START_LAST_BMSK                                                            0x100
#define HWIO_MSS_UIM3_UART_DM_SR_RX_BREAK_START_LAST_SHFT                                                              0x8
#define HWIO_MSS_UIM3_UART_DM_SR_HUNT_CHAR_BMSK                                                                       0x80
#define HWIO_MSS_UIM3_UART_DM_SR_HUNT_CHAR_SHFT                                                                        0x7
#define HWIO_MSS_UIM3_UART_DM_SR_RX_BREAK_BMSK                                                                        0x40
#define HWIO_MSS_UIM3_UART_DM_SR_RX_BREAK_SHFT                                                                         0x6
#define HWIO_MSS_UIM3_UART_DM_SR_PAR_FRAME_ERR_BMSK                                                                   0x20
#define HWIO_MSS_UIM3_UART_DM_SR_PAR_FRAME_ERR_SHFT                                                                    0x5
#define HWIO_MSS_UIM3_UART_DM_SR_UART_OVERRUN_BMSK                                                                    0x10
#define HWIO_MSS_UIM3_UART_DM_SR_UART_OVERRUN_SHFT                                                                     0x4
#define HWIO_MSS_UIM3_UART_DM_SR_TXEMT_BMSK                                                                            0x8
#define HWIO_MSS_UIM3_UART_DM_SR_TXEMT_SHFT                                                                            0x3
#define HWIO_MSS_UIM3_UART_DM_SR_TXRDY_BMSK                                                                            0x4
#define HWIO_MSS_UIM3_UART_DM_SR_TXRDY_SHFT                                                                            0x2
#define HWIO_MSS_UIM3_UART_DM_SR_RXFULL_BMSK                                                                           0x2
#define HWIO_MSS_UIM3_UART_DM_SR_RXFULL_SHFT                                                                           0x1
#define HWIO_MSS_UIM3_UART_DM_SR_RXRDY_BMSK                                                                            0x1
#define HWIO_MSS_UIM3_UART_DM_SR_RXRDY_SHFT                                                                            0x0

#define HWIO_MSS_UIM3_UART_DM_CR_ADDR                                                                           (MSS_UIM3_UART_DM_REG_BASE      + 0x000000a8)
#define HWIO_MSS_UIM3_UART_DM_CR_RMSK                                                                                0xfff
#define HWIO_MSS_UIM3_UART_DM_CR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_CR_ADDR, HWIO_MSS_UIM3_UART_DM_CR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_CR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_CR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_CR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_CR_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_CR_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_CR_IN)
#define HWIO_MSS_UIM3_UART_DM_CR_CHANNEL_COMMAND_MSB_BMSK                                                            0x800
#define HWIO_MSS_UIM3_UART_DM_CR_CHANNEL_COMMAND_MSB_SHFT                                                              0xb
#define HWIO_MSS_UIM3_UART_DM_CR_GENERAL_COMMAND_BMSK                                                                0x700
#define HWIO_MSS_UIM3_UART_DM_CR_GENERAL_COMMAND_SHFT                                                                  0x8
#define HWIO_MSS_UIM3_UART_DM_CR_CHANNEL_COMMAND_LSB_BMSK                                                             0xf0
#define HWIO_MSS_UIM3_UART_DM_CR_CHANNEL_COMMAND_LSB_SHFT                                                              0x4
#define HWIO_MSS_UIM3_UART_DM_CR_UART_TX_DISABLE_BMSK                                                                  0x8
#define HWIO_MSS_UIM3_UART_DM_CR_UART_TX_DISABLE_SHFT                                                                  0x3
#define HWIO_MSS_UIM3_UART_DM_CR_UART_TX_EN_BMSK                                                                       0x4
#define HWIO_MSS_UIM3_UART_DM_CR_UART_TX_EN_SHFT                                                                       0x2
#define HWIO_MSS_UIM3_UART_DM_CR_UART_RX_DISABLE_BMSK                                                                  0x2
#define HWIO_MSS_UIM3_UART_DM_CR_UART_RX_DISABLE_SHFT                                                                  0x1
#define HWIO_MSS_UIM3_UART_DM_CR_UART_RX_EN_BMSK                                                                       0x1
#define HWIO_MSS_UIM3_UART_DM_CR_UART_RX_EN_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_MISR_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x000000ac)
#define HWIO_MSS_UIM3_UART_DM_MISR_RMSK                                                                            0x1ffff
#define HWIO_MSS_UIM3_UART_DM_MISR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_MISR_ADDR, HWIO_MSS_UIM3_UART_DM_MISR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_MISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_MISR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_MISR_UART_MISR_BMSK                                                                  0x1ffff
#define HWIO_MSS_UIM3_UART_DM_MISR_UART_MISR_SHFT                                                                      0x0

#define HWIO_MSS_UIM3_UART_DM_IMR_ADDR                                                                          (MSS_UIM3_UART_DM_REG_BASE      + 0x000000b0)
#define HWIO_MSS_UIM3_UART_DM_IMR_RMSK                                                                             0x1ffff
#define HWIO_MSS_UIM3_UART_DM_IMR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_IMR_ADDR, HWIO_MSS_UIM3_UART_DM_IMR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_IMR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_IMR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_IMR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_IMR_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_IMR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_IMR_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_IMR_IN)
#define HWIO_MSS_UIM3_UART_DM_IMR_WWT_IRQ_BMSK                                                                     0x10000
#define HWIO_MSS_UIM3_UART_DM_IMR_WWT_IRQ_SHFT                                                                        0x10
#define HWIO_MSS_UIM3_UART_DM_IMR_TXCOMP_IRQ_BMSK                                                                   0x8000
#define HWIO_MSS_UIM3_UART_DM_IMR_TXCOMP_IRQ_SHFT                                                                      0xf
#define HWIO_MSS_UIM3_UART_DM_IMR_RX_RD_ERROR_IRQ_BMSK                                                              0x4000
#define HWIO_MSS_UIM3_UART_DM_IMR_RX_RD_ERROR_IRQ_SHFT                                                                 0xe
#define HWIO_MSS_UIM3_UART_DM_IMR_TX_WR_ERROR_IRQ_BMSK                                                              0x2000
#define HWIO_MSS_UIM3_UART_DM_IMR_TX_WR_ERROR_IRQ_SHFT                                                                 0xd
#define HWIO_MSS_UIM3_UART_DM_IMR_PAR_FRAME_ERR_IRQ_BMSK                                                            0x1000
#define HWIO_MSS_UIM3_UART_DM_IMR_PAR_FRAME_ERR_IRQ_SHFT                                                               0xc
#define HWIO_MSS_UIM3_UART_DM_IMR_RXBREAK_END_BMSK                                                                   0x800
#define HWIO_MSS_UIM3_UART_DM_IMR_RXBREAK_END_SHFT                                                                     0xb
#define HWIO_MSS_UIM3_UART_DM_IMR_RXBREAK_START_BMSK                                                                 0x400
#define HWIO_MSS_UIM3_UART_DM_IMR_RXBREAK_START_SHFT                                                                   0xa
#define HWIO_MSS_UIM3_UART_DM_IMR_TX_DONE_BMSK                                                                       0x200
#define HWIO_MSS_UIM3_UART_DM_IMR_TX_DONE_SHFT                                                                         0x9
#define HWIO_MSS_UIM3_UART_DM_IMR_TX_ERROR_BMSK                                                                      0x100
#define HWIO_MSS_UIM3_UART_DM_IMR_TX_ERROR_SHFT                                                                        0x8
#define HWIO_MSS_UIM3_UART_DM_IMR_TX_READY_BMSK                                                                       0x80
#define HWIO_MSS_UIM3_UART_DM_IMR_TX_READY_SHFT                                                                        0x7
#define HWIO_MSS_UIM3_UART_DM_IMR_CURRENT_CTS_BMSK                                                                    0x40
#define HWIO_MSS_UIM3_UART_DM_IMR_CURRENT_CTS_SHFT                                                                     0x6
#define HWIO_MSS_UIM3_UART_DM_IMR_DELTA_CTS_BMSK                                                                      0x20
#define HWIO_MSS_UIM3_UART_DM_IMR_DELTA_CTS_SHFT                                                                       0x5
#define HWIO_MSS_UIM3_UART_DM_IMR_RXLEV_BMSK                                                                          0x10
#define HWIO_MSS_UIM3_UART_DM_IMR_RXLEV_SHFT                                                                           0x4
#define HWIO_MSS_UIM3_UART_DM_IMR_RXSTALE_BMSK                                                                         0x8
#define HWIO_MSS_UIM3_UART_DM_IMR_RXSTALE_SHFT                                                                         0x3
#define HWIO_MSS_UIM3_UART_DM_IMR_RXBREAK_CHANGE_BMSK                                                                  0x4
#define HWIO_MSS_UIM3_UART_DM_IMR_RXBREAK_CHANGE_SHFT                                                                  0x2
#define HWIO_MSS_UIM3_UART_DM_IMR_RXHUNT_BMSK                                                                          0x2
#define HWIO_MSS_UIM3_UART_DM_IMR_RXHUNT_SHFT                                                                          0x1
#define HWIO_MSS_UIM3_UART_DM_IMR_TXLEV_BMSK                                                                           0x1
#define HWIO_MSS_UIM3_UART_DM_IMR_TXLEV_SHFT                                                                           0x0

#define HWIO_MSS_UIM3_UART_DM_ISR_ADDR                                                                          (MSS_UIM3_UART_DM_REG_BASE      + 0x000000b4)
#define HWIO_MSS_UIM3_UART_DM_ISR_RMSK                                                                             0x1ffff
#define HWIO_MSS_UIM3_UART_DM_ISR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_ISR_ADDR, HWIO_MSS_UIM3_UART_DM_ISR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_ISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_ISR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_ISR_WWT_IRQ_BMSK                                                                     0x10000
#define HWIO_MSS_UIM3_UART_DM_ISR_WWT_IRQ_SHFT                                                                        0x10
#define HWIO_MSS_UIM3_UART_DM_ISR_TXCOMP_IRQ_BMSK                                                                   0x8000
#define HWIO_MSS_UIM3_UART_DM_ISR_TXCOMP_IRQ_SHFT                                                                      0xf
#define HWIO_MSS_UIM3_UART_DM_ISR_RX_RD_ERROR_IRQ_BMSK                                                              0x4000
#define HWIO_MSS_UIM3_UART_DM_ISR_RX_RD_ERROR_IRQ_SHFT                                                                 0xe
#define HWIO_MSS_UIM3_UART_DM_ISR_TX_WR_ERROR_IRQ_BMSK                                                              0x2000
#define HWIO_MSS_UIM3_UART_DM_ISR_TX_WR_ERROR_IRQ_SHFT                                                                 0xd
#define HWIO_MSS_UIM3_UART_DM_ISR_PAR_FRAME_ERR_IRQ_BMSK                                                            0x1000
#define HWIO_MSS_UIM3_UART_DM_ISR_PAR_FRAME_ERR_IRQ_SHFT                                                               0xc
#define HWIO_MSS_UIM3_UART_DM_ISR_RXBREAK_END_BMSK                                                                   0x800
#define HWIO_MSS_UIM3_UART_DM_ISR_RXBREAK_END_SHFT                                                                     0xb
#define HWIO_MSS_UIM3_UART_DM_ISR_RXBREAK_START_BMSK                                                                 0x400
#define HWIO_MSS_UIM3_UART_DM_ISR_RXBREAK_START_SHFT                                                                   0xa
#define HWIO_MSS_UIM3_UART_DM_ISR_TX_DONE_BMSK                                                                       0x200
#define HWIO_MSS_UIM3_UART_DM_ISR_TX_DONE_SHFT                                                                         0x9
#define HWIO_MSS_UIM3_UART_DM_ISR_TX_ERROR_BMSK                                                                      0x100
#define HWIO_MSS_UIM3_UART_DM_ISR_TX_ERROR_SHFT                                                                        0x8
#define HWIO_MSS_UIM3_UART_DM_ISR_TX_READY_BMSK                                                                       0x80
#define HWIO_MSS_UIM3_UART_DM_ISR_TX_READY_SHFT                                                                        0x7
#define HWIO_MSS_UIM3_UART_DM_ISR_CURRENT_CTS_BMSK                                                                    0x40
#define HWIO_MSS_UIM3_UART_DM_ISR_CURRENT_CTS_SHFT                                                                     0x6
#define HWIO_MSS_UIM3_UART_DM_ISR_DELTA_CTS_BMSK                                                                      0x20
#define HWIO_MSS_UIM3_UART_DM_ISR_DELTA_CTS_SHFT                                                                       0x5
#define HWIO_MSS_UIM3_UART_DM_ISR_RXLEV_BMSK                                                                          0x10
#define HWIO_MSS_UIM3_UART_DM_ISR_RXLEV_SHFT                                                                           0x4
#define HWIO_MSS_UIM3_UART_DM_ISR_RXSTALE_BMSK                                                                         0x8
#define HWIO_MSS_UIM3_UART_DM_ISR_RXSTALE_SHFT                                                                         0x3
#define HWIO_MSS_UIM3_UART_DM_ISR_RXBREAK_CHANGE_BMSK                                                                  0x4
#define HWIO_MSS_UIM3_UART_DM_ISR_RXBREAK_CHANGE_SHFT                                                                  0x2
#define HWIO_MSS_UIM3_UART_DM_ISR_RXHUNT_BMSK                                                                          0x2
#define HWIO_MSS_UIM3_UART_DM_ISR_RXHUNT_SHFT                                                                          0x1
#define HWIO_MSS_UIM3_UART_DM_ISR_TXLEV_BMSK                                                                           0x1
#define HWIO_MSS_UIM3_UART_DM_ISR_TXLEV_SHFT                                                                           0x0

#define HWIO_MSS_UIM3_UART_DM_IRDA_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x000000b8)
#define HWIO_MSS_UIM3_UART_DM_IRDA_RMSK                                                                               0x1f
#define HWIO_MSS_UIM3_UART_DM_IRDA_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_IRDA_ADDR, HWIO_MSS_UIM3_UART_DM_IRDA_RMSK)
#define HWIO_MSS_UIM3_UART_DM_IRDA_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_IRDA_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_IRDA_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_IRDA_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_IRDA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_IRDA_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_IRDA_IN)
#define HWIO_MSS_UIM3_UART_DM_IRDA_MEDIUM_RATE_EN_BMSK                                                                0x10
#define HWIO_MSS_UIM3_UART_DM_IRDA_MEDIUM_RATE_EN_SHFT                                                                 0x4
#define HWIO_MSS_UIM3_UART_DM_IRDA_IRDA_LOOPBACK_BMSK                                                                  0x8
#define HWIO_MSS_UIM3_UART_DM_IRDA_IRDA_LOOPBACK_SHFT                                                                  0x3
#define HWIO_MSS_UIM3_UART_DM_IRDA_INVERT_IRDA_TX_BMSK                                                                 0x4
#define HWIO_MSS_UIM3_UART_DM_IRDA_INVERT_IRDA_TX_SHFT                                                                 0x2
#define HWIO_MSS_UIM3_UART_DM_IRDA_INVERT_IRDA_RX_BMSK                                                                 0x2
#define HWIO_MSS_UIM3_UART_DM_IRDA_INVERT_IRDA_RX_SHFT                                                                 0x1
#define HWIO_MSS_UIM3_UART_DM_IRDA_IRDA_EN_BMSK                                                                        0x1
#define HWIO_MSS_UIM3_UART_DM_IRDA_IRDA_EN_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_RX_TOTAL_SNAP_ADDR                                                                (MSS_UIM3_UART_DM_REG_BASE      + 0x000000bc)
#define HWIO_MSS_UIM3_UART_DM_RX_TOTAL_SNAP_RMSK                                                                  0xffffff
#define HWIO_MSS_UIM3_UART_DM_RX_TOTAL_SNAP_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RX_TOTAL_SNAP_ADDR, HWIO_MSS_UIM3_UART_DM_RX_TOTAL_SNAP_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RX_TOTAL_SNAP_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RX_TOTAL_SNAP_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RX_TOTAL_SNAP_RX_TOTAL_BYTES_BMSK                                                   0xffffff
#define HWIO_MSS_UIM3_UART_DM_RX_TOTAL_SNAP_RX_TOTAL_BYTES_SHFT                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_WWT_TIMEOUT_ADDR                                                                  (MSS_UIM3_UART_DM_REG_BASE      + 0x000000c0)
#define HWIO_MSS_UIM3_UART_DM_WWT_TIMEOUT_RMSK                                                                   0x1ffffff
#define HWIO_MSS_UIM3_UART_DM_WWT_TIMEOUT_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_WWT_TIMEOUT_ADDR, HWIO_MSS_UIM3_UART_DM_WWT_TIMEOUT_RMSK)
#define HWIO_MSS_UIM3_UART_DM_WWT_TIMEOUT_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_WWT_TIMEOUT_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_WWT_TIMEOUT_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_WWT_TIMEOUT_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_WWT_TIMEOUT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_WWT_TIMEOUT_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_WWT_TIMEOUT_IN)
#define HWIO_MSS_UIM3_UART_DM_WWT_TIMEOUT_WWT_TIMEOUT_BMSK                                                       0x1ffffff
#define HWIO_MSS_UIM3_UART_DM_WWT_TIMEOUT_WWT_TIMEOUT_SHFT                                                             0x0

#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_ADDR                                                                     (MSS_UIM3_UART_DM_REG_BASE      + 0x000000c4)
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_RMSK                                                                       0xf06731
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_CLK_CTRL_ADDR, HWIO_MSS_UIM3_UART_DM_CLK_CTRL_RMSK)
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_CLK_CTRL_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_CLK_CTRL_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_CLK_CTRL_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_CLK_CTRL_IN)
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_UART_IRDA_CLK_CGC_OPEN_BMSK                                                0x800000
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_UART_IRDA_CLK_CGC_OPEN_SHFT                                                    0x17
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_UART_SIM_CLK_CGC_OPEN_BMSK                                                 0x400000
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_UART_SIM_CLK_CGC_OPEN_SHFT                                                     0x16
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_UART_RX_CLK_CGC_OPEN_BMSK                                                  0x200000
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_UART_RX_CLK_CGC_OPEN_SHFT                                                      0x15
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_UART_TX_CLK_CGC_OPEN_BMSK                                                  0x100000
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_UART_TX_CLK_CGC_OPEN_SHFT                                                      0x14
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_AHB_RX_BAM_CLK_CGC_OPEN_BMSK                                                 0x4000
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_AHB_RX_BAM_CLK_CGC_OPEN_SHFT                                                    0xe
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_AHB_TX_BAM_CLK_CGC_OPEN_BMSK                                                 0x2000
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_AHB_TX_BAM_CLK_CGC_OPEN_SHFT                                                    0xd
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_AHB_RX_CLK_CGC_OPEN_BMSK                                                      0x400
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_AHB_RX_CLK_CGC_OPEN_SHFT                                                        0xa
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_AHB_TX_CLK_CGC_OPEN_BMSK                                                      0x200
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_AHB_TX_CLK_CGC_OPEN_SHFT                                                        0x9
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_AHB_WR_CLK_CGC_OPEN_BMSK                                                      0x100
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_AHB_WR_CLK_CGC_OPEN_SHFT                                                        0x8
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_RX_ENABLE_CGC_OPT_BMSK                                                         0x20
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_RX_ENABLE_CGC_OPT_SHFT                                                          0x5
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_TX_ENABLE_CGC_OPT_BMSK                                                         0x10
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_TX_ENABLE_CGC_OPT_SHFT                                                          0x4
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_AHB_CLK_CGC_CLOSE_BMSK                                                          0x1
#define HWIO_MSS_UIM3_UART_DM_CLK_CTRL_AHB_CLK_CGC_CLOSE_SHFT                                                          0x0

#define HWIO_MSS_UIM3_UART_DM_BCR_ADDR                                                                          (MSS_UIM3_UART_DM_REG_BASE      + 0x000000c8)
#define HWIO_MSS_UIM3_UART_DM_BCR_RMSK                                                                                0x37
#define HWIO_MSS_UIM3_UART_DM_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_BCR_ADDR, HWIO_MSS_UIM3_UART_DM_BCR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_BCR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_BCR_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_BCR_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_BCR_IN)
#define HWIO_MSS_UIM3_UART_DM_BCR_RX_DMRX_1BYTE_RES_EN_BMSK                                                           0x20
#define HWIO_MSS_UIM3_UART_DM_BCR_RX_DMRX_1BYTE_RES_EN_SHFT                                                            0x5
#define HWIO_MSS_UIM3_UART_DM_BCR_RX_STALE_IRQ_DMRX_EQUAL_BMSK                                                        0x10
#define HWIO_MSS_UIM3_UART_DM_BCR_RX_STALE_IRQ_DMRX_EQUAL_SHFT                                                         0x4
#define HWIO_MSS_UIM3_UART_DM_BCR_RX_DMRX_LOW_EN_BMSK                                                                  0x4
#define HWIO_MSS_UIM3_UART_DM_BCR_RX_DMRX_LOW_EN_SHFT                                                                  0x2
#define HWIO_MSS_UIM3_UART_DM_BCR_STALE_IRQ_EMPTY_BMSK                                                                 0x2
#define HWIO_MSS_UIM3_UART_DM_BCR_STALE_IRQ_EMPTY_SHFT                                                                 0x1
#define HWIO_MSS_UIM3_UART_DM_BCR_TX_BREAK_DISABLE_BMSK                                                                0x1
#define HWIO_MSS_UIM3_UART_DM_BCR_TX_BREAK_DISABLE_SHFT                                                                0x0

#define HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_ADDR                                                                (MSS_UIM3_UART_DM_REG_BASE      + 0x000000cc)
#define HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_RMSK                                                                       0x7
#define HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_ADDR, HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_IN)
#define HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_RX_DMRX_CYCLIC_EN_BMSK                                                     0x4
#define HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_RX_DMRX_CYCLIC_EN_SHFT                                                     0x2
#define HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_RX_TRANS_AUTO_RE_ACTIVATE_BMSK                                             0x2
#define HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_RX_TRANS_AUTO_RE_ACTIVATE_SHFT                                             0x1
#define HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_RX_STALE_AUTO_RE_EN_BMSK                                                   0x1
#define HWIO_MSS_UIM3_UART_DM_RX_TRANS_CTRL_RX_STALE_AUTO_RE_EN_SHFT                                                   0x0

#define HWIO_MSS_UIM3_UART_DM_DMRX_DBG_ADDR                                                                     (MSS_UIM3_UART_DM_REG_BASE      + 0x000000d0)
#define HWIO_MSS_UIM3_UART_DM_DMRX_DBG_RMSK                                                                      0x1ffffff
#define HWIO_MSS_UIM3_UART_DM_DMRX_DBG_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_DMRX_DBG_ADDR, HWIO_MSS_UIM3_UART_DM_DMRX_DBG_RMSK)
#define HWIO_MSS_UIM3_UART_DM_DMRX_DBG_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_DMRX_DBG_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_DMRX_DBG_UART_DM_DMRX_VAL_BMSK                                                     0x1ffffff
#define HWIO_MSS_UIM3_UART_DM_DMRX_DBG_UART_DM_DMRX_VAL_SHFT                                                           0x0

#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_ADDR                                                                   (MSS_UIM3_UART_DM_REG_BASE      + 0x000000d4)
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_RMSK                                                                   0x37377771
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_FSM_STATUS_ADDR, HWIO_MSS_UIM3_UART_DM_FSM_STATUS_RMSK)
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_FSM_STATUS_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_TX_COMP_FSM_BMSK                                                       0x30000000
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_TX_COMP_FSM_SHFT                                                             0x1c
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_RX_PACK_FSM_BMSK                                                        0x7000000
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_RX_PACK_FSM_SHFT                                                             0x18
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_RX_TRANS_FSM_BMSK                                                        0x300000
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_RX_TRANS_FSM_SHFT                                                            0x14
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_TX_TRANS_FSM_BMSK                                                         0x70000
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_TX_TRANS_FSM_SHFT                                                            0x10
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_RX_PRO_TRANS_END_FSM_BMSK                                                  0x7000
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_RX_PRO_TRANS_END_FSM_SHFT                                                     0xc
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_RX_PRO_ACTIVE_FSM_BMSK                                                      0x700
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_RX_PRO_ACTIVE_FSM_SHFT                                                        0x8
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_TX_CON_TRANS_END_FSM_BMSK                                                    0x70
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_TX_CON_TRANS_END_FSM_SHFT                                                     0x4
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_RX_TRANSFER_ACTIVE_BMSK                                                       0x1
#define HWIO_MSS_UIM3_UART_DM_FSM_STATUS_RX_TRANSFER_ACTIVE_SHFT                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_HW_VERSION_ADDR                                                                   (MSS_UIM3_UART_DM_REG_BASE      + 0x000000d8)
#define HWIO_MSS_UIM3_UART_DM_HW_VERSION_RMSK                                                                   0xffffffff
#define HWIO_MSS_UIM3_UART_DM_HW_VERSION_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_HW_VERSION_ADDR, HWIO_MSS_UIM3_UART_DM_HW_VERSION_RMSK)
#define HWIO_MSS_UIM3_UART_DM_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_HW_VERSION_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_HW_VERSION_HW_VERSION_MAJOR_BMSK                                                  0xf0000000
#define HWIO_MSS_UIM3_UART_DM_HW_VERSION_HW_VERSION_MAJOR_SHFT                                                        0x1c
#define HWIO_MSS_UIM3_UART_DM_HW_VERSION_HW_VERSION_MINOR_BMSK                                                   0xfff0000
#define HWIO_MSS_UIM3_UART_DM_HW_VERSION_HW_VERSION_MINOR_SHFT                                                        0x10
#define HWIO_MSS_UIM3_UART_DM_HW_VERSION_HW_VERSION_STEP_BMSK                                                       0xffff
#define HWIO_MSS_UIM3_UART_DM_HW_VERSION_HW_VERSION_STEP_SHFT                                                          0x0

#define HWIO_MSS_UIM3_UART_DM_GENERICS_ADDR                                                                     (MSS_UIM3_UART_DM_REG_BASE      + 0x000000dc)
#define HWIO_MSS_UIM3_UART_DM_GENERICS_RMSK                                                                           0xff
#define HWIO_MSS_UIM3_UART_DM_GENERICS_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_GENERICS_ADDR, HWIO_MSS_UIM3_UART_DM_GENERICS_RMSK)
#define HWIO_MSS_UIM3_UART_DM_GENERICS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_GENERICS_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_GENERICS_GENERIC_BAM_IFC_BMSK                                                           0x80
#define HWIO_MSS_UIM3_UART_DM_GENERICS_GENERIC_BAM_IFC_SHFT                                                            0x7
#define HWIO_MSS_UIM3_UART_DM_GENERICS_GENERIC_DM_IFC_BMSK                                                            0x40
#define HWIO_MSS_UIM3_UART_DM_GENERICS_GENERIC_DM_IFC_SHFT                                                             0x6
#define HWIO_MSS_UIM3_UART_DM_GENERICS_GENERIC_IRDA_IFC_BMSK                                                          0x20
#define HWIO_MSS_UIM3_UART_DM_GENERICS_GENERIC_IRDA_IFC_SHFT                                                           0x5
#define HWIO_MSS_UIM3_UART_DM_GENERICS_GENERIC_SIM_GLUE_BMSK                                                          0x10
#define HWIO_MSS_UIM3_UART_DM_GENERICS_GENERIC_SIM_GLUE_SHFT                                                           0x4
#define HWIO_MSS_UIM3_UART_DM_GENERICS_GENERIC_RAM_ADDR_WIDTH_BMSK                                                     0xf
#define HWIO_MSS_UIM3_UART_DM_GENERICS_GENERIC_RAM_ADDR_WIDTH_SHFT                                                     0x0

#define HWIO_MSS_UIM3_UART_DM_TF_ADDR                                                                           (MSS_UIM3_UART_DM_REG_BASE      + 0x00000100)
#define HWIO_MSS_UIM3_UART_DM_TF_RMSK                                                                           0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_UART_TF_BMSK                                                                   0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_UART_TF_SHFT                                                                          0x0

#define HWIO_MSS_UIM3_UART_DM_TF_2_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000104)
#define HWIO_MSS_UIM3_UART_DM_TF_2_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_2_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_2_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_2_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_2_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_TF_3_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000108)
#define HWIO_MSS_UIM3_UART_DM_TF_3_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_3_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_3_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_3_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_3_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_TF_4_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x0000010c)
#define HWIO_MSS_UIM3_UART_DM_TF_4_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_4_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_4_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_4_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_4_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_TF_5_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000110)
#define HWIO_MSS_UIM3_UART_DM_TF_5_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_5_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_5_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_5_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_5_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_TF_6_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000114)
#define HWIO_MSS_UIM3_UART_DM_TF_6_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_6_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_6_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_6_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_6_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_TF_7_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000118)
#define HWIO_MSS_UIM3_UART_DM_TF_7_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_7_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_7_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_7_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_7_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_TF_8_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x0000011c)
#define HWIO_MSS_UIM3_UART_DM_TF_8_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_8_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_8_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_8_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_8_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_TF_9_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000120)
#define HWIO_MSS_UIM3_UART_DM_TF_9_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_9_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_9_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_9_UART_TF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_9_UART_TF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_TF_10_ADDR                                                                        (MSS_UIM3_UART_DM_REG_BASE      + 0x00000124)
#define HWIO_MSS_UIM3_UART_DM_TF_10_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_10_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_10_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_10_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_10_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_TF_11_ADDR                                                                        (MSS_UIM3_UART_DM_REG_BASE      + 0x00000128)
#define HWIO_MSS_UIM3_UART_DM_TF_11_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_11_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_11_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_11_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_11_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_TF_12_ADDR                                                                        (MSS_UIM3_UART_DM_REG_BASE      + 0x0000012c)
#define HWIO_MSS_UIM3_UART_DM_TF_12_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_12_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_12_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_12_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_12_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_TF_13_ADDR                                                                        (MSS_UIM3_UART_DM_REG_BASE      + 0x00000130)
#define HWIO_MSS_UIM3_UART_DM_TF_13_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_13_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_13_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_13_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_13_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_TF_14_ADDR                                                                        (MSS_UIM3_UART_DM_REG_BASE      + 0x00000134)
#define HWIO_MSS_UIM3_UART_DM_TF_14_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_14_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_14_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_14_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_14_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_TF_15_ADDR                                                                        (MSS_UIM3_UART_DM_REG_BASE      + 0x00000138)
#define HWIO_MSS_UIM3_UART_DM_TF_15_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_15_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_15_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_15_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_15_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_TF_16_ADDR                                                                        (MSS_UIM3_UART_DM_REG_BASE      + 0x0000013c)
#define HWIO_MSS_UIM3_UART_DM_TF_16_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_16_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_TF_16_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_TF_16_UART_TF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_TF_16_UART_TF_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_RF_ADDR                                                                           (MSS_UIM3_UART_DM_REG_BASE      + 0x00000140)
#define HWIO_MSS_UIM3_UART_DM_RF_RMSK                                                                           0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_ADDR, HWIO_MSS_UIM3_UART_DM_RF_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_UART_RF_BMSK                                                                   0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_UART_RF_SHFT                                                                          0x0

#define HWIO_MSS_UIM3_UART_DM_RF_2_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000144)
#define HWIO_MSS_UIM3_UART_DM_RF_2_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_2_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_2_ADDR, HWIO_MSS_UIM3_UART_DM_RF_2_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_2_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_2_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_2_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_2_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_RF_3_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000148)
#define HWIO_MSS_UIM3_UART_DM_RF_3_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_3_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_3_ADDR, HWIO_MSS_UIM3_UART_DM_RF_3_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_3_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_3_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_3_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_3_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_RF_4_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x0000014c)
#define HWIO_MSS_UIM3_UART_DM_RF_4_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_4_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_4_ADDR, HWIO_MSS_UIM3_UART_DM_RF_4_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_4_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_4_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_4_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_4_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_RF_5_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000150)
#define HWIO_MSS_UIM3_UART_DM_RF_5_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_5_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_5_ADDR, HWIO_MSS_UIM3_UART_DM_RF_5_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_5_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_5_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_5_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_5_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_RF_6_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000154)
#define HWIO_MSS_UIM3_UART_DM_RF_6_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_6_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_6_ADDR, HWIO_MSS_UIM3_UART_DM_RF_6_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_6_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_6_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_6_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_6_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_RF_7_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000158)
#define HWIO_MSS_UIM3_UART_DM_RF_7_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_7_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_7_ADDR, HWIO_MSS_UIM3_UART_DM_RF_7_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_7_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_7_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_7_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_7_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_RF_8_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x0000015c)
#define HWIO_MSS_UIM3_UART_DM_RF_8_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_8_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_8_ADDR, HWIO_MSS_UIM3_UART_DM_RF_8_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_8_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_8_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_8_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_8_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_RF_9_ADDR                                                                         (MSS_UIM3_UART_DM_REG_BASE      + 0x00000160)
#define HWIO_MSS_UIM3_UART_DM_RF_9_RMSK                                                                         0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_9_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_9_ADDR, HWIO_MSS_UIM3_UART_DM_RF_9_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_9_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_9_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_9_UART_RF_BMSK                                                                 0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_9_UART_RF_SHFT                                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_RF_10_ADDR                                                                        (MSS_UIM3_UART_DM_REG_BASE      + 0x00000164)
#define HWIO_MSS_UIM3_UART_DM_RF_10_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_10_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_10_ADDR, HWIO_MSS_UIM3_UART_DM_RF_10_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_10_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_10_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_10_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_10_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_RF_11_ADDR                                                                        (MSS_UIM3_UART_DM_REG_BASE      + 0x00000168)
#define HWIO_MSS_UIM3_UART_DM_RF_11_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_11_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_11_ADDR, HWIO_MSS_UIM3_UART_DM_RF_11_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_11_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_11_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_11_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_11_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_RF_12_ADDR                                                                        (MSS_UIM3_UART_DM_REG_BASE      + 0x0000016c)
#define HWIO_MSS_UIM3_UART_DM_RF_12_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_12_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_12_ADDR, HWIO_MSS_UIM3_UART_DM_RF_12_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_12_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_12_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_12_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_12_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_RF_13_ADDR                                                                        (MSS_UIM3_UART_DM_REG_BASE      + 0x00000170)
#define HWIO_MSS_UIM3_UART_DM_RF_13_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_13_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_13_ADDR, HWIO_MSS_UIM3_UART_DM_RF_13_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_13_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_13_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_13_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_13_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_RF_14_ADDR                                                                        (MSS_UIM3_UART_DM_REG_BASE      + 0x00000174)
#define HWIO_MSS_UIM3_UART_DM_RF_14_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_14_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_14_ADDR, HWIO_MSS_UIM3_UART_DM_RF_14_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_14_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_14_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_14_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_14_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_RF_15_ADDR                                                                        (MSS_UIM3_UART_DM_REG_BASE      + 0x00000178)
#define HWIO_MSS_UIM3_UART_DM_RF_15_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_15_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_15_ADDR, HWIO_MSS_UIM3_UART_DM_RF_15_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_15_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_15_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_15_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_15_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_RF_16_ADDR                                                                        (MSS_UIM3_UART_DM_REG_BASE      + 0x0000017c)
#define HWIO_MSS_UIM3_UART_DM_RF_16_RMSK                                                                        0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_16_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_16_ADDR, HWIO_MSS_UIM3_UART_DM_RF_16_RMSK)
#define HWIO_MSS_UIM3_UART_DM_RF_16_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_RF_16_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_RF_16_UART_RF_BMSK                                                                0xffffffff
#define HWIO_MSS_UIM3_UART_DM_RF_16_UART_RF_SHFT                                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_ADDR                                                                      (MSS_UIM3_UART_DM_REG_BASE      + 0x00000180)
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_RMSK                                                                          0xf77f
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_UIM_CFG_ADDR, HWIO_MSS_UIM3_UART_DM_UIM_CFG_RMSK)
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_UIM_CFG_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_UIM_CFG_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_UIM_CFG_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_UIM_CFG_IN)
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_BATT_ALARM_QUICK_DROP_EN_BMSK                                                 0x8000
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_BATT_ALARM_QUICK_DROP_EN_SHFT                                                    0xf
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_TESTBUS_EN_BMSK                                                               0x4000
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_TESTBUS_EN_SHFT                                                                  0xe
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_SW_RESET_BMSK                                                                 0x2000
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_SW_RESET_SHFT                                                                    0xd
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_MODE18_BMSK                                                                   0x1000
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_MODE18_SHFT                                                                      0xc
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_PMIC_ALARM_EN_BMSK                                                             0x400
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_PMIC_ALARM_EN_SHFT                                                               0xa
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_BATT_ALARM_TRIGGER_EN_BMSK                                                     0x200
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_BATT_ALARM_TRIGGER_EN_SHFT                                                       0x9
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_UIM_RMV_TRIGGER_EN_BMSK                                                        0x100
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_UIM_RMV_TRIGGER_EN_SHFT                                                          0x8
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_UIM_CARD_EVENTS_ENABLE_BMSK                                                     0x40
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_UIM_CARD_EVENTS_ENABLE_SHFT                                                      0x6
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_UIM_PRESENT_POLARITY_BMSK                                                       0x20
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_UIM_PRESENT_POLARITY_SHFT                                                        0x5
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_EVENT_DEBOUNCE_TIME_BMSK                                                        0x1f
#define HWIO_MSS_UIM3_UART_DM_UIM_CFG_EVENT_DEBOUNCE_TIME_SHFT                                                         0x0

#define HWIO_MSS_UIM3_UART_DM_UIM_CMD_ADDR                                                                      (MSS_UIM3_UART_DM_REG_BASE      + 0x00000184)
#define HWIO_MSS_UIM3_UART_DM_UIM_CMD_RMSK                                                                             0x3
#define HWIO_MSS_UIM3_UART_DM_UIM_CMD_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_UIM_CMD_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_UIM_CMD_RECOVER_FROM_HW_DEACTIVATION_BMSK                                                0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_CMD_RECOVER_FROM_HW_DEACTIVATION_SHFT                                                0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_CMD_INITIATE_HW_DEACTIVATION_BMSK                                                    0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_CMD_INITIATE_HW_DEACTIVATION_SHFT                                                    0x0

#define HWIO_MSS_UIM3_UART_DM_UIM_IO_STATUS_ADDR                                                                (MSS_UIM3_UART_DM_REG_BASE      + 0x00000188)
#define HWIO_MSS_UIM3_UART_DM_UIM_IO_STATUS_RMSK                                                                       0x7
#define HWIO_MSS_UIM3_UART_DM_UIM_IO_STATUS_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_UIM_IO_STATUS_ADDR, HWIO_MSS_UIM3_UART_DM_UIM_IO_STATUS_RMSK)
#define HWIO_MSS_UIM3_UART_DM_UIM_IO_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_UIM_IO_STATUS_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_UIM_IO_STATUS_UIM_IO_WRITE_IN_PROGRESS_BMSK                                              0x4
#define HWIO_MSS_UIM3_UART_DM_UIM_IO_STATUS_UIM_IO_WRITE_IN_PROGRESS_SHFT                                              0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_IO_STATUS_UIM_DEACTIVATION_STATUS_BMSK                                               0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_IO_STATUS_UIM_DEACTIVATION_STATUS_SHFT                                               0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_IO_STATUS_CARD_PRESENCE_BMSK                                                         0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_IO_STATUS_CARD_PRESENCE_SHFT                                                         0x0

#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_ADDR                                                                  (MSS_UIM3_UART_DM_REG_BASE      + 0x0000018c)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_RMSK                                                                        0x1f
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_ADDR, HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_UIM_IO_WRITE_DONE_BMSK                                                      0x10
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_UIM_IO_WRITE_DONE_SHFT                                                       0x4
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_HW_SEQUENCE_FINISH_BMSK                                                      0x8
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_HW_SEQUENCE_FINISH_SHFT                                                      0x3
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_BATT_ALARM_BMSK                                                              0x4
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_BATT_ALARM_SHFT                                                              0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_UIM_CARD_INSERTION_BMSK                                                      0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_UIM_CARD_INSERTION_SHFT                                                      0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_UIM_CARD_REMOVAL_BMSK                                                        0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_ISR_UIM_CARD_REMOVAL_SHFT                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_ADDR                                                                 (MSS_UIM3_UART_DM_REG_BASE      + 0x00000190)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_RMSK                                                                       0x1f
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_ADDR, HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_UIM_IO_WRITE_DONE_BMSK                                                     0x10
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_UIM_IO_WRITE_DONE_SHFT                                                      0x4
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_HW_SEQUENCE_FINISH_BMSK                                                     0x8
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_HW_SEQUENCE_FINISH_SHFT                                                     0x3
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_BATT_ALARM_BMSK                                                             0x4
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_BATT_ALARM_SHFT                                                             0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_UIM_CARD_INSERTION_BMSK                                                     0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_UIM_CARD_INSERTION_SHFT                                                     0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_UIM_CARD_REMOVAL_BMSK                                                       0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_MISR_UIM_CARD_REMOVAL_SHFT                                                       0x0

#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_CLR_ADDR                                                                  (MSS_UIM3_UART_DM_REG_BASE      + 0x00000194)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_CLR_RMSK                                                                        0x1f
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_CLR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_UIM_IRQ_CLR_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_CLR_UIM_IO_WRITE_DONE_BMSK                                                      0x10
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_CLR_UIM_IO_WRITE_DONE_SHFT                                                       0x4
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_CLR_HW_SEQUENCE_FINISH_BMSK                                                      0x8
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_CLR_HW_SEQUENCE_FINISH_SHFT                                                      0x3
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_CLR_BATT_ALARM_BMSK                                                              0x4
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_CLR_BATT_ALARM_SHFT                                                              0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_CLR_UIM_CARD_INSERTION_BMSK                                                      0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_CLR_UIM_CARD_INSERTION_SHFT                                                      0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_CLR_UIM_CARD_REMOVAL_BMSK                                                        0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_CLR_UIM_CARD_REMOVAL_SHFT                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_ADDR                                                                  (MSS_UIM3_UART_DM_REG_BASE      + 0x00000198)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_RMSK                                                                        0x1f
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_ADDR, HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_RMSK)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_ADDR, m)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_ADDR,m,v,HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_IN)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_UIM_IO_WRITE_DONE_BMSK                                                      0x10
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_UIM_IO_WRITE_DONE_SHFT                                                       0x4
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_HW_SEQUENCE_FINISH_BMSK                                                      0x8
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_HW_SEQUENCE_FINISH_SHFT                                                      0x3
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_BATT_ALARM_BMSK                                                              0x4
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_BATT_ALARM_SHFT                                                              0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_UIM_CARD_INSERTION_BMSK                                                      0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_UIM_CARD_INSERTION_SHFT                                                      0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_UIM_CARD_REMOVAL_BMSK                                                        0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_UIM_CARD_REMOVAL_SHFT                                                        0x0

#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_SET_ADDR                                                              (MSS_UIM3_UART_DM_REG_BASE      + 0x0000019c)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_SET_RMSK                                                                    0x1f
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_SET_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_SET_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_SET_UIM_IO_WRITE_DONE_BMSK                                                  0x10
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_SET_UIM_IO_WRITE_DONE_SHFT                                                   0x4
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_SET_HW_SEQUENCE_FINISH_BMSK                                                  0x8
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_SET_HW_SEQUENCE_FINISH_SHFT                                                  0x3
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_SET_BATT_ALARM_BMSK                                                          0x4
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_SET_BATT_ALARM_SHFT                                                          0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_INSERTION_BMSK                                                  0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_INSERTION_SHFT                                                  0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_REMOVAL_BMSK                                                    0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_REMOVAL_SHFT                                                    0x0

#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_CLR_ADDR                                                              (MSS_UIM3_UART_DM_REG_BASE      + 0x000001a0)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_CLR_RMSK                                                                    0x1f
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_CLR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_CLR_ADDR,v)
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_CLR_UIM_IO_WRITE_DONE_BMSK                                                  0x10
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_CLR_UIM_IO_WRITE_DONE_SHFT                                                   0x4
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_CLR_HW_SEQUENCE_FINISH_BMSK                                                  0x8
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_CLR_HW_SEQUENCE_FINISH_SHFT                                                  0x3
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_CLR_BATT_ALARM_BMSK                                                          0x4
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_CLR_BATT_ALARM_SHFT                                                          0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_INSERTION_BMSK                                                  0x2
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_INSERTION_SHFT                                                  0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_REMOVAL_BMSK                                                    0x1
#define HWIO_MSS_UIM3_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_REMOVAL_SHFT                                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: MSS_PERPH
 *--------------------------------------------------------------------------*/

#define MSS_PERPH_REG_BASE                                                    (MODEM_TOP_BASE      + 0x00180000)

#define HWIO_MSS_CLAMP_MEM_ADDR                                               (MSS_PERPH_REG_BASE      + 0x00000000)
#define HWIO_MSS_CLAMP_MEM_RMSK                                                     0x1f
#define HWIO_MSS_CLAMP_MEM_IN          \
        in_dword_masked(HWIO_MSS_CLAMP_MEM_ADDR, HWIO_MSS_CLAMP_MEM_RMSK)
#define HWIO_MSS_CLAMP_MEM_INM(m)      \
        in_dword_masked(HWIO_MSS_CLAMP_MEM_ADDR, m)
#define HWIO_MSS_CLAMP_MEM_OUT(v)      \
        out_dword(HWIO_MSS_CLAMP_MEM_ADDR,v)
#define HWIO_MSS_CLAMP_MEM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLAMP_MEM_ADDR,m,v,HWIO_MSS_CLAMP_MEM_IN)
#define HWIO_MSS_CLAMP_MEM_UNCLAMP_ALL_BMSK                                         0x10
#define HWIO_MSS_CLAMP_MEM_UNCLAMP_ALL_SHFT                                          0x4
#define HWIO_MSS_CLAMP_MEM_SPARE_3_TO_1_BMSK                                         0xe
#define HWIO_MSS_CLAMP_MEM_SPARE_3_TO_1_SHFT                                         0x1
#define HWIO_MSS_CLAMP_MEM_CLAMP_MEM_BMSK                                            0x1
#define HWIO_MSS_CLAMP_MEM_CLAMP_MEM_SHFT                                            0x0

#define HWIO_MSS_CLAMP_IO_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00000004)
#define HWIO_MSS_CLAMP_IO_RMSK                                                     0x1ff
#define HWIO_MSS_CLAMP_IO_IN          \
        in_dword_masked(HWIO_MSS_CLAMP_IO_ADDR, HWIO_MSS_CLAMP_IO_RMSK)
#define HWIO_MSS_CLAMP_IO_INM(m)      \
        in_dword_masked(HWIO_MSS_CLAMP_IO_ADDR, m)
#define HWIO_MSS_CLAMP_IO_OUT(v)      \
        out_dword(HWIO_MSS_CLAMP_IO_ADDR,v)
#define HWIO_MSS_CLAMP_IO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLAMP_IO_ADDR,m,v,HWIO_MSS_CLAMP_IO_IN)
#define HWIO_MSS_CLAMP_IO_UNCLAMP_ALL_BMSK                                         0x100
#define HWIO_MSS_CLAMP_IO_UNCLAMP_ALL_SHFT                                           0x8
#define HWIO_MSS_CLAMP_IO_SPARE_7_TO_2_BMSK                                         0xfc
#define HWIO_MSS_CLAMP_IO_SPARE_7_TO_2_SHFT                                          0x2
#define HWIO_MSS_CLAMP_IO_RBCPR_BMSK                                                 0x2
#define HWIO_MSS_CLAMP_IO_RBCPR_SHFT                                                 0x1
#define HWIO_MSS_CLAMP_IO_MODEM_BMSK                                                 0x1
#define HWIO_MSS_CLAMP_IO_MODEM_SHFT                                                 0x0

#define HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00000008)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_RMSK                                               0xf
#define HWIO_MSS_BUS_MAXI2AXI_CFG_IN          \
        in_dword_masked(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR, HWIO_MSS_BUS_MAXI2AXI_CFG_RMSK)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR, m)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR,v)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR,m,v,HWIO_MSS_BUS_MAXI2AXI_CFG_IN)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_NAV_AREQPRIORITY_BMSK                        0xc
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_NAV_AREQPRIORITY_SHFT                        0x2
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_CRYPTO_AREQPRIORITY_BMSK                     0x3
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_CRYPTO_AREQPRIORITY_SHFT                     0x0

#define HWIO_MSS_ATB_ID_ADDR                                                  (MSS_PERPH_REG_BASE      + 0x0000000c)
#define HWIO_MSS_ATB_ID_RMSK                                                        0x7f
#define HWIO_MSS_ATB_ID_IN          \
        in_dword_masked(HWIO_MSS_ATB_ID_ADDR, HWIO_MSS_ATB_ID_RMSK)
#define HWIO_MSS_ATB_ID_INM(m)      \
        in_dword_masked(HWIO_MSS_ATB_ID_ADDR, m)
#define HWIO_MSS_ATB_ID_OUT(v)      \
        out_dword(HWIO_MSS_ATB_ID_ADDR,v)
#define HWIO_MSS_ATB_ID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_ATB_ID_ADDR,m,v,HWIO_MSS_ATB_ID_IN)
#define HWIO_MSS_ATB_ID_ATB_ID_BMSK                                                 0x7f
#define HWIO_MSS_ATB_ID_ATB_ID_SHFT                                                  0x0

#define HWIO_MSS_DBG_BUS_CTL_ADDR                                             (MSS_PERPH_REG_BASE      + 0x00000010)
#define HWIO_MSS_DBG_BUS_CTL_RMSK                                                0x1ffff
#define HWIO_MSS_DBG_BUS_CTL_IN          \
        in_dword_masked(HWIO_MSS_DBG_BUS_CTL_ADDR, HWIO_MSS_DBG_BUS_CTL_RMSK)
#define HWIO_MSS_DBG_BUS_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DBG_BUS_CTL_ADDR, m)
#define HWIO_MSS_DBG_BUS_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DBG_BUS_CTL_ADDR,v)
#define HWIO_MSS_DBG_BUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DBG_BUS_CTL_ADDR,m,v,HWIO_MSS_DBG_BUS_CTL_IN)
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_OFFLINE_NOCSEL_BMSK                    0x1c000
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_OFFLINE_NOCSEL_SHFT                        0xe
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_NAV_CE_SEL_BMSK                         0x3800
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_NAV_CE_SEL_SHFT                            0xb
#define HWIO_MSS_DBG_BUS_CTL_AHB2AXI_NAV_SEL_BMSK                                  0x700
#define HWIO_MSS_DBG_BUS_CTL_AHB2AXI_NAV_SEL_SHFT                                    0x8
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_SEL_BMSK                                       0xc0
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_SEL_SHFT                                        0x6
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_TOP_SEL_BMSK                               0x3c
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_TOP_SEL_SHFT                                0x2
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_GPIO_ATB_SEL_BMSK                               0x3
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_GPIO_ATB_SEL_SHFT                               0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000014)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_RMSK                                          0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_IN          \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR, HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_RMSK)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_INM(m)      \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR, m)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OUT(v)      \
        out_dword(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR,v)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR,m,v,HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_IN)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_EN_BMSK                                       0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_EN_SHFT                                       0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR                               (MSS_PERPH_REG_BASE      + 0x00000018)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_RMSK                                      0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_IN          \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR, HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_RMSK)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR, m)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_STATUS_BMSK                               0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_STATUS_SHFT                               0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_ADDR                                  (MSS_PERPH_REG_BASE      + 0x0000001c)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_RMSK                                         0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_OUT(v)      \
        out_dword(HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_ADDR,v)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_CMD_BMSK                                     0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_CMD_SHFT                                     0x0

#define HWIO_MSS_RELAY_MSG_SHADOW0_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000020)
#define HWIO_MSS_RELAY_MSG_SHADOW0_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW0_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR, HWIO_MSS_RELAY_MSG_SHADOW0_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW0_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW0_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW0_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW0_RELAY_MSG_SHADOW0_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW0_RELAY_MSG_SHADOW0_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW1_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000024)
#define HWIO_MSS_RELAY_MSG_SHADOW1_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW1_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR, HWIO_MSS_RELAY_MSG_SHADOW1_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW1_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW1_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW1_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW1_RELAY_MSG_SHADOW1_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW1_RELAY_MSG_SHADOW1_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW2_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000028)
#define HWIO_MSS_RELAY_MSG_SHADOW2_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW2_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR, HWIO_MSS_RELAY_MSG_SHADOW2_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW2_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW2_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW2_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW2_RELAY_MSG_SHADOW2_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW2_RELAY_MSG_SHADOW2_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW3_ADDR                                       (MSS_PERPH_REG_BASE      + 0x0000002c)
#define HWIO_MSS_RELAY_MSG_SHADOW3_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW3_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR, HWIO_MSS_RELAY_MSG_SHADOW3_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW3_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW3_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW3_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW3_RELAY_MSG_SHADOW3_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW3_RELAY_MSG_SHADOW3_SHFT                            0x0

#define HWIO_MSS_MSA_ADDR                                                     (MSS_PERPH_REG_BASE      + 0x00000030)
#define HWIO_MSS_MSA_RMSK                                                            0x3
#define HWIO_MSS_MSA_IN          \
        in_dword_masked(HWIO_MSS_MSA_ADDR, HWIO_MSS_MSA_RMSK)
#define HWIO_MSS_MSA_INM(m)      \
        in_dword_masked(HWIO_MSS_MSA_ADDR, m)
#define HWIO_MSS_MSA_OUT(v)      \
        out_dword(HWIO_MSS_MSA_ADDR,v)
#define HWIO_MSS_MSA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MSA_ADDR,m,v,HWIO_MSS_MSA_IN)
#define HWIO_MSS_MSA_MBA_OK_BMSK                                                     0x2
#define HWIO_MSS_MSA_MBA_OK_SHFT                                                     0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_BMSK                                                0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_SHFT                                                0x0

#define HWIO_MSS_HW_VERSION_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00000034)
#define HWIO_MSS_HW_VERSION_RMSK                                              0xffffffff
#define HWIO_MSS_HW_VERSION_IN          \
        in_dword_masked(HWIO_MSS_HW_VERSION_ADDR, HWIO_MSS_HW_VERSION_RMSK)
#define HWIO_MSS_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_HW_VERSION_ADDR, m)
#define HWIO_MSS_HW_VERSION_MAJOR_BMSK                                        0xf0000000
#define HWIO_MSS_HW_VERSION_MAJOR_SHFT                                              0x1c
#define HWIO_MSS_HW_VERSION_MINOR_BMSK                                         0xfff0000
#define HWIO_MSS_HW_VERSION_MINOR_SHFT                                              0x10
#define HWIO_MSS_HW_VERSION_STEP_BMSK                                             0xffff
#define HWIO_MSS_HW_VERSION_STEP_SHFT                                                0x0

#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00000038)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_RMSK                                    0x7fffffff
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR, HWIO_MSS_OFFLINE_MEM_SLP_CNTL_RMSK)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR, m)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR,v)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR,m,v,HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IN)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SPARE_30_27_BMSK                        0x78000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SPARE_30_27_SHFT                              0x1b
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_DMEM_SLP_NRET_N_BMSK                0x4000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_DMEM_SLP_NRET_N_SHFT                     0x1a
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_DMEM_SLP_RET_N_BMSK                 0x2000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_DMEM_SLP_RET_N_SHFT                      0x19
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_PMEM_SLP_NRET_N_BMSK                0x1000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_PMEM_SLP_NRET_N_SHFT                     0x18
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_PMEM_SLP_RET_N_BMSK                  0x800000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_PMEM_SLP_RET_N_SHFT                      0x17
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_IURF_SLP_NRET_N_BMSK                 0x400000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_IURF_SLP_NRET_N_SHFT                     0x16
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_IURF_SLP_RET_N_BMSK                  0x200000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_IURF_SLP_RET_N_SHFT                      0x15
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_LMEM_SLP_NRET_N_BMSK                 0x180000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_LMEM_SLP_NRET_N_SHFT                     0x13
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_LMEM_SLP_RET_N_BMSK                   0x40000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_LMEM_SLP_RET_N_SHFT                      0x12
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_DMEM_SLP_NRET_N_BMSK                  0x20000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_DMEM_SLP_NRET_N_SHFT                     0x11
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_DMEM_SLP_RET_N_BMSK                   0x10000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_DMEM_SLP_RET_N_SHFT                      0x10
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_PMEM_SLP_NRET_N_BMSK                   0x8000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_PMEM_SLP_NRET_N_SHFT                      0xf
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_PMEM_SLP_RET_N_BMSK                    0x4000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_PMEM_SLP_RET_N_SHFT                       0xe
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_IURF_SLP_NRET_N_BMSK                   0x2000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_IURF_SLP_NRET_N_SHFT                      0xd
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_IURF_SLP_RET_N_BMSK                    0x1000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_IURF_SLP_RET_N_SHFT                       0xc
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_LMEM_SLP_NRET_N_BMSK                    0xc00
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_LMEM_SLP_NRET_N_SHFT                      0xa
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_LMEM_SLP_RET_N_BMSK                     0x200
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_LMEM_SLP_RET_N_SHFT                       0x9
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_DMEM_SLP_NRET_N_BMSK                    0x100
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_DMEM_SLP_NRET_N_SHFT                      0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_DMEM_SLP_RET_N_BMSK                      0x80
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_DMEM_SLP_RET_N_SHFT                       0x7
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_PMEM_SLP_NRET_N_BMSK                     0x40
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_PMEM_SLP_NRET_N_SHFT                      0x6
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_PMEM_SLP_RET_N_BMSK                      0x20
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_PMEM_SLP_RET_N_SHFT                       0x5
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_IURF_SLP_NRET_N_BMSK                     0x10
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_IURF_SLP_NRET_N_SHFT                      0x4
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_IURF_SLP_RET_N_BMSK                       0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_IURF_SLP_RET_N_SHFT                       0x3
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_LMEM_SLP_NRET_N_BMSK                      0x6
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_LMEM_SLP_NRET_N_SHFT                      0x1
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_LMEM_SLP_RET_N_BMSK                       0x1
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_LMEM_SLP_RET_N_SHFT                       0x0

#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR                                   (MSS_PERPH_REG_BASE      + 0x0000003c)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_RMSK                                   0xffffffff
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR, HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_RMSK)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR, m)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR,v)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR,m,v,HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_IN)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_SPARE_31_9_BMSK                        0xfffffe00
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_SPARE_31_9_SHFT                               0x9
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_DB_CTL_PDMEM_SLP_NRET_N_BMSK                0x100
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_DB_CTL_PDMEM_SLP_NRET_N_SHFT                  0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_DB_CTL_PDMEM_SLP_RET_N_BMSK                  0x80
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_DB_CTL_PDMEM_SLP_RET_N_SHFT                   0x7
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_MEMSS_NRET_N_BMSK                            0x60
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_MEMSS_NRET_N_SHFT                             0x5
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_MEMSS_RET_N_BMSK                             0x10
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_MEMSS_RET_N_SHFT                              0x4
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_NRET_N_BMSK                            0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_NRET_N_SHFT                            0x3
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_RET_N_BMSK                             0x4
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_RET_N_SHFT                             0x2
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CCS0_SLP_NRET_N_BMSK                          0x2
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CCS0_SLP_NRET_N_SHFT                          0x1
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CCS0_SLP_RET_N_BMSK                           0x1
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CCS0_SLP_RET_N_SHFT                           0x0

#define HWIO_MSS_CLOCK_SPDM_MON_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00000040)
#define HWIO_MSS_CLOCK_SPDM_MON_RMSK                                                 0x1
#define HWIO_MSS_CLOCK_SPDM_MON_IN          \
        in_dword_masked(HWIO_MSS_CLOCK_SPDM_MON_ADDR, HWIO_MSS_CLOCK_SPDM_MON_RMSK)
#define HWIO_MSS_CLOCK_SPDM_MON_INM(m)      \
        in_dword_masked(HWIO_MSS_CLOCK_SPDM_MON_ADDR, m)
#define HWIO_MSS_CLOCK_SPDM_MON_OUT(v)      \
        out_dword(HWIO_MSS_CLOCK_SPDM_MON_ADDR,v)
#define HWIO_MSS_CLOCK_SPDM_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLOCK_SPDM_MON_ADDR,m,v,HWIO_MSS_CLOCK_SPDM_MON_IN)
#define HWIO_MSS_CLOCK_SPDM_MON_BUS_MON_CLKEN_BMSK                                   0x1
#define HWIO_MSS_CLOCK_SPDM_MON_BUS_MON_CLKEN_SHFT                                   0x0

#define HWIO_MSS_DEBUG_CLOCK_CTL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00000044)
#define HWIO_MSS_DEBUG_CLOCK_CTL_RMSK                                         0xfc00007f
#define HWIO_MSS_DEBUG_CLOCK_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR, HWIO_MSS_DEBUG_CLOCK_CTL_RMSK)
#define HWIO_MSS_DEBUG_CLOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CLOCK_CTL_IN)
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_RESET_SEL_BMSK                           0xe0000000
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_RESET_SEL_SHFT                                 0x1d
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_BYPASS_SEL_BMSK                          0x1c000000
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_BYPASS_SEL_SHFT                                0x1a
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL7_MUX_SEL_BMSK                            0x40
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL7_MUX_SEL_SHFT                             0x6
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL6_MUX_SEL_BMSK                            0x20
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL6_MUX_SEL_SHFT                             0x5
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL5_MUX_SEL_BMSK                            0x10
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL5_MUX_SEL_SHFT                             0x4
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL4_MUX_SEL_BMSK                             0x8
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL4_MUX_SEL_SHFT                             0x3
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL3_MUX_SEL_BMSK                             0x4
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL3_MUX_SEL_SHFT                             0x2
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL2_MUX_SEL_BMSK                             0x2
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL2_MUX_SEL_SHFT                             0x1
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL1_MUX_SEL_BMSK                             0x1
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL1_MUX_SEL_SHFT                             0x0

#define HWIO_MSS_DEBUG_CTL_ADDR                                               (MSS_PERPH_REG_BASE      + 0x00000048)
#define HWIO_MSS_DEBUG_CTL_RMSK                                                      0x7
#define HWIO_MSS_DEBUG_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, HWIO_MSS_DEBUG_CTL_RMSK)
#define HWIO_MSS_DEBUG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CTL_IN)
#define HWIO_MSS_DEBUG_CTL_DAC_DISABLE_ON_Q6_DBG_BMSK                                0x4
#define HWIO_MSS_DEBUG_CTL_DAC_DISABLE_ON_Q6_DBG_SHFT                                0x2
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_DBG_BMSK                                  0x2
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_DBG_SHFT                                  0x1
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_WDOG_BMSK                                 0x1
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_WDOG_SHFT                                 0x0

#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR                                     (MSS_PERPH_REG_BASE      + 0x0000004c)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_RMSK                                           0xff
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_IN          \
        in_dword_masked(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR, HWIO_MSS_CUSTOM_MEM_ARRSTBYN_RMSK)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_INM(m)      \
        in_dword_masked(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR, m)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_OUT(v)      \
        out_dword(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR,v)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR,m,v,HWIO_MSS_CUSTOM_MEM_ARRSTBYN_IN)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_CTL_BMSK                                       0xff
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_CTL_SHFT                                        0x0

#define HWIO_MSS_OFFLINE_PLL_MODE_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001000)
#define HWIO_MSS_OFFLINE_PLL_MODE_RMSK                                        0xe0ffff0f
#define HWIO_MSS_OFFLINE_PLL_MODE_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_MODE_ADDR, HWIO_MSS_OFFLINE_PLL_MODE_RMSK)
#define HWIO_MSS_OFFLINE_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_MODE_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_MODE_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_MODE_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_MODE_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_MODE_IN)
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_DET_BMSK                           0x80000000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_DET_SHFT                                 0x1f
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                        0x40000000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                              0x1e
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_ACK_LATCH_BMSK                          0x20000000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_ACK_LATCH_SHFT                                0x1d
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK               0x800000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT                   0x17
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_UPDATE_BMSK                               0x400000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_UPDATE_SHFT                                   0x16
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                       0x200000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                           0x15
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                         0x100000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                             0x14
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_BIAS_COUNT_BMSK                            0xfc000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_BIAS_COUNT_SHFT                                0xe
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_COUNT_BMSK                             0x3f00
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_COUNT_SHFT                                0x8
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_PLLTEST_BMSK                                   0x8
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_PLLTEST_SHFT                                   0x3
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_RESET_N_BMSK                                   0x4
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_RESET_N_SHFT                                   0x2
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_BYPASSNL_BMSK                                  0x2
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_BYPASSNL_SHFT                                  0x1
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_OUTCTRL_BMSK                                   0x1
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_OUTCTRL_SHFT                                   0x0

#define HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001004)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_RMSK                                           0xffff
#define HWIO_MSS_OFFLINE_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR, HWIO_MSS_OFFLINE_PLL_L_VAL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_L_VAL_IN)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_PLL_L_BMSK                                     0xffff
#define HWIO_MSS_OFFLINE_PLL_L_VAL_PLL_L_SHFT                                        0x0

#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00001008)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_RMSK                                   0xffffffff
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR, HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_IN)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_PLL_ALPHA_31_0_BMSK                    0xffffffff
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_PLL_ALPHA_31_0_SHFT                           0x0

#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR                                 (MSS_PERPH_REG_BASE      + 0x0000100c)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_RMSK                                       0xff
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR, HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_RMSK)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_IN)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_PLL_ALPHA_39_32_BMSK                       0xff
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_PLL_ALPHA_39_32_SHFT                        0x0

#define HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00001010)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RMSK                                    0xffffffff
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_USER_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_USER_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BIT31_28_BMSK                   0xf0000000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BIT31_28_SHFT                         0x1c
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_SSC_MODE_CONTROL_BMSK                    0x8000000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_SSC_MODE_CONTROL_SHFT                         0x1b
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS26_25_BMSK                   0x6000000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS26_25_SHFT                        0x19
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_ALPHA_EN_BMSK                            0x1000000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_ALPHA_EN_SHFT                                 0x18
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS23_22_BMSK                    0xc00000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS23_22_SHFT                        0x16
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_VCO_SEL_BMSK                              0x300000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_VCO_SEL_SHFT                                  0x14
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS19_15_BMSK                     0xf8000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS19_15_SHFT                         0xf
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                          0x7000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                             0xc
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_POST_DIV_RATIO_BMSK                          0xf00
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_POST_DIV_RATIO_SHFT                            0x8
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUTPUT_INV_BMSK                               0x80
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUTPUT_INV_SHFT                                0x7
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS6_5_BMSK                          0x60
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS6_5_SHFT                           0x5
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                           0x10
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                            0x4
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                           0x8
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                           0x3
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_AUX2_BMSK                            0x4
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_AUX2_SHFT                            0x2
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                             0x2
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                             0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                            0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                            0x0

#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR                                  (MSS_PERPH_REG_BASE      + 0x00001014)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RMSK                                  0xffffffff
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR, HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RMSK)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_USER_CTL_U_IN)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RESERVE_BITS31_12_BMSK                0xfffff000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RESERVE_BITS31_12_SHFT                       0xc
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LATCH_INTERFACE_BMSK                       0x800
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LATCH_INTERFACE_SHFT                         0xb
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                       0x700
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                         0x8
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_DSM_BMSK                                    0x80
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_DSM_SHFT                                     0x7
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_WRITE_STATE_BMSK                            0x40
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_WRITE_STATE_SHFT                             0x6
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_TARGET_CTL_BMSK                             0x38
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_TARGET_CTL_SHFT                              0x3
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LOCKDET_BMSK                                 0x4
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LOCKDET_SHFT                                 0x2
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_FREEZE_PLL_BMSK                              0x2
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_FREEZE_PLL_SHFT                              0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_TOGGLE_DET_BMSK                              0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_TOGGLE_DET_SHFT                              0x0

#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR                                  (MSS_PERPH_REG_BASE      + 0x00001018)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_RMSK                                  0xffffffff
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_SINGLE_DMET_MODE_ENABLE_BMSK          0x80000000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_SINGLE_DMET_MODE_ENABLE_SHFT                0x1f
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_DMET_WINDOW_ENABLE_BMSK               0x40000000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_DMET_WINDOW_ENABLE_SHFT                     0x1e
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_INTER_BMSK          0x3c000000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_INTER_SHFT                0x1a
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_THRESHOLD_BMSK              0x3800000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_THRESHOLD_SHFT                   0x17
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_BMSK                  0x700000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_SHFT                      0x14
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_LOCK_DET_THRESHOLD_BMSK                  0xff000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_LOCK_DET_THRESHOLD_SHFT                      0xc
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_LOCK_DET_SAMPLE_SIZE_BMSK                  0xf00
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_LOCK_DET_SAMPLE_SIZE_SHFT                    0x8
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_RESERVE_BITS7_6_BMSK                        0xc0
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_RESERVE_BITS7_6_SHFT                         0x6
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_REF_CYCLE_BMSK                              0x30
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_REF_CYCLE_SHFT                               0x4
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_KFN_BMSK                                     0xf
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_KFN_SHFT                                     0x0

#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x0000101c)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_RMSK                                    0xffffffff
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_TEST_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_TEST_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_BIAS_GEN_TRIM_BMSK                      0xe0000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_BIAS_GEN_TRIM_SHFT                            0x1d
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DCO_BMSK                                0x10000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DCO_SHFT                                      0x1c
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_PROCESS_CALB_BMSK                        0xc000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_PROCESS_CALB_SHFT                             0x1a
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_PROCESS_CALB_BMSK               0x2000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_PROCESS_CALB_SHFT                    0x19
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_FINE_FCW_BMSK                            0x1f00000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_FINE_FCW_SHFT                                 0x14
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_FINE_FCW_BMSK                     0x80000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_FINE_FCW_SHFT                        0x13
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_COARSE_FCW_BMSK                            0x7e000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_COARSE_FCW_SHFT                                0xd
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_COARSE_BMSK                        0x1000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_COARSE_SHFT                           0xc
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DISABLE_LFSR_BMSK                            0x800
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DISABLE_LFSR_SHFT                              0xb
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DTEST_SEL_BMSK                               0x700
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DTEST_SEL_SHFT                                 0x8
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DTEST_EN_BMSK                                 0x80
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DTEST_EN_SHFT                                  0x7
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_BYP_TESTAMP_BMSK                              0x40
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_BYP_TESTAMP_SHFT                               0x6
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_SEL_BMSK                               0x30
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_SEL_SHFT                                0x4
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_SEL_BMSK                                0xc
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_SEL_SHFT                                0x2
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_EN_BMSK                                 0x2
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_EN_SHFT                                 0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_EN_BMSK                                 0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_EN_SHFT                                 0x0

#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR                                  (MSS_PERPH_REG_BASE      + 0x00001020)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_RMSK                                      0x3fff
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR, HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_RMSK)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_IN)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OVERRIDE_FINE_FCW_MSB_BMSK                0x2000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OVERRIDE_FINE_FCW_MSB_SHFT                   0xd
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DTEST_MODE_SEL_BMSK                       0x1800
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DTEST_MODE_SEL_SHFT                          0xb
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NMO_OSC_SEL_BMSK                           0x600
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NMO_OSC_SEL_SHFT                             0x9
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NMO_EN_BMSK                                0x100
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NMO_EN_SHFT                                  0x8
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_MAG_BMSK                              0xe0
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_MAG_SHFT                               0x5
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_GEN_BMSK                              0x10
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_GEN_SHFT                               0x4
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OSC_BIAS_GND_BMSK                            0x8
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OSC_BIAS_GND_SHFT                            0x3
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                        0x6
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                        0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_CAL_CODE_UPDATE_BMSK                         0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_CAL_CODE_UPDATE_SHFT                         0x0

#define HWIO_MSS_OFFLINE_PLL_STATUS_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001024)
#define HWIO_MSS_OFFLINE_PLL_STATUS_RMSK                                      0xffffffff
#define HWIO_MSS_OFFLINE_PLL_STATUS_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_STATUS_ADDR, HWIO_MSS_OFFLINE_PLL_STATUS_RMSK)
#define HWIO_MSS_OFFLINE_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_STATUS_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_STATUS_STATUS_31_0_BMSK                          0xffffffff
#define HWIO_MSS_OFFLINE_PLL_STATUS_STATUS_31_0_SHFT                                 0x0

#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00001028)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_RMSK                                    0xffffffff
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_FREQ_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_FREQ_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_PLL_FREQ_CONTROL_WORD_BMSK              0xffffffff
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_PLL_FREQ_CONTROL_WORD_SHFT                     0x0

#define HWIO_MSS_BUS_CMD_RCGR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x0000102c)
#define HWIO_MSS_BUS_CMD_RCGR_RMSK                                            0x80000001
#define HWIO_MSS_BUS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, HWIO_MSS_BUS_CMD_RCGR_RMSK)
#define HWIO_MSS_BUS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CMD_RCGR_ADDR,m,v,HWIO_MSS_BUS_CMD_RCGR_IN)
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_BMSK                                   0x80000000
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_SHFT                                         0x1f
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_BMSK                                            0x1
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_SHFT                                            0x0

#define HWIO_MSS_BUS_CFG_RCGR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001030)
#define HWIO_MSS_BUS_CFG_RCGR_RMSK                                                 0x71f
#define HWIO_MSS_BUS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, HWIO_MSS_BUS_CFG_RCGR_RMSK)
#define HWIO_MSS_BUS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CFG_RCGR_ADDR,m,v,HWIO_MSS_BUS_CFG_RCGR_IN)
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_BMSK                                         0x700
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_SHFT                                           0x8
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_BMSK                                          0x1f
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_SHFT                                           0x0

#define HWIO_MSS_UIM_CMD_RCGR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001034)
#define HWIO_MSS_UIM_CMD_RCGR_RMSK                                            0x80000003
#define HWIO_MSS_UIM_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM_CMD_RCGR_ADDR, HWIO_MSS_UIM_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM_CMD_RCGR_IN)
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_OFF_BMSK                                   0x80000000
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_OFF_SHFT                                         0x1f
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_EN_BMSK                                           0x2
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_EN_SHFT                                           0x1
#define HWIO_MSS_UIM_CMD_RCGR_UPDATE_BMSK                                            0x1
#define HWIO_MSS_UIM_CMD_RCGR_UPDATE_SHFT                                            0x0

#define HWIO_MSS_UIM_CFG_RCGR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001038)
#define HWIO_MSS_UIM_CFG_RCGR_RMSK                                                  0x1f
#define HWIO_MSS_UIM_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM_CFG_RCGR_ADDR, HWIO_MSS_UIM_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM_CFG_RCGR_IN)
#define HWIO_MSS_UIM_CFG_RCGR_SRC_DIV_BMSK                                          0x1f
#define HWIO_MSS_UIM_CFG_RCGR_SRC_DIV_SHFT                                           0x0

#define HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x0000103c)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_RMSK                                       0x80000003
#define HWIO_MSS_UIM0_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM0_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM0_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_MSS_UIM0_MND_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_MSS_UIM0_MND_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001040)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_RMSK                                           0x3000
#define HWIO_MSS_UIM0_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM0_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM0_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_MSS_UIM0_MND_CFG_RCGR_MODE_SHFT                                         0xc

#define HWIO_MSS_UIM0_MND_M_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001044)
#define HWIO_MSS_UIM0_MND_M_RMSK                                                  0xffff
#define HWIO_MSS_UIM0_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_M_ADDR, HWIO_MSS_UIM0_MND_M_RMSK)
#define HWIO_MSS_UIM0_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_M_ADDR, m)
#define HWIO_MSS_UIM0_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_M_ADDR,v)
#define HWIO_MSS_UIM0_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_M_ADDR,m,v,HWIO_MSS_UIM0_MND_M_IN)
#define HWIO_MSS_UIM0_MND_M_M_BMSK                                                0xffff
#define HWIO_MSS_UIM0_MND_M_M_SHFT                                                   0x0

#define HWIO_MSS_UIM0_MND_N_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001048)
#define HWIO_MSS_UIM0_MND_N_RMSK                                                  0xffff
#define HWIO_MSS_UIM0_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_N_ADDR, HWIO_MSS_UIM0_MND_N_RMSK)
#define HWIO_MSS_UIM0_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_N_ADDR, m)
#define HWIO_MSS_UIM0_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_N_ADDR,v)
#define HWIO_MSS_UIM0_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_N_ADDR,m,v,HWIO_MSS_UIM0_MND_N_IN)
#define HWIO_MSS_UIM0_MND_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_MSS_UIM0_MND_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_MSS_UIM0_MND_D_ADDR                                              (MSS_PERPH_REG_BASE      + 0x0000104c)
#define HWIO_MSS_UIM0_MND_D_RMSK                                                  0xffff
#define HWIO_MSS_UIM0_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_D_ADDR, HWIO_MSS_UIM0_MND_D_RMSK)
#define HWIO_MSS_UIM0_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_D_ADDR, m)
#define HWIO_MSS_UIM0_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_D_ADDR,v)
#define HWIO_MSS_UIM0_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_D_ADDR,m,v,HWIO_MSS_UIM0_MND_D_IN)
#define HWIO_MSS_UIM0_MND_D_NOT_2D_BMSK                                           0xffff
#define HWIO_MSS_UIM0_MND_D_NOT_2D_SHFT                                              0x0

#define HWIO_MSS_UIM0_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00001050)
#define HWIO_MSS_UIM0_BCR_RMSK                                                       0x1
#define HWIO_MSS_UIM0_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_BCR_ADDR, HWIO_MSS_UIM0_BCR_RMSK)
#define HWIO_MSS_UIM0_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_BCR_ADDR, m)
#define HWIO_MSS_UIM0_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_BCR_ADDR,v)
#define HWIO_MSS_UIM0_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_BCR_ADDR,m,v,HWIO_MSS_UIM0_BCR_IN)
#define HWIO_MSS_UIM0_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_UIM0_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001054)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM0_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM0_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001058)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_UART_BIT_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM0_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM0_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_BUS_UIM0_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x0000105c)
#define HWIO_MSS_BUS_UIM0_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM0_CBCR_ADDR, HWIO_MSS_BUS_UIM0_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM0_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM0_CBCR_IN)
#define HWIO_MSS_BUS_UIM0_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_UIM0_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_UIM0_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_UIM0_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_UIM0_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_UIM0_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_UIM0_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_UIM0_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001060)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_RMSK                                       0x80000003
#define HWIO_MSS_UIM1_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM1_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM1_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_MSS_UIM1_MND_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_MSS_UIM1_MND_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001064)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_RMSK                                           0x3000
#define HWIO_MSS_UIM1_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM1_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM1_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_MSS_UIM1_MND_CFG_RCGR_MODE_SHFT                                         0xc

#define HWIO_MSS_UIM1_MND_M_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001068)
#define HWIO_MSS_UIM1_MND_M_RMSK                                                  0xffff
#define HWIO_MSS_UIM1_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_M_ADDR, HWIO_MSS_UIM1_MND_M_RMSK)
#define HWIO_MSS_UIM1_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_M_ADDR, m)
#define HWIO_MSS_UIM1_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_M_ADDR,v)
#define HWIO_MSS_UIM1_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_M_ADDR,m,v,HWIO_MSS_UIM1_MND_M_IN)
#define HWIO_MSS_UIM1_MND_M_M_BMSK                                                0xffff
#define HWIO_MSS_UIM1_MND_M_M_SHFT                                                   0x0

#define HWIO_MSS_UIM1_MND_N_ADDR                                              (MSS_PERPH_REG_BASE      + 0x0000106c)
#define HWIO_MSS_UIM1_MND_N_RMSK                                                  0xffff
#define HWIO_MSS_UIM1_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_N_ADDR, HWIO_MSS_UIM1_MND_N_RMSK)
#define HWIO_MSS_UIM1_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_N_ADDR, m)
#define HWIO_MSS_UIM1_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_N_ADDR,v)
#define HWIO_MSS_UIM1_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_N_ADDR,m,v,HWIO_MSS_UIM1_MND_N_IN)
#define HWIO_MSS_UIM1_MND_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_MSS_UIM1_MND_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_MSS_UIM1_MND_D_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001070)
#define HWIO_MSS_UIM1_MND_D_RMSK                                                  0xffff
#define HWIO_MSS_UIM1_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_D_ADDR, HWIO_MSS_UIM1_MND_D_RMSK)
#define HWIO_MSS_UIM1_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_D_ADDR, m)
#define HWIO_MSS_UIM1_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_D_ADDR,v)
#define HWIO_MSS_UIM1_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_D_ADDR,m,v,HWIO_MSS_UIM1_MND_D_IN)
#define HWIO_MSS_UIM1_MND_D_NOT_2D_BMSK                                           0xffff
#define HWIO_MSS_UIM1_MND_D_NOT_2D_SHFT                                              0x0

#define HWIO_MSS_UIM1_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00001074)
#define HWIO_MSS_UIM1_BCR_RMSK                                                       0x1
#define HWIO_MSS_UIM1_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_BCR_ADDR, HWIO_MSS_UIM1_BCR_RMSK)
#define HWIO_MSS_UIM1_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_BCR_ADDR, m)
#define HWIO_MSS_UIM1_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_BCR_ADDR,v)
#define HWIO_MSS_UIM1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_BCR_ADDR,m,v,HWIO_MSS_UIM1_BCR_IN)
#define HWIO_MSS_UIM1_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_UIM1_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001078)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM1_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM1_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x0000107c)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_UART_BIT_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM1_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM1_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_BUS_UIM1_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00001080)
#define HWIO_MSS_BUS_UIM1_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM1_CBCR_ADDR, HWIO_MSS_BUS_UIM1_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM1_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM1_CBCR_IN)
#define HWIO_MSS_BUS_UIM1_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_UIM1_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_UIM1_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_UIM1_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_UIM1_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_UIM1_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_UIM1_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_UIM1_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001084)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_RMSK                                       0x80000003
#define HWIO_MSS_UIM2_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM2_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM2_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_MSS_UIM2_MND_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_MSS_UIM2_MND_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001088)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_RMSK                                           0x3000
#define HWIO_MSS_UIM2_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM2_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM2_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_MSS_UIM2_MND_CFG_RCGR_MODE_SHFT                                         0xc

#define HWIO_MSS_UIM2_MND_M_ADDR                                              (MSS_PERPH_REG_BASE      + 0x0000108c)
#define HWIO_MSS_UIM2_MND_M_RMSK                                                  0xffff
#define HWIO_MSS_UIM2_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_M_ADDR, HWIO_MSS_UIM2_MND_M_RMSK)
#define HWIO_MSS_UIM2_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_M_ADDR, m)
#define HWIO_MSS_UIM2_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_M_ADDR,v)
#define HWIO_MSS_UIM2_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_M_ADDR,m,v,HWIO_MSS_UIM2_MND_M_IN)
#define HWIO_MSS_UIM2_MND_M_M_BMSK                                                0xffff
#define HWIO_MSS_UIM2_MND_M_M_SHFT                                                   0x0

#define HWIO_MSS_UIM2_MND_N_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001090)
#define HWIO_MSS_UIM2_MND_N_RMSK                                                  0xffff
#define HWIO_MSS_UIM2_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_N_ADDR, HWIO_MSS_UIM2_MND_N_RMSK)
#define HWIO_MSS_UIM2_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_N_ADDR, m)
#define HWIO_MSS_UIM2_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_N_ADDR,v)
#define HWIO_MSS_UIM2_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_N_ADDR,m,v,HWIO_MSS_UIM2_MND_N_IN)
#define HWIO_MSS_UIM2_MND_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_MSS_UIM2_MND_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_MSS_UIM2_MND_D_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001094)
#define HWIO_MSS_UIM2_MND_D_RMSK                                                  0xffff
#define HWIO_MSS_UIM2_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_D_ADDR, HWIO_MSS_UIM2_MND_D_RMSK)
#define HWIO_MSS_UIM2_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_D_ADDR, m)
#define HWIO_MSS_UIM2_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_D_ADDR,v)
#define HWIO_MSS_UIM2_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_D_ADDR,m,v,HWIO_MSS_UIM2_MND_D_IN)
#define HWIO_MSS_UIM2_MND_D_NOT_2D_BMSK                                           0xffff
#define HWIO_MSS_UIM2_MND_D_NOT_2D_SHFT                                              0x0

#define HWIO_MSS_UIM2_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00001098)
#define HWIO_MSS_UIM2_BCR_RMSK                                                       0x1
#define HWIO_MSS_UIM2_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_BCR_ADDR, HWIO_MSS_UIM2_BCR_RMSK)
#define HWIO_MSS_UIM2_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_BCR_ADDR, m)
#define HWIO_MSS_UIM2_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_BCR_ADDR,v)
#define HWIO_MSS_UIM2_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_BCR_ADDR,m,v,HWIO_MSS_UIM2_BCR_IN)
#define HWIO_MSS_UIM2_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_UIM2_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x0000109c)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM2_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM2_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x000010a0)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_UART_BIT_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM2_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM2_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_BUS_UIM2_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x000010a4)
#define HWIO_MSS_BUS_UIM2_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM2_CBCR_ADDR, HWIO_MSS_BUS_UIM2_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM2_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM2_CBCR_IN)
#define HWIO_MSS_BUS_UIM2_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_UIM2_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_UIM2_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_UIM2_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_UIM2_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_UIM2_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_UIM2_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_UIM2_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010a8)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_RMSK                                       0x80000003
#define HWIO_MSS_UIM3_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM3_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM3_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_MSS_UIM3_MND_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_MSS_UIM3_MND_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010ac)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_RMSK                                           0x3000
#define HWIO_MSS_UIM3_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM3_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM3_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_MSS_UIM3_MND_CFG_RCGR_MODE_SHFT                                         0xc

#define HWIO_MSS_UIM3_MND_M_ADDR                                              (MSS_PERPH_REG_BASE      + 0x000010b0)
#define HWIO_MSS_UIM3_MND_M_RMSK                                                  0xffff
#define HWIO_MSS_UIM3_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_M_ADDR, HWIO_MSS_UIM3_MND_M_RMSK)
#define HWIO_MSS_UIM3_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_M_ADDR, m)
#define HWIO_MSS_UIM3_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_M_ADDR,v)
#define HWIO_MSS_UIM3_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_M_ADDR,m,v,HWIO_MSS_UIM3_MND_M_IN)
#define HWIO_MSS_UIM3_MND_M_M_BMSK                                                0xffff
#define HWIO_MSS_UIM3_MND_M_M_SHFT                                                   0x0

#define HWIO_MSS_UIM3_MND_N_ADDR                                              (MSS_PERPH_REG_BASE      + 0x000010b4)
#define HWIO_MSS_UIM3_MND_N_RMSK                                                  0xffff
#define HWIO_MSS_UIM3_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_N_ADDR, HWIO_MSS_UIM3_MND_N_RMSK)
#define HWIO_MSS_UIM3_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_N_ADDR, m)
#define HWIO_MSS_UIM3_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_N_ADDR,v)
#define HWIO_MSS_UIM3_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_N_ADDR,m,v,HWIO_MSS_UIM3_MND_N_IN)
#define HWIO_MSS_UIM3_MND_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_MSS_UIM3_MND_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_MSS_UIM3_MND_D_ADDR                                              (MSS_PERPH_REG_BASE      + 0x000010b8)
#define HWIO_MSS_UIM3_MND_D_RMSK                                                  0xffff
#define HWIO_MSS_UIM3_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_D_ADDR, HWIO_MSS_UIM3_MND_D_RMSK)
#define HWIO_MSS_UIM3_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_D_ADDR, m)
#define HWIO_MSS_UIM3_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_D_ADDR,v)
#define HWIO_MSS_UIM3_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_D_ADDR,m,v,HWIO_MSS_UIM3_MND_D_IN)
#define HWIO_MSS_UIM3_MND_D_NOT_2D_BMSK                                           0xffff
#define HWIO_MSS_UIM3_MND_D_NOT_2D_SHFT                                              0x0

#define HWIO_MSS_UIM3_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x000010bc)
#define HWIO_MSS_UIM3_BCR_RMSK                                                       0x1
#define HWIO_MSS_UIM3_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_BCR_ADDR, HWIO_MSS_UIM3_BCR_RMSK)
#define HWIO_MSS_UIM3_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_BCR_ADDR, m)
#define HWIO_MSS_UIM3_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_BCR_ADDR,v)
#define HWIO_MSS_UIM3_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_BCR_ADDR,m,v,HWIO_MSS_UIM3_BCR_IN)
#define HWIO_MSS_UIM3_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_UIM3_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x000010c0)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM3_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM3_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x000010c4)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_UART_BIT_UIM3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM3_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM3_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_BUS_UIM3_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x000010c8)
#define HWIO_MSS_BUS_UIM3_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_UIM3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM3_CBCR_ADDR, HWIO_MSS_BUS_UIM3_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM3_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM3_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM3_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM3_CBCR_IN)
#define HWIO_MSS_BUS_UIM3_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_UIM3_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_UIM3_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_UIM3_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_UIM3_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_UIM3_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_UIM3_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_UIM3_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_MODEM_BCR_ADDR                                               (MSS_PERPH_REG_BASE      + 0x000010cc)
#define HWIO_MSS_MODEM_BCR_RMSK                                                      0x1
#define HWIO_MSS_MODEM_BCR_IN          \
        in_dword_masked(HWIO_MSS_MODEM_BCR_ADDR, HWIO_MSS_MODEM_BCR_RMSK)
#define HWIO_MSS_MODEM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MODEM_BCR_ADDR, m)
#define HWIO_MSS_MODEM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_MODEM_BCR_ADDR,v)
#define HWIO_MSS_MODEM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MODEM_BCR_ADDR,m,v,HWIO_MSS_MODEM_BCR_IN)
#define HWIO_MSS_MODEM_BCR_BLK_ARES_BMSK                                             0x1
#define HWIO_MSS_MODEM_BCR_BLK_ARES_SHFT                                             0x0

#define HWIO_MSS_AXI_MODEM_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x000010d0)
#define HWIO_MSS_AXI_MODEM_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_AXI_MODEM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_MODEM_CBCR_ADDR, HWIO_MSS_AXI_MODEM_CBCR_RMSK)
#define HWIO_MSS_AXI_MODEM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_MODEM_CBCR_ADDR, m)
#define HWIO_MSS_AXI_MODEM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_MODEM_CBCR_ADDR,v)
#define HWIO_MSS_AXI_MODEM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_MODEM_CBCR_ADDR,m,v,HWIO_MSS_AXI_MODEM_CBCR_IN)
#define HWIO_MSS_AXI_MODEM_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_AXI_MODEM_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_AXI_MODEM_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_AXI_MODEM_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x000010d4)
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_RMSK                                   0x80000001
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_MODEM_OFFLINE_CBCR_ADDR, HWIO_MSS_XO_MODEM_OFFLINE_CBCR_RMSK)
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_MODEM_OFFLINE_CBCR_ADDR, m)
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_MODEM_OFFLINE_CBCR_ADDR,v)
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_MODEM_OFFLINE_CBCR_ADDR,m,v,HWIO_MSS_XO_MODEM_OFFLINE_CBCR_IN)
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_CLKOFF_BMSK                            0x80000000
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_CLKOFF_SHFT                                  0x1f
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_CLKEN_BMSK                                    0x1
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_CLKEN_SHFT                                    0x0

#define HWIO_MSS_Q6SS_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x000010d8)
#define HWIO_MSS_Q6SS_BCR_RMSK                                                       0x1
#define HWIO_MSS_Q6SS_BCR_IN          \
        in_dword_masked(HWIO_MSS_Q6SS_BCR_ADDR, HWIO_MSS_Q6SS_BCR_RMSK)
#define HWIO_MSS_Q6SS_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6SS_BCR_ADDR, m)
#define HWIO_MSS_Q6SS_BCR_OUT(v)      \
        out_dword(HWIO_MSS_Q6SS_BCR_ADDR,v)
#define HWIO_MSS_Q6SS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6SS_BCR_ADDR,m,v,HWIO_MSS_Q6SS_BCR_IN)
#define HWIO_MSS_Q6SS_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_Q6SS_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_BUS_Q6SS_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x000010dc)
#define HWIO_MSS_BUS_Q6SS_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_BUS_Q6SS_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_Q6SS_CBCR_ADDR, HWIO_MSS_BUS_Q6SS_CBCR_RMSK)
#define HWIO_MSS_BUS_Q6SS_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_Q6SS_CBCR_ADDR, m)
#define HWIO_MSS_BUS_Q6SS_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_Q6SS_CBCR_ADDR,v)
#define HWIO_MSS_BUS_Q6SS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_Q6SS_CBCR_ADDR,m,v,HWIO_MSS_BUS_Q6SS_CBCR_IN)
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_NAV_BCR_ADDR                                                 (MSS_PERPH_REG_BASE      + 0x000010e0)
#define HWIO_MSS_NAV_BCR_RMSK                                                        0x1
#define HWIO_MSS_NAV_BCR_IN          \
        in_dword_masked(HWIO_MSS_NAV_BCR_ADDR, HWIO_MSS_NAV_BCR_RMSK)
#define HWIO_MSS_NAV_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_NAV_BCR_ADDR, m)
#define HWIO_MSS_NAV_BCR_OUT(v)      \
        out_dword(HWIO_MSS_NAV_BCR_ADDR,v)
#define HWIO_MSS_NAV_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_NAV_BCR_ADDR,m,v,HWIO_MSS_NAV_BCR_IN)
#define HWIO_MSS_NAV_BCR_BLK_ARES_BMSK                                               0x1
#define HWIO_MSS_NAV_BCR_BLK_ARES_SHFT                                               0x0

#define HWIO_MSS_BUS_NAV_CBCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x000010e4)
#define HWIO_MSS_BUS_NAV_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_BUS_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_NAV_CBCR_ADDR, HWIO_MSS_BUS_NAV_CBCR_RMSK)
#define HWIO_MSS_BUS_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_NAV_CBCR_ADDR, m)
#define HWIO_MSS_BUS_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_NAV_CBCR_ADDR,v)
#define HWIO_MSS_BUS_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_NAV_CBCR_ADDR,m,v,HWIO_MSS_BUS_NAV_CBCR_IN)
#define HWIO_MSS_BUS_NAV_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_BUS_NAV_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_BUS_NAV_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_BUS_NAV_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_XO_NAV_CBCR_ADDR                                             (MSS_PERPH_REG_BASE      + 0x000010e8)
#define HWIO_MSS_XO_NAV_CBCR_RMSK                                             0x80000001
#define HWIO_MSS_XO_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_NAV_CBCR_ADDR, HWIO_MSS_XO_NAV_CBCR_RMSK)
#define HWIO_MSS_XO_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_NAV_CBCR_ADDR, m)
#define HWIO_MSS_XO_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_NAV_CBCR_ADDR,v)
#define HWIO_MSS_XO_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_NAV_CBCR_ADDR,m,v,HWIO_MSS_XO_NAV_CBCR_IN)
#define HWIO_MSS_XO_NAV_CBCR_CLKOFF_BMSK                                      0x80000000
#define HWIO_MSS_XO_NAV_CBCR_CLKOFF_SHFT                                            0x1f
#define HWIO_MSS_XO_NAV_CBCR_CLKEN_BMSK                                              0x1
#define HWIO_MSS_XO_NAV_CBCR_CLKEN_SHFT                                              0x0

#define HWIO_MSS_CE_NAV_BRIDGE_BCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010ec)
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_RMSK                                              0x1
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_IN          \
        in_dword_masked(HWIO_MSS_CE_NAV_BRIDGE_BCR_ADDR, HWIO_MSS_CE_NAV_BRIDGE_BCR_RMSK)
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CE_NAV_BRIDGE_BCR_ADDR, m)
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_OUT(v)      \
        out_dword(HWIO_MSS_CE_NAV_BRIDGE_BCR_ADDR,v)
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CE_NAV_BRIDGE_BCR_ADDR,m,v,HWIO_MSS_CE_NAV_BRIDGE_BCR_IN)
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_BLK_ARES_BMSK                                     0x1
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_BLK_ARES_SHFT                                     0x0

#define HWIO_MSS_AXI_CE_NAV_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x000010f0)
#define HWIO_MSS_AXI_CE_NAV_CBCR_RMSK                                         0x80000001
#define HWIO_MSS_AXI_CE_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_CE_NAV_CBCR_ADDR, HWIO_MSS_AXI_CE_NAV_CBCR_RMSK)
#define HWIO_MSS_AXI_CE_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_CE_NAV_CBCR_ADDR, m)
#define HWIO_MSS_AXI_CE_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_CE_NAV_CBCR_ADDR,v)
#define HWIO_MSS_AXI_CE_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_CE_NAV_CBCR_ADDR,m,v,HWIO_MSS_AXI_CE_NAV_CBCR_IN)
#define HWIO_MSS_AXI_CE_NAV_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_AXI_CE_NAV_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_AXI_CE_NAV_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_AXI_CE_NAV_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_ADDR                                  (MSS_PERPH_REG_BASE      + 0x000010f4)
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_RMSK                                  0x80000001
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_ADDR, HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_RMSK)
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_ADDR, m)
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_ADDR,v)
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_ADDR,m,v,HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_IN)
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_CLKOFF_BMSK                           0x80000000
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_CLKOFF_SHFT                                 0x1f
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_CLKEN_BMSK                                   0x1
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_CLKEN_SHFT                                   0x0

#define HWIO_MSS_AXI_VMIDMT_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x000010f8)
#define HWIO_MSS_AXI_VMIDMT_CBCR_RMSK                                         0x80000001
#define HWIO_MSS_AXI_VMIDMT_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_VMIDMT_CBCR_ADDR, HWIO_MSS_AXI_VMIDMT_CBCR_RMSK)
#define HWIO_MSS_AXI_VMIDMT_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_VMIDMT_CBCR_ADDR, m)
#define HWIO_MSS_AXI_VMIDMT_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_VMIDMT_CBCR_ADDR,v)
#define HWIO_MSS_AXI_VMIDMT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_VMIDMT_CBCR_ADDR,m,v,HWIO_MSS_AXI_VMIDMT_CBCR_IN)
#define HWIO_MSS_AXI_VMIDMT_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_AXI_VMIDMT_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_AXI_VMIDMT_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_AXI_VMIDMT_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_CRYPTO_BCR_ADDR                                              (MSS_PERPH_REG_BASE      + 0x000010fc)
#define HWIO_MSS_CRYPTO_BCR_RMSK                                                     0x1
#define HWIO_MSS_CRYPTO_BCR_IN          \
        in_dword_masked(HWIO_MSS_CRYPTO_BCR_ADDR, HWIO_MSS_CRYPTO_BCR_RMSK)
#define HWIO_MSS_CRYPTO_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CRYPTO_BCR_ADDR, m)
#define HWIO_MSS_CRYPTO_BCR_OUT(v)      \
        out_dword(HWIO_MSS_CRYPTO_BCR_ADDR,v)
#define HWIO_MSS_CRYPTO_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CRYPTO_BCR_ADDR,m,v,HWIO_MSS_CRYPTO_BCR_IN)
#define HWIO_MSS_CRYPTO_BCR_BLK_ARES_BMSK                                            0x1
#define HWIO_MSS_CRYPTO_BCR_BLK_ARES_SHFT                                            0x0

#define HWIO_MSS_BUS_CRYPTO_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001100)
#define HWIO_MSS_BUS_CRYPTO_CBCR_RMSK                                         0x80000001
#define HWIO_MSS_BUS_CRYPTO_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, HWIO_MSS_BUS_CRYPTO_CBCR_RMSK)
#define HWIO_MSS_BUS_CRYPTO_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, m)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,v)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,m,v,HWIO_MSS_BUS_CRYPTO_CBCR_IN)
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00001104)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_RMSK                                   0x80000003
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR, HWIO_MSS_BIT_COXM_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_CMD_RCGR_IN)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_OFF_BMSK                          0x80000000
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_OFF_SHFT                                0x1f
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_EN_BMSK                                  0x2
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_EN_SHFT                                  0x1
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_UPDATE_BMSK                                   0x1
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_UPDATE_SHFT                                   0x0

#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00001108)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_RMSK                                       0x371f
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR, HWIO_MSS_BIT_COXM_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_CFG_RCGR_IN)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_MODE_BMSK                                  0x3000
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_MODE_SHFT                                     0xc
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_SEL_BMSK                                0x700
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_SEL_SHFT                                  0x8
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_DIV_BMSK                                 0x1f
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_DIV_SHFT                                  0x0

#define HWIO_MSS_BIT_COXM_MND_M_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000110c)
#define HWIO_MSS_BIT_COXM_MND_M_RMSK                                              0xffff
#define HWIO_MSS_BIT_COXM_MND_M_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_M_ADDR, HWIO_MSS_BIT_COXM_MND_M_RMSK)
#define HWIO_MSS_BIT_COXM_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_M_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_M_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_M_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_M_IN)
#define HWIO_MSS_BIT_COXM_MND_M_M_BMSK                                            0xffff
#define HWIO_MSS_BIT_COXM_MND_M_M_SHFT                                               0x0

#define HWIO_MSS_BIT_COXM_MND_N_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001110)
#define HWIO_MSS_BIT_COXM_MND_N_RMSK                                              0xffff
#define HWIO_MSS_BIT_COXM_MND_N_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_N_ADDR, HWIO_MSS_BIT_COXM_MND_N_RMSK)
#define HWIO_MSS_BIT_COXM_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_N_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_N_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_N_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_N_IN)
#define HWIO_MSS_BIT_COXM_MND_N_NOT_N_MINUS_M_BMSK                                0xffff
#define HWIO_MSS_BIT_COXM_MND_N_NOT_N_MINUS_M_SHFT                                   0x0

#define HWIO_MSS_BIT_COXM_MND_D_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001114)
#define HWIO_MSS_BIT_COXM_MND_D_RMSK                                              0xffff
#define HWIO_MSS_BIT_COXM_MND_D_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_D_ADDR, HWIO_MSS_BIT_COXM_MND_D_RMSK)
#define HWIO_MSS_BIT_COXM_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_D_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_D_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_D_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_D_IN)
#define HWIO_MSS_BIT_COXM_MND_D_NOT_2D_BMSK                                       0xffff
#define HWIO_MSS_BIT_COXM_MND_D_NOT_2D_SHFT                                          0x0

#define HWIO_MSS_COXM_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00001118)
#define HWIO_MSS_COXM_BCR_RMSK                                                       0x1
#define HWIO_MSS_COXM_BCR_IN          \
        in_dword_masked(HWIO_MSS_COXM_BCR_ADDR, HWIO_MSS_COXM_BCR_RMSK)
#define HWIO_MSS_COXM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_COXM_BCR_ADDR, m)
#define HWIO_MSS_COXM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_COXM_BCR_ADDR,v)
#define HWIO_MSS_COXM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_COXM_BCR_ADDR,m,v,HWIO_MSS_COXM_BCR_IN)
#define HWIO_MSS_COXM_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_COXM_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_BUS_COXM_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x0000111c)
#define HWIO_MSS_BUS_COXM_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_COXM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_COXM_CBCR_ADDR, HWIO_MSS_BUS_COXM_CBCR_RMSK)
#define HWIO_MSS_BUS_COXM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_COXM_CBCR_ADDR, m)
#define HWIO_MSS_BUS_COXM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_COXM_CBCR_ADDR,v)
#define HWIO_MSS_BUS_COXM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_COXM_CBCR_ADDR,m,v,HWIO_MSS_BUS_COXM_CBCR_IN)
#define HWIO_MSS_BUS_COXM_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_COXM_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_COXM_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_COXM_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_COXM_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_COXM_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_COXM_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_COXM_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_BIT_COXM_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00001120)
#define HWIO_MSS_BIT_COXM_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_BIT_COXM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_CBCR_ADDR, HWIO_MSS_BIT_COXM_CBCR_RMSK)
#define HWIO_MSS_BIT_COXM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_CBCR_ADDR, m)
#define HWIO_MSS_BIT_COXM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_CBCR_ADDR,v)
#define HWIO_MSS_BIT_COXM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_CBCR_ADDR,m,v,HWIO_MSS_BIT_COXM_CBCR_IN)
#define HWIO_MSS_BIT_COXM_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BIT_COXM_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BIT_COXM_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BIT_COXM_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001124)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_RMSK                                      0x80000003
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR, HWIO_MSS_RBCPR_REF_CMD_RCGR_RMSK)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR, m)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR,v)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR,m,v,HWIO_MSS_RBCPR_REF_CMD_RCGR_IN)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_OFF_BMSK                             0x80000000
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_OFF_SHFT                                   0x1f
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_EN_BMSK                                     0x2
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_EN_SHFT                                     0x1
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_UPDATE_BMSK                                      0x1
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_UPDATE_SHFT                                      0x0

#define HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001128)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_RMSK                                           0x71f
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR, HWIO_MSS_RBCPR_REF_CFG_RCGR_RMSK)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR, m)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR,v)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR,m,v,HWIO_MSS_RBCPR_REF_CFG_RCGR_IN)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_SEL_BMSK                                   0x700
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_SEL_SHFT                                     0x8
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_DIV_BMSK                                    0x1f
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_DIV_SHFT                                     0x0

#define HWIO_MSS_RBCPR_BCR_ADDR                                               (MSS_PERPH_REG_BASE      + 0x0000112c)
#define HWIO_MSS_RBCPR_BCR_RMSK                                                      0x1
#define HWIO_MSS_RBCPR_BCR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_BCR_ADDR, HWIO_MSS_RBCPR_BCR_RMSK)
#define HWIO_MSS_RBCPR_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_BCR_ADDR, m)
#define HWIO_MSS_RBCPR_BCR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_BCR_ADDR,v)
#define HWIO_MSS_RBCPR_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_BCR_ADDR,m,v,HWIO_MSS_RBCPR_BCR_IN)
#define HWIO_MSS_RBCPR_BCR_BLK_ARES_BMSK                                             0x1
#define HWIO_MSS_RBCPR_BCR_BLK_ARES_SHFT                                             0x0

#define HWIO_MSS_BUS_RBCPR_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001130)
#define HWIO_MSS_BUS_RBCPR_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_BUS_RBCPR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_CBCR_ADDR, HWIO_MSS_BUS_RBCPR_CBCR_RMSK)
#define HWIO_MSS_BUS_RBCPR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_CBCR_ADDR, m)
#define HWIO_MSS_BUS_RBCPR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_RBCPR_CBCR_ADDR,v)
#define HWIO_MSS_BUS_RBCPR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_RBCPR_CBCR_ADDR,m,v,HWIO_MSS_BUS_RBCPR_CBCR_IN)
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_RBCPR_REF_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001134)
#define HWIO_MSS_RBCPR_REF_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_RBCPR_REF_CBCR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CBCR_ADDR, HWIO_MSS_RBCPR_REF_CBCR_RMSK)
#define HWIO_MSS_RBCPR_REF_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CBCR_ADDR, m)
#define HWIO_MSS_RBCPR_REF_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_REF_CBCR_ADDR,v)
#define HWIO_MSS_RBCPR_REF_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_REF_CBCR_ADDR,m,v,HWIO_MSS_RBCPR_REF_CBCR_IN)
#define HWIO_MSS_RBCPR_REF_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_RBCPR_REF_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_RBCPR_REF_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_RBCPR_REF_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_MGPI_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00001138)
#define HWIO_MSS_MGPI_BCR_RMSK                                                       0x1
#define HWIO_MSS_MGPI_BCR_IN          \
        in_dword_masked(HWIO_MSS_MGPI_BCR_ADDR, HWIO_MSS_MGPI_BCR_RMSK)
#define HWIO_MSS_MGPI_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MGPI_BCR_ADDR, m)
#define HWIO_MSS_MGPI_BCR_OUT(v)      \
        out_dword(HWIO_MSS_MGPI_BCR_ADDR,v)
#define HWIO_MSS_MGPI_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MGPI_BCR_ADDR,m,v,HWIO_MSS_MGPI_BCR_IN)
#define HWIO_MSS_MGPI_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_MGPI_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_BUS_MGPI_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x0000113c)
#define HWIO_MSS_BUS_MGPI_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_BUS_MGPI_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MGPI_CBCR_ADDR, HWIO_MSS_BUS_MGPI_CBCR_RMSK)
#define HWIO_MSS_BUS_MGPI_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MGPI_CBCR_ADDR, m)
#define HWIO_MSS_BUS_MGPI_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MGPI_CBCR_ADDR,v)
#define HWIO_MSS_BUS_MGPI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MGPI_CBCR_ADDR,m,v,HWIO_MSS_BUS_MGPI_CBCR_IN)
#define HWIO_MSS_BUS_MGPI_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_MGPI_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_MGPI_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_MGPI_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_BUS_ATB_CBCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001140)
#define HWIO_MSS_BUS_ATB_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_BUS_ATB_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_ATB_CBCR_ADDR, HWIO_MSS_BUS_ATB_CBCR_RMSK)
#define HWIO_MSS_BUS_ATB_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_ATB_CBCR_ADDR, m)
#define HWIO_MSS_BUS_ATB_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_ATB_CBCR_ADDR,v)
#define HWIO_MSS_BUS_ATB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_ATB_CBCR_ADDR,m,v,HWIO_MSS_BUS_ATB_CBCR_IN)
#define HWIO_MSS_BUS_ATB_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_BUS_ATB_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_BUS_ATB_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_BUS_ATB_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR                                  (MSS_PERPH_REG_BASE      + 0x00001144)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_RMSK                                  0x80000001
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR, HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_RMSK)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR, m)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR,v)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR,m,v,HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_IN)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKOFF_BMSK                           0x80000000
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKOFF_SHFT                                 0x1f
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKEN_BMSK                                   0x1
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKEN_SHFT                                   0x0

#define HWIO_MSS_MVC_BCR_ADDR                                                 (MSS_PERPH_REG_BASE      + 0x00001148)
#define HWIO_MSS_MVC_BCR_RMSK                                                        0x1
#define HWIO_MSS_MVC_BCR_IN          \
        in_dword_masked(HWIO_MSS_MVC_BCR_ADDR, HWIO_MSS_MVC_BCR_RMSK)
#define HWIO_MSS_MVC_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MVC_BCR_ADDR, m)
#define HWIO_MSS_MVC_BCR_OUT(v)      \
        out_dword(HWIO_MSS_MVC_BCR_ADDR,v)
#define HWIO_MSS_MVC_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MVC_BCR_ADDR,m,v,HWIO_MSS_MVC_BCR_IN)
#define HWIO_MSS_MVC_BCR_BLK_ARES_BMSK                                               0x1
#define HWIO_MSS_MVC_BCR_BLK_ARES_SHFT                                               0x0

#define HWIO_MSS_BUS_MVC_CBCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x0000114c)
#define HWIO_MSS_BUS_MVC_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_BUS_MVC_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MVC_CBCR_ADDR, HWIO_MSS_BUS_MVC_CBCR_RMSK)
#define HWIO_MSS_BUS_MVC_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MVC_CBCR_ADDR, m)
#define HWIO_MSS_BUS_MVC_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MVC_CBCR_ADDR,v)
#define HWIO_MSS_BUS_MVC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MVC_CBCR_ADDR,m,v,HWIO_MSS_BUS_MVC_CBCR_IN)
#define HWIO_MSS_BUS_MVC_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_BUS_MVC_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_BUS_MVC_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_BUS_MVC_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_STMR_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00001150)
#define HWIO_MSS_STMR_BCR_RMSK                                                       0x1
#define HWIO_MSS_STMR_BCR_IN          \
        in_dword_masked(HWIO_MSS_STMR_BCR_ADDR, HWIO_MSS_STMR_BCR_RMSK)
#define HWIO_MSS_STMR_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_STMR_BCR_ADDR, m)
#define HWIO_MSS_STMR_BCR_OUT(v)      \
        out_dword(HWIO_MSS_STMR_BCR_ADDR,v)
#define HWIO_MSS_STMR_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_STMR_BCR_ADDR,m,v,HWIO_MSS_STMR_BCR_IN)
#define HWIO_MSS_STMR_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_STMR_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_BUS_STMR_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00001154)
#define HWIO_MSS_BUS_STMR_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_BUS_STMR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_STMR_CBCR_ADDR, HWIO_MSS_BUS_STMR_CBCR_RMSK)
#define HWIO_MSS_BUS_STMR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_STMR_CBCR_ADDR, m)
#define HWIO_MSS_BUS_STMR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_STMR_CBCR_ADDR,v)
#define HWIO_MSS_BUS_STMR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_STMR_CBCR_ADDR,m,v,HWIO_MSS_BUS_STMR_CBCR_IN)
#define HWIO_MSS_BUS_STMR_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_STMR_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_STMR_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_STMR_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_XO_STMR_CBCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001158)
#define HWIO_MSS_XO_STMR_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_XO_STMR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_STMR_CBCR_ADDR, HWIO_MSS_XO_STMR_CBCR_RMSK)
#define HWIO_MSS_XO_STMR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_STMR_CBCR_ADDR, m)
#define HWIO_MSS_XO_STMR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_STMR_CBCR_ADDR,v)
#define HWIO_MSS_XO_STMR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_STMR_CBCR_ADDR,m,v,HWIO_MSS_XO_STMR_CBCR_IN)
#define HWIO_MSS_XO_STMR_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_XO_STMR_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_XO_STMR_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_XO_STMR_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_XO_DTR_CBCR_ADDR                                             (MSS_PERPH_REG_BASE      + 0x0000115c)
#define HWIO_MSS_XO_DTR_CBCR_RMSK                                             0x80000001
#define HWIO_MSS_XO_DTR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_DTR_CBCR_ADDR, HWIO_MSS_XO_DTR_CBCR_RMSK)
#define HWIO_MSS_XO_DTR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_DTR_CBCR_ADDR, m)
#define HWIO_MSS_XO_DTR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_DTR_CBCR_ADDR,v)
#define HWIO_MSS_XO_DTR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_DTR_CBCR_ADDR,m,v,HWIO_MSS_XO_DTR_CBCR_IN)
#define HWIO_MSS_XO_DTR_CBCR_CLKOFF_BMSK                                      0x80000000
#define HWIO_MSS_XO_DTR_CBCR_CLKOFF_SHFT                                            0x1f
#define HWIO_MSS_XO_DTR_CBCR_CLKEN_BMSK                                              0x1
#define HWIO_MSS_XO_DTR_CBCR_CLKEN_SHFT                                              0x0

#define HWIO_MSS_SW_FEATURE_FUSES_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001170)
#define HWIO_MSS_SW_FEATURE_FUSES_RMSK                                        0xffffffff
#define HWIO_MSS_SW_FEATURE_FUSES_IN          \
        in_dword_masked(HWIO_MSS_SW_FEATURE_FUSES_ADDR, HWIO_MSS_SW_FEATURE_FUSES_RMSK)
#define HWIO_MSS_SW_FEATURE_FUSES_INM(m)      \
        in_dword_masked(HWIO_MSS_SW_FEATURE_FUSES_ADDR, m)
#define HWIO_MSS_SW_FEATURE_FUSES_MSMC_MDSP_FW_EN_BIT2_BMSK                   0x80000000
#define HWIO_MSS_SW_FEATURE_FUSES_MSMC_MDSP_FW_EN_BIT2_SHFT                         0x1f
#define HWIO_MSS_SW_FEATURE_FUSES_MSMC_MDSP_FW_EN_BIT1_BMSK                   0x40000000
#define HWIO_MSS_SW_FEATURE_FUSES_MSMC_MDSP_FW_EN_BIT1_SHFT                         0x1e
#define HWIO_MSS_SW_FEATURE_FUSES_MSMC_MDSP_FW_EN_BIT0_BMSK                   0x20000000
#define HWIO_MSS_SW_FEATURE_FUSES_MSMC_MDSP_FW_EN_BIT0_SHFT                         0x1d
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_DSDA_BMSK                        0x10000000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_DSDA_SHFT                              0x1c
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_MODEM_BMSK                        0x8000000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_MODEM_SHFT                             0x1b
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_ADC3_BMSK                         0x4000000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_ADC3_SHFT                              0x1a
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_ADC2_BMSK                         0x2000000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_ADC2_SHFT                              0x19
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_UL_CA_BMSK                    0x1000000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_UL_CA_SHFT                         0x18
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_60MHZ_BMSK                     0x800000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_60MHZ_SHFT                         0x17
#define HWIO_MSS_SW_FEATURE_FUSES_FETAURE_EN_LTE_40MHZ_BMSK                     0x400000
#define HWIO_MSS_SW_FEATURE_FUSES_FETAURE_EN_LTE_40MHZ_SHFT                         0x16
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_GERAN_BMSK                         0x300000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_GERAN_SHFT                             0x14
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_TDSCDMA_BMSK                        0xc0000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_TDSCDMA_SHFT                           0x12
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_ABOVE_CAT1_BMSK                 0x30000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_ABOVE_CAT1_SHFT                    0x10
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_ABOVE_CAT2_BMSK                  0xc000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_ABOVE_CAT2_SHFT                     0xe
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_BMSK                             0x3000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_SHFT                                0xc
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_HSDPA_DC_BMSK                         0xc00
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_HSDPA_DC_SHFT                           0xa
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_HSDPA_MIMO_BMSK                       0x300
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_HSDPA_MIMO_SHFT                         0x8
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_HSDPA_BMSK                             0xc0
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_HSDPA_SHFT                              0x6
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_WCDMA_BMSK                             0x30
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_WCDMA_SHFT                              0x4
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_DO_BMSK                                 0xc
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_DO_SHFT                                 0x2
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_1X_BMSK                                 0x3
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_1X_SHFT                                 0x0

#define HWIO_MSS_RESERVE_02_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001174)
#define HWIO_MSS_RESERVE_02_RMSK                                              0xffffffff
#define HWIO_MSS_RESERVE_02_IN          \
        in_dword_masked(HWIO_MSS_RESERVE_02_ADDR, HWIO_MSS_RESERVE_02_RMSK)
#define HWIO_MSS_RESERVE_02_INM(m)      \
        in_dword_masked(HWIO_MSS_RESERVE_02_ADDR, m)
#define HWIO_MSS_RESERVE_02_OUT(v)      \
        out_dword(HWIO_MSS_RESERVE_02_ADDR,v)
#define HWIO_MSS_RESERVE_02_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RESERVE_02_ADDR,m,v,HWIO_MSS_RESERVE_02_IN)
#define HWIO_MSS_RESERVE_02_MSS_RESERVE_02_BMSK                               0xffffffff
#define HWIO_MSS_RESERVE_02_MSS_RESERVE_02_SHFT                                      0x0

/*----------------------------------------------------------------------------
 * MODULE: TLMM_CSR
 *--------------------------------------------------------------------------*/

#define TLMM_CSR_REG_BASE                                                 (TLMM_BASE      + 0x00010000)

#define HWIO_TLMM_GPIO_CFGn_ADDR(n)                                       (TLMM_CSR_REG_BASE      + 0x00001000 + 0x10 * (n))
#define HWIO_TLMM_GPIO_CFGn_RMSK                                               0x7ff
#define HWIO_TLMM_GPIO_CFGn_MAXn                                                  89
#define HWIO_TLMM_GPIO_CFGn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(n), HWIO_TLMM_GPIO_CFGn_RMSK)
#define HWIO_TLMM_GPIO_CFGn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_CFGn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_CFGn_ADDR(n),val)
#define HWIO_TLMM_GPIO_CFGn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFGn_ADDR(n),mask,val,HWIO_TLMM_GPIO_CFGn_INI(n))
#define HWIO_TLMM_GPIO_CFGn_GPIO_HIHYS_EN_BMSK                                 0x400
#define HWIO_TLMM_GPIO_CFGn_GPIO_HIHYS_EN_SHFT                                   0xa
#define HWIO_TLMM_GPIO_CFGn_GPIO_OE_BMSK                                       0x200
#define HWIO_TLMM_GPIO_CFGn_GPIO_OE_SHFT                                         0x9
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_BMSK                                  0x1c0
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_SHFT                                    0x6
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_BMSK                                       0x3c
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_SHFT                                        0x2
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_BMSK                                       0x3
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUTn_ADDR(n)                                    (TLMM_CSR_REG_BASE      + 0x00001004 + 0x10 * (n))
#define HWIO_TLMM_GPIO_IN_OUTn_RMSK                                              0x3
#define HWIO_TLMM_GPIO_IN_OUTn_MAXn                                               89
#define HWIO_TLMM_GPIO_IN_OUTn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n), HWIO_TLMM_GPIO_IN_OUTn_RMSK)
#define HWIO_TLMM_GPIO_IN_OUTn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n),val)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n),mask,val,HWIO_TLMM_GPIO_IN_OUTn_INI(n))
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_BMSK                                     0x2
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_SHFT                                     0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_BMSK                                      0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_SHFT                                      0x0

#define HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n)                                  (TLMM_CSR_REG_BASE      + 0x00001008 + 0x10 * (n))
#define HWIO_TLMM_GPIO_INTR_CFGn_RMSK                                          0x1ff
#define HWIO_TLMM_GPIO_INTR_CFGn_MAXn                                             89
#define HWIO_TLMM_GPIO_INTR_CFGn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n), HWIO_TLMM_GPIO_INTR_CFGn_RMSK)
#define HWIO_TLMM_GPIO_INTR_CFGn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_INTR_CFGn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n),val)
#define HWIO_TLMM_GPIO_INTR_CFGn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n),mask,val,HWIO_TLMM_GPIO_INTR_CFGn_INI(n))
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_BMSK                              0x100
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_SHFT                                0x8
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_BMSK                               0xe0
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_SHFT                                0x5
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_BMSK                        0x10
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_SHFT                         0x4
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_BMSK                              0xc
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_SHFT                              0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_BMSK                               0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_SHFT                               0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_BMSK                                0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_SHFT                                0x0

#define HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n)                               (TLMM_CSR_REG_BASE      + 0x0000100c + 0x10 * (n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_RMSK                                         0x1
#define HWIO_TLMM_GPIO_INTR_STATUSn_MAXn                                          89
#define HWIO_TLMM_GPIO_INTR_STATUSn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n), HWIO_TLMM_GPIO_INTR_STATUSn_RMSK)
#define HWIO_TLMM_GPIO_INTR_STATUSn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_INTR_STATUSn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n),val)
#define HWIO_TLMM_GPIO_INTR_STATUSn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n),mask,val,HWIO_TLMM_GPIO_INTR_STATUSn_INI(n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_INTR_STATUS_BMSK                             0x1
#define HWIO_TLMM_GPIO_INTR_STATUSn_INTR_STATUS_SHFT                             0x0

#define HWIO_TLMM_CLK_GATE_EN_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00002000)
#define HWIO_TLMM_CLK_GATE_EN_RMSK                                               0x7
#define HWIO_TLMM_CLK_GATE_EN_IN          \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_ADDR, HWIO_TLMM_CLK_GATE_EN_RMSK)
#define HWIO_TLMM_CLK_GATE_EN_INM(m)      \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_ADDR, m)
#define HWIO_TLMM_CLK_GATE_EN_OUT(v)      \
        out_dword(HWIO_TLMM_CLK_GATE_EN_ADDR,v)
#define HWIO_TLMM_CLK_GATE_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_CLK_GATE_EN_ADDR,m,v,HWIO_TLMM_CLK_GATE_EN_IN)
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_BMSK                                   0x4
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_SHFT                                   0x2
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_BMSK                               0x2
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_SHFT                               0x1
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_BMSK                                  0x1
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_SHFT                                  0x0

#define HWIO_TLMM_IE_CTRL_DISABLE_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00002004)
#define HWIO_TLMM_IE_CTRL_DISABLE_RMSK                                           0x3
#define HWIO_TLMM_IE_CTRL_DISABLE_IN          \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_ADDR, HWIO_TLMM_IE_CTRL_DISABLE_RMSK)
#define HWIO_TLMM_IE_CTRL_DISABLE_INM(m)      \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_ADDR, m)
#define HWIO_TLMM_IE_CTRL_DISABLE_OUT(v)      \
        out_dword(HWIO_TLMM_IE_CTRL_DISABLE_ADDR,v)
#define HWIO_TLMM_IE_CTRL_DISABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_IE_CTRL_DISABLE_ADDR,m,v,HWIO_TLMM_IE_CTRL_DISABLE_IN)
#define HWIO_TLMM_IE_CTRL_DISABLE_SLIMBUS_GPIO_CORE_IE_CTRL_BMSK                 0x2
#define HWIO_TLMM_IE_CTRL_DISABLE_SLIMBUS_GPIO_CORE_IE_CTRL_SHFT                 0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_BMSK                           0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_SHFT                           0x0

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR                                (TLMM_CSR_REG_BASE      + 0x00002008)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_RMSK                                0x7fffffff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_IN          \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR, HWIO_TLMM_MPM_WAKEUP_INT_EN_0_RMSK)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR, m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OUT(v)      \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR,v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR,m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_0_IN)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_56_BMSK                        0x40000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_56_SHFT                              0x1e
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_55_BMSK                        0x20000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_55_SHFT                              0x1d
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_54_BMSK                        0x10000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_54_SHFT                              0x1c
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_53_BMSK                         0x8000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_53_SHFT                              0x1b
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_51_BMSK                         0x4000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_51_SHFT                              0x1a
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_BMSK                         0x2000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_SHFT                              0x19
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_43_BMSK                         0x1000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_43_SHFT                              0x18
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_25_BMSK                          0x800000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_25_SHFT                              0x17
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_24_BMSK                          0x400000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_24_SHFT                              0x16
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_21_BMSK                          0x200000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_21_SHFT                              0x15
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_20_BMSK                          0x100000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_20_SHFT                              0x14
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_19_BMSK                           0x80000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_19_SHFT                              0x13
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_18_BMSK                           0x40000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_18_SHFT                              0x12
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_17_BMSK                           0x20000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_17_SHFT                              0x11
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_16_BMSK                           0x10000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_16_SHFT                              0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_15_BMSK                            0x8000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_15_SHFT                               0xf
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_14_BMSK                            0x4000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_14_SHFT                               0xe
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_13_BMSK                            0x2000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_13_SHFT                               0xd
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_12_BMSK                            0x1000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_12_SHFT                               0xc
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_11_BMSK                             0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_11_SHFT                               0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_10_BMSK                             0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_10_SHFT                               0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_9_BMSK                              0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_9_SHFT                                0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_8_BMSK                              0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_8_SHFT                                0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_7_BMSK                               0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_7_SHFT                                0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_6_BMSK                               0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_6_SHFT                                0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_BMSK                               0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_SHFT                                0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_4_BMSK                               0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_4_SHFT                                0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_3_BMSK                                0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_3_SHFT                                0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_2_BMSK                                0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_2_SHFT                                0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_1_BMSK                                0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_1_SHFT                                0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_0_BMSK                                0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_0_SHFT                                0x0

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR                                (TLMM_CSR_REG_BASE      + 0x0000200c)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_RMSK                                  0x3fffff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_IN          \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR, HWIO_TLMM_MPM_WAKEUP_INT_EN_1_RMSK)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR, m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_OUT(v)      \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR,v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR,m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_1_IN)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_3_BMSK                      0x200000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_3_SHFT                          0x15
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_1_BMSK                      0x100000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_1_SHFT                          0x14
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SRST_N_BMSK                            0x80000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SRST_N_SHFT                               0x13
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_HSIC_STB_BMSK                          0x40000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_HSIC_STB_SHFT                             0x12
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_85_BMSK                           0x20000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_85_SHFT                              0x11
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_84_BMSK                           0x10000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_84_SHFT                              0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_83_BMSK                            0x8000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_83_SHFT                               0xf
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_82_BMSK                            0x4000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_82_SHFT                               0xe
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_81_BMSK                            0x2000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_81_SHFT                               0xd
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_80_BMSK                            0x1000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_80_SHFT                               0xc
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_77_BMSK                             0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_77_SHFT                               0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_70_BMSK                             0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_70_SHFT                               0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_68_BMSK                             0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_68_SHFT                               0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_67_BMSK                             0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_67_SHFT                               0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_66_BMSK                              0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_66_SHFT                               0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_65_BMSK                              0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_65_SHFT                               0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_64_BMSK                              0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_64_SHFT                               0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_63_BMSK                              0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_63_SHFT                               0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_62_BMSK                               0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_62_SHFT                               0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_60_BMSK                               0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_60_SHFT                               0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_59_BMSK                               0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_59_SHFT                               0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_58_BMSK                               0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_58_SHFT                               0x0

#define HWIO_TLMM_INT_JTAG_CTL_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00002010)
#define HWIO_TLMM_INT_JTAG_CTL_RMSK                                             0x1f
#define HWIO_TLMM_INT_JTAG_CTL_IN          \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR, HWIO_TLMM_INT_JTAG_CTL_RMSK)
#define HWIO_TLMM_INT_JTAG_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR, m)
#define HWIO_TLMM_INT_JTAG_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_INT_JTAG_CTL_ADDR,v)
#define HWIO_TLMM_INT_JTAG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_INT_JTAG_CTL_ADDR,m,v,HWIO_TLMM_INT_JTAG_CTL_IN)
#define HWIO_TLMM_INT_JTAG_CTL_USB3_PHY_TAP_ENA_BMSK                            0x10
#define HWIO_TLMM_INT_JTAG_CTL_USB3_PHY_TAP_ENA_SHFT                             0x4
#define HWIO_TLMM_INT_JTAG_CTL_KPSS_TAP_ENA_BMSK                                 0x8
#define HWIO_TLMM_INT_JTAG_CTL_KPSS_TAP_ENA_SHFT                                 0x3
#define HWIO_TLMM_INT_JTAG_CTL_QDSS_TAP_ENA_BMSK                                 0x4
#define HWIO_TLMM_INT_JTAG_CTL_QDSS_TAP_ENA_SHFT                                 0x2
#define HWIO_TLMM_INT_JTAG_CTL_ACC_TAP_ENA_BMSK                                  0x2
#define HWIO_TLMM_INT_JTAG_CTL_ACC_TAP_ENA_SHFT                                  0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_BMSK                                  0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_SHFT                                  0x0

#define HWIO_TLMM_ETM_MODE_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00002014)
#define HWIO_TLMM_ETM_MODE_RMSK                                                  0x3
#define HWIO_TLMM_ETM_MODE_IN          \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR, HWIO_TLMM_ETM_MODE_RMSK)
#define HWIO_TLMM_ETM_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR, m)
#define HWIO_TLMM_ETM_MODE_OUT(v)      \
        out_dword(HWIO_TLMM_ETM_MODE_ADDR,v)
#define HWIO_TLMM_ETM_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_ETM_MODE_ADDR,m,v,HWIO_TLMM_ETM_MODE_IN)
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_BMSK                                  0x3
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_SHFT                                  0x0

#define HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00002018)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_RMSK                                           0x3
#define HWIO_TLMM_DBG_BUS_OUT_SEL_IN          \
        in_dword_masked(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR, HWIO_TLMM_DBG_BUS_OUT_SEL_RMSK)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_INM(m)      \
        in_dword_masked(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR, m)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OUT(v)      \
        out_dword(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR,v)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR,m,v,HWIO_TLMM_DBG_BUS_OUT_SEL_IN)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_BMSK                                  0x3
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_SHFT                                  0x0

#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR                              (TLMM_CSR_REG_BASE      + 0x0000201c)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_RMSK                                     0x1
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_IN          \
        in_dword_masked(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR, HWIO_TLMM_PERIPH_CHAR_TEST_MODE_RMSK)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR, m)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_OUT(v)      \
        out_dword(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR,v)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR,m,v,HWIO_TLMM_PERIPH_CHAR_TEST_MODE_IN)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_CHAR_TEST_MODE_BMSK                      0x1
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_CHAR_TEST_MODE_SHFT                      0x0

#define HWIO_TLMM_CHIP_MODE_ADDR                                          (TLMM_CSR_REG_BASE      + 0x00002020)
#define HWIO_TLMM_CHIP_MODE_RMSK                                                 0x3
#define HWIO_TLMM_CHIP_MODE_IN          \
        in_dword_masked(HWIO_TLMM_CHIP_MODE_ADDR, HWIO_TLMM_CHIP_MODE_RMSK)
#define HWIO_TLMM_CHIP_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_CHIP_MODE_ADDR, m)
#define HWIO_TLMM_CHIP_MODE_MODE1_PIN_BMSK                                       0x2
#define HWIO_TLMM_CHIP_MODE_MODE1_PIN_SHFT                                       0x1
#define HWIO_TLMM_CHIP_MODE_MODE0_PIN_BMSK                                       0x1
#define HWIO_TLMM_CHIP_MODE_MODE0_PIN_SHFT                                       0x0

#define HWIO_TLMM_HW_REVISION_NUMBER_ADDR                                 (TLMM_CSR_REG_BASE      + 0x00002028)
#define HWIO_TLMM_HW_REVISION_NUMBER_RMSK                                 0xffffffff
#define HWIO_TLMM_HW_REVISION_NUMBER_IN          \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR, HWIO_TLMM_HW_REVISION_NUMBER_RMSK)
#define HWIO_TLMM_HW_REVISION_NUMBER_INM(m)      \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR, m)
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_BMSK                      0xf0000000
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_SHFT                            0x1c
#define HWIO_TLMM_HW_REVISION_NUMBER_PRODUCT_DEVICE_ID_BMSK                0xffff000
#define HWIO_TLMM_HW_REVISION_NUMBER_PRODUCT_DEVICE_ID_SHFT                      0xc
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_BMSK                      0xffe
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_SHFT                        0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_BMSK                              0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_SHFT                              0x0

#define HWIO_TLMM_SLIMBUS_CTL_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00002030)
#define HWIO_TLMM_SLIMBUS_CTL_RMSK                                               0x3
#define HWIO_TLMM_SLIMBUS_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SLIMBUS_CTL_ADDR, HWIO_TLMM_SLIMBUS_CTL_RMSK)
#define HWIO_TLMM_SLIMBUS_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SLIMBUS_CTL_ADDR, m)
#define HWIO_TLMM_SLIMBUS_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SLIMBUS_CTL_ADDR,v)
#define HWIO_TLMM_SLIMBUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SLIMBUS_CTL_ADDR,m,v,HWIO_TLMM_SLIMBUS_CTL_IN)
#define HWIO_TLMM_SLIMBUS_CTL_SR_CTL_EN_BMSK                                     0x3
#define HWIO_TLMM_SLIMBUS_CTL_SR_CTL_EN_SHFT                                     0x0

#define HWIO_TLMM_RFFE_CTL_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00002034)
#define HWIO_TLMM_RFFE_CTL_RMSK                                            0xfffffff
#define HWIO_TLMM_RFFE_CTL_IN          \
        in_dword_masked(HWIO_TLMM_RFFE_CTL_ADDR, HWIO_TLMM_RFFE_CTL_RMSK)
#define HWIO_TLMM_RFFE_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_RFFE_CTL_ADDR, m)
#define HWIO_TLMM_RFFE_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_RFFE_CTL_ADDR,v)
#define HWIO_TLMM_RFFE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_RFFE_CTL_ADDR,m,v,HWIO_TLMM_RFFE_CTL_IN)
#define HWIO_TLMM_RFFE_CTL_I2C_B_BLSP5_DATA_SR_CTL_EN_BMSK                 0xc000000
#define HWIO_TLMM_RFFE_CTL_I2C_B_BLSP5_DATA_SR_CTL_EN_SHFT                      0x1a
#define HWIO_TLMM_RFFE_CTL_I2C_B_BLSP5_CLK_SR_CTL_EN_BMSK                  0x3000000
#define HWIO_TLMM_RFFE_CTL_I2C_B_BLSP5_CLK_SR_CTL_EN_SHFT                       0x18
#define HWIO_TLMM_RFFE_CTL_I2C_A_BLSP3_DATA_SR_CTL_EN_BMSK                  0xc00000
#define HWIO_TLMM_RFFE_CTL_I2C_A_BLSP3_DATA_SR_CTL_EN_SHFT                      0x16
#define HWIO_TLMM_RFFE_CTL_I2C_A_BLSP3_CLK_SR_CTL_EN_BMSK                   0x300000
#define HWIO_TLMM_RFFE_CTL_I2C_A_BLSP3_CLK_SR_CTL_EN_SHFT                       0x14
#define HWIO_TLMM_RFFE_CTL_RFFE5_DATA_SR_CTL_EN_BMSK                         0xc0000
#define HWIO_TLMM_RFFE_CTL_RFFE5_DATA_SR_CTL_EN_SHFT                            0x12
#define HWIO_TLMM_RFFE_CTL_RFFE5_CLK_SR_CTL_EN_BMSK                          0x30000
#define HWIO_TLMM_RFFE_CTL_RFFE5_CLK_SR_CTL_EN_SHFT                             0x10
#define HWIO_TLMM_RFFE_CTL_RFFE4_DATA_SR_CTL_EN_BMSK                          0xc000
#define HWIO_TLMM_RFFE_CTL_RFFE4_DATA_SR_CTL_EN_SHFT                             0xe
#define HWIO_TLMM_RFFE_CTL_RFFE4_CLK_SR_CTL_EN_BMSK                           0x3000
#define HWIO_TLMM_RFFE_CTL_RFFE4_CLK_SR_CTL_EN_SHFT                              0xc
#define HWIO_TLMM_RFFE_CTL_RFFE3_DATA_SR_CTL_EN_BMSK                           0xc00
#define HWIO_TLMM_RFFE_CTL_RFFE3_DATA_SR_CTL_EN_SHFT                             0xa
#define HWIO_TLMM_RFFE_CTL_RFFE3_CLK_SR_CTL_EN_BMSK                            0x300
#define HWIO_TLMM_RFFE_CTL_RFFE3_CLK_SR_CTL_EN_SHFT                              0x8
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SR_CTL_EN_BMSK                            0xc0
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SR_CTL_EN_SHFT                             0x6
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SR_CTL_EN_BMSK                             0x30
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SR_CTL_EN_SHFT                              0x4
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SR_CTL_EN_BMSK                             0xc
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SR_CTL_EN_SHFT                             0x2
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SR_CTL_EN_BMSK                              0x3
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SR_CTL_EN_SHFT                              0x0

#define HWIO_TLMM_RESOUT_HDRV_CTL_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00002038)
#define HWIO_TLMM_RESOUT_HDRV_CTL_RMSK                                           0x7
#define HWIO_TLMM_RESOUT_HDRV_CTL_IN          \
        in_dword_masked(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR, HWIO_TLMM_RESOUT_HDRV_CTL_RMSK)
#define HWIO_TLMM_RESOUT_HDRV_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR, m)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR,v)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR,m,v,HWIO_TLMM_RESOUT_HDRV_CTL_IN)
#define HWIO_TLMM_RESOUT_HDRV_CTL_RESOUT_N_HDRV_BMSK                             0x7
#define HWIO_TLMM_RESOUT_HDRV_CTL_RESOUT_N_HDRV_SHFT                             0x0

#define HWIO_TLMM_JTAG_HDRV_CTL_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0000203c)
#define HWIO_TLMM_JTAG_HDRV_CTL_RMSK                                           0x3ff
#define HWIO_TLMM_JTAG_HDRV_CTL_IN          \
        in_dword_masked(HWIO_TLMM_JTAG_HDRV_CTL_ADDR, HWIO_TLMM_JTAG_HDRV_CTL_RMSK)
#define HWIO_TLMM_JTAG_HDRV_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_JTAG_HDRV_CTL_ADDR, m)
#define HWIO_TLMM_JTAG_HDRV_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_JTAG_HDRV_CTL_ADDR,v)
#define HWIO_TLMM_JTAG_HDRV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_JTAG_HDRV_CTL_ADDR,m,v,HWIO_TLMM_JTAG_HDRV_CTL_IN)
#define HWIO_TLMM_JTAG_HDRV_CTL_TDI_PULL_BMSK                                  0x300
#define HWIO_TLMM_JTAG_HDRV_CTL_TDI_PULL_SHFT                                    0x8
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_PULL_BMSK                                   0xc0
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_PULL_SHFT                                    0x6
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_HDRV_BMSK                                   0x38
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_HDRV_SHFT                                    0x3
#define HWIO_TLMM_JTAG_HDRV_CTL_TDO_HDRV_BMSK                                    0x7
#define HWIO_TLMM_JTAG_HDRV_CTL_TDO_HDRV_SHFT                                    0x0

#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR                                 (TLMM_CSR_REG_BASE      + 0x00002040)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK                                    0xfffff
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR, HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HYS_CTL_BMSK             0x80000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HYS_CTL_SHFT                0x13
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HYS_CTL_BMSK              0x40000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HYS_CTL_SHFT                 0x12
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SEN_EN_BMSK              0x20000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SEN_EN_SHFT                 0x11
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SEN_EN_BMSK               0x10000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SEN_EN_SHFT                  0x10
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_PULL_BMSK                 0xc000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_PULL_SHFT                    0xe
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_PULL_BMSK                  0x3000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_PULL_SHFT                     0xc
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PSHOLD_HDRV_BMSK                          0xe00
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PSHOLD_HDRV_SHFT                            0x9
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_CXO_EN_HDRV_BMSK                          0x1c0
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_CXO_EN_HDRV_SHFT                            0x6
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HDRV_BMSK                   0x38
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HDRV_SHFT                    0x3
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HDRV_BMSK                     0x7
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HDRV_SHFT                     0x0

#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR                                 (TLMM_CSR_REG_BASE      + 0x00002044)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK                                     0x7fff
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR, HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_PULL_BMSK                       0x6000
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_PULL_SHFT                          0xd
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_PULL_BMSK                       0x1800
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_PULL_SHFT                          0xb
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_PULL_BMSK                       0x600
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_PULL_SHFT                         0x9
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_HDRV_BMSK                        0x1c0
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_HDRV_SHFT                          0x6
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_HDRV_BMSK                         0x38
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_HDRV_SHFT                          0x3
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_HDRV_BMSK                         0x7
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_HDRV_SHFT                         0x0

#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR                                 (TLMM_CSR_REG_BASE      + 0x00002048)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_RMSK                                     0xffff
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR, HWIO_TLMM_SDC2_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_SDC2_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_PULL_BMSK                       0xc000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_PULL_SHFT                          0xe
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_BMSK                        0x2000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_SHFT                           0xd
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_PULL_BMSK                       0x1800
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_PULL_SHFT                          0xb
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_PULL_BMSK                       0x600
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_PULL_SHFT                         0x9
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_HDRV_BMSK                        0x1c0
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_HDRV_SHFT                          0x6
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_HDRV_BMSK                         0x38
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_HDRV_SHFT                          0x3
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_HDRV_BMSK                         0x7
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_HDRV_SHFT                         0x0

#define HWIO_TLMM_MODE_PULL_CTL_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0000204c)
#define HWIO_TLMM_MODE_PULL_CTL_RMSK                                             0xf
#define HWIO_TLMM_MODE_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_MODE_PULL_CTL_ADDR, HWIO_TLMM_MODE_PULL_CTL_RMSK)
#define HWIO_TLMM_MODE_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_MODE_PULL_CTL_ADDR, m)
#define HWIO_TLMM_MODE_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_MODE_PULL_CTL_ADDR,v)
#define HWIO_TLMM_MODE_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MODE_PULL_CTL_ADDR,m,v,HWIO_TLMM_MODE_PULL_CTL_IN)
#define HWIO_TLMM_MODE_PULL_CTL_MODE_1_PULL_BMSK                                 0xc
#define HWIO_TLMM_MODE_PULL_CTL_MODE_1_PULL_SHFT                                 0x2
#define HWIO_TLMM_MODE_PULL_CTL_MODE_0_PULL_BMSK                                 0x3
#define HWIO_TLMM_MODE_PULL_CTL_MODE_0_PULL_SHFT                                 0x0

#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR                           (TLMM_CSR_REG_BASE      + 0x00002050)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_RMSK                           0x1fffffff
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR, HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_RMSK)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR, m)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR,v)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR,m,v,HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_IN)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ROUT_BMSK                      0x1c000000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ROUT_SHFT                            0x1a
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_CORE_CTL_EN_BMSK                0x2000000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_CORE_CTL_EN_SHFT                     0x19
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_VM_SHIFT_EN_BMSK                0x1000000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_VM_SHIFT_EN_SHFT                     0x18
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ODT_EN_BMSK                      0x800000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ODT_EN_SHFT                          0x17
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ODT_BMSK                         0x600000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ODT_SHFT                             0x15
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_DCC_BMSK                         0x1c0000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_DCC_SHFT                             0x12
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NRXDEL_BMSK                       0x30000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NRXDEL_SHFT                          0x10
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PRXDEL_BMSK                        0xc000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PRXDEL_SHFT                           0xe
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NSLEW_BMSK                         0x3000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NSLEW_SHFT                            0xc
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PSLEW_BMSK                          0xc00
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PSLEW_SHFT                            0xa
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NCNT_BMSK                           0x3e0
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NCNT_SHFT                             0x5
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PCNT_BMSK                            0x1f
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PCNT_SHFT                             0x0

#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR                             (TLMM_CSR_REG_BASE      + 0x00002054)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_RMSK                             0x1fffffff
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR, HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_RMSK)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR, m)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR,v)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR,m,v,HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_IN)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ROUT_BMSK                        0x1c000000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ROUT_SHFT                              0x1a
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_CORE_CTL_EN_BMSK                  0x2000000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_CORE_CTL_EN_SHFT                       0x19
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_VM_SHIFT_EN_BMSK                  0x1000000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_VM_SHIFT_EN_SHFT                       0x18
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ODT_EN_BMSK                        0x800000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ODT_EN_SHFT                            0x17
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ODT_BMSK                           0x600000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ODT_SHFT                               0x15
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_DCC_BMSK                           0x1c0000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_DCC_SHFT                               0x12
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NRXDEL_BMSK                         0x30000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NRXDEL_SHFT                            0x10
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PRXDEL_BMSK                          0xc000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PRXDEL_SHFT                             0xe
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NSLEW_BMSK                           0x3000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NSLEW_SHFT                              0xc
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PSLEW_BMSK                            0xc00
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PSLEW_SHFT                              0xa
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NCNT_BMSK                             0x3e0
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NCNT_SHFT                               0x5
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PCNT_BMSK                              0x1f
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PCNT_SHFT                               0x0

#define HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR                                   (TLMM_CSR_REG_BASE      + 0x00002058)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_RMSK                                          0x3
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_IN          \
        in_dword_masked(HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR, HWIO_TLMM_HSIC_CAL_PAD_CTL_RMSK)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR, m)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR,v)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR,m,v,HWIO_TLMM_HSIC_CAL_PAD_CTL_IN)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_IMP_SEL_BMSK                                  0x3
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_IMP_SEL_SHFT                                  0x0

#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_ADDR                                 (TLMM_CSR_REG_BASE      + 0x0000205c)
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_RMSK                                 0xffffffff
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_EBI2_HDRV_PULL_CTL_ADDR, HWIO_TLMM_EBI2_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_EBI2_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_EBI2_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_EBI2_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_EBI2_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_BUSY_N_PULL_BMSK                0xc0000000
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_BUSY_N_PULL_SHFT                      0x1e
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_WE_N_PULL_BMSK                  0x30000000
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_WE_N_PULL_SHFT                        0x1c
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_CS0_N_PULL_BMSK                  0xc000000
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_CS0_N_PULL_SHFT                       0x1a
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_OE_N_PULL_BMSK                   0x3000000
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_OE_N_PULL_SHFT                        0x18
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_CLE_PULL_BMSK                     0xc00000
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_CLE_PULL_SHFT                         0x16
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_ALE_PULL_BMSK                     0x300000
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_ALE_PULL_SHFT                         0x14
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_A_D_BYTE0_PULL_BMSK                0xc0000
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_A_D_BYTE0_PULL_SHFT                   0x12
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_WE_N_HDRV_BMSK                     0x38000
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_WE_N_HDRV_SHFT                         0xf
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_CS0_N_HDRV_BMSK                     0x7000
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_CS0_N_HDRV_SHFT                        0xc
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_OE_N_HDRV_BMSK                       0xe00
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_OE_N_HDRV_SHFT                         0x9
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_CLE_HDRV_BMSK                        0x1c0
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_CLE_HDRV_SHFT                          0x6
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_ALE_HDRV_BMSK                         0x38
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_ALE_HDRV_SHFT                          0x3
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_A_D_BYTE0_HDRV_BMSK                    0x7
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_A_D_BYTE0_HDRV_SHFT                    0x0

#define HWIO_TLMM_SPARE1_ADDR                                             (TLMM_CSR_REG_BASE      + 0x00002060)
#define HWIO_TLMM_SPARE1_RMSK                                             0xffffffff
#define HWIO_TLMM_SPARE1_IN          \
        in_dword_masked(HWIO_TLMM_SPARE1_ADDR, HWIO_TLMM_SPARE1_RMSK)
#define HWIO_TLMM_SPARE1_INM(m)      \
        in_dword_masked(HWIO_TLMM_SPARE1_ADDR, m)
#define HWIO_TLMM_SPARE1_OUT(v)      \
        out_dword(HWIO_TLMM_SPARE1_ADDR,v)
#define HWIO_TLMM_SPARE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SPARE1_ADDR,m,v,HWIO_TLMM_SPARE1_IN)
#define HWIO_TLMM_SPARE1_SPARE1_BMSK                                      0xffffffff
#define HWIO_TLMM_SPARE1_SPARE1_SHFT                                             0x0

#define HWIO_TLMM_SPARE2_ADDR                                             (TLMM_CSR_REG_BASE      + 0x00002064)
#define HWIO_TLMM_SPARE2_RMSK                                             0xffffffff
#define HWIO_TLMM_SPARE2_IN          \
        in_dword_masked(HWIO_TLMM_SPARE2_ADDR, HWIO_TLMM_SPARE2_RMSK)
#define HWIO_TLMM_SPARE2_INM(m)      \
        in_dword_masked(HWIO_TLMM_SPARE2_ADDR, m)
#define HWIO_TLMM_SPARE2_OUT(v)      \
        out_dword(HWIO_TLMM_SPARE2_ADDR,v)
#define HWIO_TLMM_SPARE2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SPARE2_ADDR,m,v,HWIO_TLMM_SPARE2_IN)
#define HWIO_TLMM_SPARE2_SPARE2_BMSK                                      0xffffffff
#define HWIO_TLMM_SPARE2_SPARE2_SHFT                                             0x0

#define HWIO_TLMM_SPARE3_ADDR                                             (TLMM_CSR_REG_BASE      + 0x00002068)
#define HWIO_TLMM_SPARE3_RMSK                                             0xffffffff
#define HWIO_TLMM_SPARE3_IN          \
        in_dword_masked(HWIO_TLMM_SPARE3_ADDR, HWIO_TLMM_SPARE3_RMSK)
#define HWIO_TLMM_SPARE3_INM(m)      \
        in_dword_masked(HWIO_TLMM_SPARE3_ADDR, m)
#define HWIO_TLMM_SPARE3_OUT(v)      \
        out_dword(HWIO_TLMM_SPARE3_ADDR,v)
#define HWIO_TLMM_SPARE3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SPARE3_ADDR,m,v,HWIO_TLMM_SPARE3_IN)
#define HWIO_TLMM_SPARE3_SPARE3_BMSK                                      0xffffffff
#define HWIO_TLMM_SPARE3_SPARE3_SHFT                                             0x0

#define HWIO_TLMM_SPARE4_ADDR                                             (TLMM_CSR_REG_BASE      + 0x0000206c)
#define HWIO_TLMM_SPARE4_RMSK                                             0xffffffff
#define HWIO_TLMM_SPARE4_IN          \
        in_dword_masked(HWIO_TLMM_SPARE4_ADDR, HWIO_TLMM_SPARE4_RMSK)
#define HWIO_TLMM_SPARE4_INM(m)      \
        in_dword_masked(HWIO_TLMM_SPARE4_ADDR, m)
#define HWIO_TLMM_SPARE4_OUT(v)      \
        out_dword(HWIO_TLMM_SPARE4_ADDR,v)
#define HWIO_TLMM_SPARE4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SPARE4_ADDR,m,v,HWIO_TLMM_SPARE4_IN)
#define HWIO_TLMM_SPARE4_SPARE4_BMSK                                      0xffffffff
#define HWIO_TLMM_SPARE4_SPARE4_SHFT                                             0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n)                         (TLMM_CSR_REG_BASE      + 0x00002100 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_RMSK                                 0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_MAXn                                     1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_POLARITY_BMSK                        0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_POLARITY_SHFT                          0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_GPIO_SEL_BMSK                         0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_GPIO_SEL_SHFT                          0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n)                      (TLMM_CSR_REG_BASE      + 0x00002200 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_RMSK                              0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_MAXn                                  9
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_BMSK                     0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_SHFT                       0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_GPIO_SEL_BMSK                      0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_GPIO_SEL_SHFT                       0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n)                      (TLMM_CSR_REG_BASE      + 0x00002500 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_RMSK                              0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_MAXn                                  5
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_BMSK                     0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_SHFT                       0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_BMSK                      0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_SHFT                       0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n)                          (TLMM_CSR_REG_BASE      + 0x00002700 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_RMSK                                  0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_MAXn                                      0
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_BMSK                         0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_SHFT                           0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_GPIO_SEL_BMSK                          0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_GPIO_SEL_SHFT                           0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n)                         (TLMM_CSR_REG_BASE      + 0x00002800 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_RMSK                                 0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_MAXn                                     7
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_POLARITY_BMSK                        0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_POLARITY_SHFT                          0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_GPIO_SEL_BMSK                         0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_GPIO_SEL_SHFT                          0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n)                          (TLMM_CSR_REG_BASE      + 0x00002a00 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_RMSK                                  0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_MAXn                                      1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_BMSK                         0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_SHFT                           0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_GPIO_SEL_BMSK                          0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_GPIO_SEL_SHFT                           0x0

#define HWIO_TLMM_GPIO_OUT_0_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003000)
#define HWIO_TLMM_GPIO_OUT_0_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, HWIO_TLMM_GPIO_OUT_0_RMSK)
#define HWIO_TLMM_GPIO_OUT_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_0_ADDR,m,v,HWIO_TLMM_GPIO_OUT_0_IN)
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_BMSK                                0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_SHFT                                       0x0

#define HWIO_TLMM_GPIO_OUT_1_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003004)
#define HWIO_TLMM_GPIO_OUT_1_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_OUT_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_1_ADDR, HWIO_TLMM_GPIO_OUT_1_RMSK)
#define HWIO_TLMM_GPIO_OUT_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_1_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_1_ADDR,m,v,HWIO_TLMM_GPIO_OUT_1_IN)
#define HWIO_TLMM_GPIO_OUT_1_GPIO_OUT_BMSK                                0xffffffff
#define HWIO_TLMM_GPIO_OUT_1_GPIO_OUT_SHFT                                       0x0

#define HWIO_TLMM_GPIO_OUT_2_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003008)
#define HWIO_TLMM_GPIO_OUT_2_RMSK                                          0x3ffffff
#define HWIO_TLMM_GPIO_OUT_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_2_ADDR, HWIO_TLMM_GPIO_OUT_2_RMSK)
#define HWIO_TLMM_GPIO_OUT_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_2_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_2_ADDR,m,v,HWIO_TLMM_GPIO_OUT_2_IN)
#define HWIO_TLMM_GPIO_OUT_2_GPIO_OUT_BMSK                                 0x3ffffff
#define HWIO_TLMM_GPIO_OUT_2_GPIO_OUT_SHFT                                       0x0

#define HWIO_TLMM_GPIO_OUT_CLR_0_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003020)
#define HWIO_TLMM_GPIO_OUT_CLR_0_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_0_GPIO_OUT_CLR_BMSK                        0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_0_GPIO_OUT_CLR_SHFT                               0x0

#define HWIO_TLMM_GPIO_OUT_CLR_1_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003024)
#define HWIO_TLMM_GPIO_OUT_CLR_1_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_1_GPIO_OUT_CLR_BMSK                        0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_1_GPIO_OUT_CLR_SHFT                               0x0

#define HWIO_TLMM_GPIO_OUT_CLR_2_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003028)
#define HWIO_TLMM_GPIO_OUT_CLR_2_RMSK                                      0x3ffffff
#define HWIO_TLMM_GPIO_OUT_CLR_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_2_GPIO_OUT_CLR_BMSK                         0x3ffffff
#define HWIO_TLMM_GPIO_OUT_CLR_2_GPIO_OUT_CLR_SHFT                               0x0

#define HWIO_TLMM_GPIO_OUT_SET_0_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003040)
#define HWIO_TLMM_GPIO_OUT_SET_0_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_0_GPIO_OUT_SET_BMSK                        0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_0_GPIO_OUT_SET_SHFT                               0x0

#define HWIO_TLMM_GPIO_OUT_SET_1_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003044)
#define HWIO_TLMM_GPIO_OUT_SET_1_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_1_GPIO_OUT_SET_BMSK                        0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_1_GPIO_OUT_SET_SHFT                               0x0

#define HWIO_TLMM_GPIO_OUT_SET_2_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003048)
#define HWIO_TLMM_GPIO_OUT_SET_2_RMSK                                      0x3ffffff
#define HWIO_TLMM_GPIO_OUT_SET_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_2_GPIO_OUT_SET_BMSK                         0x3ffffff
#define HWIO_TLMM_GPIO_OUT_SET_2_GPIO_OUT_SET_SHFT                               0x0

#define HWIO_TLMM_GPIO_IN_0_ADDR                                          (TLMM_CSR_REG_BASE      + 0x00003060)
#define HWIO_TLMM_GPIO_IN_0_RMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_IN_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_0_ADDR, HWIO_TLMM_GPIO_IN_0_RMSK)
#define HWIO_TLMM_GPIO_IN_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_0_ADDR, m)
#define HWIO_TLMM_GPIO_IN_0_GPIO_IN_BMSK                                  0xffffffff
#define HWIO_TLMM_GPIO_IN_0_GPIO_IN_SHFT                                         0x0

#define HWIO_TLMM_GPIO_IN_1_ADDR                                          (TLMM_CSR_REG_BASE      + 0x00003064)
#define HWIO_TLMM_GPIO_IN_1_RMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_IN_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_1_ADDR, HWIO_TLMM_GPIO_IN_1_RMSK)
#define HWIO_TLMM_GPIO_IN_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_1_ADDR, m)
#define HWIO_TLMM_GPIO_IN_1_GPIO_IN_BMSK                                  0xffffffff
#define HWIO_TLMM_GPIO_IN_1_GPIO_IN_SHFT                                         0x0

#define HWIO_TLMM_GPIO_IN_2_ADDR                                          (TLMM_CSR_REG_BASE      + 0x00003068)
#define HWIO_TLMM_GPIO_IN_2_RMSK                                           0x3ffffff
#define HWIO_TLMM_GPIO_IN_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_2_ADDR, HWIO_TLMM_GPIO_IN_2_RMSK)
#define HWIO_TLMM_GPIO_IN_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_2_ADDR, m)
#define HWIO_TLMM_GPIO_IN_2_GPIO_IN_BMSK                                   0x3ffffff
#define HWIO_TLMM_GPIO_IN_2_GPIO_IN_SHFT                                         0x0

#define HWIO_TLMM_GPIO_OE_0_ADDR                                          (TLMM_CSR_REG_BASE      + 0x00003080)
#define HWIO_TLMM_GPIO_OE_0_RMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OE_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, HWIO_TLMM_GPIO_OE_0_RMSK)
#define HWIO_TLMM_GPIO_OE_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, m)
#define HWIO_TLMM_GPIO_OE_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_0_ADDR,m,v,HWIO_TLMM_GPIO_OE_0_IN)
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_BMSK                                  0xffffffff
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_SHFT                                         0x0

#define HWIO_TLMM_GPIO_OE_1_ADDR                                          (TLMM_CSR_REG_BASE      + 0x00003084)
#define HWIO_TLMM_GPIO_OE_1_RMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OE_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_1_ADDR, HWIO_TLMM_GPIO_OE_1_RMSK)
#define HWIO_TLMM_GPIO_OE_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_1_ADDR, m)
#define HWIO_TLMM_GPIO_OE_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_1_ADDR,m,v,HWIO_TLMM_GPIO_OE_1_IN)
#define HWIO_TLMM_GPIO_OE_1_GPIO_OE_BMSK                                  0xffffffff
#define HWIO_TLMM_GPIO_OE_1_GPIO_OE_SHFT                                         0x0

#define HWIO_TLMM_GPIO_OE_2_ADDR                                          (TLMM_CSR_REG_BASE      + 0x00003088)
#define HWIO_TLMM_GPIO_OE_2_RMSK                                           0x3ffffff
#define HWIO_TLMM_GPIO_OE_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_2_ADDR, HWIO_TLMM_GPIO_OE_2_RMSK)
#define HWIO_TLMM_GPIO_OE_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_2_ADDR, m)
#define HWIO_TLMM_GPIO_OE_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_2_ADDR,m,v,HWIO_TLMM_GPIO_OE_2_IN)
#define HWIO_TLMM_GPIO_OE_2_GPIO_OE_BMSK                                   0x3ffffff
#define HWIO_TLMM_GPIO_OE_2_GPIO_OE_SHFT                                         0x0

#define HWIO_TLMM_GPIO_OE_CLR_0_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00003100)
#define HWIO_TLMM_GPIO_OE_CLR_0_RMSK                                      0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_0_GPIO_OE_CLR_BMSK                          0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_0_GPIO_OE_CLR_SHFT                                 0x0

#define HWIO_TLMM_GPIO_OE_CLR_1_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00003104)
#define HWIO_TLMM_GPIO_OE_CLR_1_RMSK                                      0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_1_GPIO_OE_CLR_BMSK                          0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_1_GPIO_OE_CLR_SHFT                                 0x0

#define HWIO_TLMM_GPIO_OE_CLR_2_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00003108)
#define HWIO_TLMM_GPIO_OE_CLR_2_RMSK                                       0x3ffffff
#define HWIO_TLMM_GPIO_OE_CLR_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_2_GPIO_OE_CLR_BMSK                           0x3ffffff
#define HWIO_TLMM_GPIO_OE_CLR_2_GPIO_OE_CLR_SHFT                                 0x0

#define HWIO_TLMM_GPIO_OE_SET_0_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00003120)
#define HWIO_TLMM_GPIO_OE_SET_0_RMSK                                      0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_0_GPIO_OE_SET_BMSK                          0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_0_GPIO_OE_SET_SHFT                                 0x0

#define HWIO_TLMM_GPIO_OE_SET_1_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00003124)
#define HWIO_TLMM_GPIO_OE_SET_1_RMSK                                      0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_1_GPIO_OE_SET_BMSK                          0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_1_GPIO_OE_SET_SHFT                                 0x0

#define HWIO_TLMM_GPIO_OE_SET_2_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00003128)
#define HWIO_TLMM_GPIO_OE_SET_2_RMSK                                       0x3ffffff
#define HWIO_TLMM_GPIO_OE_SET_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_2_GPIO_OE_SET_BMSK                           0x3ffffff
#define HWIO_TLMM_GPIO_OE_SET_2_GPIO_OE_SET_SHFT                                 0x0


#endif /* __UIMBOLTMACROS_H__ */
