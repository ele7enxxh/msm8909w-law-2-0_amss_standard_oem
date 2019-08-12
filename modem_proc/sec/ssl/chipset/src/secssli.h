#ifndef SECSSLI_H
#define SECSSLI_H

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                S E C U R E   S O C K E T S   L A Y E R

    I N T E R N A L   D E F I N I T I O N S   &   D E C L A R A T I O N S

GENERAL DESCRIPTION
  The Secure Sockets Layr ...

EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  secssli_init() has to be called before any other function.

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/chipset/src/secssli.h#1 $
  $Date: 2016/12/13 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/15   nm      TLSv1.2 support
06/17/14   nm      TLS over IPv6
04/24/14   nm      Get subject Alt name to Cert data
11/09/10   dm      Crypto API files rearchitecture
10/20/10   yh      Old clock API replacement
07/14/10   yh      ixlock will take care critical section over all targets.
04/10/10   yh      3 New session parameters added
06/01/10   ml      Removed seccrypt dependencies and translated calls to secapi.
03/30/10   yh      SSL feature clean up
04/04/07   avm     changed hardcoded server_name[64] to 
                   server_name[SECSSL_SES_SERVER_MAX_HOSTNAME_LEN] 255:
           CR115824 SSL Server Address limit of 64 characters too low
03/08/07   df      enable 2048 bit keys - using SECAPI_PKX_MAX_KEY_SIZE
12/07/06   avm     Fixed Lint error type 665.
11/30/06   df      support for ds_socket2
11/01/06   avm     Renamed secssl_ses_certificate_pdata_type; 
                   to secx509_ses_certificate_pdata_type.
04/28/06   df      changes to support socket options for ssl session
04/14/06   df      Added command processing while secssl_wait
05/20/05   sv/ssm  Added support for mutual authentication and TLS over
                   eap transport.
11/12/04   rv      changes to correct LINT errors
10/30/03   jay     Added macros for DSM_PUSHDOWN
08/14/03   om      Added event_pending and eof_occurred flags to connection.
08/05/03   jay     Added mac_pkt_info fields to the session.
05/28/03   jay     Added global event for BrewAPI.
05/14/03   jay     Added a issl_ptr member to the SSL session for BREW 
05/05/03   jay     Added a last_error member to the SSL session for BREW 
                   support.
04/18/03   jay     Updated code to use current DNS API.
03/21/03   rwh     Removed a compile warning on an unsigned compare.
02/14/03   om      Added support for setting SSL version and cipher suite as
                   a parameter
02/10/03   rwh     Made branch to fix compilation problem with ADS 1.0.1 -
                   the forward typedef of secssl_session_type caused an error.
01/24/03   om      Added Root CA certs as session parameter.
01/14/03   om      Added brew_meta_data to session data.
11/20/02   rwh     Change scratchbuf to 32-bit alignment.
07/15/02   rwh     Moved some session parameter limit values to secssl.h.
06/18/02   om      Fixed comment for secssli_close_dssocket().
06/13/02   om      Changed secssli_close_dssocket() signature.
06/04/02   om      Added ABORT_SIG handling and TCP_CONNECT session state.
05/28/02   om      Added session_suspended_msg_stream_ptr to support multiple
                   messages per record across session suspend/continue.
05/07/02   om      Added prototype for set_socket_fd()
04/17/02   rwh     Modified VERIFY_HANDLE to use handle validation function.
                   Added prototypes for functions to report handle utilization,
                   and handle validation, updated return codes for delete and
                   crypto_free.
                   Removed unused function secssli_process_net_event_signal().
03/27/02   kvd     Modified secssli_close_dsnetwork to facilitate call back.
02/25/02   om      Added support for external sockets (BREW specific).
02/07/02   om      Added support for user action for security related events.
                   Added support for event-driven secure sockets.
01/30/02   om      Added READ and CLOSE event flags to connection type.
06/21/01   om      Fixed logging of session stats and info.
06/11/01   om      Removed obsolete interface function.
06/01/01   om      Added logging support.
05/31/01   om      Added DNS support.
05/24/01   om      Added SSL session resumption and multiple session.
04/06/01   yll     Externalized the secssli_proc_cmds().
03/02/01   om      Several fixes (first functional version)
10/23/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sec_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_SEC_SSL
#include "secdiag.h"
#include "dsm.h"
#include "timer.h"
#include "time_jul.h"
#include "rex.h"

#include "secerrno.h"
#include "secapi.h"
#include "secssldiag.h"

#include "secssldef.h"
#include "secsslconf.h"
#include "secssl.h"
#include "secx509.h"
#include "secssltask.h"
 
#include "ixlock.h"
extern strLockInfo  g_ssllock; /* Rex based critical section structure */
#define SSL_LOCK_INIT()       IxLockInit(&g_ssllock, LOCK_TASK_ONLY)
#define SSL_LOCK()            IxLock(&g_ssllock)
#define SSL_UNLOCK()          IxUnlock(&g_ssllock)  

/* <EJECT> */ 
/*===========================================================================

                 SESSION CONFIGURATION DEFINITIONS

===========================================================================*/

/* List of ciphers allowed for SSL :                                       */
/*  Key-exchge-prot.          enc-/decr. algo.   hash function      SSL Id */
/*                                                                         */
/* NOTE: The SSL Id is byte-swapped here since all session data is         */
/*       stored in network order !                                         */
/*       secssl.h export the maximum size of the list. If this list is     */
/*       increased, make sure to update SECSSL_SES_CS_LIST_MAX_SIZE.       */
/* see rfc4346 A.5. The CipherSuite                                        */
/* last 3 ciphersuite are deprecated from rfc 4346                         */
/*
  { SECSSL_KEY_DH_ANONYMOUS,  SECSSL_CRYPT_RC4,  SECSSL_HASH_MD5,   0x1800 }, \
  { SECSSL_KEY_DH_ANONYMOUS,  SECSSL_CRYPT_DES,  SECSSL_HASH_SHA,   0x1a00 }, \
  { SECSSL_KEY_DH_ANONYMOUS,  SECSSL_CRYPT_3DES, SECSSL_HASH_SHA,   0x1b00 }  
*/

#define SECSSL_CIPHER_LIST_SIZE   10
#define SECSSL_CIPHER_LIST \
{                                                                             \
  { SECSSL_KEY_RSA,           SECSSL_CRYPT_AES128, SECSSL_HASH_SHA256, 0x3c00 }, \
  { SECSSL_KEY_DH_EPHEMERAL,  SECSSL_CRYPT_AES128, SECSSL_HASH_SHA256, 0x6700 }, \
  { SECSSL_KEY_RSA,           SECSSL_CRYPT_AES256, SECSSL_HASH_SHA256, 0x3d00 }, \
  { SECSSL_KEY_DH_EPHEMERAL,  SECSSL_CRYPT_AES256, SECSSL_HASH_SHA256, 0x6B00 }, \
  { SECSSL_KEY_RSA,           SECSSL_CRYPT_AES128, SECSSL_HASH_SHA, 0x2f00 }, \
  { SECSSL_KEY_RSA,           SECSSL_CRYPT_3DES, SECSSL_HASH_SHA,   0x0a00 }, \
  { SECSSL_KEY_DH_EPHEMERAL,  SECSSL_CRYPT_AES128, SECSSL_HASH_SHA, 0x3300 }, \
  { SECSSL_KEY_DH_EPHEMERAL,  SECSSL_CRYPT_3DES, SECSSL_HASH_SHA,   0x1600 }, \
  { SECSSL_KEY_RSA,           SECSSL_CRYPT_AES256, SECSSL_HASH_SHA, 0x3500 }, \
  { SECSSL_KEY_DH_EPHEMERAL,  SECSSL_CRYPT_AES256, SECSSL_HASH_SHA, 0x3900 }, \
}
#define SECSSL_TLSV2_CIPHER_COUNT 4
#define SECSSL_SERVER_CIPHER_LIST_SIZE   2
#define SECSSL_SERVER_CIPHER_LIST \
{                                                                             \
  { SECSSL_KEY_RSA,           SECSSL_CRYPT_AES128, SECSSL_HASH_SHA, 0x2f00 }, \
  { SECSSL_KEY_RSA,           SECSSL_CRYPT_3DES, SECSSL_HASH_SHA,   0x0a00 }, \
}

/* Byte threshold and timeout [ms] for Nagle algorithm */
#define SECSSL_NAGLE_THRESHOLD        20
#define SECSSL_NAGLE_TIMEOUT         250

/* Maximum Record size for fragmentation */
#define SECSSL_MAX_RECORD_LENGTH          492

/* <EJECT> */ 
/*===========================================================================

                 SECURITY CONFIGURATION DEFINITIONS

===========================================================================*/

/* Maximum size allowed for public keys is 2048 bit */
#define SECSSL_PUBLIC_KEY_MAX_SIZE   SECAPI_PKX_MAX_KEY_SIZE

/* Minimum and maximum sizes for DH parameters in bytes (128 max) */
#define SECSSL_DH_MIN_BASE_LEN         1
#define SECSSL_DH_MAX_BASE_LEN       SECSSL_PUBLIC_KEY_MAX_SIZE
#define SECSSL_DH_MIN_MOD_LEN        128
#define SECSSL_DH_MAX_MOD_LEN        SECSSL_PUBLIC_KEY_MAX_SIZE
#define SECSSL_DH_MIN_EXP_LEN         16
#define SECSSL_DH_MAX_EXP_LEN        SECSSL_PUBLIC_KEY_MAX_SIZE
#define SECSSL_RSA_MIN_MOD_LEN       256
#define SECSSL_HSH_MAX_DIGEST_SIZE   MAX( SECAPI_HSH_SHA256_DIGEST_SIZE, \
                                           SECAPI_HSH_MD5_DIGEST_SIZE )
#define SECSSL_HMAC_SECRET_SIZE      64

/* <EJECT> */ 
/*===========================================================================

          OFF-TARGET SPECIFIC SESSION CONFIGURATION DEFINITIONS

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

          HANDSHAKE PROTOCOL DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/* Client state of the handshake phase */
typedef enum
{
  SECSSL_HSP_C_INIT_STATE = 0,
  SECSSL_HSP_HELLO_REQUEST_RCVD_STATE,
  SECSSL_HSP_SERVER_HELLO_WAIT_STATE,
  SECSSL_HSP_C_CERT_WAIT_STATE,
  SECSSL_HSP_C_CERT_SUSPENDED_STATE,
  SECSSL_HSP_SERVER_KEY_X_WAIT_STATE,
  SECSSL_HSP_CERTIFICATE_REQUEST_WAIT_STATE,
  SECSSL_HSP_SERVER_DONE_WAIT_STATE,
  SECSSL_HSP_C_CHANGE_CIPHERSPEC_WAIT_STATE,
  SECSSL_HSP_C_FINISHED_WAIT_STATE,
  SECSSL_HSP_C_NEG_COMPLETE_STATE,
  SECSSL_HSP_C_ABORTED_STATE
} secssl_handshake_c_state_enum_type;

/* Server state of the handshake phase */
typedef enum
{
  SECSSL_HSP_S_INIT_STATE = 0,
  SECSSL_HSP_CLIENT_HELLO_WAIT_STATE,
  SECSSL_HSP_S_CERT_WAIT_STATE,
  SECSSL_HSP_S_CERT_SUSPENDED_STATE,
  SECSSL_HSP_CLIENT_KEY_X_WAIT_STATE,
  SECSSL_HSP_CERTIFICATE_VERIFY_WAIT_STATE,
  SECSSL_HSP_S_CHANGE_CIPHERSPEC_WAIT_STATE,
  SECSSL_HSP_S_FINISHED_WAIT_STATE,
  SECSSL_HSP_S_NEG_COMPLETE_STATE,
  SECSSL_HSP_S_ABORTED_STATE
} secssl_handshake_s_state_enum_type;

/* State of the handshake key encryption */
typedef enum
{
  SECSSL_HSP_KEYGEN_INACTIVE = 0,

  /* Diffie-Hellman key generation states */
  SECSSL_HSP_KEYGEN_DH_PRIVATE_KEY_WAIT_STATE,
  SECSSL_HSP_KEYGEN_DH_PUBLIC_KEY_WAIT_STATE,
  SECSSL_HSP_KEYGEN_DH_SECRET_KEY_WAIT_STATE,
  SECSSL_HSP_KEYGEN_DH_FINISHED_STATE,

  /* RSA key encrypt states */
  SECSSL_HSP_KEYGEN_RSA_ENCRYPTED_KEY_WAIT_STATE,
  SECSSL_HSP_KEYGEN_RSA_FINISHED_STATE
} secssl_handshake_keygen_state_enum_type;

typedef struct
{
  uint8   mod_data[SECAPI_PKX_MAX_KEY_SIZE];
  uint16  mod_size;
  uint8   pub_key_data[SECAPI_PKX_MAX_KEY_SIZE];
  uint16  pub_key_size;
  uint8   private_key_data[SECAPI_PKX_MAX_KEY_SIZE];
  uint16  private_key_len;
  uint8   prime1[SECAPI_PKX_MAX_KEY_SIZE];
  uint16  prime1_size;
  uint8   prime2[SECAPI_PKX_MAX_KEY_SIZE];
  uint16  prime2_size;
  uint8   exponent1[SECAPI_PKX_MAX_KEY_SIZE];
  uint16  exponent1_size;
  uint8   exponent2[SECAPI_PKX_MAX_KEY_SIZE];
  uint16  exponent2_size;
  uint8   coefficient[SECAPI_PKX_MAX_KEY_SIZE];
  uint16  coefficient_size;
}secsslcert_private_key_info_type;

typedef struct
{
  uint8   mod_data[SECAPI_PKX_MAX_KEY_SIZE];
  uint16  mod_size;
}secsslcert_mod_params_type;


/* State type of the handshake protocol */
typedef struct
{
  /* Client state within the handshake phase */
  secssl_handshake_c_state_enum_type c_state;

  /* Server state within the handshake phase */
  secssl_handshake_s_state_enum_type s_state;
  
  /* State of the key encrypt - client side */
  secssl_handshake_keygen_state_enum_type keygen_state;

  /* hmac crypto instance flag */
  boolean hmac_handle_allocated;
  
  /* Hash handles needed to compute the running hashes over HSP messages */
  secapi_handle_type  client_md5_hash;
  secapi_handle_type  client_sha_hash;
  secapi_handle_type  server_md5_hash;
  secapi_handle_type  server_sha_hash;
  secapi_handle_type  cert_verify_md5_hash;
  secapi_handle_type  cert_verify_sha_hash;

  //TLSv1.2 Hash handles
  secapi_handle_type  client_sha256_hash;
  secapi_handle_type  server_sha256_hash;
  secapi_handle_type  cert_verify_sha256_hash;

  /* Protocol flags */
  boolean renegotiation_is_needed;
  boolean session_id_set_by_server;
  boolean server_key_x_required;
  boolean cert_requested_by_server;
  boolean cert_verify_required;
  boolean session_is_resumable;
  boolean short_handshake;

  /* Last message sent */
  ssl_v3_hsp_msg_id_enum_type last_msg;

  /* Session Context */
  secx509_cert_ctx_type ssl_x509_ctx;

  /* Callback for application when handshake is complete */
  secerrno_cb_fptr_type  app_cb_fptr;
  void*                  app_cb_usrdata_ptr;

  /* To save sizeof secsslhsp_state_type, hold one cert_chain and key_info.
     Add one more if AMSS SSL supports server/client in one session */

  /* buffer hold certificate chain */
  dsm_item_type*                    cert_chain;

  /* private key info container */
  secsslcert_private_key_info_type  cert_private_key_info;
} secsslhsp_state_type; 

/* <EJECT> */ 
/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/
/* Maximum value, 2~32-1, less than RFC4346 6.1 2~64-1*/
#define SECSSL_MAX_SEQ_NUM            0xFFFFFFFF

/* Maximum data sizes for ciphersuite session data (in byte) */
#define SECSSL_MAX_DIGEST_SIZE        32
#define SECSSL_MAX_CIPHER_KEY_SIZE    32
#define SECSSL_MAX_CIPHER_IV_SIZE     16
#define SECSSL_MAX_RSA_KEY_SIZE       SECSSL_PUBLIC_KEY_MAX_SIZE

/* Required session parameters */
#define SECSSL_SES_SERVER_MASK             0x00000001
#define SECSSL_SES_NUM_RENEG_MASK          0x00000002
#define SECSSL_SES_RENEG_TIMEOUT_MASK      0x00000004
#define SECSSL_SES_NAGLE_MODE_MASK         0x00000008
#define SECSSL_SES_ALERT_CB_MASK           0x00000010
#define SECSSL_SES_ERRNO_CB_MASK           0x00000020
#define SECSSL_SES_EVENT_CB_MASK           0x00000040
#define SECSSL_SES_SOCKET_MODE_MASK        0x00000080
#define SECSSL_SES_DS_APP_ID_MASK          0x00000100
#define SECSSL_SES_REQ_PARAM_MASK( s_ptr )                                 \
  ( ((s_ptr)->connection_end == SECSSL_CLIENT) ? SECSSL_SES_SERVER_MASK : 0          \
    | SECSSL_SES_ALERT_CB_MASK                                             \
    | SECSSL_SES_ERRNO_CB_MASK                                             \
    | ( ((s_ptr)->connection.socket_is_external) ? 0: SECSSL_SES_DS_APP_ID_MASK )\
    | ( ((s_ptr)->set_param_mask & SECSSL_SES_SOCKET_MODE_MASK) ?          \
        ( SECSSL_SES_SOCKET_MODE_MASK | SECSSL_SES_EVENT_CB_MASK ) : 0x00 ) )


/* ----------------------------------------------------------------------- */
/* CAPI Interface specific data needed per SSL session                     */
/* ----------------------------------------------------------------------- */

/* State enum to keep track of CAPI instances used */
typedef enum
{
  SECSSL_CAPI_INST_UNINITIALIZED_STATE = 0,
  SECSSL_CAPI_INST_INITIALIZED_STATE
} secssl_capi_inst_state_enum_type;

/* Data container of a ciphersuite */
typedef struct
{
  secssl_ses_key_prot_enum_type   key;   /**< Key exchange protocol. */
  secssl_ses_crypt_algo_enum_type crypt; /**< Encryption/decryption algorithm. */
  secssl_ses_hash_algo_enum_type  hash;  /**< Hash function. */
  uint16                          id;    /**< SSL ID for this ciphersuite. */
} secssl_ses_ciphersuite_type;

/* Crypto API state and handle info for instances used by secssl session */
typedef struct
{
  /* Ciphersuite information */
  secssl_ses_ciphersuite_type       ciphersuite;

  /* Key exchange protocol instance */
  secapi_handle_type                key_handle;
  secssl_capi_inst_state_enum_type  key_state;

  /* En- / decryption algorithm instance */
  secapi_handle_type                crypt_handle;
  secssl_capi_inst_state_enum_type  crypt_state;
  
  /* Message digest algorithm instance */
  secapi_handle_type                hash_handle;
  secssl_capi_inst_state_enum_type  hash_state;

} secssl_session_capi_type;

/* ----------------------------------------------------------------------- */
/* SSL Session State Structure & Pool                                      */
/* ----------------------------------------------------------------------- */

typedef struct secssl_session_s secssl_session_type;

/* Security Services API Session States                                    */
/* NOTE: Session structures are zeroed out upon return to the session      */
/*       pool. The SECSSL_SESSION_UNUSED state has to be = 0 to have       */
/*       the state of the session resemble it's pool usage.                */
typedef enum
{
  SECSSL_SESSION_UNUSED = 0,      /* Session is in session pool            */
  SECSSL_SESSION_INITIALIZED,     /* Session is allocated, def's are set   */
  SECSSL_SESSION_PARAMS_READY,    /* All requ'd session parameters are set */
  SECSSL_SESSION_TCP_CONNECT,     /* DNS lookup or TCP connect running     */
  SECSSL_SESSION_HANDSHAKE,       /* Session is doing handshake            */
  SECSSL_SESSION_CONNECTED,       /* Client session is open and negotiated */
  SECSSL_SESSION_ACCEPTING,       /* Server session is ready to accept     */
  SECSSL_SESSION_SERVING,         /* Server session is open and negotiated */
  SECSSL_SESSION_CLOSED,          /* Session is closed                     */
  SECSSL_SESSION_RESUMED,         /* Session is resumed                    */
  SECSSL_SESSION_ALERT,           /* Alert condition occured, alert sent   */
  SECSSL_SESSION_NUM_STATES
} secssl_session_state_enum_type;

/* Storage for the server's certificate data */
typedef struct
{
  uint8  version[SECSSLASN_MAX_REC_LEN];
  uint8  serial_no[SECSSLASN_MAX_REC_LEN];
  uint8  issuer[SECSSLASN_MAX_REC_LEN];
  uint8  not_before[SECSSLASN_MAX_REC_LEN];
  uint8  not_after[SECSSLASN_MAX_REC_LEN];
  uint8  object_organization[SECSSLASN_MAX_REC_LEN];
  uint8  object_common_name[SECSSLASN_MAX_REC_LEN];
  uint8*  subject_alt_name;
} secssl_svr_cert_data_type;

/* Session change macros to automate log records */
  #define SECSSL_SESSTATE_SET( session_ptr, state_val )  \
    session_ptr->state = state_val;

/* Session state info debug message */
extern  char* secssl_session_state_info_table[SECSSL_SESSION_NUM_STATES+1];
#define   SESSL_SESSION_STATE   secssl_session_state_info_table

extern char* secssl_client_handshake_state_table[SECSSL_HSP_C_ABORTED_STATE+1];
#define   SECSSL_C_HS_STATE     secssl_client_handshake_state_table

extern char* secssl_server_handshake_state_table[SECSSL_HSP_S_ABORTED_STATE+1];
#define   SECSSL_S_HS_STATE     secssl_server_handshake_state_table


/* State of the read data stream */
typedef enum
{
  SECSSL_RX_RECORD_HEADER = 0,
  SECSSL_RX_RECORD_PAYLOAD,
  SECSSL_RX_RECORD_ERROR
} secssl_rx_read_state_enum_type;

/* Negotiated and static session configuration parameters */
typedef struct
{
  /* ---------------------------------------------------------------- */
  /*                    Handshake Protocol State                      */
  /* ---------------------------------------------------------------- */
  secsslhsp_state_type           hsp_state;

  /* ---------------------------------------------------------------- */
  /*                      Negotiated parameters                       */
  /* ---------------------------------------------------------------- */

  /* SSL version requested and negotiated for this session */
  secssl_ssl_version_enum_type   ssl_hello_version;
  secssl_ssl_version_enum_type   ssl_version;

  /* Ciphersuite list advertised in the ClientHello msg */
  uint16                         session_cs_list[SECSSL_CIPHER_LIST_SIZE];
  uint16                         session_cs_list_size;

  /* SSL session ID this session */
  uint8                          session_id_len;
  uint8                          session_id[SSL_V3_SESSION_ID_MAX_LEN];

  /* Secure Server Certificate received during handshake */
  secx509_ses_certificate_pdata_type  svr_cert;
  secssl_svr_cert_data_type           svr_cert_data;
  boolean                             svr_cert_valid;

  /* Optional non-EFS root certificates */
  secssl_ses_ca_certificate_pdata_type  root_ca[SECSSL_SES_MAX_ROOT_CERTS];
  uint16                                root_ca_count;

  /* EFS path which contains root certificates */
  char                           session_ca_cert_folder[SECSSL_SES_CERT_FOLDER_MAX_LEN];

  /* State information and handles to the crypto instances used       */
  /* Active denotes the used cyphersuite, and pending the negotiated  */
  /* but not yet activated one.                                       */
  secssl_session_capi_type       client_active_cipher_suite;
  secssl_session_capi_type       client_pending_cipher_suite;
  secssl_session_capi_type       server_active_cipher_suite;
  secssl_session_capi_type       server_pending_cipher_suite;
  uint16                         last_negotiated_cipher_suite;
  secapi_handle_type             rsa_sig_handle;

  /* Stuff needed by the ciphersuite, e.g, the master secret,         */
  /* mac secret, etc. This data is derived from the initial handshake */
  /* and uses time/day and sequence numbers as variable elements.     */
  /* See secsslhsp_create_master_secret() and secsslhsp_create_keys() */
  uint8    server_random[SSL_V3_SERVER_RANDOM_SIZE];
  uint8    client_random[SSL_V3_CLIENT_RANDOM_SIZE];

  uint8    server_public_key[SECSSL_PUBLIC_KEY_MAX_SIZE];
  uint8    client_public_key[SECSSL_PUBLIC_KEY_MAX_SIZE];
  uint16   server_public_key_len;
  uint16   client_public_key_len;

  uint8    premaster_secret[SECSSL_PUBLIC_KEY_MAX_SIZE];
  uint16   premaster_secret_size;

  uint8    master_secret[SSL_V3_MASTER_SECRET_SIZE];
  boolean  master_secret_is_valid;

  uint8    client_mac_secret[SECSSL_MAX_DIGEST_SIZE];
  uint8    server_mac_secret[SECSSL_MAX_DIGEST_SIZE];
  uint16   client_mac_secret_size;
  uint16   server_mac_secret_size;

  uint8    client_cipher_key[SECSSL_MAX_CIPHER_KEY_SIZE];
  uint8    server_cipher_key[SECSSL_MAX_CIPHER_KEY_SIZE];
  uint16   client_cipher_key_size;
  uint16   server_cipher_key_size;

  uint8    client_cipher_iv[SECSSL_MAX_CIPHER_IV_SIZE];
  uint8    server_cipher_iv[SECSSL_MAX_CIPHER_IV_SIZE];
  uint16   client_cipher_iv_size;
  uint16   server_cipher_iv_size;

  boolean  key_material_is_valid;

  uint8    client_handshake_md5_hash[ SECAPI_HSH_MD5_DIGEST_SIZE];
  uint8    client_handshake_sha_hash[ SECAPI_HSH_SHA_DIGEST_SIZE];
  uint8    client_handshake_verify_data[ TLS_V1_VERIFY_DATA_SIZE];
  uint8    server_handshake_md5_hash[ SECAPI_HSH_MD5_DIGEST_SIZE];
  uint8    server_handshake_sha_hash[ SECAPI_HSH_SHA_DIGEST_SIZE];
  uint8    server_handshake_verify_data[ TLS_V1_VERIFY_DATA_SIZE];

  uint8    client_handshake_sha256_hash[ SECAPI_HSH_SHA256_DIGEST_SIZE];
  uint8    server_handshake_sha256_hash[ SECAPI_HSH_SHA256_DIGEST_SIZE];

  secapi_mac_pkt_info_type current_rx_mac_pkt_info;
  secapi_mac_pkt_info_type current_tx_mac_pkt_info;

  uint16   min_dh_key_size; /* min. acceptable DH key size (bytes) */
  uint16   min_rsa_key_size;/* min. acceptable RSA key size (bytes) */


  /* ---------------------------------------------------------------- */
  /*                       Static parameters                          */
  /* ---------------------------------------------------------------- */

  /* Nagle mode en- or disabled and nagle timer */
  boolean      nagle_mode;
  timer_type   nagle_cb_struct;

  /* Check if security related alerts should be handled by the app */
  secssl_ses_alert_mode_enum_type  alert_mode;

  /* Use events for sockets instead of BSD-like behavior (default) */
  boolean  socket_events;

  /* Session renegotiation */
  uint32   reneg_byte_trigger;        /* Renegotiate every so many bytes   */
  uint32   reneg_max_count;           /* Maximum number of renegotiations  */
  uint32   reneg_max_time;            /* Maximum time allowance for reneg. */
  uint32   reneg_last_time;           /* Time of last renegotiation        */
  
  /* Logging of SSL messages and records */
  boolean  log_ssl_msgs;
  boolean  log_ssl_records;

  /* Where to read in client/server cert and key */
  secssl_ses_cert_conf_data_type   clnt_cert_conf;
  secssl_ses_cert_conf_data_type   svr_cert_conf;

} secssl_session_config_type;


/* Connection-specific data */
typedef struct
{
  sint15   ds_app_id;
  sint15   server_socket;  /* client session owns server_socket, a socket connects to server */
  sint15   client_socket;  /* server session owns client_socket, a socket connects to client */
  secssl_ses_ds_sockopt_type sockopt;
  boolean  socket_is_open;
  boolean  socket_has_data;
  boolean  socket_is_external;
  boolean  event_pending;
  boolean  eof_occured; 
  boolean  register_DS_event; 
  /* function pointers for tx, rx & select*/
  secssl_rx_fptr_type         rx_fptr;
  secssl_tx_fptr_type         tx_fptr;
  secssl_select_fptr_type     select_fptr;
  secssl_deselect_fptr_type   deselect_fptr;
  sint31 next_event;  //socket2 event
  secssl_next_event_fptr_type next_event_fptr;
} secssl_connection_type;


/* Session statictics */
typedef struct
{
  /* Statistics */
  time_julian_type  start_time;       /* Time session was opened           */
  uint32   start_time_sec;            /* System time [sec] when ses opened */
  uint16   handshake_duration_ms;     /* Duration of HS phase [ms]         */
  uint16   handshake_max_msg_size;    /* Largest handshake msg found       */
  uint16   handshake_tx_total_bytes;  /* Number of bytes sent during HS    */
  uint16   handshake_rx_total_bytes;  /* Number of bytes rcvd during HS    */
  uint32   total_tx_byte_count;       /* Total bytes transmitted           */
  uint32   total_rx_byte_count;       /* Total bytes read by application   */
  uint16   total_resume_count;        /* Total number of resumptions       */
  uint16   total_reneg_count;         /* Total number of renegotiations    */    
  uint32   total_bytes_decrypted;     /* Total decrypted byte after handshake completed */
  
} secssl_session_stats_type;

/* Global data needed for BrewApi */
typedef void (*secssli_sock_event_fptr_type)(void);
typedef struct
{
  secssli_sock_event_fptr_type socket_event_handler;
}secssli_brew_data_type;

/* Metadata needed by BrewAPI */
typedef struct
{
  void*               inet_mgr_ptr;
  void*               webopt_data_ptr;
  void*               session_record;
  void*               issl_ptr;
  void                (*net_activity_report_fptr)( secssl_session_type* );
  uint16              session_hash_length;
  uint8               session_hash[SECSSL_MAX_DIGEST_SIZE];
  uint32              trust_mode;
  secerrno_enum_type  last_error;
} brew_meta_data_type;

/* Session State (internal) Structure */
struct secssl_session_s
{
  /* Session role, client or server */
  secssl_ses_connection_end_enum_type      connection_end;

  /* SSL State and hold state for security overrides */
  secssl_session_state_enum_type state;
  boolean                        session_suspended;
  dsm_item_type*                 session_suspended_msg_stream_ptr;

  /* SSL Server Info (empty if we are the server) */
  char                server_name[SECSSL_SES_SERVER_MAX_HOSTNAME_LEN]; /* Character based hostname */
  
  ip_addr_enum_type server_addr_type;   /**< Address Type.*/
#if( !defined(PLATFORM_LTK) && (DSS_VERSION >= 2100))
  struct ps_sockaddr_in  server_addr;     /* in_addr type hostname/addr */
  struct ps_sockaddr_in6 server_addr6;    /* in_addr type hostname/addr in v6 */

#else
  struct sockaddr_in     server_addr;     /* in_addr type hostname/addr */
  struct sockaddr_in6    server_addr6;    /* in_addr type hostname/addr  in v6*/

#endif 

  uint16              server_port;            /* Port number server listens on  */
  boolean             server_lookup_required; /* Keep track of DNS usage */

  /* Tag for logging */
  uint8               tag;

  /* Session configuration */
  secssl_session_config_type     configuration;

  /* Connection used by session (1-1 mapping only) */
  secssl_connection_type         connection;

  /* SSL buffers for transmission */
  dsm_item_type*  cur_tx_alt_rec_ptr; /* currently processed alert record  */
  dsm_item_type*  cur_tx_sig_rec_ptr; /* currently processed signaling rec.*/
  dsm_item_type*  cur_tx_data_rec_ptr;/* currently processed app-data rec. */
  dsm_item_type*  cur_tx_cipher_ptr;  /* currently encrypted item for TX   */

  /* SSL buffer for receiving */
  dsm_item_type*  cur_rx_plain_ptr;   /* currently processed item for RX   */
  dsm_item_type*  cur_rx_cipher_ptr;  /* currently encrypted item for RX   */

  /* SSL current state of read data stream (header or payload) */
  secssl_rx_read_state_enum_type cur_rx_state;
  boolean                        cur_rx_is_fragmented;

  /* SSL errno tracking across calls */
  secerrno_enum_type  last_write_err; /* Last error in write attempt       */
  secerrno_enum_type  last_read_err;  /* Last error in read attempt        */

  /* Callbacks for alerts and asynchronous execution */
  secssl_ses_alert_cb_fptr_type    alert_cb_fptr;
  secerrno_cb_fptr_type            execution_cb_fptr;
  secssl_ses_event_cb_fptr_type    event_cb_fptr;
  void*                            user_data_ptr;

  /* Mask to keep track of security related failures which can */
  /* be overridden by the application.                         */
  uint32 security_alert_mask;

  /* Mask to keep track of parameters set */
  uint32 set_param_mask;

  /* Keep track of what's going on ;-) */
  secssl_session_stats_type      stats;

  /* Metadata needed by BrewAPI */
  brew_meta_data_type  brew_data;

  /* 
   * RFC4366 TLS extensions 
   */
  /* Server Name List */
  secssl_ses_server_name_list_pdata_type* server_name_list_ptr;

  boolean   is_sync_write;

  /* Use secure time or user time for cert expiration check */
  secssl_sys_time_enum_type   sys_time;

  /* skip cert expiration check, only for test envrionment */
  boolean   ignore_expiration;

} /*secssl_session_type*/;

/* Static pool of session state structures */
extern secssl_session_type
secssli_session_pool[SECSSLCONF_MAX_CONC_SESSIONS];

/* SSL related command parameter type. */
typedef struct
{
  secssl_handle_type     session_handle;
  secerrno_cb_fptr_type  open_cb_fptr;
  void*                  open_cb_udata_ptr;
  
} secssl_open_session_param_type;

typedef struct
{
  secssl_handle_type     session_handle;
  secerrno_cb_fptr_type  close_cb_fptr;
  void*                  close_cb_udata_ptr;
 
} secssl_close_session_param_type;

typedef struct
{
  secssl_handle_type   session_handle;
  secerrno_cb_fptr_type   resume_cb_fptr;
  void*                   resume_cb_udata_ptr;

} secssl_resume_session_param_type;

typedef struct
{
  secssl_handle_type     session_handle;
  uint8*                 data_ptr;
  uint16*                data_len_ptr;

} secssl_read_param_type;

typedef struct
{
  secssl_handle_type     session_handle;
  uint8*                 data_ptr;
  uint16*                data_len_ptr;

} secssl_write_param_type;

/* DNS Callback parameters */
typedef struct
{
  struct dss_hostent*  hostent_ptr;
  int16                ds_errno;
} secssl_dns_cb_data_type;

/* Current session using DNS (only one allowed) */
typedef struct
{
  secssl_session_type*      session_ptr;
  secerrno_cb_fptr_type     open_cb_fptr;
  void*                     open_cb_udata_ptr;
  secssl_dns_cb_data_type   dns_cb_data;
} secssl_current_dns_user_type;

/* dss_socket2, socket event callback data. */
typedef struct
{
  q_link_type           link;         /* Queue link */
  sint15                nethandle; 
  sint15                sockfd;
  uint32                event_mask;
  secssl_session_type*  session_ptr;
} secssl_socket_event_type;

extern q_type secssl_socket_event_q;  

#define SECSSL_VERIFY_HANDLE( handle )  \
  if ( secssli_is_valid_session_handle_addr(                          \
         (secssl_session_type*) handle ) == FALSE )                   \
  {                                                                   \
    SEC_DIAG_MSG1( SECDIAG_SSL, MSG_SEC_ERROR,                        \
    "Invalid SecSSL Handle 0x%x", handle );                           \
    return E_BAD_ADDRESS;                                             \
  }

/* Helper macros to handle dsm items */
#define SECSSL_DSM_PULLUP( msg_ptr, dst_ptr, len ) \
  if ( dsm_pullup( msg_ptr,                                           \
                   (uint8*) dst_ptr,                                  \
                   len )                                              \
       != len )                                                       \
  {                                                                   \
    SEC_DIAG_MSG( SECDIAG_SSL, MSG_SEC_FATAL,"SSL: dsm_pullup failed");   \
  }

#define SECSSL_DSM_PUSHDOWN_TAIL( msg_ptr, src_ptr, len ) \
  if ( dsm_pushdown_tail( &msg_ptr,                                   \
                          (uint8*) src_ptr,                           \
                          len,                                        \
                          DSM_DS_SMALL_ITEM_POOL )                    \
       != len )                                                       \
  {                                                                   \
    SEC_DIAG_MSG( SECDIAG_SSL, MSG_SEC_FATAL,"SSL: dsm_pushdown_tail failed");   \
  }
  
#define SECSSL_DSM_PUSHDOWN( msg_ptr, src_ptr, len ) \
  if ( dsm_pushdown( &msg_ptr,                                   \
                     (uint8*) src_ptr,                           \
                     len,                                        \
                     DSM_DS_SMALL_ITEM_POOL )                    \
       != len )                                                  \
  {                                                              \
    SEC_DIAG_MSG( SECDIAG_SSL, MSG_SEC_FATAL,"SSL: dsm_pushdown failed");   \
  }


/* Structure holding our supported ciphersuites */
extern secssl_ses_ciphersuite_type secssl_cipher_list[SECSSL_CIPHER_LIST_SIZE];

/* Structure used to copy null cipher'd data */
//extern uint8 scratchbuf[2048];
/* Try 32-bit alignment. */
//extern uint32 scratchbuf[2048/4];  // defined at seccrypti.h line 499 

/* <EJECT> */ 
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECSSLI_INIT

DESCRIPTION
  This function initializes all static data.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void secssli_init( void );


/*===========================================================================

FUNCTION SECSSLI_SESPOOL_NEW

DESCRIPTION
  This function fetches a session structure from the preallocated pool.

DEPENDENCIES
  secssli_init() was called before.

PARAMETERS
  None

RETURN VALUE
  Pointer to new SSL session struct or NULL if none are available.

SIDE EFFECTS
  None
===========================================================================*/
secssl_session_type* secssli_sespool_new( void );


/*===========================================================================

FUNCTION SECSSLI_REPORT_SESSION_HANDLE_USE

DESCRIPTION
  Report the utilization of the session handles.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Interrupts are locked while reading the handle array.
===========================================================================*/
uint16 secssli_report_session_handle_use( void );


/*===========================================================================

FUNCTION SECSSLI_IS_VALID_SESSION_HANDLE_ADDR

DESCRIPTION
  Check if the pointer value is an address of one of the session structures
  in the preallocated pool.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session structure

RETURN VALUE
  TRUE - the value matches an address in the pool;
  FALSE - otherwise
  
SIDE EFFECTS
  None
===========================================================================*/
boolean secssli_is_valid_session_handle_addr
(
  secssl_session_type* session_ptr
);

/*===========================================================================

FUNCTION SECSSLI_SESPOOL_DELETE

DESCRIPTION
  This function returns a session structure to the preallocated pool.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session structure

RETURN VALUE
  E_BAD_ADDRESS - pointer value does not match an address from session pool;
  E_SUCCESS - otherwise.
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_sespool_delete
(
  secssl_session_type* session_ptr
);


/*===========================================================================

FUNCTION SECSSLI_SET_SESDEFAULTS

DESCRIPTION
  This function initializes a session structure to the defaults.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session structure

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void secssli_set_sesdefaults
(
  secssl_session_type* session_ptr
);

/* <EJECT> */ 
/*==========================================================================
 FUNCTION SECSSLI_PROC_CMDS

 DESCRIPTION 
   This function processes any commands which are in the SSL command queue.
   It first checks if any commands are on the queue, and processes them 
   based on the command type. After the processing, it put the command back
   to the command free queue. The loop continues until the SSL command
   queue is empty.
   
 DEPENDENCIES 
   secssl_cmd_q must have been intialized using q_init.

 RETURN VALUE 
   None
  
 SIDE EFFECTS
   None                          
===========================================================================*/
extern void secssli_proc_cmds
(
  void
);

/*==========================================================================
 FUNCTION SECSSLI_PROC_CMDS_WHILE_BUSY_WAITING

 DESCRIPTION 
   This is a temp function will be looked at more closely
      
 DEPENDENCIES 
   secssl_cmd_q must have been intialized using q_init.

 RETURN VALUE 
   None
  
 SIDE EFFECTS
   None                          
===========================================================================*/
extern void secssli_proc_cmds_while_busy_waiting
(
  void
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION SECSSLI_OPEN_SESSION

DESCRIPTION
  This function opens an SSL session for an existing session instance.

DEPENDENCIES
  session_handle is a valid session instance created before.

PARAMETERS
  session_handle    - Valid handle for a session instance 
  open_cb_fptr      - Function pointer to be called when session is open
  open_cb_udata_ptr - User data for callback function

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void secssli_open_session
(
  secssl_handle_type     session_handle,
  secerrno_cb_fptr_type  open_cb_fptr,
  void*                  open_cb_udata_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION SECSSLI_ACCEPT_SESSION

DESCRIPTION
  This function start SSL handshake from a server session.

DEPENDENCIES
  session_handle is a valid session instance created before.

PARAMETERS
  session_handle      - Valid handle for a session instance 
  accept_cb_fptr      - Function pointer to be called when handshake is finished
  accept_cb_udata_ptr - User data for callback function

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void secssli_accept_session
(
  secssl_handle_type     session_handle,
  secerrno_cb_fptr_type  accept_cb_fptr,
  void*                  accept_cb_udata_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION SECSSLI_WRITE

DESCRIPTION
  This function processes the write command for a session.

DEPENDENCIES
  session_handle is a valid session instance created before.

PARAMETERS
  session_handle  - Valid handle for a session instance 
  data_ptr        - Pointer to application data
  data_len        - Length of application data in bytes
  mode            - Sync vs Async write mode

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

secerrno_enum_type secssli_write
(
  secssl_handle_type    session_handle,
  uint8*                data_ptr,
  uint16*               data_len,
  secssl_cmd_mode_type  mode
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION SECSSLI_READ

DESCRIPTION
  This function processes the read command for a session.

DEPENDENCIES
  session_handle is a valid session instance created before.

PARAMETERS
  session_handle  - Valid handle for a session instance 
  data_ptr        - Pointer to application data
  data_len        - Maximum length of applcation data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

secerrno_enum_type secssli_read
(
  secssl_handle_type  session_handle,
  uint8*        data_ptr,
  uint16*             data_len_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION SECSSLI_CLOSE_SESSION

DESCRIPTION
  This function processes the close command for a session.

DEPENDENCIES
  session_handle is a valid session instance created before.

PARAMETERS
  session_handle     - Valid handle for a session instance 
  close_cb_fptr      - Callback function pointer upon completion
  close_cb_udata_ptr - User data for above callback

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void secssli_close_session  
(  
  secssl_handle_type     session_handle,
  secerrno_cb_fptr_type   close_cb_fptr,
  void*                   close_cb_udata_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION SECSSLI_RESUME_SESSION

DESCRIPTION
  This function processes the resume command for a session.

DEPENDENCIES
  session_handle is a valid session instance created before.

PARAMETERS
  session_handle      - Valid handle for a session instance 
  resume_cb_fptr      - Callback function pointer upon completion
  resume_cb_udata_ptr - User data for above callback

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void secssli_resume_session 
(  
  secssl_handle_type     session_handle,
  secerrno_cb_fptr_type   resume_cb_fptr,
  void*                   resume_cb_udata_ptr
);  

/* <EJECT> */ 
/*===========================================================================

FUNCTION SECSSLI_CONTINUE_SESSION

DESCRIPTION
  This function processes the continue command for a session.

DEPENDENCIES
  session_handle is a valid session instance created before.

PARAMETERS
  session_handle        - Valid handle for a session instance 
  continue_cb_fptr      - Callback function pointer upon completion
  continue_cb_udata_ptr - User data for above callback

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void secssli_continue_session  
(  
  secssl_handle_type     session_handle,
  secerrno_cb_fptr_type   continue_cb_fptr,
  void*                   continue_cb_udata_ptr
);  


/*===========================================================================

FUNCTION SECSSLI_SET_CIPHER_SUITE

DESCRIPTION
  This function allocates the CAPI instances needed for a particular
  ciphersuite and initializes any SSL sepcific CAPI-data, e.g callbacks,
  keysizes, etc.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session structure
  suite       - SSLv3-specific value for the suite in network order
  activate    - Allocate and activate suite immediately when TRUE,
                otherwise just allocate the suite.
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_cipher_suite
(
  secssl_session_type* session_ptr,
  uint16               suite,
  boolean              activate
);

/*===========================================================================

FUNCTION SECSSLI_SET_CIPHER_KEYS

DESCRIPTION
  This function sets the cipher keys and MAC data for the pending
  client and server ciphersuite.

  NOTE: This function is supposed to be called _before_ the ciphersuites
        are activated, but obviously _after_ the key material generation.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session structure
  suite       - SSLv3-specific value for the suite in network order

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_cipher_keys
(
  secssl_session_type* session_ptr
);

/*==========================================================================
FUNCTION SECSSLI_DSS_NET_CB
 
DESCRIPTION 
  This is the callback called by the sockets code when the SSL application
  makes a network call.  This function is registered with sockets by the SSL
  application via the dss_open_netlib() call.
  
  This function merely sets a signal.  It should do as little work as
  possible, since this function is called from the psmgr task context.
  
DEPENDENCIES 
  None

PARAMETERS
  arg1  - Dummy parameter

RETURN VALUE 
  None
 
SIDE EFFECTS
  None
===========================================================================*/
extern void secssli_dss_net_cb
(
  void* arg1
);


/*==========================================================================
FUNCTION SECSSLI_DSS_SOCKET_CB
 
DESCRIPTION 
  This is the callback called by the sockets code when the SSL application
  makes a socket call.  This function is registered with sockets by the SSL
  application via the dss_open_netlib() call.
  
  This function merely sets a signal.  It should do as little work as
  possible, since this function is called from the psmgr task context.
  
DEPENDENCIES 
  None

PARAMETERS
  arg1  - Dummy parameter

RETURN VALUE 
  None
 
SIDE EFFECTS
  None
===========================================================================*/
extern void secssli_dss_socket_cb
(
  void* arg1
);

/*==========================================================================

FUNCTION SECSSLI_DSS_SOCKET2_CB
 
DESCRIPTION 
  This is the callback called by the sockets2 code when the SSL application
  makes a socket call.  This function is registered with sockets by the SSL
  application via the dss_socket2 call.
  
  This function merely sets a signal.  It should do as little work as
  possible, since this function is called from the psmgr task context.
  
DEPENDENCIES 
  None

PARAMETERS
  sint15 appid,                            Application id
  sint15 sockfd,                           socket descriptor
  uint32 event_mask,                       Event occurred
  secssl_handle_type session_handle,       active ssl session handle

RETURN VALUE 
  None
 
SIDE EFFECTS
  None
===========================================================================*/
void secssli_dss_socket2_cb( 
  sint15 appid,                            /* Application id */
  sint15 sockfd,                           /* socket descriptor */
  uint32 event_mask,                       /* Event occurred */
  secssl_handle_type session_handle        /* active ssl session handle */
);

/*===========================================================================

FUNCTION SECSSLI_PROC_EVENT_SIG

DESCRIPTION
  This function processes the pending READ event signal.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void secssli_proc_event_sig( void );

/*===========================================================================

FUNCTION SECSSLI_PROC_SOCKET_CB_SIG

DESCRIPTION
  This function processes the SOCKET Callback signal.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void secssli_proc_socket_cb_sig( void );

/*===========================================================================

FUNCTION SECSSLI_PROC_NETWORK_CB_SIG

DESCRIPTION
  This function process the Network Callback signal.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void secssli_proc_network_cb_sig( void );

/*===========================================================================

FUNCTION SECSSLI_INET_ADDR

DESCRIPTION
  This function converts an Internet Address in dot notation
  into a word32 value in network order.

DEPENDENCIES
  None
  
PARAMETERS
  dot_addr_ptr - Pointer to string containing IP address

RETURN VALUE
  IP address in network order
  
SIDE EFFECTS
  None
===========================================================================*/

extern uint32 secssli_inet_addr
(
  const char* dot_addr_ptr
);

/*==========================================================================
FUNCTION SECSSLI_DNS_LOOKUP_PROC_SIG
 
DESCRIPTION 
  This function processes the DNS lookup done signal by getting
  the server's IP address and restarting the session_open process.
  
DEPENDENCIES 
  None

PARAMETERS
  None

RETURN VALUE 
  None
 
SIDE EFFECTS
  None
===========================================================================*/
extern void secssli_dns_lookup_proc_sig( void );

/*===========================================================================

FUNCTION SECSSLI_OPEN_NETWORK

DESCRIPTION
  This function is responsible for opening the network subsystem.

DEPENDENCIES
  None
  
PARAMETERS
  session_handle    - Valid handle for a session instance 
  open_cb_fptr      - Function pointer to be called when session is open
  open_cb_udata_ptr - User data for callback function

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_open_network
( 
  secssl_handle_type     session_handle,
  secerrno_cb_fptr_type  open_cb_fptr,
  void*                  open_cb_udata_ptr
);

/*===========================================================================

FUNCTION SECSSL_CLOSE_DSNETWORK

DESCRIPTION
  This function closes the PPP stack for this application.

DEPENDENCIES
  None

PARAMETERS
  ds_app_id - This applications Data Services ID
  secerrno_cb_fptr_type  calback  function ptr
  void*                  callback handle

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

  Possible error conditions:
  E_INVALID_ARG   - The session handle or parameter are not valid
  E_FAILURE       - Unknown error occurred during execution
  E_SUCCESS       - successfully closed the network


SIDE EFFECTS
  The current PPP session and the network library are closed if
  no other sockets application is currently open.
===========================================================================*/


void secssli_close_dsnetwork
(
  sint15 ds_app_id,
  secerrno_cb_fptr_type   close_cb_fptr,
  void*                   close_cb_udata_ptr
);

/*===========================================================================

FUNCTION SECSSLI_dns_lookup

DESCRIPTION
  This function look up dns address for an internal socket.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session structure

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
void secssli_dns_lookup
(
  secssl_session_type* session_ptr
);
/*===========================================================================

FUNCTION SECSSLI_SESSION_CONNECT

DESCRIPTION
  This function opens a TCP socket and attempts to connect to the server.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session structure

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_session_connect
(
  secssl_session_type* session_ptr
);

/*===========================================================================
FUNCTION SECSSLI_CONNECT

DESCRIPTION
  This function performs a blocking TCP connect() based on the non-blocking
  dss_connect function from the DSSock sockets API.

DEPENDENCIES
  None

PARAMETERS
  app_id  - application ID
  sockfd  - Socket file descriptor
  server  - Server's address as type sockaddr
  srv_len - length of server address

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_connect
(
  sint15 app_id,
  sint15 sockfd,
  struct sockaddr *server,
  int srv_len
);


/*===========================================================================
FUNCTION SECSSLI_CONNECT_V6

DESCRIPTION
  This function performs a blocking TCP connect() based on the non-blocking
  dss_connect function from the DSSock sockets API.

DEPENDENCIES
  None

PARAMETERS
  app_id  - application ID
  sockfd  - Socket file descriptor
  server_v6  - Server's IPv6 address as type sockaddr
  srv_len - length of server address

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_connect_v6
(
  sint15 app_id,
  sint15 sockfd,
  struct sockaddr_in6 *server_v6,
  int srv_len
);

/*===========================================================================

FUNCTION SECSSLI_CLOSE_DSSOCKET

DESCRIPTION
  This function

DEPENDENCIES
  None
  
PARAMETERS
  app_id  - application ID
  sockfd  - Socket file descriptor

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void secssli_close_dssocket
(
  sint15            app_id,
  sint15            sockfd,
  secssl_session_type* session_ptr
);

/*===========================================================================

FUNCTION SECSSLI_SESSION_CRYPTO_NEW

DESCRIPTION
  This function allocate crypto instances.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session structure

RETURN VALUE
  E_BAD_ADDRESS - pointer value does not match an address from session pool;
  E_SUCCESS - otherwise.

===========================================================================*/
secerrno_enum_type secssli_session_crypto_new
(
  secssl_session_type* session_ptr
);

/*===========================================================================

FUNCTION SECSSLI_SESSION_CRYPTO_DELETE

DESCRIPTION
  This function deletes all allocated crypto instances.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session structure

RETURN VALUE
  E_BAD_ADDRESS - pointer value does not match an address from session pool;
  E_SUCCESS - otherwise.
  
SIDE EFFECTS
  All the crypto instances for the session are freed.
===========================================================================*/
secerrno_enum_type secssli_session_crypto_delete
(
  secssl_session_type* session_ptr
);


/*===========================================================================

FUNCTION SECSSLI_SESSION_SHUTDOWN

DESCRIPTION
  This function closes the session's TCP connection and frees
  all allocated crypto instances.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session structure

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void secssli_session_shutdown
(
  secssl_session_type* session_ptr
);


/*===========================================================================

FUNCTION SECSSLI_SET_SES_SERVER

DESCRIPTION
  This function sets the SSL server's address.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session parameter belongs to
  server_ptr  - Pointer to value to be set

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_ses_server
(
  secssl_session_type*       session_ptr,
  secssl_server_pdata_type*  server_ptr
);


/*===========================================================================

FUNCTION SECSSLI_SET_SES_SSL_VERSION

DESCRIPTION
  This function sets the SSL protocol version used for this session.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr     - Pointer to session parameter belongs to
  ssl_version_ptr - Pointer to value to set

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_ses_ssl_version
(
  secssl_session_type*           session_ptr,
  secssl_ssl_version_enum_type*  ssl_version_ptr
);


/*===========================================================================

FUNCTION SECSSLI_SET_SES_CS_LIST

DESCRIPTION
  This function sets the advertised ciphersuite list.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session parameter belongs to
  cs_list_ptr - Pointer to value to set

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_ses_cs_list
(
  secssl_session_type*            session_ptr,
  secssl_ses_cs_list_pdata_type*  cs_list_ptr
);


/*===========================================================================

FUNCTION SECSSLI_SET_SES_CERTIFICATE

DESCRIPTION
  This function sets an optional root certificate to use for
  validation of the SSL server's certificate.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr        - Pointer to session parameter belongs to
  ca_certificate_ptr - Pointer to root CA certificate data

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_ses_ca_certificate
(
  secssl_session_type*                   session_ptr,
  secssl_ses_ca_certificate_pdata_type*  ca_certificate_ptr
);

/*===========================================================================

FUNCTION SECSSLI_SET_SES_NAGLE_MODE

DESCRIPTION
  This function sets the function pointer used for SSL Alert callbacks.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session parameter belongs to
  nagle_mode  - Enum value for nagle mode on/off

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_ses_nagle_mode
(
  secssl_session_type*              session_ptr,
  secssl_ses_nagle_mode_enum_type*  nagle_mode_ptr
);


/*===========================================================================

FUNCTION SECSSLI_SET_SES_ALERT_CB

DESCRIPTION
  This function sets the function pointer used for SSL Alert callbacks.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session parameter belongs to
  pdata_ptr   - Pointer to value to be set

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_ses_alert_cb
(
  secssl_session_type*             session_ptr,
  secssl_ses_alert_cb_pdata_type*  pdata_ptr
);


/*===========================================================================

FUNCTION SECSSLI_SET_ERRNO_CB_FPTR

DESCRIPTION
  This function sets the function pointer used for error condition
  callbacks.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr   - Pointer to session parameter belongs to
  errno_cb_fptr - Pointer to value to be set

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_errno_cb
(
  secssl_session_type*             session_ptr,
  secssl_ses_errno_cb_pdata_type*  errno_cb_pdata_ptr
);


/*===========================================================================

FUNCTION SECSSLI_SET_EVENT_CB

DESCRIPTION
  This function sets the function pointer used for event
  callbacks.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr   - Pointer to session parameter belongs to
  event_cb_fptr - Pointer to value to be set.

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_event_cb
(
  secssl_session_type*             session_ptr,
  secssl_ses_event_cb_fptr_type*   event_cb_fptr
);


/*===========================================================================

FUNCTION SECSSLI_SET_SOCKET_MODE

DESCRIPTION
  This function sets the socket mode for BSD or Event style operation.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr     - Pointer to session parameter belongs to
  socket_mode_ptr - Pointer to value to be set.

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_socket_mode
(
  secssl_session_type*               session_ptr,
  secssl_ses_socket_mode_enum_type*  socket_mode_ptr
);


/*===========================================================================

FUNCTION SECSSLI_SET_SOCKET_FD

DESCRIPTION
  This function sets the socket file descriptor to allow an external
  TCP socket to be used.

DEPENDENCIES
  The socket needs to be connected before secssl_open_session()
  is called.
  
PARAMETERS
  session_ptr   - Pointer to session parameter belongs to
  socket_fd_ptr - Pointer to value to be set.

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_socket_fd
(
  secssl_session_type*              session_ptr,
  secssl_ses_socket_fd_pdata_type*  socket_fd_ptr
);


/*===========================================================================

FUNCTION SECSSLI_SET_DS_APP_ID

DESCRIPTION
  This function sets the application ID needed for calls to DS
  socket functions.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr          - Pointer to session parameter belongs to
  ds_app_id_pdata_ptr  - Pointer to DS app_id 

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_ds_app_id
(
  secssl_session_type*              session_ptr,
  secssl_ses_ds_app_id_pdata_type*  ds_app_id_pdata_ptr
);


/*===========================================================================

FUNCTION SECSSLI_SET_SOCKOPT

DESCRIPTION
  This function sets the ds socket options

DEPENDENCIES
  Should be called before socket is connected
    
PARAMETERS
  session_ptr    - Pointer to session parameter belongs to
  socket_opt_ptr - Pointer to value to be set.

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_ds_sockopt
(
  secssl_session_type*              session_ptr,
  secssl_ses_ds_sockopt_type*       socket_opt_ptr
);

/*===========================================================================

FUNCTION SECSSLI_SET_SES_SERVER_NAME_LIST

DESCRIPTION
  This function sets the server name list to be used as specified in RFC4366.

DEPENDENCIES
  None
    
PARAMETERS
  session_ptr    - Pointer to session parameter belongs to
  server_name_list_ptr - Pointer to server name list.

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_ses_server_name_list
(
  secssl_session_type*                        session_ptr,
  secssl_ses_server_name_list_pdata_type*     server_name_list_ptr
);

/*===========================================================================

FUNCTION SECSSLI_SET_SES_CONNECTION_FPTRS

DESCRIPTION
  This function let the client application (for example, WLAN) to set the SSL session 
  transfer function pointers rather than use default ones.

DEPENDENCIES
  None
    
PARAMETERS
  session_ptr      - Pointer to session parameter belongs to
  connection_fptrs - Pointer to session transfer function pointers.

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_ses_connection_fptrs
(
  secssl_session_type*                        session_ptr,
  secssl_ses_connection_fptrs_pdata_type*     connection_fptrs
);

/*===========================================================================

FUNCTION SECSSLI_SET_SES_SYS_TIME

DESCRIPTION
  SSL time will use either secure time or user time for certificate expiration check 
  according to this input. Default is secure time.

DEPENDENCIES
  None
    
PARAMETERS
  session_ptr    - Pointer to session parameter belongs to
  sys_time_ptr   - Pointer to time type value.

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_ses_sys_time
(
  secssl_session_type*                        session_ptr,
  secssl_ses_sys_time_data_type*              sys_time_ptr
);
/*===========================================================================

FUNCTION SECSSLI_SET_SES_IGNORE_EXPIRATION

DESCRIPTION
  SSL time will skip certificate expiration check if input value is TRUE.
  This param shall only be used in the test environment which time can't be set properly.

DEPENDENCIES
  None
    
PARAMETERS
  session_ptr    - Pointer to session parameter belongs to
  ignore_expiration_ptr   - Pointer to a boolean value.

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_ses_ignore_expiration
(
  secssl_session_type*                        session_ptr,
  secssl_ses_ignore_expiration_data_type*     ignore_expiration_ptr
);

/*===========================================================================

FUNCTION SECSSLI_SET_SES_CLNT_CERT_CONF

DESCRIPTION
  SSL session by default will look for "client-cert/cert.conf" to find
  client cert and key. This function let the client application to set 
  client certificate location. 
  for example:
  set session_cert = TRUE;
  session_cert_config = "my_client_cert/cert.conf", SSL will look
  for client cert which stated in "my_client_cert/cert.conf"

  SSL read and parse the cert\key stated in config file, and set them in session.
  for example, clnt_cert_conf_ptr point to path \my_client_cert\cert.conf, 
  then EFS should be provisioned to 
  \my_client_cert
  |_ client.der
  |_ intermediate.der
  |_ root.der
  |_ client.der
  to match this cert.conf
  <CERT>client.der</CERT>
  <CERT>intermediate.der</CERT>
  <ROOTT>root.der</ROOT>
  <KEY>client.key</KEY>

DEPENDENCIES
  None
    
PARAMETERS
  session_ptr    - Pointer to session parameter belongs to
  clnt_cert_conf_ptr   - Pointer to a string which is client cert\key config file.

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_set_ses_clnt_cert_conf
(
  secssl_session_type*                        session_ptr,
  secssl_ses_cert_conf_data_type*             clnt_cert_conf_ptr
);
/*===========================================================================

FUNCTION SECSSLI_SET_SES_SVR_CERT_CONF
DESCRIPTION: similar to SECSSLI_SET_SES_CLNT_CERT_CONF
===========================================================================*/

secerrno_enum_type secssli_set_ses_svr_cert_conf
(
  secssl_session_type*                        session_ptr,
  secssl_ses_cert_conf_data_type*             svr_cert_conf_ptr
);

/*===========================================================================

FUNCTION SECSSLI_SET_SES_CONNECTION_END

DESCRIPTION  
  SecSSL support both server and client session now.
  This is the interface to set session is a server or client session.

DEPENDENCIES
  None
    
PARAMETERS
  session_ptr                             - Pointer to session parameter belongs to
  secssl_ses_connection_end_enum_type     - Pointer to SSL connection end enum type

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  This parameter shall only be set once, before starting a SSL handshake

===========================================================================*/
secerrno_enum_type secssli_set_ses_connection_end
(
  secssl_session_type*                        session_ptr,
  secssl_ses_connection_end_enum_type*        connection_end_ptr
);

/*===========================================================================

FUNCTION SECSSLI_SET_CA_CERT_FOLDER

DESCRIPTION
  Duirng SSL handshake, the valid x509 certificates under
  a session dedicated folder will be parsed and used as CA candiates.

DEPENDENCIES
  None
    
PARAMETERS
  session_ptr                             - Pointer to session parameter belongs to
  secssl_ses_ca_cert_folder_data_type     - Pointer to session specified CA certs folder

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

===========================================================================*/
secerrno_enum_type secssli_set_ses_ca_cert_folder
(
  secssl_session_type*                        session_ptr,
  secssl_ses_ca_cert_folder_data_type*        ca_cert_folder_ptr
);


/*===========================================================================

FUNCTION SECSSLI_SET_SES_DH_MIN_KEY_SIZE

DESCRIPTION
to Set the minimum acceptable ServerCert DH key size from UE side

DEPENDENCIES
  None
    
PARAMETERS
  session_ptr                             - Pointer to session parameter belongs to
  uint16       -                            - minimum acceptable DH key size

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

===========================================================================*/
secerrno_enum_type secssli_set_ses_dh_min_key_size
(
  secssl_session_type*                        session_ptr,
  uint16*                                     session_dh_key_size
);

/*===========================================================================

FUNCTION SECSSLI_SET_SES_RSA_MIN_KEY_SIZE

DESCRIPTION
to Set the minimum acceptable ServerCert RSA key size from UE side

DEPENDENCIES
  None
    
PARAMETERS
  session_ptr                             - Pointer to session parameter belongs to
  uint16       -                            - minimum acceptable RSA key size

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

===========================================================================*/
secerrno_enum_type secssli_set_ses_rsa_min_key_size
(
  secssl_session_type*                        session_ptr,
  uint16*                                     session_rsa_key_size
);


/*===========================================================================

FUNCTION SECSSLI_GET_SES_SERVER

DESCRIPTION
  This function gets the SSL server's address.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session parameter belongs to
  server_ptr - Pointer to value to be set.

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_get_ses_server
(
  secssl_session_type*       session_ptr,
  secssl_server_pdata_type*  server_ptr
);


/*===========================================================================

FUNCTION SECSSLI_GET_SES_SSL_VERSION

DESCRIPTION
  This function gets the SSL protocol version used for this session.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr     - Pointer to session parameter belongs to
  ssl_version_ptr - Pointer to value to get

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_get_ses_ssl_version
(
  secssl_session_type*           session_ptr,
  secssl_ssl_version_enum_type*  ssl_version_ptr
);


/*===========================================================================

FUNCTION SECSSLI_GET_SES_CS_LIST

DESCRIPTION
  This function gets the advertised ciphersuite list.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session parameter belongs to
  cs_list_ptr - Pointer to value to get

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_get_ses_cs_list
(
  secssl_session_type*            session_ptr,
  secssl_ses_cs_list_pdata_type*  cs_list_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION SECSSLI_GET_SES_ACTIVE_CS

DESCRIPTION
  This function gets active ciphersuite.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session parameter belongs to
  active_cs - Pointer to value to get

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_get_ses_active_cs
(
  secssl_session_type*  session_ptr,
  uint16*               active_cs
);


/*===========================================================================

FUNCTION SECSSLI_GET_SES_KEY_PROT

DESCRIPTION
  This function gets the negotiated key exchange protocol type.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr  - Pointer to session parameter belongs to
  key_prot_ptr - Pointer to requested value filled in by function

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_get_ses_key_prot
(
  secssl_session_type*            session_ptr,
  secssl_ses_key_prot_enum_type*  key_prot_ptr
);


/*===========================================================================

FUNCTION SECSSLI_GET_SES_CRYPT_ALGO

DESCRIPTION
  This function gets the negotiated en- / decryption algorithm type.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr    - Pointer to session parameter belongs to
  crypt_algo_ptr - Pointer to requested value filled in by function  

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_get_ses_crypt_algo
(
  secssl_session_type*              session_ptr,
  secssl_ses_crypt_algo_enum_type*  crypt_algo_ptr
);


/*===========================================================================

FUNCTION SECSSLI_GET_SES_HASH_ALGO

DESCRIPTION
  This function gets the negotiated hash algorithm type.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr   - Pointer to session parameter belongs to
  hash_algo_ptr - Pointer to requested value filled in by function  

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_get_ses_hash_algo
(
  secssl_session_type*             session_ptr,
  secssl_ses_hash_algo_enum_type*  hash_algo_ptr
);


/*===========================================================================

FUNCTION SECSSLI_GET_SES_CERTIFICATE

DESCRIPTION
  This function gets the SSL server's certificate information.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr     - Pointer to session parameter belongs to
  certificate_ptr - Pointer to requested value filled in by function  

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_get_ses_certificate
(
  secssl_session_type*                session_ptr,
  secx509_ses_certificate_pdata_type*  certificate_ptr
);


/*===========================================================================

FUNCTION SECSSLI_GET_SES_CACHE_SIZE

DESCRIPTION
  This function gets the SSL session's cache size.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr    - Pointer to session parameter belongs to
  cache_size_ptr - Pointer to requested value filled in by function

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_get_ses_cache_size
(
  secssl_session_type*               session_ptr,
  secssl_ses_cache_size_pdata_type*  cache_size_ptr
);

/*===========================================================================

FUNCTION SECSSLI_GET_SES_NAGLE_MODE

DESCRIPTION
  This function gets the status of the nagle mode.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session parameter belongs to
  nagle_mode  - Enum value for nagle mode on/off

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_get_ses_nagle_mode
(
  secssl_session_type*              session_ptr,
  secssl_ses_nagle_mode_enum_type*  nagle_mode_ptr
);


/*===========================================================================

FUNCTION SECSSLI_GET_SES_TX_PENDING

DESCRIPTION
  This function gets the number of bytes currently pending for
  transmission.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr    - Pointer to session parameter belongs to
  tx_pending_ptr - Pointer to requested value filled in by function  

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_get_ses_tx_pending
(
  secssl_session_type*               session_ptr,
  secssl_ses_tx_pending_pdata_type*  tx_pending_ptr
);


/*===========================================================================

FUNCTION SECSSLI_GET_SES_RX_PENDING

DESCRIPTION
  This function gets the number of received bytes pending
  for delivery. 

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr    - Pointer to session parameter belongs to
  rx_pending_ptr - Pointer to requested value filled in by function

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secssli_get_ses_rx_pending
(
  secssl_session_type*               session_ptr,
  secssl_ses_rx_pending_pdata_type*  rx_pending_ptr
);


/*===========================================================================

FUNCTION SECSSLI_LOG_SSL_MSG

DESCRIPTION
  This function logs an SSL protocol message to diag for DMSS builds
  and to stdout for Solaris builds.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr    - Pointer to session parameter belongs to
  prot_id        - SSL Signaling Protocol Type
  msg_ptr        - Pointer to signaling message data
  msg_was_rcvd   - Indicator for RX resp. TX

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void secssli_log_ssl_msg
(
  secssl_session_type*          session_ptr, 
  ssl_v3_protocol_id_enum_type  prot_id, 
  dsm_item_type*                msg_ptr, 
  boolean                       msg_was_rcvd
);


/*===========================================================================

FUNCTION SECSSLI_LOG_SSL_RECORD

DESCRIPTION
  This function logs an SSL protocol record to diag for DMSS builds
  and to stdout for Solaris builds.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr    - Pointer to session parameter belongs to
  prot_id        - SSL Protocol Type
  rec_len        - Length of record in bytes
  rec_ptr        - Pointer to record data
  rec_was_rcvd   - Indicator for RX resp. TX

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void secssli_log_ssl_record
(
  secssl_session_type*          session_ptr, 
  ssl_v3_protocol_id_enum_type  prot_id, 
  uint16                        rec_len,
  dsm_item_type*                rec_ptr, 
  boolean                       rec_was_rcvd
);



#endif /* FEATURE_SEC_SSL */

#endif /* SECSSLI_H */
