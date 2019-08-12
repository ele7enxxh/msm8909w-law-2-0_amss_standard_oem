#ifndef SECSSL_H
#define SECSSL_H

/*===========================================================================*/

/**
   @file secssl.h
   @brief The Secure Sockets Layer provides authenticated and encrypted 
     on-blocking connectivity to a secure server supporting SSL version 3.0.

 *   SecSSL supports one connection per session. The read/write functions run
 *   partially in the context of the calling task. Due to the additional
 *   overhead of encryption and authentication, secssl_read() and 
 *   secssl_write() impose a larger latency than the regular dss_read()
 *   and dss_write().
 */
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The SECSSL_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the SECSSL_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "ssl_api" group 
      description in the SECSSL_API_mainpage.dox file. 
===========================================================================*/
/*=============================================================================
    Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*=============================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/api/ssl/secssl.h#1 $ 
  $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/15   nm      TLSv1.2 support
06/17/14   nm      TLS over IPv6
02/17/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
07/15/10   lo      (Tech Pubs) Edited/added Doxygen comments and markup.
06/20/10   yh      Doxygen format
06/02/10   yh      Doxygen comments
04/05/10   yh      Add sys_time, ignore_expiration, client_cert session parameters.
02/03/10   yh      Add support for TLS1.1
02/06/09   qxu     port INTLOCK/INTFREE to new critical section mechnism on QDSP6 Hexgon. 
12/25/08   sl      Support for TLS Server Name Extension (RFC4366)
04/16/07   avm     increased SECSSL_SES_CS_LIST_MAX_SIZE to 25
04/04/06   avm     increased SECSSL_SES_SERVER_MAX_HOSTNAME_LEN to 255 to fix
                   CR115824 SSL Server Address limit of 64 characters too low
11/30/06   df      support for ds_socket2
11/01/06   avm     Move  and renamed secssl_ses_certificate_pdata_type to 
                   secx509.h secx509_ses_certificate_pdata_type
06/01/06   df      increased the size of write records to 16k
                   allow DSS_SO_LINGER_RESET for DSS versions that do not 
                   support it by using DSS_SO_SILENT_CLOSE instead
04/28/06   df      changes to support socket options for ssl session
05/14/03   jay     Added an alert for "cert_info" mode
02/14/03   om      Added support for setting SSL version and cipher suite as
                   a parameter.
01/24/02   om      Added support for root certs option
07/15/02   rwh     Moved some session parameter limit values from secssli.h.
06/25/02   om      Added prototype for secssl_ds_default_event_handler().
06/20/02   om      Added comment for DSM item pool sizing.
06/18/02   om      Added explicit no's tp session param enum for RPC.
06/13/02   om      Added SECSSL_ALERT_LEVEL_INFO for alerts in non-connected
                   state (appl. to use action callback, e.g. open_cb instead).
06/04/02   om      Added support for session abort.
05/30/02   kvd     Added callback params to close_network().
05/24/02   rwh     Added support for external sockets.
02/12/02   rwh     Added #def for SECSSL_MAX_WRITE_RECORD_LENGTH
02/08/02   dld     Added #def for HOSTNAME LENGTH
02/07/02   om      Added support for user action for security related events.
                   Added support for event-driven secure sockets.
06/11/01   om      Removed obsolete interface function.
05/24/01   om      Added secssl_close_network().
03/02/01   om      Several fixes (first functional version)
10/23/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "secerrno.h"
#include "secx509.h"
#include "dssocket.h"
#include "secssldef.h"


/**
  @addtogroup ssl_data_structures 
  @{ */

/*===========================================================================

              DEFINITIONS AND CONSTANTS FROM SSL / TLS

===========================================================================*/

/* Well-known server ports for secured services */ 
#define SECSSL_HTTPS_DEFAULT_SVR_PORT      443  /**< HTTP over SSL. */
#define SECSSL_FTPS_D_DEFAULT_SVR_PORT     989  /**< FTP over SSL (data
                                                     connection). */
#define SECSSL_FTPS_C_DEFAULT_SVR_PORT     990  /**< FTP over SSL (control
                                                     connection). */
#define SECSSL_NNTPS_DEFAULT_SVR_PORT      563  /**< NNTP over SSL. */



/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/* Session parameter value limits    */
#define SECSSL_SES_SERVER_MAX_HOSTNAME_LEN 255  
/**< Maximum length of addr.host_name in session parameter 
secssl_server_pdata_type. */
#define SECSSL_SES_CS_LIST_MAX_SIZE          25 
/**< Maximum list size in session parameter secssl_ses_cs_list_pdata_type. */
#define SECSSL_SES_MAX_ROOT_CERTS            SECX509_NUM_CERTS 
/**< Maximum amount of CA root certificates that can be set through session 
parameter secssl_ses_ca_certificate_pdata_type. */
#define SECSSL_SES_CERT_FOLDER_MAX_LEN       128 
/**< Maximum length of a certificate folder path can be set to secssl_ses_cert_conf_data_type
secssl_ses_ca_cert_folder_data_type */

/** Expecting that the phone is not a massive data source, the write
    is limited to half the record size of the read. The record size of the 
    write is 16384 bytes.
*/
#define SECSSL_MAX_WRITE_RECORD_LENGTH  SSL_V3_MAX_RECORD_LENGTH  /* 16384 */


/** SSL session handle.
*/
typedef void* secssl_handle_type;


/** SSL session commands.
*/
typedef enum
{
  SECSSL_SESSION_RESET_CMD = 0, /**< Reset. */
  SECSSL_SESSION_FLUSH_CMD,     /**< Flush. */
  SECSSL_SESSION_RENEG_CMD      /**< Renegotiate. */
} secssl_sescmd_enum_type;

/** 
  @}
*/
/* end_addtogroup ssl_data_structures */



/**
  @addtogroup control_session
  @{ */

/** SSL session alerts level.
*/
typedef enum
{
  SECSSL_ALERT_LEVEL_WARN = 0,      /**< Warning alert; continue session. */
  SECSSL_ALERT_LEVEL_FATAL = 1,     /**< Fatal alert; session was closed. */
  SECSSL_ALERT_LEVEL_SUSPEND = 2,   /**< Overridable alert; session was suspended. */
  SECSSL_ALERT_LEVEL_INFO = 3       /**< Informational; used during handshaking. */ 
} secssl_ses_alert_level_enum_type;

/** SSL session alerts mask.
*/
typedef enum
{
  SECSSL_ALERT_CLOSE_NOTIFY            = 0x00000000,
    /**< Session was closed by either side. */       
  SECSSL_ALERT_UNEXPECTED_MESSAGE      = 0x00000001,
    /**< SSL protocol received an unexpected message. */
  SECSSL_ALERT_BAD_RECORD_MAC          = 0x00000002,
    /**< SSL record failed authentication. */   
  SECSSL_ALERT_DECRYPTION_FAILED       = 0x00000004,
    /**< Decrypted message not a multiple of block size, or invalid padding. */
  SECSSL_ALERT_RECORD_OVERFLOW         = 0x00000008,
    /**< Record too large. */   
  SECSSL_ALERT_DECOMPRESSION_FAILURE   = 0x00000010,
    /**< SSL record failed decompression. */
  SECSSL_ALERT_HANDSHAKE_FAILURE       = 0x00000020,
    /**< SSL handshake protocol failed. */
  SECSSL_ALERT_NO_CERTIFICATE          = 0x00000040,
    /**< Certificate is missing. */
  SECSSL_ALERT_BAD_CERTIFICATE         = 0x00000080,
    /**< Certificate failed authentication. */ 
  SECSSL_ALERT_BAD_CERTIFICATE_SIGN    = 0x00000100,
    /**< Certificate failed authentication. */ 
  SECSSL_ALERT_BAD_CERTIFICATE_ISSUER  = 0x00000200,
    /**< Certificate failed authentication. */ 
  SECSSL_ALERT_UNSUPPORTED_CERTIFICATE = 0x00000400,
    /**< Certificate type not understood. */ 
  SECSSL_ALERT_CERTIFICATE_REVOKED     = 0x00000800,
    /**< Certificate invalid (was revoked). */   
  SECSSL_ALERT_CERTIFICATE_EXPIRED     = 0x00001000,
    /**< Certificate invalid (expired). */   
  SECSSL_ALERT_CERTIFICATE_UNKNOWN     = 0x00002000,
    /**< Certificate invalid (unknown error). */   
  SECSSL_ALERT_ILLEGAL_PARAMETER       = 0x00004000,
    /**< A field in the handshake was out of range or inconsistent with
         other fields.\ This is always fatal. */
  SECSSL_ALERT_INVALID                 = 0x00008000,
    /**< Alert contained an invalid ID. */
  SECSSL_ALERT_UNKNOWN_CA              = 0x00010000,
    /**< Cannot identify certificate authority. */
  SECSSL_ALERT_ACCESS_DENIED           = 0x00020000,
    /**< No access rights to continue negotiation. */
  SECSSL_ALERT_DECODE_ERROR            = 0x00040000,
    /**< Field value out of range. */
  SECSSL_ALERT_DECRYPT_ERROR           = 0x00080000,
    /**< Crypto operation failed. */
  SECSSL_ALERT_EXPORT_RESTRICTION      = 0x00100000,
    /**< Parameter is not in compliance. */
  SECSSL_ALERT_PROTOCOL_VERSION        = 0x00200000,
    /**< Cannot support requested version. */
  SECSSL_ALERT_INSUFFICIENT_SECURITY   = 0x00400000,
    /**< Server requires more secure ciphersuites than the client can support. */
  SECSSL_ALERT_INTERNAL_ERROR          = 0x00800000,
    /**< Local error independent of protocol. */
  SECSSL_ALERT_USER_CANCELED           = 0x01000000,
    /**< Cancel handshake for other reasons. */
  SECSSL_ALERT_NO_RENEGOTIATION        = 0x02000000,
    /**< Cannot renegotiate the handshake. */
  SECSSL_ALERT_HOST_MISMATCH           = 0x04000000,
    /**< Host mismatch (Common Name). */
  SECSSL_ALERT_CERT_INFO               = 0x08000000,
    /**< Alert for viewing the certificate. */
  SECSSL_ALERT_UNRECOGNIZED_NAME       = 0x10000000
    /**< Alert for an unrecognized server name list. */

} secssl_ses_alert_mask_enum_type;

/* ============================================================================
  Function:    secssl_ses_alert_cb_fptr_type
============================================================================*/
/**
  Callback function invoked by the SecSSL task to notify the 
  application of a session alert.

  To enable SUSPEND level alerts for trust failures, the Alert mode must be set
  to SECSSL_SES_ALERT_MODE_USER.
 
  <b>Application alert handling</b> \n
  For the application to achieve the default behavior, the alert handling
  is to be set to SECSSL_SES_ALERT_MODE_AUTO. For the application to let
  the user decide about trust-related alerts (e.g., an unknown server
  certificate issuer), the alert handling is to be set to 
  SECSSL_SES_ALERT_MODE_USER. When a trust failure occurs, the session 
  suspends. If the user overrides the failure, the application continues by 
  calling secssl_continue_session(). If the user does not override the failure,
  the application ends the session by calling secssl_close_session().

  @param[in] udata_ptr   Pointer to the user data; set to session handle if NULL.
  @param[in] alert_level Severity of the alert, see 
                         secssl_ses_alert_level_enum_type().\n
                         @note1 Warnings and fatal alerts cause the session
                         to close unconditionally. If the session alerts are
                         user handled, SUSPEND alerts can be overridden by
                         the application.
  @param[in] alert_mask  Type of alert (see secssl_ses_alert_mask_enum_type()).\n
                         @note1 The actual value passed in the alert callback
                         is a bit sequence containing all the alerts that
                         occurred. The application's alert callback 
                         logically compares the individual fields to test for
                         a specific alert other than a regular CLOSE alert.
                         This permits notification of multiple failures via a
                         single SUSPEND callback.
 
  @return
  None.
  
  @dependencies
  None.
*/
typedef void (*secssl_ses_alert_cb_fptr_type)
(
  void*                             udata_ptr,
  secssl_ses_alert_level_enum_type  alert_level,
  secssl_ses_alert_mask_enum_type   alert_mask
);

/** Parameter for the event callback function and user data pointer.
*/
typedef enum
{
  SECSSL_SES_SOCK_READ_EVENT = 0,      /**< Socket Read event. */
  SECSSL_SES_SOCK_CLOSE_EVENT,         /**< Socket Close event. */
  SECSSL_SES_DNS_START_EVENT,          /**< DNS Start event. */
  SECSSL_SES_DNS_SUCCESS_EVENT,        /**< DNS Success event. */
  SECSSL_SES_DNS_FAILURE_EVENT,        /**< DNS Failure event. */
  SECSSL_SES_CONNECT_START_EVENT,      /**< Socket connection Start event. */
  SECSSL_SES_CONNECT_SUCCESS_EVENT,    /**< Socket connection Success event. */
  SECSSL_SES_CONNECT_FAILURE_EVENT,    /**< Socket connection Failure event. */
  SECSSL_SES_AUTH_START_EVENT,         /**< TLS handshake Start event. */
  SECSSL_SES_AUTH_SUCCESS_EVENT,       /**< TLS handshake Success event. */
  SECSSL_SES_AUTH_FAILURE_EVENT        /**< TLS handshake Failure event. */
} secssl_ses_event_enum_type;

/* ============================================================================
**  Function:    secssl_ses_event_cb_fptr_type
** ==========================================================================*/
/**
  Callback function invoked by the Security Services task to notify the 
  application of a session event, set through session SECSSL_SES_EVENT_CB.
   
  To enable SSL events, this callback function pointer must be set to non-NULL.
 
  @param[in] session_handle Handle for a valid session. 
  @param[in] event          Type of event (see
                            secssl_ses_alert_level_enum_type()).
  @param[in] udata_ptr      Pointer to the user data.
 
  @return
  None.

  @dependencies
  None.
*/
typedef void (*secssl_ses_event_cb_fptr_type)  
(
  secssl_handle_type                session_handle,
  secssl_ses_event_enum_type        event,
  void*                             udata_ptr
);
/** 
  @}
*/
/*  end_addtogroup control_session */

/* session parameters */
/**
  @addtogroup ssl_ses_param 
  @{ */

/** Session parameters.
*/
typedef enum
{
  SECSSL_SES_SERVER = 0,
    /**< Address and port of SSL server. */ /* r/w */
  SECSSL_SES_SSL_VERSION = 1,
    /**< SSL protocol version. */ /* r/w */
  SECSSL_SES_KEY_PROT = 2,
    /**< Key exchange protocol information. */ /* r/o */
  SECSSL_SES_CRYPT_ALGO = 3,
    /**< Encryption algorithm information. */ /* r/o */
  SECSSL_SES_HASH_ALGO = 4,
    /**< Hash algorithm information. */ /* r/o */
  SECSSL_SES_CERTIFICATE = 5,
    /**< X.509 certificate information. */ /* r/o */
  SECSSL_SES_CACHE_SIZE = 6,
    /**< Maximum size of session cache. */ /* r/o */
  SECSSL_SES_NAGLE_MODE = 9,
    /**< Enable/disable Nagle mode on transmission. */ /* r/w */
  SECSSL_SES_TX_PENDING = 10,
    /**< Number of bytes pending for Tx. */ /* r/o */
  SECSSL_SES_RX_PENDING = 11,
    /**< Number of bytes pending for Rx. */ /* r/o */
  SECSSL_SES_ALERT_CB = 12,
    /**< Session alert callback function pointer. */ /* w/o */
  SECSSL_SES_ERRNO_CB = 13,
    /**< Asynchronous command callback function pointer. */ /* w/o */
  SECSSL_SES_EVENT_CB = 14,
    /**< Asynchronous event callback function pointer. */ /* w/o */
  SECSSL_SES_SOCKET_MODE = 15,
    /**< Berkeley Software Distribution (BSD) style or event-based 
	sockets. */ /* w/o */
  SECSSL_SES_DS_APP_ID = 16,
    /**< Application ID required by the DS socket. */ /* w/o */
  SECSSL_SES_SOCKET_FD = 17,
    /**< File descriptor for external socket. */ /* w/o */
  SECSSL_SES_CA_CERTIFICATE = 18,
    /**< CA root certificate buffer. */ /* w/o */
  SECSSL_SES_CS_LIST = 19,
    /**< Preferred cipherlist to advertise. */ /* r/w */
  SECSSL_SES_DS_SOCKOPT = 20,
    /**< DS socket option. */ /* w/o */
  SECSSL_SES_SERVER_NAME_LIST = 21,
    /**< Server name list, as per RFC4366, Section 3.1. */ /* w/o */
  SECSSL_SES_CONNECTION_FPTRS = 22,
    /**< Data transfer function pointers. */ /* w/o */
  SECSSL_SES_SYS_TIME = 23, 
    /**< Certification validation time type. */ /* w/o */
  SECSSL_SES_IGNORE_EXPIRATION = 24,
    /**< Ignore time check for certification. */ /* w/o */
  SECSSL_SES_CLIENT_CERT_CONF = 25,
    /**< Client certification/key configuration file pattern. */ /* w/o */
  SECSSL_SES_SERVER_CERT_CONF = 26,
    /**< Server certification/key configuration file pattern. */ /* w/o */
  SECSSL_SES_CONNECTION_END = 27,
    /**< Set the client or server role for an SSL session;
    default is client. This parameter is to be the first parameter 
    to set. */ /* w/o */
  SECSSL_SES_CA_CERT_FOLDER = 28,
    /**< SSL uses certifcates under this folder path as CA canidates. */ /* w/o */
  SECSSL_SES_ACTIVE_CS      = 29,
  /**< SSL session active CS for Get operation*/ /* w/o */
  SECSSL_SES_PSK_DATA       = 30,
    /**< SSL session PSK data*/ /* w/o */
  SECSSL_SES_DH_MIN_KEY_SIZE = 31,
    /**< SSL session min. acceptable DH Key size*/ /* w/o */
  SECSSL_SES_RSA_MIN_KEY_SIZE = 32
    /**< SSL session min. acceptable RSA Key size*/ /* w/o */

} secssl_ses_parameter_enum_type;

/** Parameter for the SSL server.
*/
typedef enum
{
  SECSSL_HOST_NAME = 0,        /**< Hostname to be resolved. */
  SECSSL_HOST_DOT_ADDR,        /**< Host's IP address in dotted notation. */
  SECSSL_HOST_IN_ADDR          /**< Host's IP address as in_addr_t. */
} secssl_host_addr_enum_type;

/** @brief Parameter to set SECSSL_SES_SERVER. \n
    @note1hang The maximum length of the server name is defined per 
	SECSSL_SES_SERVER_MAX_NAME_LENGTH.
*/
typedef struct
{
  secssl_host_addr_enum_type  host_type;  /**< Indicates the address type. */
  ip_addr_enum_type addr_type;                 /**< Address Type.*/
  /** @brief Address. */
  union
  {
    char*   host_name;            /**< Server name or IP in dotted notation. */
    uint32  host_addr;            /**< Server IP address in network order. */
    struct  ps_in_addr host_in_addr; /**< Server IP address in in_addr type. */
    struct  ps_in6_addr host_in_v6_addr;
  } addr;
  uint16 port;                    /**< Server port number in network order. */
} secssl_server_pdata_type;

/** Parameter to set SECSSL_SES_SSL_VERSION.
*/
typedef enum
{
  SECSSL_SSL_VER_UNKNOWN =   0x0000,  /**< SSL protocol ver. unknown. */
  SECSSL_SSL_VER_10 = 0x0001,         /**< SSL protocol ver. 1.0. */
  SECSSL_SSL_VER_20 = 0x0002,         /**< SSL protocol ver. 2.0. */
  SECSSL_SSL_VER_30 = 0x0003,         /**< SSL protocol ver. 3.0. */
  SECSSL_SSL_VER_TLS10 = 0x0103,      /**< TLS protocol ver. 1.0 (SSL 3.1). */
  SECSSL_SSL_VER_TLS11 = 0x0203,      /**< TLS protocol ver. 1.1 (SSL 3.2). */
  SECSSL_SSL_VER_TLS12 = 0x0303       /**< TLS protocol ver. 1.2 (SSL 3.3). */
} secssl_ssl_version_enum_type;

/** Parameter to get SECSSL_SES_KEY_PROT.
*/
typedef enum
{
  SECSSL_KEY_NONE = 0,            /**< No key exchange. */
  SECSSL_KEY_DH_ANONYMOUS,        /**< Anonymous Diffie-Hellman. */
  SECSSL_KEY_DH_EPHEMERAL,        /**< Ephemeral Diffie-Hellman. */
  SECSSL_KEY_DH_DSS_SIGN,         /**< Diffie-Hellman with DSS signature. */
  SECSSL_KEY_DH_RSA_SIGN,         /**< Diffie-Hellman with RSA signature.  */
  SECSSL_KEY_DH_DSS_CERT,         /**< Diffie-Hellman with DSS certificate. */
  SECSSL_KEY_DH_RSA_CERT,         /**< Diffie-Hellman with RSA certificate. */
  SECSSL_KEY_RSA                  /**< RSA key exchange. */
} secssl_ses_key_prot_enum_type;

/** Parameter to get SECSSL_SES_CRYPT_ALGO.
*/
typedef enum
{
  SECSSL_CRYPT_NONE = 0            /**< No encryption. */
  ,SECSSL_CRYPT_RC4                /**< RC4 algorithm. */
  ,SECSSL_CRYPT_DES                /**< DES algorithm. */
  ,SECSSL_CRYPT_3DES               /**< 3DES algorithm. */
  ,SECSSL_CRYPT_AES128             /**< AES128 algorithm. */
  ,SECSSL_CRYPT_AES256             /**< AES256 algorithm. */
} secssl_ses_crypt_algo_enum_type;

/** Parameter to get SECSSL_SES_HASH_ALGO.
*/
typedef enum
{
  SECSSL_HASH_NONE = 0,            /**< No hash function. */
  SECSSL_HASH_SHA,                 /**< SHA-1 hash function. */
  SECSSL_HASH_MD5,                 /**< MD5 hash function. */
  SECSSL_HASH_SHA256               /**< SHA-256 hash function. */
} secssl_ses_hash_algo_enum_type;

/** Parameter to get SECSSL_SES_CACHE_SIZE. \n
  @note1hang The SSL session cache size is determined by Security Services 
       and cannot be changed by the application.
*/
typedef uint8 secssl_ses_cache_size_pdata_type;

/** Parameter to set SECSSL_SES_NAGLE_MODE.
*/ 
typedef enum
{
  SECSSL_NAGLE_DISABLED = 0,      /**< Deliver all Tx data immediately. */
  SECSSL_NAGLE_ENABLED            /**< Use Nagle mode to accumulate Tx data. */
} secssl_ses_nagle_mode_enum_type;

/** Parameter to set SECSSL_SES_TX_PENDING.
*/
typedef uint16 secssl_ses_tx_pending_pdata_type;

/** Parameter to set SECSSL_SES_RX_PENDING.
*/
typedef uint16 secssl_ses_rx_pending_pdata_type;


/** Parameter for Alert mode.
*/
typedef enum
{
  SECSSL_SES_ALERT_MODE_AUTO = 0,    /**< Automatically handle all events. */
  SECSSL_SES_ALERT_MODE_USER = 1,    /**< Request user action for 
                                          security-related events. */
  SECSSL_SES_ALERT_MODE_CERTINFO = 2 /**< Always suspend after certificate
                                          parsing. */
} secssl_ses_alert_mode_enum_type;

/** @brief Parameter to set SECSSL_SES_ALERT_CB. \n
  @note1hang The value is invalid if NULL.
*/
typedef struct
{
  secssl_ses_alert_mode_enum_type  mode;     /**< Alert callback mode. */
  secssl_ses_alert_cb_fptr_type    func_ptr; /**< Alert callback function
                                                  pointer. */
} secssl_ses_alert_cb_pdata_type;

/** @brief Parameter to set SECSSL_SES_ERRNO_CB. \n
  @note1hang The value is invalid if NULL.
*/
typedef struct
{
  secerrno_cb_fptr_type            func_ptr;  /**< Error callback function
                                                   pointer. */
  void*                            udata_ptr; /**< User data pointer. */
} secssl_ses_errno_cb_pdata_type;


/** Parameter to set SECSSL_SES_SOCKET_MODE.
*/
typedef enum
{
  SECSSL_SOCKET_MODE_BSD = 0,  /**< BSD style socket. */
  SECSSL_SOCKET_MODE_EVENT     /**< Event-based socket. */
} secssl_ses_socket_mode_enum_type;

/** Parameter to set SECSSL_SES_DS_APP_ID.
*/
typedef sint15 secssl_ses_ds_app_id_pdata_type;

/** Parameter to set SECSSL_SES_SOCKET_FD.
*/
typedef sint15 secssl_ses_socket_fd_pdata_type;

/** @brief Parameter to set SECSSL_SES_CA_CERTIFICATE.
*/
typedef struct
{
  uint8*  cert_data;               /**< ASN.1-encoded X.509 certificate buffer;
								        only .der format is supported. */
  uint16  cert_len;                /**< Length of the data array for the 
                                        certificate. */
} secssl_ses_ca_certificate_pdata_type;

/** @brief Parameter to set SECSSL_SES_CS_LIST.
*/
typedef struct
{
  uint16  list[SECSSL_SES_CS_LIST_MAX_SIZE];
    /**< SSL ciphersuite list to advertise in the ClientHello message. */
  uint16  size;
    /**< Setting a list size of zero resets the session to the default. */
} secssl_ses_cs_list_pdata_type;

/** Parameter for the DS sockets options.
*/
typedef enum
{
  SECSSL_DSS_SO_LINGER,       /**< Map to DSS_SO_LINGER. */
  SECSSL_DSS_SO_LINGER_RESET  /**< Map to DSS_SO_LINGER_RESET. */
} secssl_ses_ds_sockopt_name_type;

/** @brief Parameter for the DS sockets options.
*/
typedef union
{
  int linger;                /**< Input value for dss_setsockopt. */
} secssl_ses_ds_sockopt_val_type;

/** @brief Parameter to set SECSSL_SES_DS_SOCKOPT; set only if using the SSL 
    internal socket.
*/
typedef struct
{
  secssl_ses_ds_sockopt_name_type name; /**< Input value for dss_setsockopt. */
  secssl_ses_ds_sockopt_val_type val;   /**< Input value for dss_setsockopt. */
} secssl_ses_ds_sockopt_type;

/** @brief Parameter to set SECSSL_SES_SERVER_NAME_LIST; support for the
  TLS server name list extension (RFC4366).
*/
typedef struct secssl_ses_server_name_list_pdata_struct
{
  secssl_host_addr_enum_type name_type; /**< Server name type. */
  /** @brief Name. */
  union
  {
    char* host_name_ptr; /**< SSL server name list extension (RFC4366).\ The
                              pointer is used by the SSL session and it is the
                              responsibility of the application to free it 
                              after the session is ended. */
  } name;
  struct secssl_ses_server_name_list_pdata_struct* next_ptr; 
                         /**< Pointer to the next server name. */
} secssl_ses_server_name_list_pdata_type;

/* Transport function pointer declarations */
typedef sint15 (*secssl_rx_fptr_type) (sint15, dsm_item_type **, sint15 *);
/**< Function prototype of rx_fptr in session parameter 
secssl_ses_connection_fptrs_pdata_type. */
typedef sint15 (*secssl_tx_fptr_type) (sint15, dsm_item_type **, sint15 *);
/**< Function prototype of tx_fptr in session parameter 
secssl_ses_connection_fptrs_pdata_type. */
typedef sint31 (*secssl_select_fptr_type) (sint15, sint31, sint15 *);
/**< Function prototype of select_fptr in session parameter 
secssl_ses_connection_fptrs_pdata_type. */
typedef sint15 (*secssl_deselect_fptr_type) (sint15, sint31, sint15 *);
/**< Function prototype of deselect_fptr in session parameter 
secssl_ses_connection_fptrs_pdata_type. */
typedef sint31 (*secssl_next_event_fptr_type) (sint15, sint15*, sint15 *);
/**< Function prototype of next_event_fptr in session parameter 
secssl_ses_connection_fptrs_pdata_type. */

/** @brief Parameter to set SECSSL_SES_CONNECTION_FPTRS.
*/
typedef struct secssl_ses_connection_fptr_pdata_struct
{             
  secssl_rx_fptr_type         rx_fptr;         /**< Default is
                                                    dss_read_dsm_chain. */
  secssl_tx_fptr_type         tx_fptr;         /**< Default is
                                                    dss_write_dsm_chain. */
  secssl_select_fptr_type     select_fptr;     /**< Default is
                                                    dss_async_select. */
  secssl_deselect_fptr_type   deselect_fptr;   /**< Default is
                                                    dss_async_deselect. */
  secssl_next_event_fptr_type next_event_fptr; /**< Default is
                                                    dss_getnextevent. */
} secssl_ses_connection_fptrs_pdata_type;

/** Parameter of time type for a certificate expiration check.
*/
typedef enum
{
  SECSSL_SES_SECURE_TIME = 0,     /**< Secure time comes with the device;
                                       the SSL uses this time by default. */
  SECSSL_SES_USER_TIME            /**< User time can be changed through the
                                       UI. */
} secssl_sys_time_enum_type;

/** Parameter to set SECSSL_SES_SYS_TIME.
*/
typedef secssl_sys_time_enum_type secssl_ses_sys_time_data_type;  

/** Parameter to set SECSSL_SES_IGNORE_EXPIRATION.
*/
typedef boolean secssl_ses_ignore_expiration_data_type;

/** @brief Parameter that points to the EFS path to read in the 
    certificate files and key file.
*/
typedef struct
{
  boolean  session_cert;  
  /**< TRUE -- Certificate will be put in a user folder. \n
       FALSE -- Certificates will be put under a default folder: \n
       - Default provision file for the client certificate is 
         \\client-cert\\cert.conf.
       - Default provision file for the server certificate is 
         \\server-cert\\cert.conf. */
  char     *session_cert_config; 
  /**< If session_cert is TRUE, this is the user certificate provision 
       file path. */ 
}secssl_ses_cert_conf_data_type;

/** Parameter to set SECSSL_SES_CONNECTION_END.
*/
typedef enum
{
  SECSSL_CLIENT = 0,    /**< SSL client session. */
  SECSSL_SERVER = 1     /**< SSL server session. */
}secssl_ses_connection_end_enum_type;

/** Parameter to set SECSSL_SES_CA_CERT_FOLDER.
   If user doesn't specify session CA root certificates folder path,
   SSL engine will search CA root certificates under "cert/" which is default folder.
   If SECSSL_SES_CA_CERT_FOLDER is set,SSL engine will only use certificates under this
   folder as CA candidates.
   If session_ca_cert_folder doesn't exist on EFS, SSL engine will search default folder.
   SSL engine will parse all files under session_ca_cert_folder only, sub-folder 
   will not be searched.
   Sample path: "cert/", "oem/root_cert/"
*/
typedef struct
{
  char		*session_ca_cert_folder;    /**< Caller's CA root certificates are put under this folder. */
}secssl_ses_ca_cert_folder_data_type;

/** @brief @latexonly \label{hdr:secsslSesParameterPdataUnionType} @endlatexonly
  SSL session parameters. 
*/
typedef union
{
  secssl_server_pdata_type              server_pdata;
  secssl_ssl_version_enum_type          ssl_version_pdata;
  secssl_ses_key_prot_enum_type         key_prot_pdata;
  secssl_ses_crypt_algo_enum_type       crypt_algo_pdata;
  secssl_ses_hash_algo_enum_type        hash_algo_pdata;
  secx509_ses_certificate_pdata_type    certificate_pdata;
  secssl_ses_cache_size_pdata_type      cache_size_pdata;
  secssl_ses_nagle_mode_enum_type       nagle_mode_pdata;
  secssl_ses_tx_pending_pdata_type      tx_pending_pdata;
  secssl_ses_rx_pending_pdata_type      rx_pending_pdata;
  secssl_ses_alert_cb_pdata_type        alert_cb_pdata;
  secssl_ses_errno_cb_pdata_type        errno_cb_pdata;
  secssl_ses_event_cb_fptr_type         event_cb_pdata;
  secssl_ses_socket_mode_enum_type      socket_mode_pdata;
  secssl_ses_ds_app_id_pdata_type       ds_app_id_pdata;
  secssl_ses_socket_fd_pdata_type       socket_fd_pdata;
  secssl_ses_ca_certificate_pdata_type  ca_certificate_pdata;
  secssl_ses_cs_list_pdata_type         cs_list_pdata;
  secssl_ses_ds_sockopt_type            ds_sockopt;
  secssl_ses_server_name_list_pdata_type* server_name_list_pdata_ptr;
  secssl_ses_connection_fptrs_pdata_type  connection_fptr_pdata;
  secssl_ses_sys_time_data_type           sys_time_pdata;
  secssl_ses_ignore_expiration_data_type  ignore_expiration_pdata;
  secssl_ses_cert_conf_data_type          clnt_cert_conf_pdata;
  secssl_ses_cert_conf_data_type          svr_cert_conf_pdata;
  secssl_ses_connection_end_enum_type     connection_end_pdata;
  secssl_ses_ca_cert_folder_data_type     ca_cert_folder_pdata;
  uint16                                  cs_active_pdata;
  uint16                                  dh_min_key_size;
  uint16                                  rsa_min_key_size;
} secssl_ses_parameter_pdata_union_type;


/** Data services event types (for external sockets).
*/
typedef enum
{
  SECSSL_DS_NETWORK_EVENT = 0,  /**< Network event. */
  SECSSL_DS_SOCKET_EVENT = 1    /**< Socket event. */
} secssl_ds_event_enum_type;

/** 
   @}
 */
 /*  end_addtogroup ssl_ses_param */
/*
[YH] Below parameters will not recorded in ISOD */

/** @cond
*/
/* Parameter to retrieve eap value from SSL session, required by QC WLAN eap 
*/
typedef struct
{
  secssl_handle_type    session_handle;
  uint8*                key_label;
  uint16                key_label_len;
}secssl_eap_session_key_type;

typedef struct
{
  dsm_item_type*        seed_ptr;
  uint16                seed_len;
  dsm_item_type*        secret_ptr;
  uint16                secret_len;
}secssl_eap_intermediate_combined_key_type;

typedef struct
{
  dsm_item_type**       out_ptr;
  uint16                out_len;
}secssl_eap_encrypt_output_type;

typedef union
{
  secssl_eap_session_key_type                 eap_tls_session_key;
  secssl_eap_intermediate_combined_key_type   eap_tls_intermediate_combined_key;
} secssl_eap_encrypt_input_union_type;

/* Enumumeration for the enryption data algorithm.
*/
typedef enum
{
  SECSSL_EAP_TLS_KEY = 0,           /* Generate an EAP-TLS key or IV from
									   the SSL session and key label. */
  SECSSL_EAP_SHA     = 1            /* Generate an SHA for the EAP key. */
} secssl_eap_encrypt_algorithm_enum_type;

/* @endcond
*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/**
  @addtogroup create_session 
  @{ */

/*===========================================================================
 Function:   secssl_init
===========================================================================*/
/**
  Initializes the SSL and crypto layers of the SEC.
 
  @return
  None.
 
  @dependencies
  None.
*/
extern void secssl_init( void );

/* ============================================================================
**             Creating SSL session APIs
** ========================================================================== */

/* ============================================================================
   Function: secssl_new_session
=============================================================================*/
/**
  Creates a new SSL session. SecSSL services allows a maximum of three SecSSL 
  sessions to be created on the AMSS stack.

  @latexonly
  Figure \ref{fig:createSession} illustrates the process flow for creating 
  a session. \\*

  \vspace{0.2in}
  \begin{figure}[H]
  \centering
  \includegraphics[width=3in]{images/create_session.png} %Replace with your call flow diagram.
  \renewcommand{\thefigure}{\thechapter-\arabic{figure}} %Do not change this line.
  %\vspace{0.15in}
  \caption{Create session process flow}
  \label{fig:createSession}
  \end{figure}
  @endlatexonly
 
  @param[in,out] session_handle_ptr Pointer to a session handle; set by the
                                    function as an identifier for subsequent
                                    API calls into this session.
  @return
  Returns secerrno_enum_type, which is the security services standard error
  number. A possible error condition is: \n
  - E_NOT_AVAILABLE -- All SSL sessions are already in use.
  
  @dependencies
  None.
 
  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_new_session
(
  secssl_handle_type* session_handle_ptr
);

/** 
  @}
*/
/*  end_addtogroup create_session */

/*=============================================================================
                  TLS handshake session API
=============================================================================*/
/**
  @addtogroup handshake_session 
  @{ */

/*=============================================================================
  Function:    secssl_open_session
=============================================================================*/
/**
  An SecSSL client does an SSL handshake with an SSL server.

  @latexonly
  Figure \ref{fig:clientSessionOpen} illustrates the process sequence for an 
  SecSSL server session connection to a TLS server. \\*

  \vspace{0.2in}
  \begin{figure}[H]
  \centering
  \includegraphics[width=3.5in]{images/client_session_(open).png} %Replace with your call flow diagram.
  \renewcommand{\thefigure}{\thechapter-\arabic{figure}} %Do not change this line.
  %\vspace{0.15in}
  \caption{Client session handshake process flow}
  \label{fig:clientSessionOpen}
  \end{figure}
  @endlatexonly
    
  @param[in] session_handle    Handle for a valid session.
  @param[in] open_cb_fptr      Pointer to a callback function of the following
                               type (invalid if set to NULL):\n
                @verbatim

                void (callback_func)
                (
                  void*              usr_data_ptr,
                  secerrno_enum_type errno
                )
                @endverbatim
  @param[in] open_cb_udata_ptr Pointer to application (user) data for a
                               callback.
 
  @latexonly \newpage @endlatexonly
  @return
  Returns an SecSSL error number to indicate execution outcome. 
  Possible error conditions are:\n
  - E_INVALID_ARG   -- Session handle is not valid.
  - E_NOT_ALLOWED   -- Not all required session data is initialized.
  - E_NOT_AVAILABLE -- TCP connection to the server failed.
  - E_FAILURE       -- Error occurred during handshake protocol initialization.
 
  @dependencies
  session_handle must be a valid session instance created before this
  function is called. \n
  SECSSL_SES_CONNECTION_END must be set to SECSSL_CLIENT. \n
  The SECSSL_SES_SERVER parameter must be set to a valid address. \n
  SECSSL_SES_ALERT_CB and SECSSL_SES_ERRNO_CB must set to valid callback pointers.

  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_open_session
(
  secssl_handle_type     session_handle,
  secerrno_cb_fptr_type  open_cb_fptr,
  void*                  open_cb_udata_ptr
);

/*=============================================================================
  Function:    secssl_abort_open_session
=============================================================================*/
/**
  Aborts an SecSSL client session that is doing a handshake with a server.
  This operation is supported during DNS lookups, TCP connect processing, and
  an SSL handshake.
 
  @param[in] session_handle Handle for a valid session.
 
  @return
  Returns a Security Services error number to indicate execution outcome.
  Possible error conditions are: \n
  - E_NOT_ALLOWED -- Session is not in the process of connecting or
                     opening.
   
  @dependencies
  session_handle must be a valid session instance created before calling this
  function; the session must be in the HANDSHAKE_STATE.

  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_abort_open_session
(
  secssl_handle_type     session_handle
);

/*=============================================================================
  Function:    secssl_continue_session
=============================================================================*/
/**
  Continues opening a session that had been suspended due to a trust failure,
  e.g., a missing root certificate by the application.
  The session must be configured with the Alert mode set to User.
 
  @latexonly \newpage @endlatexonly
  @param[in] session_handle   Handle for a valid session.
  @param[in] continue_cb_fptr Pointer to a callback function of the
                              following type (invalid if set to NULL): \n
                @verbatim

                void (callback_func)
                (
                  void*              usr_data_ptr,
                  secerrno_enum_type errno
                )
                @endverbatim
  @param[in] continue_cb_udata_ptr Pointer to application (user) data for
                                   callback.
  @return
  Returns a Security Services error number to indicate execution outcome.
  Possible error conditions are: \n
  - E_INVALID_ARG   -- Session handle is not valid.
  - E_NOT_ALLOWED   -- Session is not suspended.
  - E_NOT_AVAILABLE -- TCP connection to the server failed.
  - E_FAILURE       -- Error occurred during handshake protocol continuation.
 
  @dependencies
  None.

  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_continue_session  
(  
  secssl_handle_type     session_handle,
  secerrno_cb_fptr_type   continue_cb_fptr,
  void*                   continue_cb_udata_ptr
);  

  

/*=============================================================================
             Accept an SSL session, SSL server API
=============================================================================*/

/*=============================================================================
  Function:    secssl_accept_session
=============================================================================*/
/**
  An SecSSL server does an SSL handshake with an SSL client.

  @latexonly
  Figure \ref{fig:serverSessionAccept} illustrates the process sequence for 
  an SecSSL server session connection to a TLS client. \\*

  \vspace{0.2in}
  \begin{figure}[H]
  \centering
  \includegraphics[width=3.5in]{images/server_session_(accept).png} %Replace with your call flow diagram.
  \renewcommand{\thefigure}{\thechapter-\arabic{figure}} %Do not change this line.
  %\vspace{0.15in}
  \caption{Server session handshake process flow}
  \label{fig:serverSessionAccept}
  \end{figure}
  @endlatexonly

  

  @param[in] session_handle    Handle for a valid session.
  @param[in] accept_cb_fptr    Pointer to a callback function of the following
                               type (invalid if set to NULL):\n
                @verbatim

                void (callback_func)
                (
                  void*              usr_data_ptr,
                  secerrno_enum_type errno
                )
                @endverbatim
  @param[in] accept_cb_udata_ptr Pointer to application (user) data for a
                                 callback.
 
  @return
  Returns an SecSSL error number to indicate execution outcome. 
  Possible error conditions are:\n
  - E_INVALID_ARG   -- Session handle is not valid.
  - E_NOT_ALLOWED   -- Not all required session data is initialized correctly.
  - E_FAILURE       -- Error occurred during handshake protocol initialization.
 
  @dependencies
  session_handle must be a valid session instance created before this
  function is called. \n
  @latexonly \vspace{0.05in}@endlatexonly
  Caller application must create and listen to a DS server socket. When 
  ds_accept is asserted and a new socket is in the Connected state, the caller 
  can start an SecSSL server session by calling secssl_accept_session. \n
  @latexonly \vspace{0.05in}@endlatexonly
  SECSSL_SES_CONNECTION_END must be set to SECSSL_SERVER. \n
  @latexonly \vspace{0.05in}@endlatexonly
  SECSSL_SES_DS_FD parameter must be set to the new connected socket descriptor. \n
  @latexonly \vspace{0.05in}@endlatexonly
  SECSSL_SES_ALERT_CB and SECSSL_SES_ERRNO_CB must set to valid callback pointers.

  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_accept_session
(
  secssl_handle_type     session_handle,
  secerrno_cb_fptr_type  accept_cb_fptr,
  void*                  accept_cb_udata_ptr
);

/*=============================================================================
  Function:    secssl_abort_accept_session
=============================================================================*/
/**
  Aborts an SecSSL server session that is doing handshake with a client.
  This operation is supported during an SSL handshake.
 
  @param[in] session_handle Handle for a valid session.
 
  @return
  Returns a Security Services error number to indicate execution outcome.
  Possible error conditions are: \n
  - E_NOT_ALLOWED -- Session is not in the process of connecting or
                     opening.
   
  @dependencies
  session_handle must be a valid session instance created before calling this
  function; the session must be in the HANDSHAKE_STATE.

  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_abort_accept_session
(
  secssl_handle_type     session_handle
);

/** 
  @}
*/
/*  end_addtogroup handshake_session */
 

/* ============================================================================
**             Reading and writing data APIs
** ========================================================================== */
/**
  @addtogroup read_write_session 
  @{ */

/*=============================================================================
  Function : secssl_write
=============================================================================*/
/**
  Writes the specified byte array to the secure socket associated with the
  session handle in a non-blocking, event-based interface.
  Care must be taken to not release data until after the completion event.

  @latexonly
  Figure \ref{fig:writeSession} illustrates the process flow for a 
  write session. \\*

  \vspace{0.2in}
  \begin{figure}[H]
  \centering
  \includegraphics[width=3.5in]{images/write_session.png} %Replace with your call flow diagram.
  \renewcommand{\thefigure}{\thechapter-\arabic{figure}} %Do not change this line.
  %\vspace{0.15in}
  \caption{Write session process flow}
  \label{fig:writeSession}
  \end{figure}
  @endlatexonly
 
  @param[in] session_handle Handle for a valid session.
  @param[in] data_ptr       Pointer to a byte array containing the Tx data.
  @param[in] data_len_ptr   Pointer to the size of the array; a data length
                            of zero is invalid.

  @return
  Returns a Security Services error number to indicate execution outcome.
  Possible error conditions are: \n
  - E_INVALID_ARG   -- Invalid session handle, data pointer, or length.
  - E_WOULD_BLOCK   -- Socket not ready, retry later.
  - E_FAILURE       -- Unknown error occurred on the socket.
  - E_NOT_AVAILABLE -- Peer closed the TCP connection.
  - E_RESET         -- Socket received a TCP Reset.

  @dependencies
  The SSL session must be open.

  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_write
(
  secssl_handle_type  session_handle,
  uint8*              data_ptr,
  uint16*             data_len_ptr
);

/*=============================================================================
  Function:    secssl_sync_write
=============================================================================*/
/**
  Writes to a secure socket.
  
  This function is the same as secssl_write, except this is a blocking
  function. This function returns when the whole write action is completed.

  @param[in] session_handle Handle for a valid session.
  @param[in] data_ptr       Pointer to a byte array containing the Tx data.
  @param[in] data_len_ptr   Pointer to the size of the array; a data length
                            of zero is invalid.

  @return
  Returns a Security Services error number to indicate execution outcome.
  Possible error conditions are: \n
  - E_INVALID_ARG   -- Invalid session handle, data pointer, or length.
  - E_WOULD_BLOCK   -- Socket not ready; retry later.
  - E_FAILURE       -- Unknown error occurred on the socket.
  - E_NOT_AVAILABLE -- Peer closed the TCP connection.
  - E_RESET         -- Socket received a TCP Reset.

  @dependencies
  The SSL session must be open.

  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
secerrno_enum_type secssl_sync_write
(
  secssl_handle_type  session_handle,
  uint8*              data_ptr,
  uint16*             data_len_ptr
);

/*=============================================================================
  Function: secssl_read
=============================================================================*/
/**
  Reads from a secure socket.

  @latexonly
  Figure \ref{fig:readSession} illustrates the process flow for a 
  read session. \\*

  \vspace{0.2in}
  \begin{figure}[H]
  \centering
  \includegraphics[width=3.5in]{images/read_session.png} %Replace with your call flow diagram.
  \renewcommand{\thefigure}{\thechapter-\arabic{figure}} %Do not change this line.
  %\vspace{0.15in}
  \caption{Read session process flow}
  \label{fig:readSession}
  \end{figure}
  @endlatexonly
  
  This function reads up to the specified number of bytes into the given
  byte array in non-blocking mode.
 
  @param[in] session_handle   Handle for a valid session.
  @param[in,out] data_ptr     Pointer to a byte array of data_len_ptr size
                              where the Rx data is written.
  @param[in,out] data_len_ptr Pointer to the size of the array; set to the
                              actual number of bytes received by the
                              function. An input data length of zero is
                              invalid.

  @return
  Returns a Security Services error number to indicate execution outcome.
  Possible error conditions are: \n
  - E_INVALID_ARG   -- Invalid session handle, data pointer, or length.
  - E_WOULD_BLOCK   -- Socket not ready, retry later.
  - E_FAILURE       -- Unknown error occurred on the socket.
  - E_NOT_AVAILABLE -- Peer closed the TCP connection.
  - E_RESET         -- Socket received a TCP Reset.

  @dependencies
  The SSL session must be open and enough memory must be allocated by the 
  application to read data into memory.

  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_read
(
  secssl_handle_type  session_handle,
  uint8*              data_ptr,
  uint16*             data_len_ptr
);

/** 
  @}
*/
/*  end_addtogroup read_write_session */


/* ============================================================================
**             Closing a SSL session APIs
** ========================================================================== */
/**
  @addtogroup close_session 
  @{ */

/*=============================================================================
  Function:    secssl_close_session
=============================================================================*/
/**
  Closes a session that opened successfully, resumed successfully, or is in a
  suspended state due to a trust failure.

  @latexonly
  Figure \ref{fig:closeSession} illustrates the process flow for closing 
  a session. \\*

  \vspace{0.2in}
  \begin{figure}[H]
  \centering
  \includegraphics[width=3.5in]{images/close_session.png} %Replace with your call flow diagram.
  \renewcommand{\thefigure}{\thechapter-\arabic{figure}} %Do not change this line.
  %\vspace{0.15in}
  \caption{Close session process flow}
  \label{fig:closeSession}
  \end{figure}
  @endlatexonly
 
  This function closes an open SSL session without deleting the instance, which
  allows it to resume at a later time.
 
  @param[in] session_handle Handle for a valid session.
  @param[in] close_cb_fptr  Pointer to a callback function of the following
                            type (invalid if set to NULL): \n
                @verbatim

                void (callback_func)
                (
                  void*              usr_data_ptr,
                  secerrno_enum_type errno
                )
                @endverbatim
  @param[in] close_cb_udata_ptr Pointer to application (user) data for
                                callback.
  @return
  Returns a Security Services error number to indicate execution outcome.
  Possible error conditions are: \n
  - E_INVALID_ARG   -- Session handle is not valid.
  - E_FAILURE       -- Unknown error occurred.
  - E_NOT_AVAILABLE -- Peer closed the TCP connection.
  - E_RESET         -- Socket received a TCP Reset.

  @dependencies
  The SSL session must be open, otherwise the command is ignored.

  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_close_session  
(  
  secssl_handle_type     session_handle,
  secerrno_cb_fptr_type   close_cb_fptr,
  void*                   close_cb_udata_ptr
);
/** 
   @}
*/
/*  end_addtogroup close_session */


/* ============================================================================
**             Resuming a SSL session APIs
** ========================================================================== */
/**
  @addtogroup resume_session 
  @{ */

/*=============================================================================
  Function:    secssl_resume_session
=============================================================================*/
/**
  Resumes an existing SSL session that had been opened and was then closed.

  @latexonly
  Figure \ref{fig:resumeSession} illustrates the process flow for resuming 
  a client session. \\*

  \vspace{0.2in}
  \begin{figure}[H]
  \centering
  \includegraphics[width=3.5in]{images/resume_session.png} %Replace with your call flow diagram.
  \renewcommand{\thefigure}{\thechapter-\arabic{figure}} %Do not change this line.
  %\vspace{0.15in}
  \caption{Resume session process flow}
  \label{fig:resumeSession}
  \end{figure}
  @endlatexonly
 
  @param[in] session_handle Handle for a valid session.
  @param[in] resume_cb_fptr Pointer to a callback function of the following
                            type (invalid if set to NULL): \n
                @verbatim

                void (callback_func)
                (
                  void*              usr_data_ptr,
                  secerrno_enum_type errno
                )
                @endverbatim
  @param[in] resume_cb_udata_ptr Pointer to application (user) data for
                                 callback.

  @return
  Returns a Security Services error number to indicate execution outcome.
  Possible error conditions are: \n
  - E_INVALID_ARG   -- Session handle is not valid.
  - E_NOT_ALLOWED   -- Session is not resumable (marked by server or aborted).
  - E_NOT_AVAILABLE -- TCP connection to the server failed.
  - E_FAILURE       -- Error occurred during handshake protocol initialization.
  
  @dependencies
  The SSL session must have been opened at least once and must be resumable.

  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_resume_session  
(  
  secssl_handle_type     session_handle,
  secerrno_cb_fptr_type   resume_cb_fptr,
  void*                   resume_cb_udata_ptr
);  
/** 
  @}
*/
/*  end_addtogroup resume_session */

/*=============================================================================
             Deleting a SSL session APIs
** ========================================================================== */
/**
  @addtogroup delete_session 
  @{ */

/*=============================================================================
  Function:    secssl_delete_session
=============================================================================*/
/**
  Deletes a previously closed SSL session instance.

  @latexonly
  Figure \ref{fig:deleteSession} illustrates the process flow for deleting 
  a session. \\*

  \vspace{0.2in}
  \begin{figure}[H]
  \centering
  \includegraphics[width=3.5in]{images/delete_session.png} %Replace with your call flow diagram.
  \renewcommand{\thefigure}{\thechapter-\arabic{figure}} %Do not change this line.
  %\vspace{0.15in}
  \caption{Delete session process flow}
  \label{fig:deleteSession}
  \end{figure}
  @endlatexonly

  @param[in] session_handle Handle for a valid session.

  @latexonly \newpage @endlatexonly
  @return
  Returns a Security Services error number to indicate execution outcome.
  Possible error conditions are: \n
  - E_INVALID_ARG -- Session handle is not valid.

  @dependencies
  The SSL session must be closed.

  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_delete_session  
(  
  secssl_handle_type  session_handle
);
/** 
  @}
*/
/*  end_addtogroup delet_session */

/* ============================================================================
**             Controlling a SSL session APIs
** ========================================================================== */
/**
  @addtogroup control_session
  @{ */

/*=============================================================================
  Function:    secssl_set_sesparam
=============================================================================*/
/**
  Sets a session parameter.
 
  @param[in] session_handle Handle for a valid session.
  @param[in] param          Parameter to set.
  @param[in] pdata_ptr      Pointer to parameter-specific data (invalid if
                            set to NULL).
 
  @return
  Returns a Security Services error number to indicate execution outcome.
  Possible error conditions are:
  - E_INVALID_ARG -- Session handle or parameter is not valid.
  - E_NOT_ALLOWED -- Parameter is read-only.
  - E_FAILURE     -- Unknown error occurred during execution.
 
  @dependencies
  None.

  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_set_sesparam  
(  
  secssl_handle_type                      session_handle,
  secssl_ses_parameter_enum_type          param,
  secssl_ses_parameter_pdata_union_type*  pdata_ptr
);

/*=============================================================================
  Function:    secssl_get_sesparam
=============================================================================*/
/**
 Gets a session parameter.

  @param[in] session_handle Handle for a valid session.
  @param[in] param          Parameter to retrieve.
  @param[in,out] pdata_ptr  Pointer to parameter-specific data (invalid if
                            NULL).

  @return
  Returns a Security Services error number to indicate execution outcome.
  Possible error conditions are:
  - E_INVALID_ARG   -- Session handle or parameter is not valid.
  - E_NOT_AVAILABLE -- Parameter is write-only.
  - E_FAILURE       -- Unknown error occurred during execution.

  @dependencies
  None.

  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_get_sesparam  
(
  secssl_handle_type                      session_handle,
  secssl_ses_parameter_enum_type          param,
  secssl_ses_parameter_pdata_union_type*  pdata_ptr
);


/** 
  @}
*/
/*  end_addtogroup control_session */


/* ============================================================================
**            Data services interfaces APIs
** ========================================================================== */
/**
  @addtogroup dss_interface
  @{ */

/*=============================================================================
  Function:    secssl_get_ds_app_id
=============================================================================*/
/**
  Gets an application ID from Data Services (DS).
 
  This function retrieves a new application ID from DS for this SSL 
  application. It also sets DS to Sockets mode and opens the 
  network library.
 
  @param[in,out] ds_app_id_ptr Pointer to the application ID; filled in by
                               the function.
 
  @return
  Returns a Security Services error number to indicate execution outcome.
 
  @dependencies
  None.

  @sideeffects
  The Access Terminal is set to Sockets mode and the network library
  is opened.
 
  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_get_ds_app_id
(
  sint15* ds_app_id
);

/**
  @addtogroup dss_interface
  @{ */

/*=============================================================================
  Function:    secssl_get_ds_app_id_v6
=============================================================================*/
/**
  Gets an application ID from Data Services (DS) for IPv6.
 
  This function retrieves a new application ID from DS for this SSL 
  application. It also sets DS to Sockets mode and opens the 
  network library.
 
  @param[in,out] ds_app_id_ptr Pointer to the application ID; filled in by
                               the function.
                 session_handle
 
  @return
  Returns a Security Services error number to indicate execution outcome.
 
  @dependencies
  None.

  @sideeffects
  The Access Terminal is set to Sockets mode and the network library
  is opened.
 
  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
extern secerrno_enum_type secssl_get_ds_app_id_v6
(
  sint15* ds_app_id,
  secssl_handle_type session_handle
);

/*=============================================================================
  Function:    secssl_close_network
=============================================================================*/
/**
  Closes the Point-to-Point Protocol (PPP) stack for this application.
 
  This function releases the PPP session and network library for this
  application. PPP and the network are actually closed once all applications
  release their application IDs.
 
  @param[in] ds_app_id     Application ID for which to release PPP and the
                           network library.
  @param[in] close_cb_fptr Pointer to a callback function of the following
                           type (invalid if set to NULL): \n
               @verbatim

               void (callback_func)
               (
                 void*              usr_data_ptr,
                 secerrno_enum_type errno
               )
               @endverbatim
  @param[in] close_cb_udata_ptr Pointer to the application (user) data for
                                callback.

  @return
  Returns a Security Services error number to indicate execution outcome. 
  Possible error conditions are: \n
  - E_INVALID_ARG -- Session handle or parameter is not valid.
  - E_FAILURE     -- Unknown error occurred during execution.
  - E_SUCCESS     -- Successfully closed the network.
  
  @dependencies
  None.
        
  @sideeffects
  The current PPP session and the network library are closed if
  no other sockets application is currently open.
 
  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.
*/
secerrno_enum_type secssl_close_network
(
  sint15 ds_app_id,
  secerrno_cb_fptr_type   close_cb_fptr,
  void*                   close_cb_udata_ptr
);
/*=============================================================================
  Function:    secssl_ds_default_event_handler
=============================================================================*/
/**
  Default processor for DS events if the session is configured with an external
  socket. The external socket is created by dss_socket(). The SSL engine passes 
  a socket event to all active sessions when secssl_ds_default_event_handler 
  is asserted.
 
  This function allows the SSL application to only process events from the SSL
  event callback.
 
  @param[in] event_type Either a network or socket event, according to the
                        following: \n
                @verbatim

                typedef enum
                {
                  SECSSL_DS_NETWORK_EVENT = 0,
                  SECSSL_DS_SOCKET_EVENT
                } secssl_ds_event_enum_type;
                @endverbatim
 
  @return
  None.

  @dependencies
  None.
*/
extern void secssl_ds_default_event_handler
(
  secssl_ds_event_enum_type  event_type
);
/*=============================================================================
  Function:    secssl_ds2_default_sock_event_handler
=============================================================================*/
/**
  Default processor for DS events if the session is configured with an external
  socket. This external socket is created by dss_socket2(). Users are encouraged 
  to use dss_socket2() and secssl_ds2_default_sock_event_handler; with the 
  input parameter from the socket2 event callback function, the SSL engine 
  can pass the event to the corresponding session if multiple SSL sessions 
  are running.
 
  This function allows the SSL application to only process socket events from
  the SSL event callback.
 
  @param[in] appid          Application ID.
  @param[in] sockfd         Socket descriptor.
  @param[in] event_mask     Event occurred.
  @param[in] session_handle Active SSL session handle.
 
  @return
  None.

  @dependencies
  None.
*/
void secssl_ds2_default_sock_event_handler (
  sint15 appid,                            
  sint15 sockfd,                          
  uint32 event_mask,                      
  secssl_handle_type session_handle       
);
/** 
  @}
*/
/*  end_addtogroup dss_interface */

/* ============================================================================
**              Get EAP data APIs
** ========================================================================== */

/** @cond
*/

/*=============================================================================
  Function:    secssl_get_hsp_state
=============================================================================*/
/**
  Gets the lastest SSL handshake state.
 
  @param[in] session_handle Handle for a valid session.
  @param[in] hsp_state      Enumerator value of handshake state pointer.
 
  @return
  Returns a Security Services error number to indicate execution outcome.

  @dependencies
  secssl_open_session() must have been called.

  @sa
  Appendix A.
*/
secerrno_enum_type secssl_get_hsp_state
(
  secssl_handle_type                 session_handle,
  ssl_v3_hsp_msg_id_enum_type*       hsp_state
);
/*=============================================================================
  Function:    secssl_encrypt_eap_data
=============================================================================*/
/**
  Creates encryption data for EAP/EAP-TLV/PEAP/TTLS based on key algorithm and
  input data

  @param[in] encrypt_algorithm Enumerator type for which encryption data to
                               derive.
  @param[in] input_ptr         Pointer to data to be encrypted (input).
  @param[in,out] output_ptr    Placeholder for the encryption output.

  @return
  Returns a Security Services error number to indicate execution outcome.
 
  @dependencies
  secssl_open() must have been called.

  @sa
  Appendix A.
*/
secerrno_enum_type secssl_encrypt_eap_data
(
  secssl_eap_encrypt_algorithm_enum_type  encrypt_algorithm,
  secssl_eap_encrypt_input_union_type*    input_ptr,
  secssl_eap_encrypt_output_type*         output_ptr
);

/*=============================================================================
  Function:    secsslca_parse_root_folder
=============================================================================*/
/**
  Parse all files under an EFS folder, read in all valid CA certificates into RAM, 
  these CA buffer will be used as CA candidates for SSL handshake.
  The maximum number of CA files per folder is SECX509_NUM_CERTS.
  The maximum number of CA folders SSL engine can hold is SECSSLCA_MAX_ROOT_FOLDER.
  This function is called within caller task's context.

  If a customer update some files under EFS root_folder, 
  for example, delete a cert file or download a new cert file to root_folder.
  Call this function to ask SSL engine re-parse the root_folder certificates.
  After a cert file is injected to root_folder, this function must be called before 
  secssl_open(), otherwise the update of certs won't be recognized by SSL engine.

  @param[in] root_folder a valid EFS folder
  example: "cert/", "oem/root_cert/"

  @return
  Returns a Security Services error number to indicate execution outcome.
 
  @dependencies
  None

  @sa
  Section @latexonly \ref{ErrorCodes} @endlatexonly for a description of error codes.  
*/
secerrno_enum_type secsslca_parse_root_folder
( 
  const char* root_folder 
);
/** @endcond
*/

#endif  /* SECSSL_H */
