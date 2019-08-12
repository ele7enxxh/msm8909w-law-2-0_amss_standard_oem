/*!
   @file rfm_resources.c

   @brief
   This file implemnets the RF Driver interface to the NPA resources with other clients.

   @details
   
*/

/*===========================================================================
Copyright (c) 1999 - 2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2006 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/src/rfm_resources.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/23/11   aak     Adding functionality to rfm_get_awake_resource_request()
04/28/11   aak     Initial version of the file

============================================================================*/
/*============================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfm_resources.h"
#include "rfc_card.h"
#include "msg.h"

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

void rfm_get_awake_resource_request(rfm_resource_info *params)
{
  //MCPM will send the state of tech A and tech B
  // RFSW needs to find out the NPA id and send back to MCPM

  rfc_get_awake_resource_request(params);

  MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"Exiting rfm_get_awake_resource_request() npa_id=%d, npa_handle=%d", params->npa_id, params->npa_handle);
}
