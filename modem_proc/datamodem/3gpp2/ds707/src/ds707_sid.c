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
  This will send msg to 1X Pkt mgr, telling it to come out of dormancy.
    
  DS707_SID_RECON_ENABLED
    Returns TRUE/FALSE depending on if should re-orig on SID change.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
    Must call ds707_sid_init() at power-up.

   Copyright (c) 1999 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_sid.c_v   1.1   18 Nov 2002 18:32:54   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_sid.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/09   ms      Offtarget Lint fixes.
04/05/05   gr      Integrated EPZID functionality onto the main line
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "amssassert.h"
#include "ds707.h"
#include "ds707_pkt_mgr.h"
#include "ds707_sid.h"
#include "ds707_cback.h"

#include "err.h"
#include "msg.h"
#include "data_msg.h"


/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/*---------------------------------------------------------------------------
  SID based reconnection is turned on by default.  
---------------------------------------------------------------------------*/
LOCAL boolean sidi_recon_enabled = TRUE;

LOCAL void ds707_sid_enable_recon(void);
LOCAL void ds707_sid_disable_recon(void);
LOCAL boolean ds707_sid_recon_enabled(void);

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/

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
LOCAL void ds707_sid_enable_recon(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sidi_recon_enabled = TRUE;
}

/*===========================================================================
FUNCTION       DS707_SID_DISABLE_RECON

DESCRIPTION    Disables reconnection based on SID change.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
LOCAL void ds707_sid_disable_recon(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sidi_recon_enabled = FALSE;
}

/*===========================================================================
FUNCTION       DS707_SID_CHANGE_HANDLER

DESCRIPTION    Reset the Packet Data Dormant Timer to its default value, per
               IS-707-A.5/9 Section 2.2.6.

DEPENDENCIES   None

RETURN VALUE   Returns a boolean flag to indicate if a re-orig has to happen

SIDE EFFECTS   Resets the ds_dormant_timer value to its default value.
===========================================================================*/
boolean ds707_sid_change_handler
(
  word  old_sid,                                 /* Old System Id          */
  word  new_sid                                  /* New System Id          */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds707_cback_event_info_type   event_info;

  if (new_sid != old_sid)
  {
	DATA_IS707_MSG2(MSG_LEGACY_HIGH, " SID change - mc cback, old=%d, new=%d", old_sid, new_sid);
    if (ds707_sid_recon_enabled() == TRUE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Post SID change event");
      ds707_cback_post_event(DS707_CBACK_SID_CHANGED_EV, &event_info);
      return TRUE;
    }
  }
  return FALSE;
} /* ds707_sid_change_handler() */

/*===========================================================================
FUNCTION       DS707_SID_RECON_ENABLED

DESCRIPTION    Returns TRUE if reconnection is enabled for SID changes.

DEPENDENCIES   None

RETURN VALUE   TRUE if SID recon turned on.  Else FALSE.

SIDE EFFECTS   None
===========================================================================*/
LOCAL boolean ds707_sid_recon_enabled(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DATA_IS707_SID_CHNG_RECON
  if (sidi_recon_enabled == TRUE)
  {
    return(TRUE);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Sid recon not enabled now");
    return(FALSE);
  }
#else
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "SID recon FEATURE OFF");
  return(FALSE);
#endif /* FEATURE_DATA_IS707_SID_CHNG_RECON */
} /* ds707_sid_recon_enabled() */

#endif /* FEATURE_DATA_IS707 */
