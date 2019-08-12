/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/msm5200/nas/vcs/gslock.c_v   1.0   06 Apr 2001 12:02:34   tschwarz  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/gslock.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------



===========================================================================*/

/* Include files */

#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "timers_v.h"
#include "gs_v.h"

/*******************************************************************************
 *
 *  Function name: gs_attach_lock
 *  -----------------------------
 *  Description:
 *  ------------
 *  This module initializes a rex critical section.
 *
 *  Parameters:
 *  -----------
 *  IN         crit_sect
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

gs_status_T gs_attach_lock(rex_crit_sect_type  *crit_sect)
{

   /* Initialize critical section */
   rex_init_crit_sect(crit_sect);

   return GS_SUCCESS;

}

/*******************************************************************************
 *
 *  Function name: gs_lock
 *  ----------------------
 *  Description:
 *  ------------
 *  This module sets a semaphore to indicate locked.
 *
 *  Parameters:
 *  -----------
 *  IN         crit_sect
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

gs_status_T gs_lock(rex_crit_sect_type  *crit_sect)
{
   rex_enter_crit_sect(crit_sect);
   
   return GS_SUCCESS;
}

/*******************************************************************************
 *
 *  Function name: gs_unlock
 *  ------------------------
 *  Description:
 *  ------------
 *  This module sets a semaphore to indicate unlocked.
 *
 *  Parameters:
 *  -----------
 *  IN         crit_sect
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/


gs_status_T gs_unlock (rex_crit_sect_type  *crit_sect) 
{
   rex_leave_crit_sect(crit_sect);

   return GS_SUCCESS;
}
