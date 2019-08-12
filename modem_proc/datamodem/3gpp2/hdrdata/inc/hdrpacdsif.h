#ifndef HDRPACDSIF_H
#define HDRPACDSIF_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H I G H     D A T A     R A T E

          PACKET APPLICATION CONTROLLER DATA SERVICES EXTERNAL FILE

GENERAL DESCRIPTION
  This file contains functionality that needs interaction between HDR Call
  Processing and Data Services.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2003-2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrpacdsif.h#1 $

when       who    what, where, why
--------   ---    -----------------------------------------------------------
09/04/09   ss     CMI SU level modifications.
06/23/04   mpa    added hdrpacdsif_is_sn_ppp_up
03/09/04   gr     added hdrpacdsif_clear_sn_ppp_status
11/17/03   vr     added hdrpacdsif_send_smp_session_closed_cmd_to_ds
10/21/03   vas    Added hdrpacdsif_handdown_to_1x
09/23/03   vas    Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdrpac.h"
#include "hdrpac_api.h"

/*===========================================================================

                        FUNCTION DEFINITIONS            

===========================================================================*/

/*===========================================================================
FUNCTION      HDRPACDSIF_REGISTER_SNRLP_FUNCS

DESCRIPTION   Initializes the HDR RLP function pointers so that they point 
              to the correct function that initializes HDR RLP. This
              function should be called when the AT is ready to receive/
              transmit RLP data and needs RLP to be initialized when the HDR
              traffic channel comes up (and cleaned up when traffic channel
              is closed).

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpacdsif_register_snrlp_funcs( void );

/*===========================================================================
FUNCTION      HDRPACDSIF_DEREGISTER_SNRLP_FUNCS

DESCRIPTION   Sets the HDR RLP function pointers to NULL, so that the 
              functions to initialize/cleanup HDR RLP cannot be called.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpacdsif_deregister_snrlp_funcs( void );

/*===========================================================================
FUNCTION      HDRPACDSIF_INIT_REGISTERED_RLP

DESCRIPTION   This function calls RLP initialize functions for all RLPs
              (SN and AN).
              
              For the default SN, if the function pointers are set to the 
              correct RLP initialization function, then the RLP will be 
              initialized. The intent is that the function pointers will be
              registered only when the data session is active/dormant. Thus
              when there is no data session, this function will not affect
              the SN RLP
              
              For AN the RLP needs to be initialized always when the traffic
              channel comes up if the session is open.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpacdsif_init_registered_rlp( void );

/*===========================================================================
FUNCTION      HDRPACDSIF_CLEANUP_REGISTERED_RLP

DESCRIPTION   This function calls RLP cleanup functions for all RLPs
              (SN and AN).
              
              For the default SN, if the function pointers are set to the 
              correct RLP cleanup function, then the RLP will be cleaned up
              The intent is that the function pointers will be registered 
              only when the data session is active/dormant. Thus when there
              is no data session, this function will not affect the SN RLP
              
              For AN the RLP needs to be cleaned up always when the traffic
              channel goes down if the session is open.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpacdsif_cleanup_registered_rlp( void );

/*===========================================================================
FUNCTION      HDRPACDSIF_POWERUP_INIT

DESCRIPTION   Does the powerup initialization

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpacdsif_powerup_init( void );

/*===========================================================================
FUNCTION      HDRPACDSIF_PROCESS_SMP_SESSION_CLOSED

DESCRIPTION   Processes the HDR session closed indication.
              It commands DS to end it's data session and then registers
              for the Iface up indication

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpacdsif_process_smp_session_closed( void );

/*===========================================================================

FUNCTION HDRPACDSIF_PROCESS_CMD

DESCRIPTION
  This function processes commands for the PACDSIF

DEPENDENCIES
  None
  
PARAMETERS
  pac_cmd_ptr - Pointer to the stucture containing the command

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrpacdsif_process_cmd
(
  hdrpac_cmd_type *pac_cmd_ptr 
);

/*===========================================================================

FUNCTION HDRPACDSIF_HANDDOWN_TO_1X

DESCRIPTION
  This function is called when DS 707 hands down from HDR to 1X. It queues
  up a command to hdrpacdsif informing it that this event has occured.

DEPENDENCIES
  None
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrpacdsif_handdown_to_1x
(
  void
);

#endif /* HDRPACDSIF_H */

