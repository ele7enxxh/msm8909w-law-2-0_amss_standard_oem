/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         TM RRLP Utility

GENERAL DESCRIPTION
  This file contains TM's RRLP protocol utilities, which convert decoded RRLP
  messages for the consumption of TmCore.  

  Copyright (c) 2007 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_common/src/tm_rrlp.c#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  09/24/14   rk      Added missing steering source type while injecting AcqAssist.
  09/17/12   MJ      Modified gps_meas_status_bits_check call
  04/06/11   LT      ASN1 migration.
  05/29/07   LT      Moved the psudoRMS_formatter function to tm_umts_common_utils.c. 
  05/11/07   LT      Moved variables and constants common to RRLP and RRC out, etc.
  04/25/07   LT      Initial version

===========================================================================*/
#include "gps_variation.h"
#include "customer.h"   /* Customer configuration file */
#ifdef FEATURE_SUPPORT_UMTS_GPS_PROTOCOLS

#include "comdef.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "tm_prtl_iface.h"
#include "tm_asn1util.h"
#include "tm_rrlp_asn1.h"


#include "sm_api.h" /* included only for the data structures defined there */
#include "tm_umts_common_utils.h"
#include "tm_rrlp.h"

#include "msg.h"

/* Included to remove compiler warning in tm_rrlp_asn1.c*/
extern unsigned short const _oss_OSet_info[];

/* Local function protocols */


/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRLP reference time IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing ASN.1 decoded RRLP message.
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

void tm_rrlp_reftime_to_TmCore_post_data_payload( rrlp_ControlHeader *p_assist, 
                                                  tm_post_data_payload_type    *p_tm_post_data_payload
                                                )
{

  gps_RefTimeStructType *p_tm_post_data_payload_reftime = &p_tm_post_data_payload->reftime_data;
  uint32 q_GpsTow23b, q_I = 0;
  uint16 w_GpsWeek; 
  uint8 u_i;
  struct rrlp_GPSTOWAssist *p_rrlp_tow = NULL;
  struct rrlp_GPSTOWAssistElement *p_rrlp_tow_elem;

  /* Included to remove compiler warning in tm_rrlp_asn1.c, can be incluided in any funtion */
  (void) _oss_OSet_info[0];

  /* 23 bit Gps Time of Week */
  q_GpsTow23b = (uint32)p_assist->referenceTime.gpsTime.gpsTOW23b;

  /* 10-bit Gps Week */
  w_GpsWeek = (uint16)p_assist->referenceTime.gpsTime.gpsWeek;

  /* Convert the 10-bit GPS week to a 16-bit one to reflect the
     rollover */
  w_GpsWeek = gnss_GpsWeekExtend( w_GpsWeek );
  q_GpsTow23b *= 80; /* q_GpsTow23b LSB = 0.08 sec */
  tm_umts_common_utils_GpsMsecWeekLimit( (int32 *)&q_GpsTow23b, &w_GpsWeek );

  p_tm_post_data_payload_reftime->u_GpsWeekIsValid = TRUE;
  p_tm_post_data_payload_reftime->w_GpsWeek = w_GpsWeek;
  p_tm_post_data_payload_reftime->q_GpsMsec = q_GpsTow23b;

  if ( ( z_umts_nv_item_ram_copy.aagps_default_ref_time_unc_50ms * 50 ) >= 1000 )  
  {
    p_tm_post_data_payload_reftime->f_GpsTimeUnc = (float)z_umts_nv_item_ram_copy.aagps_default_ref_time_unc_50ms * 50.f;
  }
  else
  {
    p_tm_post_data_payload_reftime->f_GpsTimeUnc = 1000.f;
  }

  /* Raw TOW data is present, we thus send it across to MGP */
  if( p_assist->referenceTime.m.gpsTowAssistPresent )
  {
    p_rrlp_tow = &p_assist->referenceTime.gpsTowAssist;
    p_rrlp_tow_elem = p_rrlp_tow->elem;
    memset(&p_tm_post_data_payload_reftime->a_GpsTowAssist,0 ,sizeof(gps_rrlp_GPSTOWAssistElement) * N_SV);
    
    for ( u_i=0; 
          (u_i < p_rrlp_tow->n ) && ( u_i < ARR_SIZE (p_tm_post_data_payload_reftime->a_GpsTowAssist )); 
          u_i++ )
    {
      MSG_LOW("Original satelliteID: %d tlmWord: %d antiSpoof: %d ", 
              p_rrlp_tow_elem->satelliteID+1,
              p_rrlp_tow_elem->tlmWord, 
              p_rrlp_tow_elem->antiSpoof);
      MSG_LOW("Original satelliteID: %d Alert: %d RsvdBits: %d ", 
              p_rrlp_tow_elem->satelliteID+1,
              p_rrlp_tow_elem->alert, 
              p_rrlp_tow_elem->tlmRsvdBits);

      p_tm_post_data_payload_reftime->a_GpsTowAssist[u_i].satelliteID = (uint32) (p_rrlp_tow_elem->satelliteID+1);
      p_tm_post_data_payload_reftime->a_GpsTowAssist[u_i].tlmWord = (uint32) p_rrlp_tow_elem->tlmWord;
      p_tm_post_data_payload_reftime->a_GpsTowAssist[u_i].antiSpoof = (uint32) p_rrlp_tow_elem->antiSpoof;
      p_tm_post_data_payload_reftime->a_GpsTowAssist[u_i].alert = (uint32) p_rrlp_tow_elem->alert;
      p_tm_post_data_payload_reftime->a_GpsTowAssist[u_i].tlmRsvdBits = (uint32) p_rrlp_tow_elem->tlmRsvdBits;

      p_rrlp_tow_elem++;
      q_I++;
    }

    p_tm_post_data_payload_reftime->q_NumTowAssistSv = q_I;

  } 

}


/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRLP reference position IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing ASN.1 decoded RRLP message
 *            of rrlp_ControlHeader for GPS data.
 *
 *  p_ganss_assist: pointer to the structure containing ASN.1 decoded RRLP 
 *                  message of rrlp_GANSS_ControlHeader for GANSS data.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 * 
 *  p_gad_pos_to_save:      pointer to the structure that keeps a copy of the
 *                          GAD shape data for possible later returning of
 *                          reference position to the network.
 *
 *  u_gad_compliant:        TRUE - all GAD shape element compliant;
 *                          FALSE - unc K max. defined as 10,000 km.
 *
 * Return value:
 *
 *    FALSE: if the GAD shape is not supported by RRLP-v5; TRUE otherwise.
 *    
 *
 ******************************************************************************
*/

boolean tm_rrlp_refpos_to_TmCore_post_data_payload( rrlp_ControlHeader             *p_assist, 
                                                    rrlp_GANSS_ControlHeader       *p_ganss_assist,
                                                    tm_post_data_payload_type      *p_tm_post_data_payload, 
                                                    uint8                          *p_gad_pos_to_save,
                                                    boolean                        u_gad_compliant
                                                  )
{

  gad_ellip_alt_unc_ellip_type   *p_gad_ellip_alt_unc_ellip;

  gps_RefLocStructType *p_tm_post_data_payload_seed_data = &p_tm_post_data_payload->seed_data;

  uint8 u_Octet1;
  uint8 u_RefLocShape;  /* shape of the reference location */

  int32  l_long_word = 0;
  uint32 q_Word;
  sint31 l_Word;
  const  uint8 *p_OctetString;

  uint8 u_unc_major, u_unc_minor, u_orientation, u_alt_unc, u_confidence;
  float f_scale_up_to_99pct_conf;
  uint32   q_GpsTow23b;
  uint16   w_GpsWeek; 

  /** check to make sure that one, and only one source ( GPS vs. GANSS) is true */
  if ( ( ( p_assist == NULL ) && ( p_ganss_assist == NULL ) ) ||
       ( ( p_assist != NULL ) && ( p_ganss_assist != NULL ) )
     )
  {
    MSG_ERROR("Err: ref pos. source info. invalid", 0, 0, 0);
    return FALSE;
  }

  p_gad_ellip_alt_unc_ellip = (gad_ellip_alt_unc_ellip_type *)p_gad_pos_to_save;

  if ( p_assist != NULL )
  {
    p_OctetString = (const uint8 *)&p_assist->refLocation.threeDLocation.data[0];
  }
  else /* because of the check above, if not GPS, it has to be GANSS */
  {
    p_OctetString = 
     (const uint8 *)&p_ganss_assist->ganssCommonAssistData.ganssRefLocation.threeDLocation.data[0];
  }
  
  /* Shape */
  u_Octet1 = (uint8)*p_OctetString++;
  u_RefLocShape = u_Octet1 >> 4;


  /* Shape: per RRLP V5, only "ellipsoid point with altitude and ellipoid uncertainty" is allowed.
     Will not use any other shape and will skip reference location if that occurs.  */
  if ( u_RefLocShape != C_POS_SHAPE_ELLIP_ALT_UNC_ELLIP )
  {
    MSG_LOW("RRLP RefLoc shape not allowed: %u", u_RefLocShape, 0, 0);
    return FALSE;
  }

  else /* the only supported shape by RRLP-V5 */
  {

    /* this is for possible reporting reference position back to the network */
    p_gad_ellip_alt_unc_ellip->u_ShapeCode = u_RefLocShape; 
    p_gad_ellip_alt_unc_ellip->u_ShapeCode <<= 4; /* shape code is the high nibble of this octet */ 

    p_tm_post_data_payload_seed_data->u_AltitudeIsValid = TRUE;
    p_tm_post_data_payload_seed_data->u_HeadingIsValid = FALSE;
    p_tm_post_data_payload_seed_data->u_VelocityIsValid = FALSE;


    /********************************* LATITUDE ***************************/

    /* NOT 2's compliment, but: 23-bit unsigned values representing 
       latitude in meters. MSB = 0: latitude north; MSB = 1: latitude south; */
    q_Word = tm_umts_common_utils_NBytesGet_bigEndian( &p_OctetString, 3 );

    /* store ref. Loc. for possible reporting reference position back to the network */
    p_gad_ellip_alt_unc_ellip->u_Lat2 = (uint8)q_Word;
    p_gad_ellip_alt_unc_ellip->u_Lat1 = (uint8)(q_Word>>8);
    p_gad_ellip_alt_unc_ellip->u_Lat0 = (uint8)(q_Word>>16);

    if ( q_Word & 0x800000 )
    {
      p_tm_post_data_payload_seed_data->d_Latitude = -(double)( q_Word & 0x7FFFFF )
                                                     * C_GAD_LAT_TO_RAD;
    }
    else /* north */
    {
      p_tm_post_data_payload_seed_data->d_Latitude = (double)(q_Word & 0x7FFFFF) 
                                                     * C_GAD_LAT_TO_RAD;
    }

    /*************** LONGITUDE **************************************/
     
    /*** Already in 2's complement */
    q_Word = tm_umts_common_utils_NBytesGet_bigEndian( &p_OctetString, 3 );
    l_Word = tm_umts_common_utils_S32Convert( q_Word, 24 );

    /* convert longitude from GAD to rad. */
    p_tm_post_data_payload_seed_data->d_Longitude = (double) ( l_Word * C_GAD_LON_TO_RAD );

    /* store ref. Loc. for possible reporting reference position back to the network */
    l_long_word = l_Word;
    p_gad_ellip_alt_unc_ellip->u_Long2 = (uint8)l_long_word;
    p_gad_ellip_alt_unc_ellip->u_Long1 = (uint8)(l_long_word>>8);
    p_gad_ellip_alt_unc_ellip->u_Long0 = (uint8)(l_long_word>>16);


    /******************* ALTIDUDE **********************************************/

    /* NOT 2's compliment, but: 16 bits with 15-bit unsigned Values representing 
       altitude in meters. Bit15: MSB = 0: height; MSB = 1: depth; */

    q_Word = tm_umts_common_utils_NBytesGet_bigEndian( &p_OctetString, 2 );
    /* q_Word now contains a 16-bit word, with its MSB representing direction,
       and the rest 15 bits the absolute value of altitude */

    /* for possible return-reference-pos use */
    p_gad_ellip_alt_unc_ellip->u_Alt1 = (uint8)q_Word;
    p_gad_ellip_alt_unc_ellip->u_Alt0 = (uint8)(q_Word >> 8);

    if ( q_Word & 0x8000 )
    {
      p_tm_post_data_payload_seed_data->f_Altitude = -(float)(q_Word & 0x7FFF);
    }
    else
    {
      p_tm_post_data_payload_seed_data->f_Altitude = (float)(q_Word & 0x7FFF);
    }


    /******************* UNCERTAINTY  **************************************/

    u_unc_major = (uint8)*p_OctetString++;
    u_unc_major &= 0x7F;
    p_gad_ellip_alt_unc_ellip->u_UncMajor = u_unc_major; /* possible ret-ref-pos */
    p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMajor = tm_umts_common_utils_GADK_to_meter(TRUE, u_unc_major );

    u_unc_minor = (uint8)*p_OctetString++;
    u_unc_minor &= 0x7F;
    p_gad_ellip_alt_unc_ellip->u_UncMinor = u_unc_minor; /* possible ret-ref-pos */
    p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMinor = tm_umts_common_utils_GADK_to_meter(TRUE, u_unc_minor );

    u_orientation = (uint8)*p_OctetString++;
    p_gad_ellip_alt_unc_ellip->u_OrientMajor = u_orientation; /* possible ret-ref-pos */

    u_alt_unc = (uint8)*p_OctetString++;
    p_gad_ellip_alt_unc_ellip->u_UncAlt = u_alt_unc; /* possible ret-ref-pos */
    p_tm_post_data_payload_seed_data->f_AltitudeUnc = tm_umts_common_utils_GADK_to_meter(FALSE, u_alt_unc);

    u_confidence = (uint8)*p_OctetString++;
    u_confidence &= 0x7F;
    p_gad_ellip_alt_unc_ellip->u_Confidence = u_confidence; /* possible ret-ref-pos */

    /* Scale up the circle size based on the injected % confidence.
       For example, the PDM default % confidence is 39% (standard ellipse),
       and the AI needs a 99% confidence value, so in that case the following
       logic scales the size up radius by ~3 times. */
    /* "confidence" is uint32 In percentage (0..100) ref. rrclsmif.h */

    f_scale_up_to_99pct_conf = 
      tm_util_get_cep_scale_ratio_from_pct((float)u_confidence * 0.01f, 0.99f);

    p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMajor *=  
     f_scale_up_to_99pct_conf;

    p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMinor *=  
     f_scale_up_to_99pct_conf;

    /* map K of 0x7F to 10,000km -- deviating from 3GPP TS 03.32 which defines 
       K of 0x7F as 1800 km. */
    if ( u_gad_compliant == FALSE )
    {
      if ( u_unc_major == 0x7F )
      {
        p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMajor = 10000000.0f; 
      }
      if ( u_unc_minor == 0x7F )
      {
        p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMinor = 10000000.0f; 
      }
    }

  }

  if ((NULL != p_assist) && !( p_assist->m.referenceTimePresent) )
  {
    p_tm_post_data_payload_seed_data->q_GpsMsec = 0;
    p_tm_post_data_payload_seed_data->w_GpsWeek = C_GPS_WEEK_UNKNOWN;
  }
  else if(NULL != p_assist)
  {
    /* 23 bit Gps Time of Week */
    q_GpsTow23b = (uint32)p_assist->referenceTime.gpsTime.gpsTOW23b;
    /* 10-bit Gps Week */
    w_GpsWeek = (uint16)p_assist->referenceTime.gpsTime.gpsWeek;
    /* Convert the 10-bit GPS week to a 16-bit one to reflect the
     rollover */
    w_GpsWeek = gnss_GpsWeekExtend( w_GpsWeek );
    q_GpsTow23b *= 80; /* q_GpsTow23b LSB = 0.08 sec */
    tm_umts_common_utils_GpsMsecWeekLimit( (int32 *)&q_GpsTow23b, &w_GpsWeek );
    p_tm_post_data_payload_seed_data->w_GpsWeek = w_GpsWeek;
    p_tm_post_data_payload_seed_data->q_GpsMsec = q_GpsTow23b;
  }
  return TRUE;

}


/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRLP navigation model IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_Ephem: pointer to the structure containing the per SV based ASN.1 
 *            decoded RRLP navigation model data.
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

void tm_rrlp_eph_to_TmCore_post_data_payload( rrlp_UncompressedEphemeris     *p_Ephem,
                                              tm_post_data_payload_type      *p_tm_post_data_payload,
                                              uint16 w_week
                                            )

{
  gps_EphStructType *p_tm_post_data_payload_eph_data = &p_tm_post_data_payload->eph_data;
     

  p_tm_post_data_payload_eph_data->u_CodeL2 = (uint8)p_Ephem->ephemCodeOnL2; 
  p_tm_post_data_payload_eph_data->u_Accuracy = (uint8)p_Ephem->ephemURA;
  p_tm_post_data_payload_eph_data->u_Health = (uint8)p_Ephem->ephemSVhealth;
  p_tm_post_data_payload_eph_data->w_Iodc = (uint16)p_Ephem->ephemIODC;
  p_tm_post_data_payload_eph_data->u_Iode = p_tm_post_data_payload_eph_data->w_Iodc;

  p_tm_post_data_payload_eph_data->u_PcodeFlagIsValid = TRUE;
  p_tm_post_data_payload_eph_data->u_PcodeFlag = (uint8)p_Ephem->ephemL2Pflag;
  
  p_tm_post_data_payload_eph_data->u_Sf1BitsAreValid = FALSE; /* QQQ: change to TRUE when resolved the QQQ below */ 
  p_tm_post_data_payload_eph_data->q_Sf1ResrvBits[0] = p_Ephem->ephemSF1Rsvd.reserved1;
  p_tm_post_data_payload_eph_data->q_Sf1ResrvBits[1] = p_Ephem->ephemSF1Rsvd.reserved2;
  p_tm_post_data_payload_eph_data->q_Sf1ResrvBits[2] = p_Ephem->ephemSF1Rsvd.reserved3;
  /* QQQ what to do with reserved4 on the right side, and check the bit order */

  p_tm_post_data_payload_eph_data->u_TgdIsValid = TRUE;
  p_tm_post_data_payload_eph_data->u_Tgd = (uint8)p_Ephem->ephemTgd;

  p_tm_post_data_payload_eph_data->w_Toc = (uint16)p_Ephem->ephemToc;
  p_tm_post_data_payload_eph_data->u_Af2 = (uint8)p_Ephem->ephemAF2;
  p_tm_post_data_payload_eph_data->w_Af1 = (uint16)p_Ephem->ephemAF1;
  p_tm_post_data_payload_eph_data->q_Af0 = (uint32)p_Ephem->ephemAF0;
  p_tm_post_data_payload_eph_data->w_Crs = (uint16)p_Ephem->ephemCrs;
  p_tm_post_data_payload_eph_data->w_DeltaN = (uint16)p_Ephem->ephemDeltaN;
  p_tm_post_data_payload_eph_data->q_M0 = (uint32)p_Ephem->ephemM0;
  p_tm_post_data_payload_eph_data->w_Cuc = (uint16)p_Ephem->ephemCuc;
  p_tm_post_data_payload_eph_data->q_E = (uint32)p_Ephem->ephemE;
  p_tm_post_data_payload_eph_data->w_Cus = (uint16)p_Ephem->ephemCus;
  p_tm_post_data_payload_eph_data->q_SqrtA = (uint32)p_Ephem->ephemAPowerHalf;
  p_tm_post_data_payload_eph_data->w_Toe = (uint16)p_Ephem->ephemToe;

  p_tm_post_data_payload_eph_data->u_FitIntervalIsValid = TRUE;
  p_tm_post_data_payload_eph_data->u_FitIntervalFlag = p_Ephem->ephemFitFlag;

  p_tm_post_data_payload_eph_data->u_AodoIsValid = TRUE;
  p_tm_post_data_payload_eph_data->u_Aodo = (uint8)p_Ephem->ephemAODA;

  p_tm_post_data_payload_eph_data->w_Cic = (uint16)p_Ephem->ephemCic;
  p_tm_post_data_payload_eph_data->q_Omega0 = (uint32)p_Ephem->ephemOmegaA0;
  p_tm_post_data_payload_eph_data->w_Cis = (uint16)p_Ephem->ephemCis;
  p_tm_post_data_payload_eph_data->q_I0 = (uint32)p_Ephem->ephemI0;
  p_tm_post_data_payload_eph_data->w_Crc = (uint16)p_Ephem->ephemCrc;
  p_tm_post_data_payload_eph_data->q_Omega = (uint32)p_Ephem->ephemW;
  p_tm_post_data_payload_eph_data->q_OmegaDot = (uint32)p_Ephem->ephemOmegaADot;
  p_tm_post_data_payload_eph_data->w_Idot = (uint16)p_Ephem->ephemIDot;

  /* QQQ: we seem to need the week number for now */
  if ( w_week != 0 )
  {
    p_tm_post_data_payload_eph_data->u_GpsWeekIsValid = TRUE;
    p_tm_post_data_payload_eph_data->w_GpsWeek = w_week; 
  }
  else
  {
    p_tm_post_data_payload_eph_data->u_GpsWeekIsValid = FALSE;
  }          
}

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRLP IONO IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing ASN.1 decoded RRLP IONO data.
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

void tm_rrlp_Iono_to_TmCore_post_data_payload( rrlp_ControlHeader *p_assist, 
                                               tm_post_data_payload_type   *p_tm_post_data_payload )
{

  rrlp_IonosphericModel *p_Iono = &p_assist->ionosphericModel;
  gps_IonoStructType *p_tm_post_data_payload_iono = &p_tm_post_data_payload->iono_data;

  p_tm_post_data_payload_iono->u_Alpha0 = p_Iono->alfa0;
  p_tm_post_data_payload_iono->u_Alpha1 = p_Iono->alfa1;
  p_tm_post_data_payload_iono->u_Alpha2 = p_Iono->alfa2;
  p_tm_post_data_payload_iono->u_Alpha3 = p_Iono->alfa3;
  p_tm_post_data_payload_iono->u_Beta0 = p_Iono->beta0;
  p_tm_post_data_payload_iono->u_Beta1 = p_Iono->beta1;
  p_tm_post_data_payload_iono->u_Beta2 = p_Iono->beta2;
  p_tm_post_data_payload_iono->u_Beta3 = p_Iono->beta3;

}


/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRLP UTC Model IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing ASN.1 decoded RRLP UTC model.
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

void tm_rrlp_UtcModel_to_TmCore_post_data_payload( rrlp_ControlHeader *p_assist, 
                                                   tm_post_data_payload_type   *p_tm_post_data_payload )
{
  rrlp_UTCModel *p_Utc = &p_assist->utcModel;
  gps_UtcInfoRptStructType *p_tm_post_data_payload_utc_model = &p_tm_post_data_payload->utc_model_data;

  p_tm_post_data_payload_utc_model->q_A0 = p_Utc->utcA0;
  p_tm_post_data_payload_utc_model->q_A1 = p_Utc->utcA1;
  p_tm_post_data_payload_utc_model->u_DeltaTls = p_Utc->utcDeltaTls;
  p_tm_post_data_payload_utc_model->u_Tot = p_Utc->utcTot;
  p_tm_post_data_payload_utc_model->u_WNt = p_Utc->utcWNt;
  p_tm_post_data_payload_utc_model->u_WNlsf = p_Utc->utcWNlsf;
  p_tm_post_data_payload_utc_model->u_DN = p_Utc->utcDN;
  p_tm_post_data_payload_utc_model->u_DeltaTlsf = p_Utc->utcDeltaTls;

}




/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRLP almanac IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_Almanac: pointer to the structure containing the per SV based ASN.1 
 *             decoded RRLP almanac data.
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

void tm_rrlp_alm_to_TmCore_post_data_payload( rrlp_AlmanacElement         *p_Almanac,
                                              tm_post_data_payload_type   *p_tm_post_data_payload
                                            )

{
    gps_AlmStructType *p_tm_post_data_payload_alm_data = &p_tm_post_data_payload->alm_data;


    p_tm_post_data_payload_alm_data->u_Sv = (uint8)p_Almanac->satelliteID + 1; /* 6-bit unsigned */;
    p_tm_post_data_payload_alm_data->w_E = (uint16)p_Almanac->almanacE;
    p_tm_post_data_payload_alm_data->u_Toa = (uint8)p_Almanac->alamanacToa;
    p_tm_post_data_payload_alm_data->w_DeltaI = (uint16)p_Almanac->almanacKsii;
    p_tm_post_data_payload_alm_data->w_OmegaDot = (uint16)p_Almanac->almanacOmegaDot;
    p_tm_post_data_payload_alm_data->u_Health = (uint8)p_Almanac->almanacSVhealth;
    p_tm_post_data_payload_alm_data->q_SqrtA = (uint32)p_Almanac->almanacAPowerHalf;
    p_tm_post_data_payload_alm_data->q_Omega0 = (uint32)p_Almanac->almanacOmega0;
    p_tm_post_data_payload_alm_data->q_Omega = (uint32)p_Almanac->almanacW;
    p_tm_post_data_payload_alm_data->q_M0 = (uint32)p_Almanac->almanacM0;
    p_tm_post_data_payload_alm_data->w_Af0 = (uint16)p_Almanac->almanacAF0;
    p_tm_post_data_payload_alm_data->w_Af1 = (uint16)p_Almanac->almanacAF1;

    p_tm_post_data_payload_alm_data->u_DataIdIsValid = FALSE;
    p_tm_post_data_payload_alm_data->u_SvGhealthIsValid = FALSE;
}



/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRLP acquistion assistance's SV direction IE to 
 *  the format ready for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing the per SV based ASN.1 
 *            decoded RRLP almanac data.
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

void tm_rrlp_AcqAssistSvDir_to_TmCore_post_data_payload( rrlp_ControlHeader *p_assist, 
                                                         tm_post_data_payload_type *p_tm_post_data_payload
                                                       )
{

  gnss_SvDirStructType    *p_tm_post_data_payload_sv_dir = &p_tm_post_data_payload->gnss_sv_dir;
  gnss_AzElInfoStructType *p_zSvAzElInfo = &p_tm_post_data_payload_sv_dir->z_SvAzElInfo[0];
  rrlp_SeqOfAcquisElement *AcqElement = &p_assist->acquisAssist.acquisList;
  rrlp_AcquisElement      *p_Acq;
  
  uint8 u_i;

  memset( p_tm_post_data_payload_sv_dir, 0, sizeof(*p_tm_post_data_payload_sv_dir) );

  p_tm_post_data_payload_sv_dir->u_NumSvs = 0;
  p_tm_post_data_payload_sv_dir->q_GnssType = MGP_GNSS_TYPE_GPS;  
  
  p_Acq = AcqElement->elem;

  for ( u_i=0; u_i<AcqElement->n; u_i++ )
  {

    if ( p_Acq->m.addionalAnglePresent ) /* Optional AdditionalAngle */
    {
      
      p_zSvAzElInfo->w_Sv = (uint8)p_Acq->svid + 1;
      p_zSvAzElInfo->f_Azimuth =  (float)p_Acq->addionalAngle.azimuth * C_RRLP_RRC_AZIMUTH_TO_RAD;
      p_zSvAzElInfo->f_Elevation = (float)p_Acq->addionalAngle.elevation * C_RRLP_RRC_ELEV_TO_RAD;

      p_zSvAzElInfo->u_ElevUncValid = FALSE; 

      p_tm_post_data_payload_sv_dir->u_NumSvs++;
      p_zSvAzElInfo++;

    }

    /* Move to next element in the list */
    if ( u_i < (AcqElement->n - 1) )
      p_Acq++;
    
  } /* SV for-loop */

}




/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRLP acquistion assistance IE's steering data to 
 *  the format ready for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing the per SV based ASN.1 
 *            decoded RRLP acq. assistance data.
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

void tm_rrlp_AcqAssistSteering_to_TmCore_post_data_payload( rrlp_ControlHeader *p_assist, 
                                                            tm_post_data_payload_type *p_tm_post_data_payload,
                                                            uint16 w_gps_week
                                                          )
{

  gnss_SvAcqAssistCommonStructType *p_tm_post_data_payload_aa_data = &p_tm_post_data_payload->gnss_aa_data;  
  gnss_SvSteerStructType *p_SvSteerPerSv = &p_tm_post_data_payload_aa_data->z_SvSteerInfo[0];

  rrlp_SeqOfAcquisElement *AcqElement = &p_assist->acquisAssist.acquisList;
  rrlp_AcquisElement      *p_Acq;
  
  uint8 u_i;
  
  memset( p_tm_post_data_payload_aa_data, 0, sizeof(*p_tm_post_data_payload_aa_data) );

  p_tm_post_data_payload_aa_data->u_WeekValid = 
                                        (w_gps_week != C_GPS_WEEK_UNKNOWN);
  p_tm_post_data_payload_aa_data->w_Week = w_gps_week;

  p_tm_post_data_payload_aa_data->q_Msec = (uint32)p_assist->acquisAssist.timeRelation.gpsTOW * 80; /* LSB of the RRLP IE is 80 msec */
  p_tm_post_data_payload_aa_data->u_NumSvs = 0;
  p_tm_post_data_payload_aa_data->u_TimeRefIsGnss = TRUE; /* steering data is referenced to GPS time */
  p_tm_post_data_payload_aa_data->u_RefPnPhaseValid = FALSE;
  p_tm_post_data_payload_aa_data->q_SvSteerSrc      = MGP_GNSS_SV_STEER_SRC_GPS;
  
  
  p_Acq = AcqElement->elem;

  for ( u_i=0; u_i<AcqElement->n; u_i++ )
  {

    p_SvSteerPerSv->w_Sv = (uint8)p_Acq->svid + 1;

    /* 0th order Doppler: RRLP has resolution of 2.5 Hz  */
    p_SvSteerPerSv->f_SvSpeed = (float)p_Acq->doppler0  * 2.5 * (FLT)C_L1HzToMs * (-1.0);

    /* initialize these for FALSE */
    p_SvSteerPerSv->u_SvAccelValid = FALSE;
    p_SvSteerPerSv->u_SvSpeedUncValid = FALSE;

    if ( p_Acq->m.addionalDopplerPresent )
    {
      uint8 u_rrlp_doppler_unc_index;
      uint16 w_word;

      p_SvSteerPerSv->u_SvAccelValid = TRUE;
      p_SvSteerPerSv->u_SvSpeedUncValid = TRUE;

      /* RRLP doppler1 has 6 bits ranging (0..63), ranging fromm -1 to 0.5 Hz/sec; hence a resolution of
         1.5 / 63 = 0.023809523 Hz/sec; offset is -1.0; */
      p_SvSteerPerSv->f_SvAccel = ( (float)p_Acq->addionalDoppler.doppler1 * 0.023809523 - 1.0 ) * C_L1HzToMs;

      /* Doppler unc: Sv doppler unc in RRLP represented 
         as 200 * 2 ** (-u_rrlp_doppler_unc_index)  */
      u_rrlp_doppler_unc_index = (uint8)p_Acq->addionalDoppler.dopplerUncertainty;
      w_word = (uint16)200;
      w_word <<= 8; /* so that later less than 0.5 Hz can be retained */
      w_word >>= u_rrlp_doppler_unc_index;
      p_SvSteerPerSv->f_SvSpeedUnc = (float)(w_word >> 8);
      if ( w_word & 0x08 )
      {
        p_SvSteerPerSv->f_SvSpeedUnc += (FLT)0.5;
      }
      p_SvSteerPerSv->f_SvSpeedUnc *= (FLT)C_L1HzToMs; /* convert from Hz to m/s */

    }

    p_SvSteerPerSv->f_SubMsec = (float)(1022 - p_Acq->codePhase) / 1023.0f;
    p_SvSteerPerSv->u_Modulo80ms = TRUE; /* RRLP provides GPS Bit Number with 2 bits, so Bit 0-3, max. 80 ms */
    p_SvSteerPerSv->q_Ms = (uint32)p_Acq->gpsBitNumber * 20 + (uint32)p_Acq->intCodePhase;
    p_SvSteerPerSv->f_SvTimeUnc = (float)SvTimeUnc[(uint8)p_Acq->codePhaseSearchWindow];

    /* Move to next element in the list */
    
    if ( u_i < (AcqElement->n - 1) )
      p_Acq++;
      
    p_tm_post_data_payload_aa_data->u_NumSvs++;
    p_SvSteerPerSv++;

  } /* SV for-loop */

}


/*
 ******************************************************************************
 * Function description:
 *  Builds RRLP measurePosResponse's gps_measurement IE.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing the per SV based ASN.1 
 *            decoded RRLP acq. assistance data.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    u_sv_cnt:  number of reportable SVs.
 *    
 *
 ******************************************************************************
*/

  
uint8 tm_rrlp_mprsp_gps_meas_build ( rrlp_MsrPosition_Rsp  *p_mpr, 
                                    const sm_GpsMeasRptStructType *p_prm_data,
                                    uint8 u_list_len,
                                    boolean status_check_needed )
{
  rrlp_GPS_MeasureInfo      *p_measinfo;
  rrlp_GPS_MsrSetElement    *p_msrset;
  rrlp_GPS_MsrElement       *p_element;
  const gnss_MeasStructType *p_gps_meas_per_sv;
  uint8                    u_sv_cnt;
  uint8                    u_i;
  
  float                    f_Num;
  double                   d_AdjSubMsec, d_Num;
  int16                    x_Word;
  
  
  p_measinfo = &p_mpr->gps_MeasureInfo;
  p_msrset = p_measinfo->gpsMsrSetList.elem;
  p_element = p_msrset->gps_msrList.elem;  /* points to the first SV in SUPL RRLP message */

  p_mpr->m.gps_MeasureInfoPresent = TRUE;

  p_msrset->gpsTOW = p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.q_GpsMsec % C_FOUR_HRS_MSEC;

  p_gps_meas_per_sv = &p_prm_data->z_MeasBlk.z_Gnss[0];
  u_sv_cnt = 0;

  for (u_i=0; u_i < u_list_len; u_i++)
  { 
    /*If the status check is needed, check for status bits*/  
    if (  p_gps_meas_per_sv->w_Cno > 0 &&
          (gps_meas_status_bits_check(p_gps_meas_per_sv->q_MeasStatus, status_check_needed) ) 
        )  
    {
      /* this is a good SV to report */

      p_element->satelliteID = p_gps_meas_per_sv->w_Sv - 1; 

      p_element->cNo  = (uint32)( (float) ( p_gps_meas_per_sv->w_Cno + p_prm_data->z_MeasBlk.z_Gnss[u_i].w_RfLoss )  * 0.1);

      /* doppler:
         f_Num = f_SvSpeed * (FLT)C_MsToL1Hz * (FLT)(-1.0); 
         Per Richard, adding frequency bias to calibrate doppler : */
      f_Num = ( p_gps_meas_per_sv->z_FiltMeas.f_SvSpeed  - p_prm_data->z_MeasBlk.z_MeasClock.z_Freq.f_ClkFreqBias )
              * (float)C_MsToL1Hz * (-1.0f);
      x_Word = (int16)(f_Num * 5.0f); /* resolution 0.2 Hz, use *5 for /0.2 */
      p_element->doppler = x_Word;

      /*** We tell location server that the reference point of the measurement is
         at TOW, which is now contained in p_msrset->gpsTOW,
         but the actually GPS moment of the measurement is instead at
         the FCOUNT of the measurement, whose GPS msec is
         p_zMeasGpsTime->q_Msec - p_zMeasGpsTime->f_ClkBias.
         We can adjust each SV's submsec value by adding
         p_zMeasGpsTime->f_ClkBias to it. */

      d_AdjSubMsec = (double)p_gps_meas_per_sv->z_FiltMeas.z_SvTime.f_SubMs
                      + (double)p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.f_ClkTimeBias;

      if (d_AdjSubMsec > 1.0 )
      {
        /* if the adjustment results in the submsec value above one, at the
           reference moment (TOW) SV time's msec should be one msec more,
           thus we need to subtract 1 ms from the submsec variable */

        d_AdjSubMsec -= 1.0;
      }
      else if  ( d_AdjSubMsec < 0 )
      {
        /** or if the adjustment results in submsec value negative, at the
          reference moment (TOW) SV time's msec should be one msec less,
          thus we need to use one minus the resuting "submsec" value for
          the submsec value */

        d_AdjSubMsec = 1.0 + d_AdjSubMsec;
      }

      /* both whole chip and fraction of a chip are based on "code phase", so
         convert submsec to code phase */
      d_AdjSubMsec = 1.0 - d_AdjSubMsec;

      /* whole chips: whole chip representation of the code phase */
      d_Num = d_AdjSubMsec / C_MSEC_PER_CHIP; /* whole chips in msec,
                                                 floating */
      p_element->wholeChips = (uint16)d_Num; /* whole chips in msec, U10 */

      if ( p_element->wholeChips > 1022 )
      {
        MSG_ERROR(  "Error: WholeChip out of range", 0, 0, 0);
      }

      /* fraction of a chip: multiple of 1/1024 chips */
      d_Num = d_AdjSubMsec  - (float)p_element->wholeChips * (FLT)C_MSEC_PER_CHIP;
      p_element->fracChips = (uint16)( d_Num / C_MSEC_FRAC_CHIP ) ;


      /* multipath indicator */
      p_element->mpathIndic = rrlp_notMeasured; /* not used currently */

      /* PseudoRange RMS error: converted from SV time uncertainty */
      f_Num = p_gps_meas_per_sv->z_FiltMeas.f_SvTimeUncMs * LIGHT_MSEC;
      p_element->pseuRangeRMSErr = tm_umts_common_utils_psudoRMS_formater ( f_Num );

      if ( p_element->pseuRangeRMSErr > 62 )
      {
        p_element->pseuRangeRMSErr = 62; /* to avoid being considered as xcorelation by some pdm */
      }

      if( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
      {
        (void) snprintf( tm_f3_msg_buffer, C_TM_F3_MSG_BUF_SIZE, "%2.2d %3.3X %8.6f %2.2d",
                        (int) p_gps_meas_per_sv->w_Sv,
                        (int) p_gps_meas_per_sv->q_MeasStatus,
                        p_gps_meas_per_sv->z_FiltMeas.f_SvTimeUncMs,
                        p_element->pseuRangeRMSErr);
        MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", tm_f3_msg_buffer);
      }

      u_sv_cnt++;  /* reportable SV count */
      
      if ( u_i < ( u_list_len - 1 ) )
      {
        p_element++;
      }  
      

    } /* close-if-valid-SV */

    p_gps_meas_per_sv++;
    
    
  } /* close for-NumSVs-in-measurement-from-LM */
  
  p_msrset->gps_msrList.n =  u_sv_cnt;
  
  return u_sv_cnt;

}




/* ****************************************************************************
 * Function description:
 *  Builds RRLP measurePosResponse's glonass_measurement IE.
 *
 * Parameters:
 *
 *
 * Return value:
 *
 *    
 *
 ******************************************************************************
*/

  
boolean tm_rrlp_mprsp_glonass_meas_build ( rrlp_MsrPosition_Rsp  *p_mpr, 
                                    const sm_GpsMeasRptStructType *p_prm_data,
                                    boolean status_check_needed,
                                    boolean b_gps_meas_included,
                                    rrlp_GANSS_MsrSetElement *p_rrlp_GANSS_MsrSetElement )
{


  uint8  u_TotalGloSvs = 0, u_TotalGoodGloSVs = 0, u_num_sv_to_send = 0;

  float                    f_Num;
  double                   d_AdjSubMsec, d_Num;
  int16                    x_Word;

  uint8  u_i;
  const gnss_MeasStructType    *p_GloMeasPerSv = NULL;
  const gnss_MeasBlkStructType *p_GloMeasRpt = NULL;

  rrlp_GANSS_SgnTypeElement   *p_rrlp_GANSS_SgnTypeElement = NULL;
  rrlp_GANSS_MsrElement       *p_rrlp_GANSS_MsrElement = NULL;
  rrlp_GANSS_SgnElement       *p_rrlp_GANSS_SgnElement = NULL;
  rrlp_GANSS_SgnElement       *p_rrlp_GANSS_SgnElement_First = NULL; /* keeps a copy of the pointer to the first element*/

  boolean b_ret = TRUE;


  p_GloMeasPerSv = &p_prm_data->z_MeasBlk.z_Gnss[0];
  p_GloMeasRpt   = &p_prm_data->z_MeasBlk;

  if (p_GloMeasRpt != NULL)
  {
    /* if this number is ever greater than 14, limit to the first 14 */
    if (p_GloMeasRpt->u_NumSvs < N_ACTIVE_GLO_CHAN)
    {
      u_TotalGloSvs = p_GloMeasRpt->u_NumSvs;
    }
    else
    {
      u_TotalGloSvs = N_ACTIVE_GLO_CHAN;
    }


    /* count to see if there is any non-zero CNo in the list */
    u_TotalGoodGloSVs = 0;
    p_GloMeasPerSv = (gnss_MeasStructType*)&p_prm_data->z_MeasBlk.z_Gnss[0];
    for (u_i = 0; (u_i < u_TotalGloSvs); ++u_i, ++p_GloMeasPerSv)
    {
      if ((p_GloMeasPerSv->w_Cno > 0) &&
          (gnss_meas_status_bits_check(p_GloMeasPerSv->q_MeasStatus, status_check_needed)))
      {
        u_TotalGoodGloSVs++; 
      }
    }

    if (u_TotalGoodGloSVs == 0) 
    {
      return FALSE; /* not a single reportable SV, abort */
    }

    /* these need to be freed in the calling function */
    p_rrlp_GANSS_SgnTypeElement = os_MemAlloc ( sizeof(rrlp_GANSS_SgnTypeElement), OS_MEM_SCOPE_TASK );
    if ( p_rrlp_GANSS_SgnTypeElement != NULL ) 
    {
      memset(p_rrlp_GANSS_SgnTypeElement, 0, sizeof(rrlp_GANSS_SgnTypeElement));
    }
    else
    {
      b_ret = FALSE;
    }

    p_rrlp_GANSS_MsrElement = os_MemAlloc ( sizeof(rrlp_GANSS_MsrElement), OS_MEM_SCOPE_TASK );
    if ( p_rrlp_GANSS_MsrElement != NULL) 
    {
      memset(p_rrlp_GANSS_MsrElement, 0, sizeof(rrlp_GANSS_MsrElement));
    }
    else
    {
      b_ret = FALSE;
    }

    /* actual buffer that contains measurements*/
    p_rrlp_GANSS_SgnElement = os_MemAlloc ( sizeof(rrlp_GANSS_SgnElement) * u_TotalGoodGloSVs, OS_MEM_SCOPE_TASK );
    if ( p_rrlp_GANSS_SgnElement != NULL) 
    {
      memset(p_rrlp_GANSS_SgnElement, 0, sizeof(rrlp_GANSS_SgnElement) * u_TotalGoodGloSVs );
      p_rrlp_GANSS_SgnElement_First = p_rrlp_GANSS_SgnElement;
    }
    else
    {
      b_ret = FALSE;
    }

    /* if any of the allocation fails, free all that has been allocated and exit */ 
    if ( b_ret == FALSE )
    {

      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Allocation failure: %lx, %lx, %lx",
            p_rrlp_GANSS_SgnElement, 
            p_rrlp_GANSS_MsrElement, 
            p_rrlp_GANSS_SgnTypeElement);

      if ( p_rrlp_GANSS_SgnElement != NULL )
      {
        os_MemFree ( (void **) &p_rrlp_GANSS_SgnElement);
      }
      if (p_rrlp_GANSS_MsrElement != NULL) 
      {
        os_MemFree ( (void **) &p_rrlp_GANSS_MsrElement);
      }
      if (p_rrlp_GANSS_SgnTypeElement != NULL) 
      {
        os_MemFree ( (void **) &p_rrlp_GANSS_SgnTypeElement);
      }

      return FALSE; /* no SV to be reported */
    }

    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Total good meas. GLO SVs: %u;  Total SVs  %u (GLO)",
          u_TotalGoodGloSVs, u_TotalGloSvs);


    if ( !b_gps_meas_included ) /* if GPS meas. is not included, populate GNSS meas. time stamp */
    {
      p_rrlp_GANSS_MsrSetElement->m.ganssTODmPresent = TRUE;

      /* Save the valid Glonass TOD: sanity check first */
      if ( ( p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GloTime.u_FourYear == 255 ) ||
           ( p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GloTime.w_Days == 65535 ) ||
           ( p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GloTime.q_GloMsec  > DAY_MSECS )
         )
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, 
              "GlonassTOD invalid: Num4Year=%u, NumDaysIn4Year=%u, ToD_ms=%lu", 
              p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GloTime.u_FourYear,
              p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GloTime.w_Days,
              p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GloTime.q_GloMsec);
        return FALSE;
      }
      else
      {
        p_rrlp_GANSS_MsrSetElement->ganssTODm = 
          p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GloTime.q_GloMsec % HOUR_MSECS;
      }
    } /* if no GPS meas. included */
    

    /* populate measurement block */
    u_num_sv_to_send = 0;
    p_GloMeasPerSv = (gnss_MeasStructType*)&p_prm_data->z_MeasBlk.z_Gnss[0];
    for (u_i = 0; (u_i < u_TotalGloSvs); ++u_i, ++p_GloMeasPerSv)
    {
      if ((p_GloMeasPerSv->w_Cno > 0) &&
          (gnss_meas_status_bits_check(p_GloMeasPerSv->q_MeasStatus, status_check_needed)))
      {

        p_rrlp_GANSS_SgnElement->m.carrierQualityIndPresent =  0;  
        p_rrlp_GANSS_SgnElement->m.integerCodePhasePresent = 0;
        p_rrlp_GANSS_SgnElement->m.dopplerPresent = 1;
        p_rrlp_GANSS_SgnElement->m.adrPresent = 0;

        p_rrlp_GANSS_SgnElement->svID = (uint8)p_GloMeasPerSv->w_Sv - ( GLO_SLOTID_OFFSET + 1 );
        p_rrlp_GANSS_SgnElement->cNo = (uint32)( (float) ( p_GloMeasPerSv->w_Cno + p_prm_data->z_MeasBlk.z_Gnss[u_i].w_RfLoss )  * 0.1);

        /* doppler: provided in m/s f_Num = f_SvSpeed *(FLT)(-1.0); */
        f_Num = (p_GloMeasPerSv->z_FiltMeas.f_SvSpeed  - p_prm_data->z_MeasBlk.z_MeasClock.z_Freq.f_ClkFreqBias);
        x_Word = (int16)(f_Num * 25.0f); /* resolution .04 m/s */
        p_rrlp_GANSS_SgnElement->doppler = x_Word * (-1); 
        /* need to change polarity since protocol definition of doppler is reverse of what ME is using */

        /* code phase: Q: this following uses GPS time. In the cases where no GPS measurements included,
           and GNASS_TOD has to be used, the processing would be different */

        if (  b_gps_meas_included  ) 
        {
          /* We tell location server that the reference point of the measurement is
             at TOW, which is now contained in p_msrset->gpsTOW,
             but the actually GPS moment of the measurement is instead at
             the FCOUNT of the measurement, whose GPS msec is
             p_zMeasGpsTime->q_Msec - p_zMeasGpsTime->f_ClkBias.
             We can adjust each SV's submsec value by adding
             p_zMeasGpsTime->f_ClkBias to it. */

          d_AdjSubMsec = (double)p_GloMeasPerSv->z_FiltMeas.z_SvTime.f_SubMs
             + (double)p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.f_ClkTimeBias;

          if (d_AdjSubMsec > 1.0)
          {
            /* if the adjustment results in the submsec value above one, at the
               reference moment (TOW) SV time's msec should be one msec more,
               thus we need to subtract 1 ms from the submsec variable */

            d_AdjSubMsec -= 1.0;
          }
          else if (d_AdjSubMsec < 0)
          {
            /** or if the adjustment results in submsec value negative, at the
              reference moment (TOW) SV time's msec should be one msec less,
              thus we need to use one minus the resuting "submsec" value for
              the submsec value */

            d_AdjSubMsec = 1.0 + d_AdjSubMsec;
          }

          /* both whole chip and fraction of a chip are based on "code phase", so
             convert submsec to code phase */
          d_AdjSubMsec = 1.0 - d_AdjSubMsec;
          /* whole chips: whole chip representation of the code phase */
          d_Num = d_AdjSubMsec * 2097152.0;
          /* whole chips in msec, floating */
          p_rrlp_GANSS_SgnElement->codePhase = (uint32)d_Num; /* whole chips in msec, U10 */

        } /* if GPS measurement included */
        else  /* no GPS measurement included */
        {
          MSG_HIGH("GPS meas. absent, GNSS code phase assigned from FC to GLONASS TOD", 0, 0, 0);


          /* We tell location server that the reference point of the measurement is
             at TOW, which is now contained in rrlp_GANSS_MsrSetElement->ganssTODm,
             but the actually moment of the measurement applicable is instead at
             the FCOUNT of the measurement, whose GLONASS TOD msec is
             z_GloTime->q_Msec - z_GloTime->f_ClkBias.
             We can adjust each SV's submsec value by adding
             z_GloTime->f_ClkBias to it. */

          d_AdjSubMsec = (double)p_GloMeasPerSv->z_FiltMeas.z_SvTime.f_SubMs
             + (double)p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GloTime.f_ClkTimeBias;

          if (d_AdjSubMsec > 1.0)
          {
            /* if the adjustment results in the submsec value above one, at the
               reference moment (TOW) SV time's msec should be one msec more,
               thus we need to subtract 1 ms from the submsec variable */

            d_AdjSubMsec -= 1.0;
          }
          else if (d_AdjSubMsec < 0)
          {
            /** or if the adjustment results in submsec value negative, at the
              reference moment (TOW) SV time's msec should be one msec less,
              thus we need to use one minus the resuting "submsec" value for
              the submsec value */

            d_AdjSubMsec = 1.0 + d_AdjSubMsec;
          }

          /* both whole chip and fraction of a chip are based on "code phase", so
             convert submsec to code phase */
          d_AdjSubMsec = 1.0 - d_AdjSubMsec;
          /* whole chips: whole chip representation of the code phase */
          d_Num = d_AdjSubMsec * 2097152.0;
          /* whole chips in msec, floating */
          p_rrlp_GANSS_SgnElement->codePhase = (uint32)d_Num; /* whole chips in msec, U10 */

        } /* if no GPS measurement included */

        if ( p_GloMeasPerSv->u_MultipathEstValid == FALSE )
        {
          p_rrlp_GANSS_SgnElement->mpathDet = 0; 
        }  
        else if ( p_GloMeasPerSv->f_MultipathEst < 5.0f )
        {
          p_rrlp_GANSS_SgnElement->mpathDet = 1;
        }
        else if ( p_GloMeasPerSv->f_MultipathEst < 43.0f )     
        {
          p_rrlp_GANSS_SgnElement->mpathDet = 2;
        }
        else
        {
          p_rrlp_GANSS_SgnElement->mpathDet = 2;
        }

        /* PseudoRange RMS error: converted from SV time uncertainty */
        f_Num = (float)(p_GloMeasPerSv->z_FiltMeas.f_SvTimeUncMs * (FLT)LIGHT_MSEC);
        p_rrlp_GANSS_SgnElement->codePhaseRMSError = tm_umts_common_utils_psudoRMS_formater(f_Num);

        if (p_rrlp_GANSS_SgnElement->codePhaseRMSError > 62)
        {
          p_rrlp_GANSS_SgnElement->codePhaseRMSError = 62; /* to avoid being considered as xcorelation by some pdm */
        }

        p_rrlp_GANSS_SgnElement++;
        u_num_sv_to_send++;

        if ( u_num_sv_to_send == u_TotalGoodGloSVs ) 
        {
          break; 
        }
      }
    }

  } /* Glonass Meas Report end */


  if ( u_num_sv_to_send ) 
  {
     
    p_rrlp_GANSS_SgnTypeElement->ganssSignalID = GANSS_SIGNAL_ID_GLONASS_G1;
    p_rrlp_GANSS_SgnTypeElement->ganss_SgnList.n = u_num_sv_to_send; 
    p_rrlp_GANSS_SgnTypeElement->ganss_SgnList.elem = p_rrlp_GANSS_SgnElement_First;  

    p_rrlp_GANSS_MsrElement->m.ganssIDPresent = 1;
    p_rrlp_GANSS_MsrElement->ganssID = GANSS_ID_3_GLONASS; 
    p_rrlp_GANSS_MsrElement->ganss_SgnTypeList.n = 1;  
    p_rrlp_GANSS_MsrElement->ganss_SgnTypeList.elem = p_rrlp_GANSS_SgnTypeElement;

    p_rrlp_GANSS_MsrSetElement->ganss_MsrElementList.n = 1;
    p_rrlp_GANSS_MsrSetElement->ganss_MsrElementList.elem = p_rrlp_GANSS_MsrElement;

    p_mpr->m.rel_7_MsrPosition_Rsp_ExtensionPresent = TRUE;   
    p_mpr->rel_7_MsrPosition_Rsp_Extension.m.ganssMeasureInfoPresent = TRUE;
    p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssMeasureInfo.ganssMsrSetList.n = 1;
    p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssMeasureInfo.ganssMsrSetList.elem = p_rrlp_GANSS_MsrSetElement;

    return TRUE;
  }
  else
  {
    MSG_HIGH("No qualified SV in GLO meas. block, No GANSS meas. sent", 0, 0, 0);
    return FALSE;   
  }
}

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRLP Glonass reference time IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing ASN.1 decoded RRLP message.
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

void tm_rrlp_glo_reftime_to_TmCore_post_data_payload
( rrlp_GANSS_ControlHeader *p_assist, 
  tm_post_data_payload_type    *p_tm_post_data_payload 
)
{

  glo_RefTimeStructType *p_tm_post_data_payload_glo_reftime = 
    &p_tm_post_data_payload->reftime_glo_data;

  rrlp_GANSSRefTimeInfo *p_glo_ref_time = 
   &p_assist->ganssCommonAssistData.ganssReferenceTime.ganssRefTimeInfo;

  /* check reference time GANSS ID. Absence of ganssTimeID indicates Galieo ****/
  if ( !(p_glo_ref_time->m.ganssTimeIDPresent == 1) ||
        ( p_glo_ref_time->ganssTimeID != RRLP_GANSS_TIME_ID_GLONASS )    
     )
  {
    unsigned  temp_m;
    temp_m = (p_assist->ganssCommonAssistData.ganssReferenceTime.ganssRefTimeInfo.m.ganssDayPresent << 31)
      | (p_assist->ganssCommonAssistData.ganssReferenceTime.ganssRefTimeInfo.m.ganssTODUncertaintyPresent << 30) 
      | (p_assist->ganssCommonAssistData.ganssReferenceTime.ganssRefTimeInfo.m.ganssTimeIDPresent << 29 );
      
    MSG_HIGH("Unsupported GANSS_TIME info: bitmask=%x, ID=%u",
              temp_m,
              p_assist->ganssCommonAssistData.ganssReferenceTime.ganssRefTimeInfo.ganssTimeID,
              0 );
    return;
  }

  /*** the optional Glonass Day info. ***********/
  if (  p_glo_ref_time->m.ganssDayPresent == 1 )
  {
    /*  The RRLP provided number of days is divided into 4 year cycle number 
        and calendar day. */
    p_tm_post_data_payload_glo_reftime->u_GloFourYear = 
      (uint8)(p_glo_ref_time->ganssDay / FOURYEAR_DAYS + 1);

    p_tm_post_data_payload_glo_reftime->w_GloDays =  
      (uint16)( p_glo_ref_time->ganssDay 
                - ( p_tm_post_data_payload_glo_reftime->u_GloFourYear - 1 ) * FOURYEAR_DAYS + 1
              );
  }
  else 
  {
    p_tm_post_data_payload_glo_reftime->u_GloFourYear = C_GLO_FOUR_YEAR_UNKNOWN;
    p_tm_post_data_payload_glo_reftime->w_GloDays =     C_GLO_DAY_UNKNOWN;
  }

  p_tm_post_data_payload_glo_reftime->q_GloMsec = p_glo_ref_time->ganssTOD * 1000;

  #if 0 /* TOD unc. is of no use to MGP as we don't apply the association of GANSS time 
           to GPS time */
  if (  p_glo_ref_time->m.ganssTODUncertaintyPresent )
  #endif

  if ( ( z_umts_nv_item_ram_copy.aagps_default_ref_time_unc_50ms * 50 ) >= 1000 )  
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
 * Function description:
 *  Convert the ASN.1 decoded RRLP Glonass navigation model IE to the format 
 *  ready for TM core post data message.
 *
 * Parameters:
 *
 *  prot_glo_eph_buffer_s_type: pointer to the structure Glonass eph plus
 *  frequency number for the SVs involved.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *
 ******************************************************************************
*/

void tm_rrlp_glo_eph_to_TmCore_post_data_payload 
( 
  prot_glo_eph_buffer_s_type     *p_glo_eph_sat_element,
  tm_post_data_payload_type      *p_tm_post_data_payload
)
{

  memscpy( &p_tm_post_data_payload->eph_glo_data, sizeof(p_tm_post_data_payload->eph_glo_data),
          &p_glo_eph_sat_element->z_eph_glo_data, sizeof(p_glo_eph_sat_element->z_eph_glo_data));

  return;
}

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRLP Glonass almanac IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_Almanac: pointer to the structure containing the per SV based ASN.1 
 *             decoded RRLP almanac data.
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

void tm_rrlp_glo_alm_to_TmCore_post_data_payload
( rrlp_Almanac_GlonassAlmanacSet *p_glo_alm_set, 
  tm_post_data_payload_type      *p_tm_post_data_payload
)
{

  p_tm_post_data_payload->alm_glo_data.z_Mask.z_FilledMask.u_N4Flag = FALSE;

  /* 22-bit LSBs, Delat_T_n_A: 
     RRLP : (-2097152..2097151); API expects IDC format of 22-bit sign mag. 
  */
  p_tm_post_data_payload->alm_glo_data.q_DracCorrect = 
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_glo_alm_set->gloAkmDeltaTA, 0x00200000 );

  /* when injecting, this is ignored; and RRLP does not provide it either */
  /* p_tm_post_data_payload->alm_glo_data.q_FullToa */

  /* 18-bit LSBs, Delta_i_n_A:
     RRLP : (-131072..131071); API expects IDC format of 18-bit sign mag. */
  p_tm_post_data_payload->alm_glo_data.q_InclCorrect =
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_glo_alm_set->gloAlmDeltaIa, 0x00020000 );

  /* 21-bit LSBs, Lamda_nA:
     RRLP: (-1048576..1048575); API expects IDC format of 21-bit sign mag.  */
  p_tm_post_data_payload->alm_glo_data.q_LongAscend =
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_glo_alm_set->gloAlmLambdaA, 0x00100000 );

  /* 21-bit LSB, t_Lamda_n_A
     RRLP: (0..2097151) */
  p_tm_post_data_payload->alm_glo_data.q_TimeFirstAscend = 
    p_glo_alm_set->gloAlmtlambdaA;

  /* 7-bit LSBs, Delta_TDot_n_A:
     RRLP: (-64..63); API expects IDC format of 7-bit sign mag. */
  p_tm_post_data_payload->alm_glo_data.u_DractRate = 
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_glo_alm_set->gloAlmDeltaTdotA, 0x00000040 );

  /* TM API uses 65-88 for Glonass SVs, 
     n_A is SV_ID :  gloAlmnA RRLP: INTEGER (1..24)
     GLO_SLOTID_OFFSET = 64    */
  p_tm_post_data_payload->alm_glo_data.u_SvId = p_glo_alm_set->gloAlmnA + GLO_SLOTID_OFFSET;

  /* HnA : Carrier frequency number 5-bit LSBs
     RRLP: (0..31) */
  p_tm_post_data_payload->alm_glo_data.u_FreqNum =
    p_glo_alm_set->gloAlmHA;

  /* 1 bit, CnA  */
  p_tm_post_data_payload->alm_glo_data.u_Health =
    p_glo_alm_set->gloAlmCA;  

  /* Number of 4-year cyclel since 1996/1/1, not available from RRLP for alm. */
  p_tm_post_data_payload->alm_glo_data.z_Mask.z_FilledMask.u_N4Flag = FALSE;
  /*  p_tm_post_data_payload->alm_glo_data.u_N4 = n.a. */


  /* Mn: OSS_BIT string of 2 bits */
  if ( p_glo_alm_set->m.gloAlmMAPresent == 1 )
  {
    p_tm_post_data_payload->alm_glo_data.z_Mask.z_FilledMask.u_SvTypeFlag = TRUE;

    p_tm_post_data_payload->alm_glo_data.u_SvType =
      *(p_glo_alm_set->gloAlmMA.data);
    p_tm_post_data_payload->alm_glo_data.u_SvType >>= 6; 
  }

  /* Number of days in a 4-year cycle; RRLP ASN1 INTEGER (1..1461)*/
  p_tm_post_data_payload->alm_glo_data.w_Na =
    p_glo_alm_set->gloAlmNA;  

  /* 15-bit LSBs, Epsilon_n_A
     RRLP: (0..32767) */
  p_tm_post_data_payload->alm_glo_data.w_OrbitEcc =
    p_glo_alm_set->gloAlmEpsilonA;  

  /* 16-bit LSBs, Omega_n_A:
     RRLP: (-32768..32767); API expects IDC format of 16-bit sign mag. */
  p_tm_post_data_payload->alm_glo_data.w_Perigee =
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_glo_alm_set->gloAlmOmegaA, 0x00008000 );

  /* 10-bit LSBs, TauN:
     RRLP: (-512..511); API expects IDC format of 10-bit sign mag.    */
  p_tm_post_data_payload->alm_glo_data.w_TauN = 
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_glo_alm_set->gloAlmTauA, 0x00000200 );

}

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRLP Glonass reference time IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing ASN.1 decoded RRLP message.
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

void tm_rrlp_glo_addutc_to_TmCore_post_data_payload
( rrlp_GANSS_ControlHeader *p_assist, 
  tm_post_data_payload_type    *p_tm_post_data_payload 
)
{

  /* tauC is time diff between UTC and GLONASS. Do not need update. 
     Assume UTC to GPS offset is injected to MGP. 
  */
  p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_N4Flag = FALSE;
  p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_TaoGpsFlag = FALSE;
  p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_FreqNumFlag = FALSE;
  p_tm_post_data_payload->utc_model_glo_data.u_Src = C_GLOUTC_SRC_ASSIST;

  #if 0
  /* The TM APi has the following : */
  p_tm_post_data_payload->utc_model_glo_data.q_TaoGps;
  /* the RRLP has the follwing: not the same */
  p_assist->ganssGenericAssistDataList.elem->ganssAddUTCModel.u.utcModel3->tauC
  #endif

  p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_NtFlag = TRUE;
  p_tm_post_data_payload->utc_model_glo_data.w_Nt = 
   p_assist->ganssGenericAssistDataList.elem->ganssAddUTCModel.u.utcModel3->nA; 

  /* B1 and B2 are OPTIONAL. Do not use */
  #if 0 /* TM API has no B1 or B2 */
  if ( p_assist->ganssGenericAssistDataList.elem->ganssAddUTCModel.u.utcModel3->m.b1Present == 1 )
  {
    ?? = p_assist->ganssGenericAssistDataList.elem->ganssAddUTCModel.u.utcModel3->b1
  }
  if ( p_assist->ganssGenericAssistDataList.elem->ganssAddUTCModel.u.utcModel3->m.b2Present == 1 )
  {      
    ?? = p_assist->ganssGenericAssistDataList.elem->ganssAddUTCModel.u.utcModel3->b2
  }
  #endif

  if ( p_assist->ganssGenericAssistDataList.elem->ganssAddUTCModel.u.utcModel3->m.kpPresent == 1 )
  {
    p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_KpFlag = TRUE;
    p_tm_post_data_payload->utc_model_glo_data.u_Kp = 
      *(p_assist->ganssGenericAssistDataList.elem->ganssAddUTCModel.u.utcModel3->kp.data);
    p_tm_post_data_payload->utc_model_glo_data.u_Kp >>= 6;
  }
  else
  {
    p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_KpFlag = FALSE;
  }

}



/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded RRLP acquistion assistance IE's steering data to 
 *  the format ready for TM core post data message.
 *
 * Parameters:
 *
 *  ganssRefMeasAssistList_ptr: pointer to the structure containing the per SV
 *   based ASN.1 decoded RRLP acq. assistance data.
 *
 *  prot_glo_acqasst_buffer_ptr: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/
void tm_rrlp_glo_acq_assist_to_tmcore_post_data_payload
(
  rrlp_SeqOfGANSSRefMeasurementElement *ganssRefMeasAssistList_ptr,
  prot_glo_acqast_buffer_s_type        *prot_glo_acqasst_buffer_ptr,
  uint32 q_GloMsec
)
{
  uint8                             u_i = 0;
  gnss_SvSteerStructType           *p_prot_glo_sv_steering_persv = NULL;
  rrlp_GANSSRefMeasurementElement  *p_gnss_acq_assist_elem = NULL;
  gnss_SvAcqAssistCommonStructType *p_prot_glo_aa_steering = NULL;
  gnss_SvDirStructType             *p_prot_glo_aa_sv_dir = NULL;
  gnss_AzElInfoStructType          *p_prot_glo_sv_dir_azim_elev = NULL;
  uint32                            u_num_acq_assist = 0;

  if ( ( NULL == prot_glo_acqasst_buffer_ptr ) || ( NULL == ganssRefMeasAssistList_ptr ) )
  {
    MSG_ERROR("Invalid pointer: %lx, %lx", prot_glo_acqasst_buffer_ptr, ganssRefMeasAssistList_ptr, 0);
    return;
  }

  MSG_HIGH("Preparing AGLO-AcqAssistData for MGP injection", 0, 0, 0); 

  // Steering
  memset( &prot_glo_acqasst_buffer_ptr->z_steering_glo_data, 0, sizeof(prot_glo_acqasst_buffer_ptr->z_steering_glo_data) );
  p_prot_glo_aa_steering = &prot_glo_acqasst_buffer_ptr->z_steering_glo_data;
  p_prot_glo_sv_steering_persv = &prot_glo_acqasst_buffer_ptr->z_steering_glo_data.z_SvSteerInfo[0];
  p_prot_glo_aa_steering->u_NumSvs = 0;
  prot_glo_acqasst_buffer_ptr->z_steering_glo_data.q_SvSteerSrc = (uint32)MGP_GNSS_SV_STEER_SRC_GLO;

  // SV Direction
  memset( &prot_glo_acqasst_buffer_ptr->gnss_sv_dir, 0, sizeof(prot_glo_acqasst_buffer_ptr->gnss_sv_dir) );
  p_prot_glo_aa_sv_dir = &prot_glo_acqasst_buffer_ptr->gnss_sv_dir;
  p_prot_glo_sv_dir_azim_elev = &p_prot_glo_aa_sv_dir->z_SvAzElInfo[0];
  p_prot_glo_aa_sv_dir->u_NumSvs = 0;
  prot_glo_acqasst_buffer_ptr->gnss_sv_dir.q_GnssType = (uint32)MGP_GNSS_TYPE_GLO;

  // Source structure
  p_gnss_acq_assist_elem = ganssRefMeasAssistList_ptr->elem;

  MSG_MED("Build %lu Glonass AA SVDirection & Steering info", ganssRefMeasAssistList_ptr->n, 0, 0);

  /* ME can't eat more than it can digest */
  if ( ganssRefMeasAssistList_ptr->n > N_GLO_FREQ )
  {
    MSG_ERROR("TM_LPP_UP: Can't handle AA data of %u elements. Max %d can be consumed.", 
              ganssRefMeasAssistList_ptr->n, N_GLO_FREQ, 0);
    u_num_acq_assist = N_GLO_FREQ;
  }
  else
  {
    u_num_acq_assist = ganssRefMeasAssistList_ptr->n;
  }

 
  /* AA ganssRefMeasAssistList_ptr->n (1..64) */
  for ( u_i=0; u_i<u_num_acq_assist; ++u_i, ++p_gnss_acq_assist_elem )
  {
    // svID (0..63). 0..23 valid Glonass sv ids. 24..63 reserved.
    if ( p_gnss_acq_assist_elem->svID < N_GLO_SV )
    {
      //
      // AA-Steering
      //
      // local w_Sv (65..88)
      p_prot_glo_sv_steering_persv->w_Sv = (uint8)p_gnss_acq_assist_elem->svID + GLO_SLOTID_OFFSET + 1;

      /* RRLP AGNSS Reference Measurement oth order Doppler: unit is 0.5 m/s */
      /* need to flip the poliarity as ME is using a different convention for range-rate */
      p_prot_glo_sv_steering_persv->f_SvSpeed = (float)p_gnss_acq_assist_elem->doppler0  * (FLT)0.5 * (FLT)(-1.0); 


      /* RRLP AGNSS Reference Measurement oth order Doppler: scale unit is 1/210 m/s2*/
      if ( p_gnss_acq_assist_elem->m.additionalDopplerPresent )
      {

        p_prot_glo_sv_steering_persv->u_SvAccelValid = TRUE;
        p_prot_glo_sv_steering_persv->u_SvSpeedUncValid = TRUE;

        p_prot_glo_sv_steering_persv->f_SvAccel = 
          ((float)(p_gnss_acq_assist_elem->additionalDoppler.doppler1 - 42.0) * (FLT)0.00476190 * (FLT)(-1.0));

        p_prot_glo_sv_steering_persv->f_SvSpeedUnc = 
          (float)agnss_doppler_unc_to_meters_per_sec[(uint8) p_gnss_acq_assist_elem->additionalDoppler.dopplerUncertainty];

      }


      /* Code phase in AGNSS in different from that in RRLP GPS, no need to subtract from 1*/
      p_prot_glo_sv_steering_persv->f_SubMsec = (float)(p_gnss_acq_assist_elem->codePhase) / 1024.0f;

      if ( q_GloMsec < p_gnss_acq_assist_elem->intCodePhase )
      {
        p_prot_glo_sv_steering_persv->q_Ms = (uint32)((q_GloMsec + DAY_MSECS - p_gnss_acq_assist_elem->intCodePhase)% 80);
      }
      else
      {
        p_prot_glo_sv_steering_persv->q_Ms = (uint32)((q_GloMsec  - p_gnss_acq_assist_elem->intCodePhase)% 80);
      }

      p_prot_glo_sv_steering_persv->u_Modulo80ms = TRUE; 
      p_prot_glo_sv_steering_persv->f_SvTimeUnc = (float)gnss_SvTimeUnc[(uint8)p_gnss_acq_assist_elem->codePhaseSearchWindow];
      p_prot_glo_aa_steering->u_NumSvs++;
      ++p_prot_glo_sv_steering_persv; // increment steering dest elem

      //
      // AA-SV Direction
      //
      // local w_Sv (65..88) 
      if ( p_gnss_acq_assist_elem->m.additionalAnglePresent )
      {
        p_prot_glo_sv_dir_azim_elev->w_Sv = (uint8)p_gnss_acq_assist_elem->svID + GLO_SLOTID_OFFSET + 1; 
        p_prot_glo_sv_dir_azim_elev->f_Azimuth = (float)p_gnss_acq_assist_elem->additionalAngle.azimuth * (FLT)C_RRLP_RRC_AZIMUTH_TO_RAD;
        p_prot_glo_sv_dir_azim_elev->f_Elevation = (float)p_gnss_acq_assist_elem->additionalAngle.elevation * (FLT)C_RRLP_RRC_ELEV_TO_RAD;
        p_prot_glo_sv_dir_azim_elev->u_ElevUncValid = FALSE;

        // Glonass Frequency/Channel number field (b_GloFreqIndex) is filled by AuxiInfo message 
        p_prot_glo_aa_sv_dir->u_NumSvs++;    
        ++p_prot_glo_sv_dir_azim_elev; // increment svdir dest elem
      }

    }
    else
    {
      MSG_HIGH("Glonass AA SvId %u invalid", p_gnss_acq_assist_elem->svID, 0, 0);
    }

  } /* SV for-loop */


}

#endif /* ifdef FEATURE_SUPPORT_UMTS_GPS_PROTOCOLS */
