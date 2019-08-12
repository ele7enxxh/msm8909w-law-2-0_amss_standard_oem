/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                        D S 7 0 7 _ C B A C K
                       
GENERAL DESCRIPTION
  This file contains the callback mechanism for the various 707 call control
  events.  These events are edge-trigger events (i.e, they will not check for
  current state when a user registers for them).

  These are assumed to be called from DS task context.

EXTERNALIZED FUNCTIONS

  DS707_CBACK_INIT
    Called at startup.  Intializes the internal callback array to be empty.
    
  DS707_CBACK_REGISTER
    Called by user to register a callback for a particular event.  
        
  DS707_CBACK_DEREGISTER
    De-registers any instances of the registered callback.  Only those entries
    which match the callback are removed.  
 
  DS707_CBACK_POST_EVENT
    Called by interested party when a particular event occurs.  Calls all
    registered callbacks.  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  DS must initializes ds707_cback_init first, before any other 707 modules
  are initialized.  Other modules will probably register callbacks in their
  initialization.

   Copyright (c) 2004-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_cback.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/26/13   sc      Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
02/09/12   msh     Coan feature cleanup 
03/15/11   mg      Global variable cleanup
10/14/10   sa      Adding debug messages before ASSERT.
10/13/10   op      Migrated to MSG 2.0 macros
07/27/09   spn     Added F3 MEssage when a CBACK event is received.
03/13/08   psng    Fixed off target medium and low lint errors.
07/01/08   psng    KlocWork Fixes
05/10/07   ac      Lint fix
05/16/05   gr      Changes to support PHYS LINK NULL functionality
03/01/05   vrk/sk  increased call backs (DS707_CBACK_MAX_CBACKS) from 10 - 12
08/06/04   ak      Added new string for pzid changed event.
04/22/04   ak      Lint changes.
04/09/04   ak      Code review fixes. Option to serialize events. 
02/04/04   ak      Initial version

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_IS707
#include "amssassert.h"
#include "ds707_cback.h"
#include "dstaski.h"
#include "err.h"
#include "msg.h"
#include "data_msg.h"
#include "data_err_suppr.h"


/*===========================================================================
                        TYPEDEFS and DEFINES
===========================================================================*/

/*---------------------------------------------------------------------------
  Array sizes.  Both are arbitrary.
---------------------------------------------------------------------------*/
#define DS707_CBACK_MAX_CBACKS     12 /* max cbacks per event              */

#define DS707_CBACK_MAX_NEST_LEVEL 10 /* max nseted events                 */

/*---------------------------------------------------------------------------
  Event holding callback function information.
---------------------------------------------------------------------------*/
typedef struct
{
  ds707_cback_func_type    cback_ptr;   /* callback to call                */
} ds707_cbacki_cback_info_type;

/*---------------------------------------------------------------------------
  If events cannot be nested, then this is used when serializing events.
---------------------------------------------------------------------------*/
typedef struct
{
  ds707_cback_event_type         event;
  ds707_cback_event_info_type    event_info;
} ds707_cbacki_event_info_type;

/*===========================================================================
                           VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
  This array contains the callbacks registered by clients in 707.  
---------------------------------------------------------------------------*/
static ds707_cbacki_cback_info_type ds707_cbacki_tbl[DS707_CBACK_MAX_EV]
                                             [DS707_CBACK_MAX_CBACKS];

/*---------------------------------------------------------------------------
  This holds the event buffer used to serialize events. 
---------------------------------------------------------------------------*/
static ds707_cbacki_event_info_type 
                         ds707_cbacki_events[DS707_CBACK_MAX_NEST_LEVEL + 1];

/*---------------------------------------------------------------------------
  Used to manage the event buf.  If after incrementing free_index they are
  equal, then buffer is full.  If after increasing cur_index they are
  equal, then buffer is empty.
---------------------------------------------------------------------------*/
static uint32  ds707_cbacki_free_event_index = 0; /* next empty spot       */
static uint32  ds707_cbacki_cur_event_index  = 0; /* event to process next */

/*---------------------------------------------------------------------------
  This variable is used to indicate the nesting level of callbacks and
  events.  It allows us to have a good idea of what our code is doing.
---------------------------------------------------------------------------*/
static uint32    ds707_cback_nest_level = 0;

/*---------------------------------------------------------------------------
  Strings used for F3 debug.  This must match enum exactly.
---------------------------------------------------------------------------*/
static const char*  ds707_cback_event_strings[DS707_CBACK_MAX_EV] = 
{
  "DS707 PHYS LINK COMING UP EVENT",
  "DS707 PHYS LINK RESUMING EVENT",
  "DS707 PHYS LINK INC CALL EVENT",

  "DS707 PHYS LINK UP EVENT",
  "DS707 PHYS LINK DOWN EVENT",

  "DS707 ABORT SESSION EVENT",

  "DS707 HOLDDOWN_TIMER_STARTED EVENT",
  "DS707 HOLDDOWN_TIMER_STOPPED EVENT",

  "DS707 EPZID CHANGED EVENT",
  "DS707 SID CHANGED EVENT",
  "DS707 NID CHANGED EVENT",

  "DS707 NEW NET EVENT",

  "DS707_CBACK_HANDOFF_EPC_TO_NONEPC",
  "DS707_CBACK_HANDOFF_NONEPC_TO_EPC"

#ifdef FEATURE_DATA_OPTHO
  /* TUNNEL PHYS LINK EVENTS NEEDED FOR OPTHO */

  ,"DS707_CBACK_TUN_PHYS_LINK_COMING_UP_EV",
  "DS707_CBACK_TUN_PHYS_LINK_RESUMING_EV",
  "DS707_CBACK_TUN_INC_CALL_EV",
  "DS707_CBACK_TUN_PHYS_LINK_UP_EV",
  "DS707_CBACK_TUN_PHYS_LINK_DOWN_EV", 
  "DS707_CBACK_TUN_ABORT_SESSION_EV"
#endif /* FEATURE_DATA_OPTHO */
};

/*===========================================================================
                           INTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================
                           EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_CBACK_INIT

DESCRIPTION   Initialize the callback array.  Called at power-up.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None,
===========================================================================*/
void ds707_cback_init(void)
{
  uint8 i,j;
/*-------------------------------------------------------------------------*/

  for (i = 0; i < (uint8) DS707_CBACK_MAX_EV; i++)
  {
    for (j = 0; j < (uint8) DS707_CBACK_MAX_CBACKS; j++)
    {
      ds707_cbacki_tbl[i][j].cback_ptr = NULL;
    }
  }


}

/*===========================================================================
FUNCTION      DS707_CBACK_REGISTER

DESCRIPTION   Register a callback with an event.  A duplicate is detected
              if the callback pointer is already registered for that
              particular event.
              
DEPENDENCIES  None.

RETURN VALUE  TRUE:  callback inserted successfully
              FALSE: callback could not be inserted.

SIDE EFFECTS  None,
===========================================================================*/
boolean ds707_cback_register
(
  ds707_cback_event_type   event,
  ds707_cback_func_type    cback_ptr
)
{
  int     i;
/*-------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Check input event for valid values.
  -------------------------------------------------------------------------*/
  if ((event <= DS707_CBACK_MIN_EV) ||
      (event >= DS707_CBACK_MAX_EV)
     )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,"Bad event passed %d",event);
    DS_FORCE_ASSERT();
    return(FALSE);
  }

  /*-------------------------------------------------------------------------
     See if the callback already exists in the list.
  -------------------------------------------------------------------------*/
  for (i = 0; i < DS707_CBACK_MAX_CBACKS; i++)
  {
    if (ds707_cbacki_tbl[event][i].cback_ptr == cback_ptr)
    {
      /*---------------------------------------------------------------------
        Re-adding same callback to list.  Do nothing.
      ---------------------------------------------------------------------*/
      return(TRUE);
    }
  }

  /*-------------------------------------------------------------------------
    If here, callback is not in list.  Find first NULL and put cback there.
  -------------------------------------------------------------------------*/
  for (i = 0; i < DS707_CBACK_MAX_CBACKS; i++)
  {
    if (ds707_cbacki_tbl[event][i].cback_ptr == NULL)
    {
      /*---------------------------------------------------------------------
        Inserting callback at first NULL.
      ---------------------------------------------------------------------*/
      ds707_cbacki_tbl[event][i].cback_ptr = cback_ptr;
      return(TRUE);
    }
  }

  /*-------------------------------------------------------------------------
    If code reaches here, then there is no space for the callback.
  -------------------------------------------------------------------------*/
  DS_FORCE_ASSERT();
  return(FALSE);
} /* ds707_cback_register() */

/*===========================================================================
FUNCTION      DS707_CBACK_DEREGISTER

DESCRIPTION   Deregister a callback with an event.  To de-register, the
              callback func ptr must match.
              
              Removes all matching occurences.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_cback_deregister
(
  ds707_cback_event_type   event,
  ds707_cback_func_type    cback_ptr
)
{
  int     i;
/*-------------------------------------------------------------------------*/
  if (IS_IN_DS_TASK() == FALSE)
  {
    DS_FORCE_ASSERT();
    return;
  }
  
  /*-------------------------------------------------------------------------
    Check input event for valid values.
  -------------------------------------------------------------------------*/
  if ((event <= DS707_CBACK_MIN_EV) ||
      (event >= DS707_CBACK_MAX_EV)
     )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,"Bad event passed %d",event);
    DS_FORCE_ASSERT();
    return;
  }

  /*-------------------------------------------------------------------------
    Remove the matching entry.
  -------------------------------------------------------------------------*/
  for (i = 0; i < DS707_CBACK_MAX_CBACKS; i++)
  {
    if (ds707_cbacki_tbl[event][i].cback_ptr == cback_ptr)
    {
      DATA_IS707_MSG1(MSG_LEGACY_MED, "Removing cback at pos %d",i);
      ds707_cbacki_tbl[event][i].cback_ptr = NULL;
      break;
    }
  }
} /* ds707_cback_deregister() */


/*===========================================================================
FUNCTION      DS707_CBACK_POST_EVENT

DESCRIPTION   Calls callbacks registered for the event passed in as a param.

              Depending on how DS707_CBACK_SERIAL_EVENTS is #defined, this
              may result in nested events or may result in serial events.
              
              In either case, this function will process all at once.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_cback_post_event
(
  ds707_cback_event_type           event,
  ds707_cback_event_info_type     *event_info_ptr
)
{
  int     i;
  ds707_cback_event_type           loc_event;
  ds707_cback_event_info_type      loc_event_info;
/*-------------------------------------------------------------------------*/
#ifndef TEST_FRAMEWORK
  if (IS_IN_DS_TASK() == FALSE)
  {
    ERR_FATAL("must be in DS task to post event",0,0,0);
  }
#endif /* TEST_FRAMEWORK */

  /* Lint error fix */
  DATA_MDM_ARG_NOT_CONST( event_info_ptr );
  DATA_IS707_MSG1(MSG_LEGACY_MED, "DS707_CBACK: Post Event: %d",event);
  
  /*-------------------------------------------------------------------------
    Check input event for valid values.
  -------------------------------------------------------------------------*/
  if ((event <= DS707_CBACK_MIN_EV) ||
      (event >= DS707_CBACK_MAX_EV)
     )
  {
    DS_FORCE_ASSERT();
    return;
  }

  if (event_info_ptr == NULL)
  {
    DS_FORCE_ASSERT();
    return;
  }

  /*-------------------------------------------------------------------------
    Store event for serialized event processing.
  -------------------------------------------------------------------------*/
  /*lint -save -e507
   *               SPRINTF has a 6 byte arg; we use a 4 byte arg
   */
  DATA_IS707_MSG_SPRINTF_1(
                MSG_LEGACY_HIGH, 
                "Store event:  %s", 
                ds707_cback_event_strings[(uint32)event]);

  /*lint -restore */


  ds707_cbacki_events[ds707_cbacki_free_event_index].event = event;
  memscpy(&(ds707_cbacki_events[ds707_cbacki_free_event_index].event_info),
          sizeof(ds707_cback_event_info_type), 
          event_info_ptr,
          sizeof(ds707_cback_event_info_type));

  ds707_cbacki_free_event_index = (ds707_cbacki_free_event_index + 1) %
                                                  DS707_CBACK_MAX_NEST_LEVEL;

  /*-------------------------------------------------------------------------
    If the free_event_index catches up to the cur_event, then we have an
    overflow!
  -------------------------------------------------------------------------*/
  if (ds707_cbacki_free_event_index == ds707_cbacki_cur_event_index)
  {
    ERR_FATAL("too many events",0,0,0);
  }

  /*-------------------------------------------------------------------------
    If this is a nested event, then don't process here.
  -------------------------------------------------------------------------*/
  if (ds707_cback_get_nest_level() > 0)
  {
    return;
  }
  
  /*-------------------------------------------------------------------------
    Increment the nest level.
  -------------------------------------------------------------------------*/
  ds707_cback_nest_level++;

  do
  {

    /*-----------------------------------------------------------------------
      Get the event info from the event buf.
    -----------------------------------------------------------------------*/
    loc_event = ds707_cbacki_events[ds707_cbacki_cur_event_index].event;

    memscpy(&(loc_event_info),
	    sizeof(ds707_cback_event_info_type),
            &(ds707_cbacki_events[ds707_cbacki_cur_event_index].event_info),
            sizeof(ds707_cback_event_info_type));

    ds707_cbacki_cur_event_index = (ds707_cbacki_cur_event_index + 1) %
                                                  DS707_CBACK_MAX_NEST_LEVEL;
    /*-----------------------------------------------------------------------
      Print out a debug message.
    -----------------------------------------------------------------------*/
    /*lint -save -e507
     *               SPRINTF has a 6 byte arg; we use a 4 byte arg
     */

    if( loc_event >= DS707_CBACK_MAX_EV || loc_event <= DS707_CBACK_MIN_EV )
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Bad Event Value: %d", loc_event);
      continue;
    }
    DATA_IS707_MSG_SPRINTF_2( 
                  MSG_LEGACY_HIGH, 
                  "%d. proc event %s", 
                  ds707_cback_get_nest_level(),
                  ds707_cback_event_strings[(uint32)loc_event]);

    /*lint -restore */

    /*-----------------------------------------------------------------------
      Call any registered functions for the specified event.
    -----------------------------------------------------------------------*/
    for (i = 0; i < DS707_CBACK_MAX_CBACKS; i++)
    {
      if (ds707_cbacki_tbl[loc_event][i].cback_ptr != NULL)
      {
        ds707_cbacki_tbl[loc_event][i].cback_ptr(
                                                  loc_event,
                                                  &(loc_event_info)
                                                );
      }
    }
  } while (ds707_cbacki_cur_event_index != ds707_cbacki_free_event_index);


  /*-------------------------------------------------------------------------
    Decrement the nest level.
  -------------------------------------------------------------------------*/
  ds707_cback_nest_level--;

} /* ds707_cback_post_event() */  

/*===========================================================================
FUNCTION      DS707_CBACK_GET_NEST_LEVEL 

DESCRIPTION   Returns the nesting level

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None,
===========================================================================*/
uint32 ds707_cback_get_nest_level(void)
{
  return ds707_cback_nest_level;
}

#endif /* FEATURE_DATA_IS707 */
