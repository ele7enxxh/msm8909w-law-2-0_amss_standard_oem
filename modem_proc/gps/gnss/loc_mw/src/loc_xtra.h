/******************************************************************************
  @file: loc_xtra.h
  @brief:   This module handles gpsone XTRA related features .
  

  DESCRIPTION
   Qualcomm Location API XTRA Module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_xtra.h#1 $
$DateTime: 2016/12/13 07:59:45 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
04/27/11   spn      Updated for Loc-Api 2.0
======================================================================*/


#ifndef LOC_XTRA_H
#define LOC_XTRA_H


#include "loc_api_2.h"
#include "pdapi.h"
#include "loc_task.h"
#include "aries_os_api.h"

#define LOC_XTRA_SERVER_LENGTH 128

typedef struct 
{
   boolean                 xtra_cmd_is_in_progress;
   pdsm_xtra_cmd_e_type    xtra_cmd_in_progress;
   loc_client_handle_type  active_client_handle;
   loc_ioctl_e_type        xtra_ioctl_type;
   unsigned char*          xtra_data_ptr; // Memory will be allocated dynamically for the XTRA file
 
   // XTRA injection related data structure
   int                  total_parts_to_receive;
   int                  total_bytes_to_receive;
   int                  num_of_parts_received; 
   uint32               num_of_bytes_received;

   int                  total_parts_to_inject;
   int                  num_of_parts_injected;
   uint32               num_of_bytes_injected;

   os_TimerMsgType * 	xtra_process_timer;
   char                 xtra_servers[LOC_API_MAX_NUM_PREDICTED_ORBITS_SERVERS][LOC_XTRA_SERVER_LENGTH];
   char                 time_servers[LOC_API_MAX_NUM_PREDICTED_ORBITS_SERVERS][LOC_XTRA_SERVER_LENGTH];
} loc_xtra_module_data_s_type;


extern boolean loc_xtra_init (void);
extern int loc_xtra_process_ioctl (loc_client_handle_type  client_handle,
                                   loc_ioctl_e_type ioctl_type,
                                   const loc_ioctl_data_u_type* ioctl_data);


extern int loc_xtra_process_cmd_request (loc_cmd_s_type * const ptr_cmd);
extern void loc_xtra_process_timer_cb ( void );
#endif // LOC_XTRA_H
