#ifndef DS707_CBACK
#define DS707_CBACK
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                        D S 7 0 7 _ C B A C K
                       
GENERAL DESCRIPTION
  Header file for the callback mechanism for the various 707 call control
  events.  These events are edge-trigger events (i.e, they will not check for
  current state when a user registers for them).

  These are assumed to be called from DS task context.

EXTERNALIZED FUNCTIONS
  DS707_CBACK_INIT
    Called at startup.  Intializes the internal callback array to be empty.
    
  DS707_CBACK_REGISTER
    Called by user to register a callback for a particular event.  The user
    can also specify a (void *)user_data, which will be passed back when the
    callback is called.  Duplicates are detected by matching both the cback
    AND the user data.  This allows user to have the same callback called
    multiple times, but with different user data.
    
  DS707_CBACK_DEREGISTER
    De-registers any instances of the registered callback.  Only those entries
    which match the callback AND user data are removed.  
 
  DS707_CBACK_POST_EVENT
    Called by interested party when a particular event occurs.  Calls all
    registered callbacks.  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  DS must initializes ds707_cback_init first, before any other 707 modules
  are initialized.  Other modules will probably register callbacks in their
  initialization.

   Copyright (c) 2004 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_cback.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/11   mg      Global variable cleanup
09/13/10   op      Removed DS707_CBACK_MSG macro, using MSG 2.0 format
07/27/09   spn     Added two more events to be handled, related to handoffs
                   from EPC to NON-EPC
4/16/06    sq/gr   added call_cmd_err to struct ds707_cback_event_info_type.
09/09/05   vrk/gr  EPZID enhancements releated to L2/L3 ack
04/05/05   gr      Integrated EPZID functionality into main line
11/22/04   vas     Removed variables not needed for QOS
09/10/04   vas     QOS related changes
08/06/04   ak      Added pzid changed event.
04/09/04   ak      Code review fixes.
02/04/04   ak      Initial version

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707

#include "cm.h"
#include "ds707.h"
#include "ds707_epzid.h"
#include "msg.h"
#include "dstask_v.h"
#include "ps_iface_defs.h"

#include "dsrlp_v.h"
#ifdef FEATURE_HDR_QOS
#define DSRLP_MAX_ELEMENTS ((DSRLP_MAX_NUM_HDR_MRLP_FWD) + \
                            (DSRLP_MAX_NUM_HDR_MRLP_REV))
#endif 

/*===========================================================================
                        TYPEDEFS and DEFINES
===========================================================================*/
/*---------------------------------------------------------------------------
  List of events that DS707 Call control supports.  
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_CBACK_MIN_EV = -1,                 /* Used for bounds checking     */

  DS707_CBACK_PHYS_LINK_COMING_UP_EV,      /* MS originating from null     */
  DS707_CBACK_PHYS_LINK_RESUMING_EV ,      /* MS originating from dormancy */
  DS707_CBACK_INC_CALL_EV,                 /* MS accepting inc call        */

  DS707_CBACK_PHYS_LINK_UP_EV,             /* Phys link up event           */
  DS707_CBACK_PHYS_LINK_DOWN_EV,           /* Phys link down event.  used  */
                                           /* for go dormant & Null        */
  
  DS707_CBACK_ABORT_SESSION_EV,            /* Implies a transition to NULL */
                                           /* while in phys_link_down state*/
  
  DS707_CBACK_HOLDDOWN_TIMER_STARTED_EV,   /* Holddown timer started ev    */
  DS707_CBACK_HOLDDOWN_TIMER_STOPPED_EV,   /* Holdddown timer stopd/expired*/

  DS707_CBACK_EPZID_CHANGED_EV,            /* PZID changed callback        */
  DS707_CBACK_SID_CHANGED_EV,              /* SID changed callback         */
  DS707_CBACK_NID_CHANGED_EV,              /* NID changed callback         */

  DS707_CBACK_NEW_NET_EV,                  /* found either HDR or 1x       */

  DS707_CBACK_HANDOFF_EPC_TO_NONEPC,
  DS707_CBACK_HANDOFF_NONEPC_TO_EPC,

#ifdef FEATURE_DATA_OPTHO
  /* TUNNEL PHYS LINK EVENTS NEEDED FOR OPTHO */

  DS707_CBACK_TUN_PHYS_LINK_COMING_UP_EV,
  DS707_CBACK_TUN_PHYS_LINK_RESUMING_EV,
  DS707_CBACK_TUN_INC_CALL_EV,
  DS707_CBACK_TUN_PHYS_LINK_UP_EV,
  DS707_CBACK_TUN_PHYS_LINK_DOWN_EV, 
  DS707_CBACK_TUN_ABORT_SESSION_EV,
#endif /* FEATURE_DATA_OPTHO */
  DS707_CBACK_MAX_EV                       /* Used for bounds checking     */
} ds707_cback_event_type;

/*---------------------------------------------------------------------------
  Information sent along with an event.  Must always be sent with the event.
---------------------------------------------------------------------------*/
typedef struct
{
  ds707_so_type       so;
  uint32              pkt_inst;
  boolean             going_dormant; /* TRUE = go dormant. FALSE = go NULL */
  boolean             failed_orig;   /* TRUE = CM failed orig immediately  */ 
  cm_call_cmd_err_e_type call_cmd_err;/* the err code for cm cmd            */
  sys_sys_mode_e_type curr_network;  /* current data network               */
  sys_sys_mode_e_type new_network;   /* new data network                   */
  ds707_cm_call_end_info_type call_end_info; /*This end info is used by epzid module*/
  ps_iface_tear_down_client_data_info_type tear_down_info;   /* Call tear down info */
} ds707_cback_event_info_type;

/*---------------------------------------------------------------------------
  Callback prototype.
---------------------------------------------------------------------------*/
typedef void (*ds707_cback_func_type)
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
);


/*===========================================================================
                           EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_CBACK_INIT

DESCRIPTION   Initialize the callback array.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None,
===========================================================================*/
void ds707_cback_init(void);

/*===========================================================================
FUNCTION      DS707_CBACK_REGISTER

DESCRIPTION   Register a callback with an event.  A duplicate is detected
              if the callback pointer AND the user_data are the same.  May
              have a case where the same function is registered with
              different user_data, so that it executes different
              functionality.

DEPENDENCIES  None.

RETURN VALUE  TRUE:  callback inserted successfully
              FALSE: callback could not be inserted.

SIDE EFFECTS  None,
===========================================================================*/
boolean ds707_cback_register
(
  ds707_cback_event_type    event,
  ds707_cback_func_type     cback_ptr
);

/*===========================================================================
FUNCTION      DS707_CBACK_DEREGISTER

DESCRIPTION   Deregister a callback with an event.  To de-register, the
              callback func ptr AND the user data must match.
              
              Removes all matching occurences.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_cback_deregister
(
  ds707_cback_event_type   event,
  ds707_cback_func_type    cback_ptr
);

/*===========================================================================
FUNCTION      DS707_CBACK_POST_EVENT

DESCRIPTION   Calls callbacks registered for the event passed in as a param.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_cback_post_event
(
  ds707_cback_event_type               event,
  ds707_cback_event_info_type         *event_info_ptr
);

/*===========================================================================
FUNCTION      DS707_CBACK_GET_NEST_LEVEL

DESCRIPTION   Returns the nesting level

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None,
===========================================================================*/
uint32 ds707_cback_get_nest_level(void);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_CBACK        */
