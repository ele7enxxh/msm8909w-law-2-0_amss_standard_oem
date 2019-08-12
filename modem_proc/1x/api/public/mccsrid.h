#ifndef MCCSRID_H
#define MCCSRID_H

/*===========================================================================

          MAIN CONTROL SRID MANAGER

GENERAL DESCRIPTION
  This task manages the SRID.

DESCRIPTION
   This module manages the SRID resource for the mobile.

  Copyright (c) 2001 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccsrid.h_v   1.4   28 Jun 2002 12:50:26   lhang  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/mccsrid.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/09   jj      Meta-Comments cleanup for API's exported to DS.
05/14/09   jj      Split this file as part of CMI phase2.
11/21/06   sb      Added support for VOIP-1X handoffs.
06/23/06   pg      Added support for RPC.
03/28/05   fc      Made FCS also available to Rel D.
03/12/04   fc      Added support for Release C fast call setup.
02/07/02   lh      Added mccsrid_free_all_owned_srids().
01/08/02   lh      Function name change.
08/28/01   lh      Bug fixes.
05/14/01   lh      Initial revision of the file.
===========================================================================*/

#include "comdef.h"
#include "cmd.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS 

===========================================================================*/


/* Type to identify the ownership of an SR_ID */
typedef enum 
{
  SRID_OWNER_NULL,      /* Reserved for the BS */
  SRID_OWNER_MC,        /* Owner ID used by MC */
  SRID_OWNER_DS,        /* Owner ID used by DS */
  SRID_OWNER_RESERVED   /* Owner ID reserved, do not use */
} mccsrid_srid_owner_type;


/* Error code returned by mccsrid_allocate_srid() if no available SR_ID */
#define MCCSRID_INVALID_SRID 0xFF
/* <EJECT> */
/*===========================================================================

FUNCTION mccsrid_allocate_srid

DESCRIPTION
  This function returns the lowest unused SR_ID number to the caller.

DEPENDENCIES
  None

RETURN VALUE
  SR_ID number [1, MCCSRID_SRID_NUM_MAX-1], if a SR_ID is available. 
  Returns MCCSRID_INVALID_SRID if none of the SRID is available.

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.
===========================================================================*/
extern byte mccsrid_allocate_srid
(
  mccsrid_srid_owner_type owner
    /* Owner ID of the allocation request */
);


/* <EJECT> */


#endif /* MCCSRID_H */

