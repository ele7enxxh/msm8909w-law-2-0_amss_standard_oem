/*!
  @file
  tfw_sw_intf_srch.h

  @brief
  TD-SCDMA Srch FW-SW Interface Definitions 

*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Confidential and Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_tdscdma/api/tfw_sw_intf_srch.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
05/23/14    QT     support sawless
===========================================================================*/

#ifndef TFW_SW_INTF_SRCH_H
#define TFW_SW_INTF_SRCH_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "tfw_const.h"
#include "msgr.h"
#include "tfw_sw_intf_measurement.h"
#include "tfw_sw_intf_rx_tx.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

  /*-----------------------------------------------------------------------*/
  /*     CONSTANTS                                                         */
  /*-----------------------------------------------------------------------*/
/*! \brief Max syncl-dl detection freq bins */ 
#define TFW_MAX_SYNCDL_DET_FREQ_BINS      3
/*! \brief Max results from midamble detection */ 
#define TFW_SRCH_MAX_MIDAMBLE_DET_RESULTS 4
/*! \brief Max TS0 cells */ 
#define TFW_MAX_TS0_CELLS                 40

/**
    \defgroup cmdSearch Acquisition Search Commands & Responses  
    */
/*\{*/

/*\}*/

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR SRCH_POWER_SCAN COMMAND                       */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdPwrScan SRCH_POWER_SCAN Command
   \ingroup cmdSearch
  @verbatim
   SW Usage:
     1) This command can only be sent to FW after FW is in ACQ or Measurement
        (for IRAT) state
   FW Algorithm:
   - Wideband Power Measurement: 
     1) Detect saturation at ADC
   - Narrowband Power Measurement: 
     1) Accumulate the power after equalization filter; continuously estimate
        every 128 chips in ACQ
   - The analog gain state is determined as follows: 
     1) Only allow a subset of the set of gain states
        a) Currently considering only G1 and G3
        b) G0 has negligible gain in sensitivity compared to G1
        c) G1 and G3 have sufficient overlap in the received power range that
           G2 does not appear to be needed
     2) Start in the allowable gain state with the highest gain (G1)
        and look at the maximum wide-band and narrow-band power measurements
        over a 5ms window
        a) If either the maximum wide-band measurement exceeds a threshold
           (wbThresh[n]) or the maximum narrow-band measurement exceeds a
           threshold (nbThresh[n]), choose the next gain state and re-do the
           power scan
   @endverbatim 
*/
typedef struct {  
  /*! \brief Rx diversity flag 
  @verbatim
   0: One rx antenna
   1: Two rx antennas
   If set to 1, RxAntenna filed ignored
  @endverbatim 
  */
  uint8                     rxDiv : 1;
  /*! \brief Rx antenna config
  @verbatim
   0: Antenna 0
   1: Antenna 1
  @endverbatim 
  */
  uint8                     rxAntenna : 1;
  uint8                     : 6;
  /*! \brief Disable LNA ctrl (for test mode such as PBS baseband testing) 
  @verbatim
   0: LNA ctrl enable
   1: LNA ctrl disable
   If set to 1, lnaStateEnable[4] ignored
  @endverbatim 
  */ 
  boolean                   disableLNACtrl;
} tfw_srch_power_scan_cfg_t;

/*! @brief SRCH_POWER_SCAN Command
 *
 *   This is the message definition for SRCH_POWER_SCAN Command
 */
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief Power scan config */
  tfw_srch_power_scan_cfg_t pwrScanCfg;
} tfw_srch_power_scan_cmd_t;
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR SRCH_GAP_DETECT COMMAND                       */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdGapDetect SRCH_GAP_DETECT Command & Response
   \ingroup cmdSearch
   @verbatim
   SW Usage:
     1) This command can only be sent to FW after FW is in ACQ or Measurement
        (for IRAT) state
   FW algorithm:
   - Gap detection:
     1) Find high-low-high-low (HLHL) power pattern
     2) Determine whether gap detection is successful or failed
     3) Determine LNA state used for the rest of acquisition stages 
   - The analog gain state is determined as follows:
     1) Only allow a subset of the set of gain states
        a) Currently considering only G1, G2 and G3
     2) Start in the allowable gain state with the highest gain (G1)
     3) If gap detection is successful, re-do for the follwoing condition
        a) Either the narrow-band or the wide-band power measurement in the
           high power region around the gap crosses the threshold
     4) If Gap detection fails, re-do when both of the following are true
        due to the presence of a jammer
        a) Max wide-band power measurement over 5ms crosses a threshold,
           which indicates that there is some saturation happening
        b) Narrow-band power measurement at the same location is below a
           threshold, which indicates that saturation is not because of
           an in-band UL transmission
   @endverbatim 
*/
typedef struct {
  /*! \brief Gap detection threshold 
  @verbatim 
   Resolution: unsigned 16Q8
   Recommended configuration: TBD 
  @endverbatim 
  */
  uint16                    gapDetectThresh;  
  /*! \brief Thresholds to recheck false detection in lower band F
  @verbatim
  Resolution: unsigned 16Q8
  Recommended Configuration: TBD
  */
  uint16                    threshMaxQDiff;
  uint16                    threshMinQ;
  uint16                    threshNarrowGap;

  /*! \brief Rx diversity flag 
  @verbatim
   0: One rx antenna
   1: Two rx antennas
   If set to 1, RxAntenna filed ignored
  @endverbatim 
  */
  uint8                     rxDiv : 1;
  /*! \brief Rx antenna config
  @verbatim
   0: Antenna 0
   1: Antenna 1
  @endverbatim 
  */
  uint8                     rxAntenna : 1;
  uint8                     : 6;
  /*! \brief Disable LNA ctrl (for test mode such as PBS baseband testing) 
  @verbatim
   0: LNA ctrl enable
   1: LNA ctrl disable
   If set to 1, lnaStateEnable[4] ignored
  @endverbatim 
  */ 
  boolean                   disableLNACtrl;
  /*! \brief Number of subframes for gap detection 
  @verbatim
   Range: 0~255, placeholder for future change if needed
   Current valid configuration: 1
  @endverbatim 
  */ 
  uint8                     numSubFrame;
  /*! \brief Boolean to indicate gap detect could be false 
    due to presence of LTE signal. Need recheck.
    @verbatim
    0: No LTE signal in this band. No need to reverify gap detect pass.
    1: Check for false detection due to LTE signal in band. 
   */
  boolean                   testFalseAlarm;
} tfw_srch_gap_detect_cfg_t;

/*! @brief SRCH_GAP_DETECT Command
 *
 *   This is the message definition for SRCH_GAP_DETECT Command
 */
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief Gap detection config */
  tfw_srch_gap_detect_cfg_t gapDetectCfg;
} tfw_srch_gap_detect_cmd_t;
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR SRCH_SYNCDL_MIDAMBLE_DETECT COMMAND           */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdSyncdlMidambleDet SRCH_SYNCDL_MIDAMBLE_DETECT Command & Response
   \ingroup cmdSearch
   @verbatim
   SW Usage:
     1) This command can only be sent to FW after FW is in ACQ or Measurement
        (for IRAT) state
   FW algorithm:
     1) Offline samples collection
     2) HW SYNC-DL srch task dispatch
     3) Pruning SYNC-DL HW results
     4) HW midamble srch task dispatch
     5) Pruning midamble HW results
   @endverbatim 
*/
typedef struct {
  /*! \brief TS0 start position
  @verbatim 
   Resoltuion: cx8 
   If gap detection is successful, set based on SRCH_GAP_DETECT response 
  @endverbatim 
  */ 
  uint32                    ts0StartPosition : 16;
  /*! \brief Number of sync-dl detection freq hypotheses 
  @verbatim 
   Range: 1~3 
   Recommeded systems configuration for init acq: 3
  @endverbatim 
  */
  uint32                    syncDlNumFreqBins : 3;
  /*! \brief Full 5-ms window sync-dl srch flag 
  @verbatim
   0: FALSE, configured to 0 when gap detection is sucessful
   1: TRUE, configured to 1 when gap detection is failed/bypassed
   If set to 1, ts0StartPosition ignored
  @endverbatim 
  */
  uint32                    syncDlFullWinSrch : 1;
  /*! \brief Selected antenna
  @verbatim
   0: Antenna zero
   1: Antenna one
   Selected antenna from the gap detection response
  @endverbatim 
  */
  uint32                    selectedAntenna : 1;
  uint32                    :11;
  /*! \brief Midamble mask used to indicate known barred cells
  @verbatim 
   Bit n, midambleMask[m]: for midamble m*32+n 
  @endverbatim 
  */
  uint32                    midambleMask[4];
  /*! \brief SYNC-DL mask used to indicate known barred cells
  @verbatim 
   The same info as midambleMask used for sync-dl detection
   Bit n: for midamble from 4*n to 4*(n+1)-1
  @endverbatim 
  */
  uint32                    syncDlMask;
  /*! \brief SYNC-DL detection freq vectors
  @verbatim 
   Resoltuion: 1 Hz
   Up to 7 freqs
   Recommeded systems configuration for init acq: [-6670, 0, 6670] Hz
  @endverbatim 
  */
  int16                     syncDlfreq[TFW_MAX_SYNCDL_DET_FREQ_BINS];
  /*! \brief SYNC-DL detection pruning threshold
  @verbatim 
   Resolution: Q8 (linear)
   Recommended configuration: 16 (-12dB)
  @endverbatim 
  */
  uint16                    syncDlPruningThresh;
  /*! \brief SYNC-DL and midamble detection non-coherence integration length 
  @verbatim 
   Resoltution: 1 subframe, n means non-coherence length is n
   Recommeded systems configuration for init acq: 8 for one antenna
                                                  4 for two antennas
   Recommeded systems configuration for IRAT measurement: 1/3 for one antenna
  @endverbatim 
  */
  uint16                    nonCohLength;
  /*! \brief Midamble detection freq bin step size
  @verbatim 
   Resolution: 1 Hz 
   Recommeded systems configuration: 1000
  @endverbatim 
  */
  uint16                    midambleFreqStepSize;
  /*! \brief Midamble detection pruning timing threshold
  @verbatim
   Resolution: 1 chip
   Recommended configuration: 20 
  @endverbatim 
  */
  uint16                    midamblePruningTiming;
  /*! \brief Midamble detection pruning level 1 midamble threshold
  @verbatim
   Resolution: Q8 (linear)
   Recommended configuration: 16 (-12dB)
  @endverbatim 
  */
  uint16                    midamblePruningMid1;
  /*! \brief Midamble detection pruning level 2 midamble threshold
  @verbatim
   Resolution: Q8 (linear)
   Recommended configuration: 32 (-9dB)
  @endverbatim 
  */
  uint16                    midamblePruningMid2;
  /*! \brief Midamble detection pruning level 2 sync-dl threshold
  @verbatim
   Resolution: Q8 (linear)
   Recommended configuration: 32 (-9dB)
  @endverbatim 
  */
  uint16                    midamblePruningSync2;
  /*! \brief Midamble detection combining weight factor for non-zero shifts
  @verbatim 
   Resolution: Q14
   Recommeded systems configuration: 0x2000 (0.5)
  @endverbatim 
  */
  uint16                    midambleNonZeroShiftWt;
  /*! \brief Number of midamble detection freq hypotheses 
  @verbatim 
   Total freq bins = 2*midambleNumTwoSidedFreqBins + 1
   Recommeded systems configuration for init acq: 4
  @endverbatim 
  */
  uint8                     midambleNumTwoSidedFreqBins;
  /*! \brief LNA state info
      @verbatim
  refer to tfw_rx_lna_t infomation
  @verbatim
  */
  tfw_rx_lna_t lnaInfo;

} tfw_srch_sync_mid_detect_cfg_t;


/*! @brief SRCH_SYNCDL_MIDAMBLE_DETECT Command
 *
 *   This is the message definition for SRCH_SYNCDL_MIDAMBLE_DETECT Command
 */
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type           hdr;
  /*! \brief SYNC-DL and Midamble detection config */
  tfw_srch_sync_mid_detect_cfg_t syncMidDetectCfg;
} tfw_srch_syncdl_midamble_detect_cmd_t;
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR SRCH_REACQ COMMAND                            */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdReacq SRCH_REACQ Command & Response
   \ingroup cmdSearch
   @verbatim
   Usage:
     1) This command can only be sent to FW after FW is in Tracking state
   @endverbatim 
*/
typedef struct {
  /*! \brief Reacq mode
  @verbatim 
   0: 3 window (default recommended setting)
   1: 1 window 
  @endverbatim 
  */ 
  uint32                    mode : 1;
  /*! \brief Number of cells for RSCP measurement */
  uint32                    numCells : 6;
  /*! \brief Number of TSO JDCS cells
  @verbatim 
   1) Max number of TS0 JDCS cells = 8
   2) serving cell included
  @endverbatim 
  */ 
  uint32                    numTs0JdcsCells : 4;
  /*! \brief Rx diversity flag 
  @verbatim
   0: One rx antenna
   1: Two rx antennas
   If set to 1, RxAntenna filed ignored
  @endverbatim 
  */
  uint32                    rxDiv : 1;
  /*! \brief Rx antenna config
  @verbatim
   0: Antenna 0
   1: Antenna 1
  @endverbatim 
  */
  uint32                    rxAntenna : 1;
  /*! \brief action subframe number */
  uint32                    actionSfNum : 16;
  /*! \brief blind PCCPCH will follow reacq. 
  @verbatim
   0: No blind PCCPCH follows reacq
   1: Blind PCCPCH will follow reacq
  @endverbatim 
  */
  uint32                    stateChg : 1;
 
  /*! \brief Reacq Algo. 
  @verbatim
   0: Based on DwPTS TTL
   1: Based on Midamble TTL
  @endverbatim 
  */
  #define TFW_REACQ_DWP_TTL_MODE     0
  #define TFW_REACQ_MA_TTL_MODE      1
  uint32                    reacqTtlMode : 1;
  uint32                    : 1;

  /*! \brief Dwpts filted Freq offset (unit: Q8 Hz) */
  int32                     dwptsFiltFreqOffset;
  /*! \brief midamble filted Freq offset (unit: Q8 Hz) */
  int32                     midFiltFreqOffset;
  /*! \brief Cell cfg list
  @verbatim
   1) The top cell has to be the serving cell  
   2) Other JDCS cells follow the serving cell
   3) Other measurement cells follow JDCS cells
  @endverbatim 
  */ 
  uint8                     midambleIdx[8];
  /*! \brief Offset between RX Time counter and Wall Time counter 
  @verbatim
  RX Time Counter = Wall Time Counter - Offset Counter
  where RX Time = W.Z corrsponding to Wall Time = X.Y with Offset = L.M,
  Z, Y and M are chipX8 counts range from 0 to 6400*8-1 and wrap around 6400*8
  W, X and L are subframe numbers range from 0 to 8191 and wrap around 8192
  M = (Y >= Z)? (Y-Z) : (6400*8 + Y -Z);
  borrow = (Y >= Z)? 0 : 1;
  L = (X >= (W + borrow))? (X - W - borrow) : (8192 + X - W - borrow);
  @endverbatim 
  */ 
  union
  {
    /** sub frame count and chip x8 count */
    struct
    {
      uint32  ts0StartPosition : 16;
      uint32  subFrmCnt : 13;
      uint32  : 3;
    };
    /*! \brief Offset counter including both subframe and chipx8 count 
    at begin of TTI, relevent only for blind & S1/S2 PCCPCH */
    uint32  wallTimeOfs;
  };
  /*! @brief Drx cycle Length */
  #define TFW_DRX_CYCLE_640    0
  #define TFW_DRX_CYCLE_1280   1
  #define TFW_DRX_CYCLE_2560   2
  uint32 drxCycleLen : 2;
  /*! @brief Num of Drx cycle from last reAcq */
  uint32 drxCycleNum : 4;
  uint32 reserve     : 26;
  /*! \brief Rx config */
  tfw_rx_config_t           rxCfg;
} tfw_srch_reacq_cfg_t;

/*! @brief SRCH_REACQ Command
 *
 *   This is the message definition for SRCH_REACQ Command
 */
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief Reacq config */
  tfw_srch_reacq_cfg_t      cfg;
} tfw_srch_reacq_cmd_t;                     
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR SRCH_POWER_SCAN RESPONSE                      */
  /*-----------------------------------------------------------------------*/
/**
   \addtogroup cmdPwrScan 
   \ingroup cmdSearch
   @verbatim
   Power-scan output is computed as the maximum narrow-band power over 5ms:        
     1) (TBD) SW to converte to a dBm value using the gain of the corresponding
        gain state
     2) RF calibration is needed to determine the gain of each gain state
   @endverbatim 
*/
typedef struct {
  /*! \brief Max measured narrow-band power
  @verbatim
   Resolution: 16Q8 dBm
  @endverbatim 
  */
  int16                     maxNbPwr;
  /*! \brief Power scan LNA state
  @verbatim
   00: G0
   01: G1
   10: G2
   11: G3
   Invalid if disableLNACtrl in SRCH_POWER_SCAN command is set as 1
  @endverbatim 
  */
  uint8                     lnaState;
  
  /*! \brief errStatus
  @verbatim  
  
   If the measurement could not be done due to DSDx conflict or any other 
   reason such as inter freq RSCP measurement & UpPTS transmission, FW will
   inform the SW using the errStatus parameter.
   
  @endverbatim 
  */
  uint8                    errStatus;

} tfw_srch_power_scan_result_t;
/*! @brief SRCH_POWER_SCAN Response
 *
 *   This is the message definition for SRCH_POWER_SCAN Response
 */
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type         hdr;
  /*! \brief Power scan response */
  tfw_srch_power_scan_result_t powerScanRsp;
} tfw_srch_power_scan_rsp_t;                     
/*@}*/
             
              
  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR SRCH_GAP_DETECT RESPONSE                      */
  /*-----------------------------------------------------------------------*/
/**
   \addtogroup cmdGapDetect
   \ingroup cmdSearch
   @verbatim
   1. Only detected and ts0StartPosition in the response message are used to
      configure the next acq stage (sync-dl and midamble detection).
   2. For implementation efficiency, FW doesn't calculate ratios to find the
      power pattern. So FW returns power of high-power and low-power regions
      around the gap to SW.
   3. lnaState in the response message is used by FW for the following acq
      stages.
   @endverbatim 
*/
typedef struct {
  /*! \brief Used in IRAT state only to return TFW IRAT state 
  @verbatim
  Valid state variables for IRAT Step0 response are:
  @endverbatim */
  tfw_irat_state_t          state;
  /*! \brief Power of high-power region around the gap 
  @verbatim
   Resolution and bitwidth TBD
  @endverbatim 
  */ 
  uint32                    highPwrRegionEng;
  /*! \brief Power of high-power region around the gap 
  @verbatim
   Resolution and bitwidth TBD
  @endverbatim 
  */ 
  uint32                    lowPwrRegionEng;
  /*! \brief TS0 start position (cx8) 
   Invalid if detected flag is 0
  */   
  uint16                    ts0StartPosition;
  /*! \brief Gap detection flag
  @verbatim
   0: detection fails
   1: detected
  @endverbatim 
  */ 
  boolean                   detected;
  /*! \brief LNA state info
  @verbatim
   refer to tfw_rx_lna_t infomation
  @verbatim
  */
  tfw_rx_lna_t lnaInfo;

} tfw_srch_gap_detect_per_chain_result_t;

typedef struct {
  /*! \brief Try again flag for LNA loop
  @verbatim
   This field valid only in IRAT state
   0: consider detected field as final result
   1: retry gap detection with lna state returned in this msg
  @endverbatim 
  */ 
  boolean                   tryAgain;
  /*! \brief Selected antenna
  @verbatim
   0: Antenna zero
   1: Antenna one
   Selected antenna based on comparson of gap detection matrics of 
   Rx0 and Rx1
  @endverbatim 
  */
  uint8                     selectedAntenna;
  
  /*! \brief errStatus
  @verbatim  
  
   If the measurement could not be done due to DSDx conflict or any other 
   reason such as inter freq RSCP measurement & UpPTS transmission, FW will
   inform the SW using the errStatus parameter.
   
  @endverbatim 
  */
  uint8                     errStatus;
    
  /*! \brief per chain result */
  tfw_srch_gap_detect_per_chain_result_t chain[TFW_NUM_ANTENNAS];
} tfw_srch_gap_detect_result_t;
/*! @brief SRCH_GAP_DETECT Response
 *
 *   This is the message definition for SRCH_GAP_DETECT Response
 */
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type         hdr;
  /*! \brief Gap detection response */
  tfw_srch_gap_detect_result_t gapDetectRsp;
} tfw_srch_gap_detect_rsp_t;                     
/*@}*/
             

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR SRCH_SYNCDL_MIDAMBLE_DETECT RESPONSE          */
  /*-----------------------------------------------------------------------*/
/**
   \addtogroup cmdSyncMidambleDet
   \ingroup cmdSearch
   @verbatim
   1. Up to 32 SYNC-DL detection results are passed to midamble detection.
   2. After midamble detection with pruning procedures, FW returns up to
      TFW_SRCH_MAX_MIDAMBLE_DET_RESULTS(8) results to SW. Up to 4 cells
      (configured by SW) are used for the following stages.
   @endverbatim 
*/
typedef struct {
  /*! \brief DL-SYNC index (0~31)*/   
  uint32                    syncdlIdx : 8;
  /*! \brief Midamble index */   
  uint32                    midambleIdx : 8;
  /*! \brief TS0 start position (cx8) */   
  uint32                    ts0StartPosition : 16;
  /*! \brief Quality metrics SNR */   
  uint16                    snr;
  /*! \brief Freq offset (Hz) */ 
  int16                     freqOffset;
} tfw_srch_detected_cell_t;

typedef struct {
  /*! \brief Number of results */
  uint8                     numResult;
  
   
  /*! \brief errStatus
  @verbatim  
  
   If the measurement could not be done due to DSDx conflict or any other 
   reason such as inter freq RSCP measurement & UpPTS transmission, FW will
   inform the SW using the errStatus parameter.
   
  @endverbatim 
  */
  uint8                     errStatus; 
  /*! \brief Used in IRAT state only to return TFW IRAT state 
  @verbatim
  Valid state variables for Step1/2 response are:
  activateTime, vpeBiasFlag
  @endverbatim */
  tfw_irat_state_t          state;
  /*! \brief Midamble detection results 
  @verbatim
   detectedCell[0]: active cell (with the highest SNR)
   Rest of detectedCell: other cells
  @endverbatim 
  */
  tfw_srch_detected_cell_t  detectedCell[TFW_SRCH_MAX_MIDAMBLE_DET_RESULTS];
} tfw_srch_sync_mid_det_result_t;

/*! @brief SRCH_SYNCDL_MIDAMBLE_DETECT Response
 *
 *   This is the message definition for SRCH_SYNCDL_MIDAMBLE_DETECT Response
 */
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type           hdr;
  /*! \brief Sync-dl and midamble detection response */
  tfw_srch_sync_mid_det_result_t syncMidDetRsp;
} tfw_srch_syncdl_midamble_detect_rsp_t;                     
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR SRCH_REACQ RESPONSE                           */
  /*-----------------------------------------------------------------------*/
/**
   \addtogroup cmdReacq
   \ingroup cmdSearch
   @verbatim
   TBD
   @endverbatim 
*/
typedef struct {
  /*! \brief Freq offset (unit: Q8 Hz), estimated in serving cell's 
             primary freq */
  int32                     freqOffset;
  /*! \brief Dwpts filted Freq offset (unit: Q8 Hz) */
  int32                     dwptsFiltFreqOffset;
  /*! \brief midamble filted Freq offset (unit: Q8 Hz) */
  int32                     midFiltFreqOffset;
  /*! \brief Timing error of the serving cell (unit: cx8) */
  int32                     timingError;
  /*! \brief timing measurement results
  @verbatim
   1) The top is the new serving cell wallTime offset
  @endverbatim */ 
  uint32                    wallTimeOfs[8];
  /*! \brief Rx Subframe nubmer */
  uint16                    subframeNum;
  /*! \brief SIR for the primary antenna */
  uint16                    priAntSir;
  /*! \brief reacq out of sync flag  
  @verbatim
   0: in sync, 
   1: out of sync, used for next four subframes to continue FTL
  @endverbatim 
  */
  boolean                   outOfSync;
  
  /*! \brief errStatus
  @verbatim  
  
   If the measurement could not be done due to DSDx conflict or any other 
   reason such as inter freq RSCP measurement & UpPTS transmission, FW will
   inform the SW using the errStatus parameter.
   
  @endverbatim 
  */
  uint8                     errStatus;
    
} tfw_srch_reacq_result_t;

/*! @brief SRCH_REACQ Response
 *
 *   This is the message definition for SRCH_REACQ Response
 */
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief Reacq response */
  tfw_srch_reacq_result_t   rsp;
} tfw_srch_reacq_rsp_t;                     
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR SRCH S1/S2 DETECTION (SHALLOW SRCH)           */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup noteS1S1 SRCH S1/S2 Detection (Shallow Srch) Notes
   \ingroup cmdSearch
   @verbatim
   Usage:
     1) After midamble detection, SW shall send FW a STATE_CFG cmd msg for
        FW state change (from TFW_STATE_ACQUISITION to TFW_STATE_TRACKING)
     2) In STATE_CFG cmd msg to TFW_STATE_TRACKING, SW shall configure the
        serving cell info (midamble index, timing and freq info based on
        the top result from SRCH_SYNCDL_MIDAMBLE_DETECT response msg.
     3) SW shall wait for STATE_CFG response msg from FW
     4) SW shall send CELL_SERVING_JDCS cmd msg to configure JDCS, TS0-JDS
        and serving cell working frequency.
     5) S1/S2 detection (or blind CPCCH decoding) configuration is part of
        PCCPCH_CFG cmd msg. SW shall send PCCPCH_CFG cmd msg to FW.
    @endverbatim 
*/



#endif /* TFW_SW_INTF_SRCH_H */
