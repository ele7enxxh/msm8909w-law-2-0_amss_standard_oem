/*!
  @file
  lte_pdcp_dl.h

  @brief
  This file contains all the common define structures/function prototypes
  for LTE PDCP DL Task.

  @author
  bqiu, axiao


*/

/*===========================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regdlated by the U.S. Government.
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

                        EDIT HISTORY FOR MODDLE

This section contains comments describing changes made to the moddle.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcp_dl.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/19/14   sb      CR706535: Defense mechanism in UE to counter HFN mismatch
04/11/14   mg      CR647594: QSH Phase 1
06/27/14   sb      CR629741: Change in LTE to migrate to CFCM
06/30/14   sb      CR686848: Added support for sub_id in PDCP
11/19/13   sb      CR535830: PDCP/RLC Re-arch and support of REL-11 features
10/21/12   sb      CR408815: Handle case when there are frequent large SRB packets
02/23/11   ax      add support for SMC with invalid MAC-I followed by valid SMC
01/31/11   ax      add support for pdcp dl flow control
09/20/10   wshaw   move wdog info to lte_common.h
09/17/10   wshaw   add watchdog support for LTE tasks
04/05/10   ax      remvoed #define LTE_PDCPDL_MAILPOOL_SIZE
03/11/10   ar      added FTM support
02/07/10   sm      Added data indicaiton optimization in L2 path
08/03/09   bq      RLF implementation
02/12/09   bq      move lte_pdcpdl_task_init into lte_pdcp_dl_if.h
01/23/08   ax      increase stack size from 8192 to 16384
===========================================================================*/

#ifndef LTE_PDCP_DL_H
#define LTE_PDCP_DL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <cfcm.h>
#include <lte_l2_common.h>
#include <lte_l2_timer.h>
#include "lte_pdcp_msg.h"
#include "lte_rlc_pdcp.h"
#include "lte_pdcpi.h"
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

/*! @brief Union of all PDCPDL external and internal messages */
typedef struct
{
  union
  {
    /*! ************* system general external messages *************/
    /*! Message header */
    msgr_hdr_struct_type                hdr;

    /*! external Messages from RRC */
    lte_pdcpdl_cfg_req_msg_s            cfg_req;
    lte_pdcpdl_recfg_prep_req_msg_s     recfg_prep_req;
    lte_rrc_handover_completed_ind_s    ho_completed_ind;
    lte_pdcpdl_counter_req_msg_s        counter_check_req;

    /*! external Messages from PS */
    lte_pdcpdl_rab_register_req_msg_s   rab_register_req;
    lte_pdcp_rab_deregister_req_msg_s   rab_deregister_req;

    lte_pdcpdl_wm_low_ind_msg_s         wm_low_ind;

    cfcm_cmd_msg_type_s                    fc_msg;
    lte_pdcpdl_sec_reset_req_msg_s      sec_reset_req;
    lte_pdcp_qsh_analysis_req_msg_s     qsh_analysis_req;
    lte_pdcpdl_hfn_update_req_msg_s     hfn_update_req;
    lte_pdcpdl_hfn_state_reset_req_msg_s     hfn_state_reset_req;
    /*L2 Diag command indication*/
    lte_mac_diag_data_ind_s             diag_data_ind;
  } msg;
} lte_pdcpdl_msg_u;

/*! @brief Define Mailbox priority for PDCP DL Task
*/
#define LTE_PDCPDL_MB_PRIO 1

/*! @brief Define stack size for PDCP DL Task
*/
#define LTE_PDCPDL_TASK_STACK_SIZE 16384

/*! @brief Global structure for PDCP DL task
*/
typedef struct
{
  boolean task_is_active; /*!< Whether the "main" function of the thread is
                               running */
  msgr_client_t  msgr_client;  /*!< MSGR client */
  msgr_id_t      mb_id;        /*!< Mailbox Id */

  lte_l2_timer_s log_flush_timer;               /*!< Log Flush Timer */

  lte_l2_indication_info_s rlc_data_ind_info; /*!< rlcdl -> pdcpdl data ind
                                                      optimization */

  lte_pdcpi_mode_e mode;    /*!< PDCPDL mode (FTM or non-FTM) */
  boolean          started; /*!< whether or not PDCPDL is started with the 
                               necessary memory resources */
  lte_wdog_info_s  wdog_info;
  lte_l2_timer_s   wdog_timer;
  uint32           num_rlc_data_ind; /*Number of message reposted to PDCP message queue for SRB*/
  uint8            sub_id;      /*Subscription ID passed from RRC*/
  msgr_hdr_struct_type lte_rlcdl_srb_data_ind;
} lte_pdcpdl_data_s;

/*===========================================================================

  FUNCTION:  lte_pdcpdl_task_is_active

===========================================================================*/
/*!
  @brief
  Can be used to query if the pdcp dl task has been initialized

  @return
  TRUE if the task has been initialized.
*/
/*=========================================================================*/
boolean lte_pdcpdl_task_is_active
(
  void
);

/*==============================================================================

  FUNCTION:  lte_pdcpdl_init

==============================================================================*/
/*!
    @brief
    initialization routine

    @return
    None.
*/
/*============================================================================*/
EXTERN void lte_pdcpdl_init( void );

#endif /* LTE_PDCP_DL_H */
