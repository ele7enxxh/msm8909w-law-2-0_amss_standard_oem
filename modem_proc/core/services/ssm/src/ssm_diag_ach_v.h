#ifndef SSM_DIAG_ACH_V_H
#define SSM_DIAG_ACH_V_H
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

  $Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_diag_ach_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/12   din     File created.

===========================================================================*/


#include "ssm_diag_cmd_v.h"
#include "ssm_ach_v.h"


/* -------------------------------------------------------------------------
** Packet Definitions
** ------------------------------------------------------------------------- */



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

*/

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
} ssm_diag_ach_provision_authentication_cookie_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_v2_type hdr;
  ssm_ach_err_t                 err;
} ssm_diag_ach_provision_authentication_cookie_rsp_type;


/**
Removes Authentication Cookie
 
ACH Authenitcation cookie removal functionality is only intended for testing 
and will be featured out in CRM builds.

This will be a Delayed Diag command response.

Command : send_data 128 0x56 0x0C 0x00 
Successful Immediate Response : <12 byte Diag Header > 0x01 0x00 0x00 0x00
Successful Delayed Response : <12 byte Diag Header > 0x00 0x00 0x00 0x00 

*/

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_type hdr;
} ssm_diag_ach_remove_authentication_cookie_req_type;

typedef PACKED struct PACKED_POST {
  diagpkt_subsys_header_v2_type hdr;
  ssm_ach_err_t                 err;
} ssm_diag_ach_remove_authentication_cookie_rsp_type;


void ssm_diag_ach_init( void );

#endif /* SSM_DIAG_ACH_V_H */
