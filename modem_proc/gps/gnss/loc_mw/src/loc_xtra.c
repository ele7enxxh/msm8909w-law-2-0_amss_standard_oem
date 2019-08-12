/******************************************************************************
  @file: loc_xtra.c
  @brief:   This module handles gpsone XTRA related features .
  

  DESCRIPTION
   Qualcomm Location API XTRA Module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.

  Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
  All Rights Reserved.
  QCA Proprietary and Confidential. 
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_xtra.c#1 $
$DateTime: 2016/12/13 07:59:45 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
04/15/14   yh       XTRA version check added   
04/27/11   spn      Updated for Loc-Api 2.0
======================================================================*/



#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "msg.h"
#include "queue.h"

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

// Assembly xtra file
#define MAX_XTRA_FILE_SIZE   (100*1024)
#define MAX_XTRA_PART_SIZE   (10*1024)

// Number of seconds between January 1, 1970 and January 6, 1980
#define DELTA_UTC_TO_GPS_SECONDS        (3657 * 24 * 60 * 60)
#define XTRA_OPER_DEFAULT_TIMEOUT       (1 * 1000)  // 1 second
#define XTRA_DATA_INJECT_TIMEOUT        (60 * 1000)  // 60 seconds


static void loc_xtra_event_cb
(
      void                        *user_data,
      pdsm_xtra_event_type         xtra_event,
      const pdsm_xtra_info_s_type *xtra_info_ptr
);

static void loc_xtra_cmd_cb
(
      void                     *user_data,
      pdsm_xtra_cmd_e_type      xtra_cmd,
      pdsm_xtra_cmd_err_e_type  xtra_cmd_err
);

static void loc_xtra_process_xtra_download_event
(
      const pdsm_xtra_download_info_s_type* xtra_download_info_ptr
);

static void loc_xtra_process_data_validity_event
(
      const pdsm_xtra_download_info_s_type* xtra_download_info_ptr
);

static void loc_xtra_process_time_download_event
(
      const pdsm_xtra_time_info_req_s_type*  xtra_time_info_ptr
);

static void loc_xtra_process_status_event
(
      pdsm_xtra_status_e_type xtra_cmd_status
);


static int loc_xtra_process_data_inject
(
      loc_client_handle_type  client_handle,
      const loc_predicted_orbits_data_s_type* xtra_info_ptr
);

static int loc_xtra_process_data_validity_query
(
      void
);


static int loc_xtra_process_xtra_download_info_query
(
      void
);

static int loc_xtra_process_auto_download_config
(
      const loc_predicted_orbits_auto_download_config_s_type*  auto_download_info_ptr
);

static int loc_xtra_process_time_inject (
      loc_client_handle_type  client_handle,
      const loc_assist_data_time_s_type* time_info_ptr
);


static void loc_xtra_queue_data_inject_request (void);
static void loc_xtra_reset_module              (void);

static void loc_xtra_send_reserved_xtra_ind (
    loc_client_handle_type              client_handle,
    int32 status, 
    uint16 part_num );


// Initialize the xtra module
boolean loc_xtra_init (void)
{
   boolean                       ret_val = FALSE;
   pdsm_client_status_e_type     status;
   loc_xtra_module_data_s_type*  xtra_module_data_ptr;

   do
   {
      loc_middleware_data.xtra_client_id = pdsm_client_init (PDSM_CLIENT_TYPE_XTRA);

      if (loc_middleware_data.xtra_client_id == -1)
      {
         LOC_MSG_ERROR ( "loc_xtra_init: pdsm_client_init for XTRA failed", 0, 0, 0);
         break;
      }

      status = pdsm_client_xtra_reg(loc_middleware_data.xtra_client_id,
                                     NULL,
                                     loc_xtra_event_cb,
                                     PDSM_CLIENT_EVENT_REG,
                                     PDSM_XTRA_EVENT_MASK,
                                     NULL
                                    );
      if (status != PDSM_CLIENT_OK)
      {
         LOC_MSG_ERROR ("loc_xtra_init: pdsm_client_xtra_reg failed, error code = %d", status, 0, 0);
         break;
      }

      status = pdsm_client_act(loc_middleware_data.xtra_client_id);
      if(status != PDSM_CLIENT_OK)
      {
         LOC_MSG_ERROR ("loc_xtra_init: pdsm_client_act for xtra failed, error code =%d", status, 0, 0);
         break;
      }

      // xtra module data has been zero-initialized in loc_task.c
      xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);

      // Allocate the memory for the XTRA data when needed and free it when not needed. 
      xtra_module_data_ptr->xtra_data_ptr = NULL;
      
      // Have a timer to reset xtra state if expected callback does not come back
      xtra_module_data_ptr->xtra_process_timer = 
                          os_TimerCreate( LOC_MIDDLEWARE_TIMER_ID_XTRA, THREAD_ID_LOC_MIDDLEWARE );

      loc_xtra_reset_module ();
      ret_val = TRUE;

   } while (0);


   LOC_MSG_MED ("loc_xtra_init: returned %d", ret_val, 0, 0);
   return ret_val;
}

// Function to process xtra related loc_ioctl.
int loc_xtra_process_ioctl
(
      loc_client_handle_type       client_handle,
      loc_ioctl_e_type             ioctl_type,
      const loc_ioctl_data_u_type* ioctl_data
)
{
   loc_xtra_module_data_s_type* xtra_module_data_ptr;
   int ret_val = LOC_API_UNSUPPORTED;

   xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
   if ((xtra_module_data_ptr->xtra_cmd_is_in_progress == FALSE) ||
       ((xtra_module_data_ptr->xtra_cmd_in_progress == PDSM_XTRA_CMD_SET_DATA) &&
        (ioctl_type == LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA)))
   {

      xtra_module_data_ptr->xtra_cmd_is_in_progress = TRUE;
      xtra_module_data_ptr->active_client_handle    = client_handle;
      xtra_module_data_ptr->xtra_ioctl_type         = ioctl_type;

      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);

      switch (ioctl_type)
      {
	  // XTRA file injection
      case LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA:
         if(NULL != ioctl_data)
         {
             xtra_module_data_ptr->xtra_cmd_in_progress = PDSM_XTRA_CMD_SET_DATA;
             ret_val = loc_xtra_process_data_inject (client_handle, &(ioctl_data->predicted_orbits_data));
         }
         else
         {
             ret_val = LOC_API_INVALID_PARAMETER;
         }
         break;
      // Qeury XTRA data validity
      case LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_VALIDITY:
         xtra_module_data_ptr->xtra_cmd_in_progress = PDSM_XTRA_CMD_QUERY_DATA_VALIDITY;
         ret_val = loc_xtra_process_data_validity_query ();
         break;
      // Query XTRA server and other xtra file info (file size, maximum size can be injected in one call)
      case LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_SOURCE:
         xtra_module_data_ptr->xtra_cmd_in_progress = PDSM_XTRA_CMD_INITIATE_DOWNLOAD;
         ret_val = loc_xtra_process_xtra_download_info_query ();
         break;
      // Enable XTRA auto-download
      case LOC_IOCTL_SET_PREDICTED_ORBITS_DATA_AUTO_DOWNLOAD:
         if(NULL != ioctl_data)
         {
             xtra_module_data_ptr->xtra_cmd_in_progress = PDSM_XTRA_CMD_SET_AUTO_DOWNLOAD_PARAMS;
             ret_val = loc_xtra_process_auto_download_config (&(ioctl_data->predicted_orbits_auto_download));
         }
         else
         {
             ret_val = LOC_API_INVALID_PARAMETER;
         }
         break;
      // Inject UTC time
      case LOC_IOCTL_INJECT_UTC_TIME:
         if(NULL != ioctl_data)
         {
             xtra_module_data_ptr->xtra_cmd_in_progress = PDSM_XTRA_CMD_TIME_INFO_INJ;
             ret_val = loc_xtra_process_time_inject (client_handle, &(ioctl_data->assistance_data_time));
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

      if (ret_val != LOC_API_SUCCESS)
      {
         loc_xtra_reset_module ();
      }
   }
   else
   {
      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);

      LOC_MSG_ERROR ("loc_xtra_process_ioctl, xtra cmd = %d in-progress", xtra_module_data_ptr->xtra_cmd_in_progress, 0, 0);
      ret_val = LOC_API_ENGINE_BUSY;
   }

   return ret_val;
}

// Function to process ioctl calls from the location middleware task.
int loc_xtra_process_cmd_request (loc_cmd_s_type * const ptr_cmd)
{
   boolean                      status = FALSE;
   int                          ret_val = LOC_API_GENERAL_FAILURE;

   uint32                       xtra_data_len;
   loc_xtra_module_data_s_type* xtra_module_data_ptr = NULL;
    
   if(NULL != ptr_cmd)
   {
       const loc_cmd_type_e_type cmd_type = ptr_cmd->cmd_type;
       switch (cmd_type)
       {

       case LOC_CMD_TYPE_INJECT_XTRA_DATA:
         xtra_module_data_ptr =  
            (loc_xtra_module_data_s_type *)os_MemAlloc(
              sizeof(*xtra_module_data_ptr),OS_MEM_SCOPE_TASK);

          if (NULL != xtra_module_data_ptr)
          {
            //make a local copy of the xtra_module data 
            LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
  
            memscpy(xtra_module_data_ptr, sizeof(*xtra_module_data_ptr),
                   &(loc_middleware_data.xtra_module_data), sizeof(loc_middleware_data.xtra_module_data));
  
            LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
  
            if ((xtra_module_data_ptr->xtra_cmd_is_in_progress == TRUE) &&
                (xtra_module_data_ptr->xtra_cmd_in_progress == PDSM_XTRA_CMD_SET_DATA))
            {
               xtra_data_len = xtra_module_data_ptr->num_of_bytes_received - 
                 xtra_module_data_ptr->num_of_bytes_injected;

               if (xtra_data_len > MAX_XTRA_PART_SIZE)
               {
                  xtra_data_len = MAX_XTRA_PART_SIZE;
               }
      
               LOC_MSG_MED ("loc_xtra_process_cmd_request, data inject: "
                            "total part = %d, part number = %d, num of bytes = %d",
                            xtra_module_data_ptr->total_parts_to_inject, 
                            xtra_module_data_ptr->num_of_parts_injected, 
                            xtra_data_len);
               status = pdsm_xtra_set_data (loc_xtra_cmd_cb,
                                            loc_middleware_data.xtra_client_id,
                                            (void *) ptr_cmd->client_handle,
                                            xtra_data_len,
                                            xtra_module_data_ptr->xtra_data_ptr +
                                             xtra_module_data_ptr->num_of_bytes_injected,
                                            xtra_module_data_ptr->num_of_parts_injected,
                                            xtra_module_data_ptr->total_parts_to_inject,
                                            PDSM_XTRA_STATUS_SUCCESS);
      
               // In case of failure, nofity this through IOCTL status
               if (FALSE == status)
               {
                  loc_client_queue_ioctl_callback(
                    xtra_module_data_ptr->active_client_handle,
                    xtra_module_data_ptr->xtra_ioctl_type,
                    LOC_API_GENERAL_FAILURE,
                    NULL);

                  loc_xtra_reset_module ();

                  ret_val = LOC_API_GENERAL_FAILURE;
               }
               else
               {
                 LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
                 // parts index starts at 1
                 loc_middleware_data.xtra_module_data.num_of_parts_injected++;
                 loc_middleware_data.xtra_module_data.num_of_bytes_injected += xtra_data_len;
                 ret_val= LOC_API_SUCCESS;
                 LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
               }
            }
                        
            os_MemFree((void**) &xtra_module_data_ptr);
          }
          break;

       default:
          // this is indeed an internal error
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
       LOC_MSG_ERROR ("loc_xtra_process_cmd_request internal error", 0, 0, 0);
   }
   return ret_val;
}

// Process requst for xtra data validity query through loc_ioctl with
// LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA.
static int loc_xtra_process_data_inject (loc_client_handle_type              client_handle,
                                         const loc_predicted_orbits_data_s_type* xtra_info_ptr)
{
   int                          ret_val = LOC_API_SUCCESS;
   loc_xtra_module_data_s_type* xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);

   LOC_MSG_MED ("loc_xtra_process_data_inject, total part = %d, part number = %d, num of bytes = %d",
                 xtra_info_ptr->total_parts, xtra_info_ptr->part, xtra_info_ptr->part_len);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);

   if ((xtra_module_data_ptr->xtra_cmd_is_in_progress == TRUE) &&
       (xtra_module_data_ptr->xtra_cmd_in_progress == PDSM_XTRA_CMD_SET_DATA))
   {
      if(NULL == xtra_module_data_ptr->xtra_data_ptr) 
      {
        // The buffer for XTRA data is not yet allocated . 
        // Allocate the buffer for the XTRA data in LOC_MW to copy the injected XTRA file 
        xtra_module_data_ptr->xtra_data_ptr = (unsigned char*)os_MemAlloc(MAX_XTRA_FILE_SIZE, OS_MEM_SCOPE_TASK);
        LOC_MSG_MED ("loc_xtra_process_data_inject: allocated buffer for xtra file %p", 
                     xtra_module_data_ptr->xtra_data_ptr, 0, 0);
        if (NULL == xtra_module_data_ptr->xtra_data_ptr)
        {
           // Memory allocation failed 
           LOC_MSG_ERROR ("loc_xtra_process_data_inject: allocating buffer for xtra file failed", 0, 0, 0);
           ret_val = LOC_API_GENERAL_FAILURE ;
           LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
           return ret_val;
        }    
      }
      // First part injection, initialize the variables
      if (xtra_info_ptr->part == 1)
      {
         xtra_module_data_ptr->total_parts_to_receive = xtra_info_ptr->total_parts;
         xtra_module_data_ptr->total_bytes_to_receive = xtra_info_ptr->total_size;
         xtra_module_data_ptr->num_of_parts_received = 0;
         xtra_module_data_ptr->num_of_bytes_received = 0;
         memset (xtra_module_data_ptr->xtra_data_ptr, 0, MAX_XTRA_FILE_SIZE);

         // Set up the timer, all parts of the file need to be injected before the timeout period expires
         os_TimerStart( xtra_module_data_ptr->xtra_process_timer, XTRA_DATA_INJECT_TIMEOUT, 0 );
      }

      // Return error for out of order bytes or xtra file is too big for the buffer
      if ((xtra_module_data_ptr->num_of_parts_received != (xtra_info_ptr->part-1)) ||
          (xtra_module_data_ptr->total_bytes_to_receive != xtra_info_ptr->total_size) ||
          ((xtra_module_data_ptr->num_of_bytes_received + xtra_info_ptr->part_len) > MAX_XTRA_FILE_SIZE ))
      {
         LOC_MSG_ERROR ("loc_xtra_process_data_inject, can not handle out of order part or xtra file too big", 0, 0, 0);
         ret_val = LOC_API_INVALID_PARAMETER;
      }
      else
      {
         memscpy (xtra_module_data_ptr->xtra_data_ptr + xtra_module_data_ptr->num_of_bytes_received,
                 xtra_info_ptr->part_len, xtra_info_ptr->data_ptr, xtra_info_ptr->part_len);
         xtra_module_data_ptr->num_of_bytes_received += xtra_info_ptr->part_len;
         xtra_module_data_ptr->num_of_parts_received++;

         // We have received all parts, queue the request to inject the file to the engine
         if (xtra_module_data_ptr->total_parts_to_receive == xtra_module_data_ptr->num_of_parts_received)
         {
           if (xtra_module_data_ptr->total_bytes_to_receive == xtra_module_data_ptr->num_of_bytes_received)
           {
              xtra_module_data_ptr->total_parts_to_inject = xtra_module_data_ptr->num_of_bytes_received / MAX_XTRA_PART_SIZE;
              if ((xtra_module_data_ptr->num_of_bytes_received % MAX_XTRA_PART_SIZE) != 0)
              {
                 xtra_module_data_ptr->total_parts_to_inject++;
              }
              // XTRA injection starts at part 1
              loc_middleware_data.xtra_module_data.num_of_parts_injected = 1;
              loc_middleware_data.xtra_module_data.num_of_bytes_injected = 0;
              loc_xtra_queue_data_inject_request ();
           }
           else
           {
              ret_val = LOC_API_INVALID_PARAMETER;
           }
         }
         /* Generate Indications for intermediate Parts */
         else
         {
          loc_xtra_send_reserved_xtra_ind( client_handle,
                                           LOC_API_SUCCESS,
                                           xtra_info_ptr->part);
         }
      }
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);

   return ret_val;
}

// Queue the next cmd to inject the next part of xtra file.
static void loc_xtra_queue_data_inject_request (void)
{
   loc_cmd_s_type               cmd_data;
   loc_cmd_s_type              *cmd_ptr = &cmd_data;
   loc_xtra_module_data_s_type *xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);

   LOC_MSG_HIGH ("loc_xtra_queue_data_inject_request, bytes injected = %d, total bytes =%d\n",
                 xtra_module_data_ptr->num_of_bytes_injected, xtra_module_data_ptr->num_of_bytes_received, 0);

   memset (cmd_ptr, 0, sizeof (loc_cmd_s_type));

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);

   if ((xtra_module_data_ptr->xtra_cmd_is_in_progress == TRUE) &&
       (xtra_module_data_ptr->xtra_cmd_in_progress == PDSM_XTRA_CMD_SET_DATA))
   {
      if (xtra_module_data_ptr->num_of_bytes_injected < xtra_module_data_ptr->num_of_bytes_received)
      {
         cmd_ptr->client_handle = xtra_module_data_ptr->active_client_handle;
         cmd_ptr->cmd_type = LOC_CMD_TYPE_INJECT_XTRA_DATA;
         cmd_ptr->ptr_data = NULL;
         // no resource to reclaim if this operation failed
         (void) loc_middleware_queue_cmd (cmd_ptr);
      }
      else
      {
         // All parts have been injected successfully
         loc_xtra_send_reserved_xtra_ind( xtra_module_data_ptr->active_client_handle,
                                          LOC_API_SUCCESS,
                                          xtra_module_data_ptr->num_of_parts_received );
         // Release the XTRA buffer in LOC_MW allocated to the XTRA file .It is allocated when it is needed. 
         LOC_MSG_HIGH ("loc_xtra_queue_data_inject_request, releasing XTRA buffer in LOC_MW: %p ",
                        xtra_module_data_ptr->xtra_data_ptr,
                        0,
                        0);
         os_MemFree((void**)&xtra_module_data_ptr->xtra_data_ptr);
         xtra_module_data_ptr->xtra_data_ptr = NULL;
         // Reset the state since xtra injection is now completed
         loc_xtra_reset_module ();
      }
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);

}

// Process requst for xtra data validity query through loc_ioctl with
// LOC_IOCTL_INJECT_UTC_TIME.
int loc_xtra_process_time_inject
(
      loc_client_handle_type             client_handle,
      const loc_assist_data_time_s_type* time_info_ptr
)
{
   loc_xtra_module_data_s_type* xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);
   int                          ret_val = LOC_API_GENERAL_FAILURE;
   pdsm_xtra_time_info_type     pdapi_time_info;
   uint64                       temp;


   // UTC time conversion
   temp = DELTA_UTC_TO_GPS_SECONDS;
   temp *= 1000;
   pdapi_time_info.d_TimeMsec     = time_info_ptr->time_utc - temp;
   pdapi_time_info.f_TimeUncMsec  = time_info_ptr->uncertainty;
   pdapi_time_info.b_RefToUtcTime = TRUE;
   pdapi_time_info.b_ForceFlag    = FALSE;

   LOC_MSG_HIGH ("loc_xtra_process_time_inject, msec = %d, unc msec = %d, \n",
                 pdapi_time_info.d_TimeMsec, pdapi_time_info.f_TimeUncMsec, 0);
   os_TimerStart( xtra_module_data_ptr->xtra_process_timer, XTRA_OPER_DEFAULT_TIMEOUT, 0 );

   if (TRUE == pdsm_xtra_inject_time_info (loc_xtra_cmd_cb,
                                           loc_middleware_data.xtra_client_id,
                                           (void *) xtra_module_data_ptr->active_client_handle,
                                           &pdapi_time_info))
   {
      ret_val = LOC_API_SUCCESS;
   }
   else
   {
      ret_val = LOC_API_GENERAL_FAILURE;
   }

   return ret_val;
}

// Process requst for xtra data validity query through loc_ioctl with
// LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_VALIDITY.
static int loc_xtra_process_data_validity_query (void)
{
   loc_xtra_module_data_s_type* xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);
   int ret_val;

   LOC_MSG_HIGH ("loc_xtra_process_data_validity_query", 0, 0, 0);
   os_TimerStart( xtra_module_data_ptr->xtra_process_timer, XTRA_OPER_DEFAULT_TIMEOUT, 0 );

   if (TRUE == pdsm_xtra_query_data_validity (loc_xtra_cmd_cb,
                                              loc_middleware_data.xtra_client_id,
                                              (void *)xtra_module_data_ptr->active_client_handle))
   {
      ret_val = LOC_API_SUCCESS;
   }
   else
   {
      ret_val = LOC_API_GENERAL_FAILURE;
   }

   return ret_val;
}


// Function to process loc_ioctl with LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_SOURCE for XTRA file download
static int loc_xtra_process_xtra_download_info_query (void)
{
   loc_xtra_module_data_s_type* xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);
   int ret_val;

   LOC_MSG_MED ("loc_xtra_process_xtra_download_info_query called", 0, 0, 0);
   os_TimerStart( xtra_module_data_ptr->xtra_process_timer, XTRA_OPER_DEFAULT_TIMEOUT, 0 );

   if (TRUE == pdsm_xtra_client_initiate_download_request (loc_xtra_cmd_cb,
                                                           loc_middleware_data.xtra_client_id,
                                                           (void *)xtra_module_data_ptr->active_client_handle))
   {
      ret_val = LOC_API_SUCCESS;
   }
   else
   {
      ret_val = LOC_API_GENERAL_FAILURE;
   }

   return ret_val;
}

// Function to process loc_ioctl with LOC_IOCTL_SET_PREDICTED_ORBITS_DATA_AUTO_DOWNLOAD.
static int loc_xtra_process_auto_download_config (const loc_predicted_orbits_auto_download_config_s_type*  auto_download_info_ptr)
{
   loc_xtra_module_data_s_type* xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);
   int     ret_val;
   uint16  auto_check_period;

   LOC_MSG_HIGH ("loc_xtra_process_auto_download_config enable=%d, interval = %d\n",
                 auto_download_info_ptr->enable, auto_download_info_ptr->auto_check_every_hrs, 0);
   os_TimerStart( xtra_module_data_ptr->xtra_process_timer, XTRA_OPER_DEFAULT_TIMEOUT, 0 );

   /*
    *  The maximum value of the 5th parameter of the pdsm_xtra_set_auto_download_params()
    *  function is 168 (ie. the number of hours in one week).  Limit the value here.
   */
   auto_check_period = (uint16) auto_download_info_ptr->auto_check_every_hrs;
   if( auto_check_period > 168 )
   {
     auto_check_period = 168;
   }

   if (TRUE == pdsm_xtra_set_auto_download_params (loc_xtra_cmd_cb,
                                                   loc_middleware_data.xtra_client_id,
                                                   (void *) xtra_module_data_ptr->active_client_handle,
                                                   auto_download_info_ptr->enable,
                                                   auto_check_period))
   {
      ret_val = LOC_API_SUCCESS;
   }
   else
   {
      ret_val = LOC_API_GENERAL_FAILURE;
   }

   return ret_val;
}

// Process xtra callback regarding xtra data and time injection.
static void loc_xtra_process_status_event (pdsm_xtra_status_e_type xtra_cmd_status)
{
   loc_xtra_module_data_s_type*  xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);
   int32                         ioctl_status;

   LOC_MSG_HIGH ("loc_xtra_process_status_event, xtra_cmd_in_progress = %d, status = %d \n",
                  xtra_module_data_ptr->xtra_cmd_in_progress, xtra_cmd_status, 0);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
   if (xtra_module_data_ptr->xtra_cmd_is_in_progress != TRUE)
   {
      LOC_MSG_ERROR ("loc_xtra_process_status_event, status event received when no xtra cmd is active", 0, 0, 0);
   }
   else
   {
      // Special case for xtra file injection
      if ((xtra_module_data_ptr->xtra_cmd_in_progress == PDSM_XTRA_CMD_SET_DATA) &&
          (xtra_cmd_status == XTRA_DOWNLOAD_STATUS_SUCCESS))
      {
         loc_xtra_queue_data_inject_request ();
      }
      else
      {
         switch (xtra_cmd_status)
         {
         case XTRA_DOWNLOAD_STATUS_SUCCESS:
            ioctl_status = LOC_API_SUCCESS;
            break;
         case XTRA_DOWNLOAD_STATUS_GPS_BUSY:
            ioctl_status = LOC_API_ENGINE_BUSY;
            break;

         case XTRA_DOWNLOAD_STATUS_BAD_CRC:
         case XTRA_DOWNLOAD_STATUS_BAD_BUFFER_LENGTH:
         case XTRA_DOWNLOAD_STATUS_TOA_BAD_RANGE:
            ioctl_status = LOC_API_INVALID_PARAMETER;
            break;

         case XTRA_DOWNLOAD_STATUS_VERSION_MISMATCH:
            ioctl_status =LOC_API_XTRA_VERSION_CHECK_FAILURE;
            break;

         default:
            ioctl_status = LOC_API_GENERAL_FAILURE;
            break;
         }

         loc_client_queue_ioctl_callback (xtra_module_data_ptr->active_client_handle,
                                          xtra_module_data_ptr->xtra_ioctl_type,
                                          ioctl_status,
                                          NULL);
         loc_xtra_reset_module ();
      }
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
}

// Process the modem request to download xtra file. This can be either:
// (1) Modem requests xtra file to be downloaded
// (2) This is a response to client issue loc_ioctl to query xtra download info
static void loc_xtra_process_xtra_download_event (const pdsm_xtra_download_info_s_type* xtra_download_info_ptr)
{
   loc_cmd_s_type                            cmd_data;
   loc_cmd_s_type*                           cmd_ptr = &cmd_data;
   loc_xtra_module_data_s_type*              xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);
   loc_predicted_orbits_data_source_s_type*  data_download_info_ptr;
   int                                       i;
   boolean                                   result = FALSE;
   loc_cmd_nty_client_s_type                 * ptr_nty_client_data = NULL;
   loc_event_mask_type                       event_type_copy = 0;

   LOC_MSG_HIGH ("loc_xtra_process_xtra_download_event, cmd is in progress = %d, cmd in progress = %d",
                 xtra_module_data_ptr->xtra_cmd_is_in_progress, xtra_module_data_ptr->xtra_cmd_in_progress, 0);

   memset (cmd_ptr, 0, sizeof (loc_cmd_s_type));

   ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
   if(NULL != ptr_nty_client_data)
   {
       // Special case for client query XTRA download server
    
       LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
       LOC_MSG_HIGH ("loc_xtra_process_xtra_download_event, eneter critical section", 0, 0, 0);
    
       if ((xtra_module_data_ptr->xtra_cmd_is_in_progress == TRUE) &&
           (xtra_module_data_ptr->xtra_cmd_in_progress == PDSM_XTRA_CMD_INITIATE_DOWNLOAD))
       {
          cmd_ptr->cmd_type = LOC_CMD_TYPE_NOTIFY_CLIENT;
          cmd_ptr->client_handle = xtra_module_data_ptr->active_client_handle;
          cmd_ptr->ptr_data = NULL;
    
          event_type_copy = LOC_EVENT_IOCTL_REPORT;
          ptr_nty_client_data->event_type = event_type_copy;
          ptr_nty_client_data->event_data.ioctl_report.type = LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_SOURCE;
          ptr_nty_client_data->event_data.ioctl_report.status = LOC_API_SUCCESS;
          data_download_info_ptr = &(ptr_nty_client_data->event_data.ioctl_report.data.predicted_orbits_data_source);
          loc_xtra_reset_module ();
       }
       // Modem triggers xtra file injection
       else
       {
          cmd_ptr->cmd_type = LOC_CMD_TYPE_NOTIFY_CLIENT;
          cmd_ptr->client_handle = LOC_HANDLE_REGISTERED_CLIENTS;
          cmd_ptr->ptr_data = NULL;

          event_type_copy = LOC_EVENT_ASSISTANCE_DATA_REQUEST;
          ptr_nty_client_data->event_type = event_type_copy;
          ptr_nty_client_data->event_data.assist_data_request.event = LOC_ASSIST_DATA_PREDICTED_ORBITS_REQ;
          data_download_info_ptr = &(ptr_nty_client_data->event_data.assist_data_request.payload.data_download);
       }
    
       data_download_info_ptr->max_file_size = xtra_download_info_ptr->maxFileSize;
       data_download_info_ptr->max_part_size = xtra_download_info_ptr->maxFilePartSize;
    
       for (i = 0; i < LOC_API_MAX_NUM_PREDICTED_ORBITS_SERVERS; i++)
       {
          memset (xtra_module_data_ptr->xtra_servers[i], 0, LOC_XTRA_SERVER_LENGTH);
       }
    
       memscpy (xtra_module_data_ptr->xtra_servers[0],
               LOC_XTRA_SERVER_LENGTH-1,
               xtra_download_info_ptr->xtra_server_primary,
               LOC_XTRA_SERVER_LENGTH-1);
    
       memscpy (xtra_module_data_ptr->xtra_servers[1] ,
               LOC_XTRA_SERVER_LENGTH-1,
               xtra_download_info_ptr->xtra_server_secondary,
               LOC_XTRA_SERVER_LENGTH-1);
    
       memscpy (xtra_module_data_ptr->xtra_servers[2] ,
               LOC_XTRA_SERVER_LENGTH-1,
               xtra_download_info_ptr->xtra_server_tertiary,
               LOC_XTRA_SERVER_LENGTH-1);
    
       for (i = 0; i < LOC_API_MAX_NUM_PREDICTED_ORBITS_SERVERS; i++)
       {
          data_download_info_ptr->servers[i] = xtra_module_data_ptr->xtra_servers[i];
       }

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
       }
    
       LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
    
       LOC_MSG_HIGH ("loc_xtra_process_xtra_download_event, queue cmd = %d",
                     event_type_copy, 0, 0);
   }
    else
    {
        // memory allocation error
        result = FALSE;
    }
    
    if(TRUE != result)
    {
        // TODO: what else can we do?
        LOC_MSG_ERROR ("loc_xtra_process_xtra_download_event, processing failed", 0, 0, 0);
    }
}

// Process the event in response to loc_ioctl for query xtra data validity.
// If the XTRA data is not present, then the start_utc_time and valid_duration_hours will all be 0.
static void loc_xtra_process_data_validity_event(const pdsm_xtra_download_info_s_type* xtra_download_info_ptr)
{
   loc_cmd_s_type                                     cmd_data;
   loc_cmd_s_type*                                    cmd_ptr = &cmd_data;
   loc_xtra_module_data_s_type*                       xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);
   loc_predicted_orbits_data_validity_report_s_type*  data_validity_info_ptr;
   boolean                                            result = FALSE;
   loc_cmd_nty_client_s_type                          *ptr_nty_client_data = NULL;

   LOC_MSG_HIGH ("loc_xtra_process_data_validity_event start_gps_week = %d, start_gps_minute = %d, valid_duration = %d",
                 xtra_download_info_ptr->startGpsWeek,
                 xtra_download_info_ptr->startGpsMinutes,
                 xtra_download_info_ptr->validDurationHours);

   memset (cmd_ptr, 0, sizeof (loc_cmd_s_type));

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
   // Special case for client query XTRA download server
   if ((xtra_module_data_ptr->xtra_cmd_is_in_progress == TRUE) &&
       (xtra_module_data_ptr->xtra_cmd_in_progress == PDSM_XTRA_CMD_QUERY_DATA_VALIDITY))
   {
      result = FALSE;
      cmd_ptr->cmd_type = LOC_CMD_TYPE_NOTIFY_CLIENT;
      cmd_ptr->client_handle = xtra_module_data_ptr->active_client_handle;
      cmd_ptr->ptr_data = NULL;
      ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
      if(NULL != ptr_nty_client_data)
      {
          ptr_nty_client_data->event_type = LOC_EVENT_IOCTL_REPORT;
          ptr_nty_client_data->event_data.ioctl_report.type = LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_VALIDITY;
          ptr_nty_client_data->event_data.ioctl_report.status = LOC_API_SUCCESS;
          data_validity_info_ptr = &(ptr_nty_client_data->event_data.ioctl_report.data.predicted_orbits_data_validity);
          data_validity_info_ptr->start_time_utc = DELTA_UTC_TO_GPS_SECONDS + xtra_download_info_ptr->startGpsWeek * 7 * 24 * 60 * 60 +
                                                   xtra_download_info_ptr->startGpsMinutes * 60;
          data_validity_info_ptr->valid_duration_hrs = xtra_download_info_ptr->validDurationHours;

          cmd_ptr->ptr_data = (void *)ptr_nty_client_data;
          if (TRUE != loc_middleware_queue_cmd (cmd_ptr))
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
          LOC_MSG_ERROR ("loc_xtra_process_data_validity_event, failed send IPC", 0, 0, 0);
      }

      loc_xtra_reset_module ();
   }
   else
   {

      LOC_MSG_ERROR ("loc_xtra_process_data_validity_event should not be received, cmd_is_in_progress = %d, cmd_in_progress = %d",
                     xtra_module_data_ptr->xtra_cmd_is_in_progress, xtra_module_data_ptr->xtra_cmd_in_progress, 0);
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
}

// Process time download event. Modem triggers this event when it needs time to be injected to the location
// engine. This event will be formulated as LOC_EVENT_ASSISTANCE_DATA_REQUEST and pass it to the loc client.
static void loc_xtra_process_time_download_event (const pdsm_xtra_time_info_req_s_type*  xtra_time_info_ptr)
{
   loc_cmd_s_type                    cmd_data;
   loc_cmd_s_type*                   cmd_ptr = &cmd_data;
   loc_time_download_source_s_type*  time_download_info_ptr;
   loc_xtra_module_data_s_type*      xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);
   int                               i;
   boolean                           result = FALSE;
   loc_cmd_nty_client_s_type         *ptr_nty_client_data = NULL;

   LOC_MSG_HIGH ("loc_xtra_process_time_download_event, server[0] = %s \n",
                 xtra_time_info_ptr->xtra_server_primary, 0, 0);

   memset (cmd_ptr, 0, sizeof (loc_cmd_s_type));
   cmd_ptr->cmd_type = LOC_CMD_TYPE_NOTIFY_CLIENT;
   cmd_ptr->client_handle = LOC_HANDLE_REGISTERED_CLIENTS;
   cmd_ptr->ptr_data = NULL;
   ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
   if(NULL != ptr_nty_client_data)
   {
       ptr_nty_client_data->event_type = LOC_EVENT_ASSISTANCE_DATA_REQUEST;
       ptr_nty_client_data->event_data.assist_data_request.event = LOC_ASSIST_DATA_TIME_REQ;
    
       time_download_info_ptr = &(ptr_nty_client_data->event_data.assist_data_request.payload.time_download);
       time_download_info_ptr->delay_threshold = xtra_time_info_ptr->oneway_delay_failover_thresh;
    
       for (i = 0; i < LOC_API_MAX_NUM_PREDICTED_ORBITS_SERVERS; i++)
       {
          memset (xtra_module_data_ptr->time_servers[i], 0, LOC_XTRA_SERVER_LENGTH);
       }
    
       memscpy (xtra_module_data_ptr->time_servers[0],
               LOC_XTRA_SERVER_LENGTH-1,
               xtra_time_info_ptr->xtra_server_primary,
               LOC_XTRA_SERVER_LENGTH-1);
    
       memscpy (xtra_module_data_ptr->time_servers[1],
               LOC_XTRA_SERVER_LENGTH-1,
               xtra_time_info_ptr->xtra_server_secondary,
               LOC_XTRA_SERVER_LENGTH-1);
    
       memscpy (xtra_module_data_ptr->time_servers[2],
               LOC_XTRA_SERVER_LENGTH-1,
               xtra_time_info_ptr->xtra_server_tertiary,
               LOC_XTRA_SERVER_LENGTH-1);
    
       for (i = 0; i < LOC_API_MAX_NUM_PREDICTED_ORBITS_SERVERS; i++)
       {
          time_download_info_ptr->servers[i] = xtra_module_data_ptr->time_servers[i];
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
       LOC_MSG_ERROR ("loc_xtra_process_time_download_event, failed", 0, 0, 0);
   }
}

// XTRA event callback.
static void loc_xtra_event_cb (void                        *user_data,
                               pdsm_xtra_event_type         xtra_event,
                               const pdsm_xtra_info_s_type *xtra_info_ptr)
{
   LOC_MSG_HIGH ("loc_xtra_event_cb, event = %d flags = %d status = %d \n",
                 xtra_event, xtra_info_ptr->xtra_union.xtra_download_info.flags,
                 xtra_info_ptr->xtra_union.xtra_download_info.status);

   if (PDSM_XTRA_EVENT_STATUS == xtra_event)
   {
      if (((xtra_info_ptr->xtra_union.xtra_download_info.flags & XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_WEEK) &&
           (xtra_info_ptr->xtra_union.xtra_download_info.flags & XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_MINS) &&
           (xtra_info_ptr->xtra_union.xtra_download_info.flags & XTRA_DOWNLOAD_INFO_FLAGS_VALID_DURATION))!= 0)
      {
         loc_xtra_process_data_validity_event (&(xtra_info_ptr->xtra_union.xtra_download_info));
      }
      else if ((xtra_info_ptr->xtra_union.xtra_download_info.flags & XTRA_DOWNLOAD_INFO_FLAGS_STATUS) != 0)
      {
         loc_xtra_process_status_event (xtra_info_ptr->xtra_union.xtra_download_info.status);
      }
   }
   else if (PDSM_XTRA_EVENT_DOWNLOAD_REQ == xtra_event)
   {
      loc_xtra_process_xtra_download_event (&(xtra_info_ptr->xtra_union.xtra_download_info));
   }
   else if (PDSM_XTRA_EVENT_TIME_REQ == xtra_event)
   {
      loc_xtra_process_time_download_event (&(xtra_info_ptr->xtra_union.xtra_time_info));
   }
   else
   {
      LOC_MSG_ERROR ("loc_xtra_event_cb = %d not expected\n", xtra_event, 0, 0);
   }
}

// XTRA cmd callback. If xtra cmd callback reports error, no further processing of xtra
// cmd will be performed.
static void loc_xtra_cmd_cb (void                     *user_data,
                             pdsm_xtra_cmd_e_type      xtra_cmd_cb,
                             pdsm_xtra_cmd_err_e_type  xtra_cmd_err)
{
   loc_xtra_module_data_s_type* xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);
   pdsm_xtra_cmd_e_type         xtra_cmd_expected = xtra_module_data_ptr->xtra_cmd_in_progress;
   int32                        ioctl_status;

   LOC_MSG_HIGH ("loc_xtra_cmd_cb: cmd=%d, err=%d, expected cmd=%d\n",
                 xtra_cmd_cb, xtra_cmd_err, xtra_cmd_expected);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
   if ((xtra_module_data_ptr->xtra_cmd_is_in_progress == FALSE) ||
       (xtra_cmd_expected != xtra_cmd_cb) ||
       (xtra_module_data_ptr->active_client_handle != (loc_client_handle_type) user_data))
   {
      LOC_MSG_ERROR ("loc_xtra_cmd_cb unexpected, xtra_in_progress = %d, active client = %d, user_data = %d\n",
                     xtra_module_data_ptr->xtra_cmd_is_in_progress, xtra_module_data_ptr->active_client_handle, user_data);
   }
   else
   {
      if (xtra_cmd_err != PDSM_XTRA_CMD_ERR_NOERR)
      {
         switch (xtra_cmd_err)
         {
         case PDSM_XTRA_CMD_ERR_GPS_SUBSYSTEM_BUSY:
            ioctl_status = LOC_API_ENGINE_BUSY;
            break;

         case PDSM_XTRA_CMD_ERR_BAD_CRC:
         case PDSM_XTRA_CMD_ERR_TIME_OF_VALIDITY_OUT_OF_RANGE:
         case PDSM_XTRA_CMD_ERR_DOWNLOAD_INTRVL_OUT_OF_RANGE:
         case PDSM_XTRA_CMD_ERR_TIME_INFO_ERROR:
            ioctl_status = LOC_API_INVALID_PARAMETER;
            break;

         default:
            ioctl_status = LOC_API_GENERAL_FAILURE;
            break;
         }

         loc_client_queue_ioctl_callback (xtra_module_data_ptr->active_client_handle,
                                          xtra_module_data_ptr->xtra_ioctl_type,
                                          ioctl_status,
                                          NULL);

         // Reset the state
         loc_xtra_reset_module ();
      }
      else
      {
         LOC_MSG_ERROR ("loc_xra_cmd_cb success, wait for event cb", 0, 0, 0);
      }
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
}

// When the xtra timer is fired, it means that we have not received response from GPS engine
// notify the client about the timeout value
void loc_xtra_process_timer_cb ( void )
{
   loc_xtra_module_data_s_type *xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
   if (xtra_module_data_ptr->xtra_cmd_is_in_progress == TRUE)
   {
      loc_client_handle_type current_xtra_client = xtra_module_data_ptr->active_client_handle;

      // Reset the xtra module data
      xtra_module_data_ptr->xtra_cmd_is_in_progress = FALSE;
      xtra_module_data_ptr->xtra_cmd_in_progress    = PDSM_XTRA_CMD_NONE;
      xtra_module_data_ptr->active_client_handle    = LOC_CLIENT_HANDLE_INVALID;

      xtra_module_data_ptr->total_parts_to_receive = 0;
      xtra_module_data_ptr->total_bytes_to_receive = 0;
      xtra_module_data_ptr->num_of_parts_received = 0;
      xtra_module_data_ptr->num_of_bytes_received = 0;
      xtra_module_data_ptr->num_of_bytes_received = 0;
      xtra_module_data_ptr->total_parts_to_inject = 0;

      // Notify client about the timeout
      loc_client_queue_ioctl_callback (current_xtra_client,
                                       xtra_module_data_ptr->xtra_ioctl_type,
                                       LOC_API_TIMEOUT,
                                       NULL);
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
}

// Reset the xtra module. This function is called when the in-progress xtra cmds finishes.
static void loc_xtra_reset_module (void)
{
   loc_xtra_module_data_s_type *xtra_module_data_ptr = &(loc_middleware_data.xtra_module_data);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);

   if (xtra_module_data_ptr->xtra_cmd_is_in_progress == TRUE)
   {
      // Clear the timer
      os_TimerStop( xtra_module_data_ptr->xtra_process_timer );

      xtra_module_data_ptr->xtra_cmd_is_in_progress = FALSE;
      xtra_module_data_ptr->xtra_cmd_in_progress    = PDSM_XTRA_CMD_NONE;
      xtra_module_data_ptr->active_client_handle    = LOC_CLIENT_HANDLE_INVALID;

      xtra_module_data_ptr->total_parts_to_receive = 0;
      xtra_module_data_ptr->total_bytes_to_receive = 0;
      xtra_module_data_ptr->num_of_parts_received = 0;
      xtra_module_data_ptr->num_of_bytes_received = 0;

      xtra_module_data_ptr->total_parts_to_inject   = 0;
      xtra_module_data_ptr->num_of_parts_injected   = 0;
      xtra_module_data_ptr->num_of_bytes_injected   = 0;
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);

   LOC_MSG_MED ("loc_xtra_reset_module called", 0, 0, 0);
}

/* Function to send out XTRA callbacks in response to inject-requests,
   This function would use the reserved field to send out the information
   as we need to convey the part num */
static void loc_xtra_send_reserved_xtra_ind 
(
  loc_client_handle_type              client_handle,
  int32 status, 
  uint16 part_num 
)
{

  loc_ioctl_callback_data_u_type   z_LocIoctlCbData;

  
  z_LocIoctlCbData.w_PredOrbitsPartNum = part_num;
  loc_client_queue_ioctl_callback (client_handle,
                                   LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA,
                                   status,
                                   &z_LocIoctlCbData );
}


