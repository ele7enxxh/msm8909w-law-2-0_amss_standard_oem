/******************************************************************************
  @file    ds_profile_3gpp2_utils.c
  @brief

  DESCRIPTION
  Utililities for implementation of 3GPP2 Profile Management. This is for
  internal use only.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009 - 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsprofile/src/ds_profile_3gpp2_utils.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/26/14   hr      Data+MMS - DSDA changes Phase 1.
04/14/14   hr      Add support for common PDN and uniform callback operations
04/04/14   hr      Klocwork Fixes
03/19/14   hr      Fix APN enabled flag default value 
12/18/13   hr      Created the module. First version of the file.
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "ds_profile_db.h"
#include "ds_profile_3gpp2_utils.h"
#include "ds_profile_os.h"
#include "ds_profile_3gpp2.h"
#include "data_msg.h"

/*===========================================================================
                              Macro Definitions
===========================================================================*/
#define PDN_THROTTLE_DEF_TIMER_1      0
#define PDN_THROTTLE_DEF_TIMER_2      0
#define PDN_THROTTLE_DEF_TIMER_3      0
#define PDN_THROTTLE_DEF_TIMER_4      0
#define PDN_THROTTLE_DEF_TIMER_5      0
#define PDN_THROTTLE_DEF_TIMER_6      0

/*===========================================================================
MACRO CONV_MIN_TO_MSEC()

DESCRIPTION
  This macro returns a msec value for the give value in min

PARAMETERS
  int16 - value in min

RETURN VALUE
  value in msec
===========================================================================*/
#define CONV_MIN_TO_MSEC(timer_val) ((timer_val) * 60000)


/*===========================================================================
MACRO CONV_MSEC_TO_MIN()

DESCRIPTION
  This macro returns a min value for the give value in msec

PARAMETERS
  int16 - value in msec

RETURN VALUE
  value in min
===========================================================================*/
#define CONV_MSEC_TO_MIN(timer_val) ((timer_val) / 60000)

/*===========================================================================
MACRO MASK_OF_IDENT()

DESCRIPTION
  This macro returns the custom_val_mask for a particluar ident

PARAMETERS

RETURN VALUE
===========================================================================*/
#define MASK_OF_IDENT(ident) (uint64)( (uint64)1 << ident )

/*===========================================================================
          3GPP2 PARAMS Table internal to DS_PROFILE_3GPP2_UTILS module
===========================================================================*/

/*-----------------------------------------------------------------------------
  Table for parameter and its validity mask
-----------------------------------------------------------------------------*/
static dsi_profile_3gpp2_params_valid_mask ds_profile_3gpp2_params_valid_mask[] = {
  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_INVALID,
   DS_PROFILE_3GPP2_PROFILE_INVALID},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_NEGOTIATE_DNS_SERVER,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_DO,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_1X,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_ALLOW_LINGER,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_LCP_ACK_TIMEOUT,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_IPCP_ACK_TIMEOUT,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_TIMEOUT,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_LCP_CREQ_RETRY_COUNT,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_IPCP_CREQ_RETRY_COUNT,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_RETRY_COUNT,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_PROTOCOL,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_USER_ID,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_PASSWORD,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_DATA_RATE,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_DATA_MODE,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_APP_TYPE,
   DS_PROFILE_3GPP2_PROFILE_OMH},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_APP_PRIORITY,
   DS_PROFILE_3GPP2_PROFILE_OMH},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_APN_STRING,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_TYPE,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_IS_PCSCF_ADDR_NEEDED,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_PRIMARY,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_SECONDARY,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_RAT_TYPE,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_LINGER_PARAMS,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_APN_ENABLED,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_INACTIVITY_TIMEOUT,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_APN_CLASS,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PROTOCOL,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LEVEL_USER_ID,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PASSWORD,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LABEL, 
    (DS_PROFILE_3GPP2_PROFILE_EHRPD)},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_1,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_2,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_3,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_4,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_5,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_6,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_1,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_2,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_3,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_4,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_5,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_6,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_OP_PCO_ID,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_MCC,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_MNC,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMERS,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMERS,
   DS_PROFILE_3GPP2_PROFILE_EHRPD},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_USER_APP_DATA,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG,
   DS_PROFILE_3GPP2_PROFILE_COMMON},

  {(ds_profile_identifier_type)
      DS_PROFILE_3GPP2_PROFILE_PARAM_DNS_DHCP_REQ_FLAG,
   DS_PROFILE_3GPP2_PROFILE_COMMON}
};

/*===========================================================================
                         INTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*=========================================================================== 
4
FUNCTION dsi_profile_3gpp2_validate_ group of functions

DESCRIPTION
  This set of functions are used to validate the profile parameters when
  creating a profile.

PARAMETERS
  profile : Base profile type with common params
  info

DEPENDENCIES

RETURN VALUE 
  DB_RESULT_SUCCESS 

SIDE EFFECTS
  none
===========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_invalid_ident
(
  const ds_profile_db_info_type               *info
)
{

  return DB_RESULT_ERR_3GPP2_INVALID_IDENT_FOR_PROFILE;
}/* dsi_profile_3gpp2_validate_invalid_ident */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_negotiate_dns_server
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_3gpp2_negotiate_dns_server_type negotiate_dns_server;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len >  sizeof(ds_profile_db_3gpp2_negotiate_dns_server_type) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&negotiate_dns_server, 0, 
         sizeof(ds_profile_db_3gpp2_negotiate_dns_server_type));
  memscpy((void*)&negotiate_dns_server, 
          sizeof(ds_profile_db_3gpp2_negotiate_dns_server_type),
          info->buf, 
          info->len);

  if( !(negotiate_dns_server == FALSE || negotiate_dns_server == TRUE) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_negotiate_dns_server */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_ppp_session_close_timer_DO
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( info->len > sizeof(ds_profile_db_3gpp2_ppp_session_close_timer_DO_type) ||
      ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_ppp_session_close_timer_DO */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_ppp_session_close_timer_1X
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( info->len > sizeof(ds_profile_db_3gpp2_ppp_session_close_timer_1X_type) ||
      ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_ppp_session_close_timer_1X */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_allow_linger
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_allow_linger_type allow_linger;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_allow_linger_type) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&allow_linger, 0, 
         sizeof(ds_profile_db_allow_linger_type));
  memscpy((void*)&allow_linger, 
          sizeof(ds_profile_db_allow_linger_type),
          info->buf,
          info->len);

  if( !(allow_linger == FALSE || allow_linger == TRUE) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_allow_linger */


/*=========================================================================*/
#if 0
static ds_profile_db_result_type dsi_profile_3gpp2_validate_linger_timeout_val
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(uint16) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_linger_timeout_val */
#endif

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_lcp_ack_timeout
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_3gpp2_lcp_ack_timeout_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_lcp_ack_timeout */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_ipcp_ack_timeout
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_3gpp2_ipcp_ack_timeout_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_ipcp_ack_timeout */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_auth_timeout
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_3gpp2_auth_timeout_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_auth_timeout */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_lcp_creq_retry_count
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_3gpp2_lcp_creq_retry_count_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_lcp_creq_retry_count */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_ipcp_creq_retry_count
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_3gpp2_ipcp_creq_retry_count_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_ipcp_creq_retry_count */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_auth_retry_count
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_3gpp2_auth_retry_count_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_auth_retry_count */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_auth_protocol
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_auth_protocol auth_protocol;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_auth_protocol) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&auth_protocol, 0,
         sizeof(ds_profile_db_auth_protocol));
  memscpy((void*)&auth_protocol, 
         sizeof(ds_profile_db_auth_protocol),
         info->buf, 
         info->len);

  if( !( (auth_protocol == DB_DATA_SESS_AUTH_PROTOCOL_PAP) ||
         (auth_protocol == DB_DATA_SESS_AUTH_PROTOCOL_CHAP) ||
         (auth_protocol == DB_DATA_SESS_AUTH_PROTOCOL_PAP_CHAP) ||
         (auth_protocol == DB_DATA_SESS_AUTH_PROTOCOL_NONE) ) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_auth_protocol */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_user_id
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > ( DS_PROFILE_DB_MAX_AUTH_USER_LEN + 1 ) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  if ( ( info->len == (DS_PROFILE_DB_MAX_AUTH_USER_LEN + 1) ) &&
       ( ((char*)info->buf)[DS_PROFILE_DB_MAX_AUTH_USER_LEN] != '\0' ) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_user_id */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_auth_password
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > ( DS_PROFILE_DB_MAX_AUTH_PASS_LEN + 1) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  if ( ( info->len == (DS_PROFILE_DB_MAX_AUTH_PASS_LEN + 1) ) &&
       ( ((char*)info->buf)[DS_PROFILE_DB_MAX_AUTH_PASS_LEN] != '\0' ) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_auth_password */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_data_rate
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_3gpp2_data_rate_type data_rate;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_3gpp2_data_rate_type) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&data_rate, 0, sizeof(ds_profile_db_3gpp2_data_rate_type));
  memscpy((void*)&data_rate, 
          sizeof(ds_profile_db_3gpp2_data_rate_type),
          info->buf, 
          info->len);

  if( !( (data_rate == DS_PROFILE_3GPP2_DATARATE_LOW) ||
         (data_rate == DS_PROFILE_3GPP2_DATARATE_MED) ||
         (data_rate == DS_PROFILE_3GPP2_DATARATE_HIGH) ) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_data_rate */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_data_mode
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_3gpp2_data_mode_type data_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_3gpp2_data_mode_type) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&data_mode, 0, sizeof(ds_profile_db_3gpp2_data_mode_type));
  memscpy((void*)&data_mode, 
          sizeof(ds_profile_db_3gpp2_data_mode_type),
          info->buf, 
          info->len);

  if( !( (data_mode == DS_PROFILE_3GPP2_DATAMODE_CDMA_HDR) ||
         (data_mode == DS_PROFILE_3GPP2_DATAMODE_CDMA_ONLY) ||
         (data_mode == DS_PROFILE_3GPP2_DATAMODE_HDR_ONLY) ) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_data_mode */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_app_type
(
  const ds_profile_db_info_type    *info
)
{

  return DB_RESULT_ERR_INVAL_OP;
}/* dsi_profile_3gpp2_validate_app_type */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_app_priority
(
  const ds_profile_db_info_type    *info
)
{

  return DB_RESULT_ERR_INVAL_OP;
}/* dsi_profile_3gpp2_validate_app_priority */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_pdn_type
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_3gpp2_pdn_type pdn_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_3gpp2_pdn_type) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&pdn_type, 0, sizeof(ds_profile_db_3gpp2_pdn_type));
  memscpy((void*)&pdn_type, sizeof(ds_profile_db_pdp_type),
          info->buf, info->len);

  if( !( (pdn_type == DB_3GPP2_PDN_TYPE_V4) ||
         (pdn_type == DB_3GPP2_PDN_TYPE_V6) ||
         (pdn_type == DB_3GPP2_PDN_TYPE_V4_V6) ||
         (pdn_type == DB_3GPP2_PDN_TYPE_UNSPEC) ) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_pdn_type */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_is_pcscf_addr_needed
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_req_pcscf_address_flag_type is_pcscf_addr_needed;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len >  sizeof(ds_profile_db_req_pcscf_address_flag_type) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&is_pcscf_addr_needed, 0, 
         sizeof(ds_profile_db_req_pcscf_address_flag_type));
  memscpy((void*)&is_pcscf_addr_needed, 
          sizeof(ds_profile_db_req_pcscf_address_flag_type),
          info->buf,
          info->len);

  if( !(is_pcscf_addr_needed == FALSE || is_pcscf_addr_needed == TRUE) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_is_pcscf_addr_needed */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_v4_dns_addr_primary
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_ip_addr_v4_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_v4_dns_addr_primary */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_v4_dns_addr_secondary
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_ip_addr_v4_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_v4_dns_addr_secondary */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_v6_dns_addr_primary
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_ip_addr_v6_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_v6_dns_addr_primary */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_v6_dns_addr_secondary
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_ip_addr_v6_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_v6_dns_addr_secondary */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_rat_type
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_3gpp2_rat_type rat_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_3gpp2_rat_type) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&rat_type, 0, sizeof(ds_profile_db_3gpp2_rat_type));
  memscpy((void*)&rat_type,
          sizeof(ds_profile_db_3gpp2_rat_type),
          info->buf,
          info->len);

  if( !( (rat_type == DB_3GPP2_RAT_TYPE_HRPD) ||
         (rat_type == DB_3GPP2_RAT_TYPE_EHRPD) ||
         (rat_type == DB_3GPP2_RAT_TYPE_HRPD_EHRPD) ) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_rat_type */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_apn_enabled
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_3gpp2_apn_enabled apn_enabled;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_3gpp2_apn_enabled) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&apn_enabled, 0, 
         sizeof(ds_profile_db_3gpp2_apn_enabled));
  memscpy((void*)&apn_enabled, 
          sizeof(ds_profile_db_3gpp2_apn_enabled),
          info->buf,
          info->len);

  if( !(apn_enabled == FALSE || apn_enabled == TRUE) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_apn_enabled */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_pdn_inactivity_timeout
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_3gpp2_pdn_inactivity_timeout pdn_inactivity_timeout;
  ds_profile_db_3gpp2_pdn_inactivity_timeout max_inactivity_timeout;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_3gpp2_pdn_inactivity_timeout) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&pdn_inactivity_timeout, 0, 
         sizeof(ds_profile_db_3gpp2_pdn_inactivity_timeout));
  memscpy((void*)&pdn_inactivity_timeout, 
          sizeof(ds_profile_db_3gpp2_pdn_inactivity_timeout),
          info->buf,
          info->len);

  memset((void*)&max_inactivity_timeout, 0xFF, 
         sizeof(ds_profile_db_3gpp2_pdn_inactivity_timeout));

  if ( CONV_MSEC_TO_MIN(max_inactivity_timeout) < pdn_inactivity_timeout )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_pdn_inactivity_timeout */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_apn_class
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_apn_class_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_apn_class */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_linger_params
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_linger_params_type linger_params;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_linger_params_type) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&linger_params, 0, 
         sizeof(ds_profile_db_linger_params_type));
  memscpy((void*)&linger_params, 
          sizeof(ds_profile_db_linger_params_type),
          info->buf,
          info->len);

  if( !( (linger_params.allow_linger_flag == TRUE) ||
         (linger_params.allow_linger_flag == FALSE) ))
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_linger_params */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_pdn_level_auth_protocol
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_auth_protocol pdn_level_auth_protocol;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_auth_protocol) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&pdn_level_auth_protocol, 0,
         sizeof(ds_profile_db_auth_protocol));
  memscpy((void*)&pdn_level_auth_protocol, 
          sizeof(ds_profile_db_auth_protocol),
          info->buf, 
          info->len);

  if( !( (pdn_level_auth_protocol == DB_DATA_SESS_AUTH_PROTOCOL_PAP) ||
         (pdn_level_auth_protocol == DB_DATA_SESS_AUTH_PROTOCOL_CHAP) ||
         (pdn_level_auth_protocol == DB_DATA_SESS_AUTH_PROTOCOL_NONE) ||
         (pdn_level_auth_protocol == DB_DATA_SESS_AUTH_PROTOCOL_PAP_CHAP) ) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_pdn_level_auth_protocol */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_pdn_level_user_id
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > ( DS_PROFILE_DB_MAX_AUTH_USER_LEN + 1 ) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  if ( ( info->len == (DS_PROFILE_DB_MAX_AUTH_USER_LEN + 1) ) &&
       ( ((char*)info->buf)[DS_PROFILE_DB_MAX_AUTH_USER_LEN] != '\0' ) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_pdn_level_user_id */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_pdn_level_auth_password
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > ( DS_PROFILE_DB_MAX_AUTH_PASS_LEN + 1 ) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  if ( ( info->len == (DS_PROFILE_DB_MAX_AUTH_PASS_LEN + 1) ) &&
       ( ((char*)info->buf)[DS_PROFILE_DB_MAX_AUTH_PASS_LEN] != '\0' ) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_pdn_level_auth_password */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_pdn_label
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > ( DS_PROFILE_DB_MAX_APN_NAME_LEN + 1 )||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  if ( ( info->len == (DS_PROFILE_DB_MAX_APN_NAME_LEN + 1) ) &&
       ( ((char*)info->buf)[DS_PROFILE_DB_MAX_APN_NAME_LEN] != '\0' ) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_pdn_label */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_failure_timer
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_3gpp2_failure_timer_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_failure_timer */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_disallow_timer
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_3gpp2_disallow_timer_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_disallow_timer */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_op_pco_id
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_op_pco_id_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_op_pco_id */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_mcc
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_mcc_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_mcc */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_mnc
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_mnc_type mnc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_mnc_type) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&mnc, 0, sizeof(ds_profile_db_mnc_type));
  memscpy((void*)&mnc, sizeof(ds_profile_db_mnc_type),
          info->buf, info->len);

  if(!((mnc.mnc_includes_pcs_digit == TRUE) ||
       (mnc.mnc_includes_pcs_digit == FALSE) ) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_mnc */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_failure_timers
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > DS_PROFILE_DB_3GPP2_FAILURE_TIMER_MAX * 
                     sizeof(ds_profile_db_3gpp2_failure_timer_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_failure_timers */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_disallow_timers
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > DS_PROFILE_DB_3GPP2_DISALLOW_TIMER_MAX * 
                     sizeof(ds_profile_db_3gpp2_disallow_timer_type) ||
       ( (info->buf == NULL) && (info->len != 0) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_disallow_timers */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_user_app_data
(
  const ds_profile_db_info_type    *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len > sizeof(ds_profile_db_user_profile_class_type) ||
       ( (info->buf == NULL) && (info->len != 0) ))
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_user_app_data */

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_dhcp_request_dns_addr
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_req_dhcp_dns_address_flag_type dns_addr_using_dhcp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len >  sizeof(ds_profile_db_req_dhcp_dns_address_flag_type) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&dns_addr_using_dhcp, 0, 
         sizeof(ds_profile_db_req_dhcp_dns_address_flag_type));
  memscpy((void*)&dns_addr_using_dhcp, 
          sizeof(ds_profile_db_req_dhcp_dns_address_flag_type),
          info->buf,
          info->len);

  if( !(dns_addr_using_dhcp == FALSE || dns_addr_using_dhcp == TRUE) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_dhcp_request_dns_addr*/

/*=========================================================================*/

static ds_profile_db_result_type dsi_profile_3gpp2_validate_dhcp_request_pcscf_addr
(
  const ds_profile_db_info_type    *info
)
{
  ds_profile_db_req_dhcp_pcscf_address_flag_type pcscf_addr_using_dhcp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info->len >  sizeof(ds_profile_db_req_dhcp_pcscf_address_flag_type) ||
       ( (info->buf == NULL) ) )
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset((void*)&pcscf_addr_using_dhcp, 0, 
         sizeof(ds_profile_db_req_dhcp_pcscf_address_flag_type));
  memscpy((void*)&pcscf_addr_using_dhcp, 
          sizeof(ds_profile_db_req_dhcp_pcscf_address_flag_type),
          info->buf,
          info->len);

  if( !(pcscf_addr_using_dhcp == FALSE || pcscf_addr_using_dhcp == TRUE) )
  {
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_validate_dhcp_request_pcscf_addr */


/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION DSI_PROFILE_3GPP2_UTIL_IDENT_VALIDATION

DESCRIPTION The function calls ident specific validation function.
    
PARAMETERS: ident : Identifier of the parameter to validate
            info  : Value of the parameter
 
DEPENDENCIES None
  
RETURN VALUE ds_profile_db_result_type
  
  
SIDE EFFECTS None
  
===========================================================================*/
ds_profile_db_result_type dsi_profile_3gpp2_util_ident_validation
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type*  info
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_FAIL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( ident )
  {
  case TECH_3GPP2_PROFILE_PARAM_NEGOTIATE_DNS_SERVER: 
     ret_val = dsi_profile_3gpp2_validate_negotiate_dns_server(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_DO:
     ret_val = dsi_profile_3gpp2_validate_ppp_session_close_timer_DO(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_1X:
     ret_val = dsi_profile_3gpp2_validate_ppp_session_close_timer_1X(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_ALLOW_LINGER:  
     ret_val = dsi_profile_3gpp2_validate_allow_linger(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_LCP_ACK_TIMEOUT:
     ret_val = dsi_profile_3gpp2_validate_lcp_ack_timeout(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_IPCP_ACK_TIMEOUT:
     ret_val = dsi_profile_3gpp2_validate_ipcp_ack_timeout(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_AUTH_TIMEOUT:
     ret_val = dsi_profile_3gpp2_validate_auth_timeout(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_LCP_CREQ_RETRY_COUNT:
     ret_val = dsi_profile_3gpp2_validate_lcp_creq_retry_count(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_IPCP_CREQ_RETRY_COUNT:
     ret_val = dsi_profile_3gpp2_validate_ipcp_creq_retry_count(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_AUTH_RETRY_COUNT:
     ret_val = dsi_profile_3gpp2_validate_auth_retry_count(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_AUTH_PROTOCOL:
     ret_val = dsi_profile_3gpp2_validate_auth_protocol(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_USER_ID:
     ret_val = dsi_profile_3gpp2_validate_user_id(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_AUTH_PASSWORD:
     ret_val = dsi_profile_3gpp2_validate_auth_password(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_DATA_RATE:
     ret_val = dsi_profile_3gpp2_validate_data_rate(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_DATA_MODE:
     ret_val = dsi_profile_3gpp2_validate_data_mode(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_APP_TYPE:
     ret_val = dsi_profile_3gpp2_validate_app_type(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_APP_PRIORITY:
     ret_val = dsi_profile_3gpp2_validate_app_priority(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_PDN_TYPE:
     ret_val = dsi_profile_3gpp2_validate_pdn_type(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_IS_PCSCF_ADDR_NEEDED:
     ret_val = dsi_profile_3gpp2_validate_is_pcscf_addr_needed(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY:
     ret_val = dsi_profile_3gpp2_validate_v4_dns_addr_primary(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY:
     ret_val = dsi_profile_3gpp2_validate_v4_dns_addr_secondary(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_PRIMARY:
     ret_val = dsi_profile_3gpp2_validate_v6_dns_addr_primary(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_SECONDARY:
     ret_val = dsi_profile_3gpp2_validate_v6_dns_addr_secondary(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_RAT_TYPE:
     ret_val = dsi_profile_3gpp2_validate_rat_type(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_APN_ENABLED:
     ret_val = dsi_profile_3gpp2_validate_apn_enabled(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_PDN_INACTIVITY_TIMEOUT:
     ret_val = dsi_profile_3gpp2_validate_pdn_inactivity_timeout(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_APN_CLASS:
     ret_val = dsi_profile_3gpp2_validate_apn_class(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_LINGER_PARAMS:
     ret_val = dsi_profile_3gpp2_validate_linger_params(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PROTOCOL:
     ret_val = dsi_profile_3gpp2_validate_pdn_level_auth_protocol(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_PDN_LEVEL_USER_ID:
     ret_val = dsi_profile_3gpp2_validate_pdn_level_user_id(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PASSWORD:
     ret_val = dsi_profile_3gpp2_validate_pdn_level_auth_password(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_PDN_LABEL:
     ret_val = dsi_profile_3gpp2_validate_pdn_label(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_1:
  case TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_2:
  case TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_3:
  case TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_4:
  case TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_5:
  case TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_6:
     ret_val = dsi_profile_3gpp2_validate_failure_timer(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_1:
  case TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_2:
  case TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_3:
  case TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_4:
  case TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_5:
  case TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_6:
     ret_val = dsi_profile_3gpp2_validate_disallow_timer(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_OP_PCO_ID:
     ret_val = dsi_profile_3gpp2_validate_op_pco_id(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_MCC:
     ret_val = dsi_profile_3gpp2_validate_mcc(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_MNC:
     ret_val = dsi_profile_3gpp2_validate_mnc(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMERS:
     ret_val = dsi_profile_3gpp2_validate_failure_timers(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMERS:
     ret_val = dsi_profile_3gpp2_validate_disallow_timers(info);
     break;
                                                
  case TECH_3GPP2_PROFILE_PARAM_USER_APP_DATA:
     ret_val = dsi_profile_3gpp2_validate_user_app_data(info);
     break;
                                                
  case TECH_3GPP2_PROFILE_PARAM_DNS_DHCP_REQ_FLAG:
     ret_val = dsi_profile_3gpp2_validate_dhcp_request_dns_addr(info);
     break;

  case TECH_3GPP2_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG:
     ret_val = dsi_profile_3gpp2_validate_dhcp_request_pcscf_addr(info);
     break;
	 
  default:
     ret_val = dsi_profile_3gpp2_validate_invalid_ident(info);
     break;
  }

  if (ret_val != DB_RESULT_SUCCESS)
  {
    DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                     "dsi_profile_3gpp2_validate param %d: error %d ",
                     ident, ret_val );
  }

  return ret_val;
}/* dsi_profile_3gpp2_util_ident_validation */

/*===========================================================================
FUNCTION DSI_PROFILE_DB_3GPP2_UTIL_GEN_IDENT_VALIDATION

DESCRIPTION  The function calls ident specific validation function.
    
PARAMETERS   profile : Ptr to the profile prameter
 
DEPENDENCIES None
  
RETURN VALUE ds_profile_db_result_type
  
SIDE EFFECTS None
  
===========================================================================*/
ds_profile_db_result_type dsi_profile_3gpp2_util_gen_ident_validation
(
   const ds_profile_db_3gpp2_tech_params_type *profile
)
{
  ds_profile_db_ip_version ip_version;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(profile == NULL)
  {
    return DB_RESULT_FAIL;
  }

  ip_version = profile->tech_common_params.dns_addr.primary_ip_version;
  if( ( ip_version != DB_IP_V4) &&
      ( ip_version != DB_IP_V6) &&
      ( ip_version != DB_IP_V4V6) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "dsi_profile_3gpp2_gen_ident_validation: "
                     "Invalid Primary IP version" );
    return DB_RESULT_FAIL;
  }

  ip_version = profile->tech_common_params.dns_addr.secondary_ip_version;
  if( ( ip_version != DB_IP_V4) &&
      ( ip_version != DB_IP_V6) &&
      ( ip_version != DB_IP_V4V6) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "dsi_profile_3gpp2_gen_ident_validation: "
                     "Invalid Secondary IP version" );
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}/* dsi_profile_3gpp2_util_gen_ident_validation */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_UTIL_GET_DEFAULT_PARAMS

DESCRIPTION   Copy default values to all tech specific params

PARAMETERS    tech_params    : ptr to the tech specific params 
              ppp_params_ptr : default PPP params

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_util_get_default_params
(
  ds_profile_db_3gpp2_tech_params_type *tech_params, 
  data_sess_profile_ppp_params_type    *ppp_params_ptr
)
{
  uint32 default_timer_val[PDN_THROTTLE_MAX_TIMERS] = {
                                                   PDN_THROTTLE_DEF_TIMER_1,
                                                   PDN_THROTTLE_DEF_TIMER_2,
                                                   PDN_THROTTLE_DEF_TIMER_3,
                                                   PDN_THROTTLE_DEF_TIMER_4,
                                                   PDN_THROTTLE_DEF_TIMER_5,
                                                   PDN_THROTTLE_DEF_TIMER_6 };
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( tech_params == NULL)
  {
    return;
  }

  memset((void*)tech_params, 0, sizeof(ds_profile_db_3gpp2_tech_params_type));
  
  /*----------------------------------------------------------------------- 
    tech_common_params
   -----------------------------------------------------------------------*/
  /*---------------------------------------------------------------------- 
    TECH SPECIFIC apn_disable_flag is equal to apn_enabled in cache
   -----------------------------------------------------------------------*/
  tech_params->tech_common_params.apn_disable_flag = TRUE;

  tech_params->tech_common_params.apn_class = 0;

  //Skip memsetting dns addresses to 0 since done at beginning

  tech_params->tech_common_params.iface_linger_params.linger_params.allow_linger_flag
     = FALSE;

  tech_params->tech_common_params.iface_linger_params.linger_params.linger_timeout_val
     = 0;

  tech_params->tech_common_params.request_pcscf_address_flag = FALSE;

  tech_params->tech_common_params.auth_info.auth_type
     = DB_DATA_SESS_AUTH_PROTOCOL_PAP_CHAP;

  tech_params->tech_common_params.auth_info.username[0] = '\0';

  tech_params->tech_common_params.auth_info.password[0] = '\0';

  tech_params->tech_common_params.op_pco_id = 0;

  tech_params->tech_common_params.mcc = DS707_DATA_SESS_INITIAL_MCCMNC_VALUE;

  tech_params->tech_common_params.mnc.mnc_digits 
     = DS707_DATA_SESS_INITIAL_MCCMNC_VALUE;

  tech_params->tech_common_params.mnc.mnc_includes_pcs_digit = FALSE;

  tech_params->tech_common_params.dns_addr.primary_ip_version = DB_IP_V4V6;

  tech_params->tech_common_params.dns_addr.secondary_ip_version = DB_IP_V4V6;

  tech_params->tech_common_params.iface_linger_params.is_valid = FALSE;

  tech_params->tech_common_params.user_app_data = 0xFFFFFFFF;

  tech_params->tech_common_params.request_dns_address_using_dhcp_flag = FALSE;

  tech_params->tech_common_params.request_pcscf_address_using_dhcp_flag = FALSE;

  /*---------------------------------- 
    3GPP2 specific params
   ----------------------------------*/
  tech_params->profile_type = DB_3GPP2_PROFILE_COMMON | DB_3GPP2_PROFILE_EHRPD;

  if(ds707_roaming_is_curr_mode_jcdma()||
      ds707_roaming_is_simplified_jcdma_mode())
  {
    tech_params->profile_type |= DB_3GPP2_PROFILE_KDDI;
  }

  tech_params->negotiate_dns_server = TRUE;

  tech_params->ppp_session_close_timer_DO 
     = ds707_gonulltmr_get_timer(DS707_GONULLTMRI_DO);

  tech_params->ppp_session_close_timer_1X 
     = ds707_gonulltmr_get_timer(DS707_GONULLTMRI_1X);

  tech_params->lcp_ack_timeout = ppp_params_ptr->lcp_ack_timeout;

  tech_params->ipcp_ack_timeout = ppp_params_ptr->ipcp_ack_timeout;

  tech_params->auth_timeout = ppp_params_ptr->auth_timeout;

  tech_params->lcp_creq_retry_count = ppp_params_ptr->lcp_creq_retry_count;

  tech_params->ipcp_creq_retry_count = ppp_params_ptr->ipcp_creq_retry_count;

  tech_params->auth_retry_count = ppp_params_ptr->auth_retry_count;

  tech_params->data_rate = DS707_DATARATE_HIGH;

  tech_params->data_mode = DS707_DATAMODE_CDMA_HDR;

  /* skip app_type and app_priority, applicable to NVRUIM profiles only */

  tech_params->pdn_label[0] = '\0';

#ifdef FEATURE_DATA_EHRPD_DUAL_IP
  tech_params->pdn_type = DB_3GPP2_PDN_TYPE_V4_V6;
#else
  tech_params->pdn_type = DB_3GPP2_PDN_TYPE_V4;
#endif /* FEATURE_DATA_EHRPD_DUAL_IP  */

  //TODO: For DB profiles, do we still have HRPD_EHRPD rat_type?
  tech_params->rat_type = DB_3GPP2_RAT_TYPE_HRPD_EHRPD;

  tech_params->pdn_inactivity_timeout = 0;

  memscpy((void*)tech_params->failure_timer,
          sizeof(ds_profile_db_3gpp2_failure_timer_type) *
             DS_PROFILE_DB_3GPP2_PDN_THROTTLE_MAX_TIMERS,
          (void*)default_timer_val,
          sizeof(uint32)*PDN_THROTTLE_MAX_TIMERS);

  memscpy((void*)tech_params->disallow_timer,
          sizeof(ds_profile_db_3gpp2_disallow_timer_type) *
             DS_PROFILE_DB_3GPP2_PDN_THROTTLE_MAX_TIMERS,
          (void*)default_timer_val,
          sizeof(uint32)*PDN_THROTTLE_MAX_TIMERS);

  tech_params->pdn_level_auth_info.auth_type = DB_DATA_SESS_AUTH_PROTOCOL_NONE;

  tech_params->pdn_level_auth_info.username[0] = '\0';

  tech_params->pdn_level_auth_info.password[0] = '\0';

}/* ds_profile_3gpp2_util_get_default_params */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_UTIL_DB_TO_CACHE_TYPE_CONVERT

DESCRIPTION   This function converts a profile from profile DB type to 
              3GPP2 MH cache type.

PARAMETERS    gen_params            : C type general parameters
              tech_params           : C type tech specific parameters
              common_params         : C type common parameters for EPC profile
              data_sess_profile     : ptr to cache

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_util_db_to_cache_type_convert
(
  ds_profile_db_gen_params_type         *gen_params,
  ds_profile_db_3gpp2_tech_params_type  *tech_params,
  ds_profile_db_common_tech_params_type *common_params,
  ds707_data_session_profile_info_type  *data_sess_profile
)
{
  if (data_sess_profile == NULL)
  {
    return;
  }

  /*---------------------------------- 
     gen_params
   ----------------------------------*/
  if(gen_params != NULL)
  {
    data_sess_profile->data_session_profile_id = gen_params->profile_number;
  
    data_sess_profile->is_persistent = gen_params->persistent;
  
    memscpy((void*)data_sess_profile->apn_string,
            DS_VSNCP_3GPP2_APN_MAX_VAL_LEN,
            (void*)gen_params->apn_name,
            DS_PROFILE_DB_MAX_APN_NAME_LEN + 1);
  
    data_sess_profile->apn_string_len = strlen(gen_params->apn_name);
  
    data_sess_profile->supported_tech_mask = gen_params->tech_mask;
      
    data_sess_profile->ipv6_delegation = gen_params->ipv6_delegation;

    data_sess_profile->clat_enabled = gen_params->clat_enabled;

    /*--------------------------------------------------------------------
      If EFS profile doesn't have subs id, DB will send no change and may
      cause bringup failure. So use DDS here. This issue will be fixed in
      DB by hotswap feature.
     --------------------------------------------------------------------*/
    if ((gen_params->subscription_id > DB_ACTIVE_SUBSCRIPTION_NO_CHANGE) &&
        (gen_params->subscription_id < DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX))
    {
      data_sess_profile->subs_id = 
        (ds_profile_subs_id_enum_type)gen_params->subscription_id;
    }
    else
    {
      data_sess_profile->subs_id = ACTIVE_SUBSCRIPTION_1;
    }
  }

  if(tech_params != NULL)
  {
    /*---------------------------------- 
      tech_common_params
     ----------------------------------*/
    /*--------------------------------------------------------------------
      apn_flag is opposite in TECH SPECIFIC vs TECH_COMMON param.
     -------------------------------------------------------------------*/
    if ( ( common_params == NULL ) ||
         ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(APN_ENABLED) ) )
    {
      data_sess_profile->apn_flag = 
         tech_params->tech_common_params.apn_disable_flag;
    }
    else
    {
      data_sess_profile->apn_flag = !(common_params->apn_disable_flag);
    }

    if ( ( common_params == NULL ) ||
         ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(APN_CLASS) ) )
    {
      data_sess_profile->apn_class = tech_params->tech_common_params.apn_class;
    }
    else
    {
      data_sess_profile->apn_class = common_params->apn_class;
    }

    if ( ( common_params == NULL ) ||
         ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(PRIMARY_DNS_ADDR_V4) ) )
    {
      memscpy((void*)(&data_sess_profile->v4_dns_addr[PRIMARY_DNS_ADDR]),
              sizeof(uint32),
              (void*)(&tech_params->tech_common_params.dns_addr.primary_addr.v4),
              sizeof(ds_profile_db_ip_addr_v4_type));
    }
    else
    {
      memscpy((void*)(&data_sess_profile->v4_dns_addr[PRIMARY_DNS_ADDR]),
              sizeof(uint32),
              (void*)(&common_params->dns_addr.primary_addr.v4),
              sizeof(ds_profile_db_ip_addr_v4_type));
    }

    if ( ( common_params == NULL ) ||
         ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(PRIMARY_DNS_ADDR_V6) ) )
    {
      memscpy((void*)(&data_sess_profile->v6_dns_addr[PRIMARY_DNS_ADDR]),
              sizeof(uint32)*4, 
              (void*)(&tech_params->tech_common_params.dns_addr.primary_addr.v6),
              sizeof(ds_profile_db_ip_addr_v6_type));
    }
    else
    {
      memscpy((void*)(&data_sess_profile->v6_dns_addr[PRIMARY_DNS_ADDR]),
              sizeof(uint32)*4, 
              (void*)(&common_params->dns_addr.primary_addr.v6),
              sizeof(ds_profile_db_ip_addr_v6_type));
    }

    if ( ( common_params == NULL ) ||
         ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(SECONDARY_DNS_ADDR_V4) ) )
    {
      memscpy((void*)(&data_sess_profile->v4_dns_addr[SECONDARY_DNS_ADDR]),
              sizeof(uint32),
              (void*)(&tech_params->tech_common_params.dns_addr.secondary_addr.v4),
              sizeof(ds_profile_db_ip_addr_v4_type));
    }
    else
    {
      memscpy((void*)(&data_sess_profile->v4_dns_addr[SECONDARY_DNS_ADDR]),
              sizeof(uint32),
              (void*)(&common_params->dns_addr.secondary_addr.v4),
              sizeof(ds_profile_db_ip_addr_v4_type));
    }

    if ( ( common_params == NULL ) ||
         ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(SECONDARY_DNS_ADDR_V6) ) )
    {
      memscpy((void*)(&data_sess_profile->v6_dns_addr[SECONDARY_DNS_ADDR]),
              sizeof(uint32)*4, 
              (void*)(&tech_params->tech_common_params.dns_addr.secondary_addr.v6),
              sizeof(ds_profile_db_ip_addr_v6_type));
    }
    else
    {
      memscpy((void*)(&data_sess_profile->v6_dns_addr[SECONDARY_DNS_ADDR]),
              sizeof(uint32)*4, 
              (void*)(&common_params->dns_addr.secondary_addr.v6),
              sizeof(ds_profile_db_ip_addr_v6_type));
    }

    if(( common_params == NULL ) ||
       ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(ALLOW_LINGER) ))    
    {
      data_sess_profile->allow_linger = 
         tech_params->tech_common_params.iface_linger_params.linger_params.allow_linger_flag;
    }
    else
    {
      data_sess_profile->allow_linger = 
         common_params->iface_linger_params.linger_params.allow_linger_flag;
    }

    if(( common_params == NULL ) ||
       ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(LINGER_TIMEOUT_VAL) ))
    {
      data_sess_profile->linger_timeout_val = 
         tech_params->tech_common_params.iface_linger_params.linger_params.linger_timeout_val;
    }
    else
    {
      data_sess_profile->linger_timeout_val = 
         common_params->iface_linger_params.linger_params.linger_timeout_val;
    }

    if(( common_params == NULL ) ||
       ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(PCSCF_ADDR_NEEDED) ))
    {
      data_sess_profile->is_pcscf_addr_needed = 
         tech_params->tech_common_params.request_pcscf_address_flag;
    }
    else
    {
      data_sess_profile->is_pcscf_addr_needed = 
        common_params->request_pcscf_address_flag;
    }

    if(( common_params == NULL ) ||
       ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(AUTH_PROTOCOL) ))
    {
      data_sess_profile->auth_protocol = 
         tech_params->tech_common_params.auth_info.auth_type;
    }
    else
    {
      data_sess_profile->auth_protocol = common_params->auth_info.auth_type;
    }

    if(( common_params == NULL ) ||
       ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(PPP_USER_ID) ))
    {
      memscpy((void*)data_sess_profile->user_id,
              PPP_MAX_USER_ID_LEN,
              (void*)tech_params->tech_common_params.auth_info.username,
              DS_PROFILE_DB_MAX_AUTH_USER_LEN + 1);

      data_sess_profile->user_id_len = 
         strlen(tech_params->tech_common_params.auth_info.username);
    }
    else
    {
      memscpy((void*)data_sess_profile->user_id,
              PPP_MAX_USER_ID_LEN,
              (void*)common_params->auth_info.username,
              DS_PROFILE_DB_MAX_AUTH_USER_LEN + 1);

      data_sess_profile->user_id_len = 
        strlen(common_params->auth_info.username);
    }

    if(( common_params == NULL ) ||
       ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(PPP_PASSWORD) ))
    {
      memscpy((void*)data_sess_profile->auth_password,
              PPP_MAX_PASSWD_LEN,
              (void*)tech_params->tech_common_params.auth_info.password,
              DS_PROFILE_DB_MAX_AUTH_PASS_LEN + 1);

      data_sess_profile->auth_password_len = 
        strlen(tech_params->tech_common_params.auth_info.password);
    }
    else
    {
      memscpy((void*)data_sess_profile->auth_password,
              PPP_MAX_PASSWD_LEN,
              (void*)common_params->auth_info.password,
              DS_PROFILE_DB_MAX_AUTH_PASS_LEN + 1);

      data_sess_profile->auth_password_len = 
        strlen(common_params->auth_info.password);
    }

    if(( common_params == NULL ) ||
       ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(OP_PCO_ID) ))
    {
      data_sess_profile->op_pco_id = 
         tech_params->tech_common_params.op_pco_id;
    }
    else
    {
      data_sess_profile->op_pco_id = common_params->op_pco_id;
    }

    if( ( common_params == NULL ) ||
        ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(MCC)) )
    {
      data_sess_profile->mcc = tech_params->tech_common_params.mcc;
    }
    else
    {
      data_sess_profile->mcc = common_params->mcc;
    } 

    if( ( common_params == NULL ) ||
        ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(MNC) ) )
    {
      data_sess_profile->mnc.mnc = 
         tech_params->tech_common_params.mnc.mnc_digits;
    }
    else
    {
      data_sess_profile->mnc.mnc = common_params->mnc.mnc_digits;
    }

    if(( common_params == NULL ) ||
       ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(MNC) ))
    {
      data_sess_profile->mnc.mnc_includes_pcs_digit = 
         tech_params->tech_common_params.mnc.mnc_includes_pcs_digit;
    }
    else
    {
      data_sess_profile->mnc.mnc_includes_pcs_digit = 
        common_params->mnc.mnc_includes_pcs_digit;
    }

    if(( common_params == NULL ) ||
       ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(USER_APP_DATA) ))
    {
      data_sess_profile->user_app_data = 
        tech_params->tech_common_params.user_app_data;
    }
    else
    {
      data_sess_profile->user_app_data = common_params->user_app_data;
    }

    if(( common_params == NULL ) ||
       ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(PDN_IP_VERSION_TYPE) ))
    {
      /*--------------------------------------------------------------------
        PDN type is moved to common params, but DB will have two slots for
        this param to mask the difference between 3GPP and 3GPP2 allowed
        values. Thus, we take the tech specif value and common value from
        different structures
       --------------------------------------------------------------------*/
      data_sess_profile->pdn_type = (ds707_data_sess_pdn_type_enum_type)tech_params->pdn_type;
    }
    else
    {
      data_sess_profile->pdn_type = (ds707_data_sess_pdn_type_enum_type)common_params->pdp_type;
    }

    if(( common_params == NULL ) ||
       ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(DNS_DHCP_REQ_FLAG) ))
    {
      data_sess_profile->dns_addr_using_dhcp = 
         tech_params->tech_common_params.request_dns_address_using_dhcp_flag;
    }
    else
    {
      data_sess_profile->dns_addr_using_dhcp= 
        common_params->request_dns_address_using_dhcp_flag;
    }

    if(( common_params == NULL ) ||
       ( data_sess_profile->custom_val_mask & MASK_OF_IDENT(PCSCF_DHCP_REQ_FLAG) ))
    {
      data_sess_profile->pcscf_addr_using_dhcp = 
         tech_params->tech_common_params.request_pcscf_address_using_dhcp_flag;
    }
    else
    {
      data_sess_profile->pcscf_addr_using_dhcp = 
        common_params->request_pcscf_address_using_dhcp_flag;
    }

    /*------------------------------------------ 
     Profile DB internal params
     ------------------------------------------*/

    data_sess_profile->ip_version[PRIMARY_DNS_ADDR] = 
       (ds_profile_ip_version_enum_type)tech_params->tech_common_params.dns_addr.primary_ip_version;

    data_sess_profile->ip_version[SECONDARY_DNS_ADDR] = 
       (ds_profile_ip_version_enum_type)tech_params->tech_common_params.dns_addr.secondary_ip_version;

    data_sess_profile->is_linger_params_valid = 
       tech_params->tech_common_params.iface_linger_params.is_valid;

    /*---------------------------------- 
      3GPP2 specific params
     ----------------------------------*/
    data_sess_profile->profile_type = (ds_profile_type_enum_type)tech_params->profile_type;

    data_sess_profile->negotiate_dns_server = tech_params->negotiate_dns_server;

    data_sess_profile->ppp_session_close_timer_DO = 
      tech_params->ppp_session_close_timer_DO;

    data_sess_profile->ppp_session_close_timer_1X = 
      tech_params->ppp_session_close_timer_1X;

    data_sess_profile->lcp_ack_timeout = tech_params->lcp_ack_timeout;

    data_sess_profile->ipcp_ack_timeout = tech_params->ipcp_ack_timeout;

    data_sess_profile->auth_timeout = tech_params->auth_timeout;

    data_sess_profile->lcp_creq_retry_count = tech_params->lcp_creq_retry_count;

    data_sess_profile->ipcp_creq_retry_count = 
      tech_params->ipcp_creq_retry_count;

    data_sess_profile->auth_retry_count = tech_params->auth_retry_count;

    data_sess_profile->data_rate = tech_params->data_rate;

    data_sess_profile->data_mode = tech_params->data_mode;

    data_sess_profile->app_type = tech_params->app_type;

    data_sess_profile->app_priority = tech_params->app_priority;

    memscpy((void*)data_sess_profile->pdn_label,
            DS_VSNCP_3GPP2_APN_MAX_VAL_LEN,
            (void*)tech_params->pdn_label,
            DS_PROFILE_DB_MAX_APN_NAME_LEN + 1);

    data_sess_profile->pdn_label_len = strlen(tech_params->pdn_label);

    data_sess_profile->rat_type = (ds707_data_sess_rat_type_enum_type)tech_params->rat_type;

    data_sess_profile->pdn_inactivity_timeout = 
       CONV_MIN_TO_MSEC(tech_params->pdn_inactivity_timeout);

    memscpy((void*)data_sess_profile->failure_timer,
            sizeof(uint32)*PDN_THROTTLE_MAX_TIMERS,
            (void*)tech_params->failure_timer,
            sizeof(ds_profile_db_3gpp2_failure_timer_type) *
               DS_PROFILE_DB_3GPP2_PDN_THROTTLE_MAX_TIMERS);

    memscpy((void*)data_sess_profile->disallow_timer,
            sizeof(uint32)*PDN_THROTTLE_MAX_TIMERS,
            (void*)tech_params->disallow_timer,
            sizeof(ds_profile_db_3gpp2_disallow_timer_type) *
               DS_PROFILE_DB_3GPP2_PDN_THROTTLE_MAX_TIMERS);

    data_sess_profile->pdn_level_auth_protocol = 
       tech_params->pdn_level_auth_info.auth_type;

    memscpy((void*)data_sess_profile->pdn_level_user_id,
            PPP_MAX_USER_ID_LEN,
            (void*)tech_params->pdn_level_auth_info.username,
            DS_PROFILE_DB_MAX_AUTH_USER_LEN + 1);

    data_sess_profile->pdn_level_user_id_len = 
       strlen(tech_params->pdn_level_auth_info.username);

    memscpy((void*)data_sess_profile->pdn_level_auth_password,
            PPP_MAX_PASSWD_LEN,
            (void*)tech_params->pdn_level_auth_info.password,
            DS_PROFILE_DB_MAX_AUTH_PASS_LEN + 1);

    data_sess_profile->pdn_level_auth_password_len = 
       strlen(tech_params->pdn_level_auth_info.password);
  }

  DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                   "Profile %d cust_val_mask is set to 0x%x",
                   data_sess_profile->data_session_profile_id,
                   data_sess_profile->custom_val_mask );

  return;
}/*ds_profile_3gpp2_util_db_to_cache_type_convert*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_UTIL_CACHE_TO_DB_TYPE_CONVERT

DESCRIPTION   This function converts a profile from 3GPP2 MH cache type to 
              profile DB type. For common parameters in EPC profiles, if
              client has not explicitly set it, default value is returned

PARAMETERS    gen_params            : C type general parameters
              tech_params           : C type tech specific parameters
              data_sess_profile     : ptr to cache

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_util_cache_to_db_type_convert
(
  ds_profile_db_gen_params_type*          gen_params,
  ds_profile_db_3gpp2_tech_params_type*   tech_params,
  ds707_data_session_profile_info_type*   data_sess_profile
)
{

  if(data_sess_profile == NULL)
  {
    return;
  }

  if(tech_params != NULL)
  {
    memset((void*)tech_params, 0, sizeof(ds_profile_db_3gpp2_tech_params_type));

    /*---------------------------------- 
      tech_common_params
     ----------------------------------*/
    /*---------------------------------------------------------------------- 
      TECH SPECIFIC apn_disable_flag is equal to apn_enabled in cache
     -----------------------------------------------------------------------*/
    if(data_sess_profile->custom_val_mask & MASK_OF_IDENT(APN_ENABLED))
    {
      tech_params->tech_common_params.apn_disable_flag = 
         data_sess_profile->apn_flag;
    }
    else
    {
      tech_params->tech_common_params.apn_disable_flag = TRUE;
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(APN_CLASS))
    {
      tech_params->tech_common_params.apn_class = 
         data_sess_profile->apn_class;
    }
    else
    {
      tech_params->tech_common_params.apn_class = 0;
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(PRIMARY_DNS_ADDR_V4))
    {
      memscpy((void*)(&tech_params->tech_common_params.dns_addr.primary_addr.v4),
              sizeof(ds_profile_db_ip_addr_v4_type),
              (void*)(&data_sess_profile->v4_dns_addr[PRIMARY_DNS_ADDR]),
              sizeof(uint32));
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(PRIMARY_DNS_ADDR_V6))
    {
      memscpy((void*)(&tech_params->tech_common_params.dns_addr.primary_addr.v6),
              sizeof(ds_profile_db_ip_addr_v6_type),
              (void*)(&data_sess_profile->v6_dns_addr[PRIMARY_DNS_ADDR]),
              sizeof(uint32)*4);
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(SECONDARY_DNS_ADDR_V4))
    {
      memscpy((void*)(&tech_params->tech_common_params.dns_addr.secondary_addr.v4),
              sizeof(ds_profile_db_ip_addr_v4_type),
              (void*)(&data_sess_profile->v4_dns_addr[SECONDARY_DNS_ADDR]),
              sizeof(uint32));
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(SECONDARY_DNS_ADDR_V6))
    {
      memscpy((void*)(&tech_params->tech_common_params.dns_addr.secondary_addr.v6),
              sizeof(ds_profile_db_ip_addr_v6_type),
              (void*)(&data_sess_profile->v6_dns_addr[SECONDARY_DNS_ADDR]),
              sizeof(uint32)*4);
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(ALLOW_LINGER))
    {
      tech_params->tech_common_params.iface_linger_params.linger_params.allow_linger_flag = 
         data_sess_profile->allow_linger;
    }
    else
    {  
      tech_params->tech_common_params.iface_linger_params.linger_params.allow_linger_flag
        = FALSE;
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(LINGER_TIMEOUT_VAL))
    {
      tech_params->tech_common_params.iface_linger_params.linger_params.linger_timeout_val = 
         data_sess_profile->linger_timeout_val;
    }
    else
    {  
      tech_params->tech_common_params.iface_linger_params.linger_params.linger_timeout_val
        = 0;
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(PCSCF_ADDR_NEEDED))
    {
      tech_params->tech_common_params.request_pcscf_address_flag = 
         data_sess_profile->is_pcscf_addr_needed;
    }
    else
    {
      tech_params->tech_common_params.request_pcscf_address_flag = FALSE;
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(AUTH_PROTOCOL))
    {
      tech_params->tech_common_params.auth_info.auth_type = 
         data_sess_profile->auth_protocol;
    }
    else
    {
      tech_params->tech_common_params.auth_info.auth_type
         = DB_DATA_SESS_AUTH_PROTOCOL_PAP_CHAP;
    }
    
    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(PPP_USER_ID))
    {
      memscpy((void*)tech_params->tech_common_params.auth_info.username,
              DS_PROFILE_DB_MAX_AUTH_USER_LEN,
              (void*)data_sess_profile->user_id,
              data_sess_profile->user_id_len);

    }
    else
    {
      tech_params->tech_common_params.auth_info.username[0] = '\0';
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(PPP_PASSWORD))
    {
      memscpy((void*)tech_params->tech_common_params.auth_info.password,
              DS_PROFILE_DB_MAX_AUTH_PASS_LEN,
              (void*)data_sess_profile->auth_password,
              data_sess_profile->auth_password_len);

    }
    else
    {
      tech_params->tech_common_params.auth_info.password[0] = '\0';
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(OP_PCO_ID))
    {
      tech_params->tech_common_params.op_pco_id = 
         data_sess_profile->op_pco_id;
    }
    else
    {
      tech_params->tech_common_params.op_pco_id = 0;
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(MCC))
    {
      tech_params->tech_common_params.mcc = 
         data_sess_profile->mcc;
    }
    else
    {
      tech_params->tech_common_params.mcc = DS707_DATA_SESS_INITIAL_MCCMNC_VALUE;
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(MNC))
    {
      tech_params->tech_common_params.mnc.mnc_digits = 
         data_sess_profile->mnc.mnc;

      tech_params->tech_common_params.mnc.mnc_includes_pcs_digit = 
         data_sess_profile->mnc.mnc_includes_pcs_digit;
    }
    else
    {
      tech_params->tech_common_params.mnc.mnc_digits = 
         DS707_DATA_SESS_INITIAL_MCCMNC_VALUE;

      tech_params->tech_common_params.mnc.mnc_includes_pcs_digit = FALSE;
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(USER_APP_DATA))
    {
      tech_params->tech_common_params.user_app_data = 
         data_sess_profile->user_app_data;
    }
    else
    {
      tech_params->tech_common_params.user_app_data = 0xFFFFFFFF;
    }

     if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(DNS_DHCP_REQ_FLAG))
    {
      tech_params->tech_common_params.request_dns_address_using_dhcp_flag = 
         data_sess_profile->dns_addr_using_dhcp;
    }
    else
    {
      tech_params->tech_common_params.request_dns_address_using_dhcp_flag = FALSE;
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(PCSCF_DHCP_REQ_FLAG))
    {
      tech_params->tech_common_params.request_pcscf_address_using_dhcp_flag = 
         data_sess_profile->pcscf_addr_using_dhcp;
    }
    else
    {
      tech_params->tech_common_params.request_pcscf_address_using_dhcp_flag = FALSE;
    }

    if (data_sess_profile->custom_val_mask & MASK_OF_IDENT(PDN_IP_VERSION_TYPE))
    {
      tech_params->pdn_type = (ds_profile_db_3gpp2_pdn_type)data_sess_profile->pdn_type;
    }
    else
    {
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
      tech_params->pdn_type = DB_3GPP2_PDN_TYPE_V4_V6;
#else
      tech_params->pdn_type = DB_3GPP2_PDN_TYPE_V4;
#endif /* FEATURE_DATA_EHRPD_DUAL_IP  */
    }

    /*--------------------------------------------- 
      Profile DB internal params
     ---------------------------------------------*/
    tech_params->tech_common_params.dns_addr.primary_ip_version = 
       (ds_profile_db_ip_version)data_sess_profile->ip_version[PRIMARY_DNS_ADDR];

    tech_params->tech_common_params.dns_addr.secondary_ip_version = 
       (ds_profile_db_ip_version)data_sess_profile->ip_version[SECONDARY_DNS_ADDR];

    tech_params->tech_common_params.iface_linger_params.is_valid = 
       data_sess_profile->is_linger_params_valid;

    /*---------------------------------- 
      3GPP2 specific params
     ----------------------------------*/
    tech_params->profile_type = (ds_profile_db_3gpp2_profile_subtype)data_sess_profile->profile_type;

    tech_params->negotiate_dns_server = 
       data_sess_profile->negotiate_dns_server;

    tech_params->ppp_session_close_timer_DO = 
       data_sess_profile->ppp_session_close_timer_DO;

    tech_params->ppp_session_close_timer_1X = 
       data_sess_profile->ppp_session_close_timer_1X;

    tech_params->lcp_ack_timeout = data_sess_profile->lcp_ack_timeout;

    tech_params->ipcp_ack_timeout = data_sess_profile->ipcp_ack_timeout;

    tech_params->auth_timeout = data_sess_profile->auth_timeout;

    tech_params->lcp_creq_retry_count = 
       data_sess_profile->lcp_creq_retry_count;

    tech_params->ipcp_creq_retry_count = 
       data_sess_profile->ipcp_creq_retry_count;

    tech_params->auth_retry_count = data_sess_profile->auth_retry_count;

    tech_params->data_rate = data_sess_profile->data_rate;

    tech_params->data_mode = data_sess_profile->data_mode;

    tech_params->app_type = data_sess_profile->app_type;

    tech_params->app_priority = data_sess_profile->app_priority;

    memscpy((void*)tech_params->pdn_label,
            DS_PROFILE_DB_MAX_APN_NAME_LEN,
            (void*)data_sess_profile->pdn_label,
            data_sess_profile->pdn_label_len);

    tech_params->rat_type = (ds_profile_db_3gpp2_rat_type)data_sess_profile->rat_type;

    tech_params->pdn_inactivity_timeout = 
       CONV_MSEC_TO_MIN(data_sess_profile->pdn_inactivity_timeout);

    memscpy((void*)tech_params->failure_timer,
            sizeof(ds_profile_db_3gpp2_failure_timer_type) *
               DS_PROFILE_DB_3GPP2_PDN_THROTTLE_MAX_TIMERS,
            (void*)data_sess_profile->failure_timer,
            sizeof(uint32)*PDN_THROTTLE_MAX_TIMERS);

    memscpy((void*)tech_params->disallow_timer,
            sizeof(ds_profile_db_3gpp2_disallow_timer_type) *
               DS_PROFILE_DB_3GPP2_PDN_THROTTLE_MAX_TIMERS,
            (void*)data_sess_profile->disallow_timer,
            sizeof(uint32)*PDN_THROTTLE_MAX_TIMERS);

    tech_params->pdn_level_auth_info.auth_type = 
       data_sess_profile->pdn_level_auth_protocol;

    memscpy((void*)tech_params->pdn_level_auth_info.username,
            DS_PROFILE_DB_MAX_AUTH_USER_LEN,
            (void*)data_sess_profile->pdn_level_user_id,
            data_sess_profile->pdn_level_user_id_len);

    memscpy((void*)tech_params->pdn_level_auth_info.password,
            DS_PROFILE_DB_MAX_AUTH_PASS_LEN,
            (void*)data_sess_profile->pdn_level_auth_password,
            data_sess_profile->pdn_level_auth_password_len);
  }

  if(gen_params != NULL)
  {
    memset((void*)gen_params, 0, sizeof(ds_profile_db_gen_params_type));

    gen_params->profile_number = 
       (int16)data_sess_profile->data_session_profile_id;

    gen_params->persistent = 
       data_sess_profile->is_persistent;


    memscpy((void*)gen_params->apn_name,
            DS_PROFILE_DB_MAX_APN_NAME_LEN,
            (void*)data_sess_profile->apn_string,
             data_sess_profile->apn_string_len);

    /*--------------------------------------------------- 
      Currently profile name is always NULL for 3GPP2
     ---------------------------------------------------*/
    gen_params->profile_name[0] = '\0';

    gen_params->tech_mask = data_sess_profile->supported_tech_mask;

    gen_params->ipv6_delegation = data_sess_profile->ipv6_delegation;

    gen_params->clat_enabled = data_sess_profile->clat_enabled;

    gen_params->subscription_id = 
      (ds_profile_db_subs_type)data_sess_profile->subs_id;
  }

  return;
}/*ds_profile_3gpp2_util_cache_to_db_type_convert*/

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_UTIL_COPY_CHANGED_IDENTS

DESCRIPTION   Copy changed parameters to the profile cache

PARAMETERS    data_sess_profile     : ptr to the cache
              idents & count        : changed ident list and size
              mandatory_apn_changed : flag to indicate if mandatory APN
                                      related fields are changed

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_util_copy_changed_idents
(
  ds707_data_session_profile_info_type    *data_sess_profile,
  ds_profile_db_ident_type                *idents,
  uint16                                   idents_count,
  ds_profile_db_ident_type                *gen_idents,
  uint16                                   gen_idents_count,
  boolean                                 *mandatory_apn_changed
)
{
  uint16 index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (data_sess_profile == NULL) || (mandatory_apn_changed == NULL) )
  {
    return;
  }

  if (gen_idents !=NULL)
  {
    for(index = 0; index < gen_idents_count; index++)
    {
      switch (gen_idents[index]) 
      {
       /*---------------------- 
        gen_params
       ----------------------*/
        case GENERAL_PROFILE_PARAM_APN_NAME:
          data_sess_profile->custom_val_mask |= MASK_OF_IDENT(APN_STRING);
          *mandatory_apn_changed = TRUE;
          break;

        case GENERAL_PROFILE_PARAM_IPV6_DELEGATION:
        case GENERAL_PROFILE_PARAM_SUBS_ID:
          break;

        default:
          DATA_IS707_MSG1( MSG_LEGACY_MED,
                           "General param %d not supported to be set for 3GPP2",
                           gen_idents[index]);
          break;
      }
    }
  }

  if (idents != NULL)
  {
    for(index = 0; index < idents_count; index++)
    {
      switch (idents[index])
      {
      /*-------------------------------------------------------------------- 
        Tech common params - only care for apn flag
       -------------------------------------------------------------------*/
      case COMMON_TECH_PROFILE_PARAM_APN_DISABLE_FLAG:
           *mandatory_apn_changed = TRUE;
        break;
  
      /*-------------------------------------------------------------------- 
        Tech specific params
       -------------------------------------------------------------------*/
      case TECH_3GPP2_PROFILE_PARAM_NEGOTIATE_DNS_SERVER: 
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(NEGOTIATE_DNS_SERVER);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_DO:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(PPP_SESSION_CLOSE_TIMER_DO);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_1X:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(PPP_SESSION_CLOSE_TIMER_1X);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_ALLOW_LINGER:  
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(ALLOW_LINGER);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_LCP_ACK_TIMEOUT:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(LCP_ACK_TIMEOUT_VAL);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_IPCP_ACK_TIMEOUT:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(IPCP_ACK_TIMEOUT_VAL);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_AUTH_TIMEOUT:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(AUTH_TIMEOUT_VAL);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_LCP_CREQ_RETRY_COUNT:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(LCP_CREQ_RETRY_COUNT);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_IPCP_CREQ_RETRY_COUNT:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(IPCP_CREQ_RETRY_COUNT);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_AUTH_RETRY_COUNT:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(AUTH_RETRY_COUNT);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_AUTH_PROTOCOL:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(AUTH_PROTOCOL);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_USER_ID:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(PPP_USER_ID);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_AUTH_PASSWORD:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(PPP_PASSWORD);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_DATA_RATE:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(DATA_RATE);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_DATA_MODE:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(DATA_MODE);
         break;

      case TECH_3GPP2_PROFILE_PARAM_APP_TYPE:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(APP_TYPE);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_APP_PRIORITY:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(APP_PRIORITY);
         break;

      case TECH_3GPP2_PROFILE_PARAM_PDN_TYPE:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(PDN_IP_VERSION_TYPE);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_IS_PCSCF_ADDR_NEEDED:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(PCSCF_ADDR_NEEDED);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(PRIMARY_DNS_ADDR_V4);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(SECONDARY_DNS_ADDR_V4);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_PRIMARY:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(PRIMARY_DNS_ADDR_V6);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_SECONDARY:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(SECONDARY_DNS_ADDR_V6);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_RAT_TYPE:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(RAN_TYPE);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_APN_ENABLED:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(APN_ENABLED);
         *mandatory_apn_changed = TRUE;
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_PDN_INACTIVITY_TIMEOUT:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(PDN_INACTIVITY_TIMEOUT);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_APN_CLASS:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(APN_CLASS);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_LINGER_PARAMS:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(LINGER_TIMEOUT_VAL);
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(ALLOW_LINGER);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PROTOCOL:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(PDN_LEVEL_AUTH_PROTOCOL);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_PDN_LEVEL_USER_ID:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(PDN_LEVEL_USER_ID);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PASSWORD:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(PDN_LEVEL_AUTH_PASSWORD);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_PDN_LABEL:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(PDN_LABEL);
         *mandatory_apn_changed = TRUE;
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_1:
      case TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_2:
      case TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_3:
      case TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_4:
      case TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_5:
      case TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_6:
         data_sess_profile->custom_val_mask = 
                        data_sess_profile->custom_val_mask |
                        MASK_OF_IDENT( (idents[index] - 
                                        TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMER_1 + 
                                        FAILURE_TIMER_1) );
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_FAILURE_TIMERS:
         data_sess_profile->custom_val_mask |= 
            MASK_OF_IDENT( FAILURE_TIMER_1 ) | MASK_OF_IDENT( FAILURE_TIMER_2 ) |
            MASK_OF_IDENT( FAILURE_TIMER_3 ) | MASK_OF_IDENT( FAILURE_TIMER_4 ) |
            MASK_OF_IDENT( FAILURE_TIMER_5 ) | MASK_OF_IDENT( FAILURE_TIMER_6 );
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_1:
      case TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_2:
      case TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_3:
      case TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_4:
      case TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_5:
      case TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_6:
        data_sess_profile->custom_val_mask = 
                       data_sess_profile->custom_val_mask |
                       MASK_OF_IDENT( (idents[index] - 
                                       TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_1 + 
                                       DISALLOW_TIMER_1) );
        break;
  
      case TECH_3GPP2_PROFILE_PARAM_DISALLOW_TIMERS:
         data_sess_profile->custom_val_mask |= 
            MASK_OF_IDENT( DISALLOW_TIMER_1 ) | MASK_OF_IDENT( DISALLOW_TIMER_2 ) |
            MASK_OF_IDENT( DISALLOW_TIMER_3 ) | MASK_OF_IDENT( DISALLOW_TIMER_4 ) |
            MASK_OF_IDENT( DISALLOW_TIMER_5 ) | MASK_OF_IDENT( DISALLOW_TIMER_6 );
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_OP_PCO_ID:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT( OP_PCO_ID );
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_MCC:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT( MCC );
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_MNC:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT( MNC );
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_USER_APP_DATA:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(USER_APP_DATA);
         break;
  
      case TECH_3GPP2_PROFILE_PARAM_DNS_DHCP_REQ_FLAG:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(DNS_DHCP_REQ_FLAG);
         break;

      case TECH_3GPP2_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG:
         data_sess_profile->custom_val_mask |= MASK_OF_IDENT(PCSCF_DHCP_REQ_FLAG);
         break;
  
      default:
         DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                          "Ident %d not allowed to be modified",
                          idents[index]);
         ASSERT(0);
         break;
      }
    }
  }

  return;

}/* ds_profile_3gpp2_util_copy_changed_idents */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_UTIL_GET_PARAMS

DESCRIPTION   This function retrieves required values to copy profile

PARAMETERS    profile        : profile DB managed profile type
              gen_params     : C type general parameters
              tech_params    : C type tech specific parameters

DEPENDENCIES  None

RETURN VALUE  TRUE  : Success
              FALSE : Failure

SIDE EFFECTS  None
===========================================================================*/
boolean ds_profile_3gpp2_util_get_params
(
  const ds_profile_db_profile_managed_type *profile,
  ds_profile_db_gen_params_type            *gen_params,
  ds_profile_db_3gpp2_tech_params_type     *tech_params
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( profile == NULL )
  {
    return FALSE;
  }

  memset((void*)tech_params, 0, sizeof(ds_profile_db_3gpp2_tech_params_type));
  memset((void*)gen_params, 0, sizeof(ds_profile_db_gen_params_type));

  /*------------------------------------------------------------------- 
    Get C type profile general parameters and tech specific parameters
    from profile DB
   -------------------------------------------------------------------*/
  if( DB_RESULT_SUCCESS != ds_profile_db_get_tech_params( profile,
                                                          DB_TECH_3GPP2,
                                                          (void*)tech_params,
                                                          gen_params ) )
  {
    return FALSE;
  }

  return TRUE;
} /* ds_profile_3gpp2_util_get_params */

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_UTIL_COPY_PROFILE_AND_SET_CUST_MASK

DESCRIPTION   This function copies DB type profiles into MH cache. It sets 
              the cust_val_mask to mark params set by user. For EPC profiles,
              if the tech specific param is not set and has a corresponding
              common param, MH will take the common param value

PARAMETERS    profile        : profile DB managed profile type
              are_apn_fields_chged  : are APN, apn_flag or PDN lable changed
              gen_params            : C type general parameters
              tech_params           : C type tech specific parameters
              common_params         : C type common parameters for EPC profile
              data_sess_profile     : ptr to the cache

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_util_copy_profile_and_set_cust_mask
(
  const ds_profile_db_profile_managed_type *profile,
  boolean                                  *mandatory_apn_changed,
  ds_profile_db_gen_params_type            *gen_params,
  ds_profile_db_3gpp2_tech_params_type     *tech_params,
  ds_profile_db_common_tech_params_type   **common_params,
  ds707_data_session_profile_info_type     *data_sess_profile
)
{
  ds_profile_db_ident_type              *idents = NULL;
  uint16                                 idents_count = 0;
  ds_profile_db_ident_type              *gen_idents = NULL;
  uint16                                 gen_idents_count = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*--------------------------------------------------------------------- 
    Get C type profile common parameters from profile DB for EPC profiles
   ---------------------------------------------------------------------*/
  if( DB_TECH_TO_TECHMASK(DB_TECH_COMMON) & gen_params->tech_mask  )
  {
    *common_params = 
      (ds_profile_db_common_tech_params_type *)modem_mem_alloc(
                                sizeof(ds_profile_db_common_tech_params_type),
                                MODEM_MEM_CLIENT_DATA);

    if ((*common_params) == NULL)
    {
      ASSERT(0);
      return;
    }

    memset( (void*)(*common_params), 0, 
            sizeof(ds_profile_db_common_tech_params_type) );

    if( DB_RESULT_SUCCESS != ds_profile_db_get_tech_params( 
                                                    profile,
                                                    DB_TECH_COMMON,
                                                    (void*)(*common_params),
                                                    NULL ))
    {
      modem_mem_free((void*)(*common_params), MODEM_MEM_CLIENT_DATA);
      *common_params = NULL;
    }
  }

  /*------------------------------------------------------------------- 
    Get lists of changed general and common/tech_specific parameters
   -------------------------------------------------------------------*/
  (void)ds_profile_db_get_changed_idents( profile,
                                          DB_TECH_TO_TECHMASK(DB_TECH_3GPP2),
                                          &idents,
                                          &idents_count,
                                          &gen_idents,
                                          &gen_idents_count);

  /*--------------------------------------------------------------------- 
    Set cust_val_mask 
   ---------------------------------------------------------------------*/
  ds_profile_3gpp2_util_copy_changed_idents(data_sess_profile,
                                            idents,
                                            idents_count,
                                            gen_idents,
                                            gen_idents_count,
                                            mandatory_apn_changed);

  /*-------------------------------------------------------------------------- 
    Copy gen and tech specific params and override common params if applicable
   --------------------------------------------------------------------------*/
  ds_profile_3gpp2_util_db_to_cache_type_convert(gen_params,
                                                 tech_params,
                                                 *common_params,
                                                 data_sess_profile);


  /*--------------------------------------------------------------------- 
    Free common params
   ---------------------------------------------------------------------*/
  if ( (*common_params) != NULL )
  {
    modem_mem_free((void*)(*common_params), MODEM_MEM_CLIENT_DATA);
    *common_params = NULL;
  }

  /*----------------------------------------------------
   Free idents structure
   ----------------------------------------------------*/
  if (idents != NULL)
  {
    ds_profile_db_free_idents(idents);
    idents = NULL;
  }
  if (gen_idents != NULL)
  {
    ds_profile_db_free_idents(gen_idents);
    gen_idents = NULL;
  }

} /* ds_profile_3gpp2_util_copy_profile_and_set_override_flag */

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
/*lint -save -e656*/

ds_profile_3gpp2_valid_profile_enum_type get_valid_mask_from_ident(
  ds_profile_identifier_type param_id
)
{
  uint8 index = 0;
  for (index = 0;
       index < (sizeof(ds_profile_3gpp2_params_valid_mask)/
                sizeof(dsi_profile_3gpp2_params_valid_mask));
       index++ )
  {
    if ( param_id == ds_profile_3gpp2_params_valid_mask[index].ident )
    {
      return ds_profile_3gpp2_params_valid_mask[index].valid_mask;
    }
  }
  return DS_PROFILE_3GPP2_PROFILE_INVALID;
}

/*===========================================================================
FUNCTION      DS_PROFILE_3GPP2_UTIL_CACHE_TO_DB_GET_IS_TECH_PARAM_SET

DESCRIPTION   This function returns a boolean based on tech param being set 
              Need to add new Common params always. 

PARAMETERS    ident             - ds_profile_db_ident_type 
              data_sess_profile - ds707_data_session_profile_info_type
              *is_tech_param_set - boolean
 
DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ds_profile_3gpp2_util_cache_to_db_get_is_tech_param_set
(
  ds_profile_db_ident_type                ident,
  ds707_data_session_profile_info_type*   data_sess_profile,
  boolean*                                is_tech_param_set
)
{
  if(data_sess_profile == NULL)
  {
    return;
  }
  if (is_tech_param_set)
  {
    *is_tech_param_set = TRUE;
    switch (ident)
    {
    /*------------------------------------------------------ 
     common_tech params, check user set flag before copying
     ------------------------------------------------------*/

    case COMMON_TECH_PROFILE_PARAM_APN_DISABLE_FLAG:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(APN_ENABLED)) )
       {
         *is_tech_param_set = FALSE;
       }
       break;

    case COMMON_TECH_PROFILE_PARAM_APN_CLASS:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(APN_CLASS)) )
       {
         *is_tech_param_set = FALSE;
       }
       break;

    case COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(PRIMARY_DNS_ADDR_V4)) )
       {
         *is_tech_param_set = FALSE;
       }
       break;

    case COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY:
      if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(SECONDARY_DNS_ADDR_V4)) )
      {
         *is_tech_param_set = FALSE;
      }

      break;

    case COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(PRIMARY_DNS_ADDR_V6)) )
       {
         *is_tech_param_set = FALSE;
       }

       break;

    case COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(SECONDARY_DNS_ADDR_V6)) )
       {
         *is_tech_param_set = FALSE;
       }

       break;

    case COMMON_TECH_PROFILE_PARAM_LINGER_PARAMS:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(ALLOW_LINGER)) )
       {
         *is_tech_param_set = FALSE;
       }

       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(LINGER_TIMEOUT_VAL)) )
       {
         *is_tech_param_set = FALSE;
       }

       break;

    case COMMON_TECH_PROFILE_PARAM_PCSCF_REQ_FLAG:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(PCSCF_ADDR_NEEDED)) )
       {
         *is_tech_param_set = FALSE;
       }
       break;

    case COMMON_TECH_PROFILE_PARAM_AUTH_TYPE:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(AUTH_PROTOCOL)) )
       {
         *is_tech_param_set = FALSE;
       }
       break;

    case COMMON_TECH_PROFILE_PARAM_AUTH_USERNAME:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(PPP_USER_ID)) )
       {
         *is_tech_param_set = FALSE;
       }
       break;

    case COMMON_TECH_PROFILE_PARAM_AUTH_PASSWORD:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(PPP_PASSWORD)) )
       {
         *is_tech_param_set = FALSE;
       }
       break;

    case COMMON_TECH_PROFILE_PARAM_OPERATOR_RESERVED_PCO:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(OP_PCO_ID)) )
       {
         *is_tech_param_set = FALSE;
       }
       break;

    case COMMON_TECH_PROFILE_PARAM_MCC:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(MCC)) )
       {
         *is_tech_param_set = FALSE;
       }
       break;

    case COMMON_TECH_PROFILE_PARAM_MNC:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(MNC)) )
       {
         *is_tech_param_set = FALSE;
       }
       break;

    case COMMON_TECH_PROFILE_PARAM_USER_APP_DATA:
      if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(USER_APP_DATA)) )
      {
        *is_tech_param_set = FALSE;
      }
      break;
    
    case COMMON_TECH_PROFILE_PARAM_DNS_DHCP_REQ_FLAG:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(DNS_DHCP_REQ_FLAG)) )
       {
         *is_tech_param_set = FALSE;
       }
       break;

    case COMMON_TECH_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG:
       if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(PCSCF_DHCP_REQ_FLAG)) )      
       {
         *is_tech_param_set = FALSE;
       }
       break;

    case COMMON_TECH_PROFILE_PARAM_PDP_TYPE:
      if( !(data_sess_profile->custom_val_mask & MASK_OF_IDENT(PDN_IP_VERSION_TYPE)) )
      {
        *is_tech_param_set = FALSE;
      }
      break;
    
    default:
      DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                     "ds_profile_3gpp2_util_cache_to_db_get_is_tech_param_set: "
                     "Invalid Common Ident %d", ident );
      break;
    }
  }

  return;
}

