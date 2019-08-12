/*============================================================================
  FILE:         rr_sglte_x2g_if.c

  OVERVIEW:

  DEPENDENCIES: None

                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sglte_x2g_if.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_SGLTE

#include "comdef.h"
#include "rr_sglte_x2g_if.h"
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

const char * rr_sglte_x2g_imsg_name(rr_sglte_x2g_imsg_e imsg)
{
  switch (imsg)
  {
    case RR_SGLTE_X2G_IMSG_RESEL_REQ: return "RR_SGLTE_X2G_IMSG_RESEL_REQ";
    case RR_SGLTE_X2G_IMSG_ABORT_REQ: return "RR_SGLTE_X2G_IMSG_ABORT_REQ";
    case RR_SGLTE_X2G_IMSG_REJECT_REQ: return "RR_SGLTE_X2G_IMSG_REJECT_REQ";
	
    default:
      MSG_GERAN_ERROR_1("Unknown message: %d", imsg);
      return "?";
  }
}

const char * rr_sglte_x2g_omsg_name(rr_sglte_x2g_omsg_e omsg)
{
  switch (omsg)
  {
    case RR_SGLTE_X2G_OMSG_RESEL_CNF: return "RR_SGLTE_X2G_OMSG_RESEL_CNF";
    case RR_SGLTE_X2G_OMSG_ABORT_CNF: return "RR_SGLTE_X2G_OMSG_ABORT_CNF";
    case RR_SGLTE_X2G_OMSG_REJECT_CNF: return "RR_SGLTE_X2G_OMSG_REJECT_CNF";	
    default:
      MSG_GERAN_ERROR_1("Unknown message: %d", omsg);
      return "?";
  }
}

void rr_sglte_x2g_send_imsg(
   rr_sglte_x2g_imsg_e imsg,
   const gas_id_t gas_id)
{
  rr_message_header_t header;

  rr_rr_send(&header,
             (uint8) imsg,
             sizeof(header),
             RR_SGLTE_X2G_IMSG, gas_id);
}

void rr_sglte_x2g_send_imsg_resel_req(
  mm_as_status_T                 activation_reason,
  sys_rat_pri_list_info_s_type * rat_pri_list_info,
  sglte_l2g_resel_req_ded_pri_info_t * lte_dedicated_pri, 
  const gas_id_t gas_id
)
{
  rr_sglte_x2g_imsg_resel_req_t resel_req;

  memset(&resel_req, 0, sizeof(resel_req));

  resel_req.activation_reason = activation_reason;
  if (rat_pri_list_info != NULL)
  {
    resel_req.rat_pri_list_info = *rat_pri_list_info;
  }

  if (lte_dedicated_pri != NULL)
  {
    resel_req.lte_dedicated_pri = *lte_dedicated_pri;
  }


  rr_rr_send(&resel_req.header,
             (uint8) RR_SGLTE_X2G_IMSG_RESEL_REQ,
             sizeof(resel_req),
             RR_SGLTE_X2G_IMSG,
             gas_id);
}

void rr_sglte_x2g_send_omsg(
   rr_sglte_x2g_omsg_e omsg,
   const gas_id_t gas_id)
{
  rr_message_header_t header;

  rr_rr_send(&header,
             (uint8) omsg,
             sizeof(header),
             RR_SGLTE_X2G_OMSG,
             gas_id);
}

#endif /* FEATURE_SGLTE */

/* EOF */
