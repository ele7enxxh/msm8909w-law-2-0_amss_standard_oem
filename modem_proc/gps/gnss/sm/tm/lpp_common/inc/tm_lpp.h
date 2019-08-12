#ifndef _TM_LPP_H
#define _TM_LPP_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         TM LPP Utility

GENERAL DESCRIPTION
  This file contains TM's LPP protocol utilities, which convert decoded LPP
  messages for the consumption of TmCore.  

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/lpp_common/inc/tm_lpp.h#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  10/1/15   rk       Added LPP AGNSS position method bits to correctly set A-GNSS modes in LPP ProvCap.
  08/16/15   rk      Added common function to send a request to TMCore to stop the OTDOA engine.
  01/14/15   rk      Added LPP RSTD >24 measurement processing support
  01/09/15   rk      Fix the GLONASS SV TimeUnc value for zero-th index.
  06/04/14   rk      Upgraded lpp to release 10.C.0 & two gnss acqassit function to take confidence.
  12/13/11   gk      Lint and other fixes
  04/11/11   gk      Initial version

===========================================================================*/

#include "tm_lpp_asn1.h"
#include "tm_lpp_tx_mgr.h"
#include "msg.h"

#define TM_LPP_NUM_MSEC_IN_DAY (3600 *24 * 1000)

#define LPP_CURRENT_VERSION_MAJOR 11 /* current ASN1's major */
#define LPP_CURRENT_VERSION_TECH 6  /* current ASN1's technical */
#define LPP_CURRENT_VERSION_EDIT 0  /* current ASN1's editorial */
/**************************************************************/
/*                                                            */
/*  LPP_MessageBody_c1                                        */
/*                                                            */
/**************************************************************/

/* Choice tag constants */

#define LPP_MESSAGEBODY_C1 T_LPP_MessageBody_c1
#define LPP_MESSAGE_REQ_CAP T_LPP_MessageBody_c1_requestCapabilities 
#define LPP_MESSAGE_PROV_CAP T_LPP_MessageBody_c1_provideCapabilities 
#define LPP_MESSAGE_REQ_AD T_LPP_MessageBody_c1_requestAssistanceData 
#define LPP_MESSAGE_PROV_AD T_LPP_MessageBody_c1_provideAssistanceData
#define LPP_MESSAGE_REQ_LOCINFO T_LPP_MessageBody_c1_requestLocationInformation
#define LPP_MESSAGE_PROV_LOC_INFO T_LPP_MessageBody_c1_provideLocationInformation
#define LPP_MESSAGE_ABORT T_LPP_MessageBody_c1_abort_
#define LPP_MESSAGE_ERROR T_LPP_MessageBody_c1_error


#define LPP_PROV_CAP_C1 T_ProvideCapabilities_criticalExtensions_c1

#define LPP_PROV_CAP_C1_PROV_CAP_R9 T_ProvideCapabilities_criticalExtensions_c1_provideCapabilities_r9

#define LPP_POS_MODE_STANDALONE         0x01  /* A-GNSS (GPS or GNSS) standalone */
#define LPP_POS_MODE_UE_BASED           0x02  /* A-GPS UE Based */
#define LPP_POS_MODE_UE_ASSISTED        0x04  /* A-GPS UE Assisted */
#define LPP_AGNSS_POS_MODE_UE_BASED     0x08  /* A-GNSS UE Based */  
#define LPP_AGNSS_POS_MODE_UE_ASSISTED  0x10  /* A-GNSS UE Assisted */

#define LPP_POS_MODE_OTDOA_ONLY  1

#define LPP_PROV_OTDOA_CAP OTDOA_ProvideCapabilities_otdoa_Mode_ue_assisted_1
#define LPP_GNSS_ID_BITMAP_GPS (GNSS_ID_Bitmap_gnss_ids_gps)
#define LPP_GNSS_ID_BITMAP_GLONASS (GNSS_ID_Bitmap_gnss_ids_glonass)
#define LPP_IONO_MODEL (GNSS_IonosphericModelSupport_ionoModel_klobuchar)
/* G1 Glonass signal ID bit mask to set the bit to 1 */
#define LPP_GNSS_SIGNAL_ID_GPS_L1CA_BMASK     0x80
#define LPP_GNSS_SIGNAL_ID_GLONASS_G1_BMASK   0x80  
/* G1 Glonass signal ID */
#define LPP_GNSS_SIGNAL_ID_GPS_L1CA         0x0
#define LPP_GNSS_SIGNAL_ID_GLONASS_G1       0x0
/* GNSS Ids */  
#define LPP_GNSS_ID_GPS                     0x0
#define LPP_GNSS_ID_GLONASS                 0x4

#define LPP_REQ_AD_CRITICAL_EXT T_RequestAssistanceData_criticalExtensions_c1


#define LPP_LAT_SIGN_NORTH  EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_latitudeSign_north
#define LPP_LAT_SIGN_SOURTH EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_latitudeSign_south


#define LPP_ALT_HEIGHT EllipsoidPointWithAltitude_altitudeDirection_height
#define LPP_ALT_DEPTH  EllipsoidPointWithAltitude_altitudeDirection_depth

#define LPP_REQ_AD_R9 T_RequestAssistanceData_criticalExtensions_c1_requestAssistanceData_r9
#define LPP_REQ_AD_NAVMODEL_STORED_NAV_LIST T_GNSS_NavigationModelReq_storedNavList
#define LPP_ALAMANC_KEPL_NAV T_GNSS_AlmanacElement_keplerianNAV_Almanac
#define LPP_GNSS_UTC_MODEL1 T_GNSS_UTC_Model_utcModel1


#define LPP_REQ_CAP_CRIT_EXT T_RequestCapabilities_criticalExtensions_c1

#define LPP_REQ_CAP_CRIT_EXT_R9 T_RequestCapabilities_criticalExtensions_c1_requestCapabilities_r9

#define LPP_PROV_AD_CRITICAL_EXT T_ProvideAssistanceData_criticalExtensions_c1
#define LPP_PROV_AD_CRITICAL_EXT_R9 T_ProvideAssistanceData_criticalExtensions_c1_provideAssistanceData_r9



#define LPP_PRS_MUTINGINFO_POS2 T_PRS_Info_prs_MutingInfo_r9_po2_r9 
#define LPP_PRS_MUTINGINFO_POS4 T_PRS_Info_prs_MutingInfo_r9_po4_r9 
#define LPP_PRS_MUTINGINFO_POS8 T_PRS_Info_prs_MutingInfo_r9_po8_r9 
#define LPP_PRS_MUTINGINFO_POS16 T_PRS_Info_prs_MutingInfo_r9_po16_r9 

#define LPP_REQ_CAP_R9 T_RequestCapabilities_criticalExtensions_c1_requestCapabilities_r9
#define LPP_REQ_CAP_CRIT_EXT T_RequestCapabilities_criticalExtensions_c1

#define LPP_REQ_LOC_INFO_CRIT_EXT T_RequestLocationInformation_criticalExtensions_c1

/* Time model IDs */
#define LPP_GNSS_TO_ID_GPS     1
#define LPP_GNSS_TO_ID_GLONASS 4

#define LPP_MEAS_MULTI_PATH_LOW_MTRs   5   /// Multipath error less than 5 meters
#define LPP_MEAS_MULTI_PATH_MED_MTRs   43  /// 5m < Multipath error < 43m


#define C_LPP_OTDOA_REQ_MSEC     1000    /* 1000 ms */
#define C_LPP_ECID_REQ_MSEC      400    /* to count for IPC/MSGR delays */
#define C_LPP_OTDOA_MUL_MEAS_MSEC 150   /* 50ms for 1 IPC delay + 100ms for LPP PLI msg processing time */

/**************************************************************/
/*                                                            */
/*  LocationInformationType                                   */
/*                                                            */
/**************************************************************/

typedef enum {
   LPP_locationEstimateRequired = 0,
   LPP_locationMeasurementsRequired = 1,
   LPP_locationEstimatePreferred = 2,
   LPP_locationMeasurementsPreferred = 3,
   LPP_locationMeasurementsMax = 4
} LPP_LocationInfoType;


#define LPP_PROV_LOC_CRITICAL_EXT T_ProvideLocationInformation_criticalExtensions_c1
#define LPP_PROV_LOC_CRITICAL_EXT_R9 T_ProvideLocationInformation_criticalExtensions_c1_provideLocationInformation_r9

typedef enum {
  LPP_SatMeasElement_mpathDet_notMeasured = 0,
  LPP_SatMeasElement_mpathDet_low = 1,
  LPP_SatMeasElement_mpathDet_medium = 2,
  LPP_SatMeasElement_mpathDet_high = 3
} LPP_SatMeasElement_mpathDet_Root;


#define LPP_ERROR_TARGET_DEVICE_CAUSE T_OTDOA_Error_targetDeviceErrorCauses

#define LPP_PROC_LOC_LOC_EST_SHAPE T_LocationCoordinates_ellipsoidPointWithAltitudeAndUncertaintyEllipsoid

#define LPP_ERROR_R9 T_Error_error_r9


#define LPP_PROV_ECID_RSRP ECID_RequestLocationInformation_requestedMeasurements_rsrpReq
#define LPP_PROV_ECID_RSRQ ECID_RequestLocationInformation_requestedMeasurements_rsrqReq
#define LPP_PROV_ECID_RXTX ECID_RequestLocationInformation_requestedMeasurements_ueRxTxReq

/* GPS Acquisition Assistance message Code Phase Search Window parameters.
   The parameters are assumed to be one sided search window
   and conversion is made from units of chips to ms. (1023 chips = 1ms) */
static const float LPP_SvTimeUnc[32] = {
  (FLT)0.5,               /* When server send zero as code phase search window, setting .5msecs SV time uncertainty
                             ensures measurement engine to perform full PN search for SVs. */
  (FLT)2.0 * (FLT)0.001,
  (FLT)4.0 * (FLT)0.001,
  (FLT)8.0 * (FLT)0.001,
  (FLT)12.0 * (FLT)0.001,
  (FLT)16.0 * (FLT)0.001,
  (FLT)24.0 * (FLT)0.001,
  (FLT)32.0 * (FLT)0.001,
  (FLT)48.0 * (FLT)0.001,
  (FLT)64.0 * (FLT)0.001,
  (FLT)96.0 * (FLT)0.001,
  (FLT)128.0 * (FLT)0.001,
  (FLT)164.0 * (FLT)0.001,
  (FLT)200.0 * (FLT)0.001,
  (FLT)250.0 * (FLT)0.001,
  (FLT)300.0 * (FLT)0.001,
  (FLT)360.0 * (FLT)0.001,
  (FLT)420.0 * (FLT)0.001,
  (FLT)480.0 * (FLT)0.001,
  (FLT)540.0 * (FLT)0.001,
  (FLT)600.0 * (FLT)0.001,
  (FLT)660.0 * (FLT)0.001,
  (FLT)720.0 * (FLT)0.001,
  (FLT)780.0 * (FLT)0.001,
  (FLT)850.0 * (FLT)0.001,
  (FLT)1000.0 * (FLT)0.001,
  (FLT)1150.0 * (FLT)0.001,
  (FLT)1300.0 * (FLT)0.001,
  (FLT)1450.0 * (FLT)0.001,
  (FLT)1600.0 * (FLT)0.001,
  (FLT)1800.0 * (FLT)0.001,
  (FLT)2000.0 * (FLT)0.001 };


static const float LPP_DopplerUncToMPS[5] = { 40.0, 20.0, 10.0, 5.0, 2.5 };


typedef enum {
  LPP_ERROR_UNDEF = 0,
  LPP_ERROR_MSGHEADER_ERROR = 1,
  LPP_ERROR_MSGBODY_ERROR = 2,
  LPP_ERROR_EPDU_ERROR = 3,
  LPP_ERROR_INCORRECT_DATA = 4
} LPP_CommonErrorCause;


typedef enum {
  LPP_ABORT_UNDEF = 0,
  LPP_ABORT_PERODIC_REPORT = 1,
  LPP_ABORT_TARGET = 2,
  LPP_ABORT_NETWORK = 3
} LPP_AbortCause;

/**************************************************************/
/*                                                            */
/*  LocationFailureCause                                      */
/*                                                            */
/**************************************************************/

typedef enum {
  LPP_FAILURE_UNDEF = 0,
  LPP_FAILURE_REQMETHODNOT_SUPPORTED = 1,
  LPP_FAILURE_POSMETHOD_FAILURE = 2,
  LPP_FAILURE_LOCMEAS_NOTAVAIL = 3
} LPP_LocationFailureCause;

typedef enum {
  LPP_AGNSS_CAUSE_UNDEF = 0,
  LPP_AGNSS_CAUSE_NOTENOUGHSAT = 1,
  LPP_AGNSS_CAUSE_AD_MISSING = 2,
  LPP_AGNSS_CAUSE_NOT_ALL_MEAS_POSSIBLE = 3
} LPP_AgnssFailureCause;


typedef enum {
  LPP_OTDOA_CAUSE_UNDEF = 0,
  LPP_OTDOA_CAUSE_AD_MISSING = 1,
  LPP_OTDOA_CAUSE_UNABLE_TO_MEAS_REF = 2,
  LPP_OTDOA_CAUSE_UNABLE_TO_MEAS_NEIGH = 3,
  LPP_OTDOA_CAUSE_UNABLE_TO_MEAS_SOME_NEIGH = 4
} LPP_OtdoaFailureCause;



typedef enum {
  LPP_ECID_CAUSE_UNDEF = 0,
  LPP_ECID_CAUSE_REQMEAS_NOTAVAIL = 1,
  LPP_ECID_CAUSE_NOTALLREQ_POSS = 2
} LPP_ECIDErrorCauses;

/*===========================================================================
  tm_lpp_handle_provide_ad

  Description:
     LPP function to handle decoded Assitance Data message

  Parameters:
    *p_client - client type
    *p_lpp_tx_type - pointer to the transaction structure


  Returns
    TRUE if the decoding passed, FALSE otherwise
============================================================================*/
boolean tm_lpp_handle_prov_ad( const tm_lpp_tx_mgr_client_type *p_client,
                               tm_lpp_transaction_info_s_type *p_lpp_tx_type );


/*===========================================================================
  tm_lpp_handle_req_loc_info

  Description:
     LPP function to handle decoded Request Location Information message

  Parameters:
    *p_client - client type
    *p_lpp_tx_type - pointer to the transaction structure


  Returns
    TRUE if the decoding passed, FALSE otherwise
============================================================================*/
boolean tm_lpp_handle_req_loc_info( const tm_lpp_tx_mgr_client_type *p_client,
                                    tm_lpp_transaction_info_s_type *p_lpp_tx_type );

/*===========================================================================
  tm_lpp_encode_provide_cap

  Description:
    LPP function to encode Provide Capability message

  Parameters:
    *p_client - pointer to the client
    *p_lpp_tx_type - pointer to the transaction strucutre

  Return Value:
    TRUE if the encoding passed, FALSE otherwise
=============================================================================*/
boolean tm_lpp_encode_prov_cap( const tm_lpp_tx_mgr_client_type *p_client,
                                tm_lpp_transaction_info_s_type *p_lpp_tx_type
                                );

/*===========================================================================
  tm_lpp_encode_provide_cap

  Description:
    LPP function to encode Provide Capability message

  Parameters:
    *p_lpp_tx_type - pointer to the transaction strucutre

  Return Value:
    TRUE if the encoding passed, FALSE otherwise
=============================================================================*/
boolean tm_lpp_encode_provide_cap(
                                   tm_lpp_transaction_info_s_type *p_lpp_tx_type
                                   /**< encoded data length */
                                   );

/*===========================================================================
  tm_lpp_encode_request_assistance

  Description:
    LPP function to encode Request Assistance message

  Parameters:
    *p_lpp_tx_type - pointer to the transaction strucutre

  Return Value:
    TRUE if the encoding passed, FALSE otherwise
=============================================================================*/
boolean tm_lpp_encode_request_assistance(
                                          tm_lpp_transaction_info_s_type *p_lpp_tx_type
                                          );

/*===========================================================================
  tm_lpp_encode_provide_location

  Description:
    LPP function to encode Provide Location message

  Parameters:
    *p_lpp_tx_type - pointer to the transaction strucutre

  Return Value:
    TRUE if the encoding passed, FALSE otherwise
=============================================================================*/
boolean tm_lpp_encode_provide_location(
                                        tm_lpp_transaction_info_s_type *p_lpp_tx_type
                                        );
/*===========================================================================
  tm_lpp_encode_error

  Description:
    LPP function to encode Error message

  Parameters:
    *p_lpp_tx_type - pointer to the transaction strucutre

  Return Value:
    TRUE if the encoding passed, FALSE otherwise
=============================================================================*/
boolean tm_lpp_encode_error(
                             tm_lpp_transaction_info_s_type *p_lpp_tx_type
                             );


/*===========================================================================
  tm_lpp_encode_abort

  Description:
    LPP function to encode Abort message

  Parameters:
    *p_lpp_tx_type - pointer to the transaction strucutre

  Return Value:
    TRUE if the encoding passed, FALSE otherwise
=============================================================================*/
boolean tm_lpp_encode_abort( const tm_lpp_tx_mgr_client_type *p_client,
                             tm_lpp_transaction_info_s_type *p_lpp_tx_type
                             );


/*===========================================================================
  tm_lpp_handle_req_cap

  Description:
     LPP function to handle decoded Request Capabilities Information message

  Parameters:
    *p_client - client type
    *p_lpp_tx_type - pointer to the transaction structure


  Returns
    TRUE if the decoding passed, FALSE otherwise
============================================================================*/
boolean tm_lpp_handle_req_cap( const tm_lpp_tx_mgr_client_type *p_client,
                               tm_lpp_transaction_info_s_type *p_lpp_tx_type );



/*===========================================================================
  tm_lpp_handle_error

  Description:
     LPP function to handle decoded ERROR message

  Parameters:
    *p_client - client type
    *p_lpp_tx_type - pointer to the transaction structure


  Returns
    TRUE if the decoding passed, FALSE otherwise
============================================================================*/
boolean tm_lpp_handle_error( const tm_lpp_tx_mgr_client_type *p_client,
                             tm_lpp_transaction_info_s_type *p_lpp_tx_type );


/*===========================================================================
  tm_lpp_handle_abort

  Description:
     LPP function to handle decoded ABORT message

  Parameters:
    *p_client - client type
    *p_lpp_tx_type - pointer to the transaction structure


  Returns
    TRUE if the decoding passed, FALSE otherwise
============================================================================*/
boolean tm_lpp_handle_abort( const tm_lpp_tx_mgr_client_type *p_client,
                             tm_lpp_transaction_info_s_type *p_lpp_tx_type );

/*===========================================================================
  tm_lpp_encode_request_assistance

  Description:
    LPP function to encode Request Assistance message

  Parameters:
    *p_client - pointer to the client
    *p_lpp_tx_type - pointer to the transaction strucutre

  Return Value:
    TRUE if the encoding passed, FALSE otherwise
=============================================================================*/
boolean tm_lpp_encode_req_ad( const tm_lpp_tx_mgr_client_type *p_client,
                              tm_lpp_transaction_info_s_type *p_lpp_tx_type );


/*===========================================================================
  tm_lpp_encode_prov_loc_info

  Description:
    LPP function to encode Provide Location message

  Parameters:
    *p_client - pointer to the client
    *p_lpp_tx_type - pointer to the transaction strucutre

  Return Value:
    TRUE if the encoding passed, FALSE otherwise
=============================================================================*/
boolean tm_lpp_encode_prov_loc_info( const tm_lpp_tx_mgr_client_type *p_client,
                                     tm_lpp_transaction_info_s_type *p_lpp_tx_type );


/*===========================================================================
  tm_lpp_encode_error

  Description:
    LPP function to encode Error message

  Parameters:
    *p_client - pointer to the client
    *p_lpp_tx_type - pointer to the transaction strucutre

  Return Value:
    TRUE if the encoding passed, FALSE otherwise
=============================================================================*/
boolean tm_lpp_encode_err( const tm_lpp_tx_mgr_client_type *p_client,
                           tm_lpp_transaction_info_s_type *p_lpp_tx_type );

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef struct {
  long length;
  unsigned char *value;
} tm_lpp_pdu_buf_type;

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP Glonass reference time IE to the format ready
 *  for TM core post data message. Finally return the prepared tmcore post data
 *  for the caller to post to TMCore.
 *
 * Parameters:
 *
 *  p_ref_time: pointer to the structure containing ASN.1 decoded LPP message.
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

boolean tm_lpp_glo_reftime_to_tmcore_post_data_payload ( GNSS_ReferenceTime *p_ref_time, 
                                                           tm_post_data_payload_type *p_tm_post_data_payload );


/*
 ******************************************************************************
 *
 * tm_lpp_glo_eph_to_tmcore_post_data_payload
 *
 * Function description:
 *  Buffers Glonass navigation model in a buffer that associates its SV ID with
 *  the corresponding frequency number                  
 *  
 * Parameters:
 *   p_ganss_sat_element : pointer to the structure holding Glonass nav. model.
 *
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/
void tm_lpp_glo_eph_to_tmcore_post_data_payload (GNSS_NavModelSatelliteElement *p_NavModelEph, 
                                                   glo_EphStructType *p_TmPostDataGloEph);



/*******************************************************************************
 *  p_Almanac: pointer to the structure containing the per SV based ASN.1 
 *             decoded LPP almanac data.
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

void tm_lpp_glo_alm_to_tmcore_post_data_payload( const GNSS_AlmanacElement *p_Almanac,
                                                   glo_AlmStructType *p_TmPostDataGloAlmanac
                                                 );


/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP reference position IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_ref_loc: pointer to the structure containing ASN.1 decoded LPP
 *          reference location data.
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

boolean tm_lpp_refpos_to_TmCore_post_data_payload( const GNSS_ReferenceLocation *p_ref_loc,
                                                   tm_post_data_payload_type *p_tm_post_data_payload
                                                   );

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP IONO IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_iono: pointer to the structure containing ASN.1 decoded LPP
 *          IONO data.
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

void tm_lpp_Iono_to_TmCore_post_data_payload( const GNSS_IonosphericModel *p_Iono,
                                              tm_post_data_payload_type *p_tm_post_data_payload );

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP navigation model IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_Ephem: pointer to the structure containing the per SV based ASN.1
 *            decoded LPP navigation model data.
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

void tm_lpp_eph_to_TmCore_post_data_payload( const GNSS_NavModelSatelliteElement *p_Ephem,
                                             tm_post_data_payload_type *p_tm_post_data_payload,
                                             uint16 w_week
                                             );
/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP almanac IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_Almanac: pointer to the structure containing the per SV based ASN.1
 *             decoded LPP almanac data.
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

void tm_lpp_alm_to_TmCore_post_data_payload( const GNSS_AlmanacElement *p_Almanac,
                                             tm_post_data_payload_type *p_tm_post_data_payload
                                             );

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP UTC Model IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing ASN.1 decoded LPP UTC model.
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


void tm_lpp_UtcModel_to_TmCore_post_data_payload( const GNSS_UTC_Model *p_Utc,
                                                  tm_post_data_payload_type *p_tm_post_data_payload );

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP acquistion assistance's SV direction IE to
 *  the format ready for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing the per SV based ASN.1
 *            decoded LPPP almanac data.
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


void tm_lpp_AcqAssistSvDir_to_TmCore_post_data_payload( const GNSS_AcquisitionAssistList *p_acq_assist, 
                                                         tm_post_data_payload_type *p_tm_post_data_payload,
                                                         uint32 q_GnssType
                                                       );

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP acquistion assistance IE's steering data to
 *  the format ready for TM core post data message.
 *
 * Parameters:
 *
 *  p_gnss_acq_assist: pointer to the structure containing the per SV based ASN.1
 *            decoded LPP acq. assistance data.
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

void tm_lpp_AcqAssistSteering_to_TmCore_post_data_payload( GNSS_AcquisitionAssistance const *const p_gnss_acq_assist,
                                                           tm_post_data_payload_type *p_tm_post_data_payload,
                                                           uint16 const w_gps_week,
                                                           uint32 const q_gps_msec
                                                          );
/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP GLONASS acquistion assistance IE's steering data to
 *  the format ready for TM core post data message.
 *
 * Parameters:
 *
 *  p_gnss_acq_assist [IN]: pointer to the structure containing the per SV based ASN.1
 *                          decoded LPP GLONASS acq. assistance data.
 *  p_prot_glo_aa_buff[OUT]: pointer to Glonass AA data out structure.
 *  q_glo_rt_msec     [IN]: Glonass millisecond in day reference time.
 *
 * Return value:
 *
 *    None.
 *
 *
 ******************************************************************************
*/
void tm_lpp_glo_acq_assist_to_tmcore_post_data_payload( GNSS_AcquisitionAssistance const *const p_gnss_acq_assist,
                                                        prot_glo_acqast_buffer_s_type *p_prot_glo_aa_buff,
                                                        const uint32 q_glo_rt_msec );

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP OTDOA assistance IE's steering data to
 *  the format ready for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing the per SV based ASN.1
 *            decoded LPP OTDOA assistance data.
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

void tm_lpp_OTDOAAssist_to_TmCore_post_data_payload( OTDOA_ProvideAssistanceData *p_otdoa_assist,
                                                     tm_post_data_payload_type *p_tm_post_data_payload
                                                     );

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP FTA assistance IE's to
 *  the format ready for LOC_LTE message.
 *
 * Parameters:
 *
 *  p_ref_time : pointer to the structure containing the ref time ASN.1
 *            decoded LPP FTA assistance data.
 *
 *
 * Return value:
 *
 *    None.
 *
 *
 ******************************************************************************
*/

void tm_lpp_fta_post_loc_lte( GNSS_ReferenceTime *p_ref_time,
                              tm_post_data_payload_type *p_tm_post_data_payload
                              );

/*
 ******************************************************************************
 * Function description:
 *  Function to process 'Force LTE hybrid fix' command
 *
 * Parameters:
 *
 *  b_sub_cmd : flag to enable/disable feature.
 *
 *
 * Return value:
 *
 *    None.
 *
 *
 ******************************************************************************
*/
void tm_lpp_proc_force_lte_hybrid_fix_cmd
(
  uint8 b_sub_cmd
  );

/*
 ******************************************************************************
 *
 * tm_lpp_cp_glo_time_model_proc
 *
 * Function description:
 *  This function processes the OSYS ASN.1 decoded LPP Assistance data for glo time model  
 *  and prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *  p_assist: pointer to the OSYS ASN.1 decoded LPP gpsAsssitance data.
 * 
 * Return value:
 *  TRUE  : Leap second provided
 *  FALSE : Leap second not provided
 *
 ******************************************************************************
*/
boolean tm_lpp_glo_time_model_proc
(
  tm_prtl_type q_protocol,
  gnss_time_info* z_pos_time_stamp,
  tm_sess_handle_type q_active_sess_handle,
  const GNSS_GenericAssistDataElement* const p_elem,
  tm_post_data_payload_type* p_tm_post_data_payload
);

/*
 ******************************************************************************
 *
 * tm_lpp_cp_glo_utc_proc
 *
 * Function description:
 *  This function processes the OSYS ASN.1 decoded LPP Assistance data for glo utc and 
 *  prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *  p_assist: pointer to the OSYS ASN.1 decoded LPP gpsAsssitance data.
 * 
 * Return value:
 *
 ******************************************************************************
*/
void tm_lpp_glo_utc_proc
(
  tm_prtl_type q_protocol,
  tm_sess_handle_type q_active_sess_handle,
  const GNSS_GenericAssistDataElement* const p_elem,
  tm_post_data_payload_type* p_tm_post_data_payload
);

/*
 ******************************************************************************
 *
 * tm_lpp_glo_nav_model_proc
 *
 * Function description:
 *  This function processes the OSYS ASN.1 decoded LPP Assistance data for glo eph and 
 *  prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *  p_assist: pointer to the OSYS ASN.1 decoded LPP gpsAsssitance data.
 * 
 * Return value:
 *
 ******************************************************************************
*/
void tm_lpp_glo_nav_model_proc
(
  tm_prtl_type q_protocol,
  prot_glo_eph_buffer_s_type z_prot_glo_eph_buf[N_GLO_SV],
  tm_sess_handle_type q_active_sess_handle,
  const GNSS_GenericAssistDataElement* const p_elem,
  tm_post_data_payload_type* p_tm_post_data_payload
);

/*
 ******************************************************************************
 *
 * tm_lpp_cp_glo_alm_proc
 *
 * Function description:
 *  This function processes the OSYS ASN.1 decoded LPP Assistance data for glo alm and 
 *  prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *  p_assist: pointer to the OSYS ASN.1 decoded LPP gpsAsssitance data.
 * 
 * Return value:
 *
 ******************************************************************************
*/
void tm_lpp_glo_alm_proc
(
  tm_prtl_type q_protocol,
  prot_glo_eph_buffer_s_type z_prot_glo_eph_buf[N_GLO_SV],
  tm_sess_handle_type q_active_sess_handle,
  const GNSS_GenericAssistDataElement* const p_elem,
  tm_post_data_payload_type* p_tm_post_data_payload
);

/*
 ******************************************************************************
 *
 * tm_lpp_cp_glo_rti_proc
 *
 * Function description:
 *  This function processes the OSYS ASN.1 decoded LPP Assistance data for glo alm and 
 *  prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *  p_assist: pointer to the OSYS ASN.1 decoded LPP gpsAsssitance data.
 * 
 * Return value:
 *
 ******************************************************************************
*/
void tm_lpp_glo_rti_proc
(
  tm_prtl_type q_protocol,
  tm_sess_handle_type q_active_sess_handle,
  const GNSS_GenericAssistDataElement* const p_elem,
  tm_post_data_payload_type* p_tm_post_data_payload
);

/*
 ******************************************************************************
 *
 * tm_lpp_glo_inj_eph
 *
 * Function description:
 *  This function injects any glonass eph that didn't have freq/SV ID association before.
 *
 * Parameters:
 *  p_assist: pointer to the OSYS ASN.1 decoded LPP gpsAsssitance data.
 * 
 * Return value:
 *
 ******************************************************************************
*/
void tm_lpp_glo_inj_eph
(
  tm_prtl_type q_protocol,
  prot_glo_eph_buffer_s_type z_prot_glo_eph_buf[N_GLO_SV],
  tm_sess_handle_type q_active_sess_handle,
  tm_post_data_payload_type* p_tm_post_data_payload
);

/*
 ******************************************************************************
 *
 * tm_lpp_cp_proc_otdoa_stop_req
 *
 * Function description:
 *  This function would send a request to TMCore to stop the OTDOA engine.
 *
 * Parameters:
 *  sess_handle: LPP UP or CP session type.
 *  reason     : Reason to stop the session.
 *  prtl_type  : Protocol module type invoking this call.
 * 
 * Return value:
 *  None.
 *
 ******************************************************************************
*/
void tm_lpp_proc_otdoa_stop_req( tm_sess_handle_type sess_handle, tm_sess_stop_reason_e_type const reason, tm_prtl_type prtl_type );

#endif /* _TM_LPP_H */
