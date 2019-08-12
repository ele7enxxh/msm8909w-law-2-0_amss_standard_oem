/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         S O M E   S Y S T E M   R E L E A T E D   P A R A M E T E R S
                                         S U P P O R T   F O R   M D R

GENERAL DESCRIPTION
  This module contains supporting functions for detecting changes in packet zone ID,
  SID and the base station PREV.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 1999 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccsyobj.c_v   1.9   02 Oct 2002 15:58:34   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccsyobj.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/13   gga     Feature cleanup
06/25/13   gga     Fixed Klocwork errors
09/24/12   jj      Changes to support QCHAT on 4.x PLs.
06/29/12   ssh     1xCSFB: Updated the EPZID value based on the received
                   SIB8/C2K parameter message.
03/09/12   ppr     Changes to send location information to QChat.
02/04/12   ppr     Feature Cleanup
10/28/11   ppr     Reverting CL#2005490 (Changes to send location information to QChat)
10/27/11   ppr     Changes to send location information to QChat.
11/15/10   jtm     Initial SU API split for CP.
04/23/10   ag      Support for 1x Modem statistical report.
10/21/09   jtm     Fixed ARM compiler warnings.
05/14/09   jj      CMI Phase-II update.
05/15/08   bb      Code changes for IS707B addendum feature
08/16/06   pg      Added support for release D Packet Zone Hysteresis.
06/23/06   pg      RPC clean up.
03/31/05   fc      Added support for EPZID.
10/23/03   yll     Added support for updating HDR PANID.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
01/24/03   lh      Modified pzid change processing interface.
10/02/02   az      Mainlined FEATURE_IS2000
09/30/02   sb      Mainlined FEATURE_SD20.
10/03/01   kk      Fixed system variable in process_nid()
10/01/01   kk      Merge from common archive.
                   09/14/01   lcc     Added support for NID change handler function.
06/26/01   kk      Merged from mainline.
05/29/01   ht      Remove #ifdef FEATURE_PZID_CHANGE_EXAMPLE
                   surrounding "msg.h"
03/28/01   kk      Fixed variable name from system to current_system
03/27/01   kk      Mainline merge.
                   02/26/01   fc      Make sure retry delay of all types are
                   cleared before any PZID/SID change handlers are called
                   whenever PZID/SID changes.
03/01/01   kk      Merged to mainline.
02/16/01   fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
01/24/01   jq      Changed naming of some variables and functions so that the
                   software can compile with FEATURE_IS2000_SCH undefined.
12/28/00   fc      Added the support of clear retry delay of all types whenever
                   PZID_ID or SID/NID pair has been changed in idle state.
02/27/00   kk      Added support for storing/retrieving old system parameters
                   Old system is defined as the last system a packet data call
                   is originated successfully. Also changed the name of variable
                   system to "current_system" to prevent NT compilation problems.
11/13/00   lcc     1. Added member nid to "system" structure.
                   2. Added function for processing NID.
                   3. Updates SID, NID db items and generate events when changed
11/03/99   lcc     Merged from GD4021:
           doj     Made reg_prev_change_handler, process_prev and get_prev
                   only defined for FEATURE_SPECIAL_MDR.
03/12/99   lcc     Added sample code to demonstrate how PZID change handler is
                   used.
01/27/99   lcc     Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "db_v.h"
#include "event.h"
#include "mccsyobj_v.h"
#include "msg.h"
#include "m1x_diag.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccsch.h"
#include "mccsch_i.h"

#ifdef FEATURE_QCHAT
#include "onex_qchatupk.h"
#include "data1x_qchatupk_opts.h"
#endif /* FEATURE_QCHAT */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*===========================================================================

                 PROTOTYPES FOR SOME FUNCTIONS USED LOCALLY

===========================================================================*/
// These are the actual functions defined in mccsyobj_system object (which contains
// pointers to these functions).
static void process_pzid( byte pzid, word curr_state );
static void process_sid( word sid, word curr_state );
static void process_nid( word nid, word curr_state );



/*-------------------------------------------------------------------------
                            Variables
-------------------------------------------------------------------------*/
// Internal variable used only by the System object
// Must be initialized at power up
static struct
{
  struct
  {
    byte val;
  } pzid;

  struct
  {
    word val;
  } sid;

  struct
  {
    word val;
  } nid;

  struct
  {
   /* New call back method handler to store registration sub state
      change call back method registered by Data module.*/
    boolean (*reg_substate_handler)(word sid, word nid, byte pzid);
    void ( *change_handler )( word sid, word nid, byte pzid );

  } epzid;

#ifdef FEATURE_IS2000_REL_A
  struct
  {
    word val;
  } old_pd_orig_sid;

  struct
  {
    word val;
  } old_pd_orig_nid;

  struct
  {
    byte val;
  } old_pd_orig_pzid;
#endif /* FEATURE_IS2000_REL_A */

} current_system;


/*===========================================================================

FUNCTION mccsyobj_initialize

DESCRIPTION
  This function initializes the mccsyobj_system object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyobj_initialize
(
  void
)
{
  // Initialize the internal stored values.
  // Note that the public view of system object is already initialized
  current_system.pzid.val = 0;
  current_system.sid.val = 0;
  current_system.nid.val = 0;
  current_system.epzid.change_handler = 0;
  current_system.epzid.reg_substate_handler = 0;

#ifdef FEATURE_IS2000_REL_A
  current_system.old_pd_orig_pzid.val = 0;
  current_system.old_pd_orig_nid.val = 0;
  current_system.old_pd_orig_sid.val = 0;
#endif /* FEATURE_IS2000_REL_A */

}


/*===========================================================================

FUNCTION process_pzid

DESCRIPTION
  This function checks if the packet zone ID has changed.  If so, the stored
  packet zone ID value will be updated and if the change_handler is registered,
  it will be invoked.

  This function should be used when packet zone ID is received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void process_pzid
(
  byte id,
  word curr_state
)
{
  byte old_pzid;

  // If packet zone ID didn't change, just ignore and return
  if ( current_system.pzid.val == id )
  {
    return;
  }

  // Update packet zone ID
  old_pzid = current_system.pzid.val;
  current_system.pzid.val = id;

  // Now report the event
  event_report_payload( EVENT_PZID_CHANGE, sizeof(id),
                     &id );

  M1X_MSG( DCP, LEGACY_MED,
    "PZID changed-old:%d new:%d",
    old_pzid,
    id );

  (void) old_pzid; /* Added to satisfy compiler warning (set but not used) */

  // Clear all is not required on TC because that is explicitly specified
  // in the UHDM, and it is done before this point.
  if (MAIN_STATE(curr_state) != CDMA_TC)
  {
    /* Clear retry delay of all types. It must be done before any change handler
       is called */
    update_retry_delay(CAI_RETRY_CLR_ALL, 0);
  }

} // process_pzid


/*===========================================================================

FUNCTION mccsyobj_get_pzid

DESCRIPTION
  This function gets the current stored value of Packet Zone ID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

byte mccsyobj_get_pzid
(
  void
)
{
  return current_system.pzid.val;
} // mccsyobj_get_pzid



#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION mccsyobj_get_old_pd_orig_nid

DESCRIPTION
  This function gets the old packet zone id where a packed data call
  is successfully originated.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

word mccsyobj_get_old_pd_orig_nid
(
  void
)
{
  return current_system.old_pd_orig_nid.val;
} // mccsyobj_get_old_pd_orig_nid


/*===========================================================================

FUNCTION mccsyobj_get_old_pd_orig_pzid

DESCRIPTION
  This function gets the old packet zone id where a packed data call
  is successfully originated.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

byte mccsyobj_get_old_pd_orig_pzid
(
  void
)
{
  return current_system.old_pd_orig_pzid.val;
} // mccsyobj_get_old_pd_orig_pzid


/*===========================================================================

FUNCTION mccsyobj_get_old_pd_orig_sid

DESCRIPTION
  This function gets the old packet zone id where a packed data call
  is successfully originated.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

word mccsyobj_get_old_pd_orig_sid
(
  void
)
{
  return current_system.old_pd_orig_sid.val;
} // mccsyobj_get_old_pd_orig_sid


/*===========================================================================

FUNCTION mccsyobj_set_old_pd_orig_pzid

DESCRIPTION
  This function stores the current pzid. This function must be called
  when a successful packet data origination takes place.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyobj_set_old_pd_orig_pzid
(
  byte  pzid
)
{
  current_system.old_pd_orig_pzid.val = pzid;
} // mccsyobj_set_old_pd_orig_pzid


/*===========================================================================

FUNCTION mccsyobj_set_old_pd_orig_sid

DESCRIPTION
  This function stores the current sid. This function must be called
  when a successful packet data origination takes place.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyobj_set_old_pd_orig_sid
(
  word  sid
)
{
  current_system.old_pd_orig_sid.val = sid;
} // mccsyobj_set_old_pd_orig_sid


/*===========================================================================

FUNCTION mccsyobj_set_old_pd_orig_nid

DESCRIPTION
  This function stores the current nid. This function must be called
  when a successful packet data origination takes place.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyobj_set_old_pd_orig_nid
(
  word  nid
)
{
  current_system.old_pd_orig_nid.val = nid;
} // mccsyobj_set_old_pd_orig_nid

#endif /* FEATURE_IS2000_REL_A      */

/*===========================================================================

FUNCTION MCCSYOBJ_UPDATE_EPZID

DESCRIPTION
  This function checks if the SID or NID or PZID has changed.
  If so, the corresponding values will be updated and if the change_handler
  is registered, it will be invoked.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyobj_update_epzid
(
  boolean sid_inc,
  word sid,
  boolean nid_inc,
  word nid,
  boolean pzid_inc,
  byte pzid,
  word curr_state
)
{
  M1X_MSG( DCP, LEGACY_MED,
    "Updating EPZID (sid_inc, nid_inc, pzid_inc) = (%d,%d,%d)",
    sid_inc,
    nid_inc,
    pzid_inc);

  if(sid_inc)
  {
    process_sid(sid, curr_state);
  }
  if(nid_inc)
  {
    process_nid(nid, curr_state);
  }
  if(pzid_inc)
  {
    process_pzid(pzid, curr_state);
  }
}

/*===========================================================================

FUNCTION process_sid

DESCRIPTION
  This function checks if the SID has changed.  If so, the SID value
  will be updated and if the change_handler is registered, it will be invoked.

  This function should be used whenever SID is received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void process_sid
(
  word id,
  word curr_state
)
{
  word old_sid;


  // If SID didn't change, just ignore and return
  if ( current_system.sid.val == id )
  {
    return;
  }

  // Update SID
  old_sid = current_system.sid.val;
  current_system.sid.val = id;

  // Now report the event
  event_report_payload( EVENT_SID_CHANGE, sizeof(event_sid_change_type),
                     &id );
  M1X_MSG( DCP, LEGACY_MED,
    "SID changed-old:%d new:%d",
    old_sid,
    id );

  (void) old_sid; /* Added to satisfy compiler warning (set but not used) */

  /* Clear retry delay of all types. It must be done before any change handler
     is called */
  if (MAIN_STATE(curr_state) != CDMA_TC)
  {
    update_retry_delay(CAI_RETRY_CLR_ALL, 0);
  }

} // process_sid


/*===========================================================================

FUNCTION mccsyobj_get_sid

DESCRIPTION
  This function gets the current stored value of SID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

word mccsyobj_get_sid
(
  void
)
{
  return current_system.sid.val;
} // mccsyobj_get_sid


/*===========================================================================

FUNCTION process_nid

DESCRIPTION
  This function checks if the NID has changed.  If so, the NID value
  will be updated.

  This function should be used whenever NID is received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void process_nid
(
  word id,
  word curr_state
)
{
  word old_nid;


  // If NID didn't change, just ignore and return
  if ( current_system.nid.val == id )
  {
    return;
  }

  // Update NID
  old_nid = current_system.nid.val;
  current_system.nid.val = id;


  // Now report the event
  event_report_payload( EVENT_NID_CHANGE, sizeof(event_nid_change_type),
                     &id );
  M1X_MSG( DCP, LEGACY_MED,
    "NID changed-old:%d new:%d",
    old_nid,
    id );

  (void) old_nid; /* Added to satisfy compiler warning (set but not used) */

  /* Clear retry delay of all types if the current state is in idle */
  if (MAIN_STATE(curr_state) != CDMA_TC)
  {
    update_retry_delay(CAI_RETRY_CLR_ALL, 0);
  }

} // process_nid


/*===========================================================================

FUNCTION mccsyobj_get_nid

DESCRIPTION
  This function gets the current stored value of NID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

word mccsyobj_get_nid
(
  void
)
{
  return current_system.nid.val;
} // mccsyobj_get_nid

/*===========================================================================

FUNCTION mccsyobj_reg_epzid_change_handler

DESCRIPTION
  Registers the function to handle a change of EPZID (i.e. PZID, SID and NID).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyobj_reg_epzid_change_handler
(
  void ( *handler )( word sid, word nid, byte pzid )
)
{
  current_system.epzid.change_handler = handler;

} /* mccsyobj_reg_epzid_change_handler */

/*===========================================================================

FUNCTION mccsyobj_process_epzid

DESCRIPTION
  This function checks if the triple EPZID (i.e. SID, NID and PZID) has been
  changed. If so, the value of SID, NID and PZID will be updated and if the
  change_handler is registered, it will be invoked.

  This function should be used whenever SID, NID and PZID are received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyobj_process_epzid
(
  uint16 sid,
  uint16 nid,
  uint8 pzid,
  word curr_state
)
{

  /* If no change on SID, NID and PZID, ignore and return */
  if ((mccsyobj_get_sid() == sid) && (mccsyobj_get_nid() == nid) &&
      (mccsyobj_get_pzid() == pzid))
  {
    return ;
  }

  #ifdef FEATURE_QCHAT
  M1X_MSG( DCP, LEGACY_HIGH,
    "Change in 1x location");
  if(qchatupk_lu_enabled())
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Inform QChat about the new 1x location");
    qchatupk_update_location(SYS_SYS_MODE_CDMA);
  }
  #endif /* FEATURE_QCHAT */

  /* Process SID */
  process_sid(sid, curr_state);

  /* Process NID */
  process_nid(nid, curr_state);

  /* Process PZID */
  process_pzid(pzid, curr_state);

  /* If EPZID change handler is registered, call */
  if ( current_system.epzid.change_handler )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "EPZID changed, called registered EZID change handler");
      current_system.epzid.change_handler(mccsyobj_get_sid(),
      mccsyobj_get_nid(),
      (P_REV_IS_4_OR_GREATER ? mccsyobj_get_pzid() : MCCSYOBJ_PZID_UNKNOWN));
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "EPZID changed, no EPZID change handler registered");
  }

  /* Update modem statistics */
  cdma_stats.serv_sys_info.sid = sid;
  cdma_stats.serv_sys_info.nid = nid;
  cdma_stats.serv_sys_info.packet_zone_id = pzid;
  cdma_stats.changed_bitmask |= SYS_CDMA_SERV_SYS_INFO_BIT_MASK;

} /* mccsyobj_process_epzid */


/*===========================================================================

FUNCTION mccsyobj_epzid_reg_sub_state_handler

DESCRIPTION
  Registers the function to handle when mobile enters into the registration
  sub state due to either "timer based" or "parameter based" or "distance based" and
  "UZ based" registration types

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mccsyobj_epzid_reg_sub_state_handler
(
  boolean ( *handler )( word sid, word nid, byte pzid )
)
{
  current_system.epzid.reg_substate_handler = handler;

} /* mccsyobj_epzid_reg_sub_state_handler */

#ifdef FEATURE_IS707B_ADDENDUM
/*===========================================================================

FUNCTION mccsyobj_call_reg_change_handler

DESCRIPTION
  This method calls from mccidl_online () method to call the data module
  registered registration call back handler whose function reference address
  is stored in the "reg_change_handler" under epzid structure of current_system structure.
  This function calls whenever mobile station requires to perform registration
  due to either "Timer based" or "parameter based" or "distance based" and "user zone"
  based registration.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  -  Packet Zone Hysteresis timer is active for current EPZID parameters.
  FALSE -  Either Packet Zone Hysteresis timer is active for current EPZID parameters or Data module
           is not registsred call back method.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccsyobj_call_reg_change_handler (void)
{

    /* check whether registration sub state change handler is registered or not */
    if ( current_system.epzid.reg_substate_handler )
    {
        M1X_MSG( DCP, LEGACY_MED,
          "Registration sub state entered, called registered registration sub state change handler");
        return(current_system.epzid.reg_substate_handler (mccsyobj_get_sid(),
                mccsyobj_get_nid(),
                (P_REV_IS_4_OR_GREATER ? mccsyobj_get_pzid() : MCCSYOBJ_PZID_UNKNOWN)));
    }
    else
    {
        M1X_MSG( DCP, LEGACY_MED,
          "Registration sub state entered, no registration change handler registered");
        return FALSE;
    }
} /* mccsyobj_call_reg_change_handler */
#endif /* FEATURE_IS707B_ADDENDUM */


/*===========================================================================

FUNCTION mccsyobj_get_epzid

DESCRIPTION
  This function gets the current stored value of the triple EPZID (i.e. SID,
  NID, PZID).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyobj_get_epzid
(
  word *sid,
  word *nid,
  byte *pzid
)
{
  *sid = mccsyobj_get_sid();
  *nid = mccsyobj_get_nid();
  /* PZID is defined only in IS95 and onwards. Value 0 indicates PZID not
     defined in current P_REV in use. */
  *pzid = (P_REV_IS_4_OR_GREATER ? mccsyobj_get_pzid() : MCCSYOBJ_PZID_UNKNOWN);

} /* mccsyobj_get_epzid */

