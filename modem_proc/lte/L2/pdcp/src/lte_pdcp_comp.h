/*!
  @file
  lte_pdcp_comp.h

  @brief
  This file contains all the common define structures/function prototypes
  for LTE PDCP COMP Task.

  @author
  sbathwal


*/

/*===========================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcp_comp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/29/15   ar      include customer.h for UDC and ELS feature definition
05/15/15   sb      CR 803880: Initial check-in - PDCP Changes for UDC feature
===========================================================================*/


#include <customer.h>

#ifdef FEATURE_LTE_UDC_ENABLED 

#ifndef LTE_PDCPCOMP_H
#define LTE_PDCPCOMP_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <lte_l2_common.h>
#include <lte_l2_timer.h>
#include "lte_pdcp_comp_msg.h"
#include "lte_pdcpi.h"
#include "lte_common.h"
#include "udcif.h"
#include <custlte.h>

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Union of all PDCP COMP external and internal messages */
typedef struct
{
  union
  {
    /*! ************* system general external messages *************/
    /*! Message header */
    msgr_hdr_struct_type  hdr;

    /*! external Messages from PDCP */
    lte_pdcp_comp_rb_add_mod_ind_msg_s     rb_add_mod_ind;

    lte_pdcp_comp_rb_rel_ind_msg_s         rb_rel_ind;

    lte_pdcp_comp_rb_reset_ind_msg_s       rb_reset_ind;

    lte_pdcp_comp_rab_reg_ind_msg_s        rab_reg_ind;
    
    lte_pdcp_comp_rab_dereg_ind_msg_s      rab_dereg_ind;

    lte_pdcp_comp_mod_resume_ind_msg_s     rb_resume_ind;

    lte_pdcp_comp_wm_low_ind_msg_s         wm_low_ind;

    lte_pdcp_comp_wm_enqueue_ind_msg_s     wm_enqueue_ind;

    lte_pdcp_comp_reestab_ind_msg_s        reestab_ind;
    
    lte_pdcp_qsh_analysis_req_msg_s     qsh_analysis_req;
    
    cfcm_cmd_msg_type_s                 fc_ind;

  } msg;
} lte_pdcp_comp_msg_u;

/*! @brief Define Mailbox priority for PDCP COMP Task
*/
#define LTE_PDCPCOMP_MB_PRIO 1

/*! @brief Define stack size for PDCP COMP Task
*/
#define LTE_PDCPCOMP_TASK_STACK_SIZE 16384

/*! @brief maximum pending message router messages allowed
*/
#define LTE_PDCPCOMP_CFG_TASK_MAX_MSG  50

/*! @brief Global structure for PDCP COMP task
*/

typedef struct
{
  uint32 ps_pdcpcomp_wm_cb_cnt;
  uint32 wm_enqueue_ind_repost_cnt;
}lte_pdcp_comp_data_stats_s;

typedef struct
{
  boolean task_is_active; /*!< Whether the "main" function of the thread is
                               running */
  msgr_client_t  msgr_client;  /*!< MSGR client */
  msgr_id_t      mb_id;        /*!< Mailbox Id */

  lte_pdcp_comp_wm_enqueue_ind_msg_s wm_enqueue_ind[LTE_MAX_RB_CFG_IDX]; 
                                                 /*!<statically allocated msg*/
  lte_pdcp_comp_wm_low_ind_msg_s     wm_low_ind[LTE_MAX_RB_CFG_IDX];     
                                                  /*!<statically allocated msg*/

  boolean          started; /*!< whether or not ROHC is started with the 
                               necessary memory resources */
  lte_pdcp_comp_data_stats_s stats;
} lte_pdcp_comp_data_s;



/*===========================================================================

  FUNCTION:  lte_pdcp_comp_task_is_active

===========================================================================*/
/*!
  @brief
  Can be used to query if the PDCP COMP task has been initialized

  @return
  TRUE if the task has been initialized.
*/
/*=========================================================================*/
boolean lte_pdcp_comp_task_is_active
(
  void
);

#endif /* LTE_PDCPCOMP_H */
#endif /*FEATURE_LTE_UDC_ENABLED */

