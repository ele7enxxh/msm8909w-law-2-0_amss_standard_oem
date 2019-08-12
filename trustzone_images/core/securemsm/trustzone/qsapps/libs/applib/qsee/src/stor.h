/* @file stor.h
   
  This file contains definitions for the top level Partition Manager in 
  TZ Apps. 
  
  Copyright (c) 2012-2013, Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   -----   ----------------------------------------------------------- 
2013-02-19    jt     Added stor_parti_addition_info_t struct 
2012-12-12    jt     Initial version 

=============================================================================*/

#include "comdef.h"
#include "qsee_stor.h"

/* Handle types */
typedef enum
{
   GPT_DEVICE = 0, 
   GPT_CLIENT, 
   RPMB_DEVICE, 
   RPMB_CLIENT
} pm_handle_type; 


/* Structure for partition addition info */
typedef struct 
{
   uint8  app_name[32]; 
   uint32 partition_id;  
   uint32 num_sectors;  
} __attribute__ ((packed)) stor_parti_addition_info_t;

/* Structure for partition record */
typedef struct 
{
   uint32 partition_id; 
   uint16 start_sector; 
   uint16 num_sectors; 
   uint8  name_id[8]; 
} __attribute__ ((packed)) stor_partition_record_t; 

#define MAX_PARTITION_IN_PARTITION_TABLE 15

/* Structure for partition table */
typedef struct 
{
   uint32                       signature; 
   uint32                       version; 
   uint32                       total_sectors;  
   uint32                       num_partitions; 
   stor_partition_record_t      partitions[MAX_PARTITION_IN_PARTITION_TABLE]; 
   uint32                       reserved[64];
} __attribute__ ((packed)) stor_partition_table_t;

/* Structure for device handle */
typedef struct 
{
   uint32                      initialized; 
   qsee_stor_device_id_type    dev_id; 
   uint32                      total_sectors; 
   uint32                      available_sectors; 
   uint8                       partition_guid[16];
   uint32                      context_id;  
   stor_partition_table_t      partition_table;  
} __attribute__ ((packed)) stor_device_handle_t;


/* Structure for client handle */
typedef struct 
{
   uint32                       initialized; 
   stor_device_handle_t         *dev_context; 
   stor_partition_record_t      *partition_record; 
} __attribute__ ((packed)) stor_client_handle_t;
