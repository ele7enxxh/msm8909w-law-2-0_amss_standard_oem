/*!
  @file 
  lte_mac_ctrl.h

  @brief
  This file contains all the common define structures/function prototypes 
  for LTE MAC Control Task.

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
  QUALCOMM Technologies Incorporated and all rights therein are expressly 
  reserved. By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/mac/src/lte_mac_ctrl.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/07/16   al      CR847294: FR 27570 Improved UE Procedure for CSFB page monitoring in L+G/1xSRLTE UE
10/12/15   el      CR 884865: FR 29101 Fast RLF Declaration for fast resync with eNB
12/23/14   sb      CR772190: Extend A2 diag framework for L2 
08/03/14   mg      CR702710: QSH analysis and reset
07/07/14   el      CR 688341: SPS fix for no SR sending after reconnection
10/07/13   ta      CR 529514: DSDS feature. Support for RF Available/Unavailable
                   message
04/08/13   md      Reduced stack size
02/15/13   st      CR 399602: Added handling for a new VoLTE cfg message from RLC
04/30/12   st      CR 356031: Fix for a rare race condition when MAC queries 
                   RLC for buffer status after connection release
10//25/11  st      Added a new message to be handled by MAC CTRL
07/13/11   bn      Added in MAC support for statistics
06/16/11   bn      Moved the delay from SR trigger to UL RACH trigger
09/20/10   wshaw   move wdog info to lte_common.h
09/17/10   wshaw   add watchdog support for LTE tasks
04/05/10   ax      removed #define LTE_MAC_CTRL_MAILPOOL_SIZE
11/18/09   fhu     replaced lte_cphy_sap.h by lte_cphy_msg.h
01/23/08   ax      increase stack size from 8192 to 16384
05/15/08   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_CTRL_H
#define LTE_MAC_CTRL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <lte_l2_common.h>
#include <lte_l2_timer.h>
#include "lte_mac_log.h"
#include <lte_cphy_msg.h>
#include "lte_mac_int_msg.h"
#include "lte_mac_common.h"
#include "lte_common.h"
#include "lte_mac_rlc.h"
#include "qsh.h"
#include "lte_mac_ind.h"

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Union of all MAC Control external and internal messages */
typedef struct
{
  union
  {
    /*! ************* system general external messages *************/
    /*! Message header */
    msgr_hdr_struct_type                hdr;            

    /*! ********* external Messages ************/
    /*! Configuration request from RRC */
    lte_mac_cfg_req_msg_s               cfg_req;
    /*! RACH request from RRC */
    lte_mac_access_req_msg_s            access_req;
    /*! from ML1 to confirm the radio condition measurement */
    lte_cphy_rach_rc_cnf_s              rach_rc_cnf;
    /*! from ML1 to confirm the acceptance of new RACH configuration */
    lte_cphy_rach_cfg_cnf_s             rach_cfg_cnf;
    /*! from ML1 to indicate to MAC that there is PDCCH order */
    lte_cphy_pdcch_order_ind_s          pdcch_order_ind;
    /*! from Ml1 to confirm with MAC the status of sending out SR */
    lte_cphy_send_ul_sr_cnf_s           send_ul_sr_cnf;
    /*! from RRC to MAC to request for cancel the connection request*/
    lte_mac_cancel_conn_req_msg_s       cancel_conn_req;
    /*! from MMAL to MAC to request for update MAC statistics */
    lte_mac_stats_update_req_msg_s      stats_update_req;
    /*! from ML1 to MAC to indicate the SFN status */
    lte_cphy_handover_sfn_status_ind_s  sfn_status_ind;
    /*! from RRC to MAC to request for  rach stats*/
    lte_mac_rach_rpt_req_msg_s rach_rpt_req;

    /*! ************ internal Messages *****************/
    /*! access confirmation from UL Task */
    lte_mac_intmsg_random_access_cnf_s  access_cnf;
    /*! timer expire message from Control task timer expired handler */
    lte_mac_intmsg_timer_expired_tmr_s  timer_expired;
    /*! access abort confirmation from RACH procedure in UL Task*/
    lte_mac_intmsg_access_abort_cnf_s   access_abort_cnf;
    /*! High data arrival indication from PDCP context (by MAC QOS module)to 
      MAC CTRL */
    lte_mac_intmsg_ext_hd_arrival_ind_s high_data_arrival_indi;
    /*! VoLTE LCID notification indication from RLC */
    lte_mac_lcid_priority_ind_s         lcid_priority_ind;
    /*Internal message from CTRL task to CTRl, UL and DL tasks*/
    lte_mac_intmsg_qsh_analysis_req_s   qsh_analysis_reqi;
    /*L2 Diag command indication*/
    lte_mac_diag_data_ind_s             diag_data_ind;
  } msg;
} lte_mac_ctrl_msg_u;

/*! @brief Define Mailbox priority for MAC CTRL Task
*/
#define LTE_MAC_CTRL_MB_PRIO 1

/*! @brief Define stack size for MAC CTRL Task
*/
#define LTE_MAC_CTRL_TASK_STACK_SIZE 8192

/*! @brief Define MAC CTRL Task periodic proccessing interval : 1ms
*/
#define LTE_MAC_CTRL_PERIODIC_INTERVAL    1 

#define LTE_MAC_CFG_QSH_MAJOR_VER         1
#define LTE_MAC_CFG_QSH_MINOR_VER         1

/*! @brief Global CS structure for MAC CTRL task
*/
typedef struct
{
  boolean task_is_active; /*!< Whether the "main" function of the thread is running */
  msgr_client_t  msgr_client;  /*!< MSGR client */
  msgr_id_t      mb_id;        /*!< Mailbox Id */
  lte_wdog_info_s wdog_info;    /*!< wdog information */
} lte_mac_ctrl_cs_s;

/*! @brief Global Timer structure for MAC CTRL task
*/
typedef struct
{
  lte_l2_timer_s timer;                     /*!< Timer */
  lte_l2_timer_s wdog_timer;                /*!< wdog timer */
  int    timer_exp_ctx;                     /*!< Timer Context */
  lte_l2_signal_s signal1;                  /*!< Signal 1 */
  lte_l2_signal_s signal2;                  /*!< Signal 1 */
} lte_mac_ctrl_timer_s;

/*! @brief Access related structure for MAC CTRL task
*/
typedef struct
{
  /*! flag to indicate that the Access procedure is needed to 
  re-start after receiving the abort confirmation*/
  boolean  restart_rach;
  /*! flag to indicate Control task needs to 
  send back the abort confirmation to RRC */
  boolean  send_abort_cnf;
  /*! current RACH configuration */
  lte_mac_rach_cfg_s   cfg;
  /*! current internal access request message */
  lte_mac_intmsg_random_access_req_s   curr_req;
  /*! Shift bit factor*/
  uint8 shift_factor;
} lte_mac_ctrl_access_data_s;

/*! @brief Global Data structure for MAC CTRL task
*/
typedef struct
{
  lte_mac_common_state_e      curr_state;  /*!< current state */
  lte_mac_ctrl_access_data_s  access;      /*!< access related data*/
  lte_mac_cfg_s               mac_cfg;     /*!< mac config data*/
  uint8                       valid_cfg_bitmask; /*!< indicated valid config */
  errno_enum_type             cfg_result_status; /*!< current config status */
  lte_mac_inform_action_e     informed_action;   /*!<informed action */
  uint16                      crnti; /*!< current C-RNTI of the UE */
  uint32                      unexpected_timer_expired_count;
} lte_mac_ctrl_data_s; 

/*! @brief Global structure for MAC CTRL task
*/
typedef struct
{
  lte_mac_ctrl_cs_s     cs;    /*!< all the task control related data */
  lte_mac_ctrl_timer_s  timer; /*!< all the timer related data */
  lte_mac_ctrl_data_s   data;  /*!< all the local data structure */
  lte_mac_log_s*      log;     /*!< Pointer to global MAC Log structure */
} lte_mac_ctrl_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_mac_ctrl_task_is_active

===========================================================================*/
/*!
  @brief
  Can be used to query if the mac_ctrl task has been initialized

  @return
  TRUE if the task has been initialized.
*/
/*=========================================================================*/
boolean lte_mac_ctrl_task_is_active
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_ctrl_start_ul_rach_request

===========================================================================*/
/*!
    @brief
    This function starts the UL RACH request 

    @detail
    
    @return
    None

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_ctrl_start_ul_rach_request
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_ctrl_get_current_rach_req_ptr

===========================================================================*/
/*!
    @brief
    API to get the current RACH request ptr

    @detail
    

    @return
    Pointer to the current RACH request

    @see related_function()

*/
/*=========================================================================*/
extern lte_mac_intmsg_random_access_req_s* lte_mac_ctrl_get_current_rach_req_ptr
(
  void 
);
/*===========================================================================

  FUNCTION:  lte_mac_ctrl_set_rach_count_shift_factor

===========================================================================*/
/*!
    @brief
    overwrite the rach cfg to shrink rach try count

    @detail
    

    @return
    

    @see related_function()

*/
/*=========================================================================*/

extern void lte_mac_ctrl_set_rach_count_shift_factor
(
  uint8 value 
);
/*===========================================================================

  FUNCTION:  lte_mac_ctrl_send_ul_config_ind

===========================================================================*/
/*!
    @brief
    This function sends the indication to inform config to UL task 

    @detail

    @return

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_ctrl_send_ul_config_ind
(
  lte_mac_ul_cfg_s	*cfg_ptr,   /*!< uplink config pointer */
  uint8 ul_config_bitmask  /*!< uplink config bit mask */
);

/*==============================================================================

  FUNCTION:  lte_mac_ctrl_qsh_analysis_routine

==============================================================================*/
/*!
    @brief
    This API runs the MAC CTRL analysis routine

    @detail
    This API can run either in A2 task context during error fatal mode OR
    in L2 task (MAC CTRL task) context during Active mode 

    @return
    None

*/
/*============================================================================*/
extern void lte_mac_ctrl_qsh_analysis_routine 
(
  qsh_action_e action,
  uint32 category_mask
);

/*==============================================================================
 
   FUNCTION:  lte_mac_ctrl_qsh_analysis
 
 ==============================================================================*/
 /*!
     @brief
     MAC callback API registered with QSH.
 
     @detail
     This API should be invoked by QSH during error fatal case or when a
     request comes from DIAG. MAC is expected to execute required analysis 
     routine either in A2 task context or in L2 task context
 
     @return
     TRUE is analysis is performed else FALSE
 */
 /*============================================================================*/
extern boolean lte_mac_ctrl_qsh_analysis
(
  qsh_cb_params_s *params
);

#endif /* LTE_MAC_CTRL_H */
