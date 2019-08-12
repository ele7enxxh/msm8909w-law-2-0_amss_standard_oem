#ifndef DSHDR_AN_MGR_H
#define DSHDR_AN_MGR_H

/*===========================================================================

                        D S H D R _ A N _ M G R . H
                        
GENERAL DESCRIPTION

  This file is the controller for HDR Access Network Authentication calls. 

EXTERNALIZED FUNCTIONS
  All functions in this file are externalized.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  dshdr_an_mgr_powerup_init() needs to be called at powerup. It should be
  called from the DS task. When a HDR session is opened, if the AN stream is
  negotiated, dshdr_an_mgr_session_init is called. When the HDR session is
  closed, dshdr_an_mgr_session_close is called. Callbacks are registered with
  CM that get called when a Call gets connected or Ends.

  Copyright (c) 2003 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/dshdr_an_mgr.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/18/11    vpk    HDR SU API cleanup
03/30/10    ssh    Added dshdr_an_set_user_id()
03/27/09    ls     Added dshdr_an_auth_status_e_type, dshdr_get_an_auth_status()
                   and dshdr_an_auth_status_init()
03/09/09    sa     AU level CMI modifications.
09/02/03    vas    Added dshdr_an_process_cmd
10/13/03    vas    Added function prototypes.
02/19/03    vas    Created module

===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/
#include "ps_iface.h"
#include "dstask_v.h"
#include "hdrcom_api.h"


/*===========================================================================
                            VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
  AN authentication status definition
---------------------------------------------------------------------------*/
typedef enum
{
  DSHDR_NOT_AUTH  = 0,           /* Not Authenticated */
  DSHDR_PASS_AUTH,               /* Authenticated successfully */
  DSHDR_FAILED_AUTH              /* Failed authentication */
} dshdr_an_auth_status_e_type;
/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DSHDR_AN_MGR_SESSION_INIT

DESCRIPTION   
  This function sets up and initializes various RLP/PPP data structures for 
  the AN authentication manager. The function gets called when a HDR session
  is opened.

DEPENDENCIES  
  None.

RETURN VALUE  
  None.
  
SIDE EFFECTS  
  None.
===========================================================================*/
void dshdr_an_mgr_session_init
(
  hdrcom_hai_stream_enum_type stream_id
);

/*===========================================================================
FUNCTION      DSHDR_AN_MGR_SESSION_CLOSE

DESCRIPTION   
  This function cleans up the various RLP/PPP data structures for 
  the AN authentication manager. The function gets called when a HDR session
  is closed.

DEPENDENCIES  
  This function should be called before the PAC actually discards the
  stream mapping for the sesison.

RETURN VALUE  
  None.
  
SIDE EFFECTS  
  None.
===========================================================================*/
void dshdr_an_mgr_session_close
(
  hdrcom_hai_stream_enum_type stream_id
);

/*===========================================================================
FUNCTION      DSHDR_AN_MGR_POWERUP_INIT

DESCRIPTION   Does the powerup initialization for the HDR AN Authentication
              module.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void dshdr_an_mgr_powerup_init( void );

/*===========================================================================
FUNCTION      DSHDR_AN_GET_IFACE_PTR

DESCRIPTION   Returns the pointer to the AN PS iface.

DEPENDENCIES  None.

RETURN VALUE  pointer to the AN ps iface

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type * dshdr_an_get_iface_ptr( void );

/*===========================================================================

FUNCTION DSHDR_AN_PROCESS_CMD

DESCRIPTION
  This function processes HDR AN commands. A command processing function is
  called based on the type of command received.

  This function should be called by the top-level task dispatcher whenever
  any HDR AN command is received.

DEPENDENCIES
  HDR AN authentication module should have been initialized prior to calling
  this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  dshdr_an_process_cmd
(
  ds_cmd_type  *cmd_ptr
);
/*===========================================================================
FUNCTION      dshdr_an_auth_status_init

DESCRIPTION   Initialize AN authentication status. This function is called 
              when a new HDR session is opened in hdrpac_process_ind() or
              during the power up state

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void dshdr_an_auth_status_init( void );

/*===========================================================================
FUNCTION      dshdr_get_an_auth_status

DESCRIPTION   Get AN authentication status. This function may be called 
              by QMI module or other modules

DEPENDENCIES  None.

RETURN VALUE  DSHDR_NOT_AUTH:    Not Authenticated 
              DSHDR_PASS_AUTH:   Authenticated successfully 
              DSHDR_FAILED_AUTH: Failed authentication

SIDE EFFECTS  None.
===========================================================================*/
dshdr_an_auth_status_e_type dshdr_get_an_auth_status( void );

/*===========================================================================
FUNCTION      dshdr_an_set_user_id

DESCRIPTION   Set HDR AN user id

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
int dshdr_an_set_user_id( uint8 * user_id_ptr, uint8 user_id_len );

#endif /* DSHDR_AN_MGR_H */

