/* 
 * $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/gps_fs_task.h#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $ 
 ******************************************************************************
 * Copyright (c) 1999 - 2012 Qualcomm Technologies Incorporated.
 * Qualcomm Confidential and Proprietary. All Rights Reserved.
 * Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
 * Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
 *
 *     This program is confidential and a trade secret of Qualcomm Atheros, Inc.
 * The receipt or possession of this program does not convey any rights to 
 * reproduce or disclose its contents or to manufacture, use or sell anything 
 * that this program describes in whole or in part, without the express written
 * consent of Qualcomm Atheros, Inc. The recipient and/or possessor of this 
 * program shall not reproduce or adapt or disclose or use this program except 
 * as expressly allowed by a written authorization from Qualcomm Atheros, Inc.
 *
 ******************************************************************************
 * Description:
 *	This file contains the prototype for the task declaration
 *
 ******************************************************************************
*/
#ifndef GPS_FS_TASK_H
#define GPS_FS_TASK_H


/*
 * Include files
*/
#include "fs_public.h"

/*
 * Constant definitions
*/

#define GPS_FS_TASK_EFS_PUT_SIZE       5000  /* IF the File size is greate use efs_put */

/* Message source task */
enum
{
  GPS_FS_TASK_DIAG_MESSAGE = 0,
  GPS_FS_TASK_MGP_MESSAGE,
  GPS_FS_TASK_MAX_MESSAGE
};


/*
 * Structure definitions
*/

/*
 * Local data declarations.
*/


/*
 * Local function prototypes
*/

/*===========================================================================
FUNCTION gps_NvInitDefaultWrites

DESCRIPTION
  This function is used to read/write default NV items.

RETURN VALUE
  None
  
DEPENDENCIES
  None
===========================================================================*/
void gps_NvInitDefaultWrites (void);

/*===========================================================================
FUNCTION gps_NvWriteDone

DESCRIPTION
  This function is used to handle NV write callback status. If there are any
  clients who issued write requests, they will be notified and any pending
  write requests will be subsequently handled.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void gps_NvWriteDone (void);

#endif /* GPS_FS_TASK_H */
