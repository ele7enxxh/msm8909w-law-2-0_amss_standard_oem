#ifndef REMOTEFS_COMM_API_H
#define REMOTEFS_COMM_API_H

/*====================================================================================*//**

                      Remote FS Common Public APIs Header File


DESCRIPTION
   This is the header file that contain common definitions required by both the 
   server and the client side code.

Copyright (c) 2009-2011 by Qualcomm Technologies Incorporated.  
All Rights Reserved.
Qualcomm Confidential and Proprietary

\file remotefs_comm_api.h
*//*=====================================================================================*/


/*=======================================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/api/storage/remotefs_comm_api.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

YYYY-MM-DD       who     what, where, why
--------------   ---     ----------------------------------------------------------
2011-02-21       vj      Support for multiple remotefs users
2010-09-20       vj      Moving REMOTEFS_SERVER_STATUS definition to common header
2010-08-19       vin     Moving SMEM info structure here
2010-06-07       vj      Added force_sync and get_status support
2009-06-09       rh      Consolidate the error code used
2009-03-30       rh      Initial creation
=========================================================================================*/

/*=======================================================================================
 
                  Include Files

========================================================================================*/

#include "comdef.h"

/*=======================================================================================
 
                  Define structures

========================================================================================*/

/**
 * \enum REMOTEFS_SERVER_STATUS
 * \brief Status of the remoteFS client
 */
typedef enum
{
   REMOTEFS_SERVER_DOWN         = 0, /* Server is down */
   REMOTEFS_SERVER_RUNNING      = 1  /* Server is running */
} REMOTEFS_SERVER_STATUS;

/**
 * \enum remotefs_status_type
 * \brief Status of the remoteFS operations
 */
typedef enum 
{
   RMFS_NO_ERROR = 0,                           /**< No error detected                  */
   RMFS_ERROR_PARAM,                            /**< Error - incorrect parameter        */
   RMFS_ERROR_PIPE,                             /**< Error - RPC pipe failure           */
   RMFS_ERROR_UNINIT,                           /**< Error - Service not initialized    */
   RMFS_ERROR_BUSY,                             /**< Error - Device is busy             */
   RMFS_ERROR_DEVICE                            /**< Error - Remote storage device      */
} remotefs_status_type;

/**
 * \struct remotefs_iovec_desc
 * \brief A structure to store description of each data transfer for IOVEC operation
 */
typedef struct remotefs_iovec_desc
{
   uint32 sector_addr;                          /**< Sector address to access           */
   uint32 data_phy_addr;                        /**< Physical address of the data       */
   uint32 num_sector;                           /**< Number of sectors to write         */
} remotefs_iovec_desc_type;

/**
 * \brief Define device specific failure code.  This failure code is translated into
 * OS/platform independent error code.
 */
#define RMFS_DEV_ERR_NO_ERROR          0x0000   /**< Error of no error                  */
#define RMFS_DEV_ERR_UNKNOWN           0x0010   /**< Error of no known definition       */
#define RMFS_DEV_ERR_CMD               0x0011   /**< Error of command path issue        */
#define RMFS_DEV_ERR_DAT               0x0012   /**< Error of data path issue           */
#define RMFS_DEV_ERR_CARD_RDY          0x0020   /**< Error of card not ready            */
#define RMFS_DEV_ERR_CARD_UNDETECT     0x0021   /**< Error of no card detected          */
#define RMFS_DEV_ERR_CARD_INIT         0x0022   /**< Error of getting card initialized  */
#define RMFS_DEV_ERR_PARAM             0x0030   /**< Error of supplied parameter        */
#define RMFS_FNC_ERR_PARAM             0x1000   /**< Function call error - Parameter    */

#define REMOTEFS_SUCCESS               0
#define REMOTEFS_FAILURE               (-1)

#define REMOTEFS_SYNC_PENDING          0
#define REMOTEFS_SYNC_COMPLETE         1


/* Magic Number to make sure that we are reading meaningful data - "eMMC" */
#define REMOTEFS_SEFS_MAGIC_NUM      0x654D4D43

/* Client Identifier ID - "MEFS" for Modem EFS */
#define REMOTEFS_SEFS_CLIENT_ID_0    0x4D454653 

/**
 * \typedef remotefs_event_func_ptr_type
 * \brief Define the call back function pointer type
 */
typedef void (*remotefs_event_func_ptr_type)(remotefs_status_type, uint32);
struct remotefs_shared_ramfs_entry
{
   uint32 client_id;     /* Client id to uniquely identify a client */
   uint32 base_addr;     /* Base address of shared RAMFS memory */
   uint32 size;          /* Size of the shared RAMFS memory */
   uint32 server_status; /* This will be initialized to 1 when
                              remote storage RPC server is available */
};

#ifndef REMOTEFS_SEFS_MAX_ENTRIES 
#define REMOTEFS_SEFS_MAX_ENTRIES 3
#endif

struct remotefs_shared_ramfs_table
{
   uint32 magic_id;      /* Identify RAMFS details in SMEM */
   uint32 version;       /* Version of remotefs_shared_ramfs_entry */
   uint32 entries;       /* Total number of valid entries   */
   struct remotefs_shared_ramfs_entry ramfs_entry[REMOTEFS_SEFS_MAX_ENTRIES]; 
                         /* List all entries */
};

/**
 * \typedef remotefs_interface_table
 * \brief  This is a table of function pointers that a physical channel interface
 * (eg SDIO SMEM)  will use to expose its interface subroutines
*/
typedef struct 
{
	char *name;     
    void * (*open)(void *arg);
    uint32 (*close)(void *arg);
    uint32 (*read)(void *arg);
    uint32 (*write)(void *arg);
    byte*  (*set_buffer)(void* handle, byte* buff, uint32 size);
} remotefs_interface_table;

#ifndef REMOTEFS_MAX_CALLERS 
#define REMOTEFS_MAX_CALLERS 4
#endif

#endif /* REMOTEFS_COMM_API_H */
