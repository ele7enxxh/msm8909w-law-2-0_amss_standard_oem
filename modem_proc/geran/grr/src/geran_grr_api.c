/**
  @file geran_grr_api.c

  @brief Defines GRR functions available to software outside GERAN.
*/

/*============================================================================

  Copyright (c) 2010-2014 Qualcomm Technologies, Inc.

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/geran_grr_api.c#1 $ */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/10   gjohn   Initial revision
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 *-------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#include "comdef.h"
#include "geran_grr_api.h"
#include "rr_arfcn.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "rr_nv.h"
#include "rr_public_store.h" 
#include "geran_api_rr_l2.h"
#include "rr_l2.h"
#include "geran_eng_mode_info.h"
#include "rr_general.h"
#include "geran_multi_sim.h"
#include "rr_gprs_defs.h"
#include "grr_mutex.h"
#include "rr_l1_send.h"
#include "rr_lsm_if.h"

/*---------------------------------------------------------------------------
 * Public Function Definitions
 *-------------------------------------------------------------------------*/

sys_sband_e_type geran_grr_get_arfcn_band(
  uint16                 arfcn,
  geran_grr_band_ind_e   band_ind,
  boolean              * is_band_valid
)
{
  sys_sband_e_type band = SYS_SBAND_MAX;

  /* Lint complains about comparison always being TRUE
  for >= RR_ARFCN_EGSM_900_LOWER_RANGE_MIN as arfcn is unsigned. */
  if (/*arfcn >= RR_ARFCN_EGSM_900_LOWER_RANGE_MIN && */
      arfcn <= RR_ARFCN_EGSM_900_LOWER_RANGE_MAX)
  {
    if (arfcn >= RR_ARFCN_PGSM_900_MIN)
    {
      band = SYS_SBAND_GSM_PGSM_900;
    }
    else
    {
      band = SYS_SBAND_GSM_EGSM_900;
    }
  }
  else if (arfcn >= RR_ARFCN_EGSM_900_UPPER_RANGE_MIN &&
           arfcn <= RR_ARFCN_EGSM_900_UPPER_RANGE_MAX)
  {
    band = SYS_SBAND_GSM_EGSM_900;
  }
  else if (arfcn >= RR_ARFCN_CELL_850_MIN &&
           arfcn <= RR_ARFCN_CELL_850_MAX)
  {
    band = SYS_SBAND_GSM_850;
  }
  else if (arfcn >= RR_ARFCN_DCS_1800_MIN &&
           arfcn <= RR_ARFCN_DCS_1800_MAX)
  {
    if (arfcn <= RR_ARFCN_PCS_1900_MAX)
    {
      if (band_ind == GERAN_GRR_BAND_IND_PCS_1900)
      {
        band = SYS_SBAND_GSM_PCS_1900;
      }
      else if (band_ind == GERAN_GRR_BAND_IND_DCS_1800)
      {
        band = SYS_SBAND_GSM_DCS_1800;
      }
    }
    else
    {
      band = SYS_SBAND_GSM_DCS_1800;
    }
  }

  if (is_band_valid != NULL)
  {
    if (band == SYS_SBAND_MAX)
    {
      *is_band_valid = FALSE;
    }
    else
    {
      *is_band_valid = TRUE;
    }
  }
  return band;
}

ARFCN_T geran_grr_convert_to_arfcn_type(
  uint16 arfcn,
  geran_grr_band_ind_e band_ind
)
{
  ARFCN_T          output_arfcn;
  boolean          is_band_valid;
  sys_sband_e_type band_as_sys_sband;
  #ifdef FEATURE_DUAL_SIM
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;
  #endif /*FEATURE_DUAL_SIM */

  output_arfcn.num = arfcn;

  band_as_sys_sband = geran_grr_get_arfcn_band(arfcn, band_ind, &is_band_valid);

  if (is_band_valid)
  {
    switch (band_as_sys_sband)
    {
      case SYS_SBAND_GSM_PGSM_900:
      {
        if (rr_is_supported_band(BAND_EGSM))
        {
          output_arfcn.band = SYS_BAND_EGSM_900;
        }
        else
        {
          output_arfcn.band = SYS_BAND_PGSM_900;
        }
        break;
      } /* SYS_SBAND_GSM_PGSM_900 */

      case SYS_SBAND_GSM_EGSM_900:
      {
        output_arfcn.band = SYS_BAND_EGSM_900;
        break;
      } /* SYS_SBAND_GSM_EGSM_900 */

      case SYS_SBAND_GSM_850:
      {
        output_arfcn.band = SYS_BAND_CELL_850;
        break;
      } /* SYS_SBAND_GSM_850 */

      case SYS_SBAND_GSM_DCS_1800:
      {
        output_arfcn.band = SYS_BAND_DCS_1800;
        break;
      } /* SYS_SBAND_GSM_850 */

      case SYS_SBAND_GSM_PCS_1900:
      {
        output_arfcn.band = SYS_BAND_PCS_1900;
        break;
      } /* SYS_SBAND_GSM_850 */

      default:
      {
        MSG_GERAN_ERROR_1_G("Unexpected band_as_sys_sband: %d", band_as_sys_sband);
        output_arfcn.band = INVALID_BAND;
      }
    }
  }
  else
  {
    output_arfcn.band = INVALID_BAND;
  }

  return output_arfcn;
}

/* This function replaces the macro ARFCNS_EQUAL for GERAN clients as
   the macro includes conditional compilation */
boolean arfcns_equal(ARFCN_T arfcn1, ARFCN_T arfcn2)
{
  return ARFCNS_SAME_CHANNEL_AND_BAND(arfcn1, arfcn2);
}

/**
   @brief Access fn for cfa_send_event_if_neccessary()

   @param message_id  RR/L2 message ID
   @param msg_ptr pointer to RR/L2 message

   @return TRUE if message_ID is an OTA message
   @return fpos contains FPOS of layer3_message[0] in the message
   @return isTx is TRUE if message_id is a req, FALSE if it's an ind
*/
boolean geran_grr_get_l3_msg_FPOS(const byte message_id, const byte * msg_ptr, uint32 *fpos, boolean *isTx)
{
  boolean result = TRUE;
  l2_channel_type_T channel_type;

  switch ((rr_l2_message_id_enum_T)message_id)
  {
    /* Messages containing OTA to send */
    case DL_DATA_REQ:
      {
        *fpos = FPOS(dl_data_req_T, layer3_message[0]);
        channel_type = ((dl_data_req_T *)msg_ptr)->l2_channel_type;
        *isTx  = TRUE;
        break;
      }
    case DL_RECONNECT_REQ:
      {
        *fpos = FPOS(dl_reconnect_req_T, layer3_message[0]);
        channel_type = ((dl_reconnect_req_T *)msg_ptr)->l2_channel_type;
        *isTx  = TRUE;
        break;
      }
    case DL_RESUME_REQ:
      {
        *fpos = FPOS(dl_resume_req_T, layer3_message[0]);
        channel_type = ((dl_resume_req_T *)msg_ptr)->l2_channel_type;
        *isTx  = TRUE;
        break;
      }
    case DL_UNIT_DATA_REQ:
      {
        *fpos = FPOS(dl_unit_data_req_T, layer3_message[0]);
        channel_type = ((dl_unit_data_req_T *)msg_ptr)->l2_channel_type;
        *isTx  = TRUE;
        break;
      }
    case DL_ESTABLISH_REQ:
      {
        *fpos = FPOS(dl_establish_req_T, layer3_message[0]);
        channel_type = ((dl_establish_req_T *)msg_ptr)->l2_channel_type;
        *isTx  = TRUE;
        break;
      }

    /* Messages containing OTA received */
    case DL_DATA_IND:
      {
        *fpos = FPOS(dl_data_ind_T, layer3_message[0]);
        channel_type = ((dl_data_ind_T *)msg_ptr)->l2_channel_type;
        *isTx  = FALSE;
        break;
      }
    case DL_UNIT_DATA_IND:
      {
        *fpos = FPOS(dl_unit_data_ind_T, layer3_message[0]);
        channel_type = ((dl_unit_data_ind_T *)msg_ptr)->l2_channel_type;
        *isTx  = FALSE;
        break;
      }
    default:
        result = FALSE;
        break;
    }/* end message_id */

  if (result)
  {
    if( DCCH  != channel_type   &&
        SACCH != channel_type   &&
        SDCCH != channel_type   &&
        FACCH_F != channel_type &&
        FACCH_H != channel_type)
    {   /* Compensate for pseudo-length field */
      (*fpos)++;
    }
  }

    return result;
}

/**
   @brief Access fn for ghdi_mvs_cb_func() / ghdi_reset_sr_and_inform_AS()

   @param speech_enc_samp_freq  8000: narrow-band or 16000: wide-band, 0 - unknown: ignore
   @param call_est;             new call established, ignore speech_enc_samp_freq
   @return TRUE if the sample rate was written

*/
boolean geran_grr_set_codec_ho_sample_rate_per_subs(
  uint32 speech_enc_samp_freq,
  boolean call_est,
  sys_modem_as_id_e_type as_id
)
{
#if defined(FEATURE_ENHANCED_AMR) && defined (ENG_MODE_CODEC_HO_SAMPLE_RATE)
  geran_eng_mode_codec_ho_sample_rate_t param_copy;
  param_copy.speech_enc_samp_freq = speech_enc_samp_freq;
  param_copy.call_est = call_est;

  if (speech_enc_samp_freq != 8000 && speech_enc_samp_freq != 16000)
  {
    MSG_GERAN_ERROR_0("geran_api_set_codec_ho_sample_rate arg bad value");
    return FALSE;
  }

  // TODO - Eng Mode does not support MultiSIM yet
  (void)as_id;

  geran_eng_mode_data_write(
    ENG_MODE_CODEC_HO_SAMPLE_RATE,
    &param_copy,
    TRUE
  );

  return TRUE;
#else
  MSG_GERAN_ERROR_0("geran_api_set_codec_ho_sample_rate not implemented");
  return FALSE;
#endif
}

boolean geran_grr_set_codec_ho_sample_rate(
  uint32 speech_enc_samp_freq,
  boolean call_est
)
{
  // For legacy / SingleSIM use, assume Sub1
  return geran_grr_set_codec_ho_sample_rate_per_subs(speech_enc_samp_freq, call_est, SYS_MODEM_AS_ID_1);
}

/*!
 * \brief Returns the last timing advance paramters.
 *  
 * Note: This is a public API and so is guarded by a mutex lock.
 * 
 * \param as_id (in)
 * \param p_TimingAdvance (out)
 */
void geran_grr_get_current_timing_advance_info_per_subs(
  geran_grr_current_timing_advance_type *p_TimingAdvance,
  sys_modem_as_id_e_type as_id
)
{
  GRR_GENERAL_LOCK();

  p_TimingAdvance->timing_advance_valid = rr_l1_send_get_current_timing_advance_info(
    &p_TimingAdvance->timing_advance,   // timing_advance_ptr
    RR_AS_ID_TO_GAS_ID                  // gas_id
  );

  GRR_GENERAL_UNLOCK();

  return;
}

/*!
 * \brief Returns the last timing advance paramters.
 *  
 * Note: This is a public API and so is guarded by a mutex lock.
 * 
 * \param p_TimingAdvance (out)
 */
void geran_grr_get_current_timing_advance_info(geran_grr_current_timing_advance_type *p_TimingAdvance)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  MSG_GERAN_ERROR_0("SingleSIM API being called - as_id=0 assumed");
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  geran_grr_get_current_timing_advance_info_per_subs(p_TimingAdvance, SYS_MODEM_AS_ID_1);

  return;
}

/*!
 * \brief Gets the current camped on status, cell identity, LAI, BSIC and ARFCN. 
 *  
 * Note: This function is called from outside RR's context by the LSM task.
 * 
 * \param as_id (in)
 * \param p_BsInfo (out)
 */
void geran_grr_get_current_bs_info_per_subs(
  sys_modem_as_id_e_type as_id,
  geran_grr_current_bs_info_type *info_ptr
)
{
  gprs_system_info_database_t *db_ptr = NULL;
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);

  GRR_SYSINFO_LOCK();

  /* Check the validity of database before accessing it. */
  if (rr_get_gprs_system_info_database_valid(gas_id))
  {
    db_ptr = rr_get_gprs_system_info_database_ptr(gas_id);
  }

  /* copy the required info to the output pointer */
  if (info_ptr != NULL)
  {
    if (db_ptr != NULL)
    {
      info_ptr->camped_on_flag = db_ptr->scell.gsm.camped_on_flag;
      info_ptr->cell_identity = db_ptr->scell.gsm.cell_identity;
      info_ptr->location_area_identification = db_ptr->scell.gsm.location_area_identification;
      info_ptr->arfcn = db_ptr->scell.gsm.BCCH_ARFCN;
      info_ptr->bsic = (uint8)((db_ptr->scell.gsm.BSIC.PLMN_colour_code << 3) +
                             (db_ptr->scell.gsm.BSIC.BS_colour_code));
    }
    else
    {
      memset(info_ptr, 0xFF, sizeof(geran_grr_current_bs_info_type));
      info_ptr->camped_on_flag = FALSE;
    }
  }

  GRR_SYSINFO_UNLOCK();

  return;
}

/*!
 * \brief Gets the current camped on status, cell identity, LAI, BSIC and ARFCN. 
 *  
 * Note: This function is called from outside RR's context by the LSM task.
 * 
 * \param p_BsInfo (out)
 */
void geran_grr_get_current_bs_info(geran_grr_current_bs_info_type *p_BsInfo)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  MSG_GERAN_ERROR_0("SingleSIM API being called - as_id=0 assumed");
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  geran_grr_get_current_bs_info_per_subs(SYS_MODEM_AS_ID_1, p_BsInfo);

  return;
}

/*!
 * \brief This API Maps Engineering Mode Band Information to correct System Band Class.
 * 
 * \param gsm_band:  Engineering Mode Band Information (in)
 * 
 * \return sys_sband_e_type: System Band Class
 */
sys_sband_e_type geran_map_gsm_band_to_sys_band_class(eng_mode_band_t gsm_band)
{
  return(rr_map_sys_band_to_sys_band_class((sys_band_T)gsm_band));
}

/*!
 * \brief Determines whether the opposite ASID has a GPRS data call in progress
 *
 * \param as_id (in)
 * \return boolean - TRUE if GPRS activity is detected on the opposite 
subscription
 */
boolean geran_grr_is_opposite_sub_in_data_call(const sys_modem_as_id_e_type 
as_id)
{
#ifdef FEATURE_DUAL_SIM
  gas_id_t gas_id;

  /* convert the supplied ASID into a GAS ID */
  gas_id = geran_map_nas_id_to_gas_id(as_id);

  /* and then check the MAC status of the opposite stack */
  if (GERAN_ACCESS_STRATUM_ID_1 != gas_id)
  {
    return(MAC_IDLE != rr_macinfo_get_mac_status(GERAN_ACCESS_STRATUM_ID_1));
  }
  else
  {
    return(MAC_IDLE != rr_macinfo_get_mac_status(GERAN_ACCESS_STRATUM_ID_2));
  }
#else
  NOTUSED(as_id);
#endif /* FEATURE_DUAL_SIM */

  return(FALSE);
}

/*!
 * \brief Determines whether the opposite ASID has a GPRS data transfer state or not.
 *
 * \param as_id (in)
 * \return boolean - TRUE if GPRS Transfer Mode activity is detected on the opposite 
subscription
 */
boolean geran_grr_is_opposite_sub_in_data_transfer_state(const sys_modem_as_id_e_type 
as_id)
{
#ifdef FEATURE_DUAL_SIM
  gas_id_t gas_id;

  /* convert the supplied ASID into a GAS ID */
  gas_id = geran_map_nas_id_to_gas_id(as_id);

  /* and then check the MAC status of the opposite stack */
  if (GERAN_ACCESS_STRATUM_ID_1 != gas_id)
  {
    return(MAC_TRANSFER == rr_macinfo_get_mac_status(GERAN_ACCESS_STRATUM_ID_1));
  }
  else
  {
    return(MAC_TRANSFER == rr_macinfo_get_mac_status(GERAN_ACCESS_STRATUM_ID_2));
  }
#else
  NOTUSED(as_id);
#endif /* FEATURE_DUAL_SIM */

  return(FALSE);
}

/* EOF */

