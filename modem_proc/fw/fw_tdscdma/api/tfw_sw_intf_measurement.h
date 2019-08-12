/*!
  @file
  tfw_sw_intf_measurement.h

  @brief
  TD-SCDMA Measurement FW-SW Interface Definitions 

*/

/*===========================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_tdscdma/api/tfw_sw_intf_measurement.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
11/14/12   JP      Merge Nikel CL 2805599 Implemented M metric algorithm in X2T IRAT
===========================================================================*/

#ifndef TFW_SW_INTF_MEASUREMENT_H
#define TFW_SW_INTF_MEASUREMENT_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "tfw_sw_intf_common.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/**
    \defgroup cmdMeasurements Measurement Commands & Responses
    */
/*\{*/

  /*-----------------------------------------------------------------------*/
  /*     CONSTANTS                                                         */
  /*-----------------------------------------------------------------------*/
/*! \brief Max cells */ 
#define TFW_MAX_CELLS                     128
/*! \brief Max RSCP measurement cells per TS0*/ 
#define TFW_MAX_RSCP_CELLS                40
/*! \brief Max measurement cells for OTD tracking per TS0*/ 
#define TFW_MAX_OTD_CELLS                 8
/*! \brief the rest number of RSCP cells w/o tracking per TS0*/ 
#define TFW_MAX_NONTRAK_RSCP_CELLS         (TFW_MAX_RSCP_CELLS-TFW_MAX_OTD_CELLS)
/*! \brief Max number of ISCP measurement per subframe */ 
#define TFW_MAX_NUM_ISCP                  5

/*\}*/


/* ! \brief These are the error codes that all measurement 
            response messages, errStatus field will use */ 

/*! \brief Incidates measurement results are valid */ 
#define TFW_RESULT_OK                              0  
#define TFW_RESULT_NOK_TS0_CELL_CMD_NOT_RECEIVED   1
#define TFW_RESULT_NOK_SERVING_CELL_CONFIG_ERROR   2  

/*! \brief Indicates mesurement could not be done due to other reasons such
           as gap/hw sharing constraints, etc. and reported values should 
           be ignored */
#define TFW_RESULT_NOK_OTHER_REASON                3

#define TFW_RESULT_NOK_CXM_CONFLICT_CHECK_LOST     4

#define TFW_RESULT_NOK_REACQ_NOT_PERFORMED         5

#define TFW_RESULT_NOK_REACQ_RECEIVED_LATE         6

#define TFW_RESULT_NOK_REACQ_RF_NOT_READY          7

#define TFW_RESULT_NOK_RX_TUNING_LATE              8

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR MEASURE_RSCP COMMAND                          */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdRscp MEASURE_RSCP Command
   \ingroup cmdMeasurements
   @verbatim
   Usage:
     1) This command can only be sent to FW after FW is in Tracking state
     2) One cmd measures one TS0 slot RSCP for up to TFW_MAX_RSCP_CELLS (40)
     3) One cmd measures one set of OTD measurements for up to
        TFW_MAX_OTD_CELLS (8)
     4) When inter-freq RSCP measurement is requrested by SW and UpPTS
        transmission is scheduled, there's no RSSI measurement;
        Otherwise, RSSI measurement happens.
    @endverbatim 
*/
typedef struct {
  /*! \brief Previous timing measurement result
  @verbatim 
   1) Absolute wall time offset of the subframe boundary (cx8) 
   2) If available, configured as ts0StartPosition from previous RSCP 
      response msg.
   3) If not available, configured as ts0StartPosition from the serving 
      cell.
  @endverbatim 
  */
  uint16                    ts0StartPosition;
  /*! \brief Cells midamble index */
  uint8                     midambleIdx;
} tfw_measure_rscp_cell_t;

typedef struct {
  /*! \brief power measurement value */
  uint32                     pwrMeasureValue;
  /*! \brief old power measurement value */
  uint32                     pwrMeasureOldValue;
  /*! \brief LNA state (0=G0, 1=G1, 2=G3, 3=G4) */
  uint8                      state;
  /*! \brief LNA hold timer */
  uint8                      holdTimer;
} tfw_inter_freq_lna_t;

typedef struct {
  /*! \brief Number of cells for RSCP measurement */
  uint32                    numCells : 6;
  /*! \brief Number of TSO JDCS cells for the measured frequency
  @verbatim 
   Max number of TS0 JDCS cells = 8
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
  /*! \brief Inter freq measurement flag
  @verbatim 
   0: Intra freq
   1: Inter freq
  @endverbatim 
  */
  uint32                    interFreq : 1;
  /*! \brief Active cell primary freq flag 
  @verbatim 
   0: measured freq is not the same as primary freq of the serving cell
   1: measured freq is the same as the primary freq of the serving cell
  @endverbatim 
  */
  uint32                    activePrimaryFreq : 1;
  /*! \brief RF script buffer index for inter freq tuning
  @verbatim 
   If interFreq is 0, ignored
  @endverbatim 
  */
  uint32                    rfRxFreqBufIndex : 8;
  /*! \brief Index to RXLM static settings
  @verbatim 
   If interFreq is 0, ignored
  @endverbatim 
  */
  uint32                    rxlmStaticBufIndex : 8;
  uint32                    :2;
  /*! \brief measured frequency (Hz)
  @verbatim 
   Used by FW for Frequency Tracking Loop (FTL)
  @endverbatim 
  */
  uint32                    measuredFreq;
  /*! \brief Cells config for RSCP measurement 
  @verbatim 
   1) If numCells>8, cellCfg[0~7] has OTD measurement.
   2) If previous timing measurement results are available, configure 
      ts0StartPosition for the top 8 strongest cell.  
   3) If (measured freq==serving cell primary freq), 
      a. The first cell (cellCfg[0]) must be the serving cell
      b. The next are the rest of TS0 JDS cells for that freq
      c. The next are other cells 
  @endverbatim 
  */
  tfw_measure_rscp_cell_t   cellCfg[TFW_MAX_RSCP_CELLS];
} tfw_measure_rscp_cfg_t;

/*@{*/
/*! @brief MEASURE_RSCP Command
 *
 *   This is the message definition for MEASURE_RSCP Command
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief RSCP config */
  tfw_measure_rscp_cfg_t    rscpCfg;
} tfw_measure_rscp_cmd_t;             
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR MEASURE_ALL_RSCP COMMAND                      */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdAllRscp MEASURE_ALL_RSCP Command
   \ingroup cmdMeasurements
   @verbatim
   Usage:
     1) This command can only be sent to FW after FW is in Tracking state
        or IRAT state
     2) One cmd measures one TS0 slot RSCP for all 128 cells in the same freq
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
  uint32                    rxDiv : 1;
  /*! \brief Rx antenna config
  @verbatim 
   0: Antenna 0
   1: Antenna 1
  @endverbatim 
  */ 
  uint32                    rxAntenna : 1;
  /*! \brief Inter freq measurement flag
  @verbatim 
   0: Intra freq
   1: Inter freq
  @endverbatim 
  */
  uint32                    interFreq : 1;
  /*! \brief Active cell primary freq flag 
  @verbatim 
   0: measured freq is not the same as primary freq of the serving cell
   1: measured freq is the same as the primary freq of the serving cell
  @endverbatim 
  */
  uint32                    activePrimaryFreq : 1;
  /*! \brief RF script buffer index for inter freq tuning
  @verbatim 
   If interFreq is 0, ignored
  @endverbatim 
  */
  uint32                    rfRxFreqBufIndex : 8;
  /*! \brief Index to RXLM static settings
  @verbatim
   If interFreq is 0, ignored
  @endverbatim
  */
  uint32                    rxlmStaticBufIndex : 8;
  /*! \brief RF script buffer index for inter freq tuner update and cleanup
  @verbatim 
   If interFreq is 0, ignored
  @endverbatim 
  */
  uint32              rfInterFreqTunerBufIndex : 1;
  uint32                    :11;
  /*! \brief measured frequency (Hz)
  */
  uint32                    measuredFreq;
} tfw_measure_all_rscp_cfg_t;

/*@{*/
/*! @brief MEASURE_ALL_RSCP Command
 *
 *   This is the message definition for MEASURE_ALL_RSCP Command
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type       hdr;
  /*! \brief RSCP config */
  tfw_measure_all_rscp_cfg_t cfg;
} tfw_measure_all_rscp_cmd_t;             
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR MEASURE_ISCP COMMAND                          */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdIscp MEASURE_ISCP Command & Response
   \ingroup cmdMeasurements
   @verbatim
   Usage:
     1) This command can only be sent to FW after FW is in Tracking state
     2) One cmd measures up to TFW_MAX_NUM_ISCP (5) every subframe
     3) In case ISCP measurement is not performed for some slot (for example,
        TS2 when HS-SCCH is on slot 6) due to SW latency, ISCP measurement
        for that slot is skipped and the result is marked as 0 (or TBD number
        which is not possible for ISCP) in MEASURE_ISCP response message.
   @endverbatim 
*/
typedef struct {
  /*! \brief Slot Index 
  @verbatim 
   valid slot index for ISCP measurement: 2, 3, 4, 5, 6, 0
  @endverbatim 
  */ 
  uint8                     slotIdx;
  /*! \brief Cell midamble index */
  uint8                     midambleIdx;
} tfw_measure_iscp_slot_cell_t;

typedef struct {
  /*! \brief ISCP measurement slot & cell configuration 
  @verbatim 
   slotCellCfg[n] has the lower/same slotIdx than slotCellCfg[n+1] 
  @endverbatim 
  */
  tfw_measure_iscp_slot_cell_t slotCellCfg[TFW_MAX_NUM_ISCP];
  /*! \brief Number of ISCP measurements */
  uint8                        numIscp;
  /*! \brief LSB for the subframe number
  @verbatim 
   Placeholder, required if double buffers needed.
   Based on current SW design (sending the cmd to FW no earlier than subframe 
   n TS0 for subframe n ISCP measurement), double buffers not needed.
  @endverbatim 
  */
  uint8                        subframeLsb;
} tfw_measure_iscp_cfg_t;

/*@{*/
/*! @brief MEASURE_ISCP Command
 *
 *   This is the message definition for MEASURE_ISCP Command
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief ISCP config */
  tfw_measure_iscp_cfg_t    iscpCfg;
} tfw_measure_iscp_cmd_t;            
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR MEASURE_RSCP RESPONSE                         */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdRscp MEASURE_RSCP Response
   \ingroup cmdMeasurements
   @verbatim
    FW sends the response msg to SW after RSCP (with OTD and RSSI)
    measurements are completed.  
   @endverbatim 
*/
typedef struct {

  /*! \brief errStatus
  @verbatim  
  
   If the measurement could not be done due to DSDx conflict or any other 
   reason such as inter freq RSCP measurement & UpPTS transmission, FW will
   inform the SW using the errStatus parameter.
   
  @endverbatim 
  */
  uint8                     errStatus;
  
  /*! \brief RSSI, per antenna
  @verbatim
   
   Assert if rssi is set to 0 and attribute indicates a valid measurement result.
   rssi value should not be allowed to be 0 any more with the attribute 
   parameter defined above
  @endverbatim 
  */
  int32                     rssi[TFW_NUM_ANTENNAS];
  /*! \brief measured frequency (Hz) 
  @verbatim 
   From MEASURE_RSCP cmd msg from SW, to make it easier for SW to store
   inter freq LNA info.
  @endverbatim 
  */
  uint32                    measuredFreq;
  /*! \brief RSCP measurement results
  @verbatim
   The index of rscp array matches cellCfg index in MEASURE_RSCP Command msg.
   To reduce the payload size, RSCP is not reported for each antenna. Format 16Q8
  @endverbatim */ 
  int16                     rscp[TFW_MAX_RSCP_CELLS];
  /*! \brief timing measurement results
  @verbatim
   1) The index of rscp array matches cellCfg index (0~7) in MEASURE_RSCP 
      Command msg.
   2) Absolute wall time offset of the subframe boundary (cx8) 
  @endverbatim */ 
  int16                     ts0StartPosition[TFW_MAX_OTD_CELLS];
  /*! \brief Subframe nubmer for ISCP measurement */
  uint16                    subframeNum;
  /*LNA state info*/
  tfw_rx_lna_t lnaInfo;
} tfw_measure_rscp_result_t;

/*@{*/
/*! @brief MEASURE_RSCP Response
 *
 *   This is the message definition for MEASURE_RSCP Response
 */
typedef struct {  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief RSCP response */
  tfw_measure_rscp_result_t rscpRsp;
} tfw_measure_rscp_rsp_t;                     
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR MEASURE_ALL_RSCP RESPONSE                     */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdRscpAll MEASURE_ALL_RSCP Response
   \ingroup cmdMeasurements
   @verbatim
    FW sends the response msg to SW after RSCP measurements on all
    TFW_MAX_CELLS are completed.  
   @endverbatim 
*/


typedef struct {

  /*! \brief errStatus
  @verbatim  
  
   If the measurement could not be done due to DSDx conflict or any other 
   reason such as inter freq RSCP measurement & UpPTS transmission, FW will
   inform the SW using the errStatus parameter.
   
   FW cannot do this measurement if it looses CXM conflict check in DSDx. 
   Also In IRAT state, TFW may not be able to collect enough samples for RSCP
   measurement, either due to resource constraint or short gap 
   span. This parameter is a way to report back such errors and indicate
   to L1 that measurements should be ignored.   
  @endverbatim 
  */
  uint8                      errStatus;
  /*! \brief RSCP measurement results
  @verbatim
   The index of rscp array matches cellCfg index in MEASURE_RSCP Command msg.
   To reduce the payload size, RSCP is not reported for each antenna. Format 16Q8
  @endverbatim */ 
  int16                      rscp[TFW_MAX_CELLS];

  /*! \brief TSRSP results
  @verbatim
   Populated only in IRAT state. The index of TSRSP array matches cell ID.
  @endverbatim */ 
  int16                      tsRsp[TFW_MAX_CELLS];

  /*! \brief M-metric 
  @verbatim
  Populated only in IRAT state. The index of mMetric array matches jdsCells in
  IRAT_MEASURE_RSCP message.
  @endverbatim */ 
  uint16                      mMetric[TFW_IRAT_MAX_JDS_CELLS];

  /*! \brief RSSI
  @verbatim
  Populated only in IRAT state.
  Format 32Q8
  @endverbatim */ 
  int32                      rssi;

  /*! \brief Refined (Reacq) timing information
  */
  uint16                    ts0StartPosition;

  /*! \brief Return LNA state
  */
  tfw_rx_lna_t lnaInfo;
  /*! \brief Used in IRAT state only to return TFW IRAT state 
  @verbatim
  Valid state variables for rscp response are:
  bufferIndex, hwinitDvgaBias, dvgaBias, lnaOffset
  @endverbatim */
  tfw_irat_state_t          state;
  /*! \brief Return the command Index */
  uint8                     actionIdx;
} tfw_measure_all_rscp_result_t;
/*@{*/
typedef struct {  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief RSCP response */
  tfw_measure_all_rscp_result_t result;
} tfw_measure_all_rscp_rsp_t;   
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR MEASURE_ISCP RESPONSE                         */
  /*-----------------------------------------------------------------------*/
/**
   \addtogroup cmdIscp
   \ingroup cmdMeasurements
   @verbatim
    FW sends the response msg to SW after the last ISCP measurement
    is completed.
    @endverbatim 
*/
typedef struct {

  /*! \brief errStatus
  @verbatim  
  
   If the measurement could not be done due to DSDx conflict or any other 
   reason such as inter freq RSCP measurement & UpPTS transmission, FW will
   inform the SW using the errStatus parameter.
   
  @endverbatim 
  */
  uint8                     errStatus;
  
  /*! \brief RSCP measurement results
  @verbatim
   The index to indicate iscp measurement number matches slotCellCfg index in 
   MEASURE_ISCP Command msg.
   In case ISCP measurement is not performed for some slot due to SW latency, 
   use the errStatus parameter above. 
   Format 16Q8
  @endverbatim */ 
  int16                     iscp[TFW_MAX_NUM_ISCP][TFW_NUM_ANTENNAS];
  /*! \brief Subframe nubmer for ISCP measurement */
  uint16                    subframeNum;
} tfw_measure_iscp_result_t;
/*@{*/
/*! @brief MEASURE_ISCP Response
 *
 *   This is the message definition for MEASURE_ISCP Response
 */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief ISCP response */
  tfw_measure_iscp_result_t iscpRsp;
} tfw_measure_iscp_rsp_t;                      
/*@}*/

#endif /* TFW_SW_INTF_MEASUREMENT_H */
