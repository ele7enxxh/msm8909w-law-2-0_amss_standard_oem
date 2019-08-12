#ifndef DS707_GONULL_TMRI_H
#define DS707_GONULL_TMRI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                  G O  N U L L  T I M E R  C O N T R O L
                       
GENERAL DESCRIPTION
  This file contains functions used to handle the abort dormant ppp timer. This 
  functionality is also known as the go null timer.  It aborts the dormant ppp 
  as soon as the go null timer expires. 

EXTERNALIZED FUNCTIONS
    
  DS707_GONULLTMRI_START_TIMER
    Interface to start the go null timer

  DS707_GONULLTMRI_STOP_TIMER
    Interface to stop the go null timer

  DS707_GONULLTMRI_INIT
    Registers a callback for all the events to be handled by the gonull timer
    module

  DS707_GONULLTMRI_PROCESS_CMD
    This function processes the expiry of gonull timer.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2004 - 2011 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_gonulltmri.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/11   mg      Global variable cleanup
03/09/09   sa      AU level CMI modifications.
11/18/08   sn      Removed undefined functions.
03/18/04   gr      Initial creation of the module
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707

#include "dstask_v.h"

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_GONULLTMRI_INIT

DESCRIPTION   Called at startup

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_gonulltmri_init
(
  void
);

/*===========================================================================
FUNCTION      DS707_GONULLTMRI_PROCESS_CMD

DESCRIPTION   Processes the go null timer commands.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_gonulltmri_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);

/*===========================================================================
FUNCTION      DS707_GET_GONULLTMRI_IS_RUNNING_FLAG

DESCRIPTION   Returns value of gonulltmri_is_running flag.

DEPENDENCIES  None

RETURN VALUE  Boolean

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_get_gonulltmri_is_running_flag( void );

/*===========================================================================
FUNCTION      DS707_SET_GONULLTMRI_IS_RUNNING_FLAG

DESCRIPTION   Sets value of gonulltmri_is_running flag.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_set_gonulltmri_is_running_flag(
  boolean gonulltmri_is_running_flag 
);

/*===========================================================================
FUNCTION      DS707_GET_GONULLTMRI_VAL

DESCRIPTION   Returns value of ds707_gonulltmri_val

DEPENDENCIES  None

RETURN VALUE  uint32

SIDE EFFECTS  None
===========================================================================*/
uint32 ds707_get_gonulltmri_val( void );

/*===========================================================================
FUNCTION      DS707_SET_GONULLTMRI_VAL

DESCRIPTION   Sets value of ds707_gonulltmri_val

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_set_gonulltmri_val(
  uint32 gonulltmri_val
);

#endif /* DS707_GONULL_TMRI_H */
#endif /* FEATURE_DATA_IS707 */
