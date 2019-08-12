#ifndef PS_FLOW_EVENT_H
#define PS_FLOW_EVENT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ F L O W _ E V E N T S . H

DESCRIPTION
  This is the header file for the PS flow event functions.

EXTERNAL FUNCTIONS
  PS_FLOW_ALLOC_EVENT_CBACK_BUF()
    Allocates memory used to register for ps_flow event callbacks

  PS_FLOW_FREE_EVENT_CBACK_BUF()
    Free the memory used to register for ps_flow event callbacks

  PS_FLOW_EVENT_CBACK_REG()
    Registers callbacks for events on a flow

  PS_FLOW_EVENT_CBACK_DEREG()
    Deregisters callbacks for a specific event on an flow

Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_flow_event.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

  when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/14/08    pp     Created module as part of Common Modem Interface:
                   Public/Private API split.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ps_iface_defs.h"



/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
TYPEDEF PS_FLOW_EVENT_CBACK_TYPE

DESCRIPTION
  Definition of event callback function that can be registered with PS.
  There can be any number of events per flow and the memory is owned by the
  clients.

PARAMS
  flow_ptr      : ptr to flow
  event         : the event being registered for
  event_info    : information specific to the event: for details look at
                  typedef
  user_data_ptr : data to pass back to the user, which is passed during event
                  registration

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
typedef void (*ps_flow_event_cback_type)
(
  ps_flow_type                * flow_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * user_data_ptr
);

/*===========================================================================
TYPEDEF PS_FLOW_ASSOC_FLOW_EVENT_CBACK_TYPE

DESCRIPTION
  Definition of event callback function that can be registered for
  handling events on the associated flow.

PARAMS
  flow_ptr      : ptr to flow
  event         : the event being registered for
  event_info    : information specific to the event: for details look at
                  typedef
  user_data_ptr : data to pass back to the user, which is passed during event
                  registration

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
typedef void (*ps_flow_assoc_flow_event_cback_type)
(
  ps_flow_type                * assoc_flow_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * user_data_ptr
);


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_FLOW_ALLOC_EVENT_CBACK_BUF()

DESCRIPTION
  This function is used to allocate the memory used to register for ps_flow
  event callbacks.

PARAMETERS
  event_cback_f_ptr : the callback to be registered
  user_data_ptr     : data to be passed back with the callback.

RETURN VALUE
  ptr to buffer : on success
  NULL          : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void *ps_flow_alloc_event_cback_buf
(
  ps_flow_event_cback_type    event_cback_f_ptr,
  void                      * user_data_ptr
);

/*===========================================================================
FUNCTION PS_FLOW_FREE_EVENT_CBACK_BUF()

DESCRIPTION
  This function is used to free the memory used to register for ps_flow
  event callbacks.

PARAMETERS
  buf_ptr : the previously allocated event buffer.

RETURN VALUE
  None

DEPENDENCIES
  Event buffer MUST be valid

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_free_event_cback_buf
(
  void  * buf_ptr
);

/*===========================================================================
FUNCTION PS_FLOW_EVENT_CBACK_REG()

DESCRIPTION
  Registers callbacks for events on a flow.  Whenever an event occurs,
  the callback for that event is called.  If the event is true when
  registration happens the callback is called immediately.  This will fail if
  either the flow is invalid, or the buffer to queue (with callback and
  user data pointer) is NULL.  A callback will only be queued if the event
  has not previously been queued.

  Memory is owned by the caller and it is put into the queue for that
  event.

PARAMETERS
  flow_ptr : ptr to flow on which to operate on.
  event    : event that triggers the callbacks
  buf_ptr  : buffer that holds the callback pointer. Should not be NULL.

RETURN VALUE
   0 : on success
  -1 : on failure (any of the passed in parameters is bad)

DEPENDENCIES
  buf_ptr MUST be allocated using ps_flow_alloc_event_cback_buf()

SIDE EFFECTS
  None
===========================================================================*/
int ps_flow_event_cback_reg
(
  ps_flow_type              * flow_ptr,
  ps_iface_event_enum_type    event,
  void                      * buf_ptr
);

/*===========================================================================
FUNCTION PS_FLOW_EVENT_CBACK_DEREG()

DESCRIPTION
  DeRegisters callbacks for a specific event on an flow.

PARAMETERS
  flow_ptr : ptr to flow control block on which to operate on.
  event    : event that triggers the callbacks
  buf_ptr  : buffer that holds the callback pointer. This buffer is removed
             from the queue of callbacks.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_event_cback_dereg
(
  ps_flow_type              * flow_ptr,
  ps_iface_event_enum_type    event,
  void                      * buf_ptr
);

#ifdef __cplusplus
}
#endif

#endif /* PS_FLOW_EVEN_H */
