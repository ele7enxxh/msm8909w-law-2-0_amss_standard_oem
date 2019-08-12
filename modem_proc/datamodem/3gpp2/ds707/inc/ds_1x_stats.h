#ifndef DS_1X_STATS_H
#define DS_1X_STATS_H
/*===========================================================================

                        D S 1 X _ S T A T S 
GENERAL DESCRIPTION
  This file  has functions for reporting  data bearer statistics. 
  These functions are called from  handlers for following ioctls:
  PS_IFACE_IOCTL_GET_DATA_BEARER_RATE:
  PS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY:

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
 None

 Copyright (c) 2005-2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.h_v   1.4   27 Nov 2002 11:02:18   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds_1x_stats.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
-----------------------------------------------------------------------------  
when        who    what, where, why
--------    ---    -------------------------------------------------------
03/04/10    spn    Updated the Data Bearer Indications
09/01/05    kvd    First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "ps_iface_defs.h"

/*===========================================================================
                            VARIABLES
===========================================================================*/
/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION: ds_1x_get_bearer_data_stats

DESCRIPTION:
  Update Data Call Statistics in  stats structure passed.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

===========================================================================*/

boolean ds_1x_get_bearer_data_stats
(
  ps_iface_ioctl_data_bearer_rate *data_stats
);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS_1X_STATS_H    */

