/******************************************************************************
  @file    ps_data_call_info_logging.c
  @brief   

  DESCRIPTION
  This file implements the API used for generating DIAG events related to
  data call status.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_data_call_info_logging.c#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/11/13    svj     Initial development.

===========================================================================*/
/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "msg.h"
#include "ds_Utils_DebugMsg.h"

#include "ps_data_call_info_logging.h"
#include "ps_iface.h" 
#include "ps_ifacei_utils.h"
#include "event_defs.h"
#include "event.h"           /* For logging DIAG event and event codes      */

/*===========================================================================

                           LOCAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Data type used to log call status information
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                         call_state;
  uint32                         call_type;	
  uint32                         addr_family;	
  uint32                         profile_id_3gpp;
  uint32                         profile_id_3gpp2;
  uint32                         iface_name;
  uint8                          iface_instance;   
} ps_data_call_info_logging_type;


/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
int ps_data_call_info_logging_event_report
(
  ps_iface_type                   *this_iface_ptr,
  uint32                           call_state,
  uint32                           call_type
)
{
   ps_data_call_info_logging_type ps_data_call_info_logging_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   memset(&ps_data_call_info_logging_info,
          0,
          sizeof(ps_data_call_info_logging_info));

  if( this_iface_ptr == NULL)
  {
     LOG_MSG_ERROR_0("ps_call_info_logging_event_report: "
                     "Null iface pointer" );
     return -1;
  }

  ps_data_call_info_logging_info.iface_name     = (uint32) this_iface_ptr->name;
  ps_data_call_info_logging_info.iface_instance = this_iface_ptr->instance;
  ps_data_call_info_logging_info.call_state     = call_state;
  ps_data_call_info_logging_info.call_type      = call_type;
  ps_data_call_info_logging_info.addr_family    = 
    (uint32) ps_iface_get_addr_family(this_iface_ptr);
  ps_data_call_info_logging_info.profile_id_3gpp = 
    PS_IFACEI_GET_3GPP_PROFILE_ID(this_iface_ptr);
  ps_data_call_info_logging_info.profile_id_3gpp2 = 
    PS_IFACEI_GET_3GPP2_PROFILE_ID(this_iface_ptr);

  event_report_payload (EVENT_DS_CALL_STATUS,
                        sizeof(ps_data_call_info_logging_type),
                        (void*) &ps_data_call_info_logging_info);
  return 0;

} /* ps_data_call_info_logging_event_report */
