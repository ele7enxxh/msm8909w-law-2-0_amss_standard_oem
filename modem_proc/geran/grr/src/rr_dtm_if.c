/*! \file rr_dtm_if.c 
 
  This module provides functions to send signals to/from the RR-DTM module.
 
                Copyright (c) 2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_dtm_if.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_DTM

#include "comdef.h"
#include "rr_dtm_if.h"
#include "rr_rr.h"
#include "rr_multi_sim.h"
#include "rr_nv.h"

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

// See header file for documentation
const char *rr_dtm_imsg_name(rr_dtm_imsg_e imsg)
{
  switch (imsg)
  {
    case RR_DTM_IMSG_CS_RELEASE_REQ: return "RR_DTM_IMSG_CS_RELEASE_REQ";
    default: return "?";
  }
}

// See header file for documentation
void rr_dtm_send_imsg_cs_release_req(
  boolean edtm_cs_release_allowed,
  const gas_id_t gas_id
)
{
  if (rr_get_nv_dtm_enabled(RR_GAS_ID_TO_AS_ID))
  {
    rr_dtm_imsg_cs_release_req_t msg;

    memset(&msg, 0, sizeof(msg));

    msg.edtm_cs_release_allowed = edtm_cs_release_allowed;

    rr_rr_send(
      &msg.header,
      (uint8) RR_DTM_IMSG_CS_RELEASE_REQ,
      sizeof(msg),
      RR_DTM_IMSG,
      gas_id
    );
  }
  else
  {
    rr_dtm_send_omsg_cs_release_cnf(
      RR_DTM_CE_REL_MODE_GSM,   // ce_rel_mode
      gas_id                    // gas_id
    );
  }
}

// See header file for documentation
const char *rr_dtm_omsg_name(rr_dtm_omsg_e omsg)
{
  switch (omsg)
  {
    case RR_DTM_OMSG_CS_RELEASE_CNF: return "RR_DTM_OMSG_CS_RELEASE_CNF";
    default: return "?";
  }
}

// See header file for documentation
void rr_dtm_send_omsg_cs_release_cnf(
  rr_dtm_ce_rel_mode_e ce_rel_mode,
  const gas_id_t gas_id
)
{
  rr_dtm_omsg_cs_release_cnf_t msg;

  memset(&msg, 0, sizeof(msg));

  msg.ce_rel_mode = ce_rel_mode;

  MSG_GERAN_HIGH_1_G(
    "OMSG_CS_RELEASE_CNF: ce_rel_mode=%d",
    (int)msg.ce_rel_mode
  );

  rr_rr_send(
    &msg.header,
    (uint8) RR_DTM_OMSG_CS_RELEASE_CNF,
    sizeof(msg),
    RR_DTM_OMSG,
    gas_id
  );
}

#endif // FEATURE_GSM_DTM

/* EOF */

