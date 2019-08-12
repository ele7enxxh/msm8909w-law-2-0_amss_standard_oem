/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           P S _ F L O W I _ E V E N T . C


GENERAL DESCRIPTION
  This is the implementation of functions used to handle PS flow events.

EXTERNAL FUNCTIONS
  PS_FLOW_EVENT_INIT()
    Initializes the global queues for ps_flow

  PS_FLOW_ALLOC_EVENT_CBACK_BUF()
    Allocates memory used to register for ps_flow event callbacks

  PS_FLOW_FREE_EVENT_CBACK_BUF()
    Free the memory used to register for ps_flow event callbacks

  PS_FLOW_EVENT_CBACK_REG()
    Registers callbacks for events on a flow

  PS_FLOW_EVENT_CBACK_DEREG()
    Deregisters callbacks for a specific event on an flow

  PS_FLOWI_PHYS_LINK_EV_CBACK_REG()
    Registers a flow for all phys link state change events on the phys link
    it is bound to

  PS_FLOWI_PHYS_LINK_EV_CBACK_DEREG()
    Deregisters a flow for all phys link state change events on the phys link
    it is bound to.

  PS_FLOWI_INVOKE_EVENT_CBACKS()
    Invokes all of the event callbacks for a given flow and event

  PS_FLOWI_ASSOC_FLOW_EV_CBACK_REG()
    Registers callback for events on an associated flow

   PS_FLOWI_ASSOC_FLOW_EV_CBACK_DEREG()
    Deregisters callback for events on an associated flow

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_flowi_event.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/01/07    ssh    QoS support for logical ifaces
09/12/06    msr    Removed redundant state field in event_info structure
04/25/06    msr    L4/Tasklock code review changes
02/22/06    msr    Using single critical section
02/06/06    msr    Updated for L4 tasklock/crit sections.
11/10/05    msr    Fixed Lint errors.
10/18/05    msr    Removed support for FLOW_ACTIVATE_MODIFIED_EV and
                   PHYS_LINK_UP_MODIFIED_EV
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
08/15/05    mct    Added qos configure support.
05/12/05    ks     Fixed Lint errors.
05/03/05    msr    Using a macro to register an event with phys link
04/17/05    msr    Created file.
===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "err.h"
#include "amssassert.h"


#include "ps_crit_sect.h"
#include "ps_iface_defs.h"
#include "ps_phys_linki_event.h"
#include "ps_flow_event.h"
#include "ps_flowi_event.h"
#include "ps_flow.h"
#include "ps_flowi.h"
#include "ds_flow_control.h"
#include "ps_mem.h"
#include "ps_iface_flow.h"
#include "ps_iface_logical_flowi.h"
#include "dcc_task_svc.h"
#include "dcc_task_defs.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"





/*===========================================================================

                           LOCAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  This variable is used to store callbacks that are intended to be called
  when events happen on ANY flow.
---------------------------------------------------------------------------*/
static q_type  global_flow_event_q_array[FLOW_MAX_EV - FLOW_MIN_EV];

/*---------------------------------------------------------------------------
  Tuning the number of ps flow event buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_FLOW_EVT_BUF_SIZE  ((sizeof(ps_flow_event_buf_type) + 3) & ~3)

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET

/* We need 10 flow evt buffers for each additional QMI instance.
 * For Fusion, 8 additional QMI instances have been added to for SDIO
 * Hence increasing flow event buffers by 100 */
#if defined(FEATURE_DATA_WLAN_MAPCON) || defined(FEATURE_RMNET_PORT_CONFIG_MDM_LE)
/* For ePDG, another 9 ports were added, hence increasing buffers by 100 */
#define PS_FLOW_EVT_BUF_NUM         600
#define PS_FLOW_EVT_BUF_HIGH_WM     400
#else
#define PS_FLOW_EVT_BUF_NUM         430
#define PS_FLOW_EVT_BUF_HIGH_WM     300
#endif /* FEATURE_DATA_WLAN_MAPCON */
#define PS_FLOW_EVT_BUF_LOW_WM       70

#else /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

#define PS_FLOW_EVT_BUF_NUM         20
#define PS_FLOW_EVT_BUF_HIGH_WM     16
#define PS_FLOW_EVT_BUF_LOW_WM       5
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_flow_evt along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_flow_evt_buf_mem[PS_MEM_GET_TOT_SIZE
                               (
                                 PS_FLOW_EVT_BUF_NUM,
                                 PS_FLOW_EVT_BUF_SIZE
                               )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ps_flow_evt_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type     * ps_flow_evt_buf_hdr[PS_FLOW_EVT_BUF_NUM];
static ps_flow_event_buf_type  * ps_flow_evt_buf_ptr[PS_FLOW_EVT_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */



/*===========================================================================

                               MACROS

===========================================================================*/
/*===========================================================================
MACRO PS_FLOWI_REG_EV_WITH_PHYS_LINK()

DESCRIPTION
  Registers an event call back with specified phys link

DEPENDENCIES
  This macro has "return" statement in it. So DON'T call this inside
  TASKLOCK()
===========================================================================*/
#define PS_FLOWI_REG_EV_WITH_PHYS_LINK(phys_link_ptr, ev_buf_ptr, event,   \
                                       ev_cb_f_ptr, user_data_ptr)         \
  (ev_buf_ptr) = ps_phys_link_alloc_event_cback_buf(ev_cb_f_ptr,           \
                                                    user_data_ptr);        \
  if ((ev_buf_ptr) == NULL)                                                \
  {                                                                        \
    LOG_MSG_ERROR_1("Unable to allocate buffer for EV, %d", event);        \
    return;                                                                \
  }                                                                        \
                                                                           \
  if (ps_phys_link_event_cback_reg(phys_link_ptr, event, ev_buf_ptr) != 0) \
  {                                                                        \
    LOG_MSG_ERROR_1("Could not reg cback for EV %d", event);                 \
    ps_phys_link_free_event_cback_buf(ev_buf_ptr);                         \
    (ev_buf_ptr) = NULL;                                                   \
    ASSERT(0);                                                             \
    return;                                                                \
  }

/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_FLOWI_GET_EVENT_Q_ARRAY_INDEX()

DESCRIPTION
  This function returns the queue no for given flow event

PARAMETERS
  event               : flow event

RETURN VALUE
  Queue number
===========================================================================*/
int32 ps_flowi_get_event_q_array_index
(
  ps_iface_event_enum_type event
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (event == FLOW_TX_ENABLED_EV || event == FLOW_TX_DISABLED_EV)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}
/*===========================================================================
FUNCTION PS_FLOWI_DELETE_EV_CBACK_BUF()

DESCRIPTION
  This function deregisters the callback for the given event and frees the
  memory associated with it.

PARAMETERS
  flow_ptr            : flow containing the event callback to be deleted
  event               : event to which the buffer is associated with
  event_cback_buf_ptr : event buffer to be deleted

RETURN VALUE
  None
===========================================================================*/
static void ps_flowi_delete_ev_cback_buf
(
  ps_flow_type            * flow_ptr,
  ps_flow_type            * assoc_flow_ptr,
  ps_iface_event_enum_type  event,
  void                    * event_cback_buf_ptr
)
{
  if (event_cback_buf_ptr != NULL)
  {
    if(PS_FLOW_IS_VALID(assoc_flow_ptr) &&
       PS_FLOWI_GET_COOKIE(assoc_flow_ptr)
         == PS_FLOWI_GET_ASSOC_COOKIE(flow_ptr))
    {
      ps_flow_event_cback_dereg(assoc_flow_ptr, event, event_cback_buf_ptr);
      ps_flow_free_event_cback_buf(event_cback_buf_ptr);
    }
  }
}

/*===========================================================================
FUNCTION PS_FLOWI_PHYS_LINK_EVENT_CBACK()

DESCRIPTION
  This event callback will be registered by ps_flow as event callback with
  phys link. This function is called when a phys link event of interest
  happens, and flow either calls commands or posts indications to change its
  state appropriately. This makes flows transparent to mode handlers when
  phys link is FLOW_COUPLED.

PARAMETERS
  flow_ptr      : ptr to the flow on which we are operating
  event         : the event for which callbacks need to be called
  event_info    : event information that needs to be passed into callback.
  user_data_ptr : pointer to user data info

RETURN VALUE
  None

DEPENDENCIES
  Must be called inside TASKLOCK

SIDE EFFECTS
  None
===========================================================================*/
static void ps_flowi_phys_link_event_cback
(
  ps_phys_link_type           * phys_link_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * user_data_ptr
)
{
  ps_flow_type  * flow_ptr;
#ifdef FEATURE_DATA_PS_QOS
  int16           ps_errno;
#endif /* FEATURE_DATA_PS_QOS */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_PHYS_LINK_IS_VALID(phys_link_ptr) || user_data_ptr == NULL)
  {
    LOG_MSG_ERROR_0("ps_flowi_phys_link_event_cback(): "
                    "Invalid parameters are passed");
    ASSERT(0);
    return;
  }

  flow_ptr = (ps_flow_type *) user_data_ptr;
  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    LOG_MSG_ERROR_1("ps_flowi_phys_link_event_cback(): "
                    "Flow, 0x%p, is invalid", flow_ptr);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    A ps_flow registers for phys link events only when phys link is not
    DECOUPLED
  -------------------------------------------------------------------------*/
  if (PS_PHYS_LINKI_GET_CAPABILITY(phys_link_ptr,
                                   PS_PHYS_LINK_CAPABILITY_FLOW_DECOUPLED))
  {
    LOG_MSG_ERROR_1("ps_flowi_phys_link_event_cback(): "
                    "Phys link, 0x%p is not COUPLED to a flow", phys_link_ptr);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    A ps_flow registers for phys link events only on the phys link it is
    bound to and deregisters when it is unbound
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_GET_PHYS_LINK(flow_ptr) != phys_link_ptr)
  {
    LOG_MSG_ERROR_2("ps_flowi_phys_link_event_cback(): "
                    "Flow, 0x%p, is not bound to phys link, 0x%p",
                    flow_ptr, phys_link_ptr);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_3("ps_flowi_phys_link_event_cback(): "
                  "Processing event, %d on phys link 0x%p for flow 0x%p",
                  event, phys_link_ptr, flow_ptr);

  /*-------------------------------------------------------------------------
    Based on phys link event, call commands/indications to change flow's
    state accordingly. Since only ps_flow_activate_ind() and
    ps_flow_go_null_ind() are allowed to be posted on default flow, don't
    post other commands and indications on default flow
  -------------------------------------------------------------------------*/
  switch (event)
  {
    case PHYS_LINK_UP_EV:
      ps_flow_activate_ind(flow_ptr,
                           event_info.phys_link_event_info.info_code);
      break;

    case PHYS_LINK_GONE_EV:
      if (event_info.phys_link_event_info.state != PHYS_LINK_NULL)
      {
        ps_flow_go_null_ind(flow_ptr,
                            event_info.phys_link_event_info.info_code);
      }
      break;

#ifdef FEATURE_DATA_PS_QOS
    case PHYS_LINK_DOWN_EV:
      if (!PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
      {
        ps_flow_suspend_ind(flow_ptr,
                            event_info.phys_link_event_info.info_code);
      }
      break;

    case PHYS_LINK_COMING_UP_EV:
      if (!PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
      {
        (void) ps_flow_activate_cmd(flow_ptr, &ps_errno, NULL);
      }
      break;

    case PHYS_LINK_RESUMING_EV:
      if (!PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
      {
        (void) ps_flow_resume_cmd(flow_ptr, &ps_errno, NULL);
      }
      break;

    case PHYS_LINK_GOING_DOWN_EV:
      if (!PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
      {
        (void) ps_flow_suspend_cmd(flow_ptr, &ps_errno, NULL);
      }
      break;

    case PHYS_LINK_GOING_NULL_EV:
      if (!PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
      {
        (void) ps_flow_go_null_cmd(flow_ptr, &ps_errno, NULL);
      }
      break;
#else
    case PHYS_LINK_COMING_UP_EV:
    case PHYS_LINK_GOING_DOWN_EV:
    case PHYS_LINK_DOWN_EV:
    case PHYS_LINK_RESUMING_EV:
    case PHYS_LINK_GOING_NULL_EV:
      break;
#endif /* FEATURE_DATA_PS_QOS */

    default:
      ASSERT(0);
      LOG_MSG_ERROR_1("ps_flowi_phys_link_event_cback(): "
                      "Unknown event %d", event);
      break;
  }
} /* ps_flowi_phys_link_event_cback() */



/*===========================================================================
FUNCTION PS_FLOWI_ASSOC_FLOW_EV_CBACK()

DESCRIPTION
  This function processes flow events received from the flow associated
  with a flow.

PARAMETERS
  assoc_flow_ptr: ptr to the flow on which the event occured
  event         : the event for which callbacks need to be called
  event_info    : event information that needs to be passed into callback.
  user_data_ptr : pointer to user data info
                  (registered via ps_flow_assoc_flow_ev_cback_reg)

RETURN VALUE
  None

DEPENDENCIES
  Must be called inside TASKLOCK

SIDE EFFECTS
  None
===========================================================================*/
static void ps_flowi_assoc_flow_ev_cback
(
  ps_flow_type                * assoc_flow_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * user_data_ptr
)
{
  ps_logical_flow_assoc_flow_ev_cmd_type * assoc_flow_cmd_ptr;
  ps_flow_type                           * logical_flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if 0
  LOG_MSG_FUNCTION_ENTRY_2("ps_flowi_assoc_flow_ev_cback(): "
                           "Event %d on flow 0x%p", event, assoc_flow_ptr);
#endif

  PS_SYSTEM_HEAP_MEM_ALLOC(assoc_flow_cmd_ptr,
                           sizeof(ps_logical_flow_assoc_flow_ev_cmd_type),
                           ps_logical_flow_assoc_flow_ev_cmd_type*);
  if (assoc_flow_cmd_ptr == NULL)
  {
    return;
  }

  assoc_flow_cmd_ptr->event         = event;
  assoc_flow_cmd_ptr->event_info    = event_info;
  assoc_flow_cmd_ptr->user_data_ptr = user_data_ptr;
  assoc_flow_cmd_ptr->assoc_flow_ptr = assoc_flow_ptr;

  do
  {

    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

    logical_flow_ptr = ((ps_flow_type *) user_data_ptr);
    if (!PS_FLOW_IS_VALID(logical_flow_ptr))
    {
      LOG_MSG_INVALID_INPUT_1("ps_flowi_assoc_flow_ev_cback(): "
                              "Got invalid logical flow_ptr 0x%p",
                              logical_flow_ptr);
      break;
    }

    assoc_flow_cmd_ptr->logical_flow_cookie =
                        PS_FLOWI_GET_COOKIE(logical_flow_ptr);

    /*-----------------------------------------------------------------------
      TODO: dcc_set_cmd_handler has to be moved to a new function called
      from dcci_init
    -----------------------------------------------------------------------*/
    (void) dcc_set_cmd_handler
           (
             DCC_LOGICAL_FLOW_ASSOC_FLOW_EV_CMD,
             ps_flowi_process_assoc_flow_ev
           );

    dcc_send_cmd_ex(DCC_LOGICAL_FLOW_ASSOC_FLOW_EV_CMD, assoc_flow_cmd_ptr);
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    LOG_MSG_FUNCTION_EXIT_2("ps_flowi_assoc_flow_ev_cback(): "
                            "Success: Event %d on flow 0x%p",
                            event, assoc_flow_ptr);
    return;
  } while(0);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /* Failure case: Release the allocated DCC cmd buffer */
  PS_SYSTEM_HEAP_MEM_FREE(assoc_flow_cmd_ptr);

#if 0
  LOG_MSG_FUNCTION_EXIT_2("ps_flowi_assoc_flow_ev_cback(): "
                          "Fail: Event %d on flow 0x%p",
                          event, assoc_flow_ptr);
#endif
  return;

} /* ps_flowi_assoc_flow_ev_cback() */



/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_FLOW_EVENT_INIT()

DESCRIPTION
  This function initializes the global queues for ps_flow.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_event_init
(
  void
)
{
  uint8  ev_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize global event queues
  -------------------------------------------------------------------------*/
  for (ev_index = 0; ev_index < FLOW_MAX_EV - FLOW_MIN_EV; ev_index++)
  {
    (void) q_init(global_flow_event_q_array + ev_index);
  }

  /*-------------------------------------------------------------------------
    Initialize Pool
  -------------------------------------------------------------------------*/
  if (ps_mem_pool_init(PS_MEM_PS_FLOW_EVT_CB_TYPE,
                       ps_flow_evt_buf_mem,
                       PS_FLOW_EVT_BUF_SIZE,
                       PS_FLOW_EVT_BUF_NUM,
                       PS_FLOW_EVT_BUF_HIGH_WM,
                       PS_FLOW_EVT_BUF_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) ps_flow_evt_buf_hdr,
                       (int *) ps_flow_evt_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    LOG_MSG_ERROR_0("ps_flow_event_init(): "
                    "Can't init the module");
  }

} /* ps_flow_event_init() */



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
)
{
  ps_flow_event_buf_type  * buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (event_cback_f_ptr == NULL)
  {
    LOG_MSG_ERROR_0("ps_flow_alloc_event_cback_buf(): "
                    "NULL call back function is passed");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Allocate a new event callback buffer.
  -------------------------------------------------------------------------*/
  buf_ptr =
    (ps_flow_event_buf_type *) ps_mem_get_buf(PS_MEM_PS_FLOW_EVT_CB_TYPE);
  if (buf_ptr == NULL)
  {
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Initialize the event buffer
  -------------------------------------------------------------------------*/
  (void) q_link(buf_ptr, &(buf_ptr->link));
  buf_ptr->event_cback_f_ptr = event_cback_f_ptr;
  buf_ptr->user_data_ptr     = user_data_ptr;
  buf_ptr->event_q_ptr       = NULL;

  return buf_ptr;

} /* ps_flow_alloc_event_cback_buf() */



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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (buf_ptr == NULL)
  {
    LOG_MSG_ERROR_0("ps_flow_free_event_cback_buf(): "
                    "NULL buffer is passed");
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Validate that passed in buffer is not already on a queue
  -------------------------------------------------------------------------*/
  if (Q_ALREADY_QUEUED(&(((ps_flow_event_buf_type *) buf_ptr)->link)))
  {
    LOG_MSG_ERROR_1("ps_flow_free_event_cback_buf(): "
                    "Buffer, 0x%p, is still on a queue", buf_ptr);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Invalidate the buffer so that the caller cannot use it without
    explicitly allcating it again.
  -------------------------------------------------------------------------*/
  ((ps_flow_event_buf_type *) buf_ptr)->event_cback_f_ptr = NULL;

  PS_MEM_FREE(buf_ptr);

} /* ps_flow_free_event_cback_buf() */



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
)
{
  ps_iface_event_info_u_type    event_info;
  ps_flow_event_buf_type      * flow_ev_buf_ptr;
  boolean                       need_to_invoke_cback = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (buf_ptr == NULL || event >= FLOW_MAX_EV || event < FLOW_MIN_EV)
  {
    LOG_MSG_ERROR_0("ps_flow_event_cback_reg(): "
                    "Invalid parameters are passed");
    ASSERT(0);
    return -1;
  }

  if (ps_mem_is_valid(buf_ptr, PS_MEM_PS_FLOW_EVT_CB_TYPE) == FALSE)
  {
    LOG_MSG_ERROR_2("ps_flow_event_cback_reg(): "
                    "Invalid ps_flow buf 0x%p to reg evt %d", buf_ptr, event);
    ASSERT(0);
    return -1;
  }

  flow_ev_buf_ptr = (ps_flow_event_buf_type *) buf_ptr;

  if (Q_ALREADY_QUEUED(&(flow_ev_buf_ptr->link)))
  {
    LOG_MSG_ERROR_2("ps_flow_event_cback_reg(): "
                    "Not re-q'ing ps_flow buf 0x%p for evt %d",
                    flow_ev_buf_ptr, event);
    ASSERT(0);
    return -1;
  }

  if (flow_ev_buf_ptr->event_cback_f_ptr == NULL)
  {
    LOG_MSG_ERROR_2("ps_flow_event_cback_reg(): "
                    "Callback is null in buf 0x%p for evt %d",
                    flow_ev_buf_ptr, event);
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Register in global queue if flow_ptr is NULL, else in local queue
  -------------------------------------------------------------------------*/
  if (flow_ptr == NULL)
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    flow_ev_buf_ptr->event_q_ptr =
      &global_flow_event_q_array[event - FLOW_MIN_EV];
    flow_ev_buf_ptr->event_name = event;
    q_put(flow_ev_buf_ptr->event_q_ptr, &(flow_ev_buf_ptr->link));
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    return 0;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_event_cback_reg(): "
                    "Invalid flow ptr, 0x%p, passed", flow_ptr);
    return -1;
  }

  flow_ev_buf_ptr->event_q_ptr =
    &(flow_ptr->flow_private.event_q_array[ps_flowi_get_event_q_array_index(event)]);
  flow_ev_buf_ptr->event_name = event;
  q_put(flow_ev_buf_ptr->event_q_ptr, &(flow_ev_buf_ptr->link));

  /*-------------------------------------------------------------------------
    Initializing "event_info" so that lint won't crib
  -------------------------------------------------------------------------*/
  event_info.flow_event_info.state = FLOW_STATE_INVALID;

  /*-------------------------------------------------------------------------
    Check if the event is true - if so call the callback immediately
  -------------------------------------------------------------------------*/
  switch (event)
  {
    case FLOW_NULL_EV:
      if (PS_FLOWI_GET_STATE(flow_ptr) == FLOW_NULL)
      {
        event_info.flow_event_info.state     = FLOW_NULL;
        event_info.flow_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                 = TRUE;
      }
      break;

    case FLOW_ACTIVATING_EV:
      if (PS_FLOWI_GET_STATE(flow_ptr) == FLOW_ACTIVATING)
      {
        event_info.flow_event_info.state     = FLOW_ACTIVATING;
        event_info.flow_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                 = TRUE;
      }
      break;

    case FLOW_CONFIGURING_EV:
      if (PS_FLOWI_GET_STATE(flow_ptr) == FLOW_CONFIGURING)
      {
        event_info.flow_event_info.state     = FLOW_CONFIGURING;
        event_info.flow_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                 = TRUE;
      }
      break;

    case FLOW_ACTIVATED_EV:
      if (PS_FLOWI_GET_STATE(flow_ptr) == FLOW_ACTIVATED)
      {
        event_info.flow_event_info.state     = FLOW_ACTIVATED;
        event_info.flow_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                 = TRUE;
      }
      break;

    case FLOW_SUSPENDING_EV:
      if (PS_FLOWI_GET_STATE(flow_ptr) == FLOW_SUSPENDING)
      {
        event_info.flow_event_info.state     = FLOW_SUSPENDING;
        event_info.flow_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                 = TRUE;
      }
      break;

    case FLOW_SUSPENDED_EV:
      if (PS_FLOWI_GET_STATE(flow_ptr) == FLOW_SUSPENDED)
      {
        event_info.flow_event_info.state     = FLOW_SUSPENDED;
        event_info.flow_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                 = TRUE;
      }
      break;

    case FLOW_RESUMING_EV:
      if (PS_FLOWI_GET_STATE(flow_ptr) == FLOW_RESUMING)
      {
        event_info.flow_event_info.state     = FLOW_RESUMING;
        event_info.flow_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                 = TRUE;
      }
      break;

    case FLOW_GOING_NULL_EV:
      if (PS_FLOWI_GET_STATE(flow_ptr) == FLOW_GOING_NULL)
      {
        event_info.flow_event_info.state     = FLOW_GOING_NULL;
        event_info.flow_event_info.info_code = PS_EIC_NOT_SPECIFIED;
        need_to_invoke_cback                 = TRUE;
      }
      break;

    case FLOW_TX_ENABLED_EV:
      if (PS_FLOWI_IS_TX_ENABLED(flow_ptr))
      {
        event_info.flow_mask = DS_FLOW_IS_ENABLED;
        need_to_invoke_cback = TRUE;
      }
      break;

    case FLOW_TX_DISABLED_EV:
      if (!PS_FLOWI_IS_TX_ENABLED(flow_ptr))
      {
        event_info.flow_mask = PS_FLOWI_GET_TX_MASK(flow_ptr);
        need_to_invoke_cback = TRUE;
      }
      break;

    default:
      break;
  } /* switch (event) */

  if (need_to_invoke_cback)
  {
    flow_ev_buf_ptr->event_cback_f_ptr(flow_ptr,
                                       event,
                                       event_info,
                                       flow_ev_buf_ptr->user_data_ptr);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;

} /* ps_flow_event_cback_reg() */



/*===========================================================================
FUNCTION PS_FLOW_EVENT_CBACK_DEREG()

DESCRIPTION
  Deregisters callbacks for a specific event on an flow.

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
)
{
  ps_flow_event_buf_type  * flow_ev_buf_ptr;
  q_type                  * working_q_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (event < FLOW_MIN_EV || event >= FLOW_MAX_EV || buf_ptr == NULL)
  {
    LOG_MSG_ERROR_0("ps_flow_event_cback_dereg(): "
                    "Invalid parameters are passed");
    ASSERT(0);
    return;
  }

  if (ps_mem_is_valid(buf_ptr, PS_MEM_PS_FLOW_EVT_CB_TYPE) == FALSE)
  {
    LOG_MSG_INFO1_2("ps_flow_event_cback_dereg(): "
                    "Invalid buf 0x%p to dereg evt %d", buf_ptr, event);
    ASSERT(0);
    return;
  }

  flow_ev_buf_ptr = (ps_flow_event_buf_type *) buf_ptr;

  /*-------------------------------------------------------------------------
    Make sure that this event is on a queue - if not mark it as such
  -------------------------------------------------------------------------*/
  if (!(Q_ALREADY_QUEUED(&(flow_ev_buf_ptr->link))))
  {
    LOG_MSG_INFO2_2("ps_flow_event_cback_dereg(): "
                    "Buf 0x%p for event %d not on a queue",
                    flow_ev_buf_ptr, event);
    return;
  }

  /*-------------------------------------------------------------------------
    If this registration is for global callbacks, then use the global
    variable as the working pointer, otherwise use the q from the iface
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (flow_ptr == NULL)
  {
    working_q_ptr = &global_flow_event_q_array[event - FLOW_MIN_EV];
  }
  else
  {
    if (!PS_FLOW_IS_VALID(flow_ptr))
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_1("ps_flow_event_cback_dereg(): "
                      "Invalid flow ptr, 0x%p, passed", flow_ptr);
      return;
    }

    working_q_ptr =
      &(flow_ptr->flow_private.event_q_array[ps_flowi_get_event_q_array_index(event)]);
  }

  /*-------------------------------------------------------------------------
    Make sure that buffer belongs to the queue we are operating on, otherwise
    q_delete would end up in infinite loop
  -------------------------------------------------------------------------*/
  if (working_q_ptr != flow_ev_buf_ptr->event_q_ptr)
  {
    LOG_MSG_ERROR_3("ps_flow_event_cback_dereg(): "
                    "Buf 0x%p (ev %d) not reg 0x%p flow",
                    flow_ev_buf_ptr, event, flow_ptr);
    ASSERT(0);
  }
  else
  {
#ifdef FEATURE_Q_NO_SELF_QPTR
    q_delete(working_q_ptr, &(flow_ev_buf_ptr->link));
#else
    q_delete(&(flow_ev_buf_ptr->link));
#endif
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return;

} /* ps_flow_event_cback_dereg() */



/*===========================================================================
FUNCTION PS_FLOWI_PHYS_LINK_EV_CBACK_REG()

DESCRIPTION
  This function registers a flow for all phys link state change events on
  the phys link it is bound to.

PARAMETERS
  flow_ptr : ptr to flow which is interested in phys link events

RETURN VALUE
  None

DEPENDENCIES
  flow_ptr must be valid and it must be bound to a phys link

SIDE EFFECTS
  None
===========================================================================*/
void ps_flowi_phys_link_ev_cback_reg
(
  ps_flow_type  * flow_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(PS_FLOWI_GET_PHYS_LINK(flow_ptr) != NULL);

  /*-------------------------------------------------------------------------
    Note that PS_FLOWI_REG_EV_WITH_PHYS_LINK() macro uses "return" statement.
    So, reorganize this function if it needs to be TASKLOCKed
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Register for PHYS_LINK_COMING_UP_EV
  -------------------------------------------------------------------------*/
  PS_FLOWI_REG_EV_WITH_PHYS_LINK
  (
    PS_FLOWI_GET_PHYS_LINK(flow_ptr),
    flow_ptr->flow_private.phys_link_ev_cback_buf.coming_up,
    PHYS_LINK_COMING_UP_EV,
    ps_flowi_phys_link_event_cback,
    (void *) flow_ptr
  );

  /*-------------------------------------------------------------------------
    Register for PHYS_LINK_UP_EV
  -------------------------------------------------------------------------*/
  PS_FLOWI_REG_EV_WITH_PHYS_LINK
  (
    PS_FLOWI_GET_PHYS_LINK(flow_ptr),
    flow_ptr->flow_private.phys_link_ev_cback_buf.up,
    PHYS_LINK_UP_EV,
    ps_flowi_phys_link_event_cback,
    (void *) flow_ptr
  );

  /*-------------------------------------------------------------------------
    Register for PHYS_LINK_GOING_DOWN_EV
  -------------------------------------------------------------------------*/
  PS_FLOWI_REG_EV_WITH_PHYS_LINK
  (
    PS_FLOWI_GET_PHYS_LINK(flow_ptr),
    flow_ptr->flow_private.phys_link_ev_cback_buf.going_down,
    PHYS_LINK_GOING_DOWN_EV,
    ps_flowi_phys_link_event_cback,
    (void *) flow_ptr
  );

  /*-------------------------------------------------------------------------
    Register for PHYS_LINK_DOWN_EV
  -------------------------------------------------------------------------*/
  PS_FLOWI_REG_EV_WITH_PHYS_LINK
  (
    PS_FLOWI_GET_PHYS_LINK(flow_ptr),
    flow_ptr->flow_private.phys_link_ev_cback_buf.down,
    PHYS_LINK_DOWN_EV,
    ps_flowi_phys_link_event_cback,
    (void *) flow_ptr
  );

  /*-------------------------------------------------------------------------
    Register for PHYS_LINK_RESUMING_EV
  -------------------------------------------------------------------------*/
  PS_FLOWI_REG_EV_WITH_PHYS_LINK
  (
    PS_FLOWI_GET_PHYS_LINK(flow_ptr),
    flow_ptr->flow_private.phys_link_ev_cback_buf.resuming,
    PHYS_LINK_RESUMING_EV,
    ps_flowi_phys_link_event_cback,
    (void *) flow_ptr
  );

  /*-------------------------------------------------------------------------
    Register for PHYS_LINK_GOING_NULL_EV
  -------------------------------------------------------------------------*/
  PS_FLOWI_REG_EV_WITH_PHYS_LINK
  (
    PS_FLOWI_GET_PHYS_LINK(flow_ptr),
    flow_ptr->flow_private.phys_link_ev_cback_buf.going_null,
    PHYS_LINK_GOING_NULL_EV,
    ps_flowi_phys_link_event_cback,
    (void *) flow_ptr
  );

  /*-------------------------------------------------------------------------
    Register for PHYS_LINK_GONE_EV
  -------------------------------------------------------------------------*/
  PS_FLOWI_REG_EV_WITH_PHYS_LINK
  (
    PS_FLOWI_GET_PHYS_LINK(flow_ptr),
    flow_ptr->flow_private.phys_link_ev_cback_buf.null,
    PHYS_LINK_GONE_EV,
    ps_flowi_phys_link_event_cback,
    (void *) flow_ptr
  );

} /* ps_flowi_phys_link_ev_cback_reg() */



/*===========================================================================
FUNCTION PS_FLOWI_PHYS_LINK_EV_CBACK_DEREG()

DESCRIPTION
  This function de-registers a flow for all phys link state change events on
  the phys link it is bound to.

PARAMETERS
  flow_ptr : ptr to flow which is interested in phys link events

RETURN VALUE
  None

DEPENDENCIES
  flow_ptr must be valid and it must be bound to a phys link. If ps_low is
  not registered for phys link events, this function will result in crash

SIDE EFFECTS
  None
===========================================================================*/
void ps_flowi_phys_link_ev_cback_dereg
(
  ps_flow_type  * flow_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Deregister PHYS_LINK_COMING_UP_EV
  -------------------------------------------------------------------------*/
  ps_phys_link_event_cback_dereg
  (
    PS_FLOWI_GET_PHYS_LINK(flow_ptr),
    PHYS_LINK_COMING_UP_EV,
    flow_ptr->flow_private.phys_link_ev_cback_buf.coming_up
  );

  ps_phys_link_free_event_cback_buf
  (
    flow_ptr->flow_private.phys_link_ev_cback_buf.coming_up
  );
  flow_ptr->flow_private.phys_link_ev_cback_buf.coming_up = NULL;

  /*-------------------------------------------------------------------------
    Deregister PHYS_LINK_UP_EV
  -------------------------------------------------------------------------*/
  ps_phys_link_event_cback_dereg
  (
    PS_FLOWI_GET_PHYS_LINK(flow_ptr),
    PHYS_LINK_UP_EV,
    flow_ptr->flow_private.phys_link_ev_cback_buf.up
  );

  ps_phys_link_free_event_cback_buf
  (
    flow_ptr->flow_private.phys_link_ev_cback_buf.up
  );
  flow_ptr->flow_private.phys_link_ev_cback_buf.up = NULL;

  /*-------------------------------------------------------------------------
    Deregister PHYS_LINK_GOING_DOWN_EV
  -------------------------------------------------------------------------*/
  ps_phys_link_event_cback_dereg
  (
    PS_FLOWI_GET_PHYS_LINK(flow_ptr),
    PHYS_LINK_GOING_DOWN_EV,
    flow_ptr->flow_private.phys_link_ev_cback_buf.going_down
  );

  ps_phys_link_free_event_cback_buf
  (
    flow_ptr->flow_private.phys_link_ev_cback_buf.going_down
  );
  flow_ptr->flow_private.phys_link_ev_cback_buf.going_down = NULL;

  /*-------------------------------------------------------------------------
    Deregister PHYS_LINK_DOWN_EV
  -------------------------------------------------------------------------*/
  ps_phys_link_event_cback_dereg
  (
    PS_FLOWI_GET_PHYS_LINK(flow_ptr),
    PHYS_LINK_DOWN_EV,
    flow_ptr->flow_private.phys_link_ev_cback_buf.down
  );

  ps_phys_link_free_event_cback_buf
  (
    flow_ptr->flow_private.phys_link_ev_cback_buf.down
  );
  flow_ptr->flow_private.phys_link_ev_cback_buf.down = NULL;

  /*-------------------------------------------------------------------------
    Deregister PHYS_LINK_RESUMING_EV
  -------------------------------------------------------------------------*/
  ps_phys_link_event_cback_dereg
  (
    PS_FLOWI_GET_PHYS_LINK(flow_ptr),
    PHYS_LINK_RESUMING_EV,
    flow_ptr->flow_private.phys_link_ev_cback_buf.resuming
  );

  ps_phys_link_free_event_cback_buf
  (
    flow_ptr->flow_private.phys_link_ev_cback_buf.resuming
  );
  flow_ptr->flow_private.phys_link_ev_cback_buf.resuming = NULL;

  /*-------------------------------------------------------------------------
    Deregister PHYS_LINK_GOING_NULL_EV
  -------------------------------------------------------------------------*/
  ps_phys_link_event_cback_dereg
  (
    PS_FLOWI_GET_PHYS_LINK(flow_ptr),
    PHYS_LINK_GOING_NULL_EV,
    flow_ptr->flow_private.phys_link_ev_cback_buf.going_null
  );

  ps_phys_link_free_event_cback_buf
  (
    flow_ptr->flow_private.phys_link_ev_cback_buf.going_null
  );
  flow_ptr->flow_private.phys_link_ev_cback_buf.going_null = NULL;

  /*-------------------------------------------------------------------------
    Deregister PHYS_LINK_GONE_EV
  -------------------------------------------------------------------------*/
  ps_phys_link_event_cback_dereg
  (
    PS_FLOWI_GET_PHYS_LINK(flow_ptr),
    PHYS_LINK_GONE_EV,
    flow_ptr->flow_private.phys_link_ev_cback_buf.null
  );

  ps_phys_link_free_event_cback_buf
  (
    flow_ptr->flow_private.phys_link_ev_cback_buf.null
  );
  flow_ptr->flow_private.phys_link_ev_cback_buf.null = NULL;

} /* ps_flowi_phys_link_ev_cback_dereg() */



/*===========================================================================
FUNCTION PS_FLOWI_INVOKE_EVENT_CBACKS()

DESCRIPTION
  This function will invoke all of the event callbacks for a given flow
  and event.  It will also call the global callbacks for said event, if any.

PARAMETERS
  flow_ptr   : ptr to the flow on which we are operating
  event      : the event for which callbacks need to be called
  event_info : event information that needs to be passed into callback.

RETURN VALUE
  None

DEPENDENCIES
  Must be called inside TASKLOCK()

SIDE EFFECTS
  None
===========================================================================*/
void ps_flowi_invoke_event_cbacks
(
  ps_flow_type                * flow_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info
)
{
  ps_flow_event_buf_type  * event_buf_ptr;
  q_type                  * working_q_ptr;
  q_type                  * working_global_q_ptr;
  ps_flow_event_buf_type  * next_event_buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    LOG_MSG_ERROR_1("ps_flowi_invoke_event_cbacks(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return;
  }

  if(( event < FLOW_MIN_EV ) || ( event >= FLOW_MAX_EV ))
  {
    LOG_MSG_ERROR_1("ps_flowi_invoke_event_cbacks(): "
                    "Invalid flow event passed %d", event);
    ASSERT(0);
    return;
  }

  working_q_ptr =
    &(flow_ptr->flow_private.event_q_array[ps_flowi_get_event_q_array_index(event)]);
  working_global_q_ptr = &global_flow_event_q_array[event - FLOW_MIN_EV];

  for (;;)
  {
    /*-----------------------------------------------------------------------
      Call callbacks for all items in queue
    -----------------------------------------------------------------------*/
    event_buf_ptr = (ps_flow_event_buf_type *) q_check(working_q_ptr);
    while (event_buf_ptr != NULL)
    {
      next_event_buf_ptr = 
        (ps_flow_event_buf_type *) (q_next(working_q_ptr, &(event_buf_ptr->link)));

      if (event_buf_ptr->event_name == event)
      {
        event_buf_ptr->event_cback_f_ptr(flow_ptr,
                                         event,
                                         event_info,
                                         event_buf_ptr->user_data_ptr);
      }

      event_buf_ptr = next_event_buf_ptr;
     
    } /* while(items in queue) */

    /*-----------------------------------------------------------------------
      If the working_q_ptr is set to qlobal_event_q_array then we are done,
      otherwise we need to call those callbacks - but only if the global
      event q has been initialized.
    -----------------------------------------------------------------------*/
    if (working_q_ptr == working_global_q_ptr)
    {
      break;
    }
    else
    {
      working_q_ptr = working_global_q_ptr;
    }
  } /* for (all specific and global callbacks) */

} /* ps_flowi_invoke_event_cbacks() */


/*===========================================================================
FUNCTION PS_FLOWI_ASSOC_FLOW_EV_CBACK_REG

DESCRIPTION
  This function registers a flow for events on its associated ps_flow.
  Only the stable events (such as activated, suspended etc.) are registered.
  The transitory events (such as activating, suspending etc.) are not
  handled by a logical flow.

  If this function returns error, the caller is expected to delete
  the logical flow. Else the behaviour is undeterministic.

PARAMETERS
  flow_ptr                      : ptr to flow
  assoc_flow_event_cback_f_ptr  : event callback function to be registered
  ps_errno                      : return the specific error

RETURN VALUE
   0 : on success
  -1 : on failure (any of the passed in parameters is bad)

DEPENDENCIES
  flow_ptr must be valid and it must be associated to another flow_ptr

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_flowi_assoc_flow_ev_cback_reg
(
  ps_flow_type                        * flow_ptr,
  ps_flow_assoc_flow_event_cback_type   assoc_flow_event_cback_f_ptr,
  int16                               * ps_errno
)
{
  ps_flow_type  * assoc_flow_ptr;
  boolean         is_default_flow;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr);
  ASSERT(assoc_flow_ptr != NULL && assoc_flow_ptr != flow_ptr);

  is_default_flow =
    PS_FLOW_GET_CAPABILITY(assoc_flow_ptr, PS_FLOW_CAPABILITY_DEFAULT);

  /*-------------------------------------------------------------------------
    Use default if no callback function is supplied
  -------------------------------------------------------------------------*/
  if (assoc_flow_event_cback_f_ptr == NULL)
  {
    assoc_flow_event_cback_f_ptr = ps_flowi_assoc_flow_ev_cback;
  }

  /*-------------------------------------------------------------------------
    Register only for appropriate events on the default flow.
  -------------------------------------------------------------------------*/
  if ( ! is_default_flow)
  {
    /*-------------------------------------------------------------------------
      Register for FLOW_ACTIVATED_EV
    -------------------------------------------------------------------------*/
    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.activated =
      ps_flow_alloc_event_cback_buf(assoc_flow_event_cback_f_ptr,  flow_ptr);

    if (flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.activated ==
          NULL)
    {
      LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                      "Out of mem for ev %d", FLOW_ACTIVATED_EV);
      *ps_errno = DS_ENOMEM;
      return -1;
    }

    if (ps_flow_event_cback_reg
        (
          assoc_flow_ptr,
          FLOW_ACTIVATED_EV,
          flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.activated
        ) != 0)
    {
      LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                      "Could not reg cback for ev %d", FLOW_ACTIVATED_EV);
      ps_flow_free_event_cback_buf
      (
        flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.activated
      );

      flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.activated = NULL;
      *ps_errno = DS_EINVAL;
      return -1;
    }

    /*-------------------------------------------------------------------------
      Register for FLOW_SUSPENDED_EV
    -------------------------------------------------------------------------*/
    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.suspended =
      ps_flow_alloc_event_cback_buf(assoc_flow_event_cback_f_ptr, flow_ptr);

    if (flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.suspended ==
          NULL)
    {
      LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                      "Out of mem for ev %d", FLOW_SUSPENDED_EV);
      *ps_errno = DS_ENOMEM;
      return -1;
    }

    if (ps_flow_event_cback_reg
        (
          assoc_flow_ptr,
          FLOW_SUSPENDED_EV,
          flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.suspended
        ) != 0)
    {
      LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                      "Could not reg cback for ev %d", FLOW_SUSPENDED_EV);
      ps_flow_free_event_cback_buf
      (
        flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.suspended
      );

      flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.suspended = NULL;
      *ps_errno = DS_EINVAL;
      return -1;
    }

    /*-------------------------------------------------------------------------
      Register for FLOW_NULL_EV
    -------------------------------------------------------------------------*/
    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.null =
      ps_flow_alloc_event_cback_buf(assoc_flow_event_cback_f_ptr, flow_ptr);

    if (flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.null == NULL)
    {
      LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                      "Out of mem for ev %d", FLOW_NULL_EV);
      *ps_errno = DS_ENOMEM;
      return -1;
    }

    if (ps_flow_event_cback_reg
        (
          assoc_flow_ptr,
          FLOW_NULL_EV,
          flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.null
        ) != 0)
    {
      LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                      "Could not reg cback for ev %d", FLOW_NULL_EV);
      ps_flow_free_event_cback_buf
      (
        flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.null
      );

      flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.null = NULL;
      *ps_errno = DS_EINVAL;
      return -1;
    }

    /*-------------------------------------------------------------------------
      Register for FLOW_INFO_CODE_UPDATED_EV
    -------------------------------------------------------------------------*/
    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.info_code_updated =
      ps_flow_alloc_event_cback_buf(assoc_flow_event_cback_f_ptr, flow_ptr);

    if (flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.info_code_updated == NULL)
    {
      LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                      "Out of mem for ev %d", FLOW_INFO_CODE_UPDATED_EV);
      *ps_errno = DS_ENOMEM;
      return -1;
    }

    if (ps_flow_event_cback_reg
        (
          assoc_flow_ptr,
          FLOW_INFO_CODE_UPDATED_EV,
          flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.info_code_updated
        ) != 0)
    {
      LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                      "Could not reg cback for ev %d",
                      FLOW_INFO_CODE_UPDATED_EV);
      ps_flow_free_event_cback_buf
      (
        flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.info_code_updated
      );

      flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.info_code_updated = NULL;
      *ps_errno = DS_EINVAL;
      return -1;
    }

    /*-------------------------------------------------------------------------
      Register for FLOW_MODIFY_ACCEPTED_EV
    -------------------------------------------------------------------------*/
    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.modify_accepted =
      ps_flow_alloc_event_cback_buf(assoc_flow_event_cback_f_ptr,  flow_ptr);

    if (flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.modify_accepted ==
          NULL)
    {
      LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                      "Out of mem for ev %d", FLOW_MODIFY_ACCEPTED_EV);
      *ps_errno = DS_ENOMEM;
      return -1;
    }

    if (ps_flow_event_cback_reg
        (
          assoc_flow_ptr,
          FLOW_MODIFY_ACCEPTED_EV,
          flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.modify_accepted
        ) != 0)
    {
      LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                      "Could not reg cback for ev %d",
                      FLOW_MODIFY_ACCEPTED_EV);
      ps_flow_free_event_cback_buf
      (
        flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.modify_accepted
      );

      flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.modify_accepted = NULL;
      *ps_errno = DS_EINVAL;
      return -1;
    }

    /*-------------------------------------------------------------------------
      Register for FLOW_MODIFY_REJECTED_EV
    -------------------------------------------------------------------------*/
    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.modify_rejected =
      ps_flow_alloc_event_cback_buf(assoc_flow_event_cback_f_ptr,  flow_ptr);

    if (flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.modify_rejected ==
          NULL)
    {
      LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                      "Out of mem for ev %d", FLOW_MODIFY_REJECTED_EV);
      *ps_errno = DS_ENOMEM;
      return -1;
    }

    if (ps_flow_event_cback_reg
        (
          assoc_flow_ptr,
          FLOW_MODIFY_REJECTED_EV,
          flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.modify_rejected
        ) != 0)
    {
      LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                      "Could not reg cback for ev %d",
                      FLOW_MODIFY_REJECTED_EV);
      ps_flow_free_event_cback_buf
      (
        flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.modify_rejected
      );

      flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.modify_rejected = NULL;
      *ps_errno = DS_EINVAL;
      return -1;
    }

  }

  /*-------------------------------------------------------------------------
    Register for FLOW_TX_ENABLED_EV
  -------------------------------------------------------------------------*/
  flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.tx_enabled =
    ps_flow_alloc_event_cback_buf(assoc_flow_event_cback_f_ptr,  flow_ptr);

  if (flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.tx_enabled ==
        NULL)
  {
    LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                    "Out of mem for ev %d", FLOW_TX_ENABLED_EV);
    *ps_errno = DS_ENOMEM;
    return -1;
  }

  if (ps_flow_event_cback_reg
      (
        assoc_flow_ptr,
        FLOW_TX_ENABLED_EV,
        flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.tx_enabled
      ) != 0)
  {
    LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                    "Could not reg cback for ev %d", FLOW_TX_ENABLED_EV);
    ps_flow_free_event_cback_buf
    (
      flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.tx_enabled
    );

    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.tx_enabled = NULL;
    *ps_errno = DS_EINVAL;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Register for FLOW_TX_DISABLED_EV
  -------------------------------------------------------------------------*/
  flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.tx_disabled =
    ps_flow_alloc_event_cback_buf(assoc_flow_event_cback_f_ptr,  flow_ptr);

  if (flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.tx_disabled ==
        NULL)
  {
    LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                    "Out of mem for ev %d", FLOW_TX_DISABLED_EV);
    *ps_errno = DS_ENOMEM;
    return -1;
  }

  if (ps_flow_event_cback_reg
      (
        assoc_flow_ptr,
        FLOW_TX_DISABLED_EV,
        flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.tx_disabled
      ) != 0)
  {
    LOG_MSG_ERROR_1("ps_flowi_assoc_flow_ev_cback_reg(): "
                    "Could not reg cback for ev %d", FLOW_TX_DISABLED_EV);
    ps_flow_free_event_cback_buf
    (
      flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.tx_disabled
    );

    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.tx_disabled = NULL;
    *ps_errno = DS_EINVAL;
    return -1;
  }

  return 0;

} /* ps_flowi_assoc_flow_ev_cback_reg() */

void ps_flowi_assoc_flow_ev_cback_dereg
(
  ps_flow_type  * flow_ptr
)
{
  ps_flow_type * assoc_flow_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("ps_flowi_assoc_flow_ev_cback_dereg(): "
                           "flow 0x%p", flow_ptr);

  assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr);
  if (assoc_flow_ptr == NULL || !PS_FLOW_IS_VALID(assoc_flow_ptr))
  {
    ASSERT(0);
    LOG_MSG_FUNCTION_EXIT_1("ps_flowi_assoc_flow_ev_cback_dereg(): "
                            "Fail, flow 0x%p", flow_ptr);
    return;
  }

  ps_flowi_delete_ev_cback_buf
  (
    flow_ptr,
    assoc_flow_ptr,
    FLOW_ACTIVATED_EV,
    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.activated
  );

  ps_flowi_delete_ev_cback_buf
  (
    flow_ptr,
    assoc_flow_ptr,
    FLOW_SUSPENDED_EV,
    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.suspended
  );

  ps_flowi_delete_ev_cback_buf
  (
    flow_ptr,
    assoc_flow_ptr,
    FLOW_NULL_EV,
    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.null
  );

  ps_flowi_delete_ev_cback_buf
  (
    flow_ptr,
    assoc_flow_ptr,
    FLOW_INFO_CODE_UPDATED_EV,
    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.
    info_code_updated
  );

  ps_flowi_delete_ev_cback_buf
  (
    flow_ptr,
    assoc_flow_ptr,
    FLOW_MODIFY_ACCEPTED_EV,
    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.modify_accepted
  );

  ps_flowi_delete_ev_cback_buf
  (
    flow_ptr,
    assoc_flow_ptr,
    FLOW_MODIFY_REJECTED_EV,
    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.modify_rejected
  );

  ps_flowi_delete_ev_cback_buf
  (
    flow_ptr,
    assoc_flow_ptr,
    FLOW_TX_ENABLED_EV,
    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.tx_enabled
  );

  ps_flowi_delete_ev_cback_buf
  (
    flow_ptr,
    assoc_flow_ptr,
    FLOW_TX_DISABLED_EV,
    flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf.tx_disabled
  );

  memset(&(flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf),
         0,
         sizeof(flow_ptr->flow_private.logical_flow.ps_flow_ev_cback_buf));

  LOG_MSG_FUNCTION_EXIT_1("ps_flowi_assoc_flow_ev_cback_dereg(): "
                          "Success, flow 0x%p", flow_ptr);

} /* ps_flowi_assoc_flow_ev_cback_dereg */

void ps_flowi_process_assoc_flow_ev
(
  dcc_cmd_enum_type   cmd,
  void              * user_data_ptr
)
{
#ifdef FEATURE_DATA_PS_QOS
  ps_flow_modify_param_type                modify_param;
  ps_logical_flow_assoc_flow_ev_cmd_type * assoc_flow_cmd_ptr;
  ps_iface_event_info_u_type               event_info;
  ps_flow_type                           * flow_ptr;
  ps_flow_type                           * assoc_flow_ptr;
  int32                                    ret_val;
  ps_iface_event_enum_type                 event;
  int16                                    ps_errno;
  uint8                                    logical_flow_cookie;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("ps_flowi_process_assoc_flow_ev(): "
                           "cmd %d called with user_data_ptr 0x%p",
                           cmd, user_data_ptr);

  if (user_data_ptr == NULL)
  {
    LOG_MSG_INVALID_INPUT_0("ps_flowi_process_assoc_flow_ev(): "
                            "NULL DS cmd ptr");
    return;
  }

  if (cmd != DCC_LOGICAL_FLOW_ASSOC_FLOW_EV_CMD)
  {
    LOG_MSG_INVALID_INPUT_1("ps_flowi_process_assoc_flow_ev(): "
                            "Invalid DCC cmd id %d", cmd);
    PS_SYSTEM_HEAP_MEM_FREE(user_data_ptr);
    return;
  }

  assoc_flow_cmd_ptr = (ps_logical_flow_assoc_flow_ev_cmd_type*) user_data_ptr;

  assoc_flow_ptr      = assoc_flow_cmd_ptr->assoc_flow_ptr;
  event               = assoc_flow_cmd_ptr->event;
  event_info          = assoc_flow_cmd_ptr->event_info;
  logical_flow_cookie = assoc_flow_cmd_ptr->logical_flow_cookie;
  flow_ptr            =(ps_flow_type *) (assoc_flow_cmd_ptr->user_data_ptr);

  PS_SYSTEM_HEAP_MEM_FREE(user_data_ptr);

  LOG_MSG_INFO1_3("ps_flowi_process_assoc_flow_ev(): "
                  "Processing event, %d on assoc_ps_flow 0x%p for flow 0x%p",
                  event, assoc_flow_ptr, flow_ptr);

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    LOG_MSG_INFO1_2("ps_flowi_process_assoc_flow_ev(): "
                    "Ignoring event %d as flow 0x%p is invalid",
                    event, flow_ptr);
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    return;
  }
  else if (PS_FLOWI_GET_COOKIE(flow_ptr) != logical_flow_cookie)
  {
    LOG_MSG_INFO1_2("ps_flowi_process_assoc_flow_ev(): "
                    "Ignoring event %d as flow 0x%p is re-allocated",
                    event, flow_ptr);
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return;
  }

  /*-------------------------------------------------------------------------
    Make sure that logical flow is associated to assoc_flow_ptr
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr) != assoc_flow_ptr)
  {
    LOG_MSG_ERROR_3("ps_flowi_process_assoc_flow_ev(): "
                    "Flow 0x%p is not associated to flow 0x%p but to 0x%p",
                    flow_ptr,
                    assoc_flow_ptr,
                    PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr));
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return;
  }

  /*-------------------------------------------------------------------------
    Ignore all events except for FLOW_NULL_EV as it is possible that
    associated flow is alread deleted by the time DCC task is scheduled to
    process events on assoc flow.

    Associated flow is not deleted only if it still valid and if cookie matches
  -------------------------------------------------------------------------*/
  if (event != FLOW_NULL_EV)
  {
    if (!PS_FLOW_IS_VALID(assoc_flow_ptr))
    {
      LOG_MSG_INFO1_3("ps_flowi_process_assoc_flow_ev(): "
                      "Ignoring event %d on flow 0x%p as assoc flow 0x%p is "
                      "already deleted", event, flow_ptr, assoc_flow_ptr);
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return;
    }
    else if (PS_FLOWI_GET_COOKIE(assoc_flow_ptr) !=
             PS_FLOWI_GET_ASSOC_COOKIE(flow_ptr))
    {
      LOG_MSG_INFO1_3("ps_flowi_process_assoc_flow_ev(): "
                      "Ignoring event %d on flow 0x%p as assoc flow 0x%p is "
                      "re-allocated", event, flow_ptr, assoc_flow_ptr);
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return;
    }
  }

  /*-------------------------------------------------------------------------
    Process solid state events by posting corresponding indications etc.
  -------------------------------------------------------------------------*/
  switch (event)
  {
    case FLOW_ACTIVATED_EV:
      ps_flow_activate_ind(flow_ptr, event_info.phys_link_event_info.info_code);
      break;

    case FLOW_NULL_EV:
      ps_flow_go_null_ind(flow_ptr, event_info.phys_link_event_info.info_code);
      if (ps_iface_delete_flow(flow_ptr->flow_private.iface_ptr,
                               flow_ptr,
                               &ps_errno) != 0)
      {
        LOG_MSG_ERROR_1("ps_flowi_process_assoc_flow_ev(): "
                        "Could not delete PS flow 0x%p", flow_ptr);
      }

      break;

    case FLOW_SUSPENDED_EV:
      ps_flow_suspend_ind(flow_ptr, event_info.phys_link_event_info.info_code);
      break;

    case FLOW_INFO_CODE_UPDATED_EV:
      ps_flow_generic_ind(flow_ptr,
                          FLOW_INFO_CODE_UPDATED_EV,
                          &(event_info.phys_link_event_info.info_code));
      break;

    case FLOW_MODIFY_ACCEPTED_EV:
      if (!PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))
      {
        LOG_MSG_INFO1_1("ps_flowi_process_assoc_flow_ev(): "
                        "Network inited qos modify accept 0x%p", flow_ptr);

        /*---------------------------------------------------------------------
          Copy the modify info pointer from the underlying assoc flow
        ---------------------------------------------------------------------*/
        memset(&modify_param, 0, sizeof(ps_flow_modify_param_type));
        ret_val = ps_flowi_get_modify_qos_spec_from_flow
                  (
                    flow_ptr,
                    &modify_param.qos_spec,
                    &ps_errno
                  );

        if (0 != ret_val)
        {
          LOG_MSG_INFO1_2("ps_flowi_process_assoc_flow_ev(): "
                          "Fail ps_flowi_get_qos_spec_type_from_flow, flow 0x%p,"
                          "errno %d", flow_ptr, ps_errno);
          break;
        }

        modify_param.fltr_priority = PS_IFACE_IPFLTR_PRIORITY_DEFAULT;
        modify_param.subset_id     = 0;

        ret_val = ps_iface_modify_flow(flow_ptr->flow_private.iface_ptr,
                                       flow_ptr,
                                       &modify_param,
                                       &ps_errno);

        ps_iface_logical_flowi_free_qos_spec(&(modify_param.qos_spec));

        if (0 != ret_val)
        {
          LOG_MSG_INFO1_2("ps_flowi_process_assoc_flow_ev(): "
                          "Fail ps_iface_modify_flow, flow 0x%p, errno %d",
                          flow_ptr, ps_errno);
          break;
        }
      }

      if (ps_iface_modify_flow_accepted(flow_ptr->flow_private.iface_ptr,
                                        flow_ptr,
                                        event_info.flow_event_info.info_code,
                                        &ps_errno ) != 0 )
      {
        LOG_MSG_ERROR_1("ps_flowi_process_assoc_flow_ev(): "
                        "Could not modify PS flow 0x%p", flow_ptr);
      }

      break;

    case FLOW_MODIFY_REJECTED_EV:
      if (PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))
      {
          if (ps_iface_modify_flow_rejected
              (
                flow_ptr->flow_private.iface_ptr,
                flow_ptr,
                event_info.flow_event_info.info_code,
                &ps_errno
              ) != 0 )
          {
            LOG_MSG_ERROR_1("ps_flowi_process_assoc_flow_ev(): "
                            "Could not modify PS flow 0x%p", flow_ptr);
            ASSERT(0);
          }
      }

      break;

    case FLOW_TX_ENABLED_EV:
      ps_flow_enable_tx(flow_ptr, DS_FLOW_PROXY_MASK);
      break;

    case FLOW_TX_DISABLED_EV:
      ps_flow_disable_tx(flow_ptr, DS_FLOW_PROXY_MASK);
      break;

    case FLOW_FLTR_AUX_INFO_UPDATED_EV:
      ret_val = ps_iface_logical_flowi_process_fltr_aux_info_updated_ev
                (
                  assoc_flow_ptr->flow_private.iface_ptr,
                  assoc_flow_ptr,
                  flow_ptr,
                  &ps_errno
                );

      if (0 != ret_val)
      {
        LOG_MSG_ERROR_2("ps_flowi_process_assoc_flow_ev(): "
                        "ps_iface_logical_flowi_process_fltr_aux_info_updated_ev() "
                        "failed, PS flow 0x%p err %d", flow_ptr, ps_errno);
      }

      break;

    default:
      ASSERT(0);
      LOG_MSG_ERROR_1("ps_flowi_process_assoc_flow_ev(): "
                      "Invalid event %d", event);
      break;
  } /* switch (event) */

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
#endif /* FEATURE_DATA_PS_QOS */

  return;
} /* ps_flowi_process_assoc_flow_ev() */
