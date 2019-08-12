#ifndef SECSSLHSP_H
#define SECSSLHSP_H

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                S E C U R E   S O C K E T S   L A Y E R

            S S L   V 3   H A N D S H A K E   P R O T O C O L

GENERAL DESCRIPTION
  This file contains the SSL Handshake Protocol.

EXTERNALIZED FUNCTIONS
  secsslhsp_set_handshake_cb  - Set the completion callback
  secsslhsp_do_connect        - Initiate a handshake from a SSL client
  secsslhsp_do_accept         - SSL server do handshake with a SSL client
  secsslhsp_process_change_cipherspec  - Initiate a cipherspec change
  secsslhsp_key_gen_cb        - Key generation cb for crypto layer

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The required SSL session parameters have to be set.  

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/secsslhsp.h_v   1.12   03 Sep 2003 11:08:22   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/shared/src/secsslhsp.h#1 $ $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/15   nm      TLSv1.2 support
06/01/10   ml      Removed seccrypt.h from include definitions.
10/30/03   jay     Added enums and #defines for finsihed processing.
10/29/03   jay     Removed FEATURE_HASH_FINAL_WITH_DATA
04/11/03   jay     Removed prototype for secsslhsp_key_gen_cb().
06/04/02   om      Added verify_svr_key_x() prototype.
06/01/01   om      Added logging support.
05/24/01   om      Added SSL session resumption and multiple session.
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
#include "secerrno.h"

#include "secssldef.h"
#include "secssl.h"
#include "secssli.h"

/* <EJECT> */ 
/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

#define SECSSLHSP_SCRATCH_BUF_SIZE   128   /* 1024 bits */

/* Operation mode of the HS handshake updater */
typedef enum
{
  HSP_HASH_UPDATE_BOTH = 0x0,
  HSP_HASH_UPDATE_CLNT,
  HSP_HASH_UPDATE_SVR,
  HSP_HASH_UPDATE_BOTH_CLNT_FINAL,
  HSP_HASH_UPDATE_BOTH_SVR_FINAL
} secsslhsp_update_handshake_hash_mode_enum_type;

/* Operation mode for the HS finished message*/
typedef enum
{
  HSP_CLIENT_FINISHED = 0x0,
  HSP_SERVER_FINISHED,
} secsslhsp_finished_mode_enum_type;

/*-------------------------------------------------------------------------*/
/*                   SSL HMAC definitions                                  */
/*-------------------------------------------------------------------------*/
#define HSP_SHA_HMAC_PAD_LEN  40
#define HSP_MD5_HMAC_PAD_LEN  48

#define HSP_INNER_PAD_VALUE  0x36
#define HSP_OUTER_PAD_VALUE  0x5C


/*-------------------------------------------------------------------------*/
/* The handshale protocol state enum and structure are defined in          */
/* secssli.h to avoid cyclic dependencies. (The session structure needs    */
/* the handshake protocol state defined.)                                  */
/* See the first section of secssli.h for the definitions.                 */
/*-------------------------------------------------------------------------*/

/* <EJECT> */ 
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================

FUNCTION SECSSLHSP_RESET

DESCRIPTION
  This function resets the handshake protocol.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr  - Pointer to the SSL session

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void secsslhsp_reset
(
  secssl_session_type*  session_ptr 
);


/*===========================================================================

FUNCTION SECSSLHSP_SET_HANDSHAKE_CB

DESCRIPTION
  This function initializes the handshake completion callback.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr     - Pointer to the SSL session
  hs_cb_fptr      - Callback function pointer
  hs_usrdata_ptr  - User data for callback

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void secsslhsp_set_handshake_cb
(
  secssl_session_type*   session_ptr,
  secerrno_cb_fptr_type  hs_cb_fptr,
  void*                  hs_usrdata_ptr
);

/*===========================================================================

FUNCTION SECSSLHSP_DO_CONNECT

DESCRIPTION
  This function start the SSL handshake procdure from SSL client.

DEPENDENCIES
  All required session parameters have to be set.
  
PARAMETERS
  session_ptr  - Pointer to the SSL session

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secsslhsp_do_connect
(
  secssl_session_type*  session_ptr 
);
/*===========================================================================

FUNCTION SECSSLHSP_PROCESS_MSG

DESCRIPTION
  This function dispatches received handshake messages to
  their processing functions.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr  - Pointer to the SSL session
  msg_ptr      - Pointer to message data

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void secsslhsp_process_msg
(
  secssl_session_type*  session_ptr,
  dsm_item_type**       msg_ptr 
);


/*===========================================================================

FUNCTION SECSSLHSP_PROCESS_HELLO_REQUEST

DESCRIPTION
  This function processes the HelloRequest message.

DEPENDENCIES

PARAMETERS
  session_ptr  - Pointer to the SSL session
  msg_ptr      - Pointer to the message data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void secsslhsp_process_hello_request
( 
  secssl_session_type*  session_ptr, 
  dsm_item_type**       msg_ptr 
);


/*===========================================================================

FUNCTION SECSSLHSP_PROCESS_SERVER_HELLO

DESCRIPTION
  This function processes the ServerHello message.

DEPENDENCIES

PARAMETERS
  session_ptr  - Pointer to the SSL session
  msg_ptr      - Pointer to the message data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void secsslhsp_process_server_hello
( 
  secssl_session_type*  session_ptr, 
  dsm_item_type**       msg_ptr 
);


/*===========================================================================

FUNCTION SECSSLHSP_PROCESS_CERTIFICATE

DESCRIPTION
  This function processes the Certificate message.

DEPENDENCIES

PARAMETERS
  session_ptr  - Pointer to the SSL session
  msg_ptr      - Pointer to the message data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void secsslhsp_process_certificate
( 
  secssl_session_type*  session_ptr, 
  dsm_item_type**       msg_ptr 
);


/*===========================================================================

FUNCTION SECSSLHSP_PROCESS_SERVER_KEY_EXCHANGE

DESCRIPTION
  This function processes the ServerKeyExchange message.

DEPENDENCIES

PARAMETERS
  session_ptr  - Pointer to the SSL session
  msg_ptr      - Pointer to the message data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void secsslhsp_process_server_key_exchange
( 
  secssl_session_type*  session_ptr, 
  dsm_item_type**       msg_ptr 
);


/*===========================================================================

FUNCTION SECSSLHSP_PARSE_SERVER_KEY_X

DESCRIPTION
  This function parses the ServerKeyExchange message.

DEPENDENCIES

PARAMETERS
  session_ptr  - Pointer to the SSL session
  msg_ptr      - Pointer to the message data
  msg_len      - Length of message data

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

  Possible error conditions:
  See error codes for Crypto PKX routines.

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_parse_server_key_x
(
  secssl_session_type*  session_ptr, 
  dsm_item_type**       msg_ptr,
  uint16                msg_len
);


/*===========================================================================

FUNCTION SECSSLHSP_VERIFY_SERVER_KEY_X

DESCRIPTION
  This function verifies the RSA signature of the ServerKeyExchangeMsg.

DEPENDENCIES

PARAMETERS
  session_ptr  - Pointer to the SSL session
  msg_ptr      - Pointer to the message data
  sig_offset   - Offset of the RSA signature field in the msg
  sig_len      - Length of signature including 16-bit sign. length

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

  Possible error conditions:
  E_BAD_DATA - Signature doesn't match, or is encoded wrongly.
  For other error codes see Crypto PKX routines.

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_verify_server_key_x
( 
  secssl_session_type*  session_ptr, 
  dsm_item_type**       msg_ptr, 
  uint16                sig_offset,
  uint16                sig_len
);

/*===========================================================================

FUNCTION SECSSLHSP_TLSV12_VERIFY_SERVER_KEY_X

DESCRIPTION
  This function verifies the RSA signature of the ServerKeyExchangeMsg.

DEPENDENCIES

PARAMETERS
  session_ptr  - Pointer to the SSL session
  msg_ptr      - Pointer to the message data
  sig_offset   - Offset of the RSA signature field in the msg
  sig_len      - Length of signature including 16-bit sign. length

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

  Possible error conditions:
  E_BAD_DATA - Signature doesn't match, or is encoded wrongly.
  For other error codes see Crypto PKX routines.

SIDE EFFECTS
  None
===========================================================================*/

secerrno_enum_type secsslhsp_tlsv12_verify_server_key_x
( 
  secssl_session_type*  session_ptr, 
  dsm_item_type**       msg_ptr, 
  uint16                sig_offset,
  uint16                sig_len
);


/*===========================================================================

FUNCTION SECSSLHSP_PROCESS_CERTIFICATE_REQUEST

DESCRIPTION
  This function processes the CertificateRequest message.

DEPENDENCIES

PARAMETERS
  session_ptr  - Pointer to the SSL session
  msg_ptr      - Pointer to the message data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void secsslhsp_process_certificate_request
( 
  secssl_session_type*  session_ptr, 
  dsm_item_type**       msg_ptr 
);


/*===========================================================================

FUNCTION SECSSLHSP_PROCESS_SERVER_HELLO_DONE

DESCRIPTION
  This function processes the ServerHelloDone message.

DEPENDENCIES

PARAMETERS
  session_ptr  - Pointer to the SSL session
  msg_ptr      - Pointer to the message data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void secsslhsp_process_server_hello_done
( 
  secssl_session_type*  session_ptr, 
  dsm_item_type**       msg_ptr 
);


/*===========================================================================

FUNCTION SECSSLHSP_PROCESS_CHANGE_CIPHERSPEC

DESCRIPTION
  This function initiates the activation of the server's
  pending ciphersuite.

DEPENDENCIES

PARAMETERS
  session_ptr  - Pointer to the SSL session

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void secsslhsp_process_change_cipherspec
( 
  secssl_session_type*  session_ptr
);


/*===========================================================================

FUNCTION SECSSLHSP_PROCESS_FINISHED

DESCRIPTION
  This function processes the Finished message.

DEPENDENCIES

PARAMETERS
  session_ptr  - Pointer to the SSL session
  msg_ptr      - Pointer to the message data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void secsslhsp_process_finished
( 
  secssl_session_type*  session_ptr, 
  dsm_item_type**       msg_ptr 
);


/*===========================================================================

FUNCTION SECSSLHSP_SEND_CLIENT_HELLO

DESCRIPTION
  This function sends the ClientHello message.

DEPENDENCIES
  None

PARAMETERS
  session_ptr  - Pointer to the SSL session

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_send_client_hello
( 
  secssl_session_type*  session_ptr 
);


/*===========================================================================

FUNCTION SECSSLHSP_SEND_CERTIFICATE

DESCRIPTION
  This function sends the Certificate message.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session structure

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_send_certificate
(
  secssl_session_type*  session_ptr
);


/*===========================================================================

FUNCTION SECSSLHSP_SEND_CERTIFICATE_VERIFIY

DESCRIPTION
  This function sends the CertificateVerify message.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session structure

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_send_certificate_verify
(
  secssl_session_type*  session_ptr
);


/*===========================================================================

FUNCTION SECSSLHSP_SEND_CLIENT_KEY_EXCHANGE

DESCRIPTION
  This function sends the ClientKeyExchange message.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session structure

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_send_client_key_exchange
(
  secssl_session_type*  session_ptr
);


/*===========================================================================

FUNCTION SECSSLHSP_SEND_FINISHED

DESCRIPTION
  This function sends the Finished message.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr  - Pointer to the SSL session

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_send_finished
(
  secssl_session_type*  session_ptr
);


/*===========================================================================

FUNCTION SECSSLHSP_CREATE_SVR_FINISHED_HASHES

DESCRIPTION
  This function generates the server's finished hashes..

DEPENDENCIES
  The running server hashes have been updated with all 
  handshake messages up to this point before calling
  this function.
  
PARAMETERS
  session_ptr  - Pointer to the SSL session

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_create_svr_finished_hashes
(
  secssl_session_type*  session_ptr
);


/*===========================================================================

FUNCTION SECSSLHSP_CREATE_SSLV3_SVR_FINISHED_HASHES

DESCRIPTION
  This function generates the server's finished hashes..

DEPENDENCIES
  The running server hashes have been updated with all 
  handshake messages up to this point before calling
  this function.
  
PARAMETERS
  session_ptr  - Pointer to the SSL session

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_create_sslv3_svr_finished_hashes
(
  secssl_session_type*  session_ptr
);

/*===========================================================================

FUNCTION SECSSLHSP_CREATE_TLSV1_SVR_FINISHED_HASHES

DESCRIPTION
  This function generates the verify data to compare to the server's
  Finished Message verify data.  

DEPENDENCIES
  The running server hashes have been updated with all 
  handshake messages up to this point before calling
  this function.
  
PARAMETERS
  session_ptr  - Pointer to the SSL session

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_create_tlsv1_svr_finished_hashes
(
  secssl_session_type*  session_ptr
);

/*===========================================================================

FUNCTION SECSSLHSP_CREATE_TLSV12_SVR_FINISHED_HASHES

DESCRIPTION
  This function generates the verify data to compare to the server's
  Finished Message verify data.  

DEPENDENCIES
  The running server hashes have been updated with all 
  handshake messages up to this point before calling
  this function.
  
PARAMETERS
  session_ptr  - Pointer to the SSL session

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_create_tlsv12_svr_finished_hashes
(
  secssl_session_type*  session_ptr
);

/*===========================================================================

FUNCTION SECSSLHSP_CREATE_HSP_RANDOM

DESCRIPTION
  This function creates the client/server random number.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session for which to create the client/server random

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_create_hsp_random
(
  secssl_session_type*  session_ptr
);


/*===========================================================================

FUNCTION SECSSLHSP_CREATE_MASTER_SECRET

DESCRIPTION
  This function derives the master secret from the premaster secret
  and the server and client random numbers.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session for which to derive the master secret

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_create_master_secret
(
  secssl_session_type*  session_ptr
);


/*===========================================================================

FUNCTION SECSSLHSP_CREATE_KEYS

DESCRIPTION
  This function derives the MAC secrets, cipher keys, and cipher
  initialization vectors based on the master secret and the server
  and client random numbers.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session for which to derive the keys

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_create_keys
(
  secssl_session_type*  session_ptr
);


/*===========================================================================

FUNCTION SECSSLHSP_CREATE_SSLV3_KEY_MATERIAL

DESCRIPTION
  This function creates SSL v3 key material by chaining together
  MD5 digests from given input data up to the requested length.
  See the comment in the funxtion body for details.

DEPENDENCIES
  None
  
PARAMETERS
  inner_data_ptr   - Static inner hash input (A|BB|CCC|... + inner_data_ptr)
  outer_data_ptr   - Static outer hash input
  key_material_ptr - Pointer to resulting key material in a dsm item chain
  length           - minimum length required

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_create_sslv3_key_material
(
  dsm_item_type*   inner_data_ptr,
  dsm_item_type*   outer_data_ptr,
  dsm_item_type**  key_material_ptr,
  uint16           length
);


/*===========================================================================

FUNCTION SECSSLHSP_RESUME_CIPHERSUITE

DESCRIPTION
  This function re-establishes a previous ciphersuites and
  regenerates the derived cipher data (keys, init vectors, and
  MAC secrets) from the new random values.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr  - Pointer to SSL session being resumed

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_resume_ciphersuite
(
  secssl_session_type*  session_ptr
);


/*===========================================================================

FUNCTION SECSSLHSP_CREATE_HEADER

DESCRIPTION
  This function creates the handshake protocol header and
  prepends it to the passed-in handshake message.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to a valid SSL session
  msg_ptr     - Pointer to handshake message
  msg_id      - Handshake message ID
  msg_len     - Length of handshake message

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_create_header
(
  secssl_session_type*  session_ptr,
  dsm_item_type**       msg_ptr,
  uint8                 msg_id,
  uint16                msg_len
);


/*===========================================================================

FUNCTION SECSSLHSP_UPDATE_HANDSHAKE_HASH

DESCRIPTION
  This function updates the running handshake hashes.
  The mode allows to update both, or the client and server
  hashes individually.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to the SSL session
  msg_ptr     - Handshake message data (input for hashes)
  msg_len     - Length of handshake message data
  mode        - Update mode (client, server, both)

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_update_handshake_hash
(
  secssl_session_type*                            session_ptr,
  dsm_item_type*                                  msg_ptr,
  uint16                                          msg_len, 
  secsslhsp_update_handshake_hash_mode_enum_type  mode
);


/*===========================================================================

FUNCTION SECSSLHSP_BUMMER

DESCRIPTION
  This function is the generic failure function. Any failure
  during the handshake phase will end up here and cause a 
  session abort and cleanup.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr - Pointer to session parameter belongs to
  _errno      - Error code triggering the abort
  errmsg      - Error message to print out
  line_num    - Line number of call to this function

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void secsslhsp_bummer
(
  secssl_session_type*  session_ptr,
  secerrno_enum_type    _errno,
  const char*           errmsg,
  int                   line_num
);


/*===========================================================================

FUNCTION SECSSLHSP_TLSV1_PSEUDORANDOM

DESCRIPTION
  This function computes the requested amount of pseudorandom output
  according to the procedure specified by Transport Layer Security Version 1.
  
DEPENDENCIES
  None
  
PARAMETERS
  secret_ptr  - secret data (input for hashes)
  secret_len  - length of secret data
  label_ptr   - label data (input for hashes)
  label_len   - length of label data
  seed1_ptr   - first seed data (input for hashes)
  seed1_len   - length of first seed data
  seed2_ptr   - second seed data (input for hashes)
  seed2_len   - length of second seed data
  requested_len - requested number of bytes of output
  out_ptr     - address of output buffer
  tmp_ptr     - address of temporary output buffer
  
RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)
  
SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type  secsslhsp_tlsv1_pseudorandom(
  uint8* secret_ptr, uint16 secret_len,
  uint8*  label_ptr, uint16  label_len,
  //uint8*  seed1_ptr, uint16  seed1_len,
  //uint8*  seed2_ptr, uint16  seed2_len,
  uint16 requested_len,
  uint8*    out_ptr ,uint8*    tmp_ptr 
);

/***************************************************************************/
/*      SSL server handshake                                               */
/***************************************************************************/
/*===========================================================================

FUNCTION SECSSLHSP_DO_ACCEPT

DESCRIPTION
  This function start the SSL handshake procdure from SSL SERVER.

DEPENDENCIES
  All required session parameters have to be set.
  
PARAMETERS
  session_ptr  - Pointer to the SSL session

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secsslhsp_do_accept
(
  secssl_session_type*  session_ptr 
);

/*===========================================================================

FUNCTION SECSSLHSP_SEND_HELLO_REQUEST

DESCRIPTION
  This function sends the HelloRequest message.

DEPENDENCIES
  None

PARAMETERS
  session_ptr  - Pointer to the SSL session

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_send_hello_request
( 
  secssl_session_type*  session_ptr 
);

/*===========================================================================

FUNCTION SECSSLHSP_PROCESS_CLIENT_HELLO

DESCRIPTION
  This function processes the ClientHello message.

DEPENDENCIES

PARAMETERS
  session_ptr  - Pointer to the SSL session
  msg_ptr      - Pointer to the message data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void secsslhsp_process_client_hello
( 
  secssl_session_type*  session_ptr, 
  dsm_item_type**       msg_ptr 
);

/*===========================================================================

FUNCTION SECSSLHSP_SEND_SERVER_HELLO

DESCRIPTION
  This function sends the ServerHello message.

DEPENDENCIES
  None

PARAMETERS
  session_ptr  - Pointer to the SSL session

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_send_server_hello
( 
  secssl_session_type*  session_ptr 
);

/*===========================================================================

FUNCTION SECSSLHSP_SEND_SERVER_HELLO_DONE

DESCRIPTION
  This function sends the ServerHelloDone message.

DEPENDENCIES
  None

PARAMETERS
  session_ptr  - Pointer to the SSL session

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secsslhsp_send_server_hello_done
( 
  secssl_session_type*  session_ptr 
);

/*===========================================================================

FUNCTION SECSSLHSP_PROCESS_CLIENT_KEY_EXCHANGE

DESCRIPTION
  This function processes the ClientKeyExchange message.

DEPENDENCIES

PARAMETERS
  session_ptr  - Pointer to the SSL session
  msg_ptr      - Pointer to the message data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void secsslhsp_process_client_key_exchange
( 
  secssl_session_type*  session_ptr, 
  dsm_item_type**       msg_ptr 
);



#endif /* FEATURE_SEC_SSL */

#endif
