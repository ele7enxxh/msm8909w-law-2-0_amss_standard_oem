#ifndef __UIMDIMEHWIOMACROS_H__
#define __UIMDIMEHWIOMACROS_H__
/*
===========================================================================
*/
/**
  @file uimdimehwiomacros.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MSS_UIM0_UART_DM
    MSS_UIM1_UART_DM
    MSS_UIM2_UART_DM
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimdimehwiomacros.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include "msmhwiobase.h"

#ifdef FEATURE_BOLT_MODEM
  #define MSS_TOP_BASE       MODEM_TOP_BASE
#endif /* FEATURE_BOLT_MODEM */

/*----------------------------------------------------------------------------
 * MODULE: MSS_UIM0_UART_DM
 *--------------------------------------------------------------------------*/

#define MSS_UIM0_UART_DM_REG_BASE                                                                               (MSS_TOP_BASE      + 0x00190000)

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

#define MSS_UIM1_UART_DM_REG_BASE                                                                               (MSS_TOP_BASE      + 0x00198000)

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

#define MSS_UIM2_UART_DM_REG_BASE                                                                               (MSS_TOP_BASE      + 0x001a8000)

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
 * MODULE: MSS_PERPH
 *--------------------------------------------------------------------------*/

#define MSS_PERPH_REG_BASE                                                (MSS_TOP_BASE      + 0x00180000)

#define HWIO_MSS_ENABLE_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00000000)
#define HWIO_MSS_ENABLE_RMSK                                                   0x1ff
#define HWIO_MSS_ENABLE_IN          \
        in_dword_masked(HWIO_MSS_ENABLE_ADDR, HWIO_MSS_ENABLE_RMSK)
#define HWIO_MSS_ENABLE_INM(m)      \
        in_dword_masked(HWIO_MSS_ENABLE_ADDR, m)
#define HWIO_MSS_ENABLE_OUT(v)      \
        out_dword(HWIO_MSS_ENABLE_ADDR,v)
#define HWIO_MSS_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_ENABLE_ADDR,m,v,HWIO_MSS_ENABLE_IN)
#define HWIO_MSS_ENABLE_UIM2_BMSK                                              0x100
#define HWIO_MSS_ENABLE_UIM2_SHFT                                                0x8
#define HWIO_MSS_ENABLE_COXM_BMSK                                               0x80
#define HWIO_MSS_ENABLE_COXM_SHFT                                                0x7
#define HWIO_MSS_ENABLE_RESERVE_BIT_6_BMSK                                      0x40
#define HWIO_MSS_ENABLE_RESERVE_BIT_6_SHFT                                       0x6
#define HWIO_MSS_ENABLE_CRYPTO5_BMSK                                            0x20
#define HWIO_MSS_ENABLE_CRYPTO5_SHFT                                             0x5
#define HWIO_MSS_ENABLE_UIM1_BMSK                                               0x10
#define HWIO_MSS_ENABLE_UIM1_SHFT                                                0x4
#define HWIO_MSS_ENABLE_UIM0_BMSK                                                0x8
#define HWIO_MSS_ENABLE_UIM0_SHFT                                                0x3
#define HWIO_MSS_ENABLE_NAV_BMSK                                                 0x4
#define HWIO_MSS_ENABLE_NAV_SHFT                                                 0x2
#define HWIO_MSS_ENABLE_Q6_BMSK                                                  0x2
#define HWIO_MSS_ENABLE_Q6_SHFT                                                  0x1
#define HWIO_MSS_ENABLE_MODEM_BMSK                                               0x1
#define HWIO_MSS_ENABLE_MODEM_SHFT                                               0x0

#define HWIO_MSS_CLAMP_MEM_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00000004)
#define HWIO_MSS_CLAMP_MEM_RMSK                                                  0x7
#define HWIO_MSS_CLAMP_MEM_IN          \
        in_dword_masked(HWIO_MSS_CLAMP_MEM_ADDR, HWIO_MSS_CLAMP_MEM_RMSK)
#define HWIO_MSS_CLAMP_MEM_INM(m)      \
        in_dword_masked(HWIO_MSS_CLAMP_MEM_ADDR, m)
#define HWIO_MSS_CLAMP_MEM_OUT(v)      \
        out_dword(HWIO_MSS_CLAMP_MEM_ADDR,v)
#define HWIO_MSS_CLAMP_MEM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLAMP_MEM_ADDR,m,v,HWIO_MSS_CLAMP_MEM_IN)
#define HWIO_MSS_CLAMP_MEM_SPARE_BMSK                                            0x4
#define HWIO_MSS_CLAMP_MEM_SPARE_SHFT                                            0x2
#define HWIO_MSS_CLAMP_MEM_UNCLAMP_ALL_BMSK                                      0x2
#define HWIO_MSS_CLAMP_MEM_UNCLAMP_ALL_SHFT                                      0x1
#define HWIO_MSS_CLAMP_MEM_HM_CLAMP_BMSK                                         0x1
#define HWIO_MSS_CLAMP_MEM_HM_CLAMP_SHFT                                         0x0

#define HWIO_MSS_CLAMP_IO_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00000008)
#define HWIO_MSS_CLAMP_IO_RMSK                                                  0xff
#define HWIO_MSS_CLAMP_IO_IN          \
        in_dword_masked(HWIO_MSS_CLAMP_IO_ADDR, HWIO_MSS_CLAMP_IO_RMSK)
#define HWIO_MSS_CLAMP_IO_INM(m)      \
        in_dword_masked(HWIO_MSS_CLAMP_IO_ADDR, m)
#define HWIO_MSS_CLAMP_IO_OUT(v)      \
        out_dword(HWIO_MSS_CLAMP_IO_ADDR,v)
#define HWIO_MSS_CLAMP_IO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLAMP_IO_ADDR,m,v,HWIO_MSS_CLAMP_IO_IN)
#define HWIO_MSS_CLAMP_IO_SPARE_7_BMSK                                          0x80
#define HWIO_MSS_CLAMP_IO_SPARE_7_SHFT                                           0x7
#define HWIO_MSS_CLAMP_IO_UNCLAMP_ALL_BMSK                                      0x40
#define HWIO_MSS_CLAMP_IO_UNCLAMP_ALL_SHFT                                       0x6
#define HWIO_MSS_CLAMP_IO_CPR_BYP_MUX_SEL_Q6_BMSK                               0x20
#define HWIO_MSS_CLAMP_IO_CPR_BYP_MUX_SEL_Q6_SHFT                                0x5
#define HWIO_MSS_CLAMP_IO_BBRX_ADC_BMSK                                         0x10
#define HWIO_MSS_CLAMP_IO_BBRX_ADC_SHFT                                          0x4
#define HWIO_MSS_CLAMP_IO_GNSS_DAC_BMSK                                          0x8
#define HWIO_MSS_CLAMP_IO_GNSS_DAC_SHFT                                          0x3
#define HWIO_MSS_CLAMP_IO_COM_COMP_BMSK                                          0x4
#define HWIO_MSS_CLAMP_IO_COM_COMP_SHFT                                          0x2
#define HWIO_MSS_CLAMP_IO_NC_HM_BMSK                                             0x2
#define HWIO_MSS_CLAMP_IO_NC_HM_SHFT                                             0x1
#define HWIO_MSS_CLAMP_IO_MODEM_BMSK                                             0x1
#define HWIO_MSS_CLAMP_IO_MODEM_SHFT                                             0x0

#define HWIO_MSS_BUS_AHB2AHB_CFG_ADDR                                     (MSS_PERPH_REG_BASE      + 0x0000000c)
#define HWIO_MSS_BUS_AHB2AHB_CFG_RMSK                                            0x3
#define HWIO_MSS_BUS_AHB2AHB_CFG_IN          \
        in_dword_masked(HWIO_MSS_BUS_AHB2AHB_CFG_ADDR, HWIO_MSS_BUS_AHB2AHB_CFG_RMSK)
#define HWIO_MSS_BUS_AHB2AHB_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_AHB2AHB_CFG_ADDR, m)
#define HWIO_MSS_BUS_AHB2AHB_CFG_OUT(v)      \
        out_dword(HWIO_MSS_BUS_AHB2AHB_CFG_ADDR,v)
#define HWIO_MSS_BUS_AHB2AHB_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_AHB2AHB_CFG_ADDR,m,v,HWIO_MSS_BUS_AHB2AHB_CFG_IN)
#define HWIO_MSS_BUS_AHB2AHB_CFG_POST_EN_AHB2AHB_NAV_BMSK                        0x2
#define HWIO_MSS_BUS_AHB2AHB_CFG_POST_EN_AHB2AHB_NAV_SHFT                        0x1
#define HWIO_MSS_BUS_AHB2AHB_CFG_POST_EN_AHB2AHB_BMSK                            0x1
#define HWIO_MSS_BUS_AHB2AHB_CFG_POST_EN_AHB2AHB_SHFT                            0x0

#define HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00000010)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_RMSK                                           0xf
#define HWIO_MSS_BUS_MAXI2AXI_CFG_IN          \
        in_dword_masked(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR, HWIO_MSS_BUS_MAXI2AXI_CFG_RMSK)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR, m)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR,v)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR,m,v,HWIO_MSS_BUS_MAXI2AXI_CFG_IN)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_NAV_AREQPRIORITY_BMSK                    0xc
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_NAV_AREQPRIORITY_SHFT                    0x2
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_CRYPTO_AREQPRIORITY_BMSK                 0x3
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_CRYPTO_AREQPRIORITY_SHFT                 0x0

#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR                                 (MSS_PERPH_REG_BASE      + 0x00000014)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_RMSK                                       0xff
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_IN          \
        in_dword_masked(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR, HWIO_MSS_CUSTOM_MEM_ARRSTBYN_RMSK)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_INM(m)      \
        in_dword_masked(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR, m)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_OUT(v)      \
        out_dword(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR,v)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR,m,v,HWIO_MSS_CUSTOM_MEM_ARRSTBYN_IN)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_CTL_BMSK                                   0xff
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_CTL_SHFT                                    0x0

#define HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR                                  (MSS_PERPH_REG_BASE      + 0x00000018)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_RMSK                                         0x3
#define HWIO_MSS_ANALOG_IP_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR, HWIO_MSS_ANALOG_IP_TEST_CTL_RMSK)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR, m)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR,v)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR,m,v,HWIO_MSS_ANALOG_IP_TEST_CTL_IN)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_EXTERNAL_IQDATA_EN_BMSK                      0x2
#define HWIO_MSS_ANALOG_IP_TEST_CTL_EXTERNAL_IQDATA_EN_SHFT                      0x1
#define HWIO_MSS_ANALOG_IP_TEST_CTL_EXTERNAL_Y1Y2_EN_BMSK                        0x1
#define HWIO_MSS_ANALOG_IP_TEST_CTL_EXTERNAL_Y1Y2_EN_SHFT                        0x0

#define HWIO_MSS_ATB_ID_ADDR                                              (MSS_PERPH_REG_BASE      + 0x0000001c)
#define HWIO_MSS_ATB_ID_RMSK                                                    0x7f
#define HWIO_MSS_ATB_ID_IN          \
        in_dword_masked(HWIO_MSS_ATB_ID_ADDR, HWIO_MSS_ATB_ID_RMSK)
#define HWIO_MSS_ATB_ID_INM(m)      \
        in_dword_masked(HWIO_MSS_ATB_ID_ADDR, m)
#define HWIO_MSS_ATB_ID_OUT(v)      \
        out_dword(HWIO_MSS_ATB_ID_ADDR,v)
#define HWIO_MSS_ATB_ID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_ATB_ID_ADDR,m,v,HWIO_MSS_ATB_ID_IN)
#define HWIO_MSS_ATB_ID_ATB_ID_BMSK                                             0x7f
#define HWIO_MSS_ATB_ID_ATB_ID_SHFT                                              0x0

#define HWIO_MSS_DBG_BUS_CTL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00000020)
#define HWIO_MSS_DBG_BUS_CTL_RMSK                                            0x7ffff
#define HWIO_MSS_DBG_BUS_CTL_IN          \
        in_dword_masked(HWIO_MSS_DBG_BUS_CTL_ADDR, HWIO_MSS_DBG_BUS_CTL_RMSK)
#define HWIO_MSS_DBG_BUS_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DBG_BUS_CTL_ADDR, m)
#define HWIO_MSS_DBG_BUS_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DBG_BUS_CTL_ADDR,v)
#define HWIO_MSS_DBG_BUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DBG_BUS_CTL_ADDR,m,v,HWIO_MSS_DBG_BUS_CTL_IN)
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_NAV_SEL_BMSK                       0x70000
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_NAV_SEL_SHFT                          0x10
#define HWIO_MSS_DBG_BUS_CTL_AHB2AXI_NAV_SEL_BMSK                             0xe000
#define HWIO_MSS_DBG_BUS_CTL_AHB2AXI_NAV_SEL_SHFT                                0xd
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_NAV_CONFIG_SEL_BMSK                      0x1800
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_NAV_CONFIG_SEL_SHFT                         0xb
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_SEL_BMSK                                  0x600
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_SEL_SHFT                                    0x9
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_TOP_SEL_BMSK                          0x1e0
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_TOP_SEL_SHFT                            0x5
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_NC_HM_SEL_BMSK                         0x1c
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_NC_HM_SEL_SHFT                          0x2
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_GPIO_ATB_SEL_BMSK                           0x3
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_GPIO_ATB_SEL_SHFT                           0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR                               (MSS_PERPH_REG_BASE      + 0x00000024)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_RMSK                                      0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_IN          \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR, HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_RMSK)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_INM(m)      \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR, m)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OUT(v)      \
        out_dword(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR,v)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR,m,v,HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_IN)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_EN_BMSK                                   0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_EN_SHFT                                   0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR                           (MSS_PERPH_REG_BASE      + 0x00000028)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_RMSK                                  0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_IN          \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR, HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_RMSK)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR, m)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_STATUS_BMSK                           0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_STATUS_SHFT                           0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_ADDR                              (MSS_PERPH_REG_BASE      + 0x0000002c)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_RMSK                                     0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_OUT(v)      \
        out_dword(HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_ADDR,v)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_CMD_BMSK                                 0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_CMD_SHFT                                 0x0

#define HWIO_MSS_BUS_CTL_CFG_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00000030)
#define HWIO_MSS_BUS_CTL_CFG_RMSK                                                0x1
#define HWIO_MSS_BUS_CTL_CFG_IN          \
        in_dword_masked(HWIO_MSS_BUS_CTL_CFG_ADDR, HWIO_MSS_BUS_CTL_CFG_RMSK)
#define HWIO_MSS_BUS_CTL_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CTL_CFG_ADDR, m)
#define HWIO_MSS_BUS_CTL_CFG_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CTL_CFG_ADDR,v)
#define HWIO_MSS_BUS_CTL_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CTL_CFG_ADDR,m,v,HWIO_MSS_BUS_CTL_CFG_IN)
#define HWIO_MSS_BUS_CTL_CFG_Q6_FORCE_UNBUFFERED_BMSK                            0x1
#define HWIO_MSS_BUS_CTL_CFG_Q6_FORCE_UNBUFFERED_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW0_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000034)
#define HWIO_MSS_RELAY_MSG_SHADOW0_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW0_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR, HWIO_MSS_RELAY_MSG_SHADOW0_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW0_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW0_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW0_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW0_RELAY_MSG_SHADOW0_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW0_RELAY_MSG_SHADOW0_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW1_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000038)
#define HWIO_MSS_RELAY_MSG_SHADOW1_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW1_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR, HWIO_MSS_RELAY_MSG_SHADOW1_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW1_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW1_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW1_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW1_RELAY_MSG_SHADOW1_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW1_RELAY_MSG_SHADOW1_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW2_ADDR                                   (MSS_PERPH_REG_BASE      + 0x0000003c)
#define HWIO_MSS_RELAY_MSG_SHADOW2_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW2_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR, HWIO_MSS_RELAY_MSG_SHADOW2_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW2_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW2_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW2_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW2_RELAY_MSG_SHADOW2_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW2_RELAY_MSG_SHADOW2_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW3_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000040)
#define HWIO_MSS_RELAY_MSG_SHADOW3_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW3_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR, HWIO_MSS_RELAY_MSG_SHADOW3_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW3_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW3_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW3_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW3_RELAY_MSG_SHADOW3_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW3_RELAY_MSG_SHADOW3_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW4_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000044)
#define HWIO_MSS_RELAY_MSG_SHADOW4_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW4_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR, HWIO_MSS_RELAY_MSG_SHADOW4_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW4_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW4_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW4_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW4_RELAY_MSG_SHADOW4_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW4_RELAY_MSG_SHADOW4_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW5_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000048)
#define HWIO_MSS_RELAY_MSG_SHADOW5_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW5_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR, HWIO_MSS_RELAY_MSG_SHADOW5_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW5_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW5_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW5_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW5_RELAY_MSG_SHADOW5_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW5_RELAY_MSG_SHADOW5_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW6_ADDR                                   (MSS_PERPH_REG_BASE      + 0x0000004c)
#define HWIO_MSS_RELAY_MSG_SHADOW6_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW6_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR, HWIO_MSS_RELAY_MSG_SHADOW6_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW6_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW6_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW6_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW6_RELAY_MSG_SHADOW6_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW6_RELAY_MSG_SHADOW6_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW7_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000050)
#define HWIO_MSS_RELAY_MSG_SHADOW7_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW7_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR, HWIO_MSS_RELAY_MSG_SHADOW7_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW7_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW7_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW7_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW7_RELAY_MSG_SHADOW7_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW7_RELAY_MSG_SHADOW7_SHFT                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW8_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000054)
#define HWIO_MSS_RELAY_MSG_SHADOW8_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW8_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR, HWIO_MSS_RELAY_MSG_SHADOW8_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW8_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW8_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW8_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW8_RELAY_MSG_SHADOW8_BMSK                 0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW8_RELAY_MSG_SHADOW8_SHFT                        0x0

#define HWIO_MSS_MSA_ADDR                                                 (MSS_PERPH_REG_BASE      + 0x00000058)
#define HWIO_MSS_MSA_RMSK                                                        0x3
#define HWIO_MSS_MSA_IN          \
        in_dword_masked(HWIO_MSS_MSA_ADDR, HWIO_MSS_MSA_RMSK)
#define HWIO_MSS_MSA_INM(m)      \
        in_dword_masked(HWIO_MSS_MSA_ADDR, m)
#define HWIO_MSS_MSA_OUT(v)      \
        out_dword(HWIO_MSS_MSA_ADDR,v)
#define HWIO_MSS_MSA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MSA_ADDR,m,v,HWIO_MSS_MSA_IN)
#define HWIO_MSS_MSA_MBA_OK_BMSK                                                 0x2
#define HWIO_MSS_MSA_MBA_OK_SHFT                                                 0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_BMSK                                            0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_SHFT                                            0x0

#define HWIO_MSS_HW_VERSION_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000005c)
#define HWIO_MSS_HW_VERSION_RMSK                                          0xffffffff
#define HWIO_MSS_HW_VERSION_IN          \
        in_dword_masked(HWIO_MSS_HW_VERSION_ADDR, HWIO_MSS_HW_VERSION_RMSK)
#define HWIO_MSS_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_HW_VERSION_ADDR, m)
#define HWIO_MSS_HW_VERSION_MAJOR_BMSK                                    0xf0000000
#define HWIO_MSS_HW_VERSION_MAJOR_SHFT                                          0x1c
#define HWIO_MSS_HW_VERSION_MINOR_BMSK                                     0xfff0000
#define HWIO_MSS_HW_VERSION_MINOR_SHFT                                          0x10
#define HWIO_MSS_HW_VERSION_STEP_BMSK                                         0xffff
#define HWIO_MSS_HW_VERSION_STEP_SHFT                                            0x0

#define HWIO_MSS_DIME_MEM_SLP_CNTL_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000060)
#define HWIO_MSS_DIME_MEM_SLP_CNTL_RMSK                                    0x302030f
#define HWIO_MSS_DIME_MEM_SLP_CNTL_IN          \
        in_dword_masked(HWIO_MSS_DIME_MEM_SLP_CNTL_ADDR, HWIO_MSS_DIME_MEM_SLP_CNTL_RMSK)
#define HWIO_MSS_DIME_MEM_SLP_CNTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DIME_MEM_SLP_CNTL_ADDR, m)
#define HWIO_MSS_DIME_MEM_SLP_CNTL_OUT(v)      \
        out_dword(HWIO_MSS_DIME_MEM_SLP_CNTL_ADDR,v)
#define HWIO_MSS_DIME_MEM_SLP_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DIME_MEM_SLP_CNTL_ADDR,m,v,HWIO_MSS_DIME_MEM_SLP_CNTL_IN)
#define HWIO_MSS_DIME_MEM_SLP_CNTL_MEM_SLP_SPM_BMSK                        0x2000000
#define HWIO_MSS_DIME_MEM_SLP_CNTL_MEM_SLP_SPM_SHFT                             0x19
#define HWIO_MSS_DIME_MEM_SLP_CNTL_MEM_SLP_NOW_BMSK                        0x1000000
#define HWIO_MSS_DIME_MEM_SLP_CNTL_MEM_SLP_NOW_SHFT                             0x18
#define HWIO_MSS_DIME_MEM_SLP_CNTL_ALL_NR_SLP_NRET_N_BMSK                    0x20000
#define HWIO_MSS_DIME_MEM_SLP_CNTL_ALL_NR_SLP_NRET_N_SHFT                       0x11
#define HWIO_MSS_DIME_MEM_SLP_CNTL_CCS_SLP_NRET_N_BMSK                         0x200
#define HWIO_MSS_DIME_MEM_SLP_CNTL_CCS_SLP_NRET_N_SHFT                           0x9
#define HWIO_MSS_DIME_MEM_SLP_CNTL_CCS_SLP_RET_N_BMSK                          0x100
#define HWIO_MSS_DIME_MEM_SLP_CNTL_CCS_SLP_RET_N_SHFT                            0x8
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE1_SLP_NRET_N_BMSK                          0x8
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE1_SLP_NRET_N_SHFT                          0x3
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE1_SLP_RET_N_BMSK                           0x4
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE1_SLP_RET_N_SHFT                           0x2
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE0_SLP_NRET_N_BMSK                          0x2
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE0_SLP_NRET_N_SHFT                          0x1
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE0_SLP_RET_N_BMSK                           0x1
#define HWIO_MSS_DIME_MEM_SLP_CNTL_VPE0_SLP_RET_N_SHFT                           0x0

#define HWIO_MSS_CLOCK_SPDM_MON_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00000064)
#define HWIO_MSS_CLOCK_SPDM_MON_RMSK                                             0x3
#define HWIO_MSS_CLOCK_SPDM_MON_IN          \
        in_dword_masked(HWIO_MSS_CLOCK_SPDM_MON_ADDR, HWIO_MSS_CLOCK_SPDM_MON_RMSK)
#define HWIO_MSS_CLOCK_SPDM_MON_INM(m)      \
        in_dword_masked(HWIO_MSS_CLOCK_SPDM_MON_ADDR, m)
#define HWIO_MSS_CLOCK_SPDM_MON_OUT(v)      \
        out_dword(HWIO_MSS_CLOCK_SPDM_MON_ADDR,v)
#define HWIO_MSS_CLOCK_SPDM_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLOCK_SPDM_MON_ADDR,m,v,HWIO_MSS_CLOCK_SPDM_MON_IN)
#define HWIO_MSS_CLOCK_SPDM_MON_Q6_MON_CLKEN_BMSK                                0x2
#define HWIO_MSS_CLOCK_SPDM_MON_Q6_MON_CLKEN_SHFT                                0x1
#define HWIO_MSS_CLOCK_SPDM_MON_BUS_MON_CLKEN_BMSK                               0x1
#define HWIO_MSS_CLOCK_SPDM_MON_BUS_MON_CLKEN_SHFT                               0x0

#define HWIO_MSS_BBRX0_MUX_SEL_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000068)
#define HWIO_MSS_BBRX0_MUX_SEL_RMSK                                              0x3
#define HWIO_MSS_BBRX0_MUX_SEL_IN          \
        in_dword_masked(HWIO_MSS_BBRX0_MUX_SEL_ADDR, HWIO_MSS_BBRX0_MUX_SEL_RMSK)
#define HWIO_MSS_BBRX0_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX0_MUX_SEL_ADDR, m)
#define HWIO_MSS_BBRX0_MUX_SEL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX0_MUX_SEL_ADDR,v)
#define HWIO_MSS_BBRX0_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX0_MUX_SEL_ADDR,m,v,HWIO_MSS_BBRX0_MUX_SEL_IN)
#define HWIO_MSS_BBRX0_MUX_SEL_SECOND_MUX_SEL_BMSK                               0x2
#define HWIO_MSS_BBRX0_MUX_SEL_SECOND_MUX_SEL_SHFT                               0x1
#define HWIO_MSS_BBRX0_MUX_SEL_FIRST_MUX_SEL_BMSK                                0x1
#define HWIO_MSS_BBRX0_MUX_SEL_FIRST_MUX_SEL_SHFT                                0x0

#define HWIO_MSS_BBRX1_MUX_SEL_ADDR                                       (MSS_PERPH_REG_BASE      + 0x0000006c)
#define HWIO_MSS_BBRX1_MUX_SEL_RMSK                                              0x3
#define HWIO_MSS_BBRX1_MUX_SEL_IN          \
        in_dword_masked(HWIO_MSS_BBRX1_MUX_SEL_ADDR, HWIO_MSS_BBRX1_MUX_SEL_RMSK)
#define HWIO_MSS_BBRX1_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX1_MUX_SEL_ADDR, m)
#define HWIO_MSS_BBRX1_MUX_SEL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX1_MUX_SEL_ADDR,v)
#define HWIO_MSS_BBRX1_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX1_MUX_SEL_ADDR,m,v,HWIO_MSS_BBRX1_MUX_SEL_IN)
#define HWIO_MSS_BBRX1_MUX_SEL_SECOND_MUX_SEL_BMSK                               0x2
#define HWIO_MSS_BBRX1_MUX_SEL_SECOND_MUX_SEL_SHFT                               0x1
#define HWIO_MSS_BBRX1_MUX_SEL_FIRST_MUX_SEL_BMSK                                0x1
#define HWIO_MSS_BBRX1_MUX_SEL_FIRST_MUX_SEL_SHFT                                0x0

#define HWIO_MSS_DEBUG_CLOCK_CTL_ADDR                                     (MSS_PERPH_REG_BASE      + 0x00000078)
#define HWIO_MSS_DEBUG_CLOCK_CTL_RMSK                                           0x3f
#define HWIO_MSS_DEBUG_CLOCK_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR, HWIO_MSS_DEBUG_CLOCK_CTL_RMSK)
#define HWIO_MSS_DEBUG_CLOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CLOCK_CTL_IN)
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_MUX_SEL_BMSK                               0x20
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_MUX_SEL_SHFT                                0x5
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL5_MUX_SEL_BMSK                        0x10
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL5_MUX_SEL_SHFT                         0x4
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL4_MUX_SEL_BMSK                         0x8
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL4_MUX_SEL_SHFT                         0x3
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL3_MUX_SEL_BMSK                         0x4
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL3_MUX_SEL_SHFT                         0x2
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL2_MUX_SEL_BMSK                         0x2
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL2_MUX_SEL_SHFT                         0x1
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL1_MUX_SEL_BMSK                         0x1
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL1_MUX_SEL_SHFT                         0x0

#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR                              (MSS_PERPH_REG_BASE      + 0x0000007c)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_RMSK                                     0x3
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_IN          \
        in_dword_masked(HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR, HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_RMSK)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR, m)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR,v)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR,m,v,HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_IN)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_PLLTEST_BMSK                             0x2
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_PLLTEST_SHFT                             0x1
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_EXT_CLOCK_MUX_SEL_BMSK                   0x1
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_EXT_CLOCK_MUX_SEL_SHFT                   0x0

#define HWIO_MSS_GNSSADC_BIST_CONFIG_ADDR                                 (MSS_PERPH_REG_BASE      + 0x00000084)
#define HWIO_MSS_GNSSADC_BIST_CONFIG_RMSK                                       0x7f
#define HWIO_MSS_GNSSADC_BIST_CONFIG_IN          \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_CONFIG_ADDR, HWIO_MSS_GNSSADC_BIST_CONFIG_RMSK)
#define HWIO_MSS_GNSSADC_BIST_CONFIG_INM(m)      \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_CONFIG_ADDR, m)
#define HWIO_MSS_GNSSADC_BIST_CONFIG_OUT(v)      \
        out_dword(HWIO_MSS_GNSSADC_BIST_CONFIG_ADDR,v)
#define HWIO_MSS_GNSSADC_BIST_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_GNSSADC_BIST_CONFIG_ADDR,m,v,HWIO_MSS_GNSSADC_BIST_CONFIG_IN)
#define HWIO_MSS_GNSSADC_BIST_CONFIG_GNSSADC_EXT_CLK_SEL_BMSK                   0x40
#define HWIO_MSS_GNSSADC_BIST_CONFIG_GNSSADC_EXT_CLK_SEL_SHFT                    0x6
#define HWIO_MSS_GNSSADC_BIST_CONFIG_GNSSADC_BIST_EN_BMSK                       0x20
#define HWIO_MSS_GNSSADC_BIST_CONFIG_GNSSADC_BIST_EN_SHFT                        0x5
#define HWIO_MSS_GNSSADC_BIST_CONFIG_HITSIDEAL_BMSK                             0x1f
#define HWIO_MSS_GNSSADC_BIST_CONFIG_HITSIDEAL_SHFT                              0x0

#define HWIO_MSS_GNSSADC_BIST_STATUS_1_ADDR                               (MSS_PERPH_REG_BASE      + 0x00000088)
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_RMSK                                0xfffffff
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_IN          \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_STATUS_1_ADDR, HWIO_MSS_GNSSADC_BIST_STATUS_1_RMSK)
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_INM(m)      \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_STATUS_1_ADDR, m)
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_INL_I_MAX_BMSK              0xff80000
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_INL_I_MAX_SHFT                   0x13
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_INL_I_MIN_BMSK                0x7fc00
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_INL_I_MIN_SHFT                    0xa
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_DNL_I_POS_BMSK                  0x3e0
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_DNL_I_POS_SHFT                    0x5
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_DNL_I_NEG_BMSK                   0x1f
#define HWIO_MSS_GNSSADC_BIST_STATUS_1_GNSSADC_DNL_I_NEG_SHFT                    0x0

#define HWIO_MSS_GNSSADC_BIST_STATUS_2_ADDR                               (MSS_PERPH_REG_BASE      + 0x0000008c)
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_RMSK                               0xffffffff
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_IN          \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_STATUS_2_ADDR, HWIO_MSS_GNSSADC_BIST_STATUS_2_RMSK)
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_INM(m)      \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_STATUS_2_ADDR, m)
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_DNL_Q_POS_BMSK             0xf8000000
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_DNL_Q_POS_SHFT                   0x1b
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_GAIN_I_BMSK                 0x7fc0000
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_GAIN_I_SHFT                      0x12
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_INL_Q_MAX_BMSK                0x3fe00
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_INL_Q_MAX_SHFT                    0x9
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_INL_Q_MIN_BMSK                  0x1ff
#define HWIO_MSS_GNSSADC_BIST_STATUS_2_GNSSADC_INL_Q_MIN_SHFT                    0x0

#define HWIO_MSS_GNSSADC_BIST_STATUS_3_ADDR                               (MSS_PERPH_REG_BASE      + 0x00000090)
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_RMSK                               0xffffffff
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_IN          \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_STATUS_3_ADDR, HWIO_MSS_GNSSADC_BIST_STATUS_3_RMSK)
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_INM(m)      \
        in_dword_masked(HWIO_MSS_GNSSADC_BIST_STATUS_3_ADDR, m)
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_DNL_Q_NEG_BMSK             0xf8000000
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_DNL_Q_NEG_SHFT                   0x1b
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_OFFSET_I_BMSK               0x7fc0000
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_OFFSET_I_SHFT                    0x12
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_GAIN_Q_BMSK                   0x3fe00
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_GAIN_Q_SHFT                       0x9
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_OFFSET_Q_BMSK                   0x1ff
#define HWIO_MSS_GNSSADC_BIST_STATUS_3_GNSSADC_OFFSET_Q_SHFT                     0x0

#define HWIO_MSS_BBRX_CTL_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00000094)
#define HWIO_MSS_BBRX_CTL_RMSK                                                   0x3
#define HWIO_MSS_BBRX_CTL_IN          \
        in_dword_masked(HWIO_MSS_BBRX_CTL_ADDR, HWIO_MSS_BBRX_CTL_RMSK)
#define HWIO_MSS_BBRX_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX_CTL_ADDR, m)
#define HWIO_MSS_BBRX_CTL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX_CTL_ADDR,v)
#define HWIO_MSS_BBRX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX_CTL_ADDR,m,v,HWIO_MSS_BBRX_CTL_IN)
#define HWIO_MSS_BBRX_CTL_BBRX_HS_TEST_MUX_CTL_BMSK                              0x3
#define HWIO_MSS_BBRX_CTL_BBRX_HS_TEST_MUX_CTL_SHFT                              0x0

#define HWIO_MSS_DEBUG_CTL_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00000098)
#define HWIO_MSS_DEBUG_CTL_RMSK                                                  0x7
#define HWIO_MSS_DEBUG_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, HWIO_MSS_DEBUG_CTL_RMSK)
#define HWIO_MSS_DEBUG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CTL_IN)
#define HWIO_MSS_DEBUG_CTL_DAC_DISABLE_ON_Q6_DBG_BMSK                            0x4
#define HWIO_MSS_DEBUG_CTL_DAC_DISABLE_ON_Q6_DBG_SHFT                            0x2
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_DBG_BMSK                              0x2
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_DBG_SHFT                              0x1
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_WDOG_BMSK                             0x1
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_WDOG_SHFT                             0x0

#define HWIO_MSS_MPLL1_MODE_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001020)
#define HWIO_MSS_MPLL1_MODE_RMSK                                            0x3fffff
#define HWIO_MSS_MPLL1_MODE_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_MODE_ADDR, HWIO_MSS_MPLL1_MODE_RMSK)
#define HWIO_MSS_MPLL1_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_MODE_ADDR, m)
#define HWIO_MSS_MPLL1_MODE_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_MODE_ADDR,v)
#define HWIO_MSS_MPLL1_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_MODE_ADDR,m,v,HWIO_MSS_MPLL1_MODE_IN)
#define HWIO_MSS_MPLL1_MODE_RESERVE_21_4_BMSK                               0x3ffff0
#define HWIO_MSS_MPLL1_MODE_RESERVE_21_4_SHFT                                    0x4
#define HWIO_MSS_MPLL1_MODE_PLL_PLLTEST_BMSK                                     0x8
#define HWIO_MSS_MPLL1_MODE_PLL_PLLTEST_SHFT                                     0x3
#define HWIO_MSS_MPLL1_MODE_PLL_RESET_N_BMSK                                     0x4
#define HWIO_MSS_MPLL1_MODE_PLL_RESET_N_SHFT                                     0x2
#define HWIO_MSS_MPLL1_MODE_PLL_BYPASSNL_BMSK                                    0x2
#define HWIO_MSS_MPLL1_MODE_PLL_BYPASSNL_SHFT                                    0x1
#define HWIO_MSS_MPLL1_MODE_PLL_OUTCTRL_BMSK                                     0x1
#define HWIO_MSS_MPLL1_MODE_PLL_OUTCTRL_SHFT                                     0x0

#define HWIO_MSS_MPLL1_L_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001024)
#define HWIO_MSS_MPLL1_L_VAL_RMSK                                               0x7f
#define HWIO_MSS_MPLL1_L_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_L_VAL_ADDR, HWIO_MSS_MPLL1_L_VAL_RMSK)
#define HWIO_MSS_MPLL1_L_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_L_VAL_ADDR, m)
#define HWIO_MSS_MPLL1_L_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_L_VAL_ADDR,v)
#define HWIO_MSS_MPLL1_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_L_VAL_ADDR,m,v,HWIO_MSS_MPLL1_L_VAL_IN)
#define HWIO_MSS_MPLL1_L_VAL_PLL_L_BMSK                                         0x7f
#define HWIO_MSS_MPLL1_L_VAL_PLL_L_SHFT                                          0x0

#define HWIO_MSS_MPLL1_M_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001028)
#define HWIO_MSS_MPLL1_M_VAL_RMSK                                            0x7ffff
#define HWIO_MSS_MPLL1_M_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_M_VAL_ADDR, HWIO_MSS_MPLL1_M_VAL_RMSK)
#define HWIO_MSS_MPLL1_M_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_M_VAL_ADDR, m)
#define HWIO_MSS_MPLL1_M_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_M_VAL_ADDR,v)
#define HWIO_MSS_MPLL1_M_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_M_VAL_ADDR,m,v,HWIO_MSS_MPLL1_M_VAL_IN)
#define HWIO_MSS_MPLL1_M_VAL_PLL_M_BMSK                                      0x7ffff
#define HWIO_MSS_MPLL1_M_VAL_PLL_M_SHFT                                          0x0

#define HWIO_MSS_MPLL1_N_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x0000102c)
#define HWIO_MSS_MPLL1_N_VAL_RMSK                                            0x7ffff
#define HWIO_MSS_MPLL1_N_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_N_VAL_ADDR, HWIO_MSS_MPLL1_N_VAL_RMSK)
#define HWIO_MSS_MPLL1_N_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_N_VAL_ADDR, m)
#define HWIO_MSS_MPLL1_N_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_N_VAL_ADDR,v)
#define HWIO_MSS_MPLL1_N_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_N_VAL_ADDR,m,v,HWIO_MSS_MPLL1_N_VAL_IN)
#define HWIO_MSS_MPLL1_N_VAL_PLL_N_BMSK                                      0x7ffff
#define HWIO_MSS_MPLL1_N_VAL_PLL_N_SHFT                                          0x0

#define HWIO_MSS_MPLL1_USER_CTL_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001030)
#define HWIO_MSS_MPLL1_USER_CTL_RMSK                                      0xffffffff
#define HWIO_MSS_MPLL1_USER_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_USER_CTL_ADDR, HWIO_MSS_MPLL1_USER_CTL_RMSK)
#define HWIO_MSS_MPLL1_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_USER_CTL_ADDR, m)
#define HWIO_MSS_MPLL1_USER_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_USER_CTL_ADDR,v)
#define HWIO_MSS_MPLL1_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_USER_CTL_ADDR,m,v,HWIO_MSS_MPLL1_USER_CTL_IN)
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_31_30_BMSK                   0xc0000000
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_31_30_SHFT                         0x1e
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_29_28_BMSK                   0x30000000
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_29_28_SHFT                         0x1c
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_27_25_BMSK                    0xe000000
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_27_25_SHFT                         0x19
#define HWIO_MSS_MPLL1_USER_CTL_MN_EN_BMSK                                 0x1000000
#define HWIO_MSS_MPLL1_USER_CTL_MN_EN_SHFT                                      0x18
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_23_13_BMSK                     0xffe000
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_23_13_SHFT                          0xd
#define HWIO_MSS_MPLL1_USER_CTL_PREDIV2_EN_BMSK                               0x1000
#define HWIO_MSS_MPLL1_USER_CTL_PREDIV2_EN_SHFT                                  0xc
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_11_10_BMSK                        0xc00
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_11_10_SHFT                          0xa
#define HWIO_MSS_MPLL1_USER_CTL_POSTDIV_CTL_BMSK                               0x300
#define HWIO_MSS_MPLL1_USER_CTL_POSTDIV_CTL_SHFT                                 0x8
#define HWIO_MSS_MPLL1_USER_CTL_INV_OUTPUT_BMSK                                 0x80
#define HWIO_MSS_MPLL1_USER_CTL_INV_OUTPUT_SHFT                                  0x7
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BIT_6_5_BMSK                            0x60
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BIT_6_5_SHFT                             0x5
#define HWIO_MSS_MPLL1_USER_CTL_LVTEST_EN_BMSK                                  0x10
#define HWIO_MSS_MPLL1_USER_CTL_LVTEST_EN_SHFT                                   0x4
#define HWIO_MSS_MPLL1_USER_CTL_LVEARLY_EN_BMSK                                  0x8
#define HWIO_MSS_MPLL1_USER_CTL_LVEARLY_EN_SHFT                                  0x3
#define HWIO_MSS_MPLL1_USER_CTL_LVBIST_EN_BMSK                                   0x4
#define HWIO_MSS_MPLL1_USER_CTL_LVBIST_EN_SHFT                                   0x2
#define HWIO_MSS_MPLL1_USER_CTL_LVAUX_EN_BMSK                                    0x2
#define HWIO_MSS_MPLL1_USER_CTL_LVAUX_EN_SHFT                                    0x1
#define HWIO_MSS_MPLL1_USER_CTL_LVMAIN_EN_BMSK                                   0x1
#define HWIO_MSS_MPLL1_USER_CTL_LVMAIN_EN_SHFT                                   0x0

#define HWIO_MSS_MPLL1_CONFIG_CTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00001034)
#define HWIO_MSS_MPLL1_CONFIG_CTL_RMSK                                    0xffffffff
#define HWIO_MSS_MPLL1_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR, HWIO_MSS_MPLL1_CONFIG_CTL_RMSK)
#define HWIO_MSS_MPLL1_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR, m)
#define HWIO_MSS_MPLL1_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR,v)
#define HWIO_MSS_MPLL1_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR,m,v,HWIO_MSS_MPLL1_CONFIG_CTL_IN)
#define HWIO_MSS_MPLL1_CONFIG_CTL_SR2_PLL_FIELDS_BMSK                     0xffffffff
#define HWIO_MSS_MPLL1_CONFIG_CTL_SR2_PLL_FIELDS_SHFT                            0x0

#define HWIO_MSS_MPLL1_TEST_CTL_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001038)
#define HWIO_MSS_MPLL1_TEST_CTL_RMSK                                      0xffffffff
#define HWIO_MSS_MPLL1_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_TEST_CTL_ADDR, HWIO_MSS_MPLL1_TEST_CTL_RMSK)
#define HWIO_MSS_MPLL1_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_TEST_CTL_ADDR, m)
#define HWIO_MSS_MPLL1_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_TEST_CTL_ADDR,v)
#define HWIO_MSS_MPLL1_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_TEST_CTL_ADDR,m,v,HWIO_MSS_MPLL1_TEST_CTL_IN)
#define HWIO_MSS_MPLL1_TEST_CTL_RESERVE_31_10_BMSK                        0xfffffc00
#define HWIO_MSS_MPLL1_TEST_CTL_RESERVE_31_10_SHFT                               0xa
#define HWIO_MSS_MPLL1_TEST_CTL_IEXT_SEL_BMSK                                  0x200
#define HWIO_MSS_MPLL1_TEST_CTL_IEXT_SEL_SHFT                                    0x9
#define HWIO_MSS_MPLL1_TEST_CTL_DTEST_SEL_BMSK                                 0x180
#define HWIO_MSS_MPLL1_TEST_CTL_DTEST_SEL_SHFT                                   0x7
#define HWIO_MSS_MPLL1_TEST_CTL_BYP_TESTAMP_BMSK                                0x40
#define HWIO_MSS_MPLL1_TEST_CTL_BYP_TESTAMP_SHFT                                 0x6
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST1_SEL_BMSK                                 0x30
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST1_SEL_SHFT                                  0x4
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_SEL_BMSK                                  0xc
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_SEL_SHFT                                  0x2
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST1_EN_BMSK                                   0x2
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST1_EN_SHFT                                   0x1
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_EN_BMSK                                   0x1
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_EN_SHFT                                   0x0

#define HWIO_MSS_MPLL1_STATUS_ADDR                                        (MSS_PERPH_REG_BASE      + 0x0000103c)
#define HWIO_MSS_MPLL1_STATUS_RMSK                                           0x3ffff
#define HWIO_MSS_MPLL1_STATUS_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_STATUS_ADDR, HWIO_MSS_MPLL1_STATUS_RMSK)
#define HWIO_MSS_MPLL1_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_STATUS_ADDR, m)
#define HWIO_MSS_MPLL1_STATUS_PLL_ACTIVE_FLAG_BMSK                           0x20000
#define HWIO_MSS_MPLL1_STATUS_PLL_ACTIVE_FLAG_SHFT                              0x11
#define HWIO_MSS_MPLL1_STATUS_PLL_LOCK_DET_BMSK                              0x10000
#define HWIO_MSS_MPLL1_STATUS_PLL_LOCK_DET_SHFT                                 0x10
#define HWIO_MSS_MPLL1_STATUS_PLL_D_BMSK                                      0xffff
#define HWIO_MSS_MPLL1_STATUS_PLL_D_SHFT                                         0x0

#define HWIO_MSS_MPLL2_MODE_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001040)
#define HWIO_MSS_MPLL2_MODE_RMSK                                            0x3fffff
#define HWIO_MSS_MPLL2_MODE_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_MODE_ADDR, HWIO_MSS_MPLL2_MODE_RMSK)
#define HWIO_MSS_MPLL2_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_MODE_ADDR, m)
#define HWIO_MSS_MPLL2_MODE_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_MODE_ADDR,v)
#define HWIO_MSS_MPLL2_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_MODE_ADDR,m,v,HWIO_MSS_MPLL2_MODE_IN)
#define HWIO_MSS_MPLL2_MODE_RESERVE_21_4_BMSK                               0x3ffff0
#define HWIO_MSS_MPLL2_MODE_RESERVE_21_4_SHFT                                    0x4
#define HWIO_MSS_MPLL2_MODE_PLL_PLLTEST_BMSK                                     0x8
#define HWIO_MSS_MPLL2_MODE_PLL_PLLTEST_SHFT                                     0x3
#define HWIO_MSS_MPLL2_MODE_PLL_RESET_N_BMSK                                     0x4
#define HWIO_MSS_MPLL2_MODE_PLL_RESET_N_SHFT                                     0x2
#define HWIO_MSS_MPLL2_MODE_PLL_BYPASSNL_BMSK                                    0x2
#define HWIO_MSS_MPLL2_MODE_PLL_BYPASSNL_SHFT                                    0x1
#define HWIO_MSS_MPLL2_MODE_PLL_OUTCTRL_BMSK                                     0x1
#define HWIO_MSS_MPLL2_MODE_PLL_OUTCTRL_SHFT                                     0x0

#define HWIO_MSS_MPLL2_L_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001044)
#define HWIO_MSS_MPLL2_L_VAL_RMSK                                               0xff
#define HWIO_MSS_MPLL2_L_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_L_VAL_ADDR, HWIO_MSS_MPLL2_L_VAL_RMSK)
#define HWIO_MSS_MPLL2_L_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_L_VAL_ADDR, m)
#define HWIO_MSS_MPLL2_L_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_L_VAL_ADDR,v)
#define HWIO_MSS_MPLL2_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_L_VAL_ADDR,m,v,HWIO_MSS_MPLL2_L_VAL_IN)
#define HWIO_MSS_MPLL2_L_VAL_PLL_L_BMSK                                         0xff
#define HWIO_MSS_MPLL2_L_VAL_PLL_L_SHFT                                          0x0

#define HWIO_MSS_MPLL2_M_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001048)
#define HWIO_MSS_MPLL2_M_VAL_RMSK                                            0x7ffff
#define HWIO_MSS_MPLL2_M_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_M_VAL_ADDR, HWIO_MSS_MPLL2_M_VAL_RMSK)
#define HWIO_MSS_MPLL2_M_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_M_VAL_ADDR, m)
#define HWIO_MSS_MPLL2_M_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_M_VAL_ADDR,v)
#define HWIO_MSS_MPLL2_M_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_M_VAL_ADDR,m,v,HWIO_MSS_MPLL2_M_VAL_IN)
#define HWIO_MSS_MPLL2_M_VAL_PLL_M_BMSK                                      0x7ffff
#define HWIO_MSS_MPLL2_M_VAL_PLL_M_SHFT                                          0x0

#define HWIO_MSS_MPLL2_N_VAL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x0000104c)
#define HWIO_MSS_MPLL2_N_VAL_RMSK                                            0x7ffff
#define HWIO_MSS_MPLL2_N_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_N_VAL_ADDR, HWIO_MSS_MPLL2_N_VAL_RMSK)
#define HWIO_MSS_MPLL2_N_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_N_VAL_ADDR, m)
#define HWIO_MSS_MPLL2_N_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_N_VAL_ADDR,v)
#define HWIO_MSS_MPLL2_N_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_N_VAL_ADDR,m,v,HWIO_MSS_MPLL2_N_VAL_IN)
#define HWIO_MSS_MPLL2_N_VAL_PLL_N_BMSK                                      0x7ffff
#define HWIO_MSS_MPLL2_N_VAL_PLL_N_SHFT                                          0x0

#define HWIO_MSS_MPLL2_USER_CTL_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001050)
#define HWIO_MSS_MPLL2_USER_CTL_RMSK                                      0xfffff3ff
#define HWIO_MSS_MPLL2_USER_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_USER_CTL_ADDR, HWIO_MSS_MPLL2_USER_CTL_RMSK)
#define HWIO_MSS_MPLL2_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_USER_CTL_ADDR, m)
#define HWIO_MSS_MPLL2_USER_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_USER_CTL_ADDR,v)
#define HWIO_MSS_MPLL2_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_USER_CTL_ADDR,m,v,HWIO_MSS_MPLL2_USER_CTL_IN)
#define HWIO_MSS_MPLL2_USER_CTL_RESERVE_31_25_BMSK                        0xfe000000
#define HWIO_MSS_MPLL2_USER_CTL_RESERVE_31_25_SHFT                              0x19
#define HWIO_MSS_MPLL2_USER_CTL_MN_EN_BMSK                                 0x1000000
#define HWIO_MSS_MPLL2_USER_CTL_MN_EN_SHFT                                      0x18
#define HWIO_MSS_MPLL2_USER_CTL_RESERVE_23_21_BMSK                          0xe00000
#define HWIO_MSS_MPLL2_USER_CTL_RESERVE_23_21_SHFT                              0x15
#define HWIO_MSS_MPLL2_USER_CTL_VCO_SEL_BMSK                                0x100000
#define HWIO_MSS_MPLL2_USER_CTL_VCO_SEL_SHFT                                    0x14
#define HWIO_MSS_MPLL2_USER_CTL_RESERVE_19_13_BMSK                           0xfe000
#define HWIO_MSS_MPLL2_USER_CTL_RESERVE_19_13_SHFT                               0xd
#define HWIO_MSS_MPLL2_USER_CTL_PREDIV2_EN_BMSK                               0x1000
#define HWIO_MSS_MPLL2_USER_CTL_PREDIV2_EN_SHFT                                  0xc
#define HWIO_MSS_MPLL2_USER_CTL_POSTDIV_CTL_BMSK                               0x300
#define HWIO_MSS_MPLL2_USER_CTL_POSTDIV_CTL_SHFT                                 0x8
#define HWIO_MSS_MPLL2_USER_CTL_OUTPUT_INV_BMSK                                 0x80
#define HWIO_MSS_MPLL2_USER_CTL_OUTPUT_INV_SHFT                                  0x7
#define HWIO_MSS_MPLL2_USER_CTL_RESERVE_BIT_6_BMSK                              0x40
#define HWIO_MSS_MPLL2_USER_CTL_RESERVE_BIT_6_SHFT                               0x6
#define HWIO_MSS_MPLL2_USER_CTL_RESERVE_BIT_5_BMSK                              0x20
#define HWIO_MSS_MPLL2_USER_CTL_RESERVE_BIT_5_SHFT                               0x5
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_TEST_BMSK                             0x10
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_TEST_SHFT                              0x4
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_EARLY_BMSK                             0x8
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_EARLY_SHFT                             0x3
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_BIST_BMSK                              0x4
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_BIST_SHFT                              0x2
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_AUX_BMSK                               0x2
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_AUX_SHFT                               0x1
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_MAIN_BMSK                              0x1
#define HWIO_MSS_MPLL2_USER_CTL_PLLOUT_LV_MAIN_SHFT                              0x0

#define HWIO_MSS_MPLL2_CONFIG_CTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00001054)
#define HWIO_MSS_MPLL2_CONFIG_CTL_RMSK                                    0xffffffff
#define HWIO_MSS_MPLL2_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_CONFIG_CTL_ADDR, HWIO_MSS_MPLL2_CONFIG_CTL_RMSK)
#define HWIO_MSS_MPLL2_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_CONFIG_CTL_ADDR, m)
#define HWIO_MSS_MPLL2_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_CONFIG_CTL_ADDR,v)
#define HWIO_MSS_MPLL2_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_CONFIG_CTL_ADDR,m,v,HWIO_MSS_MPLL2_CONFIG_CTL_IN)
#define HWIO_MSS_MPLL2_CONFIG_CTL_RESERVE_BITS_31_30_BMSK                 0xc0000000
#define HWIO_MSS_MPLL2_CONFIG_CTL_RESERVE_BITS_31_30_SHFT                       0x1e
#define HWIO_MSS_MPLL2_CONFIG_CTL_FORCE_PFD_UP_BMSK                       0x20000000
#define HWIO_MSS_MPLL2_CONFIG_CTL_FORCE_PFD_UP_SHFT                             0x1d
#define HWIO_MSS_MPLL2_CONFIG_CTL_FORCE_PFD_DOWN_BMSK                     0x10000000
#define HWIO_MSS_MPLL2_CONFIG_CTL_FORCE_PFD_DOWN_SHFT                           0x1c
#define HWIO_MSS_MPLL2_CONFIG_CTL_NMOSC_FREQ_CTRL_BMSK                     0xc000000
#define HWIO_MSS_MPLL2_CONFIG_CTL_NMOSC_FREQ_CTRL_SHFT                          0x1a
#define HWIO_MSS_MPLL2_CONFIG_CTL_PFD_DZSEL_BMSK                           0x3000000
#define HWIO_MSS_MPLL2_CONFIG_CTL_PFD_DZSEL_SHFT                                0x18
#define HWIO_MSS_MPLL2_CONFIG_CTL_NMOSC_EN_BMSK                             0x800000
#define HWIO_MSS_MPLL2_CONFIG_CTL_NMOSC_EN_SHFT                                 0x17
#define HWIO_MSS_MPLL2_CONFIG_CTL_RESERVE_BIT_22_BMSK                       0x400000
#define HWIO_MSS_MPLL2_CONFIG_CTL_RESERVE_BIT_22_SHFT                           0x16
#define HWIO_MSS_MPLL2_CONFIG_CTL_ICP_DIV_BMSK                              0x300000
#define HWIO_MSS_MPLL2_CONFIG_CTL_ICP_DIV_SHFT                                  0x14
#define HWIO_MSS_MPLL2_CONFIG_CTL_IREG_DIV_BMSK                              0xc0000
#define HWIO_MSS_MPLL2_CONFIG_CTL_IREG_DIV_SHFT                                 0x12
#define HWIO_MSS_MPLL2_CONFIG_CTL_CUSEL_BMSK                                 0x30000
#define HWIO_MSS_MPLL2_CONFIG_CTL_CUSEL_SHFT                                    0x10
#define HWIO_MSS_MPLL2_CONFIG_CTL_REF_MODE_BMSK                               0x8000
#define HWIO_MSS_MPLL2_CONFIG_CTL_REF_MODE_SHFT                                  0xf
#define HWIO_MSS_MPLL2_CONFIG_CTL_RESERVE_BIT_14_BMSK                         0x4000
#define HWIO_MSS_MPLL2_CONFIG_CTL_RESERVE_BIT_14_SHFT                            0xe
#define HWIO_MSS_MPLL2_CONFIG_CTL_CFG_LOCKDET_BMSK                            0x3000
#define HWIO_MSS_MPLL2_CONFIG_CTL_CFG_LOCKDET_SHFT                               0xc
#define HWIO_MSS_MPLL2_CONFIG_CTL_FORCE_ISEED_BMSK                             0x800
#define HWIO_MSS_MPLL2_CONFIG_CTL_FORCE_ISEED_SHFT                               0xb
#define HWIO_MSS_MPLL2_CONFIG_CTL_RESERVE_BITS_10_0_BMSK                       0x7ff
#define HWIO_MSS_MPLL2_CONFIG_CTL_RESERVE_BITS_10_0_SHFT                         0x0

#define HWIO_MSS_MPLL2_TEST_CTL_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001058)
#define HWIO_MSS_MPLL2_TEST_CTL_RMSK                                      0xffffffff
#define HWIO_MSS_MPLL2_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_TEST_CTL_ADDR, HWIO_MSS_MPLL2_TEST_CTL_RMSK)
#define HWIO_MSS_MPLL2_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_TEST_CTL_ADDR, m)
#define HWIO_MSS_MPLL2_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_TEST_CTL_ADDR,v)
#define HWIO_MSS_MPLL2_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_TEST_CTL_ADDR,m,v,HWIO_MSS_MPLL2_TEST_CTL_IN)
#define HWIO_MSS_MPLL2_TEST_CTL_RESERVE_31_21_BMSK                        0xffe00000
#define HWIO_MSS_MPLL2_TEST_CTL_RESERVE_31_21_SHFT                              0x15
#define HWIO_MSS_MPLL2_TEST_CTL_NGEN_CFG_BMSK                               0x1c0000
#define HWIO_MSS_MPLL2_TEST_CTL_NGEN_CFG_SHFT                                   0x12
#define HWIO_MSS_MPLL2_TEST_CTL_NGEN_EN_BMSK                                 0x20000
#define HWIO_MSS_MPLL2_TEST_CTL_NGEN_EN_SHFT                                    0x11
#define HWIO_MSS_MPLL2_TEST_CTL_NMOSC_FREQ_CTRL_BMSK                         0x18000
#define HWIO_MSS_MPLL2_TEST_CTL_NMOSC_FREQ_CTRL_SHFT                             0xf
#define HWIO_MSS_MPLL2_TEST_CTL_NMOSC_EN_BMSK                                 0x4000
#define HWIO_MSS_MPLL2_TEST_CTL_NMOSC_EN_SHFT                                    0xe
#define HWIO_MSS_MPLL2_TEST_CTL_FORCE_PFD_UP_BMSK                             0x2000
#define HWIO_MSS_MPLL2_TEST_CTL_FORCE_PFD_UP_SHFT                                0xd
#define HWIO_MSS_MPLL2_TEST_CTL_FORCE_PFD_DOWN_BMSK                           0x1000
#define HWIO_MSS_MPLL2_TEST_CTL_FORCE_PFD_DOWN_SHFT                              0xc
#define HWIO_MSS_MPLL2_TEST_CTL_TEST_OUT_SEL_BMSK                              0x800
#define HWIO_MSS_MPLL2_TEST_CTL_TEST_OUT_SEL_SHFT                                0xb
#define HWIO_MSS_MPLL2_TEST_CTL_ICP_TST_EN_BMSK                                0x400
#define HWIO_MSS_MPLL2_TEST_CTL_ICP_TST_EN_SHFT                                  0xa
#define HWIO_MSS_MPLL2_TEST_CTL_ICP_EXT_SEL_BMSK                               0x200
#define HWIO_MSS_MPLL2_TEST_CTL_ICP_EXT_SEL_SHFT                                 0x9
#define HWIO_MSS_MPLL2_TEST_CTL_DTEST_SEL_BMSK                                 0x180
#define HWIO_MSS_MPLL2_TEST_CTL_DTEST_SEL_SHFT                                   0x7
#define HWIO_MSS_MPLL2_TEST_CTL_BYP_TESTAMP_BMSK                                0x40
#define HWIO_MSS_MPLL2_TEST_CTL_BYP_TESTAMP_SHFT                                 0x6
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST1_SEL_BMSK                                 0x30
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST1_SEL_SHFT                                  0x4
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST0_SEL_BMSK                                  0xc
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST0_SEL_SHFT                                  0x2
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST1_EN_BMSK                                   0x2
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST1_EN_SHFT                                   0x1
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST0_EN_BMSK                                   0x1
#define HWIO_MSS_MPLL2_TEST_CTL_ATEST0_EN_SHFT                                   0x0

#define HWIO_MSS_MPLL2_STATUS_ADDR                                        (MSS_PERPH_REG_BASE      + 0x0000105c)
#define HWIO_MSS_MPLL2_STATUS_RMSK                                           0x3ffff
#define HWIO_MSS_MPLL2_STATUS_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_STATUS_ADDR, HWIO_MSS_MPLL2_STATUS_RMSK)
#define HWIO_MSS_MPLL2_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_STATUS_ADDR, m)
#define HWIO_MSS_MPLL2_STATUS_PLL_ACTIVE_FLAG_BMSK                           0x20000
#define HWIO_MSS_MPLL2_STATUS_PLL_ACTIVE_FLAG_SHFT                              0x11
#define HWIO_MSS_MPLL2_STATUS_PLL_LOCK_DET_BMSK                              0x10000
#define HWIO_MSS_MPLL2_STATUS_PLL_LOCK_DET_SHFT                                 0x10
#define HWIO_MSS_MPLL2_STATUS_PLL_D_BMSK                                      0xffff
#define HWIO_MSS_MPLL2_STATUS_PLL_D_SHFT                                         0x0

#define HWIO_MSS_UIM0_BCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001060)
#define HWIO_MSS_UIM0_BCR_RMSK                                                   0x1
#define HWIO_MSS_UIM0_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_BCR_ADDR, HWIO_MSS_UIM0_BCR_RMSK)
#define HWIO_MSS_UIM0_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_BCR_ADDR, m)
#define HWIO_MSS_UIM0_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_BCR_ADDR,v)
#define HWIO_MSS_UIM0_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_BCR_ADDR,m,v,HWIO_MSS_UIM0_BCR_IN)
#define HWIO_MSS_UIM0_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_MSS_UIM0_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_MSS_UIM1_BCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001064)
#define HWIO_MSS_UIM1_BCR_RMSK                                                   0x1
#define HWIO_MSS_UIM1_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_BCR_ADDR, HWIO_MSS_UIM1_BCR_RMSK)
#define HWIO_MSS_UIM1_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_BCR_ADDR, m)
#define HWIO_MSS_UIM1_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_BCR_ADDR,v)
#define HWIO_MSS_UIM1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_BCR_ADDR,m,v,HWIO_MSS_UIM1_BCR_IN)
#define HWIO_MSS_UIM1_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_MSS_UIM1_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_MSS_Q6SS_BCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001068)
#define HWIO_MSS_Q6SS_BCR_RMSK                                                   0x1
#define HWIO_MSS_Q6SS_BCR_IN          \
        in_dword_masked(HWIO_MSS_Q6SS_BCR_ADDR, HWIO_MSS_Q6SS_BCR_RMSK)
#define HWIO_MSS_Q6SS_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6SS_BCR_ADDR, m)
#define HWIO_MSS_Q6SS_BCR_OUT(v)      \
        out_dword(HWIO_MSS_Q6SS_BCR_ADDR,v)
#define HWIO_MSS_Q6SS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6SS_BCR_ADDR,m,v,HWIO_MSS_Q6SS_BCR_IN)
#define HWIO_MSS_Q6SS_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_MSS_Q6SS_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_MSS_NC_HM_BCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x0000106c)
#define HWIO_MSS_NC_HM_BCR_RMSK                                                  0x1
#define HWIO_MSS_NC_HM_BCR_IN          \
        in_dword_masked(HWIO_MSS_NC_HM_BCR_ADDR, HWIO_MSS_NC_HM_BCR_RMSK)
#define HWIO_MSS_NC_HM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_NC_HM_BCR_ADDR, m)
#define HWIO_MSS_NC_HM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_NC_HM_BCR_ADDR,v)
#define HWIO_MSS_NC_HM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_NC_HM_BCR_ADDR,m,v,HWIO_MSS_NC_HM_BCR_IN)
#define HWIO_MSS_NC_HM_BCR_BLK_ARES_BMSK                                         0x1
#define HWIO_MSS_NC_HM_BCR_BLK_ARES_SHFT                                         0x0

#define HWIO_MSS_COXM_BCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001074)
#define HWIO_MSS_COXM_BCR_RMSK                                                   0x1
#define HWIO_MSS_COXM_BCR_IN          \
        in_dword_masked(HWIO_MSS_COXM_BCR_ADDR, HWIO_MSS_COXM_BCR_RMSK)
#define HWIO_MSS_COXM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_COXM_BCR_ADDR, m)
#define HWIO_MSS_COXM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_COXM_BCR_ADDR,v)
#define HWIO_MSS_COXM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_COXM_BCR_ADDR,m,v,HWIO_MSS_COXM_BCR_IN)
#define HWIO_MSS_COXM_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_MSS_COXM_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_MSS_UIM0_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00001078)
#define HWIO_MSS_UIM0_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_CBCR_ADDR, HWIO_MSS_UIM0_CBCR_RMSK)
#define HWIO_MSS_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_CBCR_ADDR,m,v,HWIO_MSS_UIM0_CBCR_IN)
#define HWIO_MSS_UIM0_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_UIM0_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_UIM0_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_UIM0_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_UIM1_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x0000107c)
#define HWIO_MSS_UIM1_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_CBCR_ADDR, HWIO_MSS_UIM1_CBCR_RMSK)
#define HWIO_MSS_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_CBCR_ADDR,m,v,HWIO_MSS_UIM1_CBCR_IN)
#define HWIO_MSS_UIM1_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_UIM1_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_UIM1_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_UIM1_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_XO_UIM0_CBCR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001080)
#define HWIO_MSS_XO_UIM0_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_XO_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_UIM0_CBCR_ADDR, HWIO_MSS_XO_UIM0_CBCR_RMSK)
#define HWIO_MSS_XO_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_XO_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_XO_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_UIM0_CBCR_ADDR,m,v,HWIO_MSS_XO_UIM0_CBCR_IN)
#define HWIO_MSS_XO_UIM0_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_XO_UIM0_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_XO_UIM0_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_XO_UIM0_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_XO_UIM1_CBCR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001084)
#define HWIO_MSS_XO_UIM1_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_XO_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_UIM1_CBCR_ADDR, HWIO_MSS_XO_UIM1_CBCR_RMSK)
#define HWIO_MSS_XO_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_XO_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_XO_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_UIM1_CBCR_ADDR,m,v,HWIO_MSS_XO_UIM1_CBCR_IN)
#define HWIO_MSS_XO_UIM1_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_XO_UIM1_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_XO_UIM1_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_XO_UIM1_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_XO_MODEM_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001088)
#define HWIO_MSS_XO_MODEM_CBCR_RMSK                                       0x80000000
#define HWIO_MSS_XO_MODEM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_MODEM_CBCR_ADDR, HWIO_MSS_XO_MODEM_CBCR_RMSK)
#define HWIO_MSS_XO_MODEM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_MODEM_CBCR_ADDR, m)
#define HWIO_MSS_XO_MODEM_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_XO_MODEM_CBCR_CLKOFF_SHFT                                      0x1f

#define HWIO_MSS_XO_Q6_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000108c)
#define HWIO_MSS_XO_Q6_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_XO_Q6_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_Q6_CBCR_ADDR, HWIO_MSS_XO_Q6_CBCR_RMSK)
#define HWIO_MSS_XO_Q6_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_Q6_CBCR_ADDR, m)
#define HWIO_MSS_XO_Q6_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_Q6_CBCR_ADDR,v)
#define HWIO_MSS_XO_Q6_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_Q6_CBCR_ADDR,m,v,HWIO_MSS_XO_Q6_CBCR_IN)
#define HWIO_MSS_XO_Q6_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_XO_Q6_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_XO_Q6_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_XO_Q6_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_BUS_UIM0_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001090)
#define HWIO_MSS_BUS_UIM0_CBCR_RMSK                                       0x80007ff1
#define HWIO_MSS_BUS_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM0_CBCR_ADDR, HWIO_MSS_BUS_UIM0_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM0_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM0_CBCR_IN)
#define HWIO_MSS_BUS_UIM0_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_BUS_UIM0_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_CORE_ON_BMSK                         0x4000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_CORE_ON_SHFT                            0xe
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_ON_BMSK                       0x2000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_ON_SHFT                          0xd
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                      0x1000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                         0xc
#define HWIO_MSS_BUS_UIM0_CBCR_WAKEUP_BMSK                                     0xf00
#define HWIO_MSS_BUS_UIM0_CBCR_WAKEUP_SHFT                                       0x8
#define HWIO_MSS_BUS_UIM0_CBCR_SLEEP_BMSK                                       0xf0
#define HWIO_MSS_BUS_UIM0_CBCR_SLEEP_SHFT                                        0x4
#define HWIO_MSS_BUS_UIM0_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_BUS_UIM0_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_BUS_UIM1_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001094)
#define HWIO_MSS_BUS_UIM1_CBCR_RMSK                                       0x80007ff1
#define HWIO_MSS_BUS_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM1_CBCR_ADDR, HWIO_MSS_BUS_UIM1_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM1_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM1_CBCR_IN)
#define HWIO_MSS_BUS_UIM1_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_BUS_UIM1_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_CORE_ON_BMSK                         0x4000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_CORE_ON_SHFT                            0xe
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_ON_BMSK                       0x2000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_ON_SHFT                          0xd
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                      0x1000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                         0xc
#define HWIO_MSS_BUS_UIM1_CBCR_WAKEUP_BMSK                                     0xf00
#define HWIO_MSS_BUS_UIM1_CBCR_WAKEUP_SHFT                                       0x8
#define HWIO_MSS_BUS_UIM1_CBCR_SLEEP_BMSK                                       0xf0
#define HWIO_MSS_BUS_UIM1_CBCR_SLEEP_SHFT                                        0x4
#define HWIO_MSS_BUS_UIM1_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_BUS_UIM1_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_BUS_CSR_CBCR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001098)
#define HWIO_MSS_BUS_CSR_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_BUS_CSR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CSR_CBCR_ADDR, HWIO_MSS_BUS_CSR_CBCR_RMSK)
#define HWIO_MSS_BUS_CSR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CSR_CBCR_ADDR, m)
#define HWIO_MSS_BUS_CSR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CSR_CBCR_ADDR,v)
#define HWIO_MSS_BUS_CSR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CSR_CBCR_ADDR,m,v,HWIO_MSS_BUS_CSR_CBCR_IN)
#define HWIO_MSS_BUS_CSR_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_BUS_CSR_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_BUS_CSR_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_BUS_CSR_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_BUS_BRIDGE_CBCR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x0000109c)
#define HWIO_MSS_BUS_BRIDGE_CBCR_RMSK                                     0x80000001
#define HWIO_MSS_BUS_BRIDGE_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_BRIDGE_CBCR_ADDR, HWIO_MSS_BUS_BRIDGE_CBCR_RMSK)
#define HWIO_MSS_BUS_BRIDGE_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_BRIDGE_CBCR_ADDR, m)
#define HWIO_MSS_BUS_BRIDGE_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_BRIDGE_CBCR_ADDR,v)
#define HWIO_MSS_BUS_BRIDGE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_BRIDGE_CBCR_ADDR,m,v,HWIO_MSS_BUS_BRIDGE_CBCR_IN)
#define HWIO_MSS_BUS_BRIDGE_CBCR_CLKOFF_BMSK                              0x80000000
#define HWIO_MSS_BUS_BRIDGE_CBCR_CLKOFF_SHFT                                    0x1f
#define HWIO_MSS_BUS_BRIDGE_CBCR_CLKEN_BMSK                                      0x1
#define HWIO_MSS_BUS_BRIDGE_CBCR_CLKEN_SHFT                                      0x0

#define HWIO_MSS_BUS_MODEM_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x000010a0)
#define HWIO_MSS_BUS_MODEM_CBCR_RMSK                                      0x80000000
#define HWIO_MSS_BUS_MODEM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MODEM_CBCR_ADDR, HWIO_MSS_BUS_MODEM_CBCR_RMSK)
#define HWIO_MSS_BUS_MODEM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MODEM_CBCR_ADDR, m)
#define HWIO_MSS_BUS_MODEM_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_BUS_MODEM_CBCR_CLKOFF_SHFT                                     0x1f

#define HWIO_MSS_BUS_Q6_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x000010a4)
#define HWIO_MSS_BUS_Q6_CBCR_RMSK                                         0x80000001
#define HWIO_MSS_BUS_Q6_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_Q6_CBCR_ADDR, HWIO_MSS_BUS_Q6_CBCR_RMSK)
#define HWIO_MSS_BUS_Q6_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_Q6_CBCR_ADDR, m)
#define HWIO_MSS_BUS_Q6_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_Q6_CBCR_ADDR,v)
#define HWIO_MSS_BUS_Q6_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_Q6_CBCR_ADDR,m,v,HWIO_MSS_BUS_Q6_CBCR_IN)
#define HWIO_MSS_BUS_Q6_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_BUS_Q6_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_BUS_Q6_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_BUS_Q6_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_ADDR                               (MSS_PERPH_REG_BASE      + 0x000010a8)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_RMSK                               0x80000001
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_ADDR, HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_RMSK)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_ADDR, m)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_ADDR,v)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_ADDR,m,v,HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_IN)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_CLKOFF_BMSK                        0x80000000
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_CLKOFF_SHFT                              0x1f
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_CLKEN_BMSK                                0x1
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CBCR_CLKEN_SHFT                                0x0

#define HWIO_MSS_BUS_CRYPTO_CBCR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x000010ac)
#define HWIO_MSS_BUS_CRYPTO_CBCR_RMSK                                     0x80000001
#define HWIO_MSS_BUS_CRYPTO_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, HWIO_MSS_BUS_CRYPTO_CBCR_RMSK)
#define HWIO_MSS_BUS_CRYPTO_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, m)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,v)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,m,v,HWIO_MSS_BUS_CRYPTO_CBCR_IN)
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_BMSK                              0x80000000
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_SHFT                                    0x1f
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_BMSK                                      0x1
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_SHFT                                      0x0

#define HWIO_MSS_BUS_NAV_CBCR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x000010b0)
#define HWIO_MSS_BUS_NAV_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_BUS_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_NAV_CBCR_ADDR, HWIO_MSS_BUS_NAV_CBCR_RMSK)
#define HWIO_MSS_BUS_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_NAV_CBCR_ADDR, m)
#define HWIO_MSS_BUS_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_NAV_CBCR_ADDR,v)
#define HWIO_MSS_BUS_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_NAV_CBCR_ADDR,m,v,HWIO_MSS_BUS_NAV_CBCR_IN)
#define HWIO_MSS_BUS_NAV_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_BUS_NAV_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_BUS_NAV_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_BUS_NAV_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR                            (MSS_PERPH_REG_BASE      + 0x000010b4)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_RMSK                            0x80000001
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR, HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_RMSK)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR, m)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR,v)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR,m,v,HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_IN)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_CLKOFF_BMSK                     0x80000000
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_CLKOFF_SHFT                           0x1f
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_CLKEN_BMSK                             0x1
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_CLKEN_SHFT                             0x0

#define HWIO_MSS_BUS_ATB_CBCR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x000010b8)
#define HWIO_MSS_BUS_ATB_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_BUS_ATB_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_ATB_CBCR_ADDR, HWIO_MSS_BUS_ATB_CBCR_RMSK)
#define HWIO_MSS_BUS_ATB_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_ATB_CBCR_ADDR, m)
#define HWIO_MSS_BUS_ATB_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_ATB_CBCR_ADDR,v)
#define HWIO_MSS_BUS_ATB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_ATB_CBCR_ADDR,m,v,HWIO_MSS_BUS_ATB_CBCR_IN)
#define HWIO_MSS_BUS_ATB_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_BUS_ATB_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_BUS_ATB_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_BUS_ATB_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_BUS_COXM_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010bc)
#define HWIO_MSS_BUS_COXM_CBCR_RMSK                                       0x80007ff1
#define HWIO_MSS_BUS_COXM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_COXM_CBCR_ADDR, HWIO_MSS_BUS_COXM_CBCR_RMSK)
#define HWIO_MSS_BUS_COXM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_COXM_CBCR_ADDR, m)
#define HWIO_MSS_BUS_COXM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_COXM_CBCR_ADDR,v)
#define HWIO_MSS_BUS_COXM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_COXM_CBCR_ADDR,m,v,HWIO_MSS_BUS_COXM_CBCR_IN)
#define HWIO_MSS_BUS_COXM_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_BUS_COXM_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_CORE_ON_BMSK                         0x4000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_CORE_ON_SHFT                            0xe
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                       0x2000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                          0xd
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                      0x1000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                         0xc
#define HWIO_MSS_BUS_COXM_CBCR_WAKEUP_BMSK                                     0xf00
#define HWIO_MSS_BUS_COXM_CBCR_WAKEUP_SHFT                                       0x8
#define HWIO_MSS_BUS_COXM_CBCR_SLEEP_BMSK                                       0xf0
#define HWIO_MSS_BUS_COXM_CBCR_SLEEP_SHFT                                        0x4
#define HWIO_MSS_BUS_COXM_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_BUS_COXM_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR                              (MSS_PERPH_REG_BASE      + 0x000010c4)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_RMSK                              0x80000001
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR, HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_RMSK)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR, m)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR,v)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR,m,v,HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_IN)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKOFF_BMSK                       0x80000000
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKOFF_SHFT                             0x1f
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKEN_BMSK                               0x1
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKEN_SHFT                               0x0

#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_ADDR                              (MSS_PERPH_REG_BASE      + 0x000010cc)
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_RMSK                              0x80000000
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_COMBODAC_COMP_CBCR_ADDR, HWIO_MSS_BUS_COMBODAC_COMP_CBCR_RMSK)
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_COMBODAC_COMP_CBCR_ADDR, m)
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_CLKOFF_BMSK                       0x80000000
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_CLKOFF_SHFT                             0x1f

#define HWIO_MSS_MODEM_CFG_AHB_CBCR_ADDR                                  (MSS_PERPH_REG_BASE      + 0x000010d4)
#define HWIO_MSS_MODEM_CFG_AHB_CBCR_RMSK                                  0x80000000
#define HWIO_MSS_MODEM_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MODEM_CFG_AHB_CBCR_ADDR, HWIO_MSS_MODEM_CFG_AHB_CBCR_RMSK)
#define HWIO_MSS_MODEM_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MODEM_CFG_AHB_CBCR_ADDR, m)
#define HWIO_MSS_MODEM_CFG_AHB_CBCR_CLKOFF_BMSK                           0x80000000
#define HWIO_MSS_MODEM_CFG_AHB_CBCR_CLKOFF_SHFT                                 0x1f

#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_ADDR                                 (MSS_PERPH_REG_BASE      + 0x000010d8)
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_RMSK                                 0x80000000
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MODEM_SNOC_AXI_CBCR_ADDR, HWIO_MSS_MODEM_SNOC_AXI_CBCR_RMSK)
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MODEM_SNOC_AXI_CBCR_ADDR, m)
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_CLKOFF_BMSK                          0x80000000
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_CLKOFF_SHFT                                0x1f

#define HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x000010dc)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_RMSK                                   0x80000001
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_IN          \
        in_dword_masked(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR, HWIO_MSS_NAV_SNOC_AXI_CBCR_RMSK)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR, m)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR,v)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR,m,v,HWIO_MSS_NAV_SNOC_AXI_CBCR_IN)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKOFF_BMSK                            0x80000000
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKOFF_SHFT                                  0x1f
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKEN_BMSK                                    0x1
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKEN_SHFT                                    0x0

#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_ADDR                                (MSS_PERPH_REG_BASE      + 0x000010e0)
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_RMSK                                0x80000001
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_IN          \
        in_dword_masked(HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_ADDR, HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_RMSK)
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_ADDR, m)
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_ADDR,v)
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_ADDR,m,v,HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_IN)
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_CLKOFF_BMSK                         0x80000000
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_CLKOFF_SHFT                               0x1f
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_CLKEN_BMSK                                 0x1
#define HWIO_MSS_VMIDMT_SNOC_AXI_CBCR_CLKEN_SHFT                                 0x0

#define HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_ADDR                               (MSS_PERPH_REG_BASE      + 0x000010e4)
#define HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_RMSK                               0x80000001
#define HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_ADDR, HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_RMSK)
#define HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_ADDR, m)
#define HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_ADDR,v)
#define HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_ADDR,m,v,HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_IN)
#define HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_CLKOFF_BMSK                        0x80000000
#define HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_CLKOFF_SHFT                              0x1f
#define HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_CLKEN_BMSK                                0x1
#define HWIO_MSS_MPLL2_MAIN_MODEM_CBCR_CLKEN_SHFT                                0x0

#define HWIO_MSS_MPLL1_MAIN_CBCR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x000010e8)
#define HWIO_MSS_MPLL1_MAIN_CBCR_RMSK                                     0x80000001
#define HWIO_MSS_MPLL1_MAIN_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_MAIN_CBCR_ADDR, HWIO_MSS_MPLL1_MAIN_CBCR_RMSK)
#define HWIO_MSS_MPLL1_MAIN_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_MAIN_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_MAIN_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_MAIN_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_MAIN_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_MAIN_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_MAIN_CBCR_IN)
#define HWIO_MSS_MPLL1_MAIN_CBCR_CLKOFF_BMSK                              0x80000000
#define HWIO_MSS_MPLL1_MAIN_CBCR_CLKOFF_SHFT                                    0x1f
#define HWIO_MSS_MPLL1_MAIN_CBCR_CLKEN_BMSK                                      0x1
#define HWIO_MSS_MPLL1_MAIN_CBCR_CLKEN_SHFT                                      0x0

#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_ADDR                                 (MSS_PERPH_REG_BASE      + 0x000010ec)
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_RMSK                                 0x80000000
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_MAIN_BUS_CBCR_ADDR, HWIO_MSS_MPLL1_MAIN_BUS_CBCR_RMSK)
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_MAIN_BUS_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_CLKOFF_BMSK                          0x80000000
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_CLKOFF_SHFT                                0x1f

#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR                               (MSS_PERPH_REG_BASE      + 0x000010f0)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_RMSK                               0x80000001
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR, HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_RMSK)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_IN)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_CLKOFF_BMSK                        0x80000000
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_CLKOFF_SHFT                              0x1f
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_CLKEN_BMSK                                0x1
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_CLKEN_SHFT                                0x0

#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR                               (MSS_PERPH_REG_BASE      + 0x000010f4)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_RMSK                               0x80000001
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR, HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_RMSK)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_IN)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_CLKOFF_BMSK                        0x80000000
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_CLKOFF_SHFT                              0x1f
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_CLKEN_BMSK                                0x1
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_CLKEN_SHFT                                0x0

#define HWIO_MSS_BIT_COXM_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010f8)
#define HWIO_MSS_BIT_COXM_CBCR_RMSK                                       0x80000001
#define HWIO_MSS_BIT_COXM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_CBCR_ADDR, HWIO_MSS_BIT_COXM_CBCR_RMSK)
#define HWIO_MSS_BIT_COXM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_CBCR_ADDR, m)
#define HWIO_MSS_BIT_COXM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_CBCR_ADDR,v)
#define HWIO_MSS_BIT_COXM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_CBCR_ADDR,m,v,HWIO_MSS_BIT_COXM_CBCR_IN)
#define HWIO_MSS_BIT_COXM_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_BIT_COXM_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_BIT_COXM_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_BIT_COXM_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_SLEEP_Q6_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010fc)
#define HWIO_MSS_SLEEP_Q6_CBCR_RMSK                                       0x80000001
#define HWIO_MSS_SLEEP_Q6_CBCR_IN          \
        in_dword_masked(HWIO_MSS_SLEEP_Q6_CBCR_ADDR, HWIO_MSS_SLEEP_Q6_CBCR_RMSK)
#define HWIO_MSS_SLEEP_Q6_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_SLEEP_Q6_CBCR_ADDR, m)
#define HWIO_MSS_SLEEP_Q6_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_SLEEP_Q6_CBCR_ADDR,v)
#define HWIO_MSS_SLEEP_Q6_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_SLEEP_Q6_CBCR_ADDR,m,v,HWIO_MSS_SLEEP_Q6_CBCR_IN)
#define HWIO_MSS_SLEEP_Q6_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_SLEEP_Q6_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_SLEEP_Q6_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_SLEEP_Q6_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR                           (MSS_PERPH_REG_BASE      + 0x00001100)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_RMSK                           0x80000001
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR, HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_RMSK)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_IN)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_CLKOFF_BMSK                    0x80000000
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_CLKOFF_SHFT                          0x1f
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_CLKEN_BMSK                            0x1
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_CLKEN_SHFT                            0x0

#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR                           (MSS_PERPH_REG_BASE      + 0x00001104)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_RMSK                           0x80000001
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR, HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_RMSK)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_IN)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_CLKOFF_BMSK                    0x80000000
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_CLKOFF_SHFT                          0x1f
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_CLKEN_BMSK                            0x1
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_CLKEN_SHFT                            0x0

#define HWIO_MSS_BUS_CMD_RCGR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001108)
#define HWIO_MSS_BUS_CMD_RCGR_RMSK                                        0x80000001
#define HWIO_MSS_BUS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, HWIO_MSS_BUS_CMD_RCGR_RMSK)
#define HWIO_MSS_BUS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CMD_RCGR_ADDR,m,v,HWIO_MSS_BUS_CMD_RCGR_IN)
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_BMSK                               0x80000000
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_SHFT                                     0x1f
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_BMSK                                        0x1
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_SHFT                                        0x0

#define HWIO_MSS_BUS_CFG_RCGR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x0000110c)
#define HWIO_MSS_BUS_CFG_RCGR_RMSK                                             0x71f
#define HWIO_MSS_BUS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, HWIO_MSS_BUS_CFG_RCGR_RMSK)
#define HWIO_MSS_BUS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CFG_RCGR_ADDR,m,v,HWIO_MSS_BUS_CFG_RCGR_IN)
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_BMSK                                     0x700
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_SHFT                                       0x8
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_BMSK                                      0x1f
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_SHFT                                       0x0

#define HWIO_MSS_Q6_CMD_RCGR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001110)
#define HWIO_MSS_Q6_CMD_RCGR_RMSK                                         0x80000003
#define HWIO_MSS_Q6_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_Q6_CMD_RCGR_ADDR, HWIO_MSS_Q6_CMD_RCGR_RMSK)
#define HWIO_MSS_Q6_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6_CMD_RCGR_ADDR, m)
#define HWIO_MSS_Q6_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_Q6_CMD_RCGR_ADDR,v)
#define HWIO_MSS_Q6_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6_CMD_RCGR_ADDR,m,v,HWIO_MSS_Q6_CMD_RCGR_IN)
#define HWIO_MSS_Q6_CMD_RCGR_ROOT_OFF_BMSK                                0x80000000
#define HWIO_MSS_Q6_CMD_RCGR_ROOT_OFF_SHFT                                      0x1f
#define HWIO_MSS_Q6_CMD_RCGR_ROOT_EN_BMSK                                        0x2
#define HWIO_MSS_Q6_CMD_RCGR_ROOT_EN_SHFT                                        0x1
#define HWIO_MSS_Q6_CMD_RCGR_UPDATE_BMSK                                         0x1
#define HWIO_MSS_Q6_CMD_RCGR_UPDATE_SHFT                                         0x0

#define HWIO_MSS_Q6_CFG_RCGR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001114)
#define HWIO_MSS_Q6_CFG_RCGR_RMSK                                              0x71f
#define HWIO_MSS_Q6_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_Q6_CFG_RCGR_ADDR, HWIO_MSS_Q6_CFG_RCGR_RMSK)
#define HWIO_MSS_Q6_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6_CFG_RCGR_ADDR, m)
#define HWIO_MSS_Q6_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_Q6_CFG_RCGR_ADDR,v)
#define HWIO_MSS_Q6_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6_CFG_RCGR_ADDR,m,v,HWIO_MSS_Q6_CFG_RCGR_IN)
#define HWIO_MSS_Q6_CFG_RCGR_SRC_SEL_BMSK                                      0x700
#define HWIO_MSS_Q6_CFG_RCGR_SRC_SEL_SHFT                                        0x8
#define HWIO_MSS_Q6_CFG_RCGR_SRC_DIV_BMSK                                       0x1f
#define HWIO_MSS_Q6_CFG_RCGR_SRC_DIV_SHFT                                        0x0

#define HWIO_MSS_UIM_CMD_RCGR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001118)
#define HWIO_MSS_UIM_CMD_RCGR_RMSK                                        0x80000003
#define HWIO_MSS_UIM_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM_CMD_RCGR_ADDR, HWIO_MSS_UIM_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM_CMD_RCGR_IN)
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_OFF_BMSK                               0x80000000
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_OFF_SHFT                                     0x1f
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_EN_BMSK                                       0x2
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_EN_SHFT                                       0x1
#define HWIO_MSS_UIM_CMD_RCGR_UPDATE_BMSK                                        0x1
#define HWIO_MSS_UIM_CMD_RCGR_UPDATE_SHFT                                        0x0

#define HWIO_MSS_UIM_CFG_RCGR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x0000111c)
#define HWIO_MSS_UIM_CFG_RCGR_RMSK                                              0x1f
#define HWIO_MSS_UIM_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM_CFG_RCGR_ADDR, HWIO_MSS_UIM_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM_CFG_RCGR_IN)
#define HWIO_MSS_UIM_CFG_RCGR_SRC_DIV_BMSK                                      0x1f
#define HWIO_MSS_UIM_CFG_RCGR_SRC_DIV_SHFT                                       0x0

#define HWIO_MSS_BBRX0_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001120)
#define HWIO_MSS_BBRX0_CBCR_RMSK                                          0x80000000
#define HWIO_MSS_BBRX0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BBRX0_CBCR_ADDR, HWIO_MSS_BBRX0_CBCR_RMSK)
#define HWIO_MSS_BBRX0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX0_CBCR_ADDR, m)
#define HWIO_MSS_BBRX0_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_BBRX0_CBCR_CLKOFF_SHFT                                         0x1f

#define HWIO_MSS_BBRX1_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001124)
#define HWIO_MSS_BBRX1_CBCR_RMSK                                          0x80000000
#define HWIO_MSS_BBRX1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BBRX1_CBCR_ADDR, HWIO_MSS_BBRX1_CBCR_RMSK)
#define HWIO_MSS_BBRX1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX1_CBCR_ADDR, m)
#define HWIO_MSS_BBRX1_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_BBRX1_CBCR_CLKOFF_SHFT                                         0x1f

#define HWIO_MSS_RESERVE_01_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001130)
#define HWIO_MSS_RESERVE_01_RMSK                                          0xffffffff
#define HWIO_MSS_RESERVE_01_IN          \
        in_dword_masked(HWIO_MSS_RESERVE_01_ADDR, HWIO_MSS_RESERVE_01_RMSK)
#define HWIO_MSS_RESERVE_01_INM(m)      \
        in_dword_masked(HWIO_MSS_RESERVE_01_ADDR, m)
#define HWIO_MSS_RESERVE_01_OUT(v)      \
        out_dword(HWIO_MSS_RESERVE_01_ADDR,v)
#define HWIO_MSS_RESERVE_01_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RESERVE_01_ADDR,m,v,HWIO_MSS_RESERVE_01_IN)
#define HWIO_MSS_RESERVE_01_MSS_RESERVE_01_BMSK                           0xffffffff
#define HWIO_MSS_RESERVE_01_MSS_RESERVE_01_SHFT                                  0x0

#define HWIO_MSS_RESERVE_02_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001134)
#define HWIO_MSS_RESERVE_02_RMSK                                          0xffffffff
#define HWIO_MSS_RESERVE_02_IN          \
        in_dword_masked(HWIO_MSS_RESERVE_02_ADDR, HWIO_MSS_RESERVE_02_RMSK)
#define HWIO_MSS_RESERVE_02_INM(m)      \
        in_dword_masked(HWIO_MSS_RESERVE_02_ADDR, m)
#define HWIO_MSS_RESERVE_02_OUT(v)      \
        out_dword(HWIO_MSS_RESERVE_02_ADDR,v)
#define HWIO_MSS_RESERVE_02_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RESERVE_02_ADDR,m,v,HWIO_MSS_RESERVE_02_IN)
#define HWIO_MSS_RESERVE_02_MSS_RESERVE_02_BMSK                           0xffffffff
#define HWIO_MSS_RESERVE_02_MSS_RESERVE_02_SHFT                                  0x0

#define HWIO_MSS_BBRX0_MISC_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001138)
#define HWIO_MSS_BBRX0_MISC_RMSK                                                 0xf
#define HWIO_MSS_BBRX0_MISC_IN          \
        in_dword_masked(HWIO_MSS_BBRX0_MISC_ADDR, HWIO_MSS_BBRX0_MISC_RMSK)
#define HWIO_MSS_BBRX0_MISC_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX0_MISC_ADDR, m)
#define HWIO_MSS_BBRX0_MISC_OUT(v)      \
        out_dword(HWIO_MSS_BBRX0_MISC_ADDR,v)
#define HWIO_MSS_BBRX0_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX0_MISC_ADDR,m,v,HWIO_MSS_BBRX0_MISC_IN)
#define HWIO_MSS_BBRX0_MISC_SRC_DIV_BMSK                                         0xf
#define HWIO_MSS_BBRX0_MISC_SRC_DIV_SHFT                                         0x0

#define HWIO_MSS_BBRX1_MISC_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000113c)
#define HWIO_MSS_BBRX1_MISC_RMSK                                                 0xf
#define HWIO_MSS_BBRX1_MISC_IN          \
        in_dword_masked(HWIO_MSS_BBRX1_MISC_ADDR, HWIO_MSS_BBRX1_MISC_RMSK)
#define HWIO_MSS_BBRX1_MISC_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX1_MISC_ADDR, m)
#define HWIO_MSS_BBRX1_MISC_OUT(v)      \
        out_dword(HWIO_MSS_BBRX1_MISC_ADDR,v)
#define HWIO_MSS_BBRX1_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX1_MISC_ADDR,m,v,HWIO_MSS_BBRX1_MISC_IN)
#define HWIO_MSS_BBRX1_MISC_SRC_DIV_BMSK                                         0xf
#define HWIO_MSS_BBRX1_MISC_SRC_DIV_SHFT                                         0x0

#define HWIO_MSS_BIT_COXM_DIV_MISC_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00001148)
#define HWIO_MSS_BIT_COXM_DIV_MISC_RMSK                                          0xf
#define HWIO_MSS_BIT_COXM_DIV_MISC_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_DIV_MISC_ADDR, HWIO_MSS_BIT_COXM_DIV_MISC_RMSK)
#define HWIO_MSS_BIT_COXM_DIV_MISC_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_DIV_MISC_ADDR, m)
#define HWIO_MSS_BIT_COXM_DIV_MISC_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_DIV_MISC_ADDR,v)
#define HWIO_MSS_BIT_COXM_DIV_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_DIV_MISC_ADDR,m,v,HWIO_MSS_BIT_COXM_DIV_MISC_IN)
#define HWIO_MSS_BIT_COXM_DIV_MISC_SRC_DIV_BMSK                                  0xf
#define HWIO_MSS_BIT_COXM_DIV_MISC_SRC_DIV_SHFT                                  0x0

#define HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00001150)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_RMSK                                   0x80000003
#define HWIO_MSS_UIM0_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM0_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM0_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_OFF_BMSK                          0x80000000
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_OFF_SHFT                                0x1f
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_EN_BMSK                                  0x2
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_EN_SHFT                                  0x1
#define HWIO_MSS_UIM0_MND_CMD_RCGR_UPDATE_BMSK                                   0x1
#define HWIO_MSS_UIM0_MND_CMD_RCGR_UPDATE_SHFT                                   0x0

#define HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00001154)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_RMSK                                       0x3000
#define HWIO_MSS_UIM0_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM0_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM0_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_MODE_BMSK                                  0x3000
#define HWIO_MSS_UIM0_MND_CFG_RCGR_MODE_SHFT                                     0xc

#define HWIO_MSS_UIM0_MND_M_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001158)
#define HWIO_MSS_UIM0_MND_M_RMSK                                              0xffff
#define HWIO_MSS_UIM0_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_M_ADDR, HWIO_MSS_UIM0_MND_M_RMSK)
#define HWIO_MSS_UIM0_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_M_ADDR, m)
#define HWIO_MSS_UIM0_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_M_ADDR,v)
#define HWIO_MSS_UIM0_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_M_ADDR,m,v,HWIO_MSS_UIM0_MND_M_IN)
#define HWIO_MSS_UIM0_MND_M_M_VALUE_BMSK                                      0xffff
#define HWIO_MSS_UIM0_MND_M_M_VALUE_SHFT                                         0x0

#define HWIO_MSS_UIM0_MND_N_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000115c)
#define HWIO_MSS_UIM0_MND_N_RMSK                                              0xffff
#define HWIO_MSS_UIM0_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_N_ADDR, HWIO_MSS_UIM0_MND_N_RMSK)
#define HWIO_MSS_UIM0_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_N_ADDR, m)
#define HWIO_MSS_UIM0_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_N_ADDR,v)
#define HWIO_MSS_UIM0_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_N_ADDR,m,v,HWIO_MSS_UIM0_MND_N_IN)
#define HWIO_MSS_UIM0_MND_N_N_VALUE_BMSK                                      0xffff
#define HWIO_MSS_UIM0_MND_N_N_VALUE_SHFT                                         0x0

#define HWIO_MSS_UIM0_MND_D_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001160)
#define HWIO_MSS_UIM0_MND_D_RMSK                                              0xffff
#define HWIO_MSS_UIM0_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_D_ADDR, HWIO_MSS_UIM0_MND_D_RMSK)
#define HWIO_MSS_UIM0_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_D_ADDR, m)
#define HWIO_MSS_UIM0_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_D_ADDR,v)
#define HWIO_MSS_UIM0_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_D_ADDR,m,v,HWIO_MSS_UIM0_MND_D_IN)
#define HWIO_MSS_UIM0_MND_D_D_VALUE_BMSK                                      0xffff
#define HWIO_MSS_UIM0_MND_D_D_VALUE_SHFT                                         0x0

#define HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00001164)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_RMSK                                   0x80000003
#define HWIO_MSS_UIM1_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM1_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM1_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_OFF_BMSK                          0x80000000
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_OFF_SHFT                                0x1f
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_EN_BMSK                                  0x2
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_EN_SHFT                                  0x1
#define HWIO_MSS_UIM1_MND_CMD_RCGR_UPDATE_BMSK                                   0x1
#define HWIO_MSS_UIM1_MND_CMD_RCGR_UPDATE_SHFT                                   0x0

#define HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00001168)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_RMSK                                       0x3000
#define HWIO_MSS_UIM1_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM1_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM1_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_MODE_BMSK                                  0x3000
#define HWIO_MSS_UIM1_MND_CFG_RCGR_MODE_SHFT                                     0xc

#define HWIO_MSS_UIM1_MND_M_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000116c)
#define HWIO_MSS_UIM1_MND_M_RMSK                                              0xffff
#define HWIO_MSS_UIM1_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_M_ADDR, HWIO_MSS_UIM1_MND_M_RMSK)
#define HWIO_MSS_UIM1_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_M_ADDR, m)
#define HWIO_MSS_UIM1_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_M_ADDR,v)
#define HWIO_MSS_UIM1_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_M_ADDR,m,v,HWIO_MSS_UIM1_MND_M_IN)
#define HWIO_MSS_UIM1_MND_M_M_VALUE_BMSK                                      0xffff
#define HWIO_MSS_UIM1_MND_M_M_VALUE_SHFT                                         0x0

#define HWIO_MSS_UIM1_MND_N_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001170)
#define HWIO_MSS_UIM1_MND_N_RMSK                                              0xffff
#define HWIO_MSS_UIM1_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_N_ADDR, HWIO_MSS_UIM1_MND_N_RMSK)
#define HWIO_MSS_UIM1_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_N_ADDR, m)
#define HWIO_MSS_UIM1_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_N_ADDR,v)
#define HWIO_MSS_UIM1_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_N_ADDR,m,v,HWIO_MSS_UIM1_MND_N_IN)
#define HWIO_MSS_UIM1_MND_N_N_VALUE_BMSK                                      0xffff
#define HWIO_MSS_UIM1_MND_N_N_VALUE_SHFT                                         0x0

#define HWIO_MSS_UIM1_MND_D_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001174)
#define HWIO_MSS_UIM1_MND_D_RMSK                                              0xffff
#define HWIO_MSS_UIM1_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_D_ADDR, HWIO_MSS_UIM1_MND_D_RMSK)
#define HWIO_MSS_UIM1_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_D_ADDR, m)
#define HWIO_MSS_UIM1_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_D_ADDR,v)
#define HWIO_MSS_UIM1_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_D_ADDR,m,v,HWIO_MSS_UIM1_MND_D_IN)
#define HWIO_MSS_UIM1_MND_D_D_VALUE_BMSK                                      0xffff
#define HWIO_MSS_UIM1_MND_D_D_VALUE_SHFT                                         0x0

#define HWIO_MSS_UIM2_BCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001178)
#define HWIO_MSS_UIM2_BCR_RMSK                                                   0x1
#define HWIO_MSS_UIM2_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_BCR_ADDR, HWIO_MSS_UIM2_BCR_RMSK)
#define HWIO_MSS_UIM2_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_BCR_ADDR, m)
#define HWIO_MSS_UIM2_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_BCR_ADDR,v)
#define HWIO_MSS_UIM2_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_BCR_ADDR,m,v,HWIO_MSS_UIM2_BCR_IN)
#define HWIO_MSS_UIM2_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_MSS_UIM2_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_MSS_UIM2_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x0000117c)
#define HWIO_MSS_UIM2_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_CBCR_ADDR, HWIO_MSS_UIM2_CBCR_RMSK)
#define HWIO_MSS_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_CBCR_ADDR,m,v,HWIO_MSS_UIM2_CBCR_IN)
#define HWIO_MSS_UIM2_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_UIM2_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_UIM2_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_UIM2_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_XO_UIM2_CBCR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001180)
#define HWIO_MSS_XO_UIM2_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_XO_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_UIM2_CBCR_ADDR, HWIO_MSS_XO_UIM2_CBCR_RMSK)
#define HWIO_MSS_XO_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_XO_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_XO_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_UIM2_CBCR_ADDR,m,v,HWIO_MSS_XO_UIM2_CBCR_IN)
#define HWIO_MSS_XO_UIM2_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_XO_UIM2_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_XO_UIM2_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_XO_UIM2_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_BUS_UIM2_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001184)
#define HWIO_MSS_BUS_UIM2_CBCR_RMSK                                       0x80007ff1
#define HWIO_MSS_BUS_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM2_CBCR_ADDR, HWIO_MSS_BUS_UIM2_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM2_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM2_CBCR_IN)
#define HWIO_MSS_BUS_UIM2_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_BUS_UIM2_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_CORE_ON_BMSK                         0x4000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_CORE_ON_SHFT                            0xe
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_ON_BMSK                       0x2000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_ON_SHFT                          0xd
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                      0x1000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                         0xc
#define HWIO_MSS_BUS_UIM2_CBCR_WAKEUP_BMSK                                     0xf00
#define HWIO_MSS_BUS_UIM2_CBCR_WAKEUP_SHFT                                       0x8
#define HWIO_MSS_BUS_UIM2_CBCR_SLEEP_BMSK                                       0xf0
#define HWIO_MSS_BUS_UIM2_CBCR_SLEEP_SHFT                                        0x4
#define HWIO_MSS_BUS_UIM2_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_BUS_UIM2_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00001188)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_RMSK                                   0x80000003
#define HWIO_MSS_UIM2_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM2_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM2_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_OFF_BMSK                          0x80000000
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_OFF_SHFT                                0x1f
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_EN_BMSK                                  0x2
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_EN_SHFT                                  0x1
#define HWIO_MSS_UIM2_MND_CMD_RCGR_UPDATE_BMSK                                   0x1
#define HWIO_MSS_UIM2_MND_CMD_RCGR_UPDATE_SHFT                                   0x0

#define HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x0000118c)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_RMSK                                       0x3000
#define HWIO_MSS_UIM2_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM2_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM2_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_MODE_BMSK                                  0x3000
#define HWIO_MSS_UIM2_MND_CFG_RCGR_MODE_SHFT                                     0xc

#define HWIO_MSS_UIM2_MND_M_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001190)
#define HWIO_MSS_UIM2_MND_M_RMSK                                              0xffff
#define HWIO_MSS_UIM2_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_M_ADDR, HWIO_MSS_UIM2_MND_M_RMSK)
#define HWIO_MSS_UIM2_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_M_ADDR, m)
#define HWIO_MSS_UIM2_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_M_ADDR,v)
#define HWIO_MSS_UIM2_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_M_ADDR,m,v,HWIO_MSS_UIM2_MND_M_IN)
#define HWIO_MSS_UIM2_MND_M_M_VALUE_BMSK                                      0xffff
#define HWIO_MSS_UIM2_MND_M_M_VALUE_SHFT                                         0x0

#define HWIO_MSS_UIM2_MND_N_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001194)
#define HWIO_MSS_UIM2_MND_N_RMSK                                              0xffff
#define HWIO_MSS_UIM2_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_N_ADDR, HWIO_MSS_UIM2_MND_N_RMSK)
#define HWIO_MSS_UIM2_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_N_ADDR, m)
#define HWIO_MSS_UIM2_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_N_ADDR,v)
#define HWIO_MSS_UIM2_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_N_ADDR,m,v,HWIO_MSS_UIM2_MND_N_IN)
#define HWIO_MSS_UIM2_MND_N_N_VALUE_BMSK                                      0xffff
#define HWIO_MSS_UIM2_MND_N_N_VALUE_SHFT                                         0x0

#define HWIO_MSS_UIM2_MND_D_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001198)
#define HWIO_MSS_UIM2_MND_D_RMSK                                              0xffff
#define HWIO_MSS_UIM2_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_D_ADDR, HWIO_MSS_UIM2_MND_D_RMSK)
#define HWIO_MSS_UIM2_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_D_ADDR, m)
#define HWIO_MSS_UIM2_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_D_ADDR,v)
#define HWIO_MSS_UIM2_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_D_ADDR,m,v,HWIO_MSS_UIM2_MND_D_IN)
#define HWIO_MSS_UIM2_MND_D_D_VALUE_BMSK                                      0xffff
#define HWIO_MSS_UIM2_MND_D_D_VALUE_SHFT                                         0x0

#define HWIO_MSS_TCSR_ACC_SEL_ADDR                                        (MSS_PERPH_REG_BASE      + 0x0000f000)
#define HWIO_MSS_TCSR_ACC_SEL_RMSK                                               0x3
#define HWIO_MSS_TCSR_ACC_SEL_IN          \
        in_dword_masked(HWIO_MSS_TCSR_ACC_SEL_ADDR, HWIO_MSS_TCSR_ACC_SEL_RMSK)
#define HWIO_MSS_TCSR_ACC_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_TCSR_ACC_SEL_ADDR, m)
#define HWIO_MSS_TCSR_ACC_SEL_OUT(v)      \
        out_dword(HWIO_MSS_TCSR_ACC_SEL_ADDR,v)
#define HWIO_MSS_TCSR_ACC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_TCSR_ACC_SEL_ADDR,m,v,HWIO_MSS_TCSR_ACC_SEL_IN)
#define HWIO_MSS_TCSR_ACC_SEL_ACC_MEM_SEL_BMSK                                   0x3
#define HWIO_MSS_TCSR_ACC_SEL_ACC_MEM_SEL_SHFT                                   0x0

/*----------------------------------------------------------------------------
 * MODULE: TLMM_CSR
 *--------------------------------------------------------------------------*/

#define TLMM_CSR_REG_BASE                                                (TLMM_BASE      + 0x00010000)

#define HWIO_TLMM_GPIO_CFGn_ADDR(n)                                      (TLMM_CSR_REG_BASE      + 0x00001000 + 0x10 * (n))
#define HWIO_TLMM_GPIO_CFGn_RMSK                                              0x7ff
#define HWIO_TLMM_GPIO_CFGn_MAXn                                                116
#define HWIO_TLMM_GPIO_CFGn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(n), HWIO_TLMM_GPIO_CFGn_RMSK)
#define HWIO_TLMM_GPIO_CFGn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_CFGn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_CFGn_ADDR(n),val)
#define HWIO_TLMM_GPIO_CFGn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFGn_ADDR(n),mask,val,HWIO_TLMM_GPIO_CFGn_INI(n))
#define HWIO_TLMM_GPIO_CFGn_GPIO_HIHYS_EN_BMSK                                0x400
#define HWIO_TLMM_GPIO_CFGn_GPIO_HIHYS_EN_SHFT                                  0xa
#define HWIO_TLMM_GPIO_CFGn_GPIO_OE_BMSK                                      0x200
#define HWIO_TLMM_GPIO_CFGn_GPIO_OE_SHFT                                        0x9
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_BMSK                                 0x1c0
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_SHFT                                   0x6
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_BMSK                                      0x3c
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_SHFT                                       0x2
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_BMSK                                      0x3
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_SHFT                                      0x0

#define HWIO_TLMM_GPIO_IN_OUTn_ADDR(n)                                   (TLMM_CSR_REG_BASE      + 0x00001004 + 0x10 * (n))
#define HWIO_TLMM_GPIO_IN_OUTn_RMSK                                             0x3
#define HWIO_TLMM_GPIO_IN_OUTn_MAXn                                             116
#define HWIO_TLMM_GPIO_IN_OUTn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n), HWIO_TLMM_GPIO_IN_OUTn_RMSK)
#define HWIO_TLMM_GPIO_IN_OUTn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n),val)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n),mask,val,HWIO_TLMM_GPIO_IN_OUTn_INI(n))
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_BMSK                                    0x2
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_SHFT                                    0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_BMSK                                     0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_SHFT                                     0x0

#define HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n)                                 (TLMM_CSR_REG_BASE      + 0x00001008 + 0x10 * (n))
#define HWIO_TLMM_GPIO_INTR_CFGn_RMSK                                         0x1ff
#define HWIO_TLMM_GPIO_INTR_CFGn_MAXn                                           116
#define HWIO_TLMM_GPIO_INTR_CFGn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n), HWIO_TLMM_GPIO_INTR_CFGn_RMSK)
#define HWIO_TLMM_GPIO_INTR_CFGn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_INTR_CFGn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n),val)
#define HWIO_TLMM_GPIO_INTR_CFGn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n),mask,val,HWIO_TLMM_GPIO_INTR_CFGn_INI(n))
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_BMSK                             0x100
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_SHFT                               0x8
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_BMSK                              0xe0
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_SHFT                               0x5
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_BMSK                       0x10
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_SHFT                        0x4
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_BMSK                             0xc
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_SHFT                             0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_BMSK                              0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_SHFT                              0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_BMSK                               0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_SHFT                               0x0

#define HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n)                              (TLMM_CSR_REG_BASE      + 0x0000100c + 0x10 * (n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_RMSK                                        0x1
#define HWIO_TLMM_GPIO_INTR_STATUSn_MAXn                                        116
#define HWIO_TLMM_GPIO_INTR_STATUSn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n), HWIO_TLMM_GPIO_INTR_STATUSn_RMSK)
#define HWIO_TLMM_GPIO_INTR_STATUSn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_INTR_STATUSn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n),val)
#define HWIO_TLMM_GPIO_INTR_STATUSn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n),mask,val,HWIO_TLMM_GPIO_INTR_STATUSn_INI(n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_INTR_STATUS_BMSK                            0x1
#define HWIO_TLMM_GPIO_INTR_STATUSn_INTR_STATUS_SHFT                            0x0

#define HWIO_TLMM_CLK_GATE_EN_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00002000)
#define HWIO_TLMM_CLK_GATE_EN_RMSK                                              0x7
#define HWIO_TLMM_CLK_GATE_EN_IN          \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_ADDR, HWIO_TLMM_CLK_GATE_EN_RMSK)
#define HWIO_TLMM_CLK_GATE_EN_INM(m)      \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_ADDR, m)
#define HWIO_TLMM_CLK_GATE_EN_OUT(v)      \
        out_dword(HWIO_TLMM_CLK_GATE_EN_ADDR,v)
#define HWIO_TLMM_CLK_GATE_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_CLK_GATE_EN_ADDR,m,v,HWIO_TLMM_CLK_GATE_EN_IN)
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_BMSK                                  0x4
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_SHFT                                  0x2
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_BMSK                              0x2
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_SHFT                              0x1
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_BMSK                                 0x1
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_SHFT                                 0x0

#define HWIO_TLMM_IE_CTRL_DISABLE_ADDR                                   (TLMM_CSR_REG_BASE      + 0x00002004)
#define HWIO_TLMM_IE_CTRL_DISABLE_RMSK                                          0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IN          \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_ADDR, HWIO_TLMM_IE_CTRL_DISABLE_RMSK)
#define HWIO_TLMM_IE_CTRL_DISABLE_INM(m)      \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_ADDR, m)
#define HWIO_TLMM_IE_CTRL_DISABLE_OUT(v)      \
        out_dword(HWIO_TLMM_IE_CTRL_DISABLE_ADDR,v)
#define HWIO_TLMM_IE_CTRL_DISABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_IE_CTRL_DISABLE_ADDR,m,v,HWIO_TLMM_IE_CTRL_DISABLE_IN)
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_BMSK                          0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_SHFT                          0x0

#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_CLK_HIHYS_EN_BMSK               0x1000000
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_CLK_HIHYS_EN_SHFT                    0x18
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_RESET_HIHYS_EN_BMSK              0x800000
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_RESET_HIHYS_EN_SHFT                  0x17
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_DATA_HIHYS_EN_BMSK               0x400000
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_DATA_HIHYS_EN_SHFT                   0x16
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_PRESENT_HIHYS_EN_BMSK            0x200000
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_PRESENT_HIHYS_EN_SHFT                0x15
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_CLK_PULL_BMSK                    0x180000
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_CLK_PULL_SHFT                        0x13
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_RESET_PULL_BMSK                   0x60000
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_RESET_PULL_SHFT                      0x11
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_DATA_PULL_BMSK                    0x18000
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_DATA_PULL_SHFT                        0xf
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_PRESENT_PULL_BMSK                  0x6000
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_PRESENT_PULL_SHFT                     0xd
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_CLK_HDRV_BMSK                      0x1c00
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_CLK_HDRV_SHFT                         0xa
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_RESET_HDRV_BMSK                     0x380
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_RESET_HDRV_SHFT                       0x7
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_DATA_HDRV_BMSK                       0x70
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_DATA_HDRV_SHFT                        0x4
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_PRESENT_HDRV_BMSK                     0xe
#define HWIO_TLMM_IE_CTRL_DISABLE_SMT_UIM1_PRESENT_HDRV_SHFT                     0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_BMSK                           0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_SHFT                           0x0
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_ENABLE_FVAL                    0x0
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_DISABLE_FVAL                   0x1

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR                               (TLMM_CSR_REG_BASE      + 0x00002008)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_RMSK                               0xffffffff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_IN          \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR, HWIO_TLMM_MPM_WAKEUP_INT_EN_0_RMSK)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR, m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OUT(v)      \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR,v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR,m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_0_IN)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_72_BMSK                       0x80000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_72_SHFT                             0x1f
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_71_BMSK                       0x40000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_71_SHFT                             0x1e
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_69_BMSK                       0x20000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_69_SHFT                             0x1d
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_68_BMSK                       0x10000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_68_SHFT                             0x1c
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_67_BMSK                        0x8000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_67_SHFT                             0x1b
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_66_BMSK                        0x4000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_66_SHFT                             0x1a
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_65_BMSK                        0x2000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_65_SHFT                             0x19
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_64_BMSK                        0x1000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_64_SHFT                             0x18
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_63_BMSK                         0x800000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_63_SHFT                             0x17
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_62_BMSK                         0x400000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_62_SHFT                             0x16
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_54_BMSK                         0x200000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_54_SHFT                             0x15
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_52_BMSK                         0x100000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_52_SHFT                             0x14
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_51_BMSK                          0x80000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_51_SHFT                             0x13
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_BMSK                          0x40000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_SHFT                             0x12
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_49_BMSK                          0x20000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_49_SHFT                             0x11
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_48_BMSK                          0x10000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_48_SHFT                             0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_46_BMSK                           0x8000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_46_SHFT                              0xf
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_41_BMSK                           0x4000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_41_SHFT                              0xe
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_39_BMSK                           0x2000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_39_SHFT                              0xd
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_37_BMSK                           0x1000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_37_SHFT                              0xc
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_35_BMSK                            0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_35_SHFT                              0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_33_BMSK                            0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_33_SHFT                              0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_31_BMSK                            0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_31_SHFT                              0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_29_BMSK                            0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_29_SHFT                              0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_27_BMSK                             0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_27_SHFT                              0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_21_BMSK                             0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_21_SHFT                              0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_17_BMSK                             0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_17_SHFT                              0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_13_BMSK                             0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_13_SHFT                              0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_9_BMSK                               0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_9_SHFT                               0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_BMSK                               0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_SHFT                               0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_4_BMSK                               0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_4_SHFT                               0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_1_BMSK                               0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_1_SHFT                               0x0

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR                               (TLMM_CSR_REG_BASE      + 0x0000200c)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_RMSK                                   0x3fff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_IN          \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR, HWIO_TLMM_MPM_WAKEUP_INT_EN_1_RMSK)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR, m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_OUT(v)      \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR,v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR,m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_1_IN)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_3_BMSK                       0x2000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_3_SHFT                          0xd
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_1_BMSK                       0x1000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC2_DATA_1_SHFT                          0xc
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_3_BMSK                        0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_3_SHFT                          0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_1_BMSK                        0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_1_SHFT                          0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SRST_N_BMSK                             0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SRST_N_SHFT                               0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_115_BMSK                           0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_115_SHFT                             0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_113_BMSK                            0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_113_SHFT                             0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_111_BMSK                            0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_111_SHFT                             0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_110_BMSK                            0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_110_SHFT                             0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_109_BMSK                            0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_109_SHFT                             0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_108_BMSK                             0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_108_SHFT                             0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_107_BMSK                             0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_107_SHFT                             0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_106_BMSK                             0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_106_SHFT                             0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_38_BMSK                              0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_38_SHFT                              0x0

#define HWIO_TLMM_INT_JTAG_CTL_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00002010)
#define HWIO_TLMM_INT_JTAG_CTL_RMSK                                             0xf
#define HWIO_TLMM_INT_JTAG_CTL_IN          \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR, HWIO_TLMM_INT_JTAG_CTL_RMSK)
#define HWIO_TLMM_INT_JTAG_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR, m)
#define HWIO_TLMM_INT_JTAG_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_INT_JTAG_CTL_ADDR,v)
#define HWIO_TLMM_INT_JTAG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_INT_JTAG_CTL_ADDR,m,v,HWIO_TLMM_INT_JTAG_CTL_IN)
#define HWIO_TLMM_INT_JTAG_CTL_KPSS_TAP_ENA_BMSK                                0x8
#define HWIO_TLMM_INT_JTAG_CTL_KPSS_TAP_ENA_SHFT                                0x3
#define HWIO_TLMM_INT_JTAG_CTL_QDSS_TAP_ENA_BMSK                                0x4
#define HWIO_TLMM_INT_JTAG_CTL_QDSS_TAP_ENA_SHFT                                0x2
#define HWIO_TLMM_INT_JTAG_CTL_ACC_TAP_ENA_BMSK                                 0x2
#define HWIO_TLMM_INT_JTAG_CTL_ACC_TAP_ENA_SHFT                                 0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_BMSK                                 0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_SHFT                                 0x0

#define HWIO_TLMM_ETM_MODE_ADDR                                          (TLMM_CSR_REG_BASE      + 0x00002014)
#define HWIO_TLMM_ETM_MODE_RMSK                                                 0x1
#define HWIO_TLMM_ETM_MODE_IN          \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR, HWIO_TLMM_ETM_MODE_RMSK)
#define HWIO_TLMM_ETM_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR, m)
#define HWIO_TLMM_ETM_MODE_OUT(v)      \
        out_dword(HWIO_TLMM_ETM_MODE_ADDR,v)
#define HWIO_TLMM_ETM_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_ETM_MODE_ADDR,m,v,HWIO_TLMM_ETM_MODE_IN)
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_BMSK                                 0x1
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_SHFT                                 0x0

#define HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR                                   (TLMM_CSR_REG_BASE      + 0x00002018)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_RMSK                                          0x3
#define HWIO_TLMM_DBG_BUS_OUT_SEL_IN          \
        in_dword_masked(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR, HWIO_TLMM_DBG_BUS_OUT_SEL_RMSK)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_INM(m)      \
        in_dword_masked(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR, m)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OUT(v)      \
        out_dword(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR,v)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR,m,v,HWIO_TLMM_DBG_BUS_OUT_SEL_IN)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_BMSK                                 0x3
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_SHFT                                 0x0

#define HWIO_TLMM_CHIP_MODE_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00002020)
#define HWIO_TLMM_CHIP_MODE_RMSK                                                0x3
#define HWIO_TLMM_CHIP_MODE_IN          \
        in_dword_masked(HWIO_TLMM_CHIP_MODE_ADDR, HWIO_TLMM_CHIP_MODE_RMSK)
#define HWIO_TLMM_CHIP_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_CHIP_MODE_ADDR, m)
#define HWIO_TLMM_CHIP_MODE_MODE1_PIN_BMSK                                      0x2
#define HWIO_TLMM_CHIP_MODE_MODE1_PIN_SHFT                                      0x1
#define HWIO_TLMM_CHIP_MODE_MODE0_PIN_BMSK                                      0x1
#define HWIO_TLMM_CHIP_MODE_MODE0_PIN_SHFT                                      0x0

#define HWIO_TLMM_SPARE_ADDR                                             (TLMM_CSR_REG_BASE      + 0x00002024)
#define HWIO_TLMM_SPARE_RMSK                                                   0xff
#define HWIO_TLMM_SPARE_IN          \
        in_dword_masked(HWIO_TLMM_SPARE_ADDR, HWIO_TLMM_SPARE_RMSK)
#define HWIO_TLMM_SPARE_INM(m)      \
        in_dword_masked(HWIO_TLMM_SPARE_ADDR, m)
#define HWIO_TLMM_SPARE_OUT(v)      \
        out_dword(HWIO_TLMM_SPARE_ADDR,v)
#define HWIO_TLMM_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SPARE_ADDR,m,v,HWIO_TLMM_SPARE_IN)
#define HWIO_TLMM_SPARE_MISC_BMSK                                               0xf0
#define HWIO_TLMM_SPARE_MISC_SHFT                                                0x4
#define HWIO_TLMM_SPARE_TLMM_UIM1_RST_EN_BMSK                                    0x8
#define HWIO_TLMM_SPARE_TLMM_UIM1_RST_EN_SHFT                                    0x3
#define HWIO_TLMM_SPARE_TLMM_UIM1_RST_EN_DISABLE_FVAL                            0x0
#define HWIO_TLMM_SPARE_TLMM_UIM1_RST_EN_ENABLE_FVAL                             0x1
#define HWIO_TLMM_SPARE_TLMM_UIM1_CLK_EN_BMSK                                    0x4
#define HWIO_TLMM_SPARE_TLMM_UIM1_CLK_EN_SHFT                                    0x2
#define HWIO_TLMM_SPARE_TLMM_UIM1_CLK_EN_DISABLE_FVAL                            0x0
#define HWIO_TLMM_SPARE_TLMM_UIM1_CLK_EN_ENABLE_FVAL                             0x1
#define HWIO_TLMM_SPARE_TLMM_UIM1_DATA_EN_BMSK                                   0x2
#define HWIO_TLMM_SPARE_TLMM_UIM1_DATA_EN_SHFT                                   0x1
#define HWIO_TLMM_SPARE_TLMM_UIM1_DATA_EN_DISABLE_FVAL                           0x0
#define HWIO_TLMM_SPARE_TLMM_UIM1_DATA_EN_ENABLE_FVAL                            0x1
#define HWIO_TLMM_SPARE_TLMM_DATA_BMSK                                           0x1
#define HWIO_TLMM_SPARE_TLMM_DATA_SHFT                                           0x0
#define HWIO_TLMM_SPARE_TLMM_DATA_INACTIVE_DATA_FVAL                             0x0
#define HWIO_TLMM_SPARE_TLMM_DATA_ACTIVE_DATA_FVAL                               0x1


#define HWIO_TLMM_HW_REVISION_NUMBER_ADDR                                (TLMM_CSR_REG_BASE      + 0x00002028)
#define HWIO_TLMM_HW_REVISION_NUMBER_RMSK                                0xffffffff
#define HWIO_TLMM_HW_REVISION_NUMBER_IN          \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR, HWIO_TLMM_HW_REVISION_NUMBER_RMSK)
#define HWIO_TLMM_HW_REVISION_NUMBER_INM(m)      \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR, m)
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_BMSK                     0xf0000000
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_SHFT                           0x1c
#define HWIO_TLMM_HW_REVISION_NUMBER_PARTNUM_BMSK                         0xffff000
#define HWIO_TLMM_HW_REVISION_NUMBER_PARTNUM_SHFT                               0xc
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_BMSK                     0xffe
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_SHFT                       0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_BMSK                             0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_SHFT                             0x0

#define HWIO_TLMM_SLIMBUS_CTL_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00002030)
#define HWIO_TLMM_SLIMBUS_CTL_RMSK                                              0x3
#define HWIO_TLMM_SLIMBUS_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SLIMBUS_CTL_ADDR, HWIO_TLMM_SLIMBUS_CTL_RMSK)
#define HWIO_TLMM_SLIMBUS_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SLIMBUS_CTL_ADDR, m)
#define HWIO_TLMM_SLIMBUS_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SLIMBUS_CTL_ADDR,v)
#define HWIO_TLMM_SLIMBUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SLIMBUS_CTL_ADDR,m,v,HWIO_TLMM_SLIMBUS_CTL_IN)
#define HWIO_TLMM_SLIMBUS_CTL_SEN_EN_BMSK                                       0x2
#define HWIO_TLMM_SLIMBUS_CTL_SEN_EN_SHFT                                       0x1
#define HWIO_TLMM_SLIMBUS_CTL_SR_CTL_EN_BMSK                                    0x1
#define HWIO_TLMM_SLIMBUS_CTL_SR_CTL_EN_SHFT                                    0x0

#define HWIO_TLMM_RFFE_CTL_ADDR                                          (TLMM_CSR_REG_BASE      + 0x00002034)
#define HWIO_TLMM_RFFE_CTL_RMSK                                               0xfff
#define HWIO_TLMM_RFFE_CTL_IN          \
        in_dword_masked(HWIO_TLMM_RFFE_CTL_ADDR, HWIO_TLMM_RFFE_CTL_RMSK)
#define HWIO_TLMM_RFFE_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_RFFE_CTL_ADDR, m)
#define HWIO_TLMM_RFFE_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_RFFE_CTL_ADDR,v)
#define HWIO_TLMM_RFFE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_RFFE_CTL_ADDR,m,v,HWIO_TLMM_RFFE_CTL_IN)
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SEN_EN_BMSK                             0x800
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SEN_EN_SHFT                               0xb
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SR_CTL_EN_BMSK                          0x600
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SR_CTL_EN_SHFT                            0x9
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SEN_EN_BMSK                              0x100
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SEN_EN_SHFT                                0x8
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SR_CTL_EN_BMSK                            0xc0
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SR_CTL_EN_SHFT                             0x6
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SEN_EN_BMSK                              0x20
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SEN_EN_SHFT                               0x5
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SR_CTL_EN_BMSK                           0x18
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SR_CTL_EN_SHFT                            0x3
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SEN_EN_BMSK                                0x4
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SEN_EN_SHFT                                0x2
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SR_CTL_EN_BMSK                             0x3
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SR_CTL_EN_SHFT                             0x0

#define HWIO_TLMM_RESOUT_HDRV_CTL_ADDR                                   (TLMM_CSR_REG_BASE      + 0x00002038)
#define HWIO_TLMM_RESOUT_HDRV_CTL_RMSK                                          0x7
#define HWIO_TLMM_RESOUT_HDRV_CTL_IN          \
        in_dword_masked(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR, HWIO_TLMM_RESOUT_HDRV_CTL_RMSK)
#define HWIO_TLMM_RESOUT_HDRV_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR, m)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR,v)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR,m,v,HWIO_TLMM_RESOUT_HDRV_CTL_IN)
#define HWIO_TLMM_RESOUT_HDRV_CTL_RESOUT_N_HDRV_BMSK                            0x7
#define HWIO_TLMM_RESOUT_HDRV_CTL_RESOUT_N_HDRV_SHFT                            0x0

#define HWIO_TLMM_JTAG_HDRV_CTL_ADDR                                     (TLMM_CSR_REG_BASE      + 0x0000203c)
#define HWIO_TLMM_JTAG_HDRV_CTL_RMSK                                          0x3ff
#define HWIO_TLMM_JTAG_HDRV_CTL_IN          \
        in_dword_masked(HWIO_TLMM_JTAG_HDRV_CTL_ADDR, HWIO_TLMM_JTAG_HDRV_CTL_RMSK)
#define HWIO_TLMM_JTAG_HDRV_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_JTAG_HDRV_CTL_ADDR, m)
#define HWIO_TLMM_JTAG_HDRV_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_JTAG_HDRV_CTL_ADDR,v)
#define HWIO_TLMM_JTAG_HDRV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_JTAG_HDRV_CTL_ADDR,m,v,HWIO_TLMM_JTAG_HDRV_CTL_IN)
#define HWIO_TLMM_JTAG_HDRV_CTL_TDI_PULL_BMSK                                 0x300
#define HWIO_TLMM_JTAG_HDRV_CTL_TDI_PULL_SHFT                                   0x8
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_PULL_BMSK                                  0xc0
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_PULL_SHFT                                   0x6
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_HDRV_BMSK                                  0x38
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_HDRV_SHFT                                   0x3
#define HWIO_TLMM_JTAG_HDRV_CTL_TDO_HDRV_BMSK                                   0x7
#define HWIO_TLMM_JTAG_HDRV_CTL_TDO_HDRV_SHFT                                   0x0

#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR                                (TLMM_CSR_REG_BASE      + 0x00002040)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK                                   0x3ffff
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR, HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SEN_EN_BMSK             0x20000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SEN_EN_SHFT                0x11
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SEN_EN_BMSK              0x10000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SEN_EN_SHFT                 0x10
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_PULL_BMSK                0xc000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_PULL_SHFT                   0xe
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_PULL_BMSK                 0x3000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_PULL_SHFT                    0xc
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PSHOLD_HDRV_BMSK                         0xe00
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PSHOLD_HDRV_SHFT                           0x9
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_CXO_EN_HDRV_BMSK                         0x1c0
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_CXO_EN_HDRV_SHFT                           0x6
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HDRV_BMSK                  0x38
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HDRV_SHFT                   0x3
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HDRV_BMSK                    0x7
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HDRV_SHFT                    0x0

#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR                                (TLMM_CSR_REG_BASE      + 0x00002044)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK                                    0x7fff
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR, HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_PULL_BMSK                      0x6000
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_PULL_SHFT                         0xd
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_PULL_BMSK                      0x1800
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_PULL_SHFT                         0xb
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_PULL_BMSK                      0x600
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_PULL_SHFT                        0x9
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_HDRV_BMSK                       0x1c0
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_HDRV_SHFT                         0x6
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_HDRV_BMSK                        0x38
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_HDRV_SHFT                         0x3
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_HDRV_BMSK                        0x7
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_HDRV_SHFT                        0x0

#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR                                (TLMM_CSR_REG_BASE      + 0x00002048)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_RMSK                                    0xffff
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR, HWIO_TLMM_SDC2_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_SDC2_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_PULL_BMSK                      0xc000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_PULL_SHFT                         0xe
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_BMSK                       0x2000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_SHFT                          0xd
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_PULL_BMSK                      0x1800
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_PULL_SHFT                         0xb
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_PULL_BMSK                      0x600
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_PULL_SHFT                        0x9
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_HDRV_BMSK                       0x1c0
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_HDRV_SHFT                         0x6
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_HDRV_BMSK                        0x38
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_HDRV_SHFT                         0x3
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_HDRV_BMSK                        0x7
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_HDRV_SHFT                        0x0

#define HWIO_TLMM_MODE_PULL_CTL_ADDR                                     (TLMM_CSR_REG_BASE      + 0x0000204c)
#define HWIO_TLMM_MODE_PULL_CTL_RMSK                                            0xf
#define HWIO_TLMM_MODE_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_MODE_PULL_CTL_ADDR, HWIO_TLMM_MODE_PULL_CTL_RMSK)
#define HWIO_TLMM_MODE_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_MODE_PULL_CTL_ADDR, m)
#define HWIO_TLMM_MODE_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_MODE_PULL_CTL_ADDR,v)
#define HWIO_TLMM_MODE_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MODE_PULL_CTL_ADDR,m,v,HWIO_TLMM_MODE_PULL_CTL_IN)
#define HWIO_TLMM_MODE_PULL_CTL_MODE_1_PULL_BMSK                                0xc
#define HWIO_TLMM_MODE_PULL_CTL_MODE_1_PULL_SHFT                                0x2
#define HWIO_TLMM_MODE_PULL_CTL_MODE_0_PULL_BMSK                                0x3
#define HWIO_TLMM_MODE_PULL_CTL_MODE_0_PULL_SHFT                                0x0

#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR                          (TLMM_CSR_REG_BASE      + 0x00002050)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_RMSK                           0x3ffffff
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR, HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_RMSK)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR, m)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR,v)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ADDR,m,v,HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_IN)
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_CORE_CTL_EN_BMSK               0x2000000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_CORE_CTL_EN_SHFT                    0x19
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_VM_SHIFT_EN_BMSK               0x1000000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_VM_SHIFT_EN_SHFT                    0x18
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ODT_EN_BMSK                     0x800000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ODT_EN_SHFT                         0x17
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ODT_BMSK                        0x600000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_ODT_SHFT                            0x15
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_DCC_BMSK                        0x1c0000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_DCC_SHFT                            0x12
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NRXDEL_BMSK                      0x30000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NRXDEL_SHFT                         0x10
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PRXDEL_BMSK                       0xc000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PRXDEL_SHFT                          0xe
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NSLEW_BMSK                        0x3000
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NSLEW_SHFT                           0xc
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PSLEW_BMSK                         0xc00
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PSLEW_SHFT                           0xa
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NCNT_BMSK                          0x3e0
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_NCNT_SHFT                            0x5
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PCNT_BMSK                           0x1f
#define HWIO_TLMM_GPIO_HSIC_STROBE_PAD_CTL_PCNT_SHFT                            0x0

#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR                            (TLMM_CSR_REG_BASE      + 0x00002054)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_RMSK                             0x3ffffff
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR, HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_RMSK)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR, m)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR,v)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ADDR,m,v,HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_IN)
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_CORE_CTL_EN_BMSK                 0x2000000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_CORE_CTL_EN_SHFT                      0x19
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_VM_SHIFT_EN_BMSK                 0x1000000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_VM_SHIFT_EN_SHFT                      0x18
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ODT_EN_BMSK                       0x800000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ODT_EN_SHFT                           0x17
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ODT_BMSK                          0x600000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_ODT_SHFT                              0x15
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_DCC_BMSK                          0x1c0000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_DCC_SHFT                              0x12
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NRXDEL_BMSK                        0x30000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NRXDEL_SHFT                           0x10
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PRXDEL_BMSK                         0xc000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PRXDEL_SHFT                            0xe
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NSLEW_BMSK                          0x3000
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NSLEW_SHFT                             0xc
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PSLEW_BMSK                           0xc00
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PSLEW_SHFT                             0xa
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NCNT_BMSK                            0x3e0
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_NCNT_SHFT                              0x5
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PCNT_BMSK                             0x1f
#define HWIO_TLMM_GPIO_HSIC_DATA_PAD_CTL_PCNT_SHFT                              0x0

#define HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR                                  (TLMM_CSR_REG_BASE      + 0x00002058)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_RMSK                                         0x3
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_IN          \
        in_dword_masked(HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR, HWIO_TLMM_HSIC_CAL_PAD_CTL_RMSK)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR, m)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR,v)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_HSIC_CAL_PAD_CTL_ADDR,m,v,HWIO_TLMM_HSIC_CAL_PAD_CTL_IN)
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_IMP_SEL_BMSK                                 0x3
#define HWIO_TLMM_HSIC_CAL_PAD_CTL_IMP_SEL_SHFT                                 0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n)                        (TLMM_CSR_REG_BASE      + 0x00002100 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_RMSK                                0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_MAXn                                    1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_POLARITY_BMSK                       0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_POLARITY_SHFT                         0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_GPIO_SEL_BMSK                        0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_GPIO_SEL_SHFT                         0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n)                     (TLMM_CSR_REG_BASE      + 0x00002200 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_RMSK                             0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_MAXn                                 9
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_BMSK                    0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_SHFT                      0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_GPIO_SEL_BMSK                     0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_GPIO_SEL_SHFT                      0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n)                     (TLMM_CSR_REG_BASE      + 0x00002500 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_RMSK                             0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_MAXn                                 5
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_BMSK                    0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_SHFT                      0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_BMSK                     0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_SHFT                      0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n)                         (TLMM_CSR_REG_BASE      + 0x00002700 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_RMSK                                 0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_MAXn                                     0
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_BMSK                        0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_SHFT                          0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_GPIO_SEL_BMSK                         0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_GPIO_SEL_SHFT                          0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n)                        (TLMM_CSR_REG_BASE      + 0x00002800 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_RMSK                                0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_MAXn                                    7
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_POLARITY_BMSK                       0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_POLARITY_SHFT                         0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_GPIO_SEL_BMSK                        0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_KPSS_GPIO_SEL_SHFT                         0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n)                         (TLMM_CSR_REG_BASE      + 0x00002a00 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_RMSK                                 0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_MAXn                                     1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_BMSK                        0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_SHFT                          0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_GPIO_SEL_BMSK                         0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_GPIO_SEL_SHFT                          0x0

#define HWIO_TLMM_GPIO_OUT_0_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00003000)
#define HWIO_TLMM_GPIO_OUT_0_RMSK                                        0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, HWIO_TLMM_GPIO_OUT_0_RMSK)
#define HWIO_TLMM_GPIO_OUT_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_0_ADDR,m,v,HWIO_TLMM_GPIO_OUT_0_IN)
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_BMSK                               0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_SHFT                                      0x0

#define HWIO_TLMM_GPIO_OUT_1_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00003004)
#define HWIO_TLMM_GPIO_OUT_1_RMSK                                        0xffffffff
#define HWIO_TLMM_GPIO_OUT_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_1_ADDR, HWIO_TLMM_GPIO_OUT_1_RMSK)
#define HWIO_TLMM_GPIO_OUT_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_1_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_1_ADDR,m,v,HWIO_TLMM_GPIO_OUT_1_IN)
#define HWIO_TLMM_GPIO_OUT_1_GPIO_OUT_BMSK                               0xffffffff
#define HWIO_TLMM_GPIO_OUT_1_GPIO_OUT_SHFT                                      0x0

#define HWIO_TLMM_GPIO_OUT_2_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00003008)
#define HWIO_TLMM_GPIO_OUT_2_RMSK                                        0xffffffff
#define HWIO_TLMM_GPIO_OUT_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_2_ADDR, HWIO_TLMM_GPIO_OUT_2_RMSK)
#define HWIO_TLMM_GPIO_OUT_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_2_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_2_ADDR,m,v,HWIO_TLMM_GPIO_OUT_2_IN)
#define HWIO_TLMM_GPIO_OUT_2_GPIO_OUT_BMSK                               0xffffffff
#define HWIO_TLMM_GPIO_OUT_2_GPIO_OUT_SHFT                                      0x0

#define HWIO_TLMM_GPIO_OUT_3_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0000300c)
#define HWIO_TLMM_GPIO_OUT_3_RMSK                                          0x1fffff
#define HWIO_TLMM_GPIO_OUT_3_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_3_ADDR, HWIO_TLMM_GPIO_OUT_3_RMSK)
#define HWIO_TLMM_GPIO_OUT_3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_3_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_3_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_3_ADDR,m,v,HWIO_TLMM_GPIO_OUT_3_IN)
#define HWIO_TLMM_GPIO_OUT_3_GPIO_OUT_BMSK                                 0x1fffff
#define HWIO_TLMM_GPIO_OUT_3_GPIO_OUT_SHFT                                      0x0

#define HWIO_TLMM_GPIO_OUT_CLR_0_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00003020)
#define HWIO_TLMM_GPIO_OUT_CLR_0_RMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_0_GPIO_OUT_CLR_BMSK                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_0_GPIO_OUT_CLR_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_CLR_1_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00003024)
#define HWIO_TLMM_GPIO_OUT_CLR_1_RMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_1_GPIO_OUT_CLR_BMSK                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_1_GPIO_OUT_CLR_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_CLR_2_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00003028)
#define HWIO_TLMM_GPIO_OUT_CLR_2_RMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_2_GPIO_OUT_CLR_BMSK                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_2_GPIO_OUT_CLR_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_CLR_3_ADDR                                    (TLMM_CSR_REG_BASE      + 0x0000302c)
#define HWIO_TLMM_GPIO_OUT_CLR_3_RMSK                                      0x1fffff
#define HWIO_TLMM_GPIO_OUT_CLR_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_3_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_3_GPIO_OUT_CLR_BMSK                         0x1fffff
#define HWIO_TLMM_GPIO_OUT_CLR_3_GPIO_OUT_CLR_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_SET_0_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00003040)
#define HWIO_TLMM_GPIO_OUT_SET_0_RMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_0_GPIO_OUT_SET_BMSK                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_0_GPIO_OUT_SET_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_SET_1_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00003044)
#define HWIO_TLMM_GPIO_OUT_SET_1_RMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_1_GPIO_OUT_SET_BMSK                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_1_GPIO_OUT_SET_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_SET_2_ADDR                                    (TLMM_CSR_REG_BASE      + 0x00003048)
#define HWIO_TLMM_GPIO_OUT_SET_2_RMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_2_GPIO_OUT_SET_BMSK                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_2_GPIO_OUT_SET_SHFT                              0x0

#define HWIO_TLMM_GPIO_OUT_SET_3_ADDR                                    (TLMM_CSR_REG_BASE      + 0x0000304c)
#define HWIO_TLMM_GPIO_OUT_SET_3_RMSK                                      0x1fffff
#define HWIO_TLMM_GPIO_OUT_SET_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_3_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_3_GPIO_OUT_SET_BMSK                         0x1fffff
#define HWIO_TLMM_GPIO_OUT_SET_3_GPIO_OUT_SET_SHFT                              0x0

#define HWIO_TLMM_GPIO_IN_0_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003060)
#define HWIO_TLMM_GPIO_IN_0_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_IN_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_0_ADDR, HWIO_TLMM_GPIO_IN_0_RMSK)
#define HWIO_TLMM_GPIO_IN_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_0_ADDR, m)
#define HWIO_TLMM_GPIO_IN_0_GPIO_IN_BMSK                                 0xffffffff
#define HWIO_TLMM_GPIO_IN_0_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_1_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003064)
#define HWIO_TLMM_GPIO_IN_1_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_IN_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_1_ADDR, HWIO_TLMM_GPIO_IN_1_RMSK)
#define HWIO_TLMM_GPIO_IN_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_1_ADDR, m)
#define HWIO_TLMM_GPIO_IN_1_GPIO_IN_BMSK                                 0xffffffff
#define HWIO_TLMM_GPIO_IN_1_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_2_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003068)
#define HWIO_TLMM_GPIO_IN_2_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_IN_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_2_ADDR, HWIO_TLMM_GPIO_IN_2_RMSK)
#define HWIO_TLMM_GPIO_IN_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_2_ADDR, m)
#define HWIO_TLMM_GPIO_IN_2_GPIO_IN_BMSK                                 0xffffffff
#define HWIO_TLMM_GPIO_IN_2_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_3_ADDR                                         (TLMM_CSR_REG_BASE      + 0x0000306c)
#define HWIO_TLMM_GPIO_IN_3_RMSK                                           0x1fffff
#define HWIO_TLMM_GPIO_IN_3_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_3_ADDR, HWIO_TLMM_GPIO_IN_3_RMSK)
#define HWIO_TLMM_GPIO_IN_3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_3_ADDR, m)
#define HWIO_TLMM_GPIO_IN_3_GPIO_IN_BMSK                                   0x1fffff
#define HWIO_TLMM_GPIO_IN_3_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_OE_0_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003080)
#define HWIO_TLMM_GPIO_OE_0_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_OE_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, HWIO_TLMM_GPIO_OE_0_RMSK)
#define HWIO_TLMM_GPIO_OE_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, m)
#define HWIO_TLMM_GPIO_OE_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_0_ADDR,m,v,HWIO_TLMM_GPIO_OE_0_IN)
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_BMSK                                 0xffffffff
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_SHFT                                        0x0

#define HWIO_TLMM_GPIO_OE_1_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003084)
#define HWIO_TLMM_GPIO_OE_1_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_OE_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_1_ADDR, HWIO_TLMM_GPIO_OE_1_RMSK)
#define HWIO_TLMM_GPIO_OE_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_1_ADDR, m)
#define HWIO_TLMM_GPIO_OE_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_1_ADDR,m,v,HWIO_TLMM_GPIO_OE_1_IN)
#define HWIO_TLMM_GPIO_OE_1_GPIO_OE_BMSK                                 0xffffffff
#define HWIO_TLMM_GPIO_OE_1_GPIO_OE_SHFT                                        0x0

#define HWIO_TLMM_GPIO_OE_2_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003088)
#define HWIO_TLMM_GPIO_OE_2_RMSK                                         0xffffffff
#define HWIO_TLMM_GPIO_OE_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_2_ADDR, HWIO_TLMM_GPIO_OE_2_RMSK)
#define HWIO_TLMM_GPIO_OE_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_2_ADDR, m)
#define HWIO_TLMM_GPIO_OE_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_2_ADDR,m,v,HWIO_TLMM_GPIO_OE_2_IN)
#define HWIO_TLMM_GPIO_OE_2_GPIO_OE_BMSK                                 0xffffffff
#define HWIO_TLMM_GPIO_OE_2_GPIO_OE_SHFT                                        0x0

#define HWIO_TLMM_GPIO_OE_3_ADDR                                         (TLMM_CSR_REG_BASE      + 0x0000308c)
#define HWIO_TLMM_GPIO_OE_3_RMSK                                           0x1fffff
#define HWIO_TLMM_GPIO_OE_3_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_3_ADDR, HWIO_TLMM_GPIO_OE_3_RMSK)
#define HWIO_TLMM_GPIO_OE_3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_3_ADDR, m)
#define HWIO_TLMM_GPIO_OE_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_3_ADDR,v)
#define HWIO_TLMM_GPIO_OE_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_3_ADDR,m,v,HWIO_TLMM_GPIO_OE_3_IN)
#define HWIO_TLMM_GPIO_OE_3_GPIO_OE_BMSK                                   0x1fffff
#define HWIO_TLMM_GPIO_OE_3_GPIO_OE_SHFT                                        0x0

#define HWIO_TLMM_GPIO_OE_CLR_0_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003100)
#define HWIO_TLMM_GPIO_OE_CLR_0_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_0_GPIO_OE_CLR_BMSK                         0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_0_GPIO_OE_CLR_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_CLR_1_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003104)
#define HWIO_TLMM_GPIO_OE_CLR_1_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_1_GPIO_OE_CLR_BMSK                         0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_1_GPIO_OE_CLR_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_CLR_2_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003108)
#define HWIO_TLMM_GPIO_OE_CLR_2_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_2_GPIO_OE_CLR_BMSK                         0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_2_GPIO_OE_CLR_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_CLR_3_ADDR                                     (TLMM_CSR_REG_BASE      + 0x0000310c)
#define HWIO_TLMM_GPIO_OE_CLR_3_RMSK                                       0x1fffff
#define HWIO_TLMM_GPIO_OE_CLR_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_3_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_3_GPIO_OE_CLR_BMSK                           0x1fffff
#define HWIO_TLMM_GPIO_OE_CLR_3_GPIO_OE_CLR_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_SET_0_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003120)
#define HWIO_TLMM_GPIO_OE_SET_0_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_0_GPIO_OE_SET_BMSK                         0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_0_GPIO_OE_SET_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_SET_1_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003124)
#define HWIO_TLMM_GPIO_OE_SET_1_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_1_GPIO_OE_SET_BMSK                         0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_1_GPIO_OE_SET_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_SET_2_ADDR                                     (TLMM_CSR_REG_BASE      + 0x00003128)
#define HWIO_TLMM_GPIO_OE_SET_2_RMSK                                     0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_2_GPIO_OE_SET_BMSK                         0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_2_GPIO_OE_SET_SHFT                                0x0

#define HWIO_TLMM_GPIO_OE_SET_3_ADDR                                     (TLMM_CSR_REG_BASE      + 0x0000312c)
#define HWIO_TLMM_GPIO_OE_SET_3_RMSK                                       0x1fffff
#define HWIO_TLMM_GPIO_OE_SET_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_3_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_3_GPIO_OE_SET_BMSK                           0x1fffff
#define HWIO_TLMM_GPIO_OE_SET_3_GPIO_OE_SET_SHFT                                0x0


#endif /* __UIMDIMEHWIOMACROS_H__ */
