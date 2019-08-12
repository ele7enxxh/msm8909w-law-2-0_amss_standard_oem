/*!
  @file
  lte_pdcp_offload.h

  @brief
  This file contains all the common define structures/function prototypes
  for LTE PDCP OFFLOAD Task.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcp_offload.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/15   sb      CR 803880: PDCP Changes for UDC feature
09/19/14   sb      CR706535: Defense mechanism in UE to counter HFN mismatch
09/04/14   sb      CR694693: Fix to ensure UL PDUs are not built after RAB
                   is released
08/05/14   sb      CR696130: Reset Ciphering parameter in OFFLOAD when security 
                   parameter changes
08/01/14   sb      CR701729: Remove CFM header file
04/08/13   md      Reduced stack size
05/21/12   sb      CR363363: Compiler warning fixes
05/16/12   sb      CR361321: Added L2 indication optimization structure to reduce
                   frequency between PDCP to OFFLOAD notification.
===========================================================================*/

#ifndef LTE_PDCPOFFLOAD_H
#define LTE_PDCPOFFLOAD_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <lte_l2_common.h>
#include <lte_l2_timer.h>
#include "lte_pdcp_offload_msg.h"
#include "lte_pdcp_rohc.h"
#include "lte_pdcpi.h"
#include "lte_common.h"

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Union of all PDCP OFFLOAD external and internal messages */
typedef struct
{
  union
  {
    /*! ************* system general external messages *************/
    /*! Message header */
    msgr_hdr_struct_type  hdr;

    /*! external Messages from PDCP */
    lte_pdcpul_comp_cfg_req_msg_s            comp_cfg_req;

    lte_pdcpdl_decomp_cfg_req_msg_s          decomp_cfg_req;

    lte_pdcpul_comp_pkt_req_msg_s            comp_pkt_req;

    lte_pdcpdl_decomp_pkt_req_msg_s          decomp_pkt_req;

    lte_pdcpdl_peer_ctrl_ind_msg_s     peer_ctrl_ind;

    lte_pdcpdl_peer_pb_rohc_fb_ind_msg_s pb_rohc_bf_ind;

    lte_pdcpdl_a2_rab_deregister_req_msg_s    rab_deregister_req;

    lte_pdcpdl_ciph_reset_req_msg_s    ciph_rst_req;

    lte_pdcp_offload_reset_hfn_state_req_msg_s hfn_reset_req;

    lte_pdcpdl_hfn_update_cnf_msg_s    hfn_cnf;

    /*Indications sent from PDCP OFFLOAD*/
    lte_pdcp_offload_comp_done_ind_msg_s  comp_done_ind;

    lte_pdcp_offload_rab_reg_ind_msg_s    ul_rab_reg_ind;
    
    lte_pdcp_offload_rab_dereg_ind_msg_s    ul_rab_dereg_ind;

  } msg;
} lte_pdcp_offload_msg_u;

/*! @brief Define Mailbox priority for PDCP OFFLOAD Task
*/
#define LTE_PDCPOFFLOAD_MB_PRIO 1

/*! @brief Define stack size for PDCP OFFLOAD Task
*/
#define LTE_PDCPOFFLOAD_TASK_STACK_SIZE 8192

/*! @brief Context structure for signal callback
*/
typedef struct
{
  lte_rb_cfg_idx_t    rb_cfg_idx;                /*!< RB Cfg Index */
} lte_pdcp_offload_ps_signal_ctx_s;

/*! @brief Global structure for PDCP OFFLOAD task
*/
typedef struct
{
  boolean task_is_active; /*!< Whether the "main" function of the thread is
                               running */
  msgr_client_t  msgr_client;  /*!< MSGR client */
  msgr_id_t      mb_id;        /*!< Mailbox Id */

  lte_l2_signal_s ps_signal[LTE_MAX_RB_CFG_IDX]; /*!< Signal from PS */
  lte_pdcp_offload_ps_signal_ctx_s ps_signal_ctx[LTE_MAX_RB_CFG_IDX]; /*!< Signal Context */

  lte_pdcp_offload_wm_enqueue_ind_msg_s wm_enqueue_ind[LTE_MAX_RB_CFG_IDX]; 
                                                 /*!<statically allocated msg*/
  lte_pdcp_offload_wm_low_ind_msg_s     wm_low_ind[LTE_MAX_RB_CFG_IDX];     
                                                  /*!<statically allocated msg*/
  lte_l2_ind_info_s pdcp_data_ind_info; /*!< pdcpdl -> pdcpoffload data ind optimization */

  boolean          started; /*!< whether or not ROHC is started with the 
                               necessary memory resources */
  uint32 ps_pdcp_offload_wm_cb_cnt;
} lte_pdcp_offload_data_s;



/*===========================================================================

  FUNCTION:  lte_pdcp_offload_task_is_active

===========================================================================*/
/*!
  @brief
  Can be used to query if the PDCP OFFLOAD task has been initialized

  @return
  TRUE if the task has been initialized.
*/
/*=========================================================================*/
boolean lte_pdcp_offload_task_is_active
(
  void
);

#endif /* LTE_PDCPOFFLOAD_H */
