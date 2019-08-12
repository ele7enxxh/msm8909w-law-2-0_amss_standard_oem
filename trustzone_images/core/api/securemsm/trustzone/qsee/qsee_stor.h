/* @file qsee_stor.h
   
  This file contains storage definitions for the secure apps
  
  Copyright (c) 2012-2013, Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   -----   ----------------------------------------------------------- 
2013-03-04   jt      Adjusted error codes  
2012-12-12   jt      Initial version 

=============================================================================*/

#include <comdef.h>

/* Storage Device Return Error Codes */
#define QSEE_STOR_SUCCESS                     0  /* Success */
#define QSEE_STOR_ERROR                      -1  /* Generic Failure */
#define QSEE_STOR_INVALID_PARAM_ERROR        -2  /* Invalid arguments passed to the API */
#define QSEE_STOR_NOT_FOUND_ERROR            -3  /* Device Not found Error */
#define QSEE_STOR_PARTI_NOT_FOUND_ERROR      -4  /* Partition Not found Error */
#define QSEE_STOR_VERSION_MISMATCH           -5  /* Listener version Version mismatch */
#define QSEE_STOR_OUT_OF_RESOURCES           -6  /* Out of memory/other resources*/
#define QSEE_STOR_NOT_SUPPORTED              -7  /* Operation Not supported */
#define QSEE_STOR_RPMB_UNKNOWN_ERROR         -8  /* Unknown error during RPMB access */
#define QSEE_STOR_RPMB_MAC_ERROR             -9  /* MAC autentication error during RPMB access */
#define QSEE_STOR_RPMB_WRITE_COUNTER_ERROR   -10 /* Write counter mismatch during RPMB access */
#define QSEE_STOR_RPMB_ADDR_ERROR            -11 /* RPMB address failure */
#define QSEE_STOR_RPMB_WRITE_ERROR           -12 /* Failure during RPMB Write */
#define QSEE_STOR_RPMB_READ_ERROR            -13 /* Failure during RPMB Read */
#define QSEE_STOR_RPMB_NOT_PROVISIONED_ERROR -14 /* RPMB Not provisioned */
#define QSEE_STOR_RPMB_MAC_GENERATION_ERROR  -15 /* Failure during MAC generation */
#define QSEE_STOR_RPMB_RNG_GENERATION_ERROR  -16 /* Failure during RNG generation */
#define QSEE_STOR_RPMB_NONCE_ERROR           -17 /* Nonce authentication error */
#define QSEE_STOR_RPMB_PROVISIONED_ERROR     -18 /* RPMB already provisioned */
#define QSEE_STOR_PARTITION_FOUND            -19 /* Partition Found */
#define QSEE_STOR_APP_ID_ERROR               -20 /* Failed to create/get application id */ 

/* Storage Device Types */
typedef enum
{
  QSEE_STOR_EMMC_USER = 0,            /* User Partition in eMMC */
  QSEE_STOR_EMMC_BOOT0,               /* Boot0 Partition in eMMC */
  QSEE_STOR_EMMC_BOOT1,               /* Boot1 Partition in eMMC */
  QSEE_STOR_EMMC_RPMB,                /* RPMB Partition in eMMC */
  QSEE_STOR_EMMC_GPP1,                /* GPP1 Partition in eMMC */
  QSEE_STOR_EMMC_GPP2,                /* GPP2 Partition in eMMC */
  QSEE_STOR_EMMC_GPP3,                /* GPP3 Partition in eMMC */
  QSEE_STOR_EMMC_GPP4,                /* GPP4 Partition in eMMC */
  QSEE_STOR_EMMC_ALL,                 /* Entire eMMC device */
  QSEE_STOR_ID_RESERVED = 0x7FFFFFFF  /* Reserved: Device ID Max */
} qsee_stor_device_id_type;

/* Device Information structure */
typedef struct 
{
  qsee_stor_device_id_type dev_id;             /* Device ID (Physical partition number) */
  uint8                    partition_guid[16]; /* GUID for the partition in the device (GPT only) */
  uint32                   bytes_per_sector;   /* Bytes per Sector */
  uint32                   total_sectors;      /* Total size in sectors */
  uint32                   available_sectors;  /* Total available sectors for new partitions */
} __attribute__ ((packed)) qsee_stor_device_info_t;


/* Client Information structure */
typedef struct 
{
  qsee_stor_device_id_type dev_id;             /* Device ID */
  uint8                    partition_guid[16]; /* GUID for the partition in the device (GPT only) */
  uint32 	                 bytes_per_sector;   /* Bytes per Sector */
  uint32 	                 total_sectors;      /* Total size available in sectors */
} __attribute__ ((packed)) qsee_stor_client_info_t; 

typedef void *qsee_stor_device_handle_t; 
typedef void *qsee_stor_client_handle_t; 


/******************************************************************************
* Name: qsee_stor_device_init
*
* Description:
* This function attempts to initialize the device indicated by device_id
* and partition_guid 
*
* Arguments:
*    device_id         [IN]:  Partition number of the device  
*    partition_guid    [IN]:  Partition GUID (applies only for GPT partitions)
*    device_handle     [OUT]: Pointer to a device handle 
* 
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int qsee_stor_device_init(qsee_stor_device_id_type device_id, uint8 *partition_guid, 
                          qsee_stor_device_handle_t *device_handle);

/******************************************************************************
* Name: qsee_stor_open_partition
*
* Description:
* This function attempts to open a logical partition
*
* Arguments:
*    device_handle     [IN]:  Pointer to a device handle obtained from
*                             qsee_stor_device_init()
*    partition_id      [IN]:  Logical partition ID
*    client_handle     [OUT]: Pointer to a client handle 
* 
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int qsee_stor_open_partition(qsee_stor_device_handle_t *device_handle, uint32 partition_id, 
                             qsee_stor_client_handle_t *client_handle);

/******************************************************************************
* Name: qsee_stor_device_get_info
* 
* Description:
* This function returns the device info
* 
* Arguments: 
*    device_handle   [IN]:  Pointer a device handle from qsee_stor_device_init() 
*    device_info     [OUT]: Pointer to a device info structure 
* 
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int qsee_stor_device_get_info(qsee_stor_device_handle_t *device_handle, 
                              qsee_stor_device_info_t *device_info);

/******************************************************************************
* Name: qsee_stor_client_get_info
* 
* Description:
* This function returns the client info
* 
* Arguments: 
*    client_handle   [IN]:  Pointer a client handle from qsee_stor_open_partition()
*    client_info     [OUT]: Pointer to a client info structure 
* 
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int qsee_stor_client_get_info(qsee_stor_client_handle_t *client_handle, 
                              qsee_stor_client_info_t *client_info);

/******************************************************************************
* Name: qsee_stor_add_partition
*
* Description:
* This function adds a new logical partition
*
* Arguments:
*    device_handle  [IN]:  Pointer a device handle from qsee_stor_device_init() 
*    partition_id   [IN]:  Logical Partition ID
*    num_sectors    [IN]:  Number of sectors of the new logical partition 
* 
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int qsee_stor_add_partition(qsee_stor_device_handle_t *device_handle, 
                            uint32 partition_id, uint16 num_sectors);

/******************************************************************************
* Name: qsee_stor_read_sectors
*
* Description:
* This function attempts to read num_sectors of data from start_sector 
* to data_buffer.
*
* Arguments:
*    client_handle [IN]:  Pointer to a client handle from
*                         qsee_stor_open_partition() 
*    start_sector  [IN]:  Starting sector to read from 
*    num_sectors   [IN]:  Number of sectors to read 
*    data_buffer   [OUT]: Pointer to a buffer containing data that has been read
* 
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int qsee_stor_read_sectors(qsee_stor_client_handle_t *client_handle, uint32 start_sector, 
                           uint32 num_sectors, uint8 *data_buffer);

/******************************************************************************
* Name: qsee_stor_write_sectors
*
* Description:
* This function attempts to write num_sectors of data from data_buffer to
* start_sector.
*
* Arguments:
*    client_handle [IN]: Pointer to a client handle from
*                        qsee_stor_open_partition() 
*    start_sector  [IN]: Starting sector to write to 
*    num_sectors   [IN]: Number of sectors to write 
*    data_buffer   [IN]: Pointer to a buffer containing data to be written
* 
* Returns:
*    QSEE_STOR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int qsee_stor_write_sectors(qsee_stor_client_handle_t *client_handle, uint32 start_sector, 
                            uint32 num_sectors, uint8 *data_buffer);
