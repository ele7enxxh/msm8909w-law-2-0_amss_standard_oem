/*!
  @file
  lte_rlcul_task.h

  @brief
  This file contains all the common define structures/function prototypes
  for LTE RLCUL Task.

  @author
  araina
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcul_task.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/11/14   mg      CR647594: QSH Phase 1
11/19/13   sb      CR535830: PDCP/RLC Re-arch and support of REL-11 features
04/08/13   md      Reduced stack size
02/11/09   ax      move lte_rlcul_task_init() into \inc\lte_rlc.h
01/29/09   ar      Fixed KW and compiler warnings
01/23/08   ax      increase stack size from 8192 to 16384
10/12/08   ar      Added L2 common timer support
09/03/08   ar      intial version
==============================================================================*/

#ifndef LTE_RLCUL_TASK_H
#define LTE_RLCUL_TASK_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <lte_l2_common.h>
#include <lte_l2_timer.h>
#include <lte_rlc_rrc.h>
#include "lte_common.h"
#include "lte_pdcp_ul.h"
#include "qsh.h"
/*! @brief Define Mailbox priority for RLCUL Task
*/
#define LTE_RLCUL_MB_PRIO 1

/*! @brief Define stack size for RLCUL Task
*/
#define LTE_RLCUL_TASK_STACK_SIZE 16384

/*! @brief union of RLCUL registered messages
*/
typedef union
{
  msgr_spr_loopback_struct_type  loop_back;
  lte_rlcul_cfg_req_s            cfg_req;
  msgr_hdr_struct_type           msg_hdr;
  lte_rlcul_stats_req_s          stats_req;
  lte_rlcul_qsh_req_msg_s        qsh_req;
  /*! external Messages from RRC */
  lte_pdcpul_msg_u              pdcp_msg;

} lte_rlcul_registered_msg_u;


/*! @brief Global structure for RLCUL task */
typedef struct
{
  boolean                    active;       /*!< Whether the "main" function of
                                                the thread is running */
  msgr_client_t              msgr_client;  /*!< MSGR client */
  msgr_id_t                  mb_id;        /*!< Mailbox Id */
  lte_l2_timer_s             timer;        /*!< Timer */
  lte_l2_timer_s             wdog_timer;
  lte_wdog_info_s            wdog_info;
  lte_rlcul_registered_msg_u rcv_msg;      /*!< rxed msg buf */
  boolean                    started;      /*!< start REQ rxed with memory allocated*/
} lte_rlcul_task_s;


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*===========================================================================

  FUNCTION:  lte_rlcul_task_is_active

===========================================================================*/
/*!
  @brief
  Can be used to query if the rlc_ul task has been initialized

  @return
  TRUE if the task has been initialized.
*/
/*=========================================================================*/
boolean lte_rlcul_task_is_active
(
  void
);


/*==============================================================================

  FUNCTION:  lte_rlcul_task_deinit

==============================================================================*/
/*!
  @brief
  Destructor for RLCUL task

  @return
  void
*/
/*============================================================================*/
EXTERN void lte_rlcul_task_deinit
(
  void
);


/*==============================================================================

  FUNCTION:  lte_rlcul_task_start_periodic_timer

==============================================================================*/
/*!
  @brief
  This function is a wrapper around the lte_l2_start_timer_periodic timer API.
  It starts the UL periodic timer with appropriate periodicity.

  @return
  void

*/
/*============================================================================*/
EXTERN void lte_rlcul_task_start_periodic_timer
(
  uint32 delay /*!< duration of timer in milliseconds */
);
/* lte_rlcul_task_start_periodic_timer */


/*==============================================================================

  FUNCTION:  lte_rlcul_task_stop_periodic_timer

==============================================================================*/
/*!
  @brief
  This function is a wrapper around the lte_l2_stop_timer API. It stops the UL
  periodic timer.

  @return
  void

*/
/*============================================================================*/
EXTERN void lte_rlcul_task_stop_periodic_timer
(
  void
);
/* lte_rlcul_task_stop_periodic_timer */


/*==============================================================================

  FUNCTION:  lte_rlcul_task_periodic_timer_running

==============================================================================*/
/*!
    @brief
    This function is called to determine whether the RLC periodic timer is
    already running.

    @detail
    Returns TRUE if the timer is already running

    @return
    boolean
*/
/*============================================================================*/
EXTERN boolean lte_rlcul_task_periodic_timer_running
(
  void
);
/* lte_rlcul_task_periodic_timer_running */

EXTERN lte_rlcul_task_s* lte_rlcul_get_task_ptr
(
  void
);

/*==============================================================================

  FUNCTION:  lte_rlcul_task_dynamic_deinit

==============================================================================*/
/*!
    @brief
    dynamic resource allocation

    @return
    None
*/
/*============================================================================*/
EXTERN void lte_rlcul_task_dynamic_deinit
(
  boolean verify_connect_rel
);

#endif /* LTE_RLCUL_TASK_H */
