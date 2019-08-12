#ifndef EVENTI_H
#define EVENTI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Event Services internal header file

General Description
  Internal declarations to support diag event service.

Initializing and Sequencing Requirements 
  None

Copyright (c) 2007-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                          Edit History 
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/01/14   rs      Added support for build mask centralization. 
10/14/13   sr      Added support for extended event, log and F3 pkts  
10/01/13   sa      listeners clean up function
11/09/12   is      Support for preset masks
09/28/12   sr      Filtering of logs/events based on ssm permission file  
08/31/12   sr      Support for mask retrieval command  
07/25/12   rh      Externed event_config and event_config_sec delcarations from event.c
05/10/12   sa      Mainlined DIAG_CENTRAL_ROUTING
09/15/11   is      Modifed "set mask" via CTRL msgs fot EA support
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
02/01/10   sg      Dual Mask Changes
09/28/10   mad     Added event_ctrl_stale_timer and event_ctrl_report_size
08/10/10   sg      Added function event_set_all_rt_masks
08/25/10   sg      Fixed compiler warnings
02/20/10   sg      Moved event_q_alloc and event_q_put here
01/10/08   mad     Added copyright and file description.
12/5/07    as      Created

===========================================================================*/

#include "comdef.h"
#include "event_defs.h"
#include "queue.h"
#include "diagdiag_v.h"    /* for event_id_type */
#include "diagcomm_io.h"   /* For diagcomm_port_type */

/* Size of the event mask array, which is derived from the maximum number
 of events.
*/
#define EVENT_MASK_SIZE (EVENT_LAST_ID/8 + 1)


#define EVENT_SEND_MAX 50
#define EVENT_RPT_PKT_LEN_SIZE 0x200

/* Dual Mask Defines */
#define DIAG_EXT_EVENT_REPORT_GET_MASK 1
#define DIAG_EXT_EVENT_REPORT_SET_MASK 2
#define DIAG_EXT_EVENT_REPORT_SET_ALL 3


/* NOTE: diag_event_type and event_store_type purposely use the same
   format, except that event_store type is preceeded by a Q link
   and the event ID field has a union for internal packet formatting.
   If either types are changed, the service will not function properly. */
typedef struct
{
  q_link_type qlink;            /* To be accessed by event_q_xxx() only */
  uint8 reserved;               /* Required? Do we need to align this struct? */
  uint8 stream_id;              /* Flag to indicate which stream this is needed for (including listeners) */
  uint16 version;               /* version field for extended headers */
  uint32 id;                    /* ID field associated with VERSION_1 */

  union
  {
    unsigned int id;
    event_id_type event_id_field;
    uint16 sized_field;         /* Used to copy the event ID structure */
  }
  event_id;

  qword ts;                     /* always store full time stamp */

  event_payload_type payload;
}
event_store_type;

void event_q_put(event_store_type * event,byte stream_id);

event_store_type * event_q_alloc (event_id_enum_type id, uint8 payload_length,byte stream_id);

void event_control (uint8 config, uint8 config_mask);
extern uint8 event_config;

void event_control_sec (uint8 config, uint8 config_mask);
extern uint8 event_config_sec;

void event_set_all_rt_masks( uint8 config, byte stream_id);

PACK(void *) event_pkt_get_local_masks (PACK(void *) req_pkt, uint16 pkt_len);

void event_ctrl_stale_timer(unsigned int timer_len);
void event_ctrl_report_size(unsigned int report_size, unsigned int send_max_num, unsigned int rpt_pkt_len_siz);
void diag_clean_event_listener_ext(void *param);
boolean event_mask_enabled ( const unsigned char *mask_ptr, event_id_enum_type event_id);

void diag_switch_event_preset_mask( void );

/* On MP Master only */
#if defined(DIAG_MP_MASTER)
void diagcomm_ctrl_send_event_mask( diagcomm_port_type port_type, 
                                    diagcomm_enum_port_type port_num, 
                                    uint8 stream_id,
                                    uint8 preset_id,
                                    boolean send_now );
#endif /* DIAG_MP_MASTER */

#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
/* Sends last event id supported to Master Diag */
void diag_ctrl_send_event_id_ctrl_pkt(void);
#endif

#ifdef FEATURE_SSM
void event_mask_ssm(diag_mask_ssm_buffer_type* diag_event_mask_range, uint32 size);
#endif

#endif /* EVENTI_H */
