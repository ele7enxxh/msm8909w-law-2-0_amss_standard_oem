#ifndef DS_EPC_HDLR_H
#define DS_EPC_HDLR_H
/*===========================================================================

                      DS_EPC_HDLR. H
                   
DESCRIPTION
  

EXTERNALIZED FUNCTIONS

 Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/epchandoff/inc/ds_epc_hdlr.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/27/09    sk    Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "datamodem_variation.h"
#ifdef FEATURE_EPC_HANDOFF
#include "ps_iface.h"
#include "ps_acl.h"
#include "sys.h"
#include "dstask_v.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
extern uint32  ds_epc_disable_flag; /* runtime disable EPC */

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================
FUNCTION      DS_EPC_HDLR_INIT

DESCRIPTION   Called at initialization. Creates the iface 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_hdlr_init(void);

/*===========================================================================
FUNCTION      DS_EPC_EHRPD_SYSTEM

DESCRIPTION   Called by ds707 Mode Handler for bearer tech changed to ehrpd
              system. 

DEPENDENCIES  None.

RETURN VALUE 

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_ehrpd_system(
  uint32  so_mask,
  uint32  rat_mask
);

/*===========================================================================
FUNCTION      DS_EPC_IS_HANDOFF_ALLOWED

DESCRIPTION   Called by physical MHs (LTE and eHRPD) to query if handoff is 
              possible. 

DEPENDENCIES  None.

RETURN VALUE 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_epc_is_handoff_allowed( 
  ps_iface_type *iface_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_HDLR_HANDOFF_INIT_CMD_PROCESS

DESCRIPTION   
	 
PARAMETERS    cmd_ptr   - Ptr to the command information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_hdlr_ds_cmd_process(
  ds_cmd_type *cmd_ptr                        
);

#endif /*FEATURE_EPC_HANDOFF*/
#endif /* DS_EPC_HDLR_H */
