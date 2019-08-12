#ifndef PS_EAP_INIT_H
#define PS_EAP_INIT_H


/*===========================================================================


   E X T E N S I B L E   A U T H E N T I C A T I O N   P R O T O C O L (EAP)
                      I N I T  H E A D E R   F I L E
                   
DESCRIPTION


 The EAP Initalization header file.


Copyright (c) 2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_eap_init.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/19/08    pp     Created module as part of Common Modem Interface:
                   Public/Private split.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"

#include "customer.h"
#ifdef FEATURE_DATA_PS_EAP

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION EAP_SVC_INIT

DESCRIPTION
  Initializes the EAP control blocks.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Initialize the EAP control blocks.

===========================================================================*/
void eap_svc_init
(
  void
);

#endif /* FEATURE_DATA_PS_EAP */
#endif  /* PS_EAP_H */
