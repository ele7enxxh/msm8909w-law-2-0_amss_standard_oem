/******************************************************************************
@file:  loc_ni.h
@brief: Location Middleware NI-module

DESCRIPTION
This module defines the loc API routines for a client to handle NI

INITIALIZATION AND SEQUENCING REQUIREMENTS

-----------------------------------------------------------------------------
  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
-----------------------------------------------------------------------------
******************************************************************************/

/*=====================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
04/27/11   spn      Updated for Loc-Api 2.0

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_ni.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $
======================================================================*/

#ifndef LOC_NI_H
#define LOC_NI_H

#include "loc_api_2.h"
#include "pdapi.h"
#include "loc_task.h"

extern boolean loc_ni_init (void);
extern int loc_ni_process_ioctl (loc_client_handle_type  client_handle,
                                 loc_ioctl_e_type ioctl_type,
                                 const loc_ioctl_data_u_type* ioctl_data);

typedef struct 
{
   // Cache last received lcs event
   pdsm_lcs_event_type  lcs_event;

   // Cache last received lcs info
   pdsm_lcs_info_u_type   pdsm_lcs_info;

} loc_ni_module_data_s_type;

extern int loc_ni_process_cmd_request (loc_cmd_s_type * const ptr_cmd);
#endif // LOC_NI_H
