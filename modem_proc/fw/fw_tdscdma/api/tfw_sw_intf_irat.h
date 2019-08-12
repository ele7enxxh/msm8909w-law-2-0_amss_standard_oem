/*!
  @file
  tfw_sw_intf_irat.h

  @brief
  TD-SCDMA IRAT FW-SW Interface Definitions 

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_tdscdma/api/tfw_sw_intf_irat.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
===========================================================================*/

#ifndef TFW_SW_INTF_IRAT_H
#define TFW_SW_INTF_IRAT_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "tfw_const.h"
#include "tfw_sw_intf_srch.h"
#include "tfw_sw_intf_rx_tx.h"
#include "msgr.h"



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/**
    \defgroup cmdIRAT IRAT Commands 
    */
/*\{*/

/*! @brief Enumerated tech types
 *
 *   This enum lists all possible source techs that T
 *   performs IRAT with (including T)
 */
typedef enum {
  T_IRAT_SRC_TECH_INVALID = 0,
  T_IRAT_SRC_TECH_TDS = 1,
  T_IRAT_SRC_TECH_GERAN,
  T_IRAT_SRC_TECH_LTE,
} tfw_irat_src_tech_e;
/*\}*/

/*\{*/

/*! @brief Gap timing structure
 *
 *   This structure defines a span of time by two parameters -
 *   walltime at its start and
 *   walltime at its end
 */
typedef struct {
  /*! \brief Source Tech
      @verbatim
      Resolution: cx8
      @endverbatim
      **/
  tfw_irat_src_tech_e   srcTech;
  /*! \brief Start time 
      @verbatim
      Resolution: cx8
	Upper 16bits represent sub-frame number and lower 16bits represent CX8 count that wraps around at 0xC800
	Note that this data type may represent walltime or RX time. Refer to comments in structures where this type
	is included
      @endverbatim
      **/
  uint32        startTime;
  /*! \brief End time
      @verbatim
      Resolution: cx8
	Upper 16bits represent sub-frame number and lower 16bits represent CX8 count that wraps around at 0xC800
	Note that this data type may represent walltime or RX time. Refer to comments in structures where this type
	is included
      @endverbatim
      **/
  uint32        endTime;
} tfw_home_rat_gap_timing_t;
/*\}*/
  /*-----------------------------------------------------------------------*/
  /*      DATA STRUCTURE FOR IRAT T2X GAP COMMAND                          */
  /*-----------------------------------------------------------------------*/
typedef struct {
  /*! \brief T2X gap start and cleanup times (RX time)
      @verbatim
      Resolution: cx8
      @endverbatim
      **/
  tfw_home_rat_gap_timing_t   t2xGapCfg;

 /*! \brief Buffer index in common FW SMEM that contains BSP script
  @verbatim
  This script is executed at the beginning of T->L IRAT gap
  @endverbatim
  */
  uint32  rfBspBufIdx;

  /*! \brief Buffer index in common FW SMEM that contains RF cleanup script 
  @verbatim
  This script is executed at the end of an IRAT gap
  @endverbatim
  */
  uint32  rfCleanupBufIdx;
} tfw_irat_t2x_gap_cfg_t;

/*! @brief Gap command structure
 *
 *   This command is sent to FW to inform it of an upcoming T2X measurement
 *   TFW will not schedule HDET or Temp measurements during this gap
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type        hdr;
  /*! \brief IRAT T2X Gap config */
  tfw_irat_t2x_gap_cfg_t  cfg;
} tfw_irat_t2x_gap_cfg_cmd_t;
  /*-----------------------------------------------------------------------*/
  /*      DATA STRUCTURES FOR IRAT_GAP_DETECT COMMAND                       */
  /*-----------------------------------------------------------------------*/
/**
   \defgroup cmdIratGapDetect IRAT_GAP_DETECT Command
   \ingroup cmdIRAT
   @verbatim
   Usage:
     1) This command can only be sent to FW after FW is in IRAT state
     2) One cmd detects gaps using upto 3 LNA states depending on Home RAT's gap width
     3) Each cmd carries with it rx configuration information such as LM and RF buffer indexes
     along with home RAT's gap position
    @endverbatim 
*/

typedef struct {
  /*! \brief Gap detection config */
  tfw_srch_gap_detect_cfg_t   tdsGapDetect;
  /*! \brief Home RAT's Gap position (walltime) */ 
  tfw_home_rat_gap_timing_t   gapTiming;
  /*! \brief Rx Config to use on measuring RAT */
  tfw_rx_config_t             rx;      
} tfw_irat_gap_detect_cfg_t;
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type        hdr;
  /*! \brief IRAT Gap detection config */
  tfw_irat_gap_detect_cfg_t   cfg;
} tfw_irat_gap_detect_cmd_t;
/*@}*/
  /*-----------------------------------------------------------------------*/
  /*  DATA STRUCTURES FOR IRAT_SYNCDL_MIDAMBLE_DETECT COMMAND              */
  /*-----------------------------------------------------------------------*/

/**
   \defgroup cmdIratSyncDetect IRAT_MIDAMBLE_SYNC_DL_DETECT Command
   \ingroup cmdIRAT
   @verbatim
   Usage:
     1) This command can only be sent to FW after FW is in IRAT state
     2) Each cmd carries with it rx configuration information such as LM and RF buffer indexes
     and T gap position along with home RAT's gap position
    @endverbatim 
*/

typedef struct {
  /*! \brief SYNC-DL and Midamble detection config */
  tfw_srch_sync_mid_detect_cfg_t          midambleSyncDl;
  /*! \brief Home RAT's Gap position (walltime) */ 
  tfw_home_rat_gap_timing_t               gapTiming;
  /*! \brief Rx Config to use on measuring RAT */
  tfw_rx_config_t                         rx;      
  /*! \brief TFW IRAT state to be used
  @verbatim
  Valid state variables for rscp cmd are:
  bufferIndex, dvgaBias, lnaOffset
  @endverbatim */
  tfw_irat_state_t                        state;
  /*! \brief Action to be performed */
  tfw_irat_cmd_action_t                   action;
} tfw_irat_syncdl_midamble_detect_cfg_t;
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type                    hdr;
  /*! \brief IRAT Midamble Sync DL Detect config */
  tfw_irat_syncdl_midamble_detect_cfg_t   cfg;
} tfw_irat_syncdl_midamble_detect_cmd_t;
/*@}*/

  /*-----------------------------------------------------------------------*/
  /*      DATA STRUCTURES FOR IRAT_MEASURE_RSCP COMMAND                    */
  /*-----------------------------------------------------------------------*/


/**
   \defgroup cmdIratRscp IRAT_MEASURE_RSCP Command
   \ingroup cmdIRAT
   @verbatim
   Usage:
     1) This command can only be sent to FW after FW is in IRAT state
     2) Each cmd carries with it rx configuration information such as LM and RF buffer indexes
     and TS0 position along with home RAT's gap position
     3) Each cmd returns measurement on all 128 cells
    @endverbatim 
*/

typedef struct {
  /*! \brief Home RAT's Gap position (walltime) */ 
  tfw_home_rat_gap_timing_t       gapTiming;
  /*! \brief Rx Config to use on measuring RAT */
  tfw_rx_config_t                 rx;      
  /*! \brief TFW IRAT state to be used
  @verbatim
  Valid state variables for rscp cmd are:
  bufferIndex, dvgaBias, lnaOffset
  @endverbatim */
  tfw_irat_state_t                state;
  /*! \brief TS0 start position
  @verbatim 
   Resoltuion: cx8 
   Walltime at beginning of midamble
  @endverbatim 
  */ 
  uint16                          ts0StartPosition;
  /*! \brief LNA state to use during measurement */
  uint8                           lnaState;

  /*! \brief Number of cells in JDS */
  uint8                           numJdsCells;
  /*! \brief List of cells in JDS (up to 8) */
  uint8                           jdsCells[TFW_IRAT_MAX_JDS_CELLS];
  /*! \brief Command Index */
  uint8                           actionIdx;
  /*! \brief Action to be performed */
  tfw_irat_cmd_action_t           action;
} tfw_irat_measure_rscp_cfg_t;
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type            hdr;
  /*  \brief IRAT RSCP number of Frequencies to measure. Valid values are 1, 2. */
  uint8                           numCfg;
  /*! \brief IRAT RSCP measure config. Supports 2 concurrent measurements */
  tfw_irat_measure_rscp_cfg_t     cfg[2];
} tfw_irat_measure_rscp_cmd_t;
/*@}*/

  /*-----------------------------------------------------------------------*/
  /*      DATA STRUCTURES FOR IRAT_T2X_CLEANUP COMMAND                    */
  /*-----------------------------------------------------------------------*/


/**
   \defgroup cmdIratT2xCleanup IRAT_T2X_CLEANUP Command
   \ingroup cmdIRAT
   @verbatim
   Usage:
     1) This command can only be sent to FW after FW is in IRAT state
     2) Each cmd carries with it rx configuration information such as RF buffer index
     3) Each cmd returns response indicating cleanup status
    @endverbatim 
*/

typedef struct {
  /*! \brief Cleanup RF buffer index (in IRAT SMEM space) */ 
  uint8 rfRxCleanupBufIndex;
} tfw_irat_t2x_cleanup_cfg_t;
/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type            hdr;
  /*! \brief IRAT T2X Cleanup measure config */
  tfw_irat_t2x_cleanup_cfg_t     cfg;
} tfw_irat_t2x_cleanup_cmd_t;
/*@}*/

  /*-----------------------------------------------------------------------*/
  /*      DATA STRUCTURES FOR IRAT_T2X_CLEANUP RESPONSE                    */
  /*-----------------------------------------------------------------------*/


/**
   \defgroup cmdIratT2xCleanup IRAT_T2X_CLEANUP Response
   \ingroup cmdIRAT
   @verbatim
   Usage:
     1) This resposne is sent after processing IRAT_T2X_CLEANUP_CMD in IRAT state
     2) Each response returns a cleanup status
    @endverbatim 
*/
typedef enum {
  TFW_IRAT_T2X_CLEANUP_NONE,
  TFW_IRAT_T2X_CLEANUP_SUCCESS,
  TFW_IRAT_T2X_CLEANUP_FAIL
}tfw_irat_t2x_cleanup_attribute_e;

typedef struct {
  /*! \brief Cleanup status */ 
  tfw_irat_t2x_cleanup_attribute_e attribute;
}tfw_irat_t2x_cleanup_result_t;

typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief T2X Cleanup response */
  tfw_irat_t2x_cleanup_result_t result;
} tfw_irat_t2x_cleanup_rsp_t;
#endif /* TFW_SW_INTF_IRAT_H */
