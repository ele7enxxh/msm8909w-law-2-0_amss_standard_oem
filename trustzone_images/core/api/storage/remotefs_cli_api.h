#ifndef REMOTEFS_CLI_API_H
#define REMOTEFS_CLI_API_H

/*====================================================================================*//**

                      Remote FS Client Public APIs Header File


DESCRIPTION
   This is the header file for the remote FS client.  The client provides a wrapper
   for all the server side function to allow implementation of funtionalities required
   prior to calling the server routines.

Copyright (c) 2009-2010 by Qualcomm Technologies Incorporated.  
All Rights Reserved.
Qualcomm Confidential and Proprietary

\file remotefs_cli_api.h
*//*=====================================================================================*/


/*=======================================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/storage/remotefs_cli_api.h#1 $ 
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

YYYY-MM-DD       who     what, where, why
--------------   ---     ----------------------------------------------------------
2010-07-23       vin     Added remotefs_read_iovec
2010-05-07       vj      Added remotefs_get_server_status API support
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
/*
 * Following are the status of the remotefs server.
 */
typedef enum
{
   REMOTEFS_SERVER_DOWN         = 0, /* Server is down */
   REMOTEFS_SERVER_RUNNING      = 1  /* Server is running */
} REMOTEFS_SERVER_STATUS;

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
 *    \param shared_ram_addr 
 *         Output parameter that returns a pointer to the shared ram address
 *    \param shared_ram_size
 *         Output parameter that returns the size of the shared ram address
 *
 * RETURN VALUE
 *    \return 
 *
 *//*===================================================================================*/
void remotefs_get_shared_ram_addr(uint32** rmts_ram_addr, 
                                  uint32* rmts_ram_size);
#endif /* REMOTEFS_CLI_API_H */
