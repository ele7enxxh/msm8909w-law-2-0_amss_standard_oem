#ifndef RR_BAND_AVOID_H
#define RR_BAND_AVOID_H
/*============================================================================
  @file rr_band_avoid.c

  @brief This module implements a state machine that manages band avoidance for
   multi-SIM

   CXM provides blacklisted frquencies which GERAN attemps to avoid by
   manipulating measurement reports to force the NW to handover to a non 
   blacklisted band.  If GERAN cannot avoid because there are no suitable 
   neighbours or the max number of measurement reports are sent a NACK
   is indicated to CXM.

                Copyright (c) 2014 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_band_avoid.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_BAND_AVOIDANCE

#include "comdef.h"
#include "rr_defs.h"

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/
void rr_band_avoid_modify_gsm_meas_rpt(
               surrounding_measurement_T * gsm_measurements,
               ARFCN_T                   serving_cell_arfcn,
               uint8                     * rx_lev,
               uint8                     * rx_lev_optional,
               uint8                     * rx_qual,
               uint8                     * rx_qual_optional,
               uint8                     * mean_bep,
               uint8                     * cv_bep,
               uint8                     * nbr_rcvd_blocks,
               const gas_id_t              gas_id);

uint32 rr_band_avoid_arfcn_to_centre_freq_kHz(ARFCN_T arfcn, const gas_id_t gas_id);

void rr_band_avoidance_control(rr_cmd_bdy_type * message, const gas_id_t gas_id);
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/
#endif /*RR_BAND_AVOID_H*/
