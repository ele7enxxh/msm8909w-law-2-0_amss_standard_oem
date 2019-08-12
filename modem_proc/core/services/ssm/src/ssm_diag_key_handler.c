/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              SSM Diagnostics Packet Processing
        S S M   D I A G   K E Y  H A N D L E R   F R A M E W O R K 

General Description
  Diagnostic packet processing routines for SSM operations.

Copyright (c) 2012 - 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_diag_key_handler.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/12   din     File created.

===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "ssm_v.h"
#include "ssm_diag_key_handler_v.h"
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


/**********************************************************************
 * Packet length validation.
 *********************************************************************/

static int ssm_diag_key_handler_is_valid_pkt_len 
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

#define SSM_DIAG_KEY_HANDLER_PRECODE_DELAYED( A, B )  \
A##_req_type *request; \
A##_rsp_type *response; \
uint16 rsp_len = sizeof (A##_rsp_type); \
if (! ssm_diag_key_handler_is_valid_pkt_len (pkt_len, \
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





#define SSM_DIAG_KEY_HANDLER_CB_PRECODE_DELAYED( A, B )  \
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


#define SSM_LEN_SERIAL_NUM   16




/**
Callback for Provision key

@param[in]  
  ssm_key_handler_err_t         err    Depicts status of key handler request 
  ssm_key_handler_req_t         req    SSM Key Handler Operation
  ssm_key_handler_info_t        *info   SSM Key Handler info
  uint8                         *key   SSM Key Handler key
  

@return 
None 

@dependencies
None

*/
static void ssm_diag_key_handler_provision_key_cb
(
  /** Error type : Depicts status of key handler request */
  ssm_key_handler_err_t         err,

  /** SSM Key Handler Operation */
  ssm_key_handler_req_t         req,

  /** SSM Key Handler info */
  ssm_key_handler_info_t        *info,

  /** SSM Key Handler key */
  uint8                         *key,

  /** SSM key Handler cb data */
  ssm_key_handler_cb_data_type   user_data
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_DIAG_KEY_HANDLER_CB_PRECODE_DELAYED( 
		        ssm_diag_key_handler_provision_key,
		        SSM_DIAG_KEY_HANDLER_PROVISION_KEY );
  
  response->err = err;

  diagpkt_delay_commit(response);
	
} /* ssm_diag_key_handler_provision_key_cb */




/**
Provisions key

Key provisioning should take place in the factory. Keys can be provisioned 
on the device up to the point at which the authentication cookie is 
written. After the authentication cookie is written, the keys
cannot be written, but can still be read.

This will be a Delayed Diag command response.

Command : send_data 128 0x56 0x06 0x00 <32 bit usage > <32 bit crypto_algo > 
          <32 bit source > <128 bit serial_num >
          <16 bit len_key > <key >
Successful Immediate Response : <12 byte Diag Header > 0x01 0x00 0x00 0x00
Successful Delayed Response :<12 byte Diag Header > 0x00 0x00 0x00 0x00

@param[in]  
  req_ptr                   Pointer to the Packet command
  pkt len                   Length of the Packet

@return 
None 

@dependencies
None

*/

PACK(void *) ssm_diag_key_handler_provision_key_handler 
( 
  PACK(void *) req_ptr, 
  uint16 pkt_len
)
{
  SSM_DIAG_KEY_HANDLER_PRECODE_DELAYED( 
		        ssm_diag_key_handler_provision_key,
		        SSM_DIAG_KEY_HANDLER_PROVISION_KEY );


  /* Key handler request structure */
  ssm_key_handler_struct_t  key_request;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check for status of Authentication Cookie and dont allow the operation if 
   * cookie present */

  if ( pkt_len - offsetof(ssm_diag_key_handler_provision_key_req_type,
			  key ) != request->len_key )
  {
    free ( RSP_ID( ssm_diag_key_handler_provision_key ) );
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  
  
  /* Update SSM Key Handler Key structure to add a key */
  key_request.req = SSM_KEY_HANDLER_ADD_KEY;
  key_request.cb  = ssm_diag_key_handler_provision_key_cb;
  key_request.info.usage = request->usage ;
  key_request.info.source = request->source ;
  key_request.info.crypto_algo = request->crypto_algo ; 
  key_request.info.len_key  = request->len_key ;
  key_request.user_data = RSP_ID( ssm_diag_key_handler_provision_key ) ;
  
  if ( SSM_LEN_SERIAL_NUM != memscpy( key_request.info.serial_num, 
	 SSM_LEN_SERIAL_NUM, request->serial_num, SSM_LEN_SERIAL_NUM ) )
  {
    response->err = E_SSM_KEY_HANDLER_FAILED;
    free ( RSP_ID( ssm_diag_key_handler_provision_key ) );
    return response;
  }

  if ( key_request.info.len_key == 0 )
  {
    response->err = E_SSM_KEY_HANDLER_DYNAMIC_ALLOCATION_FAILED;
    free ( RSP_ID( ssm_diag_key_handler_provision_key ) );
    return response;
  }

  key_request.key = ( uint8 * )malloc(key_request.info.len_key );

  if ( key_request.key == NULL )
  {
    response->err = E_SSM_KEY_HANDLER_FAILED;
    free ( RSP_ID( ssm_diag_key_handler_provision_key ) );
    return response;
  }

  /* Copy the key */
  if ( key_request.info.len_key != memscpy( key_request.key, key_request.info.len_key, request->key, 
	  key_request.info.len_key ))
  {
    response->err = E_SSM_KEY_HANDLER_FAILED;
    free ( RSP_ID( ssm_diag_key_handler_provision_key ) );
    return response;
  }

  /* Submit request to Key Handler */
  response->err = ssm_key_handler_request( &key_request );

  if ( response->err != E_SSM_KEY_HANDLER_IN_PROGRESS )
  {
    free ( RSP_ID( ssm_diag_key_handler_provision_key ) );
  }

  secure_memset ( key_request.key, 0 , key_request.info.len_key);
  free ( key_request.key );

  return response;

} /* ssm_diag_key_handler_provision_key_handler */



/**
Callback for Read key

@param[in]  
  ssm_key_handler_err_t         err    Depicts status of key handler request 
  ssm_key_handler_req_t         req    SSM Key Handler Operation
  ssm_key_handler_info_t        *info   SSM Key Handler info
  uint8                         *key   SSM Key Handler key
  

@return 
None 

@dependencies
None

*/
static void ssm_diag_key_handler_read_key_cb
(
  /** Error type : Depicts status of key handler request */
  ssm_key_handler_err_t         err,

  /** SSM Key Handler Operation */
  ssm_key_handler_req_t         req,

  /** SSM Key Handler info */
  ssm_key_handler_info_t        *info,

  /** SSM Key Handler key */
  uint8                         *key,

  /** SSM key Handler cb data */
  ssm_key_handler_cb_data_type   user_data
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_DIAG_KEY_HANDLER_CB_PRECODE_DELAYED( 
		        ssm_diag_key_handler_read_key,
		        SSM_DIAG_KEY_HANDLER_READ_KEY);
  
  if ( info == NULL )
  {
    if ( err == E_SSM_KEY_HANDLER_SUCCESS )
      response->err = E_SSM_KEY_HANDLER_FAILED;
    else
      response->err = err;
    diagpkt_delay_commit(response);

    return;
  }
  
  response->err = err;

  /* Populate response packet */
  if ( err == E_SSM_KEY_HANDLER_SUCCESS )
  {
    response->usage = info->usage;
    response->crypto_algo = info->crypto_algo;
    response->source = info->source;
    
    memscpy( response->serial_num, 
	 SSM_LEN_SERIAL_NUM, info->serial_num, SSM_LEN_SERIAL_NUM );
  }

  diagpkt_delay_commit(response);

} /* ssm_diag_key_handler_read_key_cb */

/** Reads key
 
Key provisioning should take place in the factory. Keys can be provisioned 
on the device up to the point at which the authentication cookie is 
written. After the authentication cookie is written, the override keys
cannot be written, but can still be read.

This will be a Delayed Diag command response.

Command : send_data 128 0x56 0x07 0x00 <32 bit usage > <32 bit crypto_algo >
                                       <32 bit source >
Successful Immediate Response : <12 byte Diag Header > 0x01 0x00 0x00 0x00
Successful Delayed Response : <12 byte Diag Header > 0x00 0x00 0x00 0x00 
                              <32 bit usage > <32 bit crypto_algo > 
                              <32 bit source> <128 bit serial_num >

@param[in]  
  req_ptr                   Pointer to the Packet command
  pkt len                   Length of the Packet

@return 
None 

@dependencies
None

*/

PACK(void *) ssm_diag_key_handler_read_key_handler 
( 
  PACK(void *) req_ptr, 
  uint16 pkt_len
)
{
  SSM_DIAG_KEY_HANDLER_PRECODE_DELAYED( 
		        ssm_diag_key_handler_read_key,
		        SSM_DIAG_KEY_HANDLER_READ_KEY );

  /* Key handler request structure */
  ssm_key_handler_struct_t  key_request;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update SSM Key Handler Key structure to get a key */
  key_request.req = SSM_KEY_HANDLER_GET_KEY;
  key_request.cb  = ssm_diag_key_handler_read_key_cb;
  key_request.info.usage = request->usage ;
  key_request.info.source = request->source ;
  key_request.info.crypto_algo = request->crypto_algo ; 
  key_request.info.len_key  = 0;
  key_request.user_data = RSP_ID( ssm_diag_key_handler_read_key ) ;
  key_request.key = NULL;
  
  /* Submit the request to Key Handler */
  response->err = ssm_key_handler_request( &key_request );

  if ( response->err != E_SSM_KEY_HANDLER_IN_PROGRESS )
  {
    free ( RSP_ID( ssm_diag_key_handler_read_key ) );
  }

  return response;

} /* ssm_diag_key_handler_read_key_handler */




/**
Function Dispatch table for Diag commands

@param[in]  
None

@return 
None 

@dependencies
None

*/


static const diagpkt_user_table_entry_type ssm_diag_key_handler_tbl_delayed[] =
{
  { SSM_DIAG_KEY_HANDLER_PROVISION_KEY,
    SSM_DIAG_KEY_HANDLER_PROVISION_KEY,
    ssm_diag_key_handler_provision_key_handler },
  { SSM_DIAG_KEY_HANDLER_READ_KEY,
    SSM_DIAG_KEY_HANDLER_READ_KEY,
    ssm_diag_key_handler_read_key_handler }
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


void ssm_diag_key_handler_init (void )
{
  /* Could use the following registration without specifying the processor */
  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(DIAG_SUBSYS_CMD_VER_2_F,
                                           DIAG_SUBSYS_SSM,
                                           ssm_diag_key_handler_tbl_delayed);
}

