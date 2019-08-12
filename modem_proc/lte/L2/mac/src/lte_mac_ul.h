/*!
  @file 
  lte_mac_ul.h

  @brief
  This file contains all the common define structures/function prototypes 
  for LTE MAC Up Link Task.

  @author
  baon

*/

/*===========================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/mac/src/lte_mac_ul.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/08/16   el      CR983631: Handle TA length from ML1 in RF available to 
                   avoid race condition
10/12/15   el      CR 884865: FR 29101 Fast RLF Declaration for fast resync with eNB
12/23/14   sb      CR772190: Extend A2 diag framework for L2
12/09/14   pw      CR 749213: Handle DTA
08/03/14   mg      CR702710: QSH analysis and reset
07/07/14   el      CR 688341: SPS fix for no SR sending after reconnection
06/23/14   sb      CR683519: MAC changes to handle QTA and LTA
10/03/13   ta      CR 529514: DSDS feature. Support for RF Available/Unavailable
                   message
06/04/13   st      CR 495466: Removed MAC_STOP_REQ/START_REQ handling in UL
04/08/13   md      Reduced stack size
09/20/10   wshaw   move wdog info to lte_common.h
09/17/10   wshaw   add watchdog support for LTE tasks
08/16/10   bn      Added in support in case the grant in RAR is invalid
04/26/10   bn      Added in handler for stop/start dynamic mem request
04/0510    ax      removed #define LTE_MAC_UL_MAILPOOL_SIZE
11/18/09   fhu     replaced lte_cphy_sap.h by lte_cphy_msg.h
01/23/08   ax      increase stack size from 8192 to 16384
05/15/08   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_UL_H
#define LTE_MAC_UL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <lte_l1_types.h>
#include <comdef.h>
#include <lte_l2_common.h>
#include <lte_l2_timer.h>
#include <lte_cphy_msg.h>
#include "lte_mac_int_msg.h"
#include "lte_mac_common.h"
#include "lte_common.h"
#include "qsh.h"
#include <lte_mac_msg.h>
#include <lte_mac_ind.h>
/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Union of all MAC Up Link external and internal messages */
typedef struct
{
  union
  {
    /*! ************* system general external messages *************/
    /*! Message header */
    msgr_hdr_struct_type                hdr;            

    /*! ***************** external Messages **********************/
    /*!from L1 grant manager to build UL packet*/
    lte_cphy_ul_pkt_build_ind_msg_s pkt_build_ind;
    /*! from ML1 to confirm the RACH abort */
    lte_cphy_rach_abort_cnf_s       rach_abort_cnf;
    /*! from ML1 to confirm the Start PRACH request */
    lte_cphy_start_rach_cnf_s       start_rach_cnf;
    /*! MSG3 transmission indiction from ML1 */
    lte_cphy_msg3_transmission_ind_s  msg3_txed_ind;
    /*! RF available indiction from ML1 */
    lte_cphy_rf_available_ind_s    rf_avai_ind;
    /*! RF unavailable indiction from ML1 */
    lte_cphy_rf_unavailable_ind_s  rf_unavai_ind;

    /*! ****************** internal Messages *********************/
    /*! send from MAC Control task to start the RACH procedure */
    lte_mac_intmsg_random_access_req_s      access_req;
     /*! send from MAC Control task to abort the RACH procedure */
    lte_mac_intmsg_access_abort_req_s       access_abort_req;
    /*! send from DL to UL to indicate the PRACH result */
    lte_mac_intmsg_prach_resp_result_ind_s  prach_resp_result;
    /*! send from DL to UL to indicate the MSG4 result */
    lte_mac_intmsg_msg4_result_ind_s        msg4_result;
    /*! timer expire message from UL task timer expired handler */
    lte_mac_intmsg_timer_expired_tmr_s      timer_expired;
     /*! send from MAC Control task to inform current action */
    lte_mac_intmsg_inform_action_ind_s      inform_action_ind;
    /*! send from MAC Control task to config the UL task */
    lte_mac_intmsg_ul_config_ind_s 	 ul_config_ind;
    /*Internal message from lte_mac_ctrl_qsh_analysis_routine*/
    lte_mac_intmsg_qsh_analysis_req_s   qsh_analysis_reqi;
    /*L2 Diag command indication*/
    lte_mac_diag_data_ind_s             diag_data_ind;
  } msg;
} lte_mac_ul_msg_u;

/*! @brief Define Mailbox priority for MAC UL Task
*/
#define LTE_MAC_UL_MB_PRIO 1

/*! @brief Define stack size for MAC UL Task
*/
#define LTE_MAC_UL_TASK_STACK_SIZE 8192

/*! @brief Global CS structure for MAC UL task
*/
typedef struct
{
  boolean task_is_active; /*!< Whether the "main" function of the thread is running */
  msgr_client_t  msgr_client;  /*!< MSGR client */
  msgr_id_t      mb_id;        /*!< Mailbox Id */
  lte_wdog_info_s wdog_info;    /*!< wdog information */
} lte_mac_ul_cs_s;

/*! @brief Global Timer structure for MAC UL task
*/
typedef struct
{
  lte_l2_timer_s timer;                     /*!< Timer */
  lte_l2_timer_s wdog_timer;                /*!< Timer */
  int    timer_exp_ctx;                     /*!< Timer Context */
  lte_l2_signal_s signal1;                  /*!< Signal 1 */
  lte_l2_signal_s signal2;                  /*!< Signal 1 */
} lte_mac_ul_timer_s;

/*! @brief Global Data structure for MAC UL task
*/
typedef struct
{
  lte_mac_common_state_e  prev_state;          /*!< previous state */
  lte_mac_common_state_e  curr_state;          /*!< current state */
  lte_mac_common_state_e  pending_curr_state;  /*!< pending current state */
  lte_mac_intmsg_random_access_req_s  *pending_rach_req_ptr;
} lte_mac_ul_data_s;

/*! @brief Global Stats structure for MAC UL task
*/
typedef struct
{
  uint32 num_rfta_rcvd;          /*!< number of tuneaways received*/
  /*Does not protect against tuneaways greater than 10240 milliseconds*/
  uint32 total_rfta_time;        /*!< total tuneaway time*/
  uint32 last_ta_interval;          /*!< last tuneaway interval */
} lte_mac_ul_stats_s;


/*! @brief Global structure for MAC UL task
*/
typedef struct
{
  lte_mac_ul_cs_s     cs;      /*!< all the task control related data */
  lte_mac_ul_timer_s  timer;   /*!< all the timer related data */
  lte_mac_ul_data_s   data;    /*!< all the local data structure */
  lte_mac_ul_stats_s  stats;   /*!< all the local stats structure*/
} lte_mac_ul_s;

/*===========================================================================

  FUNCTION:  lte_mac_ul_task_is_active

===========================================================================*/
/*!
  @brief
  Can be used to query if the mac_ul task has been initialized

  @return
  TRUE if the task has been initialized.
*/
/*=========================================================================*/
boolean lte_mac_ul_task_is_active
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_start_timer

===========================================================================*/
/*!
    @brief
    This function is a wrap-around start timer function for UL Task.

    @detail

    @return
    None

    @note
    lte_mac_ul_create_timers must be call first.

    @see related_function()
    lte_mac_ul_create_timers, lte_mac_ul_stop_timer
*/
/*=========================================================================*/
extern void lte_mac_ul_start_timer
( 
  uint32 delay  /*!< duration of timer in milliseconds */
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_stop_timer

===========================================================================*/
/*!
    @brief
    This function is a wrap-around stop timer function for UL Task.

    @detail

    @return
    None

    @note
    lte_mac_ul_create_timers must be call first.

    @see related_function()
    lte_mac_ul_create_timers, lte_mac_ul_start_timer
*/
/*=========================================================================*/
extern void lte_mac_ul_stop_timer
( 
  void
);

/*==============================================================================

  FUNCTION:  lte_mac_ul_qsh_analysis_routine

==============================================================================*/
/*!
    @brief
    This API runs the MAC UL QSH analysis routine

    @detail
    This API can run either in A2 task context during error fatal mode OR
    in L2 task (MAC UL task) context during Active mode 

    @return
    None

*/
/*============================================================================*/
extern void lte_mac_ul_qsh_analysis_routine 
(
  qsh_action_e action,
  uint32 category_mask
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_get_last_ta_interval

===========================================================================*/
/*!
    @brief
    This function is used to get the last tuneaway interval

    @detail

    @return
    None

    @note

    @see related_function()
*/
/*=========================================================================*/
extern uint32 lte_mac_ul_get_last_ta_interval
( 
  void
);
#endif /* LTE_MAC_UL_H */
