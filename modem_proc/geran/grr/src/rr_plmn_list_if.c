/*============================================================================
  FILE:         rr_plmn_list_if.c

  OVERVIEW:

  DEPENDENCIES: None

                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_plmn_list_if.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_SGLTE

#include "comdef.h"
#include "rr_plmn_list_if.h"
#include "rr_rr.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* -------------------------------------------------------------------------*/

const char * rr_plmn_list_imsg_name(rr_plmn_list_imsg_e imsg)
{
  switch (imsg)
  {
    default:
      MSG_GERAN_ERROR_1("Unknown message: %d", imsg);
      return "?";
  }
}

const char * rr_plmn_list_omsg_name(rr_plmn_list_omsg_e omsg)
{
  switch (omsg)
  {
    case RR_PLMN_LIST_OMSG_SGLTE_X2G_PLMN_LIST_CNF: return "RR_PLMN_LIST_OMSG_SGLTE_X2G_PLMN_LIST_CNF";
    default:
      MSG_GERAN_ERROR_1("Unknown message: %d", omsg);
      return "?";
  }
}

void rr_plmn_list_send_imsg(rr_plmn_list_imsg_e imsg, const gas_id_t gas_id)
{
  rr_message_header_t header;

  rr_rr_send(&header,
             (uint8) imsg,
             sizeof(header),
             RR_PLMN_LIST_IMSG, gas_id);
}

void 
rr_plmn_list_send_omsg(rr_plmn_list_omsg_e omsg, const gas_id_t gas_id)
{
  rr_message_header_t header;

  rr_rr_send(&header,
             (uint8) omsg,
             sizeof(header),
             RR_PLMN_LIST_OMSG, gas_id);
}

void rr_plmn_list_send_omsg_sglte_x2g_plmn_list_cnf(
  sys_network_selection_mode_e_type   network_selection_mode,
  sys_detailed_plmn_list_s_type     * available_plmn_list,
  boolean                             hplmn_found,
  mm_as_status_T                      status,
  byte                                trans_id,
  sys_service_search_e_type           service_search,
  detected_frequency_list_type      * detected_frequency_list,
  const gas_id_t gas_id
)
{
  rr_plmn_list_omsg_sglte_x2g_plmn_list_cnf_t sglte_x2g_plmn_list_cnf;

  memset(&sglte_x2g_plmn_list_cnf, 0, sizeof(sglte_x2g_plmn_list_cnf));

  sglte_x2g_plmn_list_cnf.network_selection_mode = network_selection_mode;
  if (available_plmn_list != NULL)
  {
    sglte_x2g_plmn_list_cnf.available_plmn_list = *available_plmn_list;
  }
  sglte_x2g_plmn_list_cnf.hplmn_found = hplmn_found;
  sglte_x2g_plmn_list_cnf.status = status;
  sglte_x2g_plmn_list_cnf.trans_id = trans_id;
  sglte_x2g_plmn_list_cnf.service_search = service_search;
  if (detected_frequency_list != NULL)
  {
    sglte_x2g_plmn_list_cnf.detected_frequency_list = *detected_frequency_list;
  }

  rr_rr_send(&sglte_x2g_plmn_list_cnf.header,
             (uint8) RR_PLMN_LIST_OMSG_SGLTE_X2G_PLMN_LIST_CNF,
             sizeof(sglte_x2g_plmn_list_cnf),
             RR_PLMN_LIST_OMSG, gas_id);
}

#endif /* FEATURE_SGLTE */

/* EOF */
