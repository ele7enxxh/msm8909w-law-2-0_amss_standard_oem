/******************************************************************************
  @file    ds_profile_server.h
  @brief   DS PROFILE server. The server listens to Subsystem Restart events and
           reacts accordingly.

  DESCRIPTION
      DS PROFILE server. The server listens to Subsystem Restart events and
      reacts accordingly. This is a stub version.
      Full implementation for the QNX HLOS may be found at:
      //source/qcom/qct/modem/datahlos/qnx/rel/AP_11.01.01.02/data/interfaces/dsprofile/src/ds_profile_server.h

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ---------------------------------------------------------------------------
******************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofile/inc/ds_profile_server.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/07/11   gs      First submit
===========================================================================*/
#ifndef DS_PROFILE_SERVER_H
#define DS_PROFILE_SERVER_H

#include "ds_profile.h"



/*===========================================================================
FUNCTION ds_profile_server_start

DESCRIPTION 
  Starts the dsprofile server.

PARAMETERS

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
ds_profile_status_etype ds_profile_server_start(void);

/*===========================================================================
FUNCTION ds_profile_server_main

DESCRIPTION 
  Main function of the server thread.
 
PARAMETERS

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void ds_profile_server_main(void);

/*===========================================================================
FUNCTION    ds_profile_server_close

DESCRIPTION 
  Closes the server.

PARAMETERS

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void ds_profile_server_close(void);

/*===========================================================================
FUNCTION ds_profile_server_ss_restart_in_action

DESCRIPTION 
  Returns TRUE if SubSystem Restart is taking place, or FALSE otherwise
  
PARAMETERS

DEPENDENCIES

RETURN VALUE
  TRUE - SubSystem Restart is taking place
  FALSE - otherwise
 
SIDE EFFECTS

===========================================================================*/
boolean ds_profile_server_ss_restart_in_action(void);

#endif /* DS_PROFILE_SERVER_H */
