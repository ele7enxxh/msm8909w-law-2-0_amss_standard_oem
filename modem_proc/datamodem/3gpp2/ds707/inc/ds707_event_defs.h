
#ifndef _DS707_EVENT_DEFS_H
#define _DS707_EVENT_DEFS_H

/*===========================================================================

         D S 707 E V E N T S    D E F I N I T I O N S
                   
DESCRIPTION

 The Data Services  707 events  Header File.  Contains the definition
 for 707 events 

 Copyright (c) 2004-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_event_defs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/04/09   sa      AU level CMI modifications.
03/13/09   ss      Added GoNULL timer events
07/28/08  mga      Added events ds707_pkt_ln_update_event_type and 
                   ds707_pkt_idm_change_event_type
06/17/08   sn      Added reporting of events for arbitration manager.
05/14/08   ms     Updated the BCMCS events
10/10/06  az       Added events for RESV messages. DQOS.
10/17/05  as/kvd   Removed BCMCS events enum type and updated 
                   BCMCS event structs
10/03/05   kvd     Added BCMCS events
05/26/05   vrk     Packed the payload of DS diag events
04/29/05   vrk     Modified payloads and added a new diag event 
04/26/05   vrk     Created events to enable automated testing
11/09/04   rsl     Added new async. and pkt. event enums and payload type.
06/09/04   kvd     Initial definition. 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "event.h"
#include "ds707.h"
#include "dsat_v.h"
#include "sys.h"

/*===========================================================================

                             EXPORTED DEFINITIONS

===========================================================================*/

/*===========================================================================
  NOTE: changes to these enums MUST be coordinated with the diag/tools team.
        as of the creation date mail had to be sent to asw.diag.request
===========================================================================*/

#define UNDEFINED 0


#define MAX_FLOWS_TO_REPORT_IN_EVENTS 10

/*---------------------------------------------------------------------------
  TYPEDEF DS707_EVENT_ENUM_TYPE

  Type enumerating the possible  ds 707 (pkt manager) events to be generated. 
  Translated by the tool into strings. 
---------------------------------------------------------------------------*/

typedef enum
{
  EVENT_DS707_PPP_RESYNC = 0, 
  EVENT_DS707_ASYNC_ORIG =            1,
  EVENT_DS707_ASYNC_CALL_CONNECTED =  2,
  EVENT_DS707_ASYNC_INCOMING_CALL =   3,
  EVENT_DS707_ASYNC_CALL_ENDED =      4,
  EVENT_DS707_PKT_ORIG =              5,
  EVENT_DS707_PKT_CALL_CONNECTED =    6,
  EVENT_DS707_PKT_INCOMING_CALL =     7,
  EVENT_DS707_PKT_CALL_ENDED =        8,
  EVENT_DS707_DATA_CALL_REQ_ACCEPTED= 9,   /* OMH-Data call request accepted */
  EVENT_DS707_DATA_CALL_REQ_REJECTED=10,   /* OMH-Data call request rejected */
  EVENT_DS707_INVALID  = 255
} ds707_event_enum_type;

/*---------------------------------------------------------------------------
  TYPEDEF DS707_EVENT_INFO_ENUM

  Possible values for the info field for ds707 ppp_resync event. 
  Translated by the tool into strings. 
---------------------------------------------------------------------------*/

typedef enum
{
  DS707_RESYNC_ON_HDR_1X_HANDOFF = 0,
  DS707_RESYNC_ON_ALN = 1,
  DS707_RESYNC_ON_QCODH = 2,
  DS707_RESYNC_INVALID_REASON = 255
} ds707_event_resync_info_enum_type;

/*---------------------------------------------------------------------------
  TYPEDEF DS707_EVENT_INFO_TYPE

  Union of info contents in 707 events depending on 'ds707_event' type
---------------------------------------------------------------------------*/

typedef union
{
  uint32                      zero;      /* use this to zero the structure */
  ds707_event_resync_info_enum_type  i_resync; /* info on ppp_resync event */
} ds707_event_info_type;

/*---------------------------------------------------------------------------
  TYPEDEF DS707_EVENT_PAYLOAD_TYPE

  DS707 event payload type. 
---------------------------------------------------------------------------*/

typedef struct
{
  ds707_event_enum_type  ds707_event;     /* event                       */
  ds707_event_info_type  info;            /* info field (event specific) */
} ds707_event_payload_type;

typedef struct
{
  ds707_event_enum_type  ds707_event;     /* event                       */
  ds707_so_type          so_type;
  cm_call_id_type        call_id;
}ds707_call_event_payload_type;

typedef enum
{
  DS_CAM_EVENT_TIMER_START=0,
  DS_CAM_EVENT_TIMER_CANCEL=1,
  DS_CAM_EVENT_TIMER_EXPIRY=2
}ds_cam_timer_enum_type;

typedef enum
{
  DS_RDUD_EVENT_TIMER_START=0,
  DS_RDUD_EVENT_TIMER_CANCEL=1,
  DS_RDUD_EVENT_TIMER_EXPIRY=2
}ds_rdud_timer_enum_type;

typedef enum
{
  DS707_GONULL_EVENT_TIMER_START=0,
  DS707_GONULL_EVENT_TIMER_STOP=1,
  DS707_GONULL_EVENT_TIMER_EXPIRY=2
}ds707_gonull_timer_enum_type;

/*---------------------------------------------------------------------------
Enum: ds_fallback_enum_type
Event: EVENT_DS_FALLBACK
Description: This event is currently generated when the 
             DS (Data Services) falls back from 
			 MIP (Mobile IP) to SIP (Simple IP)
Field Name(s): DS_MIP_TO_SIP_FALLBACK
Event payload size in bytes: 1
---------------------------------------------------------------------------*/
typedef enum
{
  DS_MIP_TO_SIP_FALLBACK
}ds_fallback_enum_type;

typedef enum
{
  DS3G_CAM_FLOW_CTRL_EVENT_TIMER_START=0,
  DS3G_CAM_FLOW_CTRL_EVENT_TIMER_CANCEL=1,
  DS3G_CAM_FLOW_CTRL_EVENT_TIMER_EXPIRY=2
}ds3g_cam_flow_ctrl_timer_enum_type;

/*---------------------------------------------------------------------------
Struct: ds_cta_timer_struct
Event: EVENT_DS_CTA_TIMER
Description: This event is generated when the CTA timer 
            (Inactivity timer) is changed. The old and the 
    		 new value of the CTA timer are provided in 
	    	 the payload.
Field Type: dsat_num_item_type, dsat_num_item_type
Event payload size in bytes: 8 
---------------------------------------------------------------------------*/

typedef struct
{
  dsat_num_item_type ds_old_cta_val;
  dsat_num_item_type ds_new_cta_val;
}ds_cta_timer_struct;

/*---------------------------------------------------------------------------
Struct: ds3g_cam_flow_ctrl_timer_struct
Event Name: EVENT_DS3G_CAM_FLOW_CTRL_TIMER
Event Description: This event is triggered when the CAM (Chatty 
                  Application Manager) Flow Control timer is 
			   started, cancelled, or expired.
Event Fields Type: ds3g_cam_flow_ctrl_timer_enum_type(1 byte) 
                  and dword(4 bytes)
Event Payload Size: 5 bytes
---------------------------------------------------------------------------*/

typedef PACKED struct PACKED_POST
{
  ds3g_cam_flow_ctrl_timer_enum_type timer_state;
  dword timer_value;
}ds3g_cam_flow_ctrl_timer_struct;

/*---------------------------------------------------------------------------
Struct: ds_cam_timer_struct
Event Name: EVENT_DS_CAM_TIMER
Event Description: This event is triggered when the CAM (Chatty 
                   Application Manager) timer is started, 
				   cancelled, or expired. The payload will carry 
				   the timer state and the duration of the timer.
Event Fields Type: ds_cam_timer_enum_type (1 byte) and 
                   dword (4 bytes)
Event Payload Size: 5 bytes
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  ds_cam_timer_enum_type timer_state;
  dword timer_value;
}ds_cam_timer_struct;

/*---------------------------------------------------------------------------
Struct: ds_rdud_timer_struct
Event Name: EVENT_DS_RDUD_TIMER
Event Description: This event is generated when the RDUD (Reduced 
                   Dormancy followed by Unsolicited Data) timer is 
				   started, cancelled, or when it is expired. 
				   The payload will carry the timer state and 
				   the duration of the timer.
Event Fields Type:  ds_rdud_timer_enum_type (1 byte) 
                    and dword (4 bytes)
Event Payload Size: 5 bytes
---------------------------------------------------------------------------*/

typedef PACKED struct PACKED_POST
{
  ds_rdud_timer_enum_type timer_state;
  dword timer_value;
}ds_rdud_timer_struct;

/*---------------------------------------------------------------------------
Struct: ds707_gonull_timer_struct
Event Name: EVENT_GONULL_TIMER
Event Description: This event is triggered when the GoNull timer is 
                   started, stopped or expired. The payload will carry 
                   the timer state and the duration of the timer.
Event Fields Type: ds707_gonull_timer_enum_type (1 byte) and 
                   dword (4 bytes)
Event Payload Size: 5 bytes
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  ds707_gonull_timer_enum_type timer_state;
  dword                        timer_value;
} ds707_gonull_timer_struct;

/*---------------------------------------------------------------------------
  wmk allocate
---------------------------------------------------------------------------*/
typedef struct
{
  void    *wmk_ptr;
  uint32   lo_watermark;		/* Low watermark for total bytes */
  uint32   hi_watermark;		/* Hi watermark for total bytes */
  void    *ps_flow_ptr;
}ds_wmk_allocated_event_type;

/*---------------------------------------------------------------------------
  wmk deallocate
---------------------------------------------------------------------------*/
typedef struct
{
  void    *wmk_ptr;
  void    *ps_flow_ptr;
}ds_wmk_deallocated_event_type;

/*---------------------------------------------------------------------------
  wmk flushed
---------------------------------------------------------------------------*/
typedef struct
{
  void    *wmk_ptr;
  uint32   current_cnt; 
}ds_wmk_flushed_event_type;

/*---------------------------------------------------------------------------
  wmk flow enabled
---------------------------------------------------------------------------*/
typedef struct
{
  void    *ps_flow_ptr;
  void    *wmk_ptr;
  uint32   current_cnt; 
}ds_wmk_flow_enabled_event_type;

/*-------------------------------------------------------------------------
  wmk flow disabled
---------------------------------------------------------------------------*/
typedef struct
{
  void    *ps_flow_ptr;
  void    *wmk_ptr;
  uint32   current_cnt; 
}ds_wmk_flow_disabled_event_type;

/*-------------------------------------------------------------------------
  RESV msg sent event
---------------------------------------------------------------------------*/
typedef struct
{
  uint32   ts_id;
  uint8    opcode_dir_numflows;
  uint8    flow_ids[MAX_FLOWS_TO_REPORT_IN_EVENTS];
} ds_resv_msg_sent_event_type;

/*-------------------------------------------------------------------------
  RESV msg response received event
---------------------------------------------------------------------------*/
typedef struct
{
  uint32   ts_id;
} ds_resv_msg_response_recd_event_type;

/*-------------------------------------------------------------------------
  Location Notification Update event
---------------------------------------------------------------------------*/
typedef struct
{
  sys_hdr_location_notification_status_e_type  ln_status;
  sys_sys_mode_e_type curr_data_session_network;
  sys_sys_mode_e_type last_data_session_network;
}ds707_pkt_ln_update_event_type;
 
/*-------------------------------------------------------------------------
  IDM change event
---------------------------------------------------------------------------*/
typedef struct
{
  sys_sys_mode_e_type last_data_session_network;
  sys_sys_mode_e_type new_data_session_network;  
}ds707_pkt_idm_change_event_type;

#endif /* DS707_EVENT_DEFS_H */
