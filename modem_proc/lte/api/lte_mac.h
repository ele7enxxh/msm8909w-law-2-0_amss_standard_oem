/*!
  @file
  lte_mac.h

  @brief
  This file contains all External interfaces exported by MAC layer.


*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_mac.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/14   el      CR 697620: Replace Mac Padding to PN31 for GPS
12/20/13   mg      CR 592094: FR 19078: SS eMBMS API Requirement - L2 MAC
11/27/13   mg      Allowed throughput API for Bolt
08/09/12   st      CR 385047: Increased the size of TA value
06/13/12   st      CR 368591: Added certain unit test functions for FR 1606
07/13/11   bn      Added in MAC support for statistics
05/15/08   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_H
#define LTE_MAC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <pthread.h>
#include <lte_as.h>
#include <lte.h>


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Access reason enumeration type
*/
typedef enum
{
  LTE_MAC_ACCESS_REASON_CONNECTION_REQ,   /*!< connection request  */
  LTE_MAC_ACCESS_REASON_RADIO_FAILURE,    /*!< radio failure */
  LTE_MAC_ACCESS_REASON_HAND_OVER,        /*!< hand over */
  LTE_MAC_ACCESS_REASON_UL_DATA_ARRIVAL,  /*!< up link data arrival */
  LTE_MAC_ACCESS_REASON_DL_DATA_ARRIVAL,  /*!< down link data arrival */
  LTE_MAC_ACCESS_REASON_DSDS_INACTIVITY_TMR_EXPIRY, /*!< dsds traffic inactivity timer expiry */
  LTE_MAC_ACCESS_REASON_DSDS_LTA_LOCAL_RELEASE /*!< DSDS LTA Local connection release*/
} lte_mac_access_reason_e;

/*! @brief MAC information action enumeration type
*/
typedef enum
{
  LTE_MAC_ACTION_NONE,               /*!< no action  */
  LTE_MAC_ACTION_CONNECTION_RELEASE, /*!< connection release */
  LTE_MAC_ACTION_HO,                 /*!< hand off */
  LTE_MAC_ACTION_RADIO_FAILURE,      /*!< radio failure */
  LTE_MAC_ACTION_CONNECTION_CANCEL   /*!< connection cancel */
} lte_mac_inform_action_e;

/*! @brief BSR LC Group enumeration type
*/
typedef enum
{
  LTE_MAC_BSR_LCGROUP_0,   /*!< BSR LC group 0 */
  LTE_MAC_BSR_LCGROUP_1,   /*!< BSR LC group 1 */
  LTE_MAC_BSR_LCGROUP_2,   /*!< BSR LC group 2 */
  LTE_MAC_BSR_LCGROUP_3,   /*!< BSR LC group 3 */
  LTE_MAC_BSR_LCGROUP_MAX, /*!< Maximum number of BSR LC group */
} lte_mac_bsr_lcgroup_e;

/*! @brief MAC statistics data: LT07
*/
typedef struct 
{
  uint16 c_rnti;          /*!<current C-RNTI: 1 to 65523.0 = not initialized*/
  uint8  lcg_buf_size[LTE_MAC_BSR_LCGROUP_MAX];/*!< buffer size for all LCGs*/
  uint16 timing_adv;      /*!< current timing advance: 0 to 63 */
  uint8  power_headroom;  /*!< current power headroom index: 0 to 63 */
} lte_mac_stats_data_s;

/*! @brief MAC to update the statistics metric Request MSG. Modem 
    statistics manager will send this request to MAC with the data pointer */
typedef struct 
{
  msgr_hdr_struct_type   hdr;  /*!< Message header */
  /* Message payload */
  lte_mac_stats_data_s   *payload_ptr;
} lte_mac_stats_update_req_msg_s;

/*! @brief MAC to update the statistics metric CNF MSG. MAC will send back 
    this CNF when MAC done updating the statistics due to a earlier request */
typedef struct 
{
  msgr_hdr_struct_type   hdr;  /*!< Message header */
  /* Message payload */
} lte_mac_stats_update_cnf_msg_s;

/*! @brief contains the info provided to CMAPI for each active embms session*/
typedef struct
{
  lte_mbsfn_area_id_t area_id; /*!< Area id */
  lte_pmch_id_t pmch_id; /*!< pmch id */
  lte_lc_id_t lc_id; /*!< lc id */
  /*!< Number of MTCH transport block bytes received on this channel since
       last activation */
  uint64 num_mtch_tb_bytes; 
} mtch_info_s;

/*! @brief MAC to update the statistics of this structure provided by CMAPI*/
typedef struct
{
  uint8 num_active_embms_comb; /*Number of active area_id,pmch_id combinations*/
  mtch_info_s mtch_info_arr[LTE_EMBMS_MAX_ACTIVE_SESSIONS];
} lte_mac_active_embms_stats_s;

/*===========================================================================

  FUNCTION:  lte_mac_embms_get_stats

===========================================================================*/
/*!
    @brief

    @detail
    This function updates the entries into the structure lte_mac_active_embms_stats
    pointer provided by CMAPI.
    The num_active_embms_comb refers to the number of active embms sessions.
    The mtch_info_arr array entries upto indices (num_active_embms_comb - 1) are filled.
    The number of accumulated MTCH TB bytes gets reset whenever an LCID stops
    being monitored.


    @return
    None

    @note

    @see related_function()

*/
/*=========================================================================*/
EXTERN void lte_mac_embms_get_stats
( 
  /*!< pointer to the structure to fill the active embms combos MTCH information. */
  lte_mac_active_embms_stats_s* active_embms_stats_ptr  
);
/*===========================================================================

  FUNCTION:  lte_mac_ctrl_task_init

===========================================================================*/
/*!
    @brief
    This function is a initialization function for MAC Control Task.

    @detail
    Initialization consists of:
    - Initialization MAC control attributes
    - Initialization stack size and priority
    - Starting the MAC Control Task

    @return
    Thread ID

    @note

    @see related_function()

*/
/*=========================================================================*/
EXTERN pthread_t lte_mac_ctrl_task_init
( 
  int priority  /*!< thread priority passed in by app mgr */
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_task_init

===========================================================================*/
/*!
    @brief
    This function is a initialization function for MAC UL Task.

    @detail
    Initialization consists of:
    - Initialization MAC UL attributes
    - Setting stack size and priority
    - Starting the MAC UL Task

    @return
    Initialized Status: TRUE or FALSE

    @note

    @see related_function()

*/
/*=========================================================================*/
EXTERN pthread_t lte_mac_ul_task_init
( 
  int priority  /*!< thread priority passed in by app mgr */
);

/*===========================================================================

  FUNCTION:  lte_mac_dl_task_init

===========================================================================*/
/*!
  @brief
  This function is a initialization function for MAC DL Task.

  @detail
    Initialization consists of:
    - Initialization MAC DL attributes
    - Setting stack size and priority
    - Starting the MAC DL Task

    @return
    Thread ID

    @note

    @see related_function()
*/
/*=========================================================================*/
EXTERN pthread_t lte_mac_dl_task_init
( 
  int priority  /*!< thread priority passed in by app mgr */
);

/*===========================================================================

  FUNCTION:  lte_mac_ctrl_test_start_periodic_process

===========================================================================*/
/*!
  @brief
  This function commences the MAC periodic QoS processing

  @detail
    - Starts the periodic Qos processing for buffer status, RACH/SR req etc
    - Starts the QoS periodic logging
    - Sets the periodic interval timer

  @return
 
  @note

  @see related_function()
*/
/*=========================================================================*/
EXTERN void lte_mac_ctrl_test_start_periodic_process
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_ctrl_test_set_connected

===========================================================================*/
/*!
  @brief
  This function is a test function to set the MAC state to connected 

  @detail
    Sets the internal MAC state to connected for testing purposes

  @return
    
  @note

  @see related_function()
*/
/*=========================================================================*/
EXTERN void lte_mac_ctrl_test_set_connected
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_test_set_connected

===========================================================================*/
/*!
  @brief
  This function sets the MAC UL state to connected

  @detail
  This function sets the internal state of MAC UL to connected for testing
  purposes.

  @return

  @note

  @see related_function()
*/
/*=========================================================================*/
EXTERN void lte_mac_ul_test_set_connected
(
  void
);


/*===========================================================================

  FUNCTION:  lte_mac_qos_get_allowed_tput

===========================================================================*/
/*!
    @brief
    This function returns the allowed throughput for each calculation cycle 

    @detail
    This function returns 0 if it is called before the first calculation cycle is over.
    This function returns the same value when called anytime during the same calculation cycle.
    If the value returned by this function is less than the sum of the GBR for all bearers,
    then DS should return that value instead of the value returned by this API.
    

    @return
    Allowed throughput in bytes/sec

    @see

*/
/*=========================================================================*/
EXTERN uint32 lte_mac_qos_get_allowed_tput
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_data_set_gps_padding_status

===========================================================================*/
/*!
    @brief
    This function sets the current gps status to decide whether to do gps padding or not

    @detail
    This function returns void and will set the gps status to decide whether 
    to do gps padding or not in mac_ul task
    
    @return
    none

    @see

*/
/*=========================================================================*/
EXTERN void lte_mac_ul_data_set_gps_padding_status
(
  boolean enable_gps_padding   /*!< Flag to decide whether to do gps padding or not */
);

#endif /* LTE_MAC_H */
