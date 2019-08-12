#ifndef DS707_SID_H
#define DS707_SID_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                               S I D 

GENERAL DESCRIPTION
  This file contains functions used to handle SID changes.

EXTERNALIZED FUNCTIONS
  DS707_SID_ENABLE_RECON
    Enables reconnection out of dormancy when the SID changes.
    
  DS707_SID_DISABLE_RECON
    Disables reconnection out of dormancy when the SID changes.
    
  DS707_SID_CHANGE_HANDLER
    Registered with MC. Called by MC when the SID changes.  This will send
    a msg to 1X Pkt mgr, telling it to come out of dormancy.
    
  DS707_SID_REGISTER
    Registers the SID change handler with MC.
    
  DS707_SID_DEREGISTER
    Deregisters the SID change handler with MC.
 
  DS707_SID_PROCESS_CMD
    Processes DS Task commands specific to SID.
    
  DS707_SID_RECON_ENABLED
    Returns TRUE/FALSE depending on if should re-orig on SID change.
    
  DS707_SID_INIT
    Registers for various 707 events.

INITIALIZATION AND SEQUENCING REQUIREMENTS
    Must call ds707_sid_init() at power-up.

   Copyright (c) 1999 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_sid.h_v   1.1   18 Nov 2002 18:33:00   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_sid.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/09   sa      AU level CMI modifications.
04/05/05   gr      Integrated EPZID functionality onto the main line
02/04/04   ak      Updated to use new pkt mgr event callbacks.
11/15/02   ak      Updated file header comments.

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
FUNCTION       DS707_SID_ENABLE_RECON

DESCRIPTION    Enables reconnection based on SID change.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_sid_enable_recon(void);

/*===========================================================================
FUNCTION       DS707_SID_DISABLE_RECON

DESCRIPTION    Disables reconnection based on SID change.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_sid_disable_recon(void);

/*===========================================================================
FUNCTION       DS707_SID_CHANGE_HANDLER

DESCRIPTION    Reset the Packet Data Dormant Timer to its default value, per
               IS-707-A.5/9 Section 2.2.6.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Resets the ds_dormant_timer value to its default value.
===========================================================================*/
boolean ds707_sid_change_handler
(
  word  old_sid,                                 /* Old System Id          */
  word  new_sid                                  /* New System Id          */
);

/*===========================================================================
FUNCTION       DS707_SID_REGISTER

DESCRIPTION    Registers the handler with MC.  Does not change the 
               enabled/disabled reconnection attribute.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_sid_register(void);

/*===========================================================================
FUNCTION       DS707_SID_DEREGISTER

DESCRIPTION    Deregisters the handler with MC.  Does not change the 
               enabled/disabled reconnection attribute.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_sid_deregister(void);

/*===========================================================================
FUNCTION       DS707_SID_INIT

DESCRIPTION    Initialize SID module.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_sid_init(void);

/*===========================================================================
FUNCTION       DS707_SID_RECON_ENABLED

DESCRIPTION    Returns TRUE if reconnection is enabled for SID changes.

DEPENDENCIES   None

RETURN VALUE   TRUE if SID recon turned on.  Else FALSE.

SIDE EFFECTS   None
===========================================================================*/
boolean ds707_sid_recon_enabled(void);

/*===========================================================================
FUNCTION      DS707_SID_PROCESS_CMD

DESCRIPTION   Processes the SID commands.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sid_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_SID_H        */
