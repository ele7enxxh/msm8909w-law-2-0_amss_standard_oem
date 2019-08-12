/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              rrcrrif.c

GENERAL DESCRIPTION

   This module contains functions to be used by RRC to send RR messages.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_rrc_if.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/18/03   JAC      Lint changes
02/14/03   RG       Added support for G2W blind Handover
08/13/02   Rao      Fixed the Msg Length problem, byte is not sufficient for
                    InterRAT Handover Req.
07/22/02   Rao      Initial Revision.

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_WCDMA

#include <string.h>
#include "ms.h"
#include "rr_seg_load.h"
#include "rr_rrc_if.h"
#include "gs.h"
#include "msg.h"
#include "err.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "rr_general.h"
#include "rr_pscan.h"
#include "rr_acq_db.h"
#include "rr_multi_sim.h"
#include "rr_plmn_list.h"

void rr_send_rrc_message_to_rr(
  void *            message_ptr,
  rrc_rr_cmd_e_type rrc_cmd_id,
  word              length
)
{
  gs_status_T  gs_status;
  IMH_T *imh_ptr = (IMH_T *) message_ptr;

  imh_ptr->message_id = (byte) rrc_cmd_id;
  imh_ptr->message_set = MS_RRC_RR;

  PUT_IMH_LEN((length - sizeof(IMH_T)),imh_ptr);

  gs_status = gs_send_message(GS_QUEUE_RR, (void *) message_ptr, TRUE);
  if (gs_status != GS_SUCCESS)
  {
     MSG_GERAN_ERROR_1("Unable to send message, gs status %d", gs_status);
  }

  return;
}


void rr_rrc_set_band_pref_sub_id(sys_band_mask_type band_pref, sys_modem_as_id_e_type as_id)
{
  rr_PLMN_request_details_T *plmn_request_details_ptr;
  gas_id_t                   gas_id = geran_map_nas_id_to_gas_id(as_id);

  plmn_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  MSG_GERAN_HIGH_2_G("RRC sets band_pref from 0x%08X to 0x%08X", rr_band_pref(gas_id), band_pref);

  if (plmn_request_details_ptr != NULL)
  {
    plmn_request_details_ptr->rat_pri_list_info.num_items = 1;
    plmn_request_details_ptr->rat_pri_list_info.next_acq_sys_index = 0;
    plmn_request_details_ptr->rat_pri_list_info.priority_list_info[0].band_cap.chgwt_band_cap = band_pref;
    plmn_request_details_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_GSM;
    plmn_request_details_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_time_interval = 0;
  }

  rr_set_preferred_bands(band_pref, gas_id);
}

/* wrapper function for legacy codebases */
void rr_rrc_set_band_pref(sys_band_mask_type band_pref)
{
  rr_rrc_set_band_pref_sub_id(band_pref, SYS_MODEM_AS_ID_1);

  return;
}

void rr_rrc_set_bst_band_pref_sub_id(sys_band_mask_type band_pref, sys_modem_as_id_e_type as_id)
{
  rr_plc_set_band_filter(band_pref, geran_map_nas_id_to_gas_id(as_id));
}

void rr_rrc_set_bst_band_pref(sys_band_mask_type band_pref)
{
  rr_rrc_set_bst_band_pref_sub_id(band_pref, SYS_MODEM_AS_ID_1);
}

sys_band_T rr_rrc_get_arfcn_band_sub_id(
  uint16                       arfcn,
  rrc_rr_frequence_band_e_type band_ind,
  sys_modem_as_id_e_type       as_id
)
{
  rr_internal_band_T rr_band;
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);

  if (band_ind == RRC_RR_PCS_1900)
  {
    rr_band = rr_get_band_using_band_indicator((word)arfcn, SI_PSI_BAND_IND_PCS1900, gas_id);
  }
  else
  {
    rr_band = rr_get_band_using_band_indicator((word)arfcn, SI_PSI_BAND_IND_DCS1800, gas_id);
  }

  /* map the RR internal band type to a system level definition */
  switch(rr_band)
  {
    case BAND_EGSM:
      return SYS_BAND_EGSM_900;

    case BAND_PGSM:
      return SYS_BAND_PGSM_900;

    case BAND_PCS:
      return SYS_BAND_PCS_1900;

    case BAND_DCS:
      return SYS_BAND_DCS_1800;

    case BAND_GSM_850:
      return SYS_BAND_CELL_850;

    default:
      break;
  }

  return INVALID_BAND;
}

/* wrapper function for legacy codebases */
sys_band_T rr_rrc_get_arfcn_band(
  uint16                       arfcn,
  rrc_rr_frequence_band_e_type band_ind)
{
  return(rr_rrc_get_arfcn_band_sub_id(arfcn, band_ind, SYS_MODEM_AS_ID_1));
}


PLMN_id_T rrc_get_rr_curr_camped_plmn_sub_id(sys_modem_as_id_e_type as_id)
{
  gprs_scell_info_t *scell_info_ptr = NULL;
  PLMN_id_T          plmn_id;
  boolean            rr_camped;
  gas_id_t           gas_id = GERAN_ACCESS_STRATUM_ID_1;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  gas_id = geran_map_nas_id_to_gas_id(as_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  rr_camped = rr_is_camped(gas_id);

  if (rr_camped)
  {
    plmn_id = scell_info_ptr->gsm.location_area_identification.plmn_id;
  }
  else
  {
    plmn_id.identity[0] = 0xFF;
    plmn_id.identity[1] = 0xFF;
    plmn_id.identity[2] = 0xFF;
  }

  return plmn_id;
}

/* wrapper function for legacy codebases */
PLMN_id_T rrc_get_rr_curr_camped_plmn(void)
{
  return(rrc_get_rr_curr_camped_plmn_sub_id(SYS_MODEM_AS_ID_1));
}

void rr_rrc_acq_db_save_sub_id(sys_modem_as_id_e_type as_id)
{
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);

  rr_acq_db_save(gas_id);

  return;
}

/* wrapper function for legacy codebases */
void rr_rrc_acq_db_save(void)
{
  rr_rrc_acq_db_save_sub_id(SYS_MODEM_AS_ID_1);

  return;
}

#else
/* no WCDMA support in this build */
int rr_rrc_interface_supported(void)
{
  return(0);
}

#endif /* #ifdef FEATURE_WCDMA */
