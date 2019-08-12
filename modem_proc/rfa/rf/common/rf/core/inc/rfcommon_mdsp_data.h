#ifndef RFCOMMON_MDSP_DATA_H
#define RFCOMMON_MDSP_DATA_H
/*! 
  @file
  rfcommon_mdsp_data.h
 
  @brief
  This file manages all the Data used by RF COMMON MDSP. This module is completely
  internal to this layer.
*/

/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_mdsp_data.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/20/14   hdz/ra  FW cleanup of deprecated items 
07/13/13   aro/pl  Added data structure to hold ASDiv data
06/17/13   zhw     Added flag to indicate whether FW has sent AOL response
06/16/13   zhw     Tuner AOL call flow update
06/07/13   ndb     Increase the cb status to accomadate both AOL & CL 
06/06/13   zhw     Added timetick in AOL data structure for debugging purpose
03/21/13   zhw     Added support for Antenna Tuner Open Loop common module 
08/28/12   rsr     Added support for TXAGC override.
08/02/12   Saul    COMMON. Added support to allocate/write/free DPD tables.
07/23/12   Saul    RF COMMON. MDSP/MSGR framework. Initial Revision.
 
==============================================================================*/

#include "comdef.h"
#include "msgr_types.h"
#include "rfcommon_locks.h"
#include "rfcmd_msg.h"
#include "fw_rf_common_intf.h"
#include "rfcommon_mdsp.h"

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enum to define the number of common mdsp events related to tuner
  sent from RF to commonfw .
*/
typedef enum 
{
  RFCOMMON_AOL_EVENT,
  RFCOMMON_CL_MEAS_EVENT,
  RFCOMMON_TUNER_MAX_NUM_EVENTS
}rfcommon_mdsp_tuner_event_type;

/*----------------------------------------------------------------------------*/
#define RFCOMMON_MDSP_MSG_LIST_MAX 100 /*!< Max num of Cmds that can be logged */

/*----------------------------------------------------------------------------*/
/*! Structure to hold data related to sequence number generation for 
    SW-FW communication */
 
typedef struct
{
  /*! Most recent sequence number generated which was used in last message 
  sent to FW from FW side. */
  uint32 sequence_number;
  
  /*! Semaphore lock to be used while accessing the sequence number */
  rf_lock_data_type snum_lock;

} rfcommon_mdsp_sum_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold Message Sent Logs

  @details
  This structure holds logs related to the messages sent
*/
typedef struct
{
  /*! Array to hold list Messages sent to MDSP in sequential order */
  msgr_umid_type msg_id;
  
  /*! Time Delta in uS between previous msg sent and current msg sent */
  uint32 time_delta;
  
  /*! the result of msgr API that is called to send msg_id */
  errno_enum_type msgr_ret_status;
  
} rfcommon_mdsp_msg_log_param_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold Message Sent Logs

  @details
  This structure holds logs related to the messages sent
*/
typedef struct
{
  /*! Flag to enable/disable Message Router Log. Flag is defaulted to suppress 
  log */
  boolean log_en;
  
  /*! Flag to enable/disable MSGR debug message. Flag is 
  defaulted to suppress the debug message */
  boolean dbg_msg_en;
  
  /*! Value indicating which index of the array was populated last */
  uint16 idx;
  
  /*! Array to hold list Messages sent to MDSP in sequential order */
  rfcommon_mdsp_msg_log_param_type sent_msg[RFCOMMON_MDSP_MSG_LIST_MAX];
  
} rfcommon_mdsp_msg_log_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold all Shared Memory (SMEM) Messages to be sent to COMMON MDSP from RF.
*/
typedef struct
{
  /*! Flag to indicate whether SMEM is initialized */
  boolean initialized;

  /*! Address to SMEM */
  fw_rf_common_intf_s *smem_ptr;
} rfcommon_mdsp_smem_data_type;


/*----------------------------------------------------------------------------*/
typedef struct
{
  /*! Flag indicating SW is waiting for FW response */
  boolean is_waiting_for_fw_rsp;

  /*! The last FW token sent by SW */
  uint32 last_tuner_snum;

  /*! XO time tick at the moment of the last SW->FW Tuner status update  */
  rf_time_tick_type last_time_tick;

} rfcommon_mdsp_tuner_tech_status_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing state variable for common Tuner Update module . 
*/ 
typedef struct
{  
  /*! Structure that holds the status data for Tuner update between RF/FW */
  rfcommon_mdsp_tuner_tech_status_type tech_tuner_cb_status
                               [RF_TUNER_BUF_ID_NUM_TECHS][RF_TUNER_CHAIN_NUM];

} rfcommon_mdsp_tuner_common_status_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing state variable for common ASDIV module.
*/
typedef struct
{
  uint32 last_snum;
  /*!< Last sequence number */

  rf_time_tick_type last_time_tick;
  /*!< Time tick log */

  rfcommon_mdsp_event_handle* asdiv_event_handle;
  /*!< MDSP event handler */

} rfcommon_mdsp_asdiv_status_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to hold data pertaining to RF COMMON MDSP.
*/
typedef struct
{
  /*! This structure shall be used to hold all the info to be sent to COMMON
  MDSP from RF. */
  rfcommon_mdsp_smem_data_type smem_data;

  /*! Structure to hold Message Sent Logs. This is used for debugging purpose 
  only. */
  rfcommon_mdsp_msg_log_type msg_log;

  /*! Structure to hold data related to sequence number generation for 
  SW-FW communication */
  rfcommon_mdsp_sum_data_type snum_data;

  /*! Structure to store tuner update status fw response data */
  /* RFCOMMON_AOL_EVENT for AOL 
     RFCOMMON_CL_MEAS_EVENT for CL */
  rfcommon_mdsp_tuner_common_status_type common_tuner_cb_status[RFCOMMON_TUNER_MAX_NUM_EVENTS];

  /*! Structure to hold data related asdiv information for fw response */
  rfcommon_mdsp_asdiv_status_type asdiv_cb_status;

} rfcommon_mdsp_data_type;

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_init_data
(
  void
);

/*----------------------------------------------------------------------------*/
rfcommon_mdsp_msg_log_type*
rfcommon_mdsp_get_msgr_log_data
(
  void
);

/*----------------------------------------------------------------------------*/
rfcommon_mdsp_smem_data_type*
rfcommon_mdsp_get_smem_data
(
  void
);

/*----------------------------------------------------------------------------*/
rfcommon_mdsp_tuner_common_status_type*
rfcommon_mdsp_get_common_tuner_cb_status
(
  rfcommon_mdsp_tuner_event_type index
);

/*----------------------------------------------------------------------------*/
rfcommon_mdsp_asdiv_status_type*
rfcommon_mdsp_get_asdiv_cb_status
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_MDSP_DATA_H */
