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
    
  DS707_NID_REGISTER
    Registers the NID change handler with MC.
    
  DS707_NID_DEREGISTER
    De-registers the NID change handler with MC.
    
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

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_nid.c_v   1.1   18 Nov 2002 18:26:00   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_nid.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/06/09   ss      Off Target Lint Fixes
04/05/05   gr      Integrated EPZID functionality onto the main line
04/22/04   ak      Lint changes.
04/09/04   ak      Make event callback take a const.
02/04/04   ak      Updated to use new pkt mgr event callbacks.
08/25/03   ak      Enable NID by default.
11/15/02   ak      Updated file header comments.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "ds707.h"
#include "ds707_nid.h"
#include "ds707_cback.h"
#include "ds707_pkt_mgr.h"

#include "dstask.h"
#include "mccsyobj.h"
#include "msg.h"
#include "data_msg.h"
#include "err.h"
#include "amssassert.h"


/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/*---------------------------------------------------------------------------
  NID based reconnection is turned on by default.  
---------------------------------------------------------------------------*/
LOCAL boolean nidi_recon_enabled = TRUE;

/*==========================================================================
           LOCAL FUNCTION DECLARATION
===========================================================================*/
LOCAL boolean ds707_nid_recon_enabled(void);

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/
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
void ds707_nid_enable_recon(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  nidi_recon_enabled = TRUE;
}

/*===========================================================================
FUNCTION       DS707_NID_DISABLE_RECON

DESCRIPTION    Disables reconnection based on NID change.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_nid_disable_recon(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  nidi_recon_enabled = FALSE;
}

/*===========================================================================
FUNCTION       DS707_NID_CHANGE_HANDLER

DESCRIPTION    Causes a data reconnection.  

DEPENDENCIES   None

RETURN VALUE   Returns a boolean flag to indicate if a re-orig has to happen

SIDE EFFECTS   None
===========================================================================*/
boolean ds707_nid_change_handler
(
  word  old_nid,                                 /* Old Network Id         */
  word  new_nid                                  /* New Network Id         */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds707_cback_event_info_type   event_info;

  if (new_nid != old_nid)
  {
	DATA_IS707_MSG2(MSG_LEGACY_HIGH, " NID change - mc cback, old=%d, new=%d", old_nid, new_nid);
    if (ds707_nid_recon_enabled() == TRUE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Post NID change event");
      ds707_cback_post_event(DS707_CBACK_NID_CHANGED_EV, &event_info);
      return TRUE;
    }
  }
  return FALSE;
} /* ds707_nid_change_handler() */

/*===========================================================================
FUNCTION       DS707_NID_RECON_ENABLED

DESCRIPTION    Returns TRUE if reconnection is enabled for NID changes.

DEPENDENCIES   None

RETURN VALUE   TRUE if NID recon turned on.  Else FALSE.

SIDE EFFECTS   None
===========================================================================*/
LOCAL boolean ds707_nid_recon_enabled(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DATA_IS707_NID_CHNG_RECON
  if (nidi_recon_enabled == TRUE)
  {
    return(TRUE);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Nid recon not enabled now");
    return(FALSE);
  }
#else
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "NID recon FEATURE OFF");
  return(FALSE);
#endif /* FEATURE_DATA_IS707_NID_CHNG_RECON */
} /* ds707_nid_recon_enabled() */

#endif /* FEATURE_DATA_IS707 */
