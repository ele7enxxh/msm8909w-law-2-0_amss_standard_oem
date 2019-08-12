#ifndef __UART_HWIO_H__
#define __UART_HWIO_H__
/*
===========================================================================
*/
/**
  @file Uart_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    PERIPH_SS_BLSP1_BLSP_UART0_UART0_DM

  'Include' filters applied:
  'Exclude' filters applied: RESERVED DUMMY
*/
/*
  ===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/uart_lite/hw/Uart_hwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: UART_DM
 *--------------------------------------------------------------------------*/

#define HWIO_UART_DM_MR1_ADDR(x)                                                                                                  ((x) + 0x00000000)
#define HWIO_UART_DM_MR1_RMSK                                                                                                     0xffffffff
#define HWIO_UART_DM_MR1_IN(x)      \
        in_dword_masked(HWIO_UART_DM_MR1_ADDR(x), HWIO_UART_DM_MR1_RMSK)
#define HWIO_UART_DM_MR1_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_MR1_ADDR(x), m)
#define HWIO_UART_DM_MR1_OUT(x, v)      \
        out_dword(HWIO_UART_DM_MR1_ADDR(x),v)
#define HWIO_UART_DM_MR1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_MR1_ADDR(x),m,v,HWIO_UART_DM_MR1_IN(x))
#define HWIO_UART_DM_MR1_AUTO_RFR_LEVEL1_BMSK                                                                                     0xffffff00
#define HWIO_UART_DM_MR1_AUTO_RFR_LEVEL1_SHFT                                                                                            0x8
#define HWIO_UART_DM_MR1_RX_RDY_CTL_BMSK                                                                                                0x80
#define HWIO_UART_DM_MR1_RX_RDY_CTL_SHFT                                                                                                 0x7
#define HWIO_UART_DM_MR1_CTS_CTL_BMSK                                                                                                   0x40
#define HWIO_UART_DM_MR1_CTS_CTL_SHFT                                                                                                    0x6
#define HWIO_UART_DM_MR1_AUTO_RFR_LEVEL0_BMSK                                                                                           0x3f
#define HWIO_UART_DM_MR1_AUTO_RFR_LEVEL0_SHFT                                                                                            0x0

#define HWIO_UART_DM_MR2_ADDR(x)                                                                                                  ((x) + 0x00000004)
#define HWIO_UART_DM_MR2_RMSK                                                                                                          0x7ff
#define HWIO_UART_DM_MR2_IN(x)      \
        in_dword_masked(HWIO_UART_DM_MR2_ADDR(x), HWIO_UART_DM_MR2_RMSK)
#define HWIO_UART_DM_MR2_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_MR2_ADDR(x), m)
#define HWIO_UART_DM_MR2_OUT(x, v)      \
        out_dword(HWIO_UART_DM_MR2_ADDR(x),v)
#define HWIO_UART_DM_MR2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_MR2_ADDR(x),m,v,HWIO_UART_DM_MR2_IN(x))
#define HWIO_UART_DM_MR2_RFR_CTS_LOOPBACK_BMSK                                                                                         0x400
#define HWIO_UART_DM_MR2_RFR_CTS_LOOPBACK_SHFT                                                                                           0xa
#define HWIO_UART_DM_MR2_RX_ERROR_CHAR_OFF_BMSK                                                                                        0x200
#define HWIO_UART_DM_MR2_RX_ERROR_CHAR_OFF_SHFT                                                                                          0x9
#define HWIO_UART_DM_MR2_RX_BREAK_ZERO_CHAR_OFF_BMSK                                                                                   0x100
#define HWIO_UART_DM_MR2_RX_BREAK_ZERO_CHAR_OFF_SHFT                                                                                     0x8
#define HWIO_UART_DM_MR2_LOOPBACK_BMSK                                                                                                  0x80
#define HWIO_UART_DM_MR2_LOOPBACK_SHFT                                                                                                   0x7
#define HWIO_UART_DM_MR2_ERROR_MODE_BMSK                                                                                                0x40
#define HWIO_UART_DM_MR2_ERROR_MODE_SHFT                                                                                                 0x6
#define HWIO_UART_DM_MR2_BITS_PER_CHAR_BMSK                                                                                             0x30
#define HWIO_UART_DM_MR2_BITS_PER_CHAR_SHFT                                                                                              0x4
#define HWIO_UART_DM_MR2_BITS_PER_CHAR_ENUM_5_BITS_FVAL                                                                                  0x0
#define HWIO_UART_DM_MR2_BITS_PER_CHAR_ENUM_6_BITS_FVAL                                                                                  0x1
#define HWIO_UART_DM_MR2_BITS_PER_CHAR_ENUM_7_BITS_FVAL                                                                                  0x2
#define HWIO_UART_DM_MR2_BITS_PER_CHAR_ENUM_8_BITS_FVAL                                                                                  0x3
#define HWIO_UART_DM_MR2_STOP_BIT_LEN_BMSK                                                                                               0xc
#define HWIO_UART_DM_MR2_STOP_BIT_LEN_SHFT                                                                                               0x2
#define HWIO_UART_DM_MR2_STOP_BIT_LEN_ENUM_0_563_FVAL                                                                                    0x0
#define HWIO_UART_DM_MR2_STOP_BIT_LEN_ENUM_1_000_BIT_TIME_FVAL                                                                           0x1
#define HWIO_UART_DM_MR2_STOP_BIT_LEN_ENUM_1_563_FVAL                                                                                    0x2
#define HWIO_UART_DM_MR2_STOP_BIT_LEN_ENUM_2_000_BIT_TIMES_FVAL                                                                          0x3
#define HWIO_UART_DM_MR2_PARITY_MODE_BMSK                                                                                                0x3
#define HWIO_UART_DM_MR2_PARITY_MODE_SHFT                                                                                                0x0
#define HWIO_UART_DM_MR2_PARITY_MODE_NO_PARITY_FVAL                                                                                      0x0
#define HWIO_UART_DM_MR2_PARITY_MODE_ODD_PARITY_FVAL                                                                                     0x1
#define HWIO_UART_DM_MR2_PARITY_MODE_EVEN_PARITY_FVAL                                                                                    0x2
#define HWIO_UART_DM_MR2_PARITY_MODE_SPACE_PARITY_FVAL                                                                                   0x3

#define HWIO_UART_DM_CSR_SR_DEPRECATED_ADDR(x)                                                                                    ((x) + 0x00000008)
#define HWIO_UART_DM_CSR_SR_DEPRECATED_RMSK                                                                                       0xffffffff
#define HWIO_UART_DM_CSR_SR_DEPRECATED_IN(x)      \
        in_dword_masked(HWIO_UART_DM_CSR_SR_DEPRECATED_ADDR(x), HWIO_UART_DM_CSR_SR_DEPRECATED_RMSK)
#define HWIO_UART_DM_CSR_SR_DEPRECATED_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_CSR_SR_DEPRECATED_ADDR(x), m)
#define HWIO_UART_DM_CSR_SR_DEPRECATED_OUT(x, v)      \
        out_dword(HWIO_UART_DM_CSR_SR_DEPRECATED_ADDR(x),v)
#define HWIO_UART_DM_CSR_SR_DEPRECATED_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_CSR_SR_DEPRECATED_ADDR(x),m,v,HWIO_UART_DM_CSR_SR_DEPRECATED_IN(x))
#define HWIO_UART_DM_CSR_SR_DEPRECATED_UART_DM_CSR_SR_DEPRECATED_BMSK                                                             0xffffffff
#define HWIO_UART_DM_CSR_SR_DEPRECATED_UART_DM_CSR_SR_DEPRECATED_SHFT                                                                    0x0

#define HWIO_UART_DM_CR_MISR_DEPRECATED_ADDR(x)                                                                                   ((x) + 0x00000010)
#define HWIO_UART_DM_CR_MISR_DEPRECATED_RMSK                                                                                      0xffffffff
#define HWIO_UART_DM_CR_MISR_DEPRECATED_IN(x)      \
        in_dword_masked(HWIO_UART_DM_CR_MISR_DEPRECATED_ADDR(x), HWIO_UART_DM_CR_MISR_DEPRECATED_RMSK)
#define HWIO_UART_DM_CR_MISR_DEPRECATED_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_CR_MISR_DEPRECATED_ADDR(x), m)
#define HWIO_UART_DM_CR_MISR_DEPRECATED_OUT(x, v)      \
        out_dword(HWIO_UART_DM_CR_MISR_DEPRECATED_ADDR(x),v)
#define HWIO_UART_DM_CR_MISR_DEPRECATED_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_CR_MISR_DEPRECATED_ADDR(x),m,v,HWIO_UART_DM_CR_MISR_DEPRECATED_IN(x))
#define HWIO_UART_DM_CR_MISR_DEPRECATED_UART_DM_CR_MISR_DEPRECATED_BMSK                                                           0xffffffff
#define HWIO_UART_DM_CR_MISR_DEPRECATED_UART_DM_CR_MISR_DEPRECATED_SHFT                                                                  0x0

#define HWIO_UART_DM_IMR_ISR_DEPRECATED_ADDR(x)                                                                                   ((x) + 0x00000014)
#define HWIO_UART_DM_IMR_ISR_DEPRECATED_RMSK                                                                                      0xffffffff
#define HWIO_UART_DM_IMR_ISR_DEPRECATED_IN(x)      \
        in_dword_masked(HWIO_UART_DM_IMR_ISR_DEPRECATED_ADDR(x), HWIO_UART_DM_IMR_ISR_DEPRECATED_RMSK)
#define HWIO_UART_DM_IMR_ISR_DEPRECATED_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_IMR_ISR_DEPRECATED_ADDR(x), m)
#define HWIO_UART_DM_IMR_ISR_DEPRECATED_OUT(x, v)      \
        out_dword(HWIO_UART_DM_IMR_ISR_DEPRECATED_ADDR(x),v)
#define HWIO_UART_DM_IMR_ISR_DEPRECATED_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_IMR_ISR_DEPRECATED_ADDR(x),m,v,HWIO_UART_DM_IMR_ISR_DEPRECATED_IN(x))
#define HWIO_UART_DM_IMR_ISR_DEPRECATED_UART_DM_IMR_ISR_DEPRECATED_BMSK                                                           0xffffffff
#define HWIO_UART_DM_IMR_ISR_DEPRECATED_UART_DM_IMR_ISR_DEPRECATED_SHFT                                                                  0x0

#define HWIO_UART_DM_IPR_ADDR(x)                                                                                                  ((x) + 0x00000018)
#define HWIO_UART_DM_IPR_RMSK                                                                                                     0xffffffdf
#define HWIO_UART_DM_IPR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_IPR_ADDR(x), HWIO_UART_DM_IPR_RMSK)
#define HWIO_UART_DM_IPR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_IPR_ADDR(x), m)
#define HWIO_UART_DM_IPR_OUT(x, v)      \
        out_dword(HWIO_UART_DM_IPR_ADDR(x),v)
#define HWIO_UART_DM_IPR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_IPR_ADDR(x),m,v,HWIO_UART_DM_IPR_IN(x))
#define HWIO_UART_DM_IPR_STALE_TIMEOUT_MSB_BMSK                                                                                   0xffffff80
#define HWIO_UART_DM_IPR_STALE_TIMEOUT_MSB_SHFT                                                                                          0x7
#define HWIO_UART_DM_IPR_SAMPLE_DATA_BMSK                                                                                               0x40
#define HWIO_UART_DM_IPR_SAMPLE_DATA_SHFT                                                                                                0x6
#define HWIO_UART_DM_IPR_STALE_TIMEOUT_LSB_BMSK                                                                                         0x1f
#define HWIO_UART_DM_IPR_STALE_TIMEOUT_LSB_SHFT                                                                                          0x0

#define HWIO_UART_DM_TFWR_ADDR(x)                                                                                                 ((x) + 0x0000001c)
#define HWIO_UART_DM_TFWR_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_TFWR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_TFWR_ADDR(x), HWIO_UART_DM_TFWR_RMSK)
#define HWIO_UART_DM_TFWR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_TFWR_ADDR(x), m)
#define HWIO_UART_DM_TFWR_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TFWR_ADDR(x),v)
#define HWIO_UART_DM_TFWR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_TFWR_ADDR(x),m,v,HWIO_UART_DM_TFWR_IN(x))
#define HWIO_UART_DM_TFWR_TFW_BMSK                                                                                                0xffffffff
#define HWIO_UART_DM_TFWR_TFW_SHFT                                                                                                       0x0

#define HWIO_UART_DM_RFWR_ADDR(x)                                                                                                 ((x) + 0x00000020)
#define HWIO_UART_DM_RFWR_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_RFWR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RFWR_ADDR(x), HWIO_UART_DM_RFWR_RMSK)
#define HWIO_UART_DM_RFWR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RFWR_ADDR(x), m)
#define HWIO_UART_DM_RFWR_OUT(x, v)      \
        out_dword(HWIO_UART_DM_RFWR_ADDR(x),v)
#define HWIO_UART_DM_RFWR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_RFWR_ADDR(x),m,v,HWIO_UART_DM_RFWR_IN(x))
#define HWIO_UART_DM_RFWR_RFW_BMSK                                                                                                0xffffffff
#define HWIO_UART_DM_RFWR_RFW_SHFT                                                                                                       0x0

#define HWIO_UART_DM_HCR_ADDR(x)                                                                                                  ((x) + 0x00000024)
#define HWIO_UART_DM_HCR_RMSK                                                                                                           0xff
#define HWIO_UART_DM_HCR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_HCR_ADDR(x), HWIO_UART_DM_HCR_RMSK)
#define HWIO_UART_DM_HCR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_HCR_ADDR(x), m)
#define HWIO_UART_DM_HCR_OUT(x, v)      \
        out_dword(HWIO_UART_DM_HCR_ADDR(x),v)
#define HWIO_UART_DM_HCR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_HCR_ADDR(x),m,v,HWIO_UART_DM_HCR_IN(x))
#define HWIO_UART_DM_HCR_DATA_BMSK                                                                                                      0xff
#define HWIO_UART_DM_HCR_DATA_SHFT                                                                                                       0x0

#define HWIO_UART_DM_DMRX_ADDR(x)                                                                                                 ((x) + 0x00000034)
#define HWIO_UART_DM_DMRX_RMSK                                                                                                     0x1ffffff
#define HWIO_UART_DM_DMRX_IN(x)      \
        in_dword_masked(HWIO_UART_DM_DMRX_ADDR(x), HWIO_UART_DM_DMRX_RMSK)
#define HWIO_UART_DM_DMRX_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_DMRX_ADDR(x), m)
#define HWIO_UART_DM_DMRX_OUT(x, v)      \
        out_dword(HWIO_UART_DM_DMRX_ADDR(x),v)
#define HWIO_UART_DM_DMRX_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_DMRX_ADDR(x),m,v,HWIO_UART_DM_DMRX_IN(x))
#define HWIO_UART_DM_DMRX_RX_DM_CRCI_CHARS_BMSK                                                                                    0x1ffffff
#define HWIO_UART_DM_DMRX_RX_DM_CRCI_CHARS_SHFT                                                                                          0x0

#define HWIO_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR(x)                                                                        ((x) + 0x00000038)
#define HWIO_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_RMSK                                                                           0xffffffff
#define HWIO_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_IN(x)      \
        in_dword_masked(HWIO_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR(x), HWIO_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_RMSK)
#define HWIO_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR(x), m)
#define HWIO_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_OUT(x, v)      \
        out_dword(HWIO_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR(x),v)
#define HWIO_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_ADDR(x),m,v,HWIO_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_IN(x))
#define HWIO_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_BMSK                                     0xffffffff
#define HWIO_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_UART_DM_IRDA_RX_TOTAL_SNAP_DEPRECATED_SHFT                                            0x0

#define HWIO_UART_DM_DMEN_ADDR(x)                                                                                                 ((x) + 0x0000003c)
#define HWIO_UART_DM_DMEN_RMSK                                                                                                          0x3c
#define HWIO_UART_DM_DMEN_IN(x)      \
        in_dword_masked(HWIO_UART_DM_DMEN_ADDR(x), HWIO_UART_DM_DMEN_RMSK)
#define HWIO_UART_DM_DMEN_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_DMEN_ADDR(x), m)
#define HWIO_UART_DM_DMEN_OUT(x, v)      \
        out_dword(HWIO_UART_DM_DMEN_ADDR(x),v)
#define HWIO_UART_DM_DMEN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_DMEN_ADDR(x),m,v,HWIO_UART_DM_DMEN_IN(x))
#define HWIO_UART_DM_DMEN_RX_SC_ENABLE_BMSK                                                                                             0x20
#define HWIO_UART_DM_DMEN_RX_SC_ENABLE_SHFT                                                                                              0x5
#define HWIO_UART_DM_DMEN_TX_SC_ENABLE_BMSK                                                                                             0x10
#define HWIO_UART_DM_DMEN_TX_SC_ENABLE_SHFT                                                                                              0x4
#define HWIO_UART_DM_DMEN_RX_BAM_ENABLE_BMSK                                                                                             0x8
#define HWIO_UART_DM_DMEN_RX_BAM_ENABLE_SHFT                                                                                             0x3
#define HWIO_UART_DM_DMEN_TX_BAM_ENABLE_BMSK                                                                                             0x4
#define HWIO_UART_DM_DMEN_TX_BAM_ENABLE_SHFT                                                                                             0x2

#define HWIO_UART_DM_NO_CHARS_FOR_TX_ADDR(x)                                                                                      ((x) + 0x00000040)
#define HWIO_UART_DM_NO_CHARS_FOR_TX_RMSK                                                                                           0xffffff
#define HWIO_UART_DM_NO_CHARS_FOR_TX_IN(x)      \
        in_dword_masked(HWIO_UART_DM_NO_CHARS_FOR_TX_ADDR(x), HWIO_UART_DM_NO_CHARS_FOR_TX_RMSK)
#define HWIO_UART_DM_NO_CHARS_FOR_TX_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_NO_CHARS_FOR_TX_ADDR(x), m)
#define HWIO_UART_DM_NO_CHARS_FOR_TX_OUT(x, v)      \
        out_dword(HWIO_UART_DM_NO_CHARS_FOR_TX_ADDR(x),v)
#define HWIO_UART_DM_NO_CHARS_FOR_TX_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_NO_CHARS_FOR_TX_ADDR(x),m,v,HWIO_UART_DM_NO_CHARS_FOR_TX_IN(x))
#define HWIO_UART_DM_NO_CHARS_FOR_TX_TX_TOTAL_TRANS_LEN_BMSK                                                                        0xffffff
#define HWIO_UART_DM_NO_CHARS_FOR_TX_TX_TOTAL_TRANS_LEN_SHFT                                                                             0x0

#define HWIO_UART_DM_BADR_ADDR(x)                                                                                                 ((x) + 0x00000044)
#define HWIO_UART_DM_BADR_RMSK                                                                                                    0xfffffffc
#define HWIO_UART_DM_BADR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_BADR_ADDR(x), HWIO_UART_DM_BADR_RMSK)
#define HWIO_UART_DM_BADR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_BADR_ADDR(x), m)
#define HWIO_UART_DM_BADR_OUT(x, v)      \
        out_dword(HWIO_UART_DM_BADR_ADDR(x),v)
#define HWIO_UART_DM_BADR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_BADR_ADDR(x),m,v,HWIO_UART_DM_BADR_IN(x))
#define HWIO_UART_DM_BADR_RX_BASE_ADDR_BMSK                                                                                       0xfffffffc
#define HWIO_UART_DM_BADR_RX_BASE_ADDR_SHFT                                                                                              0x2

#define HWIO_UART_DM_TESTSL_ADDR(x)                                                                                               ((x) + 0x00000048)
#define HWIO_UART_DM_TESTSL_RMSK                                                                                                        0x1f
#define HWIO_UART_DM_TESTSL_IN(x)      \
        in_dword_masked(HWIO_UART_DM_TESTSL_ADDR(x), HWIO_UART_DM_TESTSL_RMSK)
#define HWIO_UART_DM_TESTSL_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_TESTSL_ADDR(x), m)
#define HWIO_UART_DM_TESTSL_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TESTSL_ADDR(x),v)
#define HWIO_UART_DM_TESTSL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_TESTSL_ADDR(x),m,v,HWIO_UART_DM_TESTSL_IN(x))
#define HWIO_UART_DM_TESTSL_TEST_EN_BMSK                                                                                                0x10
#define HWIO_UART_DM_TESTSL_TEST_EN_SHFT                                                                                                 0x4
#define HWIO_UART_DM_TESTSL_TEST_SEL_BMSK                                                                                                0xf
#define HWIO_UART_DM_TESTSL_TEST_SEL_SHFT                                                                                                0x0

#define HWIO_UART_DM_TXFS_ADDR(x)                                                                                                 ((x) + 0x0000004c)
#define HWIO_UART_DM_TXFS_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_TXFS_IN(x)      \
        in_dword_masked(HWIO_UART_DM_TXFS_ADDR(x), HWIO_UART_DM_TXFS_RMSK)
#define HWIO_UART_DM_TXFS_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_TXFS_ADDR(x), m)
#define HWIO_UART_DM_TXFS_TX_FIFO_STATE_MSB_BMSK                                                                                  0xffffc000
#define HWIO_UART_DM_TXFS_TX_FIFO_STATE_MSB_SHFT                                                                                         0xe
#define HWIO_UART_DM_TXFS_TX_ASYNC_FIFO_STATE_BMSK                                                                                    0x3c00
#define HWIO_UART_DM_TXFS_TX_ASYNC_FIFO_STATE_SHFT                                                                                       0xa
#define HWIO_UART_DM_TXFS_TX_BUFFER_STATE_BMSK                                                                                         0x380
#define HWIO_UART_DM_TXFS_TX_BUFFER_STATE_SHFT                                                                                           0x7
#define HWIO_UART_DM_TXFS_TX_FIFO_STATE_LSB_BMSK                                                                                        0x7f
#define HWIO_UART_DM_TXFS_TX_FIFO_STATE_LSB_SHFT                                                                                         0x0

#define HWIO_UART_DM_RXFS_ADDR(x)                                                                                                 ((x) + 0x00000050)
#define HWIO_UART_DM_RXFS_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_RXFS_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RXFS_ADDR(x), HWIO_UART_DM_RXFS_RMSK)
#define HWIO_UART_DM_RXFS_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RXFS_ADDR(x), m)
#define HWIO_UART_DM_RXFS_RX_FIFO_STATE_MSB_BMSK                                                                                  0xffffc000
#define HWIO_UART_DM_RXFS_RX_FIFO_STATE_MSB_SHFT                                                                                         0xe
#define HWIO_UART_DM_RXFS_RX_ASYNC_FIFO_STATE_BMSK                                                                                    0x3c00
#define HWIO_UART_DM_RXFS_RX_ASYNC_FIFO_STATE_SHFT                                                                                       0xa
#define HWIO_UART_DM_RXFS_RX_BUFFER_STATE_BMSK                                                                                         0x380
#define HWIO_UART_DM_RXFS_RX_BUFFER_STATE_SHFT                                                                                           0x7
#define HWIO_UART_DM_RXFS_RX_FIFO_STATE_LSB_BMSK                                                                                        0x7f
#define HWIO_UART_DM_RXFS_RX_FIFO_STATE_LSB_SHFT                                                                                         0x0

#define HWIO_UART_DM_MISR_MODE_ADDR(x)                                                                                            ((x) + 0x00000060)
#define HWIO_UART_DM_MISR_MODE_RMSK                                                                                                      0x3
#define HWIO_UART_DM_MISR_MODE_IN(x)      \
        in_dword_masked(HWIO_UART_DM_MISR_MODE_ADDR(x), HWIO_UART_DM_MISR_MODE_RMSK)
#define HWIO_UART_DM_MISR_MODE_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_MISR_MODE_ADDR(x), m)
#define HWIO_UART_DM_MISR_MODE_OUT(x, v)      \
        out_dword(HWIO_UART_DM_MISR_MODE_ADDR(x),v)
#define HWIO_UART_DM_MISR_MODE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_MISR_MODE_ADDR(x),m,v,HWIO_UART_DM_MISR_MODE_IN(x))
#define HWIO_UART_DM_MISR_MODE_MODE_BMSK                                                                                                 0x3
#define HWIO_UART_DM_MISR_MODE_MODE_SHFT                                                                                                 0x0
#define HWIO_UART_DM_MISR_MODE_MODE_DISABLED_FVAL                                                                                        0x0
#define HWIO_UART_DM_MISR_MODE_MODE_ENABLED_TX_TEST_FVAL                                                                                 0x1
#define HWIO_UART_DM_MISR_MODE_MODE_ENABLED_RX_TEST_FVAL                                                                                 0x2

#define HWIO_UART_DM_MISR_RESET_ADDR(x)                                                                                           ((x) + 0x00000064)
#define HWIO_UART_DM_MISR_RESET_RMSK                                                                                                     0x1
#define HWIO_UART_DM_MISR_RESET_OUT(x, v)      \
        out_dword(HWIO_UART_DM_MISR_RESET_ADDR(x),v)
#define HWIO_UART_DM_MISR_RESET_RESET_BMSK                                                                                               0x1
#define HWIO_UART_DM_MISR_RESET_RESET_SHFT                                                                                               0x0

#define HWIO_UART_DM_MISR_EXPORT_ADDR(x)                                                                                          ((x) + 0x00000068)
#define HWIO_UART_DM_MISR_EXPORT_RMSK                                                                                                    0x1
#define HWIO_UART_DM_MISR_EXPORT_IN(x)      \
        in_dword_masked(HWIO_UART_DM_MISR_EXPORT_ADDR(x), HWIO_UART_DM_MISR_EXPORT_RMSK)
#define HWIO_UART_DM_MISR_EXPORT_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_MISR_EXPORT_ADDR(x), m)
#define HWIO_UART_DM_MISR_EXPORT_OUT(x, v)      \
        out_dword(HWIO_UART_DM_MISR_EXPORT_ADDR(x),v)
#define HWIO_UART_DM_MISR_EXPORT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_MISR_EXPORT_ADDR(x),m,v,HWIO_UART_DM_MISR_EXPORT_IN(x))
#define HWIO_UART_DM_MISR_EXPORT_EXPORT_BMSK                                                                                             0x1
#define HWIO_UART_DM_MISR_EXPORT_EXPORT_SHFT                                                                                             0x0

#define HWIO_UART_DM_MISR_VAL_ADDR(x)                                                                                             ((x) + 0x0000006c)
#define HWIO_UART_DM_MISR_VAL_RMSK                                                                                                     0x3ff
#define HWIO_UART_DM_MISR_VAL_IN(x)      \
        in_dword_masked(HWIO_UART_DM_MISR_VAL_ADDR(x), HWIO_UART_DM_MISR_VAL_RMSK)
#define HWIO_UART_DM_MISR_VAL_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_MISR_VAL_ADDR(x), m)
#define HWIO_UART_DM_MISR_VAL_VAL_BMSK                                                                                                 0x3ff
#define HWIO_UART_DM_MISR_VAL_VAL_SHFT                                                                                                   0x0

#define HWIO_UART_DM_TF_RF_DEPRECATED_ADDR(x)                                                                                     ((x) + 0x00000070)
#define HWIO_UART_DM_TF_RF_DEPRECATED_RMSK                                                                                        0xffffffff
#define HWIO_UART_DM_TF_RF_DEPRECATED_IN(x)      \
        in_dword_masked(HWIO_UART_DM_TF_RF_DEPRECATED_ADDR(x), HWIO_UART_DM_TF_RF_DEPRECATED_RMSK)
#define HWIO_UART_DM_TF_RF_DEPRECATED_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_TF_RF_DEPRECATED_ADDR(x), m)
#define HWIO_UART_DM_TF_RF_DEPRECATED_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_RF_DEPRECATED_ADDR(x),v)
#define HWIO_UART_DM_TF_RF_DEPRECATED_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_TF_RF_DEPRECATED_ADDR(x),m,v,HWIO_UART_DM_TF_RF_DEPRECATED_IN(x))
#define HWIO_UART_DM_TF_RF_DEPRECATED_UART_DM_TF_RF_DEPRECATED_BMSK                                                               0xffffffff
#define HWIO_UART_DM_TF_RF_DEPRECATED_UART_DM_TF_RF_DEPRECATED_SHFT                                                                      0x0

#define HWIO_UART_DM_TF_RF_2_DEPRECATED_ADDR(x)                                                                                   ((x) + 0x00000074)
#define HWIO_UART_DM_TF_RF_2_DEPRECATED_RMSK                                                                                      0xffffffff
#define HWIO_UART_DM_TF_RF_2_DEPRECATED_IN(x)      \
        in_dword_masked(HWIO_UART_DM_TF_RF_2_DEPRECATED_ADDR(x), HWIO_UART_DM_TF_RF_2_DEPRECATED_RMSK)
#define HWIO_UART_DM_TF_RF_2_DEPRECATED_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_TF_RF_2_DEPRECATED_ADDR(x), m)
#define HWIO_UART_DM_TF_RF_2_DEPRECATED_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_RF_2_DEPRECATED_ADDR(x),v)
#define HWIO_UART_DM_TF_RF_2_DEPRECATED_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_TF_RF_2_DEPRECATED_ADDR(x),m,v,HWIO_UART_DM_TF_RF_2_DEPRECATED_IN(x))
#define HWIO_UART_DM_TF_RF_2_DEPRECATED_UART_DM_TF_RF_2_DEPRECATED_BMSK                                                           0xffffffff
#define HWIO_UART_DM_TF_RF_2_DEPRECATED_UART_DM_TF_RF_2_DEPRECATED_SHFT                                                                  0x0

#define HWIO_UART_DM_TF_RF_3_DEPRECATED_ADDR(x)                                                                                   ((x) + 0x00000078)
#define HWIO_UART_DM_TF_RF_3_DEPRECATED_RMSK                                                                                      0xffffffff
#define HWIO_UART_DM_TF_RF_3_DEPRECATED_IN(x)      \
        in_dword_masked(HWIO_UART_DM_TF_RF_3_DEPRECATED_ADDR(x), HWIO_UART_DM_TF_RF_3_DEPRECATED_RMSK)
#define HWIO_UART_DM_TF_RF_3_DEPRECATED_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_TF_RF_3_DEPRECATED_ADDR(x), m)
#define HWIO_UART_DM_TF_RF_3_DEPRECATED_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_RF_3_DEPRECATED_ADDR(x),v)
#define HWIO_UART_DM_TF_RF_3_DEPRECATED_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_TF_RF_3_DEPRECATED_ADDR(x),m,v,HWIO_UART_DM_TF_RF_3_DEPRECATED_IN(x))
#define HWIO_UART_DM_TF_RF_3_DEPRECATED_UART_DM_TF_RF_3_DEPRECATED_BMSK                                                           0xffffffff
#define HWIO_UART_DM_TF_RF_3_DEPRECATED_UART_DM_TF_RF_3_DEPRECATED_SHFT                                                                  0x0

#define HWIO_UART_DM_TF_RF_4_DEPRECATED_ADDR(x)                                                                                   ((x) + 0x0000007c)
#define HWIO_UART_DM_TF_RF_4_DEPRECATED_RMSK                                                                                      0xffffffff
#define HWIO_UART_DM_TF_RF_4_DEPRECATED_IN(x)      \
        in_dword_masked(HWIO_UART_DM_TF_RF_4_DEPRECATED_ADDR(x), HWIO_UART_DM_TF_RF_4_DEPRECATED_RMSK)
#define HWIO_UART_DM_TF_RF_4_DEPRECATED_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_TF_RF_4_DEPRECATED_ADDR(x), m)
#define HWIO_UART_DM_TF_RF_4_DEPRECATED_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_RF_4_DEPRECATED_ADDR(x),v)
#define HWIO_UART_DM_TF_RF_4_DEPRECATED_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_TF_RF_4_DEPRECATED_ADDR(x),m,v,HWIO_UART_DM_TF_RF_4_DEPRECATED_IN(x))
#define HWIO_UART_DM_TF_RF_4_DEPRECATED_UART_DM_TF_RF_4_DEPRECATED_BMSK                                                           0xffffffff
#define HWIO_UART_DM_TF_RF_4_DEPRECATED_UART_DM_TF_RF_4_DEPRECATED_SHFT                                                                  0x0

#define HWIO_UART_DM_SIM_CFG_ADDR(x)                                                                                              ((x) + 0x00000080)
#define HWIO_UART_DM_SIM_CFG_RMSK                                                                                                    0x3ffef
#define HWIO_UART_DM_SIM_CFG_IN(x)      \
        in_dword_masked(HWIO_UART_DM_SIM_CFG_ADDR(x), HWIO_UART_DM_SIM_CFG_RMSK)
#define HWIO_UART_DM_SIM_CFG_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_SIM_CFG_ADDR(x), m)
#define HWIO_UART_DM_SIM_CFG_OUT(x, v)      \
        out_dword(HWIO_UART_DM_SIM_CFG_ADDR(x),v)
#define HWIO_UART_DM_SIM_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_SIM_CFG_ADDR(x),m,v,HWIO_UART_DM_SIM_CFG_IN(x))
#define HWIO_UART_DM_SIM_CFG_UIM_TX_MODE_BMSK                                                                                        0x20000
#define HWIO_UART_DM_SIM_CFG_UIM_TX_MODE_SHFT                                                                                           0x11
#define HWIO_UART_DM_SIM_CFG_UIM_RX_MODE_BMSK                                                                                        0x10000
#define HWIO_UART_DM_SIM_CFG_UIM_RX_MODE_SHFT                                                                                           0x10
#define HWIO_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_BMSK                                                                                    0xff00
#define HWIO_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_SHFT                                                                                       0x8
#define HWIO_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_ENUM_1_BIT_TIMES_FVAL                                                                      0x1
#define HWIO_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_ENUM_2_BIT_TIMES_FVAL                                                                      0x2
#define HWIO_UART_DM_SIM_CFG_SIM_CLK_ON_BMSK                                                                                            0x80
#define HWIO_UART_DM_SIM_CFG_SIM_CLK_ON_SHFT                                                                                             0x7
#define HWIO_UART_DM_SIM_CFG_SIM_CLK_TD8_SEL_BMSK                                                                                       0x40
#define HWIO_UART_DM_SIM_CFG_SIM_CLK_TD8_SEL_SHFT                                                                                        0x6
#define HWIO_UART_DM_SIM_CFG_SIM_CLK_TD8_SEL_TD4_FVAL                                                                                    0x0
#define HWIO_UART_DM_SIM_CFG_SIM_CLK_TD8_SEL_TD8_FVAL                                                                                    0x1
#define HWIO_UART_DM_SIM_CFG_SIM_CLK_STOP_HIGH_BMSK                                                                                     0x20
#define HWIO_UART_DM_SIM_CFG_SIM_CLK_STOP_HIGH_SHFT                                                                                      0x5
#define HWIO_UART_DM_SIM_CFG_SIM_CLK_STOP_HIGH_LOW_FVAL                                                                                  0x0
#define HWIO_UART_DM_SIM_CFG_SIM_CLK_STOP_HIGH_HIGH_FVAL                                                                                 0x1
#define HWIO_UART_DM_SIM_CFG_MASK_RX_BMSK                                                                                                0x8
#define HWIO_UART_DM_SIM_CFG_MASK_RX_SHFT                                                                                                0x3
#define HWIO_UART_DM_SIM_CFG_SWAP_D_BMSK                                                                                                 0x4
#define HWIO_UART_DM_SIM_CFG_SWAP_D_SHFT                                                                                                 0x2
#define HWIO_UART_DM_SIM_CFG_INV_D_BMSK                                                                                                  0x2
#define HWIO_UART_DM_SIM_CFG_INV_D_SHFT                                                                                                  0x1
#define HWIO_UART_DM_SIM_CFG_SIM_SEL_BMSK                                                                                                0x1
#define HWIO_UART_DM_SIM_CFG_SIM_SEL_SHFT                                                                                                0x0

#define HWIO_UART_DM_TEST_WR_ADDR_ADDR(x)                                                                                         ((x) + 0x00000084)
#define HWIO_UART_DM_TEST_WR_ADDR_RMSK                                                                                            0xffffffff
#define HWIO_UART_DM_TEST_WR_ADDR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_TEST_WR_ADDR_ADDR(x), HWIO_UART_DM_TEST_WR_ADDR_RMSK)
#define HWIO_UART_DM_TEST_WR_ADDR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_TEST_WR_ADDR_ADDR(x), m)
#define HWIO_UART_DM_TEST_WR_ADDR_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TEST_WR_ADDR_ADDR(x),v)
#define HWIO_UART_DM_TEST_WR_ADDR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_TEST_WR_ADDR_ADDR(x),m,v,HWIO_UART_DM_TEST_WR_ADDR_IN(x))
#define HWIO_UART_DM_TEST_WR_ADDR_TEST_WR_ADDR_BMSK                                                                               0xffffffff
#define HWIO_UART_DM_TEST_WR_ADDR_TEST_WR_ADDR_SHFT                                                                                      0x0

#define HWIO_UART_DM_TEST_WR_DATA_ADDR(x)                                                                                         ((x) + 0x00000088)
#define HWIO_UART_DM_TEST_WR_DATA_RMSK                                                                                            0xffffffff
#define HWIO_UART_DM_TEST_WR_DATA_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TEST_WR_DATA_ADDR(x),v)
#define HWIO_UART_DM_TEST_WR_DATA_TEST_WR_DATA_BMSK                                                                               0xffffffff
#define HWIO_UART_DM_TEST_WR_DATA_TEST_WR_DATA_SHFT                                                                                      0x0

#define HWIO_UART_DM_TEST_RD_ADDR_ADDR(x)                                                                                         ((x) + 0x0000008c)
#define HWIO_UART_DM_TEST_RD_ADDR_RMSK                                                                                            0xffffffff
#define HWIO_UART_DM_TEST_RD_ADDR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_TEST_RD_ADDR_ADDR(x), HWIO_UART_DM_TEST_RD_ADDR_RMSK)
#define HWIO_UART_DM_TEST_RD_ADDR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_TEST_RD_ADDR_ADDR(x), m)
#define HWIO_UART_DM_TEST_RD_ADDR_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TEST_RD_ADDR_ADDR(x),v)
#define HWIO_UART_DM_TEST_RD_ADDR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_TEST_RD_ADDR_ADDR(x),m,v,HWIO_UART_DM_TEST_RD_ADDR_IN(x))
#define HWIO_UART_DM_TEST_RD_ADDR_TEST_RD_ADDR_BMSK                                                                               0xffffffff
#define HWIO_UART_DM_TEST_RD_ADDR_TEST_RD_ADDR_SHFT                                                                                      0x0

#define HWIO_UART_DM_TEST_RD_DATA_ADDR(x)                                                                                         ((x) + 0x00000090)
#define HWIO_UART_DM_TEST_RD_DATA_RMSK                                                                                            0xffffffff
#define HWIO_UART_DM_TEST_RD_DATA_IN(x)      \
        in_dword_masked(HWIO_UART_DM_TEST_RD_DATA_ADDR(x), HWIO_UART_DM_TEST_RD_DATA_RMSK)
#define HWIO_UART_DM_TEST_RD_DATA_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_TEST_RD_DATA_ADDR(x), m)
#define HWIO_UART_DM_TEST_RD_DATA_TEST_RD_DATA_BMSK                                                                               0xffffffff
#define HWIO_UART_DM_TEST_RD_DATA_TEST_RD_DATA_SHFT                                                                                      0x0

#define HWIO_UART_DM_CSR_ADDR(x)                                                                                                  ((x) + 0x000000a0)
#define HWIO_UART_DM_CSR_RMSK                                                                                                           0xff
#define HWIO_UART_DM_CSR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_CSR_ADDR(x), HWIO_UART_DM_CSR_RMSK)
#define HWIO_UART_DM_CSR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_CSR_ADDR(x), m)
#define HWIO_UART_DM_CSR_OUT(x, v)      \
        out_dword(HWIO_UART_DM_CSR_ADDR(x),v)
#define HWIO_UART_DM_CSR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_CSR_ADDR(x),m,v,HWIO_UART_DM_CSR_IN(x))
#define HWIO_UART_DM_CSR_UART_RX_CLK_SEL_BMSK                                                                                           0xf0
#define HWIO_UART_DM_CSR_UART_RX_CLK_SEL_SHFT                                                                                            0x4
#define HWIO_UART_DM_CSR_UART_TX_CLK_SEL_BMSK                                                                                            0xf
#define HWIO_UART_DM_CSR_UART_TX_CLK_SEL_SHFT                                                                                            0x0

#define HWIO_UART_DM_SR_ADDR(x)                                                                                                   ((x) + 0x000000a4)
#define HWIO_UART_DM_SR_RMSK                                                                                                           0xfff
#define HWIO_UART_DM_SR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_SR_ADDR(x), HWIO_UART_DM_SR_RMSK)
#define HWIO_UART_DM_SR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_SR_ADDR(x), m)
#define HWIO_UART_DM_SR_TRANS_END_TRIGGER_BMSK                                                                                         0xc00
#define HWIO_UART_DM_SR_TRANS_END_TRIGGER_SHFT                                                                                           0xa
#define HWIO_UART_DM_SR_TRANS_ACTIVE_BMSK                                                                                              0x200
#define HWIO_UART_DM_SR_TRANS_ACTIVE_SHFT                                                                                                0x9
#define HWIO_UART_DM_SR_RX_BREAK_START_LAST_BMSK                                                                                       0x100
#define HWIO_UART_DM_SR_RX_BREAK_START_LAST_SHFT                                                                                         0x8
#define HWIO_UART_DM_SR_HUNT_CHAR_BMSK                                                                                                  0x80
#define HWIO_UART_DM_SR_HUNT_CHAR_SHFT                                                                                                   0x7
#define HWIO_UART_DM_SR_RX_BREAK_BMSK                                                                                                   0x40
#define HWIO_UART_DM_SR_RX_BREAK_SHFT                                                                                                    0x6
#define HWIO_UART_DM_SR_PAR_FRAME_ERR_BMSK                                                                                              0x20
#define HWIO_UART_DM_SR_PAR_FRAME_ERR_SHFT                                                                                               0x5
#define HWIO_UART_DM_SR_UART_OVERRUN_BMSK                                                                                               0x10
#define HWIO_UART_DM_SR_UART_OVERRUN_SHFT                                                                                                0x4
#define HWIO_UART_DM_SR_TXEMT_BMSK                                                                                                       0x8
#define HWIO_UART_DM_SR_TXEMT_SHFT                                                                                                       0x3
#define HWIO_UART_DM_SR_TXRDY_BMSK                                                                                                       0x4
#define HWIO_UART_DM_SR_TXRDY_SHFT                                                                                                       0x2
#define HWIO_UART_DM_SR_RXFULL_BMSK                                                                                                      0x2
#define HWIO_UART_DM_SR_RXFULL_SHFT                                                                                                      0x1
#define HWIO_UART_DM_SR_RXRDY_BMSK                                                                                                       0x1
#define HWIO_UART_DM_SR_RXRDY_SHFT                                                                                                       0x0

#define HWIO_UART_DM_CR_ADDR(x)                                                                                                   ((x) + 0x000000a8)
#define HWIO_UART_DM_CR_RMSK                                                                                                           0xfff
#define HWIO_UART_DM_CR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_CR_ADDR(x), HWIO_UART_DM_CR_RMSK)
#define HWIO_UART_DM_CR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_CR_ADDR(x), m)
#define HWIO_UART_DM_CR_OUT(x, v)      \
        out_dword(HWIO_UART_DM_CR_ADDR(x),v)
#define HWIO_UART_DM_CR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_CR_ADDR(x),m,v,HWIO_UART_DM_CR_IN(x))
#define HWIO_UART_DM_CR_CHANNEL_COMMAND_MSB_BMSK                                                                                       0x800
#define HWIO_UART_DM_CR_CHANNEL_COMMAND_MSB_SHFT                                                                                         0xb
#define HWIO_UART_DM_CR_GENERAL_COMMAND_BMSK                                                                                           0x700
#define HWIO_UART_DM_CR_GENERAL_COMMAND_SHFT                                                                                             0x8
#define HWIO_UART_DM_CR_CHANNEL_COMMAND_LSB_BMSK                                                                                        0xf0
#define HWIO_UART_DM_CR_CHANNEL_COMMAND_LSB_SHFT                                                                                         0x4
#define HWIO_UART_DM_CR_UART_TX_DISABLE_BMSK                                                                                             0x8
#define HWIO_UART_DM_CR_UART_TX_DISABLE_SHFT                                                                                             0x3
#define HWIO_UART_DM_CR_UART_TX_EN_BMSK                                                                                                  0x4
#define HWIO_UART_DM_CR_UART_TX_EN_SHFT                                                                                                  0x2
#define HWIO_UART_DM_CR_UART_RX_DISABLE_BMSK                                                                                             0x2
#define HWIO_UART_DM_CR_UART_RX_DISABLE_SHFT                                                                                             0x1
#define HWIO_UART_DM_CR_UART_RX_EN_BMSK                                                                                                  0x1
#define HWIO_UART_DM_CR_UART_RX_EN_SHFT                                                                                                  0x0

#define HWIO_UART_DM_MISR_ADDR(x)                                                                                                 ((x) + 0x000000ac)
#define HWIO_UART_DM_MISR_RMSK                                                                                                       0x1ffff
#define HWIO_UART_DM_MISR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_MISR_ADDR(x), HWIO_UART_DM_MISR_RMSK)
#define HWIO_UART_DM_MISR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_MISR_ADDR(x), m)
#define HWIO_UART_DM_MISR_UART_MISR_BMSK                                                                                             0x1ffff
#define HWIO_UART_DM_MISR_UART_MISR_SHFT                                                                                                 0x0

#define HWIO_UART_DM_IMR_ADDR(x)                                                                                                  ((x) + 0x000000b0)
#define HWIO_UART_DM_IMR_RMSK                                                                                                        0x1ffff
#define HWIO_UART_DM_IMR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_IMR_ADDR(x), HWIO_UART_DM_IMR_RMSK)
#define HWIO_UART_DM_IMR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_IMR_ADDR(x), m)
#define HWIO_UART_DM_IMR_OUT(x, v)      \
        out_dword(HWIO_UART_DM_IMR_ADDR(x),v)
#define HWIO_UART_DM_IMR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_IMR_ADDR(x),m,v,HWIO_UART_DM_IMR_IN(x))
#define HWIO_UART_DM_IMR_WWT_IRQ_BMSK                                                                                                0x10000
#define HWIO_UART_DM_IMR_WWT_IRQ_SHFT                                                                                                   0x10
#define HWIO_UART_DM_IMR_TXCOMP_IRQ_BMSK                                                                                              0x8000
#define HWIO_UART_DM_IMR_TXCOMP_IRQ_SHFT                                                                                                 0xf
#define HWIO_UART_DM_IMR_RX_RD_ERROR_IRQ_BMSK                                                                                         0x4000
#define HWIO_UART_DM_IMR_RX_RD_ERROR_IRQ_SHFT                                                                                            0xe
#define HWIO_UART_DM_IMR_TX_WR_ERROR_IRQ_BMSK                                                                                         0x2000
#define HWIO_UART_DM_IMR_TX_WR_ERROR_IRQ_SHFT                                                                                            0xd
#define HWIO_UART_DM_IMR_PAR_FRAME_ERR_IRQ_BMSK                                                                                       0x1000
#define HWIO_UART_DM_IMR_PAR_FRAME_ERR_IRQ_SHFT                                                                                          0xc
#define HWIO_UART_DM_IMR_RXBREAK_END_BMSK                                                                                              0x800
#define HWIO_UART_DM_IMR_RXBREAK_END_SHFT                                                                                                0xb
#define HWIO_UART_DM_IMR_RXBREAK_START_BMSK                                                                                            0x400
#define HWIO_UART_DM_IMR_RXBREAK_START_SHFT                                                                                              0xa
#define HWIO_UART_DM_IMR_TX_DONE_BMSK                                                                                                  0x200
#define HWIO_UART_DM_IMR_TX_DONE_SHFT                                                                                                    0x9
#define HWIO_UART_DM_IMR_TX_ERROR_BMSK                                                                                                 0x100
#define HWIO_UART_DM_IMR_TX_ERROR_SHFT                                                                                                   0x8
#define HWIO_UART_DM_IMR_TX_READY_BMSK                                                                                                  0x80
#define HWIO_UART_DM_IMR_TX_READY_SHFT                                                                                                   0x7
#define HWIO_UART_DM_IMR_CURRENT_CTS_BMSK                                                                                               0x40
#define HWIO_UART_DM_IMR_CURRENT_CTS_SHFT                                                                                                0x6
#define HWIO_UART_DM_IMR_DELTA_CTS_BMSK                                                                                                 0x20
#define HWIO_UART_DM_IMR_DELTA_CTS_SHFT                                                                                                  0x5
#define HWIO_UART_DM_IMR_RXLEV_BMSK                                                                                                     0x10
#define HWIO_UART_DM_IMR_RXLEV_SHFT                                                                                                      0x4
#define HWIO_UART_DM_IMR_RXSTALE_BMSK                                                                                                    0x8
#define HWIO_UART_DM_IMR_RXSTALE_SHFT                                                                                                    0x3
#define HWIO_UART_DM_IMR_RXBREAK_CHANGE_BMSK                                                                                             0x4
#define HWIO_UART_DM_IMR_RXBREAK_CHANGE_SHFT                                                                                             0x2
#define HWIO_UART_DM_IMR_RXHUNT_BMSK                                                                                                     0x2
#define HWIO_UART_DM_IMR_RXHUNT_SHFT                                                                                                     0x1
#define HWIO_UART_DM_IMR_TXLEV_BMSK                                                                                                      0x1
#define HWIO_UART_DM_IMR_TXLEV_SHFT                                                                                                      0x0

#define HWIO_UART_DM_ISR_ADDR(x)                                                                                                  ((x) + 0x000000b4)
#define HWIO_UART_DM_ISR_RMSK                                                                                                        0x1ffff
#define HWIO_UART_DM_ISR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_ISR_ADDR(x), HWIO_UART_DM_ISR_RMSK)
#define HWIO_UART_DM_ISR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_ISR_ADDR(x), m)
#define HWIO_UART_DM_ISR_WWT_IRQ_BMSK                                                                                                0x10000
#define HWIO_UART_DM_ISR_WWT_IRQ_SHFT                                                                                                   0x10
#define HWIO_UART_DM_ISR_TXCOMP_IRQ_BMSK                                                                                              0x8000
#define HWIO_UART_DM_ISR_TXCOMP_IRQ_SHFT                                                                                                 0xf
#define HWIO_UART_DM_ISR_RX_RD_ERROR_IRQ_BMSK                                                                                         0x4000
#define HWIO_UART_DM_ISR_RX_RD_ERROR_IRQ_SHFT                                                                                            0xe
#define HWIO_UART_DM_ISR_TX_WR_ERROR_IRQ_BMSK                                                                                         0x2000
#define HWIO_UART_DM_ISR_TX_WR_ERROR_IRQ_SHFT                                                                                            0xd
#define HWIO_UART_DM_ISR_PAR_FRAME_ERR_IRQ_BMSK                                                                                       0x1000
#define HWIO_UART_DM_ISR_PAR_FRAME_ERR_IRQ_SHFT                                                                                          0xc
#define HWIO_UART_DM_ISR_RXBREAK_END_BMSK                                                                                              0x800
#define HWIO_UART_DM_ISR_RXBREAK_END_SHFT                                                                                                0xb
#define HWIO_UART_DM_ISR_RXBREAK_START_BMSK                                                                                            0x400
#define HWIO_UART_DM_ISR_RXBREAK_START_SHFT                                                                                              0xa
#define HWIO_UART_DM_ISR_TX_DONE_BMSK                                                                                                  0x200
#define HWIO_UART_DM_ISR_TX_DONE_SHFT                                                                                                    0x9
#define HWIO_UART_DM_ISR_TX_ERROR_BMSK                                                                                                 0x100
#define HWIO_UART_DM_ISR_TX_ERROR_SHFT                                                                                                   0x8
#define HWIO_UART_DM_ISR_TX_READY_BMSK                                                                                                  0x80
#define HWIO_UART_DM_ISR_TX_READY_SHFT                                                                                                   0x7
#define HWIO_UART_DM_ISR_CURRENT_CTS_BMSK                                                                                               0x40
#define HWIO_UART_DM_ISR_CURRENT_CTS_SHFT                                                                                                0x6
#define HWIO_UART_DM_ISR_DELTA_CTS_BMSK                                                                                                 0x20
#define HWIO_UART_DM_ISR_DELTA_CTS_SHFT                                                                                                  0x5
#define HWIO_UART_DM_ISR_RXLEV_BMSK                                                                                                     0x10
#define HWIO_UART_DM_ISR_RXLEV_SHFT                                                                                                      0x4
#define HWIO_UART_DM_ISR_RXSTALE_BMSK                                                                                                    0x8
#define HWIO_UART_DM_ISR_RXSTALE_SHFT                                                                                                    0x3
#define HWIO_UART_DM_ISR_RXBREAK_CHANGE_BMSK                                                                                             0x4
#define HWIO_UART_DM_ISR_RXBREAK_CHANGE_SHFT                                                                                             0x2
#define HWIO_UART_DM_ISR_RXHUNT_BMSK                                                                                                     0x2
#define HWIO_UART_DM_ISR_RXHUNT_SHFT                                                                                                     0x1
#define HWIO_UART_DM_ISR_TXLEV_BMSK                                                                                                      0x1
#define HWIO_UART_DM_ISR_TXLEV_SHFT                                                                                                      0x0

#define HWIO_UART_DM_IRDA_ADDR(x)                                                                                                 ((x) + 0x000000b8)
#define HWIO_UART_DM_IRDA_RMSK                                                                                                          0x1f
#define HWIO_UART_DM_IRDA_IN(x)      \
        in_dword_masked(HWIO_UART_DM_IRDA_ADDR(x), HWIO_UART_DM_IRDA_RMSK)
#define HWIO_UART_DM_IRDA_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_IRDA_ADDR(x), m)
#define HWIO_UART_DM_IRDA_OUT(x, v)      \
        out_dword(HWIO_UART_DM_IRDA_ADDR(x),v)
#define HWIO_UART_DM_IRDA_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_IRDA_ADDR(x),m,v,HWIO_UART_DM_IRDA_IN(x))
#define HWIO_UART_DM_IRDA_MEDIUM_RATE_EN_BMSK                                                                                           0x10
#define HWIO_UART_DM_IRDA_MEDIUM_RATE_EN_SHFT                                                                                            0x4
#define HWIO_UART_DM_IRDA_IRDA_LOOPBACK_BMSK                                                                                             0x8
#define HWIO_UART_DM_IRDA_IRDA_LOOPBACK_SHFT                                                                                             0x3
#define HWIO_UART_DM_IRDA_INVERT_IRDA_TX_BMSK                                                                                            0x4
#define HWIO_UART_DM_IRDA_INVERT_IRDA_TX_SHFT                                                                                            0x2
#define HWIO_UART_DM_IRDA_INVERT_IRDA_RX_BMSK                                                                                            0x2
#define HWIO_UART_DM_IRDA_INVERT_IRDA_RX_SHFT                                                                                            0x1
#define HWIO_UART_DM_IRDA_IRDA_EN_BMSK                                                                                                   0x1
#define HWIO_UART_DM_IRDA_IRDA_EN_SHFT                                                                                                   0x0

#define HWIO_UART_DM_RX_TOTAL_SNAP_ADDR(x)                                                                                        ((x) + 0x000000bc)
#define HWIO_UART_DM_RX_TOTAL_SNAP_RMSK                                                                                             0xffffff
#define HWIO_UART_DM_RX_TOTAL_SNAP_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RX_TOTAL_SNAP_ADDR(x), HWIO_UART_DM_RX_TOTAL_SNAP_RMSK)
#define HWIO_UART_DM_RX_TOTAL_SNAP_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RX_TOTAL_SNAP_ADDR(x), m)
#define HWIO_UART_DM_RX_TOTAL_SNAP_RX_TOTAL_BYTES_BMSK                                                                              0xffffff
#define HWIO_UART_DM_RX_TOTAL_SNAP_RX_TOTAL_BYTES_SHFT                                                                                   0x0

#define HWIO_UART_DM_WWT_TIMEOUT_ADDR(x)                                                                                          ((x) + 0x000000c0)
#define HWIO_UART_DM_WWT_TIMEOUT_RMSK                                                                                              0x1ffffff
#define HWIO_UART_DM_WWT_TIMEOUT_IN(x)      \
        in_dword_masked(HWIO_UART_DM_WWT_TIMEOUT_ADDR(x), HWIO_UART_DM_WWT_TIMEOUT_RMSK)
#define HWIO_UART_DM_WWT_TIMEOUT_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_WWT_TIMEOUT_ADDR(x), m)
#define HWIO_UART_DM_WWT_TIMEOUT_OUT(x, v)      \
        out_dword(HWIO_UART_DM_WWT_TIMEOUT_ADDR(x),v)
#define HWIO_UART_DM_WWT_TIMEOUT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_WWT_TIMEOUT_ADDR(x),m,v,HWIO_UART_DM_WWT_TIMEOUT_IN(x))
#define HWIO_UART_DM_WWT_TIMEOUT_WWT_TIMEOUT_BMSK                                                                                  0x1ffffff
#define HWIO_UART_DM_WWT_TIMEOUT_WWT_TIMEOUT_SHFT                                                                                        0x0

#define HWIO_UART_DM_CLK_CTRL_ADDR(x)                                                                                             ((x) + 0x000000c4)
#define HWIO_UART_DM_CLK_CTRL_RMSK                                                                                                  0xf06731
#define HWIO_UART_DM_CLK_CTRL_IN(x)      \
        in_dword_masked(HWIO_UART_DM_CLK_CTRL_ADDR(x), HWIO_UART_DM_CLK_CTRL_RMSK)
#define HWIO_UART_DM_CLK_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_CLK_CTRL_ADDR(x), m)
#define HWIO_UART_DM_CLK_CTRL_OUT(x, v)      \
        out_dword(HWIO_UART_DM_CLK_CTRL_ADDR(x),v)
#define HWIO_UART_DM_CLK_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_CLK_CTRL_ADDR(x),m,v,HWIO_UART_DM_CLK_CTRL_IN(x))
#define HWIO_UART_DM_CLK_CTRL_UART_IRDA_CLK_CGC_OPEN_BMSK                                                                           0x800000
#define HWIO_UART_DM_CLK_CTRL_UART_IRDA_CLK_CGC_OPEN_SHFT                                                                               0x17
#define HWIO_UART_DM_CLK_CTRL_UART_SIM_CLK_CGC_OPEN_BMSK                                                                            0x400000
#define HWIO_UART_DM_CLK_CTRL_UART_SIM_CLK_CGC_OPEN_SHFT                                                                                0x16
#define HWIO_UART_DM_CLK_CTRL_UART_RX_CLK_CGC_OPEN_BMSK                                                                             0x200000
#define HWIO_UART_DM_CLK_CTRL_UART_RX_CLK_CGC_OPEN_SHFT                                                                                 0x15
#define HWIO_UART_DM_CLK_CTRL_UART_TX_CLK_CGC_OPEN_BMSK                                                                             0x100000
#define HWIO_UART_DM_CLK_CTRL_UART_TX_CLK_CGC_OPEN_SHFT                                                                                 0x14
#define HWIO_UART_DM_CLK_CTRL_AHB_RX_BAM_CLK_CGC_OPEN_BMSK                                                                            0x4000
#define HWIO_UART_DM_CLK_CTRL_AHB_RX_BAM_CLK_CGC_OPEN_SHFT                                                                               0xe
#define HWIO_UART_DM_CLK_CTRL_AHB_TX_BAM_CLK_CGC_OPEN_BMSK                                                                            0x2000
#define HWIO_UART_DM_CLK_CTRL_AHB_TX_BAM_CLK_CGC_OPEN_SHFT                                                                               0xd
#define HWIO_UART_DM_CLK_CTRL_AHB_RX_CLK_CGC_OPEN_BMSK                                                                                 0x400
#define HWIO_UART_DM_CLK_CTRL_AHB_RX_CLK_CGC_OPEN_SHFT                                                                                   0xa
#define HWIO_UART_DM_CLK_CTRL_AHB_TX_CLK_CGC_OPEN_BMSK                                                                                 0x200
#define HWIO_UART_DM_CLK_CTRL_AHB_TX_CLK_CGC_OPEN_SHFT                                                                                   0x9
#define HWIO_UART_DM_CLK_CTRL_AHB_WR_CLK_CGC_OPEN_BMSK                                                                                 0x100
#define HWIO_UART_DM_CLK_CTRL_AHB_WR_CLK_CGC_OPEN_SHFT                                                                                   0x8
#define HWIO_UART_DM_CLK_CTRL_RX_ENABLE_CGC_OPT_BMSK                                                                                    0x20
#define HWIO_UART_DM_CLK_CTRL_RX_ENABLE_CGC_OPT_SHFT                                                                                     0x5
#define HWIO_UART_DM_CLK_CTRL_TX_ENABLE_CGC_OPT_BMSK                                                                                    0x10
#define HWIO_UART_DM_CLK_CTRL_TX_ENABLE_CGC_OPT_SHFT                                                                                     0x4
#define HWIO_UART_DM_CLK_CTRL_AHB_CLK_CGC_CLOSE_BMSK                                                                                     0x1
#define HWIO_UART_DM_CLK_CTRL_AHB_CLK_CGC_CLOSE_SHFT                                                                                     0x0

#define HWIO_UART_DM_BCR_ADDR(x)                                                                                                  ((x) + 0x000000c8)
#define HWIO_UART_DM_BCR_RMSK                                                                                                           0x37
#define HWIO_UART_DM_BCR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_BCR_ADDR(x), HWIO_UART_DM_BCR_RMSK)
#define HWIO_UART_DM_BCR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_BCR_ADDR(x), m)
#define HWIO_UART_DM_BCR_OUT(x, v)      \
        out_dword(HWIO_UART_DM_BCR_ADDR(x),v)
#define HWIO_UART_DM_BCR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_BCR_ADDR(x),m,v,HWIO_UART_DM_BCR_IN(x))
#define HWIO_UART_DM_BCR_RX_DMRX_1BYTE_RES_EN_BMSK                                                                                      0x20
#define HWIO_UART_DM_BCR_RX_DMRX_1BYTE_RES_EN_SHFT                                                                                       0x5
#define HWIO_UART_DM_BCR_RX_STALE_IRQ_DMRX_EQUAL_BMSK                                                                                   0x10
#define HWIO_UART_DM_BCR_RX_STALE_IRQ_DMRX_EQUAL_SHFT                                                                                    0x4
#define HWIO_UART_DM_BCR_RX_DMRX_LOW_EN_BMSK                                                                                             0x4
#define HWIO_UART_DM_BCR_RX_DMRX_LOW_EN_SHFT                                                                                             0x2
#define HWIO_UART_DM_BCR_STALE_IRQ_EMPTY_BMSK                                                                                            0x2
#define HWIO_UART_DM_BCR_STALE_IRQ_EMPTY_SHFT                                                                                            0x1
#define HWIO_UART_DM_BCR_TX_BREAK_DISABLE_BMSK                                                                                           0x1
#define HWIO_UART_DM_BCR_TX_BREAK_DISABLE_SHFT                                                                                           0x0

#define HWIO_UART_DM_RX_TRANS_CTRL_ADDR(x)                                                                                        ((x) + 0x000000cc)
#define HWIO_UART_DM_RX_TRANS_CTRL_RMSK                                                                                                  0x7
#define HWIO_UART_DM_RX_TRANS_CTRL_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RX_TRANS_CTRL_ADDR(x), HWIO_UART_DM_RX_TRANS_CTRL_RMSK)
#define HWIO_UART_DM_RX_TRANS_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RX_TRANS_CTRL_ADDR(x), m)
#define HWIO_UART_DM_RX_TRANS_CTRL_OUT(x, v)      \
        out_dword(HWIO_UART_DM_RX_TRANS_CTRL_ADDR(x),v)
#define HWIO_UART_DM_RX_TRANS_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_RX_TRANS_CTRL_ADDR(x),m,v,HWIO_UART_DM_RX_TRANS_CTRL_IN(x))
#define HWIO_UART_DM_RX_TRANS_CTRL_RX_DMRX_CYCLIC_EN_BMSK                                                                                0x4
#define HWIO_UART_DM_RX_TRANS_CTRL_RX_DMRX_CYCLIC_EN_SHFT                                                                                0x2
#define HWIO_UART_DM_RX_TRANS_CTRL_RX_TRANS_AUTO_RE_ACTIVATE_BMSK                                                                        0x2
#define HWIO_UART_DM_RX_TRANS_CTRL_RX_TRANS_AUTO_RE_ACTIVATE_SHFT                                                                        0x1
#define HWIO_UART_DM_RX_TRANS_CTRL_RX_STALE_AUTO_RE_EN_BMSK                                                                              0x1
#define HWIO_UART_DM_RX_TRANS_CTRL_RX_STALE_AUTO_RE_EN_SHFT                                                                              0x0

#define HWIO_UART_DM_DMRX_DBG_ADDR(x)                                                                                             ((x) + 0x000000d0)
#define HWIO_UART_DM_DMRX_DBG_RMSK                                                                                                 0x1ffffff
#define HWIO_UART_DM_DMRX_DBG_IN(x)      \
        in_dword_masked(HWIO_UART_DM_DMRX_DBG_ADDR(x), HWIO_UART_DM_DMRX_DBG_RMSK)
#define HWIO_UART_DM_DMRX_DBG_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_DMRX_DBG_ADDR(x), m)
#define HWIO_UART_DM_DMRX_DBG_UART_DM_DMRX_VAL_BMSK                                                                                0x1ffffff
#define HWIO_UART_DM_DMRX_DBG_UART_DM_DMRX_VAL_SHFT                                                                                      0x0

#define HWIO_UART_DM_FSM_STATUS_ADDR(x)                                                                                           ((x) + 0x000000d4)
#define HWIO_UART_DM_FSM_STATUS_RMSK                                                                                              0x37377771
#define HWIO_UART_DM_FSM_STATUS_IN(x)      \
        in_dword_masked(HWIO_UART_DM_FSM_STATUS_ADDR(x), HWIO_UART_DM_FSM_STATUS_RMSK)
#define HWIO_UART_DM_FSM_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_FSM_STATUS_ADDR(x), m)
#define HWIO_UART_DM_FSM_STATUS_TX_COMP_FSM_BMSK                                                                                  0x30000000
#define HWIO_UART_DM_FSM_STATUS_TX_COMP_FSM_SHFT                                                                                        0x1c
#define HWIO_UART_DM_FSM_STATUS_RX_PACK_FSM_BMSK                                                                                   0x7000000
#define HWIO_UART_DM_FSM_STATUS_RX_PACK_FSM_SHFT                                                                                        0x18
#define HWIO_UART_DM_FSM_STATUS_RX_TRANS_FSM_BMSK                                                                                   0x300000
#define HWIO_UART_DM_FSM_STATUS_RX_TRANS_FSM_SHFT                                                                                       0x14
#define HWIO_UART_DM_FSM_STATUS_TX_TRANS_FSM_BMSK                                                                                    0x70000
#define HWIO_UART_DM_FSM_STATUS_TX_TRANS_FSM_SHFT                                                                                       0x10
#define HWIO_UART_DM_FSM_STATUS_RX_PRO_TRANS_END_FSM_BMSK                                                                             0x7000
#define HWIO_UART_DM_FSM_STATUS_RX_PRO_TRANS_END_FSM_SHFT                                                                                0xc
#define HWIO_UART_DM_FSM_STATUS_RX_PRO_ACTIVE_FSM_BMSK                                                                                 0x700
#define HWIO_UART_DM_FSM_STATUS_RX_PRO_ACTIVE_FSM_SHFT                                                                                   0x8
#define HWIO_UART_DM_FSM_STATUS_TX_CON_TRANS_END_FSM_BMSK                                                                               0x70
#define HWIO_UART_DM_FSM_STATUS_TX_CON_TRANS_END_FSM_SHFT                                                                                0x4
#define HWIO_UART_DM_FSM_STATUS_RX_TRANSFER_ACTIVE_BMSK                                                                                  0x1
#define HWIO_UART_DM_FSM_STATUS_RX_TRANSFER_ACTIVE_SHFT                                                                                  0x0

#define HWIO_UART_DM_HW_VERSION_ADDR(x)                                                                                           ((x) + 0x000000d8)
#define HWIO_UART_DM_HW_VERSION_RMSK                                                                                              0xffffffff
#define HWIO_UART_DM_HW_VERSION_IN(x)      \
        in_dword_masked(HWIO_UART_DM_HW_VERSION_ADDR(x), HWIO_UART_DM_HW_VERSION_RMSK)
#define HWIO_UART_DM_HW_VERSION_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_HW_VERSION_ADDR(x), m)
#define HWIO_UART_DM_HW_VERSION_HW_VERSION_MAJOR_BMSK                                                                             0xf0000000
#define HWIO_UART_DM_HW_VERSION_HW_VERSION_MAJOR_SHFT                                                                                   0x1c
#define HWIO_UART_DM_HW_VERSION_HW_VERSION_MINOR_BMSK                                                                              0xfff0000
#define HWIO_UART_DM_HW_VERSION_HW_VERSION_MINOR_SHFT                                                                                   0x10
#define HWIO_UART_DM_HW_VERSION_HW_VERSION_STEP_BMSK                                                                                  0xffff
#define HWIO_UART_DM_HW_VERSION_HW_VERSION_STEP_SHFT                                                                                     0x0

#define HWIO_UART_DM_GENERICS_ADDR(x)                                                                                             ((x) + 0x000000dc)
#define HWIO_UART_DM_GENERICS_RMSK                                                                                                      0xff
#define HWIO_UART_DM_GENERICS_IN(x)      \
        in_dword_masked(HWIO_UART_DM_GENERICS_ADDR(x), HWIO_UART_DM_GENERICS_RMSK)
#define HWIO_UART_DM_GENERICS_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_GENERICS_ADDR(x), m)
#define HWIO_UART_DM_GENERICS_GENERIC_BAM_IFC_BMSK                                                                                      0x80
#define HWIO_UART_DM_GENERICS_GENERIC_BAM_IFC_SHFT                                                                                       0x7
#define HWIO_UART_DM_GENERICS_GENERIC_DM_IFC_BMSK                                                                                       0x40
#define HWIO_UART_DM_GENERICS_GENERIC_DM_IFC_SHFT                                                                                        0x6
#define HWIO_UART_DM_GENERICS_GENERIC_IRDA_IFC_BMSK                                                                                     0x20
#define HWIO_UART_DM_GENERICS_GENERIC_IRDA_IFC_SHFT                                                                                      0x5
#define HWIO_UART_DM_GENERICS_GENERIC_SIM_GLUE_BMSK                                                                                     0x10
#define HWIO_UART_DM_GENERICS_GENERIC_SIM_GLUE_SHFT                                                                                      0x4
#define HWIO_UART_DM_GENERICS_GENERIC_RAM_ADDR_WIDTH_BMSK                                                                                0xf
#define HWIO_UART_DM_GENERICS_GENERIC_RAM_ADDR_WIDTH_SHFT                                                                                0x0

#define HWIO_UART_DM_TF_ADDR(x)                                                                                                   ((x) + 0x00000100)
#define HWIO_UART_DM_TF_RMSK                                                                                                      0xffffffff
#define HWIO_UART_DM_TF_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_ADDR(x),v)
#define HWIO_UART_DM_TF_UART_TF_BMSK                                                                                              0xffffffff
#define HWIO_UART_DM_TF_UART_TF_SHFT                                                                                                     0x0

#define HWIO_UART_DM_TF_2_ADDR(x)                                                                                                 ((x) + 0x00000104)
#define HWIO_UART_DM_TF_2_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_TF_2_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_2_ADDR(x),v)
#define HWIO_UART_DM_TF_2_UART_TF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_TF_2_UART_TF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_TF_3_ADDR(x)                                                                                                 ((x) + 0x00000108)
#define HWIO_UART_DM_TF_3_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_TF_3_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_3_ADDR(x),v)
#define HWIO_UART_DM_TF_3_UART_TF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_TF_3_UART_TF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_TF_4_ADDR(x)                                                                                                 ((x) + 0x0000010c)
#define HWIO_UART_DM_TF_4_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_TF_4_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_4_ADDR(x),v)
#define HWIO_UART_DM_TF_4_UART_TF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_TF_4_UART_TF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_TF_5_ADDR(x)                                                                                                 ((x) + 0x00000110)
#define HWIO_UART_DM_TF_5_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_TF_5_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_5_ADDR(x),v)
#define HWIO_UART_DM_TF_5_UART_TF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_TF_5_UART_TF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_TF_6_ADDR(x)                                                                                                 ((x) + 0x00000114)
#define HWIO_UART_DM_TF_6_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_TF_6_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_6_ADDR(x),v)
#define HWIO_UART_DM_TF_6_UART_TF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_TF_6_UART_TF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_TF_7_ADDR(x)                                                                                                 ((x) + 0x00000118)
#define HWIO_UART_DM_TF_7_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_TF_7_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_7_ADDR(x),v)
#define HWIO_UART_DM_TF_7_UART_TF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_TF_7_UART_TF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_TF_8_ADDR(x)                                                                                                 ((x) + 0x0000011c)
#define HWIO_UART_DM_TF_8_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_TF_8_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_8_ADDR(x),v)
#define HWIO_UART_DM_TF_8_UART_TF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_TF_8_UART_TF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_TF_9_ADDR(x)                                                                                                 ((x) + 0x00000120)
#define HWIO_UART_DM_TF_9_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_TF_9_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_9_ADDR(x),v)
#define HWIO_UART_DM_TF_9_UART_TF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_TF_9_UART_TF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_TF_10_ADDR(x)                                                                                                ((x) + 0x00000124)
#define HWIO_UART_DM_TF_10_RMSK                                                                                                   0xffffffff
#define HWIO_UART_DM_TF_10_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_10_ADDR(x),v)
#define HWIO_UART_DM_TF_10_UART_TF_BMSK                                                                                           0xffffffff
#define HWIO_UART_DM_TF_10_UART_TF_SHFT                                                                                                  0x0

#define HWIO_UART_DM_TF_11_ADDR(x)                                                                                                ((x) + 0x00000128)
#define HWIO_UART_DM_TF_11_RMSK                                                                                                   0xffffffff
#define HWIO_UART_DM_TF_11_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_11_ADDR(x),v)
#define HWIO_UART_DM_TF_11_UART_TF_BMSK                                                                                           0xffffffff
#define HWIO_UART_DM_TF_11_UART_TF_SHFT                                                                                                  0x0

#define HWIO_UART_DM_TF_12_ADDR(x)                                                                                                ((x) + 0x0000012c)
#define HWIO_UART_DM_TF_12_RMSK                                                                                                   0xffffffff
#define HWIO_UART_DM_TF_12_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_12_ADDR(x),v)
#define HWIO_UART_DM_TF_12_UART_TF_BMSK                                                                                           0xffffffff
#define HWIO_UART_DM_TF_12_UART_TF_SHFT                                                                                                  0x0

#define HWIO_UART_DM_TF_13_ADDR(x)                                                                                                ((x) + 0x00000130)
#define HWIO_UART_DM_TF_13_RMSK                                                                                                   0xffffffff
#define HWIO_UART_DM_TF_13_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_13_ADDR(x),v)
#define HWIO_UART_DM_TF_13_UART_TF_BMSK                                                                                           0xffffffff
#define HWIO_UART_DM_TF_13_UART_TF_SHFT                                                                                                  0x0

#define HWIO_UART_DM_TF_14_ADDR(x)                                                                                                ((x) + 0x00000134)
#define HWIO_UART_DM_TF_14_RMSK                                                                                                   0xffffffff
#define HWIO_UART_DM_TF_14_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_14_ADDR(x),v)
#define HWIO_UART_DM_TF_14_UART_TF_BMSK                                                                                           0xffffffff
#define HWIO_UART_DM_TF_14_UART_TF_SHFT                                                                                                  0x0

#define HWIO_UART_DM_TF_15_ADDR(x)                                                                                                ((x) + 0x00000138)
#define HWIO_UART_DM_TF_15_RMSK                                                                                                   0xffffffff
#define HWIO_UART_DM_TF_15_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_15_ADDR(x),v)
#define HWIO_UART_DM_TF_15_UART_TF_BMSK                                                                                           0xffffffff
#define HWIO_UART_DM_TF_15_UART_TF_SHFT                                                                                                  0x0

#define HWIO_UART_DM_TF_16_ADDR(x)                                                                                                ((x) + 0x0000013c)
#define HWIO_UART_DM_TF_16_RMSK                                                                                                   0xffffffff
#define HWIO_UART_DM_TF_16_OUT(x, v)      \
        out_dword(HWIO_UART_DM_TF_16_ADDR(x),v)
#define HWIO_UART_DM_TF_16_UART_TF_BMSK                                                                                           0xffffffff
#define HWIO_UART_DM_TF_16_UART_TF_SHFT                                                                                                  0x0

#define HWIO_UART_DM_RF_ADDR(x)                                                                                                   ((x) + 0x00000140)
#define HWIO_UART_DM_RF_RMSK                                                                                                      0xffffffff
#define HWIO_UART_DM_RF_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_ADDR(x), HWIO_UART_DM_RF_RMSK)
#define HWIO_UART_DM_RF_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_ADDR(x), m)
#define HWIO_UART_DM_RF_UART_RF_BMSK                                                                                              0xffffffff
#define HWIO_UART_DM_RF_UART_RF_SHFT                                                                                                     0x0

#define HWIO_UART_DM_RF_2_ADDR(x)                                                                                                 ((x) + 0x00000144)
#define HWIO_UART_DM_RF_2_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_RF_2_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_2_ADDR(x), HWIO_UART_DM_RF_2_RMSK)
#define HWIO_UART_DM_RF_2_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_2_ADDR(x), m)
#define HWIO_UART_DM_RF_2_UART_RF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_RF_2_UART_RF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_RF_3_ADDR(x)                                                                                                 ((x) + 0x00000148)
#define HWIO_UART_DM_RF_3_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_RF_3_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_3_ADDR(x), HWIO_UART_DM_RF_3_RMSK)
#define HWIO_UART_DM_RF_3_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_3_ADDR(x), m)
#define HWIO_UART_DM_RF_3_UART_RF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_RF_3_UART_RF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_RF_4_ADDR(x)                                                                                                 ((x) + 0x0000014c)
#define HWIO_UART_DM_RF_4_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_RF_4_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_4_ADDR(x), HWIO_UART_DM_RF_4_RMSK)
#define HWIO_UART_DM_RF_4_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_4_ADDR(x), m)
#define HWIO_UART_DM_RF_4_UART_RF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_RF_4_UART_RF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_RF_5_ADDR(x)                                                                                                 ((x) + 0x00000150)
#define HWIO_UART_DM_RF_5_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_RF_5_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_5_ADDR(x), HWIO_UART_DM_RF_5_RMSK)
#define HWIO_UART_DM_RF_5_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_5_ADDR(x), m)
#define HWIO_UART_DM_RF_5_UART_RF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_RF_5_UART_RF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_RF_6_ADDR(x)                                                                                                 ((x) + 0x00000154)
#define HWIO_UART_DM_RF_6_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_RF_6_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_6_ADDR(x), HWIO_UART_DM_RF_6_RMSK)
#define HWIO_UART_DM_RF_6_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_6_ADDR(x), m)
#define HWIO_UART_DM_RF_6_UART_RF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_RF_6_UART_RF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_RF_7_ADDR(x)                                                                                                 ((x) + 0x00000158)
#define HWIO_UART_DM_RF_7_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_RF_7_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_7_ADDR(x), HWIO_UART_DM_RF_7_RMSK)
#define HWIO_UART_DM_RF_7_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_7_ADDR(x), m)
#define HWIO_UART_DM_RF_7_UART_RF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_RF_7_UART_RF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_RF_8_ADDR(x)                                                                                                 ((x) + 0x0000015c)
#define HWIO_UART_DM_RF_8_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_RF_8_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_8_ADDR(x), HWIO_UART_DM_RF_8_RMSK)
#define HWIO_UART_DM_RF_8_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_8_ADDR(x), m)
#define HWIO_UART_DM_RF_8_UART_RF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_RF_8_UART_RF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_RF_9_ADDR(x)                                                                                                 ((x) + 0x00000160)
#define HWIO_UART_DM_RF_9_RMSK                                                                                                    0xffffffff
#define HWIO_UART_DM_RF_9_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_9_ADDR(x), HWIO_UART_DM_RF_9_RMSK)
#define HWIO_UART_DM_RF_9_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_9_ADDR(x), m)
#define HWIO_UART_DM_RF_9_UART_RF_BMSK                                                                                            0xffffffff
#define HWIO_UART_DM_RF_9_UART_RF_SHFT                                                                                                   0x0

#define HWIO_UART_DM_RF_10_ADDR(x)                                                                                                ((x) + 0x00000164)
#define HWIO_UART_DM_RF_10_RMSK                                                                                                   0xffffffff
#define HWIO_UART_DM_RF_10_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_10_ADDR(x), HWIO_UART_DM_RF_10_RMSK)
#define HWIO_UART_DM_RF_10_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_10_ADDR(x), m)
#define HWIO_UART_DM_RF_10_UART_RF_BMSK                                                                                           0xffffffff
#define HWIO_UART_DM_RF_10_UART_RF_SHFT                                                                                                  0x0

#define HWIO_UART_DM_RF_11_ADDR(x)                                                                                                ((x) + 0x00000168)
#define HWIO_UART_DM_RF_11_RMSK                                                                                                   0xffffffff
#define HWIO_UART_DM_RF_11_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_11_ADDR(x), HWIO_UART_DM_RF_11_RMSK)
#define HWIO_UART_DM_RF_11_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_11_ADDR(x), m)
#define HWIO_UART_DM_RF_11_UART_RF_BMSK                                                                                           0xffffffff
#define HWIO_UART_DM_RF_11_UART_RF_SHFT                                                                                                  0x0

#define HWIO_UART_DM_RF_12_ADDR(x)                                                                                                ((x) + 0x0000016c)
#define HWIO_UART_DM_RF_12_RMSK                                                                                                   0xffffffff
#define HWIO_UART_DM_RF_12_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_12_ADDR(x), HWIO_UART_DM_RF_12_RMSK)
#define HWIO_UART_DM_RF_12_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_12_ADDR(x), m)
#define HWIO_UART_DM_RF_12_UART_RF_BMSK                                                                                           0xffffffff
#define HWIO_UART_DM_RF_12_UART_RF_SHFT                                                                                                  0x0

#define HWIO_UART_DM_RF_13_ADDR(x)                                                                                                ((x) + 0x00000170)
#define HWIO_UART_DM_RF_13_RMSK                                                                                                   0xffffffff
#define HWIO_UART_DM_RF_13_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_13_ADDR(x), HWIO_UART_DM_RF_13_RMSK)
#define HWIO_UART_DM_RF_13_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_13_ADDR(x), m)
#define HWIO_UART_DM_RF_13_UART_RF_BMSK                                                                                           0xffffffff
#define HWIO_UART_DM_RF_13_UART_RF_SHFT                                                                                                  0x0

#define HWIO_UART_DM_RF_14_ADDR(x)                                                                                                ((x) + 0x00000174)
#define HWIO_UART_DM_RF_14_RMSK                                                                                                   0xffffffff
#define HWIO_UART_DM_RF_14_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_14_ADDR(x), HWIO_UART_DM_RF_14_RMSK)
#define HWIO_UART_DM_RF_14_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_14_ADDR(x), m)
#define HWIO_UART_DM_RF_14_UART_RF_BMSK                                                                                           0xffffffff
#define HWIO_UART_DM_RF_14_UART_RF_SHFT                                                                                                  0x0

#define HWIO_UART_DM_RF_15_ADDR(x)                                                                                                ((x) + 0x00000178)
#define HWIO_UART_DM_RF_15_RMSK                                                                                                   0xffffffff
#define HWIO_UART_DM_RF_15_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_15_ADDR(x), HWIO_UART_DM_RF_15_RMSK)
#define HWIO_UART_DM_RF_15_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_15_ADDR(x), m)
#define HWIO_UART_DM_RF_15_UART_RF_BMSK                                                                                           0xffffffff
#define HWIO_UART_DM_RF_15_UART_RF_SHFT                                                                                                  0x0

#define HWIO_UART_DM_RF_16_ADDR(x)                                                                                                ((x) + 0x0000017c)
#define HWIO_UART_DM_RF_16_RMSK                                                                                                   0xffffffff
#define HWIO_UART_DM_RF_16_IN(x)      \
        in_dword_masked(HWIO_UART_DM_RF_16_ADDR(x), HWIO_UART_DM_RF_16_RMSK)
#define HWIO_UART_DM_RF_16_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_RF_16_ADDR(x), m)
#define HWIO_UART_DM_RF_16_UART_RF_BMSK                                                                                           0xffffffff
#define HWIO_UART_DM_RF_16_UART_RF_SHFT                                                                                                  0x0

#define HWIO_UART_DM_UIM_CFG_ADDR(x)                                                                                              ((x) + 0x00000180)
#define HWIO_UART_DM_UIM_CFG_RMSK                                                                                                     0xf77f
#define HWIO_UART_DM_UIM_CFG_IN(x)      \
        in_dword_masked(HWIO_UART_DM_UIM_CFG_ADDR(x), HWIO_UART_DM_UIM_CFG_RMSK)
#define HWIO_UART_DM_UIM_CFG_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_UIM_CFG_ADDR(x), m)
#define HWIO_UART_DM_UIM_CFG_OUT(x, v)      \
        out_dword(HWIO_UART_DM_UIM_CFG_ADDR(x),v)
#define HWIO_UART_DM_UIM_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_UIM_CFG_ADDR(x),m,v,HWIO_UART_DM_UIM_CFG_IN(x))
#define HWIO_UART_DM_UIM_CFG_BATT_ALARM_QUICK_DROP_EN_BMSK                                                                            0x8000
#define HWIO_UART_DM_UIM_CFG_BATT_ALARM_QUICK_DROP_EN_SHFT                                                                               0xf
#define HWIO_UART_DM_UIM_CFG_TESTBUS_EN_BMSK                                                                                          0x4000
#define HWIO_UART_DM_UIM_CFG_TESTBUS_EN_SHFT                                                                                             0xe
#define HWIO_UART_DM_UIM_CFG_SW_RESET_BMSK                                                                                            0x2000
#define HWIO_UART_DM_UIM_CFG_SW_RESET_SHFT                                                                                               0xd
#define HWIO_UART_DM_UIM_CFG_MODE18_BMSK                                                                                              0x1000
#define HWIO_UART_DM_UIM_CFG_MODE18_SHFT                                                                                                 0xc
#define HWIO_UART_DM_UIM_CFG_PMIC_ALARM_EN_BMSK                                                                                        0x400
#define HWIO_UART_DM_UIM_CFG_PMIC_ALARM_EN_SHFT                                                                                          0xa
#define HWIO_UART_DM_UIM_CFG_BATT_ALARM_TRIGGER_EN_BMSK                                                                                0x200
#define HWIO_UART_DM_UIM_CFG_BATT_ALARM_TRIGGER_EN_SHFT                                                                                  0x9
#define HWIO_UART_DM_UIM_CFG_UIM_RMV_TRIGGER_EN_BMSK                                                                                   0x100
#define HWIO_UART_DM_UIM_CFG_UIM_RMV_TRIGGER_EN_SHFT                                                                                     0x8
#define HWIO_UART_DM_UIM_CFG_UIM_CARD_EVENTS_ENABLE_BMSK                                                                                0x40
#define HWIO_UART_DM_UIM_CFG_UIM_CARD_EVENTS_ENABLE_SHFT                                                                                 0x6
#define HWIO_UART_DM_UIM_CFG_UIM_PRESENT_POLARITY_BMSK                                                                                  0x20
#define HWIO_UART_DM_UIM_CFG_UIM_PRESENT_POLARITY_SHFT                                                                                   0x5
#define HWIO_UART_DM_UIM_CFG_EVENT_DEBOUNCE_TIME_BMSK                                                                                   0x1f
#define HWIO_UART_DM_UIM_CFG_EVENT_DEBOUNCE_TIME_SHFT                                                                                    0x0

#define HWIO_UART_DM_UIM_CMD_ADDR(x)                                                                                              ((x) + 0x00000184)
#define HWIO_UART_DM_UIM_CMD_RMSK                                                                                                        0x3
#define HWIO_UART_DM_UIM_CMD_OUT(x, v)      \
        out_dword(HWIO_UART_DM_UIM_CMD_ADDR(x),v)
#define HWIO_UART_DM_UIM_CMD_RECOVER_FROM_HW_DEACTIVATION_BMSK                                                                           0x2
#define HWIO_UART_DM_UIM_CMD_RECOVER_FROM_HW_DEACTIVATION_SHFT                                                                           0x1
#define HWIO_UART_DM_UIM_CMD_INITIATE_HW_DEACTIVATION_BMSK                                                                               0x1
#define HWIO_UART_DM_UIM_CMD_INITIATE_HW_DEACTIVATION_SHFT                                                                               0x0

#define HWIO_UART_DM_UIM_IO_STATUS_ADDR(x)                                                                                        ((x) + 0x00000188)
#define HWIO_UART_DM_UIM_IO_STATUS_RMSK                                                                                                  0x7
#define HWIO_UART_DM_UIM_IO_STATUS_IN(x)      \
        in_dword_masked(HWIO_UART_DM_UIM_IO_STATUS_ADDR(x), HWIO_UART_DM_UIM_IO_STATUS_RMSK)
#define HWIO_UART_DM_UIM_IO_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_UIM_IO_STATUS_ADDR(x), m)
#define HWIO_UART_DM_UIM_IO_STATUS_UIM_IO_WRITE_IN_PROGRESS_BMSK                                                                         0x4
#define HWIO_UART_DM_UIM_IO_STATUS_UIM_IO_WRITE_IN_PROGRESS_SHFT                                                                         0x2
#define HWIO_UART_DM_UIM_IO_STATUS_UIM_DEACTIVATION_STATUS_BMSK                                                                          0x2
#define HWIO_UART_DM_UIM_IO_STATUS_UIM_DEACTIVATION_STATUS_SHFT                                                                          0x1
#define HWIO_UART_DM_UIM_IO_STATUS_CARD_PRESENCE_BMSK                                                                                    0x1
#define HWIO_UART_DM_UIM_IO_STATUS_CARD_PRESENCE_SHFT                                                                                    0x0

#define HWIO_UART_DM_UIM_IRQ_ISR_ADDR(x)                                                                                          ((x) + 0x0000018c)
#define HWIO_UART_DM_UIM_IRQ_ISR_RMSK                                                                                                   0x1f
#define HWIO_UART_DM_UIM_IRQ_ISR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_UIM_IRQ_ISR_ADDR(x), HWIO_UART_DM_UIM_IRQ_ISR_RMSK)
#define HWIO_UART_DM_UIM_IRQ_ISR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_UIM_IRQ_ISR_ADDR(x), m)
#define HWIO_UART_DM_UIM_IRQ_ISR_UIM_IO_WRITE_DONE_BMSK                                                                                 0x10
#define HWIO_UART_DM_UIM_IRQ_ISR_UIM_IO_WRITE_DONE_SHFT                                                                                  0x4
#define HWIO_UART_DM_UIM_IRQ_ISR_HW_SEQUENCE_FINISH_BMSK                                                                                 0x8
#define HWIO_UART_DM_UIM_IRQ_ISR_HW_SEQUENCE_FINISH_SHFT                                                                                 0x3
#define HWIO_UART_DM_UIM_IRQ_ISR_BATT_ALARM_BMSK                                                                                         0x4
#define HWIO_UART_DM_UIM_IRQ_ISR_BATT_ALARM_SHFT                                                                                         0x2
#define HWIO_UART_DM_UIM_IRQ_ISR_UIM_CARD_INSERTION_BMSK                                                                                 0x2
#define HWIO_UART_DM_UIM_IRQ_ISR_UIM_CARD_INSERTION_SHFT                                                                                 0x1
#define HWIO_UART_DM_UIM_IRQ_ISR_UIM_CARD_REMOVAL_BMSK                                                                                   0x1
#define HWIO_UART_DM_UIM_IRQ_ISR_UIM_CARD_REMOVAL_SHFT                                                                                   0x0

#define HWIO_UART_DM_UIM_IRQ_MISR_ADDR(x)                                                                                         ((x) + 0x00000190)
#define HWIO_UART_DM_UIM_IRQ_MISR_RMSK                                                                                                  0x1f
#define HWIO_UART_DM_UIM_IRQ_MISR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_UIM_IRQ_MISR_ADDR(x), HWIO_UART_DM_UIM_IRQ_MISR_RMSK)
#define HWIO_UART_DM_UIM_IRQ_MISR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_UIM_IRQ_MISR_ADDR(x), m)
#define HWIO_UART_DM_UIM_IRQ_MISR_UIM_IO_WRITE_DONE_BMSK                                                                                0x10
#define HWIO_UART_DM_UIM_IRQ_MISR_UIM_IO_WRITE_DONE_SHFT                                                                                 0x4
#define HWIO_UART_DM_UIM_IRQ_MISR_HW_SEQUENCE_FINISH_BMSK                                                                                0x8
#define HWIO_UART_DM_UIM_IRQ_MISR_HW_SEQUENCE_FINISH_SHFT                                                                                0x3
#define HWIO_UART_DM_UIM_IRQ_MISR_BATT_ALARM_BMSK                                                                                        0x4
#define HWIO_UART_DM_UIM_IRQ_MISR_BATT_ALARM_SHFT                                                                                        0x2
#define HWIO_UART_DM_UIM_IRQ_MISR_UIM_CARD_INSERTION_BMSK                                                                                0x2
#define HWIO_UART_DM_UIM_IRQ_MISR_UIM_CARD_INSERTION_SHFT                                                                                0x1
#define HWIO_UART_DM_UIM_IRQ_MISR_UIM_CARD_REMOVAL_BMSK                                                                                  0x1
#define HWIO_UART_DM_UIM_IRQ_MISR_UIM_CARD_REMOVAL_SHFT                                                                                  0x0

#define HWIO_UART_DM_UIM_IRQ_CLR_ADDR(x)                                                                                          ((x) + 0x00000194)
#define HWIO_UART_DM_UIM_IRQ_CLR_RMSK                                                                                                   0x1f
#define HWIO_UART_DM_UIM_IRQ_CLR_OUT(x, v)      \
        out_dword(HWIO_UART_DM_UIM_IRQ_CLR_ADDR(x),v)
#define HWIO_UART_DM_UIM_IRQ_CLR_UIM_IO_WRITE_DONE_BMSK                                                                                 0x10
#define HWIO_UART_DM_UIM_IRQ_CLR_UIM_IO_WRITE_DONE_SHFT                                                                                  0x4
#define HWIO_UART_DM_UIM_IRQ_CLR_HW_SEQUENCE_FINISH_BMSK                                                                                 0x8
#define HWIO_UART_DM_UIM_IRQ_CLR_HW_SEQUENCE_FINISH_SHFT                                                                                 0x3
#define HWIO_UART_DM_UIM_IRQ_CLR_BATT_ALARM_BMSK                                                                                         0x4
#define HWIO_UART_DM_UIM_IRQ_CLR_BATT_ALARM_SHFT                                                                                         0x2
#define HWIO_UART_DM_UIM_IRQ_CLR_UIM_CARD_INSERTION_BMSK                                                                                 0x2
#define HWIO_UART_DM_UIM_IRQ_CLR_UIM_CARD_INSERTION_SHFT                                                                                 0x1
#define HWIO_UART_DM_UIM_IRQ_CLR_UIM_CARD_REMOVAL_BMSK                                                                                   0x1
#define HWIO_UART_DM_UIM_IRQ_CLR_UIM_CARD_REMOVAL_SHFT                                                                                   0x0

#define HWIO_UART_DM_UIM_IRQ_IMR_ADDR(x)                                                                                          ((x) + 0x00000198)
#define HWIO_UART_DM_UIM_IRQ_IMR_RMSK                                                                                                   0x1f
#define HWIO_UART_DM_UIM_IRQ_IMR_IN(x)      \
        in_dword_masked(HWIO_UART_DM_UIM_IRQ_IMR_ADDR(x), HWIO_UART_DM_UIM_IRQ_IMR_RMSK)
#define HWIO_UART_DM_UIM_IRQ_IMR_INM(x, m)      \
        in_dword_masked(HWIO_UART_DM_UIM_IRQ_IMR_ADDR(x), m)
#define HWIO_UART_DM_UIM_IRQ_IMR_OUT(x, v)      \
        out_dword(HWIO_UART_DM_UIM_IRQ_IMR_ADDR(x),v)
#define HWIO_UART_DM_UIM_IRQ_IMR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_UART_DM_UIM_IRQ_IMR_ADDR(x),m,v,HWIO_UART_DM_UIM_IRQ_IMR_IN(x))
#define HWIO_UART_DM_UIM_IRQ_IMR_UIM_IO_WRITE_DONE_BMSK                                                                                 0x10
#define HWIO_UART_DM_UIM_IRQ_IMR_UIM_IO_WRITE_DONE_SHFT                                                                                  0x4
#define HWIO_UART_DM_UIM_IRQ_IMR_HW_SEQUENCE_FINISH_BMSK                                                                                 0x8
#define HWIO_UART_DM_UIM_IRQ_IMR_HW_SEQUENCE_FINISH_SHFT                                                                                 0x3
#define HWIO_UART_DM_UIM_IRQ_IMR_BATT_ALARM_BMSK                                                                                         0x4
#define HWIO_UART_DM_UIM_IRQ_IMR_BATT_ALARM_SHFT                                                                                         0x2
#define HWIO_UART_DM_UIM_IRQ_IMR_UIM_CARD_INSERTION_BMSK                                                                                 0x2
#define HWIO_UART_DM_UIM_IRQ_IMR_UIM_CARD_INSERTION_SHFT                                                                                 0x1
#define HWIO_UART_DM_UIM_IRQ_IMR_UIM_CARD_REMOVAL_BMSK                                                                                   0x1
#define HWIO_UART_DM_UIM_IRQ_IMR_UIM_CARD_REMOVAL_SHFT                                                                                   0x0

#define HWIO_UART_DM_UIM_IRQ_IMR_SET_ADDR(x)                                                                                      ((x) + 0x0000019c)
#define HWIO_UART_DM_UIM_IRQ_IMR_SET_RMSK                                                                                               0x1f
#define HWIO_UART_DM_UIM_IRQ_IMR_SET_OUT(x, v)      \
        out_dword(HWIO_UART_DM_UIM_IRQ_IMR_SET_ADDR(x),v)
#define HWIO_UART_DM_UIM_IRQ_IMR_SET_UIM_IO_WRITE_DONE_BMSK                                                                             0x10
#define HWIO_UART_DM_UIM_IRQ_IMR_SET_UIM_IO_WRITE_DONE_SHFT                                                                              0x4
#define HWIO_UART_DM_UIM_IRQ_IMR_SET_HW_SEQUENCE_FINISH_BMSK                                                                             0x8
#define HWIO_UART_DM_UIM_IRQ_IMR_SET_HW_SEQUENCE_FINISH_SHFT                                                                             0x3
#define HWIO_UART_DM_UIM_IRQ_IMR_SET_BATT_ALARM_BMSK                                                                                     0x4
#define HWIO_UART_DM_UIM_IRQ_IMR_SET_BATT_ALARM_SHFT                                                                                     0x2
#define HWIO_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_INSERTION_BMSK                                                                             0x2
#define HWIO_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_INSERTION_SHFT                                                                             0x1
#define HWIO_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_REMOVAL_BMSK                                                                               0x1
#define HWIO_UART_DM_UIM_IRQ_IMR_SET_UIM_CARD_REMOVAL_SHFT                                                                               0x0

#define HWIO_UART_DM_UIM_IRQ_IMR_CLR_ADDR(x)                                                                                      ((x) + 0x000001a0)
#define HWIO_UART_DM_UIM_IRQ_IMR_CLR_RMSK                                                                                               0x1f
#define HWIO_UART_DM_UIM_IRQ_IMR_CLR_OUT(x, v)      \
        out_dword(HWIO_UART_DM_UIM_IRQ_IMR_CLR_ADDR(x),v)
#define HWIO_UART_DM_UIM_IRQ_IMR_CLR_UIM_IO_WRITE_DONE_BMSK                                                                             0x10
#define HWIO_UART_DM_UIM_IRQ_IMR_CLR_UIM_IO_WRITE_DONE_SHFT                                                                              0x4
#define HWIO_UART_DM_UIM_IRQ_IMR_CLR_HW_SEQUENCE_FINISH_BMSK                                                                             0x8
#define HWIO_UART_DM_UIM_IRQ_IMR_CLR_HW_SEQUENCE_FINISH_SHFT                                                                             0x3
#define HWIO_UART_DM_UIM_IRQ_IMR_CLR_BATT_ALARM_BMSK                                                                                     0x4
#define HWIO_UART_DM_UIM_IRQ_IMR_CLR_BATT_ALARM_SHFT                                                                                     0x2
#define HWIO_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_INSERTION_BMSK                                                                             0x2
#define HWIO_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_INSERTION_SHFT                                                                             0x1
#define HWIO_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_REMOVAL_BMSK                                                                               0x1
#define HWIO_UART_DM_UIM_IRQ_IMR_CLR_UIM_CARD_REMOVAL_SHFT                                                                               0x0


#endif /* __UART_HWIO_H__ */
