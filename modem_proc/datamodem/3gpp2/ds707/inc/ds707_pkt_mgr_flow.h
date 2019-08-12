/*===========================================================================

                      DS707_PKT_MGR_FLOW

DESCRIPTION
  This file handles all commands and notification related to a PS Flow. If the
  command is for the default flow, the module handles it. If it is for a 
  secondary flow, then it routes it to the secondary packet manager.  

EXTERNALIZED FUNCTIONS
  ds707_pkt_flow_register_callback_func  

INITIALIZING AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_pkt_mgr_flow.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when      who    what, where, why
--------  ---    ----------------------------------------------------------
01/02/12  msh     Coan: Feature cleanup
11/21/05  sk     Updated clean up procedure
06/04/05  vas    Created module
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ps_flow.h"
#include "ds707_pkt_mgri.h"



/*===========================================================================
                            TYPEDEFS
===========================================================================*/

/*===========================================================================
                            VARIABLES
===========================================================================*/

/*===========================================================================

                               FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION     DS707_PKT_FLOW_REGISTER_CALLBACK_FUNC

DESCRIPTION

PARAMETERS
  flow_ptr - Pointer to the PS Flow to register callbacks for.

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds707_pkt_flow_register_callback_func
(
  ps_flow_type  *flow_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_FLOW_INIT

DESCRIPTION   Called once at mobile power-up. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_flow_init
(
  void
);

