#ifndef SSM_DIAG_KEY_HANDLER_V_H
#define SSM_DIAG_KEY_HANDLER_V_H
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

  $Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_diag_key_handler_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/12   din     File created.

===========================================================================*/



#include "ssm_diag_cmd_v.h"
#include "ssm_key_handler_v.h"


/* -------------------------------------------------------------------------
** Packet Definitions
** ------------------------------------------------------------------------- */


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

*/

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  /* Key usage field */
  uint32                     usage;
  /* Crypto Algo that the key will be used for */
  uint32                     crypto_algo;
  /* Indicates key to be provisioned, 0x00000000 denotes Qualcomm  */ 
  uint32                     source;
  /* 128-bit serial number field which can be used to identify the key from the 
   * complete set that the owner has ever used. */
  uint8                      serial_num[16];
  /* Length of key */
  uint16                     len_key;
  /* Variable-length key field  */
  uint8                       key[1];          
} ssm_diag_key_handler_provision_key_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_v2_type hdr;
  ssm_key_handler_err_t         err;
} ssm_diag_key_handler_provision_key_rsp_type;


/**

Reads key
 
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
                              
 
*/

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
  /* Key usage field */
  uint32                     usage;
  /* Crypto Algo that the key will be used for */
  uint32                     crypto_algo;
  /* Indicates key to be provisioned, 0x00000000 denotes Qualcomm  */ 
  uint32                     source;
} ssm_diag_key_handler_read_key_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_v2_type hdr;
  ssm_key_handler_err_t         err;
  /* Key usage field */
  uint32                        usage;
  /* Crypto Algo that the key will be used for */
  uint32                        crypto_algo;
  /* Indicates key to be provisioned, 0x00000000 denotes Qualcomm  */ 
  uint32                        source;
  /* 128-bit serial number field which can be used to identify the key from the 
   * complete set that the owner has ever used. */
  uint8                         serial_num[16];

} ssm_diag_key_handler_read_key_rsp_type;




/**
Removes key
 
Removes the entire SFS file.
OCH Override Key removal functionality is only intended for testing and 
will be featured out in CRM builds.

This will be a Delayed Diag command response.

Command : send_data 128 0x56 0x0D 0x00 <32 bit usage > <32 bit crypto_algo >
                                       <32 bit source >
Successful Immediate Response : <12 byte Diag Header > 0x01 0x00 0x00 0x00
Successful Delayed Response : <12 byte Diag Header > 0x00 0x00 0x00 0x00 

*/

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
   /* Key usage field */
  uint32                     usage;
  /* Crypto Algo that the key will be used for */
  uint32                     crypto_algo;
  /* Indicates key to be provisioned, 0x00000000 denotes Qualcomm  */ 
  uint32                     source;
 
} ssm_diag_key_handler_remove_key_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_v2_type hdr;
  ssm_key_handler_err_t         err;
} ssm_diag_key_handler_remove_key_rsp_type;


void ssm_diag_key_handler_init( void );
void ssm_diag_key_handler_test_init( void );

#endif /* SSM_DIAG_KEY_HANDLER_V_H */
