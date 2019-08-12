
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            Event Service

General Description
  This file containsthe implementation for the event reporting service.

  An event consists of a 12 bit event ID and a timestamp.

  The requirement is that the DMSS will never drop events as long as it does
  not exceed bandwidth limitations with event reporting.  Event reporting
  has the highest priority of all unsolicited diagnostic data.

  To minimize impact of the caller's task, the event is generated and placed
  in a queue.  The DIAG task will build the final packet structure and send
  to the external device.

  Just in case an event is dropped, a "drop event" will be reported and a
  count of dropped events will be reproted as payload.

Initialization and Sequencing Requirements
  Call event_init() before reporting any events.

Copyright (c) 2011-2015 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                              Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/rtos/src/event_api.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/18/15   sa      Added support for time sync feature.
08/22/14   ph      Extended listener masks are updated properly when a listener is removed.
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
12/19/13   rs      Fixed LLVM compilation warnings. 
10/14/13   sr      Added support for extended event, log and F3 pkts  
09/20/13   tbg     Added support for Diag over STM 
09/18/13   sa      Free the stale listeners array in the diag context.
09/18/13   sa      Suppport for listeners mask.
04/22/13   ph      converted diag_listeners heap usage to system heap.
04/12/13   ph      Fixed race condition which avoids double init crit sect of 
                   event_listeners_ext.
04/03/13   sr      Replace memcpy() and memmove() with memscpy() and memsmove() 
02/08/13   sr      Fixed compiler  and Klockwork warnings  
08/02/13   sr      Added support for extended event APIs 
12/20/12   rh      Removed event tracing for F3 Trace
08/24/12   rh      Fixed event listener allocation/deallocation race condition
08/03/12   rh      Moved event listeners to using diag_listeners_malloc and free 
07/25/12   rh      Moved event_config and event_config_sec delcarations to eventi.h
06/26/12   sa      Added new extended event API to remove a listener which takes
                   listener and param as arguments.
03/30/12   hvm     changes for invoking all registered listeners for a event
03/26/12   hvm     support for storing params to be returned to callback
11/07/11   is      Fixed compiler warnings
10/26/11   hm      Extended Event/Log Listener APIs added  
06/23/11   vs      changes to make event_listeners accessible
04/27/11   is      Resolve compiler warnings
10/29/10   vs      Changes by debugtools for lock-less implementation
09/28/10   sg     Moved diag_time_get to diag_cfg.h
09/17/10   is      Migrate from deprecated clk driver APIs
08/25/10   sg      Fixed compiler warnings
06/30/10   sg      Moved the inclusion of diagi_v.h up for DIAG_QDSP6_APPS_PROC
                   to be defined earlier before using it on adsp
04/20/10   sg     Created file.

===========================================================================*/
#include "core_variation.h"
#include "comdef.h"
#include "event.h"
#include "eventi.h"
#include "msg.h"
#include "osal.h"
#include "stringl.h"
#include "diagtarget.h"
#include "diagdiag_v.h" 
#include "diag_cfg.h"
#include "diagi_v.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#ifndef FEATURE_WINCE
#include "dog.h"
#endif 
#endif

#include "diag_stm.h"


#define EVENT_CONFIG_LISTENER_MASK 0x02

/* Time Get Function Pointer from diag.c */
extern uint8 (*diag_time_get)(qword time);

void event_service_listener (event_store_type *event_store);

#if defined (DIAG_MV_LISTENER_RPC)
extern boolean diag_add_event_listener_on_modem(
    const unsigned int event_id,  
    const diag_event_listener listener,  
    void *param);

extern boolean diag_remove_event_listener_on_modem(
    const unsigned int event_id,  
    const diag_event_listener listener,  
    void *param);
#endif
extern byte event_read_stream_id(event_id_enum_type event_id);
/* From event.c */
extern unsigned char diag_event_listener_mask[EVENT_MASK_SIZE];
extern unsigned char diag_event_listener_ext_mask[EVENT_MASK_SIZE];
typedef struct
{
  diag_event_listener listener;
  void *param; /* User's parameter */
}
event_listener_entry_type;

/* This data member is used for the event listener functinality.
   The API is in diag.h. */
/*lint -save -e{785} */
extern  uint32* diag_event_listeners_heap_to_delete[];
diag_searchlist_type event_listeners = {FALSE, {0, 0, 0}, {{0}}, 0, 0, 0,0, NULL, NULL};
diag_searchlist_ext_type event_listeners_ext = {{NULL,NULL,0}, FALSE, 0, 0, {{0}}};
#ifdef FEATURE_DEBUG_DIAG_HEAP
uint32 event_listener_match_and_free_cnt =0;
uint32 diag_add_event_listener_malloc_cnt = 0;
uint32 diag_add_event_listener_free_cnt = 0;
#endif 
static int event_ext_init_start = FALSE;
static boolean event_listeners_ext_init(void);

/*lint -restore */

boolean event_searchlist_ext(uint16 event_id, diag_event_type *event)
{
  event_searchlist_ext_entry_type *node = NULL;
  boolean listener_registered = FALSE;
  
  uint32 index  = 0;
  uint32 mask   = 0;

  if(!event_listeners_ext.initialized)
  {
    return FALSE;
  }

  osal_enter_crit_sect(&event_listeners_ext.lock);
  node = (event_searchlist_ext_entry_type *)list_peek_front((list_type *)&event_listeners_ext);

  if(node)
  {
      if (event_id <= EVENT_LAST_ID)
      {
        index  = event_id/8;
        mask   = (1 << ((event_id) & 0x07));
      }
  }

  while(node)
  {
      
        if(node->entry.mask[index] & mask)
        {
          if(event && (FALSE == node->remove_flag) )
          {
            node->entry.listener(event, node->entry.param);
          }
          listener_registered = TRUE;
        }

        node = (event_searchlist_ext_entry_type *)list_peek_next((list_type *)&event_listeners_ext, (list_link_type *)node);
      
  }
  osal_exit_crit_sect(&event_listeners_ext.lock);

  return listener_registered;
}/* event_searchlist_ext */


#if defined FEATURE_DIAG_STM
/*===========================================================================     
     
FUNCTION EVENT_GENERATE_STM
     
DESCRIPTION     
  Generate STM event report for event being generated, and send immediately
 
PARAMETERS 
  event_id      ID of the event to be generated
  length        Length of the payload pointed to by 'payload'
  *payload      Pointer to the event payload
  version       Extended packet version information
                  0 - Standard event packet (no extended header)
                  1 - Extended packet event (v1)
  *parameter    Parameter associated with extended packets

=============================================================================*/
static void event_generate_stm (event_id_enum_type event_id, uint8 length, void *payload, unsigned int version, void *parameter)
{
  event_rpt_type stm_event_hdr;
  uint8 *event_pkt, *ptr;
  event_id_type stm_event_id;
  uint32 pkt_length;
  uint32 copy_bytes;
  uint32 hdr_size = sizeof(event_rpt_type)-1;
  qword diag_ts;
  diag_cmd_ext_v1_type *event_ext_hdr = NULL;

  /* Calculate packet length for event report (single event) */
  pkt_length = sizeof(event_rpt_type) - 1;
  pkt_length += sizeof(event_type);
  pkt_length += length;
  if (length > EVENT_PAY_TWO_BYTE)
  {
    /* include extra byte for payload length field */
    pkt_length += 1;
  }

  /* Check if this is an extended report and allow space for header */
  if (version == DIAGBUF_VERSION_1)
  {
    if (parameter == NULL)
    {
      /* No parameter includes with v1...return without sending anything */
      return;
    }
    pkt_length += sizeof(diag_cmd_ext_v1_type);
  }

  /* Allocate packet for event report */
  event_pkt = diag_stm_event_alloc(pkt_length);

  if (event_pkt)
  {
    ptr = event_pkt;

    if (version == DIAGBUF_VERSION_1)
    {
      /* Populate extended header information*/
      event_ext_hdr = (diag_cmd_ext_v1_type*)event_pkt;
      event_ext_hdr->cmd_code = DIAG_CMD_EXT_F;
      event_ext_hdr->id = *(uint32*)parameter;
      event_ext_hdr->version = DIAGBUF_VERSION_1;
      event_ext_hdr->proc_id = DIAG_MY_PROC_ID;

      /* Move pionter past header to start of event report */
      ptr += sizeof(diag_cmd_ext_v1_type);
    }

    /*  Note: Not accounting for dropped STM events here if alloc fails.
        Should be added?
     */

    /* Set fields of event_report */
    stm_event_hdr.cmd_code = DIAG_EVENT_REPORT_F;
    stm_event_hdr.length = pkt_length - hdr_size;

    copy_bytes = memscpy(ptr,pkt_length,&stm_event_hdr,hdr_size);
    ptr += copy_bytes;

    stm_event_id.id = event_id;
    if (length <= EVENT_PAY_TWO_BYTE)
    {
      stm_event_id.payload_len = length;
    }
    else
    {
      stm_event_id.payload_len = EVENT_PAY_LENGTH;
    }
    stm_event_id.time_trunc_flag = FALSE;

    copy_bytes = memscpy(ptr,(pkt_length-(ptr-event_pkt)),
                          &stm_event_id,sizeof(event_id_type));
    ptr += copy_bytes;

    diag_time_get(diag_ts);
    copy_bytes = memscpy(ptr,(pkt_length-(ptr-event_pkt)),diag_ts,sizeof(qword));
    ptr += copy_bytes;

    if (payload)
    {
      if (length > EVENT_PAY_TWO_BYTE)
      {
        *ptr++ = length;
      }
      /* Copy payload */
      copy_bytes = memscpy(ptr,(pkt_length-(ptr-event_pkt)),payload,length);
    }

    /* Send event report to STM */
    diag_stm_write_pkt((void*)event_pkt,pkt_length);

    /* Free event report */
    diag_stm_event_free((void*)event_pkt);
  }
}
#endif /*/ FEATURE_DIAG_STM */


/*===========================================================================
FUNCTION EVENT_REPORT_EXT

DESCRIPTION
   Report an event without payload data . Similar to event_report() except
   that this function can take additional parameters - version and void * to
   specify information regarding where the event is being generated.
 
PARAMETERS
   event_id       ID of the event to be reported.Event IDs are defined in event_defs.h
   version        Specifies the version - currently, there are two versions 
                  0 - same as event_report() (for backwards compatibility)
                  1 - The outgoing event report header will have support to
                      include ID field to differentiate the events coming
                      from different instances of the processor.
  *parameter      Based on the version passed in, this pointer is interpreted
                  as follows-
                  version       pointer
                  0             NULL
                  1             uint32 * . The uint32 value this pointer points to,
                                is the ID of the instance generating this event.
 
RETURN VALUE 
  TRUE: The event was successfully queued
  FALSE: The event was not queued
=========================================================================*/
boolean event_report_ext(event_id_enum_type event_id, unsigned int version, void *parameter)
{
  event_store_type *event= NULL;
  event_store_type *event_ctrl = NULL;
  byte stream_id = 0;
  boolean status = FALSE;
  uint32 *id = NULL;
  boolean ret_value = FALSE;

  if (version > DIAGBUF_MAX_VERSION_NUM) 
  {
    return FALSE;
  }

  if ((version == DIAGBUF_VERSION_1) && parameter == NULL)
  {
    return FALSE;
  }


  /* This will check which masks are enabled */
  stream_id = event_read_stream_id(event_id);

  if( !stream_id )
  {
    status = event_mask_enabled(diag_event_listener_mask, event_id);
    if( !status)
    {
      status = event_mask_enabled(diag_event_listener_ext_mask, event_id);
    }
  }

  if (status)
  {
    stream_id |= DIAG_STREAM_LISTENER;
  }

#if defined FEATURE_DIAG_STM
  /* Is STM enabled, allocate entire event report and send event immediately. */
  if( (diag_stm_enabled) && (stream_id & DIAG_STREAM_STM))
  {
    event_generate_stm(event_id,0,NULL,version,parameter);
    /* Remove STREAM_STM from consideration now that STM has been sent */
    /* If a listener is active that STREAM flag will still be set    */
    stream_id &= ~DIAG_STREAM_STM;
  }

#endif /* FEATURE_DIAG_STM */

  /* Checks for event registration on stream_id and listeners done in 
     event_q_alloc() */
  if (stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER))
  {
    event = (event_store_type *) event_q_alloc (event_id, 0,
            (stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER)));
    if (event)
    {
      event->stream_id = (stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER));
      switch (version)
      {
      case DIAGBUF_VERSION_0:
        event->version = DIAGBUF_VERSION_0;
        event->id = 0;
        break;
        
      case DIAGBUF_VERSION_1:
        id = (uint32 *)parameter;
        event->version = DIAGBUF_VERSION_1;
        event->id = *id;
        break;

      default:
        return FALSE;
      }
      event_q_put (event,(stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER)));
      ret_value = TRUE;
    } 
    else
    {
      /*Error: could not allocate event - increase the drop count*/
    }
  }

  if(stream_id & DIAG_STREAM_2)
  {
    event_ctrl = (event_store_type *) event_q_alloc (event_id, 0,DIAG_STREAM_2);
    if (event_ctrl)
    {
      event_ctrl->stream_id = DIAG_STREAM_2;
      switch (version)
      {
      case 0:
        event_ctrl->version = 0;
        event_ctrl->id = 0;
        break;
      case 1:
        /*DCI does not support extended packets yet*/
        event_ctrl->version = 0;
        event_ctrl->id = 0;
        break;
      default:
        return FALSE;
      }
      event_q_put(event_ctrl, DIAG_STREAM_2);
      ret_value = TRUE;
    } 
    else
    {
      /*Error: could not allocate event */
    }

  }
  return ret_value;
}

/*===========================================================================
FUNCTION EVENT_REPORT

DESCRIPTION
   Static events are reported by the mobile device to indicate actions, such
   as state changes and configuration, which are directly related to the
   operating standards of the system.The intent is to provide a reporting
   mechanism that minimizes resource usage (in particular, bandwidth) and
   maximizes storage and transmission priority for operating information that
   is considered most important in software test and verification.
 
PARAMETERS
   event_id   ID of the event to be reported.Event IDs are defined in event_defs.h
 
RETURN VALUE 
   None 
=========================================================================*/
void
event_report (event_id_enum_type event_id)
{
  event_report_ext(event_id, 0, NULL);
  return;
}                               /* event_report */

/*===========================================================================
FUNCTION EVENT_REPORT_PAYLOAD

DESCRIPTION
   Report an event with payload data .Payload structure format must be published
 
PARAMETERS
   event_id   ID of the event to be reported.Event IDs are defined in event_defs.h
   length     Length of the associated  payload
   *data      Pointer to the payload structure .Payload should be published.
 
RETURN VALUE 
   None
=========================================================================*/
void
event_report_payload (event_id_enum_type event_id, uint8 length, void *payload)
{
  event_report_payload_ext(event_id, length, payload, 0, NULL);
  return;
}                               /* event_report_payload */

/*===========================================================================
FUNCTION EVENT_REPORT_PAYLOAD_EXT

DESCRIPTION
   Report an event with payload data .Payload structure format must be published
 
PARAMETERS
   event_id   ID of the event to be reported.Event IDs are defined in event_defs.h
   length     Length of the associated  payload
   *data      Pointer to the payload structure .Payload should be published.
   version    Specifies the version - currently, there are two versions 
               0 - same as event_report_payload() (for backwards compatibility)
               1 - The outgoing event report header will have support to
                   include ID field to differentiate the events
                   coming from different instances of the processor.
   
   *parameter   Based on the version passed in, this pointer is interpreted
                as follows-
                version       pointer
                 0             NULL
                 1             uint32 * . The uint32 value this pointer points to,
                               is the ID of the instance generating this event.
 
RETURN VALUE 
  TRUE:  The event was successfully queued
  FALSE: The event was not queued
=========================================================================*/
boolean
event_report_payload_ext(event_id_enum_type event_id, uint8 length, void *payload, unsigned int version, void *parameter)
{
  event_store_type *event = NULL;
  event_store_type *event_ctrl = NULL;
  byte stream_id = 0;
  boolean status = FALSE;
  uint32 *id = NULL;
  boolean ret_value = FALSE;

  if (version > DIAGBUF_MAX_VERSION_NUM)
  {
    return FALSE;
  }
 
  if ((version == DIAGBUF_VERSION_1) && (parameter == NULL))
  {
    return FALSE;
  }
  
  if (length > 0 && payload)
  {
  /* Checks for event registration on stream_id and listeners done in 
     event_q_alloc() */
    
    stream_id = event_read_stream_id(event_id);

  if( !stream_id )
  {
    status = event_mask_enabled(diag_event_listener_mask, event_id);
    if( !status)
    {
      status = event_mask_enabled(diag_event_listener_ext_mask, event_id);
    }
  }

    if (status)
    {
      stream_id |= DIAG_STREAM_LISTENER;
    }

#if defined FEATURE_DIAG_STM
    if( (diag_stm_enabled) && (stream_id & DIAG_STREAM_STM))
    {
      event_generate_stm(event_id,length,payload,version,parameter);
      /* Remove STREAM_STM from consideration now that STM has been sent */
      /* If a listener is active that STREAM flag will still be set    */
      stream_id &= ~DIAG_STREAM_STM;
    }
#endif /* FEATURE_DIAG_STM */

  /* Checks for event registration on stream_id and listeners done in 
     event_q_alloc() */
    if ( (stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER)) )
      event = (event_store_type *) event_q_alloc (event_id, length , 
              (stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER)));

    if (event)
    {
      event->stream_id = (stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER));
      switch (version)
        {
        case DIAGBUF_VERSION_0:
          event->version = DIAGBUF_VERSION_0;
          event->id = 0;
          break;
        case DIAGBUF_VERSION_1:
          id = (uint32 *)parameter;
          event->version = DIAGBUF_VERSION_1;
          event->id = *id;
          break;
        default:
          return FALSE;
        }
      memscpy (event->payload.payload, event->payload.length, payload, length);
      event_q_put(event,(stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER)));
        ret_value = TRUE;
  }
  else
  {
        /*Error: could not allocate event*/
  }

    if ((stream_id & DIAG_STREAM_2))
    {
      event_ctrl = (event_store_type *)event_q_alloc(event_id, length, DIAG_STREAM_2);
      if (event_ctrl)
      {
        event_ctrl->stream_id = DIAG_STREAM_2;
        switch (version)
        {
        case 0:
          event_ctrl->version = 0;
          event_ctrl->id = 0;
          break;
        case 1:
          /* DCI packets do not support extended events yet*/
          event_ctrl->version = 0;
          event_ctrl->id = 0;
          break;
        default:
          return FALSE;
        }
        memscpy(event_ctrl->payload.payload, event_ctrl->payload.length, payload, length);
        event_q_put(event_ctrl, DIAG_STREAM_2);
        ret_value = TRUE;
      }
    }
  } 
  else
  {
    event_report_ext(event_id, version, parameter);
  }
  return ret_value;
}
/*===========================================================================

FUNCTION EVENT_LISTENER_NOTIFY

DESCRIPTION
  This routine is called by the searchlist when a matching listener node is
  found.  It is called once per entry.  
  
  This routine is a wrapper to call the registered listeners.
  
===========================================================================*/
static void
event_listener_notify (uint32 seq_num, void *event, diag_searchlist_node_type *node)
{
  event_listener_entry_type *entry = NULL;
  diag_event_type *event_notify = (diag_event_type *) event;
  
  if (node && event)
  {
    entry = (event_listener_entry_type *) node->goo;

    if (entry->listener)
    {
      entry->listener (seq_num, event_notify, entry->param);
    }    
  }
}

/*===========================================================================

FUNCTION EVENT_SERVICE_LISTENER

DESCRIPTION
  This routine is to be called once per event before the event is formatted
  for the outbound comm packet.
  
ASSUMPTIONS
  'event_store_type' and 'diag_event_type' are assumed to be formatted 
  the same, with the exception of the q_link at the beginning of the store
  type.  Changing either types could result in undesired behavior.

===========================================================================*/
void
event_service_listener (event_store_type *event_store)
{
  if (event_store && (event_config & EVENT_CONFIG_LISTENER_MASK))
  {
    /* NOTE: diag_event_type and event_store_type purposely use the same
       format, except that event_store_type is preceeded by a Q link
       and the event ID field has a union for internal packet formatting.
       If either types are changed, the service will not function properly.
       &event_store->event_id is cast to diag_event_type * in 
       event_listener_notify(). */
    if(event_mask_enabled(diag_event_listener_mask, event_store->event_id.id))
    {
    (void) diag_searchlist_search_all (event_store->event_id.id, 
                                &event_listeners, 
                                (void *) &event_store->event_id, /* cast!!! */
                                event_listener_notify);
    }

  }
}

/*===========================================================================

FUNCTION EVENT_LISTENER_MATCH_AND_FREE

DESCRIPTION
  This routine is passed to and called by diag_searchlist_delete() for each 
  node of the search until this routine returns TRUE or the search is 
  exhausted.  This routine is responsible for freeing the 'entry' which 
  was allocated in diag_add_event_listener().

===========================================================================*/
static boolean
event_listener_match_and_free (diag_searchlist_node_type *match_node, 
                               diag_searchlist_node_type *list_node)
{
  boolean found = FALSE;
  event_listener_entry_type *match = NULL;
  event_listener_entry_type *entry = NULL;

  if (match_node && list_node)
  {
    match = (event_listener_entry_type *) match_node->goo;
    entry = (event_listener_entry_type *) list_node->goo;

    if (match && entry)
    {
      if (match->listener == entry->listener &&
          match->param == entry->param && FALSE == list_node->remove_flag )
      {
        found = TRUE;
        list_node->remove_flag = TRUE;

#ifdef FEATURE_DEBUG_DIAG_HEAP
        event_listener_match_and_free_cnt++;
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "FREE event_listener_match_and_free = %d ",
                event_listener_match_and_free_cnt);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
      }
    }
  }

  return found;
}

/*=========================================================================*/
boolean
diag_add_event_listener (const unsigned int event_id,
             const diag_event_listener listener, void *param)
{
  event_listener_entry_type *entry = NULL;
  boolean success = FALSE;
  
  entry = (event_listener_entry_type *) 
    diag_listeners_malloc (sizeof (event_listener_entry_type));
  
  
#if defined (DIAG_MV_LISTENER_RPC)
  diag_add_event_listener_on_modem(event_id, listener, param);
#endif

  if (entry)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_add_event_listener_malloc_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "MALLOC diag_add_event_listener_malloc = %d Bytes allocated %d",
        diag_add_event_listener_malloc_cnt, sizeof (event_listener_entry_type));
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    entry->listener = listener;
    entry->param = param;

    success = diag_searchlist_add (&event_listeners, event_id, entry);

    if (success)
    {
      /* Assume the critical section is initialized (success, right?). */
      osal_enter_crit_sect (&event_listeners.crit_sect);

	  if(NULL == event_listeners.listeners_list_to_delete )
      {
        event_listeners.listeners_list_to_delete = diag_event_listeners_heap_to_delete;
      }
      DIAG_SET_MASK (diag_cx_state, DIAG_CX_EVENT_LISTEN_S);

      event_control (
        (event_listeners.count > 0) ? EVENT_CONFIG_LISTENER_MASK : 0, 
        EVENT_CONFIG_LISTENER_MASK);

      osal_exit_crit_sect (&event_listeners.crit_sect);
	   /* Update the event listener mask */
      diag_event_listener_mask[event_id/8] |= ( 1 << (event_id & 0x07));
    }
    else
    {
      /* unable to add, free 'entry' */
      diag_listeners_free ((void *) entry);
#ifdef FEATURE_DEBUG_DIAG_HEAP
      diag_add_event_listener_free_cnt++;
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "Unable to add event listener freeing it,  \
			diag_add_event_listener_free_cnt = %d ",
            diag_add_event_listener_free_cnt);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    }
  }

  return success;
}

/*===========================================================================

FUNCTION EVENT_REMOVE_EVENT_LISTENER

DESCRIPTION
  This is the packet handler for the event service configuration packet.

===========================================================================*/
boolean
diag_remove_event_listener (const unsigned int event_id,
                const diag_event_listener listener, void *param)
{
  event_listener_entry_type entry;
  diag_searchlist_node_type node;
  boolean success = FALSE;
    unsigned char mask = 0;
  boolean status = FALSE;
  
  entry.listener = listener;
  entry.param = param;
  
  node.key = event_id;
  node.goo = (void *) &entry;

#if defined (DIAG_MV_LISTENER_RPC)
  diag_remove_event_listener_on_modem(event_id, listener, param);
#endif

  success = diag_searchlist_delete (&event_listeners, &node,
                                    event_listener_match_and_free);

  if (success)
  {
    /* Assume the critical section is initialized (success, right?). */
    osal_enter_crit_sect (&event_listeners.crit_sect);

    event_control (
      (event_listeners.count > 0) ? EVENT_CONFIG_LISTENER_MASK : 0, 
      EVENT_CONFIG_LISTENER_MASK);
    /* Check if there are any more listeners registered for this event id*/
    status = diag_searchlist_search_all(event_id, &event_listeners,NULL, NULL);

    
    if(FALSE == status)
    {
      mask = ~(1 << (event_id & 0x07 ));
      diag_event_listener_mask[event_id/8] &= mask;
    }

    osal_exit_crit_sect (&event_listeners.crit_sect);
  }

  return success;
}

boolean
diag_add_event_listener_ext (const uint16 *events, const unsigned int num_events, 
                         const diag_event_listener_ext listener, void *param)
{
  event_searchlist_ext_entry_type *node = NULL;

  uint16 count = 0;
  uint16 current_event = 0; 
  boolean listener_ext_init = event_listeners_ext.initialized;
  
  if(!listener)
  {
    return FALSE;
  }
  while (!listener_ext_init)
  {
    listener_ext_init = event_listeners_ext_init();
	/* Register extended Event listener clean up function for idle processing*/
    diag_idle_processing_register(diag_clean_event_listener_ext, NULL);
  }

  node = (event_searchlist_ext_entry_type *)diag_listeners_malloc(sizeof(event_searchlist_ext_entry_type));

  if(node)
  {
    memset(node, 0, sizeof(event_searchlist_ext_entry_type));
	node->remove_flag = FALSE;
    /* Save the callback function and ptr to argument to be passed to the cb func */
    node->entry.listener = listener;
    node->entry.param = param;

    osal_enter_crit_sect(&event_listeners_ext.lock);
    while(count++ < num_events)
    {
      current_event = *events++;
      if(current_event <= EVENT_LAST_ID)
      {
        node->entry.mask[current_event/8] |= (1 << ((current_event) & 0x07));
        node->entry.num_events_set++;
		        /* Update the event mask listener */
        diag_event_listener_ext_mask[current_event/8] |= (1 << ((current_event) & 0x07));
      }
      else
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, 
                "Attempt to listen to invalid Event:%d",current_event);
      }
    }

    list_push_back((list_type *)&event_listeners_ext, (list_link_type *)&node->link);

    event_listeners_ext.count++;

    if(event_listeners_ext.count > 0)
    {
        DIAG_SET_MASK (diag_cx_state, DIAG_CX_EVENT_LISTEN_EXT_S);
    }
    osal_exit_crit_sect(&event_listeners_ext.lock);

    return TRUE;
  }
  else
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "Cannot allocate memory for extended event listener");
    return FALSE;
  }
} /* diag_add_event_listener_ext */

/*===========================================================================

FUNCTION DIAG_REMOVE_EVENT_LISTENER_EXT

DESCRIPTION
  This routine marks a function to be deleted from the event listener table.
  'listener' will no longer be called when any of the events registered
  earlier by calling diag_add_event_listener_ext() is generated 
    
  'listener' is used to match a registered listener.
  'param' used with listener while adding the listener is not used to
   match listener.
  
  If duplicate entries exist, all are marked to be deleted.

RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.

===========================================================================*/
boolean diag_remove_event_listener_ext(diag_event_listener_ext listener)
{
  event_searchlist_ext_entry_type *node = NULL;
  event_searchlist_ext_entry_type *next_node = NULL;
  boolean listener_found = FALSE;
  int i = 0;
  if(!listener || !event_listeners_ext.initialized)
  {
    return FALSE;
  }
  osal_enter_crit_sect(&event_listeners_ext.lock);
  node = (event_searchlist_ext_entry_type *)list_peek_front((list_type *)&event_listeners_ext);
  while(node)
  {
    if( (node->entry.listener == listener) && (FALSE == node->remove_flag) )
    {
      /* Mark the listener to be deleted */
      node->remove_flag = TRUE;
      /* Increment the count of listeners to be deleted */
      event_listeners_ext.free_count++;
      /* Decrement the listeners count */
      event_listeners_ext.count--;
      if(event_listeners_ext.count == 0)
      {
        DIAG_CLR_MASK (diag_cx_state, DIAG_CX_EVENT_LISTEN_EXT_S);
      }

      /*Get the first node in the listener_ext list */
      next_node = (event_searchlist_ext_entry_type *)list_peek_front((list_type *)&event_listeners_ext);

      if(next_node)
      {
        memscpy(diag_event_listener_ext_mask, EVENT_MASK_SIZE, next_node->entry.mask, EVENT_MASK_SIZE);
      }
      else
      {
        memset(diag_event_listener_ext_mask, 0, EVENT_MASK_SIZE);
      }
      while(next_node)
      {
        for(i = 0; i < EVENT_MASK_SIZE; i++)
        {
          diag_event_listener_ext_mask[i] |= next_node->entry.mask[i];
        }
        next_node = (event_searchlist_ext_entry_type *)list_peek_next((list_type *)&event_listeners_ext, (list_link_type *)next_node);
      }
      listener_found = TRUE;
    }
    /*Check for all the matching listeners */
    node = (event_searchlist_ext_entry_type *)list_peek_next((list_type *)&event_listeners_ext, (list_link_type *)node);
  }
  osal_exit_crit_sect(&event_listeners_ext.lock);
  return listener_found;
}/* diag_remove_event_listener_ext */
/*===========================================================================
FUNCTION DIAG_CLEAN_EVENT_LISTENER_EXT
DESCRIPTION
  This routine deletes the listener  from the event listener table.
  'listener' will no longer be called when any of the events registered
  earlier by calling diag_add_event_listener_ext() is generated 
    
  
  If duplicate entries exist, all are removed.
RETURN VALUE
void
===========================================================================*/
void diag_clean_event_listener_ext(void *param)
{
  event_searchlist_ext_entry_type *node = NULL;
  event_searchlist_ext_entry_type *next_node = NULL;
  if(!event_listeners_ext.initialized)
  {
    return ;
  }
  osal_enter_crit_sect(&event_listeners_ext.lock);
  node = (event_searchlist_ext_entry_type *)list_peek_front((list_type *)&event_listeners_ext);
  while( node && (event_listeners_ext.free_count > 0) )
  {
    if( TRUE == node->remove_flag)
    {
      next_node = (event_searchlist_ext_entry_type *)list_peek_next((list_type *)&event_listeners_ext, (list_link_type *)node);
      /*Remove the listener from the list */
      list_pop_item((list_type *)&event_listeners_ext, (list_link_type *)node);
      /* Free the listener */
      diag_listeners_free(node);
    /* Decrement the count of listeners to be deleted*/
      event_listeners_ext.free_count--;
      node = next_node;
    }
    else
    {
      /*Check for all the matching listeners */
      node = (event_searchlist_ext_entry_type *)list_peek_next((list_type *)&event_listeners_ext, (list_link_type *)node);
    }
  }
  osal_exit_crit_sect(&event_listeners_ext.lock);
  return;
}/* diag_remove_event_listener_ext */
/*===========================================================================
FUNCTION DIAG_REMOVE_EVENT_LISTENER_EXTP
DESCRIPTION
  This routine marks a function to be deleted from the event listener table.
  'listener' with param 'param' will no longer be called when any of the events registered
  earlier by calling diag_add_event_listener_ext() is generated 
    
  'listener' and 'param' are used to match a registered listener.
  
  If duplicate entries exist, all are marked to be deleted.
RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.
===========================================================================*/
boolean diag_remove_event_listener_extp(diag_event_listener_ext listener, void *param)
{
  event_searchlist_ext_entry_type *node = NULL;
  boolean listener_found = FALSE;

  if(!listener || !event_listeners_ext.initialized)
  {
    return FALSE;
  }

  osal_enter_crit_sect(&event_listeners_ext.lock);
  node = (event_searchlist_ext_entry_type *)list_peek_front((list_type *)&event_listeners_ext);

  while(node)
  {
    if( (node->entry.listener == listener) && (node->entry.param == param) && (FALSE == node->remove_flag) )
    {
      /* Mark the listener to be deleted*/
      node->remove_flag = TRUE;
      /*Increment the count of listeners to be deleted*/
      event_listeners_ext.free_count++;

      /* Decrement the listeners count */
      event_listeners_ext.count--;

      if(event_listeners_ext.count == 0)
      {
        DIAG_CLR_MASK (diag_cx_state, DIAG_CX_EVENT_LISTEN_EXT_S);
      }





    
  








      listener_found = TRUE;
    }

      /*Check for all the matching listeners */
      node = (event_searchlist_ext_entry_type *)list_peek_next((list_type *)&event_listeners_ext, (list_link_type *)node);
  }
  osal_exit_crit_sect(&event_listeners_ext.lock);

  return listener_found;
}
/* ==========================================================================
FUNCTION event_listeners_ext_init

DESCRIPTION 
Initializes the event_listeners_ext list and also the critical section for 
event_listeners_ext.

RETURN VALUE
  Boolean indicating the success of the operation.  
  
============================================================================= */
static boolean event_listeners_ext_init()
{
    if(!osal_atomic_compare_and_set((osal_atomic_word_t *)&event_ext_init_start, FALSE, TRUE))
        return FALSE;
        
    list_init(&event_listeners_ext.list);
    osal_init_crit_sect(&event_listeners_ext.lock);
    event_listeners_ext.initialized = 1;
    
    return TRUE;
 }

