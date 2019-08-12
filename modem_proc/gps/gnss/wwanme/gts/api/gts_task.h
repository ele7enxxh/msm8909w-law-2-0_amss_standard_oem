#ifndef GTS_TASK_H
#define GTS_TASK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Global Time Services Location Tech Header File

GENERAL DESCRIPTION
This file contains API definitions between Global Time Service module between
other Location Tech Modules.

Copyright (c) 2014-2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/wwanme/gts/api/gts_task.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "aries_os_api.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * Function Declarations
 *-----------------------------------------------------------------------*/

/*
 ******************************************************************************
 * gts_InitQtime
 *
 * Function description:
 *
 *  This function initializes the DAL interface for GTS for reading 
 *  Qtime values
 *
 * Parameters:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
*/
void gts_InitQtime( void );

/*
 ******************************************************************************
 * Function gts_Init
 *
 * Description:
 *
 *  Initialize GTS Module
 *
 * Parameters:
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_Init( void );

/*
 ******************************************************************************
 * Function gts_MessageProc
 *
 * Description:
 *
 *  Main proc which will handle all GTS IPC Messages
 *
 * Parameters:
 *
 *  p_Data - IPC Message Pointer
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_MessageProc ( const os_IpcMsgType* pz_Msg );

#endif /* GTS_TASK_H */

