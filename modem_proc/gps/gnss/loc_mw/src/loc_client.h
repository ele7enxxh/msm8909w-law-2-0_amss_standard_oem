/******************************************************************************
@file:  loc_client.h
@brief: Location Middleware client

DESCRIPTION
This module defines the loc API routines for a client to initiate a connection

INITIALIZATION AND SEQUENCING REQUIREMENTS

-----------------------------------------------------------------------------
Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
All Rights Reserved. QUALCOMM Proprietary and Confidential. 
Copyright (c) 2013 - 2015 QUALCOMM Atheros Incorporated.
All Rights Reserved.QUALCOMM Atheros Proprietary and Confidential.
Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------------
******************************************************************************/

/*=====================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ------------------------------------------------------- 
06/11/15   sjk      LB 2.0 Integration 
04/27/11   spn      Updated for Loc-Api 2.0

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_client.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $
======================================================================*/

#ifndef LOC_CLIENT_H
#define LOC_CLIENT_H

#include "loc_api_2.h"
#include "loc_task.h"
#include "pdapi.h"
#include "location_service_v02.h"

#define MAX_PREFERRED_RESPONSE_TIME_MS       (255*1000) /* 255 seconds */
#define DEFAULT_SESSION_TIMEOUT_MS           (1*1000)   /* 1 second */
#define SESSION_END_THRESHOLD                (10*1000)  /* 10 seconds */

/* Client state */
typedef enum
{
   CLIENT_STATE_INACTIVE = 0,
   CLIENT_STATE_ACTIVE,    /* Client requests for tracking session */
   CLIENT_STATE_TRANSIENT, /* client requests for single-shot session */
   CLIENT_STATE_TRANSIENT_WAIT_FOR_POSITION, /* Client single shot session is being processed */
} loc_client_state_e_type;


/* control block to store batching related info for a client */
typedef struct
{
   /* to mark if this is a batching client */
   boolean        is_batching_client;

   /* numbering each fix stored */
   uint32         fix_id;

   /* TimeStamp of Last fix reported, this is to avoid batching the same fix
    * from GM in two different batches */
   uint64         last_fix_timestamp_utc;


   /* write_index, read_index to push/pop entries into the batching array */
   int32          write_index;
   int32          read_index;

   /* number of fixes batched during lifetime of batching request */
   uint32         num_fixes_batched_lifetime;

   /* counter to keep track of number of entries filled in buffer */
   int32          total_occupied;

   /* flag to indicate the buffer is full and overwriting */
   /* true == overwrite on state */
   /* false == overwrite off state  */
   boolean        overwrite_flag; 

   /* maximum size of the buffer units */
   int32          max_array_size;

   /* number of fixes in the batch for client */
   int32          client_batch_size;

   /* total number of fixes in the batch = client-size + reserved size */
   int32          total_batch_size;

   /* buffer pointer */
   qmiLocBatchedReportStructT_v02 *loc_batching_array;
        
   /* UTC timestamp of position when the current batch interval begins */
   uint64 last_batched_fix_utc ;

   /* UTC timestamp of the second last position batched */
   uint64 last_last_batched_fix_utc ;

    /* A flag to track if unable to track has been reported for this client, in order to avoid
      sending duplicate indications to the same client for multiple sessions and to avoid bathing the
	  invalid position multiple times */
   boolean unable_to_track_reported;
      
    /* A flag to track if buffer full has been reported. So if we overwrite the 
     * last buffer, we dont send multiple 'batch full' messages. */
   boolean buffer_full_reported;   
}loc_batching_client_info_s_type;


typedef struct
{
   q_link_type                  z_Link; /*< Data Structure required by the queue 
                                           library to add this as part of a queue */
   boolean                      in_use;
   int                          client_handle;

   /* parameter passed from client */
   loc_event_cb_f_type         *event_callback;
   loc_event_mask_type          event_reg_mask;
   loc_fix_criteria_s_type      fix_criteria_info;
  
   loc_client_state_e_type      client_state; /* state for position request */

   boolean                      is_first_fix_for_tracking;    
   uint64                       last_fix_request_time;
   
   pdsm_pd_server_info_s_type   server_info;  // PDE server for this client

   /* denotes that the client has sent a start for a single shot but 
      the corresponding stop has not been received */
   boolean                      ss_started_not_stopped; 

   /* control block to store batching related info for a client */
   loc_batching_client_info_s_type  batching_info;
   
   /* Saved to calculate the elapsed time from last reported intermediate fix */
   uint64                       last_intermediate_fix_report_time;

} loc_client_info_s_type;

typedef struct
{
   // The location handle that will be returned to client, will be incremented per each call to loc_open */
   int                      global_client_handle; 
   int                      max_num_clients;
   int                      active_client_handle;
   int                      stop_client_handle;
   q_type                   z_loc_client_queue;   /**< This is queue of type loc_client_info_s_type */
} loc_client_module_data_s_type;

extern loc_client_handle_type loc_client_init 
(
      loc_event_mask_type       event_reg_mask,
      loc_event_cb_f_type       *event_callback
);

extern int loc_client_release 
(
      loc_client_handle_type  handle
);

extern int loc_client_process_ioctl 
(
      loc_client_handle_type  client_handle,
      loc_ioctl_e_type ioctl_type,
      const loc_ioctl_data_u_type* ioctl_data
);

extern loc_client_info_s_type* loc_client_find_handle 
(
      loc_client_handle_type handle
);

// Retunr LOC_API_XXX
extern int loc_client_start_fix 
(
      loc_client_handle_type  client_handle
);

extern int loc_client_stop_fix  
(
      loc_client_handle_type  client_handle
);

extern int loc_client_process_cmd_request 
(  
      loc_cmd_s_type * const ptr_cmd
);

extern boolean loc_client_invoke_callback 
(
      loc_client_handle_type    client_handle,
      loc_event_mask_type       event_type,
      loc_event_payload_u_type* event_payload_ptr
);

extern void loc_client_queue_ioctl_callback 
(
      loc_client_handle_type          client_handle,
      loc_ioctl_e_type                ioctl_type,
      int32                           ioctl_status,
      loc_ioctl_callback_data_u_type* ioctl_cb_data_ptr
);

extern void loc_client_queue_qmi_ioctl_callback
(
  loc_client_handle_type              client_handle,
  uint32                              ioctl_type,
  int32                               ioctl_status,
  void                                *ioctl_data
);


// Interaction between client and pd module
// Set up gps configuration for requesting fixes
extern int     loc_client_setup_request_config (void);

// Returns TRUE if there is any client that is requesting fixes
extern boolean loc_client_is_any_requesting_fix  (void);

// Get active client handle
extern loc_client_handle_type loc_client_get_active_client (void);

// Update the state of the client after fix is done
extern boolean loc_client_update_state_after_fix (void);

extern uint32 loc_client_get_fix_interval 
(
      loc_fix_criteria_s_type* fix_criteria_ptr
);

// Report position to concurrent cell id request
extern void loc_client_report_position_to_cellid_request 
(
      loc_event_payload_u_type* pos_payload_ptr,
      uint64                    current_time_ms
);

extern boolean loc_client_is_server_request_handler_registered (void);


// Broadcast position injection request to all clients that have registered for it
extern boolean loc_client_queue_pos_inj_request (const loc_pos_inj_request_s_type * const pPosInjRequest);

// retrives the time for next fix and associated client index
extern int loc_client_get_min_wait_time(loc_client_info_s_type** min_time_client_ptr);

#endif // LOC_CLIENT_H
