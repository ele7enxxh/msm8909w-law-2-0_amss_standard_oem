#ifndef REMOTEFS_CLI_API_H
#define REMOTEFS_CLI_API_H

/*====================================================================================*//**

                      Remote FS Client Public APIs Header File


DESCRIPTION
   This is the header file for the remote FS client.  The client provides a wrapper
   for all the server side function to allow implementation of funtionalities required
   prior to calling the server routines.

Copyright (c) 2009-2011 by Qualcomm Technologies Incorporated.  
All Rights Reserved.
Qualcomm Confidential and Proprietary

\file remotefs_cli_api.h
*//*=====================================================================================*/


/*=======================================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/api/storage/remotefs_cli_api.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

YYYY-MM-DD       who     what, where, why
--------------   ---     ----------------------------------------------------------
2011-03-15       vj      Added new remotefs APIs to support QMI
2011-02-23       vj      Change the remotefs_init prototype
2011-02-21       vj      Support for multiple remotefs users
2010-12-23       vin     Added handle parameter to remotefs_get_shared_ram_addr
2010-09-20       vj      Moving REMOTEFS_SERVER_STATUS definition to common header
2010-07-23       vin     Added remotefs_read_iovec
2010-05-07       vj      Added remotefs_get_server_status API support
2009-06-09       rh      Add open by friendly name feature
2009-03-30       rh      Initial creation
=========================================================================================*/

/*=======================================================================================
 
                  Include Files

========================================================================================*/

#include "comdef.h"
#include "rex.h"
#include "remotefs_comm_api.h"

/* Indicates the users of remotefs that remotefs_register_open()
 * is available
 */
#define HAVE_REMOTEFS_REGISTER_OPEN

/* Indicates the users of remotefs that
 * remotefs_init_remote_client
 * remotefs_release_remote_client API's are available
 */
#define HAVE_REMOTEFS_CLIENT_REGISTRATION

/*=======================================================================================
 
                  Define structures

========================================================================================*/

/*=======================================================================================
 
                  Function Prototype

========================================================================================*/

/*===================================================================================*//**
 *
 *    remotefs_open - 
 *       Initialize a remote file system partition access pipe
 *
 * PARAMETERS
 *    \param path
 *         String containing the location of the file system to access
 *
 * RETURN VALUE
 *    \return 
 *         Handle for the current transaction.  Return NULL if failure occured.
 *
 *//*===================================================================================*/
void *remotefs_open(const char *path);

/*===================================================================================*//**
 *
 *    remotefs_close - 
 *       Close a remote file system parition access pipe
 *
 * PARAMETERS
 *    \param handle
 *         Handle for the current transaction
 *
 * RETURN VALUE
 *    \return 
 *         Status of the operation
 *
 *//*===================================================================================*/
remotefs_status_type remotefs_close(void *handle);

/*===================================================================================*//**
 *
 *    remotefs_init - 
 *       Initialize a remote file system 
 *
 * PARAMETERS
 *    None
 *
 *//*===================================================================================*/

void remotefs_init(void);

/*===================================================================================*//**
 *
 *    remotefs_srv_get_dev_error - Get device error code from the last data access
 *
 * PARAMETERS
 *    \param handle 
 *         Handle for the current transaction
 *
 * RETURN VALUE
 *    \return 
 *         Platform independent device error code
 *
 *//*===================================================================================*/
uint32 remotefs_get_dev_error(void *handle);

/*===================================================================================*//**
 *
 *    remotefs_write_block - 
 *       Write a signle block of data from memory buffer.  This function is a 
 *       non-blocking function and will return to the caller immidiately.  If 
 *       the caller wish to be notified when write is completed, use 
 *       remotefs_register_callback to register for event notification.
 *       
 *
 * PARAMETERS
 *    \param handle 
 *         Handle for the current transaction
 *    \param data_addr 
 *         The physical address of the data to be written
 *    \param sector_addr 
 *         The sector which the data will be written into
 *    \param counts
 *         Number of blocks to be written
 *
 * RETURN VALUE
 *    \return 
 *         Status of the operation
 *
 *//*===================================================================================*/
remotefs_status_type remotefs_write_block(
      void *handle, 
      uint32 data_addr, 
      uint32 sector_addr, 
      uint32 counts);


/*===================================================================================*//**
 *
 *    remotefs_write_iovec - Write multiple blocks of data from memory buffer 
 *                           described by the IOVEC structure
 *
 * PARAMETERS
 *    \param handle 
 *         Handle for the current transaction
 *    \param iovec_struct 
 *         The physical address of the data to be written
 *    \param ent_cnt
 *         Number of entries in the IOVEC array
 *
 * RETURN VALUE
 *    \return 
 *         Status of the operation
 *
 *//*===================================================================================*/
remotefs_status_type remotefs_write_iovec(
      void *handle, 
      remotefs_iovec_desc_type *iovec_struct, 
      uint32 ent_cnt);


/*===================================================================================*//**
 *
 *    remotefs_register_callback - 
 *       Register event complete callback.  When the callback function is called,
 *       the first parameter of the callback function will indicate the status of
 *       the last operation.  User specific data can also be supplied to this function
 *       so when the callback occurs, this data will be available for the caller.
 *    
 *
 * PARAMETERS
 *    \param handle 
 *         Handle for the current transaction
 *    \param event_cb 
 *         The function to call when the last issued task has been completed
 *    \param user_data 
 *         A single uint32 field to store user specific data.  This data will be 
 *         part of the argument passed to the callback function when the funcation
 *         is called
 *
 * RETURN VALUE
 *    \return 
 *         Status of the operation
 *
 *//*===================================================================================*/
remotefs_status_type remotefs_register_callback(
      void *handle, 
      remotefs_event_func_ptr_type event_cb,
      uint32 user_data);

/*===================================================================================*//**
 *
 *    remotefs_get_server_status - Get the APPS server status by probing the SMEM
 *                                 location
 *
 * PARAMETERS
 *    \param void
 *
 * RETURN VALUE
 *    \return 
 *         Status of the server
 *
 *//*===================================================================================*/
REMOTEFS_SERVER_STATUS remotefs_get_server_status(void);

/*===================================================================================*//**
 *
 *    remotefs_read_iovec -  Read multiple blocks of data into a memory buffer 
 *                           described by the IOVEC structure
 *
 * PARAMETERS
 *    \param handle 
 *         Handle for the current transaction
 *    \param iovec_struct 
 *         The IOVEC structure to populate
 *    \param ent_cnt
 *         Number of entries in the IOVEC array
 *
 * RETURN VALUE
 *    \return 
 *         Status of the operation
 *
 *//*===================================================================================*/
remotefs_status_type remotefs_read_iovec(
      void *handle, 
      remotefs_iovec_desc_type *iovec_struct, 
      uint32 ent_cnt);

/*===================================================================================*//**
 *
 *    remotefs_get_shared_ram_addr -  Provide shared ram address from SMEM
 *
 * PARAMETERS
 *    \param handle
 *         Handle returned by remotefs_open
 *    \param shared_ram_addr 
 *         Output parameter that returns a pointer to the shared ram address
 *    \param shared_ram_size
 *         Output parameter that returns the size of the shared ram address
 *
 * RETURN VALUE
 *    \return 
 *
 *//*===================================================================================*/
void remotefs_get_shared_ram_addr(void *handle,
                                  uint32** rmts_ram_addr, 
                                  uint32* rmts_ram_size);

/*===================================================================================*//**
 *
 *    remotefs_init_remote_client - Initializes a new client for the next
 *       remotefs call to be able to execute in the same task context
 *
 * PARAMETERS
 *    \param handle
 *         Handle for the current transaction
 *
 * RETURN VALUE
 *    \return 
 *         Status of the operation
 *//*===================================================================================*/
remotefs_status_type remotefs_init_remote_client(void *handle);

/*===================================================================================*//**
 *
 *    remotefs_release_remote_client - Release the client allocated
 *       before the remotefs call to execute in the same task context
 *
 * PARAMETERS
 *    \param handle
 *         Handle for the current transaction
 *
 * RETURN VALUE
 *    \return 
 *         Status of the operation
 *//*===================================================================================*/
remotefs_status_type remotefs_release_remote_client(void *handle);

/*===================================================================================*//**
 *
 *    remotefs_register_open - Initialize a remote file system partition 
 *                             access pipe
 *
 * PARAMETERS
 *    \param path
 *         String for the location of the file system to pass to the hotplug layer
 *    \param tcb
 *         TCB of the calling task
 *    \param sigs
 *         Signal to be provided by the task
 *
 * RETURN VALUE
 *    \return 
 *         Handle for the current transaction.  Return NULL if failure occured.
 *//*===================================================================================*/
void *remotefs_register_open
(
   const char *path,
   rex_tcb_type *tcb,
   rex_sigs_type sigs
);
                                  
#endif /* REMOTEFS_CLI_API_H */
