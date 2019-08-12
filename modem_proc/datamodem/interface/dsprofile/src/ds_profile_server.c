/******************************************************************************
  @file    ds_profile_server.c
  @brief   DS PROFILE server. The server listens to Subsystem Restart events and
           reacts accordingly.

  DESCRIPTION
      DS PROFILE server. The server listens to Subsystem Restart events and
      reacts accordingly. This is a stub version.
      Full implementation for the QNX HLOS may be found at:
      //source/qcom/qct/modem/datahlos/qnx/rel/AP_11.01.01.02/data/interfaces/dsprofile/src/ds_profile_server.c

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  ---------------------------------------------------------------------------
  Copyright (c) 2012 Qualcomm Technologies Incorporated. 
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
#include "ds_profile_server.h"

/* When the flag below is set, modem restart is in progress */
boolean ds_profile_ss_restart_in_action = FALSE;

/*===========================================================================
FUNCTION ds_profile_server_start

DESCRIPTION 
  Starts the dsprofile server.
  Stub version.
  Full implementation for the QNX HLOS may be found at:
  //source/qcom/qct/modem/datahlos/qnx/rel/AP_11.01.01.02/data/interfaces/dsprofile/src/ds_profile_server.c

PARAMETERS

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
ds_profile_status_etype ds_profile_server_start(void)
{
  return DS_PROFILE_REG_RESULT_SUCCESS;
}

/*===========================================================================
FUNCTION ds_profile_server_main

DESCRIPTION 
  Main function of the server thread.
  Stub version.
  Full implementation for the QNX HLOS may be found at:
  //source/qcom/qct/modem/datahlos/qnx/rel/AP_11.01.01.02/data/interfaces/dsprofile/src/ds_profile_server.c

PARAMETERS

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void ds_profile_server_main(void)
{
}

/*===========================================================================
FUNCTION    ds_profile_server_close()

DESCRIPTION 
  Closes the server.
  Stub version.
  Full implementation for the QNX HLOS may be found at:
  //source/qcom/qct/modem/datahlos/qnx/rel/AP_11.01.01.02/data/interfaces/dsprofile/src/ds_profile_server.c

PARAMETERS

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void ds_profile_server_close(void)
{
}

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
boolean ds_profile_server_ss_restart_in_action(void)
{
  return ds_profile_ss_restart_in_action;
}
