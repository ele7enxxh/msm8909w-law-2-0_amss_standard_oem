/*----------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Atheros, Inc..
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  TM UMTS COMMON RRC Module

GENERAL DESCRIPTION
  This file contains functions processing the decoded RRC messages.

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_common/src/tm_rrc.c#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  02/02/15   gk      velocity is incorrectly encoded.
  09/24/14   rk      Added missing steering source type while injecting AcqAssist.
  06/28/13     rk     Fixed north velocity in the velocity estimate computation.
  04/21/13     rk     Added RRC R8 GANSS Assistance data processing interfaces.
  04/21/13     rk     Modified tm_rrc_refpos_to_TmCore_post_data_payload interface to process
                      reference location from GPS & GANSS assistance data.
  07/26/09     atien  Scale location unc by 0.82 when loc confidence = 0 per DoCoMo
                      spec
  05/30/07     LT     Initial check in.

===========================================================================*/
#include "gps_variation.h"
#include "customer.h"   /* Customer configuration file */

#ifdef FEATURE_CGPS_UMTS_CP_WCDMA /* WCDMA only at this time, may be used for other purposes at later time */

#include "comdef.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "tm_rrc.h"
#include "tm_prtl_iface.h"
#include "sm_api.h" /* included only for the data structures defined there */
#include "tm_umts_common_utils.h"

#include "msg.h"


static const float f_doppler_unc[5] = {12.5, 25.0, 50.0, 100.0, 200.0 };
 /* doppler unc: uint8 from RRC decoder, ENUMERATED {
    hz12-5, hz25, hz50, hz100, hz200, spare3, spare2, spare1 } 
  */

/* This table is generated based on the new GAD shape specification
   for 3GPP RRC R8

   r = C * ( ((1 + x) ^ k) - 1)

  where C = 0.0022 and x = 0.18 and 'k' ranges from 0 to 127.
*/
static const float f_GadKTimeUncMapNewRrc[128] =
{
  0.000000F, /*K= 0*/
  0.000396F, /*K= 1*/
  0.000863F, /*K= 2*/
  0.001415F, /*K= 3*/
  0.002065F, /*K= 4*/
  0.002833F, /*K= 5*/
  0.003739F, /*K= 6*/
  0.004808F, /*K= 7*/
  0.006069F, /*K= 8*/
  0.007558F, /*K= 9*/
  0.009314F, /*K= 10*/
  0.011387F, /*K= 11*/
  0.013833F, /*K= 12*/
  0.016719F, /*K= 13*/
  0.020124F, /*K= 14*/
  0.024142F, /*K= 15*/
  0.028884F, /*K= 16*/
  0.034479F, /*K= 17*/
  0.041081F, /*K= 18*/
  0.048872F, /*K= 19*/
  0.058065F, /*K= 20*/
  0.068912F, /*K= 21*/
  0.081713F, /*K= 22*/
  0.096817F, /*K= 23*/
  0.114640F, /*K= 24*/
  0.135671F, /*K= 25*/
  0.160488F, /*K= 26*/
  0.189772F, /*K= 27*/
  0.224327F, /*K= 28*/
  0.265102F, /*K= 29*/
  0.313216F, /*K= 30*/
  0.369991F, /*K= 31*/
  0.436985F, /*K= 32*/
  0.516039F, /*K= 33*/
  0.609322F, /*K= 34*/
  0.719395F, /*K= 35*/
  0.849283F, /*K= 36*/
  1.002550F, /*K= 37*/
  1.183405F, /*K= 38*/
  1.396814F, /*K= 39*/
  1.648636F, /*K= 40*/
  1.945787F, /*K= 41*/
  2.296425F, /*K= 42*/
  2.710177F, /*K= 43*/
  3.198405F, /*K= 44*/
  3.774514F, /*K= 45*/
  4.454323F, /*K= 46*/
  5.256497F, /*K= 47*/
  6.203063F, /*K= 48*/
  7.320011F, /*K= 49*/
  8.638009F, /*K= 50*/
  10.193248F, /*K= 51*/
  12.028429F, /*K= 52*/
  14.193943F, /*K= 53*/
  16.749250F, /*K= 54*/
  19.764511F, /*K= 55*/
  23.322521F, /*K= 56*/
  27.520973F, /*K= 57*/
  32.475143F, /*K= 58*/
  38.321068F, /*K= 59*/
  45.219261F, /*K= 60*/
  53.359127F, /*K= 61*/
  62.964172F, /*K= 62*/
  74.298126F, /*K= 63*/
  87.672180F, /*K= 64*/
  103.453583F, /*K= 65*/
  122.075630F, /*K= 66*/
  144.049637F, /*K= 67*/
  169.978973F, /*K= 68*/
  200.575607F, /*K= 69*/
  236.679626F, /*K= 70*/
  279.282379F, /*K= 71*/
  329.553619F, /*K= 72*/
  388.873688F, /*K= 73*/
  458.871368F, /*K= 74*/
  541.468628F, /*K= 75*/
  638.933411F, /*K= 76*/
  753.941833F, /*K= 77*/
  889.651855F, /*K= 78*/
  1049.789673F, /*K= 79*/
  1238.752197F, /*K= 80*/
  1461.728149F, /*K= 81*/
  1724.839722F, /*K= 82*/
  2035.311279F, /*K= 83*/
  2401.667969F, /*K= 84*/
  2833.968750F, /*K= 85*/
  3344.083496F, /*K= 86*/
  3946.019287F, /*K= 87*/
  4656.303223F, /*K= 88*/
  5494.438965F, /*K= 89*/
  6483.438477F, /*K= 90*/
  7650.458008F, /*K= 91*/
  9027.541992F, /*K= 92*/
  10652.500000F, /*K= 93*/
  12569.952148F, /*K= 94*/
  14832.543945F, /*K= 95*/
  17502.404297F, /*K= 96*/
  20652.835938F, /*K= 97*/
  24370.349609F, /*K= 98*/
  28757.013672F, /*K= 99*/
  33933.281250F, /*K= 100*/
  40041.277344F, /*K= 101*/
  47248.703125F, /*K= 102*/
  55753.472656F, /*K= 103*/
  65789.109375F, /*K= 104*/
  77631.156250F, /*K= 105*/
  91604.757813F, /*K= 106*/
  108093.632813F, /*K= 107*/
  127550.492188F, /*K= 108*/
  150509.578125F, /*K= 109*/
  177601.312500F, /*K= 110*/
  209569.562500F, /*K= 111*/
  247292.109375F, /*K= 112*/
  291804.718750F, /*K= 113*/
  344329.562500F, /*K= 114*/
  406308.906250F, /*K= 115*/
  479444.562500F, /*K= 116*/
  565744.625000F, /*K= 117*/
  667578.625000F, /*K= 118*/
  787742.812500F, /*K= 119*/
  929536.625000F, /*K= 120*/
  1096853.250000F, /*K= 121*/
  1294287.000000F, /*K= 122*/
  1527258.750000F, /*K= 123*/
  1802165.375000F, /*K= 124*/
  2126555.250000F, /*K= 125*/
  2509335.250000F, /*K= 126*/
  2961015.750000F, /*K= 127*/
};

/* Local function protocols */


/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRC reference time IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing ASN.1 decoded RRC message.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_rrc_reftime_to_TmCore_post_data_payload( rrc_assistance_data_struct_type* p_assist, 
                                                  tm_post_data_payload_type    *p_tm_post_data_payload
                                                )
{

  gps_RefTimeStructType *p_tm_post_data_payload_reftime = &p_tm_post_data_payload->reftime_data;
  uint16 w_GpsWeek; 
  uint32 q_GpsTow;
  uint32 q_I = 0;
  rrc_gps_tow_assist_struct_type *p_rrcTow = NULL;

  /* 10-bit Gps Week */
  w_GpsWeek = p_assist->gps_ref_time.week; 

  /* Gps Time of Week */
  q_GpsTow = (uint32)p_assist->gps_ref_time.time;

  /* Convert the 10-bit GPS week to a 16-bit one to reflect the rollover */
  w_GpsWeek = gnss_GpsWeekExtend( w_GpsWeek );
  tm_umts_common_utils_GpsMsecWeekLimit( (int32 *)&q_GpsTow, &w_GpsWeek );

  p_tm_post_data_payload_reftime->u_GpsWeekIsValid = TRUE;
  p_tm_post_data_payload_reftime->w_GpsWeek = w_GpsWeek;
  p_tm_post_data_payload_reftime->q_GpsMsec = q_GpsTow;

  if ( ( z_umts_nv_item_ram_copy.aagps_default_ref_time_unc_50ms * 50 ) >= 1000 )  
  {
    p_tm_post_data_payload_reftime->f_GpsTimeUnc = (float)z_umts_nv_item_ram_copy.aagps_default_ref_time_unc_50ms * 50.f;
  }
  else
  {
    p_tm_post_data_payload_reftime->f_GpsTimeUnc = 1000.f;
  }

  if( p_assist->gps_ref_time.no_of_gps_tow_assist > 0 )                                                                                                                                                                                         
  {   
    p_rrcTow = p_assist->gps_ref_time.gps_tow_assist;   
 
    memset(&p_tm_post_data_payload_reftime->a_GpsTowAssist,0 ,sizeof(gps_rrlp_GPSTOWAssistElement) * N_SV);   
                                                                                                                                                  
    while (q_I < p_assist->gps_ref_time.no_of_gps_tow_assist)                                                                                                                                                                                                           
    {    
      MSG_LOW("Original satelliteID: %d tlmWord: %d tlmRsvd: %d ", 
              p_rrcTow->satellite_identity+1,
              p_rrcTow->tlm_msg,
              p_rrcTow->tlm_rsvd );

      MSG_LOW("Original satelliteID: %d Alert: %d Spoof: %d ", 
              p_rrcTow->satellite_identity+1,
              p_rrcTow->alert, 
              p_rrcTow->anti_spoof);                                                                                                                                                                                                                                                    
                         
      p_tm_post_data_payload_reftime->a_GpsTowAssist[q_I].satelliteID = (uint32) (p_rrcTow->satellite_identity+1);                                                                                                                    
      p_tm_post_data_payload_reftime->a_GpsTowAssist[q_I].tlmWord = (uint32) p_rrcTow->tlm_msg;                                                                                                                            
      p_tm_post_data_payload_reftime->a_GpsTowAssist[q_I].antiSpoof = (uint32) p_rrcTow->anti_spoof;                                                                                                                        
      p_tm_post_data_payload_reftime->a_GpsTowAssist[q_I].alert = (uint32) p_rrcTow->alert;                                                                                                                                
      p_tm_post_data_payload_reftime->a_GpsTowAssist[q_I].tlmRsvdBits = (uint32) p_rrcTow->tlm_rsvd;                                                                                                                    

      p_rrcTow++;                                                                                                                                                                                                                        
      q_I++;                                                                                                                                                                                                                                                
    } 

    p_tm_post_data_payload_reftime->q_NumTowAssistSv =  q_I;
  }
                                                                                                                                                                                                                                                               

}


/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRC reference position IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing ASN.1 decoded RRC message.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 * 
 *  p_gad_pos_to_save:      pointer to the structure that keeps a copy of the
 *                          GAD shape data for possible later returning of
 *                          reference position to the network.
 *
 * Return value:
 *
 *    FALSE: if the GAD shape is not supported by RRC-v5; TRUE otherwise.
 *    
 *
 ******************************************************************************
*/

boolean tm_rrc_refpos_to_TmCore_post_data_payload( rrc_gps_reference_loc_struct_type const *const p_assist_gps_ref_loc,
                                                    tm_post_data_payload_type       *p_tm_post_data_payload, 
                                               RefPos_GAD *p_RefPosGADSave,
                                               boolean const b_GpsRefTimeIncl,
                                               uint16 const w_GpsRefTimeWeek,
                                               uint32 const q_GpsRefTimeTowMs
                                                  )
{
  gad_ellip_alt_unc_ellip_type *p_gad_ellip_alt_unc_ellip = NULL;
  gps_RefLocStructType *p_tm_post_data_payload_seed_data = NULL;
  uint32 q_long_word = 0;
  float f_scale_up_to_99pct_conf;
  uint32   q_GpsTow;
  uint16   w_GpsWeek; 

  if ( (NULL == p_assist_gps_ref_loc) || (NULL == p_tm_post_data_payload) || (NULL == p_RefPosGADSave) )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRC GpsRefPos to TM post data translate condition not met. %p, %p, %p",
          p_assist_gps_ref_loc, p_tm_post_data_payload, p_RefPosGADSave);
    return FALSE;
  }

  p_tm_post_data_payload_seed_data = &p_tm_post_data_payload->seed_data;
  p_gad_ellip_alt_unc_ellip = &p_RefPosGADSave->gad_ellip_alt_unc_ellip;

    /* this is for possible reporting reference position back to the network */
  p_gad_ellip_alt_unc_ellip->u_ShapeCode = C_POS_SHAPE_ELLIP_ALT_UNC_ELLIP; /* only shape supported by the spec. */
  p_gad_ellip_alt_unc_ellip->u_ShapeCode <<= 4; /* shape code is the high nibble of this octet */ 

  p_tm_post_data_payload_seed_data->u_AltitudeIsValid = TRUE;
  p_tm_post_data_payload_seed_data->u_HeadingIsValid = FALSE;
  p_tm_post_data_payload_seed_data->u_VelocityIsValid = FALSE;


  /********************************* LATITUDE ***************************/

  /* store ref. Loc. for possible reporting reference position back to the network */
  q_long_word = (uint32)((int32)p_assist_gps_ref_loc->latitude & 0x007FFFFF);
  p_gad_ellip_alt_unc_ellip->u_Lat2 = (uint8)q_long_word;
  p_gad_ellip_alt_unc_ellip->u_Lat1 = (uint8)(q_long_word>>8);
  p_gad_ellip_alt_unc_ellip->u_Lat0 = (uint8)(q_long_word>>16);

  if ( p_assist_gps_ref_loc->latitude_sign == NORTH )
  {
    p_tm_post_data_payload_seed_data->d_Latitude = 
                                                   (double)(p_assist_gps_ref_loc->latitude * C_GAD_LAT_TO_RAD);
    p_gad_ellip_alt_unc_ellip->u_Lat0 &= 0x7F;
  }
  else if ( p_assist_gps_ref_loc->latitude_sign == SOUTH )
  {
    p_tm_post_data_payload_seed_data->d_Latitude =
                                                   -(double)(p_assist_gps_ref_loc->latitude * C_GAD_LAT_TO_RAD);
    p_gad_ellip_alt_unc_ellip->u_Lat0 |= 0x80;
  }
  

  /*************** LONGITUDE **************************************/

  p_tm_post_data_payload_seed_data->d_Longitude =
                                                  (double)(p_assist_gps_ref_loc->longitude * C_GAD_LON_TO_RAD);

  /* store ref. Loc. for possible reporting reference position back to the network */
  p_gad_ellip_alt_unc_ellip->u_Long2 = (uint8)p_assist_gps_ref_loc->longitude;
  p_gad_ellip_alt_unc_ellip->u_Long1 = (uint8)(p_assist_gps_ref_loc->longitude >> 8);
  p_gad_ellip_alt_unc_ellip->u_Long0 = (uint8)(p_assist_gps_ref_loc->longitude >> 16);


  /******************* ALTIDUDE **********************************************/

  /* store ref. Loc. for possible reporting reference position back to the network */
  p_gad_ellip_alt_unc_ellip->u_Alt1 = (uint8)p_assist_gps_ref_loc->altitude;
  p_gad_ellip_alt_unc_ellip->u_Alt0 = (uint8)(p_assist_gps_ref_loc->altitude >> 8);

  if ( p_assist_gps_ref_loc->altitude_direction == HEIGHT )
  {
    p_tm_post_data_payload_seed_data->f_Altitude = 
                                                   (float)p_assist_gps_ref_loc->altitude; /* in meters */
    p_gad_ellip_alt_unc_ellip->u_Alt0 &= 0x7F;
  }
  else if ( p_assist_gps_ref_loc->altitude_direction == DEPTH )
  {
    p_tm_post_data_payload_seed_data->f_Altitude = 
                                                   -(float)p_assist_gps_ref_loc->altitude; /* in meters */
    p_gad_ellip_alt_unc_ellip->u_Alt0 |= 0x80;
  }


  /******************* UNCERTAINTY  **************************************/

  p_gad_ellip_alt_unc_ellip->u_UncMajor = p_assist_gps_ref_loc->uncertainity_semi_major & 0x7F; /* possible ret-ref-pos */
  p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMajor = tm_umts_common_utils_GADK_to_meter(TRUE, p_gad_ellip_alt_unc_ellip->u_UncMajor );

  p_gad_ellip_alt_unc_ellip->u_UncMinor = p_assist_gps_ref_loc->uncertainity_semi_minor;; /* possible ret-ref-pos */
  p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMinor = tm_umts_common_utils_GADK_to_meter(TRUE, p_gad_ellip_alt_unc_ellip->u_UncMinor );

  p_gad_ellip_alt_unc_ellip->u_OrientMajor = p_assist_gps_ref_loc->orientation_major_axis; /* possible ret-ref-pos */

  p_gad_ellip_alt_unc_ellip->u_UncAlt = p_assist_gps_ref_loc->uncertainity_altitude; /* possible ret-ref-pos */
  p_tm_post_data_payload_seed_data->f_AltitudeUnc = tm_umts_common_utils_GADK_to_meter(FALSE, p_assist_gps_ref_loc->uncertainity_altitude);

  p_gad_ellip_alt_unc_ellip->u_Confidence = p_assist_gps_ref_loc->confidence; /* possible ret-ref-pos */

  
  /* Scale up the circle size based on the injected % confidence.
     For example, the PDM default % confidence is 39% (standard ellipse),
     and the AI needs a 99% confidence value, so in that case the following
     logic scales the size up radius by ~3 times. */
  /* "confidence" is uint32 In percentage (0..100) ref. rrclsmif.h */

  f_scale_up_to_99pct_conf = 
    tm_util_get_cep_scale_ratio_from_pct
                             ((float)p_assist_gps_ref_loc->confidence * 0.01f,
       0.99f);

  if ( p_assist_gps_ref_loc->confidence == 0 )
  {
  #ifdef FEATURE_CGPS_ZERO_CONF_SCALING
    p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMajor *=  0.82;

    p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMinor *= 0.82; 
  #else
    p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMajor *=  
     f_scale_up_to_99pct_conf;
    p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMinor *=  
     f_scale_up_to_99pct_conf;
  #endif
  }
  else
  {
    p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMajor *=  
   f_scale_up_to_99pct_conf;
    p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMinor *=  
   f_scale_up_to_99pct_conf;
  }

  if ( TRUE == b_GpsRefTimeIncl )
  {
    /* Convert the 10-bit GPS week to a 16-bit one to reflect the rollover */
    w_GpsWeek = gnss_GpsWeekExtend(w_GpsRefTimeWeek);   /* 10-bit Gps Week */
    /* Gps Time of Week */
    q_GpsTow = q_GpsRefTimeTowMs;

    p_RefPosGADSave->w_week = w_GpsWeek;
    p_RefPosGADSave->q_tow_ms = q_GpsRefTimeTowMs;
    p_RefPosGADSave->u_timestamp_valid = TRUE;

    tm_umts_common_utils_GpsMsecWeekLimit( (int32 *)&q_GpsTow, &w_GpsWeek );
    p_tm_post_data_payload_seed_data->w_GpsWeek = w_GpsWeek;
    p_tm_post_data_payload_seed_data->q_GpsMsec = q_GpsTow;

  }
  else
  {
    p_RefPosGADSave->w_week = 0;
    p_RefPosGADSave->q_tow_ms = 0;
    p_RefPosGADSave->u_timestamp_valid = FALSE;

    p_tm_post_data_payload_seed_data->q_GpsMsec = 0;
    p_tm_post_data_payload_seed_data->w_GpsWeek = C_GPS_WEEK_UNKNOWN;
  }

  return TRUE;
}


/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRC navigation model IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_Ephem: pointer to the structure containing the per SV based ASN.1 
 *            decoded RRC navigation model data.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_rrc_eph_to_TmCore_post_data_payload( rrc_ephermeral_clock_struct_type *p_ephermeral_clock_params,
                                              tm_post_data_payload_type      *p_tm_post_data_payload,
                                              uint16 w_week
                                            )

{
  gps_EphStructType *p_tm_post_data_payload_eph_data = &p_tm_post_data_payload->eph_data;
     

  p_tm_post_data_payload_eph_data->u_CodeL2 = (uint8)p_ephermeral_clock_params->code_on_l2; 
  p_tm_post_data_payload_eph_data->u_Accuracy = (uint8)p_ephermeral_clock_params->ura_index;
  p_tm_post_data_payload_eph_data->u_Health = (uint8)p_ephermeral_clock_params->sat_health;
  p_tm_post_data_payload_eph_data->w_Iodc = (uint16)p_ephermeral_clock_params->iodc;
  p_tm_post_data_payload_eph_data->u_Iode = p_tm_post_data_payload_eph_data->w_Iodc;

  p_tm_post_data_payload_eph_data->u_PcodeFlagIsValid = TRUE;
  p_tm_post_data_payload_eph_data->u_PcodeFlag = (uint8)p_ephermeral_clock_params->l2_pflag;
  
  p_tm_post_data_payload_eph_data->u_Sf1BitsAreValid = FALSE;  

  p_tm_post_data_payload_eph_data->u_TgdIsValid = TRUE;
  p_tm_post_data_payload_eph_data->u_Tgd = (uint8)p_ephermeral_clock_params->t_gd;

  p_tm_post_data_payload_eph_data->w_Toc = (uint16)p_ephermeral_clock_params->t_oc;
  p_tm_post_data_payload_eph_data->u_Af2 = (uint8)p_ephermeral_clock_params->af2;
  p_tm_post_data_payload_eph_data->w_Af1 = (uint16)p_ephermeral_clock_params->af1;
  p_tm_post_data_payload_eph_data->q_Af0 = (uint32)p_ephermeral_clock_params->af0;
  p_tm_post_data_payload_eph_data->w_Crs = (uint16)p_ephermeral_clock_params->c_rs;
  p_tm_post_data_payload_eph_data->w_DeltaN = (uint16)p_ephermeral_clock_params->delta_n;
  p_tm_post_data_payload_eph_data->q_M0 = (uint32)p_ephermeral_clock_params->m0;
  p_tm_post_data_payload_eph_data->w_Cuc = (uint16)p_ephermeral_clock_params->c_uc;
  p_tm_post_data_payload_eph_data->q_E = (uint32)p_ephermeral_clock_params->e;
  p_tm_post_data_payload_eph_data->w_Cus = (uint16)p_ephermeral_clock_params->c_us;
  p_tm_post_data_payload_eph_data->q_SqrtA = (uint32)p_ephermeral_clock_params->a_Sqrt;
  p_tm_post_data_payload_eph_data->w_Toe = (uint16)p_ephermeral_clock_params->t_oe;

  p_tm_post_data_payload_eph_data->u_FitIntervalIsValid = TRUE;
  p_tm_post_data_payload_eph_data->u_FitIntervalFlag = p_ephermeral_clock_params->fit_interval;

  p_tm_post_data_payload_eph_data->u_AodoIsValid = TRUE;
  p_tm_post_data_payload_eph_data->u_Aodo = (uint8)p_ephermeral_clock_params->aodo;

  p_tm_post_data_payload_eph_data->w_Cic = (uint16)p_ephermeral_clock_params->c_ic;
  p_tm_post_data_payload_eph_data->q_Omega0 = (uint32)p_ephermeral_clock_params->omega0;
  p_tm_post_data_payload_eph_data->w_Cis = (uint16)p_ephermeral_clock_params->c_is;
  p_tm_post_data_payload_eph_data->q_I0 = (uint32)p_ephermeral_clock_params->i0;
  p_tm_post_data_payload_eph_data->w_Crc = (uint16)p_ephermeral_clock_params->c_rc;
  p_tm_post_data_payload_eph_data->q_Omega = (uint32)p_ephermeral_clock_params->omega;
  p_tm_post_data_payload_eph_data->q_OmegaDot = (uint32)p_ephermeral_clock_params->omegaDot;
  p_tm_post_data_payload_eph_data->w_Idot = (uint16)p_ephermeral_clock_params->iDot;

  if ( w_week == 0) 
  {
    p_tm_post_data_payload_eph_data->u_GpsWeekIsValid = FALSE; /* let MGP handle week number */
    p_tm_post_data_payload_eph_data->w_GpsWeek = 0; 
  }
  else
  {
    p_tm_post_data_payload_eph_data->u_GpsWeekIsValid = TRUE; 
    p_tm_post_data_payload_eph_data->w_GpsWeek = w_week; 
  }
            
}

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRC IONO IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to RRC GPS IONO assistancce data.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_rrc_Iono_to_TmCore_post_data_payload( rrc_gps_iono_struct_type const * const p_gps_iono, 
                                               tm_post_data_payload_type   *p_tm_post_data_payload )
{
  gps_IonoStructType *p_tm_post_data_payload_iono = &p_tm_post_data_payload->iono_data;

  p_tm_post_data_payload_iono->u_Alpha0 = p_gps_iono->alfa0;
  p_tm_post_data_payload_iono->u_Alpha1 = p_gps_iono->alfa1;
  p_tm_post_data_payload_iono->u_Alpha2 = p_gps_iono->alfa2;
  p_tm_post_data_payload_iono->u_Alpha3 = p_gps_iono->alfa3;
  p_tm_post_data_payload_iono->u_Beta0 = p_gps_iono->beta0;
  p_tm_post_data_payload_iono->u_Beta1 = p_gps_iono->beta1;
  p_tm_post_data_payload_iono->u_Beta2 = p_gps_iono->beta2;
  p_tm_post_data_payload_iono->u_Beta3 = p_gps_iono->beta3;
}


/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRC UTC Model IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing ASN.1 decoded RRC UTC model.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_rrc_UtcModel_to_TmCore_post_data_payload( rrc_assistance_data_struct_type *p_assist, 
                                                  tm_post_data_payload_type   *p_tm_post_data_payload )
{
  gps_UtcInfoRptStructType *p_tm_post_data_payload_utc_model = &p_tm_post_data_payload->utc_model_data;

  p_tm_post_data_payload_utc_model->q_A0 = p_assist->gps_utc_model.q_A0;
  p_tm_post_data_payload_utc_model->q_A1 = p_assist->gps_utc_model.q_A1;
  p_tm_post_data_payload_utc_model->u_DeltaTls = p_assist->gps_utc_model.u_DeltaTls;
  p_tm_post_data_payload_utc_model->u_Tot = p_assist->gps_utc_model.u_Tot;
  p_tm_post_data_payload_utc_model->u_WNt = p_assist->gps_utc_model.u_WNt;
  p_tm_post_data_payload_utc_model->u_WNlsf = p_assist->gps_utc_model.u_WNlsf;
  p_tm_post_data_payload_utc_model->u_DN = p_assist->gps_utc_model.u_DN;
  p_tm_post_data_payload_utc_model->u_DeltaTlsf = p_assist->gps_utc_model.u_DeltaTlsf;

}

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRC almanac IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_Almanac: pointer to the structure containing the per SV based ASN.1 
 *             decoded RRC almanac data.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_rrc_alm_to_TmCore_post_data_payload( rrc_almanac_sat_info_struct_type  *p_sat_info_list,
                                              tm_post_data_payload_type        *p_tm_post_data_payload
                                            )

{
    gps_AlmStructType *p_tm_post_data_payload_alm_data = &p_tm_post_data_payload->alm_data;


    p_tm_post_data_payload_alm_data->u_Sv = (uint8)p_sat_info_list->satellite_id + 1; /* 6-bit unsigned */;
    p_tm_post_data_payload_alm_data->w_E = (uint16)p_sat_info_list->e;
    p_tm_post_data_payload_alm_data->u_Toa = (uint8)p_sat_info_list->t_oa;
    p_tm_post_data_payload_alm_data->w_DeltaI = (uint16)p_sat_info_list->deltaI;
    p_tm_post_data_payload_alm_data->w_OmegaDot = (uint16)p_sat_info_list->omega_dot;
    p_tm_post_data_payload_alm_data->u_Health = (uint8)p_sat_info_list->sat_health;
    p_tm_post_data_payload_alm_data->q_SqrtA = (uint32)p_sat_info_list->a_Sqrt;
    p_tm_post_data_payload_alm_data->q_Omega0 = (uint32)p_sat_info_list->omega0;
    p_tm_post_data_payload_alm_data->q_Omega = (uint32)p_sat_info_list->omega;
    p_tm_post_data_payload_alm_data->q_M0 = (uint32)p_sat_info_list->m0;
    p_tm_post_data_payload_alm_data->w_Af0 = (uint16)p_sat_info_list->af0;
    p_tm_post_data_payload_alm_data->w_Af1 = (uint16)p_sat_info_list->af1;

    p_tm_post_data_payload_alm_data->u_DataIdIsValid = TRUE;
    p_tm_post_data_payload_alm_data->u_DataId = p_sat_info_list->data_id;

}



/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRC acquistion assistance's SV direction IE to 
 *  the format ready for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing the per SV based ASN.1 
 *            decoded RRC SV Direction.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_rrc_AcqAssistSvDir_to_TmCore_post_data_payload( rrc_assistance_data_struct_type *p_assist, 
                                                        tm_post_data_payload_type *p_tm_post_data_payload
                                                       )
{

  gnss_SvDirStructType                 *p_tm_post_data_payload_sv_dir = &p_tm_post_data_payload->gnss_sv_dir;
  gnss_AzElInfoStructType              *p_zSvAzElInfo = &p_tm_post_data_payload_sv_dir->z_SvAzElInfo[0];
  rrc_gps_acq_assistance_struct_type   *p_acq_assist = &p_assist->gps_acq_assistance;
  rrc_gps_sat_info_struct_type         *p_gps_sat_info;
  uint8                                u_i;

  memset( p_tm_post_data_payload_sv_dir, 0, sizeof(*p_tm_post_data_payload_sv_dir) );

  p_tm_post_data_payload_sv_dir->u_NumSvs = 0;
  p_tm_post_data_payload_sv_dir->q_GnssType = MGP_GNSS_TYPE_GPS;  

  p_gps_sat_info =  &p_acq_assist->gps_sat_info[0]; 

  u_i=0;
  for ( u_i=0; u_i < p_acq_assist->no_of_gps_sat; u_i++ )
  {

    if (p_gps_sat_info->azimuth_elevation_incl)
  {
      p_zSvAzElInfo->w_Sv = p_gps_sat_info->satellite_id + 1;
      p_zSvAzElInfo->f_Azimuth =  (float)p_gps_sat_info->azimuth_elevation_info.azimuth * C_RRLP_RRC_AZIMUTH_TO_RAD;
      p_zSvAzElInfo->f_Elevation = (float)p_gps_sat_info->azimuth_elevation_info.elevation * C_RRLP_RRC_ELEV_TO_RAD;

      p_zSvAzElInfo->u_ElevUncValid = FALSE; 

      p_tm_post_data_payload_sv_dir->u_NumSvs++;
      p_zSvAzElInfo++;
  }
    p_gps_sat_info++;

  }

}


/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRC acquistion assistance IE's steering data to 
 *  the format ready for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing the per SV based ASN.1 
 *            decoded RRC acq. assistance data.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_rrc_AcqAssistSteering_to_TmCore_post_data_payload( rrc_assistance_data_struct_type *p_assist, 
                                                           tm_post_data_payload_type *p_tm_post_data_payload,
                                                           uint16 w_gps_week
                                                          )
{

  gnss_SvAcqAssistCommonStructType *p_tm_post_data_payload_aa_data = &p_tm_post_data_payload->gnss_aa_data;  
  gnss_SvSteerStructType *p_SvSteerPerSv = &p_tm_post_data_payload_aa_data->z_SvSteerInfo[0];
  rrc_gps_acq_assistance_struct_type  *p_acq_assist = &p_assist->gps_acq_assistance;
  rrc_gps_sat_info_struct_type*        p_gps_sat_info;
  uint8                                u_i;


  memset( p_tm_post_data_payload_aa_data, 0, sizeof(*p_tm_post_data_payload_aa_data) );

  p_tm_post_data_payload_aa_data->u_TimeRefIsGnss = 
                                            (w_gps_week != C_GPS_WEEK_UNKNOWN);
  p_tm_post_data_payload_aa_data->w_Week = w_gps_week;

  p_tm_post_data_payload_aa_data->q_Msec = p_acq_assist->gps_reference_time;
  
  p_tm_post_data_payload_aa_data->u_NumSvs = 0;
  p_tm_post_data_payload_aa_data->u_TimeRefIsGnss = TRUE; /* steering data is referenced to GPS time */
  p_tm_post_data_payload_aa_data->u_RefPnPhaseValid = FALSE;
  p_tm_post_data_payload_aa_data->q_SvSteerSrc      = MGP_GNSS_SV_STEER_SRC_GPS;

  p_gps_sat_info = &p_acq_assist->gps_sat_info[0];

  u_i=0;
  while ( u_i < p_acq_assist->no_of_gps_sat )
  {
    p_SvSteerPerSv->w_Sv = p_gps_sat_info->satellite_id + 1;

    /* Check if SV is within range */
    if (p_SvSteerPerSv->w_Sv > 32)
    {
      MSG_ERROR("TM-RRC: Invalid AA SV: %d", p_SvSteerPerSv->w_Sv, 0, 0);
      p_gps_sat_info++;   /* the structure for data from RRC */
      u_i++;
      continue;
    }

    /* 0th order Doppler: RRC has resolution of 2.5 Hz  */
    p_SvSteerPerSv->f_SvSpeed = p_gps_sat_info->doppler * 2.5 * (FLT)C_L1HzToMs * (-1.0);

    /* initialize these for FALSE */
    p_SvSteerPerSv->u_SvAccelValid = FALSE;
    p_SvSteerPerSv->u_SvSpeedUncValid = FALSE;

    if (p_gps_sat_info->extra_doppler_info_incl)
    {

      p_SvSteerPerSv->u_SvAccelValid = TRUE;

      /* RRC doppler1 has 6 bits ranging (-42...21), mapping to -1 to 0.5 Hz/sec; hence a resolution of
         1.5 / 63 = 0.023809523 Hz/sec; */
      p_SvSteerPerSv->f_SvAccel = (float)p_gps_sat_info->extra_doppler_info.doppler_first_order * 0.023809523 * C_L1HzToMs;       
      
      /* doppler unc: uint8 from RRC decoder, ENUMERATED {
                    hz12-5, hz25, hz50, hz100, hz200,
                    spare3, spare2, spare1 }
      */
      if ( p_gps_sat_info->extra_doppler_info.Doppler_uncertainity <= 4)
      {
        p_SvSteerPerSv->f_SvSpeedUnc = 
          f_doppler_unc[p_gps_sat_info->extra_doppler_info.Doppler_uncertainity] * (float)C_L1HzToMs;
        p_SvSteerPerSv->u_SvSpeedUncValid = TRUE;
      }
    }

    p_SvSteerPerSv->f_SubMsec = (float)(1022 - p_gps_sat_info->code_phase) / 1023.0f;

    p_SvSteerPerSv->u_Modulo80ms = TRUE; 
    
    /* RRC provides GPS Bit Number with 2 bits, so Bit 0-3, max. 80 ms */
    p_SvSteerPerSv->q_Ms = (uint32)p_gps_sat_info->gps_bit_number * 20 + (uint32)p_gps_sat_info->integer_code_phase;
    p_SvSteerPerSv->f_SvTimeUnc = (float)SvTimeUnc[(uint8)p_gps_sat_info->search_window];


    p_tm_post_data_payload_aa_data->u_NumSvs++;
    p_SvSteerPerSv++;   /* the structure for TmCore posting */
    p_gps_sat_info++;   /* the structure for data from RRC */
    u_i++;

  }

}




#ifdef FEATURE_GANSS_SUPPORT

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRC Glonass reference time IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing ASN.1 decoded RRC message.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_rrc_glo_reftime_to_tm_post_data_payload( rrc_ganss_reference_time_struct_type const *const pRrcGanssRefTime,
                                                 tm_post_data_payload_type *p_tm_post_data_payload
                                                 )
{
  glo_RefTimeStructType *p_tm_post_data_payload_glo_reftime = NULL;

  if ( (NULL == p_tm_post_data_payload) || (NULL == pRrcGanssRefTime) )
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRC GloRefTime to TM post data translate condition not met. %p %p",
          pRrcGanssRefTime, p_tm_post_data_payload);
    return;
  }

  /* check reference time GANSS ID. Absence of ganssTimeID indicates Galieo ****/
  if ( (pRrcGanssRefTime->ganss_Time_Id_incl != TRUE)
       || (pRrcGanssRefTime->ganss_Time_Id != RRC_GLONASS_SYSTEM_TIME) )
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unsupported GANSS_TIME info %d, %u",
          pRrcGanssRefTime->ganss_Time_Id_incl,
          pRrcGanssRefTime->ganss_Time_Id);
    return;
  }

  p_tm_post_data_payload_glo_reftime = &p_tm_post_data_payload->reftime_glo_data;

  /* Optional Glonass Day info. */
  if ( TRUE == pRrcGanssRefTime->day_incl )
  {
    /*  The RRC provided number of days is divided into 4 year cycle number
        and calendar day. */
    /* ganss_ref_time.day (0..8191) */
    p_tm_post_data_payload_glo_reftime->u_GloFourYear = (uint8)(pRrcGanssRefTime->day / FOURYEAR_DAYS + 1);

    p_tm_post_data_payload_glo_reftime->w_GloDays = (uint16)(pRrcGanssRefTime->day -
                                                             (p_tm_post_data_payload_glo_reftime->u_GloFourYear - 1) * FOURYEAR_DAYS + 1);
  }
  else
  {
    p_tm_post_data_payload_glo_reftime->u_GloFourYear = C_GLO_FOUR_YEAR_UNKNOWN;
    p_tm_post_data_payload_glo_reftime->w_GloDays = C_GLO_DAY_UNKNOWN;
  }

  /* ganss_ref_time.ganss_tod - GANSS Time of Day in seconds */
  p_tm_post_data_payload_glo_reftime->q_GloMsec = pRrcGanssRefTime->ganss_tod * 1000;

  /* ganssTodUncertainty is not supported by MSM */

  /* Glo Clock uncertainty in msecs*/
  if ( (z_umts_nv_item_ram_copy.aagps_default_ref_time_unc_50ms * 50) >= 1000 )
  {
    p_tm_post_data_payload_glo_reftime->f_GloTimeUncMs =
                                                         (float)z_umts_nv_item_ram_copy.aagps_default_ref_time_unc_50ms * 50.f;
  }
  else
  {
    p_tm_post_data_payload_glo_reftime->f_GloTimeUncMs = 1000.f;
  }

  return;
}

/*
 ******************************************************************************
 *  tm_rrc_glo_utcmodel_to_tm_post_data_payload
 *
 * Function description:
 *    Translate RRC additional UTCModel assistance data to TM post structure.
 *
 * Parameters:
 *  p_ganss_utc_model: pointer to the structure containing the RRC AddUTCModel AD.
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data structure.
 *
 * Return value:
 *    True on success or else failure.
 *
 ******************************************************************************
*/

boolean tm_rrc_glo_utcmodel_to_tm_post_data_payload( rrc_GANSS_Add_UTC_Models_type const *const p_ganss_utc_model,
                                                     tm_post_data_payload_type *p_tm_post_data_payload )
{
  if ( p_ganss_utc_model == NULL )
  {
    return ( FALSE );
  }

  /* ONLY model 2 is supported by GLONASS */
  if ( UTC_MODEL2 == p_ganss_utc_model->utc_model_type )
  {
    /*Init*/
    memset(&p_tm_post_data_payload->utc_model_glo_data, 0, sizeof(p_tm_post_data_payload->utc_model_glo_data));

    p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_N4Flag = FALSE;
    p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_TaoGpsFlag = FALSE;
    p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_FreqNumFlag = FALSE;
    p_tm_post_data_payload->utc_model_glo_data.u_Src = C_GLOUTC_SRC_ASSIST;

    /* Calendar day number within four-year period beginning since the leap year - (1..1461) scale factor 1 day ?*/
    p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_NtFlag = TRUE;
    p_tm_post_data_payload->utc_model_glo_data.w_Nt = p_ganss_utc_model->u.utc_model_2.nA;

    if ( TRUE == p_ganss_utc_model->u.utc_model_2.kp_incl )
    {
      /* Notification of expected leap second correction. Present only GLONASS M satellites are present in GLONASS
      *  constellation.
      *  KP : 2 bit string (LSB)
      *       00 -> No UTC correction at the end of current quarter
              01 -> UTC correction by plus (+1 s) in the end of current quarter.
              11 -> UTC correction by minus (-1 s) in the end of current quarter.
      */
      p_tm_post_data_payload->utc_model_glo_data.u_Kp = p_ganss_utc_model->u.utc_model_2.kp;
      p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_KpFlag = TRUE;
    }

    /* B1 and B2 are OPTIONAL. TM API & PE has no B1 or B2 */

    /* p_tm_post_data_payload->utc_model_glo_data.q_TaoGps = ? */

    /* tauC is time diff between UTC and GLONASS. Do not need update. Assume UTC to GPS offset is injected to MGP. */

    return ( TRUE );
  }

  return ( FALSE );
}


/*
 ******************************************************************************
 *
 * tm_rrc_map_glo_eph_svid_to_freq_num_from_aux
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

void tm_rrc_map_glo_eph_svid_to_freq_num_from_aux( rrc_Auxiliary_ganssID3_type const p_GanssSvAuxiInfo[MAX_GANSS_SAT],
                                                   uint8 const u_RrcAuxiInfoCnt,
                                                   prot_glo_eph_buffer_s_type p_GlonasEphFreqBuf[N_GLO_SV] )
{
  uint8 u_index = 0;

  if ( (p_GanssSvAuxiInfo == NULL) || (p_GlonasEphFreqBuf == NULL) )
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRC GANSS Glo AuxiInfo svid & freq/channel number mapping failed. Not enough data %p %p",
          p_GanssSvAuxiInfo, p_GlonasEphFreqBuf);
    return;
  }

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "RRC GANSS Glonass AuxiInfo list = %3u", u_RrcAuxiInfoCnt);

  for ( u_index = 0; u_index < u_RrcAuxiInfoCnt; ++u_index )
  {
    /* Valid SatId (0..23)*/
    /* Glonass carrier frequency number of the SatId (-7..13) */
    /* Supported Glonass signal - MSB (leftmost) Bit 1 - G1 */
    if ( (p_GanssSvAuxiInfo[u_index].sv_id < N_GLO_SV) &&
         ((p_GanssSvAuxiInfo[u_index].channel_Number >= C_PROT_GLO_FREQ_MIN) &&
          (p_GanssSvAuxiInfo[u_index].channel_Number <= C_PROT_GLO_FREQ_MAX)) &&
         (p_GanssSvAuxiInfo[u_index].signals_Available & RRC_GANSS_GLONASS_SIGNAL_ID_G1_BMASK)
         )
    {
      p_GlonasEphFreqBuf[p_GanssSvAuxiInfo[u_index].sv_id].u_freq_num_valid = TRUE;
      p_GlonasEphFreqBuf[p_GanssSvAuxiInfo[u_index].sv_id].z_eph_glo_data.u_FreqNum = p_GanssSvAuxiInfo[u_index].channel_Number & 0x1F; /* the MGP API expects 5 bits */

      MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Mapped RRC GANSS Glonass SatId=%3u and channel#=%d",
            p_GanssSvAuxiInfo[u_index].sv_id, p_GanssSvAuxiInfo[u_index].channel_Number);
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRC GANSS Glonass SatId=%3u, channel#=%d, signal id=0x%8dX out of range",
            p_GanssSvAuxiInfo[u_index].sv_id, p_GanssSvAuxiInfo[u_index].channel_Number,
            p_GanssSvAuxiInfo[u_index].signals_Available);
    }

  }
} /* tm_rrc_map_glo_eph_svid_to_freq_num_from_aux */

/*
 ******************************************************************************
 *
 * tm_rrc_map_glo_acqast_svid_to_freq_num_from_aux
 *
 * Function description:
 *  Associates sv ID with a frequencey number for Glonass Aquisition Assistance data using
 *  information from auxilary assistance.
 *
 * Parameters:
 *   p_gnss_aux_info : pointer to the structure holding Glonass aux. data.
 *   p_GloAcqAsstFreqBuf : pointer to LPP Glonass aquisition assistance & auxi info struct.
 *   p_GloAuxiInfoBuf : Store Glo auxilliary info AD if Glonass AcqAsst data isn't available.
 *
 * Return value:
 *   None.
 *
 ******************************************************************************
*/
void tm_rrc_map_glo_acqast_svid_to_freq_num_from_aux( rrc_Auxiliary_ganssID3_type const p_RrcGloAuxiInfo[MAX_GANSS_SAT],
                                                      uint8 u_RrcAuxiInfoCnt,
                                                      prot_glo_acqast_buffer_s_type *p_GloAcqAsstBuf,
                                                      prot_glo_svcn_buffer_s_type *p_GloSvCnMapBuf )
{
  uint8 u_index = 0;

  if ( (p_RrcGloAuxiInfo == NULL) || (p_GloAcqAsstBuf == NULL) )
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRC Glo svid to freq/channel number mapping failed due to not enough data: %p %p",
          p_RrcGloAuxiInfo, p_GloAcqAsstBuf);
    return;
  }

  /* Check if auxi info sat_cnt isn't greater than the array size */
  if ( u_RrcAuxiInfoCnt > MAX_GANSS_SAT )
  {
    u_RrcAuxiInfoCnt = MAX_GANSS_SAT;
  }

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "RRC Glonass AuxiInfo list = %3u", u_RrcAuxiInfoCnt);


  /* Store the Sv/Channel number mapping from AuxiInfo */
  /* Set num of SVs */
  p_GloSvCnMapBuf->u_n = u_RrcAuxiInfoCnt;

  for ( u_index = 0; u_index < u_RrcAuxiInfoCnt; ++u_index )
  {
    /* Valid SatId (0..23)*/
    /* Glonass carrier frequency number of the SatId (-7..13) */
    /* Supported Glonass signal - MSB (leftmost) Bit 1 - G1 */
    if ( (p_RrcGloAuxiInfo[u_index].sv_id < N_GLO_SV) &&
         ((p_RrcGloAuxiInfo[u_index].channel_Number >= C_PROT_GLO_FREQ_MIN) &&
          (p_RrcGloAuxiInfo[u_index].channel_Number <= C_PROT_GLO_FREQ_MAX)) &&
         (p_RrcGloAuxiInfo[u_index].signals_Available & RRC_GANSS_GLONASS_SIGNAL_ID_G1_BMASK)
         )
    {
      /* Source : -7..13 Glonass carrier frequency number.
      *  Local : -7..13
         Per GLONASS ICD page 10, after year 2005, GLO SV would use frequency ID -7 to +6. Freq channel +7 to +13 would not be used. */
      p_GloSvCnMapBuf->z_glo_sat_elem[p_RrcGloAuxiInfo[u_index].sv_id].channel_num = p_RrcGloAuxiInfo[u_index].channel_Number & 0xFF;
      p_GloSvCnMapBuf->z_glo_sat_elem[p_RrcGloAuxiInfo[u_index].sv_id].m.channelNumValid = TRUE;

      MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "RRC : Store Glonass SV_id=%u to channel#=%d mapping",
            p_RrcGloAuxiInfo[u_index].sv_id, p_GloSvCnMapBuf->z_glo_sat_elem[p_RrcGloAuxiInfo[u_index].sv_id].channel_num);
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "RRC Glo AuxiInfo params not valid : %u %u %d",
            p_RrcGloAuxiInfo[u_index].sv_id, p_RrcGloAuxiInfo[u_index].signals_Available,
            p_RrcGloAuxiInfo[u_index].channel_Number);
    }
  }

  /* Map Glonass Acquisition Assistance SvId to frequency number if AA is already available */
  tm_util_map_glo_acqast_svid_to_freq_num(p_GloAcqAsstBuf, p_GloSvCnMapBuf);
}

/*
 ******************************************************************************
 *
 * tm_rrc_glo_eph_to_common_prot_struct
 *
 * Function description:
 *  Buffers Glonass navigation model in a buffer that associates its SV ID with
 *  the corresponding frequency number
 *
 * Parameters:
 *  p_NavSatelliteInfo : pointer to the structure holding Glonass nav. model.
 *  p_TmPostDataGloEph : pointer to TmCore post structure.
 *
 * Return value:
 *   None.
 *
 ******************************************************************************
*/
void tm_rrc_glo_eph_to_common_prot_struct( rrc_sat_info_nav_list_type const *const p_RrcNavSatelliteInfo,
                                           glo_EphStructType *pz_ProtGloEphToPost )
{
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Translate Glo Eph (%3u) to internal type", p_RrcNavSatelliteInfo->sat_id);

  /* TM API uses 65-88 for Glonass SVs, RRC decoded SV ID ranges from 0-63 though Glonass uses only 0-23 SVs. */
  if ( p_RrcNavSatelliteInfo->sat_id < N_GLO_SV )
  {
    pz_ProtGloEphToPost->z_Mask.z_FilledMask.u_GloMFlag = 0;
    pz_ProtGloEphToPost->z_Mask.z_FilledMask.u_LnFlag = 0;
    pz_ProtGloEphToPost->z_Mask.z_FilledMask.u_NtFlag = 0;
    pz_ProtGloEphToPost->z_Mask.z_FilledMask.u_P4Flag = 0;
    pz_ProtGloEphToPost->z_Mask.z_FilledMask.u_TimeSrcPFlag = 0;

    pz_ProtGloEphToPost->u_SvId = p_RrcNavSatelliteInfo->sat_id + GLO_SLOTID_OFFSET + 1; /* start at 65 */
    pz_ProtGloEphToPost->z_Mask.u_FilledMask = 0;

    /* Glonass eph. reference time
    *  RRC : 11 bit string. Tb is minutes, scale factor 15, range 0–1425 of 7-bit LSB.
    *  API : 7-bit LSBs
    */
    pz_ProtGloEphToPost->u_Tb = p_RrcNavSatelliteInfo->iod;

    /* svHealth - Ft
       RRC RG: 6 bit string. 1st LSB bit reserved. next 4 LSB bits is Ft;
       API: Ft is a parameter shown the URE at time tb: 4-bit LSBs;
       */
    pz_ProtGloEphToPost->u_Ft = (p_RrcNavSatelliteInfo->sv_health >> 1) & 0xF;
    pz_ProtGloEphToPost->z_Mask.z_FilledMask.u_FtFlag = TRUE;

    /* svHealth - Bn
    *  RRC: 6 bit string. 1st LSB bit reserved. next 4 LSB bits is Ft; Next 1-bit is Bn
    *  API: Health flag Bn: 3-bit LSBs, only check bit 2
    *  */
    pz_ProtGloEphToPost->u_Bn = (p_RrcNavSatelliteInfo->sv_health >> 5) & 0x1;

    /* GLONASS-ClockModel */
    if ( GLONASS_CLK_MODEL == p_RrcNavSatelliteInfo->ganss_clk_model.utc_model_type )
    {
      /* gloGamma : 11 bit string
       * First convert to signed integer (-1024..1023) & then to sign magnitude.
       * MGP API: w_FreqOff using ICD format of sign+magnitude
       */
      pz_ProtGloEphToPost->w_FreqOff = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                           p_RrcNavSatelliteInfo->ganss_clk_model.u.glo.glonass_clk_model.gloGamma, 0x400);

      /* gloTau: 22 bit string
       * First convert to signed integer (-2097152..2097151) & then to sign magnitude.
       * MGP API: q_ClkCorr using ICD format of sign+magnitude
       */
      pz_ProtGloEphToPost->q_ClkCorr = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                           p_RrcNavSatelliteInfo->ganss_clk_model.u.glo.glonass_clk_model.gloTau, 0x200000);

      /* gloDeltaTau: 5 bit string
      *  First convert to signed integer (-16..15) & then to sign magnitude.
      *  MGP API: u_L1L2;
      */
      if ( TRUE == p_RrcNavSatelliteInfo->ganss_clk_model.u.glo.glonass_clk_model.gloDeltaTau_incl )
      {
        pz_ProtGloEphToPost->u_L1L2 = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                          p_RrcNavSatelliteInfo->ganss_clk_model.u.glo.glonass_clk_model.gloDeltaTau, 0x10);
      }
      else
      {
        /* temp. solution until gnss_common.h is updated with added FilledMask element to support this */
        pz_ProtGloEphToPost->u_L1L2 = 0;
      }
    }

    /* ganss_orbit_model */
    if ( TRUE == p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF_incl )
    {
      /* M: 5 bits. Integer (0...31) */
      pz_ProtGloEphToPost->u_En = p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF.gloEn;

      /* P1: 2 bits. Bit string size 2 */
      pz_ProtGloEphToPost->u_P1 = p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF.gloP1;
      pz_ProtGloEphToPost->u_P1 >>= 6;

      /* P2: boolean */
      pz_ProtGloEphToPost->u_P2 = p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF.gloP2;

      /* M: Integer (0..3) */
      if ( TRUE == p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF.gloM_incl )
      {
        pz_ProtGloEphToPost->u_GloM = p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF.gloM;
        pz_ProtGloEphToPost->z_Mask.z_FilledMask.u_GloMFlag = 1;
      }

      /* gloX: unsigned 27 bit string.
      *  First convert to signed integer (-6710864..67108863) & then to sign magnitude.
      *  API expects ICD format 27 bits: sign magnitude
      */
      pz_ProtGloEphToPost->q_SatPos[0] = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                             p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF.gloX,
                                                                                             0x04000000);

      /* gloXdot: unsigned 24 bit string.
      *  First convert to signed integer ( -8388608..6388607) & then to sign magnitude.
      *  API expects ICD format 24 bits: sign-magnitude
      */
      pz_ProtGloEphToPost->q_SatVel[0] = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                             p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF.gloXdot,
                                                                                             0x00800000);

      /* gloXdotdot: unsigned 5 bit string.
      *  First convert to signed integer ( -16..15 ) & then to sign magnitude.
      *  API expects ICD format 5 bits: sign-magnitude
      */
      pz_ProtGloEphToPost->u_Acc[0] = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                          p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF.gloXdotdot,
                                                                                          0x00000010);

      /* gloY: unsigned 27 bit string.
      *  First convert to signed integer (-6710864..67108863) & then to sign magnitude.
      *  API expects ICD format 24 bits: sign-magnitude
      */
      pz_ProtGloEphToPost->q_SatPos[1] = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                             p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF.gloY,
                                                                                             0x04000000);

      /* gloYdot: unsigned 24 bit string.
      *  First convert to signed integer ( -8388608..6388607) & then to sign magnitude.
         API expects ICD format 24 bits: sign-magnitude */
      pz_ProtGloEphToPost->q_SatVel[1] = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                             p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF.gloYdot,
                                                                                             0x00800000);

      /* gloYdotdot: unsigned 5 bit string.
      *  First convert to signed integer ( -16..15 ) & then to sign magnitude.
      *  API expects ICD format 5 bits: sign-magnitude
      */
      pz_ProtGloEphToPost->u_Acc[1] = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                          p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF.gloXdotdot,
                                                                                          0x00000010);

      /* gloZ: unsigned 27 bit string.
      *   First convert to signed integer (-6710864..67108863) & then to sign magnitude.
      *  API expects ICD format 24 bits: sign-magnitude
      */
      pz_ProtGloEphToPost->q_SatPos[2] = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                             p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF.gloZ,
                                                                                             0x04000000);

      /* gloZdot: unsigned 24 bit string.
      *  First convert to signed integer ( -8388608..6388607) & then to sign magnitude.
      *  API expects ICD format 24 bits: sign-magnitude
      */
      pz_ProtGloEphToPost->q_SatVel[2] = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                             p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF.gloZdot,
                                                                                             0x00800000);

      /* gloZdotdot: unsigned 5 bit string.
      *  First convert to signed integer ( -16..15 ) & then to sign magnitude.
      *  API expects ICD format 5 bits: sign-magnitude
      */
      pz_ProtGloEphToPost->u_Acc[2] = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                          p_RrcNavSatelliteInfo->ganss_orbit_model.glonass_ECEF.gloZdotdot,
                                                                                          0x00000010);

    } /* T_GNSS_OrbitModel_glonass_ECEF */

  } /* Valid SatId check ends */

}

/*
 ******************************************************************************
 *
 * tm_rrc_convert_velocity_estimate
 *
 * Function description:
 *  Convert the fix velocity information to RRC specific velocity estimate IEs.
 *
 * Parameters:
 *  f_vel_enu [IN] : East, North, Up velocity (m/s).
 *  f_velunc [IN]: Velocity uncertainty (m/s).
 *  f_velvertunc [IN] : Vertical velocity unc.
 *  p_velocity [OUT] : Filled velocity estimate RRC LSM structure.
 *
 * Return value:
 *   None.
 *
 ******************************************************************************
*/
void tm_rrc_convert_velocity_estimate( float const f_vel_enu[], float const f_velunc, float const f_velvertunc,
                                       rrc_velocity_estimate_type_info *p_velocity )
{
  float f_heading_ang = 0, f_vel_east = 0, f_vel_north = 0, f_vel_vertical = 0, f_temp = 0;
  uint16 w_hor_velocity = 0; 
  int16 s_bearing = 0;
  uint8 u_ver_velocity_bit_string=0;
  uint8 u_ver_direct=0;
  int8 b_vertical_velocity;

  if ( p_velocity == NULL )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Mem not allocated for velocity estimate");
    return;
  }
  f_vel_east = f_vel_enu[0];
  f_vel_north = f_vel_enu[1];
  f_vel_vertical = f_vel_enu[2];
  tm_umts_common_convert_velocity(&f_heading_ang,
                                  &w_hor_velocity,
                                  &s_bearing,
                                  &b_vertical_velocity,
                                  &u_ver_direct,
                                  &u_ver_velocity_bit_string,
                                  &f_vel_east,
                                  &f_vel_north,
                                  &f_vel_vertical);
  /* vertical velocity direction */
  p_velocity->verticalSpeed =  u_ver_velocity_bit_string;
  if (f_vel_vertical < 0.0f)
  {
    p_velocity->verticalSpeedDirection = DOWNWARD;
  }
  else
  {
    p_velocity->verticalSpeedDirection = UPWARD;
  }
 
  /* RRC bearing : (0..359) */
  p_velocity->bearing = (uint16)(fabs(s_bearing));
  /* RRC horizontalSpeed : ( 0..2047 ) */
  p_velocity->horizontalSpeed = w_hor_velocity;
  /* RRC horizontalUncertaintySpeed : (0..255) */
  p_velocity->horizontalUncertaintySpeed = (uint8)f_velunc;
  /* RRC verticalUncertaintySpeed : (0..255) */
  p_velocity->verticalUncertaintySpeed = (uint8)f_velvertunc;
  /* p_velocity->horizontalSpeedUncertainty = ? */
}


/*
 ******************************************************************************
 *  tm_rrc_glo_alm_to_tm_post_data_payload
 *
 * Function description:
 *    Translate RRC Almanac assistance data to TM post structure.
 *
 * Parameters:
 *  p_Almanac: pointer to the structure containing the per SV RRC almanac data.
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data structure.
 *
 * Return value:
 *    None.
 *
 ******************************************************************************
*/
void tm_rrc_glo_alm_to_tm_post_data_payload( const rrc_alm_glonass_Almanac_Set_type *const p_Almanac,
                                             glo_AlmStructType *p_TmPostDataGloAlmanac )
{
  if ( p_Almanac != NULL )
  {
    /* Number of days in a 4-year cycle;
    *  gloAlmNA : 11 bit string (LSB)
    *  Glonass ICD : (1..1461)
    */
    p_TmPostDataGloAlmanac->w_Na = p_Almanac->gloAlmNA;

    /* gloAlmnA is SV_ID : 5 bit string (LSB)
    *  Glonass ICD : (1..24)
       API : uses 65-88 for Glonass SVs */
    p_TmPostDataGloAlmanac->u_SvId = p_Almanac->gloAlmnA + GLO_SLOTID_OFFSET;

    /* gloAkmDeltaTA : 22 bit string (LSB)
       Glonass ICD : (-2097152..2097151)
       API expects IDC format of 22-bit sign mag.
    */
    p_TmPostDataGloAlmanac->q_DracCorrect = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                                p_Almanac->gloAkmDeltaTA, 0x00200000);

    /* gloAlmDeltaIA : 18 bit string (LSB)
    Glonass ICD : (-131072..131071)
    API expects IDC format of 18-bit sign mag. */
    p_TmPostDataGloAlmanac->q_InclCorrect = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                                p_Almanac->gloAlmDeltaIA, 0x00020000);

    /* gloAlmLambdaA : 21 bit string (LSB)
    Glonass ICD : (-1048576..1048575)
    API expects IDC format of 21-bit sign mag.  */
    p_TmPostDataGloAlmanac->q_LongAscend = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                               p_Almanac->gloAlmLambdaA, 0x00100000);

    /* gloAlmTlambdaA : 21 bit string (LSB)
    *  Glonass ICD: (0..2097151)
    *  API : expects IDC format of 21-bit LSBs
    */
    p_TmPostDataGloAlmanac->q_TimeFirstAscend = p_Almanac->gloAlmTlambdaA;

    /* gloAlmDeltaTdotA : 7 bit string (LSB)
    *  Glonass ICD: (-64..63);
       API expects IDC format of 7-bit sign mag. */
    p_TmPostDataGloAlmanac->u_DractRate = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                              p_Almanac->gloAlmDeltaTdotA, 0x00000040);

    /* gloAlmHA : Carrier frequency number 5-bit string (LSB)
    *  Glonass ICD: (0..31)
    *  API expects ICD format of 5-bit LSBs
    */
    p_TmPostDataGloAlmanac->u_FreqNum = p_Almanac->gloAlmHA;

    /* gloAlmCA : 1 bit string (LSB)
    *  Glonass ICD : (0..1)
       */
    p_TmPostDataGloAlmanac->u_Health = p_Almanac->gloAlmCA;

    /* Number of 4-year cycle since 1996/1/1, not available from for alm. */
    p_TmPostDataGloAlmanac->z_Mask.z_FilledMask.u_N4Flag = FALSE;

    /* gloAlmEpsilonA : 15 bit string (LSB)
       Glonass ICD : (0..32767) */
    p_TmPostDataGloAlmanac->w_OrbitEcc = p_Almanac->gloAlmEpsilonA;

    /* gloAlmOmegaA : 16 bit string (LSB)
    *  Glonass ICD : (-32768..32767)
       API expects IDC format of 16-bit sign mag. */
    p_TmPostDataGloAlmanac->w_Perigee = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(
                                                                                            p_Almanac->gloAlmOmegaA, 0x00008000);

    /* gloAlmTauA : 10 bit string (LSB)
    *  Glonass ICD : (-512..511);
    *  API expects IDC format of 10-bit sign mag.
    */
    p_TmPostDataGloAlmanac->w_TauN = tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal(p_Almanac->gloAlmTauA, 0x00000200);

    /* Mn: OSS_BIT string of 2 bits */
    if ( TRUE == p_Almanac->gloAlmMA_incl )
    {
      p_TmPostDataGloAlmanac->z_Mask.z_FilledMask.u_SvTypeFlag = TRUE;
      /* gloAlmMA : 2 bit string (LSB)
      *  Glonass ICD : "00" indicates a GLONASS, satellite, coding "01" indicates a GLONASS-M satellite
      *  API expects IDC format of 2 bits.
         */
      p_TmPostDataGloAlmanac->u_SvType = p_Almanac->gloAlmMA;
    }
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "RRC Glo Alm is NULL");
  }
}

/*
 ******************************************************************************
 *  tm_rrc_translate_glo_acq_asst_no_intcodephase
 *
 * Function description:
 *    Translate RRC Glonass Acquisition assistance data to TM post structure, except
 *    integer code phase parameter.
 *
 * Parameters:
 *  p_GloAcqAsstSatInfo: pointer to the structure containing the per SV RRC acquisition assistance data.
 *  p_prot_glo_aa_steering: pointer to TmCore Glo acquisition assistance(steering) buffer structure.
 *  p_prot_glo_sv_steering_persv: pointer to TmCore Glo per SV acquisition assistance (steering) buffer structure.
 *  p_prot_glo_aa_sv_dir: pointer to TmCore Glo acquisition assistance (SV direction) buffer structure.
 *  p_prot_glo_sv_dir_azim_elev: pointer to TmCore Glo per SV acquisition assistance (SV direction) buffer structure.
 *
 * Return value:
 *    None.
 *
 ******************************************************************************
*/
void tm_rrc_translate_glo_acq_asst_no_intcodephase( rrc_gps_sat_info_struct_type const *const p_GloAcqAsstSatInfo,
                                                    gnss_SvAcqAssistCommonStructType *p_prot_glo_aa_steering, gnss_SvSteerStructType *p_prot_glo_sv_steering_persv,
                                                    gnss_SvDirStructType *p_prot_glo_aa_sv_dir, gnss_AzElInfoStructType *p_prot_glo_sv_dir_azim_elev )
{
  /*
  * AA-Steering
  */
  /* local w_Sv (65..88). Satellite id range (0..63). For Glonass valid from (0..23) */
  p_prot_glo_sv_steering_persv->w_Sv = p_GloAcqAsstSatInfo->satellite_id + GLO_SLOTID_OFFSET + 1;

  /* A-GNSS Reference Measurement 0th order Doppler (-2048..2047) unit is 0.5 m/s */
  /* need to flip the poliarity as ME is using a different convention for range-rate */
  p_prot_glo_sv_steering_persv->f_SvSpeed = (float)p_GloAcqAsstSatInfo->doppler * (FLT)0.5 * (FLT)(-1.0);


  /*  A-GNSS Reference Measurement extra doppler info */
  if ( p_GloAcqAsstSatInfo->extra_doppler_info_incl )
  {
    p_prot_glo_sv_steering_persv->u_SvAccelValid = TRUE;
    p_prot_glo_sv_steering_persv->u_SvSpeedUncValid = TRUE;
    p_prot_glo_sv_steering_persv->f_SvAccel =
                                              ((float)(p_GloAcqAsstSatInfo->extra_doppler_info.doppler_first_order - 42.0) * (FLT)0.00476190 * (FLT)(-1.0));
    p_prot_glo_sv_steering_persv->f_SvSpeedUnc =
                                                 (float)agnss_doppler_unc_to_meters_per_sec[p_GloAcqAsstSatInfo->extra_doppler_info.Doppler_uncertainity];

  }

  /* Code phase in AGNSS in different from that in GPS, no need to subtract from 1*/
  p_prot_glo_sv_steering_persv->f_SubMsec = (float)(p_GloAcqAsstSatInfo->code_phase) / 1024.0f;

  p_prot_glo_sv_steering_persv->u_Modulo80ms = TRUE;
  p_prot_glo_sv_steering_persv->f_SvTimeUnc = (float)gnss_SvTimeUnc[(uint8)p_GloAcqAsstSatInfo->search_window];
  p_prot_glo_aa_steering->u_NumSvs++;


  /*
  * AA-SV Direction
  */
  /* local w_Sv (65..88) */
  if ( TRUE == p_GloAcqAsstSatInfo->azimuth_elevation_incl )
  {
    p_prot_glo_sv_dir_azim_elev->w_Sv = p_GloAcqAsstSatInfo->satellite_id + GLO_SLOTID_OFFSET + 1;
    p_prot_glo_sv_dir_azim_elev->f_Azimuth = (float)p_GloAcqAsstSatInfo->azimuth_elevation_info.azimuth * (FLT)C_RRLP_RRC_AZIMUTH_TO_RAD;
    p_prot_glo_sv_dir_azim_elev->f_Elevation = (float)p_GloAcqAsstSatInfo->azimuth_elevation_info.elevation * (FLT)C_RRLP_RRC_ELEV_TO_RAD;
    p_prot_glo_sv_dir_azim_elev->u_ElevUncValid = FALSE;

    /* Glonass Frequency/Channel number field (b_GloFreqIndex) is filled by AuxiInfo message */
    p_prot_glo_aa_sv_dir->u_NumSvs++;
  }
}

/*
 ******************************************************************************
 *  tm_rrc_glo_acq_asst_to_tm_post_data_and_store
 *
 * Function description:
 *    Translate RRC Acquisition assistance data to TM post structure.
 *    If Glonass msec in a day isn't available to compute GNSS msec of week, then the
 *    'SvId' & 'Integer code phase' will be stored away in the given buffer struct
 *    (p_GloAcqAsstSubParams).
 *
 *
 * Parameters:
 *  p_GloAcqAsst: pointer to the structure containing the per SV RRC acquisition assistance data.
 *  prot_glo_acqasst_buffer_ptr: pointer to Glo SV acquisition assistance buffer structure. This structure
 *                               should be cleaned only at end of each session to avoid losing AA sent in multiple
 *                               RRC MCM message.
 *  q_GloMsecInDay: Glonass milli second in a day & this parameter is mutually exclusive to p_GloAcqAsstSubParams.
 *                  i.e. when q_GloMsecInDay available (through GPS UTCModel or GLONASS TimeModel AD) the
 *                  'SvId' & 'Integer code phase' need not be stored internally, so the p_GloAcqAsstSubParams should
 *                  be NULL.
 *  p_GloAcqAsstSubParams: pointer to buffer struct to store the 'SvId' & 'Integer code phase' pair.
 *                             This is conditional parameter, should be present if q_GloMsecInDay is unavailable.
 *                             This structure should be cleaned only at end of each session to avoid losing AA sent
 *                             in multiple RRC MCM message.
 *
 * Return value:
 *    None.
 *
 ******************************************************************************
*/

void tm_rrc_glo_acq_asst_to_tm_post_data_and_store( const rrc_GANSS_Ref_meas_Info_type *const p_RrcGloAcqAsst,
                                                    prot_glo_acqast_buffer_s_type *p_ProtGloAcqAsstBuf,
                                                    uint32 const q_GloMsecInDay, prot_glo_acqasst_sat_buffer_s_type *p_GloAcqAsstSubParams)
{
  uint8 u_i = 0;
  gnss_SvSteerStructType *p_prot_glo_sv_steering_persv = NULL;
  gnss_SvAcqAssistCommonStructType *p_prot_glo_aa_steering = NULL;
  gnss_SvDirStructType *p_prot_glo_aa_sv_dir = NULL;
  gnss_AzElInfoStructType *p_prot_glo_sv_dir_azim_elev = NULL;
  uint32 u_num_acq_assist = 0;
  prot_glo_acqasst_subset_s_type *p_svid_icp_temp = NULL;

  if ( (NULL == p_ProtGloAcqAsstBuf) || (NULL == p_RrcGloAcqAsst) )
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid pointer: %p, %p", p_ProtGloAcqAsstBuf,
          p_RrcGloAcqAsst);
    return;
  }

  MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Preparing GLO-AcqAssistData for injection");

  /* Steering */
  p_prot_glo_aa_steering = &p_ProtGloAcqAsstBuf->z_steering_glo_data;
  p_prot_glo_sv_steering_persv = &p_ProtGloAcqAsstBuf->z_steering_glo_data.z_SvSteerInfo[0];
  p_prot_glo_aa_steering->u_NumSvs = 0;
  p_ProtGloAcqAsstBuf->z_steering_glo_data.q_SvSteerSrc = MGP_GNSS_SV_STEER_SRC_GLO;

  /* SV Direction */
  p_prot_glo_aa_sv_dir = &p_ProtGloAcqAsstBuf->gnss_sv_dir;
  p_prot_glo_sv_dir_azim_elev = &p_prot_glo_aa_sv_dir->z_SvAzElInfo[0];
  p_prot_glo_aa_sv_dir->u_NumSvs = 0;
  p_ProtGloAcqAsstBuf->gnss_sv_dir.q_GnssType = MGP_GNSS_TYPE_GLO;

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Build %u Glonass AA SVDirection & Steering info", p_RrcGloAcqAsst->sat_cnt);

  /* ME can't eat more than it can digest */
  if ( p_RrcGloAcqAsst->sat_cnt > N_GLO_SV )
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Can't handle Glo AA data of %u SVs. Max %d SVs can be consumed.", p_RrcGloAcqAsst->sat_cnt, N_GLO_SV);
    u_num_acq_assist = N_GLO_SV;
  }
  else
  {
    u_num_acq_assist = p_RrcGloAcqAsst->sat_cnt;
  }

  /* Conditionally alloc space to store svid, integer code phase (due to q_GloMsecInDay isn't available now) */
  if ( p_GloAcqAsstSubParams != NULL )
  {
    if ( NULL == p_GloAcqAsstSubParams->p_svid_icp )
    {
      p_GloAcqAsstSubParams->p_svid_icp = os_MemAlloc((sizeof(prot_glo_acqasst_subset_s_type) * u_num_acq_assist), OS_MEM_SCOPE_TASK);

      if ( NULL == p_GloAcqAsstSubParams->p_svid_icp )
      {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Dynamic mem alloc failed for Glo AA data size %d bytes", (sizeof(prot_glo_acqasst_subset_s_type) * u_num_acq_assist));
        return;
      }
      p_GloAcqAsstSubParams->u_cnt = u_num_acq_assist;
    }

    /* Clone pointer & use for incrementing */
    p_svid_icp_temp = p_GloAcqAsstSubParams->p_svid_icp;
  }



  /* The satellite count - sat_cnt (1..64) */
  for ( u_i = 0; u_i < u_num_acq_assist; ++u_i )
  {
    /* Satellite id range (0..63). For Glonass valid from (0..23) */
    if ( p_RrcGloAcqAsst->sat_info[u_i].satellite_id < N_GLO_SV )
    {
      tm_rrc_translate_glo_acq_asst_no_intcodephase(&p_RrcGloAcqAsst->sat_info[u_i], p_prot_glo_aa_steering, p_prot_glo_sv_steering_persv,
                                                    p_prot_glo_aa_sv_dir, p_prot_glo_sv_dir_azim_elev);

      /* Store or compute using integer code phase ? */
      if ( (p_GloAcqAsstSubParams != NULL) && (p_svid_icp_temp != NULL) )
      {
        /* Store sv-id & integer code phase pair */
        p_svid_icp_temp->satellite_id = p_RrcGloAcqAsst->sat_info[u_i].satellite_id;
        p_svid_icp_temp->integer_code_phase = p_RrcGloAcqAsst->sat_info[u_i].integer_code_phase;
        ++(p_svid_icp_temp);
      }
      else
      {
        /* Compute GNSS msec of week from integer code phase */
        p_prot_glo_sv_steering_persv->q_Ms = tm_util_translate_glo_intcodephase(p_RrcGloAcqAsst->sat_info[u_i].integer_code_phase, q_GloMsecInDay);
        p_ProtGloAcqAsstBuf->m.gnssMsecOfWeekValid = 1;
      }

      /* Increment per sv struct */
      ++p_prot_glo_sv_steering_persv;
      ++p_prot_glo_sv_dir_azim_elev;
    }
    else
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Glonass AA SvId %u invalid", p_RrcGloAcqAsst->sat_info[u_i].satellite_id);
    }
  } /* SV for-loop */
}

/*
 ******************************************************************************
 *
 * tm_rrc_utils_microsec_to_gadk
 *
 * Function description:
 * This is obtained from the 3GPP RRC spec V9.0.0
 * The uncertainty r, expressed in microseconds, is mapped to a number K, with the following formula:
                        r = C*(((1+x)K)-1)
 * with C = 0.0022 and x = 0.18. To encode any higher value of uncertainty than that corresponding in the above
 * formula to K=127, the same value, K=127, shall also be used. The uncertainty is then coded on 7 bits, as the
 * binary encoding of K. Examples of gnss-TOD-unc value are as in the table Value of K to Value of uncertainty
 * relation below.
 *
 * Parameters:
 *
 *   q_time_unc_microseconds - Time uncertainity in microseconds
 *
 * Return value:
 *
 *   K value, which has a range 0 <= K <= 127;
 *
 ******************************************************************************
*/

uint8 tm_rrc_utils_microsec_to_gadk( float f_time_unc_microseconds )
{
  uint8 u_KValue, u_Offset;
  uint32 i;
  static const float *p_GadKUncMap;

  p_GadKUncMap = f_GadKTimeUncMapNewRrc;

  u_Offset = (128 / 4);
  u_KValue = (128 / 2);

  /* Perform binary search of 128 possible K values.
   * 6 compares here, followed by one after the for loop.  */
  for ( i = 6; i; i-- )
  {
    if ( u_KValue < 128 )
    {
      if ( f_time_unc_microseconds > p_GadKUncMap[u_KValue] )
        u_KValue += u_Offset;
      else
        u_KValue -= u_Offset;

      u_Offset >>= 1;
    }
  }

  /* Final check; bump up if not equal to cover unc region */
  if ( f_time_unc_microseconds > p_GadKUncMap[u_KValue] )
    u_KValue += 1;


  if ( u_KValue < 1 )
    u_KValue = 1;   /* Min value is 1 */
  else if ( u_KValue > 127 )
    u_KValue = 127; /* Max value is 127 */

  return ( u_KValue );
}

#endif /* FEATURE_GANSS_SUPPORT */

#endif /* ifdef FEATURE_SUPPORT_UMTS_GPS_PROTOCOLS */

