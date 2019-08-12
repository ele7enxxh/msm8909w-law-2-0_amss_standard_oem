/*!
  @file
  hw_mem_pool_buf.h

  @brief
  Include file contains enums for mem pool virtual buffers

  @detail
  description of functions in this file
 
*/

/*===========================================================================

  Copyright (c) 2007 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/hw_mem_pool_buf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef HW_MEM_POOL_BUF_H
#define HW_MEM_POOL_BUF_H


/*===========================================================================

      Constants

===========================================================================*/

/*===========================================================================

      Macros

===========================================================================*/

/*===========================================================================

      Typedefs

===========================================================================*/


/*! @brief Enum of LTE memory pool virtual buffers.  This is used to 
           index hw_mem_pool_buf array in hw_mem_pool.c  This is also
           used for HW memory read/write command for testing.
*/
typedef enum
{
  HW_MEM_POOL_BUF_MIN_INDEX,
  // --------------------------------------------------------------------------
  // Enumeration related to IRAT buffers
  // --------------------------------------------------------------------------
  HW_MEM_POOL_BUF_IRAT1 = HW_MEM_POOL_BUF_MIN_INDEX, ///<  GSM, and DO
  HW_MEM_POOL_BUF_IRAT2,                  ///< SVLTE
  HW_MEM_POOL_BUF_IRAT3,                  ///< IRAT_WCDMA_CHAIN0, TDSCDMA
                                          ///< LTE related buffers During the Gap (x2L, L2L, etc.)
  HW_MEM_POOL_BUF_IRAT4,                  ///< IRAT_SRCH_SS
  HW_MEM_POOL_BUF_SEARCH_TD_SEC = HW_MEM_POOL_BUF_IRAT4, ///< Search TD buffer (rx ant 0 & 1) secondary
//  HW_MEM_POOL_BUF_IRAT5,                ///< This is really an overlay. During IRAT gap, these will be used for LTE
                                          ///< NB_NCELL_CIR_IN_LP_TX0/1,NB_NCELL_SNE, SE, NSRCH_NONCOH
  HW_MEM_POOL_BUF_SRCH_PEAK_VAL,
                                            ///< peak sort buffer for search PSS peak values from NONCOH buffer
  HW_MEM_POOL_BUF_SRCH_PEAK_POS,            ///< peak sort buffer for search PSS peak locations in NONCOH buffer
                                            ///< SRCH_NON_COH_FREQ_HYPO0 and FSCAN_INBAND_TONE_EE are overlays. Please do not change the order.
  HW_MEM_POOL_BUF_SRCH_NONCOH_FREQ_HYPO0,   ///< Search non-coherent buffer for frequency Hypo0
  HW_MEM_POOL_BUF_CQI,                      ///< CQI buffer
  
  HW_MEM_POOL_BUF_DEMOD_TD,                 ///< Demod TD buffer  (rx ant 0 & 1)
  HW_MEM_POOL_BUF_SEARCH_TD,                ///< Search TD buffer (rx ant 0 & 1)
  HW_MEM_POOL_BUF_BB_FILT_CIR,              ///< broadband filtered CIR (rx ant 0 & 1)
  HW_MEM_POOL_BUF_BB_SE,                    ///< broadband signal energy (rx ant 0 & 1)
  HW_MEM_POOL_BUF_BB_SNE,                   ///< broadband signal + noise energy (rx ant 0 & 1)
  HW_MEM_POOL_BUF_NB_NCELL_SNE,             ///< narrowband signal + noise energy ncell meas+TTL/FTL (rx ant 0 & 1)
  HW_MEM_POOL_BUF_NB_NCELL_SE,              ///< narrowband signal energy ncell meas+TTL/FTL (rx ant 0 & 1)
  HW_MEM_POOL_BUF_NB_SCELL_TRACK_OUTER_SNE, ///< narrowband SNE for scell tracking (rx ant 0 & 1)
  HW_MEM_POOL_BUF_NB_NCELL_TRACK_OUTER_SNE, ///< narrowband SNE for ncell tracking (rx ant 0 & 1)
  HW_MEM_POOL_BUF_NB_SCELL_TRACK_OUTER_SE,  ///< narrowband SNE for scell tracking (rx ant 0 & 1)
  HW_MEM_POOL_BUF_NB_NCELL_TRACK_OUTER_SE,  ///< narrowband SNE for ncell tracking (rx ant 0 & 1)

  HW_MEM_POOL_BUF_BB_CIR_TX0,               ///< broadband CIR (rx ant 0 & 1, eNb Tx ant0)
  HW_MEM_POOL_BUF_BB_CIR_TX1,               ///< broadband CIR (rx ant 0 & 1, eNb Tx ant1)
  HW_MEM_POOL_BUF_BB_CIR_TX2,               ///< broadband CIR (rx ant 0 & 1, eNb Tx ant2)
  HW_MEM_POOL_BUF_BB_CIR_TX3,               ///< broadband CIR (rx ant 0 & 1, eNb Tx ant3)
  HW_MEM_POOL_BUF_BB_CIR_DOPPLER_TX0,       ///< broadband CIR for Doppler (rx ant 0 & 1, eNb Tx ant0)
  HW_MEM_POOL_BUF_BB_CIR_DOPPLER_TX1,       ///< broadband CIR for Doppler (rx ant 0 & 1, eNb Tx ant1)
  HW_MEM_POOL_BUF_NB_SCELL_CIR_TX0,         ///< narrowband scell CIR (rx ant 0 & 1, eNb Tx ant0)
  HW_MEM_POOL_BUF_NB_SCELL_CIR_TX1,         ///< narrowband scell CIR (rx ant 0 & 1, eNb Tx ant1)

  HW_MEM_POOL_BUF_NB_PBCH_CIR_TX02,         ///< narrowband RAW CIR (PBCH Channel Est) (rx ant 0 & 1, eNb Tx ant0 & 2)
  HW_MEM_POOL_BUF_NB_PBCH_CIR_TX13,         ///< narrowband RAW CIR (PBCH Channel Est) (rx ant 0 & 1, eNb Tx ant1 & 3)
  HW_MEM_POOL_BUF_NB_NCELL_CIR_OUT_LP_TX0,  ///< narrowband ncell CIR for outer loop (rx ant 0 & 1, eNb Tx ant0)
  HW_MEM_POOL_BUF_NB_NCELL_CIR_OUT_LP_TX1,  ///< narrowband ncell CIR for outer loop (rx ant 0 & 1, eNb Tx ant1)

  HW_MEM_POOL_BUF_NB_NCELL_CIR_IN_LP_TX0,   ///< narrowband ncell CIR for inner loop (rx ant 0 & 1, eNb Tx ant0)
  HW_MEM_POOL_BUF_NB_NCELL_CIR_IN_LP_TX1,   ///< narrowband ncell CIR for inner loop (rx ant 0 & 1, eNb Tx ant1)
  HW_MEM_POOL_BUF_UERS_CH_EST,              ///< UERS channel estimation parameters (rx ant 0 & 1)
  HW_MEM_POOL_BUF_UERS_DESCR,
    
  HW_MEM_POOL_BUF_BB_WHITENING,             ///< broadband whitening buffer
  HW_MEM_POOL_BUF_BB_WHITENING_HIGH_SNR,    ///< broadband whitening buffer for high SNR case
  HW_MEM_POOL_BUF_UERS_WHITENING,           ///< UERS whitening (& raw covariance) buffer
  HW_MEM_POOL_BUF_PBCH_WHITENING,           ///< PBCH whitening buffer
                                            ///< DEMOD_FD and NONCOH_HYPO1&2 are overlays. Do not change the order
  HW_MEM_POOL_BUF_DEMOD_FD,                 ///< Demod FD buffer (rx ant 0 & 1)
  HW_MEM_POOL_BUF_SRCH_NONCOH_FREQ_HYPO1,   ///< non-coh buffer for init acq, frequency hypothesis1
  HW_MEM_POOL_BUF_SRCH_NONCOH_FREQ_HYPO2,   ///< non-coh buffer for init acq, frequency hypothesis2

  HW_MEM_POOL_BUF_FFT_BAND_NRG,             ///< FFT band energy (rx ant 0 & 1)
  HW_MEM_POOL_BUF_PBCH_FD_DATA,             ///< PBCH FD buffer (rx ant 0 & 1)
  HW_MEM_POOL_BUF_PBCH_FD_TX02,             ///< PBCH FD buffer (rx ant 0 & 1)
  HW_MEM_POOL_BUF_PBCH_FD_TX13,             ///< PBCH FD buffer (rx ant 0 & 1)

  HW_MEM_POOL_BUF_BB_CH_EST_TX0_RX0,        ///< broadband channel est RAM (rx ant 0 & 1, eNb Tx ant0)
  HW_MEM_POOL_BUF_BB_CH_EST_TX0_RX1,

  HW_MEM_POOL_BUF_BB_CH_EST_TX1_RX0,        ///< broadband channel est RAM (rx ant 0 & 1, eNb Tx ant1)
  HW_MEM_POOL_BUF_BB_CH_EST_TX1_RX1,

  HW_MEM_POOL_BUF_PBCH_CH_EST_TX0_RX0,      ///< PBCH channel est RAM (rx ant 0 & 1, eNb Tx ant0)
  HW_MEM_POOL_BUF_PBCH_CH_EST_TX0_RX1,

  HW_MEM_POOL_BUF_PBCH_CH_EST_TX1_RX0,      ///< PBCH channel est RAM (rx ant 0 & 1, eNb Tx ant1)
  HW_MEM_POOL_BUF_PBCH_CH_EST_TX1_RX1,

  HW_MEM_POOL_BUF_BB_CH_EST_TX2_RX0,        ///< broadband channel est RAM (rx ant 0 & 1, eNb Tx ant2)
  HW_MEM_POOL_BUF_BB_CH_EST_TX2_RX1,

  HW_MEM_POOL_BUF_BB_CH_EST_TX3_RX0,        ///< broadband channel est RAM (rx ant 0 & 1, eNb Tx ant3)
  HW_MEM_POOL_BUF_BB_CH_EST_TX3_RX1,

  HW_MEM_POOL_BUF_PBCH_CH_EST_TX2_RX0,      ///< PBCH channel est RAM (rx ant 0 & 1, eNb Tx ant2)
  HW_MEM_POOL_BUF_PBCH_CH_EST_TX2_RX1,

  HW_MEM_POOL_BUF_PBCH_CH_EST_TX3_RX0,      ///< PBCH channel est RAM (rx ant 0 & 1, eNb Tx ant3)
  HW_MEM_POOL_BUF_PBCH_CH_EST_TX3_RX1,

  HW_MEM_POOL_BUF_PRS_ACCUM,
  HW_MEM_POOL_BUF_FSCAN8_INBAND_TONE_EE,    ///< Freq scan in band tone energy values
  HW_MEM_POOL_BUF_FSCAN9_INBAND_TONE_EE,    ///< Freq scan in band tone energy values

  HW_MEM_POOL_BUF_PRS_S_SNE,
  HW_MEM_POOL_BUF_SCELL_RS_CORR_TX0,        ///< RAM for RS_CORR values for FTL/AGC usage for TX0
  HW_MEM_POOL_BUF_SCELL_RS_CORR_TX1,        ///< RAM for RS_CORR values for FTL/AGC usage for TX1
  HW_MEM_POOL_BUF_NCELL_RS_CORR_TX0,        ///< RAM for RS_CORR values for FTL/AGC usage for TX0
  HW_MEM_POOL_BUF_NCELL_RS_CORR_TX1,        ///< RAM for RS_CORR values for FTL/AGC usage for TX1
  HW_MEM_POOL_BUF_VPE_MEM_SWAP,
  HW_MEM_POOL_BUF_SCRATCH,
  HW_MEM_POOL_BUF_DEBUG_LOGGING,

  HW_MEM_POOL_BUF_BB_EMBMS_WHITENING,       ///< eMBMS whitening
  HW_MEM_POOL_BUF_BB_EMBMS_CH_EST_TX0_RX0,  ///< eMBMS chest, 1 tx * 2 rx
  HW_MEM_POOL_BUF_BB_EMBMS_CH_EST_TX0_RX1,
  HW_MEM_POOL_BUF_BB_EMBMS_RAW_CIR,         ///< eMBMS raw CIR

  HW_MEM_POOL_BUF_NB_NCELL_IDLE_SNE,
  HW_MEM_POOL_BUF_NB_NCELL_IDLE_SE,
  HW_MEM_POOL_BUF_IRAT_SEARCH_TD_SEC,
  HW_MEM_POOL_BUF_IQ_SAMPLE_CAPTURE,         ///< Mempool allocation to capture I/Q samples
  HW_MEM_POOL_BUF_FSCAN0_INBAND_TONE_EE,    ///< Freq scan in band tone energy values
  HW_MEM_POOL_BUF_FSCAN1_INBAND_TONE_EE,    ///< Freq scan in band tone energy values
  HW_MEM_POOL_BUF_FSCAN2_INBAND_TONE_EE,    ///< Freq scan in band tone energy values
  HW_MEM_POOL_BUF_FSCAN3_INBAND_TONE_EE,    ///< Freq scan in band tone energy values
  HW_MEM_POOL_BUF_FSCAN4_INBAND_TONE_EE,    ///< Freq scan in band tone energy values
  HW_MEM_POOL_BUF_FSCAN5_INBAND_TONE_EE,    ///< Freq scan in band tone energy values
  HW_MEM_POOL_BUF_FSCAN6_INBAND_TONE_EE,    ///< Freq scan in band tone energy values
  HW_MEM_POOL_BUF_FSCAN7_INBAND_TONE_EE,    ///< Freq scan in band tone energy values
  HW_MEM_POOL_BUF_VPE_LMEM_SWAP,            ///< Mempool to be used for VPE LMEM Swap
  HW_MEM_POOL_BUF_VPE_DMEM_SWAP,            ///< Mempool to be used for VPE DMEM Swap

  HW_MEM_POOL_BUF_MAX_INDEX
  
} hw_mem_pool_buf_enum;

/*===========================================================================

      Functions

===========================================================================*/

#endif /* HW_MEM_POOL_BUF_H */
