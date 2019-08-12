#ifndef REMOTE_STORAGE_H
#define REMOTE_STORAGE_H

/*====================================================================================*//**

                      Remote FS Server Public APIs Header File


DESCRIPTION
   This is the header file for the remote FS server service that contain
   definition for the publically exposed APIs

   Copyright (c) 2009-2010 by Qualcomm Technologies Incorporated.  
   All Rights Reserved.
   Qualcomm Confidential and Proprietary

*//*=====================================================================================*/


/*=======================================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/rapi/storage/inc/remote_storage.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $ 


 YYYY-MM-DD       who     what, where, why
 --------------   ---     ----------------------------------------------------------
 2010-08-19      vin      Support of read_iovec
 2010-06-07       vj      Added force_sync and get_status support
 2010-03-04      yog      Rename file remotefs_srv_api.h with remote_storage.h
 2010-01-27      yog      Implemented the callback server on Modem.
 2009-06-09       rh      Add open by friendly name feature
 2009-03-30       rh      Initial creation
 =========================================================================================*/
/*=======================================================================================
 Include Files
========================================================================================*/

#include "comdef.h"
#include "remotefs_comm_api.h"

/*=======================================================================================

Define structures

========================================================================================*/

/*=======================================================================================

Function Prototype

========================================================================================*/

/* Remotefs Srv Events */
typedef enum
{
   REMOTEFS_SRV_EVNT_OPEN = 0,
   REMOTEFS_SRV_EVNT_CLOSE,
   REMOTEFS_SRV_EVNT_WRITE_BLOCK,
   REMOTEFS_SRV_EVNT_GET_DEV_ERROR,
   REMOTEFS_SRV_EVNT_WRITE_IOVEC,
   REMOTEFS_SRV_EVNT_SEND_USER_DATA,
   REMOTEFS_SRV_EVNT_READ_IOVEC,
   REMOTEFS_SRV_EVNT_MAX32=0x10000000
}remotefs_srv_event_e_type;

#define REMOTEFS_SRV_EVNT_MAX (REMOTEFS_SRV_EVNT_READ_IOVEC + 1)

/*============================================================================
 *    remotefs_srv_write_finish - Function called from the APPS (client) side,
 *     intimating that write is being complete and thus Modem can intimate 
 *     the upperlayer (FS) by calling its registered callback function
 *
 * PARAMETERS
 *    \param handle 
 *       Handle for the current transaction
 *    \param last_op_status 
 *       The last_op_status of the WRITE operation at Client side
 *    \param user_data
 *       User data data will be part of the argument which we have passed using 
         send_user_data api 
 *
 * RETURN VALUE
 *    \return 
 *         Status of the operation
 *===========================================================================*/
remotefs_status_type remotefs_srv_write_finish(
	void *handle, 
	remotefs_status_type last_op_status,
	uint32 user_data);
/*~ FUNCTION remotefs_srv_write_finish */

/*============================================================================
 *    remotefs_srv_open_cb_type - CallBack function for the OPEN.
 *      When there is remotefs_open call from FS on Modem side then Remote-FS
 *      would call the registered callback function for the OPEN of the Apps
 *      side.
 *
 * PARAMETERS
 *    \param open_event 
 *       Event need to be send for opening REMOTEFS_SRV_EVNT_OPEN
 *    \param path
 *       String for the location of the file system to pass to the hotplug layer
 *
 * RETURN VALUE
 *    \return 
 *       Handle for the current transaction. Return NULL if failure occured
 *===========================================================================*/
/* CallBack function for the OPEN */
typedef void* (*remotefs_srv_open_cb_type) (
   remotefs_srv_event_e_type open_event,
   char *path );
/*~ PARAM IN path STRING*/
/*~ CALLBACK remotefs_srv_open_cb_type
    ONERROR return NULL */

typedef struct {
   boolean                      init;
   remotefs_srv_open_cb_type    open_event_cb;
} remotefs_open_s_type;

/*===========================================================================
 *  remotefs_srv_register_open - Allow the client to register itself with 
 *    the API for the Remote-FS event REMOTEFS_SRV_EVNT_OPEN
 *
 *  RETURN VALUE
 *    remotefs_status_type
 *  SIDE EFFECTS
 *    Internal data initialized for the new client.
 *===========================================================================*/
/* Register function for the OPEN */
remotefs_status_type remotefs_srv_register_open
(
 remotefs_srv_event_e_type   open_event,
 remotefs_srv_open_cb_type   remotefs_srv_open_cb
);
/*~ FUNCTION remotefs_srv_register_open RELEASE_FUNC remotefs_srv_un_register_cb(open_event) 
    ONERROR return RMFS_ERROR_PIPE */

/*============================================================================
 *
 *    remotefs_srv_write_iovec_cb_type - CallBack function for the WRITE IOVEC.
 *
 * PARAMETERS
 *    \param write_iovec_event 
 *       Event need to be send for opening REMOTEFS_SRV_EVNT_WRITE_IOVEC
 *
 * RETURN VALUE
 *   
 *===========================================================================*/
/* CallBack function for the WRITE IOVEC */
typedef remotefs_status_type (*remotefs_srv_write_iovec_cb_type) (
   remotefs_srv_event_e_type  write_iovec_event,
   void *handle,
   remotefs_iovec_desc_type  *iovec_struct, 
   uint32 ent_cnt );
/*~ PARAM IN iovec_struct */
/*~ PARAM iovec_struct VARRAY LENGTH ent_cnt */
/*~ CALLBACK remotefs_srv_write_iovec_cb_type
    ONERROR return NULL */

typedef struct {
   boolean                      init;
   remotefs_srv_write_iovec_cb_type    write_iovec_event_cb;
} remotefs_write_iovec_s_type;

/*===========================================================================
 *  remotefs_srv_register_write_iovec - Allow the client to register itself
 *   with the API for the Remote-FS event REMOTEFS_SRV_EVNT_WRITE_IOVEC
 *
 *  RETURN VALUE
 *    remotefs_status_type
 *  SIDE EFFECTS
 *    Internal data initialized for the new client.
=========================================================================*/
/* Register function for the WRITE IOVEC */
remotefs_status_type remotefs_srv_register_write_iovec
(
 remotefs_srv_event_e_type   write_iovec_event,
 remotefs_srv_write_iovec_cb_type   remotefs_srv_write_iovec_cb
);
/*~ FUNCTION remotefs_srv_register_write_iovec RELEASE_FUNC remotefs_srv_un_register_cb (write_iovec_event) 
    ONERROR return RMFS_ERROR_PIPE */

/* Struct for Close args */
typedef struct remotefs_srv_close_args {
   void *handle;	
} remotefs_srv_close_args_type;

/* Struct for Write Block args */
typedef struct remotefs_srv_write_block_args {
   void *handle;
   uint32 data_addr;
   uint32 sector_data;
   uint32 counts;	
} remotefs_srv_write_block_args_type;

/* Struct for Get Dev Error args */
typedef struct remotefs_srv_get_dev_error_args {
   void *handle;
} remotefs_srv_get_dev_error_args_type;

/* Struct for Send User Data args */
typedef struct remotefs_srv_send_user_data_args {
   void *handle;
   uint32 user_data;
} remotefs_srv_send_user_data_args_type;

typedef union remotefs_srv_evnt_info_u
{
   remotefs_srv_close_args_type  remotefs_srv_close;
   /* REMOTEFS_SRV_EVNT_CLOSE */
   /*~ CASE REMOTEFS_SRV_EVNT_CLOSE remotefs_srv_evnt_info_u.remotefs_srv_close */

   remotefs_srv_write_block_args_type  remotefs_srv_write_block;
   /* REMOTEFS_SRV_EVNT_WRITE_BLOCK */
   /*~ CASE REMOTEFS_SRV_EVNT_WRITE_BLOCK remotefs_srv_evnt_info_u.remotefs_srv_write_block */	

   remotefs_srv_get_dev_error_args_type  remotefs_srv_get_dev_error;
   /* REMOTEFS_SRV_EVNT_GET_DEV_ERROR */
   /*~ CASE REMOTEFS_SRV_EVNT_GET_DEV_ERROR remotefs_srv_evnt_info_u.remotefs_srv_get_dev_error */
   
   remotefs_srv_send_user_data_args_type remotefs_srv_send_user_data;
   /* REMOTEFS_SRV_EVNT_SEND_USER_DATA */
   /*~ CASE REMOTEFS_SRV_EVNT_SEND_USER_DATA remotefs_srv_evnt_info_u.remotefs_srv_send_user_data */	
}remotefs_srv_cfg_event_info_u_type;

/*============================================================================
 * remotefs_srv_event_cb_type - CallBack function for the Remote-FS events
 *        REMOTEFS_SRV_EVNT_CLOSE
 *        REMOTEFS_SRV_EVNT_WRITE_BLOCK
 *        REMOTEFS_SRV_EVNT_GET_DEV_ERROR
 *        REMOTEFS_SRV_EVNT_SEND_USER_DATA
 *       When there is remotefs_xxx call from FS on Modem side then Remote-FS
 *       would call the registered callback function for the called Api's
 *
 * PARAMETERS
 *    \param event 
 *       Event need to be send.
 *    \param info_ptr
 *       Instance of the Union remotefs_srv_cfg_event_info_u_type.
 *
 * RETURN VALUE
 *    \return 
 *       Return status of the transaction.
 *===========================================================================*/
/* CallBack function for all EVENT's except OPEN*/
typedef remotefs_status_type (*remotefs_srv_event_cb_type) (
   remotefs_srv_event_e_type              event,
   remotefs_srv_cfg_event_info_u_type    *info_ptr
);
/*~ PARAM IN info_ptr POINTER DISC event */
/*~ CALLBACK remotefs_srv_event_cb_type 
    ONERROR return RMFS_ERROR_PIPE */

typedef struct {
   boolean                      initialized;
   remotefs_srv_event_cb_type   cfg_event_cb;
} remotefs_srv_s_type;

/*===========================================================================
 *  remotefs_srv_register_cb - Allow the client to register itself with
 *    the API for the Remote-FS events
 *     REMOTEFS_SRV_EVNT_CLOSE
 *     REMOTEFS_SRV_EVNT_WRITE_BLOCK
 *     REMOTEFS_SRV_EVNT_GET_DEV_ERROR
 *     REMOTEFS_SRV_EVNT_SEND_USER_DATA
 *
 *  RETURN VALUE
 *    remotefs_status_type
 *  SIDE EFFECTS
 *    Internal data initialized for the new client.
=========================================================================*/
/* Register function for all EVENT's except OPEN*/
remotefs_status_type remotefs_srv_register_cb
(
 remotefs_srv_event_e_type   remotefs_srv_event_e,
 remotefs_srv_event_cb_type  remotefs_srv_event_cb
);
/*~ FUNCTION remotefs_srv_register_cb RELEASE_FUNC remotefs_srv_un_register_cb(remotefs_srv_event_e)
    ONERROR return RMFS_ERROR_PIPE */

/*===========================================================================
 * remotefs_srv_un_register_cb - Allow the client to release itself
 *  from the API for all events.
 *
 *  RETURN VALUE
 *    remotefs_status_type
 *  SIDE EFFECTS
 *    Internal data reset for this client.
=========================================================================*/
/* Un-Register function for all EVENT's */
remotefs_status_type remotefs_srv_un_register_cb
(
 remotefs_srv_event_e_type remotefs_srv_event_e
);
/*~ FUNCTION remotefs_srv_un_register_cb
    ONERROR return RMFS_ERROR_PIPE */

/*===========================================================================
 * remotefs_srv_force_sync - Allow the client to do a force sync. The API
 *                           calls efs_sync_no_wait and return a token
 *
 *  RETURN VALUE
 *    Sync token on success, REMOTEFS_FAILURE on failure
=========================================================================*/
int remotefs_srv_force_sync ( void );
/*~ FUNCTION remotefs_srv_force_sync
    ONERROR return REMOTEFS_FAILURE */

/*===========================================================================
 * remotefs_srv_get_sync_status - Allow the client to get the status of the
 *                                force sync operation of the sync token
 * PARAMETERS
 *    \param sync_token 
 *       Sync token for which the status is to fetched.
 *  RETURN VALUE
 *    Sync status on success, REMOTEFS_FAILURE on failure
=========================================================================*/
int remotefs_srv_get_sync_status ( int sync_token );
/*~ FUNCTION remotefs_srv_get_sync_status
    ONERROR return REMOTEFS_FAILURE */

/* CallBack function for the READ IOVEC */
typedef remotefs_status_type (*remotefs_srv_read_iovec_cb_type) (
   remotefs_srv_event_e_type  read_iovec_event,
   void *handle,
   remotefs_iovec_desc_type  *iovec_struct, 
   uint32 ent_cnt );
/*~ PARAM IN iovec_struct */
/*~ PARAM iovec_struct VARRAY LENGTH ent_cnt */
/*~ CALLBACK remotefs_srv_read_iovec_cb_type
    ONERROR return NULL */

typedef struct {
   boolean                      init;
   remotefs_srv_read_iovec_cb_type    read_iovec_event_cb;
} remotefs_read_iovec_s_type;

/*===========================================================================
 *  remotefs_srv_register_read_iovec - Allow the client to register itself
 *   with the API for the Remote-FS event REMOTEFS_SRV_EVNT_READ_IOVEC
 *
 *  RETURN VALUE
 *    remotefs_status_type
 *  SIDE EFFECTS
 *    Internal data initialized for the new client.
=========================================================================*/
/* Register function for the READ IOVEC */
remotefs_status_type remotefs_srv_register_read_iovec
(
 remotefs_srv_event_e_type   read_iovec_event,
 remotefs_srv_read_iovec_cb_type   remotefs_srv_read_iovec_cb
);
/*~ FUNCTION remotefs_srv_register_read_iovec RELEASE_FUNC remotefs_srv_un_register_cb (read_iovec_event) 
    ONERROR return RMFS_ERROR_PIPE */

#endif /* REMOTE_STORAGE_H*/
