#ifndef RR_SGLTE_X2G_IF_H
#define RR_SGLTE_X2G_IF_H

/*============================================================================
  @file rr_sglte_x2g.h

  Signalling interface for the SGLTE Band Coexistence manager state machine.

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sglte_x2g_if.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_SGLTE

#include "comdef.h"
#include "rr_message_header.h"
#include "rr_defs.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* Input messages. */
typedef enum
{
  RR_SGLTE_X2G_IMSG_RESEL_REQ,
  RR_SGLTE_X2G_IMSG_ABORT_REQ,
  RR_SGLTE_X2G_IMSG_REJECT_REQ,
  RR_SGLTE_X2G_IMSG_COUNT
} rr_sglte_x2g_imsg_e;

typedef struct
{
  rr_message_header_t          header;
  mm_as_status_T               activation_reason;
  sys_rat_pri_list_info_s_type rat_pri_list_info;
  sglte_l2g_resel_req_ded_pri_info_t lte_dedicated_pri;
} rr_sglte_x2g_imsg_resel_req_t;

typedef union
{
  rr_message_header_t           header;
  rr_sglte_x2g_imsg_resel_req_t resel_req;
} rr_sglte_x2g_imsg_u;

/* Output messages. */
typedef enum
{
  RR_SGLTE_X2G_OMSG_RESEL_CNF,
  RR_SGLTE_X2G_OMSG_ABORT_CNF,
  RR_SGLTE_X2G_OMSG_REJECT_CNF,
  RR_SGLTE_X2G_OMSG_COUNT
} rr_sglte_x2g_omsg_e;

typedef union
{
  rr_message_header_t                         header;
} rr_sglte_x2g_omsg_u;

/* Union of all messages. */
typedef union
{
  rr_sglte_x2g_imsg_u imsg;
  rr_sglte_x2g_omsg_u omsg;
} rr_sglte_x2g_msg_u;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern const char * rr_sglte_x2g_imsg_name(
  rr_sglte_x2g_imsg_e imsg
);

extern const char * rr_sglte_x2g_omsg_name(
  rr_sglte_x2g_omsg_e omsg
);

extern void rr_sglte_x2g_send_imsg(
  rr_sglte_x2g_imsg_e imsg,
  gas_id_t gas_id
);

extern void rr_sglte_x2g_send_imsg_resel_req(
  mm_as_status_T                 activation_reason,
  sys_rat_pri_list_info_s_type * rat_pri_list_info,
  sglte_l2g_resel_req_ded_pri_info_t * lte_dedicated_pri, 
  gas_id_t gas_id
);

extern void rr_sglte_x2g_send_omsg(
  rr_sglte_x2g_omsg_e omsg,
  gas_id_t gas_id
);

#endif /* FEATURE_SGLTE */

#endif /* RR_SGLTE_X2G_IF_H */
