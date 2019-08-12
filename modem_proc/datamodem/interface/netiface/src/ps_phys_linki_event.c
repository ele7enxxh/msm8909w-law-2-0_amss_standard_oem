/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    P S _ P H Y S _ L I N K I _ E V E N T . C


GENERAL DESCRIPTION
  This is the implementation of the PS phys link event functions. This
  file implement functions used for phys_link events.

EXTERNAL FUNCTIONS

  PS_PHYS_LINK_EVENT_INIT()
    Called at power-up to initialize event arrays, etc.

  PS_PHYS_LINK_ALLOC_EVENT_CBACK_BUF
    Called by client to allocate a buffer used when registering for phys link
    events.

  PS_PHYS_LINK_FREE_EVENT_CBACK_BUF
    Called by client to free a buffer which was used when registering for
    phys link events.

  PS_PHYS_LINK_EVENT_CBACK_REG
    Called by client when registering for phys link events.

  PS_PHYS_LINK_EVENT_CBACK_DEREG
    Called by client when de-registering for phys link events.

  PS_PHYS_LINK_DOWN_IND
    Can only be called by phys link owner!!!  Indicates that the phys link
    down.

  PS_PHYS_LINK_UP_IND
    Can only be called by phys link owner!!!  Indicates that the phys link
    up.

  PS_PHYS_LINK_GON_IND
    Can only be called by phys link owner!!!  Indicates that the phys link
    has gone NULL (i.e, physical layer indicates end of the data session).

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Must call ps_phys_link_event_init() at startup.

Copyright (c) 2003-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_phys_linki_event.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
11/30/11    su     Adding log code support for DPL logging  
12/08/10    rp     RAM optimization changes.
06/19/10    vs     Increasing phys link ev buffer due to QMI Dual IP support
03/26/09    pp     CMI De-featurization.
02/01/07    msr    Added ps_phys_link_dos_ack_ind()
09/12/06    msr    Removed redundant state field in event_info structure
05/10/06    rt     Fixed a typo in ps_phys_link_down_ind_ex().
04/25/06    msr    L4/Tasklock code review changes
02/22/06    msr    Using single critical section
02/06/06    msr    Updated for L4 tasklock/crit sections.
10/18/05    msr    Removed support for FLOW_ACTIVATE_MODIFIED_EV and
                   PHYS_LINK_UP_MODIFIED_EV
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
05/12/05    mct    Lint changes.
04/17/05    msr    Changed ps_phys_link_gone_ind() to ensure that all flows
                   are released.
04/16/05    ks     Added changes for PHYS_LINK_NULL state.
11/19/04    msr    Added F3 messages to indications.
11/17/04    ks     Setting event_info.link_state to PHYS_LINK_DOWN for
                   PHYS_LINK_GONE_EV in PS_PHYS_LINK_EVENT_CBACK_REG().
11/02/04    msr    Checking if ps_mem_get_buf succeeded in
                   ps_phys_link_alloc_event_cback_buf().
10/06/04    sv     Added phys_link_down_ind_ex function.
08/12/04    sv     Enable physlink flow upon up/down_ind()
05/10/04    mct    Fixed lint errors.
05/21/04    aku    Notify clients about PHYS_LINK_DOWN_EV when processing the
                   GONE indication.
12/30/03    ak     Extended for IFACE/PHYS LINK separation.
11/03/03    aku    Added Null check in ps_iface_alloc_event_cback_buf() after
                   call to ps_mem_get_buf().
10/21/03    usb    Added required included files.
10/14/03    mct    Added callbacks for the coming_up, going_down, and
                   phys_link equivalent events.
08/25/03    aku    ps_mem_free() takes pointer-to-pointer as arg.
08/15/03    aku    created file.
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "amssassert.h"
#include "err.h"
#include "msg.h"
#include "ps_utils.h"
#include "ps_crit_sect.h"
#include "ps_iface_defs.h"
#include "ds_flow_control.h"
#include "ps_mem.h"
#include "ps_phys_link.h"
#include "ps_phys_linki_event.h"
#include "ps_ifacei_event.h"
#include "queue.h"
#include "ds_Utils_DebugMsg.h"

#include "ps_logging_diag.h"


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           LOCAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  Tuning the number of ps phys link event buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_PHYS_LINK_EVT_BUF_SIZE  ((sizeof(ps_phys_link_event_buf_type) + 3) & ~3)

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET

/* We need 5 phys link evt buffers for each additional QMI instance.
 * For Fusion, 8 additional QMI instances have been added to for SDIO
 * Hence increasing phys link event buffers by 50 */
#if defined(FEATURE_DATA_WLAN_MAPCON) || defined(FEATURE_RMNET_PORT_CONFIG_MDM_LE)
/* For ePDG, 9 new ports are added, hence increasing buffers by 50 */
#define PS_PHYS_LINK_EVT_BUF_NUM          500
#define PS_PHYS_LINK_EVT_BUF_HIGH_WM      400
#else
#define PS_PHYS_LINK_EVT_BUF_NUM          335
#define PS_PHYS_LINK_EVT_BUF_HIGH_WM      250
#endif /* FEATURE_DATA_WLAN_MAPCON */
#define PS_PHYS_LINK_EVT_BUF_LOW_WM        90

#else

#define PS_PHYS_LINK_EVT_BUF_NUM           85
#define PS_PHYS_LINK_EVT_BUF_HIGH_WM       75
#define PS_PHYS_LINK_EVT_BUF_LOW_WM        50
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_phys_link_evt along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_phys_link_evt_buf_mem[PS_MEM_GET_TOT_SIZE
                                    (
                                      PS_PHYS_LINK_EVT_BUF_NUM,
                                      PS_PHYS_LINK_EVT_BUF_SIZE
                                    )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ps_phys_link_evt_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type         * ps_phys_link_evt_buf_hdr[PS_PHYS_LINK_EVT_BUF_NUM];
static ps_phys_link_event_buf_type * ps_phys_link_evt_buf_ptr[PS_PHYS_LINK_EVT_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  GLOBAL_PHYS_LINK_EVENT_Q_ARRAY - this is used to store callbacks that are
    intended to be called when events happen on ANY phys_link.
---------------------------------------------------------------------------*/
q_type  global_phys_link_event_q_array[PHYS_LINK_MAX_EV];



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PS_PHYS_LINK_EVENT_INIT()

DESCRIPTION
  This function initializes the global queues for ps_phys_link.  It also
  registers the ps_iface phys_link function on the global phys_link queue.

  Note: Currently, only the events in ps_iface need initialization. Hence,
  the implementation of this function resides in ps_ifacei_event.c.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_event_init
(
  void
)
{
  int loop;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize Pools
  -------------------------------------------------------------------------*/
  if (ps_mem_pool_init(PS_MEM_PS_PHYS_LINK_EVT_CB_TYPE,
                       ps_phys_link_evt_buf_mem,
                       PS_PHYS_LINK_EVT_BUF_SIZE,
                       PS_PHYS_LINK_EVT_BUF_NUM,
                       PS_PHYS_LINK_EVT_BUF_HIGH_WM,
                       PS_PHYS_LINK_EVT_BUF_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) ps_phys_link_evt_buf_hdr,
                       (int *) ps_phys_link_evt_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    LOG_MSG_ERROR_0("ps_phys_link_event_init(): "
                    "Can't init the module");
  }

  /*-------------------------------------------------------------------------
    Initialize global event queues
  -------------------------------------------------------------------------*/
  for (loop = 0; loop < PHYS_LINK_MAX_EV; loop++)
  {
    (void)q_init(global_phys_link_event_q_array + loop);
  }
} /* ps_phys_link_event_init() */

/*===========================================================================
FUNCTION PS_PHYS_LINKI_GET_EVENT_Q_ARRAY_INDEX()

DESCRIPTION
  This function returns the event queue number for given event. 
 
PARAMETERS
  event      : phys link event
 
RETURN VALUE
  Returns the queue number

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_phys_linki_get_event_q_array_index
(
  ps_iface_event_enum_type event
)
{
  if (event == PHYS_LINK_FLOW_ENABLED_EV || event == PHYS_LINK_FLOW_DISABLED_EV)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}



/*===========================================================================
FUNCTION PS_PHYS_LINK_ALLOC_EVENT_CBACK_BUF()

DESCRIPTION
  This function is used to allocate the memory used to register for phys_link
  event callbacks.

PARAMETERS
  event_cback_f_ptr: the callback to be registered
  user_data_ptr: data to be called with the callback.

RETURN VALUE
  NULL: if allocation failed
  ptr to memory being allocated.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void *ps_phys_link_alloc_event_cback_buf
(
  ps_phys_link_event_cback_type event_cback_f_ptr,
  void                          *user_data_ptr
)
{
  ps_phys_link_event_buf_type *buf_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(event_cback_f_ptr == NULL)
  {
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Allocate a new event callback buffer.
  -------------------------------------------------------------------------*/
  buf_ptr= (ps_phys_link_event_buf_type *)
                             ps_mem_get_buf(PS_MEM_PS_PHYS_LINK_EVT_CB_TYPE);

  if (NULL != buf_ptr)
  {
    /*-----------------------------------------------------------------------
    Initialize the event buffer
    -----------------------------------------------------------------------*/
    (void)q_link(buf_ptr, &(buf_ptr->link));
    buf_ptr->event_cback_f_ptr = event_cback_f_ptr;
    buf_ptr->user_data_ptr     = user_data_ptr;
    buf_ptr->event_q_ptr       = NULL;
  }

  return buf_ptr;

} /* ps_phys_link_alloc_event_cback_buf() */

/*===========================================================================
FUNCTION PS_PHYS_LINK_FREE_EVENT_CBACK_BUF()

DESCRIPTION
  This function is used to free the memory used to register for phys_link
  event callbacks.

PARAMETERS
  buf: the previously allocated buffer.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_free_event_cback_buf
(
  void *buf
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(buf == NULL)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Validate the buffer that is passed in not already on a queue
  -------------------------------------------------------------------------*/
  if(Q_ALREADY_QUEUED(&(((ps_phys_link_event_buf_type*)buf)->link)))
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Invalidate the buffer so that the caller cannot use it without
    explicitly allcating it again.
  -------------------------------------------------------------------------*/
  ((ps_phys_link_event_buf_type*)buf)->event_cback_f_ptr = NULL;
  PS_MEM_FREE(buf);

} /* ps_phys_link_free_event_cback_buf() */


/*===========================================================================
FUNCTION PS_PHYS_LINK_EVENT_CBACK_REG()

DESCRIPTION
  Registers callbacks for events on a phys link.  Whenever an event occurs,
  the callback for that event is called.  If the event is true when
  registration happens the callback is called immediately.  This will fail if
  either the phys link is invalid, or the buffer to queue (with callback and
  user data pointer) is NULL.  A callback will only be queued if the event
  has not previously been queued.

  Memory is owned by the caller and it is put into the queue for that
  event.

PARAMETERS
  this_phys_link_ptr: ptr to phys link control block on which to operate on.
  event:  Event that triggers the callbacks
  buf:    Buffer that holds the callback pointer. Should not be NULL.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  The link field must have been initialized with q_link().

SIDE EFFECTS
  None
===========================================================================*/
int ps_phys_link_event_cback_reg
(
  ps_phys_link_type       *this_phys_link_ptr,
  ps_iface_event_enum_type event,
  void                    *buf
)
{
  ps_iface_event_info_u_type      event_info;
  ps_phys_link_event_buf_type    *buf_ptr;
  boolean                         need_to_invoke_cback = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(buf == NULL || event >= PHYS_LINK_MAX_EV)
  {
    ASSERT(0);
    return -1;
  }

  buf_ptr = (ps_phys_link_event_buf_type *) buf;

  /*-------------------------------------------------------------------------
    Validate the buffer that is passed in
  -------------------------------------------------------------------------*/
  if(ps_mem_is_valid(buf, PS_MEM_PS_PHYS_LINK_EVT_CB_TYPE) == FALSE)
  {
    LOG_MSG_INFO1_2("ps_phys_link_event_cback_reg(): "
                    "Invalid phy link buf 0x%p to reg evt %d", buf, event);
    return -1;

  }

  /*-------------------------------------------------------------------------
    make sure that this is not already queued - if not mark it as such
  -------------------------------------------------------------------------*/
  if(Q_ALREADY_QUEUED(&(buf_ptr->link)))
  {
    LOG_MSG_INFO1_2("ps_phys_link_event_cback_reg(): "
                    "Not re-q'ing phys link buf 0x%p, evt %d", buf, event);
    return -1;
  }

  /*-------------------------------------------------------------------------
    As the contents of the buffer are supposed to be set by ps_iface
    functions it should NEVER have a NULL callback
  -------------------------------------------------------------------------*/
  if(buf_ptr->event_cback_f_ptr == NULL)
  {
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    is this for the global queue?
  -------------------------------------------------------------------------*/
  if(this_phys_link_ptr == NULL)
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    buf_ptr->event_q_ptr = global_phys_link_event_q_array + event;
    buf_ptr->event_name = event;
    q_put(global_phys_link_event_q_array + event, &(buf_ptr->link));
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    return 0;
  }

  /*-------------------------------------------------------------------------
    if not the global queue then make sure that the interface is valid
  -------------------------------------------------------------------------*/
  if (!PS_PHYS_LINK_IS_VALID(this_phys_link_ptr))
  {
    LOG_MSG_ERROR_1("ps_phys_link_event_cback_reg(): "
                    "Invalid phys link, 0x%p, is passed", this_phys_link_ptr);
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  buf_ptr->event_q_ptr = 
    &(this_phys_link_ptr->phys_private.event_q_array[ps_phys_linki_get_event_q_array_index(event)]);
  buf_ptr->event_name = event;
  q_put(buf_ptr->event_q_ptr,&(buf_ptr->link));

  /*-------------------------------------------------------------------------
    Initializing "event_info" so that lint won't crib
  -------------------------------------------------------------------------*/
  event_info.phys_link_event_info.state = PHYS_LINK_DOWN;

  /*-------------------------------------------------------------------------
    check if the event is true - if so call the callback immediately
  -------------------------------------------------------------------------*/
  switch (event)
  {
    /*-----------------------------------------------------------------------
                            PHYS_LINK_DOWN_EV
    -----------------------------------------------------------------------*/
    case PHYS_LINK_DOWN_EV:
      if (PS_PHYS_LINKI_GET_STATE(this_phys_link_ptr) == PHYS_LINK_DOWN)
      {
        event_info.phys_link_event_info.state     = PHYS_LINK_DOWN;
        event_info.phys_link_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                      = TRUE;
      }
      break;

    /*-----------------------------------------------------------------------
                          PHYS_LINK_COMING_UP_EV
    -----------------------------------------------------------------------*/
    case PHYS_LINK_COMING_UP_EV:
      if (PS_PHYS_LINKI_GET_STATE(this_phys_link_ptr) == PHYS_LINK_COMING_UP)
      {
        event_info.phys_link_event_info.state     = PHYS_LINK_COMING_UP;
        event_info.phys_link_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                      = TRUE;
      }
      break;

    /*-----------------------------------------------------------------------
                          PHYS_LINK_UP_EV
    -----------------------------------------------------------------------*/
    case PHYS_LINK_UP_EV:
      if (PS_PHYS_LINKI_GET_STATE(this_phys_link_ptr) == PHYS_LINK_UP)
      {
        event_info.phys_link_event_info.state     = PHYS_LINK_UP;
        event_info.phys_link_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                      = TRUE;
      }
      break;

    /*-----------------------------------------------------------------------
                         PHYS_LINK_GOING_DOWN_EV
    -----------------------------------------------------------------------*/
    case PHYS_LINK_GOING_DOWN_EV:
      if (PS_PHYS_LINKI_GET_STATE(this_phys_link_ptr) == PHYS_LINK_GOING_DOWN)
      {
        event_info.phys_link_event_info.state     = PHYS_LINK_GOING_DOWN;
        event_info.phys_link_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                      = TRUE;
      }
      break;

    /*-----------------------------------------------------------------------
                         PHYS_LINK_GONE_EV
    -----------------------------------------------------------------------*/
    case PHYS_LINK_GONE_EV:
      if (PS_PHYS_LINKI_GET_STATE(this_phys_link_ptr) == PHYS_LINK_NULL)
      {
        event_info.phys_link_event_info.state     = PHYS_LINK_NULL;
        event_info.phys_link_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                      = TRUE;
      }
      break;

    /*-----------------------------------------------------------------------
                         PHYS_LINK_GOING_NULL_EV
    -----------------------------------------------------------------------*/
    case PHYS_LINK_GOING_NULL_EV:
      if (PS_PHYS_LINKI_GET_STATE(this_phys_link_ptr) == PHYS_LINK_GOING_NULL)
      {
        event_info.phys_link_event_info.state     = PHYS_LINK_GOING_NULL;
        event_info.phys_link_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                      = TRUE;
      }
      break;

    /*-----------------------------------------------------------------------
                         PHYS_LINK_RESUMING_EV
    -----------------------------------------------------------------------*/
    case PHYS_LINK_RESUMING_EV:
      if (PS_PHYS_LINKI_GET_STATE(this_phys_link_ptr) == PHYS_LINK_RESUMING)
      {
        event_info.phys_link_event_info.state     = PHYS_LINK_RESUMING;
        event_info.phys_link_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                      = TRUE;
      }
      break;

    /*-----------------------------------------------------------------------
                          PHYS_LINK_FLOW_ENABLED_EV
    -----------------------------------------------------------------------*/
    case PHYS_LINK_FLOW_ENABLED_EV:
      if (PS_PHYS_LINKI_FLOW_ENABLED(this_phys_link_ptr))
      {
        event_info.flow_mask = ALL_FLOWS_ENABLED;
        need_to_invoke_cback = TRUE;
      }
      break;

    /*-----------------------------------------------------------------------
                         PHYS_LINK_FLOW_DISABLED_EV
    -----------------------------------------------------------------------*/
    case PHYS_LINK_FLOW_DISABLED_EV:
      if (!PS_PHYS_LINKI_FLOW_ENABLED(this_phys_link_ptr))
      {
        event_info.flow_mask = ps_phys_link_get_flow_mask(this_phys_link_ptr);
        need_to_invoke_cback = TRUE;
      }
      break;

    default:
      need_to_invoke_cback = FALSE;
      break;

  } /* switch(event) */

  /*-------------------------------------------------------------------------
    If the callback needs to be called, do so
  -------------------------------------------------------------------------*/
  if(need_to_invoke_cback)
  {
    buf_ptr->event_cback_f_ptr(this_phys_link_ptr,
                               event,
                               event_info,
                               buf_ptr->user_data_ptr);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;

} /* ps_phys_link_event_cback_reg() */



/*===========================================================================
FUNCTION PS_PHYS_LINK_EVENT_CBACK_DEREG()

DESCRIPTION
  DeRegisters callbacks for a specific event on an phys link.

PARAMETERS
  this_phys_link_ptr: ptr to phys link control block on which to operate on.
  event:  Event that triggers the callbacks
  buf:    Buffer that holds the callback pointer. This buffer is removed
          from the queue of callbacks. Should not be NULL.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_event_cback_dereg
(
  ps_phys_link_type       *this_phys_link_ptr,
  ps_iface_event_enum_type event,
  void                    *buf
)
{
  q_type                       * working_q_ptr = NULL;
  ps_phys_link_event_buf_type  * buf_ptr;
  boolean                        quit          = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(event >= PHYS_LINK_MAX_EV || buf == NULL)
  {
    ASSERT(0);
    return;
  }

  buf_ptr = (ps_phys_link_event_buf_type*)buf;

  /*-------------------------------------------------------------------------
    Validate the buffer that is passed in
  -------------------------------------------------------------------------*/
  if(ps_mem_is_valid(buf, PS_MEM_PS_PHYS_LINK_EVT_CB_TYPE) == FALSE)
  {
    LOG_MSG_INFO1_2("ps_phys_link_event_cback_dereg(): "
                    "Invalid buf 0x%p to dereg evt %d", buf, event);
    return;
  }

  /*-------------------------------------------------------------------------
    make sure that this event is on a queue - if not mark it as such
  -------------------------------------------------------------------------*/
  if(!(Q_ALREADY_QUEUED(&(buf_ptr->link))))
  {
    LOG_MSG_INFO2_2("ps_phys_link_event_cback_dereg(): "
                    "Buf 0x%p for event %d not on a queue", buf, event);
    return;
  }

  /*-------------------------------------------------------------------------
    As the contents of the buffer are supposed to be set by ps_iface
    functions it should NEVER have a NULL callback
  -------------------------------------------------------------------------*/
  if(buf_ptr->event_cback_f_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    If this registration is for global callbacks, then use the global
    variable as the working pointer, otherwise use the q from the iface
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if(this_phys_link_ptr == NULL)
  {
    working_q_ptr = global_phys_link_event_q_array + event;
  }
  else if(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr))
  {
    working_q_ptr = 
      &(this_phys_link_ptr->phys_private.event_q_array[ps_phys_linki_get_event_q_array_index(event)]);
  }
  else
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    ASSERT(0);
    LOG_MSG_ERROR_1("ps_phys_link_event_cback_dereg(): "
                    "Invalid phys_link 0x%p is passed", this_phys_link_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Make sure the event buf is either on an event queue of the specified
    iface or on the gloabl event queue
  -------------------------------------------------------------------------*/
  if(working_q_ptr != buf_ptr->event_q_ptr)
  {
    ASSERT(0);
    LOG_MSG_ERROR_3("ps_phys_link_event_cback_dereg(): "
                    "Buf 0x%p (ev %d) not reg 0x%p phys_link",
                    buf, event, this_phys_link_ptr);
    quit = TRUE;
  }

  /*-------------------------------------------------------------------------
    make sure we have a valid queue to remove from - if so use the queue
    function to remove the item from the queue.
  -------------------------------------------------------------------------*/
  if(quit == FALSE)
  {
#ifdef FEATURE_Q_NO_SELF_QPTR
    q_delete(working_q_ptr, &(buf_ptr->link));
#else
    q_delete(&(buf_ptr->link));
#endif
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return;

} /* ps_phys_link_event_cback_dereg() */


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                                 INDICATIONS

  These are intended to only be called by the interface clents/owners.  While
  there is no way to enforce this, having someone else call the indications
  is not supported behavior.
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PS_PHYS_LINK_DOWN_IND()

DESCRIPTION
  The physical link of the given interface is down (707 dormancy).  Call
  callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_down_ind
(
  ps_phys_link_type *this_phys_link_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_phys_link_down_ind_ex(this_phys_link_ptr, PS_EIC_NOT_SPECIFIED);

} /* ps_phys_link_down_ind() */



/*===========================================================================
FUNCTION PS_PHYS_LINK_UP_IND()

DESCRIPTION
  The physical link of the given interface is up.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_up_ind
(
  ps_phys_link_type *this_phys_link_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_phys_link_up_ind_ex(this_phys_link_ptr, PS_EIC_NOT_SPECIFIED);
  return;
} /* ps_phys_link_up_ind() */


/*===========================================================================
FUNCTION PS_PHYS_LINK_GONE_IND()

DESCRIPTION
  The physical link of the given interface has been aborted, indicating
  that the session has gone NULL.  This is called in lieu of
  phys link_down_ind().

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_phys_link_ptr: ptr to interface control block on which to operate
  on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  Note that the clients that have registered for the PHYS_LINK_DOWN callback
  also get notified that the phys link has transitioned to the down state.
===========================================================================*/
void ps_phys_link_gone_ind
(
  ps_phys_link_type *this_phys_link_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_phys_link_gone_ind_ex(this_phys_link_ptr, PS_EIC_NOT_SPECIFIED);
  return;
} /* ps_phys_link_gone_ind() */



/*===========================================================================
FUNCTION PS_PHYS_LINK_DOS_ACK_IND()

DESCRIPTION
  Indicates that a packet sent over an access channel is acknowledged by the
  network

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  phys_link_ptr           : ptr to a phys link
  dos_ack_handle          : handle to the DOS packet
  dos_ack_status_info_ptr : status of DOS transmission

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_dos_ack_ind
(
  ps_phys_link_type                      * phys_link_ptr,
  int32                                    dos_ack_handle,
  ps_phys_link_dos_ack_status_info_type  * dos_ack_status_info_ptr
)
{
  ps_iface_event_info_u_type  event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!(PS_PHYS_LINK_IS_VALID(phys_link_ptr)))
  {
    ASSERT(0);
    return;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  LOG_MSG_INFO2_1("ps_phys_link_dos_ack_ind(): "
                  "PHYS LINK DOS ACK IND 0x%p", phys_link_ptr);

  memset( &event_info, 0, sizeof(event_info) );

  event_info.dos_ack_info.handle      = dos_ack_handle;
  event_info.dos_ack_info.status_info = *dos_ack_status_info_ptr;

  ps_ifacei_invoke_event_cbacks( NULL,
                                 phys_link_ptr,
                                 PHYS_LINK_707_DOS_ACK_EV,
                                 event_info);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_phys_link_dos_ack_ind() */



/*===========================================================================
FUNCTION PS_PHYS_LINK_DOWN_IND_EX()

DESCRIPTION
  The physical link of the given interface is down (707 dormancy).  Call
  callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_down_ind_ex
(
  ps_phys_link_type *this_phys_link_ptr,
  ps_extended_info_code_enum_type info_code
)
{
  ps_iface_event_info_u_type event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr)))
  {
    ASSERT(0);
    return;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  LOG_MSG_INFO2_2("ps_phys_link_down_ind_ex(): "
                  "PHYS LINK DOWN IND 0x%p, state %d",
                  this_phys_link_ptr,
                  PS_PHYS_LINKI_GET_STATE(this_phys_link_ptr));

  /*-------------------------------------------------------------------------
    store the previous state, change the current link state to DOWN and call
    the callbacks.
  -------------------------------------------------------------------------*/
  event_info.phys_link_event_info.state =
    PS_PHYS_LINK_GET_STATE(this_phys_link_ptr);
  event_info.phys_link_event_info.info_code = info_code;

  if (PS_PHYS_LINKI_GET_CAPABILITY(this_phys_link_ptr,
                                   PS_PHYS_LINK_CAPABILITY_PRIMARY))
  {
    this_phys_link_ptr->dormancy_info_code = info_code;
  }

  PS_PHYS_LINKI_SET_STATE(this_phys_link_ptr, PHYS_LINK_DOWN);

  ps_ifacei_invoke_event_cbacks( NULL,
                                 this_phys_link_ptr,
                                 PHYS_LINK_DOWN_EV,
                                 event_info);

  /*-------------------------------------------------------------------------
    Need to enable flow so that DS_WRITE_EVENT can be posted in sockets
    layer when phys link is in this state
  -------------------------------------------------------------------------*/
  ps_phys_link_enable_flow(this_phys_link_ptr, DS_FLOW_PHYS_LINK_MASK);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_phys_link_down_ind_ex() */

/*===========================================================================
FUNCTION PS_PHYS_LINK_UP_IND_EX()

DESCRIPTION
  The physical link of the given interface is up.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  info_code:  Extended info code for the event.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_up_ind_ex
(
  ps_phys_link_type                * this_phys_link_ptr,
  ps_extended_info_code_enum_type    info_code
)
{
  ps_iface_event_info_u_type event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr)))
  {
    ASSERT(0);
    return;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  LOG_MSG_INFO2_2("ps_phys_link_up_ind_ex(): "
                  "PHYS LINK UP IND 0x%p, state %d",
                  this_phys_link_ptr,
                  PS_PHYS_LINKI_GET_STATE(this_phys_link_ptr));

  /*-------------------------------------------------------------------------
    store the previous state, change the current link state to UP and call
    the callbacks.
  -------------------------------------------------------------------------*/
  event_info.phys_link_event_info.state =
    PS_PHYS_LINK_GET_STATE(this_phys_link_ptr);
  event_info.phys_link_event_info.info_code = info_code;

  PS_PHYS_LINKI_SET_STATE(this_phys_link_ptr, PHYS_LINK_UP);

  ps_ifacei_invoke_event_cbacks( NULL,
                                 this_phys_link_ptr,
                                 PHYS_LINK_UP_EV,
                                 event_info);

  /*-------------------------------------------------------------------------
    Need to enable flow so that DS_WRITE_EVENT can be posted in sockets
    layer when phys link is in this state
  -------------------------------------------------------------------------*/
  ps_phys_link_enable_flow(this_phys_link_ptr, DS_FLOW_PHYS_LINK_MASK);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_phys_link_up_ind_ex() */


/*===========================================================================
FUNCTION PS_PHYS_LINK_GONE_IND_EX()

DESCRIPTION
  The physical link of the given interface has been aborted, indicating
  that the session has gone NULL.  This is called in lieu of
  phys link_down_ind().

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_phys_link_ptr: ptr to interface control block on which to operate
  on.
  info_code:  Extended info code for the event.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  Note that the clients that have registered for the PHYS_LINK_DOWN callback
  also get notified that the phys link has transitioned to the down state.
===========================================================================*/
void ps_phys_link_gone_ind_ex
(
  ps_phys_link_type                * this_phys_link_ptr,
  ps_extended_info_code_enum_type    info_code
)
{
  ps_iface_event_info_u_type event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr)))
  {
    ASSERT(0);
    return;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  LOG_MSG_INFO2_2("ps_phys_link_gone_ind_ex(): "
                  "PHYS LINK GONE IND 0x%p, state %d",
                  this_phys_link_ptr,
                  PS_PHYS_LINKI_GET_STATE(this_phys_link_ptr));

  /*-------------------------------------------------------------------------
    Ensure that no flows are bound to a phys link if it is in FLOW_DECOUPLED
    mode
  -------------------------------------------------------------------------*/
  if (PS_PHYS_LINKI_GET_CAPABILITY(this_phys_link_ptr,
                                   PS_PHYS_LINK_CAPABILITY_FLOW_DECOUPLED))
  {
    if (PS_PHYS_LINKI_GET_REF_CNT(this_phys_link_ptr) != 0)
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_1("ps_phys_link_gone_ind_ex(): "
                      "Flows are still bound to phys link, 0x%p",
                      this_phys_link_ptr);
      ASSERT(0);
      return;
    }
  }

  /*-------------------------------------------------------------------------
    store the previous state, change the current link state to UP and call
    the callbacks.
  -------------------------------------------------------------------------*/
  event_info.phys_link_event_info.state =
    PS_PHYS_LINK_GET_STATE(this_phys_link_ptr);
  event_info.phys_link_event_info.info_code = info_code;

  PS_PHYS_LINKI_SET_STATE(this_phys_link_ptr, PHYS_LINK_NULL);

  /*-------------------------------------------------------------------------
    Bearer id was reset when physlink goes down and modehandler does not set
    the bearer id again. This results in bearer id being 255. So the bearer
    id is reset only when physlink goes NULL.
  -------------------------------------------------------------------------*/
  PS_PHYS_LINK_RESET_BEARER_ID(this_phys_link_ptr);

  ps_ifacei_invoke_event_cbacks(NULL,
                                this_phys_link_ptr,
                                PHYS_LINK_GONE_EV,
                                event_info);

  /*-------------------------------------------------------------------------
    Reset flow control mask. Otherwise, iface will forever be flow controlled
    forever if client doesn't flow enable.

    Also, flow needs to be enabled flow so that DS_WRITE_EVENT can be posted
    in sockets layer when phys link is in this state
  -------------------------------------------------------------------------*/
  ps_phys_link_enable_flow(this_phys_link_ptr,
                           this_phys_link_ptr->phys_private.tx_flow_mask);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_phys_link_gone_ind_ex() */

