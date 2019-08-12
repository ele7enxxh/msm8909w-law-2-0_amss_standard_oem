#ifndef PS_LOGGING_DIAG_H
#define PS_LOGGING_DIAG_H
/*===========================================================================

                        P S _ L O G G I N G  _ D I A G . H

GENERAL DESCRIPTION
  The file defines diag sub system commands and regesters the request handler
  callback functions at powerup. This contains diag specific data. 

EXTERNALIZED FUNCTIONS
 dpl_init()
   Registers the functions(dpl request handlers) with DIAG.
   
 Copyright (c) 2004 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ps_logging_diag.h#1 $
 $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
  
when        who     what, where, why
--------    ---     --------------------------------------------------------- 
11/30/11    su     Adding log code support for DPL logging
01/12/05    msr    Added code review comments.
01/10/05    msr    Added support for flow and link logging.
11/2/04    ks/msr   Changed Diag cmd handler code's names.
09/27/04   ks/msr   Major cleanup.
07/29/04    vd      Created the file. 
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "customer.h"
#include "ps_logging_defs.h"


#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
 
                      DIAG REQUEST / RESPONSE STRUCTURES 

===========================================================================*/
/*---------------------------------------------------------------------------
  NOTE : DO NOT compile in these structures. These are for informational
  purposes only.
---------------------------------------------------------------------------*/ 
/*---------------------------------------------------------------------------
struct dpl_reset_logging_req
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
}

struct dpl_reset_logging_resp
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
}

struct dpl_get_supported_ifaces_req
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
}

struct dpl_get_supported_ifaces_resp
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
  uint8   num_ifaces;
  uint8   iface_id[num_ifaces];
}

struct dpl_get_iface_desc_req
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
  uint8   iface_id;
}

struct dpl_get_iface_desc_resp
{
  uint8     command_code;
  uint8     subsys_id;
  uint16    subsys_command_code;
  uint8     iface_id;
  char    * iface_desc; // Can at most be 32 bytes and is null termnated
  uint8     num_links;
  char    * link_desc[num_links]; // Can at most be 32 bytes and is null
                                  // termnated
  uint8     num_flows;
  char    * flow_desc[num_flows]; // Can at most be 32 bytes and is null
                                  // termnated
}

struct dpl_set_iface_logging_req
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
  uint8   iface_id;
  uint8   num_log_flags;

  struct
  {
    uint32  iid;
    uint32  snaplen;
  } log_flag[num_log_flags];
}

struct dpl_set_iface_logging_resp
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
  uint8   iface_id;
  uint8   num_log_flags;

  struct
  {
    uint32  iid;
    uint32  link_type;
  } log_flag[num_log_flags];
}

struct dpl_get_supported_protocols_req
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
}

struct dpl_get_supported_protocols_resp
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
  uintt8  num_nw_protocols; // Number of supported network level protocols
  uint8   protocol[num_nw_protocols];
}
---------------------------------------------------------------------------*/


/*===========================================================================
 
                                  EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DPL_INIT

DESCRIPTION
  Registers functions with DIAG.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dpl_init
(
  void
);

/*===========================================================================
FUNCTION DPL_GET_DPL_STATE
 
DESCRIPTION
  This method will return current DPL state
   
RETURN VALUE
     None
 
DEPENDENCIES
  	 None 
  
SIDE EFFECTS
    None
===========================================================================*/
dpl_state_enum_type dpl_get_dpl_state
(
  void
);

/*===========================================================================
FUNCTION dpli_update_logging_config

DESCRIPTION
  This method enables the logging in iface, sets protocol and packet
  log length
 
PARAMETERS
  ps_iface_ptr[in]   Pointer to the interface 
   
RETURN VALUE
    None
 
DEPENDENCIES
    None 
  
SIDE EFFECTS
    None
===========================================================================*/
 void dpli_update_logging_config
(
  ps_iface_type       *ps_iface_ptr,
  dpl_state_enum_type  dpl_state
);

#ifdef __cplusplus
}
#endif

#endif /* PS_LOGGING_DIAG_H */
