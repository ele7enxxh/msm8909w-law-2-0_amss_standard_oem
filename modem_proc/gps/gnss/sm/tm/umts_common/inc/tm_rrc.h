#ifndef _TM_RRC_H
#define _TM_RRC_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  TM RRC Utility

GENERAL DESCRIPTION
  This file contains TM's RRC protocol utilities, which convert decoded RRC
  messages for the consumption of TmCore.

  Copyright (c) 2007 - 2012 Qualcomm Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_common/inc/tm_rrc.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  10/16/13    rk      Added RRC Glonass MSA helper functions.
  09/30/13    rk      Removed reference to deprecated NV item 3649 & use RRC API to get
                             the selected RRC protocol release major version.
  04/21/13     rk     Added RRC R8 GANSS Assistance data processing interfaces.
  04/21/13     rk     Modified tm_rrc_refpos_to_TmCore_post_data_payload interface to process
                      reference location from GPS & GANSS assistance data.
  05/30/07     LT     Initial check in.

===========================================================================*/


/*=============================================================================
                                INCLUDE FILES
=============================================================================*/
#include "rrclsmif.h"
#include "gnss_common.h"
#include "tm_data.h"
#include "tm_umts_common_utils.h"

/*=============================================================================

       Constants

=============================================================================*/

/* RRC periodicReportingInterval to maxFixTimeout mapping:

   assuming that the real number encoded in ASN.1 for this field are
   ( 0,        1,   2, 3, 4, 5, 6, 7, 8,  9, 10, 11, 12, 13, 14, 15) and they correspond to
   ( undef, 0.25, 0.5, 1, 2, 3, 4, 6, 8, 12, 16, 20, 24, 28, 32, 64 ) seconds.
   and also that the OSS decoder will only provide ( 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14, 15)
   because the structure defined in RRC in the UE is a uint8.

   also, since 0.25 and 0.5 make no sense to maxFixTimeout, or to the "real" periodic
   reporting interval for this UE which cannot produce fixes faster than every one
   second, 0.25 and 0.5 seconds are mapped to 1 second

   0 is not mapped here, as it is defined as "undefined" and we use LsmConfig default.

*/

static const uint16 w_RrcPeriodIntervalToMaxFixTimeout[15] =
{ 1, 1, 1, 2, 3, 4, 6, 8, 12, 16, 20, 24, 28, 32, 64 };

static const uint16 w_RrcPeriodReportAmount[8] =
{ 1, 2, 4, 8, 16, 32, 64, 65535 }; /* note 65535 to indicate infinity */



/*=============================================================================

       Macros

=============================================================================*/

#define C_RRC_MAX_SAT  16 /* RRC defined maxSat */

/* MSB (leftmost) Bit 0 – OTDOA, Bit 1 – GPS, Bit 6 – GLONASS in BIT STRING (SIZE (16)) */
#define RRC_GANSS_POSITION_DATA_GPS_BMASK     0x40
#define RRC_GANSS_POSITION_DATA_GLONASS_BMASK 0x02

/* G1 Glonass signal ID */
#define RRC_GANSS_GLONASS_SIGNAL_ID_G1_BMASK  0x80
/* Glonass time model bits */
#define RRC_GANSS_TIMEMODEL_GPS_BMASK     0x80

/* Supports G1 GLONASS GANSS signal Id */
#define RRC_GANSS_GLONASS_SIGNAL_ID_G1  0

/* Def. RRC release major version number returned by rrc_return_wcdma_rrc_version() API */
#define GNSS_RRC_RELEASE_VERSION_7  7   /* RRC release version 7 */

/*=============================================================================

       Typedefs

=============================================================================*/


/*=============================================================================

       Function Declarations

=============================================================================*/

void tm_rrc_reftime_to_TmCore_post_data_payload( rrc_assistance_data_struct_type* p_assist, 
                                                 tm_post_data_payload_type    *p_tm_post_data_payload
                                                );

boolean tm_rrc_refpos_to_TmCore_post_data_payload( rrc_gps_reference_loc_struct_type const * const p_assist_gps_ref_loc, 
                                               tm_post_data_payload_type *p_tm_post_data_payload, 
                                               RefPos_GAD *p_RefPosGADSave, 
                                               boolean const b_GpsRefTimeIncl, 
                                               uint16 const w_GpsRefTimeWeek, 
                                               uint32 const q_GpsRefTimeTowMs );

void tm_rrc_eph_to_TmCore_post_data_payload( rrc_ephermeral_clock_struct_type *p_ephermeral_clock_params,
                                             tm_post_data_payload_type      *p_tm_post_data_payload,
                                             uint16 w_week
                                            );

void tm_rrc_Iono_to_TmCore_post_data_payload( rrc_gps_iono_struct_type const *const p_gps_iono,
                                              tm_post_data_payload_type   *p_tm_post_data_payload );


void tm_rrc_UtcModel_to_TmCore_post_data_payload( rrc_assistance_data_struct_type *p_assist, 
                                                  tm_post_data_payload_type   *p_tm_post_data_payload );

void tm_rrc_alm_to_TmCore_post_data_payload( rrc_almanac_sat_info_struct_type  *p_sat_info_list,
                                             tm_post_data_payload_type        *p_tm_post_data_payload
                                            );

void tm_rrc_AcqAssistSvDir_to_TmCore_post_data_payload( rrc_assistance_data_struct_type *p_assist, 
                                                        tm_post_data_payload_type *p_tm_post_data_payload
                                                       );

void tm_rrc_AcqAssistSteering_to_TmCore_post_data_payload( rrc_assistance_data_struct_type *p_assist,
                                                           tm_post_data_payload_type *p_tm_post_data_payload,
                                                           uint16 w_gps_week
                                                           );
#ifdef FEATURE_GANSS_SUPPORT

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
void tm_rrc_glo_eph_to_common_prot_struct( rrc_sat_info_nav_list_type const *const pz_NavSatelliteInfo,
                                           glo_EphStructType *pz_ProtGloEphToPost );


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
                                                   uint8 const u_AuxiInfoCnt,
                                                   prot_glo_eph_buffer_s_type p_GlonasEphFreqBuf[N_GLO_SV] );

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
                                       rrc_velocity_estimate_type_info *p_velocity );

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
void tm_rrc_glo_reftime_to_tm_post_data_payload( rrc_ganss_reference_time_struct_type const *const p_assist,
                                                 tm_post_data_payload_type *p_tm_post_data_payload );


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
                                                     tm_post_data_payload_type *p_tm_post_data_payload );
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
                                             glo_AlmStructType *p_TmPostDataGloAlmanac );

/*
 ******************************************************************************
 *  tm_rrc_glo_acq_asst_to_tm_post_data_payload
 *
 * Function description:
 *    Translate RRC Acquisition assistance data to TM post structure.
 *
 * Parameters:
 *  p_GloAcqAsst: pointer to the structure containing the per SV RRC acquisition assistance data.
 *  p_TmPostDataGloSvAcqAsst: pointer to TmCore Glo SV acquisition assistance post-data structure.
 *  p_TmPostDataGloSVDir: pointer to TmCore Glo SV direction post-data structure.
 *
 * Return value:
 *    None.
 *
 ******************************************************************************
*/

void tm_rrc_glo_acq_asst_to_tm_post_data_payload( const rrc_GANSS_Ref_meas_Info_type *const p_GloAcqAsst,
                                                  gnss_SvAcqAssistCommonStructType *p_TmPostDataGloSvAcqAsst,
                                                  gnss_SvDirStructType *p_TmPostDataGloSVDir );

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
                                                    uint32 const q_GloMsecInDay, prot_glo_acqasst_sat_buffer_s_type *p_GloAcqAsstSubParams );

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
void tm_rrc_map_glo_acqast_svid_to_freq_num_from_aux( rrc_Auxiliary_ganssID3_type const p_GanssSvAuxiInfo[MAX_GANSS_SAT],
                                                      uint8 u_AuxiInfoCnt,
                                                      prot_glo_acqast_buffer_s_type *p_GloAcqAsstFreqBuf,
                                                      prot_glo_svcn_buffer_s_type *p_GloAuxiInfoBuf );

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
uint8 tm_rrc_utils_microsec_to_gadk( float f_time_unc_microseconds );

#endif /* FEATURE_GANSS_SUPPORT */

#endif /* _TM_RRC_H */
