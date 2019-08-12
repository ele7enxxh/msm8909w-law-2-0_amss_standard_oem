/*! \file rr_pscan_if.c 
 
  This module provides functions to send signals to/from the RR-PSCAN module
  (i.e. RR_PSCAN_IMSG, RR_PSCAN_OMSG).
 
                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_pscan_if.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_pscan_if.h"
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

/*!
 * \brief Sends RR_PSCAN_IMSG_START_IND.
 * 
 * \param search_mode 
 * \param band_pref 
 * \param gas_id 
 */
void rr_pscan_send_start_ind(
  rr_search_mode_T search_mode,
  sys_band_mask_type band_pref,
  const gas_id_t gas_id
)
{
  rr_pscan_start_ind_t msg;

  msg.band_pref = band_pref;
  msg.search_mode = search_mode;

  rr_rr_send(
    &msg.header,
    (uint8) RR_PSCAN_IMSG_START_IND,
    sizeof(msg),
    RR_PSCAN_IMSG,
    gas_id
  );
}

/*!
 * \brief Sends RR_PSCAN_IMSG_ABORT_REQ.
 * 
 * \param gas_id 
 */
void rr_pscan_send_abort_req(
  const gas_id_t gas_id
)
{
  rr_pscan_abort_req_t msg;

  rr_rr_send(
    &msg.header,
    (uint8) RR_PSCAN_IMSG_ABORT_REQ,
    sizeof(msg),
    RR_PSCAN_IMSG,
    gas_id
  );
}

/*!
 * \brief Sends RR_PSCAN_OMSG_FINISHED_IND.
 * 
 * \param l1_suspended (in) - TRUE if L1 suspended following completion of the pscan, FALSE otherwise
 * \param gas_id (in)
 */
void rr_pscan_send_finish_ind(
  boolean l1_suspended,
  const gas_id_t gas_id
)
{
  rr_pscan_finished_ind_t msg;

  msg.l1_suspended = l1_suspended;

  rr_rr_send(
    &msg.header,
    (uint8) RR_PSCAN_OMSG_FINISHED_IND,
    sizeof(msg),
    RR_PSCAN_OMSG,
    gas_id
  );
}

/*!
 * \brief Sends RR_PSCAN_OMSG_ABORT_CNF.
 * 
 * \param gas_id 
 */
void rr_pscan_send_abort_cnf(
  const gas_id_t gas_id
)
{
  rr_pscan_abort_cnf_t msg;

  rr_rr_send(
    &msg.header,
    (uint8) RR_PSCAN_OMSG_ABORT_CNF,
    sizeof(msg),
    RR_PSCAN_OMSG,
    gas_id
  );
}

/* EOF */

