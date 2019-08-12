#ifndef RR_PLMN_LIST_IF_H
#define RR_PLMN_LIST_IF_H

/*============================================================================
  @file rr_plmn_list_if.h

  Signalling interface for the PLMN List Search state machine.

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_plmn_list_if.h#1 $ */

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
  RR_PLMN_LIST_IMSG_COUNT
} rr_plmn_list_imsg_e;

typedef union
{
  rr_message_header_t header;
} rr_plmn_list_imsg_u;

/* Output messages. */
typedef enum
{
  RR_PLMN_LIST_OMSG_SGLTE_X2G_PLMN_LIST_CNF,
  RR_PLMN_LIST_OMSG_COUNT
} rr_plmn_list_omsg_e;

typedef struct
{
  rr_message_header_t               header;
  sys_network_selection_mode_e_type network_selection_mode;
  sys_detailed_plmn_list_s_type     available_plmn_list;
  boolean                           hplmn_found;
  mm_as_status_T                    status;
  byte                              trans_id;
  sys_service_search_e_type         service_search;
  detected_frequency_list_type      detected_frequency_list;
} rr_plmn_list_omsg_sglte_x2g_plmn_list_cnf_t;

typedef union
{
  rr_message_header_t                         header;
  rr_plmn_list_omsg_sglte_x2g_plmn_list_cnf_t sglte_x2g_plmn_list_cnf;
} rr_plmn_list_omsg_u;

/* Union of all messages. */
typedef union
{
  rr_plmn_list_imsg_u imsg;
  rr_plmn_list_omsg_u omsg;
} rr_plmn_list_msg_u;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern const char * rr_plmn_list_imsg_name(
  rr_plmn_list_imsg_e imsg
);

extern const char * rr_plmn_list_omsg_name(
  rr_plmn_list_omsg_e omsg
);

extern void rr_plmn_list_send_imsg(
  rr_plmn_list_imsg_e imsg,
  gas_id_t gas_id
);

extern void rr_plmn_list_send_omsg(
  rr_plmn_list_omsg_e omsg,
  gas_id_t gas_id
);

extern void rr_plmn_list_send_omsg_sglte_x2g_plmn_list_cnf(
  sys_network_selection_mode_e_type   network_selection_mode,
  sys_detailed_plmn_list_s_type     * available_plmn_list,
  boolean                             hplmn_found,
  mm_as_status_T                      status,
  byte                                trans_id,
  sys_service_search_e_type           service_search,
  detected_frequency_list_type      * detected_frequency_list,
  gas_id_t gas_id
);

#endif /* FEATURE_SGLTE */

#endif /* RR_PLMN_LIST_IF_H */
