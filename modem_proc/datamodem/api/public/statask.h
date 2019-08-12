#ifndef STATASK_H
#define STATASK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        S T A T A S K . H 

GENERAL DESCRIPTION
  Header file for socket test app.

Copyright (c) 2001,2005,2009 by Qualcomm Technologies, Incorporated.  
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/statask.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/19/09    vm     Move sta_set_phone_opmode to internal API (sta_task_cmd.h)
02/17/09    pp     CMI Public/Private API split.
02/11/09    ar     Add sta_set_phone_opmode prototype.
02/06/09    mjb    Add sta_wait_sync_sig & sta_post_sync_sig_tcb to decouple from ONCRPC
09/29/05    mjb    Added prototypes for sta_prog_start(),sta_prog_init()
09/28/05    clp    Split call of prog_init -> prog_init and prog_start.
12/14/01    clp    Added header and comments to source file.


===========================================================================*/

#include "comdef.h"

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                        FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================
FUNCTION STA_TASK

DESCRIPTION
  This function is the task and processes signals/events for the task.

PARAMETERS
  Ignored.

RETURN VALUE
  Never returns.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void
sta_task
(
  dword unused
);

#endif /* STATASK_H */
