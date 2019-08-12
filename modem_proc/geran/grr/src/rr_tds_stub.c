/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              rr_tds_stubs.c

GENERAL DESCRIPTION

   This module contains functions to be used by TDS RRC to send RR messages.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_tds_stub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/17/11   ps      Initial Revision.

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"

#if defined (FEATURE_TDSCDMA) && !defined (FEATURE_GSM_TDS)

#include "ms.h"
#include "sys.h"
#include "sys_v.h"
#include "rr_seg_load.h"


#include "tdsrrc_rr_if.h"
#include "rr_tdsrrc_if.h"

void tdsrr_send_rrc_message_to_rr(
  void *            message_ptr,
  tdsrr_rrc_cmd_e_type rrc_cmd_id,
  word              length
)
{
  //Stubbed out
  MSG_GERAN_HIGH_0_G("tdsrr_send_rrc_message_to_rr stubbed out");
  return;
}


/**
  @brief Get the band for a GSM ARFCN given the band indicator value provided.

  This function is used to determine the band that the specified ARFCN is a
  member of, using the provided band indicator to resolve any conflict if the
  ARFCN is in the DCS 1800/PCS 1900 overlap region.

  @param arfcn:    The ARFCN that the caller requires the band of.
  @param band_ind: Band indicator value, as received OTA in UMTS.  This
                   parameter is unused if the ARFCN is not in DCS 1800/PCS
                   1900 band.
  @return:         The band that ARFCN is in, or INVALID_BAND if it is in a
                   band that is not currently supported by RR.
*/
sys_band_T tdsrr_get_arfcn_band(
  uint16                       arfcn,
  tdsrr_frequence_band_e_type band_ind
)
{


  MSG_GERAN_HIGH_0_G("tdsrr_get_arfcn_band stubbed out");
  //Stubbed out for now for compilation
  return INVALID_BAND;
}

/**
  @brief Interface used by RRC to update RR's band_pref.

  This function should be called whenever RRC receives a new band_pref from MM.

  @param band_pref: The band_pref for RR to set.
*/
void tdsrr_set_band_pref(
  sys_band_mask_type band_pref
)
{
  //stubbed out for now..

  MSG_GERAN_HIGH_0_G("tdsrr_set_band_pref stubbed out");
  return;
}
#endif /*FEATURE_TDSCDMA && !FEATURE_GSM_TDS*/

