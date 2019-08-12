#ifndef _HDRFW_LOGGING_H_
#define _HDRFW_LOGGING_H_

/*!
  @file
  hdrfw_logging.h

  
  Defines the HDR firmware logging interface for the HDRFW module

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_c2k/api/hdrfw_logging.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

// comdef.h is NOT included here.
#include "hdrfw_const.h"


/*===========================================================================

   MCDO QXDM DIAG LOGGING

===========================================================================*/
#define HDRFW_DIAG_LOG_RX_AGC_PKT_ID 0x15CE
#define HDRFW_DIAG_LOG_AFC_PKT_ID    0x15CF
#define HDRFW_DIAG_LOG_TXAGC_PKT_ID  0x15D0
#define HDRFW_DIAG_LOG_DLNA_PKT_ID   0x15D1
#define HDRFW_DIAG_LOG_DSDA_TX_PKT_ID   0x15D2
#define HDRFW_DIAG_LOG_DSDA_RX_PKT_ID   0x15D3
#define HDRFW_DIAG_LOG_FING_PKT_ID      0x15D4
#define HDRFW_DIAG_LOG_MAC_PKT_ID       0x15D5
#define HDRFW_DIAG_LOG_RX_IQ_SAMPLE_PKT_ID 0x15D6
#define HDRFW_DIAG_LOG_VSRC_STMR_PKT_ID   0x15D7

/*===========================================================================
   
  MCDO 4-slot PING-PONG LOGGING

============================================================================*/

// TX log buffer RL log word bit positions.
#define LOG_RL__DECACTIVE          15
#define LOG_RL__FORCE_ACK          14
#define LOG_RL__ACK_EN             13
#define LOG_RL__NACK               12
#define LOG_RL__DRC_BOOST          11
#define LOG_RL__DSC_BOOST          10
#define LOG_RL__MACK                7
#define LOG_RL__DSC                 4
#define LOG_RL__DRC_COVER           0

// TX log buffer DRC log word bit positions.
#define LOG_DRC__TENTATIVE          8
#define LOG_DRC__TRANSMITTED        0

#define LOG_AGC_STAT__THROTTLE     15
#define LOG_AGC_STAT__TXLIMITMAX   14
#define LOG_AGC_STAT__TXLIMITMIN   13
#define LOG_AGC_STAT__RPC           0

#define LOG_RX__GSET_ASP2           13
#define LOG_RX__GSET_ASP1           10
#define LOG_RX__GSET_ASP0           7

#define LOG_RX__GSET_ASP5           13
#define LOG_RX__GSET_ASP4           10
#define LOG_RX__GSET_ASP3           7

#define LOG_RX__DEMODASPP           4
#define LOG_RX__NEXTBESTASP         0

// RX per carrier log structure.
typedef struct {

  // Must unsigned because we shift bits from
  // msb.
  uint16 RPCValid;
  uint16 RPC[HDRFW_NUM_ASP];
  uint16 RAB[HDRFW_NUM_ASP];
  uint16 ASP[HDRFW_LOG_SLOTS];
  /**< See LOG_RX__XXXXXX for bit positions of each field. Specifically,       
       bit 0-2: ASP on which we are requesting service 
       bit 3: reserved
       bit 4-6: ASP used for demodulation
       bit 7-9:   Active set pilot indexes x corresponding to
                  entry 0 in gRICE tracking set (i.e. G-set)
       bit 10-12:  Active set pilot indexes x corresponding to
                  entry 1 in gRICE tracking set (i.e. G-set)
       bit 13-15:  Active set pilot indexes x corresponding to
                  entry 2 in gRICE tracking set (i.e. G-set)
    */


  uint16 RPCMean[HDRFW_NUM_CELLS];

  /**  See LOG_RX__XXXXXX for bit positions of each field. Specifically,       
       bit 0-2: Chosen Equalizer - always equal to 0 (31 tap) for grice
       bit 3: Flat Channel detected
       bit 4: Single tap Channel detected
       bit 5: 4mmse mode
       bit 6: FAB for ASP1 in GSET detected
       bit 7: FAB for ASP2 in GSET detected
       bit 8: FAB for ASP3 in GSET detected
       bit 9: FAB for ASP4 in GSET detected
       bit 10: FAB for ASP5 in GSET detected
       bit 11-13: reserved. 
       bit 14: channel is disperive
       bit 15: Demod decision, 1 - EQ, 0 - Rake
  */
  uint16 EQDemodDecision[HDRFW_LOG_SLOTS];
  uint16 EQRakeCIAvr[HDRFW_LOG_SLOTS];
  int32 EQBestCIAvr[HDRFW_LOG_SLOTS];
  int32 BestASPSINR[HDRFW_LOG_SLOTS];
  uint16 BestASPRakePrxSINR[HDRFW_LOG_SLOTS];
  /**< Best ASP's Primary antenna SINR per slot */

  
  /** 32 bit Eq Status: the definition of the upper 16 bit is the same as EQDemodDecision. The definition of the lower 16 bit is:
      bit 0: 1 if grice is chosen
      bit 1: 1 if Cnn is chosen. 
      bit 2: 1 if Rnn Rank deficiency is effective.
      bit 3: not use in Q6. Q4 use for safety mechanism.
      bit 4: 1 if null off-diagonal happens.
      bit 5-6: reserved. 
      bit 7-9:   Active set pilot indexes x corresponding to
                  entry 3 in gRICE tracking set (i.e. G-set)
      bit 10-12:  Active set pilot indexes x corresponding to
                  entry 4 in gRICE tracking set (i.e. G-set)
      bit 13-15:  Active set pilot indexes x corresponding to
                  entry 5 in gRICE tracking set (i.e. G-set)
 
      */ 
  uint32 EQStatus[HDRFW_LOG_SLOTS]; 


} hdrfw_log_carrier_t;

//AGC info structure
typedef struct {

  struct{
   int16  RxAGCCommon;
   int16  RxAGCAccum[HDRFW_TOTAL_CARRIERS];
   int16  LNAState;
  }antenna[HDRFW_NUM_ANTENNAS];

} hdrfw_log_agc_t;

/** Structue of FAB results that are reported by FW. */
typedef struct {
  int32 FABMetric;
  /*!< FAB metric for the serving sector, for use in FAC gain estimation algorithm*/

  int32 FABCp; 
  /*!< FAB metric for the serving sector, for use in FAC gain estimation algorithm. */
} hdrfw_grice_fac_gain_est_entry_t; 

/** Structure of FAB result buffer that are to be read by SW. */
typedef struct {

  hdrfw_grice_fac_gain_est_entry_t fabRes[HDRFW_FAC_GAIN_EST_DEPTH]; 
  /*!< FAB results for FAC gain estimation. FW fill this buffer.
       Buffer size can hold up to 3 carriers, 4 slots of report. */

  uint32 numElements; 
  /*!< Number of elements in buffer. */

} hdrfw_grice_fac_gain_est_buffer_t;

// RX log buffer
typedef struct {

  uint16 SlotTime;
  int16  RxAGCCommon[HDRFW_NUM_ANTENNAS];
  int16  RxAGCAccum[HDRFW_NUM_ANTENNAS][HDRFW_TOTAL_CARRIERS];
  uint16 CarrierBufferValid;
  uint16 EnabledCarrierMask;
  hdrfw_log_agc_t RxAGC[HDRFW_LOG_SLOTS];
  hdrfw_log_carrier_t C[HDRFW_DEMOD_CARRIERS];
  hdrfw_grice_fac_gain_est_buffer_t FABEstBuffer;
} hdrfw_log_buffer_t;

// Digital gain log structure.
typedef struct {

  uint16 Pilot;
  uint16 Data1Cos;
  uint16 Data0Cos;
  uint16 AUX;
  uint16 RRI;
  uint16 DSC;
  uint16 DRC;
  uint16 ACK;
  uint16 BetaP;
  uint16 TxTotal;
  uint16 TxPilotPower;
  uint16 TxGainAdjVal;
  uint16 TxOpenLoop;
  uint16 PwrMargin;
  uint16 MaxRLRatePeakTP;
  uint16 TxAGCStatus;

} tx_log_digital_gain_t;

// TX per carrier log structure.
typedef struct {
  
  uint16 RL[HDRFW_LOG_SLOTS];
  uint16 DRC[HDRFW_LOG_SLOTS];
  // Two half-slots of digital gain logging.
  tx_log_digital_gain_t DigitalGain[HDRFW_LOG_DGAIN_HS];

} hdrfw_tx_log_carrier_t;

// DTX log buffer structure.
// DTX logging once every slot (in sync with dtx frame offset)
// NOTE: This is irrespective of carrier
typedef struct {

  /*!< indicates FW DTX HS index */
  int16 txHsIndex[HDRFW_LOG_SLOTS];
  
  /*!< indicates FW DTX state */
  int16 dtxCtl[HDRFW_LOG_SLOTS];

  /*!< indicates FW DTX frame offset */
  int16 dtxFrameOffset[HDRFW_LOG_SLOTS];

  /*!< indicates if slot is DTX slot */
  int16 dtxSlot[HDRFW_LOG_SLOTS];

  /*!< indicates if PA/TX is ON/OFF. */
  int16 dtxPaTxState[HDRFW_LOG_SLOTS];

  /*!< indicates ratcheting max */
  int16 txLimitingMax[HDRFW_LOG_SLOTS];

  /*!< indicates ratcheting min */
  int16 txLimitingMin[HDRFW_LOG_SLOTS];

  /*!< indicates closed loop power */
  int16 closedLoopPwr[HDRFW_DEMOD_CARRIERS][HDRFW_LOG_SLOTS];

  /*!< indicates loop power power */
  int16 openLoopPwr[HDRFW_DEMOD_CARRIERS][HDRFW_LOG_SLOTS];

  /*!< indicates total power. */
  int16 txTotalPwr[HDRFW_DEMOD_CARRIERS][HDRFW_LOG_SLOTS];

  /*!< indicates PA head room available */
  int16 pwrMargin[HDRFW_DEMOD_CARRIERS][HDRFW_LOG_SLOTS];

   /*!< indicates pilot channel gain */
  int16 pilotGain[HDRFW_DEMOD_CARRIERS][HDRFW_LOG_SLOTS];

   /*!< indicates drc channel gain */
  int16 drcGain[HDRFW_DEMOD_CARRIERS][HDRFW_LOG_SLOTS];

} hdrfw_dtx_logging_t;

typedef struct {

  /*!< indicates if slot is DTX slot */
  int16 dtxSlot[HDRFW_LOG_SLOTS_IN_FRAME];

  /*!< indicates if slot is DRX slot */
  int16 drxSlot[HDRFW_LOG_SLOTS_IN_FRAME];

}hdrfw_power_logging_t;

// Flexconn log buffer structure.
// Flexconn logging once every 4 slots (i.e sub-frame)
typedef struct {

  /*!< indicates FW flexconn state */
  int16 fcEnable;

  /*!< indicates FW flexconn sub-state */
  int16 fcSubState;

  /*!< indicates if PA/TX is ON/OFF. */
  int16 fcPaTxState; 

  /*!< indicates DSC/DRC repointing */
  int16 fcRepoint; 

  /*!< indicates if PA/TX is ON/OFF. */
  // xxx: need confirmation on this
  int16 fcDrcLock[HDRFW_DEMOD_CARRIERS][HDRFW_NUM_ASP];

} hdrfw_flexconn_logging_t;

// TX log buffer structure.
typedef struct {

  uint16 TxHalfSlotIndex;
  uint16 CarrierBufferValid;
  uint16 EnabledCarrierMask;
  int16 TxTotal[HDRFW_LOG_DGAIN_HS]; // TxTotal over all carriers.
  int16 TxLimiting[HDRFW_LOG_DGAIN_HS]; // TxTotal over all carriers.
  hdrfw_tx_log_carrier_t C[HDRFW_DEMOD_CARRIERS];

//#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  uint16 TxMaxLimitingCount; // Counter for counting TX total pwr > max limit
//#endif
  
  // dtx/flexconn logging
  hdrfw_dtx_logging_t dtxLog;
  hdrfw_flexconn_logging_t fcLog;  

} hdrfw_tx_log_buffer_t;

#endif
