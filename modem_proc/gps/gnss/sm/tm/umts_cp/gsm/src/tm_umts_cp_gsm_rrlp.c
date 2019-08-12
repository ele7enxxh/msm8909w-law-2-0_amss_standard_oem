/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  G S M   C P   R R L P   M O D U L E

GENERAL DESCRIPTION
  This file contains the code which decodes RRLP messages from the
  SMLC and encodes RRLP messages to be sent to the SMLC

Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.


Copyright (c) 2011, 2013, 2014-2015  by Qualcomm Atheros, Inc.  All Rights Reserved. 
Qualcomm Atheros Confidential and Proprietary.

Copyright (c) 2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_cp/gsm/src/tm_umts_cp_gsm_rrlp.c#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  08/05/15   rh      Moved carrier-specific NV 1920 (pos mode support) to TM core 
  07/31/15   mj     Changes to fix E911 on second SUB in G+G config
  06/09/15   mj     Add support for W+W feature
  04/02/15   mj     Modify tm_umts_common_utils_rr_session_status_notify to support cached as_id
  09/11/14   mj     Modify tm_umts_common_utils_rr_session_status_notify to support asid
  05/31/13   rk      Added GPS RTI processing support for GSM (RRLP).
  09/17/12   MJ      Modified gps_meas_status_bits_check call
  04/06/11   LT      ASN1 migration.
  05/14/2010 lt      Activate max_receiver_on timer upon RRLP-AD
  12/2/09    gk      intermediate position fixes
  08/18/09   atien   tm_umts_cp_gsm_handle_prm_data() - 
                     Make sure to stop GPS PRM timer in LM to 
                     prevent premature timeout of current session while 
                     waiting for Return Result message from network.
  09/19/08   LT      Buffering ACQ-assist. for injecting at right time.
  05/03/07   LT      Initial version

===========================================================================*/

#include "gps_variation.h"
#include "customer.h"

#ifdef FEATURE_CGPS_UMTS_CP_GSM

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include <limits.h>
#include "tm_umts_cp_gsm.h"
#include "tm_umts_cp_gsm_rrlp.h"



#if (defined(T_WINNT) && defined(SMART_AMSS))
#error code not present
#else
#include "tm_asn1util.h"
#endif

#include "tm_prtl_iface.h"
#include "sm_log.h"


/* =======================================================================
**                            Function Definitions
** ======================================================================= */

static void tm_umts_cp_gsm_rrlp_encode_protocol_err
( 
  rrlp_ErrorCodes     u_reason,
  int8                b_ref_num 
);

static boolean tm_umts_cp_gsm_log_and_tx_rrlp
(
  uint8 *     p_buff,
  uint16      w_length
);

static void tm_umts_cp_gsm_tx_rrlp_ack
( 
  void 
);

static void tm_umts_cp_gsm_proc_msr_pos_req_qos
(
   const rrlp_MsrPosition_Req  *p_mpr
);

static boolean tm_umts_cp_gsm_encode_msrReport_meas
( 
  const sm_GpsMeasRptStructType *p_prm_data,  
  boolean status_check_needed
);

#ifdef MSG_LOW
#undef MSG_LOW
#endif
#define MSG_LOW(s,p1,p2,p3) MSG_MED(s,p1,p2,p3)


/*===========================================================================

FUNCTION TO_OSS_REF_NUM

DESCRIPTION
  This function converts an 8 bit signed reference number into a reference
  number suitable to use by the RRLP encode/decoder
  
DEPENDENCIES: none

RETURN VALUE: OSS_UINT32 - unsigned 32 bit RRLP compatible reference number

SIDE EFFECTS: none

===========================================================================*/

static uint32 TO_OSS_REF_NUM( int8 b_ref_num )
{
  if( b_ref_num < 1 )
  {
    return 0;
  }

/*lint -e{571} safe cast*/
  return (uint32) b_ref_num;
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_post_data

DESCRIPTION
  This function posts data to TM-Core
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static uint32 map_post_data_to_bit_mask( tm_post_data_e_type  e_post_data )
{
  uint32  bm;

  switch( e_post_data )
  {
    case TM_POST_DATA_TYPE_AA:
      bm = C_GPS_ASSIS_ACQ;
      break;

    case TM_POST_DATA_TYPE_SV_DIR:
      bm = C_GPS_ASSIS_ACQ;
      break;

    case TM_POST_DATA_TYPE_SA:
      bm = C_GPS_ASSIS_SA;
      break;

    case TM_POST_DATA_TYPE_IONO:
      bm = C_GPS_ASSIS_IONO;
      break;

    case TM_POST_DATA_TYPE_EPH:
      bm = C_GPS_ASSIS_NAVMODL;
      break;

    case TM_POST_DATA_TYPE_ALM:
      bm = C_GPS_ASSIS_ALM;
      break;

    case TM_POST_DATA_TYPE_SEED:
      bm = C_GPS_ASSIS_REFLOC;
      break;

    case TM_POST_DATA_TYPE_REFTIME:
      bm = C_GPS_ASSIS_REFTIME;
      break;

    case TM_POST_DATA_TYPE_UTC_MODEL:
      bm = C_GPS_ASSIS_UTC;
      break;

    case TM_POST_DATA_TYPE_SV_NO_EXIST:
    case TM_POST_DATA_TYPE_LR:
    case TM_POST_DATA_TYPE_PREFIX_PPM:
    case TM_POST_DATA_TYPE_NONE:
    default:
      bm = 0;
      break;     
  }

  return bm;
}

static void tm_umts_cp_gsm_post_data
( 
  tm_post_data_e_type               data_type,
  const tm_post_data_payload_type   *p_payload )
{
  boolean post_ok;

  post_ok = tm_post_data( 
                      TM_PRTL_TYPE_UMTS_CP,
                      z_umts_cp_gsm_session_status.tm_active_session_handle,
                      data_type,
                      p_payload );
    
  if( post_ok )
  {
    z_umts_cp_gsm_session_status.q_agps_data_rcvd |= 
                        map_post_data_to_bit_mask( data_type );

    MSG_MED("CP-GSM: %d posted to TmCore; rcvd=0x%08X, ~rcvd=0x%08X", 
            (int) data_type,
            z_umts_cp_gsm_session_status.q_agps_data_rcvd, 
            ~z_umts_cp_gsm_session_status.q_agps_data_rcvd );
  }
  else
  {
    MSG_ERROR("CP-GSM: post of %d to Tmcore failed", 
              (int)data_type, 0, 0 );
  }
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_process_ref_time

DESCRIPTION
  This function processes the OSS ASN.1 decoded RRLP ref_time data and 
  prepares it for injection into MGP.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static void tm_umts_cp_gsm_process_ref_time
(
  rrlp_ControlHeader   *p_assist,
  uint16               *p_gps_week 
)
{
  tm_post_data_payload_type                 z_tm_post_data_payload;
  umts_cp_gsm_session_agps_mode_enum_type   e_agps_mode;
  msa_pos_time_stamp_type                   *p_msa_pos_time_stamp;
  
  p_msa_pos_time_stamp = &z_umts_cp_gsm_session_status.z_msa_pos_time_stamp;
 
  e_agps_mode = z_umts_cp_gsm_session_status.e_agps_mode;

  (void) e_agps_mode;

  MSG_MED("CP-GSM: process_ref_time; e_agps_mode=%d", e_agps_mode, 0, 0);

  p_msa_pos_time_stamp->w_ref_time_week_num =
                                              gnss_GpsWeekExtend(
                                              (uint16)p_assist->referenceTime.gpsTime.gpsWeek);

  p_msa_pos_time_stamp->u_ref_time_week_num_valid = TRUE;

  tm_rrlp_reftime_to_TmCore_post_data_payload(  p_assist, 
                                                &z_tm_post_data_payload);

  *p_gps_week = z_tm_post_data_payload.reftime_data.w_GpsWeek;

  tm_umts_cp_gsm_post_data( TM_POST_DATA_TYPE_REFTIME,
                            &z_tm_post_data_payload );

  #if 0
  if( p_assist->referenceTime.bit_mask & rrlp_ReferenceTime_gsmTime_present )
  {
    /* Not supported */
  }
  #endif

  #ifdef TBD
  /*** QQQ-PM: need to discuss how to handle sparse pattern match **/
  if( p_assist->referenceTime.bit_mask & rrlp_gpsTowAssist_present )
  {
    lsm_raw_tow_type *p_zTow = &p_zSmlcAssist->z_SmlcTow[0];
    struct rrlp_GPSTOWAssist *p_rrlp_tow = p_assist->referenceTime.gpsTowAssist;

    /* Initialize TOW array */
    memset( p_zSmlcAssist->z_SmlcTow, 0, sizeof( p_zSmlcAssist->z_SmlcTow ) );

    while ( p_rrlp_tow != NULL )
    {
      /* SvId */
      p_zTow->u_SvId = (U8)p_rrlp_tow->value.satelliteID + 1;

      /* Tlm word */
      p_zTow->w_TlmWord = (U16)p_rrlp_tow->value.tlmWord;

      /* AntiSpoof */
      p_zTow->u_AntiSpoofFlag = (U8)p_rrlp_tow->value.antiSpoof;

      /* Alert */
      p_zTow->u_AlertFlag = (U8)p_rrlp_tow->value.alert;

      /* TlmRsvBits */
      p_zTow->u_TlmRsvBits = (U8)p_rrlp_tow->value.tlmRsvdBits;

      /* Update TOW pointer to next element in the list */
      p_zTow++;
      p_rrlp_tow = p_rrlp_tow->next;
    }
  } /* close if-GpsToW-Assist-available */
  #endif

  z_umts_cp_gsm_session_status.q_agps_data_rcvd |= C_GPS_ASSIS_REFTIME;
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_process_ref_pos

DESCRIPTION
  This function processes the OSS ASN.1 decoded RRLP ref_pos data and 
  prepares it for injection into MGP.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static void tm_umts_cp_gsm_process_ref_pos
(
  rrlp_ControlHeader   *p_assist 
)
{
  tm_post_data_payload_type                 z_tm_post_data_payload;
  boolean                                   parse_ok;
  gad_ellip_alt_unc_ellip_type              *p_gad_ellip_alt_unc_ellip;
  uint32                q_time_stamp_sec = 0;
  pdsm_pd_event_type    q_pd_event; /* pdsm_pd_event_type is uint32 */
  pdsm_pd_info_s_type   pd_info_buffer;

  MSG_MED("CP-GSM: process_ref_pos", 0, 0, 0);
  
  memset(&pd_info_buffer, 0, sizeof(pdsm_pd_info_s_type));  
  /*
   *  This is for storing the reference position in the original GAD shape
   *  format, in case there is a need later to report this back to the 
   *  network
   */
  p_gad_ellip_alt_unc_ellip =
          &z_last_refloc_info.z_RefPos_GAD.gad_ellip_alt_unc_ellip;
 
  parse_ok = tm_rrlp_refpos_to_TmCore_post_data_payload( 
                                    p_assist, 
                                    NULL,
                                    &z_tm_post_data_payload, 
                                    (uint8 *)p_gad_ellip_alt_unc_ellip,
                                    TRUE );
  if( parse_ok )
  {
    tm_umts_cp_gsm_post_data( TM_POST_DATA_TYPE_SEED,
                              &z_tm_post_data_payload );

  
  if ( z_umts_cp_gsm_session_status.q_agps_data_rcvd & C_GPS_ASSIS_REFTIME )
  {

      q_time_stamp_sec = (uint32)(tm_umts_common_utils_full_gps_ms(gnss_GpsWeekExtend((uint16)p_assist->referenceTime.gpsTime.gpsWeek),
                                                                   p_assist->referenceTime.gpsTime.gpsTOW23b
                                                                   ) / SEC_MSECS
                                  );

  }

  (void)tm_umts_common_utils_gad_to_pdsm_pd_info 
  ( z_umts_cp_gsm_session_status.q_pdapi_client_id,
    (uint8 *)&z_last_refloc_info.z_RefPos_GAD.gad_ellip_alt_unc_ellip, 
    PDSM_PD_POSITION_TYPE_NEW,
    PDSM_PD_POSITION_MODE_UNKNOWN,
    PDSM_PD_POSITION_SOURCE_DEFAULT,
    q_time_stamp_sec,
    &pd_info_buffer,
    &q_pd_event
  );


  tm_post_ext_status_ref_loc_rep_event( 
    TM_PRTL_TYPE_UMTS_CP,
    z_umts_cp_gsm_session_status.tm_active_session_handle,
    &pd_info_buffer.pd_info.pd_data);

  }

}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_process_nav_model

DESCRIPTION
  This function processes the OSS ASN.1 decoded RRLP nav_model and
  prepares it for injection into MGP.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static void tm_umts_cp_gsm_process_nav_model
(
  const rrlp_ControlHeader  *p_assist,
  uint16                    w_gps_week 
)
{
  tm_post_data_payload_type         z_tm_post_data_payload;
  const rrlp_SeqOfNavModelElement   *navModelList;
  rrlp_UncompressedEphemeris        *p_Ephem;
  rrlp_NavModelElement              *p_navModelList_elem;
  
  uint32                            q_sv_id;
  uint8                             u_i;

  MSG_MED("CP-GSM: process_nav_model", 0, 0, 0);
  
    
  navModelList = &p_assist->navigationModel.navModelList;
  p_navModelList_elem = navModelList->elem;

  for ( u_i=0; u_i < navModelList->n; u_i++ )
  {
    /* SvId */
    q_sv_id = p_navModelList_elem->satelliteID + 1;
         
    z_tm_post_data_payload.eph_data.u_Sv = (uint8)q_sv_id;

    /* SatStatus ChoiceTag - currently SMLC only supports
       newNavModelUC (2) */
    if ( p_navModelList_elem->satStatus.t == rrlp_oldSatelliteAndModel_chosen ) 
    {
      /* oldStatelliteAndModel: no encoded eph.
         values for this SV will follow */
      if ( u_i < ( navModelList->n - 1) )
      {
        p_navModelList_elem++;
      }
      continue;
    }

    if ( p_navModelList_elem->satStatus.t == rrlp_newSatelliteAndModelUC_chosen )
    {
      p_Ephem = p_navModelList_elem->satStatus.u.newSatelliteAndModelUC;  
    }
    else if ( p_navModelList_elem->satStatus.t == rrlp_newNaviModelUC_chosen )
    {
      p_Ephem = p_navModelList_elem->satStatus.u.newNaviModelUC;
    }
    else
    {
      /* Unknown */
      MSG_HIGH("Unknown Sat Status",0,0,0);
      if ( u_i < ( navModelList->n - 1) )
      {
        p_navModelList_elem++;
      }
      continue;
    }

    tm_rrlp_eph_to_TmCore_post_data_payload(  p_Ephem, 
                                              &z_tm_post_data_payload, 
                                              w_gps_week);

    z_umts_cp_gsm_session_status.q_eph_sv_list |= (1L << q_sv_id);     
 
    tm_umts_cp_gsm_post_data( TM_POST_DATA_TYPE_EPH,
                              &z_tm_post_data_payload );

    /* Go to next element in the list */
    if ( u_i < ( navModelList->n - 1) )
    {
      p_navModelList_elem++;
  }
  }

  MSG_MED("GSM-CP: EphSvList=0x%lx", 
          z_umts_cp_gsm_session_status.q_eph_sv_list, 0, 0);
          
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_process_iono_model

DESCRIPTION
  This function processes the OSS ASN.1 decoded RRLP iono_model and
  prepares it for injection into MGP.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static void tm_umts_cp_gsm_process_iono_model
(
  rrlp_ControlHeader   *p_assist 
)
{
  tm_post_data_payload_type                 z_tm_post_data_payload;
 
  MSG_MED("CP-GSM: process iono model", 0, 0, 0);
    
  tm_rrlp_Iono_to_TmCore_post_data_payload( p_assist, 
                                            &z_tm_post_data_payload );

  tm_umts_cp_gsm_post_data( TM_POST_DATA_TYPE_IONO,
                            &z_tm_post_data_payload );
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_process_utc

DESCRIPTION
  This function processes the OSS ASN.1 decoded RRLP UTC data and
  prepares it for injection into MGP.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static void tm_umts_cp_gsm_process_utc
(
  rrlp_ControlHeader   *p_assist 
)
{
  tm_post_data_payload_type                 z_tm_post_data_payload;
  
  MSG_MED("CP-GSM: process_utc", 0, 0, 0);
    
  tm_rrlp_UtcModel_to_TmCore_post_data_payload( p_assist, 
                                                &z_tm_post_data_payload );

  tm_umts_cp_gsm_post_data( TM_POST_DATA_TYPE_UTC_MODEL,
                            &z_tm_post_data_payload );
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_process_almanac

DESCRIPTION
  This function processes the OSS ASN.1 decoded RRLP almanac data and
  prepares it for injection into MGP.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static void tm_umts_cp_gsm_process_almanac
( 
  const rrlp_ControlHeader  *p_assist,
  uint16                    w_GpsWeek 
)
{
  tm_post_data_payload_type           z_tm_post_data_payload;
  const rrlp_SeqOfAlmanacElement      *almanacList;
  rrlp_AlmanacElement                 *p_Almanac;
  uint16                              w_AlmanacWeek;
  uint8                               u_i;
  
  MSG_MED("CP-GSM: process_almanac", 0, 0, 0);
  
  almanacList = &p_assist->almanac.almanacList;

  /* Almanac WNa  */
  w_AlmanacWeek = (uint16)p_assist->almanac.alamanacWNa; /* 8 LSBs of the full 10-bit week
                                                             number */

  /*
   *  IF reference time is present then 
   *    go ahead and form a 10-bit week number
   *  ELSE
   *    leave it as an 8-bit value and PE will handle the rest.
   *  ENDIF
   */
  if ( p_assist->m.referenceTimePresent )
  {
    uint16 w_projected_alm_week; 
    int16  s_delta_week = 0;

    /*
     *  Create a 16-bit week number
     */
    w_projected_alm_week = w_AlmanacWeek & 0x00FF;
    w_projected_alm_week |= w_GpsWeek & 0xFF00;
    
    s_delta_week = (int16) w_GpsWeek - (int16)w_projected_alm_week;

    /*
     *  Assuming the true intended almanac week can't be 2.5 years away
     *  from the current time, a difference larger than 2.5 years can
     *  be attributed to the rollover of the 8-bit boundary to which
     *  RRLP almanac week is limited.
     */
    if ( s_delta_week > 128 )
    {
      w_projected_alm_week += 256;
    }
    else if ( s_delta_week < -128 )
    {
      w_projected_alm_week -= 256;
    }

    /* alm. week reconciled */ 
    w_AlmanacWeek = w_projected_alm_week;

    w_AlmanacWeek &= 0x03FF; /* trancate back to 10 bits */
  }
  else
  {
    MSG_LOW("CP-GSM: can't get 10-bit alm. week since there is no ref time", 0, 0, 0);
  }
  
  p_Almanac = almanacList->elem;

  /* start almanac list */
  for ( u_i=0; u_i<p_assist->almanac.almanacList.n; u_i++ )
  {
    uint8 u_SvId;

    u_SvId = (uint8)p_Almanac->satelliteID + 1; /* 6-bit unsigned */

    if ( u_SvId > N_SV )
    {
      if ( u_i < (p_assist->almanac.almanacList.n - 1) )
      {
        p_Almanac++;
      }
      continue;
    }

    tm_rrlp_alm_to_TmCore_post_data_payload(  p_Almanac, 
                                              &z_tm_post_data_payload );
    z_tm_post_data_payload.alm_data.w_GpsWeek = w_AlmanacWeek;

    z_umts_cp_gsm_session_status.q_alm_sv_list |= (1L << u_SvId);

    tm_umts_cp_gsm_post_data( TM_POST_DATA_TYPE_ALM,
                              &z_tm_post_data_payload );

    /* Move to next element of the list */
    if ( u_i < (p_assist->almanac.almanacList.n - 1) )
    {
      p_Almanac++;
    }  
  }

  MSG_MED("GSM-CP: AlmSvList=0x%lx", 
          z_umts_cp_gsm_session_status.q_alm_sv_list, 0, 0);
          
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_process_acq_assist

DESCRIPTION
  This function processes the OSS ASN.1 decoded RRLP Acquisition assist data
  and prepares it for injection into MGP.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static void tm_umts_cp_gsm_process_acq_assist
(
  rrlp_ControlHeader   *p_assist 
)
{
  tm_post_data_payload_type     z_tm_post_data_payload;
  uint16                        w_gps_week = C_GPS_WEEK_UNKNOWN;
  tm_post_data_payload_type     z_local_cached_post_data_payload;
  msa_pos_time_stamp_type       *p_msa_pos_time_stamp;
  tm_post_data_payload_type     *p_global_cached;
  uint8                         u_i;
  uint8                         u_num_existing_svs;
  

  MSG_MED("CP-GSM: process_acq_assist", 0, 0, 0);
  p_global_cached = NULL;
  p_msa_pos_time_stamp  = &z_umts_cp_gsm_session_status.z_msa_pos_time_stamp;
  memset(&z_local_cached_post_data_payload, 0, sizeof(tm_post_data_payload_type) );
    
  if ( ( z_umts_cp_gsm_session_status.e_agps_mode == 
          C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED ) &&
        ( z_umts_cp_gsm_session_status.u_mpr_rcvd == TRUE ) &&
        !z_umts_cp_gsm_session_status.z_cached_acq_assist.u_valid
     )    
                                
  {
 
    if ( p_assist->m.referenceTimePresent )
    {
      /* use the week number in the current message if available */
      w_gps_week = gnss_GpsWeekExtend(
                   (uint16)p_assist->referenceTime.gpsTime.gpsWeek);
    }
    else
    {
      /* use the week number in a previous message if available */
      if ( p_msa_pos_time_stamp->u_ref_time_week_num_valid )
      {
        w_gps_week = p_msa_pos_time_stamp->w_ref_time_week_num;
      }
    }

    /* Only if it is known to be MSA at this point, and MPR has arrived, also
       that there has been no cached acq. assist. (note acq. assist could 
       arrive in different segmented RRLP ), would injectin take place here
     */  

    tm_umts_cp_gsm_inform_core_nt_assist_end(); 
  
    /* inject SV directions */  
    tm_rrlp_AcqAssistSvDir_to_TmCore_post_data_payload( 
                                                    p_assist, 
                                                    &z_tm_post_data_payload );
    if( z_tm_post_data_payload.gnss_sv_dir.u_NumSvs ) 
    {
      tm_umts_cp_gsm_post_data( TM_POST_DATA_TYPE_SV_DIR,
                                &z_tm_post_data_payload );
    }
     
    /* prepare data for the format suitable for posting */   
    tm_rrlp_AcqAssistSteering_to_TmCore_post_data_payload( 
                                                p_assist, 
                                                &z_tm_post_data_payload, 
                                                w_gps_week );
    /* posting steering */ 
    tm_umts_cp_gsm_post_data( TM_POST_DATA_TYPE_AA,
                              &z_tm_post_data_payload );
  }
  
  else /* receiving acq. assist. while not ready to inject */
  {
    
    /* note although map_post_data_to_bit_mask() is called, it only means
       to register the receipt of ACQ assist. data; such data is not yet
       posted in this case */
    z_umts_cp_gsm_session_status.q_agps_data_rcvd |= 
                        map_post_data_to_bit_mask( TM_POST_DATA_TYPE_AA );
                        
    /* prepare data for SV directions */      
    tm_rrlp_AcqAssistSvDir_to_TmCore_post_data_payload( 
     p_assist, 
     &z_local_cached_post_data_payload );
     
    /* point to the SV dir. part of the global cache */ 
    p_global_cached = 
     &z_umts_cp_gsm_session_status.z_cached_acq_assist.z_cached_svdir;
     
    /* move SV_dir info. in this RRLP message to the global cache; this is done
       in this way to accommodate the possbile scenarion where ACQ is delivered 
       in multiple RRLP messages. */ 
  u_num_existing_svs = p_global_cached->gnss_aa_data.u_NumSvs;
    for ( u_i=0; 
          ( (u_i < z_local_cached_post_data_payload.gnss_sv_dir.u_NumSvs) &&
            (u_i < ARR_SIZE(z_local_cached_post_data_payload.gnss_sv_dir.z_SvAzElInfo)) &&
           ((u_i + u_num_existing_svs) < ARR_SIZE(p_global_cached->gnss_sv_dir.z_SvAzElInfo)) );
          u_i++ )
    {
      p_global_cached->gnss_sv_dir.z_SvAzElInfo[u_i + u_num_existing_svs] = z_local_cached_post_data_payload.gnss_sv_dir.z_SvAzElInfo[u_i];
      p_global_cached->gnss_sv_dir.u_NumSvs++;
    }

    /* point to the steering part of the global cache */ 
    p_global_cached = 
     &z_umts_cp_gsm_session_status.z_cached_acq_assist.z_cached_steer;
     
    if ( p_assist->m.referenceTimePresent )
    {
      /* use the week number in the current message if available */
      w_gps_week = gnss_GpsWeekExtend(
                   (uint16)p_assist->referenceTime.gpsTime.gpsWeek);
    }  
    else
    {
      /* use the week number in a previous message if available */
      if ( p_msa_pos_time_stamp->u_ref_time_week_num_valid )
      {
        w_gps_week = p_msa_pos_time_stamp->w_ref_time_week_num;
      }
    }
  
       
    /* prepare data for steering */ 
    tm_rrlp_AcqAssistSteering_to_TmCore_post_data_payload( 
      p_assist, 
      &z_local_cached_post_data_payload, 
      w_gps_week );


    if ( !z_umts_cp_gsm_session_status.z_cached_acq_assist.u_valid )
    {
      /* If first time caching ACQ */
      memscpy( &p_global_cached->gnss_aa_data, 
              sizeof(p_global_cached->gnss_aa_data),
              &z_local_cached_post_data_payload.gnss_aa_data,
              sizeof(z_local_cached_post_data_payload.gnss_aa_data)
            ); 
    } 
    else 
    {       
    
      if ( z_local_cached_post_data_payload.gnss_aa_data.q_Msec !=
           p_global_cached->gnss_aa_data.q_Msec )
      {
        /* Note that while technically still compliant, splitting acquisistion 
           assistance accross different RRLP messages by SMLC makes no practical 
           sense and thus should be discouraged. If this happens and if TOAs are 
           different among AcqAssist from different messages, the segment with 
           most number of SVs will be injected to MGP, as it is warranted to 
           propagate AcqAssist for this scenario that should not happen in the first
           place */
           
        if ( z_local_cached_post_data_payload.gnss_aa_data.u_NumSvs >  
              p_global_cached->gnss_aa_data.u_NumSvs )
        {
          memscpy( &p_global_cached->gnss_aa_data, 
		  	      sizeof(p_global_cached->gnss_aa_data),
                  &z_local_cached_post_data_payload.gnss_aa_data,
                  sizeof(z_local_cached_post_data_payload.gnss_aa_data)
                ); 
        }     
      
      } /* new and existing ACQ have different TOA */ 
      
      else
      {
        u_num_existing_svs = p_global_cached->gnss_aa_data.u_NumSvs;
        for ( u_i=0; u_i < z_local_cached_post_data_payload.gnss_aa_data.u_NumSvs; u_i++ )
        {
          if ((p_global_cached->gnss_aa_data.u_NumSvs >= N_SV ) || (u_i >= N_SV)	|| ((u_num_existing_svs + u_i) >= N_SV))
            break;  /* buffer limit check */

          memscpy( &p_global_cached->gnss_aa_data.z_SvSteerInfo[u_num_existing_svs + u_i ],
		  	      sizeof(p_global_cached->gnss_aa_data.z_SvSteerInfo[u_num_existing_svs + u_i ]),
                  &z_local_cached_post_data_payload.gnss_aa_data.z_SvSteerInfo[u_i],
                  sizeof(z_local_cached_post_data_payload.gnss_aa_data.z_SvSteerInfo[u_i])
                );  
          p_global_cached->gnss_aa_data.u_NumSvs++;      
        }
      } /* TOA same as that of the already cached */  
    } /* if not 1st time to cache ACQ */
    
    /* cache acq. assistance data flag ON */
    z_umts_cp_gsm_session_status.z_cached_acq_assist.u_valid = TRUE;
      
    MSG_HIGH("Caching ACQ: Sv_dir %u  Steer %u", 
     z_umts_cp_gsm_session_status.z_cached_acq_assist.z_cached_svdir.gnss_sv_dir.u_NumSvs,
     z_umts_cp_gsm_session_status.z_cached_acq_assist.z_cached_steer.gnss_aa_data.u_NumSvs,
     0 );                        
  
  }  
  
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_process_rti_assist

DESCRIPTION
  This function processes the OSS ASN.1 decoded RRLP RTI assist data
  and prepares it for injection into MGP.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static void tm_umts_cp_gsm_process_rti_assist( rrlp_ControlHeader *p_assist)
{
  rrlp_SeqOf_BadSatelliteSet const *const p_RTI = &p_assist->realTimeIntegrity; 
  rrlp_SatelliteID *p_SatelliteID;

  uint32 q_RTIValidSvs = 0;
  uint32 q_RTIInValidSvs = 0;
  uint32 u_i;
  uint32 q_Word; 

  p_SatelliteID = p_RTI->elem;

  for ( u_i = 0; u_i < p_RTI->n; u_i++ )
  {
    q_Word = *p_SatelliteID;

    if ( q_Word < N_GPS_SV ) 
    {
      q_RTIValidSvs |= 1L << q_Word;
    }
    else
    {
      q_RTIInValidSvs |= 1L << (q_Word - N_GPS_SV); 
    }

    /* Move to next element of the list */
    if ( u_i < (p_RTI->n - 1) )
    {
      p_SatelliteID++;
    }
  }

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Received RRLP GPS RTI information 0x%X, 0x%X (SV > 32)", q_RTIValidSvs,
        q_RTIInValidSvs); 

  /* Determine if we have enough info. about some SVs' health and
     inform MGP about such info., if available */
  tm_util_post_unhealthy_svs(TM_PRTL_TYPE_UMTS_CP,
                             z_umts_cp_gsm_session_status.tm_active_session_handle, 
                              q_RTIValidSvs); 
}

/*===========================================================================

FUNCTION tm_umts_cp_gsm_process_assistance_data

DESCRIPTION
  This function processes the OSS ASN.1 decoded RRLP assistance data
  and prepares it for injection into MGP.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static boolean tm_umts_cp_gsm_process_assistance_data
(
  rrlp_ControlHeader  *p_assist )
{
  uint16    w_gps_week = 0;
  
  /**************** Real Time Integrity  **********************/
  /* 
  * Note : CD/ME would benefit only when RTI is injected before RefLoc, NavModel & Alm.
  */
  if ( p_assist->m.realTimeIntegrityPresent )
  {
    tm_umts_cp_gsm_process_rti_assist(p_assist);
  }

  /************** REFERENCE TIME **********************************/

  if ( p_assist->m.referenceTimePresent )
  {
    tm_umts_cp_gsm_process_ref_time( p_assist, &w_gps_week );
  }


  /************** REFERENCE LOCATION **********************************/

  if ( p_assist->m.refLocationPresent )
  {
    tm_umts_cp_gsm_process_ref_pos( p_assist );
  }


  /*********  DGPS *********************************************/

  if ( p_assist->m.dgpsCorrectionsPresent )
  {
    MSG_MED("CP-GSM: Ignoring DGPS data", 0, 0, 0);
    
    /* Not supported, ignored */
  }


  /************** Nav. Model **********************************/

  if ( p_assist->m.navigationModelPresent )
  {
    if ( !p_assist->m.referenceTimePresent )
    {
      MSG_LOW("CP-GSM: can't get eph. week since there is no ref time", 0, 0, 0);
    }

    tm_umts_cp_gsm_process_nav_model( p_assist, w_gps_week );
  }

  /****************************** IONO ***************************************/

  if ( p_assist->m.ionosphericModelPresent )
  {
    tm_umts_cp_gsm_process_iono_model( p_assist );
  }


  /****************************** UTC ****************************************/

  if ( p_assist->m.utcModelPresent )
  {
    tm_umts_cp_gsm_process_utc( p_assist );
  }

  /******************* almanac ***********************************************/

  if ( p_assist->m.almanacPresent )
  {
    tm_umts_cp_gsm_process_almanac( p_assist, w_gps_week );
  }

  /**************** acquisition assistance **********************/

  if ( p_assist->m.acquisAssistPresent )
  {
    tm_umts_cp_gsm_process_acq_assist( p_assist );
  }

  return ( TRUE );
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_rrlp_msr_pos_req

DESCRIPTION
  This function processes the OSS ASN.1 decoded RRLP-msrPosReq and prepares
  such data for TM usage.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static boolean tm_umts_cp_gsm_rrlp_msr_pos_req
( 
  rrlp_MsrPosition_Req  *p_mpr 
)
{
  uint8                                 success;
  tm_network_assist_s_type              *p_net_assist;
  rrlp_params_s_type                    *p_rrlp_params;
  rrlp_PositionInstruct                 *p_rrlp_pos_instruct;
  boolean                               u_invalid_switch;
  umts_cp_gsm_callflow_kind_enum_type   e_callflow_kind;
  umts_cp_gsm_session_agps_mode_enum_type e_agps_mode;
  int32                                 l_long_word;

  p_net_assist        = &z_umts_cp_gsm_session_status.z_network_assist;
  p_rrlp_params       = &p_net_assist->radio_link_prot_params.z_rrlp_params;
  p_rrlp_pos_instruct = &p_mpr->positionInstruct;
  success             = TRUE;
  u_invalid_switch    = FALSE;
  e_callflow_kind     = z_umts_cp_gsm_session_status.e_callflow_kind;
  e_agps_mode         = z_umts_cp_gsm_session_status.e_agps_mode;
  l_long_word         = 0;
 
  /*First MPR is received, If E911 simulaiton is ON and no voice call was originated, 
    u_e911_in_progress will not be set. Set it now*/
  if ( (z_umts_cp_gsm_session_status.u_mpr_rcvd == FALSE) && 
        (tm_umts_common_utils_emergency_effect() != C_EMERGENCY_HANDLING_NULL)
     ) 
  {
    z_umts_cp_gsm_data.u_e911_in_progress = TRUE;
  }

  if ( p_mpr->m.rel5_MsrPosition_Req_extensionPresent  )
  {
    p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_present = TRUE;

    p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_ref.smlc_code =
      p_mpr->rel5_MsrPosition_Req_extension.extended_reference.smlc_code;

    p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_ref.transaction_ID =
      p_mpr->rel5_MsrPosition_Req_extension.extended_reference.transaction_ID;
  }
  

  /*
   *  Reject invalid requests
   */
  if( p_rrlp_pos_instruct->positionMethod == rrlp_PositionMethod_eotd )
  {
    MSG_ERROR("CP-GSM: rrlp_eotd not supported", 0, 0, 0);
    u_invalid_switch = TRUE;
  } 

  /*
   *  IF doing an MO session THEN 
   *    double check to make sure that the MPR position
   *    method chosen matches.  Return an error if not.
   *  ENDIF
   */
  if( e_callflow_kind == C_UMTS_CP_GSM_SESSION_UI)
  {
    if( (p_rrlp_pos_instruct->methodType.t == rrlp_msAssisted_chosen) &&
        (e_agps_mode != C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED) )
    {
      MSG_ERROR("CP-GSM: Invalid switch to MS-Assisted", 0, 0, 0);
      u_invalid_switch = TRUE;
    }
    else if( (p_rrlp_pos_instruct->methodType.t == rrlp_msBased_chosen) &&
             (e_agps_mode != C_UMTS_CP_GSM_AGPS_MODE_UE_BASED) )
    {
      MSG_ERROR("CP-GSM: Invalid switch to MS-Based", 0, 0, 0);
      u_invalid_switch = TRUE;
    }

    /*
     *  NOTE - the other two choices (msBasedPreferred and msAssistedPreferred)
     *  are only requests.  It is not an error to switch the preference.
     *  Our code will simply ignore the request.
     */
  }

  if( u_invalid_switch )
  {
    if( !tm_umts_cp_gsm_tx_rrlp_mpr_err( rrlp_methodNotSupported ) )
    {
      tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_GENERAL_ERROR );
    }
    return FALSE;
  }

  /*
   *  IF doing an MT session THEN
   *    This is where MS-Assisted or MS-Based final gets chosen.
   *    IF the server requests a different "method" here THEN
   *      switch to the new "method"
   *    ENDIF
   *  ENDIF
   */
  if( z_umts_cp_gsm_session_status.e_callflow_kind == C_UMTS_CP_GSM_SESSION_NI)
  {
    if( p_rrlp_pos_instruct->methodType.t == rrlp_msBased_chosen )
    {
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_2G_CP_MS_BASED) )
      {
        z_umts_cp_gsm_session_status.e_agps_mode = 
                                    C_UMTS_CP_GSM_AGPS_MODE_UE_BASED;
      }
      else
      {
        (void) tm_umts_cp_gsm_tx_rrlp_mpr_err( rrlp_methodNotSupported );
        return FALSE;
      }
    }
    else if( p_rrlp_pos_instruct->methodType.t == rrlp_msAssisted_chosen )
    {
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_2G_CP_MS_ASSISTED) )
      {
        z_umts_cp_gsm_session_status.e_agps_mode = 
                                    C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED;
      }
      else
      {
        (void) tm_umts_cp_gsm_tx_rrlp_mpr_err( rrlp_methodNotSupported );
        return FALSE;
      }
    }
    else if ( p_rrlp_pos_instruct->methodType.t == rrlp_msBasedPref_chosen )
    {
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_2G_CP_MS_BASED) )
      {
        z_umts_cp_gsm_session_status.e_agps_mode = 
                                    C_UMTS_CP_GSM_AGPS_MODE_UE_BASED;
      }
      else if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_2G_CP_MS_ASSISTED) )
      {
        z_umts_cp_gsm_session_status.e_agps_mode = 
                                    C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED;
      }
      else
      { 
        (void) tm_umts_cp_gsm_tx_rrlp_mpr_err( rrlp_methodNotSupported );
        return FALSE;
      }
    }
    else if ( p_rrlp_pos_instruct->methodType.t == rrlp_msAssistedPref_chosen )
    {
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_2G_CP_MS_ASSISTED) )
      {
        z_umts_cp_gsm_session_status.e_agps_mode = 
                                    C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED;
      }
      else if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_2G_CP_MS_BASED) )
      {
        z_umts_cp_gsm_session_status.e_agps_mode = 
                                    C_UMTS_CP_GSM_AGPS_MODE_UE_BASED;
      }
      else
      { 
        (void) tm_umts_cp_gsm_tx_rrlp_mpr_err( rrlp_methodNotSupported );
        return FALSE;
      }
    }

  }

  /*
   *  IF a PDAPI fix has already been reported THEN
   *    indicate to TM Core that there is no more data coming
   *    send the cached fix to the SMLC
   *    stop the session
   *    return without processing the rest of the info
   *  ELSE
   *    Process the MsrPosReq packet; providing new Qos, new positioning
   *    modes, etc. to LM as necessary and set the RRLP MEAS RESP timer
   *  ENDIF
   */
  if( z_umts_cp_gsm_session_status.u_pdapi_fix_reported )
  {
    MSG_MED("CP-GSM: Sending Cached Position to Network", 0, 0, 0);

    tm_umts_cp_gsm_send_pos_rrlp( 
                &z_umts_cp_gsm_session_status.z_pending_pos.z_pos,
                z_umts_cp_gsm_session_status.z_pending_pos.e_fix_type,
                z_umts_cp_gsm_session_status.z_pending_pos.q_time_stamp_msec );

    tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_COMPLETED );

    return TRUE;
  }

  /*
   *  Signal the fact that we have received an MPR.  This will be used
   *  in MT Callflows to set the "u_final_mpr_rcvd" flag when we
   *  finally determine that we need no more assistance data.
   */
  z_umts_cp_gsm_session_status.u_mpr_rcvd = TRUE;

  /*
   *  Process this MPR and update QOS information
   */
  tm_umts_cp_gsm_proc_msr_pos_req_qos( p_mpr );

  /*
   *  IF doing an MO callflow OR the wishlist has already been requested THEN
   *    Set a flag indicating that the "final" MPR request has been rcvd
   *    This will be used later to decide whethr to cache position or not
   *  ENDIF
   */
  if( z_umts_cp_gsm_session_status.u_wishlist_requested )
  {
    MSG_LOW("CP-GSM: Setting u_final_mpr_rcvd", 0, 0, 0);

    z_umts_cp_gsm_session_status.u_final_mpr_rcvd = TRUE;
  }


  if ( p_mpr->m.gps_AssistDataPresent )
  {
    success = tm_umts_cp_gsm_process_assistance_data( 
                                    &p_mpr->gps_AssistData.controlHeader );
  }


  /*
   *  IF we have already requested additional assistance date THEN
   *    Set the Response Timer
   *    Change to the "Wait for Response" state
   *    Indicate End Of Assistance Data and Network Callflow to TM-Core
   *  ENDIF
   */
  if( z_umts_cp_gsm_session_status.u_wishlist_requested )
  {
  
    tm_umts_common_send_pdapi_comm_event(
                     PDSM_PD_COMM_PROTOCOL_UMTS_CP_GSM,
                     C_COMM_DONE,
                     z_umts_cp_gsm_session_status.tm_active_session_handle ,
                     z_umts_cp_gsm_session_status.q_pdapi_client_id
                     );
  
    /* Don't repeat these if it has already been in the state of
       waiting-to-report, which could have been entered if this MPR
       contains the only set of ACQ data of the session */   
    if ( z_umts_cp_gsm_session_status.e_cf_state !=
         C_UMTS_CP_GSM_CF_STATE_TIMER_RRLP_MEAS_RESP_TIME )
    {
       
      tm_umts_cp_gsm_set_cf_state( 
                  C_UMTS_CP_GSM_CF_STATE_TIMER_RRLP_MEAS_RESP_TIME );

      tm_umts_cp_gsm_inform_core_nt_assist_end();
    
      /* If this RRLP is of MSA, check if there is cached ACQ. assist., 
         inject the cached if available. */
      if ((z_umts_cp_gsm_session_status.e_agps_mode == 
             C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED ) &&
           z_umts_cp_gsm_session_status.z_cached_acq_assist.u_valid 
         )
      {  
        tm_umts_cp_gsm_post_cached_acq_assist();
      }
    }   
  }
  
  else
  {
    tm_umts_cp_gsm_request_wishlist();
  }
  
  /* Also need to check if there is need to start max_mgp_on timer
     or the extend it if already on:
     
     If timer is not on, start it here for the purpose of possible 
     later preemptions, to avoid MGP to run all the way to the maximum default
     of 255 seconds. Timer value should be the longer of 40 seconds or whatever
     RRLP-mpr responseTime dictates.
     
     If the timer is already on, and the remaining time is shorter than this 
     the responseTime requested by this current RRLP-mpr, set this timer so 
     that MGP will be kept on for the maximum time of the responseTime 
     
  */
     
  l_long_word = os_TimerCheck(z_umts_cp_gsm_data.p_max_mgp_on_timer); 
  
  if ( l_long_word == 0 ) /* max_mgp_on timer not currently running */
  {
    MSG_HIGH("Start max_mgp_on timer", 0, 0, 0);
  
    if ( ( p_rrlp_params->u_measure_response_time_sec * 1000 ) > 
         TM_UMTS_CP_GSM_MAX_MGP_ON_IN_MS 
       )
    {
      tm_umts_cp_gsm_start_max_mgp_on_timer( 
        p_rrlp_params->u_measure_response_time_sec * 1000 );
    
    }
    else
    {     
      tm_umts_cp_gsm_start_max_mgp_on_timer(TM_UMTS_CP_GSM_MAX_MGP_ON_IN_MS );
    }
  }

  else if ( ( (uint32)p_rrlp_params->u_measure_response_time_sec * 1000 ) >
            (uint32)l_long_word 
          ) /* max_mgp_on timer is currently running */ 
  {
  
    MSG_HIGH("mprRespTime > remaining max_revr_on: %u sec, %u msec", 
              p_rrlp_params->u_measure_response_time_sec, 
              l_long_word, 
              0);
  
    /* stop current running timer in order to restart it with new value */
    tm_umts_cp_gsm_cancel_max_mgp_on_timer();
       
    tm_umts_cp_gsm_start_max_mgp_on_timer( 
      p_rrlp_params->u_measure_response_time_sec * 1000 );
         
  } 
 
  return ( success );
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_proc_msr_pos_req_qos_msa

DESCRIPTION
  This function processes QoS related information from MsrPositionReq
  packets for MS-Assisted callflows
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/


static uint8 tm_umts_cp_gsm_proc_msr_pos_req_qos_msa
(
  const rrlp_PositionInstruct         *p_rrlp_pos_instruct,
  rrlp_params_s_type                  *p_rrlp_params,
  uint32                              *p_hor_acc_meters,
  uint8                               u_meas_resp_time
)
{
  MSG_LOW("CP-GSM: proc_msr_pos_req_qos_msa", 0, 0, 0);
  

  /*
   *  Update the measure response time in the rrlp params
   */
  p_rrlp_params->u_measure_response_time_sec = u_meas_resp_time;

  /*
   *  IF this is an MTLR/NILR callflow AND
   *    the network has provided an accuracy threshold THEN
   *      Override our default accuracy threshold with the network provided 
   *      accuracy threshold; however, this has no real effect on MGP because
   *      in MSA mode, MGP cannot determine quality of measurement in
   *      terms of meters. It can be used by TM in determining whether it 
   *      should instruct the LM/MGP to run all the way to timeout for E911
   *      purposes by comparing it to the value of 0.
   *  ENDIF
   */
   
  *p_hor_acc_meters = 255; /* arbitary init. for later possible comparision
                              with 0 */
   
  /* Being MSA here could be the result of any of the following:
     p_rrlp_pos_instruct->methodType.t == rrlp_msAssisted_chosen,
     p_rrlp_pos_instruct->methodType.t == rrlp_msAssistedPref_chosen,
     p_rrlp_pos_instruct->methodType.t == rrlp_msBasedPref_chosen; 
  */   
   
  if( ( p_rrlp_pos_instruct->methodType.t == rrlp_msAssisted_chosen ) &&
      ( p_rrlp_pos_instruct->methodType.u.msAssisted != NULL )
    ) 
  {                                              
     if ( p_rrlp_pos_instruct->methodType.u.msAssisted->m.accuracyPresent )
    {
      *p_hor_acc_meters = 
        (uint32) tm_umts_common_utils_GADK_to_meter( 
                        TRUE,
                        p_rrlp_pos_instruct->methodType.u.msAssisted->accuracy );
    }
  }
  
  else if ( p_rrlp_pos_instruct->methodType.t == rrlp_msAssistedPref_chosen )
  {
    *p_hor_acc_meters = 
      (uint32) tm_umts_common_utils_GADK_to_meter( 
                      TRUE,
                      p_rrlp_pos_instruct->methodType.u.msAssistedPref );
  }
  
  else if ( p_rrlp_pos_instruct->methodType.t == rrlp_msBasedPref_chosen )
  {
    *p_hor_acc_meters = 
      (uint32) tm_umts_common_utils_GADK_to_meter( 
                      TRUE,
                      p_rrlp_pos_instruct->methodType.u.msBasedPref );
  }
  
  /* first RRLP session in an E911 */
  if ( z_umts_cp_gsm_data.u_e911_in_progress &&
       !z_umts_cp_gsm_data.u_e911_subsequent_rrlp_sess )
  {
    if ( !z_umts_cp_gsm_session_status.u_preemption_flag ) 
    {
      /* if no preemption ever happened: LM shall run all the way to its 
         timeout */
      *p_hor_acc_meters = 0;  
    }
  }

  /*  Set LM timeout value = RRLP_respTime - 1.
   *  This return value will be used by LM as its timeout.
   *  Return the network provided  MINUS 1 so that LM will time out
   *  first; If there is a previously disqualified position, it
   *  can be reported at LM timeout, before TM has to report to the network.
   */
   
   
  return (u_meas_resp_time - 1);
  
}

/*===========================================================================

FUNCTION tm_umts_cp_gsm_proc_msr_pos_req_qos_mtmb

DESCRIPTION
  This function processes QoS related information from MsrPositionReq
  packets for MT-MB callflows
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static uint8 tm_umts_cp_gsm_proc_msr_pos_req_qos_mtmb
(
  const rrlp_PositionInstruct         *p_rrlp_pos_instruct,
  rrlp_params_s_type                  *p_rrlp_params,
  uint32                              *p_hor_acc_meters,
  uint8                               u_meas_resp_time
)
{
  boolean                             u_use_rrlp_accuracy;

  MSG_LOW("CP-GSM: proc_msr_pos_req_qos_mtmb", 0, 0, 0);
  
  u_use_rrlp_accuracy = FALSE;

  /*
   *  Update the measure response time in the rrlp params;
   *  this will be used for RRLP msrPos timeout -- use the value
   *  provided by the network ( RRLP_respTime )
   */
  p_rrlp_params->u_measure_response_time_sec = u_meas_resp_time;

  /*
   *  IF this is an MTLR/NILR callflow THEN
   *    Override our default accuracy threshold with the network provided 
   *    accuracy threshold unless in emergency call in which case acc_meters is 
   *    0. Note pre-emption and the expiration of AccuracyResetTimer can alter 
   *    this behavior
   *  ENDIF
   */
   
  if( z_umts_cp_gsm_session_status.e_callflow_kind == 
                                                C_UMTS_CP_GSM_SESSION_NI )
  {
    /* first RRLP session in an E911 */
    if ( z_umts_cp_gsm_data.u_e911_in_progress &&
         !z_umts_cp_gsm_data.u_e911_subsequent_rrlp_sess )
    {
      if ( !z_umts_cp_gsm_session_status.u_preemption_flag ) 
      {
        /* if no preemption ever happened: set accuracy to 0, LM
           shall run all the way to its timeout */
        *p_hor_acc_meters = 0;
      }
      else 
      {
        /* there has been preemptions */
        if ( !z_umts_cp_gsm_session_status.u_accuracy_reset_timer_expired )
        {
          /* Before AccuracyResetTimer expiration:
             set accuracy to that provided by RRLP */
          u_use_rrlp_accuracy = TRUE;

        }
        else
        {
          /* After AccuracyResetTimer expiration:
             set accuracy to 250 meters */
          *p_hor_acc_meters = 250;
        }
      }
    }
    else /* if not E911 or is E911 subsequent RRLP session, use RRLP respTime */ 
    {
      u_use_rrlp_accuracy = TRUE;
    }

    if ( u_use_rrlp_accuracy )
    {
    
      /* Being MSB here could be the result of any of the following:
         p_rrlp_pos_instruct->methodType.t == rrlp_msBased_chosen,
         p_rrlp_pos_instruct->methodType.t == rrlp_msBasedPref_chosen,
         p_rrlp_pos_instruct->methodType.t == rrlp_msAssistedPref_chosen; 
      */   
      
      if ( p_rrlp_pos_instruct->methodType.t == rrlp_msBased_chosen )
      {
        *p_hor_acc_meters = 
            (uint32) tm_umts_common_utils_GADK_to_meter( 
                       TRUE,
                       p_rrlp_pos_instruct->methodType.u.msBased );
      }
      
      else if ( p_rrlp_pos_instruct->methodType.t == rrlp_msBasedPref_chosen )
      {
        *p_hor_acc_meters = 
            (uint32) tm_umts_common_utils_GADK_to_meter( 
                       TRUE,
                       p_rrlp_pos_instruct->methodType.u.msBasedPref );
      }
      
      else if ( p_rrlp_pos_instruct->methodType.t == rrlp_msAssistedPref_chosen )
      {
        *p_hor_acc_meters = 
            (uint32) tm_umts_common_utils_GADK_to_meter( 
                       TRUE,
                       p_rrlp_pos_instruct->methodType.u.msAssistedPref );
      }
      
                       
    } /* close if use_rrlp_accuracy */

  }

  /*  Set LM timeout value = RRLP_respTime - 1.
   *  This return value will be used by LM as its timeout.
   *  Return the network provided  MINUS 1 so that LM will time out
   *  first; If there is a previously disqualified position, it
   *  can be reported at LM timeout, before TM has to report to the network.
   */
  return (u_meas_resp_time - 1);
  
}

/*===========================================================================

FUNCTION tm_umts_cp_gsm_proc_msr_pos_req_qos

DESCRIPTION
  This function processes QoS related information from MsrPositionReq
  packets and provides new values to TM-Core.  
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static void tm_umts_cp_gsm_proc_msr_pos_req_qos
(
  const rrlp_MsrPosition_Req  *p_mpr
)
{
  umts_cp_gsm_session_status_s_type   *p_sess_stat;
  tm_network_assist_s_type            *p_net_assist;
  rrlp_params_s_type                  *p_rrlp_params;
  uint8                               u_timeout_sec;
  const rrlp_PositionInstruct *p_rrlp_pos_instruct = &p_mpr->positionInstruct;

  MSG_LOW("CP-GSM: proc_msr_pos_req_qos", 0, 0, 0);
  

  /*
   *  Set up local pointers for easy (and readable) access to data structures
   */
  p_sess_stat = &z_umts_cp_gsm_session_status;
  p_net_assist = &p_sess_stat->z_network_assist;
  p_rrlp_params = &p_net_assist->radio_link_prot_params.z_rrlp_params;

  /*
   *  Fetch Meas Resp Time from the network provided structure.  Store it
   *  into a local variable that will be used to pass this value
   *  to the "mode" specific functions that follow.
   *
   */ 
  /* Determine whether the REL-7 responseTime is present */
  if ( p_mpr->m.rel7_MsrPosition_Req_extensionPresent &&
       p_mpr->rel7_MsrPosition_Req_extension.m.requiredResponseTimePresent
     )
  {
    u_timeout_sec = p_mpr->rel7_MsrPosition_Req_extension.requiredResponseTime; 
  }
  else
  {
    u_timeout_sec = (uint32)((uint32)1 << p_rrlp_pos_instruct->measureResponseTime);
  }

  /*
   *  Set the accuracy threshold to our NV default value.
   *  This can be overriden in the qos_msa or qos_msb routine if
   *  doing an MTLR/NILR callflow and the network has provided one.
   */
  p_sess_stat->z_session_qos.q_hor_acc_meters = 
                              z_umts_nv_item_ram_copy.aagps_default_qos_unc;
  
  /*
   *  Process the MsrPositionReq packet and store various values
   *  into our session_status structure for use later.  They will be used
   *  when we tell TM-Core to "continue" this session.
   */
  if( z_umts_cp_gsm_session_status.e_agps_mode == 
                                    C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED )
  {
    u_timeout_sec =
        tm_umts_cp_gsm_proc_msr_pos_req_qos_msa( 
                                p_rrlp_pos_instruct,
                                p_rrlp_params,
                                &p_sess_stat->z_session_qos.q_hor_acc_meters,
                                u_timeout_sec );
  }
  else if( z_umts_cp_gsm_session_status.e_callflow_kind == 
                                                C_UMTS_CP_GSM_SESSION_NI )
  {
    u_timeout_sec = 
          tm_umts_cp_gsm_proc_msr_pos_req_qos_mtmb(
                                p_rrlp_pos_instruct,
                                p_rrlp_params,
                                &p_sess_stat->z_session_qos.q_hor_acc_meters,
                                u_timeout_sec );
  }

  /*
   *  Save the (possibly adjusted) measure response timeout value
   *  into the session status structure.  It will get used to set
   *  the measure response timer inside tm_umts_cp_gsm_set_cf_state().
   */
  p_sess_stat->z_session_qos.u_gps_session_timeout_value_secs = u_timeout_sec;
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_decode_rrlp

DESCRIPTION
  This function decodes RRLP messages received from the SMLC
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static boolean tm_umts_cp_gsm_decode_rrlp
(
  tm_network_assist_s_type  *p_net_assist,
  uint8 *                   p_Data, 
  uint16                    w_max_len_byte,
  int *                     p_asn1_decode_status, 
  boolean *                 p_ack_required
)
{

  rrlp_PDU           z_rrlp_pdu_decoded; 
  rrlp_PDU *p_rrlp = &z_rrlp_pdu_decoded;
  int asn1_decode_status =  0xFFFFFFFF;

  boolean                                   success = TRUE;
  int8                                      b_RefNum;
  rrlp_params_s_type                        *p_rrlp_params;
  boolean                                   parse_ok;
  umts_cp_gsm_session_agps_mode_enum_type   e_agps_mode;  
  emergency_handling_e_type  e_e911_effect = C_EMERGENCY_HANDLING_NULL;
  boolean reject_ni_request = FALSE;
  
  /*
   *  Initialize return parameters
   */
  *p_ack_required = TRUE;

  /*
   *  Kick off the OSYS decoder
   */
  tm_osys_asn1_decode_pdu( p_Data, w_max_len_byte, (void *)p_rrlp, PROTO_RRLP, &asn1_decode_status);
                                      

  /*
   *  IF asn1_decode_status is none-zero THEN
   *    there was an error in decoding
   *    Output an error message and return FALSE to the caller
   *  ENDIF
   */
  if ( asn1_decode_status != 0 )
  {
    MSG_ERROR("CP-GSM: RRLP Decode Failure: %d",asn1_decode_status,0,0);
    
    /* convert ASN1 decoder specific enums to TM maintained enums */
    switch ( asn1_decode_status )
    {
      case RTERR_ENDOFBUF:  /* rtxErrCodes.h :  -2 */
        *p_asn1_decode_status = RRLP_ASN1_DECODE_ERR_PDU_TOO_SHORT;
        break;
        
      case RTERR_IDNOTFOU:  /* rtxErrCodes.h :  -3 */
      case RTERR_INVENUM:   /* rtxErrCodes.h :  -4 */
        *p_asn1_decode_status = RRLP_ASN1_DECODE_ERR_PDU_INCORRECT_DATA;
        break;
      
      default:
        *p_asn1_decode_status = RRLP_ASN1_DECODE_ERR_PDU_UNDEFINED;
    }
    
    if ( asn1_decode_status != C_ASN1_DECODE_BUFFER_NOT_ALLOCATED ) 
    {
      tm_osys_asn1_free_pdu (p_rrlp , PROTO_RRLP);
    }  
         
    return FALSE;
  }
  
  /*
   *  The PDU was decoded successfully, but it still may not be possible
   *  to operate on what was provided.
   *  Set a flag indicating that decoding was successful.  This is different
   *  than indicating that the message was successfully processed.
   */
  *p_asn1_decode_status = RRLP_ASN1_DECODE_ERR_PDU_DECODED;

  /*
   *  IF no GSM session has been started yet THEN
   *    This must be an MT/NI callflow
   *   Check whether it can be accepted
    */

  if( !z_umts_cp_gsm_session_status.u_tm_core_session_started )
  {      
    /*** check if in emergency call , and if so, any special priority issues to handle */
    e_e911_effect = tm_umts_common_utils_emergency_effect();

    if(e_e911_effect == C_EMERGENCY_HANDLING_REJ_ALL)
    {
    
      MSG_MED("RRLP message rejected due to NV item %d", 
              e_e911_effect,0,0 );
      reject_ni_request = TRUE;
      
    }
    else if(e_e911_effect == C_EMERGENCY_HANDLING_NULL)
    {
      
      if ( ( z_umts_nv_item_ram_copy.gps_lock == PDSM_GPS_LOCK_ALL ) ||
           ( z_umts_nv_item_ram_copy.gps_lock == PDSM_GPS_LOCK_MT )
         )
      {
        MSG_MED("MT GPS Locked",0,0,0 );
        reject_ni_request = TRUE;      
      }      
    }
    
    if(reject_ni_request)
    {
        tm_osys_asn1_free_pdu (p_rrlp , PROTO_RRLP);
    
        /*Send RRLP Error*/
        if( p_rrlp->component.t == rrlp_msrPositionReq_chosen )
        {        
          (void) tm_umts_cp_gsm_tx_rrlp_mpr_err( rrlp_LocErrorReason_unDefined );
        }
        return FALSE;
     }    
   }

  /* tell TmCore to send pdapi event of EventGpsPdConnectionStart */
  if ( !z_umts_cp_gsm_session_status.u_rrlp_rcvd )
  {

    if (tm_umts_common_utils_emergency_effect() != C_EMERGENCY_HANDLING_NULL)
    {
      tm_core_send_pdapi_emerg_sess_type(PDSM_PD_EXT_STATUS_EMERG_SESS_GSM_CP);
    }
    tm_umts_common_send_pdapi_comm_event
    (  
      PDSM_PD_COMM_PROTOCOL_UMTS_CP_GSM,
      C_COMM_BEGIN,
      z_umts_cp_gsm_session_status.tm_active_session_handle ,
      z_umts_cp_gsm_session_status.q_pdapi_client_id
    );
    z_umts_cp_gsm_session_status.u_rrlp_rcvd = TRUE;
  }
  
  /* if in wait-for-RRLP state, cancel it */
  if ( z_umts_cp_gsm_session_status.e_cf_state == 
       C_UMTS_CP_GSM_CF_STATE_WAIT_RRLP )
  {
    tm_umts_cp_gsm_cancel_cf_timer();
  }     


  /*
   *  Load up local variables to make the code easier to read
   */
  p_rrlp_params = &p_net_assist->radio_link_prot_params.z_rrlp_params;
  b_RefNum = p_rrlp_params->b_RefNum;

  /*
   *  Output a helpful diagnostic message 
   */
  MSG_MED("CP-GSM: RefNums 1=%d, 2=%d",
                b_RefNum, p_rrlp->referenceNumber, 0 );

  /* if the incoming reference number is the same as that of an existing
     session, ignore the incoming message */
  if ( ( b_RefNum != -1 ) && ( b_RefNum == (int8)p_rrlp->referenceNumber ) )
  {
    *p_ack_required = FALSE;
    success = FALSE; /* asn1_decode_status points to no decoding error, 
                        so no action will be taken; message simply ignored */
                        
    tm_osys_asn1_free_pdu (p_rrlp , PROTO_RRLP);
    return ( success );
  }

  p_rrlp_params->b_RefNum = (int8)p_rrlp->referenceNumber;

  /*
   *  IF no GSM session has been started yet THEN
   *    This must be an MT/NI callflow
   *    IF the first message sent in an MT/NI callflow is an MPR THEN
   *      ASSUME this is an MS-Assisted callflow
   *    ELSE
   *      ASSUME this is an MS-Based callflow
   *    ENDIF
   *  ENDIF
   */
  if( !z_umts_cp_gsm_session_status.u_tm_core_session_started )
  {
    if( p_rrlp->component.t == rrlp_msrPositionReq_chosen )
    {
      e_agps_mode = C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED;
    }
    else
    {
      e_agps_mode = C_UMTS_CP_GSM_AGPS_MODE_UE_BASED;
    }
    
    /* possible service interation arbitration */
    if ( !tm_umts_common_srv_interact(C_UMTS_SESSION_GSM_CP_NI) )  
    {
      MSG_HIGH("Unable to Start %d session, service interaction failed", e_agps_mode, 0, 0);
      tm_osys_asn1_free_pdu (p_rrlp , PROTO_RRLP);
      return TRUE; 
    }
    if( !tm_umts_cp_gsm_start_sess_req( e_agps_mode ) )
    {
      MSG_HIGH("Unable to Start %d session", e_agps_mode, 0, 0);
      tm_osys_asn1_free_pdu (p_rrlp , PROTO_RRLP);
      return TRUE; 
    }
  }

  /*
   *  Parse data now
   */
  switch ( p_rrlp->component.t )
  {
    case rrlp_msrPositionReq_chosen:
      p_rrlp_params->u_rrlp_choice = C_RRLP_CHOICE_MSRPOSREQ;

      *p_ack_required = FALSE;

      if( !tm_umts_cp_gsm_rrlp_msr_pos_req ( 
                                p_rrlp->component.u.msrPositionReq ) )
      {
        MSG_ERROR("CP-GSM: err RRLP-msrPosReq", 0, 0, 0);
        success = FALSE;
      }
      break;

    case rrlp_msrPositionRsp_chosen:
      MSG_ERROR("CP-GSM: Unexpected RRLP-msrPosRsp received",0,0,0);
      *p_ack_required = FALSE;
      success = FALSE;
      break;

    case rrlp_assistanceData_chosen:
    {
      rrlp_AssistanceData *p_Assist = p_rrlp->component.u.assistanceData;

      p_rrlp_params->u_rrlp_choice = C_RRLP_CHOICE_ASSISTDATA;

      if( p_Assist->m.gps_AssistDataPresent )
      {
        parse_ok = tm_umts_cp_gsm_process_assistance_data(
             &p_rrlp->component.u.assistanceData->gps_AssistData.controlHeader );

        if( !parse_ok )
        {
          MSG_ERROR("CP-GSM: Unable to parse AssistanceData",0,0,0);
          success = FALSE;
        }
      }
      
      if ( os_TimerCheck(z_umts_cp_gsm_data.p_max_mgp_on_timer) == 0 ) 
      {
        /* max_mgp_on timer not currently running, start the timer now */  
        MSG_HIGH("Start max_mgp_on timer", 0, 0, 0);
        tm_umts_cp_gsm_start_max_mgp_on_timer(TM_UMTS_CP_GSM_MAX_MGP_ON_IN_MS );
      }
      
      /*
       *  Before entering this next IF, make sure 
       *  p_tm_network_assist->u_MoreToCome is set to FALSE because 
       *  although SnapTrack SMLC always provide this optional sequence of
       *  "more-assistance-to-come" to indicate whether or not more is
       *  on the way, non-SnapTrack yet compliant SMLCs may not have that
       *  sequence, in which case it means no-more-to-come.
       */
      p_net_assist->u_MoreToCome = FALSE;

      if ( p_Assist->m.moreAssDataToBeSentPresent )
      {
        if ( p_Assist->moreAssDataToBeSent == rrlp_moreMessagesOnTheWay )
        {
          p_net_assist->u_MoreToCome = TRUE;
        }
        else
        {
          if ( z_umts_cp_gsm_session_status.e_mo_lr_type == ASSISTANCE_DATA )
          {
            tm_umts_common_send_pdapi_comm_event(
              PDSM_PD_COMM_PROTOCOL_UMTS_CP_GSM,
              C_COMM_DONE,
              z_umts_cp_gsm_session_status.tm_active_session_handle ,
              z_umts_cp_gsm_session_status.q_pdapi_client_id
            );
          }
        
        }
      }

      if ( p_Assist->m.rel5_AssistanceData_ExtensionPresent )
      {
        p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_present = TRUE;

        p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_ref.smlc_code =
          p_Assist->rel5_AssistanceData_Extension.extended_reference.smlc_code;

        p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_ref.transaction_ID =
          p_Assist->rel5_AssistanceData_Extension.extended_reference.transaction_ID;
      }
      break;
    }

    case rrlp_assistanceDataAck_chosen:
    {
      MSG_ERROR("CP-GSM: Unexpected RRLP-msrPosRsp received",0,0,0);
      *p_ack_required = FALSE;
      success = FALSE;
      break;
    }

    case rrlp_protocolError_chosen:
    {
      p_rrlp_params->u_rrlp_choice = C_RRLP_CHOICE_PROTCOL_ERR;
      break;
    }

    default:
    {
      /* Unknown type */
      MSG_MED("CP-GSM: Unknown RRLP-choice received",0,0,0);
      *p_ack_required = FALSE;
      success = FALSE;
      break;
    }
  }

  tm_osys_asn1_free_pdu (p_rrlp , PROTO_RRLP);
  return ( success );
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_send_pos_rrlp

DESCRIPTION
  This function encodes messages in RRLP format and sends them to the SMLC
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_send_pos_rrlp
(
  gad_ellip_alt_unc_ellip_type  *p_gad_shape_pos,
  rrlp_FixType                  fix_type,
  uint32                        q_timestamp_ms 
)
{
  rrlp_PDU                  z_rrlp;
  rrlp_MsrPosition_Rsp      z_rrlp_MsrPosition_Rsp;
  
  rrlp_MsrPosition_Rsp      *p_mpr;
  rrlp_LocationInfo         *p_locinfo;
  tm_pdu_buf_type           pdu_buff;
  tm_network_assist_s_type  *p_net_assist;
  rrlp_params_s_type        *p_rrlp_params;
  
  int encode_status;
  uint32 q_block_size;

  /* initialize buffer for structure buffers used by OSYS ASN1 encoder */
  memset( &z_rrlp, 0, sizeof( z_rrlp ));
  memset( &z_rrlp_MsrPosition_Rsp, 0, sizeof(rrlp_MsrPosition_Rsp));
  
  
  /*
   *  Initialize various local variables to make the code prettier
   */
  z_rrlp.component.u.msrPositionRsp = &z_rrlp_MsrPosition_Rsp;
  p_mpr  = z_rrlp.component.u.msrPositionRsp;
  p_locinfo       = &p_mpr->locationInfo;
  p_net_assist    = &z_umts_cp_gsm_session_status.z_network_assist;
  p_rrlp_params   = &p_net_assist->radio_link_prot_params.z_rrlp_params;
  
  /* it is garanteed by the definition of the structures that no capping 
     is needed, but just in case */
  q_block_size = memscpy(&p_locinfo->posEstimate.data[0], sizeof (p_locinfo->posEstimate.data),
                         p_gad_shape_pos, sizeof(gad_ellip_alt_unc_ellip_type));
  p_locinfo->posEstimate.numocts = q_block_size;
  
  p_locinfo->refFrame             = 65535;
  p_locinfo->m.gpsTOWPresent      = TRUE;
  p_locinfo->gpsTOW               = q_timestamp_ms % C_FOUR_HRS_MSEC;
  p_locinfo->fixType              = fix_type;

  if( fix_type != rrlp_threeDFix )
    MSG_ERROR("CP-GSM: Invalid Fix Type in send_pos_rrlp", 0, 0, 0 );

  /* ref. num: the same as the one came from SMLC */
  z_rrlp.referenceNumber = TO_OSS_REF_NUM(p_rrlp_params->b_RefNum);
  
  z_rrlp.component.t = rrlp_msrPositionRsp_chosen;

  p_mpr->m.locationInfoPresent = TRUE;

  /*
   *  IF REL-5 Extension has been included in the message sent from the network,
   *    then include it in this uplink message too
   */
  if ( p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_present )
  {
    p_mpr->m.rel_5_MsrPosition_Rsp_ExtensionPresent = TRUE;

    p_mpr->rel_5_MsrPosition_Rsp_Extension.m.extended_referencePresent = TRUE; 

    p_mpr->rel_5_MsrPosition_Rsp_Extension.extended_reference.smlc_code = 
              p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_ref.smlc_code;

    p_mpr->rel_5_MsrPosition_Rsp_Extension.extended_reference.transaction_ID = 
              p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_ref.transaction_ID;
  }

  /* Do the encode */
  encode_status = tm_osys_asn1_encode_pdu((void *)&z_rrlp, &pdu_buff, PROTO_RRLP);
  
  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
  
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. Freeing: %d,%lx,%u", 
            encode_status, pdu_buff.value, pdu_buff.length);
               
      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. No alloc.: %d,%lx,%u", 
            encode_status, pdu_buff.value, pdu_buff.length);
    }            
  
    return;
  }

  /*
   *  Send the PDU to the RRC, then free the PDU buffer
   */
  (void) tm_umts_cp_gsm_log_and_tx_rrlp(  pdu_buff.value, 
                                          (uint16) pdu_buff.length );

  tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_tx_rrlp_mpr_err

DESCRIPTION
  This function encodes an RRLP message carrying RRLP-measurePosResp-error
  and sends it to the SMLC
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

/* the limit of 40 octets is defined in tm_rrlp_asn1.h */
#define MAX_WISHLIST_OCTETS   40

boolean tm_umts_cp_gsm_tx_rrlp_mpr_err
( 
  rrlp_LocErrorReason e_error_reason 
)
{
  rrlp_PDU                  z_rrlp;
  rrlp_MsrPosition_Rsp      z_rrlp_MsrPosition_Rsp;
  
  
  rrlp_MsrPosition_Rsp      *p_mpr;
  tm_pdu_buf_type           pdu_buff;
  tm_network_assist_s_type  *p_net_assist;
  rrlp_params_s_type        *p_rrlp_params;
  
  /* pointer to a sturcture for data as defined in 3GPP 09.31 GPS Assistance Data */
  GpsAssisData_Gsm0931      *p_GpsAssistData;
  
  /* num. of remaining octets of GpsAssisData_Gsm0931: u_Octet2_LengthIndicator - 6   */
  uint8                     u_num_remaining_octets;   
  
  int encode_status;
  

  MSG_LOW("CP-GSM: tx_rrlp_mpr_err called", 0, 0, 0);
  
 
  memset(&z_rrlp, 0, sizeof(rrlp_PDU) );
  memset(&z_rrlp_MsrPosition_Rsp, 0, sizeof(rrlp_MsrPosition_Rsp));
  
  z_rrlp.component.u.msrPositionRsp = &z_rrlp_MsrPosition_Rsp;
  p_mpr = &z_rrlp_MsrPosition_Rsp;

  p_net_assist    = &z_umts_cp_gsm_session_status.z_network_assist;
  p_rrlp_params   = &p_net_assist->radio_link_prot_params.z_rrlp_params;
  p_GpsAssistData = &z_umts_cp_gsm_session_status.z_GpsAssistData;
  

  /*
   *  ref. num: the same as the one came from SMLC
   */
  z_rrlp.referenceNumber  = TO_OSS_REF_NUM(p_rrlp_params->b_RefNum);
  z_rrlp.component.t = rrlp_msrPositionRsp_chosen;

  p_mpr->m.locationErrorPresent       = TRUE;
  p_mpr->locationError.m.additionalAssistanceDataPresent = 0;
  p_mpr->locationError.locErrorReason = e_error_reason;
  

  /*
   *  Handle Assistance Data Requests as a special case
   */
  if( (e_error_reason == rrlp_gpsLocCalAssDataMissing) ||
      (e_error_reason == rrlp_gpsAssDataMissing) )
  {
    MSG_LOW("CP-GSM: requesting assistance data", 0, 0, 0);
  
    p_mpr->locationError.m.additionalAssistanceDataPresent = TRUE;

    if( p_GpsAssistData->u_Octet2_LengthIndicator > MAX_WISHLIST_OCTETS )
      p_GpsAssistData->u_Octet2_LengthIndicator = MAX_WISHLIST_OCTETS;

    p_mpr->locationError.additionalAssistanceData.m.gpsAssistanceDataPresent = TRUE; 

    /* limit of 40 octets */
    p_mpr->locationError.additionalAssistanceData.gpsAssistanceData.numocts = 
       p_GpsAssistData->u_Octet2_LengthIndicator;
       
  
    p_mpr->locationError.additionalAssistanceData.gpsAssistanceData.data[0] =   
      p_GpsAssistData->u_Octet3_Whishlist_AH;
      
    p_mpr->locationError.additionalAssistanceData.gpsAssistanceData.data[1] =   
      p_GpsAssistData->u_Octet4_Whishlist_IP;
      
    p_mpr->locationError.additionalAssistanceData.gpsAssistanceData.data[2] =   
      p_GpsAssistData->u_Octet5_GpsWeek_MSB2;
      
    p_mpr->locationError.additionalAssistanceData.gpsAssistanceData.data[3] =   
      p_GpsAssistData->u_Octet6_GpsWeek_LSB8;
      
    p_mpr->locationError.additionalAssistanceData.gpsAssistanceData.data[4] =   
      p_GpsAssistData->u_Octet7_GpsToe;
      
    p_mpr->locationError.additionalAssistanceData.gpsAssistanceData.data[5] =   
      p_GpsAssistData->u_Octet8_NSAT_ToeLimit;
      
    if ( p_GpsAssistData->u_Octet2_LengthIndicator > 6 ) 
    {
      /* borrow this variable to first get number of octects remaining */
      u_num_remaining_octets =  ( p_GpsAssistData->u_Octet2_LengthIndicator - 6 );
      
      if ( (u_num_remaining_octets & 0x1 ) == 0 ) /* number of octects must be even number */
      {
        if ( ( u_num_remaining_octets / 2 ) < N_SV )
        {
          memscpy (&p_mpr->locationError.additionalAssistanceData.gpsAssistanceData.data[6],
		  	      u_num_remaining_octets,
                  &p_GpsAssistData->w_SvId_Iode[0],
                  sizeof(p_GpsAssistData->w_SvId_Iode));
        }
      }
    }    
  }

  /*
   *  IF REL-5 Extension has been included in the message sent from the network
   *    then include it in this uplink message too.
   */
  if( p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_present )
  {
    MSG_LOW("CP-GSM: adding REL-5 Extension", 0, 0, 0);
  
    p_mpr->m.rel_5_MsrPosition_Rsp_ExtensionPresent = TRUE;

    p_mpr->rel_5_MsrPosition_Rsp_Extension.m.extended_referencePresent = TRUE;

    p_mpr->rel_5_MsrPosition_Rsp_Extension.extended_reference.smlc_code =
      p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_ref.smlc_code;

    p_mpr->rel_5_MsrPosition_Rsp_Extension.extended_reference.transaction_ID =
      p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_ref.transaction_ID;
  }
  
  p_rrlp_params->b_RefNum = -1; /* will be able to accept new RRLP of any 
                                   refNumb now */

  /*
   *  Do the ASN.1 encoding for RRLP
   */
  encode_status = tm_osys_asn1_encode_pdu((void *)&z_rrlp, &pdu_buff, PROTO_RRLP);
  
  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
  
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. Freeing: %d,%lx,%u", 
            encode_status, pdu_buff.value, pdu_buff.length);
               
      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. No alloc.: %d,%lx,%u", 
            encode_status, pdu_buff.value, pdu_buff.length);
    }            
  
    return FALSE;
  }
  
  /*
   *  Transmit the message to the SMLC via RR
   */
  (void) tm_umts_cp_gsm_log_and_tx_rrlp(  pdu_buff.value, 
                                          (uint16) pdu_buff.length );

  /*
   *  Free the PDU
   */
  tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );

  return TRUE;
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_handle_prm_data

DESCRIPTION
  This function constructs gpsMsrInfo in an RRLP compliant format, using
  information in the Gps measurement database which is just updated
  with the new fix event that triggered the entering of this function.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_handle_prm_data 
( 
  sm_GpsMeasRptStructType *p_prm_data 
)
{
  boolean                   ok;
  msa_pos_time_stamp_type   *p_msa_pos_time_stamp;
  uint32                    q_session_remaining_time_msec;
  uint8                     u_list_len;
  const gnss_MeasStructType *p_gps_meas_per_sv;
  uint8                     u_num_non_zero_cno_sv;
  uint8                     u_i;
  boolean                   status_check_needed = TRUE;

  if (NULL == p_prm_data)
  {
	MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL pointer passed to tm_umts_cp_gsm_handle_prm_data");
	return;
  }
  
  if ( z_umts_cp_gsm_session_status.e_agps_mode != C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED )
  {
    return;
  }

  q_session_remaining_time_msec = os_TimerCheck ( z_umts_cp_gsm_data.p_gsm_timer );

  /*
   *  IF not DONE then ignore this event, unless approaching end of session 
   */
  if ( p_prm_data->e_MeasRptReason != GPS_MEAS_RPT_MEAS_DONE )
  {
    if ( q_session_remaining_time_msec > 2500 )
    {
      /* Unless approaching end of the RRLP session, not interested in 
         anything other than DONE, ignore it.
         Value of 2.5 sec. choosen to allow the meas. that arrives 2 sec.
         prior to RRLP timeout to be processed, knowing that LM is given
         RRLP_timeout minus one second to run and as a result at 
         RRLP_timeout minus one second session will be terminated. */
      MSG_MED("CP-GSM: discarding non-DONE MeasRpt", 0, 0, 0);
      return;
    }
    else
    {
      MSG_HIGH("CP-GSM: proc non-DONE MeasRpt or send cached: %u", 
                z_umts_cp_gsm_session_status.z_cached_gps_meas.u_valid, 0, 0);
      
      /* if there is cached meas., it is for sure a DONE meas., use that
         one --- only E911 may have cached meas. */
      if ( z_umts_cp_gsm_session_status.z_cached_gps_meas.u_valid )
      {
        tm_umts_cp_gsm_rrlp_send_cached_gps_meas();
        return;
      }  
      status_check_needed = FALSE;   
    }
  }
  else
  {
    MSG_MED("CP-GSM: processing DONE MeasRpt", 0, 0, 0);
  }
  
  /* Regardless if this one is DONE, if E911's first RRLP session, without
     previous preemption, do not report measurment unless approaching the end
     of a session. Note the 2500ms check actually only applies to DONE
     measurements because non-DONE ones would not get here if remaining
     time is greater than 2500ms */
  if ( z_umts_cp_gsm_data.u_e911_in_progress &&
       !z_umts_cp_gsm_data.u_e911_subsequent_rrlp_sess &&
       ( q_session_remaining_time_msec > 2500 )
     )
  {
    if ( !z_umts_cp_gsm_session_status.u_preemption_flag ) 
    {
      MSG_HIGH("In E911, not rept. meas, wait to TO", 0, 0, 0);
      
      /* cache it for later (at responseTime ) report used by E911 */
      z_umts_cp_gsm_session_status.z_cached_gps_meas.u_valid = TRUE;
      memscpy ( &z_umts_cp_gsm_session_status.z_cached_gps_meas.z_cached_gps_meas_rpt,
               sizeof(z_umts_cp_gsm_session_status.z_cached_gps_meas.z_cached_gps_meas_rpt),
               p_prm_data, sizeof(*p_prm_data));   
      return;
    }
  }

  /*
   *  Nothing more to do if there are no SVs in the list
   */
  if ( p_prm_data->z_MeasBlk.u_NumSvs == 0 )
  {
    MSG_MED("CP-GSM: MeasRpt from TM contains 0 SV", 0, 0, 0);
    return;
  }
  
  /*
   * Nothing more to do if no SV in the meas. has non-zero CN0 and passes
   * status check.
   */
  u_list_len = p_prm_data->z_MeasBlk.u_NumSvs;
  if ( p_prm_data->z_MeasBlk.u_NumSvs > 16 )
  {
    u_list_len = 16;
  }

  /*
   *  Count the number of non-zero CNo's in the list
   */
  p_gps_meas_per_sv = &p_prm_data->z_MeasBlk.z_Gnss[0];
  u_num_non_zero_cno_sv = 0;
  for (u_i=0; u_i < u_list_len; u_i++, p_gps_meas_per_sv++)
  {
    if (  p_gps_meas_per_sv->w_Cno > 0)    
    {
      /*If the status check is needed, check for status bits*/
      if(gps_meas_status_bits_check( p_gps_meas_per_sv->q_MeasStatus, status_check_needed ))
      {
        u_num_non_zero_cno_sv++; /* this variable is used to count SVs that are
                                    having non-zero CN0 and also passing status
                                    test */
      }
    }
  }
  if ( u_num_non_zero_cno_sv == 0 )
  {
    MSG_MED("CP-GSM: MeasRpt from TM contains 0 qualified SV", 0, 0, 0);
    return;
  }

  /*
   *  Return if this event was received while in the wrong callflow state
   */
  if ( z_umts_cp_gsm_session_status.e_cf_state !=
                  C_UMTS_CP_GSM_CF_STATE_TIMER_RRLP_MEAS_RESP_TIME )
  {
    MSG_MED("CP-GSM: GPS meas. ignored - not expecting", 0, 0, 0);
    return;
  }

  /*
   *  Clear the measurement timer
   */
  tm_umts_cp_gsm_cancel_cf_timer();
  
  /*
   *  Propagate each SV's measurement to the common reference point: 
   *  p_prm_data->z_MeasBlk.z_MeasClock.q_RefFCount
   */
  gps_MeasBlkPropagate( &p_prm_data->z_MeasBlk );

  /*
   *  Attempt to encode into an RRLP packet and queue it for output to the
   *  SMLC.
   */
  ok = tm_umts_cp_gsm_encode_msrReport_meas( p_prm_data, status_check_needed );
  
  /* once a measurement is sent, cancel the cached-meas flag regardless if 
     there is anything cached */
  z_umts_cp_gsm_session_status.z_cached_gps_meas.u_valid = FALSE;

  /*
   *  IF encoding and queueing was successfull THEN
   *    IF this is an MT session THEN
   *      we are essentially done
   *      Tell MM and RR that the LCS session is over
   *      Tell TM-Core that the session is complete
   *    ELSE
   *      Tell RR that the LCS session is over
   *      Set the state to WAIT_RETURN_RESULT while waiting for position
   *    ENDIF
   *  ELSE
   *    Send an error message and stop TM-Core
   *  ENDIF
   */
  if( ok )
  {
    sm_log_event_report( SM_LOG_EVENT_ID_GPS_COMPLETE, 0 );

    if( z_umts_cp_gsm_session_status.e_callflow_kind== C_UMTS_CP_GSM_SESSION_NI)
    {
      #ifdef FEATURE_CGPS_DUAL_WCDMA
      tm_umts_common_utils_mm_session_status_notify( FALSE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id );
	  #else
	  tm_umts_common_utils_mm_session_status_notify( FALSE );
	  #endif
      tm_umts_common_utils_rr_session_status_notify( FALSE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id);
    
      tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_COMPLETED );
    }
    else
    {
      p_msa_pos_time_stamp = &z_umts_cp_gsm_session_status.z_msa_pos_time_stamp;

      if( p_msa_pos_time_stamp->u_ref_time_week_num_valid )
      {
        p_msa_pos_time_stamp->q_pos_time_stamp = (uint32)
                ( tm_umts_common_utils_full_gps_ms(
                        p_msa_pos_time_stamp->w_ref_time_week_num,
                        p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.q_GpsMsec ) / SEC_MSECS );
        p_msa_pos_time_stamp->u_pos_time_stamp_valid = TRUE;
      }

      tm_umts_common_utils_rr_session_status_notify( FALSE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id );
 
      tm_umts_cp_gsm_set_cf_state( C_UMTS_CP_GSM_CF_STATE_WAIT_RETURN_RESULT );

      /* Make sure to stop GPS PRM timer in LM to prevent premature timeout of current session while 
      ** waiting for Return Result message from network.
      */
      if (z_umts_cp_gsm_session_status.e_agps_mode == C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED)
      {
       (void) tm_umts_cp_gsm_stop_prm();
      }
    }
  }
  else
  {
    tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_GENERAL_ERROR );
  }
  
  if ( z_umts_cp_gsm_data.u_e911_in_progress && 
       !z_umts_cp_gsm_data.u_e911_subsequent_rrlp_sess &&
       tm_umts_common_utils_cm_emergency_call_status())
       
  {
    /* If E911 is going on, from this point on, any subsequent RRLP 
       sessions will subject to the E911 NILR multiple session rules. */ 
    z_umts_cp_gsm_data.u_e911_subsequent_rrlp_sess = TRUE;
  }
  
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_rrlp_send_cached_gps_meas

DESCRIPTION
  This function constructs gpsMsrInfo in an RRLP compliant format, using
  information in the cached GPS measurement of the current session.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_rrlp_send_cached_gps_meas()
{
  cached_gps_meas_s_type    *cached_gps_meas_prt;
  msa_pos_time_stamp_type   *p_msa_pos_time_stamp;
  boolean                   ok;
  
  cached_gps_meas_prt = 
   &z_umts_cp_gsm_session_status.z_cached_gps_meas;

  /*
   *  Clear the measurement timer
   */
  tm_umts_cp_gsm_cancel_cf_timer();
  
  /*
   *  Propagate each SV's measurement to the common reference point: 
   *  p_prm_data->z_MeasBlk.z_MeasClock.q_RefFCount
   */
  gps_MeasBlkPropagate( &cached_gps_meas_prt->z_cached_gps_meas_rpt.z_MeasBlk );

  /*
   *  Attempt to encode into an RRLP packet and queue it for output to the
   *  SMLC.
   */
  ok = tm_umts_cp_gsm_encode_msrReport_meas(&cached_gps_meas_prt->z_cached_gps_meas_rpt, TRUE);
  
  /* once a measurement is sent, cancel the cached-meas flag regardless if 
     there is anything cached */
  cached_gps_meas_prt->u_valid = FALSE;

  /*
   *  IF encoding and queueing was successfull THEN
   *    IF this is an MT session THEN
   *      we are essentially done
   *      Tell MM and RR that the LCS session is over
   *      Tell TM-Core that the session is complete
   *    ELSE
   *      Tell RR that the LCS session is over
   *      Set the state to WAIT_RETURN_RESULT while waiting for position
   *    ENDIF
   *  ELSE
   *    Send an error message and stop TM-Core
   *  ENDIF
   */
  if( ok )
  {
    sm_log_event_report( SM_LOG_EVENT_ID_GPS_COMPLETE, 0 );

    if( z_umts_cp_gsm_session_status.e_callflow_kind== C_UMTS_CP_GSM_SESSION_NI)
    {
      #ifdef FEATURE_CGPS_DUAL_WCDMA
      tm_umts_common_utils_mm_session_status_notify( FALSE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id );
	  #else
	  tm_umts_common_utils_mm_session_status_notify( FALSE );
	  #endif
      tm_umts_common_utils_rr_session_status_notify( FALSE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id );
    
      tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_COMPLETED );
    }
    else
    {
      p_msa_pos_time_stamp = &z_umts_cp_gsm_session_status.z_msa_pos_time_stamp;

      if( p_msa_pos_time_stamp->u_ref_time_week_num_valid )
      {
        p_msa_pos_time_stamp->q_pos_time_stamp = (uint32)
         ( tm_umts_common_utils_full_gps_ms(
           p_msa_pos_time_stamp->w_ref_time_week_num,
           cached_gps_meas_prt->z_cached_gps_meas_rpt.z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.q_GpsMsec ) /
           SEC_MSECS );
        p_msa_pos_time_stamp->u_pos_time_stamp_valid = TRUE;
      }

      tm_umts_common_utils_rr_session_status_notify( FALSE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id );
 
      tm_umts_cp_gsm_set_cf_state( C_UMTS_CP_GSM_CF_STATE_WAIT_RETURN_RESULT );
    }
  }
  else
  {
    tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_GENERAL_ERROR );
  }
  
  if ( z_umts_cp_gsm_data.u_e911_in_progress && 
       !z_umts_cp_gsm_data.u_e911_subsequent_rrlp_sess &&
       tm_umts_common_utils_cm_emergency_call_status())
  {
    /* If E911 is going on, from this point on, any subsequent RRLP 
       sessions will subject to the E911 NILR multiple session rules. */ 
    z_umts_cp_gsm_data.u_e911_subsequent_rrlp_sess = TRUE;
  }

}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_encode_msrReport_meas

DESCRIPTION
  This function constructs gpsMsrInfo in an RRLP compliant format, using
  information in the Gps measurement database which is just updated
  with the new fix event that triggered the entering of this function.
  
DEPENDENCIES: none

RETURN VALUE: TRUE if successfully encoded and queued for output, else FALSE

SIDE EFFECTS: none

===========================================================================*/

static boolean tm_umts_cp_gsm_encode_msrReport_meas
( 
  const sm_GpsMeasRptStructType *p_prm_data,
  boolean status_check_needed
)
{
  boolean                       ok;
  uint8                         u_i, u_list_len, u_num_non_zero_cno_sv;
  const gnss_MeasStructType     *p_gps_meas_per_sv;
  
  rrlp_PDU                      z_rrlp;
  rrlp_MsrPosition_Rsp          z_rrlp_MsrPosition_Rsp;
  rrlp_GPS_MsrSetElement        z_rrlp_GPS_MsrSetElement;
  rrlp_GPS_MsrElement           z_MsrElement[16]; /* max. RRLP gps-measurement can have 16 SVs */
  
 
  rrlp_MsrPosition_Rsp          *p_mpr;
  tm_network_assist_s_type      *p_net_assist;
  rrlp_params_s_type            *p_rrlp_params;
  tm_pdu_buf_type               pdu_buff;

  int encode_status;
  
  
  
  ok = FALSE;
  
  /*
   *  Initialize the rrlp structure for OSYS encoding 
   */
  memset( &z_rrlp, 0, sizeof( z_rrlp ));
  memset( &z_rrlp_MsrPosition_Rsp, 0, sizeof(rrlp_MsrPosition_Rsp) );
  memset(&z_rrlp_GPS_MsrSetElement, 0, sizeof(rrlp_GPS_MsrSetElement));
  memset( &z_MsrElement[0], 0, (sizeof(rrlp_GPS_MsrElement)* 16) );
  
  /*
   *  Initialize some local variables to make code more readable
   */
  p_net_assist          = &z_umts_cp_gsm_session_status.z_network_assist;
  p_rrlp_params         = &p_net_assist->radio_link_prot_params.z_rrlp_params;
  
  z_rrlp.component.u.msrPositionRsp = &z_rrlp_MsrPosition_Rsp;
  z_rrlp_MsrPosition_Rsp.gps_MeasureInfo.gpsMsrSetList.n = 1;
  z_rrlp_MsrPosition_Rsp.gps_MeasureInfo.gpsMsrSetList.elem = &z_rrlp_GPS_MsrSetElement;
  z_rrlp_GPS_MsrSetElement.gps_msrList.elem = &z_MsrElement[0]; 
  p_mpr = &z_rrlp_MsrPosition_Rsp;
  

  /*
   *  If this number is ever greater than 16, limit to the first 16
   */
  u_list_len = p_prm_data->z_MeasBlk.u_NumSvs;
  if ( p_prm_data->z_MeasBlk.u_NumSvs > 16 )
  {
    u_list_len = 16;
  }

  /*
   *  Use the same reference number that came from the SMLC
   */
  z_rrlp.referenceNumber  = TO_OSS_REF_NUM(p_rrlp_params->b_RefNum);
  z_rrlp.component.t = rrlp_msrPositionRsp_chosen;

  /*
   *  Count the number of non-zero CNo's in the list
   */
  p_gps_meas_per_sv = &p_prm_data->z_MeasBlk.z_Gnss[0];
  u_num_non_zero_cno_sv = 0;
  for (u_i=0; u_i < u_list_len; u_i++, p_gps_meas_per_sv++)
  {
    if (  p_gps_meas_per_sv->w_Cno > 0)            
    {
      /*If the status check is needed, check for status bits*/
      if(gps_meas_status_bits_check( p_gps_meas_per_sv->q_MeasStatus, status_check_needed ) )
      {
        u_num_non_zero_cno_sv++; /* this variable is used to count SVs that are
                                    having non-zero CN0 and also passing status
                                    test */
      }
    }
  }

  /*
   *  IF there are no SVs with a non-zero CNo AND acceptable SV status THEN
   *    send an RRLP error message indicating "not enough satellites"
   *    to the network
   *  ELSE
        build the RRLP message for sending, but if in the process no qualified
        SV is found, send an RRLP error message indicating "not enough satellites"
        to the network; otherwise send an RRLP measurement message to the network
   *  ENDIF
   */    
  if( u_num_non_zero_cno_sv == 0 )
  {
    return tm_umts_cp_gsm_tx_rrlp_mpr_err( rrlp_notEnoughSats );
  }
  else
  {
    if ( tm_rrlp_mprsp_gps_meas_build( p_mpr, p_prm_data, 
                                       u_list_len, status_check_needed
                                     ) == 0 
       )
    {
      return tm_umts_cp_gsm_tx_rrlp_mpr_err( rrlp_notEnoughSats );
    }  
  }

  /*
   *  IF REL-5 Extension has been included in the message sent from the network,
   *    then include it in this uplink message too
   */
  if ( p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_present )
  {
    p_mpr->m.rel_5_MsrPosition_Rsp_ExtensionPresent = TRUE;

    p_mpr->rel_5_MsrPosition_Rsp_Extension.m.extended_referencePresent = TRUE; 

    p_mpr->rel_5_MsrPosition_Rsp_Extension.extended_reference.smlc_code = 
              p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_ref.smlc_code;

    p_mpr->rel_5_MsrPosition_Rsp_Extension.extended_reference.transaction_ID = 
              p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_ref.transaction_ID;
  }

  if(!status_check_needed)
  {
    /*Status check was not done, include time uncertainity*/
    p_mpr->m.rel_98_MsrPosition_Rsp_ExtensionPresent = TRUE;
    p_mpr->rel_98_MsrPosition_Rsp_Extension.m.timeAssistanceMeasurementsPresent = TRUE;
    p_mpr->rel_98_MsrPosition_Rsp_Extension.rel_98_Ext_MeasureInfo.m.otd_MeasureInfo_R98_ExtPresent = 0;
    p_mpr->rel_98_MsrPosition_Rsp_Extension.timeAssistanceMeasurements.m.gpsReferenceTimeUncertaintyPresent = TRUE;
    p_mpr->rel_98_MsrPosition_Rsp_Extension.timeAssistanceMeasurements.gpsReferenceTimeUncertainty = 
      tm_umts_common_utils_microsec_to_GADK(p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.f_ClkTimeUncMs * 1000); 
  }

  /* Do the encode */
  encode_status = tm_osys_asn1_encode_pdu((void *)&z_rrlp, &pdu_buff, PROTO_RRLP);
  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. Freeing: %d,%lx,%u", 
            encode_status, pdu_buff.value, pdu_buff.length);
               
      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. No alloc.: %d,%lx,%u", 
            encode_status, pdu_buff.value, pdu_buff.length);
    }            
               
    return ok;
  }
  
 

  MSG_MED("CP-GSM: transmitting PRM data to SMLC", 0, 0, 0);
  
  /*
   *  Transmit the data to the SMLC via RR
   */
  ok = tm_umts_cp_gsm_log_and_tx_rrlp(  pdu_buff.value, 
                                        (uint16) pdu_buff.length );
  if( !ok )
  {
    MSG_ERROR("CP-GSM: Unable to transmit PRM data to SMLC", 0, 0, 0);
  }
  
  /*
   *  Free the PDU
   */
  tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );

  return ok;
}



/*===========================================================================

FUNCTION tm_umts_cp_gsm_log_and_tx_rrlp

DESCRIPTION
  This function logs an RRLP message (via DM) and sends it to the SMLC
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

/*
 *  C_CONN_GCP - 
 *    This is used as the 2nd argument to sm_log_svr_txrx_rep.
 *    This argument is only used by Snapper to lookup a string to indicate
 *    the "connection type", a concept that is no longer relevant.  
 */
#define C_CONN_GCP    3

static boolean tm_umts_cp_gsm_log_and_tx_rrlp
(
  uint8 *     p_buff,
  uint16      w_length
)
{
  uint16      w_written;

  /*
   *  Log outgoing RRLP messages to DIAG
   */
  sm_log_svr_txrx_rep(  C_LSM_LOG_SVR_TX_TYPE, 
                        C_CONN_GCP,
                        C_LSM_SVR_PROTOCOL_TYPE_RRLP, 
                        w_length, p_buff );

  /*
   *  Send data to RR
   */
  w_written = tm_umts_cp_gsm_write_data( p_buff, w_length );

  if( w_written != w_length )
  {
    MSG_ERROR("CP-GSM: Unable to write data to RR", 0, 0, 0);
    return FALSE;
  }

  return TRUE;
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_parse_rrlp

DESCRIPTION
  This function parses and processes an RRLP message
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_parse_rrlp
(
  uint8   *p_rrlp,
  uint16  w_length 
)
{
  int                                       asn1_decode_status;
  uint8                                     u_rrlp_ref_num;
  boolean                                   decode_ok;
  boolean                                   u_ack_required;
  rrlp_ErrorCodes                           rrlp_protocol_reason;
  tm_network_assist_s_type                  *p_net_assist;

  MSG_MED("CP-GSM: Parsing RRLP", 0, 0, 0);
 
  /*
   *  Initialize local variables to make code easier to read later.
   */
  p_net_assist = &z_umts_cp_gsm_session_status.z_network_assist;
  p_net_assist->u_MoreToCome = FALSE; /* only RRLP-AD could set it to TRUE,
                                         but it is used without considering
                                         if it is AD, so it needs to be cleared
                                         whenever a new RRLP is parsed */ 

  /*
   *  Log the data to DM/Snapper
   */
  sm_log_svr_txrx_rep(  C_LSM_LOG_SVR_RX_TYPE, 
                        C_CONN_GCP,
                        C_LSM_SVR_PROTOCOL_TYPE_RRLP,
                        w_length,
                        p_rrlp );
 
  /*
   *  hand-decode the first byte to get the reference number if possible
   */  
  u_rrlp_ref_num = *p_rrlp;
  u_rrlp_ref_num >>= 5;

  /*
   *  Check for 0 length RRLP messages (just in case)
   */
  if( w_length == 0 )
  {
    MSG_ERROR("CP-GSM: Zero Length RRLP message rcvd", 0, 0, 0);
    tm_umts_cp_gsm_rrlp_encode_protocol_err( rrlp_messageTooShort, -1 );
    return;
  }

  /*
   *  Decode the rest of the RRLP message
   */
  decode_ok = tm_umts_cp_gsm_decode_rrlp(
                            p_net_assist,
                            p_rrlp, 
                            w_length,
                            &asn1_decode_status,
                            &u_ack_required );

  if( decode_ok )
  {
    if( u_ack_required )
    {
      tm_umts_cp_gsm_tx_rrlp_ack();
      
      /*
       *  IF there is more RRLP data to come THEN
       *    stay in the WAIT_RRLP state
       *    restart the WAIT_RRLP timer
       */ 
      if( p_net_assist->u_MoreToCome )
      {
        /* RRLP-AssistData indicating more message on the way */
        MSG_HIGH("Expect more AD from SMLC", p_net_assist->u_MoreToCome, 0, 0); 
        tm_umts_cp_gsm_set_cf_state( C_UMTS_CP_GSM_CF_STATE_WAIT_RRLP );
        return;
      }
    }

    /*
     *  The session could have been terminated already if there was
     *  a cached position fix ready when the MPR came in.  If the
     *  session is already over then don't do anything else.
     *  
     *  Checking the tm_active_session_handle to determine if a session is
     *  active is a little skanky, but it works!  This variable gets sets
     *  when the TM session is started and gets cleared when the TM session
     *  is stopped and the session variables are re-initialized!
     */
    if( z_umts_cp_gsm_session_status.tm_active_session_handle == 
                                                      TM_SESS_HANDLE_NONE )
    {
      return; 
    }

    if ( u_ack_required ) /* for RRLP-AD only. Note MPR details have been taken
                             care of in msr_pos_req() function */
    {                         
         
      /* if MO-AD call flow */
      if ( ( z_umts_cp_gsm_session_status.e_callflow_kind == 
             C_UMTS_CP_GSM_SESSION_UI) &&
           (z_umts_cp_gsm_session_status.e_mo_lr_type == 
             ASSISTANCE_DATA )
         )
      {
        if ( z_umts_cp_gsm_session_status.u_pdapi_fix_reported )
        {
          /* Wait for MOLR_ReturnResult to complete the session */
          tm_umts_cp_gsm_set_cf_state( C_UMTS_CP_GSM_CF_STATE_WAIT_RETURN_RESULT );
        }
        else
        {
          /* inform TmCore no more network message to expect */
          tm_umts_cp_gsm_inform_core_nt_assist_end();
        }  
      } /* if MO-AD */
    } 
  }
  else if( asn1_decode_status == RRLP_ASN1_DECODE_ERR_PDU_DECODED )
  {
    /*
     *  Code reaches here if the PDU was decoded successfully, but
     *  there was some other sort of protocol error.  A good example
     *  of this is the GCF test case for "method not supported" where
     *  an EOTD fix is requested.  In this case, we should not output
     *  an RRLP protocol error at this part of the code. It can also
     *  be the case where same reference numbered RRLP arrived during
     *  one RRLP session and the new message is to be ignored.
     */
    MSG_HIGH("CP-GSM: protocol error or ignored message. Reset GSM cached as_id", 0, 0, 0);
	z_umts_cp_gsm_session_status.z_rr_as_id_info.v_rr_event_as_id_received = FALSE;
  }
  else
  {
    
    /*
     *  Send appropriate RRLP protocol error
     */
    if ( asn1_decode_status == RRLP_ASN1_DECODE_ERR_PDU_TOO_SHORT )
    {
      rrlp_protocol_reason = rrlp_ErrorCodes_messageTooShort;
    }
    else if ( asn1_decode_status == RRLP_ASN1_DECODE_ERR_PDU_INCORRECT_DATA )
    {
      rrlp_protocol_reason = rrlp_ErrorCodes_incorrectData;
    }
    else 
    {
      rrlp_protocol_reason = rrlp_ErrorCodes_unDefined; 
    }

    MSG_ERROR("Sending RRLP-protocolError: %d", rrlp_protocol_reason, 0, 0);

    tm_umts_cp_gsm_rrlp_encode_protocol_err(
                                    rrlp_protocol_reason,
                                    (int8) u_rrlp_ref_num );

	MSG( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "CP-GSM: Reset GSM cached as_id");
	z_umts_cp_gsm_session_status.z_rr_as_id_info.v_rr_event_as_id_received = FALSE;
  }
}
 


/*===========================================================================

FUNCTION tm_umts_cp_gsm_rrlp_encode_protocol error

DESCRIPTION
  This function handles RRLP protocol error situations and calls the proper 
  function to form and send RRLP-ProtocolError to the SMLC
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static void tm_umts_cp_gsm_rrlp_encode_protocol_err
( 
  rrlp_ErrorCodes     u_reason,
  int8                b_ref_num 
)
{
  rrlp_PDU                  z_rrlp;
  rrlp_ProtocolError        *p_err;  
  rrlp_ProtocolError        z_protocol_error;
  tm_network_assist_s_type  *p_net_assist;
  rrlp_params_s_type        *p_rrlp_params;
  tm_pdu_buf_type           pdu_buff;
  int encode_status;


  /*
   *  Initialize rrlp structure
   */
  memset( &z_rrlp, 0, sizeof( z_rrlp ));
  memset( &z_protocol_error, 0, sizeof(rrlp_ProtocolError));
  
  /*
   *  Initialize local variables used to make the code
   *  a bit easier to read
   */
  z_rrlp.component.u.protocolError = &z_protocol_error; 
  
  p_err           = &z_protocol_error;  
  p_net_assist    = &z_umts_cp_gsm_session_status.z_network_assist;
  p_rrlp_params   = &p_net_assist->radio_link_prot_params.z_rrlp_params;

  /*
   * ref. num: the same as the one that came from the SMLC
   */
  z_rrlp.referenceNumber = TO_OSS_REF_NUM(b_ref_num);

  z_rrlp.component.t = rrlp_protocolError_chosen;

  z_protocol_error.errorCause = u_reason;
  

  /*
   *  IF REL-5 Extension has been included in the message sent from the network,
   *    then include it in this uplink message too
   */
  if( p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_present )
  {
    p_err->m.rel_5_ProtocolError_ExtensionPresent = TRUE;

    p_err->rel_5_ProtocolError_Extension.m.extended_referencePresent = TRUE; 

    p_err->rel_5_ProtocolError_Extension.extended_reference.smlc_code =
              p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_ref.smlc_code;

    p_err->rel_5_ProtocolError_Extension.extended_reference.transaction_ID = 
              p_rrlp_params->z_rrlp_rel5_ext.rel5_ext_ref.transaction_ID;
  }
  
  p_net_assist->radio_link_prot_params.z_rrlp_params.b_RefNum = -1;

  /*
   *  Do the encode
   */
  encode_status = tm_osys_asn1_encode_pdu((void *)&z_rrlp, &pdu_buff, PROTO_RRLP);
  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. Freeing: %d,%lx,%u", 
            encode_status, pdu_buff.value, pdu_buff.length);
               
      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. No alloc.: %d,%lx,%u", 
            encode_status, pdu_buff.value, pdu_buff.length);
    }            
               
    return;
  }
  else
  {
    /*
     *  Send the PDU
     */
    (void) tm_umts_cp_gsm_log_and_tx_rrlp(  pdu_buff.value, 
                                            (uint16) pdu_buff.length );
  }

  /*
   *  Free the PDU buffer
   */
  tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_tx_rrlp_ack

DESCRIPTION
  This function encodes and transmit an RRLP ACK
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static void tm_umts_cp_gsm_tx_rrlp_ack( void )
{
  tm_pdu_buf_type           pdu_buff;
  rrlp_PDU                  z_rrlp;
  tm_network_assist_s_type  *p_net_assist;
  rrlp_params_s_type        *p_rrlp_params;
  boolean                   u_ok;
  int encode_status;

  /*
   *  Initialize local variables to make the code easier to read
   */
  p_net_assist          = &z_umts_cp_gsm_session_status.z_network_assist,
  p_rrlp_params         = &p_net_assist->radio_link_prot_params.z_rrlp_params;

  /*
   *  Initialize the OSS rrlp structure
   */
  memset( &z_rrlp, 0, sizeof( z_rrlp ));

  /*
   *  Use the same reference number that was received from the SMLC
   */
  z_rrlp.referenceNumber  = TO_OSS_REF_NUM(p_rrlp_params->b_RefNum);
  z_rrlp.component.t = rrlp_assistanceDataAck_chosen;

  /*
   *  Do ASN.1 encoding
   */
  encode_status = tm_osys_asn1_encode_pdu((void *)&z_rrlp, &pdu_buff, PROTO_RRLP);
  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. Freeing: %d,%lx,%u", 
            encode_status, pdu_buff.value, pdu_buff.length);
               
      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. No alloc.: %d,%lx,%u", 
            encode_status, pdu_buff.value, pdu_buff.length);
    }            
               
    return;
  }
   

  /*
   *  Send the PDU to the RRC, then free the PDU buffer
   */
  u_ok = tm_umts_cp_gsm_log_and_tx_rrlp(  pdu_buff.value, 
                                          (uint16) pdu_buff.length );
  if( !u_ok )
  {
    tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_GENERAL_ERROR);
    MSG_ERROR("CP-GSM: TX_RRLP failed", 0, 0, 0);
  }
  p_rrlp_params->b_RefNum = -1; /* so that subsequent AssistanceData with the 
                                   same reference number can be processed */
  tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_post_cached_acq_assist

DESCRIPTION
  This function posts cached acquisition assistance to MGP.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_post_cached_acq_assist( )
{

  if( z_umts_cp_gsm_session_status.z_cached_acq_assist.z_cached_svdir.gnss_sv_dir.u_NumSvs )
  {
    tm_umts_cp_gsm_post_data( 
      TM_POST_DATA_TYPE_SV_DIR,
      &z_umts_cp_gsm_session_status.z_cached_acq_assist.z_cached_svdir );
  }
  /* post steering */
  tm_umts_cp_gsm_post_data( 
    TM_POST_DATA_TYPE_AA,
    &z_umts_cp_gsm_session_status.z_cached_acq_assist.z_cached_steer );

  /* clear the cached assistance data -- ACQ cached variable */  
  z_umts_cp_gsm_session_status.z_cached_acq_assist.u_valid = FALSE;
  MSG_HIGH("Posting cached SvDir %u Steer %u", 
             z_umts_cp_gsm_session_status.z_cached_acq_assist.z_cached_svdir.gnss_sv_dir.u_NumSvs,
             z_umts_cp_gsm_session_status.z_cached_acq_assist.z_cached_steer.gnss_aa_data.u_NumSvs,
             0 );
}

#endif /* FEATURE_CGPS_UMTS_CP_GSM */

