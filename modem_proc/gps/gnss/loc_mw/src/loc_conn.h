/******************************************************************************
@file:  loc_conn.h
@brief: Location Middleware Connection Module

DESCRIPTION
This module defines the loc API routines for a client that connects to 
loc-mw

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_conn.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $
======================================================================*/


#ifndef LOC_CONN_H
#define LOC_CONN_H

#include "loc_api_2.h"
#include "pdsm_atl.h"

typedef struct
{
   // Information being cached from PDSM_ATL request 
   pdsm_atl_session_handle_type  session_handle;
   pdsm_atl_open_params_type     open_param;
} loc_conn_module_data_s_type;

extern boolean loc_conn_init (void);
extern int loc_conn_process_ioctl (loc_client_handle_type  client_handle,
                                   loc_ioctl_e_type ioctl_type,
                                   const loc_ioctl_data_u_type* ioctl_data);
#endif // LOC_CONN_H
