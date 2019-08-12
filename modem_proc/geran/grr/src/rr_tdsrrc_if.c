/*===========================================================================
         TDSRRC  -  RR   INTERFACE

DESCRIPTION

  This module is the header module that contains the type definitions for
  the primitives sent to GERAN RR from TD-SCDMA RRC. The module is only used for dual-mode.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_tdsrrc_if.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
24/02/11   rmn     Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_TDS
#include "comdef.h"
#include <string.h>
#include "ms.h"
#include "rr_seg_load.h"
#include "rr_rrc_if.h"
#include "gs.h"
#include "msg.h"
#include "err.h"
#include "rr_multiband.h"
#include "rr_general.h"
#include "rr_pscan.h"
#include "rr_acq_db.h"
#include "tdsrrc_rr_if.h"
#include "rr_tdsrrc_if.h"
#include "geran_multi_sim.h"
#include "rr_plmn_list.h"

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Function Declarations
** ----------------------------------------------------------------------- */
/**
  @brief Interface function used by TDSRRC to send messages to RR.

  This function may be used by TDSRRC to put a message on RR's message queue.

  @param message_ptr: Pointer to the RRC message to be sent to RR.
  @param rrc_cmd_id:  Message Id
  @param length:      Message Length.
*/
void tdsrr_send_rrc_message_to_rr(
  void *                message_ptr,
  tdsrr_rrc_cmd_e_type  rrc_cmd_id,
  word                  length
)
{
  gs_status_T  gs_status;
  IMH_T *imh_ptr = (IMH_T *) message_ptr;

  imh_ptr->message_id = (byte) rrc_cmd_id;
  imh_ptr->message_set = MS_TDSRRC_RR;

  PUT_IMH_LEN((length - sizeof(IMH_T)),imh_ptr);

  gs_status = gs_send_message(GS_QUEUE_RR, (void *) message_ptr, TRUE);
  if (gs_status != GS_SUCCESS)
  {
     MSG_GERAN_ERROR_1("Unable to send message, gs status %d", gs_status);
  }
}


/**
  @brief Get the band for a GSM ARFCN given the band indicator value provided.

  This function is used to determine the band that the specified ARFCN is a
  member of, using the provided band indicator to resolve any conflict if the
  ARFCN is in the DCS 1800/PCS 1900 overlap region.

  @param arfcn:    The ARFCN that the caller requires the band of.
  @param band_ind: Band indicator value, as received OTA in
                   UTRAN TDD. This parameter is unused if the
                   ARFCN is not in DCS 1800/PCS 1900 band.
  @param as_id:    Subscriber ID.
  @return:         The band that ARFCN is in, or INVALID_BAND if it is in a
                   band that is not currently supported by RR.
*/
sys_band_T tdsrr_get_arfcn_band_sub_id(
  uint16                       arfcn,
  tdsrr_frequence_band_e_type  band_ind,
  sys_modem_as_id_e_type       as_id
)
{
  rr_internal_band_T rr_band;

  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);

  if (band_ind == TDSRR_PCS_1900)
  {
    rr_band = rr_get_band_using_band_indicator(
      (word)arfcn,
      SI_PSI_BAND_IND_PCS1900,
      gas_id
    );
  }
  else
  {
    rr_band = rr_get_band_using_band_indicator(
      (word)arfcn,
      SI_PSI_BAND_IND_DCS1800,
      gas_id
    );
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

/**
  @brief Get the band for a GSM ARFCN given the band indicator value provided.

  This function is used to determine the band that the specified ARFCN is a
  member of, using the provided band indicator to resolve any conflict if the
  ARFCN is in the DCS 1800/PCS 1900 overlap region.

  @param arfcn:    The ARFCN that the caller requires the band of.
  @param band_ind: Band indicator value, as received OTA in
                   UTRAN TDD. This parameter is unused if the
                   ARFCN is not in DCS 1800/PCS 1900 band.
  @return:         The band that ARFCN is in, or INVALID_BAND if it is in a
                   band that is not currently supported by RR.
*/
sys_band_T tdsrr_get_arfcn_band(
  uint16                       arfcn,
  tdsrr_frequence_band_e_type  band_ind
)
{
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
  MSG_GERAN_ERROR_0("SS API called - use tdsrr_get_arfcn_band_sub_id()");
#endif // FEATURE_TDS_SUB_ID_INTERFACE

  return tdsrr_get_arfcn_band_sub_id(arfcn, band_ind, SYS_MODEM_AS_ID_1);
}

/**
  @brief Interface used by TDS-RRC to update RR's band_pref.

  This function should be called whenever TDS-RRC receives a new band_pref from MM.

  @param band_pref: The band_pref for RR to set.
  @param as_id:     Subscriber ID.
*/
void tdsrr_set_band_pref_sub_id(sys_band_mask_type band_pref, sys_modem_as_id_e_type as_id)
{
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);

  rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);

  MSG_GERAN_HIGH_2_G("TDSRRC sets band_pref from 0x%08x to 0x%08x", rr_band_pref(gas_id), band_pref);

  plmn_request_details->rat_pri_list_info.num_items = 1;
  plmn_request_details->rat_pri_list_info.next_acq_sys_index = 0;
  plmn_request_details->rat_pri_list_info.priority_list_info[0].band_cap.chgwt_band_cap = band_pref;
  plmn_request_details->rat_pri_list_info.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_GSM;
  plmn_request_details->rat_pri_list_info.priority_list_info[0].acq_sys_time_interval = 0;

  rr_set_preferred_bands(rr_band_pref(gas_id), gas_id);
}

/**
  @brief Interface used by TDS-RRC to update RR's band_pref.

  This function should be called whenever TDS-RRC receives a new band_pref from MM.

  @param band_pref: The band_pref for RR to set.
*/
void tdsrr_set_band_pref(sys_band_mask_type band_pref)
{
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
  MSG_GERAN_ERROR_0("SS API called - use tdsrr_set_band_pref_sub_id()");
#endif // FEATURE_TDS_SUB_ID_INTERFACE

  tdsrr_set_band_pref_sub_id(band_pref, SYS_MODEM_AS_ID_1);
  return;
}

/**
  @brief Interface used by TDS-RRC to update RR's BST band_pref.

  This function should be called whenever TDS-RRC receives a new BST band_pref from MM.

  @param band_pref: The band_pref for RR to set.
  @param as_id:     Subscriber ID.
*/
void tdsrr_set_bst_band_pref_sub_id(sys_band_mask_type band_pref, sys_modem_as_id_e_type as_id)
{
  rr_plc_set_band_filter(band_pref, geran_map_nas_id_to_gas_id(as_id));
  return;
}

/**
  @brief Interface used by TDS-RRC to update RR's BST band_pref.

  This function should be called whenever TDS-RRC receives a new BST band_pref from MM.

  @param band_pref: The band_pref for RR to set.
*/
void tdsrr_set_bst_band_pref(sys_band_mask_type band_pref)
{
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
  MSG_GERAN_ERROR_0("SS API called - use tdsrr_set_bst_band_pref_sub_id()");
#endif // FEATURE_TDS_SUB_ID_INTERFACE

  tdsrr_set_bst_band_pref_sub_id(band_pref, SYS_MODEM_AS_ID_1);
  return;
}

/**
  @brief Interface used by TDSRRC to get GSM camped PLMN id.

  This function may be used to access the PLMN of the cell that RR is currently
  camped on.
   
  @param band_pref: Subscriber ID.
 
  @return: PLMN ID of the cell that RR is camped on, or (0xFF 0xFF 0xFF) is
           returned if RR is not camped.
*/
PLMN_id_T tdsrr_get_rr_curr_camped_plmn_sub_id(sys_modem_as_id_e_type as_id)
{
  PLMN_id_T          plmn_id;

  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);

  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  if (rr_is_camped(gas_id))
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

/**
  @brief Interface used by TDSRRC to get GSM camped PLMN id.

  This function may be used to access the PLMN of the cell that RR is currently
  camped on.

  @return: PLMN ID of the cell that RR is camped on, or (0xFF 0xFF 0xFF) is
           returned if RR is not camped.
*/
PLMN_id_T tdsrr_get_rr_curr_camped_plmn(void)
{
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
  MSG_GERAN_ERROR_0("SS API called - use tdsrr_get_rr_curr_camped_plmn_sub_id()");
#endif // FEATURE_TDS_SUB_ID_INTERFACE

  return tdsrr_get_rr_curr_camped_plmn_sub_id(SYS_MODEM_AS_ID_1);
}


/**
  @brief Save RR's acquisition database to NV.
   
  @param band_pref: Subscriber ID.

  This function must be called by TDSRRC at power-off to write
  RR's acquisition database to NV.
*/
void tdsrr_acq_db_save_sub_id(sys_modem_as_id_e_type as_id)
{
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);

  rr_acq_db_save(gas_id);
  return;
}

/**
  @brief Save RR's acquisition database to NV.

  This function must be called by TDSRRC at power-off to write
  RR's acquisition database to NV.
*/
void tdsrr_acq_db_save(void)
{
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
  MSG_GERAN_ERROR_0("SS API called - use tdsrr_acq_db_save_sub_id()");
#endif // FEATURE_TDS_SUB_ID_INTERFACE

  tdsrr_acq_db_save_sub_id(SYS_MODEM_AS_ID_1);
  return;
}

#endif /* FEATURE_GSM_TDS */

