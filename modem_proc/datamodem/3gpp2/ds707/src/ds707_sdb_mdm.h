#ifndef DSHDR_SDB_MDM
#define DSHDR_SDB_MDM

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S   

      S H O R T   D A T A   B U R S T   M O D E M   S I D E   S U P P O R T

GENERAL DESCRIPTION
  This file contains functions used for implementing MT-DOS data path

EXTERNALIZED FUNCTIONS
  DSHDR_DOS_MDM_RX_HANDLER
    Processes MT-DOS packets and send them to the APPS processor for further
    routing to higher layers
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None

   Copyright (c) 2007 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
   
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_sdb_mdm.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/09   ms      Offtarget Lint fixes.
04/17/07   ssr     New file: contains code that implements MT-DOS data path
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#if defined(FEATURE_HDR_DOS) && defined (FEATURE_HDR_DOS)

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/


/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      ds707_sdb_mdm_rx_handler

DESCRIPTION   Redirects received DoS packets back from the APPS processor, 
              to the SHIM layer, for rerouting to APPS and subsequent 
              processing by higher layers
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sdb_mdm_rx_handler
(
  void *dsm_payload_ptr,
  uint32 rlp_flow,
  byte   route
);

#endif /* defined(FEATURE_HDR_DOS) && defined (FEATURE_HDR_DOS) */

#endif /* DSHDR_SDB_MDM */
