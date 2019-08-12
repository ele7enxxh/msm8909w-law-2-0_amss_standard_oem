/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                               E P Z I D 

GENERAL DESCRIPTION
  This file contains functions used to handle EPZID changes.

EXTERNALIZED FUNCTIONS
  DS707_EPZID_RECONNECT_CB
    Called by MC/CM when the system EPZID changes.  Sends a message to DS
    pkt mgr telling it to come out of dormancy.
    
  DS707_EPZID_DISABLE_RECON
    Disables dormant re-origination when EPZID changes.
    
  DS707_EPZID_REG_RECON
    Registers the EPZID reconnect callback with MC/CM.
    
  DS707_EPZID_DEREG_RECON
    De-registers the EPZID reconnect callback with MC/CM.
  
  DS707_EPZID_PARSE_SOCM
    Used for parsing the service option control message which contains the
    EPZID reconnect parameters.
    
  DS707_PROCESS_CMD
    Processes DS task commands specific to EPZID.  
    
  DS707_EPZID_INIT
    Registers with for 707 events.  

INITIALIZATION AND SEQUENCING REQUIREMENTS
    Must call ds707_epzid_init() at startup.

   Copyright (c) 1999 - 2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_epzid.c_v   1.2   24 Jan 2003 16:09:22   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_epzid.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/13/10   op      Migrated to MSG 2.0 macros
02/04/09   ss      Off Target Lint fixes
09/09/05   vrk/gr  EPZID enhancements releated to L2/L3 ack
04/05/05   gr      Initial version of the file to support IS707-B 
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
#include "ds3gtimer.h"
#include "ds707.h"
#include "ds707_epzid_util.h"
#include "ds707_epzid.h"
#include "ds707_pkt_mgr.h"
#include "dstask.h"
#include "err.h"
#include "mccsyobj.h"
#include "msg.h"
#include "data_msg.h"
#include "snm.h"
#include "ds707_cback.h"
#include "data_err_suppr.h"


/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/


/*===========================================================================
FUNCTION      DS707_EPZID_PARSE_SOCM

DESCRIPTION   This function parses the service option control message.

DEPENDENCIES  It is assumed that the message is already known to be a EPZID
              message.

RETURN VALUE  Returns byte (SOCM Message Id)

SIDE EFFECTS  None
===========================================================================*/
byte ds707_epzid_parse_socm
(
  byte           cntl,            /* SOCM CNTL field                       */
  byte           list_len         /* list_len (needs to masked off)       */
)
{
  byte 	  socm_message;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  socm_message = cntl & DS707_EPZID_CNTL_MASK;

   DATA_IS707_MSG2(MSG_LEGACY_MED, 
                   "Rx'ed SO Ctl Msg %d , max_len = %d", 
                   socm_message, ds707_epzid_get_epzid_max_list_len());

  switch(socm_message)
  {
    case DS707_EPZID_CNTL_DISABLE:
	  /*---------------------------------------------------------------------
       If cntl field tells mobile to disable EPZID based reconnection, reset
       the list.
       ---------------------------------------------------------------------*/
      ds707_epzid_disable_recon();
      break;

    case DS707_EPZID_CNTL_ENABLE:
      /*-------------------------------------------------------------------------
       If cntl field tells mobile to enable EPZID recon, initialize the list and
       set the maximum elements of the list.
      -------------------------------------------------------------------------*/
      ds707_epzid_update_max_list_len(list_len & DS707_EPZID_LIST_LEN_MASK);
      break;

    case DS707_EPZID_CNTL_CLEAR_LIST:
      /*-------------------------------------------------------------------------
       If cntl field tells mobile to clear the EPZID list, clear the list and
       reset the epzid list length to 0.
      -------------------------------------------------------------------------*/
      ds707_epzid_update_max_list_len(list_len & DS707_EPZID_LIST_LEN_MASK);
      ds707_epzid_init_list();
      break;

    default:
	  /*-------------------------------------------------------------------------
      Unknown EPZID_CNTL value. Return with SNM_INVALID_MSG_TYP. This will
      result in MS sending a Mobile Station Reject Order as per spec.
      -------------------------------------------------------------------------*/
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Unknown EPZID_CNTL value: 0x%x",
                (cntl & DS707_EPZID_CNTL_MASK) >> 5);
      break;
  }
  return socm_message;
} /* ds707_epzid_parse_socm() */



/*===========================================================================
FUNCTION       DS707_EPZID_RECON_ENABLED

DESCRIPTION    Returns TRUE if reconnection is enabled for EPZID changes.

DEPENDENCIES   None

RETURN VALUE   TRUE if EPZID recon turned on.  Else FALSE.

SIDE EFFECTS   None
===========================================================================*/
boolean ds707_epzid_recon_enabled(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DATA_IS707_EPZID_RECON

  return(TRUE);

#else

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "PZID recon FEATURE OFF");
  return(FALSE);

#endif /* FEATURE_DATA_IS707_EPZID_RECON */

} /* ds707_epzid_recon_enabled() */

/*===========================================================================
FUNCTION      DS707_EPZID_DISABLE_RECON

DESCRIPTION   Disables packet zone id based reconnection for packet calls.
              This function deregisters the epzidi_change_cb(). The list of
              visited packet zones is also cleared.
  
              This function is called when (see sec.2.2.7 of IS-707.A.5):
             (a) BS sends a SOCTL message disable EPZID reconnection
             (b) BS sends a EPZID of 0.
             (c) MS comes out of the active state - PPP session has ended.
             (d) MS detects a change in SID.
  
             This function can be called from either the DS or the MC task
             (from the callbacks).

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_epzid_disable_recon(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
     Reset the list of visited packet zones and set the default EPZID MAX
	 LIST LENGTH to 1. Also clear all the elements in the list
  -------------------------------------------------------------------------*/

  DATA_IS707_MSG0(MSG_LEGACY_MED, "Disabling EPZID based reconnection");

  if (ds707_epzid_get_epzid_max_list_len() > 0)
  {
    ds707_epzid_update_max_list_len(1);
    ds707_epzid_init_list();
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Trying to disable de-registerd EPZID recon");
  }

} /*  ds707_epzid_disable_recon() */

/*===========================================================================
FUNCTION      DS707_EPZID_CHANGE_HANDLER

DESCRIPTION   This function handles EZID change notification and manipulates
              the EPZID list. It also sets the trigger for the EPZID Mgr to
	            re-originate

DEPENDENCIES  It is assumed that the message is already known to be a EPZID
              message.

RETURN VALUE  Returns indication if message was okay or not okay.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_epzid_change_handler
( 
  ds707_epzid_type *new_epzid_ptr
)
{
  boolean reorig_flag = FALSE; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(new_epzid_ptr == NULL)
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Invalid pointer passed = %x", new_epzid_ptr);
    ASSERT(0);
    return FALSE;
  }
  DATA_MDM_ARG_NOT_CONST(new_epzid_ptr);

  /*-------------------------------------------------------------------------
    If EPZID list has not been initialized and the EPZID reconnection feature
	has been disabled. There is no need to re-orig here,log error and return
  -------------------------------------------------------------------------*/
  if ( ds707_epzid_get_epzid_max_list_len() == 0)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "EPZID change handler called, but EPZID recon not active");
    return reorig_flag;
  }
  /*-------------------------------------------------------------------------
    If EPZID sent in signaling message is 0, disable EPZID based reconnection.
  -------------------------------------------------------------------------*/
  if ( new_epzid_ptr->pzid == 0) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, " Got new EPZID=0, disabling EPZID reconnection");
    ds707_epzid_disable_recon();  
  }
  /*-------------------------------------------------------------------------
    Check if EPZID is in the list and return TRUE if the entry is a new EPZID
    If the new EPZID wasn't  on the list before this, set the reorig flag
    to originate pkt call with DRS=0.
  -------------------------------------------------------------------------*/
  else
  {
    if (ds707_epzid_is_epzid_in_list(new_epzid_ptr) >=  0)
    {
      /*-------------------------------------------------------------------------
        New EPZID already in the list. Since we have come back to the EPZID
        visited some time back, do not send a DRS=0 re-orig
       -------------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_MED, " EPZID already in the list, no re-orig required");
      reorig_flag = FALSE;
    }
    else
    { 
       DATA_IS707_MSG0(MSG_LEGACY_MED, " current EPZID is not in list, re-orig required");
      reorig_flag = TRUE;
    }
  }  /* EPZID reconnect is  active */  

  return reorig_flag;

} /* ds707_epzid_change_handler */
#endif /* FEATURE_DATA_IS707 */
