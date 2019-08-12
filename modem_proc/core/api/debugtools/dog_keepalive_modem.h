#ifndef DOG_KEEPALIVE_MODEM_H
#define DOG_KEEPALIVE_MODEM_H
/******************************************************************************
  @file  dog_keepalive_modem.h
  @brief Dog Keepalive rpc program interface

  DESCRIPTION
    Interface definition for Dog Keepalive Modem, which is a modem server
    that allows remote processors to register for dog keepalive callbacks.
  
  INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  -----------------------------------------------------------------------------
  Copyright (c) 2010 Qualcomm Technologies Incorporated. 
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  -----------------------------------------------------------------------------
******************************************************************************/

/*===========================================================================

                             EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/debugtools/dog_keepalive_modem.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/10/08     rr    Initial version.
===========================================================================*/

/*============================================================================

             TYPE DEFINITIONS

============================================================================*/
#include "comdef.h"

typedef boolean (dog_keepalive_modem_cb_f_type) (void);
/*~ CALLBACK dog_keepalive_modem_cb_f_type
    ONERROR return FALSE */

/*===========================================================================
FUNCTION DOG_KEEPALIVE_MODEM_REGISTER

DESCRIPTION
  Register a dog keepalive client.

PARAMETERS
  dog_keepalive_modem_cb_f_type cb_func - Callback function to register.
  uint32 response_msec - maximum time allowed in mili-seconds for processor
                         to respond.
  uint32 *clnt_id - pointer for function to populate client id.

RETURN VALUE
  TRUE - Succeeded in registering.
  FALSE - No resources available.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean dog_keepalive_modem_register(dog_keepalive_modem_cb_f_type cb_func,
                                     uint32 response_msec,
                                     uint32 *clnt_id); 
/*~ FUNCTION dog_keepalive_modem_register     
    RELEASE_FUNC dog_keepalive_modem_unregister(*clnt_id)     
    ONERROR return FALSE */
/*~ PARAM IN name */
/*~ PARAM OUT clnt_id POINTER */

typedef enum 
{
	DOG_KEEPALIVE_MODE_SIGNAL_APPS,
	DOG_KEEPALIVE_MODE_RESET_CHIP,
	DOG_KEEPALIVE_MODE_LOG_MSG,
	DOG_KEEPALIVE_MODE_LAST=DOG_KEEPALIVE_MODE_LOG_MSG,
	DOG_KEEPALIVE_MODE_DEFAULT = DOG_KEEPALIVE_MODE_SIGNAL_APPS
}dog_keepalive_mode_type;


/*===========================================================================
FUNCTION DOG_KEEPALIVE_MODEM_UNREGISTER

DESCRIPTION
  Unregister a dog keepalive client.

PARAMETERS
  uint32 clnt_id - The callback ID that was returned in the register function
                   to identify which keepalive callback to unregister.

RETURN VALUE
  TRUE - Succeeded in registering.
  FALSE - Registration not found.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean dog_keepalive_modem_unregister(uint32 clnt_id);
/*~ FUNCTION dog_keepalive_modem_unregister ONERROR return FALSE*/


/*===========================================================================
FUNCTION DOG_KEEPALIVE_MODEM_SET_MODE

DESCRIPTION
  Set the time out mode of the keepalive client. The mode decides the action taken
  by the modem in the event of a client not responding within the registered time
  out period.

PARAMETERS
  dog_keepalive_mode_type mode - An enumeration of possible modes.

  uint32 clnt_id - The callback ID that was returned in the register function
                   to identify which keepalive callback to unregister.

RETURN VALUE
  TRUE - Succeeded in registering.
  FALSE - Registration not found.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/

boolean dog_keepalive_modem_set_mode(dog_keepalive_mode_type mode, 
				     uint32 clnt_id); 
/*~ FUNCTION dog_keepalive_modem_set_mode ONERROR return FALSE*/


#endif /* DOG_KEEPALIVE_MODEM_H */
