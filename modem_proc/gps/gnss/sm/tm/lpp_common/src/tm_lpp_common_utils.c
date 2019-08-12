
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         TM LPP Common Utility Module

GENERAL DESCRIPTION
  This file contains TM's LPP Common Utility Functions

  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/lpp_common/src/tm_lpp_common_utils.c#1 $
 
  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  08/05/15   rh      Moved carrier-specific NV 1920 (pos mode support) to TM core 
  01/14/15   rk      LPP >24 AD/Meas feature support.
  08/25/14   jv    16-bit SVID Support 
  05/14/13   rk      Promoted Glonass conversion function to this LPP common module.
  02/1/12    atien   Initial version

===========================================================================*/
#include "customer.h"   /* Customer configuration file */
/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "comdef.h"    /* Definition for basic types and macros */
#include "msg.h"
#include "math.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "sm_nv.h"
#include "sm_nv_efs.h"
#include "tm_lpp_common_utils.h"

lpp_cp_nv_items_s_type z_lpp_cp_nv_item_ram_copy;

/*
 ******************************************************************************
 *
 * tm_lpp_common_utils_read_nv
 *
 * Function description:
 *  This function reads NV items that are common to LPP into a structure.
 *
 * Parameters:
 *   None.
 *
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/
void tm_lpp_common_utils_read_nv (void)
{
  /* NV buffer, used to read NV items */
  nv_item_type      z_nv_read_buffer;
  
  /* Default is NV_AAGPS_POSITIONING_MODES_SUPPORTED_V
     PDSM_POS_MODE_4G_UP_MS_BASED    0x0100
     PDSM_POS_MODE_4G_UP_MS_ASSISTED 0x0200
     PDSM_POS_MODE_4G_CP_MS_BASED    0x0400
     PDSM_POS_MODE_4G_CP_MS_ASSISTED 0x0800
    
     NOTE: carrier-specific NV 1920 moved to TM core */ 
  
  /* NOTE - LPP CP enable/disable NV item is maintained in TM CORE via tm_core_get_lpp_cp_enable() */

  /*
  Bit 0 - CS-MT-LR             (for CP GSM/WCDMA)
  Bit 1 - Ignore UP NI trigger (for UP GSM/WCDMA)
  Bit 2 - UP NI trigger (for UP LPP)
  Bit 3 - CP NI trigger (for CP LPP EPC-MT-LR)

  NV_AAGPS_MT_LR_CP_GSM_WCDMA_SUPPORTED             0x01 
  NV_AAGPS_MT_LR_UP_GSM_WCDMA_BLOCKED               0x02
  NV_AAGPS_MT_LR_UP_LPP_SUPPORTED                   0x04
  NV_AAGPS_MT_LR_CP_LPP_SUPPORTED                   0x08
  */
  (void)sm_nv_read( NV_AAGPS_MT_LRSUPPORT_I, (void *)&z_nv_read_buffer);
  z_lpp_cp_nv_item_ram_copy.mt_lr_support = z_nv_read_buffer.aagps_mt_lrsupport;
 

  (void) sm_nv_read( NV_AAGPS_DEFAULT_QOS_TIME_I, (void *)&z_nv_read_buffer);
  z_lpp_cp_nv_item_ram_copy.aagps_default_qos_time = z_nv_read_buffer.aagps_default_qos_time;

  (void) sm_nv_read( NV_AAGPS_DEFAULT_QOS_UNC_I, (void *)&z_nv_read_buffer);
  z_lpp_cp_nv_item_ram_copy.aagps_default_qos_unc = z_nv_read_buffer.aagps_default_qos_unc;   /* ayt_todo - not in nv_lpp.xml */

  (void) sm_nv_read( NV_AAGPS_DEFAULT_REF_TIME_UNC_I, (void *)&z_nv_read_buffer);
  z_lpp_cp_nv_item_ram_copy.aagps_default_ref_time_unc_50ms = z_nv_read_buffer.aagps_default_ref_time_unc; /* ayt_todo - not in nv_lpp.xml */
  
  (void) sm_nv_efs_reg_item_read( GNSS_NV_EFS_SM_TM_LPP_CP_TRANSPORT_TIMEOUT_SECS,
      sizeof(uint8), &z_lpp_cp_nv_item_ram_copy.aagps_default_lpp_cp_timeout_secs);
  {
    /* if returned false for any reason, force it to default value */
    z_lpp_cp_nv_item_ram_copy.aagps_default_lpp_cp_timeout_secs = GNSS_NV_EFS_SM_TM_LPP_CP_TRANSPORT_TIMEOUT_SECS_DEFAULT;
  }
  
} /* tm_lpp_common_utils_read_nv */


/*
 ******************************************************************************
 *
 * tm_lpp_common_utils_refresh_carrier_specific_nv
 *
 * Function description:
 *    This function refresh carrier-specific NVs after SIM hot swap
 *    sm_nv_read() will always read from HW. sm_nv_read_ex() may read from cache
 *    if available.
 *
 * Parameters:
 *   None.
 *
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/
void tm_lpp_common_utils_refresh_carrier_specific_nv (void)
{
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  /* Add any other carrier-specific NVs here, NV 1920 (pos mode support) moved to TM core */
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
}


/*
 ******************************************************************************
 *
 * tm_lpp_common_glo_eph_svid_freq_num_map_aux 
 *
 * Function description:
 *  Associates sv ID with a frequencey number for Glonass eph. data using 
 *  information from auxilary assistance.                   
 *  
 * Parameters:
 *   p_gnss_aux_info : pointer to the structure holding Glonass aux. data.
 *
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/

void tm_lpp_common_glo_eph_svid_freq_num_map_aux (const GNSS_AuxiliaryInformation * const p_gnss_aux_info, prot_glo_eph_buffer_s_type p_lpp_glo_eph_freq[N_GLO_SV])
{
  uint8  u_index = 0;
  GNSS_ID_GLONASS_SatElement *p_lpp_glonass_sat_elem = NULL;

  if (( p_gnss_aux_info->t != T_GNSS_AuxiliaryInformation_gnss_ID_GLONASS ) || ( p_lpp_glo_eph_freq == NULL) ||
      (p_gnss_aux_info->u.gnss_ID_GLONASS == NULL))
  {
    MSG_ERROR( "AuxiInfo doesn't contain Glonass parameters. Map svid & freq/channel num failed!", 0, 0, 0 );
    return;
  }

  MSG_HIGH("LPP Glonass AuxiInfo list = %u", p_gnss_aux_info->u.gnss_ID_GLONASS->n, 0, 0);

  for ( u_index = 0; u_index < p_gnss_aux_info->u.gnss_ID_GLONASS->n; ++u_index )
  {
    p_lpp_glonass_sat_elem = &p_gnss_aux_info->u.gnss_ID_GLONASS->elem[u_index];

    if ( p_lpp_glonass_sat_elem && p_lpp_glonass_sat_elem->m.channelNumberPresent )
    {
      if ( ( p_lpp_glonass_sat_elem->svID.satellite_id <= ( N_GLO_SV - 1 ) ) &&
           ( ( p_lpp_glonass_sat_elem->channelNumber >= C_PROT_GLO_FREQ_MIN ) &&
             ( p_lpp_glonass_sat_elem->channelNumber <= C_PROT_GLO_FREQ_MAX ) 
           )
         )
      {
        p_lpp_glo_eph_freq[p_lpp_glonass_sat_elem->svID.satellite_id].u_freq_num_valid = TRUE;
        p_lpp_glo_eph_freq[p_lpp_glonass_sat_elem->svID.satellite_id].z_eph_glo_data.u_FreqNum = 
          p_lpp_glonass_sat_elem->channelNumber & 0x1F; /* the MGP API expects 5 bits */

        MSG_LOW("Mapped LPP Glonass SV_id=%u and channel#=%d. u_i=%u",  
                 p_lpp_glonass_sat_elem->svID.satellite_id, p_lpp_glonass_sat_elem->channelNumber, u_index);
      }
      else
      {
        MSG_ERROR("Incorrect LPP Glonass SV_id=%u and channel#=%d, u_i=%u",  
                 p_lpp_glonass_sat_elem->svID.satellite_id, p_lpp_glonass_sat_elem->channelNumber, u_index);
      }
    }
  }
} /* tm_lpp_common_glo_eph_svid_freq_num_map_aux */


/*
 ******************************************************************************
 *
 * tm_lpp_common_is_freq_sat_valid 
 *
 * Function description:
 *  Verify if the frequency number & satellite SV id is valid. Generally these
 *  information are are available in auxiliary info assistance data.                   
 *  
 * Parameters:
 *   p_gnss_glonass_sat_elem : pointer to the structure holding Glonass aux. data.
 * 
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/
boolean tm_lpp_common_is_freq_sat_valid( const GNSS_ID_GLONASS_SatElement * const p_gnss_glonass_sat_elem )
{   
  if ( (p_gnss_glonass_sat_elem != NULL ) && (p_gnss_glonass_sat_elem->m.channelNumberPresent) )
  {
    // Check freqNum & svID.satellite_id (0..63) correctness. For glonass satellite_id 0..23 is valid
    if ( ( p_gnss_glonass_sat_elem->svID.satellite_id <= ( N_GLO_SV - 1 ) ) &&
         ( p_gnss_glonass_sat_elem->channelNumber >= C_PROT_GLO_FREQ_MIN ) && ( p_gnss_glonass_sat_elem->channelNumber <= GLO_HIGHEST_FREQ_NUMBER ) // ME expect FreqNum -7..6, though protocol max is 13.
       )
    {
      return TRUE;
    }
    else
    {
      MSG_ERROR("Incorrect LPP Glonass SV_id=%u and channel#=%d", p_gnss_glonass_sat_elem->svID.satellite_id, 
                p_gnss_glonass_sat_elem->channelNumber, 0);
    }
  }

  return FALSE;
}

/*
 ******************************************************************************
 *
 * tm_lpp_common_glo_acqast_svid_freq_num_map_aux 
 *
 * Function description:
 *  Associates sv ID with a frequencey number for Glonass Aquisition Assistance data using 
 *  information from auxilary assistance.                   
 *  
 * Parameters:
 *   p_gnss_aux_info : pointer to the structure holding Glonass aux. data.
 *   p_lpp_glo_aa_freq : pointer to LPP Glonass aquisition assistance & auxi info struct.
 * 
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/
void tm_lpp_common_glo_acqast_svid_freq_num_map_aux (const GNSS_AuxiliaryInformation* const p_gnss_aux_info, 
                                                     prot_glo_acqast_buffer_s_type* p_lpp_glo_aa_freq)
{
  uint8 u_index = 0, u_sv_iter = 0, u_save_loop = 0;
  GNSS_ID_GLONASS_SatElement *p_gnss_glonass_sat_elem = NULL;
  uint8 u_glo_num_svs = 0;

  if ( ( p_lpp_glo_aa_freq == NULL ) || (p_gnss_aux_info->t != T_GNSS_AuxiliaryInformation_gnss_ID_GLONASS ) || 
       ( p_gnss_aux_info->u.gnss_ID_GLONASS == NULL ) )
  {
    MSG_ERROR( "AuxiInfo doesn't contain Glonass parameters. Map svid & freq/channel num failed!", 0, 0, 0 );
    return;
  }

  MSG_HIGH("LPP Glonass AuxiInfo num of list = %u", p_gnss_aux_info->u.gnss_ID_GLONASS->n, 0, 0);

  /* Glonass Acq Assistance isn't received ? */
  if ( 0 == p_lpp_glo_aa_freq->gnss_sv_dir.u_NumSvs )
  {
    MSG_HIGH( "Glonass Acq Assistance isn't received yet. Glo SvId to FreqNum correlation deferred.", 0, 0, 0 );
    return;
  }
  else 
  {
    /* Glonass Acq Assistance is received already */

    /* Ensure the total SV count for SV-Dir & Steering are same. This help avoid looping twice. */
    u_glo_num_svs = p_lpp_glo_aa_freq->gnss_sv_dir.u_NumSvs;
    if( p_lpp_glo_aa_freq->gnss_sv_dir.u_NumSvs != p_lpp_glo_aa_freq->z_steering_glo_data.u_NumSvs )
    {
      if (p_lpp_glo_aa_freq->gnss_sv_dir.u_NumSvs > p_lpp_glo_aa_freq->z_steering_glo_data.u_NumSvs)
      {
        u_glo_num_svs = p_lpp_glo_aa_freq->z_steering_glo_data.u_NumSvs;
      }

      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SV-Dir & Steering SV count differs %u, %u. Using only %u AA SVs", 
            p_lpp_glo_aa_freq->gnss_sv_dir.u_NumSvs, p_lpp_glo_aa_freq->z_steering_glo_data.u_NumSvs, u_glo_num_svs);
    }

    for ( u_index = 0; u_index < p_gnss_aux_info->u.gnss_ID_GLONASS->n; ++u_index )
    {
      p_gnss_glonass_sat_elem = &p_gnss_aux_info->u.gnss_ID_GLONASS->elem[u_index];

      if ( TRUE == tm_lpp_common_is_freq_sat_valid( p_gnss_glonass_sat_elem ) )
      {
        u_save_loop = 0;
        /* Loop through AA SVs & set the corresponding FreqNum */
        for ( u_sv_iter = 0; u_sv_iter < u_glo_num_svs; ++u_sv_iter )
        {
          // AA-SvDir
          // local u_Sv (65..88) & protocol p_gnss_glonass_sat_elem->svID (0..23)
          if (p_lpp_glo_aa_freq->gnss_sv_dir.z_SvAzElInfo[u_sv_iter].w_Sv == (p_gnss_glonass_sat_elem->svID.satellite_id + GLO_SLOTID_OFFSET + 1))
          {
            p_lpp_glo_aa_freq->u_freq_num_valid = TRUE;
            /* TomL: Per GLONASS ICD page 10, after year 2005, GLO SV would use frequency ID -7 to +6. Freq channel +7 to +13 would not be used. */
            p_lpp_glo_aa_freq->gnss_sv_dir.z_SvAzElInfo[u_sv_iter].b_GloFreqIndex = p_gnss_glonass_sat_elem->channelNumber; /* the MGP API expects 5 bits */
            u_save_loop |= 0x1;
          }

          // AA-Steering
          // local u_Sv (65..88) & protocol p_gnss_glonass_sat_elem->svID (0..23)
          if (p_lpp_glo_aa_freq->z_steering_glo_data.z_SvSteerInfo[u_sv_iter].w_Sv == (p_gnss_glonass_sat_elem->svID.satellite_id + GLO_SLOTID_OFFSET + 1))
          {
            p_lpp_glo_aa_freq->u_freq_num_valid = TRUE;
            p_lpp_glo_aa_freq->z_steering_glo_data.z_SvSteerInfo[u_sv_iter].b_GloFreqIndex = p_gnss_glonass_sat_elem->channelNumber;
            u_save_loop |= 0x2;
          }

          if ( u_save_loop == 3)
          {
            break; // break when both freqNum are set, else loop to max.
          }
        }

        MSG_HIGH("Mapped LPP Glonass SV_id=%u and channel#=%d. u_i=%u", p_gnss_glonass_sat_elem->svID.satellite_id, 
                     p_gnss_glonass_sat_elem->channelNumber, u_index);
      }
    }
  }
}

/*
 ******************************************************************************
 *
 * tm_lpp_common_glo_eph_svid_freq_num_map_alm 
 *
 * Function description:
 *  Associates sv ID with a frequencey number for Glonass eph. data using 
 *  information from Almanac model for Glonass.                   
 *  
 * Parameters:
 *   p_keplerianGLONASS : pointer to the structure holding Almanac model for Glonass.
 *
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/

void tm_lpp_common_glo_eph_svid_freq_num_map_alm (const AlmanacGLONASS_AlmanacSet * const p_keplerianGLONASS, prot_glo_eph_buffer_s_type p_lpp_glo_eph_freq[N_GLO_SV])
{
  if ((p_keplerianGLONASS == NULL) || (p_keplerianGLONASS->gloAlmnA < 1) || (p_keplerianGLONASS->gloAlmnA > N_GLO_SV)) // gloAlmnA (1..24) - sv slot id
  {
    MSG_ERROR("GLO Alm SvId invalid: %p", p_keplerianGLONASS, 0, 0 );
    return;
  }

  /* gloAlmnA is SV_ID :  gloAlmnA : INTEGER (1..24) */
  p_lpp_glo_eph_freq[p_keplerianGLONASS->gloAlmnA - 1].u_freq_num_valid = TRUE;
  // gloAlmHA (0..31)
  p_lpp_glo_eph_freq[p_keplerianGLONASS->gloAlmnA - 1].z_eph_glo_data.u_FreqNum =  p_keplerianGLONASS->gloAlmHA;
  MSG_LOW("GLO Alm. SV slot id %u to FreqNum %d mapping", p_keplerianGLONASS->gloAlmnA, p_keplerianGLONASS->gloAlmHA, 0);
}

/*
 ******************************************************************************
 *
 * tm_lpp_common_convert_to_glonass_time
 *
 * Function description:
 *  This function processes GPS offset time (deltaT) to compute the Glonass time
 *  & set the appropiate Glonass time fields in set_assist_pos_time_stamp struct.
 *  The deltaT could be from GPS UTCModel AD and/or GLONASS TimeModel AD.
 *
 * Parameters:
 *  bDeltaT [IN]: Positive GPS time offset value
 *  p_supl_session_postime [OUT]: Pointer to position time structure.
 * 
 * Return value:
 *  On success TRUE, else FALSE is return to indicate the failure.
 *
 *
 ******************************************************************************
*/
boolean tm_lpp_common_convert_to_glonass_time(const int8 bDeltaT, gnss_time_info *p_supl_session_postime)
{
  // the delta time (a.k.a LeapSecondOffset) for GPS to GLONASS should be positive value
  if ( ( p_supl_session_postime == NULL ) || ( p_supl_session_postime->b_deltaT < 0 ) )
  {
    MSG_ERROR( "TM_LPP_UP: SUPL: GPS time difference deltaT should be positive. %p", 
               p_supl_session_postime, 0, 0 );
    return FALSE;
  }

  p_supl_session_postime->b_deltaT = bDeltaT;
  p_supl_session_postime->m.deltaTPresent = 1;

  // Convert GPS to GLONASS time
  if ( (TRUE == p_supl_session_postime->m.refTimeWeekNumValid) && (1 == p_supl_session_postime->m.refTimeTowMsValid) ) 
  {
    gps_TimeStructType z_GpsTime;
    memset(&z_GpsTime, 0, sizeof(z_GpsTime));
    memset(&p_supl_session_postime->z_GlonassTime, 0, sizeof(glo_TimeStructType));

    z_GpsTime.w_GpsWeek = p_supl_session_postime->w_ref_time_week_num;
    z_GpsTime.q_GpsMsec = p_supl_session_postime->q_ref_time_tow_ms;

    gnss_ConvertGpsTime2GloTime(&z_GpsTime, (uint8)p_supl_session_postime->b_deltaT,
                                &p_supl_session_postime->z_GlonassTime );

    p_supl_session_postime->m.glonassTimeValid = TRUE;
    MSG_6( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TM_LPP_UP: SUPL: GLONASS time (%u, %u, %ld) computed from GPS time (%u, %lu) offset of %d", 
           p_supl_session_postime->z_GlonassTime.u_FourYear, p_supl_session_postime->z_GlonassTime.w_Days, 
           p_supl_session_postime->z_GlonassTime.q_GloMsec, z_GpsTime.w_GpsWeek, z_GpsTime.q_GpsMsec,
           p_supl_session_postime->b_deltaT );

    return TRUE;
  }
  else
  {
    MSG_ERROR( "TM_LPP_UP: SUPL: GPS Reference Time (%d, %d) isn't valid to compute Glonass time.", 
               p_supl_session_postime->m.refTimeWeekNumValid, p_supl_session_postime->m.posTimeStampValid, 0 );
  }

  return FALSE;
}


/*
 ******************************************************************************
 *
 * tm_lpp_util_add_otdoa_data_to_list
 *
 * FUNCTION DESCRIPTION:
 *  Remove the OTDOA/RSTD measurements from the structure that's already processed.
 *
 * PARAMETERS:
 *  pz_otdoa_data_in_list [IN/OUT]: allocate memory and add the OTDOA data to the list.
 * 
 * RETURN VALUE:
 *  On success return pointer to newly added OTDOA data, else return NULL.
 *
 ******************************************************************************
*/
sm_loc_lte_otdoa_meas_info_type* tm_lpp_util_add_otdoa_data_to_list(tm_otdoa_data_store *pz_otdoa_data_store)
{
  uint8 const u_idx = pz_otdoa_data_store->u_num_otdoa_meas_set_avail;

  /* Don't add more than 3 OTDOA measurements */
  if ( (NULL == pz_otdoa_data_store->pz_otdoa_data_list[u_idx]) &&
       (u_idx < TM_LPP_MAX_OTDOA_RSTD_MEAS_DATA) )
  {
    pz_otdoa_data_store->pz_otdoa_data_list[u_idx] = os_MemAlloc(sizeof(sm_loc_lte_otdoa_meas_info_type), OS_MEM_SCOPE_TASK);

    /* Increment the total count */
    pz_otdoa_data_store->u_num_otdoa_meas_set_avail++;
  }
  else
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Can't take more than 3 OTDOA meas. %p %d", 
          pz_otdoa_data_store->pz_otdoa_data_list[u_idx], u_idx);
  }

  return ( pz_otdoa_data_store->pz_otdoa_data_list[u_idx] ); 
}


/*
 ******************************************************************************
 *
 * tm_lpp_util_remove_otdoa_data_from_list
 *
 * FUNCTION DESCRIPTION:
 *  Remove the OTDOA/RSTD measurements based on the last processed index.
 *
 * PARAMETERS: remove and free the data from the list.
 * 
 * RETURN VALUE:
 *  None.
 *
 ******************************************************************************
*/
void tm_lpp_util_remove_otdoa_data_from_list(tm_otdoa_data_store *pz_otdoa_data_store)
{
  if  ( pz_otdoa_data_store->pz_otdoa_data_list[pz_otdoa_data_store->u_tot_otdoa_meas_set_sent] != NULL )
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Remove %d otdoa data item (%p)", pz_otdoa_data_store->u_tot_otdoa_meas_set_sent,
          pz_otdoa_data_store->pz_otdoa_data_list[pz_otdoa_data_store->u_tot_otdoa_meas_set_sent]);

    os_MemFree((void **)&(pz_otdoa_data_store->pz_otdoa_data_list[pz_otdoa_data_store->u_tot_otdoa_meas_set_sent]));
    pz_otdoa_data_store->pz_otdoa_data_list[pz_otdoa_data_store->u_tot_otdoa_meas_set_sent] = NULL;

    /* Increase the number of processed count */
    pz_otdoa_data_store->u_tot_otdoa_meas_set_sent++;
  }
}


/*
 ******************************************************************************
 *
 * tm_lpp_util_remove_all_otdoa_data_from_list
 *
 * FUNCTION DESCRIPTION:
 *  Remove all the OTDOA/RSTD measurements from the structure & initialize the index fields.
 *
 * PARAMETERS:
 *  pz_otdoa_data_store [IN/OUT]: Pointer to the OTDOA data store
 * 
 * RETURN VALUE:
 *  None.
 *
 ******************************************************************************
*/
void tm_lpp_util_remove_all_otdoa_data_from_list(tm_otdoa_data_store *pz_otdoa_data_store)
{
  uint8 u_index_to_free;
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Remove all stored otdoa data %p", pz_otdoa_data_store);

  if (( NULL == pz_otdoa_data_store) || (0 == pz_otdoa_data_store->u_num_otdoa_meas_set_avail ))
  {
    return;
  }
  for (u_index_to_free = 0; u_index_to_free < pz_otdoa_data_store->u_num_otdoa_meas_set_avail; u_index_to_free++)
  {
    if (pz_otdoa_data_store->pz_otdoa_data_list[u_index_to_free] != NULL)
    {
      os_MemFree((void **)&(pz_otdoa_data_store->pz_otdoa_data_list[u_index_to_free]));
      pz_otdoa_data_store->pz_otdoa_data_list[u_index_to_free] = NULL;
    }
  }
  pz_otdoa_data_store->u_num_otdoa_meas_set_avail = 0; 
  pz_otdoa_data_store->u_tot_otdoa_meas_set_sent = 0;
}
