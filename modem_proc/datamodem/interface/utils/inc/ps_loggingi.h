#ifndef PS_LOGGINGI_H
#define PS_LOGGINGI_H
/*===========================================================================

               P S _ L O G G I N G I . H

GENERAL DESCRIPTION
  This file defines the accessor functions to manipulate lookup table
  maintained by ps_logging.c

EXTERNAL FUNCTIONS
  dpli_get_ps_iface_default_desc()
    Get description from lookup table given an IID ifname

  dpli_set_ps_iface_ptr()
    Set ps_iface ptr for specified ifname in the lookup table

 Copyright (c) 2004 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ps_loggingi.h#1 $
 $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/31/04   ks/msr  Created file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include"customer.h"
#include "comdef.h"
#include "customer.h"
#include "ps_iface_defs.h"
#include "ps_logging_defs.h"



/*===========================================================================

                          EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION    DPLI_SET_PS_IFACE_PTR

DESCRIPTION
  Set ps_iface_ptr in the lookup table

PARAMETERS
  ps_iface_ptr : pointer to ps_iface structure
  ifname       : IID iface ID

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dpli_set_ps_iface_ptr
(
  ps_iface_type           * ps_iface_ptr,
  dpl_iid_ifname_enum_type  ifname
);



/*===========================================================================
FUNCTION    DPLI_GET_PS_IFACE_DEFAULT_DESC

DESCRIPTION
  Get default description for this ifname from the lookup table

PARAMETERS
  ifname : IID ifname

RETURN VALUE
  Description of the iface corresponding to this ifname

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
const char *dpli_get_ps_iface_default_desc
(
  dpl_iid_ifname_enum_type  ifname
);

#endif /* PS_LOGGINGI_H */
