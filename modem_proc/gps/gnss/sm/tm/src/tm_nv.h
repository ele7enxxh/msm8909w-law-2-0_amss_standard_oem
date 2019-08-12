#ifndef TM_NV_H
#define TM_NV_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM - NV Interface

General Description
  This file contains implementations for TM NV Interface
  
  Copyright (c) 2002 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_nv.h#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/14   skm    NV Async Read changes 
12/26/08   kgj     Original - Taken from tm_pdapi_iface.c
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */
#include "sm_nv.h"

#ifdef FEATURE_TM_SECURITY
#include "tm_security.h"
#endif /* FEATURE_TM_SECURITY */


/*===========================================================================

FUNCTION tm_core_nv_write

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_nv_write
(
  nv_items_enum_type nv_item,
  void* item_ptr
);

/*===========================================================================

FUNCTION tm_core_nv_async_read

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_nv_async_read
(
  nv_items_enum_type nv_item,
  void* item_ptr
);

#endif /* TM_NV_H */
