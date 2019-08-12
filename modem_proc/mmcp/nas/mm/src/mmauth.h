#ifndef AUTH_H
#define AUTH_H
/*===========================================================================

              GMM AUTHENTICATION PROCEDURE's HEADER FILE

DESCRIPTION
  This file contains types, function prototypes and data declarations are 
  required in order to implement generic MM/GMM Authentication server.

Copyright (c) 2001,2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/auth.h_v   1.7   16 Jul 2002 14:44:28   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmauth.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/17/04   vdr      Added new definitions and prototypes which are needed in 
                    order to implement authentication cache memory
04/16/03   vdr      Added new AKA failure cause AUTH_AUTHENTICATION_UNACCEPTABLE
11/14/02   ks       Added extern boolean auth_gsm_gprs_keys_present
10/15/02   vdr      Changed Authentication server's data structure according
                    to the definitions of new GSDI interface
10/01/02   vdr      Changed Authentication Server interface
09/04/02   vdr      Removed Security Context ID definition 
08/10/02   mks      Added extern declaration for auth_domain_identity
07/16/02   vdr      Removed prototypes of the obsolete Kc & CKSN access functions
05/02/02   vdr      Added necessary access function prototypes
04/24/02   vdr      Added function prototypes in order to support HFN handling
03/27/02   vdr      Added temporary access to the GSM Kc and Kc Sequence Number
02/28/02   vdr      Added new member into Authentication Request propertie 
                    structures: current_auth_request_context.
02/04/03   ks       Added FEATURE_MMGSDI_UMTS for usim_authentication_req
                    in the sim_authentication_request_type structure.
03/18/05   sm       Added num_failures in the timer structure and 
                    AUTHENTICATION_NETWORK_FAILURE status
10/20/09   RI      Added non EPS access error for LTE.
12/18/09   RI      Updates for combined EFS/NV/USIM reads for multimode.
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
===========================================================================*/
/* <EJECT> */

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "mmcp_variation.h"
#include "comdef.h"
#include "timers_v.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "mm_v.h"
#include "mm_umts_v.h"

/* <EJECT> */

/*===========================================================================

                            MACROS

===========================================================================*/

/* Number of possible authentication clients */
#define MAX_REQUEST_STORAGE_SIZE 2

/* Invalid Ciphering Key Sequence Number */
#define DELETE_CKSN 0x07

/* Timer array length */
#define MAX_TIMER_ARRAY_LENGTH 4

#define MAX_AUTH_DATA_LENGTH 16

#define MM_HFN_SIZE 3

#define UMTS_SECURITY_KEY_LENGTH 16

#define GSM_SECURITY_KEY_LENGTH 8

#define GPRS_ALLOCATED_N_ACTIVATED_MASK 0x0C

#define NO_TIMER_EXPIRED 0xFF

#define FIELD_NOT_IN_USE 0xFF

#define UMTS_GENERATOR_KEY_LEN 32
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*
** Authentication Server's Error Codes
*/
typedef enum
{
  AUTH_NO_ERROR,
  AUTH_CAN_NOT_PERFORM_AUTHENTICATION,
  AUTH_SKIP_AUTHENTICATION_RESPONSE,
  AUTH_MAK_CODE_FAILURE = 20,
  AUTH_SQN_FAILURE = 21,
  AUTH_AUTHENTICATION_UNACCEPTABLE = 23
#ifdef FEATURE_LTE
  ,AUTH_NON_EPS_ACCESS = 26
#endif  // FEATURE_LTE
} auth_error_type ;

/* 
** Authentication transaction status 
*/
typedef enum
{
  AUTHENTICATION_IN_PROGRESS,
  AUTHENTICATION_DONE,
  AUTHENTICATION_NETWORK_FAILURE
} auth_status_type ;

typedef enum
{
  MM_CS_DOMAIN_CN_ID = RRC_CS_DOMAIN_CN_ID ,
  MM_PS_DOMAIN_CN_ID = RRC_PS_DOMAIN_CN_ID ,
  MM_NO_DOMAIN_IDENTITY
} mm_cn_domain_identity_type ;

typedef struct authentication_value_tag
{
  byte value_length ;
  byte value_data[MAX_AUTH_DATA_LENGTH] ;
} authentication_value_type ;

typedef authentication_value_type auth_rand_type ;

typedef authentication_value_type auth_autn_type ;

typedef authentication_value_type auth_res_type ;

typedef authentication_value_type auth_gprs_cipherkey_type ;

typedef authentication_value_type auth_gsm_cipherkey_type ;

typedef authentication_value_type auth_gprs_integritykey_type ;

typedef authentication_value_type auth_auts_type ;

typedef struct hfn_tag
{
   byte cs_hfn[MM_HFN_SIZE];     /* CS HFN   */
   byte ps_hfn[MM_HFN_SIZE];     /* PS HFN   */
   byte threshold[MM_HFN_SIZE];  /* HFN MAX  */
} hfn_type;

#ifdef FEATURE_NAS_REL11
typedef struct hfn_nv_stored_tag
{
  hfn_type current_hfn;
  byte  hotswap;
} hfn_nv_stored_type;
#endif
/* Pointer to a call back function that is called upon reception of
** the SIM or USIM card authentication response (GSDI Authentication 
** response)
**/
typedef
void (*auth_response_callback_type)
  (
    /* Authentication transaction status code */
    auth_status_type status_code,

    /* Error code */
    auth_error_type auth_error,

    /* Pointer to Authentication Response Value (RES) */
    auth_res_type *auth_res,

    /* Pointer to AUTS value */
    auth_auts_type *auth_auts
  ) ; /* end auth_response_callback() */

/* Authentication Client ID type */
typedef auth_response_callback_type client_id ;

/* Retx timers Client's definitions */
typedef struct auth_timer_list_tag
{
  boolean   timer_was_active  ;
  byte      timer             ;
} auth_timer_list_type ;

/* Authentication client's timers */
typedef struct auth_timer_set_tag
{
  byte mak_failure_timer ;
  byte sqn_failure_timer ;
  byte timer_list_length ;
  auth_timer_list_type timer_list[MAX_TIMER_ARRAY_LENGTH] ;
  byte num_failures;
} auth_timer_set_type ;

/*
** Authentication request properties
*/
typedef struct auth_request_properties_tag
{
  byte                        ciphering_key_sequence_number ;
  byte                        ciphering_algorithm ;
  rrc_cn_domain_identity_e_type cn_domain_identity ;
  auth_response_callback_type client_response_callback ;
  auth_timer_set_type         client_timer_set ;
  mmgsdi_autn_context_enum_type    current_auth_request_context ;
  byte                        cache_guard_timer_id ;
} auth_request_properties_type ;

/*
** Authentication Request command format
*/
typedef struct auth_request_storage_tag
{
  dword auth_request_reference_num ;
  auth_request_properties_type auth_request_properties ;
} auth_request_storage_type ;

/*===========================================================================

FUNCTION AUTH_DELETE_CKSN

DESCRIPTION
  This function deletes the loaded CKSN from the SIM and set the
  auth_security_contex_availability varible to NO_SECURITY_CONTEXT_AVAILABLE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Reset auth_security_contex_availability varible

===========================================================================*/
extern void auth_delete_cksn
(
  auth_security_context_id cksn_context
) ;


/*===========================================================================

FUNCTION      AUTH_AUTHENTICATION_REQUEST

DESCRIPTION
  This function saves incoming MM/GMM Authentication Request and sends 
  Authentication Request to the GSDI

DEPENDENCIES
  None

RETURN VALUE
  The function returns Client ID

SIDE EFFECTS
  None

===========================================================================*/
extern client_id auth_authentication_request
(
  /* Ciphering Key Sequence Number */
  byte                          ciphering_key_sequence_number,

  /* Pointer to the RAND value */
  auth_rand_type                *rand,

  /* Pointer to the AUTN value */
  auth_autn_type                *autn,

  /* Pointer to the MM/GMM Authentication Request properties */
  auth_request_properties_type  *request_properties
) ;


/*===========================================================================

FUNCTION      AUTH_HANDLE_SIM_AUTHENTICATION_RESPONSE

DESCRIPTION
  This function performs the actions that are required by the MM/GMM 
  authentication procedure after the SIM card returns its authentication 
  response

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void auth_handle_sim_authentication_response
(
  mm_cmd_type *message,          /* SIM card authentication response */
  dword        client_reference  /* Erroneous auth request reference number */
) ;


/*===========================================================================

FUNCTION      AUTH_HANDLE_TIMER_EXPIRY

DESCRIPTION
  This function performs the actions which are required by TS 24.008 when one
  of the authentication error handling timers (MAK or SQN timer) has been 
  expired.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_handle_timer_expiry
(
  /* Incoming timer expiration message */
  mm_cmd_type *message
) ;


/*===========================================================================

FUNCTION        AUTH_RESET_TRANSACTION

DESCRIPTION
  This function allows the MM or GMM authentication clients to reset its
  own authentication attempts.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_reset_transaction
(
  /* Authentication Client's ID */
  client_id id
) ;
/*===========================================================================

FUNCTION        AUTH_STOP_AUTH_FAIL_TIMERS

DESCRIPTION
  This function allows the MM or GMM authentication clients to stop the timers T3320/T3318

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_stop_auth_fail_timers
(
  /* Authentication Client's ID */
  client_id id
) ;

/*===========================================================================

FUNCTION         AUTH_STOP_AUTH_MAK_SQN_TIMERS

DESCRIPTION
  This function allows the MM, EMM or GMM authentication clients to stop the
  timers the MAK/SQN timers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_stop_auth_mak_sqn_timers
(
  /* Authentication Client's ID */
  client_id id
);

/*===========================================================================

FUNCTION      AUTH_SIM_WRITE_HFN

DESCRIPTION
  This function writes down the PS nad CS Hyper Frame Numbers on the SIM card

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_sim_write_hfn
(
  /* CS HFN output buffer */
  byte *cs_hfn,

  /* PS HFN output buffer */
  byte *ps_hfn,

  /* Domain identity */
  rrc_cn_domain_identity_e_type domain_identity
) ;

/*===========================================================================

FUNCTION      AUTH_GET_PS_HFN

DESCRIPTION
  This functions returns current PS Hyper Frame Number
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_get_ps_hfn
(
  /* Output buffer */
  byte *ps_hfn
) ;


/*===========================================================================

FUNCTION      AUTH_GET_CS_HFN

DESCRIPTION
  This functions returns current CS Hyper Frame Number
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_get_cs_hfn
(
  /* Output buffer */
  byte *cs_hfn
) ;

/*===========================================================================

FUNCTION    AUTH_HANDLE_NETWORK_REJECT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_handle_network_reject
(
  client_id id,
  byte timer_list_length,
  auth_timer_list_type *timer_list
) ;

/*===========================================================================

FUNCTION    AUTH_IS_CS_CKIK_DERIVED_FROM_GSMKC

DESCRIPTION
  This function checks whether the UMTS/TDS CS keys CKIK are derived from GSM KC or given by card.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If keys are derived from GSM KC
  FALSE - If keys are not derived and given by card

SIDE EFFECTS
  None
===========================================================================*/
extern boolean auth_is_cs_ckik_derived_from_gsmkc();

/*===========================================================================

FUNCTION    AUTH_GET_CIPHERING_KEY_SEQUENCE_NUMBER

DESCRIPTION
  This function returns current Ciphering Key Sequence Number is stored on the
  SIM/USIM card and saves this number into internal Authentication server 
  variable

DEPENDENCIES
  None

RETURN VALUE
  Ciphering Key Sequence Number

SIDE EFFECTS
  None
===========================================================================*/
extern byte auth_get_ciphering_key_sequence_number
(
  auth_security_context_id cksn_context
) ;

/*===========================================================================

FUNCTION        AUTH_DELETE_HFNS

DESCRIPTION
  This function deletes the CS and PS Hyper Frame numbers from the SIM card by
  setting CS and PS HFNs to be equal the HFN Threshold

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_delete_hfns
(
  void
) ;

/*===========================================================================

FUNCTION    AUTH_TRANSLATE_HFN

DESCRIPTION
  This function translate HFN value from interface into comparison able format 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern dword auth_translate_hfn
(
  byte *incoming_hfn  
) ;

/*===========================================================================

FUNCTION    AUTH_GET_HFN_THRESHOLD

DESCRIPTION
  This function returns the HFN threshhold is restored on the USIM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern dword auth_get_hfn_threshold
(
  void
) ;

/*===========================================================================

FUNCTION        AUTH_DELETE_CS_HFNS

DESCRIPTION
  This function deletes the CS Hyper Frame numbers from the USIM card by
  setting CS HFN to be equal the HFN Threshold

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_delete_cs_hfn
(
  void
) ;

/*===========================================================================

FUNCTION        AUTH_DELETE_PS_HFNS

DESCRIPTION
  This function deletes the PS Hyper Frame numbers from the USIM card by
  setting PS HFN to be equal the HFN Threshold

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_delete_ps_hfn
(
  void
) ;

/*===========================================================================
FUNCTION      AUTH_LOAD_CS_DOMAIN_KEYS

DESCRIPTION   load cs domain keys for sim/usim

DEPENDENCIES
  None

RETURN VALUE 
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void auth_load_cs_domain_keys
(
  void
);

/*===========================================================================
FUNCTION      AUTH_LOAD_PS_DOMAIN_KEYS

DESCRIPTION   load ps domain keys for sim/usim

DEPENDENCIES
  None

RETURN VALUE 
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void auth_load_ps_domain_keys
(
  void
);

/*===========================================================================

FUNCTION    AUTH_LOAD_SECURITY_CONTEXTE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean auth_load_security_context
(
  void
) ;

/*===========================================================================

FUNCTION    AUTH_HANDLE_HFN_RELEASE_IND

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_handle_hfn_release_ind
(
  byte *cs_hfn,
  byte *ps_hfn
) ;

/*===========================================================================

FUNCTION      AUTH_MM_PERFORM_CACHE_AUTHENTICATION

DESCRIPTION
  This function performs authentication in case if cache memory is valid and 
  incoming RAND is equal to the last one which has been successfully processed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean auth_mm_perform_cache_authentication
(
  byte rand_length,
  byte *rand_value,
  auth_response_callback_type authentication_response_builder
) ;
/*===========================================================================

FUNCTION      AUTH_GMM_PERFORM_CACHE_AUTHENTICATION

DESCRIPTION
  This function performs authentication in case if cache memory is valid and 
  incoming RAND is equal to the last one which has been successfully processed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean auth_gmm_perform_cache_authentication
(
  byte rand_length,
  byte *rand_value,
  auth_response_callback_type authentication_response_builder
) ;

/*===========================================================================

FUNCTION      AUTH_RESET_CACHE_MEMORY

DESCRIPTION
  This function resets the authentication cache memory 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void auth_reset_cache_memory
(
  boolean timer_expired,
  byte timer_id,
  rrc_cn_domain_identity_e_type cn_domain_id
) ;


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

/*
** GSM Ciphering Key
*/
extern auth_gsm_cipherkey_type auth_gsm_cipherkey_sim[MAX_NAS_STACKS];

/*
** GSM CKSN
*/
extern byte auth_gsm_cksn_sim[MAX_NAS_STACKS] ;

/*
** CS domain GPRS Ciphering Key
*/
extern auth_gprs_cipherkey_type auth_cs_gprs_cipherkey_sim[MAX_NAS_STACKS] ;

/*
** CS domain GPRS Integrity Key
*/
extern auth_gprs_integritykey_type auth_cs_gprs_integritykey_sim[MAX_NAS_STACKS] ;

/*
** CS domain GPRS CKSN
*/
extern byte auth_cs_gprs_cksn_sim[MAX_NAS_STACKS] ;

/*
** PS domain GPRS Ciphering Key
*/
extern auth_gprs_cipherkey_type auth_ps_gprs_cipherkey_sim[MAX_NAS_STACKS] ;

/*
** PS domain GPRS Integrity Key
*/
extern auth_gprs_integritykey_type auth_ps_gprs_integritykey_sim[MAX_NAS_STACKS] ;

/*
** PS domain GPRS CKSN
*/
extern byte auth_ps_gprs_cksn_sim[MAX_NAS_STACKS] ;

/* Selected GPRS ciphering algorithm */
extern byte auth_ps_ciphering_algorithm_sim[MAX_NAS_STACKS] ;

extern auth_gsm_cipherkey_type auth_gsm_gprs_cipherkey_sim[MAX_NAS_STACKS] ;

extern byte auth_gsm_gprs_cksn_sim[MAX_NAS_STACKS] ;

extern rrc_cn_domain_identity_e_type auth_domain_identity_sim[MAX_NAS_STACKS];

/* SIM Service Table Indicator For GPRS Service */
extern boolean auth_gsm_gprs_keys_present_sim[MAX_NAS_STACKS] ;

extern auth_gsm_cipherkey_type     gsm_kc_128bit_sim[MAX_NAS_STACKS] ;
extern auth_gsm_cipherkey_type     gsm_gprs_kc_128bit_sim[MAX_NAS_STACKS] ;


#define  auth_gsm_cipherkey auth_gsm_cipherkey_sim[mm_as_id]
#define  auth_gsm_cksn auth_gsm_cksn_sim[mm_as_id]
#define  auth_cs_gprs_cipherkey auth_cs_gprs_cipherkey_sim[mm_as_id]
#define  auth_cs_gprs_integritykey auth_cs_gprs_integritykey_sim[mm_as_id]
#define  auth_cs_gprs_cksn auth_cs_gprs_cksn_sim[mm_as_id]
#define  auth_ps_gprs_cipherkey auth_ps_gprs_cipherkey_sim[mm_as_id]
#define  auth_ps_gprs_integritykey auth_ps_gprs_integritykey_sim[mm_as_id]

#define  auth_gsm_gprs_cipherkey auth_gsm_gprs_cipherkey_sim[mm_as_id]

#define  auth_domain_identity auth_domain_identity_sim[mm_as_id]
#define  auth_gsm_gprs_keys_present auth_gsm_gprs_keys_present_sim[mm_as_id]

/* NV related data has to be always accessed using AS ID - NV Context ID mapping */

#define  auth_ps_gprs_cksn auth_ps_gprs_cksn_sim[mm_as_id]
#define  auth_gsm_gprs_cksn auth_gsm_gprs_cksn_sim[mm_as_id]
#define  auth_ps_ciphering_algorithm auth_ps_ciphering_algorithm_sim[mm_as_id]
#define  gsm_kc_bit128 gsm_kc_128bit_sim[mm_as_id]
#define  gsm_gprs_kc_bit128  gsm_gprs_kc_128bit_sim[mm_as_id]
#else

/* 
** GSM Ciphering Key
*/
extern auth_gsm_cipherkey_type auth_gsm_cipherkey;

/* 
** GSM CKSN
*/
extern byte auth_gsm_cksn ;

/* 
** CS domain GPRS Ciphering Key
*/
extern auth_gprs_cipherkey_type auth_cs_gprs_cipherkey ;

/* 
** CS domain GPRS Integrity Key
*/
extern auth_gprs_integritykey_type auth_cs_gprs_integritykey ;

/* 
** CS domain GPRS CKSN
*/
extern byte auth_cs_gprs_cksn ;

/* 
** PS domain GPRS Ciphering Key
*/
extern auth_gprs_cipherkey_type auth_ps_gprs_cipherkey ;

/* 
** PS domain GPRS Integrity Key
*/
extern auth_gprs_integritykey_type auth_ps_gprs_integritykey ;

/* 
** PS domain GPRS CKSN
*/
extern byte auth_ps_gprs_cksn ;

/* Selected GPRS ciphering algorithm */
extern byte auth_ps_ciphering_algorithm ;

extern auth_gsm_cipherkey_type auth_gsm_gprs_cipherkey ;

extern byte auth_gsm_gprs_cksn ;

extern auth_gsm_cipherkey_type  gsm_kc_bit128;
extern auth_gsm_cipherkey_type  gsm_gprs_kc_bit128;

extern rrc_cn_domain_identity_e_type auth_domain_identity;

/* SIM Service Table Indicator For GPRS Service */
extern boolean auth_gsm_gprs_keys_present ;

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/
void mm_auth_delete_rand(void);
void mm_auth_delete_res(void);
void gmm_auth_delete_rand(void);
void gmm_auth_delete_res(void);

auth_gprs_cipherkey_type mm_auth_get_ck( void );
auth_gprs_integritykey_type mm_auth_get_ik( void );

/*===========================================================================
FUNCTION      AUTH_UPDATE_SECURITY_KEYS

DESCRIPTION
  This function saves the GSM/UMTS security keys into internal database of
  the Authentication Server

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void auth_update_security_keys
(
  rrc_cn_domain_identity_e_type domain_identity,   /* Current domain ID */
  byte                          cur_cksn,
  byte                          ciphering_algorithm,
  auth_gprs_cipherkey_type      *gprs_cipherkey,
  auth_gsm_cipherkey_type       *gsm_cipherkey,
  auth_gprs_integritykey_type   *gprs_integritykey,
  auth_gsm_cipherkey_type       *gsm_kc_128bit_in
);


/*===========================================================================

FUNCTION      AUTH_GENERATE_KC

DESCRIPTION
  This function converts UMTS CK & IK into Kc according to the c3 conversion 
  function which is described in TS 33.102

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void auth_generate_kc
(
  /* Pointer to GPRS Ciphering key value */
  auth_gprs_cipherkey_type    *gprs_cipherkey,

  /* Pointer to GPRS Integrity key value */
  auth_gprs_integritykey_type *gprs_integritykey,

  /* Pointer to the GSM Ciphering Key value */
  auth_gsm_cipherkey_type     *gsm_cipherkey,
   /* Pointer to the GSM Ciphering Key value kc_128*/
  auth_gsm_cipherkey_type     *gsm_kc_128bit_in 
);

#if defined(FEATURE_LTE)
/*===========================================================================

FUNCTION      AUTH_SAVE_SECURITY_CONTEX

DESCRIPTION
  This function saves a security context that is currently active according
  to the type of active security context and SIM card mode (SIM / USIM)

DEPENDENCIES
  None

RETURN VALUE
  The function returns TRUE if the current security context has been saved
  successfully and FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean auth_save_security_context
(
  byte                          ciphering_key_sequence_number,
  rrc_cn_domain_identity_e_type domain_identity,
  auth_gprs_cipherkey_type      *gprs_cipherkey,
  auth_gprs_integritykey_type   *gprs_integritykey,
  auth_gsm_cipherkey_type       *gsm_cipherkey
);

/*===========================================================================

FUNCTION GMM_NOTIFY_LOWER_LAYER

DESCRIPTION
  This function is called when the Authentication procedure has been finished
  successfully, in order to configure ciphering subsystem of the RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void auth_notify_lower_layer
(
  auth_gprs_cipherkey_type    *gmm_cipherkey, /* GMM Ciphering key */
  auth_gprs_integritykey_type *gmm_integritykey,   /* GPRS Integrity key */
  auth_gsm_cipherkey_type     *gsm_cipherkey ,
  rrc_cn_domain_identity_e_type domain_identity,    /* Current domain ID */
  rrc_sim_update_e_type           sim_update_type
);

#endif

#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS

boolean auth_is_procedure_to_be_aborted(client_id  id);

extern void gmm_handle_sim_busy_ind(boolean cancel_auth);
extern void mm_handle_sim_busy_ind(boolean cancel_auth);
#endif
#if 0
void mm_auth_set_ik( auth_gprs_integritykey_type *ik );
void mm_auth_set_ck( auth_gprs_cipherkey_type *ck );
#endif 

#ifdef FEATURE_NAS_REL11
/*===========================================================================

FUNCTION        AUTH_RESET_CS_HFNS

DESCRIPTION
  This function resets the S Hyper Frame numbers from the USIM card

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_reset_cs_hfn(boolean update_to_lower_layer_req);
/*===========================================================================

FUNCTION        AUTH_RESET_PS_HFNS

DESCRIPTION
  This function resets the PS Hyper Frame numbers from the USIM card

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_reset_ps_hfn(boolean update_to_lower_layer_req);
/*===========================================================================

FUNCTION    MM_AUTH_UPDATE_HFNS_TO_SIM_CARD

DESCRIPTION
  This function saves the CS,PS Hyper Frame numbers to EF

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void auth_update_hfns_to_sim_card( boolean set_threshold );
#endif

/*==================================================================================
FUNCTION AUTH_DEACT_RETX_TIMER

DESCRIPTION
  This function deactivates the specified timer if it was running and stopped during
  Authentication failure.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===================================================================================*/
extern void auth_deact_retx_timer
(
  client_id    id,
  timer_id_T   timer_id
);

#endif /* #ifndef AUTH_H */




























