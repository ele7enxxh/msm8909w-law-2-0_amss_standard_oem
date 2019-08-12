#ifndef RFM_RESOURCES_H
#define RFM_RESOURCES_H


/*
   @file
   rfm_resources.h

   @brief
   RF Driver's interface for resources request from other clients.

   @details

*/

/*===========================================================================
Copyright (c) 2002 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfm_resources.h#1 $
$DateTime: 2016/12/13 07:59:52 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/2/13    ra      Move CCS memory collapse to modem specific component 
10/01/13   ra      Added support for CCS memory collapse 
07/07/11   aak     Changes to rfm_resource_info 
05/23/11   aak     Changes to rfm_resource_info
05/17/11   aak     Including rfm_mode_types.h and rfm_device_types.h instead of rfm.h
04/22/11   aak     Initial revision

==========================================================================*/
#include "rfm_mode_types.h"
#include "rfm_device_types.h"
#include "npa.h"


typedef struct
{
  rfm_mode_enum_type    mode;
  rfm_path_state        path_state;
  rfm_device_enum_type  device_num;
} rfm_tech_info;

typedef struct
{
  rfm_tech_info       tech_A;
  rfm_tech_info       tech_B;
  uint8               npa_id;
  npa_client_handle   npa_handle;
} rfm_resource_info;


void rfm_get_awake_resource_request(rfm_resource_info *params);

#endif  /* RFM_RESOURCES_H */
