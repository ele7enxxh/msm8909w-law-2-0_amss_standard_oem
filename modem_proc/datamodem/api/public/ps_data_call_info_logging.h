/**
   @file ps_data_call_info_logging.h
   @brief This file defines the API used for generating DIAG events related 
   to data call status 
*/

#ifndef PS_DATA_CALL_INFO_LOGGING_H
#define PS_DATA_CALL_INFO_LOGGING_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              P S _ D A T A _ C A L L _ I N F O _ L O G G I N G . H
 
GENERAL DESCRIPTION
  This file defines the API used for generating DIAG events related 
  to data call status

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_data_call_info_logging.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/09/13    svj    Initial version
===========================================================================*/


/*=============================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "event_defs.h"
#include "ps_iface.h"
/*===========================================================================

                      EXTERNAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Call status enum type
---------------------------------------------------------------------------*/
typedef enum 
{
  PS_DATA_CALL_INFO_LOGGING_STATUS_UNKNOWN        = 0x00, 
  /* This value is currently not supported and reserved
     for DIAG purposes only */
  PS_DATA_CALL_INFO_LOGGING_STATUS_ACTIVATED      = 0x01,
  PS_DATA_CALL_INFO_LOGGING_STATUS_TERMINATED     = 0x02,
  PS_DATA_CALL_INFO_LOGGING_STATUS_ACTIVATING     = 0x03
} ps_data_call_info_logging_status_type;

typedef enum 
{
  PS_DATA_CALL_INFO_LOGGING_MODEM_EMBEDDED         = 0x00,
  PS_DATA_CALL_INFO_LOGGING_DUN                    = 0X01,
  PS_DATA_CALL_INFO_LOGGING_EMBEDDED_RMNET         = 0x02,
  PS_DATA_CALL_INFO_LOGGING_TETHERED_RMNET         = 0x03,
  PS_DATA_CALL_INFO_LOGGING_ALL_CALLS              = 0x7FFFFFFD
} ps_data_call_info_logging_call_type;

typedef enum 
{
  PS_DATA_CALL_INFO_LOGGING_PROC_ID_MODEM_EMBEDDED = 0x00,
  PS_DATA_CALL_INFO_LOGGING_PROC_ID_TE_DUN         = 0X01,
  PS_DATA_CALL_INFO_LOGGING_PROC_ID_APPS1          = 0x02,
  PS_DATA_CALL_INFO_LOGGING_PROC_ID_TE_LAPTOP1     = 0x03
} ps_data_proc_id_type;

#define  PS_DATA_CALL_INFO_LOGGING_APP_ID_MODEM_EMBEDDED 0x0000000000000000ULL
#define  PS_DATA_CALL_INFO_LOGGING_APP_ID_TE_DUN         0x0000000100000000ULL
#define  PS_DATA_CALL_INFO_LOGGING_APP_ID_APPS           0x0000000200000000ULL
#define  PS_DATA_CALL_INFO_LOGGING_APP_ID_TE_RMNET       0x0000000300000000ULL

/**
  @brief   This function generates EVENT_DS_CALL_STATUS over 
           DIAG and provides call status information.

  @param[in] this_iface_ptr     ptr to interface control block 
                                on which to operate on.
  @param[in] call_state         Identifies current state of the 
                                data call.
  @param[in] call_state         Identifies the data call type. 
   
  @return  0                    Event logging Success.
  @return  -1                   Event logging failed.
  
*/
int ps_data_call_info_logging_event_report
(
  ps_iface_type                   *this_iface_ptr,
  uint32                           call_state,
  uint32                           call_type
);

#endif /* PS_DATA_CALL_INFO_LOGGING_H */

