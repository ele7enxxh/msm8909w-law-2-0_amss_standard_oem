/*==============================================================================

                          GPS FILE SYSTEM MODULE

GENERAL DESCRIPTION
   This file contains the prototype for the task declaration

 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 1999 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
Copyright (c) 2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.


Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

==============================================================================*/


/*==============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/gps_fs_api.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 

==============================================================================*/

#ifndef GPS_FS_API_H
#define GPS_FS_API_H


/*
 * Include files
*/
#include "pdapi.h"

/*
 * Constant definitions
*/

enum GP_FS_MESSAGE_CMDS
{
  C_GPS_FS_MSG_TASK_ALIVE = C_USR_MSG_ID_FIRST,     
  C_GPS_FS_MSG_DIAG_CMD,
  C_GPS_FS_MSG_MGP_CMD_0,  /* this msg ID is deprecated */
  C_GPS_FS_MSG_CELLDB_UPDATE,
  C_GPS_FS_MSG_GDT_EXTENDED_SEND,
  C_GPS_FS_MSG_MGP_CMD,
  C_GPS_FS_MSG_MAX        
};

/*  GPS FS task operations. Make sure these match the definitions in pdapi.h */
enum
{
  GPS_FS_CREATE_WRITE_FILE = PDSM_FS_CREATE_WRITE_FILE,
  GPS_FS_APPEND_FILE       = PDSM_FS_APPEND_FILE,
  GPS_FS_DELETE_FILE       = PDSM_FS_DELETE_FILE,
  GPS_FS_READ_FILE         = PDSM_FS_READ_FILE,
  /* Delete all regular (not links, dirs etc.) files in a dir */
  GPS_FS_PURGE_DIR         = PDSM_FS_PURGE_DIR,
  GPS_FS_DELETE_FILE_LIST  = PDSM_FS_DELETE_FILE_LIST
};


/*
 * Structure definitions
*/
#define GPS_FS_MAX_PATH_LEN (64) /* Maximum FS file path length. Make sure this matches 
                                    PDSM_PD_FS_MAX_PATH_LEN_BYTES in pdapi.h */

typedef struct
{
  uint8 u_FileNameLen;       /* Length of FileName */
  uint8 p_FileName[GPS_FS_MAX_PATH_LEN];        /* Variable length file name */
  uint8 u_FileOP;           /* Type of Flash file operation to be performed */
  uint8 u_Reserved[4];         /* Reserved data for future use; u_Reserved[3] is used to 
                                  force sync file to EFS */
  uint64 t_DeleteMask;     /* Defines which satellites are deleted in bulk delete */
  uint32 q_DataSize;       /* Number of bytes to be written */
  void *p_DataPtr;      /* Pointer to data */
  void (*gps_fs_efs_status_get)(uint32 );  /* Callback function for obtainting GPS FS */
                                    /*  Operation status */
} gps_fs_u_cmd_type;


/* CellDb update message type */
typedef struct
{
  uint32 q_DeleteCellDBMask;       /* Bit-Mask which specifies the information to be deleted */
                                   /* Check the bit-mask definition in gps_common.h (C_DELETE_INFO_CELLDB_) */
  boolean u_DeleteOldTimetagInfo;  /* TRUE if Old time-tag information needs to be deleted */
  gps_TimeStructType z_MinGpsTimeAllowed; /* Time-tag data older than this specified GPS time will be deleted */
} gps_fs_celldb_update_type;

/*
 * Local data declarations.
*/

/*
 * Local function prototypes
*/

#endif /* GPS_FS_API_H */
