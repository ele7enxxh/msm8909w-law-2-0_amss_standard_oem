/******************************************************************************
  @file:  loc_client.c
  @brief: Location Middleware client

  DESCRIPTION
  This module defines the loc API routines for a client that connects to 
  loc-mw

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2008-2012 by Qualcomm Technologies Incorporated.
  All Rights Reserved.QUALCOMM Proprietary and Confidential.
  Copyright (c) 2013-2015 QUALCOMM Atheros Incorporated.
  All Rights Reserved.QUALCOMM Atheros Proprietary and Confidential.
  Copyright (c) 2015-2016 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ------------------------------------------------------- 
04/05/16   yh       Fix "only one client is scheduled when multiple client are 1HZ" problem
01/28/16   yh       Broadcast intermediate fix
12/10/15   yh       Remove timer when fix interval is one second
09/03/15   yh       Add loc_client_set_fix_min_interval
06/11/15   sjk      LB 2.0 Integration 
04/27/11   spn      Updated for Loc-Api 2.0

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_client.c#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $
======================================================================*/


/*===================================================================
This module manages location api client.
====================================================================*/

#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "limits.h"
#include "msg.h"
#include "queue.h"
#include "time_svc.h"

#include "aries_os_api.h"
#include "pdapi.h"
#include "pdsm_atl.h"
#include "loc_api_2.h"

#include "loc_task.h"
#include "loc_client.h"
#include "loc_pd.h"
#include "loc_conn.h"
#include "loc_xtra.h"
#include "loc_pa.h"
#include "loc_ni.h"
#include "loc_wifi.h"
#include "loc_api_internal.h"
#include "loc_batching.h"

#define LOC_CLIENT_MAX_NUM                   (50)
#define DEFAULT_MODE                         eQMI_LOC_OPER_MODE_STANDALONE_V02
#define DEFAULT_1HZ_FIX_INTERVAL             (1000)   /* 1 second */
#define MAXIMUM_OVERHEAD_TIME                (400)
#define LOC_CLIENT_POS_OFFSET                (50)

// Function declaration
static int loc_client_set_fix_criteria
(
      loc_client_info_s_type        *client_ptr,
      const loc_fix_criteria_s_type *fix_criteria_ptr
);

// Function to set the custom server address to be used for this location client.
// Custom server will be used when doing MSA/MSB fixes
static int loc_client_set_custom_pde_server_addr
(
      loc_client_info_s_type        *client_ptr,
      const loc_server_info_s_type  *server_addr_ptr
);

// Function to retrieve information that is client specific
static int loc_client_process_info_request
(
      const loc_client_info_s_type        *client_ptr,
      loc_ioctl_e_type ioctl_type
);


// Set event registration mask for the calling client
static int loc_client_set_event_reg_mask_locked
(
      loc_client_info_s_type* const   client_ptr,
      const loc_event_mask_type       event_mask
);



static int loc_client_set_operation_mode
(
      loc_client_info_s_type* const   client_ptr,
      const  qmiLocSetOperationModeReqMsgT_v02 *oper_mode
);


static int loc_client_compare_handle
(
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
   loc_client_info_s_type  *client_ptr = (loc_client_info_s_type *)p_ItemPtr;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   /* item_ptr contains an element of the Client Q i.e. Client Handle, 
     and compare val is also a Client Handle */
  return ( (client_ptr->client_handle == *((int *)p_CompareVal) ) ? 1 : 0 );
}

// This function is used to check whether a client with the specific handle exists or not.
// If so, it returns a pointer to the client. If it does not exist, NULL will be returned.
loc_client_info_s_type* loc_client_find_handle 
(
   loc_client_handle_type client_handle
)
{
   loc_client_info_s_type  *client_ptr = NULL;

   client_ptr = (loc_client_info_s_type *)q_linear_search(
                                   &loc_middleware_data.client_module_data.z_loc_client_queue,
                                   loc_client_compare_handle,
                                   &client_handle );

   return client_ptr;
}

/*-------------------------------------------------------------------------------
**
** Starting function definition
**
**------------------------------------------------------------------------------*/
// This function is used to process loc_open. It will return a client handle (>=0) when successful.
loc_client_handle_type loc_client_init
(
   loc_event_mask_type       event_reg_mask,
   loc_event_cb_f_type       *event_callback
)
{
   loc_client_module_data_s_type* client_module_data_ptr = &(loc_middleware_data.client_module_data);
   loc_client_info_s_type*        client_ptr = NULL;
   loc_client_handle_type         client_handle = LOC_CLIENT_HANDLE_INVALID;

   if( LOC_CLIENT_MAX_NUM == client_module_data_ptr->max_num_clients )
   {
      LOC_MSG_ERROR("loc_client_init max_num_clients = %d",
                    client_module_data_ptr->max_num_clients, 0, 0);
      return client_handle;
   }

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   client_ptr = (loc_client_info_s_type*)os_MemAlloc( sizeof(loc_client_info_s_type), OS_MEM_SCOPE_TASK);

   if ( client_ptr != NULL ) 
   {
      /* Memset to "0" for first use, it might have garbage and cause trouble */
      memset(client_ptr, 0, sizeof(*client_ptr) );
      /* Initialize the link, return value is a pointer to the link, 
         not needed */
      (void) q_link ( client_ptr, &client_ptr->z_Link );
      /* Add it to the queue of clients */
      q_put( &client_module_data_ptr->z_loc_client_queue,
             &client_ptr->z_Link );

      client_module_data_ptr->max_num_clients++;

      client_ptr->in_use = TRUE;
      client_ptr->client_handle = client_module_data_ptr->global_client_handle++;

      // Client handle wrapped around
      if (client_ptr->client_handle < 0)
      {
         client_module_data_ptr->global_client_handle = 0;
         client_ptr->client_handle = client_module_data_ptr->global_client_handle++;
      }

      client_ptr->client_state = CLIENT_STATE_INACTIVE;
      client_ptr->event_reg_mask = event_reg_mask;
      client_ptr->event_callback = event_callback;
      client_ptr->is_first_fix_for_tracking = TRUE;
      client_ptr->ss_started_not_stopped = FALSE;

     //set the default valid bits in the fix criteria
      client_ptr->fix_criteria_info.valid_mask = 
         (LOC_FIX_CRIT_VALID_INTERMEDIATE_POS_REPORT_ENABLED |
          LOC_FIX_CRIT_VALID_MIN_DIST_SAMPLE_INTERVAL |
          LOC_FIX_CRIT_VALID_MIN_DISTANCE |
          LOC_FIX_CRIT_VALID_MIN_INTERVAL |
          LOC_FIX_CRIT_VALID_NOTIFY_TYPE |
          LOC_FIX_CRIT_VALID_PREFERRED_ACCURACY |
          LOC_FIX_CRIT_VALID_PREFERRED_OPERATION_MODE |
          LOC_FIX_CRIT_VALID_PREFERRED_RESPONSE_TIME |
          LOC_FIX_CRIT_VALID_RECURRENCE_TYPE);

      // set the default values for each field in the fix criteria
      client_ptr->fix_criteria_info.preferred_operation_mode = DEFAULT_MODE;
      client_ptr->fix_criteria_info.recurrence_type = LOC_PERIODIC_FIX;
      client_ptr->fix_criteria_info.preferred_accuracy = LOC_DEFAULT_ACCURACY;
      client_ptr->fix_criteria_info.preferred_response_time = LOC_DEFAULT_RESPONSE_TIME;
      client_ptr->fix_criteria_info.notify_type = LOC_NOTIFY_ON_INTERVAL;
      client_ptr->fix_criteria_info.min_interval = DEFAULT_1HZ_FIX_INTERVAL;
      client_ptr->fix_criteria_info.min_distance = 0;
      client_ptr->fix_criteria_info.min_dist_sample_interval = DEFAULT_1HZ_FIX_INTERVAL;
      client_ptr->fix_criteria_info.intermediate_pos_report_enabled = TRUE;

      memset((void *)&client_ptr->fix_criteria_info.application_id,
             0, sizeof(client_ptr->fix_criteria_info.application_id));

      // Initialize to use default server. Client will use custom server
      // if loc_ioctl & LOC_IOCTL_SET_CUSTOMER_PDE_SERVER is being called
      memset (&(client_ptr->server_info), 0, sizeof (pdsm_pd_server_info_s_type));
      client_ptr->server_info.server_option = PDSM_SERVER_OPTION_USE_DEFAULT;

      client_handle = client_ptr->client_handle;
      LOC_MSG_MED ("loc_client_init returned client_handle = %d, client_ptr = 0x%x, max_num_clients = %d",
                    client_handle, client_ptr, client_module_data_ptr->max_num_clients);
   }
   else
   {
      LOC_MSG_ERROR ("loc_client_init failed to alloate loc_client_info_s_type", 0, 0, 0);
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   return client_handle;
}

// When loc_close is called, loc_client_release will be called to free the resource
// being used by this client
int loc_client_release (loc_client_handle_type  client_handle)
{
   loc_client_info_s_type*        client_ptr;
   int                            ret_val = LOC_API_INVALID_HANDLE;

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
   client_ptr = loc_client_find_handle(client_handle);

   LOC_MSG_MED ("loc_client_release client_ptr = 0x%x",
                 client_ptr,0,0);
   if (client_ptr == NULL)
   {
      LOC_MSG_ERROR ("loc_client_release, invalid client handle", client_handle, 0, 0);
   }
   else
   {
      /* Remove from Queue */
     if ( TRUE != q_delete_ext( &loc_middleware_data.client_module_data.z_loc_client_queue,
                                &client_ptr->z_Link ) )
     {
         LOC_MSG_ERROR("Could not delete the loc client from queue ",0, 0, 0);
     }
     else
     {
         /* Free the memory if delete queue is successful */
         os_MemFree( (void**) &client_ptr );
         ret_val = LOC_API_SUCCESS;
         loc_middleware_data.client_module_data.max_num_clients--;
     }
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   LOC_MSG_MED ("loc_client_release for client_handle = %d, ret_val = %d, max_num_clients = %d",
                 client_handle, ret_val, loc_middleware_data.client_module_data.max_num_clients);
   return ret_val;
}

// This function goes through the queue and determines the min fix interval
// Becuase read/write global variable loc_middleware_data,
// this function must be protected by LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec)
void loc_client_set_fix_min_interval(void)
{
   loc_client_info_s_type* p_client= NULL;
   uint32                  q_min_fix_interval = LOC_MIN_RESPONSE_TIME/1000;   /* 1 second */
   boolean                 v_first_client = TRUE;

   for (p_client = (loc_client_info_s_type *)q_check(&loc_middleware_data.client_module_data.z_loc_client_queue);
        p_client != NULL;
        p_client = (loc_client_info_s_type *)q_next(&loc_middleware_data.client_module_data.z_loc_client_queue, &p_client->z_Link) )
   {
      if (TRUE == p_client->in_use && 
          CLIENT_STATE_ACTIVE == p_client->client_state )
      {
         /* 1. Set q_min_fix_interval to the first client's min_interval
            2. Compare q_min_fix_interval with the next client's min_interval, set to the smaller value */
         if( ( TRUE == v_first_client ) ||
             ( p_client->fix_criteria_info.min_interval < q_min_fix_interval) )
         {
            q_min_fix_interval = p_client->fix_criteria_info.min_interval;
            v_first_client = FALSE;
         }
      }
   }

   if (TRUE == v_first_client)
   {
      LOC_MSG_HIGH("loc_client_set_fix_min_interval, no active client ",0, 0, 0);
      return;
   }

   loc_middleware_data.pd_module_data.loc_fix_criteria_rule.min_interval = q_min_fix_interval;
   LOC_MSG_MED("loc_client_set_fix_min_interval, min_interval = %d ", q_min_fix_interval, 0, 0);

   return;
}

// This function is called to proces loc_start_fix.
// Returns LOC_CLIENT_HANDLE_INVALID if the passed-in client handle is not valid.
// Otherwise, it will return LOC_API_SUCCESS.
int loc_client_start_fix (loc_client_handle_type  client_handle)
{
   loc_client_info_s_type* client_ptr = NULL;
   time_type               time_ms;
   loc_cmd_s_type          cmd_data;
   int                     ret_val = LOC_API_SUCCESS;
   boolean                 client_state_changed = FALSE;

   memset ( &time_ms, 0, sizeof(time_ms) );
   memset ( &cmd_data, 0, sizeof(cmd_data) );

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   LOC_MSG_MED ("loc_client_start_fix for client_handle = %d", client_handle, 0, 0);

   client_ptr = loc_client_find_handle (client_handle);
   if (client_ptr == NULL)
   {
      LOC_MSG_ERROR ("loc_client_start_fix, invalid client handle", client_handle, 0, 0);
      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
      return LOC_API_INVALID_HANDLE;
   }

   if (client_ptr->fix_criteria_info.recurrence_type == LOC_PERIODIC_FIX)
   {
     // First fix has not been requested yet when client state is either
     // INACTIVE or TRANSIENT, in this case, set up the necessary information
     if (client_ptr->client_state != CLIENT_STATE_TRANSIENT_WAIT_FOR_POSITION) 
     {
       client_ptr->is_first_fix_for_tracking = TRUE;
       time_get_uptime_ms (time_ms);
       client_ptr->last_fix_request_time = QW_CVT_Q2N(time_ms);
       client_ptr->last_intermediate_fix_report_time = QW_CVT_Q2N(time_ms);
       LOC_MSG_MED("loc_client_start_fix, last_intermediate_fix_report_time %u", 
                    client_ptr->last_intermediate_fix_report_time, 0, 0);
     }

     //clear the single shot started but not stopped flag
     client_ptr->ss_started_not_stopped = FALSE;
     LOC_MSG_MED("loc_client_start_fix: periodic fix req. clearing ss_started_not_stopped flag", 
                   0, 0, 0);
     client_ptr->client_state = CLIENT_STATE_ACTIVE;
     client_state_changed = TRUE;

     // Set min_interval for all clients
     loc_client_set_fix_min_interval();

   }
   // Single shot
   else
   {
      if ((client_ptr->client_state == CLIENT_STATE_INACTIVE) ||
           (client_ptr->client_state == CLIENT_STATE_TRANSIENT_WAIT_FOR_POSITION))
      {
          client_ptr->client_state = CLIENT_STATE_TRANSIENT;
          time_get_uptime_ms(time_ms);
          client_ptr->last_fix_request_time = QW_CVT_Q2N(time_ms);

          //set the single shot started but not stopped flag
          client_ptr->ss_started_not_stopped = TRUE;
          LOC_MSG_MED("loc_client_start_fix: single shot req. setting ss_started_not_stopped flag", 
                       0, 0, 0);
          client_state_changed = TRUE;

      }
      // Position being requested when client is active, ignore the request
      else
      {
         LOC_MSG_MED ("loc_client_start_fix: client is already requesting fix", 0, 0, 0);
      }
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   if (client_state_changed == TRUE) 
   {
      // run the fix scheduler
      loc_pd_process_fix_request(eLOC_PD_START_REQUEST);
   }

   LOC_MSG_MED ("loc_client_start_fix for client = %d, return value = %d", client_handle, ret_val, 0);

   return ret_val;
}


// This function is called to proces loc_stop_fix.
// Return TRUE if the client state changed, eg: from requesting position to idle
int loc_client_stop_fix
(
      loc_client_handle_type  client_handle
)
{
   int                     ret_val = LOC_API_SUCCESS;
   loc_client_info_s_type* client_ptr = NULL;
   loc_cmd_s_type          cmd_data;

   memset(&cmd_data, 0, sizeof(cmd_data) );

   client_ptr = loc_client_find_handle (client_handle);

   do
   {
      if (client_ptr == NULL)
      {
         LOC_MSG_ERROR ("loc_client_stop_fix, invalid client handle", client_handle, 0, 0);
         ret_val = LOC_API_INVALID_HANDLE;
         break;
      }

      LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

      /* for tracking clients drop the stop if state is inactive
         but for single shot clients allow the stop to go through
         if a stop has not been received. Single shot clients become
         INACTIVE after the position report is generated, but the engine
         will be kept on unless explicitly stopped. */

      if (client_ptr->client_state == CLIENT_STATE_INACTIVE &&
          FALSE == client_ptr->ss_started_not_stopped )
      {
         LOC_MSG_MED("loc_client_stop_fix client %d is already inactive,"
                     " not doing anything for the stop request", 
                     client_handle,0, 0);

         LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

         break;
      }
      
      LOC_MSG_MED("loc_client_stop_fix client = %d, state is %d ss flag is %d  "
                  "scheduling a stop", client_ptr->client_handle, 
                  client_ptr->client_state, client_ptr->ss_started_not_stopped);

      /* When client state becomes inactive, no more position requests will
          be processed on behalf of this client */

      client_ptr->client_state = CLIENT_STATE_INACTIVE;

      //clear the single shot started but not stopped flag
      client_ptr->ss_started_not_stopped = FALSE;

      LOC_MSG_HIGH("loc_client_stop_fix: clearing ss_started_not_stopped flag", 0, 0, 0);
      client_ptr->server_info.server_option = PDSM_SERVER_OPTION_USE_DEFAULT;

      loc_middleware_data.client_module_data.stop_client_handle = client_ptr->client_handle;
      
      // Recalculate min_interval for all clients
      loc_client_set_fix_min_interval();

      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
         
      //run the fix scheduler
      loc_pd_process_fix_request(eLOC_PD_STOP_REQUEST);
      
   }while (0); 

   LOC_MSG_MED ("loc_client_stop_fix for client = %d, return value = %d", client_handle, ret_val, 0);
   return ret_val;
}

// This function goes through the queue and determines which client is ready to schedule fix next. The client
// with the minimal value of (last request time + fix_interval) will be returned.
int loc_client_get_min_wait_time(loc_client_info_s_type** min_time_client_ptr)
{
   loc_client_info_s_type* client_ptr = NULL;
   time_type               time_ms = {0};
   uint64                  current_time_ms;
   uint64                  time_delta_ms;
   dword                   last_fix_ttf;
   double                  temp_wait_time;

   // initialize variable to over range so error can be easily detected
   double                  wait_time = (double)INT_MAX + 1.0;
   uint32                  fix_interval;
   boolean                 first_client_ptr = TRUE;

   time_get_uptime_ms(time_ms);
   current_time_ms = QW_CVT_Q2N(time_ms);

   LOC_MSG_MED ("loc_client_get_min_wait_time %d", current_time_ms, 0, 0);
   *min_time_client_ptr = NULL;
      
   /* CRIT_SECTION taken care in the caller function */
   for (client_ptr = (loc_client_info_s_type *)q_check(&loc_middleware_data.client_module_data.z_loc_client_queue);
        client_ptr != NULL;
        client_ptr = (loc_client_info_s_type *)q_next(&loc_middleware_data.client_module_data.z_loc_client_queue, &client_ptr->z_Link) )
   {
      if (TRUE == client_ptr->in_use)
      {
         if (CLIENT_STATE_ACTIVE == client_ptr->client_state ||
             CLIENT_STATE_TRANSIENT == client_ptr->client_state)
         {
            // Calculate wait time based on last fix request time and current time
            time_delta_ms = current_time_ms - client_ptr->last_fix_request_time;

            temp_wait_time = 0 - (double) time_delta_ms;

            if((temp_wait_time <= INT_MAX) && (temp_wait_time >= INT_MIN))
            {
               LOC_MSG_MED ("loc_client_get_min_wait_time, client_handle %d temp_wait_time %d, last_fix_request_time %u", 
                             client_ptr->client_handle, (int32)temp_wait_time, client_ptr->last_fix_request_time);
            }
            else
            {
                LOC_MSG_MED ("temp wait time is over range", 0, 0, 0);
            }

            // Adjust for fix interval
            if ((CLIENT_STATE_ACTIVE == client_ptr->client_state) &&
                (FALSE == client_ptr->is_first_fix_for_tracking))
            {
               fix_interval = loc_client_get_fix_interval (&(client_ptr->fix_criteria_info));
               if( (DEFAULT_1HZ_FIX_INTERVAL >= fix_interval) &&
                   ((temp_wait_time + fix_interval) > 0) &&
                   (time_delta_ms > LOC_CLIENT_POS_OFFSET) )
               {
                  LOC_MSG_MED("loc_client_get_min_wait_time, client %d interval is one second", client_ptr->client_handle,0,0);
                  temp_wait_time = 0;
               }
               else
               {
                 // to avoid overflow issue
               temp_wait_time += fix_interval; 
               }
            }

            if ( (TRUE == first_client_ptr) ||
                 (temp_wait_time < wait_time) )

            {
               wait_time    = temp_wait_time;
               *min_time_client_ptr = client_ptr;
               first_client_ptr = FALSE;
            }
         }
      }
   }

      // there is no active client, wait forever
   if (TRUE == first_client_ptr)
   {
      return wait_time;
   }

   // Retrieve info regarding how long it takes to process the last fix
   last_fix_ttf = loc_middleware_data.pd_module_data.last_fix_ttf;

   // Shorten the wait interval to avoid the case we missed the fix from the engine
   if ((last_fix_ttf != 0) && (last_fix_ttf < MAXIMUM_OVERHEAD_TIME))
   {
      wait_time -= (MAXIMUM_OVERHEAD_TIME - last_fix_ttf);
   }
   
   LOC_MSG_MED("loc_client_get_min_wait_time, wait_time %d \n", (int32)wait_time, 0, 0);
   
   return wait_time; 
}

// This function goes through the queue and decides which client request to be processed next. The client
// with the minimal value of (last request time + fix_interval) will be processed next.
int loc_client_setup_request_config ()
{
   loc_client_info_s_type* client_ptr = NULL;
   time_type               time_ms = {0};
   uint64                  current_time_ms;

   // initialize variable to over range so error can be easily detected
   double                  wait_time = (double)INT_MAX + 1.0;

   time_get_uptime_ms(time_ms);
   current_time_ms = QW_CVT_Q2N(time_ms);


   LOC_MSG_MED ("loc_client_setup_request_config %d", current_time_ms, 0, 0);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   // First, check whether if there is cell id request that can be serviced from
   // cached position report
   loc_pd_process_concurrent_cellid_request ();

   // After cell id fix requests have been serviced from cached position report
   // every fix request will need to be serviced by calling pdsm_get_position

   wait_time = loc_client_get_min_wait_time(&client_ptr);
   if ((NULL != client_ptr) && (wait_time <= 0))
   {
      // there is one active client whose request need to be serviced, save the
      // configure from that application into the global setting
      loc_middleware_data.client_module_data.active_client_handle = client_ptr->client_handle;

      client_ptr->last_fix_request_time = current_time_ms;
      client_ptr->is_first_fix_for_tracking = FALSE;

      // change this client state to CLIENT_STATE_TRANSIENT_WAIT_FOR_POSITION
      if (client_ptr->client_state == CLIENT_STATE_TRANSIENT)
      {
         client_ptr->client_state = CLIENT_STATE_TRANSIENT_WAIT_FOR_POSITION;
      }

      // Translate pd fix config(fix criteria and server information)
      // from loc API to PD API
      loc_pd_xlate_fix_config_from_locapi_to_pdapi(client_ptr);
   } 

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   if (NULL == client_ptr) 
   {
      LOC_MSG_HIGH("loc_client_setup_request_config, no client is waiting, wait_time %d \n", 
                     (int32)wait_time, 0, 0);
      return INT32_MAX;
   }
   else if (wait_time >= (INT32_MAX-1))
   {
      LOC_MSG_MED("loc_client_setup_request_config, wait_time %d client_handle = %d \n", 
                     (int32)wait_time, client_ptr->client_handle, 0);
      return (INT32_MAX-1);
   }
   else
   {
      LOC_MSG_MED("loc_client_setup_request_config, wait_time %d client_handle = %d \n", 
                     (int32)wait_time, client_ptr->client_handle, 0);
      return (int32) wait_time;
   }
}

// This function is used by other modules to queue the loc_ioctl callback to be delivered to client
boolean loc_client_queue_pos_inj_request (const loc_pos_inj_request_s_type * const pPosInjRequest)
{
   loc_cmd_s_type             cmd_data;
   boolean                    result = FALSE;
   loc_cmd_nty_client_s_type * ptr_nty_client_data = NULL;

   memset( &cmd_data, 0, sizeof(cmd_data) );

   LOC_MSG_MED ( "loc_client_queue_pos_inj_request", 0,0,0);

   if(NULL != pPosInjRequest)
   {
       cmd_data.cmd_type = LOC_CMD_TYPE_NOTIFY_CLIENT;
       cmd_data.client_handle = LOC_HANDLE_REGISTERED_CLIENTS;
       cmd_data.ptr_data = NULL;
       ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
       if(NULL != ptr_nty_client_data)
   {
           ptr_nty_client_data->event_type = LOC_EVENT_ASSISTANCE_DATA_REQUEST;
           ptr_nty_client_data->event_data.assist_data_request.event = LOC_ASSIST_DATA_POSITION_INJECTION_REQ;
           ptr_nty_client_data->event_data.assist_data_request.payload.pos_injection = *pPosInjRequest;

           cmd_data.ptr_data = (void *)ptr_nty_client_data;
           if ( TRUE != loc_middleware_queue_cmd (&cmd_data))
      {
         // reclaim resources allocated for the message
              loc_free(ptr_nty_client_data);
              ptr_nty_client_data = NULL;
              cmd_data.ptr_data = NULL;
              result = FALSE;
           }
           else
           {
              // IPC sent, we're done.
              result = TRUE;
           }
      }
      else
      {
          // memory allocation error
          result = FALSE;
      }
   }
   else
   {
       // TODO: what else can we do?
       // null position injection data...internal error...
   }
   if(TRUE != result)
   {
      // TODO: what else can we do?
      LOC_MSG_ERROR ("loc_client_queue_pos_inj_request, failed", 0, 0, 0);
   }
   return result;
}


// This function is used by other modules to queue the loc_ioctl callback to be delivered to client
void loc_client_queue_ioctl_callback (loc_client_handle_type          client_handle,
                                      loc_ioctl_e_type                ioctl_type,
                                      int32                           ioctl_status,
                                      loc_ioctl_callback_data_u_type* ioctl_cb_data_ptr)
{
   loc_cmd_s_type             cmd_data;
   loc_cmd_s_type*            cmd_ptr = &cmd_data;
   boolean                    result = FALSE;
   loc_cmd_nty_client_s_type * ptr_nty_client_data = NULL;

   LOC_MSG_MED ("loc_client_queue_ioctl_callback, client handle = %d, ioctl type = %d, ioctl status = %d",
                client_handle, ioctl_type, ioctl_status);

   memset (cmd_ptr, 0, sizeof (loc_cmd_s_type));
   cmd_ptr->cmd_type = LOC_CMD_TYPE_NOTIFY_CLIENT;
   cmd_ptr->client_handle = client_handle;
   cmd_ptr->ptr_data = NULL;
   ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
   if(NULL != ptr_nty_client_data)
   {
       ptr_nty_client_data->event_type = LOC_EVENT_IOCTL_REPORT;
       ptr_nty_client_data->event_data.ioctl_report.type = ioctl_type;
       ptr_nty_client_data->event_data.ioctl_report.status = ioctl_status;
       if (ioctl_cb_data_ptr != NULL)
       {
          memscpy (&(ptr_nty_client_data->event_data.ioctl_report.data),
                  sizeof(ptr_nty_client_data->event_data.ioctl_report.data),
                  ioctl_cb_data_ptr,
                  sizeof (*ioctl_cb_data_ptr));
       }
       cmd_ptr->ptr_data = (void *)ptr_nty_client_data;
       if ( TRUE != loc_middleware_queue_cmd (cmd_ptr))
       {
          // reclaim resources allocated for the message
          loc_free(ptr_nty_client_data);
          ptr_nty_client_data = NULL;
          cmd_ptr->ptr_data = NULL;
          result = FALSE;
       }
       else
       {
          // IPC sent, we're done.
          result = TRUE;
       }
   }
   else
   {
      // memory allocation error
      result = FALSE;
   }

   if(TRUE != result)
   {
      // TODO: what else can we do?
      LOC_MSG_ERROR ("loc_client_queue_ioctl_callback, failed", 0, 0, 0);
   }
}

// Returns true if any of the location client is active
boolean loc_client_is_any_requesting_fix  ()
{
   loc_client_info_s_type* client_ptr = NULL;
   boolean                 ret_val = FALSE;

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
   for (client_ptr = (loc_client_info_s_type *)q_check(&loc_middleware_data.client_module_data.z_loc_client_queue);
        client_ptr != NULL;
        client_ptr = (loc_client_info_s_type *)q_next(&loc_middleware_data.client_module_data.z_loc_client_queue, &client_ptr->z_Link) )
   {
      if ((client_ptr->in_use == TRUE) &&
          ((client_ptr->client_state == CLIENT_STATE_ACTIVE) ||
           (client_ptr->client_state == CLIENT_STATE_TRANSIENT_WAIT_FOR_POSITION) ||
           (client_ptr->client_state == CLIENT_STATE_TRANSIENT)))
      {
         ret_val = TRUE;
         break;
      }
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   LOC_MSG_MED ("loc_client_is_any_requesting_fix %d", ret_val, 0, 0);

   return ret_val;
}

// Update the client state after a fix is done. If a client is requesting
// a single-shot fix, after the single shot fix is serviced, the client state
// will be updated to INACTIVE.
boolean loc_client_update_state_after_fix (void)
{
   loc_client_info_s_type* client_ptr = NULL;

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
   client_ptr = loc_client_find_handle (loc_middleware_data.client_module_data.active_client_handle);
   if (client_ptr != NULL)
   {
      if (client_ptr->client_state == CLIENT_STATE_TRANSIENT_WAIT_FOR_POSITION)
      {
         LOC_MSG_MED ("loc_client_update_state_after_fix client = %d requested single shot"
                      "changing state from transient wait for positin to inactive ", 
                      client_ptr->client_handle, 0, 0);
         client_ptr->client_state = CLIENT_STATE_INACTIVE;
         client_ptr->server_info.server_option = PDSM_SERVER_OPTION_USE_DEFAULT;
         loc_middleware_data.client_module_data.active_client_handle = LOC_CLIENT_HANDLE_INVALID;
      }
      else if (client_ptr->client_state == CLIENT_STATE_INACTIVE)
      {
         /* Set active client handle to INVALID for clients that are inactive */
         LOC_MSG_MED ("loc_client_update_state_after_fix client = %d was in inactive state", 
                      client_ptr->client_handle, 0, 0);
         loc_middleware_data.client_module_data.active_client_handle = LOC_CLIENT_HANDLE_INVALID;
      }
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   return TRUE;
}

// Returns the client that is requesting the fix
loc_client_handle_type loc_client_get_active_client  (void)
{
   return loc_middleware_data.client_module_data.active_client_handle;
}

// This function is used to process the cmd request from the loc middleware cmd queue.
// For example, if client issues an RPC call to retrieve information, the inforamtion
// can not be sent directly via callbacks, it will have to be put in the location
// middleware cmd queue and from there, the callback can be issued via RPC callback to
// the client.
int loc_client_process_cmd_request (loc_cmd_s_type * const ptr_cmd)
{
   int ret_val = LOC_API_GENERAL_FAILURE;
   boolean status = FALSE;

   if(NULL != ptr_cmd)
   {
       const loc_cmd_type_e_type cmd_type = ptr_cmd->cmd_type;
       const loc_client_handle_type client_handle = ptr_cmd->client_handle;
       loc_cmd_nty_client_s_type * const ptr_notify = (loc_cmd_nty_client_s_type *)ptr_cmd->ptr_data;

       switch (cmd_type)
       {
          case LOC_CMD_TYPE_NOTIFY_CLIENT:
          {
             if(NULL != ptr_notify)
             {
              status = loc_client_invoke_callback (client_handle,
                                             ptr_notify->event_type,
                                          &(ptr_notify->event_data));
              if (TRUE == status)
                 {
                  ret_val = LOC_API_SUCCESS;
                 }
                 else
                 {
                     // something wrong with the call back
                  ret_val = LOC_API_GENERAL_FAILURE;
                 }
             }
             else
             {
                 // internal error
              ret_val = LOC_API_GENERAL_FAILURE;
             }
             break;
          }
          default:
             // internal error
          ret_val = LOC_API_UNSUPPORTED;
             break;
       }
   }
   else
   {
       // internal error
       ret_val = LOC_API_GENERAL_FAILURE;
   }

   if(LOC_API_SUCCESS != ret_val)
   {
       LOC_MSG_ERROR ("loc_client_process_cmd_request internal error", 0, 0, 0);
   }
   return ret_val;
}

// This function is used to invoke the callback for the client. If the
// client_handle is LOC_HANDLE_REGISTERED_CLIENTS, then all clients registered
// for this particular event will get the callback. Otherwise, only the specified
// client handle that regiteres for the callback will get the event.
boolean loc_client_invoke_callback
(
      loc_client_handle_type    client_handle,
      loc_event_mask_type       event_type,
      loc_event_payload_u_type *event_payload_ptr
)
{
   loc_client_info_s_type* client_ptr = NULL;
   loc_client_handle_type  client_handle_1;
   boolean                 v_ReportIntermediate = FALSE;

   LOC_MSG_MED ("loc_client_invoke_callback, client_handle = %d, event_type = %d, max clients = %d",
                client_handle, event_type, loc_middleware_data.client_module_data.max_num_clients);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   for(client_ptr = (loc_client_info_s_type *)q_check(&(loc_middleware_data.client_module_data.z_loc_client_queue));
       client_ptr != NULL; 
       client_ptr = (loc_client_info_s_type *)q_next(&(loc_middleware_data.client_module_data.z_loc_client_queue), &client_ptr->z_Link) )
   {
      v_ReportIntermediate = FALSE;
      if ((client_ptr->in_use == TRUE) &&
          (((client_ptr->event_reg_mask & event_type) != 0))) 
      {
         client_handle_1 = client_ptr->client_handle;

         if ( ( LOC_EVENT_PARSED_POSITION_REPORT == event_type) && 
              ( LOC_SESS_STATUS_IN_PROGESS ==  event_payload_ptr->parsed_location_report.session_status ) &&
              (CLIENT_STATE_ACTIVE == client_ptr->client_state))
          {
            if( FALSE == client_ptr->fix_criteria_info.intermediate_pos_report_enabled )
            {
              LOC_MSG_LOW("loc_client_invoke_callback, supress the intermediate position reports",
                         0, 0, 0);

              /** Supress the intermediate position reports if the client has 
                  disabled it in the fix criteria */
              continue;
            }
            else if( 0 == client_ptr->fix_criteria_info.min_intermediate_interval)
            {
              LOC_MSG_LOW("loc_client_invoke_callback, min_intermediate_interval = 0",0,0,0);
              v_ReportIntermediate = TRUE;
            }
            else
            {
              uint64                  time_delta_ms = 0;
              uint64                  current_time_ms = 0;
              time_type               time_ms={0};

              time_get_uptime_ms (time_ms);
              current_time_ms = QW_CVT_Q2N(time_ms);

              if(current_time_ms > client_ptr->last_intermediate_fix_report_time)
              {
                time_delta_ms = current_time_ms - client_ptr->last_intermediate_fix_report_time;
              }
              else
              {
                /* In case of time service reset or error case, report this fix */
                time_delta_ms = (uint64)(client_ptr->fix_criteria_info.min_intermediate_interval);
              }

              LOC_MSG_LOW("loc_client_invoke_callback, intermediate position last_report_time=%u, current_time=%u, interval=%u",
                           client_ptr->last_intermediate_fix_report_time, current_time_ms, 
                           client_ptr->fix_criteria_info.min_intermediate_interval);
              if( time_delta_ms < (uint64)(client_ptr->fix_criteria_info.min_intermediate_interval - LOC_CLIENT_POS_OFFSET ) )
              {
                LOC_MSG_LOW("loc_client_invoke_callback, drop intermediate position ",0,0,0);
                continue;
              }
              else
              {
                client_ptr->last_intermediate_fix_report_time = current_time_ms;
                LOC_MSG_LOW("loc_client_invoke_callback, set client_ptr->last_intermediate_fix_report_time %llu ",client_ptr->last_intermediate_fix_report_time,0,0);
                v_ReportIntermediate = TRUE;
              }
            }
          }

         if ( ( client_handle_1 == client_handle) ||
              ( client_handle == LOC_HANDLE_REGISTERED_CLIENTS) ||
              ( TRUE == v_ReportIntermediate) )
         {
            LOC_MSG_MED ("loc_client_invoke_callback 2, client_handle = %d, event_type = %d",
             client_handle_1, event_type, 0);

            LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
            client_ptr->event_callback(client_handle_1,
                                       event_type,
                                       event_payload_ptr);
            LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
         }
      }
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);


   LOC_MSG_MED ("loc_client_invoke_callback done, client_handle = %d, event_type = %d",
                client_handle, event_type, 0);

   return TRUE;
}

// This function is used to process the loc_ioctl that belongs to the location client module.
int loc_client_process_ioctl
(
      loc_client_handle_type       client_handle,
      loc_ioctl_e_type             ioctl_type,
      const loc_ioctl_data_u_type* ioctl_data
)
{
   int                     ret_val    = LOC_API_UNSUPPORTED;
   loc_client_info_s_type* client_ptr = NULL;
   uint32                  error_code = 0;

   LOC_MSG_MED ("loc_client_process_ioctl, client_handle=%d, ioctl_type = %d", client_handle, ioctl_type, 0);

  LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
   client_ptr = loc_client_find_handle (client_handle);

   if (client_ptr == NULL)
  {
      ret_val =  LOC_API_INVALID_HANDLE;
  }
   else
  {
      switch (ioctl_type)
    {
      case LOC_IOCTL_GET_API_VERSION:
      case LOC_IOCTL_GET_FIX_CRITERIA:
      case LOC_IOCTL_GET_CUSTOM_PDE_SERVER_ADDR:
      case LOC_IOCTL_GET_OPERATION_MODE:
      case LOC_IOCTL_GET_EVENT_REG_MASK:
        ret_val = loc_client_process_info_request (client_ptr, ioctl_type);
      break;



      case LOC_IOCTL_SET_FIX_CRITERIA:
         if(NULL != ioctl_data)
    {
             ret_val = loc_client_set_fix_criteria (client_ptr, &(ioctl_data->fix_criteria));
    }
         else
    {
             ret_val = LOC_API_INVALID_PARAMETER;
    }
      break;

      case LOC_IOCTL_SET_CUSTOM_PDE_SERVER_ADDR:
         if(NULL != ioctl_data)
    {
             ret_val = loc_client_set_custom_pde_server_addr (client_ptr, &(ioctl_data->server_addr));
    }
         else
    {
             ret_val = LOC_API_INVALID_PARAMETER;
    }
      break;

      case LOC_IOCTL_SET_EVENT_REG_MASK:
         if(NULL != ioctl_data)
  {
             ret_val = loc_client_set_event_reg_mask_locked (client_ptr, (ioctl_data->event_reg_mask));
  }
  else
  {
             ret_val = LOC_API_INVALID_PARAMETER;
  }
         break;

        case LOC_IOCTL_SET_OPERATION_MODE:

          if(NULL != ioctl_data)
{
              ret_val = loc_client_set_operation_mode (client_ptr, &(ioctl_data->operation_mode));
          }
          else
   {
              ret_val = LOC_API_INVALID_PARAMETER;
   }
          break;

      default:
         ret_val = LOC_API_UNSUPPORTED;
         break;
      }
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   if(LOC_API_SUCCESS == ret_val)
   {
      // done
      }
      else
      {
      LOC_MSG_ERROR ( "loc_client_process_ioctl, ioctl: %d, return value: %d, error code: %u\n", ioctl_type, ret_val, error_code);
   }

   return ret_val;
   }

// Function to retrieve information from location client module.
static int loc_client_process_info_request
(
      const loc_client_info_s_type*  client_ptr,
      loc_ioctl_e_type         ioctl_type
)
{

   loc_cmd_s_type           cmd_data;
   loc_cmd_s_type*          cmd_ptr = &cmd_data;
   int                      ret_val = LOC_API_GENERAL_FAILURE;
   boolean                      result = FALSE;
   loc_cmd_nty_client_s_type *  ptr_nty_client_data = NULL;

   LOC_MSG_HIGH ("loc_client_process_info_request, ioctl_type = %d \n", ioctl_type, 0, 0);

   memset (cmd_ptr, 0, sizeof (loc_cmd_s_type));

   cmd_ptr->cmd_type = LOC_CMD_TYPE_NOTIFY_CLIENT;
   cmd_ptr->client_handle = client_ptr->client_handle;
   cmd_ptr->ptr_data = NULL;
   ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
   if(NULL != ptr_nty_client_data)
      {
       ptr_nty_client_data->event_type = LOC_EVENT_IOCTL_REPORT;
       ptr_nty_client_data->event_data.ioctl_report.type = ioctl_type;
       ptr_nty_client_data->event_data.ioctl_report.status = LOC_API_SUCCESS;

       switch (ioctl_type)
   {
       case LOC_IOCTL_GET_FIX_CRITERIA:
          memscpy (&(ptr_nty_client_data->event_data.ioctl_report.data.fix_criteria),
		  sizeof(ptr_nty_client_data->event_data.ioctl_report.data.fix_criteria),
                  &(client_ptr->fix_criteria_info),
                  sizeof (client_ptr->fix_criteria_info));
          ret_val = LOC_API_SUCCESS;
         break;

       case LOC_IOCTL_GET_API_VERSION:
          ptr_nty_client_data->event_data.ioctl_report.data.api_version.major = LOC_API_MAJOR_VERSION_NUMBER;
          ptr_nty_client_data->event_data.ioctl_report.data.api_version.minor = LOC_API_MINOR_VERSION_NUMBER;
          ret_val = LOC_API_SUCCESS;
            break;

       case LOC_IOCTL_GET_CUSTOM_PDE_SERVER_ADDR:
          if (client_ptr->server_info.server_option == PDSM_SERVER_OPTION_USE_LOCAL)
      {
             loc_pa_xlate_server_addr_from_pdapi_to_locapi (&(client_ptr->server_info.server_adrs),
                                                            &(ptr_nty_client_data->event_data.ioctl_report.data.server_addr));
      }
          else
      {
             // In case client has not set the custom server address, all fields will be 0.
             ptr_nty_client_data->event_data.ioctl_report.data.server_addr.addr_type = LOC_SERVER_ADDR_IPV4;
             ptr_nty_client_data->event_data.ioctl_report.data.server_addr.addr_info.ipv4.addr = 0;
             ptr_nty_client_data->event_data.ioctl_report.data.server_addr.addr_info.ipv4.port = 0;
      }
          ret_val = LOC_API_SUCCESS;
          break;

       case LOC_IOCTL_GET_EVENT_REG_MASK:
   {
           ptr_nty_client_data->event_data.ioctl_report.data.event_reg_mask= 
                             client_ptr->event_reg_mask;
           ret_val = LOC_API_SUCCESS;
   }
       break;


       case LOC_IOCTL_GET_OPERATION_MODE:
{
           ptr_nty_client_data->event_data.ioctl_report.data.operation_mode = 
                 client_ptr->fix_criteria_info.preferred_operation_mode;
           ret_val = LOC_API_SUCCESS;
   }
         break;


         default:
          ret_val = LOC_API_UNSUPPORTED;
            break;
         }

       if (LOC_API_SUCCESS == ret_val)
      {
         cmd_ptr->ptr_data = (void *)ptr_nty_client_data;
         if(TRUE != loc_middleware_queue_cmd (cmd_ptr))
      {
              // reclaim resources allocated for the message
              loc_free(ptr_nty_client_data);
              ptr_nty_client_data = NULL;
              cmd_ptr->ptr_data = NULL;
              result = FALSE;
      }
          else
      {
              // IPC sent, we're done.
              result = TRUE;
              ptr_nty_client_data = NULL;
            }
         }
       else
         {
              // reclaim resources allocated for the message
              loc_free(ptr_nty_client_data);
              ptr_nty_client_data = NULL;
              cmd_ptr->ptr_data = NULL;
              result = FALSE;
      }
      }
         else
         {
      // memory allocation error
      result = FALSE;
      ret_val = LOC_API_GENERAL_FAILURE;
         }

   if(TRUE != result)
      {
      LOC_MSG_ERROR ("loc_client_process_info_request, failed", 0, 0, 0);
      }
   return ret_val;
}

// Set event registration mask for the calling client
// note that this function is supposed to be called when the client
// data stucture is locked from concurrent access
static int loc_client_set_event_reg_mask_locked
(
      loc_client_info_s_type* const   client_ptr,
      const loc_event_mask_type       new_event_mask
)
{
    /* static variable to check if any client has ever registered for event
     * LOC_EVENT_LOCATION_SERVER_REQUEST */
    static boolean      conn_clnt_registered = FALSE;

    loc_event_mask_type old_event_mask   = client_ptr->event_reg_mask;
    boolean             ret_val          = TRUE;

    /* If LOC_EVENT_LOCATION_SERVER_REQUEST event registration changes, handle differently */
    if((old_event_mask  ^ new_event_mask) & LOC_EVENT_LOCATION_SERVER_REQUEST)
   {
      if(new_event_mask & LOC_EVENT_LOCATION_SERVER_REQUEST)
      {
        /* client trying to register for LOC_EVENT_LOCATION_SERVER_REQUEST. 
         * if the connection client has previously registered, ignore the req. */
        if( conn_clnt_registered == FALSE )
      {
          ret_val = loc_conn_init();
          if(ret_val == TRUE)
         {
            conn_clnt_registered = TRUE;
            }
            }
            }
      else
            {
        /* client trying to deregister for LOC_EVENT_LOCATION_SERVER_REQUEST. 
         * simply filter his req at qmi_loc level, do not change loc_conn state */
        LOC_MSG_ERROR("Dereg LOC_EVENT_LOCATION_SERVER_REQUEST req rcvd!!", 0, 0, 0);
         }
      }

    if(ret_val == FALSE)
      {
      loc_client_queue_ioctl_callback (client_ptr->client_handle, LOC_IOCTL_SET_EVENT_REG_MASK, LOC_API_GENERAL_FAILURE, NULL);
      return LOC_API_GENERAL_FAILURE;
      }


    client_ptr->event_reg_mask = new_event_mask;

    loc_client_queue_ioctl_callback (client_ptr->client_handle, LOC_IOCTL_SET_EVENT_REG_MASK, LOC_API_SUCCESS, NULL);

    return LOC_API_SUCCESS;
}

static int loc_client_set_operation_mode
(
  loc_client_info_s_type* const              client_ptr,
  const qmiLocSetOperationModeReqMsgT_v02   *op_mode
)
{
  if(NULL == client_ptr || NULL == op_mode)
      {
    LOC_MSG_ERROR ("loc_client_set_operation_mode, client_ptr= 0x%x, op_mode= 0x%x\n",client_ptr, op_mode, 0);
    return LOC_API_INVALID_HANDLE;
  }

  client_ptr->fix_criteria_info.preferred_operation_mode = op_mode->operationMode;
  if(op_mode->minInterval_valid)
            {
    client_ptr->fix_criteria_info.min_interval =  op_mode->minInterval;
    loc_client_set_fix_min_interval();
            }
  LOC_MSG_MED ("loc_client_set_operation_mode, mode = %d, min_interval_valid= %d, min_interval = %d",
       op_mode->operationMode,
       op_mode->minInterval_valid,
       op_mode->minInterval);
  loc_client_queue_ioctl_callback (client_ptr->client_handle, LOC_IOCTL_SET_OPERATION_MODE, LOC_API_SUCCESS, NULL);

  return LOC_API_SUCCESS;
}


// Set the fix criteria for a location client.
static int loc_client_set_fix_criteria
(
      loc_client_info_s_type*         client_ptr,
      const loc_fix_criteria_s_type*  fix_criteria_ptr
)
{
   LOC_MSG_MED ("loc_client_set_fix_criteria, valid mask = 0x%x \n", 
                 fix_criteria_ptr->valid_mask, 0, 0);
   if (fix_criteria_ptr->valid_mask & LOC_FIX_CRIT_VALID_PREFERRED_OPERATION_MODE)
    {
      client_ptr->fix_criteria_info.preferred_operation_mode = fix_criteria_ptr->preferred_operation_mode;
    }

   if (fix_criteria_ptr->valid_mask & LOC_FIX_CRIT_VALID_RECURRENCE_TYPE)
   {
      client_ptr->fix_criteria_info.recurrence_type = fix_criteria_ptr->recurrence_type;
}

   if (fix_criteria_ptr->valid_mask & LOC_FIX_CRIT_VALID_PREFERRED_ACCURACY)
{
      client_ptr->fix_criteria_info.preferred_accuracy = fix_criteria_ptr->preferred_accuracy;
   }

   if (fix_criteria_ptr->valid_mask & LOC_FIX_CRIT_VALID_PREFERRED_RESPONSE_TIME)
   {
      client_ptr->fix_criteria_info.preferred_response_time = fix_criteria_ptr->preferred_response_time;
   }

   if (fix_criteria_ptr->valid_mask & LOC_FIX_CRIT_VALID_INTERMEDIATE_POS_REPORT_ENABLED )
   {
      client_ptr->fix_criteria_info.intermediate_pos_report_enabled = fix_criteria_ptr->intermediate_pos_report_enabled;
      }

   if (fix_criteria_ptr->valid_mask & LOC_FIX_CRIT_VALID_NOTIFY_TYPE)
      {
      client_ptr->fix_criteria_info.notify_type = fix_criteria_ptr->notify_type;
      }

   if (fix_criteria_ptr->valid_mask & LOC_FIX_CRIT_VALID_MIN_INTERVAL)
      {
      client_ptr->fix_criteria_info.min_interval = fix_criteria_ptr->min_interval;
      }

   if (fix_criteria_ptr->valid_mask & LOC_FIX_CRIT_VALID_MIN_DISTANCE)
      {
      client_ptr->fix_criteria_info.min_distance = fix_criteria_ptr->min_distance;
      }

   if (fix_criteria_ptr->valid_mask & LOC_FIX_CRIT_VALID_MIN_DIST_SAMPLE_INTERVAL)
   {
      client_ptr->fix_criteria_info.min_dist_sample_interval = fix_criteria_ptr->min_dist_sample_interval;
}

   // Update the sample interval
   if (fix_criteria_ptr->valid_mask & LOC_FIX_CRIT_VALID_MIN_INTERVAL)
{
      client_ptr->fix_criteria_info.min_interval = fix_criteria_ptr->min_interval;
   }

   //update the supl application ID
   if (fix_criteria_ptr->valid_mask & LOC_FIX_CRIT_VALID_APPLICATION_ID)
   {
      //set the application id mask
      client_ptr->fix_criteria_info.valid_mask |= LOC_FIX_CRIT_VALID_APPLICATION_ID;
      memscpy((void *)&client_ptr->fix_criteria_info.application_id,
              sizeof(client_ptr->fix_criteria_info.application_id),
             (void *)&fix_criteria_ptr->application_id,
             sizeof(fix_criteria_ptr->application_id));
   }

   client_ptr->fix_criteria_info.min_intermediate_interval = fix_criteria_ptr->min_intermediate_interval;
   LOC_MSG_MED("loc_client_set_fix_criteria min_intermediate_interval=%d",
                client_ptr->fix_criteria_info.min_intermediate_interval , 0,0);
   loc_client_queue_ioctl_callback (client_ptr->client_handle, LOC_IOCTL_SET_FIX_CRITERIA, LOC_API_SUCCESS, NULL);

   return LOC_API_SUCCESS;
      }

// Return fix interval in milliseconds
uint32 loc_client_get_fix_interval (loc_fix_criteria_s_type*  fix_criteria_ptr)
{
   uint32 fix_interval;

   // Time between fixes
   if (fix_criteria_ptr->notify_type == LOC_NOTIFY_ON_DISTANCE)
      {
      fix_interval = fix_criteria_ptr->min_dist_sample_interval;
      }
      else
      {
      fix_interval = fix_criteria_ptr->min_interval;
      }

   return fix_interval;
      }

// Set up the custom server address for this client
static int loc_client_set_custom_pde_server_addr
(
   loc_client_info_s_type        *client_ptr,
   const loc_server_info_s_type  *server_addr_ptr
)
   {
   client_ptr->server_info.server_option = PDSM_SERVER_OPTION_USE_LOCAL;
   loc_pa_xlate_server_addr_from_locapi_to_pdapi (server_addr_ptr, &(client_ptr->server_info.server_adrs));

   // Queue the callback to the client
   loc_client_queue_ioctl_callback (client_ptr->client_handle, LOC_IOCTL_SET_CUSTOM_PDE_SERVER_ADDR, LOC_API_SUCCESS, NULL);

   return LOC_API_SUCCESS;
}

// Report position to concurrent cell id request
extern void loc_client_report_position_to_cellid_request 
(
      loc_event_payload_u_type* pos_payload_ptr,
      uint64                    current_time_ms
)
{
   loc_client_info_s_type* client_ptr = NULL;

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
   for (client_ptr = (loc_client_info_s_type *)q_check(&loc_middleware_data.client_module_data.z_loc_client_queue);
        client_ptr != NULL;
        client_ptr = (loc_client_info_s_type *)q_next(&loc_middleware_data.client_module_data.z_loc_client_queue, &client_ptr->z_Link) )
   {
         if ((TRUE == client_ptr->in_use) &&
             (client_ptr->fix_criteria_info.preferred_operation_mode == eQMI_LOC_OPER_MODE_CELL_ID_V02))
   {
            if (CLIENT_STATE_TRANSIENT == client_ptr->client_state)
      {
               // Report position
               // Set state
               loc_client_invoke_callback (client_ptr->client_handle,
                                           LOC_EVENT_PARSED_POSITION_REPORT,
                                           pos_payload_ptr);
               client_ptr->client_state = CLIENT_STATE_INACTIVE;

               LOC_MSG_MED ("loc_client_report_position_to_cellid_request for client = %d", 
                            client_ptr->client_handle, 0, 0);
         }
            else if ((CLIENT_STATE_ACTIVE == client_ptr->client_state) &&
                     (TRUE == client_ptr->is_first_fix_for_tracking))
         {
               // Report position
               // Set state
               loc_client_invoke_callback (client_ptr->client_handle,
                                           LOC_EVENT_PARSED_POSITION_REPORT,
                                           pos_payload_ptr);

               client_ptr->is_first_fix_for_tracking = FALSE;
               client_ptr->last_fix_request_time = current_time_ms;

               LOC_MSG_MED ("loc_client_report_position_to_cellid_request for client = %d", 
                            client_ptr->client_handle, 0, 0);
      }
   }
}

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
   }

boolean loc_client_is_server_request_handler_registered (void)
   {
    loc_client_info_s_type* client_ptr = NULL;
    boolean found =         FALSE;

    LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   for (client_ptr = (loc_client_info_s_type *)q_check(&loc_middleware_data.client_module_data.z_loc_client_queue);
        client_ptr != NULL;
        client_ptr = (loc_client_info_s_type *)q_next(&loc_middleware_data.client_module_data.z_loc_client_queue, &client_ptr->z_Link) )
{
       if ((client_ptr->in_use == TRUE) &&
           ((client_ptr->event_reg_mask & LOC_EVENT_LOCATION_SERVER_REQUEST) != 0))
   {
           found = TRUE;
   }
   }
    LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

    LOC_MSG_HIGH ("loc_client_is_server_request_handler_registered: %d", (int)found,0,0);

    return found;
}

