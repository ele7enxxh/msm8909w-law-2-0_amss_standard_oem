#ifndef _TM_RRLP_H
#define _TM_RRLP_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         TM RRLP Utility

GENERAL DESCRIPTION
  This file contains TM's RRLP protocol utilities, which convert decoded RRLP
  messages for the consumption of TmCore.  

  Copyright (c) 2007 - 2012 Qualcomm Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_common/inc/tm_rrlp.h#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  01/09/15   rk      Fix the GLONASS SV TimeUnc value for zero-th index.
  04/06/11   LT      ASN1 migration.
  05/29/07   LT      Moved the psudoRMS_formatter function to tm_umts_common_utils.c. 
  05/11/07   LT      Moved variables and constants common to RRLP and RRC out.
  04/25/07   LT      Initial version

===========================================================================*/
#include "gps_variation.h"
#include "tm_rrlp_asn1.h"


#include "tm_prtl_iface.h"
#include "tm_data.h"
/* mapping existing constants to those used by the ASN1 encoder/decoder */

#define rrlp_PDU  PDU

#define rrlp_msrPositionReq_chosen T_RRLP_Component_msrPositionReq //1
#define rrlp_msrPositionRsp_chosen T_RRLP_Component_msrPositionRsp //2
#define rrlp_assistanceData_chosen T_RRLP_Component_assistanceData //3
#define rrlp_assistanceDataAck_chosen T_RRLP_Component_assistanceDataAck //4
#define rrlp_protocolError_chosen     T_RRLP_Component_protocolError  //5

#define rrlp_posCapabilityReq_chosen  T_RRLP_Component_posCapabilityReq //6
#define rrlp_posCapabilityRsp_chosen  T_RRLP_Component_posCapabilityRsp //7

/* 
#define T_RRLP_Component_extElem1       8
*/ 

#define rrlp_newSatelliteAndModelUC_chosen  T_rrlp_SatStatus_newSatelliteAndModelUC //1
#define rrlp_oldSatelliteAndModel_chosen  T_rrlp_SatStatus_oldSatelliteAndModel //2
#define rrlp_newNaviModelUC_chosen        T_rrlp_SatStatus_newNaviModelUC //3

#define rrlp_msAssisted_chosen   T_rrlp_MethodType_msAssisted   // 1
#define rrlp_msBased_chosen      T_rrlp_MethodType_msBased      // 2
#define rrlp_msBasedPref_chosen  T_rrlp_MethodType_msBasedPref  // 3
#define rrlp_msAssistedPref_chosen  T_rrlp_MethodType_msAssistedPref   //4


#define rrlp_LocErrorReason_unDefined  rrlp_LocErrorReason_unDefined       //0
//  rrlp_notEnoughBTSs             rrlp_LocErrorReason_notEnoughBTSs  //1
#define  rrlp_notEnoughSats        rrlp_LocErrorReason_notEnoughSats //2
//  rrlp_eotdLocCalAssDataMissing    rrlp_LocErrorReason_eotdLocCalAssDataMissing //3
//  rrlp_eotdAssDataMissing          rrlp_LocErrorReason_eotdAssDataMissing  //4
#define  rrlp_gpsLocCalAssDataMissing  rrlp_LocErrorReason_gpsLocCalAssDataMissing  //5
#define  rrlp_gpsAssDataMissing        rrlp_LocErrorReason_gpsAssDataMissing  //6
#define  rrlp_methodNotSupported       rrlp_LocErrorReason_methodNotSupported  //7
//  rrlp_notProcessed                  rrlp_LocErrorReason_notProcessed   //8
//  rrlp_refBTSForGPSNotServingBTS    rrlp_LocErrorReason_refBTSForGPSNotServingBTS //9
//  rrlp_refBTSForEOTDNotServingBTS   rrlp_LocErrorReason_refBTSForEOTDNotServingBTS  //10
//  rrlp_notEnoughGANSSSats           rrlp_LocErrorReason_notEnoughGANSSSats //11
//  rrlp_ganssAssDataMissing          rrlp_LocErrorReason_ganssAssDataMissing //12
//  rrlp_refBTSForGANSSNotServingBTS  rrlp_LocErrorReason_refBTSForGANSSNotServingBTS //13

#define rrlp_moreMessagesOnTheWay  rrlp_MoreAssDataToBeSent_moreMessagesOnTheWay
#define rrlp_messageTooShort  rrlp_ErrorCodes_messageTooShort

#define rrlp_notMeasured   rrlp_MpathIndic_notMeasured //0

#if defined ( FEATURE_GNSS_OSYS_V665 )
 #define rrlp_threeDFix  rrlp_FixType_threeDFix  //1
#elif defined ( FEATURE_GNSS_OSYS_V668 )
 #define rrlp_threeDFix  rrlp_FixType_threeDFix  //1
#else
 #error "Undefined OSYS LIB Version"
#endif

#define GANSS_SIGNAL_ID_GLONASS_G1 0
#define GANSS_ID_3_GLONASS 3


static const float agnss_doppler_unc_to_meters_per_sec[5] = {40.0, 20.0,10.0,5.0,2.5};


/* GNSS Acquisition Assistance message Code Phase Search Window parameters.
   The parameters are assumed to be one sided search window
   and conversion is made from units of chips to ms. (1023 chips = 1ms)
 
   Note the GPS counterpart is in tm_umts_common_utils.h because it is used by SUPL-RRLP, CP-RRLP and CP-RRC */

static const float gnss_SvTimeUnc[32] = {
  (FLT)0.5,              /* When server send zero as code phase search window, setting .5msecs SV time uncertainty
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
  (FLT)2000.0 * (FLT)0.001};


enum
{
  RRLP_ASN1_DECODE_ERR_PDU_DECODED,
  RRLP_ASN1_DECODE_ERR_PDU_TOO_SHORT,
  RRLP_ASN1_DECODE_ERR_PDU_INCORRECT_DATA,
  RRLP_ASN1_DECODE_ERR_PDU_UNDEFINED
};

typedef enum
{
  C_RRLP_CHOICE_MSRPOSREQ,
  C_RRLP_CHOICE_MSRPOSRESP,
  C_RRLP_CHOICE_ASSISTDATA,
  C_RRLP_CHOICE_ASSISTACK,
  C_RRLP_CHOICE_PROTCOL_ERR
}rrlp_choice_e_type;

typedef struct
{
  uint32 smlc_code;
  uint32 transaction_ID;
} rrlp_rel5_ext_ext_ref_s_type; 


typedef struct
{

  boolean                          rel5_ext_present;
  rrlp_rel5_ext_ext_ref_s_type     rel5_ext_ref;

}rrlp_rel5_extension_s_type;

      
typedef struct
{
  boolean     rel7_ext_present;
  boolean     u_velocity_requested; /* REL_7 ext. */
  uint8       u_ganss_pos_methods_bitmap; /* REL_7 ext. :
                                             0 - not included;
                                             1 - RRLP_GANSS_POS_METHOD_GPS ;
                                             2 - GANSS_POS_METHOD_GLONASS */
}rrlp_rel7_extension_s_type; 

typedef struct
{
  /* Bits: 8 7 6 5 4 3 2 1 */
  
  uint8 u_ShapeCode_verDir_MsbBearing; 
  /* use 0x1_ for hor and ver; Bit 2 for verDir; Bit 1 for MSB bearing  */
  
  uint8 u_hor_bearing_Lsb8; /* the low 8 bits of horizontal bearing */
  uint8 u_hor_speed0; /* km/hour */
  uint8 u_hor_speed1; /* km/hour */
  uint8 u_ver_speed;  /* km/hour */
  
}gad_velocity_hor_ver_s_type;


#define RRLP_GANSS_POS_METHOD_GPS     1
#define RRLP_GANSS_POS_METHOD_GLONASS 2
#define RRLP_GANSS_TIME_ID_GLONASS    2

typedef struct
{

  int8                         b_RefNum;       /* RRLP reference number */
  rrlp_choice_e_type           u_rrlp_choice;  /* RRLP PDU component tag */
  rrlp_rel5_extension_s_type   z_rrlp_rel5_ext;
  uint8                        u_measure_response_time_sec; /* 1...128 */
  
  rrlp_rel7_extension_s_type   z_rrlp_rel7_ext; 
  
} rrlp_params_s_type;


/**** external functions **/
void tm_rrlp_reftime_to_TmCore_post_data_payload( rrlp_ControlHeader           *p_assist, 
                                                  tm_post_data_payload_type    *p_tm_post_data_payload);

boolean tm_rrlp_refpos_to_TmCore_post_data_payload( rrlp_ControlHeader             *p_assist, 
                                                    rrlp_GANSS_ControlHeader       *p_ganss_assist,
                                                    tm_post_data_payload_type      *p_tm_post_data_payload, 
                                                    uint8                          *p_gad_pos_to_save,
                                                    boolean                        u_gad_compliant
                                                  );

void tm_rrlp_eph_to_TmCore_post_data_payload( rrlp_UncompressedEphemeris     *p_Ephem,
                                              tm_post_data_payload_type      *p_tm_post_data_payload,
                                              uint16 w_week );

void tm_rrlp_Iono_to_TmCore_post_data_payload( rrlp_ControlHeader *p_assist, 
                                               tm_post_data_payload_type   *p_tm_post_data_payload );

void tm_rrlp_alm_to_TmCore_post_data_payload( rrlp_AlmanacElement         *p_Almanac,
                                              tm_post_data_payload_type   *p_tm_post_data_payload );

void tm_rrlp_UtcModel_to_TmCore_post_data_payload( rrlp_ControlHeader *p_assist, 
                                                   tm_post_data_payload_type   *p_tm_post_data_payload );

void tm_rrlp_AcqAssistSvDir_to_TmCore_post_data_payload( rrlp_ControlHeader *p_assist, 
                                                         tm_post_data_payload_type *p_tm_post_data_payload );


void tm_rrlp_AcqAssistSteering_to_TmCore_post_data_payload( rrlp_ControlHeader *p_assist, 
                                                            tm_post_data_payload_type *p_tm_post_data_payload,
                                                            uint16 w_gps_week );
                                                            
uint8 tm_rrlp_mprsp_gps_meas_build ( rrlp_MsrPosition_Rsp  *p_mpr, 
                                    const sm_GpsMeasRptStructType *p_prm_data,
                                    uint8 u_list_len,
                                    boolean status_check_needed );


boolean tm_rrlp_mprsp_glonass_meas_build ( rrlp_MsrPosition_Rsp  *p_mpr, 
                                    const sm_GpsMeasRptStructType *p_prm_data,
                                    boolean status_check_needed,
                                    boolean b_gps_meas_included,
                                    rrlp_GANSS_MsrSetElement *p_rrlp_GANSS_MsrSetElement );
                                                            

void tm_rrlp_glo_reftime_to_TmCore_post_data_payload
( rrlp_GANSS_ControlHeader *p_assist, 
  tm_post_data_payload_type    *p_tm_post_data_payload 
);

void tm_rrlp_glo_eph_to_TmCore_post_data_payload
( 
 prot_glo_eph_buffer_s_type *p_glo_eph_sat_element_and_aux,
 tm_post_data_payload_type      *p_tm_post_data_payload
);

void tm_rrlp_glo_alm_to_TmCore_post_data_payload
( rrlp_Almanac_GlonassAlmanacSet *p_glo_alm_set, 
  tm_post_data_payload_type      *p_tm_post_data_payload
);

void tm_rrlp_glo_addutc_to_TmCore_post_data_payload
( rrlp_GANSS_ControlHeader *p_assist, 
  tm_post_data_payload_type    *p_tm_post_data_payload 
);


void tm_rrlp_glo_acq_assist_to_tmcore_post_data_payload 
( rrlp_SeqOfGANSSRefMeasurementElement *ganssRefMeasAssistList_ptr,
  prot_glo_acqast_buffer_s_type *prot_glo_acqasst_buffer_ptr,
  uint32 q_GloMsec
);

#endif /* _TM_RRLP_H */
