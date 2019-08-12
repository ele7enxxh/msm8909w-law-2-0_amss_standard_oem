#ifndef DS_EPC_IOTCL_H
#define DS_EPC_IOTCL_H
/*===========================================================================

                      DS_EPC_IOTCL. H
                   
DESCRIPTION
  DS EPC IOCTL module.
 
  This module handles DS EPC IOCTL.

EXTERNALIZED FUNCTIONS

 Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: 
  $Author: 

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/19/11    sc    Created Module
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "datamodem_variation.h"
#ifdef FEATURE_EPC_HANDOFF
#include "sys.h"
#include "rex.h"

#include "ps_iface_defs.h"
#include "ps_flow_ioctl.h"
#include "ps_iface_ioctl.h"


/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_EPC_FLOW_IOCTL

DESCRIPTION   Flow specific IOCTL processing

DEPENDENCIES  None.

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None
===========================================================================*/
int ds_epc_flow_ioctl
(
  ps_flow_type         * ps_flow_ptr,
  ps_flow_ioctl_type     ioctl_name,
  void                 * argval_ptr,
  int16                * ps_errno
);

/*===========================================================================
FUNCTION      DS_EPC_IFACE_IOCTL_HDLR

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

===========================================================================*/
int ds_epc_iface_ioctl_hdlr
(
  ps_iface_type          *iface_ptr,
  ps_iface_ioctl_type     ioctl_name,
  void                   *argval_ptr,
  sint15                 *ps_errno
);

#endif /*FEATURE_EPC_HANDOFF*/
#endif /* DS_EPC_IOTCL_H */
