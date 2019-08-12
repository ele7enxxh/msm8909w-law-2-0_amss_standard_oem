#ifndef _SECSSLDIAG_H
#define _SECSSLDIAG_H

/*===========================================================================

                  S E C U R I T Y   S E R V I C E S

            D I A G   I N T E R F A C E   D E F I N I T I O N S


This file defines the DIAG interface (events, logs) used by SSL/TLS.

Changes to these enums MUST be coordinated with the diag/tools team.
As of the creation date mail had to be sent to asw.diag.request.

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/COMMON/vcs/secssldiag.h_v   1.2   14 Oct 2003 17:12:16   rhummel  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/shared/src/secssldiag.h#1 $ $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/10   yh      SSL feature clean up.
11/12/04   rv      changes to correct LINT errors
09/29/03   rwh     Added comments about tool output.
09/10/03   rwh     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sec_variation.h"
#include "customer.h"
#include "comdef.h"
#include "event.h"
#include "log.h"
#include <stringl.h>

#ifdef FEATURE_SEC_SSL

#include "secssl.h"

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
The following indicates the nature of the SECSSL_EVENT.
--------------------------------------------------------------------------*/
typedef enum                          
{                                     // Display: 
  SECSSL_SOCK_READ_EVENT       =  1,  // "Socket Read Event:"
  SECSSL_SOCK_WRITE_EVENT      =  2,  // "Write to socket:"
  SECSSL_SOCK_CLOSE_EVENT      =  3,  // "Socket Close Event:"

  SECSSL_DNS_START_EVENT       =  4,  // "DNS Start:"
  SECSSL_DNS_SUCCESS_EVENT     =  5,  // "DNS Success:"
  SECSSL_DNS_FAILURE_EVENT     =  6,  // "DNS Failure:"

  SECSSL_CONNECT_START_EVENT   =  7,  // "Connect Start:"
  SECSSL_CONNECT_SUCCESS_EVENT =  8,  // "Connect Success:"
  SECSSL_CONNECT_FAILURE_EVENT =  9,  // "Connect Failure:"

  SECSSL_OPEN_START_EVENT      = 10,  // "Open Start:"
  SECSSL_OPEN_SUCCESS_EVENT    = 11,  // "Open Success:"
  SECSSL_OPEN_FAILURE_EVENT    = 12,  // "Open Failure:"

  SECSSL_READ_ERROR_EVENT      = 13,  // "Read Error:"
  SECSSL_WRITE_ERROR_EVENT     = 14,  // "Write Error:"

  SECSSL_ACCEPT_START_EVENT    = 15,  // "Accept Start:"
  SECSSL_ACCEPT_SUCCESS_EVENT  = 16,  // "Accept Success:"
  SECSSL_ACCEPT_FAILURE_EVENT  = 17,  // "Accept Failure:"
  SECSSL_RESUME_SUCCESS_EVENT  = 18,  // "Resume Success:"
  SECSSL_RESUME_FAILURE_EVENT  = 19,  // "Resume Failure:"
  SECSSL_CLOSE_EVENT           = 20,  // "Close session:"

  SECSSL_MAX_EVENT             = 0xffff /* 2-byte length */
} secssl_event_enum_type;


/*--------------------------------------------------------------------------
The payload for the SECSSL_EVENT indicates the nature of the event,
the status, the session identifier, the application data pointer, 
and one reserved value.
The display order should be:
"Open Success: 0xss, Session: 0xyyyy, App: 0xzzzz, Val1: 0xvvvv"
--------------------------------------------------------------------------*/
typedef struct
{                                           // Display:
   secssl_event_enum_type    specific_event;// (see above)
   uint16                    status;        // 0xss
   uint32                    session_id;    // "Session: " 0xyyyy
   uint32                    app_data_id;   // "App: " 0xzzzz
   uint32                    value1;        // "Val1: " 0xvvvv
}secssl_event_payload_type;


/*** Macros to make code more readable ***/

  #define SECSSL_DIAG_EVENT_ALLOC( payload ) 
  #define SECSSL_DIAG_EVENT_SET( payload_ptr, event, sess_ptr, stat )
  #define SECSSL_DIAG_EVENT_REPORT( payload_ptr )



/*--------------------------------------------------------------------------
The SECSSL_EVENT enum value (973) is displayed by QXDM when using a version
of the tool which predates the enum assignment.
The payload is displayed as a sequence of unsigned 8-bit hex values:
2 bytes of specific event;
2 bytes of status (zero so far);
4 bytes of session id;
4 bytes of app. data id;
4 bytes of zero.

The display order is network order, thus the values are reversed from the
ARM order, e.g. the ARM value for "Close Session" event is 0x0013, 
and is displayed as "13  00"
--------------------------------------------------------------------------*/



/*** LOG Codes  --  Range 169 - 187 assigned by Steve Harris ***/
#define LOG_SEC_SSL_C_BASE                  169

#define LOG_SEC_SSL_SESSION_C               (LOG_SEC_SSL_C_BASE + 0)
#define LOG_SEC_SSL_SESSION_SVR_CERT_C      (LOG_SEC_SSL_C_BASE + 1)
#define LOG_SEC_SSL_SESSION_CIPHERDATA_C    (LOG_SEC_SSL_C_BASE + 2)
#define LOG_SEC_SSL_SESSION_STATS_C         (LOG_SEC_SSL_C_BASE + 3)

#define LOG_SEC_SSL_TX_STATS_C              (LOG_SEC_SSL_C_BASE + 4)
#define LOG_SEC_SSL_RX_STATS_C              (LOG_SEC_SSL_C_BASE + 5)

#define LOG_SEC_SSLHSP_HELLOREQUEST_C       (LOG_SEC_SSL_C_BASE + 6)
#define LOG_SEC_SSLHSP_CLIENTHELLO_C        (LOG_SEC_SSL_C_BASE + 7)
#define LOG_SEC_SSLHSP_SERVERHELLO_C        (LOG_SEC_SSL_C_BASE + 8)
#define LOG_SEC_SSLHSP_SERVERKEYX_DH_C      (LOG_SEC_SSL_C_BASE + 9)
#define LOG_SEC_SSLHSP_SERVERKEYX_RSA_C     (LOG_SEC_SSL_C_BASE + 10)
#define LOG_SEC_SSLHSP_SERVERHELLODONE_C    (LOG_SEC_SSL_C_BASE + 11)
#define LOG_SEC_SSLHSP_CLIENTKEYX_DH_C      (LOG_SEC_SSL_C_BASE + 12)
#define LOG_SEC_SSLHSP_CLIENTKEYX_RSA_C     (LOG_SEC_SSL_C_BASE + 13)
#define LOG_SEC_SSLHSP_CLIENTFINISHED_C     (LOG_SEC_SSL_C_BASE + 14)
#define LOG_SEC_SSLHSP_SERVERFINISHED_C     (LOG_SEC_SSL_C_BASE + 15)
#define LOG_SEC_SSLCSP_CHANGECS_C           (LOG_SEC_SSL_C_BASE + 16)
#define LOG_SEC_SSLALP_ALERT_C              (LOG_SEC_SSL_C_BASE + 17)


/* These definitionse have to be in sync with the */
/* seclog_session_state_enum_type definition      */
/* in file secssli.h !!                           */
#define SECLOG_SESSION_UNUSED          0x00
#define SECLOG_SESSION_INITIALIZED     0x01
#define SECLOG_SESSION_PARAMS_READY    0x02
#define SECLOG_SESSION_HANDSHAKE       0x03
#define SECLOG_SESSION_CONNECTED       0x04
#define SECLOG_SESSION_ACCEPTING       0x05
#define SECLOG_SESSION_SERVING         0x06
#define SECLOG_SESSION_CLOSED          0x07
#define SECLOG_SESSION_RESUMED         0x08
#define SECLOG_SESSION_ALERT           0x09


/*** Macros to make code more readable ***/

  #define SECSSL_LOG_LOCAL( ptr, lcode )
  #define SECSSL_LOG_ALLOC( ptr, lcode )
  #define SECSSL_LOG_SET( lvalue_ptr, lvalue_offset, data )
  #define SECSSL_LOG_MEMCP( lvalue_ptr, lvalue_offset, data_ptr, len )
  #define SECSSL_LOG_MEMSET( lvalue_ptr, lvalue_offset, datum, len )   
  #define SECSSL_LOG_COMMIT( ptr )



/*** Maximum payload size per variable log record ***/
#define SECSSL_MAX_LOG_SIZE         900

/***************************************************************************/
/***                   Secure Sockets Layer Log Types                    ***/
/***************************************************************************/

/*** SSL Session Data and Statistics ***/
LOG_RECORD_DEFINE( LOG_SEC_SSL_SESSION_C )

  uint8          session_tag;             /* Identifier for a session      */
  char           server_name[SECSSL_SES_SERVER_MAX_HOSTNAME_LEN]; 
                                          /* ASCII based hostname/dot-addr */
  uint16         server_port;             /* Port number server listens on */
  uint16         version;                 /* SSL version negotiated        */
  uint8          session_id_len;          /* Session ID length             */
  uint8          session_id[32];          /* Session ID data               */
  uint16         ciphersuite;             /* SSL ciphersuite negotiated    */
  uint8          server_random[32];       /* Server's SSL-Random           */
  uint8          client_random[32];       /* Client's SSL-Random           */
  uint8          nagle_mode;              /* Nagle mode en- / disabled     */
  uint32         reneg_byte_trigger;      /* Renegot. trigger (# bytes)    */
  uint32         reneg_max_count;         /* Maximum # of renegotiations   */
  uint32         reneg_max_time;          /* Maximum time allowed for reneg*/
  uint32         reneg_last_time;         /* Time of last renegotiation    */
  uint8          start_time_h;            /* Time session was opened       */
  uint8          start_time_m;            /* Time session was opened       */
  uint8          start_time_s;            /* Time session was opened       */
  uint16         handshake_max_msg_size;  /* Largest handshake msg found   */
  uint16         handshake_tx_total_bytes;/* Number of bytes sent during HS*/
  uint16         handshake_rx_total_bytes;/* Number of bytes rcvd during HS*/
  uint16         handshake_duration_ms;   /* Duration of HS phase [ms]     */

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_SSL_SESSION_SVR_CERT_C )

  uint8          session_tag;             /* Identifier for a session      */
  uint8          frg_seqno;               /* Fragment # (starts at 0)      */
  uint16         frg_len;                 /* Length of this fragment       */
  uint8          cert_data[SECSSL_MAX_LOG_SIZE]; /* Fragment data          */

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_SSL_SESSION_CIPHERDATA_C )

  uint8          session_tag;             /* Identifier for a session      */
  uint8          premaster_secret_len;    /* Length of premaster secret    */
  uint8          premaster_secret[48];    /* Premaster secret              */
  uint8          master_secret[48];       /* Master secret (always 48 byte)*/
  uint8          client_mac_secret_len;   /* Clnt MAC write secret length  */
  uint8          client_mac_secret[20];   /* Clnt MAC write secret         */
  uint8          server_mac_secret_len;   /* Svr MAC write secret length   */
  uint8          server_mac_secret[20];   /* Svr MAC write secret          */
  uint8          client_cipher_key_len;   /* Client encryption key length  */
  uint8          client_cipher_key[16];   /* Client encryption key         */
  uint8          server_cipher_key_len;   /* Server encryption key length  */
  uint8          server_cipher_key[16];   /* Server encryption key         */
  uint8          client_cipher_iv_len;    /* Client cipher init vector len */
  uint8          client_cipher_iv[16];    /* Client cipher init vector     */
  uint8          server_cipher_iv_len;    /* Server cipher init vector len */
  uint8          server_cipher_iv[16];    /* Server cipher init vector     */

LOG_RECORD_END


LOG_RECORD_DEFINE( LOG_SEC_SSL_SESSION_STATS_C )

  uint8          session_tag;             /* Identifier for a session      */
  uint8          state;                   /* Session state (see above)     */
  uint32         uptime;                  /* Time since session open [sec] */
  uint32         tx_total_bytes;          /* Total application data tx'd   */
  uint32         rx_total_bytes;          /* Total application data rx'd   */
  uint16         num_resume;              /* Number of session resumptions */
  uint16         num_reneg;               /* Number of session renegotiat. */

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_SSL_TX_STATS_C )

  uint8          session_tag;             /* Identifier for a session      */
  uint16         num_bytes;               /* Number of bytes transmitted   */
  uint32         duration;                /* Duration ssl_write - dss_write*/

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_SSL_RX_STATS_C )

  uint8          session_tag;             /* Identifier for a session      */
  uint16         num_bytes;               /* Number of bytes received      */
  uint32         duration;                /* Duration dss_read - ssl_read  */

LOG_RECORD_END 


/*** SSL Handshake Protocol Messages ***/
LOG_RECORD_DEFINE( LOG_SEC_SSLHSP_HELLOREQUEST_C )

  uint8          session_tag;             /* Identifier for a session      */

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_SSLHSP_CLIENTHELLO_C )

  uint8          session_tag;             /* Identifier for a session      */
  uint16         version;                 /* SSL version                   */
  uint8          random[32];              /* Client random                 */
  uint8          session_id_len;          /* Session ID length (0-32)      */
  uint8          session_id[32];          /* Session ID                    */
  uint8          cipherlist_len;          /* # of entries in cipherlist    */
  uint16         cipherlist[10];          /* Available ciphersuites        */
  uint8          comprlist_len;           /* # of entries in compr. list   */
  uint8          comprlist[1];            /* Available compr. routines     */

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_SSLHSP_SERVERHELLO_C )

  uint8          session_tag;             /* Identifier for a session      */
  uint16         version;                 /* Selected protocl version      */
  uint8          random[32];              /* Server random                 */
  uint8          session_id_len;          /* Session ID length (0<l<=32)   */
  uint8          session_id[32];          /* Session ID                    */
  uint16         ciphersuite;             /* Selected ciphersuite          */
  uint8          compression;             /* Selected compr. routine       */

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_SSLHSP_SERVERKEYX_DH_C )

  uint8          session_tag;             /* Identifier for a session      */
  uint16         modulo_len;              /* DH modulo length              */
  uint8          modulo[128];             /* DH modulo                     */
  uint16         svr_pubkey_len;          /* DH server public key length   */
  uint8          svr_pubkey[128];         /* DH server public key          */
  uint16         base_len;                /* DH base length                */
  uint16         base[128];               /* DH Base                       */
  uint8          signature_len;           /* Message signature length      */
  uint8          signature[20];           /* Optional msg signature        */

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_SSLHSP_SERVERKEYX_RSA_C )

  uint8          session_tag;             /* Identifier for a session      */
  uint16         modulo_len;              /* RSA modulo length             */
  uint8          modulo[128];             /* RSA modulo                    */
  uint16         svr_pubkey_len;          /* RSA server public key length  */
  uint8          svr_pubkey[128];         /* RSA server public key         */
  uint8          signature_len;           /* Message signature length      */
  uint8          signature[20];           /* Optional msg signature        */

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_SSLHSP_SERVERHELLODONE_C )

  uint8          session_tag;             /* Identifier for a session      */

LOG_RECORD_END


LOG_RECORD_DEFINE( LOG_SEC_SSLHSP_CLIENTKEYX_DH_C )

  uint8          session_tag;             /* Identifier for a session      */
  uint16         clnt_pubkey_len;         /* DH client public key length   */
  uint8          clnt_pubkey[128];        /* DH client public key          */

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_SSLHSP_CLIENTKEYX_RSA_C )

  uint8          session_tag;             /* Identifier for a session      */
  uint8          encr_secret[48];         /* RSA encrypted premaster secret*/

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_SSLHSP_CLIENTFINISHED_C )

  uint8          session_tag;             /* Identifier for a session      */
  uint8          md5_hash[16];            /* Client's handshake MD5 hash   */
  uint8          sha_hash[20];            /* Client's handshake SHA hash   */
  uint8          verify_data[12];         /* Client's verify data          */

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_SSLHSP_SERVERFINISHED_C )

  uint8          session_tag;             /* Identifier for a session      */
  uint8          md5_hash[16];            /* Server's handshake MD5 hash   */
  uint8          sha_hash[20];            /* Server's handshake SHA hash   */
  uint8          verify_data[12];         /* Server's verify data          */

LOG_RECORD_END 


/*** SSL Cipherspec Protocol Messages */
LOG_RECORD_DEFINE( LOG_SEC_SSLCSP_CHANGECS_C )

  uint8          session_tag;             /* Identifier for a session      */

LOG_RECORD_END 


/*** SSL Alert Protocol Messages ***/
LOG_RECORD_DEFINE( LOG_SEC_SSLALP_ALERT_C )

  uint8          session_tag;             /* Identifier for a session      */
  uint8          level;                   /* Alert level                   */
  uint8          id;                      /* Alert ID (description)        */

LOG_RECORD_END 


#endif   /* FEATURE_SEC_SSL */

#endif /* _SECSSLDIAG_H */


