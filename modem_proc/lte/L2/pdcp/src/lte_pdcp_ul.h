/*!
  @file
  lte_pdcp_ul.h

  @brief
  This file contains all the common define structures/function prototypes
  for LTE PDCP UL Task.

  @author
  bqiu, axiao


*/

/*===========================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcp_ul.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/08/15   ar      CR894006: UDC Reset Error Handling
05/15/15   sb      CR 803880: PDCP Changes for UDC feature
09/19/14   sb      CR706535: Defense mechanism in UE to counter HFN mismatch
04/11/14   mg      CR647594: QSH Phase 1
07/24/14   mg      CR699600: Revert CR669056
07/14/14   mg      CR665352: PDCP UL statistics API
07/03/14   mg      CR669056: PDCP discard timer algorithm
06/30/14   sb      CR686848: Added support for sub_id in PDCP
11/19/13   sb      CR535830: PDCP/RLC Re-arch and support of REL-11 features
10/06/11   ax      add ps-pdcp non_empty callback counter
09/06/11   ax      add support for piggyback RoHC feedback
06/15/11   ax      RoHC support
02/03/11   ax      fix CR268752: data stall caused by PDCP not serving PS wm
09/20/10   wshaw   move wdog info to lte_common.h
09/17/10   wshaw   add watchdog support for LTE tasks
04/05/10   ax      removed #define LTE_PDCPUL_MAILPOOL_SIZE
03/11/10   ar      added FTM support
08/03/09   bq      RLF implementation
03/26/09   ar      removed unused msg router indications
02/10/09   bq      move lte_pdcpul_task_init into lte_pdcp_ul_if.h
01/23/08   ax      increase stack size from 8192 to 16384
===========================================================================*/

#ifndef LTE_PDCP_UL_H
#define LTE_PDCP_UL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <lte_l2_common.h>
#include <lte_l2_timer.h>
#include "lte_pdcp_msg.h"
#include "lte_rlc_pdcp.h"
#include "lte_pdcpi.h"
#include "lte_common.h"
#include "lte_pdcp_offload_msg.h"
#include <custlte.h>
#ifdef FEATURE_LTE_UDC_ENABLED
#include "lte_pdcp_comp_msg.h"
#endif /*FEATURE_LTE_UDC_ENABLED*/
#include "qsh.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Union of all PDCPUL external and internal messages */
typedef struct
{
  union
  {
    /*! ************* system general external messages *************/
    /*! Message header */
    msgr_hdr_struct_type                hdr;

    /*! external Messages from RRC */
    lte_pdcpul_cfg_req_msg_s            cfg_req;
    lte_pdcpul_sdu_req_msg_s            sdu_req;
    lte_pdcpul_recfg_prep_req_msg_s     recfg_prep_req;
    lte_rrc_handover_completed_ind_s    ho_completed_ind;
    lte_pdcpul_counter_req_msg_s        counter_check_req;

    /*! external Messages from PS */
    lte_pdcpul_rab_register_req_msg_s   rab_register_req;
    lte_pdcp_rab_deregister_req_msg_s   rab_deregister_req;

    /*external message from PDCP Offload*/
    lte_pdcp_offload_comp_done_ind_msg_s comp_done_ind;

    lte_pdcpul_conn_reestab_req_msg_s    conn_reestab_req;

    /*! internal Messages from PDCP DL*/
    lte_pdcpdl_peer_ctrl_ind_msg_s      peer_ctrl_ind;

    lte_pdcpdl_peer_pb_rohc_fb_ind_msg_s pb_rohc_bf_ind;
    lte_pdcp_qsh_analysis_req_msg_s     qsh_analysis_req;

    /*! external message from IMS*/
    lte_pdcpul_rb_stats_req_msg_s      rb_stats_req;

    /*! external ind message from OFFLOAD/COMP*/
    lte_pdcpul_rab_reg_ind_msg_s     rab_reg_ind;
  } msg;
} lte_pdcpul_msg_u;


/*! @brief Global structure for PDCP UL task
*/
typedef struct
{
  lte_l2_timer_s discard_tick_timer;               /*!< Discard Tick Timer */

  lte_l2_timer_s log_stats_flush_timer;            /*!< Log Flush Timer */
  #ifdef FEATURE_LTE_UDC_ENABLED
  lte_l2_timer_s udc_reset_timer;                  /*!< UDC reset Timer */
  #endif /*FEATURE_LTE_UDC_ENABLED*/

  lte_pdcpi_mode_e mode; /*!< PDCPUL mode (FTM or non-FTM) */

  boolean          started; /*!< resource allocated */
  lte_wdog_info_s  wdog_info;
  lte_l2_timer_s   wdog_timer;
  uint8            sub_id;      /*Subscription ID passed from RRC*/
} lte_pdcpul_data_s;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

EXTERN boolean lte_pdcpul_create_timers
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_cb_handler

===========================================================================*/
/*!
  @brief
  Handler for all messages of PDCP UL task

  @details
  The Message begins with the message header followed by the body.
  msg_length is the total length of message including the header
  msg_ptr points to the beginning of the message.

*/
/*=========================================================================*/

EXTERN void lte_pdcpul_cb_handler
(
   lte_pdcpul_msg_u* msg_ptr        /*!< Pointer to the message */
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_register_msg_router_msgs

===========================================================================*/
/*!
  @brief
  Registers PDCP UL messages with the message router

  @return
  void
*/
/*=========================================================================*/
EXTERN void lte_pdcpul_register_msg_router_msgs
(
  boolean register_ftm_msgs, /*!< flag to indicate weather to register factory
                                 test mode (FTM) msgs */
  msgr_client_t*              msgr_client,  /*!< MSGR client */
  msgr_id_t                   mb_id        /*!< Mailbox Id */
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_task_deinit

===========================================================================*/
/*!
  @brief
  Termination for PDCP UL task

  @return
*/
/*=========================================================================*/

EXTERN void lte_pdcpul_task_deinit
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_task_start_discard_tick_timer

===========================================================================*/
/*!
  @brief
  Can be used to start PDCP UL discard tick timer

  @return
*/
/*=========================================================================*/
extern void lte_pdcpul_task_start_discard_tick_timer
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_task_stop_discard_tick_timer

===========================================================================*/
/*!
  @brief
  Can be used to stop PDCP UL discard tick timer

  @return
*/
/*=========================================================================*/
extern void lte_pdcpul_task_stop_discard_tick_timer
(
  void
);

#ifdef FEATURE_LTE_UDC_ENABLED
/*===========================================================================

  FUNCTION:  lte_pdcpul_task_start_udc_reset_timer

===========================================================================*/
/*!
  @brief
  Can be used to start PDCP UL discard tick timer

  @return
*/
/*=========================================================================*/
extern void lte_pdcpul_task_start_udc_reset_timer
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_task_stop_udc_reset_timer

===========================================================================*/
/*!
  @brief
  Can be used to stop PDCP UL UDC reset timer

  @return
*/
/*=========================================================================*/
extern void lte_pdcpul_task_stop_udc_reset_timer
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_task_udc_reset_timer_running

===========================================================================*/
/*!
  @brief
  Can be used to check whether PDCP UL UDC reset timer is running

  @return
*/
/*=========================================================================*/
extern boolean lte_pdcpul_task_udc_reset_timer_running
(
  void
);

#endif /*FEATURE_LTE_UDC_ENABLED*/

/*===========================================================================

  FUNCTION:  lte_pdcpul_task_discard_tick_timer_running

===========================================================================*/
/*!
  @brief
  Can be used to check whether PDCP UL discard tick timer is running

  @return
*/
/*=========================================================================*/
extern boolean lte_pdcpul_task_discard_tick_timer_running
(
  void
);

#endif /* LTE_PDCP_UL_H */
