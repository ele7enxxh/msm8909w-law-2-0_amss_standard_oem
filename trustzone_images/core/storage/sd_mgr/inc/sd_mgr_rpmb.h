/* @file sd_mgr_rpmb.h
   
  This file contains definitions for RPMB operations for the 
  Partition Manager in TZBSP. 
  
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------  -----   ----------------------------------------------------------- 
2013-02-19   jt     Added partition addition info struct; adjusted error codes   
2012-12-12   jt     Initial version 

=============================================================================*/

#include "comdef.h"

#define RPMB_SVC_ID 0x2000

#define SD_MGR_SUCCESS                     0  /* Success */
#define SD_MGR_ERROR                      -1  /* Generic Failure */
#define SD_MGR_INVALID_PARAM_ERROR        -2  /* Invalid arguments passed to the API */
#define SD_MGR_NOT_FOUND_ERROR            -3  /* Device Not found Error */
#define SD_MGR_PARTI_NOT_FOUND_ERROR      -4  /* Partition Not found Error */
#define SD_MGR_VERSION_MISMATCH           -5  /* Listener version Version mismatch */
#define SD_MGR_OUT_OF_RESOURCES           -6  /* Out of memory/other resources*/
#define SD_MGR_NOT_SUPPORTED              -7  /* Operation Not supported */
#define SD_MGR_RPMB_UNKNOWN_ERROR         -8  /* Unknown error during RPMB access */
#define SD_MGR_RPMB_MAC_ERROR             -9  /* MAC autentication error during RPMB access */
#define SD_MGR_RPMB_WRITE_COUNTER_ERROR   -10 /* Write counter mismatch during RPMB access */
#define SD_MGR_RPMB_ADDR_ERROR            -11 /* RPMB address failure */
#define SD_MGR_RPMB_WRITE_ERROR           -12 /* Failure during RPMB Write */
#define SD_MGR_RPMB_READ_ERROR            -13 /* Failure during RPMB Read */
#define SD_MGR_RPMB_NOT_PROVISIONED_ERROR -14 /* RPMB Not provisioned */
#define SD_MGR_RPMB_MAC_GENERATION_ERROR  -15 /* Failure during MAC generation */
#define SD_MGR_RPMB_RNG_GENERATION_ERROR  -16 /* Failure during RNG generation */
#define SD_MGR_RPMB_NONCE_ERROR           -17 /* Nonce authentication error */
#define SD_MGR_RPMB_PROVISIONED_ERROR     -18 /* RPMB already provisioned */
#define SD_MGR_PARTITION_FOUND            -19 /* Partition Found */
#define SD_MGR_APP_ID_ERROR               -20 /* Failed to create/get application id */ 

/* Storage Device Types */
typedef enum
{
  SD_MGR_EMMC_USER = 0,         /* User Partition in eMMC */
  SD_MGR_EMMC_BOOT1,            /* Boot1 Partition in eMMC */
  SD_MGR_EMMC_BOOT0,            /* Boot2 Partition in eMMC */
  SD_MGR_EMMC_RPMB,             /* RPMB Partition in eMMC */
  SD_MGR_EMMC_GPP1,             /* GPP1 Partition in eMMC */
  SD_MGR_EMMC_GPP2,             /* GPP2 Partition in eMMC */
  SD_MGR_EMMC_GPP3,             /* GPP3 Partition in eMMC */
  SD_MGR_EMMC_GPP4,             /* GPP4 Partition in eMMC */
  SD_MGR_EMMC_ALL,              /* Entire eMMC device */
  SD_MGR_ID_RESERVED=0x7FFFFFFF /* Reserved: Device ID Max */
} sd_mgr_device_id_type;

/* Device Information structure */
typedef struct 
{
  sd_mgr_device_id_type    dev_id;             /* Device ID (Physical partition number) */
  uint8                    partition_guid[16]; /* GUID for the partition in the device (GPT only) */
  uint32                   bytes_per_sector;   /* Bytes per Sector */
  uint32                   total_sectors;      /* Total size in sectors */
  uint32                   available_sectors;  /* Total available sectors for new partitions */
} __attribute__ ((packed)) sd_mgr_device_info_t;

/* Client Information structure */
typedef struct 
{
  sd_mgr_device_id_type    dev_id;             /* Device ID */
  uint8                    partition_guid[16]; /* GUID for the partition in the device (GPT only) */
  uint32 	           bytes_per_sector;   /* Bytes per Sector */
  uint32 	           total_sectors;      /* Total size available in sectors */
} __attribute__ ((packed)) sd_mgr_client_info_t; 

/* Partition addition structure */
typedef struct 
{
   uint8  app_name[32];
   uint32 partition_id; 
   uint32 num_sectors; 
} __attribute__ ((packed)) sd_mgr_parti_addition_info_t;

/* Partition record structure */
typedef struct 
{
   uint32 partition_id; 
   uint16 start_sector; 
   uint16 num_sectors; 
   uint8  name_id[8]; 
} __attribute__ ((packed)) sd_mgr_partition_record_t;

/* Partition table structure*/
typedef struct 
{
   uint32                       signature; 
   uint32                       version; 
   uint32                       total_sectors;  
   uint32                       num_partitions; 
   sd_mgr_partition_record_t    partitions[15]; 
} __attribute__ ((packed)) sd_mgr_partition_table_t;

/* Device handle structure */
typedef struct 
{
   uint32                      initialized; 
   sd_mgr_device_id_type       dev_id;  
   uint32                      total_sectors; 
   uint32                      available_sectors; 
   uint8                       partition_guid[16];
   uint32                      context_id;  
   sd_mgr_partition_table_t    partition_table;  
} __attribute__ ((packed)) sd_mgr_device_handle_t;

/* Client handle structure */
typedef struct 
{
   uint32                       initialized; 
   sd_mgr_device_handle_t       *dev_context; 
   sd_mgr_partition_record_t    *partition_record; 
} __attribute__ ((packed)) sd_mgr_client_handle_t;

typedef void *qsee_sd_mgr_device_handle_t; 
typedef void *qsee_sd_mgr_client_handle_t; 

/******************************************************************************
* Name: sd_mgr_device_init
*
* Description:
* This function initializes the RPMB. It first reads the RPMB write counter
* to see if the device is provisioned. If it is provisioned, it will then
* read the partition table. 
*
* Arguments:
*    dev_id         [IN]:  Partition number of the device  
*    partition_guid [IN]:  Partition GUID (applies only for GPT partitions)
*    dev_handle     [OUT]: Pointer to a device handle
* 
* Returns:
*    SD_MGR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int sd_mgr_device_init(sd_mgr_device_id_type dev_id, uint8 *partition_guid, 
                       qsee_sd_mgr_device_handle_t *dev_handle);

/******************************************************************************
* Name: sd_mgr_open_partition
* 
* Description:
* This function opens a logical partition within the RPMB 
* 
* Arguments: 
*    device_handle  [IN]:  Pointer to a device handle from sd_mgr_device_init() 
*    partition_id   [IN]:  Logical partition ID
*    cli_handle     [OUT]: Pointer to a client handle 
* 
* Returns:
*    SD_MGR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int sd_mgr_open_partition(qsee_sd_mgr_device_handle_t *device_handle, uint32 partition_id, 
                          qsee_sd_mgr_client_handle_t *cli_handle);

/******************************************************************************
* Name: sd_mgr_device_get_info
* 
* Description:
* This function returns the RPMB device info
* 
* Arguments: 
*    device_handle  [IN]:  Pointer a device handle from sd_mgr_device_init() 
*    dev_info     [OUT]: Pointer to a device info structure 
* 
* Returns:
*    SD_MGR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int sd_mgr_device_get_info(qsee_sd_mgr_device_handle_t *device_handle, 
                           sd_mgr_device_info_t *dev_info);

/******************************************************************************
* Name: sd_mgr_client_get_info
* 
* Description:
* This function returns the RPMB client info
* 
* Arguments: 
*    client_handle  [IN]:  Pointer a client handle from sd_mgr_open_partition()
*    cli_info     [OUT]: Pointer to a client info structure 
* 
* Returns:
*    SD_MGR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int sd_mgr_client_get_info(qsee_sd_mgr_client_handle_t *client_handle, 
                           sd_mgr_client_info_t *cli_info);

/******************************************************************************
* Name: sd_mgr_add_partition
*
* Description:
* This function adds a new logical partition to the RPMB
*
* Arguments:
*    device_handle  [IN]:  Pointer a device handle from sd_mgr_device_init() 
*    partition_id   [IN]:  Logical Partition ID
*    num_sectors    [IN]:  Number of sectors of the new logical partition 
* 
* Returns:
*    SD_MGR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int sd_mgr_add_partition(qsee_sd_mgr_device_handle_t *device_handle, uint32 partition_id, 
                         uint16 num_sectors);

/******************************************************************************
* Name: sd_mgr_rpmb_provision_key
*
* Description:
* This function provisions the RPMB, using either the HW key or a dummy key
*
* Arguments:
*    key        [IN]: Security Key
*    key_length [IN]: Length of the key provided 
* 
* Returns:
*    SD_MGR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int sd_mgr_rpmb_provision_key(uint8 *key, uint32 key_length);

/******************************************************************************
* Name: sd_mgr_read_sectors
*
* Description:
* This function reads num_sectors of data from the RPMB partition starting from
* start_sector and stores it in data_buffer
*
* Arguments:
*    client_handle  [IN]:  Pointer to a client handle from sd_mgr_open_partition()
*    start_sector [IN]:  Starting sector of the RPMB read
*    num_sectors  [IN]:  Number of sectors to read 
*    data_buffer  [OUT]: Buffer to hold the data read from RPMB
* 
* Returns:
*    SD_MGR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int sd_mgr_read_sectors(qsee_sd_mgr_client_handle_t *client_handle, uint32 start_sector, 
                        uint32 num_sectors, uint8 *data_buffer);

/******************************************************************************
* Name: sd_mgr_write_sectors
*
* Description:
* This function writes num_sectors of data in data_buffer to the RPMB partition,
* starting from start_sector. 
*
* Arguments:
*    client_handle  [IN]: Pointer to a client handle from sd_mgr_open_partition() 
*    start_sector [IN]: Starting sector to write to 
*    num_sectors  [IN]: Number of sectors to write 
*    data_buffer  [IN]: Pointer to a buffer containing data to be written
* 
* Returns:
*    SD_MGR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int sd_mgr_write_sectors(qsee_sd_mgr_client_handle_t *client_handle, uint32 start_sector, 
                         uint32 num_sectors, uint8 *data_buffer);

/******************************************************************************
* Name: sd_mgr_erase_sectors
*
* Description:
* This function "erases" the RPMB partitions, by overwriting the entire
* partition with zeroes. 
*
* Arguments:
*    None 
* 
* Returns:
*    SD_MGR_SUCCESS if no errors, error code otherwise 
* 
******************************************************************************/
int sd_mgr_erase_sectors(void);
