#ifndef CMMBMS_H
#define CMMBMS_H
/*===========================================================================

         C A L L   M A N A G E R   M B M S    H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with   CM MBMS data object.

  The MBMS data object is responsible for:
  1. Processing clients' MBMS data commands.
  2. Processing LL replies for clients' MBMS data commands.
  3. Processing LL MBMS data related notifications.
  4. Notifying the client list of MBMS data events.

Copyright (c) 2004 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmmbms.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/08   rm      Mainlining of MBMS changes
03/18/07   rp      Initial release for MBMS phase 2A integration
===========================================================================*/


/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "comdef.h"    /* Definition for basic types and macros */
#include "sys.h"       /* system wide common types */
#include "sys_v.h"
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"  
#include "cmwll.h"

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/


/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Macros
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Functions - internal
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Functions - external
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cmmbms_client_cmd_proc

DESCRIPTION
  Process clients' MBMS data commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmmbms_client_cmd_proc(

  cm_mbms_cmd_s_type   *mbms_cmd_ptr
    /* pointer to a CM command
    */
);

/*===========================================================================

FUNCTION cmmbms_rpt_proc

DESCRIPTION

  Process LL reports related to MBMS


DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmmbms_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);




#endif /* CMMBMS_H */

