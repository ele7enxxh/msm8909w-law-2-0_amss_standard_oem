/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      MGP NV Items Interface Header File

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for 
  the MGP Non-Volatile Memory interface.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/mgp/common/inc/mgp_nv.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $ 

=============================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/20/07    ah     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef MGP_NV_H
#define MGP_NV_H

/*
 * Include files
*/

#include "comdef.h"

#include "aries_os_api.h"
#include "aries_os_globals.h"

#include "nv.h"
#include "queue.h"

/*
 * Constant definitions
*/

/* PGI task was added after the below #define was set to 5. Increment if necessary. Currently
 * this const is not being used.
 */
#define MGP_NV_CLIENT_HANDLE_MAX      5

#define MGP_NV_CLIENT_HANDLE_ERROR    0xFF

#define NUM_OF_AAGPS_IPC_DM_THREAD_MASK_I 32

typedef enum
{
  GPSONE_ENGINE_TYPE_GEN6C, /* CDMA */
  GPSONE_ENGINE_TYPE_GEN6W, /* WCDMA */
  GPSONE_ENGINE_TYPE_GEN7,  /* Converged C2K + UMTS */
  GPSONE_ENGINE_TYPE_LAST,
} mgp_gpsone_engine_type;


/*
 * Structure definitions
*/

/* Type for MGP NV write command callback function
 *
 * A client that wants to be notified of the result of a MGP NV write
 * command needs to supply a pointer to a function of this type as the
 * only parameter to the mgp_nv_init() function.
 */
typedef void (mgp_nv_cmd_cb_f_type) (

  /* NV item that this write command status is for */
  nv_items_enum_type    item,

  /* Pointer to MGP NV module provided write data */
  void*                 data,

  /* Command status code indicating write pass/fail */
  nv_stat_enum_type     status
);

typedef union
{
  nv_item_type          nv_item;
} mgp_nv_u_type;

typedef struct
{
  q_link_type           link;
  nv_items_enum_type    item;
  mgp_nv_u_type         data;
  uint32                handle;
} mgp_nv_item_s_type;

typedef struct
{
  boolean               active;
  uint32                thread_id;
  mgp_nv_cmd_cb_f_type* nv_cb_fn;
} mgp_nv_clnt_s_type;


/*
 * Function definitions
*/

/* See CGPS_NV_Loader.pl script which will write MGP NV item default values */

/*===========================================================================

FUNCTION mgp_nv_write_reg

DESCRIPTION
  This function is used to register a client that will use this NV module.
  It will register the client's callback function after an NV Write command
  has completed, and return a client handle that is needed for future writes.
  
RETURN VALUE
  Client handle value (or 0xFF if error)

DEPENDENCIES
 
===========================================================================*/

uint32 mgp_nv_write_reg (uint32 thread_id, mgp_nv_cmd_cb_f_type nv_clnt_cb_fn);

/*===========================================================================

FUNCTION mgp_nv_write

DESCRIPTION
  This function is used to queue NV Write commands to NV. The writes to NV
  are performed in asynchronous fashion.

RETURN VALUE
  TRUE  - NV write command successfully sent to NV
  FALSE - NV write command failed to be sent to NV

DEPENDENCIES
  MGP NV write callback needs to be registered via call to mgp_nv_write_reg

===========================================================================*/

boolean mgp_nv_write (nv_items_enum_type nv_item, const void* item_ptr, uint32 handle);

/*===========================================================================

FUNCTION   mgp_nv_write_done

DESCRIPTION
  This function is used to handle NV write callback status. If there are any
  clients who issued write requests, they will be notified and any pending
  write requests will be subsequently handled.

RETURN VALUE
  TRUE  - NV write success
  FALSE - NV write failure

DEPENDENCIES
 
===========================================================================*/

boolean mgp_nv_write_done (void);

/*===========================================================================

FUNCTION mgp_nv_read

DESCRIPTION
  This function is used to perform a synchronous read from NV.

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/

boolean mgp_nv_read (nv_items_enum_type nv_item, void* item_ptr, os_ThreadIdEnumType thread_id);

/*===========================================================================

FUNCTION mgp_nv_ValidateIpcMasks

DESCRIPTION
  This function is used to enable default recommended settings for  
  IPC_NV_settings. If all masks are zero the default setting will be used

RETURN VALUE
   None
DEPENDENCIES
 
===========================================================================*/

void mgp_nv_ValidateIpcMasks(uint32 * p_NvIpcMasks);

#endif /* MGP_NV_H */
