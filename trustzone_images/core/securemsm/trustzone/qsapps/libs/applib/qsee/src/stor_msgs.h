/* @file stor_msgs.h
 
  This file contains GPT message definitions for the GPT Partition
  Manager in TZ apps.  
  
  Copyright (c) 2012-2013, Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------  -----   ----------------------------------------------------------- 
2013-04-02   jt     Added listener buffer size to gpt_init_rsp 
2013-02-19   jt     Added command to configure GPT partitions  
2012-12-14   jt     Initial version 

=============================================================================*/

#define QSEE_STOR_MSGS_VERSION  0x01

#define TZ_CM_CMD_SD_GPT_INIT      0x00000401
#define TZ_CM_CMD_SD_GPT_READ      0x00000402
#define TZ_CM_CMD_SD_GPT_WRITE     0x00000403
#define TZ_CM_CMD_SD_GPT_PARTITION 0x00000404

/** Command structure for GPT initialization
*/
typedef struct tz_sd_gpt_init_req_s
{
  /** First 4 bytes should always be command id */
  uint32                   cmd_id;   
  /** Messaging Version from sd_mgr in TZ */
  uint32                   version;  
  /** Device ID (physical partition number)  */
  uint32                   dev_id;    
  /** GUID for the partition to be opened */     
  uint8                    gpt_guid[16];   
} __attribute__ ((packed)) tz_sd_gpt_init_req_t;

typedef struct tz_sd_gpt_init_rsp_s
{
  /** First 4 bytes should always be command id */
  uint32                   cmd_id;  
  /** Messaging version from RPMB listener */       
  uint32                   version;     
  /** GPT init status */
  uint32                   status;    
  /** Size of GPT partition (in sectors) */
  uint32                   num_sectors;    
  /** Size of the listener buffer (in bytes) */
  uint32                   list_buf_size; 
  /** Context ID/Handle for the GPT partition */
  uint32                   ctxt_id;        
} __attribute__ ((packed)) tz_sd_gpt_init_rsp_t;


/** Command structure for GPT read/write
*/
typedef struct tz_sd_gpt_rw_req_s
{
  /** First 4 bytes should always be command id */
  uint32                   cmd_id;  
  /** Context ID/Handle for the GPT partition */
  uint32                   ctxt_id;  
  /** Start sector for R/W operation */
  uint32                   start_sector;   
  /** Number of sectors to read/write */
  uint32                   num_sectors;   
  /** Buffer length */ 
  uint32                   req_buff_len;   
  /** Offset to RPMB frames in the request buffer */
  uint32                   req_buff_offset;
} __attribute__ ((packed)) tz_sd_gpt_rw_req_t;

typedef struct tz_sd_gpt_rw_rsp_s
{
  /** First 4 bytes should always be command id */
  uint32                   cmd_id;  
  /** Context ID/Handle for the GPT partition */
  uint32                   ctxt_id;  
  /** GPT read/write status */
  uint32                   status; 
  /** Response buffer length */  
  uint32                   rsp_buff_len;    
  /** Offset to data in the response buffer */
  uint32                   rsp_buff_offset; 
} __attribute__ ((packed)) tz_sd_gpt_rw_rsp_t;


/** Command structure for GPT partitioning 
*/
typedef struct tz_sd_gpt_partitioning_req_s
{
  /** First 4 bytes should always be command id */
  uint32                   cmd_id; 
  /** GPT Partition table Version number */
  uint32                   version; 
  /** Device ID for the GPT partition */
  uint32                   dev_id;  
  /** GUID for the partition */     
  uint8                    partition_guid[16]; 
} __attribute__ ((packed)) tz_sd_gpt_partitioning_req_t;

typedef struct tz_sd_gpt_partitioning_rsp_s
{
  /** First 4 bytes should always be command id */
  uint32                   cmd_id;  
  /** GPT partitioning status */
  uint32                   status; 
  /** Number of partitions to be added */
  uint32                   num_partitions; 
  /** Offset to the partition information  */
  uint32                   rsp_buff_offset; 
} __attribute__ ((packed)) tz_sd_gpt_partitioning_rsp_t;
