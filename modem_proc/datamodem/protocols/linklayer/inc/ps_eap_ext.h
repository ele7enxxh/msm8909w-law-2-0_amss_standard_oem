#ifndef PS_EAP_EXT_H
#define PS_EAP_EXT_H

/*===========================================================================


   E X T E N S I B L E   A U T H E N T I C A T I O N   P R O T O C O L (EAP)
                       E X T E N D E D   M E T H O D
                       A P I   H E A D E R   F I L E
                   
DESCRIPTION


 The EAP API header file.


  Copyright (c) 2005-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_eap_ext.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/26/12    fn     Changed EXT_MAX_INST to match EAP_MAX_INSTANCES
10/06/11    scb    Number of EAP ext instances increased by 1 for ATCOP
04/24/05    lti    Created module

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"

#include "customer.h"
#ifdef FEATURE_DATA_PS_EAP
#include "dsm.h"
#include "ps_eapi.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*Match the number of extended instances with the number of EAP instances*/
#define EAP_EXT_MAX_INST  EAP_MAX_INSTANCES

/* Expanded NAK type */
#define EAP_EXT_NAK_TYPE  3

typedef struct
{
  eap_handle_type                    eap_handle;
  boolean                            vendor_info_set;
  eap_ext_vendor_id_meta_info_type   vendor_id_info;
  eap_role_enum_type                 eap_role;
}eap_ext_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION EAP_EXT_INIT

DESCRIPTION
  Initialize the EAP-EXT software entity.

DEPENDENCIES
  None

PARAMETERS
  None.
RETURN VALUE
  None

SIDE EFFECTS
  Register for EAP_EXT packets over EAP transport.
  
===========================================================================*/
void 
eap_ext_init
(
  void
);

/*===========================================================================
FUNCTION EAP_EXT_CREATE_INST

DESCRIPTION
  Creates an eap extended instance.
   
DEPENDENCIES
  None

PARAMETERS
  eap_handle - handle of the current EAP instance
  
RETURN VALUE
  TRUE if successful

SIDE EFFECTS
  None
  
===========================================================================*/
boolean
eap_ext_create_inst
(
  eap_handle_type    eap_handle
);

/*===========================================================================
FUNCTION EAP_EXT_DELETE_INST

DESCRIPTION
  Deletes an eap extended instance.
   
DEPENDENCIES
  None

PARAMETERS
  eap_handle - handle of the current EAP instance
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void
eap_ext_delete_instance
(
  eap_handle_type    eap_handle
);

#endif /* FEATURE_DATA_PS_EAP */
#endif  /* PS_EAP_EXT_H */
