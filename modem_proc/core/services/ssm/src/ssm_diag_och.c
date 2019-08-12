/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              SSM Diagnostics Packet Processing
           S S M   D I A G   O C H   F R A M E W O R K 

General Description
  Diagnostic packet processing routines for SSM operations.

Copyright (c) 2012 - 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_diag_och.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
4/16/13    din     Removed warnings.
04/02/13   din     Replaced memcpy with memscpy.
11/05/12   din     File created.

===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "diagcmd.h"
#include "diagpkt.h"
#include "ssm_v.h"
#include "ssm_diag_och_v.h"
#include "ssm_key_handler.h"
#include <stringl/stringl.h>
#include "stdlib.h"




/*=============================================================================

                         DATA DEFINITIONS
=============================================================================*/


/* The size of the largest possible request packet.  */
#ifdef  DIAG_MAX_RX_PKT_SIZ
  #define LARGEST_DIAG_PACKET     DIAG_MAX_RX_PKT_SIZ
#else
  #define LARGEST_DIAG_PACKET     0x800
#endif

#ifdef DIAG_MAX_TX_PKT_SIZ
  #define LARGEST_DIAG_RESPONSE   DIAG_MAX_TX_PKT_SIZ
#else
  #define LARGEST_DIAG_RESPONSE   0x800
#endif


#define LEN_SIGNATURE  256

/**********************************************************************
 * Packet length validation.
 *********************************************************************/

static int ssm_diag_och_is_valid_pkt_len 
(
  uint32 pkt_len, 
  uint32 min_pkt_len
)
{
  /* Validate pkt_len range */
  if (pkt_len > LARGEST_DIAG_PACKET || pkt_len < min_pkt_len)
  {
    return 0;
  }
  return 1;
}

/* Macro to calculate length of sine of the structure sans "mem" data member */
#ifndef offsetof
#define offsetof(str, mem) \
     ((uint32) ( (char *)&((str *)0)->mem - (char *)0 ))
#endif

/* RSP id for Diag Command */
#define RSP_ID( A)  A##_rsp_id  


/* Pre Code macros  are internal to this file and use certain paramenters 
 * which are not passed as argument */

#define SSM_DIAG_OCH_PRECODE( A, B )  \
A##_rsp_type *response; \
uint16 rsp_len = sizeof (A##_rsp_type); \
if (! ssm_diag_och_is_valid_pkt_len (pkt_len, \
      sizeof (A##_req_type))) \
{ \
  return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len); \
} \
response = (A##_rsp_type *) \
diagpkt_subsys_alloc ( DIAG_SUBSYS_SSM, \
                       B, \
                       rsp_len); \
if (response == NULL) \
    return NULL;

#define SSM_DIAG_OCH_PRECODE_EXT( A, B, C )  \
A##_req_type *request; \
A##_rsp_type *response; \
uint16 rsp_len = sizeof (A##_rsp_type); \
if (! ssm_diag_och_is_valid_pkt_len (pkt_len, \
      sizeof (A##_req_type))) \
{ \
  return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len); \
} \
request = (A##_req_type *) req_ptr; \
response = (A##_rsp_type *) \
diagpkt_subsys_alloc ( DIAG_SUBSYS_SSM, \
                       B, \
                       rsp_len + C ); \
if (response == NULL) \
    return NULL;



#define SSM_DIAG_OCH_PRECODE_DELAYED( A, B )  \
A##_req_type *request; \
A##_rsp_type *response; \
uint16 rsp_len = sizeof (A##_rsp_type); \
if (! ssm_diag_och_is_valid_pkt_len (pkt_len, \
      sizeof (A##_req_type))) \
{ \
  return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len); \
} \
request = (A##_req_type *) req_ptr; \
response = (A##_rsp_type *) \
diagpkt_subsys_alloc_v2	( DIAG_SUBSYS_SSM, \
                          B, \
                          rsp_len); \
if (response == NULL) \
    return NULL; \
diagpkt_subsys_delayed_rsp_id_type * A##_rsp_id \
             = malloc( sizeof( diagpkt_subsys_delayed_rsp_id_type) ); \
if ( A##_rsp_id == NULL ) \
{ \
  free( response ); \
  return NULL; \
} \
*A##_rsp_id = diagpkt_subsys_get_delayed_rsp_id(response); 


#define SSM_DIAG_OCH_PRECODE_DELAYED_CANCEL( A, B )  \
A##_rsp_type *response; \
uint16 rsp_len = sizeof (A##_rsp_type); \
if (! ssm_diag_och_is_valid_pkt_len (pkt_len, \
      sizeof (A##_req_type))) \
{ \
  return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len); \
} \
response = (A##_rsp_type *) \
diagpkt_subsys_alloc_v2	( DIAG_SUBSYS_SSM, \
                          B, \
                          rsp_len); \
if (response == NULL) \
    return NULL; \
diagpkt_subsys_delayed_rsp_id_type * A##_rsp_id \
             = malloc( sizeof( diagpkt_subsys_delayed_rsp_id_type) ); \
if ( A##_rsp_id == NULL ) \
{ \
  free( response ); \
  return NULL; \
} \
*A##_rsp_id = diagpkt_subsys_get_delayed_rsp_id(response); 



#define SSM_DIAG_OCH_CB_PRECODE_DELAYED( A, B )  \
A##_rsp_type *response; \
uint16 rsp_len = sizeof (A##_rsp_type); \
diagpkt_subsys_delayed_rsp_id_type * A##_rsp_id; \
A##_rsp_id = ( diagpkt_subsys_delayed_rsp_id_type * ) user_data; \
if ( A##_rsp_id == NULL ) \
  return; \
response = (A##_rsp_type *) \
diagpkt_subsys_alloc_v2_delay ( DIAG_SUBSYS_SSM, \
                          B, \
		          *A##_rsp_id, \
                          rsp_len); \
free ( A##_rsp_id ); \
if (response == NULL) \
    return ; 



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

@param[in]  
  req_ptr                   Pointer to the Packet command
  pkt len                   Length of the Packet

@return 
None 

@dependencies
None

*/

PACK(void *) ssm_diag_och_request_random_nonce_handler 
( 
  PACK(void *) req_ptr, 
  uint16 pkt_len
)
{
  SSM_DIAG_OCH_PRECODE( ssm_diag_och_request_random_nonce,
		        SSM_DIAG_OCH_REQUEST_RANDOM_NONCE );

  uint8 * random_nonce_ptr = response->random_nonce;

  response->err =ssm_och_get_random_nonce( random_nonce_ptr ) ;

  return response;

} /* ssm_diag_och_request_random_nonce_handler */




		  
/**
Callback for Temporary Override

@param[in]  
  ssm_och_err_t         err    Depicts status of Command request 

@return 
None 

@dependencies
None

*/
static void ssm_diag_och_request_temporary_override_cb
(
  /** Error type : Depicts status of Command request */
  ssm_och_err_t         err,

  /* Length of return data */
  uint16                len_data,

  /** Output Data type */
  uint8 *               output_data,

  /** User data */
  ssm_och_cb_data_type user_data
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_DIAG_OCH_CB_PRECODE_DELAYED( 
		        ssm_diag_och_request_temporary_override,
		        SSM_DIAG_OCH_REQUEST_TEMPORARY_OVERRIDE );
  
  response->err = err;

  diagpkt_delay_commit(response);
	
} /* ssm_diag_och_request_temporary_override_cb */



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
                  

@param[in]  
  req_ptr                   Pointer to the Packet command
  pkt len                   Length of the Packet

@return 
None 

@dependencies
None

*/

PACK(void *) ssm_diag_och_request_temporary_override_handler 
( 
  PACK(void *) req_ptr, 
  uint16 pkt_len
)
{
  SSM_DIAG_OCH_PRECODE_DELAYED( 
		        ssm_diag_och_request_temporary_override,
		        SSM_DIAG_OCH_REQUEST_TEMPORARY_OVERRIDE );

  /* Command request structure */
  ssm_och_cmd_struct_t  cmd_request;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( pkt_len - offsetof(ssm_diag_och_request_temporary_override_req_type,
			  signature ) != request->len_signature )
  {
    free ( RSP_ID( ssm_diag_och_request_temporary_override ) );
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }
  

  /* Update SSM OCH Cmd structure to request temporary override */
  cmd_request.ovrr_cmd.cmd = SSM_OCH_TEMP_OVERRIDE;
  cmd_request.ovrr_cmd.cb  = ssm_diag_och_request_temporary_override_cb;
  cmd_request.ovrr_cmd.source  = request->source;
  cmd_request.ovrr_cmd.len_sig  = request->len_signature;

  if ( cmd_request.ovrr_cmd.len_sig == 0 )
  {
    response->err = E_SSM_OCH_DYNAMIC_ALLOCATION_FAILED;
    free ( RSP_ID( ssm_diag_och_request_temporary_override ) );
    return response;
  }

  cmd_request.ovrr_cmd.signature = ( uint8 * )malloc(cmd_request.ovrr_cmd.len_sig);
  cmd_request.ovrr_cmd.user_data = RSP_ID( ssm_diag_och_request_temporary_override ) ;

  if ( cmd_request.ovrr_cmd.signature == NULL )
  {
    response->err = E_SSM_OCH_FAILED;
    free ( RSP_ID( ssm_diag_och_request_temporary_override ) );
    return response;
  }

  /* Copy the key */
  if ( cmd_request.ovrr_cmd.len_sig != memscpy( cmd_request.ovrr_cmd.signature, 
	                            cmd_request.ovrr_cmd.len_sig,
	                            request->signature , cmd_request.ovrr_cmd.len_sig ) )
  {
    
    response->err = E_SSM_OCH_FAILED;
    free ( RSP_ID( ssm_diag_och_request_temporary_override ) );
    return response;
  }

  /* Submit request to SSM OCH */
  response->err = ssm_och_request( &cmd_request );

  if ( response->err != E_SSM_OCH_IN_PROGRESS )
  {
    free ( RSP_ID( ssm_diag_och_request_temporary_override ) );
  }

  /* Clear up buffer allocated for signature */
  secure_memset( cmd_request.ovrr_cmd.signature, 0 , cmd_request.ovrr_cmd.len_sig );
  /* Free up allocated space */
  free( cmd_request.ovrr_cmd.signature );

  return response;

} /* ssm_diag_och_request_temporary_override_handler */




		  
/**
Callback for Permanent Override

@param[in]  
  ssm_och_err_t         err    Depicts status of Command request 

@return 
None 

@dependencies
None

*/
static void ssm_diag_och_request_permanent_override_cb
(
  /** Error type : Depicts status of Command request */
  ssm_och_err_t         err,

  /* Length of return data */
  uint16                len_data,

  /** Output Data type */
  uint8 *               output_data,

  /** User data */
  ssm_och_cb_data_type user_data
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_DIAG_OCH_CB_PRECODE_DELAYED( 
		        ssm_diag_och_request_permanent_override,
		        SSM_DIAG_OCH_REQUEST_PERMANENT_OVERRIDE );
  
  response->err = err;

  diagpkt_delay_commit(response);
	
} /* ssm_diag_och_request_permanent_override_cb */



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
                  

@param[in]  
  req_ptr                   Pointer to the Packet command
  pkt len                   Length of the Packet

@return 
None 

@dependencies
None

*/

PACK(void *) ssm_diag_och_request_permanent_override_handler 
( 
  PACK(void *) req_ptr, 
  uint16 pkt_len
)
{
  SSM_DIAG_OCH_PRECODE_DELAYED( 
		        ssm_diag_och_request_permanent_override,
		        SSM_DIAG_OCH_REQUEST_PERMANENT_OVERRIDE );

  /* Command request structure */
  ssm_och_cmd_struct_t  cmd_request;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( pkt_len - offsetof(ssm_diag_och_request_permanent_override_req_type,
			 signature ) != request->len_signature )
  {
    free ( RSP_ID( ssm_diag_och_request_permanent_override ) );
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }


  /* Update SSM OCH Cmd structure to request permanent override */
  cmd_request.ovrr_cmd.cmd = SSM_OCH_PERM_OVERRIDE;
  cmd_request.ovrr_cmd.cb  = ssm_diag_och_request_permanent_override_cb;
  cmd_request.ovrr_cmd.source  = request->source;
  cmd_request.ovrr_cmd.len_sig  = request->len_signature;

  if ( cmd_request.ovrr_cmd.len_sig == 0 )
  {
    response->err = E_SSM_OCH_DYNAMIC_ALLOCATION_FAILED;
    free ( RSP_ID( ssm_diag_och_request_permanent_override ) );
    return response;
  }

  cmd_request.ovrr_cmd.user_data = RSP_ID( ssm_diag_och_request_permanent_override ) ;

  cmd_request.ovrr_cmd.signature = ( uint8 * )malloc(cmd_request.ovrr_cmd.len_sig);

  if ( cmd_request.ovrr_cmd.signature == NULL )
  {
    free ( RSP_ID( ssm_diag_och_request_permanent_override ) );
    response->err = E_SSM_OCH_FAILED;
    return response;
  }

  /* Copy the key */
  if ( cmd_request.ovrr_cmd.len_sig != memscpy( cmd_request.ovrr_cmd.signature, 
	                            cmd_request.ovrr_cmd.len_sig,
	                          request->signature , cmd_request.ovrr_cmd.len_sig ) )
  {
    free ( RSP_ID( ssm_diag_och_request_permanent_override ) );
    response->err = E_SSM_OCH_FAILED;
    return response;
  }

  /* Submit request to SSM OCH */
  response->err = ssm_och_request( &cmd_request );

  if ( response->err != E_SSM_OCH_IN_PROGRESS )
  {
    free ( RSP_ID( ssm_diag_och_request_permanent_override ) );
  }

  /* Clear up buffer allocated for signature */
  secure_memset( cmd_request.ovrr_cmd.signature, 0 , cmd_request.ovrr_cmd.len_sig );
  /* Free up allocated space */
  free( cmd_request.ovrr_cmd.signature );

  return response;


} /* ssm_diag_och_request_permanent_override_handler */



		  
/**
Callback for Cancel Override

@param[in]  
  ssm_och_err_t         err    Depicts status of Command request 

@return 
None 

@dependencies
None

*/
static void ssm_diag_och_cancel_override_cb
(
  /** Error type : Depicts status of Command request */
  ssm_och_err_t         err,

  /** Length of return data */
  uint16                len_data,

  /** Output Data type */
  uint8 *               output_data,

  /** User data */
  ssm_och_cb_data_type user_data
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_DIAG_OCH_CB_PRECODE_DELAYED( 
		        ssm_diag_och_cancel_override,
		        SSM_DIAG_OCH_CANCEL_OVERRIDE );
  
  response->err = err;

  diagpkt_delay_commit(response);
	
} /* ssm_diag_och_cancel_override_cb */



/**
Cancels Override 
 
The override command handler will delete the override cookie 
(if it exists) and will inform its clients.

This will be a Delayed Diag command response.

Command : send_data 128 0x56 0x0B 0x00 
Successful Immediate Response : <12 byte Diag Header > 0x01 0x00 0x00 0x00
Successful Delayed Response : <12 byte Diag Header > 0x00 0x00 0x00 0x00 
                  

@param[in]  
  req_ptr                   Pointer to the Packet command
  pkt len                   Length of the Packet

@return 
None 

@dependencies
None

*/

PACK(void *) ssm_diag_och_cancel_override_handler 
( 
  PACK(void *) req_ptr, 
  uint16 pkt_len
)
{
  SSM_DIAG_OCH_PRECODE_DELAYED_CANCEL( 
		        ssm_diag_och_cancel_override,
		        SSM_DIAG_OCH_CANCEL_OVERRIDE );

  /* Command request structure */
  ssm_och_cmd_struct_t  cmd_request;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update SSM OCH Cmd structure to add a Authentication Cookie */
  cmd_request.ovrr_cmd.cmd = SSM_OCH_CANCEL_OVERRIDE;
  cmd_request.ovrr_cmd.cb  = ssm_diag_och_cancel_override_cb;
  cmd_request.ovrr_cmd.signature = NULL;
  cmd_request.ovrr_cmd.user_data = RSP_ID( ssm_diag_och_cancel_override ) ;

  /* Submit request to SSM OCH */
  response->err = ssm_och_request( &cmd_request );

  if ( response->err != E_SSM_OCH_IN_PROGRESS )
  {
    free ( RSP_ID( ssm_diag_och_cancel_override ) );
  }
  
  return response;

} /* ssm_diag_och_cancel_override_handler */




/**
Function Dispatch table for Diag commands

@param[in]  
None

@return 
None 

@dependencies
None

*/


static const diagpkt_user_table_entry_type ssm_diag_och_tbl_delayed[] =
{
  { SSM_DIAG_OCH_REQUEST_TEMPORARY_OVERRIDE,
    SSM_DIAG_OCH_REQUEST_TEMPORARY_OVERRIDE,
    ssm_diag_och_request_temporary_override_handler },
  { SSM_DIAG_OCH_REQUEST_PERMANENT_OVERRIDE,
    SSM_DIAG_OCH_REQUEST_PERMANENT_OVERRIDE,
    ssm_diag_och_request_permanent_override_handler },
  { SSM_DIAG_OCH_CANCEL_OVERRIDE,
    SSM_DIAG_OCH_CANCEL_OVERRIDE,
    ssm_diag_och_cancel_override_handler }
};

static const diagpkt_user_table_entry_type ssm_diag_och_tbl[] =
{
  { SSM_DIAG_OCH_REQUEST_RANDOM_NONCE,
    SSM_DIAG_OCH_REQUEST_RANDOM_NONCE,
    ssm_diag_och_request_random_nonce_handler }
};




/**
Registers the diag packet function dispatch table.

@param[in]  
None

@return 
None 

@dependencies
None

*/


void ssm_diag_och_init (void )
{
  /* Could use the following registration without specifying the processor */
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_SSM, ssm_diag_och_tbl);

  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(DIAG_SUBSYS_CMD_VER_2_F,
                                           DIAG_SUBSYS_SSM,
                                           ssm_diag_och_tbl_delayed);
}

