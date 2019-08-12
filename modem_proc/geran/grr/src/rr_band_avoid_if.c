/*============================================================================
  @file rr_band_avoid_if.c

  @brief Interface for band avoidance feature

                Copyright (c) 2014 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_band_avoid_if.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_BAND_AVOIDANCE

#include "comdef.h"

#include "rr_band_avoid_if.h"
#include "rr_general.h"
#include "wwan_coex_mgr.h"
#include "msgr_geran.h"
#include "rr_msgr.h"
#include "rr_band_avoid.h"
#include "rr_rr.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/
/*!
 * \brief Convert RR band to sys band class type
 *
 * \param band
 *        gas_id
 *
 * \return sys_band_class_e_type
 */
static sys_band_class_e_type rr_band_avoid_gsm_band_to_sys_band(sys_band_T band, gas_id_t gas_id)
{
   switch(band)
   {
     case SYS_BAND_DCS_1800:
        return SYS_BAND_GSM_DCS_1800;
        break;
     case SYS_BAND_PCS_1900:
        return SYS_BAND_GSM_PCS_1900;
        break;
     case SYS_BAND_CELL_850:
        return SYS_BAND_GSM_850;
        break;
     case SYS_BAND_PGSM_900:
        return SYS_BAND_GSM_PGSM_900;
        break;
     case SYS_BAND_EGSM_900:
        return SYS_BAND_GSM_EGSM_900;
        break;
     default:
        MSG_GERAN_ERROR_1_G("RR_BA Unsupported RF band: %d", band);
        return SYS_BAND_CLASS_NONE;
        break;
   }
}

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/
/*!
 * \brief Send blacklist response to CXM
 *
 * \param blist_id
 *        gas_id
 *
 * \return void
 */
void rr_band_avoid_send_cxm_blist_rsp(uint16 blist_id,
                                      const gas_id_t gas_id)
{
  cxm_coex_ba_blist_res_s blist_rsp;
  errno_enum_type result;

  memset(&blist_rsp, 0, sizeof blist_rsp);

  msgr_init_hdr(
    &blist_rsp.msg_hdr,
    MSGR_GERAN_GRR,
    MCS_CXM_BAND_AVOID_BLIST_RSP
  );

  blist_rsp.blist_res_data.blacklist_id = blist_id;

  if (geran_is_multi_mode_gas_id(gas_id))
  {
    blist_rsp.blist_res_data.tech_id = CXM_TECH_GSM1;   
  }
  else
  {
    blist_rsp.blist_res_data.tech_id = CXM_TECH_GSM2;
  }
  
  blist_rsp.blist_res_data.freq_avoid_res = FREQ_AVOID_NACK; /*currently this is the only valid response from GERAN*/

  result = rr_msgr_send(&blist_rsp.msg_hdr, sizeof(blist_rsp), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
}

/*!
 * \brief Send Freqency Ind to CXM
 *
 * \param *freq
 *         gas_id
 *
 * \return void
 */
void rr_band_avoid_send_cxm_freq_ind(frequency_list_T *freq, const gas_id_t gas_id)
{
  cxm_coex_ba_freq_ind_s freq_ind;
  uint16 i;
  errno_enum_type result;

  memset(&freq_ind, 0, sizeof freq_ind);

  msgr_init_hdr(
    &freq_ind.msg_hdr,
    MSGR_GERAN_GRR,
    MCS_CXM_BAND_AVOID_FREQ_IND
  );

  for (i=0;i<freq->no_of_items;i++)
  {    
    freq_ind.serv_freq_data.freq_list[i].frequency = rr_band_avoid_arfcn_to_centre_freq_kHz(freq->channel[i], gas_id);
    freq_ind.serv_freq_data.freq_list[i].band = rr_band_avoid_gsm_band_to_sys_band(freq->channel[i].band, gas_id);
    freq_ind.serv_freq_data.freq_list[i].bandwidth = 200000; /*Hz*/
    freq_ind.serv_freq_data.freq_list[i].channel = freq->channel[i].num;
    freq_ind.serv_freq_data.freq_list[i].direction = CXM_LNK_DRCTN_UL_AND_DL;
    freq_ind.serv_freq_data.num_entries++;
    
    if ((freq_ind.serv_freq_data.num_entries+1) == NELEMS(freq_ind.serv_freq_data.freq_list))
    {
      MSG_GERAN_HIGH_0_G("Freq list full");
      break;
    }    
  }

  if (geran_is_multi_mode_gas_id(gas_id))
  {
    freq_ind.serv_freq_data.tech_id = CXM_TECH_GSM1;   
  }
  else
  {
    freq_ind.serv_freq_data.tech_id = CXM_TECH_GSM2;
  }

  result = rr_msgr_send(&freq_ind.msg_hdr, sizeof(freq_ind), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
}

/*!
 * \brief Send RR internal Frequency Update message
 *
 * \param *freq_upd
 *         gas_id
 *
 * \return void
 */
void rr_band_avoid_ded_freq_upd_send_imsg(frequency_list_T *freq_upd, const gas_id_t gas_id)
{
  rr_band_avoid_ded_freq_upd_t ded_fre_upd_ind;
  
  memset(&ded_fre_upd_ind, 0, sizeof ded_fre_upd_ind);

  if(freq_upd == NULL)
  {
    MSG_GERAN_ERROR_0("rr_band_avoid_ded_freq_upd_send_imsg: Frequency list empty");
  }
  else
  {
    ded_fre_upd_ind.freq = *freq_upd;
  }

  rr_rr_send(&ded_fre_upd_ind.header,
             RR_BAND_AVOID_IMSG_DED_FREQ_UPD_IND,
             sizeof(ded_fre_upd_ind),
             RR_BAND_AVOID_IMSG,
             gas_id);
}

/*!
 * \brief Send RR internal Measurement Report Ind message
 *
 * \param gas_id
 *
 * \return void
 */
void rr_band_avoid_meas_rpt_sent_send_imsg(const gas_id_t gas_id)
{
  rr_message_header_t meas_rpt_sent;

  rr_rr_send(&meas_rpt_sent,
             RR_BAND_AVOID_IMSG_MEAS_RPT_SENT_IND,
             sizeof(meas_rpt_sent),
             RR_BAND_AVOID_IMSG,
             gas_id);
}

/*!
 * \brief Output string for band avoid internal messages
 *
 * \param omsg
 *
 * \return char *
 */
const char * rr_band_avoid_imsg_name(rr_band_avoid_imsg_e omsg)
{
  switch (omsg)
  {
    case RR_BAND_AVOID_IMSG_DED_FREQ_UPD_IND: return "RR_BAND_AVOID_IMSG_DED_FREQ_UPD_IND";
    case RR_BAND_AVOID_IMSG_MEAS_RPT_SENT_IND: return "RR_BAND_AVOID_IMSG_MEAS_RPT_SENT_IND";
    default:
      return "?";
  }
}

#endif /*FEATURE_GSM_BAND_AVOIDANCE*/
