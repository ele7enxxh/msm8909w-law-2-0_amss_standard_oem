#ifndef CMNAS_H
#define CMNAS_H

/*===========================================================================

         C A L L   M A N A G E R   N A S   H E A D E R   F I L E

DESCRIPTION
  This module contains the Call Manager NAS Object.
  It is the main implementation of shuffling events from NAS to CM clients.

Copyright (c) 2004 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmnas.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/05   sk      Separated CM reports and commands.
12/03/04   ws      Initial release.

===========================================================================*/

#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "comdef.h"    /* Definition for basic types and macros */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION cmnas_client_cmd_proc

DESCRIPTION
  Process clients' NAS commands.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmnas_client_cmd_proc(

    cm_nas_cmd_s_type  *nas_cmd_ptr
        /* pointer to a CM command */
);

/*===========================================================================

FUNCTION cmnas_rpt_proc

DESCRIPTION
  Process Lower Layer reports (i.e. link control, cell change, ... )

DEPENDENCIES
  none  

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmnas_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
);


#endif /* CMNAS_H */
