/******************************************************************************
  @file    dsi_profile_3gpp2.h
  @brief   Definitions associated with profile related functions for 3GPP2. This
           is for internal use only

  DESCRIPTION
  

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009-2011 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/api/ds_profile_3gpp2i.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/10/11   jz      Add PDN Label 
10/27/11   op      Added support for PDN level authentication 
05/25/11   ack     Adding profile persistence option
04/29/11   ack     Write profile changes to efs
01/25/11   ss      SU API defeaturization and removal of customer.h.
01/21/11   mg      Added support for APN class field
01/17/11   ss      Added support for Iface linger.
10/20/10   op      Added support for PDN throttling timers in profiles 
09/30/09   mg      Created the module. First version of the file.
===========================================================================*/
#ifndef DS_PROFILE_3GPP2I_H
#define DS_PROFILE_3GPP2I_H

#include "ds_profile_3gpp2.h"
#include "ds_profile_tech_common.h"

/* Macro to check if identifier is valid */
#define DS_PROFILE_3GPP2_IDENT_IS_VALID( ident ) \
  ( ( ident >= DS_PROFILE_3GPP2_PROFILE_PARAM_MIN ) && \
    ( ident <= DS_PROFILE_3GPP2_PROFILE_PARAM_MAX ) )

#define PDN_THROTTLE_MAX_TIMERS 6

typedef uint64 dsi_profile_3gpp2_mask_type;

/* internal implementation for list node */
typedef struct
{
  ds_profile_num_type   num;
  char  name[DS_PROFILE_3GPP2_MAX_PROFILE_NAME_LEN+1];
} ds_profile_3gpp2_list_info_type;

/* bitmask to specify validity of parameters in a profile */
typedef enum
{
  DS_PROFILE_3GPP2_PROFILE_INVALID     = 0,
  DS_PROFILE_3GPP2_PROFILE_COMMON      = 1 << 0, // ONLY COMMON PARAMS ARE VALID
  DS_PROFILE_3GPP2_PROFILE_KDDI        = 1 << 1, // ONLY KDDI PARAMS ARE VALID
  DS_PROFILE_3GPP2_PROFILE_EHRPD       = 1 << 2, // ONLY EHRPD PARAMS ARE VALID
  DS_PROFILE_3GPP2_PROFILE_OMH         = 1 << 3, // ONLY OMH PARAMS ARE VALID
  DS_PROFILE_3GPP2_PROFILE_MAX         = 1 << 30
}ds_profile_3gpp2_valid_profile_enum_type;

typedef ds_profile_linger_params_type ds_profile_3gpp2_linger_params_type;

/* Internal data structure for storing data session profile parameter */
typedef struct
{
  ds_profile_3gpp2_profile_id           data_session_profile_id; 
                                    
  /* Data profile specific (common across all the builds) */
  /* ON or OFF default ON */
  ds_profile_3gpp2_negotiate_dns_server_type       negotiate_dns_server; 
  /* session close timer values need to be specified in sec */
  ds_profile_3gpp2_ppp_session_close_timer_DO_type ppp_session_close_timer_DO;
  ds_profile_3gpp2_ppp_session_close_timer_1X_type ppp_session_close_timer_1X;
  /* allowed or disallowed */
  ds_profile_3gpp2_allow_linger_type               allow_linger;
  /* linger timeout value */
  ds_profile_3gpp2_linger_timeout_val_type         linger_timeout_val;
  /* Expose only to KDDI though it is common to all carriers */
  /* Timeout values need to be specified in msec*/
  /* lcp C-Req Retry Timer */
  ds_profile_3gpp2_lcp_ack_timeout_type       lcp_ack_timeout; 
  /* ipcp C-Req Retry Timer */
  ds_profile_3gpp2_ipcp_ack_timeout_type      ipcp_ack_timeout; 
  /* PAP or CHAP Retry Timer */
  ds_profile_3gpp2_auth_timeout_type          auth_timeout; 
                                      
  /* number of retries */
  ds_profile_3gpp2_lcp_creq_retry_count_type  lcp_creq_retry_count; 
  ds_profile_3gpp2_ipcp_creq_retry_count_type ipcp_creq_retry_count; 
  ds_profile_3gpp2_auth_retry_count_type      auth_retry_count; 
                                      
  /* Which auth protocol is negotiated PAP or CHAP */
  ds_profile_3gpp2_auth_protocol_type         auth_protocol;
  char                                 user_id[DS_PROFILE_3GPP2_PPP_MAX_USER_ID_LEN + 1];
  uint8                                user_id_len;
  char                                 auth_password[DS_PROFILE_3GPP2_PPP_MAX_PASSWD_LEN + 1];
  uint8                                auth_password_len;
                                      
  /* high, medium or low */
  ds_profile_3gpp2_data_rate_type             data_rate; 
  /* hybrid or 1x */
  ds_profile_3gpp2_data_mode_type             data_mode; 
                                      
  /* OMH related params */            
  ds_profile_3gpp2_app_type_type              app_type;
  ds_profile_3gpp2_app_priority_type          app_priority;
                                      
  /* eHRPD related params */          
  char                                 apn_string[DS_PROFILE_3GPP2_APN_MAX_VAL_LEN + 1];
  uint8                                apn_string_len;
  char                                 pdn_label[DS_PROFILE_3GPP2_APN_MAX_VAL_LEN + 1];
  uint8                                pdn_label_len;
  ds_profile_3gpp2_apn_enabled         apn_enabled;
  ds_profile_3gpp2_apn_class           apn_class;
  ds_profile_3gpp2_pdn_type_enum_type   pdn_type;
  ds_profile_3gpp2_is_pcscf_addr_needed_type  is_pcscf_addr_needed; 
  ds_profile_3gpp2_in_addr_type               v4_dns_addr[DS_PROFILE_3GPP2_MAX_NUM_DNS_ADDR];
  ds_profile_3gpp2_in6_addr_type              v6_dns_addr[DS_PROFILE_3GPP2_MAX_NUM_DNS_ADDR];
  ds_profile_3gpp2_rat_type_enum_type   rat_type;
  ds_profile_3gpp2_pdn_inactivity_timeout     pdn_inactivity_timeout;
  uint32 failure_timer[PDN_THROTTLE_MAX_TIMERS];
  uint32 disallow_timer[PDN_THROTTLE_MAX_TIMERS];

  /* validity mask */
  ds_profile_3gpp2_valid_profile_enum_type profile_type;

  /* mask to indicate updated profile param */
  uint64 custom_val_mask; 
  boolean is_persistent;

  /* PDN level authentication parameters */
  uint8 pdn_level_auth_protocol;
  char  pdn_level_user_id[DS_PROFILE_3GPP2_PPP_MAX_USER_ID_LEN];
  uint8 pdn_level_user_id_len;
  char  pdn_level_auth_password[DS_PROFILE_3GPP2_PPP_MAX_PASSWD_LEN];
  uint8 pdn_level_auth_password_len;
  /*Operator Reserved pco container ID*/
  ds_profile_3gpp2_op_pco_id_type  op_pco_id;
  ds_profile_3gpp2_mcc_type mcc;
  ds_profile_3gpp2_mnc_type mnc;

  ds_profile_3gpp2_user_app_data user_app_data;

  boolean dns_addr_using_dhcp;
  boolean pcscf_addr_using_dhcp;
}ds_profile_3gpp2_profile_info_type;

/* Used internally to exchange 3GPP2 parameters */
typedef struct 
{
  dsi_profile_3gpp2_mask_type         mask;
  ds_profile_3gpp2_profile_info_type *prf;
  void                              *self;
}dsi_profile_3gpp2_type;

/* structure to store identifier and validity mask mapping */
typedef struct
{
  ds_profile_identifier_type ident;  
  ds_profile_3gpp2_valid_profile_enum_type valid_mask;  
}dsi_profile_3gpp2_params_valid_mask;

/*===========================================================================
FUNCTION GET_VALID_MASK_FROM_IDENT

DESCRIPTION
  This function returns the valid mask of the identifier.

PARAMETERS 
  param_id : identifier for which mask is to be returned 

DEPENDENCIES 
  
RETURN VALUE 
  returns the valid mask for the identifier
SIDE EFFECTS 
  
===========================================================================*/

ds_profile_3gpp2_valid_profile_enum_type get_valid_mask_from_ident( 
  ds_profile_identifier_type param_id 
);

/*===========================================================================
FUNCTION DSI_PROFILE_3GPP2_GET_INDEX_FROM_IDENT

DESCRIPTION
  This function gets index for the identifier to index into the function table.

PARAMETERS 
  ident : identifier for which index is to be returned

DEPENDENCIES 
  
RETURN VALUE 
  returns index
SIDE EFFECTS 
  
===========================================================================*/
uint8 dsi_profile_3gpp2_get_index_from_ident( 
  ds_profile_identifier_type ident
);

/*===========================================================================
FUNCTION DS_PROFILE_3GPP2_INIT

DESCRIPTION
  This function is called on the library init. It initializes the function
  table pointers to valid functions for 3gpp2

PARAMETERS 
  fntbl : pointer to function table

DEPENDENCIES 
  
RETURN VALUE 
  returns the mask for 3gpp2. (Used later as valid mask which is ORed value
  returned from all techs.
SIDE EFFECTS 
  
===========================================================================*/
uint8 ds_profile_3gpp2_init ( 
  tech_fntbl_type *fntbl 
);

extern void dsi_profile_get_profile_num_range (
  ds_profile_tech_etype tech,
  uint16               *min_num,
  uint16               *max_num
);

#endif /* DS_PROFILE_3GPP2I_H */
