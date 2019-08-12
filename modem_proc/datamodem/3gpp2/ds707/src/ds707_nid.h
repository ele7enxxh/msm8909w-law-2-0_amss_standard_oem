#ifndef DS707_NID_H
#define DS707_NID_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                        D A T A   S E R V I C E S

                                   N I D 

GENERAL DESCRIPTION
  This file contains functions used to handle NID changes.

EXTERNALIZED FUNCTIONS
  DS707_NID_ENABLE_RECON
    Enables a packet reconnection when the NID changes.
    
  DS707_NID_DISABLE_RECON
    Disables a packet reconnection when the NID changes.
    
  DS707_NID_CHANGE_HANDLER
    Registered with MC.  Called when the NID changes.  If enabled, will 
    send a msg to DS pkt mgr, telling it to come out of dormancy.
    
  DS707_NID_PROCESS_CMD
    Processes DS Task commands specific to NID.
    
  DS707_NID_RECON_ENABLED
    Returns TRUE/FALSE depending on if should re-orig on NID change.
    
  DS707_NID_INIT
    Registers for various 707 events.

INITIALIZATION AND SEQUENCING REQUIREMENTS
    Must call ds707_nid_init() at power-up.

   Copyright (c) 1999-2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_nid.h_v   1.1   18 Nov 2002 18:26:04   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_nid.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/09   sa      AU level CMI modifications.
02/06/09   ss      Off Target Lint Fixes
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
FUNCTION       DS707_NID_ENABLE_RECON

DESCRIPTION    Enables reconnection based on NID change.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_nid_enable_recon(void);

/*===========================================================================
FUNCTION       DS707_NID_DISABLE_RECON

DESCRIPTION    Disables reconnection based on NID change.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_nid_disable_recon(void);

/*===========================================================================
FUNCTION       DS707_NID_CHANGE_HANDLER

DESCRIPTION    Causes a data reconnection.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
boolean ds707_nid_change_handler
(
  word  old_nid,                                 /* Old Network Id         */
  word  new_nid                                  /* New Network Id         */
);

/*===========================================================================
FUNCTION       DS707_NID_REGISTER

DESCRIPTION    Registers the handler with MC.  Does not change the 
               enabled/disabled reconnection attribute.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_nid_register(void);

/*===========================================================================
FUNCTION       DS707_NID_DEREGISTER

DESCRIPTION    Deregisters the handler with MC.  Does not change the 
               enabled/disabled reconnection attribute.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_nid_deregister(void);

/*===========================================================================
FUNCTION       DS707_NID_INIT

DESCRIPTION    Initialize SID module.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_nid_init(void);

/*===========================================================================
FUNCTION      DS707_NID_PROCESS_CMD

DESCRIPTION   Processes the NID commands.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_nid_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);
#endif /* FEATURE_DATA_IS707   */
#endif /* DS707_NID_H          */

