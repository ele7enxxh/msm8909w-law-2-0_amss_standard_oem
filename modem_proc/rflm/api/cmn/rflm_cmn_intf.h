/*!
  @file
  rflm_cmn_intf.h

  @brief
  Generic interface for common RF interface
 
  @addtogroup RFLM_API_COMMON
  @{
*/

/*===========================================================================

  Copyright (c) 2011-14 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/cmn/rflm_cmn_intf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/14   ndb     Added FBRx CL support
07/17/14   hdz     Changed RFLM_LOG_PACKET_TXAGC_ID
07/10/14   kg      Adding new variable for atomic operation
03/20/14   kg      Adding new TUNER_CL common event
03/12/14   kg      Increase size of the CL/AOL event to take into account
                   the 25+ RFFE transactions requied for RL measurements  
02/18/14   hdz     Added RFLM_CMN_SCRIPT_ASD
01/07/14   hdz     Used log_status API
01/06/14   hdz     Added RFLM txagc and txagc lut log packet support
12/16/13   sty     changed return type of rflm_cmn_init()
12/14/13   zhw     Documentation Update
12/13/13   hdz     Added rflm_cmn_init()
12/12/13   zhw     Documentation Update
12/02/13   zhw     Initial Revision

===========================================================================*/
#ifndef RFLM_CMN_INTF_H
#define RFLM_CMN_INTF_H

#include "rflm_ccs_rf_event_intf.h"
#include "log.h"
#include "rflm_dm_api.h"
#include "rflm_api_fbrx.h"


/*! @brief RFLM txagc log packet ID */
#define RFLM_LOG_PACKET_TXAGC_ID                                          0x1844
/*! @brief RFLM txagc LUT log packet ID */
#define RFLM_LOG_PACKET_TXAGC_LUT_ID                                      0x1845


/*! @brief RFLM TxAGC user ID */
#define RFLM_LOG_PACKET_RAT_TXAGC                                              0
/*! @brief RFLM TxAGC LUT user ID */
#define RFLM_LOG_PACKET_RAT_TXAGC_LUT                                          1


/*! @brief RFLM TxAGC tag size */
#define RFLM_LOG_PACKET_TXAGC_TAG_SIZE_WORDS                                   0
/*! @brief RFLM TxAGC tag size */
#define RFLM_LOG_PACKET_TXAGC_LUT_TAG_SIZE_WORDS                               0

/*! Added new log packet user ID and tag size 
*/

/*! @brief Maxmimum size of CCS memory to be allocated for a Tuner 
  AOL/CL event in WORD. */
#define RFLM_CMN_TUNER_SCRIPT_DATA_BUF_SIZE_WORDS                             20

/*! @brief Data structure for passing tuner update event based script to RFLM. */
typedef struct
{
  /*! @brief Header and tasks for tuner update event */
  rflm_ccs_rf_event_header_task_type header_task;
  /*! @brief Data buffer for the tasks  */
  uint32 data_buf[RFLM_CMN_TUNER_SCRIPT_DATA_BUF_SIZE_WORDS];
} ALIGN(4) rflm_cmn_tuner_ccs_event_type;

/*! @brief RF settings for tuner close loop update */
typedef struct
{
  /*! @brief Close Loop TX AGC power threshold */
  int32 cl_tx_agc_threshold;
  /*! @brief Close Loop event timeout value. It is absolute time set by SW 
      (current time + timeout delta time). Tech RFLM need to do a wraparound
      comparison with half range of the USTMR counter */
  uint32 cl_timeout;
  /*! @brief Settling time for the length of the CL measurement */
  uint32 cl_settling_time;
  /*! @brief Flag to signify if we are in FTM mode or not. This 
    will help mimic actions that transpire in online only
    scenarios. One such current example is RPC bits. RPC bits
    are not toggled\used in FTM. */
  boolean cl_is_in_ftm_mode ;
} rflm_cmn_tuner_setting_type;

/*! @brief Tuner update status information enum. */
typedef enum
{
  /*! @brief This value indicates Tuner Update AOL module is ready*/ 
  RFLM_CMN_TUNER_IDLE,
  /*! @brief This value indicates an outstanding Tuner AOL update in RFLM */
  RFLM_CMN_TUNER_AOL_PENDING,
  /*! @brief This value indicates that RFLM has commited the event for AOL */
  RFLM_CMN_TUNER_AOL_COMMITTED,
  /*! @brief This value indicates that RFLM aborted an AOL update */
  RFLM_CMN_TUNER_AOL_ABORTED,
  /*! @brief This value indicates an outstanding QTF CL update in RFLM */
  RFLM_CMN_TUNER_CL_PENDING,
  /*! @brief This value indicates that Close Loop event has been occurred */
  RFLM_CMN_TUNER_CL_OCCURRED,
  /*! @brief This value indicates that Close Loop event timeout */
  RFLM_CMN_TUNER_CL_TIMEOUT,
  /*! @brief This value indicates an outstanding QTF FBRx CL update in RFLM */
  RFLM_CMN_TUNER_CL_FBRX_PENDING,
  /*! @brief This value indicates that Close Loop event is ready to commit to common FBRX */
  RFLM_CMN_TUNER_CL_FBRX_COMMIT,
  /*! @brief This value indicates that Close Loop event has been completed and responded by FBRX */
  RFLM_CMN_TUNER_CL_FBRX_COMPLETED,
  /*! @brief This value indicates that Close Loop event timeout */
  RFLM_CMN_TUNER_CL_FBRX_TIMEOUT
} rflm_cmn_tuner_event_status_type; 

/*! @brief Tuner update information Data structure RFLM handle
    @details SW updates this interface and checks the SSBI commit status flag before issuing
    next update. So there is no need for double buffering. */
typedef struct
{
  /*! @brief QTF AOL tuner update status. 
     0) RFLM sets this to RFLM_CMN_TUNER_UPDATE_ABORTED to signal an AOL event
     has been aborted
     1) SW sets this to RFLM_CMN_TUNER_UPDATE_PENDING after updating the structure
     2) When the event are committed, RFLM sets the flag to RFLM_CMN_TUNER_UPDATE_COMMITTED
     3) SW sets this to RFLM_RF_QTF_ANT_TUNER_UPDATE_CL_PENDING after updating the structure
     4) When Close Loop event occurred, RFLM sets the flag to RFLM_CMN_TUNER_UPDATE_CL_OCCURRED
     5) When Close Loop event timeout and did not occur, RFLM sets the flag to
     RFLM_CMN_TUNER_UPDATE_CL_TIMEOUT */
  rflm_cmn_tuner_event_status_type tuner_event_status;

  /*! @brief Tuner update CCS event */
  rflm_cmn_tuner_ccs_event_type tuner_update_script;

  /*! @brief Sequence number for RF->RFLM message and response. Used to avoid race
  condition between multiple concurrent tuner AOL updates                */
  uint32 sequence_number;

  /*! @brief TxAGC level updated by CMN RFLM to indicate TxPwr when Tuner CL happens*/
  int32 tx_agc_level;

  /*! @brief CL event settings populated by RF to configure tech and cmn RFLM's 
    CL event behavior*/
  rflm_cmn_tuner_setting_type tuner_closed_loop_info;

  atomic_plain_word_t  concurrent;


} rflm_cmn_tuner_event_data_type; 

/*! 
  @brief  
  error type for rflm cmn init API
*/
typedef enum
{
  RFLM_INIT_ERR_NONE,
  RFLM_INIT_MAX_ERR
}rflm_init_err_t;

/*!
  @brief 
  Common scripts ID., e.g., AsDiv, tuner, hdet etc.

  @detail
  The script buffer ptr associated with the common script ID  will be stored in
  one of the handle script buffers. The script ID starts from the bottom and goes up.
  
*/
typedef enum
{
  /*---------------------------------------
   Adding new script ID here,  e.g,
   RFLm_CMN_SCRIPT_XXX = RFLM_MAX_NUM_HANDLE_SCRIPT_BUF -2,
  */
  RFLM_CMN_TUNER_FBRX_CL_EVENT = RFLM_MAX_NUM_HANDLE_SCRIPT_BUF -3, /*Create a new CL event for FBRx based CL*/
  RFLM_CMN_TUNER_CL_EVENT = RFLM_MAX_NUM_HANDLE_SCRIPT_BUF -2,
  RFLM_CMN_SCRIPT_ASD = RFLM_MAX_NUM_HANDLE_SCRIPT_BUF -1,
  RFLM_CMN_MAX_SCRIPT_ID = RFLM_MAX_NUM_HANDLE_SCRIPT_BUF,
}rflm_cmn_script_e;


typedef struct
{

  /*! @brief  Event ID */
  rflm_cmn_script_e  event_id;
  
  /*! @brief QTF FBRx CL  tuner update status.
   0) Tuner SW sets this flag to RFLM_CMN_TUNER_CL_FBRX_PENDING after updating the DM structure.
   1) When FBRx Captures are scheduled, Tech FED sets this flag to RFLM_CMN_TUNER_CL_FBRX_COMMIT.
   2) When FBRx Captures are not scheduled before the timeout period, Tech FED sets this flag to RFLM_CMN_TUNER_CL_FBRX_TIMEOUT
       and send CL_TIME_OUT response to Tuner.
   3) When scheduled FBRx captures are processed, Tech FED sets this flag to RFLM_CMN_TUNER_CL_FBRX_COMPLETED
       and send CL_COMPLETED response to Tuner.
  */
  rflm_cmn_tuner_event_status_type tuner_event_status;
  
  /*! @brief Sequence number for RF->RFLM message and response. Used to avoid race
   condition between multiple concurrent tuner FBRx CL updates                */
  uint32 sequence_number;

  /*! @brief TxAGC level updated by CMN RFLM to indicate TxPwr when Tuner CL happens*/
  int32 tx_agc_level;

  /*! @brief CL event settings populated by RF to configure tech and cmn RFLM's 
  CL event behavior*/
  rflm_cmn_tuner_setting_type tuner_closed_loop_info;

  /* VSWR measurements data*/
  rflm_fbrx_vswr_meas_data_t vswr_data;

  atomic_plain_word_t  concurrent;

} rflm_cmn_event_fbrx_cl_data_type; 


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean
rflm_cmn_init
(
  void
);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // RFLM_CMN_INTF_H
