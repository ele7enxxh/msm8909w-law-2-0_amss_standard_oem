#ifndef GMM_MSG_H
#define GMM_MSG_H
/*===========================================================================

              GMM L3 MESSAGE DEFINITIONS HEADER FILE

DESCRIPTION
  This file contains data and function declarations necessary for the
  GMM subsystem of the UMTS/GSM MM task.

Copyright (c) 2001,2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/gmm_msg.h_v   1.3   02 May 2002 16:20:20   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/gmm_msg.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/01   jca     Created file.
05/23/01   jca     Added support for Release 1 GMM L3 messages.
04/30/02   ks      Added new struct gmm_ie_LV_resp_mobile_id_T for holding
                   the Mobile Identity during Identity Response message. 
09/04/02   vdr     Changed prototype of the gmm_send_service_request_message()
10/07/02   jca     Modified gmm_attach_accept_msg_T and gmm_ra_update_accept_msg_T
                   to use gmm_ie_LV_mobile_id_T for ms_identity field.
10/09/02   vdr     Changed prototypes of the following func's:
                   o gmm_send_attach_request_message()
                   o gmm_send_routing_area_update_request_message()
04/03/03   cd      Added support for equivalent PLMN list in the gmm_attach_accept 
                   and gmm_rau_accept message structures
06/19/03   ks      modified the length to word in gmm_ie_LV_ms_radio_access_caps_T
08/27/04   ab      CR39294.  Updated gmm_npdu_number_list_type structured.
09/16/05   ajt     Added emergency_number_list member to gmm_attach_accept_msg_T,
                   gmm_ra_update_accept_msg_T.
10/03/05   up      Set RAC size to 52 in gmm_ie_LV_ms_radio_access_caps_T.
10/06/05   ajt     Added symbolic constant for GMM_IEI_NW_FTR_SUPPORT
11/23/05   ajt     Added pdp_context_status member in gmm_ra_update_accept_msg_T. 
03/18/09   hnam    Added get_drx_parameter() function definition, need to access 
                    this in LTE code
04/22/10   zren    Moved difinitions needed by LTE to gmm_emm_common.h
09/29/10   abhi    Removed defination of gmm_ie_LV_ms_radio_access_caps_T from gmm_msg.h, 
                      this will now be only in sys_stru.h
01/31/11   rajesh  voice domain preference and UE usage setting related changes 
08/18/11   abhi    Added support for PTMSI type 
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "comdef.h"
#include "gmm_int_v.h"
#include "rrcmmif.h"
#include "gmmsecurity.h"
#include "mm_iei.h"
#include "reg_mm_v.h"
#include "gmm_emm_common.h"



/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* ----------------------------------
** Attach Result (TS 24.008 10.5.5.1)
** ---------------------------------- */
#define GMM_GPRS_ONLY_ATTACHED           0x1
#define GMM_COMBINED_GPRS_IMSI_ATTACHED  0x3

/* -----------------------------------
** Update Result (TS 24.008 10.5.5.18)
** ----------------------------------- */
#define GMM_RA_UPDATED             0x0
#define GMM_COMBINED_RA_LA_UPDATED 0x1
#ifdef FEATURE_LTE 
#define GMM_RA_UPDATED_AND_ISR_ACTIVATED         0x04
#define GMM_COMBINED_RA_LA_UPDATED_AND_ISR_ACTIVATED     0x05
#endif 

/* --------------------------------------------------
** Mobile terminated Detach type (TS 24.008 10.5.5.5)
** -------------------------------------------------- */
#define GMM_REATTACH_REQUIRED     0x1
#define GMM_REATTACH_NOT_REQUIRED 0x2
#define GMM_MT_IMSI_DETACH        0x3

/* ------------------------------------
** Identity type 2 (TS 24.008 10.5.5.9)
** ------------------------------------ */
#define GMM_IMSI   0x1
#define GMM_IMEI   0x2
#define GMM_IMEISV 0x3
#define GMM_TMSI   0x4

/* -----------
** IEI defines
** ----------- */
#define GMM_IEI_PTMSI_SIGNATURE           0x19
#define GMM_IEI_READY_TIMER_VALUE         0x17
#define GMM_IEI_TMSI_STATUS               0x09   /* Usually half octet */

#define GMM_IEI_PTMSI                     0x18
#define GMM_IEI_MS_IDENTITY               0x23
#define GMM_IEI_CAUSE                     0x25
#define GMM_IEI_T3302_VALUE               0x2A
#ifdef FEATURE_NAS_REL10
#define GMM_IEI_T3346_VALUE               0x3A
#endif
#if defined(FEATURE_LTE)
#define GMM_IEI_T3323_VALUE               0x38
#define GMM_IEI_UE_NW_CAPABILITY          0x58
#endif

#ifdef FEATURE_NAS_REL10
#define GMM_IEI_EX_T3312_VALUE            0x39
#endif

#define GMM_IEI_CELL_NOTIFICATION         0x8C

#define GMM_IEI_AUTH_PARAMETER_RAND       0x21
#define GMM_IEI_AUTH_GPRS_CKSN            0x08
#define GMM_IEI_AUTH_PARAMETER_AUTN       0x28

#define GMM_IEI_AUTH_PARAM_RESPONSE       0x22
#define GMM_IEI_IMEISV                    0x23
#define GMM_IEI_AUTH_PARAM_RESPONSE_EXT   0x29

#define GMM_IEI_AUTH_FAILURE_PARAMETER    0x30

#define GMM_IEI_DRX_PARAMETER             0x27
#define GMM_IEI_MS_NETWORK_CAPABILITY     0x31
#define GMM_IEI_PDP_CONTEXT_STATUS          0x32

#define GMM_IEI_RECEIVE_NPDU_NUMBERS_LIST 0x26

#define GMM_IEI_NW_FTR_SUPPORT            0xB0
#define GMM_IEI_MBMS_CONTEXT_STATUS       0x35

#define GMM_IEI_UPLINK_DATA_STATUS 0x36

#define GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_LENGTH     17
#define GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPIS     11
#define GMM_RECEIVE_NPDU_NUMBER_LIST_MIN_NSAPI       5
#define GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPI      15

#define GMM_PS_LCS_SUPPORTED              0x08
#define GMM_MBMS_SUPPORTED                0x04
#define GMM_VOIMS_SUPPORTED_IN_UMTS       0x02

#define GMM_MBMS_CONTEXT_STATUS_LENGTH    0x12

#define GMM_ADDITIONAL_MOBILE_IDENTITY   0x1A
#define GMM_ADDITIONAL_OLD_RAI    0x1B
#ifdef FEATURE_NAS_REL11
#define GMM_IEI_RAI2        0x1B
#define GMM_IEI_PTMSI_SIG2  0x19
#define GMM_IEI_RAI2_LENGTH  0x06
#define GMM_IEI_PTMSI_SIG2_LENGTH  0x03
#endif

#define GMM_VOICE_DOMAIN_PREF_IEI   0x5D
#define GMM_IEI_PTMSI_TYPE   0xE0
#define GMM_IEI_OLD_LAI     0x14
#ifdef FEATURE_NAS_REL10
#define GMM_IEI_UL_NW_FTR_SUPPORT         0xC0
#endif
#ifdef FEATURE_NAS_REL11 
#define GMM_IEI_TMSI_BASED_NRI_CONTAINER         0x10
#define GMM_IEI_TMSI_BASED_NRI_CONTAINER_CONTENT_LENGTH   0x02
#endif

#define GMM_IEI_REQUESTED_MS_INFORMATION  0xA0

#define GMM_IEI_T3319_VALUE        0x37

typedef struct
{
  boolean nsapi_valid_list[GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPIS];
  byte    npdu_number_list[GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPIS];
} gmm_npdu_number_list_type;




typedef struct
{
  byte length;
  byte value[GMM_MBMS_CONTEXT_STATUS_LENGTH];
}gmm_ie_mbms_context_status_T;

typedef struct
{
  byte length;
  byte value[9];
} gmm_ie_LV_resp_mobile_id_T;

typedef struct
{
  byte IEI;
  byte length;
  byte id_type;     /* type of identity, digit 1, odd/even indication */
  byte value[4];
} gmm_ie_TLV_mobile_id_T;

typedef struct 
{
  byte IEI;
  byte signature[3];
} gmm_ie_TV_ptmsi_signature_T;

typedef struct 
{
  byte IEI;
  byte timer_value;
} gmm_ie_TV_timer_value_T;

typedef struct 
{
  byte IEI;
  byte length;
  byte timer_value;
} gmm_ie_TLV_timer_value_T;

typedef struct 
{
  byte IEI;
  byte cause_value;
} gmm_ie_TV_cause_value_T;

/******************************************************************* 
 *
 *                     GMM Mobile ==>> Network
 *
 *******************************************************************/

/********************************************************************
 * GMM Attach Request Message
 */

typedef struct 
{
  mm_msg_head_T msg_head;
  byte          data[  sizeof(gmm_ie_LV_ms_network_caps_T)
                + 1     /* Attach type & GMM CKSN */
                + 2     /* DRX param */
                + sizeof(gmm_ie_LV_mobile_id_T)
                + 6     /* Old Routing area identification */
                + sizeof(gmm_ie_LV_ms_radio_access_caps_T)
                + 4     /* Old PTMSI signature */
                + 2     /* GMM Timer */
                + 1   /* TMSI status */
                +15 /* UE network capability*/
                + 7  /*Additional Mobile Identity*/
                + 8  /*Additional Old RAI */
                + 3 /*Voice domain preference and UE's usage setting */
                +1  /*PTMSI Type*/
                +7
                +1  /* Network Support feature IE*/
                ]; /*Old LAI*/

} gmm_attach_request_msg_T;

/********************************************************************
 * GMM Attach Complete Message
 */

typedef struct 
{
  mm_msg_head_T     msg_head;
} gmm_attach_complete_msg_T;

/********************************************************************
 * Mobile originting Detach Request Message
 */

typedef struct 
{
  mm_msg_head_T msg_head;
  byte          detach_type_spare;
  byte          ptmsi[7];           /* P-TMSI mobile ID */
  byte          ptmsi_signature[5]; /* P-TMSI Signature */
} gmm_mo_detach_request_msg_T;

/********************************************************************
 * Mobile Terminated Detach Accept Message
 */

typedef struct 
{
  mm_msg_head_T     msg_head;
} gmm_mt_detach_accept_msg_T;

/********************************************************************
 * P-TMSI Reallocation Complete Message
 */

typedef struct 
{
  mm_msg_head_T msg_head;
} gmm_ptmsi_realloc_complete_msg_T;

/********************************************************************
 * Authentication and Ciphering Response
 */

typedef struct 
{
  mm_msg_head_T     msg_head;
  byte AC_ref_num_spare;
  byte auth_param_resp[5];
  byte imeisv[11];
  byte auth_param_resp_ext[14];
} gmm_auth_ciph_response_msg_T;

/********************************************************************
 * Authentication and Ciphering Failure
 */

typedef struct 
{
  mm_msg_head_T     msg_head;
  byte GMM_cause;
  byte auth_failure_param[16];
} gmm_auth_ciph_failure_msg_T;

/********************************************************************
 * Identity Response
 */

typedef struct 
{
  mm_msg_head_T         msg_head;
  gmm_ie_LV_resp_mobile_id_T mobile_identity;
} gmm_identity_response_msg_T;

/********************************************************************
 * Routing area update request
 */

typedef struct 
{
  mm_msg_head_T msg_head;
  byte          update_type_cksn;
  byte          rai[6];
  byte          data [sizeof(gmm_ie_LV_ms_radio_access_caps_T)
                      + 4  /* Old P-TMSI Signature        */
                      + 2  /* Requested Ready timer value */
                      + 3  /* DRX parameter               */
                      + 1  /* TMSI status                 */
                      + 7  /* P_TMSI                      */
                      + 10 /* MS Network Capability       */
                      + 4  /* PDP Context Status          */
                      + 3  /* PS LCS Capability           */
                      + 18 /* MBMS Context Status         */
                      + 15 /* UE network capability*/
                      + 7  /*Additional Mobile Identity*/
                      + 8  /*Additional Old RAI */
                      + 3 /*Voice domain preference and UE's usage setting */
                      +1  /*PTMSI type IEI */
                      +7  /*Old LAI*/
                      +1  /*Netowrk Support feature type IEI */
                     ];
} gmm_ra_update_request_msg_T;

/********************************************************************
 * Routing area update complete
 */

typedef struct 
{
  mm_msg_head_T msg_head;
  byte          npdu_num_list[19];
} gmm_ra_update_complete_msg_T;

/********************************************************************
 * Service Request
 */

typedef struct 
{
  mm_msg_head_T msg_head;
  byte          cksn_service_type;
  byte          ptmsi[6];
  byte          data[4  /*PDP context status*/
                     + 18  /*MBMS context status*/
                     +4    /*Uplink data status*/
                    ]; 
} gmm_service_request_msg_T;

/********************************************************************
 * Mobile originated GMM Status
 */

typedef struct 
{
  mm_msg_head_T     msg_head;
  byte gmm_cause;
} gmm_mo_gmm_status_msg_T;

/******************************************************************* 
 *
 *                     GMM Network ==>> Mobile
 *
 *******************************************************************/

/********************************************************************
 * Attach Accept
 */

typedef struct 
{
  byte    attach_result;
  byte    force_to_standby;
  byte    periodic_ra_update_timer;
  byte    sms_radio_priority_nw;
  byte    routing_area_identification[6];
  boolean ptmsi_signature_present;
  byte    ptmsi_signature[3];
  boolean ready_timer_value_present;
#ifdef FEATURE_REL6_PS_FOP
  boolean follow_on_proceed_response;
#endif/* FEATURE_REL6_PS_FOP */
  byte    ready_timer_value;
  boolean allocated_ptmsi_present;
  byte    allocated_ptmsi[4];
  boolean ms_identity_present;
  gmm_ie_LV_mobile_id_T ms_identity;
  boolean gmm_cause_present;
  byte    gmm_cause;
  boolean t3302_value_present;
  byte    t3302_value;
  boolean cell_notification_present;
  boolean equiv_plmn_list_present;
  equiv_plmn_list_s_type equiv_plmn_list;
  boolean emergency_number_list_present;
  mmr_emergency_num_list_s_type emergency_number_list;
  boolean nw_ftr_support_present;
  byte    nw_ftr_support;

  byte    req_ms_info;
  boolean t3319_value_present;
  byte    t3319_value;  
#if defined(FEATURE_LTE)
  boolean t3323_value_present;
  byte    t3323_value;
#endif
#ifdef FEATURE_NAS_REL10
boolean ext_t3312_value_present;
byte    ext_t3312_value;
#endif

} gmm_attach_accept_msg_T;

/********************************************************************
 * Attach Reject
 */

typedef struct 
{
  byte    gmm_cause;
  boolean t3302_value_present;
  byte    t3302_value;
#ifdef FEATURE_NAS_REL10
  boolean t3346_value_present;
  byte    t3346_value;
#endif
} gmm_attach_reject_msg_T;

/********************************************************************
 * Mobile terminating Detach Request
 */

typedef struct 
{
  byte    detach_type;
  byte    force_to_standby;
  boolean gmm_cause_present;
  byte    gmm_cause;
} gmm_mt_detach_request_msg_T;

/********************************************************************
 * Mobile orginating Detach Accept
 */

typedef struct 
{
  byte force_to_standby;
} gmm_mo_detach_accept_msg_T;

/********************************************************************
 * P-TMSI reallocation command
 */

typedef struct 
{
  byte    allocated_ptmsi[4];
  byte    routing_area_identification[6];
  byte    force_to_standby;
  boolean ptmsi_signature_present;
  byte    ptmsi_signature[3];
} gmm_ptmsi_realloc_command_msg_T;

/********************************************************************
 * Authentication and Ciphering Request
 */

typedef struct 
{
  mm_msg_head_T     msg_head;
  byte ciph_algorithm_IMEISV_request;
  byte force_to_standby_AC_ref_num;
} gmm_auth_ciphering_request_msg_T;

/********************************************************************
 * Authentication and Ciphering Reject
 */

typedef struct 
{
  mm_msg_head_T     msg_head;
} gmm_auth_ciphering_reject_msg_T;

/********************************************************************
 * Identity Request
 */

typedef struct 
{
  byte identity_type;
  byte force_to_standby;
} gmm_identity_request_msg_T;

/********************************************************************
 * Routing Area Update Accept
 */

typedef struct 
{
  byte    force_to_standby;
  byte    update_result;
  byte    periodic_ra_update_timer;
  byte    routing_area_identification[6];
  boolean ptmsi_signature_present;
  byte    ptmsi_signature[3];
  boolean allocated_ptmsi_present;
  byte    allocated_ptmsi[4];
  boolean ms_identity_present;
  gmm_ie_LV_mobile_id_T ms_identity;
  boolean list_of_receive_npdu_numbers_present;
  boolean nsapi_valid_list[GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPIS];
  byte    npdu_number_list[GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPIS];
  boolean ready_timer_value_present;
  byte    ready_timer_value;
#ifdef FEATURE_REL6_PS_FOP
  boolean follow_on_proceed_response;
#endif/* FEATURE_REL6_PS_FOP */
  boolean gmm_cause_present;
  byte    gmm_cause;
  boolean t3302_value_present;
  byte    t3302_value;
  boolean cell_notification_present;
  boolean equiv_plmn_list_present;
  equiv_plmn_list_s_type equiv_plmn_list;
  mmr_emergency_num_list_s_type emergency_number_list;
  boolean emergency_number_list_present;
  boolean                       pdp_context_status_present;  
  gmm_ie_pdp_context_status_T   pdp_context_status;  
  boolean                       nw_ftr_support_present;
  byte                          nw_ftr_support;
  boolean                       mbms_context_status_present;    
  gmm_ie_mbms_context_status_T  mbms_context_status;
  
  byte    req_ms_info;
  boolean t3319_value_present;
  byte    t3319_value;    
#if defined(FEATURE_LTE)
  boolean t3323_value_present;
  byte    t3323_value;
#endif
#ifdef FEATURE_NAS_REL10
boolean ext_t3312_value_present;
byte    ext_t3312_value;
#endif
} gmm_ra_update_accept_msg_T;

/********************************************************************
 * Routing Area Update Reject
 */

typedef struct 
{
  byte    gmm_cause;
  byte    force_to_standby;
  boolean t3302_value_present;
  byte    t3302_value;
#ifdef FEATURE_NAS_REL10
  boolean t3346_value_present;
  byte    t3346_value;
#endif
} gmm_ra_update_reject_msg_T;

/********************************************************************
 * Network originated GMM Status
 */

typedef struct 
{
  byte gmm_cause;
} gmm_mt_gmm_status_msg_T;

/********************************************************************
 * Service Reject
 */

typedef struct 
{
  byte gmm_cause;
#ifdef FEATURE_NAS_REL10
  boolean t3346_value_present;
  byte    t3346_value;
#endif
} gmm_service_reject_msg_T;


/********************************************************************
 * Service Accept
 */

typedef struct 
{
  boolean                       pdp_context_status_present;  
  gmm_ie_pdp_context_status_T   pdp_context_status;  
  boolean                       mbms_context_status_present;    
  gmm_ie_mbms_context_status_T  mbms_context_status;     
} gmm_service_accept_msg_T;


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

extern void gmm_send_attach_request_message
(
  gmm_attach_type_type attach_type,
  auth_security_context_id cksn_context
) ;

extern void gmm_send_attach_complete_message( void );

extern void gmm_send_mo_detach_request_message
(
  gmm_detach_type_type detach_type,
  boolean              power_down
);

extern void gmm_send_mt_detach_accept_message( void );

extern void gmm_send_ptmsi_reallocation_complete_message( void );

extern void gmm_send_identity_response_message
(
  gmm_ie_LV_resp_mobile_id_T mobile_identity
);

extern void gmm_send_routing_area_update_request_message
( 
  gmm_ra_update_type_type ra_update_type,
  auth_security_context_id cksn_context
) ;

extern void gmm_send_routing_area_update_complete_message
(
  boolean                   receive_npdu_number_list_present,
  gmm_npdu_number_list_type *receive_npdu_number_list
);

extern void gmm_send_service_request_message
(
  rrc_establish_cause_e_type establish_cause,
  auth_security_context_id security_context,
  gmm_ie_pdp_context_status_T* uplink_data_status
);

extern void gmm_send_gmm_status_message( byte gmm_cause );

extern boolean gmm_valid_attach_accept_message
( 
  mm_cmd_type             *message,
  gmm_attach_accept_msg_T *attach_accept_msg
);

extern boolean gmm_valid_attach_reject_message
( 
  mm_cmd_type             *message,
  gmm_attach_reject_msg_T *attach_reject_msg
);

extern boolean gmm_valid_mt_detach_request_message
( 
  mm_cmd_type                 *message,
  gmm_mt_detach_request_msg_T *detach_request_msg
);

extern boolean gmm_valid_mo_detach_accept_message
( 
  mm_cmd_type                *message,
  gmm_mo_detach_accept_msg_T *detach_accept_msg
);

extern boolean gmm_valid_ptmsi_reallocation_command_message
( 
  mm_cmd_type                     *message,
  gmm_ptmsi_realloc_command_msg_T *ptmsi_reallocation_command_msg
);

extern boolean gmm_valid_identity_request_message
( 
  mm_cmd_type                *message,
  gmm_identity_request_msg_T *identity_request_msg
);

extern boolean gmm_valid_routing_area_update_accept_message
( 
  mm_cmd_type                *message,
  gmm_ra_update_accept_msg_T *ra_update_accept_msg
);

extern boolean gmm_valid_routing_area_update_reject_message
( 
  mm_cmd_type                *message,
  gmm_ra_update_reject_msg_T *ra_update_reject_msg
);

extern boolean gmm_valid_gmm_status_message
( 
  mm_cmd_type             *message,
  gmm_mt_gmm_status_msg_T *gmm_status_msg
);

extern boolean gmm_valid_service_accept_message
(
  mm_cmd_type              *message, 
  gmm_service_accept_msg_T *service_accept_msg 
);

extern boolean gmm_valid_service_reject_message
( 
  mm_cmd_type              *message,
  gmm_service_reject_msg_T *service_reject_msg
);

extern void gmm_decode_nw_ftr_support
(
  byte                     *ie_ptr,
  byte                     *nw_ftr_support
);

extern void gmm_parse_pdp_context_status_ie
(
  gmm_ie_pdp_context_status_T  *pdp_context_status,
  byte                         *msg_p
);

extern void gmm_parse_mbms_context_status_ie
(
  gmm_ie_mbms_context_status_T  *mbms_context_status,
  const byte                          *msg_p
);

extern void gmm_send_sm_pdp_mbms_status_ind_if_needed
(
  const gmm_ie_pdp_context_status_T  *pdp_context_status,
  boolean                      pdp_context_status_present,
  const gmm_ie_mbms_context_status_T *mbms_context_status,
  boolean                      mbms_context_status_present
);

extern void gmm_send_sm_pdp_mbms_status_ind
(
  const gmm_ie_pdp_context_status_T *pdp_context_status,
  const gmm_ie_mbms_context_status_T *mbms_context_status
);

extern void gmm_send_msm_mbms_supp_ind
(
  byte nw_feat_supp  
);

/*===========================================================================

FUNCTION GMM_TRANSLATE_ANC_REQ

DESCRIPTION
  This function translates the Authentication And Ciphering Request message
  from the OTA format into internal representation.

DEPENDENCIES
  None

RETURN VALUE
  This function returns TRUE in case if translation has been completed successfully
  and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean gmm_get_valid_anc_request_message
(
  /* Pointer to the incoming AUTHENTICATION AND CIPHERING REQUEST message */
  mm_cmd_type     *message,

  /* Pointer to valid AUTHENTICATION AND CIPERING REQUEST message parameters */
  gmm_anc_request_message_T *gmm_anc_request_message
) ;


/*===========================================================================

FUNCTION GMM_BUILD_ANC_FAILURE

DESCRIPTION
  This function builds AUTHENTICATION AND CIPHERING FAILURE message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gmm_build_anc_failure
(  
  /* Authentication error code */
  auth_error_type gmm_auth_error,
  
  /* AUTS value */
  auth_auts_type *gmm_auth_auts, 

  /* Generated AUTHENTICATION AND CIPHERING FAILURE message length */
  byte *gmm_anc_failure_message_length, 

  /* Generated AUTHENTICATION AND CIPHERING FAILURE message body */
  gmm_authentication_message_buffer_type *message_buffer
) ;


/*===========================================================================

FUNCTION GMM_BUILD_ANC_RESPONSE

DESCRIPTION
  This function builds the Authentication And Ciphering Response message
  according to TS 24.008 9.4.9

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gmm_build_anc_response
(
  /*pointer to Authentication Response parameter */
  auth_res_type *gmm_auth_res_ptr, 

  /* built message length */
  byte *gmm_anc_response_message_length, 

  /* built message buffer */
  gmm_authentication_message_buffer_type *message_buffer
) ;
extern boolean gmm_is_periodic_rau_accept_msg_valid
( 
  mm_cmd_type                *message
);

#endif /* GMM_MSG_H */
