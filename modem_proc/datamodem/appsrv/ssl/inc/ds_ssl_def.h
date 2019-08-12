/******************************************************************************
  @file    ds_ssl_def.h
  @brief   ds ssl variable definition header file

  DESCRIPTION
  Definitions for DS SSL variables declarations

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A
******************************************************************************/

/*=============================================================================
   Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/



/*=============================================================================
                        EDIT HISTORY FOR MODULE
  # $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/appsrv/ssl/inc/ds_ssl_def.h#1 $
  # $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 when       who                    what, where, why
--------   ---       ----------------------------------------------------------
5/20/14   youjunf                      Created
===========================================================================*/


#ifndef DS_SSL_DEF_H
#define DS_SSL_DEF_H

#define DS_SSL_SERVER_NAME_MAX_SIZE  255

/*error code*/
#define DS_SSL_SUCCESS                     0                      /*successful operation */
#define DS_SSL_WOULDBLOCK                 -1                      /*operation block*/
#define DS_SSL_INVALID_ARG                -2                      /*Arg is not recognizied*/
#define DS_SSL_NOT_AVAILABLE              -3                      /*resource or operation not available*/
#define DS_SSL_ERROR_UNKNOW               -4                      /*other unknow error*/


/*SSL version*/
typedef enum
{
  DS_SSL_VER_DEFAULT,    /**< SSL protocol ver. selected by SECSSL module */
  DS_SSL_VER_10,         /**< SSL protocol ver. 1.0. */
  DS_SSL_VER_20,         /**< SSL protocol ver. 2.0. */
  DS_SSL_VER_30,         /**< SSL protocol ver. 3.0. */
  DS_SSL_VER_TLS10,      /**< TLS protocol ver. 1.0 (SSL 3.1). */
  DS_SSL_VER_TLS11,      /**< TLS protocol ver. 1.1 (SSL 3.2). */
  DS_SSL_VER_TLS12       /**< TLS protocol ver. 1.2 (SSL 3.3). */
} ds_ssl_ver_enum_type;


/*---------------------------------------------------------------------------
  Structure specifying policy to be used in ssl session configuration

  Fields are:
    ssl_ver                 -Specifies the SSL version, recommend using DS_SSL_VER_TLS10
    server_port             -Specifies the dest port of server
    server_name             -specifies the host_name of server
---------------------------------------------------------------------------*/
typedef struct
{
  ds_ssl_ver_enum_type ssl_ver;
  uint16               server_port;
  char                 server_name[DS_SSL_SERVER_NAME_MAX_SIZE + 1];
} ds_ssl_config_info_type;


/*Defines the user action when get an suspend alert notification callback*/
typedef enum
{
  DS_SSL_CONTROL_CONTINUE,   /*continue the current session*/
  DS_SSL_CONTROL_ABORT       /*abort the current session*/
} ds_ssl_control_type;


/* The DS SSL session stauts provided in the status callback function */
typedef enum
{
  DS_SSL_STATUS_CONNECTED,      /*ssl finishes the handshake and the session is connected*/
  DS_SSL_STATUS_DISCONNECTED,   /*ssl session is disconnected*/
  DS_SSL_STATUS_READ_EVENT,     /*ssl has data ready for read*/
  DS_SSL_STATUS_TCP_CLOSE,      /*peer closed tcp socket, client has to close the ssl session and then tcp session*/
  DS_SSL_STATUS_ALERT_SUSPEND,  /*ssl session is suspended, client can choose to continue or abort*/
  DS_SSL_STATUS_ALERT_FATAL,    /*ssl session receives fatal alert, client has to close the ssl session*/
  DS_SSL_STATUS_ALERT_WARNING,  /*ssl session receives warning level alert*/
  DS_SSL_STATUS_FATAL_ERROR     /*fatal error happened. client has to close the ssl session*/
} ds_ssl_status_enum_type;



/*error status info type*/
typedef enum
{
  SESSION_OPEN_FAIL,       /*ds ssl session open failed*/
  SESSION_CONTINUE_FAIL,   /*ds ssl session close failed*/
  SESSION_OTHER_ERROR     /*other operation error*/
} ds_ssl_error_mask_type;


typedef enum
{
  DS_SSL_ALERT_CLOSE_NOTIFY            = 0x00000000,
    /* Session was closed by either side. */
  DS_SSL_ALERT_UNEXPECTED_MESSAGE      = 0x00000001,
    /* SSL protocol received an unexpected message. */
  DS_SSL_ALERT_BAD_RECORD_MAC          = 0x00000002,
    /* SSL record failed authentication. */
  DS_SSL_ALERT_DECRYPTION_FAILED       = 0x00000004,
    /* Decrypted message not a multiple of block size, or invalid padding. */
  DS_SSL_ALERT_RECORD_OVERFLOW         = 0x00000008,
    /* Record too large. */
  DS_SSL_ALERT_DECOMPRESSION_FAILURE   = 0x00000010,
    /* SSL record failed decompression. */
  DS_SSL_ALERT_HANDSHAKE_FAILURE       = 0x00000020,
    /* SSL handshake protocol failed. */
  DS_SSL_ALERT_NO_CERTIFICATE          = 0x00000040,
    /* Certificate is missing. */
  DS_SSL_ALERT_BAD_CERTIFICATE         = 0x00000080,
    /* Certificate failed authentication. */
  DS_SSL_ALERT_BAD_CERTIFICATE_SIGN    = 0x00000100,
    /* Certificate failed authentication. */
  DS_SSL_ALERT_BAD_CERTIFICATE_ISSUER  = 0x00000200,
    /* Certificate failed authentication. */
  DS_SSL_ALERT_UNSUPPORTED_CERTIFICATE = 0x00000400,
    /* Certificate type not understood. */
  DS_SSL_ALERT_CERTIFICATE_REVOKED     = 0x00000800,
    /* Certificate invalid (was revoked). */
  DS_SSL_ALERT_CERTIFICATE_EXPIRED     = 0x00001000,
    /* Certificate invalid (expired). */
  DS_SSL_ALERT_CERTIFICATE_UNKNOWN     = 0x00002000,
    /* Certificate invalid (unknown error). */
  DS_SSL_ALERT_ILLEGAL_PARAMETER       = 0x00004000,
    /* A field in the handshake was out of range or inconsistent with
         other fields.\ This is always fatal. */
  DS_SSL_ALERT_INVALID                 = 0x00008000,
    /* Alert contained an invalid ID. */
  DS_SSL_ALERT_UNKNOWN_CA              = 0x00010000,
    /* Cannot identify certificate authority. */
  DS_SSL_ALERT_ACCESS_DENIED           = 0x00020000,
    /* No access rights to continue negotiation. */
  DS_SSL_ALERT_DECODE_ERROR            = 0x00040000,
    /* Field value out of range. */
  DS_SSL_ALERT_DECRYPT_ERROR           = 0x00080000,
    /* Crypto operation failed. */
  DS_SSL_ALERT_EXPORT_RESTRICTION      = 0x00100000,
    /* Parameter is not in compliance. */
  DS_SSL_ALERT_PROTOCOL_VERSION        = 0x00200000,
    /* Cannot support requested version. */
  DS_SSL_ALERT_INSUFFICIENT_SECURITY   = 0x00400000,
    /* Server requires more secure ciphersuites than the client can support. */
  DS_SSL_ALERT_INTERNAL_ERROR          = 0x00800000,
    /* Local error independent of protocol. */
  DS_SSL_ALERT_USER_CANCELED           = 0x01000000,
    /* Cancel handshake for other reasons. */
  DS_SSL_ALERT_NO_RENEGOTIATION        = 0x02000000,
    /* Cannot renegotiate the handshake. */
  DS_SSL_ALERT_HOST_MISMATCH           = 0x04000000,
    /* Host mismatch (Common Name). */
  DS_SSL_ALERT_CERT_INFO               = 0x08000000,
    /* Alert for viewing the certificate. */
  DS_SSL_ALERT_UNRECOGNIZED_NAME       = 0x10000000,
    /* Alert for an unrecognized server name list. */
} ds_ssl_alert_mask_type;







/*Detailed status info that provided in status callback function*/
typedef union
{
  ds_ssl_error_mask_type error_mask;
  ds_ssl_alert_mask_type alert_mask;
} ds_ssl_status_info_type;




/*---------------------------------------------------------------------------
  The callback function that is used to notify the client of SSL status and stauts_info
DESCRIPTION
  client_id         -- client id
  ssl_session_id    -- ds ssl session id
  status            -- status of ssl session
  status_info       -- detailed status info
  user_data_ptr     -- user data specified during registration
---------------------------------------------------------------------------*/
typedef void (*ds_ssl_cb_fcn)
(
  sint15                  client_id,
  sint15                  ssl_session_id,
  ds_ssl_status_enum_type status,
  ds_ssl_status_info_type *status_info,
  void                    *user_data_ptr
);

#endif   //DS_SSL_DEF_H

