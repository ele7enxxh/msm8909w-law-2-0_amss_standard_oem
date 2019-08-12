#ifndef DS707_SOCM_H
#define DS707_SOCM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       D A T A   S E R V I C E S

              S E R V I C E   O P T I O N   C O N T R O L

GENERAL DESCRIPTION
  This file contains a handler function to parse and process service option
  control messages related to data service options.

EXTERNALIZED FUNCTIONS
  ds707_socm_parse_socm
    Called when a SOCM (service option control message) is received from
    the base station.  Function parses SOCM to see if it has info regarding
    either PZID or the dormant (holddown) timer. 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2002 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_socm.h_v   1.1   19 Nov 2002 18:41:40   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_socm.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/02   ak      Updated file header comments.

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "snm_v.h"

/*===========================================================================
              DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/


/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SOCM_PARSE_SOCM

DESCRIPTION  This function processes service option control messages related
             to data service options. Currently, the only Service Option
             Control Message that is supported is for Packet Data Dormant
             Timer control.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
snm_socm_return_type ds707_socm_parse_socm
(
  word  length,          /* Length of the type-specific fields (in bytes)  */
  byte *soctl_msg,       /* Pointer to the type-specific fields            */
  word  service_option   /* The service option that is currently connected */
);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_SOCM_H       */
