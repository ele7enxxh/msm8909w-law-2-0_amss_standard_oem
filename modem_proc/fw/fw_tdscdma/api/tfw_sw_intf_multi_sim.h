/*!
  @file
  tfw_sw_intf_cxm.h

  @brief
  TD-SCDMA common FW-SW Interface Definitions 

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_tdscdma/api/tfw_sw_intf_multi_sim.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
===========================================================================*/

#ifndef TFW_SW_INTF_MULTI_SIM_H
#define TFW_SW_INTF_MULTI_SIM_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "tfw_const.h"
#include "msgr.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/**
    \defgroup cmdCommon Common Configuration Commands & Responses
    */
/*\{*/

  /*-----------------------------------------------------------------------*/
  /*     CONSTANTS                                                         */
  /*-----------------------------------------------------------------------*/

/* Interface provides passing parameters for 7 Regular + 2 Special slots   */
#define TFW_MULTI_SIM_SLOT_MAX                      9

#define TFW_MULTI_SIM_DWPTS_LOC                     7
#define TFW_MULTI_SIM_UPPTS_LOC                     8

#define TFW_MULTI_SIM_SFN_IMMEDIATE            0xFFFF

/* This is the max time from current Sfn multi sim message can be sent*/
#define TFW_MULTI_SIM_MAX_ACTION_SFN_LOOKAHEAD      8


/*! @brief Enumerated Gap Command types
 *
 *  TL1 will use this types to inform TFW whether to start the GAP or
 *  abort the current gap
 *
 */
typedef enum 
{
  TFW_DSDS_GAP_START = 1,
  TFW_DSDS_GAP_ABORT 
} tfw_dsds_gap_cmd_e;


/*! @brief Enumerated SIM Mode types
 *
 *  This enum lists all possible SIM Modes
 */
typedef enum 
{
  TFW_NON_COEX_MODE = 0,
  TFW_SINGLE_SIM_MODE = TFW_NON_COEX_MODE,
  TFW_DSDS_COEX_MODE,
  TFW_MULTI_SIM_MODE_DSDS = TFW_DSDS_COEX_MODE,
  TFW_COMMON_COEX_MODE,
  TFW_MULTI_SIM_MODE_DSDA = TFW_COMMON_COEX_MODE,
  /*! \brief This is the multi SIM mode of TFW in the TFW_STATE_STANDBY or
     TFW_STATE_INACTIVE states. 
     TL1 will configure a different multi SIM mode for changing TFW to a 
     different state. 
     IF TL1 places TFW to TFW_STATE_STANDBY or TFW_STATE_INACTIVE state
     TL1 sets state of Multi SIM Mode to TFW_SIM_MODE_NOT_CONFIGURED
  */
  TFW_SIM_MODE_NOT_CONFIGURED
} tfw_sim_mode_e;
/*\}*/

/*! @brief Enumerated CXM Update Timing types
 *
 */
typedef enum 
{
  /*! \brief TFW will start using CXM parameters immediately. This is needed if
     TFW is in TFW_STATE_ACQUISITION or TFW_STATE_IRAT state
     and CXM parameters should be used immediately.
  */
  TFW_CXM_UPDATE_IMMEDIATE =0,
  /*! \brief TFW will start using CXM parameters for registering activities
     for the next subframe. If this is selected, CXM message should be 
     received by TFW before the schedule point of TS3 of the current 
     subframe. This must be used if TL1 places TFW to tracking or track_
     idle states from standby state.
  */  
  TFW_CXM_UPDATE_NEXT_SUBFRAME
  
} tfw_cxm_update_timing_e;

typedef enum
{ 
  TFW_CXM_SLOT_TYPE_INVALID, 
  TFW_CXM_SLOT_TYPE_VOICE_CALL,
  TFW_CXM_SLOT_TYPE_PS_CALL,
  TFW_CXM_SLOT_TYPE_MRAB_CALL,

} tfw_cxm_slot_call_type_e;

typedef struct 
{
  /*! \brief TFW uses this priority to register activities with 
     the coexistence manager  
  */  
  uint32  cxmPriority;

  /*! \brief TFW uses desense Id for register activities with
     the coexistence manager  
  */
  uint32  cxmFreqId;  

  /*! \brief TFW uses slot call type to decide to enable 'Tx blank' or disable it
  */
  tfw_cxm_slot_call_type_e cxmCallType;


} tfw_cxm_parameters_t;

/**
   \defgroup cmdMultiSimConfig MULTI_SIM_CONFIG Command
   \ingroup cmdMultiSim
   @verbatim
   Usage:
          
    @endverbatim 
*/

typedef struct 
{  
  tfw_sim_mode_e mode;

  /*! \brief This is the indication of subframe in rxTime the MULTI_SIM_CONFIG message will be 
     valid. This parameter will be strictly used for debugging and error checking.
     If timing parameter is set to TFW_CXM_UPDATE_IMMEDIATE then pls set 
     this to the beginning of the current subframe if the current subframe information
     is available to TL1. If no subframe information is available,  pls. set it to
     0xFFFFFFFF e.g. in acquisition state.
     If CXM_UPDATE_NEXT_SUBFRAME is selected in the timing paramter, set this to 
     the start of next subframe
     Resolution: cx8
  */
  uint32 actionTime;

  /*! \brief This is the indication of subframe number in rxTime the MULTI_SIM_CONFIG
     message will be latched by the FW. For TFW updating CXM parameters immediately,
     pls. set it to 0xFFFF using the macro TFW_MULTI_SIM_SFN_IMMEDIATE define above.
  */
  uint16 actionSfn;

  /*! \brief flag to swap priority every frame(10 ms), for cell-DCH state, to protect DL
      TFCI decoding, need to use a high-high-low-low approach to protect whole TFCI
      and not block GSM activity. Refer to TFW design doc for details. 
      TRUE:  swap priority every 10ms ( For sfm_no % 4 = 0,1 params will be used )
                                      ( For sfm_no % 4 = 2,3 paramsSwap will be used )
      FALSE: do not swap priority     ( TFW will only use param buffer for all subframes)
  */
  boolean swapPriority;
  uint32                  :31;

  /*! \brief  Timing info for TFW to use the contents of this message*/
  tfw_cxm_update_timing_e timing;
  
  /*! \brief For TFW states such as acquisition and IRAT, TFW will use the first 
     element of the array for registration as no slot timings have been established 
     yet. If needed, TL1 can set all array elements to the same values for 
     Acqisition or IRAT states.
     Special slot dwpts uses array no 7 and Uppts uses array no 8. Regular slots use
     locations 0-7
   */

  tfw_cxm_parameters_t params[TFW_MULTI_SIM_SLOT_MAX];

  tfw_cxm_parameters_t paramsSwap[TFW_MULTI_SIM_SLOT_MAX];

} tfw_multi_sim_settings_t;



/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type          hdr;
  /*! \brief Body of the message */
  tfw_multi_sim_settings_t      multiSimCfg;
  
} tfw_multi_sim_config_cmd_t;
/*@}*/



/*@}*/

  /*-----------------------------------------------------------------------*/
  /*      DATA STRUCTURES FOR DSDS_QTA_GAP COMMAND                         */
  /*-----------------------------------------------------------------------*/


/**
   \defgroup cmdmultiSimdsdsQtaGap MULTI_SIM_DSDS_QTA_GAP Command
   \ingroup cmdMultiSim
   @verbatim
   Usage:
     1) This command can only be sent to FW while FW is in DSDS mode.
     2) Each time TFW receives this message, it will start using CXM. It will 
        use CXM until it sends back the DSDS_CLEANUP indication to TL1. TL1 will 
        send this message for every QTA gap.
     3) If TFW receives this message in any other mode than DSDS, it will assert.
          
    @endverbatim 
*/

typedef struct 
{
  tfw_dsds_gap_cmd_e gapCmd;
  /*! \brief Cleanup RF buffer index (in SMEM space) */ 
  uint8 rfRxCleanupBufIndex;
} tfw_multi_sim_dsds_qta_gap_cfg_t;

/*@{*/
typedef struct 
{
  /*! \brief Message header */
  msgr_hdr_struct_type               hdr;
  /*! \brief IRAT T2X Cleanup measure config */
  tfw_multi_sim_dsds_qta_gap_cfg_t   dsdsGapCfg;
} tfw_multi_sim_dsds_qta_gap_cmd_t;
/*@}*/

/*! @brief DSDS_CLEANUP Indication
 *
 *   This is the message definition for DSDS_CLEANUP Indication
 *   sent to TL1 at the end of QTA gap.
 */

/*@{*/ 
typedef struct 
{

  /*! \brief Start time 
      @verbatim
      Resolution: cx8
	Upper 16bits represent sub-frame number and lower 16bits represent CX8 count that wraps around at 0xC800
	Note that this data type may represent walltime or RX time. Refer to comments in structures where this type
	is included
      @endverbatim
      **/
  uint32        cxmLockLostTime;
  /*! \brief End time
      @verbatim
      Resolution: cx8
	Upper 16bits represent sub-frame number and lower 16bits represent CX8 count that wraps around at 0xC800
	Note that this data type may represent walltime or RX time. Refer to comments in structures where this type
	is included
      @endverbatim
      **/
  uint32        cxmLockAcqTime;
 


} tfw_multi_sim_cxm_lock_lost_timings_t;


/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type     hdr;
  /*! \brief Body of the message */
  tfw_multi_sim_cxm_lock_lost_timings_t  cxmLostTimingsInd;

} tfw_dsds_cleanup_ind_t;
/*@}*/

/**
   \defgroup WLAN COEX POLICY Command
   \ingroup 
   @verbatim
   Usage:
          
    @endverbatim 
*/

/*@{*/
typedef struct 
{
  /*! \brief Message header */
  msgr_hdr_struct_type                hdr;
  /*! \brief WLAN Coex Policy */
  cxm_active_config_v01_s   wlanCoexPolicy;

} tfw_wlan_coex_policy_cmd_t;
/*@}*/

/**
   \defgroup WLAN COEX METRIC CONTROL Command
   \ingroup 
   @verbatim
   Usage:
          
    @endverbatim 
*/

/*! @brief */
typedef enum
{
  TFW_CXM_ACTION_UNINIT,
  TFW_CXM_ACTION_START,
  TFW_CXM_ACTION_STOP,
}tfw_wlan_coex_action_e;

typedef struct 
{  
  tfw_wlan_coex_action_e        wlanMetricAction;  /*!< whether to START/STOP metric collection */
  uint32                               wlanAlpha;  /*!< Alpha for first order IIR filtering of metric */

} tfw_wlan_coex_mertic_ctr_t;

/*@{*/
typedef struct 
{
  /*! \brief Message header */
  msgr_hdr_struct_type                       hdr;
  /*! \brief WLAN Coex Metric Control */
  tfw_wlan_coex_mertic_ctr_t   wlanCoexMetricCtr;

} tfw_wlan_coex_mertic_ctr_cmd_t;
/*@}*/

/**
   \defgroup WLAN COEX TXFRMDNL PARM indicator
   \ingroup 
   @verbatim
   Usage:
          
    @endverbatim 
*/

/*@{*/ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type                  hdr;
  /*! \brief Body of the message */
  cxm_wcn_txfrmdnl_stats_v01_s    wlanCoexStatus;

} tfw_wlan_txfrmdnl_ind_t;
/*@}*/
  
#endif /* TFW_SW_INTF_MULTI_SIM_H */
