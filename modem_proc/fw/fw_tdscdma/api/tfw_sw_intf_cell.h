/*!
  @file
  tfw_sw_intf_cell.h

  @brief
  TD-SCDMA Cell FW-SW Interface Definitions 

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_tdscdma/api/tfw_sw_intf_cell.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/28/14    SL     Merge FTL changes.  
===========================================================================*/

#ifndef TFW_SW_INTF_CELL_H
#define TFW_SW_INTF_CELL_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "tfw_sw_intf_common.h"
#include "tfw_sw_intf_measurement.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/**
    \defgroup cmdCell Cell Configuration Commands & Responses
    */
/*\{*/

  /*-----------------------------------------------------------------------*/
  /*     CONSTANTS                                                         */
  /*-----------------------------------------------------------------------*/
/*! \brief Max JDCS cells (not including serving cell) */ 
#define TFW_MAX_JDCS_CELLS                7
/*! \brief Max TS0 working frequency JDCS cells (including serving cell) */ 
#define TFW_MAX_TS0_WFREQ_JDCS_CELLS      8
/*! \brief Max TS-RSP measurement slots per subframe */ 
#define TFW_MAX_NUM_TS_RSP_SLOTS          5

#define TFW_MAX_TS0_TS_RSP_CELLS          40
#define TFW_MAX_NONTS0_TS_RSP_CELLS       8  

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR CELL_SERVING_JDCS COMMAND                     */
  /*-----------------------------------------------------------------------*/

/*\}*/


/**
   \defgroup cmdCellServingJdcs CELL_SERVING_JDCS Command
   \ingroup cmdCell
   @verbatim
   Usage:
     1) This command can only be sent to FW after FW is in Tracking state
   @endverbatim 
*/
/*@{*/
typedef struct {
  /*! \brief Primary frequency (Hz)
  @verbatim 
   Used by FW Frequency Tracking Loop (FTL) 
  @endverbatim 
  */ 
  uint32                    primaryFreq;
  /*! \brief Previous timing measurement result (cx8)    
  @verbatim 
   1) Absolute wall time offset of the subframe boundary (cx8) 
   2) If its not available when serving cell changes (not init acq), the 
      same ts0StartPosition as the old serving cell shall be provided. 
   3) The new serving cell should already in JDCS before changing to it, so 
      ts0StartPosition should be availabe because FW does TTL for all JDCS
      cells. FW sends JDCS timing and freq info to SW every 5ms in 
      MEASURE_UNSCHEDULED_STATS indication msg. Additionally, whenever FW 
      measurements RSCP for a frequency, FW does FW for top 8 cells for each 
      freq. So it's unlikely the timing information is not available when 
      serving cells changes.
   4) During init acq, FW returns top four cells with freq and timing 
      information after midamble detection. SW should keep those information
      in case cell re-selection happens after init acq.
   5) Ranges from 0 to 6400*8-1
  @endverbatim 
  */ 
  uint16                    ts0StartPosition;                                            
  /*! \brief Freq offset (unit: Hz) 
  @verbatim 
   If its not available when serving cell changes (not init acq), the same
   freqOffset as the old serving cell (TBD: scaling needed if freq offset 
   unit is Hz) shall be provided. 
  @endverbatim 
  */
  int16                     freqOffset;
  /*! \brief Cells midamble index */
  uint8                     midambleIdx;
} tfw_serving_cell_cfg_t;

typedef struct {
  /*! \brief Cells midamble index */
  uint8                     midambleIdx;
} tfw_ts0_wfreq_jdcs_cfg_t;

typedef struct {
  /*! \brief RSCP measurement results
  @verbatim
   From MEASURE_RSCP RESPONSE msg, averaged over two antennas, Q8
  @endverbatim */ 
  int16                     rscp;
  /*! \brief Cells midamble index */
  uint8                     midambleIdx;
} tfw_jdcs_cfg_t;

typedef struct {
  /*! \brief serving cell change flag 
  @verbatim 
   0: serving cell un-changed 
   1: serving cell changed 
   Notes: 
   1) When set to 1, FW checks servingCell field. 
   2) When serving cell changes, FW picks up the new serving cell 
      configuration 400us before the subframe boundary. 
  @endverbatim 
  */
  uint32                    newServingCellFlag : 1;
  /*! \brief JDCS cell change flag 
  @verbatim 
   0: JDCS cells un-changed 
   1: JDCS cells changed 
   Notes: 
   1) When set to 1, FW checks newJdcsFlag and jdcs fields. 
   2) TFW_MAX_JDCS_CELLS = 7
   3) Serving cell should not be in JDCS cells. 
   4) When JDCS cells changed without serving cell change, FW picks up the 
      new JDCS cells in TS1.
   5) When JDCS cells changed with serving cell change, FW picks up the 
      new JDCS cells 400us before the subframe boundary.
   6) When serving cell changes, SW must remove the new serving cell from 
      JDCS if it was in it.
   7) During init acq, FW returns top four cells after midamble detection. 
      SW can use the top cell as the serving cell and SW shall config the
      rest of three cells as JDCS cells.
   8) FW picks the top four cells in TS0 JDCS cells as TSO JDS cells.
  @endverbatim 
  */
  uint32                    newJdcsFlag : 1;
  /*! \brief TS0 working frequency JDS cells change flag
  @verbatim 
   0: TS0 working frequency JDS cells un-changed 
   1: TS0 working frequency JDS cells changed 
   Notes: 
   1) When set to 1, FW checks numTs0WFreqJds and ts0WFreqJds fields. 
   2) TFW_MAX_TS0_WFREQ_JDCS_CELLS = 8 (serving cell included) 
   3) When working frequency change (newTs0WFreqJdcsFlag set to 1), TS0 
      working frequency JDCS cells must be updated.
   4) When TS0 working frequency JDCS cells changed without serving cell 
      change, FW picks up the new JDCS cells in TS1.
   5) When TS0 working frequency JDCS cells changed without serving cell 
      change, FW picks up the new JDCS cells 400us before the subframe
      boundary.
   6) When serving cell changes, SW must remove the new serving cell from 
      JDCS if it was in it.
   7) During init acq, FW returns top four cells after midamble detection. 
      Before S1/S2 detection or CPCCH decoding, SW shall config all four
      cells as TS0 working frequency JDCS cells.  
  @endverbatim 
  */
  uint32                    newTs0WFreqJdcsFlag : 1;
  /*! \brief serving cell working frequency flag 
  @verbatim 
   0: serving cell working frequency un-changed 
   1: serving cell working frequency changed 
   Notes: 
   1) When set to 1, FW checks servingWFreq field. 
   2) When serving cell working frequency changes, FW picks up the new 
      working frequency 400us before the subframe boundary.
   3) Working frequency can only be changed once every subframe.  
   4) Working frequency != primary freq can only happen in CELL_DCH. 
   5) In CELL_DCH, TS0 channel decoding happens on working frequency only. 
   6) When transition from some state (ex, CELL_FACH) to CELL_DCH, SW must 
      re-configure the working freq and TS0-JDS if (working frequency !=
      primary freq).
  @endverbatim 
  */
  uint32                    newServingWFreqFlag : 1;
  /*! \brief number of JDCS cells 
  @verbatim  
   Checked when newJdcsFlag set to 1
  @endverbatim 
  */
  uint32                    numJdcsCells : 3;
  uint32                    :1;
  /*! \brief number of TS0 working frequency JDCS cells 
  @verbatim  
   Checked when newTs0WFreqJdcsFlag set to 1
  @endverbatim 
  */
  uint32                    numTs0WFreqJdcs : 4;
  uint32                    :20;
  /*! \brief serving cell configuration  
  @verbatim 
   Checked when newServingCellFlag set to 1
  @endverbatim 
  */
  tfw_serving_cell_cfg_t    servingCell;
  /*! \brief JDCS cells configuration  
  @verbatim 
   Checked when newJdcsFlag set to 1
  @endverbatim 
  */
  tfw_jdcs_cfg_t            jdcs[TFW_MAX_JDCS_CELLS];
  /*! \brief TS0 working frequency JDS cells configuration 
  @verbatim 
   Checked when newTs0WFreqJdsFlag set to 1
  @endverbatim 
  */
  tfw_ts0_wfreq_jdcs_cfg_t  ts0WFreqJdcs[TFW_MAX_TS0_WFREQ_JDCS_CELLS];
  /*! \brief serving cell working frequency 
  @verbatim 
   Checked when newServingWFreqFlag set to 1
  @endverbatim 
  */
  uint32                    servingWFreq;
} tfw_cell_serving_jdcs_cfg_t;

/*! @brief CELL_SERVING_JDCS Command
 *
 *   This is the message definition for CELL_SERVING_JDCS Command
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type               hdr;
  /*! \brief Serving cell, jdcs, working frequency TS0-JDS config */
  tfw_cell_serving_jdcs_cfg_t        cellCfg;
} tfw_cell_serving_jdcs_cmd_t;             
/*@}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR CELL_INFO INDICATION                          */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup indUnscheduled CELL_INFO Indication
   \ingroup cmdCell
   @verbatim
   FW sends this indication message to SW every TS0 for stats of last
   subframe.
   @endverbatim 
*/
/*@{*/
typedef struct {
  /*! \brief wallTimeOfs(28:16) = subframe offset 
             wallTimeOfs(15:0) = ts0StartPosition (in units of Cx8)      */
  uint32                    wallTimeOfs;
  /*! \brief Freq offset (unit: Q8 Hz), estimated in cell's primary freq) */
  int32                     freqOffset;
  /*! \brief Dwpts filted Freq offset (unit: Q8 Hz) */
  int32                     dwptsFiltFreqOffset;
  /*! \brief midamble filted Freq offset (unit: Q8 Hz) */
  int32                     midFiltFreqOffset;
  /*! \brief Previous timing measurement result (cx8)    
  @verbatim 
   Absolute wall time offset of the subframe boundary (cx8) 
  @endverbatim 
  */ 
  uint16                    ts0StartPosition;                                            
  /*! \brief Cells midamble index */
  uint8                     midambleIdx;
} tfw_cell_freq_timing_t;

typedef struct {

  /*! \brief errorCode
  @verbatim  
  
   If the measurement could not be done due to DSDx conflict or any other 
   reason such as inter freq RSCP measurement & UpPTS transmission, FW will
   inform the SW using the errorCode parameter.
   
  @endverbatim 
  */
  uint8                     errStatus; 
  
  /*! \brief Rx Subframe nubmer for unscheduled measurement */
  uint16                    subframeNum;
  /*! \brief Serving cell timing, freq, midamble idx */
  tfw_cell_freq_timing_t    servingCell;
  /*! \brief number of JDCS cells */
  uint8                     numJdcsCells;
  /*! \brief JDCS cells timing, freq, midamble idx */
  tfw_cell_freq_timing_t    jdcs[TFW_MAX_JDCS_CELLS];
  /*! \brief TS-RSP measurement result  
  @verbatim
   tsRsp[n]: for slot n+2
   If the slot (n+2) is not active for DL, tsRsp[n]=0 (or TBD number
   which is not possible for TS-RSP).
  @endverbatim
  */
  uint16                    tsRsp[TFW_MAX_NUM_TS_RSP_SLOTS];
  /* Antenna selection for lanInfo reports */
  tfw_ant_sel_e     antSel;
  /*LNA state info*/
  tfw_rx_lna_t lnaInfo;
} tfw_cell_info_t;

/*! @brief CELL_INFO Indication
 *
 *   This is the message definition for CELL_INFO Indication
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type             hdr;
  /*! \brief Unscheduled measurement config */
  tfw_cell_info_t                  cellInfoInd;
} tfw_cell_info_ind_t;                     

typedef struct {
  /*! \brief Rx Subframe nubmer for this confiuration to take into effect */
  uint16                    subframeNum;
  /*! \brief Cells midamble index */
  uint32                     midambleIdx;
  /*! \brief wallTimeOfs(28:16) = subframe offset 
             wallTimeOfs(15:0) = ts0StartPosition (in units of Cx8) */
  uint32                     wallTimeOfs;
} tfw_serving_cell_updt_t;

/*! @brief SERVING_CELL_UPDT Command
 *
 *   This is the message definition for SERVING_CELL_UPDT Command
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief serving cell update */
  tfw_serving_cell_updt_t    servingCellUpdt;
} tfw_serving_cell_updt_cmd_t;             

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
  uint32                    wallTimeOffset;
  /*! \brief Cells midamble index */
  uint8                     midambleIdx;
} tfw_rscp_cell_info_t;

typedef tfw_rscp_cell_info_t tfw_demod_cell_info_t;

typedef struct
{
  /*! \brief Rx Subframe nubmer for unscheduled measurement */
    uint16                    subframeNum;
   /*! \brief message sequence number */ 
    uint16                    msgId;
  /*! \brief total number of cells for TS0 operation
  */
    uint32                    numCells : 6;      
  /*! \brief total number of cells for for iterative cancellation in TS0 
  @verbatim 
   1-4: if it is for demodualtion
   1-8: if it is for measurement 
  @endverbatim 
  */
    uint32                    numItrvCells : 4;
  /*! \brief total number of cells for for ISCP measurement in TS0
  */
    uint32                    numIscpCells: 3;

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
  /*! \brief data aided FTL on/off flag when there is no demod channel on TS0 
             When demod channel enabled on TS0, this flag is useless
  @verbatim 
   0: data aided FTL off, however FW will decide if running data aided FTL for 
                          TS0 based on demod on/off
   1: data aided FTL on,  meaning FW will run demod and data aided FTL no matter 
                          demod channel on/off for TS0
                          if demod channel is on, both 1st and 2nd data bursts 
                                                  will be used in data aided FTL
                          if demod channel is off,both 1st and 2nd or 
                                                  only 2nd data bursts will be used 
                                                  in data aided FTL 
  @endverbatim 
  */
  uint32                    dataAidedFTLOn : 1;

  /*! \brief RF script buffer index for inter freq tuning
  @verbatim 
   If interFreq is 0, ignored
  @endverbatim 
  */
  uint32                    rfRxFreqBufIndex : 8;
  /*! \brief RF script buffer index for inter freq tuner update and cleanup
  @verbatim 
   If interFreq is 0, ignored
  @endverbatim 
  */
  uint32             rfInterFreqTunerBufIndex : 1;
  /*! \brief reserved */
  uint32                                      : 7;
  /*! \brief Antenna selection */
  tfw_ant_sel_e antSel;

  /*! \brief Index to RXLM static settings
  @verbatim 
   If interFreq is 0, ignored
  @endverbatim 
  */
  uint8                    rxlmStaticBufIndex;
  /*! \brief Index to RXLM static settings
  @verbatim 
   If interFreq is 0, ignored
  @endverbatim 
  */
  uint8                    rxlmStaticBufIndexDiv;
  /*! \brief measured frequency (Hz)
  @verbatim 
   Used by FW for Frequency Tracking Loop (FTL)
  @endverbatim 
  */
  uint32                    measuredFreq;
  /*! \brief Cells config for RSCP measurement 
  @verbatim 
   1) If numCells>8, cellCfg[0~7] has timing measurement.
   2) configure wallTimeOffset if it is available for this particulr cell, 
      otherwise 0.
   3) If (measured freq==serving cell primary freq), 
      a. The first cell (cellCfg[0]) must be the serving cell
      b. The next are the rest of TS0 interative cells for that freq
      c. The next are the rest cells for timing measurement
      c. The last cells are ISCP cells 
  @endverbatim 
  */
  tfw_rscp_cell_info_t   cellCfg[TFW_MAX_RSCP_CELLS];

  /*! \brief Flag to control if FW needs to apply the LNA for measurement
  @verbatim 
   Flag to control if FW needs to apply the LNA for measurement, SW only set this flag to be true during DRX for inter-f measurement
  @endverbatim 
  */ 
  boolean isLnaIfreqValid; 

  /*! \brief LNA information for inter-f measurement 
  @verbatim 
   LNA information from SW for inter-f measurement during DRX, it is ignored if isLnaIfreqValid is false
  @endverbatim 
  */ 
  tfw_rx_lna_t lnaIfreq;

  /*! \brief TFW uses desense Id for register activities with
     the coexistence manager, used only in DSDA, otherwise set to 0
  */
  uint32  cxmFreqId;
  
  /*! \brief TFW check sfm according to sfmToCheckFlag
       True: check
       False: not check
  */
  boolean sfmToCheckFlag;

}tfw_ts0_cell_updt_t;

/*! @brief TS0_CELL_UPDT Command
 *
 *   This is the message definition for TS0_CELL_UPDT Command
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief ts0 cell update */
  tfw_ts0_cell_updt_t    ts0CellUpdt;
} tfw_ts0_cell_updt_cmd_t;             

typedef struct
{
  /*! \brief TS-RSP value of a particulr cell
  @verbatim
   Format 16Q8
  @endverbatim 
  */
  int16  tsRsp;
  /*! \brief RSCP value of a particulr cell
  @verbatim
   Format 16Q8
  @endverbatim 
  */
  int16  rscp;
}tfw_cell_rscp_report_t;

typedef struct
{
  /*! \brief Cells midamble index */
  int8  midambleIdx;
  /*! \brief ISCP value of a particulr cell
  @verbatim
   Format 16Q8
  @endverbatim 
  */
  int16  iscp;
}tfw_ts0_cell_iscp_report_t;

typedef struct
{

    /*! \brief Rx Subframe nubmer for unscheduled measurement */
    uint16                    subframeNum;
    /*! \brief message sequence number, should be the same as msgId in its corresponding CMD*/ 
    uint16                    msgId;
  /*! \brief total number of cells for TS0 operation
  */
    uint16                    numCells : 6;      
  /*! \brief total number of cells for for iterative cancellation in TS0 
  @verbatim 
   1-4: if it is for demodualtion
   1-8: if it is for measurement 
  @endverbatim 
  */
    uint16                    numItrvCells : 4;
  /*! \brief total number of cells for for ISCP measurement in TS0
  */
    uint16                    numIscpCells: 3;

    uint16                    reserved: 3;    

    uint32                    measuredFreq;
  /*! \brief RSSI, per antenna from DwPTS
  @verbatim
   RSSI is also measured when TS0 is inter-frequency and upPTS is present 
   (because dual PLL is employed). 
   Format 16Q8
  @endverbatim 
  */
    int16                     rssi[TFW_NUM_ANTENNAS];
  /*! \brief RSSI, per antenna from TS0 midamble
  @verbatim
   Format 16Q8
  @endverbatim 
  */
    int16                     midRssi[TFW_NUM_ANTENNAS];

  /*! \brief metric M, per OTD cell, for purpose of fake cell pruning 
  @verbatim
   M = |CIR_mid_1stShift*conj(CIR_dwpts)|^2/(|CIR_mid_1stShift|^2*|CIR_dwpts|^2)
   Format 16Q16
  @endverbatim 
  */
    uint16                     metricM[TFW_MAX_OTD_CELLS];
  /*! \brief metric M prime, per OTD cell, reserved 
  @verbatim   
   M' = |CIR_mid_1stShift*conj(CIR_dwpts)|^2/(|CIR_mid|^2*|CIR_dwpts|^2)
   Format 16Q16
  @endverbatim 
  */
    uint16                     metricMPrime[TFW_MAX_OTD_CELLS];

  /*! \brief TS-RSP and RSCP of all configured cells
  @verbatim
   SW and FW should assume that cells are arranged in the same order as in its 
   corresponding cmd message 
  @endverbatim 
  */
    tfw_cell_rscp_report_t   cellRsp[TFW_MAX_TS0_TS_RSP_CELLS];  
  /*! \brief ISCP of all configured cells
  @verbatim
   SW and FW should assume that cells are arranged in the same order as in its 
   corresponding cmd message 
  @endverbatim 
  */
    tfw_ts0_cell_iscp_report_t   cellIscp[TFW_MAX_NUM_ISCP][TFW_NUM_ANTENNAS]; 

  /*! \brief wallTimeOfs(28:16) = subframe offset 
             wallTimeOfs(15:0) = ts0StartPosition (in units of Cx8) */
    uint32                        wallTimeOfs[TFW_MAX_OTD_CELLS];
  /*! \brief Antenna selection for lanInfo reports */
  tfw_ant_sel_e     antSel;
  /*! \brief initial lna info */
  tfw_rx_lna_t lnaInfo;
  
  /*! \brief noise variance computed from DWPTS_GP in dBm (format 16Q8)*/             
  int16                     noiseVar;
  /*! \brief serving cell RSCP computed from RX1 in dBm (format 16Q8)
   If RXD is not enabled. It is set to 0x8000 (-128 dBm)*/
  int16                     servCellRscpRx1;
  /*flag to indicate if this servCellRscpRx1 is valid or not*/
  boolean                   bServCellRscpRx1Valid;
  /*! \brief sir computed from antenna0(primary antenna)shift0(RSCP-n0EstPreCond) 
   in dB (format 16Q8)*/             
  int16                     prxShift0SirdB16Q8;
  /*! \brief errorCode
  @verbatim  
  
   If the measurement could not be done due to DSDx conflict or any other 
   reason such as inter freq RSCP measurement & UpPTS transmission, FW will
   inform the SW using the errorCode parameter.
   
  @endverbatim 
  */
    uint8                     errStatus;  
    
} tfw_ts0_cell_report_t;

/*! @brief TS0_CELL_RSP Command
 *
 *   This is the message definition for TS0_CELL_RSP Command
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief ts0 cell measurement results */
  tfw_ts0_cell_report_t    ts0CellRpt;
} tfw_ts0_cell_rsp_t;             

typedef struct
{

   /*! \brief Rx Subframe nubmer for unscheduled measurement */
    uint16                    subframeNum;
   /*! \brief message sequence number*/ 
    uint16                    msgId;
  /*! \brief total number of cells for non-TS0 operation, up to 8
  */
    uint16                    numCells : 6;      
  /*! \brief total number of cells for for iterative cancellation in non-TS0 
  @verbatim 
   1-4: if it is for demodualtion
   1-8: if it is for measurement (ISCP)
  @endverbatim 
  */
    uint16                    numItrvCells : 4;
   /*! \brief total number of cells for for ISCP measurement in non-TS0
   */
    uint16                    numIscpCells: 3;
    uint16                                : 3;
   /*! \brief Cell midamble index for demodulation, the first one must be serving cell*/
    tfw_demod_cell_info_t     demodCellCfg[TFW_MAX_NONTS0_TS_RSP_CELLS];

  /*! \brief ISCP measurement for nonTs0 slots & their respective midamble ID 
  @verbatim 
   slotCellCfg[n] has the lower/same slotIdx than slotCellCfg[n+1] 
  @endverbatim 
  */
    tfw_measure_iscp_slot_cell_t iscpCellCfg[TFW_MAX_NUM_ISCP];

}tfw_nonts0_cell_updt_t;

/*! @brief NONTS0_CELL_UPDT Command
 *
 *   This is the message definition for NONTS0_CELL_UPDT Command
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief nonts0 cell update */
  tfw_nonts0_cell_updt_t    nonts0CellUpdt;
} tfw_nonts0_cell_updt_cmd_t;             

typedef struct
{
    
    /*! \brief Rx Subframe nubmer for unscheduled measurement */
    uint16                    subframeNum;
    /*! \brief message sequence number, should be the same as msgId in its corresponding CMD*/ 
    uint16                    msgId;
    /*! \brief total number of cells for non-TS0 operation
     */
    uint16                    numCells : 6;      
    /*! \brief total number of cells for for iterative cancellation in non-TS0 
  @verbatim 
   1-4: if it is for demodualtion
   1-8: if it is for measurement (ISCP)
  @endverbatim 
  */
    uint16                    numItrvCells : 4;
  /*! \brief total number of cells for for ISCP measurement in TS0
  */
    uint16                    numIscpCells: 3;
    
    uint16                    reserved: 3;    
  /*! \brief errorCode
  @verbatim  
  
   If the measurement could not be done due to DSDx conflict or any other 
   reason such as inter freq RSCP measurement & UpPTS transmission, FW will
   inform the SW using the errorCode parameter.
   
  @endverbatim 
  */
    uint8                     errStatus;
    
    /* Bit field 0->Slot 2, 1->3, 2->4, 3->5, 4->6 */
    uint8                     cxmLostBitMask;    
   /*! \brief filtered TS RSP value of the requested cells
   @verbatim
   Format 16Q8
   @endverbatim 
   */
    int16                     filtTsRsp[TFW_MAX_NONTS0_TS_RSP_CELLS + TFW_MAX_NUM_ISCP];  
   /*! \brief nonTS0 Metric F value of the requested cells 
   @verbatim
   Format 16Q8 
   @endverbatim */
    uint16                    nonTs0MetricF[TFW_MAX_NONTS0_TS_RSP_CELLS];
  /*! \brief ISCP measurement results
  @verbatim
   The index to indicate iscp measurement number matches slotCellCfg index in 
   MEASURE_ISCP Command msg.
   Format 16Q8
  @endverbatim */ 
    tfw_ts0_cell_iscp_report_t cellIscp[TFW_MAX_NUM_ISCP][TFW_NUM_ANTENNAS];

  /*! \brief wallTimeOfs of current serving cell
  @verbatim
     wallTimeOfs(28:16) = subframe offset 
     wallTimeOfs(15:0) = ts0StartPosition (in units of Cx8)
  @endverbatim */
  uint32                    currTimeOffset;
  /*! \brief current Freq offset (unit: Q8 Hz) */
  int32                     currFreqOffset;

  /*! \brief noise variance for each slot in dBm (format 16Q8)
     index 0-> slot 2, 1 -> 3, 2->4, 3->5, 4->6*/             
  int16                     noiseVar[5];
  /*! \brief midamble RSSI for each slot in dBm (format 16Q8)
     index 0-> slot 2, 1 -> 3, 2->4, 3->5, 4->6*/             
  int16                     rssi[5];
  /*! \brief serving cell tsRsp for each slot in dBm (format 16Q8)
     index 0-> slot 2, 1 -> 3, 2->4, 3->5, 4->6*/  
  int16                     servingCelltsRsp[5];

} tfw_nonts0_cell_report_t;


/*! @brief NONTS0_CELL_RSP Command
 *
 *   This is the message definition for NONTS0_CELL_RSP Command
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief nonts0 cell measurement results */
  tfw_nonts0_cell_report_t    nonts0CellRpt;
} tfw_nonts0_cell_rsp_t;         
/*@}*/    
#endif /* TFW_SW_INTF_CELL_H */
