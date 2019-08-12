#ifndef DSWPSD_H
#define DSWPSD_H
/*===========================================================================

               W C D M A   P A C K E T - S W I T C H E D   D A T A

                     E X T E R N A L   H E A D E R   F I L E

  DESCRIPTION
  This file contains external data declarations and function prototypes for 
  UMTS (wcdma / gprs) Packet-Switched Data Services.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/dswpsd.h#1 $DateTime: 2009/04/20 14:38:27 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/09   vs      Removed the inclusion of customer.h
10/15/09   vs      SU Level Api Defeaturization
08/31/09   ss      New header file for SU level apis.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "uecomdef.h"
#include "dsumtspdpreg.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATION

===========================================================================*/

/*===========================================================================
FUNCTION      DSWPSD_GET_TRAFFIC_CLASS

DESCRIPTION   
  This function returns Traffic Class for given context.

PARAMETERS   
  rlc_dl_id - DL LC id for context .
  
DEPENDENCIES  
  None.

RETURN VALUE  
  ip_traffic_class_enum_type -- DL Traffic Class for the context.

SIDE EFFECTS  
  None.
===========================================================================*/
ds_umts_qos_tclass_enum_type  dswpsd_get_traffic_class
(
  rlc_lc_id_type rlc_dl_id
);

#endif /* DSWPSD_H */
