#ifndef DSGCSD_TASK_H
#define DSGCSD_TASK_H
/*===========================================================================

              G C S D   A P I   D E F I N I T I O N      
                             H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions that are shared between GCSD
  task  and the T M C and R A P I modules

  
  Copyright (c) 2009 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsd.h_v   1.3   21 Mar 2002 10:54:04   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dsgcsd_task.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/04/09   SA      Intial API file created

===========================================================================*/


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Data Services Task Command Definitions
---------------------------------------------------------------------------*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Commands that may be sent to the GCSD protocol stack task.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef enum
{
   DS_GCSD_CONNECT_MO_REQ,    /* Mobile originated connect     */
   DS_GCSD_CONNECT_MT_REQ,    /* Network originated connect    */
   DS_GCSD_DISCONNECT_MO_REQ, /* Mobile originated disconnect  */
   DS_GCSD_DISCONNECT_MT_REQ, /* Network originated disconnect */
   DS_GCSD_CMD_MODE_REQ,      /* On-line command mode          */
   DS_GCSD_RECONNECT_REQ,     /* On-line data mode             */
      
   DS_GCSD_T31_AT_FTH_REQ,
   DS_GCSD_T31_AT_FRH_REQ,
   DS_GCSD_T31_AT_FTM_REQ,
   DS_GCSD_T31_AT_FRM_REQ,
   DS_GCSD_T31_AT_FTS_REQ,
   DS_GCSD_T31_AT_FRS_REQ,
   
   DS_GCSD_SUSPEND_DATA_REQ,  /* Suspend data transfer */
   DS_GCSD_RESUME_DATA_REQ,   /* Resume data transfer */
      
   DS_GCSD_MAX_CMDS           /* Must remain last              */
} ds_gcsd_cmd_enum_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DS_GCSD_TASK

DESCRIPTION
  This is the entry point for the Data Services GCSD Protocol Stack Task. 
  This function contains
  the main processing loop that waits for events (signals or commands) and
  dispatches each event to the appropriate entity for further processing.

DEPENDENCIES
  None

RETURN VALUE
  This function does not currently return.  It may be modified in the future
  when this task is folded into the uplink or downlink processing task.

SIDE EFFECTS
  None

===========================================================================*/

extern void  
ds_gcsd_task
(
  dword ignored
    /* lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
);

#endif /* DSGCSD_TASK_H */
