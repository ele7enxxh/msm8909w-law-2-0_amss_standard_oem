#ifndef SSM_DIAG_OCH_V_H
#define SSM_DIAG_OCH_V_H
/*==========================================================================

   Diagnostic Packet Definitions for SSM Override Command Handler

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

  !!! NOTE: All member structures of diag packets must be PACKED.

  !!! WARNING: Each command code number is part of the externalized
  diagnostic command interface.  This number *MUST* be assigned
  by a member of QCT's tools development team.

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_diag_och_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/12   din     File created.

===========================================================================*/



#include "ssm_diag_cmd_v.h"
#include "ssm_key_handler.h"
#include "ssm_och_v.h"


/* -------------------------------------------------------------------------
** Packet Definitions
** ------------------------------------------------------------------------- */



/**
Requests Random Nonce

Initiates Authentication Process using RSA PKCS#1 PSS digital signature.
The override control handler generates a cryptographically random nonce 
of a fixed size (256 bits) and stores this nonce value in the appropriate 
global variable. The Boolean nonce indicator is set to true.

This will be a Normal Diag command response.

Command : send_data 0x4B 0x56 0x08 0x00 
Successful Immediate Response : 0x4B 0x56 0x08 0x00 0x00 0x00 0x00 0x00
                               <256 bit random nonce>

*/

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
} ssm_diag_och_request_random_nonce_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  ssm_och_err_t      err;
  uint8              random_nonce[32];
} ssm_diag_och_request_random_nonce_rsp_type;


/**
Requests Temporary override

The user sends a DIAG message containing an RSA PKCS#1 PSS digital signature 
of the random nonce concatenated with the 8-bit value 0x00. ( 8 - bit value part 
of uint8 type signature ). 
The override command handler checks that the key returned by the key handler 
is suitable for use with the RSA PKCS#1 PSS algorithm and that the signature 
value in the DIAG message verifies under the public key returned by the key 
handler. If so, it informs all its clients and returns a 
successful response message. Otherwise, it returns an unsuccessful DIAG 
response message.

This will be a Delayed Diag command response.

Command : send_data 128 0x56 0x09 0x00 <32 bit source > <16 bit len_sig > <signature >
Successful Immediate Response : <12 byte Diag Header > 0x01 0x00 0x00 0x00
Successful Delayed Response : <12 byte Diag Header > 0x00 0x00 0x00 0x00 

*/

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  uint32                     source;
  uint16                     len_signature;
  uint8                      signature[1];
} ssm_diag_och_request_temporary_override_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_v2_type hdr;
  ssm_och_err_t         err;
} ssm_diag_och_request_temporary_override_rsp_type;

/**
Requests Permanent override

The user sends a DIAG message containing an RSA PKCS#1 PSS digital signature 
of the random nonce concatenated with the 8-bit value 0x01. ( 8 - bit value part 
of uint8 type signature ). 
The override command handler checks that the key returned by the key handler 
is suitable for use with the RSA PKCS#1 PSS algorithm and that the signature 
value in the DIAG message verifies under the public key returned by the key 
handler. If so, it writes the override cookie, and inform all its clients
and returns a successful response message. Otherwise, it 
returns an unsuccessful DIAG response message.

This will be a Delayed Diag command response.

Command : send_data 128 0x56 0x0A 0x00 <32 bit source > <16 bit len_sig > <signature >
Successful Immediate Response : <12 byte Diag Header > 0x01 0x00 0x00 0x00
Successful Delayed Response : <12 byte Diag Header > 0x00 0x00 0x00 0x00 

*/

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  uint32                     source;
  uint16                     len_signature;
  uint8                      signature[1];
} ssm_diag_och_request_permanent_override_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_v2_type hdr;
  ssm_och_err_t         err;
} ssm_diag_och_request_permanent_override_rsp_type;


/**
Cancels Override 
 
The override command handler will delete the override cookie 
(if it exists) and will inform its clients.

This will be a Delayed Diag command response.

Command : send_data 128 0x56 0x0B 0x00 
Successful Immediate Response : <12 byte Diag Header > 0x01 0x00 0x00 0x00
Successful Delayed Response : <12 byte Diag Header > 0x00 0x00 0x00 0x00 

*/

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
} ssm_diag_och_cancel_override_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_v2_type hdr;
  ssm_och_err_t         err;
} ssm_diag_och_cancel_override_rsp_type;


/**
Removes Authentication Cookie
 
OCH Authenitcation cookie removal functionality is only intended for testing 
and will be featured out in CRM builds.

This will be a Delayed Diag command response.

Command : send_data 128 0x56 0x0C 0x00 
Successful Immediate Response : <12 byte Diag Header > 0x01 0x00 0x00 0x00
Successful Delayed Response : <12 byte Diag Header > 0x00 0x00 0x00 0x00 

*/

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
} ssm_diag_och_remove_authentication_cookie_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_v2_type hdr;
  ssm_och_err_t         err;
} ssm_diag_och_remove_authentication_cookie_rsp_type;



/**
Reset Random Nonce
 
Boolean indicator will be set to FALSE.
OCH Authenitcation cookie removal functionality is only intended for testing 
and will be featured out in CRM builds.

This will be a Normal Diag command response.

Command : send_data 0x4B 0x56 0x0E 0x00 
Successful Immediate Response : send_data 0x4B 0x56 0x0E 0x00 0x00 0x00 0x00 0x00
 
*/

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
} ssm_diag_och_reset_random_nonce_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  ssm_och_err_t      err;
} ssm_diag_och_reset_random_nonce_rsp_type;



/**
Sign Random Nonce
 
OCH Sign Random Nonce functionality is only intended for testing 
and will be featured out in CRM builds.

This will be a Delayed Diag command response.

Command : send_data 0x4B 0x56 0x0F 0x00 <uint8 temp_perm_override> < uint16 mod_len > 
                                        < uint16 exp_len > <uint8 data>
Successful Immediate Response : <12 byte Diag header > 0x00 0x00 0x00 0x00
Successful Delayed Response : <12 byte Diag header > 0x00 0x00 0x00 0x00 
                              <uint16 signature length > <uint8 signature>
 
 */

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  /* 0 indicates temp override , 1 indicates perm override */
  uint8                      temp_perm_override;
  uint16                     mod_len;
  uint16                     exp_len;
  uint8                      mod_exp[1];  
} ssm_diag_och_sign_random_nonce_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_v2_type hdr;
  ssm_och_err_t              err;
  uint16                     len_sig;
  uint8                      sig[1];
} ssm_diag_och_sign_random_nonce_rsp_type;


void ssm_diag_och_init( void );
void ssm_diag_och_test_init( void );

#endif /* SSM_DIAG_OCH_V_H */
