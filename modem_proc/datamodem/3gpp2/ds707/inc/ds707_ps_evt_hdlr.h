#ifndef DS707_PS_EVT_HDLR_H
#define DS707_PS_EVT_HDLR_H

/** 
  @file ds707_ps_evt_hdlr.h
  @brief 
   Handles PS events
*/
/** Contains API's to handle PS events */

/*===========================================================================

                       D S 7 0 7 _ P S _ E V T _ H D L R
GENERAL DESCRIPTION
  This file contains the fuctions to handle ps events

EXTERNALIZED FUNCTIONS

INTERNAL FUNCTIONS 

 Copyright (c) 2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_ps_evt_hdlr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/03/12   jee     Created Module

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_EHRPD
/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PS_EVT_HDLR_DS_CMD_PROCESS

DESCRIPTION   Process the command in DS context

PARAMETERS    cmd_ptr   - Ptr to the command information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ps_evt_hdlr_ds_cmd_process 
(
  const ds_cmd_type *cmd_ptr                         /* Pointer to Command */
);

/*===========================================================================
FUNCTION      DS707_PS_EVT_HDLR_INIT

DESCRIPTION   Called at initialization.     

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_ps_evt_hdlr_init
(
  void
);
#endif /* FEATURE_EHRPD */
#endif /* DS707_PS_EVT_HDLR_H */
