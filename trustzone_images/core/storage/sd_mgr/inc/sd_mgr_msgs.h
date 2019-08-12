/* @file sd_mgr_msgs.h
   
  This file contains the RPMB messages from TZ to HLOS side.  
  
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   -----   ----------------------------------------------------------- 
2013-05-28   jt      Added reliable write count/version to read/write request 
2013-02-19   jt      Added RPMB partitioning command  
2012-12-07   jt      Initial version 

=============================================================================*/

#define SD_MGR_MSGS_VERSION   0x01

#define TZ_CM_CMD_RPMB_INIT      0x00000101
#define TZ_CM_CMD_RPMB_READ      0x00000102
#define TZ_CM_CMD_RPMB_WRITE     0x00000103
#define TZ_CM_CMD_RPMB_PARTITION 0x00000104

/* RPMB Init request message */
typedef struct tz_sd_rpmb_init_req_s
{
  uint32 cmd_id;           /* Command ID */
  uint32 version;          /* Messaging Version from sd_mgr in TZ */
} __attribute__ ((packed)) tz_sd_rpmb_init_req_t;


/* RPMB Init response message */
typedef struct tz_sd_rpmb_init_rsp_s
{
  uint32 cmd_id;           /* Command ID */
  uint32 version;          /* Messaging version from RPMB listener */
  uint32 status;           /* RPMB init status */
  uint32 num_sectors;      /* Size of RPMB (in sectors) */
  uint32 rel_wr_count;     /* Reliable write count for the RPMB */
} __attribute__ ((packed)) tz_sd_rpmb_init_rsp_t;

/* RPMB read/write request message */
typedef struct tz_sd_rpmb_rw_req_s
{
  uint32 cmd_id;           /* Command ID */
  uint32 num_half_sectors; /* Number of half-sectors to read/write */
  uint32 req_buff_len;     /* Buffer length */
  uint32 req_buff_offset;  /* Offset to RPMB frames in the request buffer */
  uint32 version;          /* Message version of TZ */
  uint32 rel_wr_count;     /* Reliable write count */
} __attribute__ ((packed)) tz_sd_rpmb_rw_req_t;


/* RPMB read/write response message */
typedef struct tz_sd_rpmb_rw_rsp_s
{
  uint32 cmd_id;          /* Command ID */
  uint32 status;          /* Status from read/write operation */
  uint32 rsp_buff_len;    /* Response buffer length */
  uint32 rsp_buff_offset; /* Offset to RPMB frames in the response buffer */
  uint32 version;         /* Message version from listener */
} __attribute__ ((packed)) tz_sd_rpmb_rw_rsp_t;


/* RPMB partitioning request message */
typedef struct tz_sd_rpmb_partitioning_req_s
{
  uint32 cmd_id;           /* Command ID */
  uint32 version;          /* RPMB partition table Version */
  uint32 dev_id;           /* Device ID */
} __attribute__ ((packed)) tz_sd_rpmb_partitioning_req_t;


/* RPMB partitioning response message */
typedef struct tz_sd_rpmb_partitioning_rsp_s
{
  uint32 cmd_id;           /* Command ID */
  uint32 status;           /* RPMB partitioning status */
  uint32 num_partitions;   /* Number of partitions added */
  uint32 rsp_buff_offset;  /* Offset to the partition addition info */     
} __attribute__ ((packed)) tz_sd_rpmb_partitioning_rsp_t;
