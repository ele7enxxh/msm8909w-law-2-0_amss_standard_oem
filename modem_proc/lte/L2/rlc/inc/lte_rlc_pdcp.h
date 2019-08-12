/*!
  @file
  lte_rlc_pdcp.h

  @brief
  This file contains all external interfaces exposed by RLC for PDCP.

*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/inc/lte_rlc_pdcp.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/02/13   mg      CR489345: Get num of PDCP PDU bytes transmitted for an EPSID
11/21/11   ax      CR320054: move definition needed by FTM to API dir for CMI
01/04/11   ax      consolidate RLC APIs and place them under lte/api/
05/19/10   ar      added PDCP ciphering log packet support
03/26/09   ar      removed unused indication structures
11/17/08   ax      use lte_rb_cfg_idx_t as a unique identifier across AS
11/17/08   ar      Added macros for functions
                   lte_dsmi_enqueue_with_length_in_app_field() and
                   lte_dsmi_dequeue_with_length_in_app_field()
07/31/2008 ar      initial version
==============================================================================*/

#ifndef LTE_RLC_PDCP_H
#define LTE_RLC_PDCP_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <lte_as.h>
#include <msgr.h>
#include <dsm_item.h>
#include <lte_dsm_util.h>
#include <lte_pdcp_ul_if.h>
#include "lte_rlc_ext_api.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief LTE_RLCUL_FREE_PDU_IND payload definition
*/
typedef struct
{
  msgr_hdr_struct_type  msg_hdr;        /*!< message header */
  dsm_item_type*        pdu_chain_ptr;  /*!< dsm pointer to the chain of RLC
                                             sdus that need to be freed */
} lte_rlcul_free_pdu_ind_s;


/*! @brief LTE_RLCUL_ACK_IND payload definition
*/
typedef struct
{
  msgr_hdr_struct_type  msg_hdr;         /*!< message header */
  lte_rb_cfg_idx_t      rb_cfg_idx;      /*!< unique RB configuration index */
  uint32                first_ack_count; /*!< PDCP COUNT of the first acked
                                              sdu */
  uint32                last_ack_count;  /*!< PDCP COUNT of the first acked
                                              sdu */
} lte_rlcul_ack_ind_s;


/*! @brief LTE_RLCUL_RE_EST_STATUS_IND payload definition
*/
typedef struct
{
  msgr_hdr_struct_type  msg_hdr;                   /*!< message header */
  lte_rb_cfg_idx_t      rb_cfg_idx;                /*!< RB configuration index*/
  dsm_item_type*        ack_chain_ptr;             /*!< chain of ACKed sdus */
  dsm_item_type*        nack_chain_ptr;            /*!< chain of pdus never
                                                        txed out of UE */
  dsm_item_type*        status_unknown_chain_ptr;  /*!< chain of txed pdus
                                                        for which no ACK has
                                                        been received yet */
} lte_rlcul_re_est_status_ind_s;

/*! @brief Structure for PDCPUL cipher Log buffer
*/
typedef struct
{
  lte_pdcp_log_sub_pkt_cipher_pdu_s  pdus[2];   /*!< PDU sub pkt dbl buffer */
  lte_pdcp_log_sub_pkt_cipher_pdu_s* read_ptr;  /*!< Flush happens on this
                                                     buffer */
  lte_pdcp_log_sub_pkt_cipher_pdu_s* write_ptr; /*!< sub pkts are written to
                                                     this buffer */
  boolean                            used;      /*!< being used */
} lte_pdcpul_cipher_log_buf_s;

/*==============================================================================

                   EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void lte_rlcdl_init_circ_q_handle( void );

extern lte_pdcpul_cipher_log_buf_s*
    lte_pdcpul_log_get_ciph_log_buf_handle(void);

EXTERN uint64 lte_rlculi_no_sdu_bytes_tx
( 
  lte_rb_cfg_idx_t rb_cfg_idx /* Radio Bearer configuration index */
);



#endif /* LTE_RLC_PDCP_H */

