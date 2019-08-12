/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              SSM Diagnostics Packet Processing
           S S M   D I A G   O C H   F R A M E W O R K 

General Description
  Diagnostic packet processing routines for SSM operations.

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_diag_ach.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
4/16/13    din     Removed warnings.
11/05/12   din     File created.

===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "diagcmd.h"
#include "diagpkt.h"
#include "ssm_v.h"
#include "ssm_diag_ach_v.h"
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

static int ssm_diag_ach_is_valid_pkt_len 
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

#define SSM_DIAG_ACH_PRECODE_DELAYED( A, B )  \
A##_rsp_type *response; \
uint16 rsp_len = sizeof (A##_rsp_type); \
if (! ssm_diag_ach_is_valid_pkt_len (pkt_len, \
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




#define SSM_DIAG_ACH_CB_PRECODE_DELAYED( A, B )  \
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
Callback for Add Authentication Cookie

@param[in]  
  ssm_ach_err_t         err    Depicts status of Command request 

@return 
None 

@dependencies
None

*/
static void ssm_diag_ach_provision_authentication_cookie_cb
(
  /** Error type : Depicts status of Command request */
  ssm_ach_err_t         err,

  /** Data returned */
  ssm_ach_cb_data_type user_data
  
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_DIAG_ACH_CB_PRECODE_DELAYED( 
		        ssm_diag_ach_provision_authentication_cookie,
		        SSM_DIAG_ACH_PROVISION_AUTHENTICATION_COOKIE );
  
  response->err = err;

  diagpkt_delay_commit(response);
	
} /* ssm_diag_ach_provision_authentication_cookie_cb */




/**
Provisions Authentication Cookie
 
The authentication cookie indicates the state of the device. If the 
cookie is missing from SFS, it indicates that the device was still in 
the factory. The absence of this cookie will mean that is enabled 
and that the override keys can be provisioned. 
The authentication cookie can only be written once.

This will be a Delayed Diag command response.

Command : send_data 128 0x56 0x05 0x00
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

PACK(void *) ssm_diag_ach_provision_authentication_cookie_handler 
( 
  PACK(void *) req_ptr, 
  uint16 pkt_len
)
{
  SSM_DIAG_ACH_PRECODE_DELAYED ( 
		        ssm_diag_ach_provision_authentication_cookie,
		        SSM_DIAG_ACH_PROVISION_AUTHENTICATION_COOKIE );

  /* Command request structure */
  ssm_ach_cmd_struct_t  cmd_request;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update SSM ACH Cmd structure to add a Authentication Cookie */
  cmd_request.cmd = SSM_ACH_ADD_AUTHENTICATION_COOKIE;
  cmd_request.cb  = ssm_diag_ach_provision_authentication_cookie_cb;
  cmd_request.user_data  = RSP_ID( ssm_diag_ach_provision_authentication_cookie );

  /* Submit request to SSM ACH */
  response->err = ssm_ach_request( &cmd_request );

  if ( response->err != E_SSM_ACH_IN_PROGRESS )
  {
    free ( RSP_ID( ssm_diag_ach_provision_authentication_cookie ) );
  }

  return response;

} /* ssm_diag_ach_provision_authentication_cookie_handler */






/**
Function Dispatch table for Diag commands

@param[in]  
None

@return 
None 

@dependencies
None

*/


static const diagpkt_user_table_entry_type ssm_diag_ach_tbl_delayed[] =
{
  { SSM_DIAG_ACH_PROVISION_AUTHENTICATION_COOKIE,
    SSM_DIAG_ACH_PROVISION_AUTHENTICATION_COOKIE,
    ssm_diag_ach_provision_authentication_cookie_handler },
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


void ssm_diag_ach_init (void )
{
  /* Could use the following registration without specifying the processor */
  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(DIAG_SUBSYS_CMD_VER_2_F,
                                           DIAG_SUBSYS_SSM,
                                           ssm_diag_ach_tbl_delayed);
}

