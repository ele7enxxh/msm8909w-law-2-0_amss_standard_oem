/*! \file rr_x2g_cgi_if.c 
 
  Signalling interface for the RR X2G CGI module. 
 
                Copyright (c) 2011-2013 QUALCOMM Technologies, Inc
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_x2g_cgi_if.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_LTE_TO_GSM_CGI

#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_x2g_cgi_if.h"
#include "rr_rr.h"
#include "geran_multi_sim.h"

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

const char * rr_x2g_cgi_imsg_name(
  rr_x2g_cgi_imsg_e imsg
)
{
  switch (imsg)
  {
    case RR_X2G_CGI_IMSG_CGI_REQ:
    {
      return "RR_X2G_CGI_IMSG_CGI_REQ";
    } /* RR_X2G_CGI_IMSG_CGI_REQ */
    
    case RR_X2G_CGI_IMSG_ABORT_REQ:
    {
      return "RR_X2G_CGI_IMSG_ABORT_REQ";
    } /* RR_X2G_CGI_IMSG_ABORT_REQ */

    case RR_X2G_CGI_IMSG_L1_DEACTIVATED_IND:
    {
      return "RR_X2G_CGI_IMSG_L1_DEACTIVATED_IND";
    }

    case RR_X2G_CGI_IMSG_L1_ACTIVATED_IND:
    {
      return "RR_X2G_CGI_IMSG_L1_ACTIVATED_IND";
    }

    default:
    {
      MSG_GERAN_ERROR_1("Unknown message: %d", imsg);
      return "?";
    }
  }
}

void rr_x2g_cgi_send_imsg(
  rr_x2g_cgi_imsg_e imsg,
  const gas_id_t gas_id
)
{
  rr_message_header_t header;

  rr_rr_send(
    &header,
    (uint8) imsg,
    sizeof(header),
    RR_X2G_CGI_IMSG,
    gas_id
  );
}

void rr_x2g_cgi_send_imsg_cgi_req(
  rr_x2g_cgi_arfcn_list_t * arfcn_list,
  BSIC_T                    bsic,
  uint32                    time_available,
  rr_x2g_cgi_info_t       * info,
  const gas_id_t            gas_id
)
{
  rr_x2g_cgi_imsg_cgi_req_t message;

  memset(&message, 0, sizeof(message));

  if (arfcn_list != NULL)
  {
    message.arfcn_list = *arfcn_list;
  }

  message.bsic = bsic;
  message.time_available = time_available;
  if (info != NULL)
  {
    message.info = *info;
  }

  MSG_GERAN_HIGH_0_G("RR_X2G_CGI_IMSG_CGI_REQ:");
  MSG_GERAN_HIGH_1_G("  arfcn_list.count:       %d", message.arfcn_list.count);
  if (message.arfcn_list.count > 0)
  {
    uint32 i;
    for (i = 0; i < message.arfcn_list.count && i < RR_X2G_CGI_ARFCN_LIST_SIZE; ++i)
    {
      MSG_GERAN_HIGH_3_G("  arfcn_list.entries[%2d]: %d (band %d)",
        i,
        message.arfcn_list.entries[i].num,
        message.arfcn_list.entries[i].band);
    }
  }
  MSG_GERAN_HIGH_2_G("  bsic:                   %d, %d", bsic.PLMN_colour_code, bsic.BS_colour_code);
  MSG_GERAN_HIGH_1_G("  time_available:         %d", time_available);

  rr_rr_send(
    &message.header,
    (uint8) RR_X2G_CGI_IMSG_CGI_REQ,
    sizeof(message),
    RR_X2G_CGI_IMSG,
    gas_id
  );
}

const char * rr_x2g_cgi_omsg_name(
  rr_x2g_cgi_omsg_e omsg
)
{
  switch (omsg)
  {
    case RR_X2G_CGI_OMSG_CGI_CNF:
    {
      return "RR_X2G_CGI_OMSG_CGI_CNF";
    } /* RR_X2G_CGI_OMSG_CGI_CNF */
    
    case RR_X2G_CGI_OMSG_ABORT_CNF:
    {
      return "RR_X2G_CGI_OMSG_ABORT_CNF";
    } /* RR_X2G_CGI_OMSG_ABORT_CNF */

    default:
    {
      MSG_GERAN_ERROR_1("Unknown message: %d", omsg);
      return "?";
    }
  }
}


void rr_x2g_cgi_send_omsg(
  rr_x2g_cgi_omsg_e omsg,
  const gas_id_t gas_id
)
{
  rr_message_header_t header;

  rr_rr_send(
    &header,
    (uint8) omsg,
    sizeof(header),
    RR_X2G_CGI_OMSG,
    gas_id
  );
}

void rr_x2g_cgi_send_omsg_cgi_cnf(
  ARFCN_T             arfcn,
  BSIC_T              bsic,
  rr_x2g_cgi_info_t * info,
  const gas_id_t      gas_id
)
{
  rr_x2g_cgi_omsg_cgi_cnf_t cgi_cnf;

  memset(&cgi_cnf, 0, sizeof(cgi_cnf));

  cgi_cnf.arfcn = arfcn;
  cgi_cnf.bsic = bsic;
  if (info != NULL)
  {
    cgi_cnf.info = *info;
  }

  MSG_GERAN_HIGH_0_G("RR_X2G_CGI_OMSG_CGI_CNF:");
  MSG_GERAN_HIGH_2_G("  arfcn: %d (band %d)", cgi_cnf.arfcn.num, cgi_cnf.arfcn.band);
  MSG_GERAN_HIGH_2_G("  bsic:  %d, %d", cgi_cnf.bsic.PLMN_colour_code, cgi_cnf.bsic.BS_colour_code);
  if (info != NULL)
  {
    MSG_GERAN_HIGH_1_G("  cgi_cnf.info.mask: 0x%02x", cgi_cnf.info.mask);
    MSG_GERAN_HIGH_1_G("  cgi_cnf.info.cell_id: 0x%04x", cgi_cnf.info.cell_id);
    MSG_GERAN_HIGH_3_G("  cgi_cnf.info.lai.plmn_id: [ %02x, %02x, %02x ]",
             cgi_cnf.info.lai.plmn_id.identity[0],
             cgi_cnf.info.lai.plmn_id.identity[1],
             cgi_cnf.info.lai.plmn_id.identity[2]);
    MSG_GERAN_HIGH_1_G("  cgi_cnf.info.lai.location_area_code: 0x%04x", cgi_cnf.info.lai.location_area_code);
    if (cgi_cnf.info.rac_present)
    {
      MSG_GERAN_HIGH_0_G("  cgi_cnf.info.rac_present: TRUE");
      MSG_GERAN_HIGH_1_G("  cgi_cnf.info.rac: 0x%02x", cgi_cnf.info.rac);
    }
    else
    {
      MSG_GERAN_HIGH_0_G("  cgi_cnf.info.rac_present: TRUE");
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("  cgi_valid: FALSE");
  }

  rr_rr_send(
    &cgi_cnf.header,
    (uint8) RR_X2G_CGI_OMSG_CGI_CNF,
    sizeof(cgi_cnf),
    RR_X2G_CGI_OMSG,
    gas_id
  );
}

void rr_x2g_cgi_send_omsg_abort_cnf(
  ARFCN_T             arfcn,
  BSIC_T              bsic,
  rr_x2g_cgi_info_t * info,
  const gas_id_t      gas_id
)
{
  rr_x2g_cgi_omsg_abort_cnf_t abort_cnf;

  memset(&abort_cnf, 0, sizeof(abort_cnf));

  abort_cnf.arfcn = arfcn;
  abort_cnf.bsic = bsic;
  if (info != NULL)
  {
    abort_cnf.info = *info;
  }

  MSG_GERAN_HIGH_0_G("RR_X2G_CGI_OMSG_CGI_CNF:");
  MSG_GERAN_HIGH_2_G("  arfcn: %d (band %d)", abort_cnf.arfcn.num, abort_cnf.arfcn.band);
  MSG_GERAN_HIGH_2_G("  bsic:  %d, %d", abort_cnf.bsic.PLMN_colour_code, abort_cnf.bsic.BS_colour_code);
  if (info != NULL)
  {
    MSG_GERAN_HIGH_1_G("  abort_cnf.info.mask: 0x%02x", abort_cnf.info.mask);
    MSG_GERAN_HIGH_1_G("  abort_cnf.info.cell_id: 0x%04x", abort_cnf.info.cell_id);
    MSG_GERAN_HIGH_3_G("  abort_cnf.info.lai.plmn_id: [ %02x, %02x, %02x ]",
             abort_cnf.info.lai.plmn_id.identity[0],
             abort_cnf.info.lai.plmn_id.identity[1],
             abort_cnf.info.lai.plmn_id.identity[2]);
    MSG_GERAN_HIGH_1_G("  abort_cnf.info.lai.location_area_code: 0x%04x", abort_cnf.info.lai.location_area_code);
    if (abort_cnf.info.rac_present)
    {
      MSG_GERAN_HIGH_0_G("  abort_cnf.info.rac_present: TRUE");
      MSG_GERAN_HIGH_1_G("  abort_cnf.info.rac: 0x%02x", abort_cnf.info.rac);
    }
    else
    {
      MSG_GERAN_HIGH_0_G("  abort_cnf.info.rac_present: TRUE");
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("  cgi_valid: FALSE");
  }

  rr_rr_send(
    &abort_cnf.header,
    (uint8) RR_X2G_CGI_OMSG_ABORT_CNF,
    sizeof(abort_cnf),
    RR_X2G_CGI_OMSG,
    gas_id
  );
}

#endif /* FEATURE_LTE_TO_GSM_CGI */

/* EOF */

