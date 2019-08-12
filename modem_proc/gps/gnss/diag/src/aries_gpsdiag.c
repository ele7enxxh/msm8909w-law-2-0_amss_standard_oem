/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       GPS Diag Packet Processing

GENERAL DESCRIPTION
  This file contains the diagnostic packet processing routines for the GPS
  module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/diag/src/aries_gpsdiag.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

=============================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/30/15   jv      QZSS Support
06/11/15   am      Fixed response in gpsdiag_TestForceMemAllocError()
05/26/15   jv      SBAS user preference changed from boolean to uint8
02/24/15   ssu     Adding GM in memory logging.
11/26/14   am      Added deletion of GAL ephe & alm data
11/21/14   jv      Added assistance data deletion support for GAL
11/18/14   am      Added SBAS support to gpsdiag_TestForceMemAllocError
10/17/14   jv      Added gpsdiag_TestForceMemAllocError
06/07/13   rh      Added assistance data deletion support for BDS 
03/11/12   rk      Fixed the crash due to client_data_ptr overwritten by "pdsm_pd_inject_external_time" 
                   PD command with NULL value [CR 459792]. 
10/11/10   gk      set the client_id in pdsm_end_session correctly
09/13/10   ssk     Make additional GPS XTRA Diag changes from code review feeback
08/20/10   ssk     Add new GPS Diag commands for XTRA data injection.
1/12/10    gk      set the user data pointer when calling the pdsm_ fns
04/01/09   jlp     Removed output of log and f3 for successful fix count.  Also
                   removed event for first successful fix.  Also fixed lint
                   Critical and High Errors.
02/03/09    atien  MOLR vs AD call flow
01/12/09    atien  XTRA-T support
07/25/08    ah     Fixed Klocwork Critical errors.
06/27/08   jlp     Added log and event dealing with successful fixes.
06/12/08    lt     Support for external coarse position injection.
06/04/08    ah     Modified TBF to 0.5 sec less than required before next fix
                   is triggered during App Tracking via diag.
04/25/08    gk     On-Demand Changes
11/13/07    gk     Add SUPL Certificate injection through PDAPI
09/14/07    gk     NMEA CONFIG set parameters support
08/02/07    ah     Replaced defined(FEATURE_CGPS_PD_COMMS_TCP_ON_MODEM) with
                   !defined(FEATURE_DATA_ON_APPS).
07/20/07    gk     Notify verify remove error checks before sending response.
06/18/07    ah     Moved GPS File System support to TM Diag (modem proc).
06/18/07    ld     Added support for Deleting Cell DB Information.
05/29/07    jd     Implemented GPSDIAG support for GPS File System support.
05/15/07    ah     Implemented SUPL and UMTS CP support for Notify Verify cmd.
05/07/07    ah     Moved Subsystem Cmd Codes for CGPS to start from 100.
                   Renumbered CGPS Cmd Codes for all CGPS DM commands defined.
                   Broke up CGPS_API_CMD into individual CGPS DM commands.
                   Added Version field to all Req/Rsp msgs for each DM cmd.
04/12/07    ah     Various code cleanup and lint fixes.
04/11/07    gk     Modified Time injection cmd to properly handle the time_msec
                   on 7200
04/05/07    gk     Added Time injection cmd, modified get and set params for
                   network address
04/04/07    ah     Broke up CGPS_LCS_VX_CMD into separate individual cmds.
03/07/07    ah     Added LCS support (Notify_Verify_Send_User_Action and
                   various Vx commands), Get_PDAPI_Version command also.
02/05/07    ah     Added App Tracking termination when End Session is called.
01/30/07    ah     Resolved Lint errors & recent PDAPI updates.
01/15/07    ah     Added Multiple App support (up to 10 simultaneous clients).
                   Modified some CGPS_SET_PARAM_CMD values to reflect recent
                   PDAPI updates.
01/05/07    ah     Added Application Tracking functionality.
                   Cleaned up gpsdiag_StartCmd() function implementation.
12/07/06    ah     Renamed Aries GPS references to CGPS.
12/04/06    gk     Removed BS_INFO and modified the set parameters fn param.
07/20/06    ah     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                Lint Configuration
===========================================================================*/
/*lint -save */
/*lint -e818 Pointer parameter could be declared as pointing to const */


/*
 * Include files
*/
#include "gps_variation.h"
#include "comdef.h"

#include "aries_gpsdiag.h"
#include "aries_os_api.h"
#include "aries_os_globals.h"

#include "diag.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "err.h"
#include "msg.h"
#include "queue.h"
#include "sm_oemdre.h"
#include "sm_oemdrsync.h"
#include "mgp_api.h"
#include "event_defs.h"
#include "event.h"
#ifdef FEATURE_CGPS_XTRA_T
#include "tle_api.h"
#endif

/*
 * Constant definitions
*/

/* following is automagically filled in by version-control system */
/* NOTE: DO NOT access this variable directly. Get using gpsdiag_GetDescriptionString() instead. */
static const char *c_sm_query_file_description = "$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/diag/src/aries_gpsdiag.c#1 $";

/* GPS Diag Mutex (Task Context) */
static os_MutexBlockType  z_GpsDiagTaskMutex = {0, MUTEX_TASK_ONLY_CONTEXT};
static os_MutexBlockType* p_GpsDiagTaskMutex = &z_GpsDiagTaskMutex;

#ifdef FEATURE_CGPS_PDCOMM_TEST_DRIVER
/* PDCOMM TD Mutex (Task Context) */
static os_MutexBlockType  z_PdcommTdTaskMutex = {0, MUTEX_TASK_ONLY_CONTEXT, NULL};
static os_MutexBlockType* p_PdcommTdTaskMutex = &z_PdcommTdTaskMutex;
#endif /* FEATURE_CGPS_PDCOMM_TEST_DRIVER */

/* Boolean to determine if the task is already running */
static volatile boolean gpsdiag_TaskIsRunning = FALSE;

/* How many instances of gpsdiag_task were created */
static volatile uint32  gpsdiag_task_count = 0;
static uint32 gpsdiag_registered = 0;

#ifdef FEATURE_CGPS_PDCOMM_TEST_DRIVER
static boolean pdcommtd_TaskIsRunning = FALSE;
#endif /* FEATURE_CGPS_PDCOMM_TEST_DRIVER */

/* GPSDIAG task related declarations for Rex (task.c) */
rex_stack_word_type  gpsdiag_stack[ GPSDIAG_STACK_SIZ ];  /* Stack for GPSDIAG task */
rex_tcb_type         gpsdiag_tcb;                         /* TCB for GPSDIAG task */

#ifdef FEATURE_CGPS_PDCOMM_TEST_DRIVER
/* PDCOMM TD task related declarations for Rex (task.c) */
rex_stack_word_type  pdcommtd_stack[ PDCOMMTD_STACK_SIZ ];  /* Stack for PDCOMM TD task */
rex_tcb_type         pdcommtd_tcb;                         /* TCB for PDCOMM TD task */
#endif /* FEATURE_CGPS_PDCOMM_TEST_DRIVER */

#if INCLUDE_PDAPI_TEST

/* Last Known Position Data Structure from SM */
static pdsm_pd_data_type   gpsdiag_pPdData;
static boolean             gpsdiag_uPdDataValid;
static pdsm_efs_data_s_type z_EfsData;  /* EFS data */
static char u_Efs_FileName[255];
static unsigned char u_Efs_Data[2034];

#endif /* INCLUDE_PDAPI_TEST */

#if INCLUDE_LCS_CMDS

/* SUPL Data Structure from SM */
static pdsm_lcs_supl_notify_verify_req_s_type     gpsdiag_zSuplData;

/* UMTS CP Data Structure from SM */
static pdsm_lcs_umts_cp_notify_verify_req_s_type  gpsdiag_zUmtsCpData;

#endif /* INCLUDE_LCS_CMDS */


/*
 * Structure definitions
*/

/* gpsdiag_PdsmGetPosType
 *
 * This structure is used internally within GPSDIAG to hold GPS fix parameters
 * that are used when calling pdsm_get_position().
 */
typedef struct
{
   pdsm_pd_option_s_type  z_PdOptions;  /* PD option type */
   pdsm_pd_qos_type       z_PdQos;      /* User QOS */
} gpsdiag_PdsmGetPosType;


/* gpsdiag_eFixType
 *
 * This enumeration is used internally within GPSDIAG to list out the
 * possible position fix scenarios
 *   0   -> no more position fixes (stop application tracking)
 *   1   -> oneshot position fix
 *   >=2 -> multiple position fixes (ongoing application tracking)
 */
typedef enum
{
   GPSDIAG_FIX_TYPE_STOP = 0,
   GPSDIAG_FIX_TYPE_ONESHOT,
   GPSDIAG_FIX_TYPE_APP_TRACKING,
   GPSDIAG_FIX_TYPE_MAX
} gpsdiag_eFixType;

/* gpsdiag_FixFlowCtrlType
 *
 * This structure is used internally within GPSDIAG to store the parameters
 * related to application tracking flow control.
 */
typedef PACKED struct PACKED_POST
{
   uint32              q_NumFixesLeft;
   uint32              q_NumFixesTried;
   uint32              q_NumFixesSuccess;
   uint32              q_NumFixesFailed;
   gpsdiag_eFixType    e_FixType;
   boolean             u_AppTrackingTimerExpired;
   boolean             u_PdsmGetPosDone;
} gpsdiag_FixFlowCtrlType;

/* gpsdiag_ClientTrackingType
 *
 * This structure is used internally within GPSDIAG to store all parameters
 * related to a client so that multiple clients can be supported simultaneously.
 */
typedef struct
{
   pdsm_client_type_e_type    e_ClientType;        /* Client Type enumeration */
   pdsm_client_id_type        l_ClientId;          /* Client ID for this session */
   gpsdiag_PdsmGetPosType     z_PdsmGetPosParams;  /* GPS options for Get Position */
   gpsdiag_FixFlowCtrlType    z_FixFlowCtrl;       /* State vars for App Tracking */
   os_TimerMsgType*           p_AppTrackingTimer;  /* App Tracking Timer pointer */
} gpsdiag_ClientTrackingType;

/* Array that stores application tracking information for each client type */
static gpsdiag_ClientTrackingType gpsdiag_ClientTable[PDSM_CLIENT_TYPE_MAX];

/*----------------------------------------------------------------------------*/

/*
 * Function definitions
*/
extern void mgp_BypassSvChecks( uint8 u_BypassMask );

/* Application Tracking helper functions */
static void gpsdiag_InitAppTracking     (void);
static void gpsdiag_StartAppTracking    (pdsm_client_type_e_type e_ClientType);
static void gpsdiag_RestartAppTracking  (pdsm_client_type_e_type e_ClientType);

/* Map Client Id with its corresponding Client Type */
static uint32 gpsdiag_GetClientType (pdsm_client_id_type l_ClientId);

/* Diag request packet handler */
PACKED void* gpsdiag_StartTask         (PACKED void* p_PktReq, uint16 w_PktLength);
PACKED void* gpsdiag_EndTask           (PACKED void* p_PktReq, uint16 w_PktLength);

#ifdef FEATURE_CGPS_PDCOMM_TEST_DRIVER
/* Diag request packet handler */
PACKED void* pdcommtd_StartTask        (PACKED void* p_PktReq, uint16 w_PktLength);
PACKED void* pdcommtd_EndTask          (PACKED void* p_PktReq, uint16 w_PktLength);
extern void pdcomm_test_driver_task(dword dummy);
#endif /* FEATURE_CGPS_PDCOMM_TEST_DRIVER */

#if INCLUDE_PDAPI_TEST
PACKED void* gpsdiag_PdapiCmdHandler   (PACKED void* p_PktReq, uint16 w_PktLength);
#endif /* INCLUDE_PDAPI_TEST */


static gpsdiag_QueryVersionRespType* gpsdiag_HandleQueryVersion_1(gpsdiag_QueryVersionReqType *p_Req);
static boolean gpsdiag_EncryptDescriptionString(uint8 *p_Description, uint8 u_DescriptionLength);
static const char* gpsdiag_GetDescriptionString(void);
PACKED void* gpsdiag_QueryVersionCmd(PACKED void* p_PktReq, uint16 w_PktLength);
static uint32 gpsdiag_TranslateGpsDiagToPdsmDeleteParams(pdsm_delete_parms_type *pDest,
                                                         const gpsdiag_PaDelParamsType *pSrc);
static uint32 gpsdiag_TranslateGpsDiagToPdsmNmeaType(pdsm_pa_nmea_type *pDest,
                                                     const uint16 w_src_diag_nmea_sentence_mask);


static PACKED void* gpsdiag_HandleExternalSPIInjection(PACKED void* p_PktReq, uint16 w_PktLength);
void  gpsdiag_ExternalSPInjectCb(

    void                      *data_block_ptr,
        /* Pointer to client provided data block.
        **
        ** NOTE! Depending on the client implementation of this function
        ** the data block pointed by this pointer might be accessed from
        ** client, as well as, PDSM context simultaneously */

    pdsm_pd_cmd_e_type        pd_cmd,
        /* Indicate which command is this error status for */

    pdsm_pd_cmd_err_e_type    pd_cmd_err
        /* Command error code, indicating whether command
        ** is rejected and the reason */
);

/* OEM Control function */
static PACKED void* gpsdiag_OemControl(PACKED void* p_PktReq, uint16 w_PktLength);

/* OEM DRE function */
static PACKED void* gpsdiag_OemDre(gpsdiag_OemControlReqType* p_PktReq, uint16 w_PktLength);

/* OEM Geofence log function  for start and stop Geofence logging */
static PACKED void* gpsdiag_GeofenceInMemLog(gpsdiag_GeofenceInMemLogCmdReqType* p_PktReq, uint16 w_PktLength);

/* OEM DRSync function  for get config and arming dr pulse */
PACKED void* gpsdiag_OemDrSync(gpsdiag_OemControlReqType* p_PktReq, uint16 w_PktLength);

/* OEM DRSync function  for set config */
PACKED void* gpsdiag_OemDrSyncSetCfg(PACKED void* p_PktReq, uint16 w_PktLength);

/* Xtra function  for Xtra client registration */
static PACKED void* gpsdiag_XtraRegCmd(PACKED void* p_PktReq, uint16 w_PktLength);

/* Memory Allocation Error Test function  for Constellation Configuration  */
static PACKED void* gpsdiag_TestForceMemAllocError(PACKED void* p_PktReq, uint16 w_PktLength);


/*Mapping table for converting an enum of type "pdsm_xtra_cmd_err_e_type" to
 type "pdsm_xtra_status_e_type. The table is indexed by pdsm_xtra_cmd_err_e_type enum values */
static const  pdsm_xtra_status_e_type GPSDIAG_XTRACMDERR_TO_STATUS[]=
{
  XTRA_DOWNLOAD_STATUS_SUCCESS,          /* Maps to PDSM_XTRA_CMD_ERR_NOERR*/
  XTRA_DOWNLOAD_STATUS_TOA_BAD_RANGE,    /* Maps to PDSM_XTRA_CMD_ERR_CLIENT_ID_P */
  XTRA_DOWNLOAD_STATUS_BAD_BUFFER_LENGTH,/* Maps to PDSM_XTRA_CMD_ERR_NO_BUF_L */
  XTRA_DOWNLOAD_STATUS_BAD_CRC,          /* Maps to PDSM_XTRA_CMD_ERR_BAD_CRC */
  XTRA_DOWNLOAD_STATUS_TOA_BAD_RANGE,    /* Maps to PDSM_XTRA_CMD_ERR_TIME_OF_VALIDITY_OUT_OF_RANGE */
  XTRA_DOWNLOAD_STATUS_TOA_BAD_RANGE,    /* Maps to PDSM_XTRA_CMD_ERR_DOWNLOAD_INTRVL_OUT_OF_RANGE */
  XTRA_DOWNLOAD_STATUS_FAILURE,          /* Maps to PDSM_XTRA_CMD_ERR_INTERNAL_RESOURCE_ERROR */
  XTRA_DOWNLOAD_STATUS_FAILURE,          /* Maps to PDSM_XTRA_CMD_ERR_GPS_SUBSYSTEM_BUSY */
  XTRA_DOWNLOAD_STATUS_FAILURE,          /* Maps to PDSM_XTRA_CMD_ERR_TIME_INFO_ERROR*/
  XTRA_DOWNLOAD_STATUS_FAILURE           /* Maps to PDSM_XTRA_CMD_ERR_OTHER*/
};

/*===========================================================================
FUNCTION gpsdiag_XlateToIntApnProfileList

DESCRIPTION
  This function translated gps Diag apn profile list structure to PDSM apn profile
  list structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

static void gpsdiag_XlateToIntApnProfileList(gpsdiag_apn_profiles_type *gpsDiag_profileList,
                                             pdsm_pa_apn_profiles_type *pdsm_profileList)
{
  int profile_num=0;

  for(profile_num = 0; profile_num < C_GPSDIAG_MAX_APN_PROFILES; profile_num++)
  {
    int i=0;

    pdsm_profileList[profile_num].srv_system_type = (uint32)gpsDiag_profileList[profile_num].srv_system_type;
    pdsm_profileList[profile_num].pdp_type = (uint32)gpsDiag_profileList[profile_num].pdp_type;
    pdsm_profileList[profile_num].reserved = (uint32)gpsDiag_profileList[profile_num].reserved;
    for(i=0; i < C_GPSDIAG_MAX_APN_NAME_LENGTH; i++)
    {
      pdsm_profileList[profile_num].apn_name[i] = (uint8)gpsDiag_profileList[profile_num].apn_name[i];
    }
  }
}
/*===========================================================================
FUNCTION gpsdiag_TaskInit

DESCRIPTION
  This function initializes the data structures that will be used by the
  GPSDIAG task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void gpsdiag_TaskInit (void)
{
   MSG_MED("GPSDIAG task initializing...", 0, 0, 0);

   /* Enable the IPC module for GPSDIAG task */
   if(!os_IpcStart((uint32)THREAD_ID_GPSDIAG))
   {
      MSG_ERROR("GPSDIAG: IPC Start Failed", 0, 0, 0);
   }

   /* Initialize gpsdiag static variables */

#if INCLUDE_PDAPI_TEST
   gpsdiag_uPdDataValid     = FALSE;
#endif /* INCLUDE_PDAPI_TEST */

   /* Initialize gpsdiag_AppTrackingTimer */
   gpsdiag_InitAppTracking();
} /* gpsdiag_TaskInit */

/*===========================================================================
FUNCTION gpsdiag_TaskCleanup

DESCRIPTION
  This function frees resources that were reserved by GPSDIAG task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void gpsdiag_TaskCleanup(void)
{
   uint32 q_ClientType = 0;

   os_MutexLock(p_GpsDiagTaskMutex); /* TASKLOCK(); */

   /* clean up any resources allocated here */
   for(q_ClientType = 0; q_ClientType < (uint32)PDSM_CLIENT_TYPE_MAX; q_ClientType++)
   {

      /* delete the application tracking timer */
      if(NULL != gpsdiag_ClientTable[q_ClientType].p_AppTrackingTimer)
      {
         if(!os_TimerDestroy(gpsdiag_ClientTable[q_ClientType].p_AppTrackingTimer))
         {
            MSG_ERROR("Invalid timer in cleanup", 0, 0, 0);
         }
         gpsdiag_ClientTable[q_ClientType].p_AppTrackingTimer = NULL;
      }
   }
   /* Release the Mutex before stopping the task. */
   os_MutexUnlock(p_GpsDiagTaskMutex); /* TASKFREE(); */
   
   /* Unset the Task Running flag and stop the task. */
   gpsdiag_TaskIsRunning = FALSE;
   rex_kill_task(&gpsdiag_tcb); /* commit suicide */
} /* gpsdiag_TaskCleanup */

/*===========================================================================
FUNCTION gpsdiag_ProcessMsg

DESCRIPTION
  This function processes the IPC messages that are received in the gpsdiag
  task.

DEPENDENCIES
  None.

RETURN VALUE
  FALSE if exit message is received, TRUE otherwise

SIDE EFFECTS
  None.
===========================================================================*/

boolean gpsdiag_ProcessMsg (const os_IpcMsgType* p_Msg)
{
   pdsm_client_type_e_type e_ClientType;
   uint32 q_TimerId;
   uint32 q_Param;
   boolean u_retVal = TRUE;


   if(p_Msg == NULL)
   {
      ERR_FATAL("GPSDIAG Ipc Msg id=%d IS NULL!!\r\n", 0, 0, 0);
   }
   else
   {
      /* Now handle the IPC message received */
      switch(p_Msg->q_MsgId)
      {
      case C_OS_MSG_ID_TIMER_EXPIRY:
         /*lint -e826 Suspicious pointer-to-pointer conversion (area too small) */
         q_TimerId = ((os_TimerExpiryType*)(p_Msg->p_Data))->q_TimerId;
         q_Param   = ((os_TimerExpiryType*)(p_Msg->p_Data))->q_Param;

         MSG_MED("C_OS_MSG_ID_TIMER_EXPIRY IpcMsg received (Id %d, Param #%d).", q_TimerId, q_Param, 0);

         /* Continue application tracking if necessary */
         e_ClientType = (pdsm_client_type_e_type)GET_CLIENT_TYPE(q_TimerId);
         gpsdiag_RestartAppTracking(e_ClientType);
         break;

      case C_GPSDIAG_TEST_PDAPI:
         MSG_MED("C_GPSDIAG_TEST_PDAPI IpcMsg received.", 0, 0, 0);
         /* Call dispatch function here */
         break;

      case C_GPSDIAG_TEST_APP_TRACKING:
        MSG_MED("C_GPSDIAG_TEST_APP_TRACKING IpcMsg received.", 0, 0, 0);
        /* Call dispatch function here */
        e_ClientType = (pdsm_client_type_e_type)((os_AppTrackType*)(p_Msg->p_Data))->w_ClientType;
        gpsdiag_StartAppTracking((pdsm_client_type_e_type)e_ClientType);
        break;

      case C_GPSDIAG_TEST_LCS:
         MSG_MED("C_GPSDIAG_TEST_LCS IpcMsg received.", 0, 0, 0);
         /* Call dispatch function here */
         break;
      case C_GPSDIAG_END_TASK:
         MSG_MED("C_GPSDIAG_END_TASK IpcMsg received ", 0, 0 ,0);
         u_retVal = FALSE;
         break;

      default:
         ERR_FATAL("GPSDIAG IpcMsg %d INVALID!!", p_Msg->q_MsgId, 0, 0);
         break;
      }
   }

   return u_retVal;
}

/*===========================================================================
FUNCTION gpsdiag_SendMsg

DESCRIPTION
  This function creates an IPC msg of desired length and sends it to the
  desired thread.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean depending on whether IPC msg was created and sent successfully.

SIDE EFFECTS
  None.
===========================================================================*/
gpsdiag_eStatusType gpsdiag_SendMsg (uint32 q_Size, uint32 q_ThreadId, uint32 q_MsgId, void *p_MsgData)

{
  os_IpcMsgType* p_Msg;

  if (q_ThreadId == THREAD_ID_GPSDIAG && gpsdiag_TaskIsRunning == FALSE) // DIAG task is not running!
  {
    MSG_ERROR("GPSDIAG IpcMsg %d NOT sent! GPSDIAG task not running!", q_MsgId, 0, 0);
    return GPSDIAG_ERROR;
  }

  p_Msg = os_IpcCreate(q_Size, IPC_ALLOCATION_DYNAMIC, q_ThreadId);
  if (p_Msg)
  {
    p_Msg->q_MsgId = q_MsgId;
    if (q_Size > 0)
    {
        memscpy(p_Msg->p_Data, q_Size, p_MsgData, q_Size);
    }

    if (os_IpcSend(p_Msg, q_ThreadId))
    {
      MSG_MED("GPSDIAG IpcMsg %d sent.", p_Msg->q_MsgId, 0, 0);
      return GPSDIAG_SUCCESS;
    }
    else
    {
      MSG_ERROR("GPSDIAG IpcMsg %d NOT sent!!", p_Msg->q_MsgId, 0, 0);
    }
  }
  else
  {
    MSG_ERROR("GPSDIAG IpcMsg is NULL!", 0, 0, 0);
  }
  return GPSDIAG_ERROR;
}

/*===========================================================================
FUNCTION gpsdiag_task

DESCRIPTION
  This function defines the gpsdiag task and is run when the
  gpsdiag_StartTask function is called.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -e715 Symbol 'ignored' not referenced */
void gpsdiag_task (dword ignored)
{
   os_IpcMsgType* p_Msg;
   boolean u_processResult = TRUE;

   MSG_MED("GPSDIAG task running... %d", gpsdiag_task_count, 0, 0);

   /* Initialize the GPSDIAG data structures */
   gpsdiag_TaskInit();

   // Test if GPSDIAG task can send IPC msg to itself.
#if INCLUDE_PDAPI_TEST
   if (gpsdiag_SendMsg(0, (uint32)THREAD_ID_GPSDIAG, (uint32)C_GPSDIAG_TEST_PDAPI, NULL))
   {
     MSG_ERROR("GPSDIAG IPC Msg %d Not Sent.", C_GPSDIAG_TEST_PDAPI, 0, 0);
   }
#endif /* INCLUDE_PDAPI_TEST */

#if INCLUDE_APP_TRACKING
   if (gpsdiag_SendMsg(0, (uint32)THREAD_ID_GPSDIAG, (uint32)C_GPSDIAG_TEST_APP_TRACKING, NULL))
   {
     MSG_ERROR("GPSDIAG IPC Msg %d Not Sent.", C_GPSDIAG_TEST_APP_TRACKING, 0, 0);
   }
#endif /* INCLUDE_APP_TRACKING */

#if INCLUDE_LCS_CMDS
   if (gpsdiag_SendMsg(0, (uint32)THREAD_ID_GPSDIAG, (uint32)C_GPSDIAG_TEST_LCS, NULL))
   {
     MSG_ERROR("GPSDIAG IPC Msg %d Not Sent.", C_GPSDIAG_TEST_LCS, 0, 0);
   }
#endif /* INCLUDE_LCS_CMDS */

   /* Resort to a suspend forever loop after task init. */
   while(u_processResult)
   {
      /* Block until an IPC message is received */
      p_Msg = os_IpcReceive();
      u_processResult = gpsdiag_ProcessMsg(p_Msg);
      if(!os_IpcDelete(p_Msg))
      {
         MSG_ERROR("GPSDIAG IPC Msg %d Not Deleted.", p_Msg->q_MsgId, 0, 0);
      }
   }

   gpsdiag_TaskCleanup();
}

/* The dispatch tables for the Aries CGPS subsystem.
 *
 * GPSDIAG (client) creates thise tables and registers them with the
 * diag packet dispatch service (server). The appropriate request
 * handlers are called when an inbound packet matches the subsystem ID
 * and is within the command code range specified in the tables below.
 */

/* Diag table for the new subsystem commands. This table is registered with
** Diag along with our subsystem ID. Any packet received over the serial
** interface that contains our subsystem ID triggers a lookup into this table.
** the lookup is based on the sub command id specified in the diag packet
** if the lookup is successful, the corresponding function is called from
** this table
*/
static const diagpkt_user_table_entry_type gpsdiag_SubsysCmdTable[] =
{
   /* start id, end id, callback function */

#if INCLUDE_PDAPI_TEST
   {CGPS_DIAG_PDAPI_CMD,        CGPS_DIAG_PDAPI_CMD,        gpsdiag_PdapiCmdHandler},
#endif /* INCLUDE_PDAPI_TEST */

   {CGPS_DIAG_START_TASK,       CGPS_DIAG_START_TASK,       gpsdiag_StartTask},
   {CGPS_DIAG_END_TASK,         CGPS_DIAG_END_TASK,         gpsdiag_EndTask},
#ifdef FEATURE_CGPS_PDCOMM_TEST_DRIVER
   {CGPS_PDCOMM_TD_START_TASK,  CGPS_PDCOMM_TD_START_TASK,  pdcommtd_StartTask},
   {CGPS_PDCOMM_TD_END_TASK,    CGPS_PDCOMM_TD_END_TASK,    pdcommtd_EndTask},
#endif /* FEATURE_CGPS_PDCOMM_TEST_DRIVER */

};


/*===========================================================================
FUNCTION gpsdiag_init

DESCRIPTION
  Initialization function for this module. The packet handlers are registered
  with the diagnostics packet dispatch service, and local data structures are
  set up too.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/

void gpsdiag_init (void)
{
   /* Adding this check to prevent registering with Diag more than once*/
   if(gpsdiag_registered == 0)
   {
   #if (!defined(IMAGE_MODEM_PROC) && !defined(IMAGE_APPS_PROC))

      /* Single processor or modem-only */
      DIAGPKT_DISPATCH_TABLE_REGISTER ((uint16)DIAG_SUBSYS_GPS, gpsdiag_SubsysCmdTable);

   #else

      /* Dual processor */
      #if defined(IMAGE_APPS_PROC)

      /* BREW Data on Apps */
      DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_APP_PROC, (uint16)DIAG_SUBSYS_GPS, gpsdiag_SubsysCmdTable);

      #elif defined(IMAGE_MODEM_PROC)

      /* 3rd Party OS Modem build (i.e. WinMobile, BREW Data on Modem) */
         #if !defined(FEATURE_DATA_ON_APPS)
      DIAGPKT_DISPATCH_TABLE_REGISTER ((uint16)DIAG_SUBSYS_GPS, gpsdiag_SubsysCmdTable);
         #endif /* !FEATURE_DATA_ON_APPS */

      #else

      ERR_FATAL("gpsdiag_init: IMAGE_[MODEM|APPS]_PROC not defined for dual proc build !!", 0, 0, 0);

      #endif /* IMAGE_APPS_PROC | IMAGE_MODEM_PROC */

   #endif /* !defined(IMAGE_MODEM_PROC) && !defined(IMAGE_APPS_PROC) */

    /* Initialize gpsdiag_TaskMutex */
    if(os_MutexInit(p_GpsDiagTaskMutex, MUTEX_TASK_ONLY_CONTEXT))
    {
       MSG_MED("GPSDIAG Task Mutex Initialization Success.", 0, 0, 0);
    }
    else
    {
       MSG_ERROR("GPSDIAG Task Mutex Initialization Failed.", 0, 0, 0);
    }
   
#ifdef FEATURE_CGPS_PDCOMM_TEST_DRIVER
    /* Initialize pdcommtd_TaskMutex */
    if(os_MutexInit(p_PdcommTdTaskMutex, MUTEX_TASK_ONLY_CONTEXT))
    {
       MSG_MED("PDCOMM TD Task Mutex Initialization Success.", 0, 0, 0);
    }
    else
    {
       MSG_ERROR("PDCOMM TD Task Mutex Initialization Failed.", 0, 0, 0);
    }
#endif /* FEATURE_CGPS_PDCOMM_TEST_DRIVER */
   
   gpsdiag_registered = 1;
   }
} /* gpsdiag_init */

#if INCLUDE_PDAPI_TEST

/*===========================================================================
FUNCTION gpsdiag_PositionEventCb

DESCRIPTION
  This is the callback function to be used when a GPS position is available.
  Called by the callback dispatcher when a PD position event fires.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void gpsdiag_PositionEventCb (const void* p_DataBlock, pdsm_pd_event_type u_PdEvent,
                              const pdsm_pd_info_s_type* p_PdInfo)
{
   pdsm_client_type_e_type e_ClientType;

   /* Retrieve Client Type (stored in p_DataBlock) */
   e_ClientType = *((pdsm_client_type_e_type*)p_DataBlock);

   /* Client Type Sanity Check */
   if(e_ClientType >= PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_PositionEventCb: Client Type Error %d", e_ClientType, 0, 0);
      return;
   }

   if(p_PdInfo == NULL)
   {
      MSG_ERROR("gpsdiag_PositionEventCb: No Valid Information", 0, 0, 0);
      return;
   }

   /* Update the last known position static data structure */
   memscpy(&gpsdiag_pPdData, sizeof(gpsdiag_pPdData), &(p_PdInfo->pd_info.pd_data), sizeof(p_PdInfo->pd_info.pd_data));

   /*lint -e506 Constant value boolean */
   gpsdiag_uPdDataValid = TRUE;

   if(u_PdEvent & PDSM_PD_EVENT_POSITION)
   {
      MSG_MED("gpsdiag_PositionEventCb: PDSM_PD_EVENT_POSITION", 0, 0, 0);
      MSG_MED("[Location Info] Latitude:  %d", gpsdiag_pPdData.lat, 0, 0);
      MSG_MED("                Longitude: %d", gpsdiag_pPdData.lon, 0, 0);
   }

   if(u_PdEvent & PDSM_PD_EVENT_VELOCITY)
   {
      MSG_MED("gpsdiag_PositionEventCb: PDSM_PD_EVENT_VELOCITY", 0, 0, 0);
      MSG_MED("[Velocity Info] Horizonal: %d", gpsdiag_pPdData.velocity_hor, 0, 0);
      MSG_MED("                Vertical:  %d", gpsdiag_pPdData.velocity_ver, 0, 0);
   }

   if(u_PdEvent & PDSM_PD_EVENT_HEIGHT)
   {
      MSG_MED("gpsdiag_PositionEventCb: PDSM_PD_EVENT_HEIGHT", 0, 0, 0);
      MSG_MED("[Altitude Info] Altitude:  %d", gpsdiag_pPdData.altitude, 0, 0);
   }

   /* Update client structure with successful position fix */
   if(gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.e_FixType == GPSDIAG_FIX_TYPE_APP_TRACKING)
   {
      gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesSuccess++;
   }

} /* gpsdiag_PositionEventCb */

/*===========================================================================
FUNCTION gpsdiag_DoneEventCb

DESCRIPTION
  This is the callback function to be used when a GPS session is completed.
  Called by the callback dispatcher when a PD done event fires (either data
  DL or non-data DL). Done events indicate a proper end to a GPS session.
  Currently, it has been promised that every start event will be accompanied
  by a done event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void gpsdiag_DoneEventCb (const void *p_DataBlock, pdsm_pd_event_type u_PdEvent,
                          const pdsm_pd_info_s_type* p_PdInfo)
{
  pdsm_client_type_e_type e_ClientType;
  os_AppTrackType z_AppTrack;

   /* Retrieve Client Type (stored in p_DataBlock) */
   e_ClientType = *((pdsm_client_type_e_type*)p_DataBlock);

   /* Client Type Sanity Check */
   if(e_ClientType >= PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_DoneEventCb: Client Type Error %d", e_ClientType, 0, 0);
      return;
   }

   if(u_PdEvent & PDSM_PD_EVENT_DONE)
   {
      MSG_MED("gpsdiag_DoneEventCb: PDSM_PD_EVENT_DONE", 0, 0, 0);

      /* Check if App Tracking has finished yet */
      if(gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.u_AppTrackingTimerExpired &&
         gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesLeft == 0 &&
         gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.e_FixType == GPSDIAG_FIX_TYPE_APP_TRACKING)
      {
         MSG_HIGH("GPSDIAG Application Tracking Complete", 0, 0, 0);
      }

      /*lint -e506 Constant value boolean */
      gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.u_PdsmGetPosDone = TRUE;

    /* When it gets a position fix, 3 things need to be checked to see if it
     * should invoke position fix again:
     *   1) Timer has timed out
     *   2) Number of fixes left > 0
     *   3) QXDM didn't send a stop application tracking cmd
     */
     if ((gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.u_AppTrackingTimerExpired) &&
        (gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesLeft > 0) &&
        (gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.e_FixType == GPSDIAG_FIX_TYPE_APP_TRACKING))
     {
       MSG_MED("gpsdiag_DoneEventCb: Starting Application Tracking Again...", 0, 0, 0);
       z_AppTrack.w_ClientType = (uint16)e_ClientType;
       if (gpsdiag_SendMsg(sizeof(z_AppTrack), (uint32)THREAD_ID_GPSDIAG,
                         (uint32)C_GPSDIAG_TEST_APP_TRACKING, (void*)&z_AppTrack))
       {
         MSG_ERROR("GPSDIAG IPC Msg %d Not Sent.", C_GPSDIAG_TEST_APP_TRACKING, 0, 0);
       }
     }
   }
   else if(u_PdEvent & PDSM_PD_EVENT_DLOAD_DONE)
   {
      MSG_MED("gpsdiag_DoneEventCb: PDSM_PD_EVENT_DLOAD_DONE", 0, 0, 0);
   }
   else if(u_PdEvent & PDSM_PD_EVENT_COMM_DONE)
   {
      MSG_MED("gpsdiag_DoneEventCb: PDSM_PD_EVENT_COMM_DONE", 0, 0, 0);
   }
   else if(u_PdEvent & PDSM_PD_EVENT_GPS_DONE)
   {
      MSG_MED("gpsdiag_DoneEventCb: PDSM_PD_EVENT_GPS_DONE", 0, 0, 0);
   }
} /* gpsdiag_DoneEventCb */

/*===========================================================================
FUNCTION gpsdiag_EndEventCb

DESCRIPTION
  This is the callback function to be used when a GPS session is ended.
  Called by the callback dispatcher when a PD end event fires (either data
  DL or non-data DL). End events usually indicate an erroneous end to a GPS
  session. A successful completion is signified by the DONE events.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void gpsdiag_EndEventCb (const void *p_DataBlock, pdsm_pd_event_type u_PdEvent,
                         const pdsm_pd_info_s_type* p_PdInfo)
{
   pdsm_client_type_e_type e_ClientType;

   /* Retrieve Client Type (stored in p_DataBlock) */
   e_ClientType = *((pdsm_client_type_e_type*)p_DataBlock);

   /* Client Type Sanity Check */
   if(e_ClientType >= PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_EndEventCb: Client Type Error %d", e_ClientType, 0, 0);
      return;
   }

   if(u_PdEvent & (pdsm_pd_event_type)PDSM_PD_EVENT_END)
   {
      MSG_MED("gpsdiag_EndEventCb: PDSM_PD_EVENT_END", 0, 0, 0);

      /* Report an unsuccessful PDSM session back to QXDM */
      MSG_ERROR("GPSDIAG Position Fix Unsuccessful (%d/%d)",
                (gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesTried),
                (gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesLeft + gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesTried),
                0);
   }
   else if(u_PdEvent & (pdsm_pd_event_type)PDSM_PD_DLOAD_EVENT_END)
   {
      MSG_MED("gpsdiag_EndEventCb: PDSM_PD_DLOAD_EVENT_END", 0, 0, 0);

      /* Report an unsuccessful PDSM session back to QXDM */
      MSG_ERROR("GPSDIAG Data Download Unsuccessful", 0, 0, 0);
   }
   else
   {
      MSG_ERROR("gpsdiag_EndEventCb: Unknown event %d", u_PdEvent, 0, 0);

      /* Report an unsuccessful PDSM session back to QXDM */
      MSG_ERROR("GPSDIAG Location Fix Unsuccessful", 0, 0, 0);
   }

   /* Update client structure with unsuccessful position fix */
   if(gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.e_FixType == GPSDIAG_FIX_TYPE_APP_TRACKING)
   {
      gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesFailed++;
   }
} /* gpsdiag_EndEventCb */

/*===========================================================================
FUNCTION gpsdiag_StartEventCb

DESCRIPTION
  This is the callback function to be used when a GPS session is started.
  Called by the callback dispatcher when a PD start event fires (either data
  DL or non-data DL).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void gpsdiag_StartEventCb (const void* p_DataBlock, pdsm_pd_event_type u_PdEvent,
                           const pdsm_pd_info_s_type* p_PdInfo)
{
   if(u_PdEvent & PDSM_PD_EVENT_BEGIN)
   {
      MSG_MED("gpsdiag_StartEventCb: PDSM_PD_EVENT_BEGIN", 0, 0, 0);
   }
   else if(u_PdEvent & PDSM_PD_EVENT_DLOAD_BEGIN)
   {
      MSG_MED("gpsdiag_StartEventCb: PDSM_PD_EVENT_DLOAD_BEGIN", 0, 0, 0);
   }
   else if(u_PdEvent & PDSM_PD_EVENT_COMM_BEGIN)
   {
      MSG_MED("gpsdiag_StartEventCb: PDSM_PD_EVENT_COMM_BEGIN", 0, 0, 0);
   }
   else if(u_PdEvent & PDSM_PD_EVENT_GPS_BEGIN)
   {
      MSG_MED("gpsdiag_StartEventCb: PDSM_PD_EVENT_GPS_BEGIN", 0, 0, 0);
   }
} /* gpsdiag_StartEventCb */

/*===========================================================================
FUNCTION gpsdiag_FailureEventCb

DESCRIPTION
  This is the callback function to be used when a failure event is received.
  This does not necessarily mean a position fix has failed, as it may
  eventually succeed at some point during the session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void gpsdiag_FailureEventCb (const void* p_DataBlock, pdsm_pd_event_type u_PdEvent,
                             const pdsm_pd_info_s_type* p_PdInfo)
{
   if(u_PdEvent & PDSM_PD_EVENT_UPDATE_FAILURE)
   {
      MSG_MED("gpsdiag_FailureEventCb: PDSM_PD_EVENT_UPDATE_FAILURE", 0, 0, 0);
   }
   else if(u_PdEvent & PDSM_PD_EVENT_COMM_FAILURE)
   {
      MSG_MED("gpsdiag_FailureEventCb: PDSM_PD_EVENT_COMM_FAILURE", 0, 0, 0);
   }
} /* gpsdiag_FailureEventCb */

/*===========================================================================
FUNCTION gpsdiag_ConnectedEventCb

DESCRIPTION
  This is the callback function to be used when a communication connected
  event is received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void gpsdiag_ConnectedEventCb (const void* p_DataBlock, pdsm_pd_event_type u_PdEvent,
                               const pdsm_pd_info_s_type* p_PdInfo)
{
   if(u_PdEvent & PDSM_PD_EVENT_COMM_CONNECTED)
   {
      MSG_MED("gpsdiag_ConnectedEventCb: PDSM_PD_EVENT_COMM_CONNECTED", 0, 0, 0);
   }
} /* gpsdiag_ConnectedEventCb */

/*===========================================================================
FUNCTION gpsdiag_DloadOccurredEventCb

DESCRIPTION
  Updates the dload state machine to show that a dload occured.  Called from
  the dispatcher when the appropriate PD event occurs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/

void gpsdiag_DloadOccurredEventCb (const void *p_DataBlock, pdsm_pd_event_type u_PdEvent,
                                   const pdsm_pd_info_s_type* p_PdInfo)
{
   if(u_PdEvent & PDSM_PD_EVENT_DLOAD)
   {
      MSG_MED("gpsdiag_DloadOccurredEventCb: PDSM_PD_EVENT_DLOAD", 0, 0, 0);
   }
} /* gpsdiag_DloadOccurredEventCb */

/*===========================================================================
FUNCTION gpsdiag_PdEventsDispatchCb

DESCRIPTION
  This is the callback dispatcher for PD events. If we find an event
  pertaining to our client ID, we call the appropriate callback function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

/*lint -e818 Pointer parameter could be declared as pointing to const */
void gpsdiag_PdEventsDispatchCb (void* p_DataBlock, pdsm_pd_event_type u_PdEvent,
                                 const pdsm_pd_info_s_type* p_PdInfo)
{
   pdsm_client_type_e_type e_ClientType;

   if ((p_PdInfo == NULL) || (NULL == p_DataBlock))
   {
      MSG_HIGH("gpsdiag_PdEventsDispatchCb: No Valid Information. %p, %p", p_PdInfo, p_DataBlock, 0);
      return;
   }

   /* Retrieve Client Type (stored in p_DataBlock) */
   e_ClientType = *((pdsm_client_type_e_type*)p_DataBlock);

   /* Client Type Sanity Check */
   if(e_ClientType >= PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_PdEventsDispatchCb: Client Type Error %d", e_ClientType, 0, 0);
      return;
   }

   /*  Check if this event pertains to our client ID. If not, return now. */
   if((p_PdInfo->client_id != gpsdiag_ClientTable[e_ClientType].l_ClientId))
   {
      if(gpsdiag_ClientTable[e_ClientType].l_ClientId != (pdsm_client_id_type)GPSDIAG_INIT_VALUE)
      {
//      MSG_MED("Duplicate event for Client ID %d", e_ClientType, 0, 0);
      }
      else
      {
         MSG_ERROR("gpsdiag_PdEventsDispatchCb: Unknown Client ID %d", e_ClientType, 0, 0);
      }
      return;
   }

   if(u_PdEvent & (PDSM_PD_EVENT_POSITION | PDSM_PD_EVENT_VELOCITY | PDSM_PD_EVENT_HEIGHT))
   {
      gpsdiag_PositionEventCb(p_DataBlock, u_PdEvent, p_PdInfo);
   }
   else if(u_PdEvent & (PDSM_PD_EVENT_DONE | PDSM_PD_EVENT_DLOAD_DONE |
                        PDSM_PD_EVENT_COMM_DONE | PDSM_PD_EVENT_GPS_DONE))
   {
      gpsdiag_DoneEventCb(p_DataBlock, u_PdEvent, p_PdInfo);
   }
   else if(u_PdEvent & (pdsm_pd_event_type)(PDSM_PD_EVENT_END | PDSM_PD_DLOAD_EVENT_END))
   {
      gpsdiag_EndEventCb(p_DataBlock, u_PdEvent, p_PdInfo);
   }
   else if(u_PdEvent & (PDSM_PD_EVENT_BEGIN | PDSM_PD_EVENT_DLOAD_BEGIN |
                        PDSM_PD_EVENT_COMM_BEGIN | PDSM_PD_EVENT_GPS_BEGIN))
   {
      gpsdiag_StartEventCb(p_DataBlock, u_PdEvent, p_PdInfo);
   }
   else if(u_PdEvent & (PDSM_PD_EVENT_UPDATE_FAILURE | PDSM_PD_EVENT_COMM_FAILURE))
   {
      gpsdiag_FailureEventCb(p_DataBlock, u_PdEvent, p_PdInfo);
   }
   else if(u_PdEvent & (PDSM_PD_EVENT_COMM_CONNECTED))
   {
      gpsdiag_ConnectedEventCb(p_DataBlock, u_PdEvent, p_PdInfo);
   }
   else if(u_PdEvent & (PDSM_PD_EVENT_DLOAD))
   {
      gpsdiag_DloadOccurredEventCb(p_DataBlock, u_PdEvent, p_PdInfo);
   }
} /* gpsdiag_PdEventsDispatchCb */

/*===========================================================================
FUNCTION gpsdiag_PaEventsDispatchCb

DESCRIPTION
  This is the callback dispatcher for Parameter-related events. If we find an
  event pertaining to our client ID, we call the appropriate callback
  function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void gpsdiag_PaEventsDispatchCb (void* p_DataBlock, pdsm_pa_event_type u_PaEvent,
                                 const pdsm_pa_info_s_type* p_PaInfo)
{
   pdsm_client_type_e_type e_ClientType;

   if(p_PaInfo == NULL)
   {
      MSG_ERROR("gpsdiag_PaEventsDispatchCb: No Valid Information", 0, 0, 0);
      return;
   }

   /* Retrieve Client Type (stored in p_DataBlock) */
   e_ClientType = *((pdsm_client_type_e_type*)p_DataBlock);

   /* Client Type Sanity Check */
   if(e_ClientType >= PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_PaEventsDispatchCb: Client Type Error %d", e_ClientType, 0, 0);
      return;
   }

   /*  Check if this event pertains to our client ID. If not, return now. */
   if((p_PaInfo->client_id != gpsdiag_ClientTable[e_ClientType].l_ClientId))
   {
      if(gpsdiag_ClientTable[e_ClientType].l_ClientId != (pdsm_client_id_type)GPSDIAG_INIT_VALUE)
      {
//      MSG_MED("Duplicate event for Client ID %d", e_ClientType, 0, 0);
      }
      else
      {
         MSG_ERROR("gpsdiag_PaEventsDispatchCb: Unknown Client ID %d", e_ClientType, 0, 0);
      }
      return;
   }

   if(u_PaEvent & PDSM_PA_EVENT_APP_INFO)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_APP_INFO received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_GPS_LOCK)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_GPS_LOCK received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_KEEP_WARM)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_KEEP_WARM received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_DELETE_PARAMS)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_DELETE_PARAMS received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_POSITION_MODE)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_POSITION_MODE received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_MT_LR_SUPPORT)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_MT_LR_SUPPORT received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_MO_METHOD)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_MO_METHOD received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_NMEA_SENTENCE_TYPE)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_NMEA_SENTENCE_TYPE received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_1X_PDE_NETWORK_ADDRESS)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_1X_PDE_NETWORK_ADDRESS received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_1X_MPC_NETWORK_ADDRESS)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_1X_MPC_NETWORK_ADDRESS received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_UMTS_PDE_NETWORK_ADDRESS)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_UMTS_PDE_NETWORK_ADDRESS received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_NMEA_CONFIG_INFO)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_NMEA_CONFIG_INFO received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_SBAS_STATUS)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_SBAS_STATUS received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_SBAS_USER_PREFERENCE)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_SBAS_USER_PREFERENCE received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_DPO_CONTROL)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_DPO_CONTROL received", 0, 0, 0);
   }
   else if(u_PaEvent & PDSM_PA_EVENT_EXTERNAL_SPI)
   {
      MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_EXTERNAL_SPI received", 0, 0, 0);
   }
   else if (u_PaEvent & PDSM_PA_EVENT_EFS_DATA)
   {
     MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_EFS_DATA received", 0, 0, 0);
   }
   else if (u_PaEvent & PDSM_PA_EVENT_SUPL_VERSION)
   {
     MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_SUPL_VERSION received", 0, 0, 0);
   }
   else if (u_PaEvent & PDSM_PA_EVENT_SUPL_SECURITY)
   {
     MSG_MED("gpsdiag_PaEventsDispatchCb: PDSM_PA_EVENT_SUPL_SECURITY received", 0, 0, 0);
   }

} /* gpsdiag_PaEventsDispatchCb */

   #if INCLUDE_LCS_CMDS

/*===========================================================================
FUNCTION gpsdiag_LcsEventsDispatchCb

DESCRIPTION
  This is the callback dispatcher for LCS events. If we find an event
  pertaining to our client ID, we call the appropriate callback function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void gpsdiag_LcsEventsDispatchCb (void* p_DataBlock, pdsm_lcs_event_type u_LcsEvent,
                                  const pdsm_lcs_info_s_type* p_LcsInfo)
{
   pdsm_client_type_e_type e_ClientType;

   if(p_LcsInfo == NULL)
   {
      MSG_ERROR("gpsdiag_LcsEventsDispatchCb: No Valid Information", 0, 0, 0);
      return;
   }

   /* Retrieve Client Type (stored in p_DataBlock) */
   e_ClientType = *((pdsm_client_type_e_type*)p_DataBlock);

   /* Client Type Sanity Check */
   if(e_ClientType >= PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_LcsEventsDispatchCb: Client Type Error %d", e_ClientType, 0, 0);
      return;
   }

   /*  Check if this event pertains to our client ID. If not, return now. */
   if((p_LcsInfo->client_id != gpsdiag_ClientTable[e_ClientType].l_ClientId))
   {
      if(gpsdiag_ClientTable[e_ClientType].l_ClientId != (pdsm_client_id_type)GPSDIAG_INIT_VALUE)
      {
//      MSG_MED("Duplicate event for Client ID %d", e_ClientType, 0, 0);
      }
      else
      {
         MSG_ERROR("gpsdiag_LcsEventsDispatchCb: Unknown Client ID %d", e_ClientType, 0, 0);
      }
      return;
   }

   if(u_LcsEvent & PDSM_LCS_EVENT_VX_NOTIFY_VERIFY_REQ)
   {
      MSG_MED("gpsdiag_LcsEventsDispatchCb: PDSM_LCS_EVENT_VX_NOTIFY_VERIFY_REQ received", 0, 0, 0);
      MSG_MED("notify_priv_type = %d", p_LcsInfo->lcs_info.vx_req.notification_priv_type, 0, 0);
      MSG_MED("pos_mode = %d", p_LcsInfo->lcs_info.vx_req.pos_mode, 0, 0);
      MSG_MED("Requestor_id[0] = %c", p_LcsInfo->lcs_info.vx_req.requester_id.requester_id[0], 0, 0);
      MSG_MED("Requestor_id[1] = %c", p_LcsInfo->lcs_info.vx_req.requester_id.requester_id[1], 0, 0);
      MSG_MED("Requestor_id[2] = %c", p_LcsInfo->lcs_info.vx_req.requester_id.requester_id[2], 0, 0);
      MSG_MED("Requestor_id[3] = %c", p_LcsInfo->lcs_info.vx_req.requester_id.requester_id[3], 0, 0);
      MSG_MED("Requestor_id[4] = %c", p_LcsInfo->lcs_info.vx_req.requester_id.requester_id[4], 0, 0);
      MSG_MED("Requestor_id[5] = %c", p_LcsInfo->lcs_info.vx_req.requester_id.requester_id[5], 0, 0);
   }
   else if(u_LcsEvent & PDSM_LCS_EVENT_SUPL_NOTIFY_VERIFY_REQ)
   {
      MSG_MED("gpsdiag_LcsEventsDispatchCb: PDSM_LCS_EVENT_SUPL_NOTIFY_VERIFY_REQ received", 0, 0, 0);

      /* Save SUPL data from server (passed in LCS Info struct) */
      if(p_LcsInfo->lcs_cb_cmd_type == PDSM_LCS_CMD_TYPE_SUPL_REQ)
      {
         gpsdiag_zSuplData = p_LcsInfo->lcs_info.supl_req;
         MSG_MED("gpsdiag_LcsEventsDispatchCb: Received SUPL data", 0, 0, 0);
      }
      else
      {
         MSG_ERROR("gpsdiag_LcsEventsDispatchCb: Unexpected SUPL data", 0, 0, 0);
      }
   }
   else if(u_LcsEvent & PDSM_LCS_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ)
   {
      MSG_MED("gpsdiag_LcsEventsDispatchCb: PDSM_LCS_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ received", 0, 0, 0);

      /* Save UMTS Control Plane data from server (passed in LCS Info struct) */
      if(p_LcsInfo->lcs_cb_cmd_type == PDSM_LCS_CMD_TYPE_UMTS_CP_REQ)
      {
         gpsdiag_zUmtsCpData = p_LcsInfo->lcs_info.umts_cp_req;
         MSG_MED("gpsdiag_LcsEventsDispatchCb: Received UMTS CP data", 0, 0, 0);
      }
      else
      {
         MSG_ERROR("gpsdiag_LcsEventsDispatchCb: Unexpected UMTS CP data", 0, 0, 0);
      }
   }
   else if(u_LcsEvent & PDSM_LCS_VX_EVENT_USER_NOTIFY_SERVICE_INTERACTION)
   {
      MSG_MED("gpsdiag_LcsEventsDispatchCb: PDSM_LCS_VX_EVENT_USER_NOTIFY_SERVICE_INTERACTION received", 0, 0, 0);
   }
   else if(u_LcsEvent & PDSM_LCS_EVENT_CLIENT_STATUS)
   {
      MSG_MED("gpsdiag_LcsEventsDispatchCb: PDSM_LCS_EVENT_CLIENT_STATUS received", 0, 0, 0);
   }
   else if(u_LcsEvent & PDSM_LCS_EVENT_CLIENT_VX_CONFIG)
   {
      MSG_MED("gpsdiag_LcsEventsDispatchCb: PDSM_LCS_EVENT_CLIENT_VX_CONFIG received", 0, 0, 0);
      MSG_MED("vx version = %d, trustness setting = %d", p_LcsInfo->lcs_info.vx_config.vxVersion,  p_LcsInfo->lcs_info.vx_config.trustedApp, 0);
   }
} /* gpsdiag_LcsEventsDispatchCb */

   #endif /* INCLUDE_LCS_CMDS */

/*===========================================================================
FUNCTION gpsdiag_ExtStatEventsDispatchCb

DESCRIPTION
  This is the callback dispatcher for External Status events. If we find an
  event pertaining to our client ID, we call the appropriate callback
  function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void gpsdiag_ExtStatEventsDispatchCb (void* p_DataBlock, pdsm_ext_status_event_type u_ExtStatEvent,
                                      const pdsm_ext_status_info_s_type* p_ExtStatInfo)
{
   pdsm_client_type_e_type e_ClientType;

   if(p_ExtStatInfo == NULL)
   {
      MSG_ERROR("gpsdiag_ExtStatEventsDispatchCb: No Valid Information", 0, 0, 0);
      return;
   }

   /* Retrieve Client Type (stored in p_DataBlock) */
   e_ClientType = *((pdsm_client_type_e_type*)p_DataBlock);

   /* Client Type Sanity Check */
   if(e_ClientType >= PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_ExtStatEventsDispatchCb: Client Type Error %d", e_ClientType, 0, 0);
      return;
   }

   /*  Check if this event pertains to our client ID. If not, return now. */
   if((p_ExtStatInfo->client_id != gpsdiag_ClientTable[e_ClientType].l_ClientId))
   {
      if(gpsdiag_ClientTable[e_ClientType].l_ClientId != (pdsm_client_id_type)GPSDIAG_INIT_VALUE)
      {
//      MSG_MED("Duplicate event for Client ID %d", e_ClientType, 0, 0);
      }
      else
      {
         MSG_ERROR("gpsdiag_ExtStatEventsDispatchCb: Unknown Client ID %d", e_ClientType, 0, 0);
      }
      return;
   }

   if(u_ExtStatEvent & PDSM_EXT_STATUS_EVENT_NMEA_POS_REPORT)
   {
      MSG_MED("gpsdiag_ExtStatEventsDispatchCb: PDSM_EXT_STATUS_EVENT_NMEA_POS_REPORT received", 0, 0, 0);
   }
   else if(u_ExtStatEvent & PDSM_EXT_STATUS_EVENT_POS_REPORT)
   {
      MSG_MED("gpsdiag_ExtStatEventsDispatchCb: PDSM_EXT_STATUS_EVENT_POS_REPORT received", 0, 0, 0);
   }
   else if(u_ExtStatEvent & PDSM_EXT_STATUS_EVENT_MEASUREMENT)
   {
      MSG_MED("gpsdiag_ExtStatEventsDispatchCb: PDSM_EXT_STATUS_EVENT_MEASUREMENT received", 0, 0, 0);
   }
} /* gpsdiag_ExtStatEventsDispatchCb */

/*===========================================================================
FUNCTION gpsdiag_EndSessionCmdCb

DESCRIPTION
  This is the callback function issuing a pdsm_end_session command for a
  fix. This function ensures that the command went through successfully.
  If so, an event should be received which gives no error. If not, an error
  msg results.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

/*lint -e818 Pointer parameter could be declared as pointing to const */
void gpsdiag_EndSessionCmdCb (void* p_DataBlock,
                              pdsm_pd_cmd_e_type u_PdCmd,
                              pdsm_pd_cmd_err_e_type u_PdCmdErr)
{
   pdsm_client_type_e_type e_ClientType;

   /* If this command was not asking for an end session, return now */
   if(u_PdCmd != PDSM_PD_CMD_END_SESSION)
   {
      return;
   }

   /* If there was an error associated with the request, print error */
   if(u_PdCmdErr != PDSM_PD_CMD_ERR_NOERR)
   {
      MSG_HIGH("gpsdiag_EndSessionCmdCb: Err=%d", u_PdCmdErr, 0, 0);
   }
   else
   {
      MSG_MED("gpsdiag_EndSessionCmdCb: Success.", 0, 0, 0);

      /* Retrieve Client Type (stored in p_DataBlock) */
      e_ClientType = *((pdsm_client_type_e_type*)p_DataBlock);

      /* Client Type Sanity Check */
      if(e_ClientType >= PDSM_CLIENT_TYPE_MAX ||
         e_ClientType == PDSM_CLIENT_TYPE_NONE)
      {
         MSG_ERROR("gpsdiag_EndSessionCmdCb: Client Type Error %d", e_ClientType, 0, 0);
         return;
      }

      /* No more position fix sessions should happen now */
      if(gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.e_FixType == GPSDIAG_FIX_TYPE_APP_TRACKING)
      {
         gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesFailed++;
         gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesLeft = 0;
      }
   }
} /* gpsdiag_EndSessionCmdCb */

/*===========================================================================
FUNCTION gpsdiag_GetPosFixCmdCb

DESCRIPTION
  This is the callback function issuing a pdsm_get_position command for a
  fix. This function ensures that the command went through successfully.
  If so, an event should be received which gives the positional information
  shortly. If not, an error msg results.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

/*lint -e818 Pointer parameter could be declared as pointing to const */
void gpsdiag_GetPosFixCmdCb (void* p_DataBlock,
                             pdsm_pd_cmd_e_type u_PdCmd,
                             pdsm_pd_cmd_err_e_type u_PdCmdErr)
{
   pdsm_client_type_e_type e_ClientType;

   /* If this command was not asking for a position fix, return now */
   if(u_PdCmd != PDSM_PD_CMD_GET_POS)
   {
      return;
   }

   /* If there was an error associated with the request, print error */
   if(u_PdCmdErr != PDSM_PD_CMD_ERR_NOERR)
   {
      MSG_ERROR("gpsdiag_GetPosFixCmdCb: Err=%d", u_PdCmdErr, 0, 0);

      /* Retrieve Client Type (stored in p_DataBlock) */
      e_ClientType = *((pdsm_client_type_e_type*)p_DataBlock);

      /* Client Type Sanity Check */
      if(e_ClientType >= PDSM_CLIENT_TYPE_MAX ||
         e_ClientType == PDSM_CLIENT_TYPE_NONE)
      {
         MSG_ERROR("gpsdiag_GetPosFixCmdCb: Client Type Error %d", e_ClientType, 0, 0);
         return;
      }

      /* No position fix session is going on */
      if(gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.e_FixType == GPSDIAG_FIX_TYPE_APP_TRACKING)
      {
         /*lint -e506 Constant value boolean */
         gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.u_PdsmGetPosDone = TRUE;
      }
   }
   else
   {
      MSG_MED("gpsdiag_GetPosFixCmdCb: Success.", 0, 0, 0);
   }
} /* gpsdiag_GetPosFixCmdCb */


/*===========================================================================
FUNCTION gpsdiag_InjectExternPosCmdCb

DESCRIPTION
  This is the callback function for external coarse position injection.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

/*lint -e818 Pointer parameter could be declared as pointing to const */
void gpsdiag_InjectExternPosCmdCb (void* p_DataBlock,
                                   pdsm_pd_cmd_e_type u_PdCmd,
                                   pdsm_pd_cmd_err_e_type u_PdCmdErr)
{
   /* If this command was not asking for a position fix, return now */
   if(u_PdCmd != PDSM_PD_CMD_INJECT_POS)
   {
      return;
   }

   /* If there was an error associated with the request, print error */
   if(u_PdCmdErr != PDSM_PD_CMD_ERR_NOERR)
   {
      MSG_ERROR("gpsdiag_InjectExternPosCmdCb: Err=%d", u_PdCmdErr, 0, 0);
   }
   else
   {
      MSG_MED("gpsdiag_InjectExternPosCmdCb: Success.", 0, 0, 0);
   }
} /* gpsdiag_InjectExternPosCmdCb */


/*===========================================================================
FUNCTION gpsdiag_GetPosDlCmdCb

DESCRIPTION
  This is the callback function issuing a pdsm_get_position command for a dl
  session position request.
  This function ensured that the command went through ok, if it did, we should
  receive an event giving us the positional information shortly.  If not, the
  state is updated to show an error.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

/*lint -e818 Pointer parameter could be declared as pointing to const */
void gpsdiag_GetPosDlCmdCb (void* p_DataBlock,
                            pdsm_pd_cmd_e_type u_PdCmd,
                            pdsm_pd_cmd_err_e_type u_PdCmdErr)
{
   /* If this command was not asking for a position fix, return now */
   if(u_PdCmd != PDSM_PD_CMD_GET_POS)
   {
      return;
   }

   /* If there was an error associated with the request, print error */
   if(u_PdCmdErr != PDSM_PD_CMD_ERR_NOERR)
   {
      MSG_ERROR("gpsdiag_GetPosDlCmdCb: Err=%d", u_PdCmdErr, 0, 0);
   }
   else
   {
      MSG_MED("gpsdiag_GetPosDlCmdCb: Success.", 0, 0, 0);
   }
} /* gpsdiag_GetPosDlCmdCb */

/*===========================================================================
FUNCTION gpsdiag_SetParamCmdCb

DESCRIPTION
  This is the callback function for setting NV parameters. It updates the
  NV state to ensure that the write was ok. It is a command callback, so it
  will always be called when a request to write the NV parameters is made.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

/*lint -e818 Pointer parameter could be declared as pointing to const */
void gpsdiag_SetParamCmdCb (void* p_DataBlock,
                            pdsm_pa_cmd_e_type u_PaCmd,
                            pdsm_pa_cmd_err_e_type u_PaCmdErr)
{
   /* If this event was not related to setting parameters, return now */
   if(u_PaCmd != PDSM_PA_CMD_SET_PARAM)
   {
      return;
   }

   /* If there was an error associated with the request, print error */
   if(u_PaCmdErr != PDSM_PA_CMD_ERR_NOERR)
   {
      MSG_ERROR("gpsdiag_SetParamCmdCb: Err=%d", u_PaCmdErr, 0, 0);
   }
   else
   {
      MSG_MED("gpsdiag_SetParamCmdCb: Success.", 0, 0, 0);
   }
} /* gpsdiag_SetParamCmdCb */

/*===========================================================================
FUNCTION gpsdiag_GetParamCmdCb

DESCRIPTION
  This is the callback function for getting NV parameters. It updates the
  NV state to ensure that the read was ok. It is a command callback, so it
  will always be called when a request to read the NV parameters is made.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

/*lint -e818 Pointer parameter could be declared as pointing to const */
void gpsdiag_GetParamCmdCb (void* p_DataBlock,
                            pdsm_pa_cmd_e_type u_PaCmd,
                            pdsm_pa_cmd_err_e_type u_PaCmdErr)
{
   /* If this event was not related to setting parameters, return now */
   if(u_PaCmd != PDSM_PA_CMD_GET_PARAM)
   {
      return;
   }

   /* If there was an error associated with the request, print error */
   if(u_PaCmdErr != PDSM_PA_CMD_ERR_NOERR)
   {
      MSG_ERROR("gpsdiag_GetParamCmdCb: Err=%d", u_PaCmdErr, 0, 0);
   }
   else
   {
      MSG_MED("gpsdiag_GetParamCmdCb: Success.", 0, 0, 0);
   }
} /* gpsdiag_GetParamCmdCb */

   #if INCLUDE_LCS_CMDS

/*===========================================================================
FUNCTION gpsdiag_LcsCmdCb

DESCRIPTION
  This is the callback function for a pdsm_lcs_agent_client_response command
  to get notified of LCS events. This function ensures that the command went
  through successfully. If so, an event should be received. If not, an error
  msg results.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void gpsdiag_LcsCmdCb (void* p_DataBlock, pdsm_lcs_cmd_e_type u_LcsCmd,
                       pdsm_lcs_cmd_err_e_type p_LcsCmdError)
{
   /* If this command is not recognized, return now */
   if(u_LcsCmd <= PDSM_LCS_CMD_NONE || u_LcsCmd >= PDSM_LCS_CMD_MAX)
   {
      MSG_ERROR("gpsdiag_LcsCmdCb: LCS Vx Cmd Not Recognized", 0, 0, 0);
      return;
   }

   /* If there was an error associated with the request, print error */
   if(p_LcsCmdError != PDSM_LCS_CMD_ERR_NOERR)
   {
      MSG_ERROR("gpsdiag_LcsCmdCb: Err=%d", p_LcsCmdError, 0, 0);
   }
   else
   {
      if(u_LcsCmd == PDSM_LCS_CMD_NOTIFY_VERIFY_RESP)
      {
         MSG_MED("gpsdiag_LcsCmdCb: PDSM_LCS_CMD_NOTIFY_VERIFY_RESP", 0, 0, 0);
      }
      else if(u_LcsCmd == PDSM_LCS_CMD_CLIENT_APP_INFO_INIT)
      {
         MSG_MED("gpsdiag_LcsCmdCb: PDSM_LCS_CMD_CLIENT_APP_INFO_INIT", 0, 0, 0);
      }
      else if(u_LcsCmd == PDSM_LCS_CMD_CLIENT_RELEASE)
      {
         MSG_MED("gpsdiag_LcsCmdCb: PDSM_LCS_CMD_CLIENT_RELEASE", 0, 0, 0);
      }
      else if(u_LcsCmd == PDSM_LCS_CMD_GET_CLIENT_STATUS)
      {
         MSG_MED("gpsdiag_LcsCmdCb: PDSM_LCS_CMD_GET_CLIENT_STATUS", 0, 0, 0);
      }
      else if(u_LcsCmd == PDSM_LCS_CMD_QUERY_VX_CONFIG)
      {
         MSG_MED("gpsdiag_LcsCmdCb: PDSM_LCS_CMD_QUERY_VX_CONFIG", 0, 0, 0);
      }
      MSG_MED("p_LcsCmdError: Success.", 0, 0, 0);
   }
} /* gpsdiag_LcsCmdCb */

   #endif /* INCLUDE_LCS_CMDS */

/*===========================================================================
FUNCTION gpsdiag_ApiCmdsDispatch

DESCRIPTION
  This function initializes a client object according to which PDAPI
  initialization command is received (i.e. init, activate, deactivate,
  release) and performs the corresponding operation as appropriate. A
  session client Id will be established after calling the init function,
  so all subsequent PDAPI commands will require this ID.

  Register and deregister commands are handled in gpsdiag_RegCmdsDispatch().

DEPENDENCIES
  This command is sent after a call to gpsdiag_StartTask().

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_ApiCmdsDispatch (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_ApiCmdReqType* p_Req = (gpsdiag_ApiCmdReqType*) p_PktReq;
   gpsdiag_ApiCmdRspType* p_Rsp = NULL;
   gpsdiag_eStatusType        e_Status = GPSDIAG_SUCCESS;
   pdsm_client_status_e_type  e_Result;
   uint32                     e_ClientType;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Map Client Id with its corresponding Client Type */
   if(p_Req->z_DiagCmdHdr.u_CgpsCmdCode == (uint8)CGPS_INIT_CMD)
   {
      /* Client Type found in Client ID field of request packet for CGPS_INIT_CMD only */
      e_ClientType = p_Req->l_ClientId;
   }
   else
   {
      e_ClientType = gpsdiag_GetClientType(p_Req->l_ClientId);
   }

   /* Client Type Sanity Check */
   if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_ApiCmdsDispatch: ClientType Error %d Clientid: %d Cmdcode:%u",
                 e_ClientType,
                 p_Req->l_ClientId,
                 p_Req->z_DiagCmdHdr.u_CgpsCmdCode);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      MSG_MED("gpsdiag_ApiCmdsDispatch: Client Type %d", e_ClientType, 0, 0);
   }

   switch(p_Req->z_DiagCmdHdr.u_CgpsCmdCode)
   {
   case CGPS_INIT_CMD:
      MSG_MED("ApiCmd: CGPS_INIT_CMD", 0, 0, 0);

      gpsdiag_ClientTable[e_ClientType].l_ClientId   = pdsm_client_init((pdsm_client_type_e_type)e_ClientType);
      gpsdiag_ClientTable[e_ClientType].e_ClientType = (pdsm_client_type_e_type)e_ClientType;

      /* Verify if Client ID returned is valid */
      if(gpsdiag_ClientTable[e_ClientType].l_ClientId == (pdsm_client_id_type)GPSDIAG_ERROR_VALUE)
      {
         MSG_ERROR("CGPS_INIT_CMD: Cannot initialize PDAPI!!", 0, 0, 0);
         e_Status = GPSDIAG_ERROR;
      }
      else
      {
         MSG_MED("CGPS_INIT_CMD: ClientType=%d, Client ID %d", e_ClientType, gpsdiag_ClientTable[e_ClientType].l_ClientId, 0);
         e_Status = GPSDIAG_SUCCESS;
      }
      break;

   case CGPS_ACTIVATE_CMD:
      MSG_MED("ApiCmd: CGPS_ACTIVATE_CMD", 0, 0, 0);

      /* Client ID Sanity Check */
      if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
      {
         MSG_ERROR("CGPS_ACTIVATE_CMD: UNKNOWN CLIENT ID!!", 0, 0, 0);
      }
      e_Result = pdsm_client_act(p_Req->l_ClientId);
      e_Status = (e_Result == PDSM_CLIENT_OK) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;
      break;

   case CGPS_DEACTIVATE_CMD:
      MSG_MED("ApiCmd: CGPS_DEACTIVATE_CMD", 0, 0, 0);

      /* Client ID Sanity Check */
      if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
      {
         MSG_ERROR("CGPS_DEACTIVATE_CMD: UNKNOWN CLIENT ID!!", 0, 0, 0);
      }
      e_Result = pdsm_client_deact(p_Req->l_ClientId);
      e_Status = (e_Result == PDSM_CLIENT_OK) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;
      break;

   case CGPS_RELEASE_CMD:
      MSG_MED("ApiCmd: CGPS_RELEASE_CMD", 0, 0, 0);

      /* Client ID Sanity Check */
      if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
      {
         MSG_ERROR("CGPS_RELEASE_CMD: UNKNOWN CLIENT ID!!", 0, 0, 0);
      }
      e_Result = pdsm_client_release(p_Req->l_ClientId);
      e_Status = (e_Result == PDSM_CLIENT_OK) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;
      break;

   default:
      ERR_FATAL("ApiCmd: UNKNOWN PDAPI CMD!!", 0, 0, 0);
      break;
   }

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_ApiCmdRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_ApiCmdRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_ApiCmdsDispatch: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->q_Status      = (uint32)e_Status;

      if(p_Req->z_DiagCmdHdr.u_CgpsCmdCode == CGPS_INIT_CMD)
      {
         p_Rsp->l_ClientId = gpsdiag_ClientTable[e_ClientType].l_ClientId;
      }
      else
      {
         p_Rsp->l_ClientId = p_Req->l_ClientId;
      }
   }

   return p_Rsp;
} /* gpsdiag_ApiCmdsDispatch */

/*===========================================================================
FUNCTION gpsdiag_RegCmdsDispatch

DESCRIPTION
  This function registers or deregisters a client object according to which
  PDAPI register command is received (i.e. PD, PA, LCS, External Status) and
  performs the corresponding operation as appropriate. All PDAPI commands
  here will require the session client ID that was passed during init.

  Init, activate, deactivate, and release commands are handled in
  gpsdiag_ApiCmdsDispatch().

DEPENDENCIES
  This command is sent after a call to gpsdiag_StartTask().

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_RegCmdsDispatch (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_RegCmdReqType* p_Req = (gpsdiag_RegCmdReqType*) p_PktReq;
   gpsdiag_RegCmdRspType* p_Rsp = NULL;
   gpsdiag_eStatusType        e_Status = GPSDIAG_SUCCESS;
   pdsm_client_status_e_type  e_Result;
   uint32                     e_ClientType;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Map Client Id with its corresponding Client Type */
   e_ClientType = gpsdiag_GetClientType(p_Req->l_ClientId);

   /* Client Type Sanity Check */
   if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_RegCmdsDispatch: Client Type Error %d", e_ClientType, 0, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      MSG_MED("gpsdiag_RegCmdsDispatch: Client Type %d", e_ClientType, 0, 0);
   }

   switch(p_Req->z_DiagCmdHdr.u_CgpsCmdCode)
   {
   case CGPS_REGISTER_PD_CMD:

      /* Client ID Sanity Check */
      if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
      {
         MSG_ERROR("CGPS_REGISTER_PD_CMD: UNKNOWN CLIENT ID!!", 0, 0, 0);
      }

      if(p_Req->u_RegisterType == (uint8)GPSDIAG_REG_TYPE_REGISTER)
      {
         MSG_MED("RegCmd: CGPS_REGISTER_PD_CMD: Register: %d, %u", e_ClientType, gpsdiag_ClientTable[e_ClientType].l_ClientId, 0);
         e_Result = pdsm_client_pd_reg(p_Req->l_ClientId,
                                       (void*)&(gpsdiag_ClientTable[e_ClientType].e_ClientType),
                                       gpsdiag_PdEventsDispatchCb,
                                       PDSM_CLIENT_EVENT_REG,
                                       (pdsm_pd_event_type)PDSM_PD_EVENT_MASK,
                                       NULL);
      }
      else /* p_Req->u_RegisterType == GPSDIAG_REG_TYPE_DEREGISTER */
      {
         MSG_MED("RegCmd: CGPS_REGISTER_PD_CMD: Deregister", 0, 0, 0);
         e_Result = pdsm_client_pd_reg(p_Req->l_ClientId,
                                       NULL,
                                       NULL, /*gpsdiag_PdEventsDispatchCb*/
                                       PDSM_CLIENT_EVENT_DEREG,
                                       0, /*PDSM_PD_EVENT_MASK*/
                                       NULL);
      }

      e_Status = (e_Result == PDSM_CLIENT_OK) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;
      break;

   case CGPS_REGISTER_PA_CMD:

      /* Client ID Sanity Check */
      if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
      {
         MSG_ERROR("CGPS_REGISTER_PA_CMD: UNKNOWN CLIENT ID!!", 0, 0, 0);
      }

      if(p_Req->u_RegisterType == (uint8)GPSDIAG_REG_TYPE_REGISTER)
      {
         MSG_MED("RegCmd: CGPS_REGISTER_PA_CMD: Register", 0, 0, 0);
         e_Result = pdsm_client_pa_reg(p_Req->l_ClientId,
                                       (void*)&(gpsdiag_ClientTable[e_ClientType].e_ClientType),
                                       gpsdiag_PaEventsDispatchCb, /* Cb fn to notify client of PA events */
                                       PDSM_CLIENT_EVENT_REG,
                                       PDSM_PA_EVENT_MASK,
                                       NULL /* pdsm_pa_cmd_err_f_type */);
      }
      else /* p_Req->u_RegisterType == GPSDIAG_REG_TYPE_DEREGISTER */
      {
         MSG_MED("RegCmd: CGPS_REGISTER_PA_CMD: Deregister", 0, 0, 0);
         e_Result = pdsm_client_pa_reg(p_Req->l_ClientId,
                                       NULL,
                                       NULL, /* gpsdiag_PaEventsDispatchCb */
                                       PDSM_CLIENT_EVENT_DEREG,
                                       0, /* PDSM_PA_EVENT_MASK */
                                       NULL  /* pdsm_pa_cmd_err_f_type */);
      }

      e_Status = (e_Result == PDSM_CLIENT_OK) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;
      break;

   case CGPS_REGISTER_LCS_CMD:

      /* Client ID Sanity Check */
      if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
      {
         MSG_ERROR("CGPS_REGISTER_LCS_CMD: UNKNOWN CLIENT ID!!", 0, 0, 0);
      }

      if(p_Req->u_RegisterType == (uint8)GPSDIAG_REG_TYPE_REGISTER)
      {
         MSG_MED("RegCmd: CGPS_REGISTER_LCS_CMD: Register", 0, 0, 0);
         e_Result = pdsm_client_lcs_reg(p_Req->l_ClientId,
                                        (void*)&(gpsdiag_ClientTable[e_ClientType].e_ClientType),
                                        gpsdiag_LcsEventsDispatchCb, /* Cb fn to notify client of LCS events */
                                        PDSM_CLIENT_EVENT_REG,
                                        PDSM_LCS_EVENT_MASK,
                                        NULL /* pdsm_lcs_cmd_err_f_type */);
      }
      else /* p_Req->u_RegisterType == GPSDIAG_REG_TYPE_DEREGISTER */
      {
         MSG_MED("RegCmd: CGPS_REGISTER_LCS_CMD: Deregister", 0, 0, 0);
         e_Result = pdsm_client_lcs_reg(p_Req->l_ClientId,
                                        NULL,
                                        NULL, /* gpsdiag_LcsEventsDispatchCb */
                                        PDSM_CLIENT_EVENT_DEREG,
                                        0, /* PDSM_LCS_EVENT_MASK */
                                        NULL  /* pdsm_lcs_cmd_err_f_type */);
      }

      e_Status = (e_Result == PDSM_CLIENT_OK) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;
      break;
   case CGPS_REGISTER_EXT_STAT_CMD:
      {

         /* Client ID Sanity Check */
         if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
         {
            MSG_ERROR("CGPS_REGISTER_EXT_STAT_CMD: UNKNOWN CLIENT ID!!", 0, 0, 0);
         }

         if(p_Req->u_RegisterType == (uint8)GPSDIAG_REG_TYPE_REGISTER)
         {
            MSG_MED("RegCmd: CGPS_REGISTER_EXT_STAT_CMD: Register", 0, 0, 0);;
            e_Result = pdsm_client_ext_status_reg(p_Req->l_ClientId,
                                                  ((void*)&(gpsdiag_ClientTable[e_ClientType].e_ClientType)),
                                                  gpsdiag_ExtStatEventsDispatchCb,
                                                  PDSM_CLIENT_EVENT_REG,
                                                  PDSM_EXT_STATUS_EVENT_MASK,
                                                  NULL); /* pdsm_ext_status_cmd_err_f_type */
         }
         else /* p_Req->u_RegisterType == GPSDIAG_REG_TYPE_DEREGISTER */
         {
            MSG_MED("RegCmd: CGPS_REGISTER_EXT_STAT_CMD: Deregister", 0, 0, 0);
            e_Result = pdsm_client_ext_status_reg(p_Req->l_ClientId,
                                                  NULL,
                                                  NULL, /* gpsdiag_ExtStatEventsDispatchCb */
                                                  PDSM_CLIENT_EVENT_DEREG,
                                                  0, /* PDSM_EXT_STATUS_EVENT_MASK */
                                                  NULL  /* pdsm_ext_status_cmd_err_f_type */);
         }

         e_Status = (e_Result == PDSM_CLIENT_OK) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;
         break;
      }


   default:
      ERR_FATAL("RegCmd: UNKNOWN PDAPI CMD!!", 0, 0, 0);
      break;
   }

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_RegCmdRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_RegCmdRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_RegCmdsDispatch: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = p_Req->l_ClientId;
      p_Rsp->q_Status      = (uint32)e_Status;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_RegCmdsDispatch */


/*===========================================================================

FUNCTION diag_SendXtraDataAckEvent

DESCRIPTION
  This function generates a DM message when the gpsOneEngine
  Acknowledges the xtra data segment sent by the Xtra clients

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void diag_SendXtraDataAckEvent
(
  pdsm_xtra_status_e_type e_EventReason
)
{
  gpsdiag_XtraDataAckEvtType z_XtraDataAckEvt;

  z_XtraDataAckEvt.q_Status = (uint32)e_EventReason;

  event_report_payload( EVENT_GPSONEXTRA_DATA_ACK,
                        sizeof(z_XtraDataAckEvt),
                        (void*)&z_XtraDataAckEvt);

} /* diag_SendXtraDataAckEvent*/

/*===========================================================================
FUNCTION gpsdiag_StartCmd

DESCRIPTION
  This function is used to start a GPS position session from the PDAPI.

  It starts a new GPS session with parameters for the session passed via
  the diag request packet.

DEPENDENCIES
  This client should have first been initialized, registered, and activated
  via the gpsdiag_ApiCmdsDispatch() and gpsdiag_RegCmdsDispatch() functions.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_StartCmd (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_StartCmdReqType* p_Req = (gpsdiag_StartCmdReqType*) p_PktReq;
   gpsdiag_StartCmdRspType* p_Rsp = NULL;
   gpsdiag_eStatusType      e_Status;
   boolean                  e_Result;
   uint32                   e_ClientType;

   /* Get Position (Start) Data Structures from PDSM */
   pdsm_server_address_s_type z_ServerAddr; /* Server address */
   uint16*                    p_ServerAddrIpV6;
   uint8*                     p_UrlValue;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Map Client Id with its corresponding Client Type */
   e_ClientType = gpsdiag_GetClientType(p_Req->l_ClientId);

   /* Client Type Sanity Check */
   if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_StartCmd: Client Type Error %d", e_ClientType, 0, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      MSG_MED("gpsdiag_StartCmd: Client Type %d", e_ClientType, 0, 0);
   }

   /* Client ID Sanity Checks */
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId == GPSDIAG_INIT_VALUE)
   {
      MSG_ERROR("Must initialize PDAPI client first", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
   {
      MSG_ERROR("gpsdiag_StartCmd: UNKNOWN CLIENT ID!!", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }

   /* Verify that the session parameters are within the specified ranges */
   if((p_Req->u_SessionType    >= (uint8)GPSDIAG_SESS_TYPE_MAX)   ||
      (p_Req->u_OperatingType  >= (uint8)GPSDIAG_SESS_OP_MAX)     ||
      (p_Req->u_ServerOption   >= (uint8)GPSDIAG_SERVER_OP_MAX))
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Initialize flow control parameters for position fix here
    *
    * Don't modify u_AppTrackingTimerExpired and u_PdsmGetPosDone variables
    * here (to avoid race conditions).
    *
    */
   if(p_Req->q_NumFixes == 0)
   {
      gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.e_FixType = GPSDIAG_FIX_TYPE_STOP;
   }
   else if(p_Req->u_SessionType == (uint8)GPSDIAG_SESS_TYPE_TRACK_IND)
   {
      gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.e_FixType = GPSDIAG_FIX_TYPE_APP_TRACKING;
   }
   else /* p_Req->u_SessionType == (GPSDIAG_SESS_TYPE_LATEST || GPSDIAG_SESS_TYPE_NEW) */
   {
      gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.e_FixType = GPSDIAG_FIX_TYPE_ONESHOT;
   }
   gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesLeft = p_Req->q_NumFixes;

   /* Check the Bypass Mask to set appropriate settings in MC
      (bypass for SV Steering, SV Direction, Alm Health, Eph Health) */
   if(p_Req->u_BypassMask != 0)
   {
     // Send the Bypass Mask to MC
     mgp_BypassSvChecks(p_Req->u_BypassMask);
   }

   /* Set up parameters for pdsm_get_position() */
   z_ServerAddr.server_addr_type    = (pdsm_server_address_e_type)p_Req->u_ServerAddrType;
   if(z_ServerAddr.server_addr_type == PDSM_SERVER_TYPE_IPV4_ADDRESS)
   {
      /* Fill in pdsm_server_ipv4_address_type struct */
      z_ServerAddr.server_adrs.server_adrs_v4.port_id     = (dword)p_Req->u.z_IpV4Data.q_ServerPort;
      z_ServerAddr.server_adrs.server_adrs_v4.server_adrs = (dword)p_Req->u.z_IpV4Data.q_ServerAdrs;
   }
   else if(z_ServerAddr.server_addr_type == PDSM_SERVER_TYPE_IPV6_ADDRESS)
   {
      /* Fill in pdsm_server_ipv6_address_type struct */
      z_ServerAddr.server_adrs.server_adrs_v6.port_id     = (dword)p_Req->u.z_IpV6Data.q_ServerPort;
      p_ServerAddrIpV6                                  = (uint16*)p_Req->u.z_IpV6Data.w_ServerAdrs;
      memscpy(z_ServerAddr.server_adrs.server_adrs_v6.server_adrs, sizeof(z_ServerAddr.server_adrs.server_adrs_v6.server_adrs),
	  	      p_ServerAddrIpV6, sizeof(z_ServerAddr.server_adrs.server_adrs_v6.server_adrs));
   }
   else /* (z_ServerAddr.server_addr_type == PDSM_SERVER_TYPE_URL_ADDRESS) */
   {
      /* Fill in pdsm_server_url_address_type struct */
      z_ServerAddr.server_adrs.server_adrs_url.url_length = (uint8)p_Req->u.z_UrlData.u_UrlLength;
      p_UrlValue                                         = (uint8*)p_Req->u.z_UrlData.u_Url;
      memscpy(z_ServerAddr.server_adrs.server_adrs_url.url, sizeof(z_ServerAddr.server_adrs.server_adrs_url.url),
             p_UrlValue, z_ServerAddr.server_adrs.server_adrs_url.url_length*sizeof(char));
   } //pdsm_server_address_u_type

   /* PD option type */
   gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdOptions.session =
   (pdsm_pd_session_e_type)p_Req->u_SessionType;

   /* Operation Mode Type */
   gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdOptions.operation_mode =
   (pdsm_pd_session_operation_e_type)p_Req->u_OperatingType;

   /* Fix rate for tracking sessions (pdsm_pd_fix_rate_s_type) */
   gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdOptions.fix_rate.num_fixes          =
   p_Req->q_NumFixes;
   gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdOptions.fix_rate.time_between_fixes_ms =
   p_Req->q_TimeBwFixes * 1000;

   /* Location server information (pdsm_pd_server_info_s_type) */
   gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdOptions.lsinfo.server_option =
   (pdsm_server_option_e_type)p_Req->u_ServerOption;
   gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdOptions.lsinfo.server_adrs   =
   z_ServerAddr; //pdsm_server_address_s_type

   /* Class Id in case of multiple apps */
   gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdOptions.class_id =
   (uint32)p_Req->q_ClassId;

   gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdQos.accuracy_threshold  = (dword)p_Req->q_QosAccThrshld;
   gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdQos.gps_session_timeout = (byte)p_Req->u_QosGpsSessTout;

   /* If starting a data DL session */
   if((gpsdiag_eSessionType)p_Req->u_SessionType == GPSDIAG_SESS_TYPE_DATA_DLOAD)
   {
      /* Get the position */
      e_Result = pdsm_get_position(gpsdiag_GetPosDlCmdCb,
                                   (void*)&(gpsdiag_ClientTable[e_ClientType].e_ClientType), /* user data pointer */
                                   &(gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdOptions),
                                   &(gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdQos),
                                   p_Req->l_ClientId);
      if(e_Result)
      {
         MSG_MED("GPSDIAG Start: Data D/L", 0, 0, 0);
      }
      else
      {
         /* If the function couldn't be called, there was no buffer for the request. */
         MSG_ERROR("GPSDIAG Start: Data D/L Unsuccessful %d!!", e_Result, 0, 0);
      }
   }

   /* If not starting a data DL session */
   else
   {
      if(gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.e_FixType == GPSDIAG_FIX_TYPE_STOP)
      {
         MSG_MED("gpsdiag_StartCmd: Stopping Application Tracking", 0, 0, 0);

         /*lint -e506 Constant value boolean */
         e_Result = TRUE;

         return NULL;
      }
      else if(gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.e_FixType == GPSDIAG_FIX_TYPE_APP_TRACKING)
      {
         /* Application Tracking Strategy
          *
          * When an application tracking cmd is received from QXDM, there are two possible scenarios:
          *   1) no previous application tracking
          *   2) pending application tracking
          * In either case, just issue a new tracking cmd. If there is a pending fix, the
          * new tracking cmd will take effect after the next timer expires.
          */
         MSG_MED("gpsdiag_StartCmd: Starting Application Tracking", 0, 0, 0);

         /* Reset internal flow control variables before starting app tracking */
         gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesTried   = 0;
         gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesSuccess = 0;
         gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesFailed  = 0;

         gpsdiag_StartAppTracking((pdsm_client_type_e_type)e_ClientType);

         /*lint -e506 Constant value boolean */
         e_Result = TRUE;
      }
      else /* GPSDIAG_FIX_TYPE_ONESHOT */
      {
         /* Reset internal flow control variables before starting get position */
         gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesTried   = 0;
         gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesSuccess = 0;
         gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesFailed  = 0;

         /* Get the position */
         e_Result = pdsm_get_position(gpsdiag_GetPosFixCmdCb,
                                      (void*)&(gpsdiag_ClientTable[e_ClientType].e_ClientType), /* user data pointer */
                                      &(gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdOptions),
                                      &(gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdQos),
                                      p_Req->l_ClientId);
         if(e_Result)
         {
            MSG_MED("gpsdiag_StartCmd: Single Shot Position Fix", 0, 0, 0);
         }
         else
         {
            /* If the function couldn't be called, there was no buffer for the request. */
            MSG_ERROR("GPSDIAG Start: Position Fix Unsuccessful %d!!", e_Result, 0, 0);
         }
      }
   }

   e_Status = (e_Result) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_StartCmdRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_StartCmdRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_StartCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = p_Req->l_ClientId;
      p_Rsp->q_Status      = (uint32)e_Status;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_StartCmd */

/*===========================================================================
FUNCTION gpsdiag_EndCmd

DESCRIPTION
  This function is used to invoke end of an ongoing position session or data
  download request.

  It attempts to end an open GPS session on the client ID that was
  established during the call to Start command.

DEPENDENCIES
  To function properly, the gpsdiag_StartCmd() function should have been
  called before this. If not, the diag response packet returned will notify
  the user that there was no session to end.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_EndCmd (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_EndCmdReqType* p_Req = (gpsdiag_EndCmdReqType*) p_PktReq;
   gpsdiag_EndCmdRspType* p_Rsp = NULL;
   gpsdiag_eStatusType    e_Status;
   boolean                e_Result;
   uint32                 e_ClientType;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Map Client Id with its corresponding Client Type */
   e_ClientType = gpsdiag_GetClientType(p_Req->l_ClientId);


   /* Client Type Sanity Check */
   if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_EndCmd: Client Type Error %d", e_ClientType, 0, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      MSG_MED("gpsdiag_EndCmd: Client Type %d", e_ClientType, 0, 0);
   }

   /* Client ID Sanity Checks */
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId == GPSDIAG_INIT_VALUE)
   {
      MSG_ERROR("Must initialize PDAPI client first", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
   {
      MSG_ERROR("gpsdiag_EndCmd: UNKNOWN CLIENT ID!!", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }

   /* Verify that the end parameter is within the specified range */
   if(p_Req->q_EndType >= (uint32)GPSDIAG_END_TYPE_MAX)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* In case that SV Check Bypass bits were set from the previous
   CGPS_START_CMD, clear all the SV Check bypass bits in MC */
   mgp_BypassSvChecks(0);

  /* If ending a data DL session */
  if ((gpsdiag_eEndType)p_Req->q_EndType == GPSDIAG_END_DLOAD)
  {
    /* End the session now */
    e_Result = pdsm_end_session(NULL, /* Command callback */
                                PDSM_PD_END_SESSION_DLOAD_REQ,
                                (void*)&(gpsdiag_ClientTable[e_ClientType].e_ClientType), /* Client specified ptr */
                                p_Req->l_ClientId);
  }

   /* If not ending a data DL session */
   else
   {
      /* End Application Tracking first */
      if(gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.e_FixType == GPSDIAG_FIX_TYPE_APP_TRACKING)
      {
         MSG_MED("gpsdiag_EndCmd: Attempting to stop App Tracking...", 0, 0, 0);
         gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesLeft = 0;
      }

      /* End the session now */
      e_Result = pdsm_end_session(gpsdiag_EndSessionCmdCb, /* Command callback */
                                  PDSM_PD_END_SESSION_FIX_REQ,
                                  (void*)&(gpsdiag_ClientTable[e_ClientType].e_ClientType), /* Client specified ptr */
                                  p_Req->l_ClientId);
   }

   e_Status = (e_Result) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_EndCmdRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_EndCmdRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_EndCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = p_Req->l_ClientId;
      p_Rsp->q_Status      = (uint32)e_Status;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_EndCmd */

/*===========================================================================
FUNCTION gpsdiag_SetParamCmd

DESCRIPTION
  This function is used to set the parameters.

DEPENDENCIES
  This client should have first been initialized via the
  gpsdiag_ApiCmdsDispatch() function.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_SetParamCmd (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_SetParamCmdReqType* p_Req = (gpsdiag_SetParamCmdReqType*) p_PktReq;
   gpsdiag_SetParamCmdRspType* p_Rsp = NULL;
   gpsdiag_eStatusType         e_Status;
   boolean                     e_Result;
   uint32                      e_ClientType;

   /* Set NV Parameters Data Structures from PDSM */
   pdsm_server_address_s_type    z_PaNtwrkAdrs;
   pdsm_delete_parms_type        z_PaDelParams;

   uint16*                       p_ServerAddrIpV6;
   uint8*                        p_UrlValue;

   pdsm_pa_set_type              z_ParamSetVal;
   uint16                        i=0;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Map Client Id with its corresponding Client Type */
   e_ClientType = gpsdiag_GetClientType(p_Req->l_ClientId);

   /* Client Type Sanity Check */
   if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_SetParamCmd: Client Type Error %d", e_ClientType, 0, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      MSG_MED("gpsdiag_SetParamCmd: Client Type %d", e_ClientType, 0, 0);
   }

   /* Client ID Sanity Checks */
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId == GPSDIAG_INIT_VALUE)
   {
      MSG_ERROR("Must initialize PDAPI client first", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
   {
      MSG_ERROR("gpsdiag_SetParamCmd: UNKNOWN CLIENT ID [%d]!!", p_Req->l_ClientId, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }

   /* Verify that the parameter is within the specified range */
   if(p_Req->u_Param >= (uint32)GPSDIAG_PA_MAX)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Verify that certain particular parameter values are valid */
   if((((gpsdiag_eParamType)p_Req->u_Param == GPSDIAG_PA_GPS_LOCK) &&
       (p_Req->z_ParamVal.u_GpsLock >= (uint8)GPSDIAG_PA_LOCK_MAX)) ||
      (((gpsdiag_eParamType)p_Req->u_Param == GPSDIAG_PA_MT_LR_SUPPORT) &&
       (p_Req->z_ParamVal.u_MtLrSpprt >= 2)) ||
      (((gpsdiag_eParamType)p_Req->u_Param == GPSDIAG_PA_MO_METHOD) &&
       (p_Req->z_ParamVal.u_MoMethod >= 2)) )
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Fill in the parameter value, depending on which parameter is being set */
   switch(p_Req->u_Param)
   {
   case GPSDIAG_PA_1X_PDE_NETWORK_ADRS:
   case GPSDIAG_PA_1X_MPC_NETWORK_ADRS:
   case GPSDIAG_PA_UMTS_PDE_NETWORK_ADRS:

      /* Fill in pdsm_server_address_s_type struct */
      z_PaNtwrkAdrs.server_addr_type = (pdsm_server_address_e_type)p_Req->z_ParamVal.z_NtwrkAdrs.u_ServerAddrType;

      /* Fill in pdsm_server_address_u_type struct */
      if(z_PaNtwrkAdrs.server_addr_type == PDSM_SERVER_TYPE_IPV4_ADDRESS)
      {
         MSG_MED("Param: GPSDIAG_PA_[]_NETWORK_ADRS (IPV4)", 0, 0, 0);

         /* Verify that the parameter length is correct */
         if(p_Req->w_Length != (sizeof(uint8) + sizeof(gpsdiag_IpV4Type)))
         {
            /* Return a diag error packet indicating bad parameters were passed */
            return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
         }

         /* Fill in pdsm_server_ipv4_address_type struct */
         z_PaNtwrkAdrs.server_adrs.server_adrs_v4.port_id     = (dword)p_Req->z_ParamVal.z_NtwrkAdrs.z_ServerAddr.z_IpV4Data.q_ServerPort;
         z_PaNtwrkAdrs.server_adrs.server_adrs_v4.server_adrs = (dword)p_Req->z_ParamVal.z_NtwrkAdrs.z_ServerAddr.z_IpV4Data.q_ServerAdrs;
      }
      else if(z_PaNtwrkAdrs.server_addr_type == PDSM_SERVER_TYPE_IPV6_ADDRESS)
      {
         MSG_MED("Param: GPSDIAG_PA_[]_NETWORK_ADRS (IPV6)", 0, 0, 0);

         /* Verify that the parameter length is correct */
         if(p_Req->w_Length != (sizeof(uint8) + sizeof(gpsdiag_IpV6Type)))
         {
            /* Return a diag error packet indicating bad parameters were passed */
            return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
         }

         /* Fill in pdsm_server_ipv6_address_type struct */
         z_PaNtwrkAdrs.server_adrs.server_adrs_v6.port_id     = (dword)p_Req->z_ParamVal.z_NtwrkAdrs.z_ServerAddr.z_IpV6Data.q_ServerPort;
         p_ServerAddrIpV6                                   = (uint16*)p_Req->z_ParamVal.z_NtwrkAdrs.z_ServerAddr.z_IpV6Data.w_ServerAdrs;
         memscpy(z_PaNtwrkAdrs.server_adrs.server_adrs_v6.server_adrs,
		 	    sizeof(z_PaNtwrkAdrs.server_adrs.server_adrs_v6.server_adrs), p_ServerAddrIpV6, 
		 	    sizeof(z_PaNtwrkAdrs.server_adrs.server_adrs_v6.server_adrs));
      }
      else /* (z_PaNtwrkAdrs.server_addr_type == PDSM_SERVER_TYPE_URL_ADDRESS) */
      {
         MSG_MED("Param: GPSDIAG_PA_[]_NETWORK_ADRS (URL)", 0, 0, 0);

         /* Verify that the parameter length is correct */
         if(p_Req->w_Length != (sizeof(uint8) + sizeof(gpsdiag_UrlType)))
         {
            /* Return a diag error packet indicating bad parameters were passed */
            return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
         }

         /* Fill in pdsm_server_url_address_type struct */
         z_PaNtwrkAdrs.server_adrs.server_adrs_url.url_length = (uint8)p_Req->z_ParamVal.z_NtwrkAdrs.z_ServerAddr.z_UrlData.u_UrlLength;
         p_UrlValue                                          = (uint8*)p_Req->z_ParamVal.z_NtwrkAdrs.z_ServerAddr.z_UrlData.u_Url;
         memscpy(z_PaNtwrkAdrs.server_adrs.server_adrs_url.url, sizeof(z_PaNtwrkAdrs.server_adrs.server_adrs_url.url),
                p_UrlValue, z_PaNtwrkAdrs.server_adrs.server_adrs_url.url_length*sizeof(char));
      }

      z_ParamSetVal.pa_set = (pdsm_pa_e_type)p_Req->u_Param;
      z_ParamSetVal.info.server_address = z_PaNtwrkAdrs; //pdsm_server_address_s_type

      break;

   case GPSDIAG_PA_GPS_LOCK:
      MSG_MED("Param: GPSDIAG_PA_GPS_LOCK", 0, 0, 0);

      /* Verify that the parameter length is correct */
      if(p_Req->w_Length != sizeof(uint8))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      z_ParamSetVal.pa_set = PDSM_PA_GPS_LOCK; //pdsm_pa_e_type
      z_ParamSetVal.info.gps_lock = (pdsm_gps_lock_e_type)p_Req->z_ParamVal.u_GpsLock;

      break;


   case GPSDIAG_PA_KEEP_WARM:
      MSG_MED("Param: GPSDIAG_PA_KEEP_WARM", 0, 0, 0);

      /* Verify that the parameter length is correct */
      if(p_Req->w_Length != sizeof(boolean))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      z_ParamSetVal.pa_set = PDSM_PA_KEEP_WARM; //pdsm_pa_e_type
      z_ParamSetVal.info.keep_warm_enable = (boolean)p_Req->z_ParamVal.u_KeepWarm;

      break;


   case GPSDIAG_PA_DELETE_PARAMS_EX:
      MSG_MED("Param: GPSDIAG_PA_DELETE_PARAMS1", 0, 0, 0);

      /* Verify that the parameter length is correct */
      if(p_Req->w_Length != sizeof(gpsdiag_PaDelParamsTypeEx))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      (void) gpsdiag_TranslateGpsDiagToPdsmDeleteParams(&z_PaDelParams, &(p_Req->z_ParamVal.z_DelParams_ex.z_DelParams));

      z_PaDelParams.f_time_unc = p_Req->z_ParamVal.z_DelParams_ex.f_TimeUnc;
      z_PaDelParams.f_time_offset = p_Req->z_ParamVal.z_DelParams_ex.f_TimeOffset;
      z_ParamSetVal.pa_set = PDSM_PA_DELETE_PARAMS_EX1; //pdsm_pa_e_type
      z_ParamSetVal.info.delete_params = z_PaDelParams; //pdsm_delete_parms_type

      break;
   case GPSDIAG_PA_DELETE_PARAMS:
      MSG_MED("Param: GPSDIAG_PA_DELETE_PARAMS", 0, 0, 0);

      /* Verify that the parameter length is correct */
      if(p_Req->w_Length != sizeof(gpsdiag_PaDelParamsType))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      (void) gpsdiag_TranslateGpsDiagToPdsmDeleteParams(&z_PaDelParams, &(p_Req->z_ParamVal.z_DelParams));

      z_ParamSetVal.pa_set = PDSM_PA_DELETE_PARAMS_EX; //pdsm_pa_e_type
      z_ParamSetVal.info.delete_params = z_PaDelParams; //pdsm_delete_parms_type

      break;

   case GPSDIAG_PA_POSITION_MODE:
      MSG_MED("Param: GPSDIAG_PA_POSITION_MODE", 0, 0, 0);

      /* Verify that the parameter length is correct */
      if(p_Req->w_Length != sizeof(uint16))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      z_ParamSetVal.pa_set = PDSM_PA_POSITION_MODE; //pdsm_pa_e_type
      z_ParamSetVal.info.position_mode = (pdsm_position_mode_type)p_Req->z_ParamVal.q_PosMode; //uint32

      break;

   case GPSDIAG_PA_MT_LR_SUPPORT:
      MSG_MED("Param: GPSDIAG_PA_MT_LR_SUPPORT", 0, 0, 0);

      /* Verify that the parameter length is correct */
      if(p_Req->w_Length != sizeof(uint8))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      z_ParamSetVal.pa_set = PDSM_PA_MT_LR_SUPPORT; //pdsm_pa_e_type
      z_ParamSetVal.info.mt_lr_support = (pdsm_pa_mt_lr_support_e_type)p_Req->z_ParamVal.u_MtLrSpprt;

      break;

   case GPSDIAG_PA_MO_METHOD:
      MSG_MED("Param: GPSDIAG_PA_MO_METHOD", 0, 0, 0);

      /* Verify that the parameter length is correct */
      if(p_Req->w_Length != sizeof(uint8))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      z_ParamSetVal.pa_set = PDSM_PA_MO_METHOD; //pdsm_pa_e_type
      z_ParamSetVal.info.mo_method = (pdsm_pa_mo_method_e_type)p_Req->z_ParamVal.u_MoMethod;

      break;

   case GPSDIAG_PA_NMEA_SENTENCE_TYPE:
      MSG_MED("Param: GPSDIAG_PA_NMEA_SENTENCE_TYPE", 0, 0, 0);

      /* Verify that the parameter length is correct */
      if(p_Req->w_Length != sizeof(uint8))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      z_ParamSetVal.pa_set = PDSM_PA_NMEA_SENTENCE_TYPE; //pdsm_pa_e_type
      (void) gpsdiag_TranslateGpsDiagToPdsmNmeaType(&z_ParamSetVal.info.nmea_sentence_type,
                                                    p_Req->z_ParamVal.w_NmeaSentence);

      break;

   case GPSDIAG_PA_NMEA_CONFIG_INFO:
      MSG_MED("Param: GPSDIAG_PA_NMEA_CONFIG_INFO", 0, 0, 0);
      /* Verify that the parameter length is correct */
      if(p_Req->w_Length != sizeof(gpsdiag_PaNmeaConfigInfoType))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      z_ParamSetVal.pa_set = PDSM_PA_NMEA_CONFIG_INFO; //pdsm_pa_e_type
      z_ParamSetVal.info.nmea_config_info.pd_nmea_port_type = (pdsm_pa_nmea_port_e_type)p_Req->z_ParamVal.z_NmeaConfigInfo.q_NmeaPortType;
      z_ParamSetVal.info.nmea_config_info.pd_nmea_reporting_type =
      (pdsm_pa_nmea_reporting_e_type)p_Req->z_ParamVal.z_NmeaConfigInfo.q_NmeaReportingType;

      break;

   case GPSDIAG_PA_EFS_DATA:
      {
         MSG_MED("Param: GPSDIAG_PA_EFS_DATA", 0, 0, 0);
         /* check for valid request packet length */
         memset(&z_EfsData, 0, sizeof(z_EfsData));
         z_EfsData.efs_data_filename = (char*)u_Efs_FileName;
         for(i = 0; i< p_Req->z_ParamVal.z_EfsData.u_FullPathAndFileNameLength; i++)
         {
            z_EfsData.efs_data_filename[i] = (char)p_Req->z_ParamVal.z_EfsData.u_FilePathAndFileName[i];
         }
         z_EfsData.efs_data_filename[i] = '\0';
         z_EfsData.efs_data_ptr = (unsigned char *)u_Efs_Data;
         z_EfsData.efs_data_filename_len =  p_Req->z_ParamVal.z_EfsData.u_FullPathAndFileNameLength;
         z_EfsData.efs_data_file_operation = p_Req->z_ParamVal.z_EfsData.u_FileOperation;
         z_EfsData.efs_data_len = p_Req->z_ParamVal.z_EfsData.q_FileSize;
         z_EfsData.efs_data_reserved = 0;
         for(i = 0; i<p_Req->z_ParamVal.z_EfsData.q_FileSize;  i++)
         {
            z_EfsData.efs_data_ptr[i] = (unsigned char)p_Req->z_ParamVal.z_EfsData.u_FilePayload[i];
         }
         z_EfsData.efs_data_total_parts = p_Req->z_ParamVal.z_EfsData.u_Data_Total_Parts;
         z_EfsData.efs_data_part_number = p_Req->z_ParamVal.z_EfsData.u_Data_Part_Number;
         /* check for valid request packet length */
         z_ParamSetVal.pa_set = PDSM_PA_EFS_DATA;
         z_ParamSetVal.info.efs_data = z_EfsData;
      }
      break;

   case GPSDIAG_PA_SBAS_USER_PREFERENCE:
      MSG_MED("Param: GPSDIAG_PA_SBAS_USER_PREFERENCE", 0, 0, 0);

      /* Verify that the parameter length is correct */
      if(p_Req->w_Length != sizeof(uint8))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      z_ParamSetVal.pa_set = PDSM_PA_SBAS_USER_PREFERENCE; //pdsm_pa_e_type
      z_ParamSetVal.info.sbas_user_preference = (uint8)p_Req->z_ParamVal.u_SbasUserPreference;

      break;

   case GPSDIAG_PA_DPO_CONTROL:
      MSG_MED("Param: GPSDIAG_PA_DPO_CONTROL %d", p_Req->z_ParamVal.u_DpoControl, 0, 0);

      /* Verify that the parameter length is correct */
      if(p_Req->w_Length != sizeof(uint8))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      z_ParamSetVal.pa_set = PDSM_PA_DPO_CONTROL; //pdsm_pa_e_type
      z_ParamSetVal.info.dpo_control = (uint8)p_Req->z_ParamVal.u_DpoControl;

      break;

   case GPSDIAG_PA_ON_DEMAND_ENABLE:

      MSG_MED("Param: GPSDIAG_PA_ON_DEMAND_ENABLE", 0, 0, 0);

      /* Verify that the parameter length is correct */
      if(p_Req->w_Length != sizeof(uint8))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      z_ParamSetVal.pa_set = PDSM_PA_ON_DEMAND_LOW_POWER_MODE; //pdsm_pa_e_type
      z_ParamSetVal.info.on_demand_lpm = (boolean)p_Req->z_ParamVal.u_OnDemandLpm; //uint8

      break;

   case GPSDIAG_PA_ON_DEMAND_READY:

      MSG_MED("Param: GPSDIAG_PA_ON_DEMAND_READY", 0, 0, 0);

      /* Verify that the parameter length is correct */
      if(p_Req->w_Length != sizeof(uint8))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      z_ParamSetVal.pa_set = PDSM_PA_ON_DEMAND_READY_MODE; //pdsm_pa_e_type
      z_ParamSetVal.info.on_demand_ready = (boolean)p_Req->z_ParamVal.u_OnDemandReady; //uint8

      break;

   case GPSDIAG_PA_MOLR_AD_CALLFLOW_SELECT:
      MSG_MED("Param: GPSDIAG_PA_MOLR_AD_CALLFLOW_SELECT", 0, 0, 0);
      /* Verify that the parameter length is correct */
      if(p_Req->w_Length != sizeof(uint8))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }
      z_ParamSetVal.pa_set = PDSM_PA_CALL_FLOW_SELECT; //pdsm_pa_e_type
      z_ParamSetVal.info.call_flow_select_mask = p_Req->z_ParamVal.u_CallFlowSelectMask;
      break;

   case GPSDIAG_PA_SET_EXTERNAL_SPI:
      MSG_MED("Param: GPSDIAG_PA_SET_EXTERNAL_SPI", 0, 0, 0);
      if(p_Req->w_Length != sizeof(gpsdiag_XspiConfigType))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }
      z_ParamSetVal.pa_set = PDSM_PA_EXTERNAL_SPI;
      z_ParamSetVal.info.external_spi.mode = (p_Req->z_ParamVal.z_XspiConfig.u_Enabled == 0) ?
                                             PDSM_PA_EXTERNAL_SPI_MODE_DISABLE :
                                             PDSM_PA_EXTERNAL_SPI_MODE_ENABLE;
      break;

   case GPSDIAG_PA_XTRAT_CLIENT_TOKEN:
      MSG_MED("Param: GPSDIAG_PA_XTRAT_CLIENT_TOKEN", 0, 0, 0);
      if(p_Req->w_Length != sizeof(z_ParamSetVal.info.client_cookie))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }
      z_ParamSetVal.pa_set = PDSM_PA_XTRAT_CLIENT_TOKEN;
      z_ParamSetVal.info.client_cookie = p_Req->z_ParamVal.q_ClientCookie;
      break;

   case GPSDIAG_PA_XTRAT_USER_SESSION_CONTROL:
      if(p_Req->w_Length != sizeof(z_ParamSetVal.info.session_control))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }
      z_ParamSetVal.pa_set = PDSM_PA_XTRAT_USER_SESSION_CONTROL;
      z_ParamSetVal.info.session_control = p_Req->z_ParamVal.u_SessControl;
      MSG_MED("Param: GPSDIAG_PA_XTRAT_USER_SESSION_CONTROL setparam:%d  diag:%d",
              z_ParamSetVal.info.session_control, p_Req->z_ParamVal.u_SessControl, 0);
      break;

   case GPSDIAG_PA_DATA_ENABLE:
      if(p_Req->w_Length != sizeof(uint8))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }
      z_ParamSetVal.pa_set = PDSM_PA_DATA_ENABLE;
      z_ParamSetVal.info.data_enable = (boolean)p_Req->z_ParamVal.u_DataEnable; //uint8
      MSG_MED("Param: GPSDIAG_PA_DATA_ENABLE setparam Mask:%d  diag:%d",
              z_ParamSetVal.info.data_enable , p_Req->z_ParamVal.u_DataEnable, 0);
      break;

   case GPSDIAG_PA_LBS_APN_PROFILE_LIST:
      MSG_MED("Param: GPSDIAG_PA_LBS_APN_PROFILE_LIST", 0, 0, 0);
      if(p_Req->w_Length != sizeof(gpsdiag_apn_profiles_type) * C_GPSDIAG_MAX_APN_PROFILES)
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }
      z_ParamSetVal.pa_set = PDSM_PA_LBS_APN_PROFILE;
      gpsdiag_XlateToIntApnProfileList(p_Req->z_ParamVal.z_ApnProfiles_List,
                                       z_ParamSetVal.info.apn_profiles);
      break;

  case GPSDIAG_PA_XTRA_APN_PROFILE_LIST:
     MSG_MED("Param: GPSDIAG_PA_XTRA_APN_PROFILE_LIST", 0, 0, 0);
     if(p_Req->w_Length != sizeof(gpsdiag_apn_profiles_type) * C_GPSDIAG_MAX_APN_PROFILES)
     {
        /* Return a diag error packet indicating bad parameters were passed */
        return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
     }
     z_ParamSetVal.pa_set = PDSM_PA_XTRA_APN_PROFILE;
     gpsdiag_XlateToIntApnProfileList(p_Req->z_ParamVal.z_ApnProfiles_List,
                                      z_ParamSetVal.info.apn_profiles);
     break;

   case GPSDIAG_PA_SUPL_VERSION:
      if(p_Req->w_Length != sizeof(uint32))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }
      z_ParamSetVal.pa_set = PDSM_PA_SUPL_VERSION;
      z_ParamSetVal.info.supl_version = p_Req->z_ParamVal.u_SuplVersion; //uint32
      MSG_MED("Param: GPSDIAG_PA_SUPL_VERSION setparam Mask:%d  diag:%d",
              z_ParamSetVal.info.supl_version , p_Req->z_ParamVal.u_SuplVersion, 0);
      break;

   case GPSDIAG_PA_SUPL_SECURITY:
      if(p_Req->w_Length != sizeof(uint8))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }
      z_ParamSetVal.pa_set = PDSM_PA_SUPL_SECURITY;
      z_ParamSetVal.info.supl_security = (boolean)p_Req->z_ParamVal.u_SuplSecurity; //uint8
      MSG_MED("Param: GPSDIAG_PA_SUPL_SECURITY setparam Mask:%d  diag:%d",
              z_ParamSetVal.info.supl_security , p_Req->z_ParamVal.u_SuplSecurity, 0);
      break;

   case GPSDIAG_PA_LPP_ENABLE:
      if(p_Req->w_Length != sizeof(uint8))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }
      z_ParamSetVal.pa_set = PDSM_PA_LPP_CONFIG_INFO;
      z_ParamSetVal.info.lpp_config_info = (boolean)p_Req->z_ParamVal.u_lpp_enable; //uint8
      MSG_MED("Param: GPSDIAG_PA_LPP_CONFIG setparam Mask:%d  diag:%d",
              z_ParamSetVal.info.supl_security , p_Req->z_ParamVal.u_lpp_enable, 0);
      break;

   default:
      MSG_ERROR("Param: UNKNOWN PARAMETER %d!!", p_Req->u_Param, 0, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

  /* Now try setting the parameter */
  e_Result = pdsm_set_parameters(gpsdiag_SetParamCmdCb,
                                 (void*)&(gpsdiag_ClientTable[e_ClientType].e_ClientType),
                                 z_ParamSetVal.pa_set,
                                 &z_ParamSetVal.info,
                                 p_Req->l_ClientId);

   e_Status = (e_Result) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_SetParamCmdRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_SetParamCmdRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_SetParamCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = p_Req->l_ClientId;
      p_Rsp->q_Status      = (uint32)e_Status;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_SetParamCmd */

/*===========================================================================
FUNCTION gpsdiag_GetParamCmd

DESCRIPTION
  This function is used to get the PDSM parameters from the NV.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_GetParamCmd (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_GetParamCmdReqType* p_Req = (gpsdiag_GetParamCmdReqType*) p_PktReq;
   gpsdiag_GetParamCmdRspType* p_Rsp = NULL;
   gpsdiag_eStatusType         e_Status;
   boolean                     e_Result;
   uint32                      e_ClientType;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Map Client Id with its corresponding Client Type */
   e_ClientType = gpsdiag_GetClientType(p_Req->l_ClientId);

   /* Client Type Sanity Check */
   if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_GetParamCmd: Client Type Error %d", e_ClientType, 0, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      MSG_MED("gpsdiag_GetParamCmd: Client Type %d", e_ClientType, 0, 0);
   }

   /* Client ID Sanity Checks */
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId == GPSDIAG_INIT_VALUE)
   {
      MSG_ERROR("Must initialize PDAPI client first", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
   {
      MSG_ERROR("gpsdiag_GetParamCmd: UNKNOWN CLIENT ID!!", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }

   /* Verify that the parameter is within the specified range */
   if(p_Req->u_Param >= (uint32)GPSDIAG_PA_MAX)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

  /* Now try getting the parameter */
  e_Result = pdsm_get_parameters(gpsdiag_GetParamCmdCb,
                                 (void*)&(gpsdiag_ClientTable[e_ClientType].e_ClientType),
                                 (pdsm_pa_e_type)p_Req->u_Param,
                                 p_Req->l_ClientId);

   e_Status = (e_Result) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_GetParamCmdRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_GetParamCmdRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_GetParamCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = p_Req->l_ClientId;
      p_Rsp->q_Status      = (uint32)e_Status;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_GetParamCmd */

/*===========================================================================
FUNCTION gpsdiag_GetLastKnownPosCmd

DESCRIPTION
  This function is used to get the last known position.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_GetLastKnownPosCmd (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_GetLastKnownPosCmdReqType* p_Req = (gpsdiag_GetLastKnownPosCmdReqType*) p_PktReq;
   gpsdiag_GetLastKnownPosCmdRspType* p_Rsp = NULL;
   gpsdiag_eStatusType                e_Status;
   uint32                             e_ClientType;


   if (NULL == p_PktReq)
   {
     MSG_ERROR("gpsdiag_GetLastKnownCmd: Null pointer", 0, 0, 0);

     /* Return a diag error packet indicating bad parameters were passed */
     return (diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Map Client Id with its corresponding Client Type */
   e_ClientType = gpsdiag_GetClientType(p_Req->l_ClientId);

   /* Client Type Sanity Check */
   if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_GetLastKnownPosCmd: Client Type Error %d", e_ClientType, 0, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      MSG_MED("gpsdiag_GetLastKnownPosCmd: Client Type %d", e_ClientType, 0, 0);
   }

   /* Client ID Sanity Checks */
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId == GPSDIAG_INIT_VALUE)
   {
      MSG_ERROR("Must initialize PDAPI client first", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
   {
      MSG_ERROR("gpsdiag_GetLastKnownPosCmd: UNKNOWN CLIENT ID!!", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }

   e_Status = (gpsdiag_uPdDataValid) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_GetLastKnownPosCmdRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_GetLastKnownPosCmdRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_GetLastKnownPosCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = p_Req->l_ClientId;
      p_Rsp->q_Status      = (uint32)e_Status;

      /* Fill in pdsm_pd_data_type values */
      p_Rsp->l_Latitude       = (int32)  gpsdiag_pPdData.lat;
      p_Rsp->l_Longitude      = (int32)  gpsdiag_pPdData.lon;
      p_Rsp->q_TimeStamp      = (uint32) gpsdiag_pPdData.time_stamp; //dword
      p_Rsp->u_LocUncAng      = (uint8)  gpsdiag_pPdData.loc_uncertainty_ang; //byte
      p_Rsp->u_LocUncA        = (uint8)  gpsdiag_pPdData.loc_uncertainty_a; //byte
      p_Rsp->u_LocUncP        = (uint8)  gpsdiag_pPdData.loc_uncertainty_p; //byte
      p_Rsp->u_OptFieldMask   = (uint8)  gpsdiag_pPdData.opt_field_mask; //pdsm_pd_opt_field
      p_Rsp->x_Altitude       = (int16)  gpsdiag_pPdData.altitude;
      p_Rsp->w_Heading        = (uint16) gpsdiag_pPdData.heading; //word
      p_Rsp->w_VelocityHor    = (uint16) gpsdiag_pPdData.velocity_hor; //word
      p_Rsp->u_FixType        = (uint8)  gpsdiag_pPdData.fix_type; //boolean
      p_Rsp->b_VelocityVer    = (int8)   gpsdiag_pPdData.velocity_ver;
      p_Rsp->u_LocUncV        = (uint8)  gpsdiag_pPdData.loc_uncertainty_v; //byte
      p_Rsp->e_GpsPosMethod   = (uint8)  gpsdiag_pPdData.position_mode; //pdsm_pd_position_mode_e_type
      p_Rsp->u_PosSource      = (uint8)  gpsdiag_pPdData.positioning_source; //pdsm_pd_positioning_source_type
      p_Rsp->u_UncEllipseConf = (uint8)  gpsdiag_pPdData.loc_uncertainty_conf; //byte
      p_Rsp->b_UtcOffset      = (int8)   gpsdiag_pPdData.gpsUtcOffset;

      /* Fill in pdsm_pd_ext_pos_data_s_type values */
      p_Rsp->l_ExtLocLat      = (int32)  gpsdiag_pPdData.ext_pos_data.lat;
      p_Rsp->l_ExtLocLong     = (int32)  gpsdiag_pPdData.ext_pos_data.lon;
      p_Rsp->w_ExtLocUncA     = (uint16) gpsdiag_pPdData.ext_pos_data.alongAxisUnc;
      p_Rsp->w_ExtLocUncP     = (uint16) gpsdiag_pPdData.ext_pos_data.perpAxisUnc;
      p_Rsp->x_VelHoriz       = (int16)  gpsdiag_pPdData.ext_pos_data.velHor;
      p_Rsp->x_VelVert        = (int16)  gpsdiag_pPdData.ext_pos_data.velVert;
      p_Rsp->q_TStampTowMs    = (uint32) gpsdiag_pPdData.ext_pos_data.timestamp_tow_ms; //dword
      p_Rsp->w_TStampGpsWeek  = (uint16) gpsdiag_pPdData.ext_pos_data.timestamp_gps_week;
      p_Rsp->u_PosReportToNw  = (uint8)  gpsdiag_pPdData.ext_pos_data.pos_reported_to_network; //boolean
      p_Rsp->w_VelUncHoriz    = (uint16) gpsdiag_pPdData.ext_pos_data.velUncHoriz;
      p_Rsp->w_VelUncVert     = (uint16) gpsdiag_pPdData.ext_pos_data.velUncVert;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_GetLastKnownPosCmd */

   #if INCLUDE_LCS_CMDS

/*===========================================================================
FUNCTION gpsdiag_NtfyVrfySndUsrActnCmd

DESCRIPTION
  This function is used to inform the PDAPI from the UI, of the response to
  the incoming request for positioning request msg.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_NtfyVrfySndUsrActnCmd (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_NtfyVrfySndUsrActnCmdReqType* p_Req = (gpsdiag_NtfyVrfySndUsrActnCmdReqType*) p_PktReq;
   gpsdiag_NtfyVrfySndUsrActnCmdRspType* p_Rsp = NULL;
   gpsdiag_eStatusType                   e_Status;
   boolean                               e_Result;
   uint32                                e_ClientType;

      #if 0
   pdsm_supl_slp_address                 z_SlpAddr;
   uint16*                               p_SlpAddrIpV6;
   uint8*                                p_SlpAddrUrlValue;
      #endif /* 0 */

   /* Notify Verify Send User Action Data Structures from PDSM */
   pdsm_lcs_notification_resp_s_type     z_LcsNotifRsp;

      #if 0
   uint8*                                p_DataString;
      #endif

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Map Client Id with its corresponding Client Type */
   e_ClientType = gpsdiag_GetClientType(p_Req->l_ClientId);

   /* Client Type Sanity Check */
   if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_NtfyVrfySndUsrActnCmd: Client Type Error %d", e_ClientType, 0, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      MSG_MED("gpsdiag_NtfyVrfySndUsrActnCmd: Client Type %d", e_ClientType, 0, 0);
   }

   /* Client ID Sanity Checks */
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId == GPSDIAG_INIT_VALUE)
   {
      MSG_ERROR("Must initialize PDAPI client first", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
   {
      MSG_ERROR("gpsdiag_NtfyVrfySndUsrActnCmd: UNKNOWN CLIENT ID!!", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }

   /* Verify that certain particular enumerated input values are valid */
   if((p_Req->u_AcceptDeny >= (uint8)GPSDIAG_NVSUA_MAX) ||
      (p_Req->u_CmdType    >= (uint8)GPSDIAG_NVSUA_CMD_MAX))
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Fill in pdsm_server_address_s_type struct */
   z_LcsNotifRsp.resp              = (pdsm_lcs_agent_notification_resp_e_type)p_Req->u_AcceptDeny;
   z_LcsNotifRsp.lcs_info_cmd_type = (pdsm_lcs_info_cmd_e_type)p_Req->u_CmdType;

   /* Fill in pdsm_lcs_info_u_type struct */
   if(p_Req->u_CmdType == GPSDIAG_NVSUA_CMD_VX_REQ)
   {
      /* Fill in pdsm_lcs_vx_notify_verify_req_s_type struct */
      z_LcsNotifRsp.lcs_info.vx_req.notification_priv_type = (pdsm_lcs_notify_verify_e_type)p_Req->z_RespVal.z_VxData.u_NotifPrivType;
      z_LcsNotifRsp.lcs_info.vx_req.pos_tech               = (pdsm_lcs_pos_tech_indicator_e_type)p_Req->z_RespVal.z_VxData.u_PosTechIndicator;
      z_LcsNotifRsp.lcs_info.vx_req.pos_qos_incl           = (boolean)p_Req->z_RespVal.z_VxData.u_PosQosPresent;
      z_LcsNotifRsp.lcs_info.vx_req.pos_qos                = (uint8)p_Req->z_RespVal.z_VxData.u_PosQos;
      z_LcsNotifRsp.lcs_info.vx_req.num_fixes              = (dword)p_Req->z_RespVal.z_VxData.q_NumFixes;
      z_LcsNotifRsp.lcs_info.vx_req.tbf                    = (dword)p_Req->z_RespVal.z_VxData.q_TimeBwFixes;
      z_LcsNotifRsp.lcs_info.vx_req.pos_mode               = (pdsm_lcs_pos_mode_e_type)p_Req->z_RespVal.z_VxData.u_PositionMode;
      z_LcsNotifRsp.lcs_info.vx_req.encoding_scheme        = (pdsm_lcs_requester_id_encoding_scheme_e_type)p_Req->z_RespVal.z_VxData.u_DataCodingScheme;
      z_LcsNotifRsp.lcs_info.vx_req.user_resp_timer_val    = p_Req->z_RespVal.z_VxData.w_UserRspTime;

      //<<<[* by Liao]
      z_LcsNotifRsp.lcs_info.vx_req.requester_id.requester_id_length = 0;
      #if 0
      /* Verify that the requester ID length is correct */
      if(p_Req->z_RespVal.z_VxData.u_ReqIdLength < sizeof(p_Req->z_RespVal.z_VxData.u_ReqId))
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      /* Fill in pdsm_lcs_requester_id_s_type struct */
      z_LcsNotifRsp.lcs_info.vx_req.requester_id.requester_id_length = (byte)p_Req->z_RespVal.z_VxData.u_ReqIdLength;
      p_ReqIdData = (uint8*)p_Req->z_RespVal.z_VxData.u_ReqId;
      memscpy(z_LcsNotifRsp.lcs_info.vx_req.requester_id.requester_id,
              sizeof(z_LcsNotifRsp.lcs_info.vx_req.requester_id.requester_id) 
              p_ReqIdData, p_Req->z_RespVal.z_VxData.u_ReqIdLength);
      #endif
      //>>>[* End]
   }

   else if(p_Req->u_CmdType == GPSDIAG_NVSUA_CMD_SUPL_REQ)
   {
      /* Verify if saved SUPL data corresponds to this Notify Verify Send User Action Cmd */
/*     if (p_Req->z_RespVal.z_SuplData.z_SuplSlpSessId.u_Presence && gpsdiag_zSuplData.supl_slp_session_id.presence &&          */
/*         p_Req->z_RespVal.z_SuplData.z_SuplSlpSessId.u_SessionId[0] == gpsdiag_zSuplData.supl_slp_session_id.session_id[0] && */
/*         p_Req->z_RespVal.z_SuplData.z_SuplSlpSessId.u_SessionId[1] == gpsdiag_zSuplData.supl_slp_session_id.session_id[1] && */
/*         p_Req->z_RespVal.z_SuplData.z_SuplSlpSessId.u_SessionId[2] == gpsdiag_zSuplData.supl_slp_session_id.session_id[2] && */
/*         p_Req->z_RespVal.z_SuplData.z_SuplSlpSessId.u_SessionId[3] == gpsdiag_zSuplData.supl_slp_session_id.session_id[3] && */
/*         gpsdiag_uSuplDataValid)                                                                                              */
/*     {                                                                                                                        */
      MSG_MED("gpsdiag_NtfyVrfySndUsrActnCmd: SUPL data verified!!", 0, 0, 0);

      /* Fill in pdsm_lcs_supl_notify_verify_req_s_type struct */
      z_LcsNotifRsp.lcs_info.supl_req = gpsdiag_zSuplData;
/*     }                                                                              */
/*     else                                                                           */
/*     {                                                                              */
/*       MSG_ERROR("gpsdiag_NtfyVrfySndUsrActnCmd: SUPL data unverified!!", 0, 0, 0); */
/*     }                                                                              */

      #if 0
      /* Fill in pdsm_lcs_supl_notify_verify_req_s_type struct */
      z_LcsNotifRsp.lcs_info.supl_req.notification_priv_type = (pdsm_lcs_notify_verify_e_type)p_Req->z_RespVal.z_SuplData.u_NotifPrivType;
      z_LcsNotifRsp.lcs_info.supl_req.flags                  = (uint16)p_Req->z_RespVal.z_SuplData.w_LcsFlags;

      /* Fill in pdsm_supl_slp_session_id_s_type struct */
      z_LcsNotifRsp.lcs_info.supl_req.supl_slp_session_id.presence = (boolean)p_Req->z_RespVal.z_SuplData.z_SuplSlpSessId.u_Presence;
      p_DataString = (uint8*)p_Req->z_RespVal.z_SuplData.z_SuplSlpSessId.u_SessionId;
      memscpy(z_LcsNotifRsp.lcs_info.supl_req.supl_slp_session_id.session_id,
              sizeof(z_LcsNotifRsp.lcs_info.supl_req.supl_slp_session_id.session_id),
              p_DataString, 
              PDSM_SUPL_SLP_SESSION_ID_BYTE_LENGTH);

      /* Fill in pdsm_supl_slp_address struct */
      z_SlpAddr = z_LcsNotifRsp.lcs_info.supl_req.supl_slp_session_id.slp_address;
      z_SlpAddr.choice = (pdsm_server_address_e_type)p_Req->z_RespVal.z_SuplData.z_SuplSlpSessId.z_SlpAddr.u_SlpAddrType;

      /* Fill in pdsm_server_address_u_type struct */
      if(z_SlpAddr.choice == PDSM_SERVER_TYPE_IPV4_ADDRESS)
      {
         MSG_MED("gpsdiag_NtfyVrfySndUsrActnCmd: SUPL SLP Address (IPV4)", 0, 0, 0);

         /* Fill in pdsm_server_ipv4_address_type struct */
         z_SlpAddr.supl_slp_address.server_adrs_v4.port_id     = (dword)p_Req->z_RespVal.z_SuplData.z_SuplSlpSessId.z_SlpAddr.z_SlpAddr.z_IpV4Data.q_ServerPort;
         z_SlpAddr.supl_slp_address.server_adrs_v4.server_adrs = (dword)p_Req->z_RespVal.z_SuplData.z_SuplSlpSessId.z_SlpAddr.z_SlpAddr.z_IpV4Data.q_ServerAdrs;
      }
      else if(z_SlpAddr.choice == PDSM_SERVER_TYPE_IPV6_ADDRESS)
      {
         MSG_MED("gpsdiag_NtfyVrfySndUsrActnCmd: SUPL SLP Address (IPV6)", 0, 0, 0);

         /* Fill in pdsm_server_ipv6_address_type struct */
         z_SlpAddr.supl_slp_address.server_adrs_v6.port_id  = (dword)p_Req->z_RespVal.z_SuplData.z_SuplSlpSessId.z_SlpAddr.z_SlpAddr.z_IpV6Data.q_ServerPort;
         p_SlpAddrIpV6                                    = (uint16*)p_Req->z_RespVal.z_SuplData.z_SuplSlpSessId.z_SlpAddr.z_SlpAddr.z_IpV6Data.w_ServerAdrs;
         memscpy(z_SlpAddr.supl_slp_address.server_adrs_v6.server_adrs,
                sizeof(z_SlpAddr.supl_slp_address.server_adrs_v6.server_adrs),
                p_SlpAddrIpV6,
                8*sizeof(uint16));
      }
      else /* (z_SlpAddr.choice == PDSM_SERVER_TYPE_URL_ADDRESS) */
      {
         MSG_MED("gpsdiag_NtfyVrfySndUsrActnCmd: SUPL SLP Address (URL)", 0, 0, 0);

         /* Fill in pdsm_server_url_address_type struct */
         z_SlpAddr.supl_slp_address.server_adrs_url.url_length = (uint8)p_Req->z_RespVal.z_SuplData.z_SuplSlpSessId.z_SlpAddr.z_SlpAddr.z_UrlData.u_UrlLength;
         p_SlpAddrUrlValue                                    = (uint8*)p_Req->z_RespVal.z_SuplData.z_SuplSlpSessId.z_SlpAddr.z_SlpAddr.z_UrlData.u_Url;
         memscpy(z_SlpAddr.supl_slp_address.server_adrs_url.url,
                sizeof(z_SlpAddr.supl_slp_address.server_adrs_url.url),
                p_SlpAddrUrlValue,
                z_SlpAddr.supl_slp_address.server_adrs_url.url_length*sizeof(uint8));
      }

      /* Fill in HASH for SUPL_INIT array */
      p_DataString = (uint8*)p_Req->z_RespVal.z_SuplData.u_SuplHash;
      memscpy(z_LcsNotifRsp.lcs_info.supl_req.supl_hash,
             sizeof(z_LcsNotifRsp.lcs_info.supl_req.supl_hash),
             p_DataString, 
             PDSM_SUPL_HASH_LEN);

      z_LcsNotifRsp.lcs_info.supl_req.datacoding_scheme = (pdsm_lcs_notify_verify_datacoding_scheme_e_type)p_Req->z_RespVal.z_SuplData.u_DatacodingScheme;
      z_LcsNotifRsp.lcs_info.supl_req.pos_method        = (pdsm_lcs_pos_method_e_type)p_Req->z_RespVal.z_SuplData.u_SuplPosMethod;

      /* Fill in pdsm_lcs_requestor_id_s_type struct */
      z_LcsNotifRsp.lcs_info.supl_req.requestor_id.data_coding_scheme = (byte)p_Req->z_RespVal.z_SuplData.z_RequestorId.u_DatacodingScheme;
      z_LcsNotifRsp.lcs_info.supl_req.requestor_id.string_len         = (uint8)p_Req->z_RespVal.z_SuplData.z_RequestorId.u_ReqIdStrLen;
      /* Fill in requestor_id_string array */
      p_DataString = (uint8*)p_Req->z_RespVal.z_SuplData.z_RequestorId.u_ReqIdStr;
      memscpy(z_LcsNotifRsp.lcs_info.supl_req.requestor_id.requestor_id_string,
              sizeof(z_LcsNotifRsp.lcs_info.supl_req.requestor_id.requestor_id_string),
              p_DataString, z_LcsNotifRsp.lcs_info.supl_req.requestor_id.string_len);

      /* Fill in pdsm_lcs_client_name_s_type struct */
      z_LcsNotifRsp.lcs_info.supl_req.client_name.data_coding_scheme = (byte)p_Req->z_RespVal.z_SuplData.z_ClientName.u_DatacodingScheme;
      z_LcsNotifRsp.lcs_info.supl_req.client_name.string_len         = (uint8)p_Req->z_RespVal.z_SuplData.z_ClientName.u_ClientNameStrLen;
      /* Fill in requestor_id_string array */
      p_DataString = (uint8*)p_Req->z_RespVal.z_SuplData.z_ClientName.u_ClientNameStr;
      memscpy(z_LcsNotifRsp.lcs_info.supl_req.client_name.client_name_string,
              sizeof(z_LcsNotifRsp.lcs_info.supl_req.client_name.client_name_string)
              p_DataString, z_LcsNotifRsp.lcs_info.supl_req.client_name.string_len);

      /* Fill in pdsm_lcs_supl_qop_s_type struct */
      z_LcsNotifRsp.lcs_info.supl_req.supl_qop.bit_mask  = (uint8)p_Req->z_RespVal.z_SuplData.z_SuplQop.u_BitMask;
      z_LcsNotifRsp.lcs_info.supl_req.supl_qop.horacc    = (uint8)p_Req->z_RespVal.z_SuplData.z_SuplQop.u_HorAcc;
      z_LcsNotifRsp.lcs_info.supl_req.supl_qop.veracc    = (uint8)p_Req->z_RespVal.z_SuplData.z_SuplQop.u_VerAcc;
      z_LcsNotifRsp.lcs_info.supl_req.supl_qop.maxLocAge = (uint16)p_Req->z_RespVal.z_SuplData.z_SuplQop.w_MaxLocAge;
      z_LcsNotifRsp.lcs_info.supl_req.supl_qop.delay     = (uint8)p_Req->z_RespVal.z_SuplData.z_SuplQop.u_Delay;

      z_LcsNotifRsp.lcs_info.supl_req.user_response_timer = (uint8)p_Req->z_RespVal.z_SuplData.w_UserRspTime;
      #endif /* 0 */
   }

   else if(p_Req->u_CmdType == GPSDIAG_NVSUA_CMD_UMTS_CP_REQ)
   {
      /* Verify if saved UMTS CP data corresponds to this Notify Verify Send User Action Cmd */
/*     if (p_Req->z_RespVal.z_UmtsCPData.u_SessInvokeId == gpsdiag_zUmtsCpData.invoke_id && */
/*         gpsdiag_uUmtsCpDataValid)                                                        */
/*     {                                                                                    */
      MSG_MED("gpsdiag_NtfyVrfySndUsrActnCmd: UMTS CP data verified", 0, 0, 0);

      /* Fill in pdsm_lcs_umts_cp_notify_verify_req_s_type struct */
      z_LcsNotifRsp.lcs_info.umts_cp_req = gpsdiag_zUmtsCpData;
/*     }                                                                               */
/*     else                                                                            */
/*     {                                                                               */
/*       MSG_ERROR("gpsdiag_NtfyVrfySndUsrActnCmd: UMTS CP data unverified", 0, 0, 0); */
/*     }                                                                               */

      #if 0
      /* Fill in pdsm_lcs_umts_cp_notify_verify_req_s_type struct */
      z_LcsNotifRsp.lcs_info.umts_cp_req.notification_priv_type = (pdsm_lcs_notify_verify_e_type)p_Req->z_UmtsCPData.;
      z_LcsNotifRsp.lcs_info.umts_cp_req.invoke_id              = (uint8)p_Req->z_UmtsCPData.;
      z_LcsNotifRsp.lcs_info.umts_cp_req.flags                  = (uint16)p_Req->z_UmtsCPData.;
      z_LcsNotifRsp.lcs_info.umts_cp_req.notification_length    = (uint8)p_Req->z_UmtsCPData.;
      z_LcsNotifRsp.lcs_info.umts_cp_req.notification_text      = (char*)p_Req->z_UmtsCPData.;
      z_LcsNotifRsp.lcs_info.umts_cp_req.datacoding_scheme      = (pdsm_lcs_notify_verify_datacoding_scheme_e_type)p_Req->z_UmtsCPData.;
      z_LcsNotifRsp.lcs_info.umts_cp_req.ext_client_address_data= (pdsm_lcs_ext_client_address_s_type)p_Req->z_UmtsCPData.;
      z_LcsNotifRsp.lcs_info.umts_cp_req.location_type          = (pdsm_lcs_location_type_e_type)p_Req->z_UmtsCPData.;
      z_LcsNotifRsp.lcs_info.umts_cp_req.deferred_location      = (pdsm_lcs_deferred_location_s_type)p_Req->z_UmtsCPData.;
      z_LcsNotifRsp.lcs_info.umts_cp_req.requestor_id           = (pdsm_lcs_requestor_id_s_type)p_Req->z_UmtsCPData.;
      z_LcsNotifRsp.lcs_info.umts_cp_req.codeword_string        = (pdsm_lcs_codeword_string_s_type)p_Req->z_UmtsCPData.;
      z_LcsNotifRsp.lcs_info.umts_cp_req.service_type_id        = (pdsm_lcs_service_type_ID_s_type)p_Req->z_UmtsCPData.;
      z_LcsNotifRsp.lcs_info.umts_cp_req.user_response_timer    = (uint8)p_Req->z_UmtsCPData.;
      #endif /* 0 */
   }
   else /* Other protocol type () */
   {
      MSG_ERROR("gpsdiag_NtfyVrfySndUsrActnCmd: Unknown CmdType!!", 0, 0, 0);
      // PDSM_LCS_CMD_TYPE_VX_SERVICE_INTERACTION
      // PDSM_LCS_CMD_TYPE_CLIENT_TYPE
      // PDSM_LCS_CMD_TYPE_VX_CONFIG
   }

   /* Now try responding to a notification verification request */
   e_Result = pdsm_lcs_agent_client_response(gpsdiag_LcsCmdCb, /* pdsm_lcs_cmd_cb_f_type */
                                             (void*)&(gpsdiag_ClientTable[e_ClientType].e_ClientType),
                                             (pdsm_lcs_notification_resp_s_type*)&z_LcsNotifRsp,
                                             p_Req->l_ClientId);

   e_Status = (e_Result) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_NtfyVrfySndUsrActnCmdRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_NtfyVrfySndUsrActnCmdRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_NtfyVrfySndUsrActnCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = p_Req->l_ClientId;
      p_Rsp->q_Status      = (uint32)e_Status;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_NtfyVrfySndUsrActnCmd */


/*===========================================================================
FUNCTION gpsdiag_LcsVxInitClientCmd

DESCRIPTION
  LCS Vx Init Client Command.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_LcsVxInitClientCmd (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_LcsVxInitClientCmdReqType* p_Req = (gpsdiag_LcsVxInitClientCmdReqType*) p_PktReq;
   gpsdiag_LcsVxInitClientCmdRspType* p_Rsp = NULL;
   gpsdiag_eStatusType                e_Status;
   boolean                            e_ResultBool;
   uint32                             e_ClientType;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Map Client Id with its corresponding Client Type */
   e_ClientType = p_Req->q_ClientType;

   /* Client Type Sanity Check */
   if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_LcsVxInitClientCmd: Client Type Error %d", e_ClientType, 0, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      MSG_MED("gpsdiag_LcsVxInitClientCmd: Client Type %d", e_ClientType, 0, 0);
   }

   /* Client Type Sanity Check */
   if(gpsdiag_ClientTable[e_ClientType].e_ClientType != (pdsm_client_type_e_type)e_ClientType)
   {
      MSG_ERROR("gpsdiag_LcsVxInitClientCmd: UNKNOWN CLIENT TYPE!!", 0, 0, 0);
   }
   e_ResultBool = pdsm_lcs_client_init(p_Req->q_ClassId,
                                       (pdsm_client_type_e_type)e_ClientType,
                                       p_Req->u_IsBrewApp);
   e_Status = (e_ResultBool) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_LcsVxInitClientCmdRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_LcsVxInitClientCmdRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_LcsVxInitClientCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = (uint32)gpsdiag_ClientTable[e_ClientType].l_ClientId;
      p_Rsp->q_Status      = (uint32)e_Status;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_LcsVxInitClientCmd */

/*===========================================================================
FUNCTION gpsdiag_LcsVxInitClientAppInfoCmd

DESCRIPTION
  LCS Vx Init Client App Info Command.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_LcsVxInitClientAppInfoCmd (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_LcsVxInitClientAppInfoCmdReqType* p_Req = (gpsdiag_LcsVxInitClientAppInfoCmdReqType*) p_PktReq;
   gpsdiag_LcsVxInitClientAppInfoCmdRspType* p_Rsp = NULL;
   gpsdiag_eStatusType                       e_Status = GPSDIAG_SUCCESS;
   boolean                                   e_ResultBool;
   uint32                                    e_ClientType;

   /* Function parameters Sanity Check */
   if ((p_Req == NULL) ||
       (p_Req->q_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
        p_Req->q_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE) )
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      /* Map Client Id with its corresponding Client Type */
      e_ClientType = p_Req->q_ClientType;
      MSG_MED("gpsdiag_LcsVxInitClientAppInfoCmd: Client Type %d", e_ClientType, 0, 0);
   }

   /* Client Type Sanity Check */
   if (gpsdiag_ClientTable[e_ClientType].e_ClientType != (pdsm_client_type_e_type)e_ClientType)
   {
      MSG_ERROR("gpsdiag_LcsVxInitClientAppInfoCmd: UNKNOWN CLIENT TYPE!!", 0, 0, 0);
   }

   e_ResultBool = pdsm_lcs_client_app_info_init(p_Req->q_ClassId,
                                                (pdsm_client_type_e_type)e_ClientType,
                                                (char*)p_Req->u_AppInfo,
                                                p_Req->u_IsBrewApp);
   e_Status = (e_ResultBool) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_LcsVxInitClientAppInfoCmdRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_LcsVxInitClientAppInfoCmdRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_LcsVxInitClientAppInfoCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = gpsdiag_ClientTable[e_ClientType].l_ClientId;
      p_Rsp->q_Status      = (uint32)e_Status;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_LcsVxInitClientAppInfoCmd */

/*===========================================================================
FUNCTION gpsdiag_LcsVxReleaseClientCmd

DESCRIPTION
  LCS Vx Release Client Command.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_LcsVxReleaseClientCmd (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_LcsVxReleaseClientCmdReqType* p_Req = (gpsdiag_LcsVxReleaseClientCmdReqType*) p_PktReq;
   gpsdiag_LcsVxReleaseClientCmdRspType* p_Rsp = NULL;
   gpsdiag_eStatusType                   e_Status = GPSDIAG_SUCCESS;
   boolean                               e_ResultBool;
   uint32                                e_ClientType;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Map Client Id with its corresponding Client Type */
   e_ClientType = p_Req->q_ClientType;

   /* Client Type Sanity Check */
   if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_LcsVxReleaseClientCmd: Client Type Error %d", e_ClientType, 0, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      MSG_MED("gpsdiag_LcsVxReleaseClientCmd: Client Type %d", e_ClientType, 0, 0);
   }

   /* Client Type Sanity Check */
   if(gpsdiag_ClientTable[e_ClientType].e_ClientType != (pdsm_client_type_e_type)e_ClientType)
   {
      MSG_ERROR("gpsdiag_LcsVxReleaseClientCmd: UNKNOWN CLIENT TYPE!!", 0, 0, 0);
   }
   e_ResultBool = pdsm_lcs_client_release(NULL, /* gpsdiag_LcsCmdCb */
                                          NULL, /* Client Data Ptr */
                                          p_Req->q_ClassId,
                                          (pdsm_client_type_e_type)e_ClientType);
   e_Status = (e_ResultBool) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_LcsVxReleaseClientCmdRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_LcsVxReleaseClientCmdRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_LcsVxReleaseClientCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = (uint32)gpsdiag_ClientTable[e_ClientType].l_ClientId;
      p_Rsp->q_Status      = (uint32)e_Status;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_LcsVxReleaseClientCmd */

/*===========================================================================
FUNCTION gpsdiag_LcsVxQueryConfigCmd

DESCRIPTION
  LCS Vx Query Config Command.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_LcsVxQueryConfigCmd (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_LcsVxQueryConfigCmdReqType* p_Req = (gpsdiag_LcsVxQueryConfigCmdReqType*) p_PktReq;
   gpsdiag_LcsVxQueryConfigCmdRspType* p_Rsp = NULL;
   gpsdiag_eStatusType                 e_Status = GPSDIAG_SUCCESS;
   boolean                             e_ResultBool;
   uint32                              e_ClientType;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Map Client Id with its corresponding Client Type */
   e_ClientType = p_Req->q_ClientType;

   /* Client Type Sanity Check */
   if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_LcsVxQueryConfigCmd: Client Type Error %d", e_ClientType, 0, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      MSG_MED("gpsdiag_LcsVxQueryConfigCmd: Client Type %d", e_ClientType, 0, 0);
   }

   /* Client Type Sanity Check */
   if(gpsdiag_ClientTable[e_ClientType].e_ClientType != (pdsm_client_type_e_type)e_ClientType)
   {
      MSG_ERROR("gpsdiag_LcsVxQueryConfigCmd: UNKNOWN CLIENT TYPE!!", 0, 0, 0);
   }
   e_ResultBool = pdsm_lcs_query_vx_config((pdsm_client_type_e_type)e_ClientType);
   e_Status = (e_ResultBool) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_LcsVxQueryConfigCmdRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_LcsVxQueryConfigCmdRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_LcsVxQueryConfigCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = (uint32)gpsdiag_ClientTable[e_ClientType].l_ClientId;
      p_Rsp->q_Status      = (uint32)e_Status;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_LcsVxQueryConfigCmd */

/*===========================================================================
FUNCTION gpsdiag_LcsVxGetClientTypeCmd

DESCRIPTION
  LCS Vx Get Client Type Command.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_LcsVxGetClientTypeCmd (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_LcsVxGetClientTypeCmdReqType* p_Req = (gpsdiag_LcsVxGetClientTypeCmdReqType*) p_PktReq;
   gpsdiag_LcsVxGetClientTypeCmdRspType* p_Rsp = NULL;
   pdsm_client_type_e_type               e_ClientType;
   pdsm_client_type_e_type               q_ClientTypeVal;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /*lint -e{571} q_ClientTypeVal is an index to an array and is bounds checked after it is set */
   q_ClientTypeVal = pdsm_lcs_get_client_type(p_Req->q_ClassId);
   if(q_ClientTypeVal >= ARR_SIZE(gpsdiag_ClientTable) || q_ClientTypeVal < 0)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /*lint -e{571} e_ClientType is an index to an array and is bounds checed after it is set */
   e_ClientType = gpsdiag_ClientTable[q_ClientTypeVal].e_ClientType;

   /* Client Type Sanity Check */
   if(e_ClientType >= ARR_SIZE(gpsdiag_ClientTable) || e_ClientType < 0)
   {
      MSG_ERROR("gpsdiag_LcsVxGetClientTypeCmd: Client Type Error %d", e_ClientType, 0, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_LcsVxGetClientTypeCmdRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_LcsVxGetClientTypeCmdRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_LcsVxGetClientTypeCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->q_ClientType  = (uint32)q_ClientTypeVal;
      p_Rsp->l_ClientId    = (uint32)gpsdiag_ClientTable[e_ClientType].l_ClientId;
      /* If we got this far, we were successful */
      p_Rsp->q_Status      = (uint32)GPSDIAG_SUCCESS;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_LcsVxGetClientTypeCmd */

/*===========================================================================
FUNCTION gpsdiag_LcsVxGetStatusCmd

DESCRIPTION
  LCS Vx Get Status Type Command.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_LcsVxGetStatusCmd (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_LcsVxGetStatusCmdReqType* p_Req = (gpsdiag_LcsVxGetStatusCmdReqType*) p_PktReq;
   gpsdiag_LcsVxGetStatusCmdRspType* p_Rsp = NULL;
//  gpsdiag_eStatusType               e_Status = GPSDIAG_SUCCESS;
   boolean                           e_ResultBool;
   uint32                            e_ClientType;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Map Client Id with its corresponding Client Type */
   e_ClientType = p_Req->q_ClientType;

   /* Client Type Sanity Check */
   if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_LcsVxGetStatusCmd: Client Type Error %d", e_ClientType, 0, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      MSG_MED("gpsdiag_LcsVxGetStatusCmd: Client Type %d", e_ClientType, 0, 0);
   }

   /* Client Type Sanity Check */
   if(gpsdiag_ClientTable[e_ClientType].e_ClientType != (pdsm_client_type_e_type)e_ClientType)
   {
      MSG_ERROR("gpsdiag_LcsVxGetStatusCmd: UNKNOWN CLIENT TYPE!!", 0, 0, 0);
   }
   e_ResultBool = pdsm_lcs_get_status();
//  e_Status = (e_ResultBool) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_LcsVxGetStatusCmdRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_LcsVxGetStatusCmdRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_LcsVxGetStatusCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = (uint32)gpsdiag_ClientTable[e_ClientType].l_ClientId;
      p_Rsp->q_Status      = (uint32)e_ResultBool;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_LcsVxGetStatusCmd */

   #endif /* INCLUDE_LCS_CMDS */

/*===========================================================================
FUNCTION gpsdiag_Trigger_Xtrat_Session

DESCRIPTION
This function Starts / Stops an XTRAT session

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void*  gpsdiag_Trigger_Xtrat_Session (PACKED void* p_PktReq, uint16 w_PktLength)
{
#ifdef FEATURE_CGPS_XTRA_T
  gpsdiag_Trigger_Xtrat_Session_req_type* p_Req = (gpsdiag_Trigger_Xtrat_Session_req_type*) p_PktReq;
  gpsdiag_Trigger_Xtrat_Session_rsp_type* p_Rsp = NULL;
  tle_request_session_s_type XTRAT_req_session;
  boolean session_trigger_success;

  /* Function parameters Sanity Check */
  if(p_Req == NULL)
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }

  if (p_Req->session_control == 0)
  {
    XTRAT_req_session.e_req_type = TLE_SESSION_REQUEST_STOP;
    XTRAT_req_session.e_source = TLE_SESSION_SOURCE_TEST;
    XTRAT_req_session.u_req_data.e_end_reason = TLE_SESS_END_SESS_NONE;

    session_trigger_success = tle_RequestSession(&XTRAT_req_session);
    if (session_trigger_success)
    {
      MSG_MED("gpsdiag_Trigger_Xtrat_Session: An IPC to stop xtrat session was sent", 0, 0, 0);
    }
    else
    {
      MSG_MED("gpsdiag_XtraT_Cmd: Failed to send an IPC to stop xtrat session", 0, 0, 0);
    }
  }
  else if(p_Req->session_control == 1)
  {
    XTRAT_req_session.e_req_type = TLE_SESSION_REQUEST_START;
    XTRAT_req_session.e_source = TLE_SESSION_SOURCE_TEST;
    XTRAT_req_session.u_req_data.data_xfr_type = (TLE_DATA_XFR_UPLOAD_MASK | TLE_DATA_XFR_DNLOAD_MASK);
    session_trigger_success = tle_RequestSession(&XTRAT_req_session);

    if (session_trigger_success)
    {
      MSG_MED("gpsdiag_Trigger_Xtrat_Session: An IPC to start xtrat session was sent", 0, 0, 0);
    }
    else
    {
      MSG_MED("gpsdiag_XtraT_Cmd: Failed to send an IPC to start xtrat session", 0, 0, 0);
    }
  }
  else
  {
    MSG_MED("gpsdiag_XtraT_Cmd: Bad parameters were passed to XTRAT session controller -choose between either 0-stop session /1-trigger a new session", 0, 0, 0);
    session_trigger_success = FALSE;
    /* Return a diag error packet indicating bad parameters were passed */
    return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_Trigger_Xtrat_Session_rsp_type*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,CGPS_DIAG_PDAPI_CMD,sizeof(gpsdiag_Trigger_Xtrat_Session_rsp_type) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_Trigger_Xtrat_Session: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
     //fill in th eappropriate response parameters
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->q_Status      = (uint32)session_trigger_success;
   }

   return p_Rsp;
#else
   return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
#endif
}


/*===========================================================================
FUNCTION gpsdiag_GetPdapiVersCmd

DESCRIPTION
  This function is used to get the version of the PDAPI that is used for
  diagnostic messages. This version number is used to send the appropriate
  GPSDIAG messages.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_GetPdapiVersCmd (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_GetPdapiVersReqType* p_Req = (gpsdiag_GetPdapiVersReqType*) p_PktReq;
   gpsdiag_GetPdapiVersRspType* p_Rsp = NULL;
   uint32                       q_VersionNum;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   MSG_MED("gpsdiag_GetPdapiVersCmd: Accessing PDAPI version number...", 0, 0, 0);

   /* Get the PDAPI version number */
   q_VersionNum = pdsm_get_pdapi_version();

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_GetPdapiVersRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_GetPdapiVersRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_GetPdapiVersCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->u_MajorRev    = (uint8)q_VersionNum;
      p_Rsp->u_MinorRev    = 0; //AHTODO: How to determine major/minor?
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_GetPdapiVersCmd */

/*===========================================================================
FUNCTION gpsdiag_InjectTimeCmd

DESCRIPTION
  This function is used to inject External Time to the GPS engine through
  the PDAPI.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_InjectTimeCmd(PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_InjectExtTimeReqType* p_Req = (gpsdiag_InjectExtTimeReqType*) p_PktReq;
   gpsdiag_InjectExtTimeRspType* p_Rsp = NULL;
   gpsdiag_eStatusType                e_Status;
   uint32                             e_ClientType;
   pdsm_pd_external_time_info_type    z_ExtTimeInfo;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Map Client Id with its corresponding Client Type */
   e_ClientType = gpsdiag_GetClientType(p_Req->l_ClientId);

   /* Client Type Sanity Check */
   if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_InjectTimeCmd: Client Type Error %d", e_ClientType, 0, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      MSG_MED("gpsdiag_InjectTimeCmd: Client Type %d", e_ClientType, 0, 0);
   }

   /* Client ID Sanity Checks */
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId == GPSDIAG_INIT_VALUE)
   {
      MSG_ERROR("Must initialize PDAPI client first", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
   {
      MSG_ERROR("gpsdiag_InjectTimeCmd: UNKNOWN CLIENT ID!!", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }

   /* Fill in pdsm_pd_external_time_info_type struct */
   z_ExtTimeInfo.time_msec = (uint64) p_Req->q_UpperTimeMsec << 32 | p_Req->q_LowerTimeMsec;

   z_ExtTimeInfo.time_unc_msec       = p_Req->q_TimeUncMsec;
   z_ExtTimeInfo.time_source         = (pdsm_pd_clk_src_e_type)p_Req->u_TimeSource;
   z_ExtTimeInfo.time_is_gps         = p_Req->u_TimeIsGps;
   z_ExtTimeInfo.discontinuity_count = p_Req->u_DiscontinuityCount;
   z_ExtTimeInfo.force_flag          = p_Req->u_ForceFlag;

   /* Now try injecting the external time info */
   (void)pdsm_pd_inject_external_time(NULL, NULL, &z_ExtTimeInfo, p_Req->l_ClientId);

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_InjectExtTimeRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_InjectExtTimeRspType) );
   e_Status = GPSDIAG_SUCCESS;

   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_InjectTimeCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = p_Req->l_ClientId;
      p_Rsp->q_Status      = (uint32)e_Status;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_InjectTimeCmd */


/*===========================================================================
FUNCTION gpsdiag_ExternCoarsePosInjCmd

DESCRIPTION
  This function is used to inject external coarse position to the GPS engine
  through the PDAPI.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_ExternCoarsePosInjCmd(PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_InjectExternCoasePosReqType* p_Req = (gpsdiag_InjectExternCoasePosReqType*) p_PktReq;
   gpsdiag_InjectExternCoasePosRspType* p_Rsp = NULL;

   gpsdiag_eStatusType                e_Status = GPSDIAG_SUCCESS;
   uint32                             e_ClientType;
   pdsm_pd_external_pos_info_type     z_ext_pos_info;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   /* Map Client Id with its corresponding Client Type */
   e_ClientType = gpsdiag_GetClientType(p_Req->l_ClientId);

   /* Client Type Sanity Check */
   if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_ExternCoarsePosInjCmd: Client Type Error!! %lu, %lu", e_ClientType, p_Req->l_ClientId, 0);

      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   else
   {
      MSG_MED("gpsdiag_ExternCoarsePosInjCmd: Client Type %d", e_ClientType, 0, 0);
   }

   /* Client ID Sanity Checks */
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId == GPSDIAG_INIT_VALUE)
   {
      MSG_ERROR("Must initialize PDAPI client first", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }
   if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
   {
      MSG_ERROR("gpsdiag_ExternCoarsePosInjCmd: UNKNOWN CLIENT ID!!", 0, 0, 0);

      /* Return a diag error packet indicating packet not allowed in this mode */
      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
   }

   z_ext_pos_info.opt_field_mask =
   (pdsm_pd_extern_coarse_pos_opt_field)p_Req->opt_field_mask;

   z_ext_pos_info.time_info_type =
   (pdsm_pd_extern_coarse_pos_time_stamp_e_type)p_Req->time_info_type;

   z_ext_pos_info.timestamp_sec = p_Req->timestamp_sec;

   z_ext_pos_info.source =
   (pdsm_pd_extern_coarse_pos_source_e_type)p_Req->source;

   z_ext_pos_info.latitude = p_Req->latitude;
   z_ext_pos_info.longitude = p_Req->longitude;
   z_ext_pos_info.loc_unc_hor = p_Req->loc_unc_hor;
   z_ext_pos_info.alt_def = (pdsm_pd_extern_coarse_pos_alt_def_e_type)p_Req->alt_def;
   z_ext_pos_info.altitude = p_Req->altitude;
   z_ext_pos_info.loc_unc_vert = p_Req->loc_unc_vert;
   z_ext_pos_info.confidence_hor = p_Req->confidence_hor;
   z_ext_pos_info.confidence_vert = p_Req->confidence_vert;

   e_Status = pdsm_pd_inject_external_pos(
                                         gpsdiag_InjectExternPosCmdCb,                             /* Command callback */
                                         (void*)&(gpsdiag_ClientTable[e_ClientType].e_ClientType), /* user data pointer */
                                         &z_ext_pos_info,                                          /* external position */
                                         p_Req->l_ClientId                                        /* Client id of user */
                                         )
              ? GPSDIAG_SUCCESS
              : GPSDIAG_ERROR ;

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_InjectExternCoasePosRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_InjectExternCoasePosRspType) );

   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_ExternCoarsePosInjCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = p_Req->l_ClientId;
      p_Rsp->q_Status      = (uint32)e_Status;
   }

   return p_Rsp;
} /* gpsdiag_ExternCoarsePosInjCmd */


/*===========================================================================
FUNCTION gpsdiag_QueryGpsStateCmd

DESCRIPTION
  This function is used to Query the GPS engine state through
  the PDAPI.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* gpsdiag_QueryGpsStateCmd(PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_QueryGpsStateReqType* p_Req = (gpsdiag_QueryGpsStateReqType*) p_PktReq;
   gpsdiag_QueryGpsStateRspType* p_Rsp = NULL;
   gpsdiag_eStatusType                e_Status;
   uint32                             q_ClientType;
   uint32                             e_CmdErrorType = 0;
   pdsm_client_id_type                l_ClientId;
   pdsm_pd_ext_gps_state_s_type z_GpsStateInfo;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }
   l_ClientId = p_Req->l_ClientId;

   /* Map Client Id with its corresponding Client Type */
   q_ClientType = gpsdiag_GetClientType(l_ClientId);

   /* Client Type Sanity Check */
   if(q_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
      q_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_QueryGpsStateCmd: Client Type Error %d", q_ClientType, 0, 0);
      e_CmdErrorType = DIAG_BAD_PARM_F;
   }
   else
   {
      MSG_MED("gpsdiag_QueryGpsStateCmd: Client Type %d", q_ClientType, 0, 0);

      /* Client ID Sanity Checks */
      if((gpsdiag_ClientTable[q_ClientType].l_ClientId == GPSDIAG_INIT_VALUE) ||
         (gpsdiag_ClientTable[q_ClientType].l_ClientId != l_ClientId))
      {
         MSG_ERROR("Must initialize PDAPI client first", 0, 0, 0);
         e_CmdErrorType = DIAG_BAD_MODE_F;
      }
   }

   if(e_CmdErrorType)
   {
      return(diagpkt_err_rsp((diagpkt_cmd_code_type)e_CmdErrorType, p_PktReq, w_PktLength));
   }

   /* Now Query GPS state info */
   (void)pdsm_pd_query_gps_state(NULL, NULL, &z_GpsStateInfo, l_ClientId);

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_QueryGpsStateRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_QueryGpsStateRspType) );
   e_Status = GPSDIAG_SUCCESS;

   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_QueryGpsStateCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId    = p_Req->l_ClientId;
      p_Rsp->q_Status      = (uint32)e_Status;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_InjectTimeCmd */

/*===========================================================================
FUNCTION gpsdiag_process_XtraCmdCb

DESCRIPTION
  This function processes the Event callbacks for  XTRA commands.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void gpsdiag_process_XtraCmdCb( void *p_DataBlock,
                                pdsm_xtra_cmd_e_type e_XtraCmd,
                                pdsm_xtra_cmd_err_e_type e_XtraCmdErr)
{
  pdsm_xtra_status_e_type  e_XtraStatus = XTRA_DOWNLOAD_STATUS_FAILURE;

  MSG_MED("gpsdiag_process_XtraCmdCb: user_data ptr 0x%x, cmdtype %d, cmderr %d",
    p_DataBlock, e_XtraCmd, e_XtraCmdErr);

  if ( e_XtraCmd >= PDSM_XTRA_CMD_MAX )
  {
    MSG_ERROR("gpsdiag_process_XtraCmdCb: Invalid command %d",
               e_XtraCmd,0,0);
    return;
  }
  if ( e_XtraCmdErr > PDSM_XTRA_CMD_ERR_OTHER )
  {
    MSG_ERROR("gpsdiag_process_XtraCmdCb: Invalid command error %d",
               e_XtraCmdErr,0,0);
    return;
  }

  switch ( e_XtraCmd )
  {
  case PDSM_XTRA_CMD_SET_DATA:
    MSG_MED("gpsdiag_process_XtraCmdCb:XTRA Cmd SET DATA (%d)", e_XtraCmd,0,0);

    /* Convert the command status code */
    if ( PDSM_XTRA_CMD_ERR_NONE==e_XtraCmdErr || PDSM_XTRA_CMD_ERR_NOERR==e_XtraCmdErr)
    {
      e_XtraStatus = XTRA_DOWNLOAD_STATUS_SUCCESS;
    }
    else
    {
       e_XtraStatus = GPSDIAG_XTRACMDERR_TO_STATUS[e_XtraCmdErr];
       MSG_ERROR("gpsdiag_process_XtraCmdCb: Xtra Data Pkt Unsuccesful err:%d",
                  e_XtraCmdErr,0,0);
    }
    /* Send the DATA ack event to Diag Client or QPST */
    diag_SendXtraDataAckEvent(e_XtraStatus);

    break;

  case PDSM_XTRA_CMD_TIME_INFO_INJ:
    MSG_MED("gpsdiag_process_XtraCmdCb:XTRA Cmd TIME INJECT (%d)",
             e_XtraCmdErr,0,0);
    break;

  case PDSM_XTRA_CMD_INITIATE_DOWNLOAD:
    MSG_MED("gpsdiag_process_XtraCmdCb:XTRA Cmd INIT DOWNLOAD Err (%d)",
             e_XtraCmdErr,0,0);
    break;

  case PDSM_XTRA_CMD_SET_AUTO_DOWNLOAD_PARAMS:
    MSG_MED("gpsdiag_process_XtraCmdCb:XTRA Cmd SET DLD PARAMS Err (%d)",
             e_XtraCmdErr,0,0);
    break;

  case PDSM_XTRA_CMD_QUERY_DATA_VALIDITY:
    MSG_MED("gpsdiag_process_XtraCmdCb:XTRA Cmd QUERY DATA  Err (%d)",
             e_XtraCmd, e_XtraCmdErr,0);
    break;
  default:
    MSG_MED("gpsdiag_process_XtraCmdCb:XTRA Cmd ignored (%d)", e_XtraCmd,0,0);
    break;
  }

}/* gpsdiag_process_XtraCmdCb */

/*===========================================================================
FUNCTION gpsdiag_InjectXtraDataCmd

DESCRIPTION
  This procedure handles the Xtra Data Inject Diag command by requesting the
  appropriate PDAPI command.

DEPENDENCIES
  This client should have first been initialized, registered, and activated
  via the gpsdiag_ApiCmdsDispatch() and gpsdiag_RegCmdsDispatch() functions.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* gpsdiag_InjectXtraDataCmd(PACKED void* p_PktReq, uint16 w_PktLength)
{
  gpsdiag_XtraDataInjectReqType* p_Req = (gpsdiag_XtraDataInjectReqType*) p_PktReq;
  gpsdiag_XtraDataInjectRspType* p_Rsp = NULL;
  gpsdiag_eStatusType      e_Status = GPSDIAG_ERROR;
  uint32                   e_ClientType;
  boolean                  u_Status = FALSE;

  /* Function parameters Sanity Check */
  if(p_Req == NULL)
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }

  /* Map Client Id with its corresponding Client Type */
  e_ClientType = gpsdiag_GetClientType(p_Req->l_ClientId);

  /* Client Type Sanity Check */
  if(e_ClientType >= (uint32)PDSM_CLIENT_TYPE_MAX ||
     e_ClientType == (uint32)PDSM_CLIENT_TYPE_NONE)
  {
    MSG_ERROR("gpsdiag_InjectXtraDataCmd: Client Type Error type:%d id:%d",
               e_ClientType, p_Req->l_ClientId, 0);

    /* Return a diag error packet indicating bad parameters were passed */
    return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }
  else
  {
    MSG_MED("gpsdiag_InjectXtraDataCmd: Client Type %d id:%d", e_ClientType,
             p_Req->l_ClientId, 0);
  }

  /* Client ID Sanity Checks */
  if(gpsdiag_ClientTable[e_ClientType].l_ClientId == GPSDIAG_INIT_VALUE)
  {
    MSG_ERROR("Must initialize PDAPI client first", 0, 0, 0);

    /* Return a diag error packet indicating packet not allowed in this mode */
    return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
  }

  if(gpsdiag_ClientTable[e_ClientType].l_ClientId != p_Req->l_ClientId)
  {
    MSG_ERROR("gpsdiag_InjectXtraDataCmd: UNKNOWN CLIENT ID!! 1:%d 2:%d",
               gpsdiag_ClientTable[e_ClientType].l_ClientId,
               p_Req->l_ClientId, 0);

    /* Return a diag error packet indicating packet not allowed in this mode */
    return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
  }

  if ( p_Req->q_Xtra_Data_Len > C_GPSDIAG_XTRA_DATA_MAX_PAYLOAD )
  {
    MSG_ERROR ("gpsdiag_InjectXtraDataCmd, Payload exceeds limit,Xtralen:%d part:%d Totlen:%d",
                p_Req->q_Xtra_Data_Len, p_Req->q_Data_Part_Number, w_PktLength);

  }
  else
  {
    u_Status = pdsm_xtra_set_data ( gpsdiag_process_XtraCmdCb,
                                  gpsdiag_ClientTable[PDSM_CLIENT_TYPE_XTRA].l_ClientId,
                                  (void*)&gpsdiag_ClientTable[PDSM_CLIENT_TYPE_XTRA].l_ClientId,
                                  p_Req->q_Xtra_Data_Len,
                                  p_Req->u_Xtra_Data_Load,
                                  (uint8)p_Req->q_Data_Part_Number,
                                  (uint8)p_Req->q_Data_Total_Parts,
                                  PDSM_XTRA_STATUS_SUCCESS);

    /* In case of failure, nofity this through QPST Message */
    if (FALSE == u_Status)
    {
      MSG_ERROR ("gpsdiag_InjectXtraDataCmd, Injection rejected,  datelen: %d  segment:%d  totalparts:%d",
                 p_Req->q_Xtra_Data_Len, p_Req->q_Data_Part_Number, p_Req->q_Data_Total_Parts);
    }
    else
    {
      MSG_HIGH("gpsdiag_InjectXtraDataCmd, Injection sucessful,  datelen: %d  segment:%d  totalparts:%d",
                p_Req->q_Xtra_Data_Len, p_Req->q_Data_Part_Number, p_Req->q_Data_Total_Parts);

    }
    e_Status = (u_Status == TRUE) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;
  }

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_XtraDataInjectRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_XtraDataInjectRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_InjectXtraDataCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
      p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
      p_Rsp->l_ClientId                 = p_Req->l_ClientId;
      p_Rsp->q_Status                   = (uint32)e_Status;
      p_Rsp->q_Xtra_Data_Len            = p_Req->q_Xtra_Data_Len ;
      p_Rsp->q_Data_Part_Number         = p_Req->q_Data_Part_Number;
      p_Rsp->q_Data_Total_Parts         = p_Req->q_Data_Total_Parts;
      p_Rsp->p_Client_Data_Ptr          = (uint32)&gpsdiag_ClientTable[PDSM_CLIENT_TYPE_XTRA].l_ClientId;
   }

   return p_Rsp;
} /* gpsdiag_InjectXtraDataCmd */


/*===========================================================================
FUNCTION gpsdiag_Req1xEvdoRfInfoCmd

DESCRIPTION
  This function is used to trigger 1xEVDO RF Info Request. This command
  requests some 1xEVDO specific parameters from 1xEVDO module. This set
  of parameters is referred to as "RF Info".

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_Req1xEvdoRfInfoCmd (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_1xEvdoRfInfoReqType* p_Req = (gpsdiag_1xEvdoRfInfoReqType*) p_PktReq;
   gpsdiag_1xEvdoRfInfoRspType* p_Rsp = NULL;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   if(w_PktLength != sizeof(gpsdiag_1xEvdoRfInfoReqType))
   {
     /* Return a diag error packet indicating bad packet length */
      return (diagpkt_err_rsp(DIAG_BAD_LEN_F, p_PktReq, w_PktLength));
   }

   /* Send EVDO RF Info request to PGI module */
   mgp_RfInfoReq( SOURCE_EVDO, FALSE );

   /* Allocate space through diag for response packet */
   p_Rsp = (gpsdiag_1xEvdoRfInfoRspType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                 CGPS_DIAG_PDAPI_CMD,
                                 sizeof(gpsdiag_1xEvdoRfInfoRspType) );
   /* Fill in response packet */
   if(p_Rsp == NULL)
   {
      MSG_ERROR("gpsdiag_Req1xEvdoRfInfoCmd: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Rsp->q_Status      = (uint32)GPSDIAG_SUCCESS;
   }

   /* Send response packet to diag */
//  diagpkt_commit((PACKED void*)p_Rsp);

   return p_Rsp;
} /* gpsdiag_Req1xEvdoRfInfoCmd */


/*===========================================================================
FUNCTION gpsdiag_PdapiCmdHandler

DESCRIPTION
  This procedure handles the diag command by requesting the appropriate PDAPI
  command.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* gpsdiag_PdapiCmdHandler (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_ApiCmdReqType* p_Req = (gpsdiag_ApiCmdReqType*) p_PktReq;
   uint8 u_CmdCode;
   uint8 u_SubsysId;
   uint16 w_SubsysCmdCode;

   /* Function parameters Sanity Check */
   if(p_Req == NULL)
   {
      /* Return a diag error packet indicating bad parameters were passed */
      return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
   }

   u_CmdCode = p_Req->z_DiagCmdHdr.u_CmdCode;
   u_SubsysId = p_Req->z_DiagCmdHdr.u_SubsysId;
   w_SubsysCmdCode = p_Req->z_DiagCmdHdr.w_SubsysCmdCode;

   /* Check to see if cmd_code is correct */
   if(u_CmdCode != (uint8)DIAG_SUBSYS_CMD_F)
   {
      MSG_ERROR("CmdCode NOT DIAG_SUBSYS_CMD_F.", 0, 0, 0);

      /* Return a diag error packet indicating invalid command */
      return(diagpkt_err_rsp(DIAG_BAD_CMD_F, p_PktReq, w_PktLength));
   }

   /* Check to see if subsys_id is correct */
   if(u_SubsysId != (uint8)DIAG_SUBSYS_GPS)
   {
      MSG_ERROR("SubsysId NOT DIAG_SUBSYS_GPS.", 0, 0, 0);

      /* Return a diag error packet indicating invalid command */
      return(diagpkt_err_rsp(DIAG_BAD_CMD_F, p_PktReq, w_PktLength));
   }

   /* Dispatch GPS Subsystem requests to PDAPI */
   if(w_SubsysCmdCode == (uint16)CGPS_DIAG_PDAPI_CMD)
   {
      MSG_HIGH("gpsdiag_PdapiCmdHandler - CgpsCmdCode:%d len:%d",
               p_Req->z_DiagCmdHdr.u_CgpsCmdCode, w_PktLength, 0);

      if(p_Req->z_DiagCmdHdr.u_CgpsCmdCode == CGPS_INIT_CMD ||
         (p_Req->z_DiagCmdHdr.u_CgpsCmdCode >= CGPS_ACTIVATE_CMD &&
          p_Req->z_DiagCmdHdr.u_CgpsCmdCode <= CGPS_RELEASE_CMD) )
      {
         return gpsdiag_ApiCmdsDispatch(p_PktReq, w_PktLength);
      }
      else if(p_Req->z_DiagCmdHdr.u_CgpsCmdCode >= CGPS_REGISTER_PD_CMD &&
              p_Req->z_DiagCmdHdr.u_CgpsCmdCode <= CGPS_REGISTER_EXT_STAT_CMD)
      {
         return gpsdiag_RegCmdsDispatch(p_PktReq, w_PktLength);
      }
      else
      {
         switch(p_Req->z_DiagCmdHdr.u_CgpsCmdCode)
         {
         case CGPS_XTRA_INJECT_DATA_CMD:
            MSG_MED("CgpsCmdCode: CGPS_XTRA_INJECT_DATA_CMD pktlen:%d",
                     w_PktLength, 0, 0);
            return gpsdiag_InjectXtraDataCmd(p_PktReq, w_PktLength);

         case CGPS_XTRA_CLIENT_REGISTER_CMD:
            MSG_MED("CgpsCmdCode: CGPS_XTRA_CLIENT_REGISTER_CMD pktlen:%d",
                     w_PktLength, 0, 0);
            return gpsdiag_XtraRegCmd(p_PktReq, w_PktLength);

         case CGPS_START_CMD:
            MSG_MED("CgpsCmdCode: CGPS_START_CMD", 0, 0, 0);
            return gpsdiag_StartCmd(p_PktReq, w_PktLength);

         case CGPS_END_CMD:
            MSG_MED("CgpsCmdCode: CGPS_END_CMD", 0, 0, 0);
            return gpsdiag_EndCmd(p_PktReq, w_PktLength);

         case CGPS_SET_PARAM_CMD:
            MSG_MED("CgpsCmdCode: CGPS_SET_PARAM_CMD", 0, 0, 0);
            return gpsdiag_SetParamCmd(p_PktReq, w_PktLength);

         case CGPS_GET_PARAM_CMD:
            MSG_MED("CgpsCmdCode: CGPS_GET_PARAM_CMD", 0, 0, 0);
            return gpsdiag_GetParamCmd(p_PktReq, w_PktLength);

         case CGPS_GET_LAST_KNOWN_POS_CMD:
            MSG_MED("CgpsCmdCode: CGPS_GET_LAST_KNOWN_POS_CMD", 0, 0, 0);
            return gpsdiag_GetLastKnownPosCmd(p_PktReq, w_PktLength);

         case CGPS_GET_PDAPI_VERS_CMD:
            MSG_MED("CgpsCmdCode: CGPS_GET_PDAPI_VERS_CMD", 0, 0, 0);
            return gpsdiag_GetPdapiVersCmd(p_PktReq, w_PktLength);

         case CGPS_NTFY_VRFY_SND_USR_ACTN_CMD:
            MSG_MED("CgpsCmdCode: CGPS_NTFY_VRFY_SND_USR_ACTN_CMD", 0, 0, 0);
            return gpsdiag_NtfyVrfySndUsrActnCmd(p_PktReq, w_PktLength);

         case CGPS_INJECT_TIME_CMD:
            MSG_MED("CgpsCmdCode: CGPS_INJECT_TIME_CMD", 0, 0, 0);
            return gpsdiag_InjectTimeCmd(p_PktReq, w_PktLength);

         case CGPS_EXTERN_COARSE_POS_INJ_CMD:
            MSG_MED("CgpsCmdCode: CGPS_EXTERN_COARSE_POS_INJ_CMD", 0, 0, 0);
            return gpsdiag_ExternCoarsePosInjCmd(p_PktReq, w_PktLength);

         case CGPS_LCS_VX_INIT_CLIENT_CMD:
            MSG_MED("CgpsCmdCode: CGPS_LCS_VX_INIT_CLIENT_CMD", 0, 0, 0);
            return gpsdiag_LcsVxInitClientCmd(p_PktReq, w_PktLength);

         case CGPS_LCS_VX_INIT_CLIENT_APP_INFO_CMD:
            MSG_MED("CgpsCmdCode: CGPS_LCS_VX_INIT_CLIENT_APP_INFO_CMD", 0, 0, 0);
            return gpsdiag_LcsVxInitClientAppInfoCmd(p_PktReq, w_PktLength);

         case CGPS_LCS_VX_RELEASE_CLIENT_CMD:
            MSG_MED("CgpsCmdCode: CGPS_LCS_VX_RELEASE_CLIENT_CMD", 0, 0, 0);
            return gpsdiag_LcsVxReleaseClientCmd(p_PktReq, w_PktLength);

         case CGPS_LCS_VX_QUERY_CONFIG_CMD:
            MSG_MED("CgpsCmdCode: CGPS_LCS_VX_QUERY_CONFIG_CMD", 0, 0, 0);
            return gpsdiag_LcsVxQueryConfigCmd(p_PktReq, w_PktLength);

         case CGPS_LCS_VX_GET_CLIENT_TYPE_CMD:
            MSG_MED("CgpsCmdCode: CGPS_LCS_VX_GET_CLIENT_TYPE_CMD", 0, 0, 0);
            return gpsdiag_LcsVxGetClientTypeCmd(p_PktReq, w_PktLength);

         case CGPS_LCS_VX_GET_STATUS_CMD:
            MSG_MED("CgpsCmdCode: CGPS_LCS_VX_GET_STATUS_CMD", 0, 0, 0);
            return gpsdiag_LcsVxGetStatusCmd(p_PktReq, w_PktLength);

         case CGPS_QUERY_GPS_STATE_CMD:
            MSG_MED("CgpsCmdCode: CGPS_QUERY_GPS_STATE_CMD", 0, 0, 0);
            return gpsdiag_QueryGpsStateCmd(p_PktReq, w_PktLength);

         case CGPS_TRIGGER_XTRAT_SESSION:
           MSG_MED("CgpsCmdCode: CGPS_TRIGGER_XTRAT_SESSION", 0, 0, 0);
           return gpsdiag_Trigger_Xtrat_Session(p_PktReq, w_PktLength);

         case CGPS_TEST_FORCE_MEM_ALLOC_ERROR:
           MSG_MED("CgpsCmdCode: CGPS_TEST_FORCE_MEM_ALLOC_ERROR", 0, 0, 0);
           return gpsdiag_TestForceMemAllocError(p_PktReq, w_PktLength);

         case CGPS_QUERY_VERSION:
            MSG_MED("CgpsCmdCode: CGPS_QUERY_VERSION", 0, 0, 0);
            return gpsdiag_QueryVersionCmd(p_PktReq, w_PktLength);

         case CGPS_INJECT_EXTERNAL_SPI:
            MSG_MED("CgpsCmdCode: CGPS_INJECT_EXTERNAL_SPI", 0, 0, 0);
            return gpsdiag_HandleExternalSPIInjection(p_PktReq, w_PktLength);

         case CGPS_OEM_CONTROL:
            MSG_MED("CgpsCmdCode: CGPS_OEM_CONTROL", 0, 0, 0);
            return gpsdiag_OemControl(p_PktReq, w_PktLength);

         case CGPS_OEM_DRSYNC_SET_CFG:
           MSG_MED("CgpsCmdCode: CGPS_OEM_DRSYNC_SET_CFG", 0, 0, 0);
           return gpsdiag_OemDrSyncSetCfg(p_PktReq, w_PktLength);

         case CGPS_INTERNAL_REQ_1XEVDO_RF_INFO_CMD:
            MSG_MED("CgpsCmdCode: CGPS_INTERNAL_REQ_1XEVDO_RF_INFO_CMD", 0, 0, 0);
            return gpsdiag_Req1xEvdoRfInfoCmd(p_PktReq, w_PktLength);

         case CGPS_GEOFENCE_IN_MEM_LOG_CMD:
            MSG_MED("CgpsCmdCode: CGPS_GEOFENCE_IN_MEM_LOG_CMD", 0, 0, 0);
            return gpsdiag_GeofenceInMemLog(p_PktReq, w_PktLength);

         default:
            MSG_ERROR("CgpsCmdCode: UNKNOWN GPS CMD CODE!!", 0, 0, 0);

            /* Return a diag error packet indicating invalid command */
            return(diagpkt_err_rsp(DIAG_BAD_CMD_F, p_PktReq, w_PktLength));
         }
      }
   }
   else
   {
      MSG_ERROR("SubsysCmdCode NOT CGPS_DIAG_PDAPI_CMD.", 0, 0, 0);

      /* Return a diag error packet indicating invalid command */
      return(diagpkt_err_rsp(DIAG_BAD_CMD_F, p_PktReq, w_PktLength));
   }
} /* gpsdiag_PdapiCmdHandler */

#endif /* INCLUDE_PDAPI_TEST */


/*===========================================================================
FUNCTION gpsdiag_XtraEventCb

DESCRIPTION
  This function processes XTRA events.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void gpsdiag_XtraEventCb( void *p_UserData,
                          pdsm_xtra_event_type  u_XtraEvent,
                          const pdsm_xtra_info_s_type *p_XtraInfo )
{

  MSG_HIGH("gpsdiag_XtraEventCb: Event %d, usr ptr 0x%x", u_XtraEvent, p_UserData, 0);

  if (p_XtraInfo == NULL)
  {
    MSG_ERROR(" gpsdiag_XtraEventCb: No Valid Information in Callback", 0, 0, 0);
    return;
  }

  /* Check if event is valid */
  if ( u_XtraEvent & (~PDSM_XTRA_EVENT_MASK) )
  {
    MSG_ERROR("gpsdiag_XtraEventCb:Invalid event:%d id:$d",
               u_XtraEvent,p_XtraInfo->client_id,0);
    return;
  }
  switch( u_XtraEvent )
  {
  case PDSM_XTRA_EVENT_STATUS:
    /* This is a status only event */
    if (p_XtraInfo->xtra_union.xtra_download_info.flags == XTRA_DOWNLOAD_INFO_FLAGS_STATUS)
    {
      /* Do Nothing here, this is response to set auto download parameters */
      MSG_MED("gpsdiag_XtraEventCb:autoDld Event Ignored: STATUS (%d)",
               p_XtraInfo->xtra_union.xtra_download_info.status,0,0);
     }
    /* Query data validity response */
    else if (((p_XtraInfo->xtra_union.xtra_download_info.flags & XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_WEEK) &&
        (p_XtraInfo->xtra_union.xtra_download_info.flags & XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_MINS) &&
        (p_XtraInfo->xtra_union.xtra_download_info.flags & XTRA_DOWNLOAD_INFO_FLAGS_VALID_DURATION))!= 0)
    {
      MSG_MED("gpsdiag_XtraEventCb: DATA VALIDITY INFO Event",
               0,0,0);
    }
    break;

  case PDSM_XTRA_EVENT_DOWNLOAD_REQ:
    MSG_HIGH("gpsdiag_XtraEventCb: DLD REQ ignored", 0,0,0);
    break;

  case PDSM_XTRA_EVENT_TIME_REQ:
    MSG_HIGH("gpsdiag_XtraEventCb: TIME REQ Ignored", 0,0,0);
    break;

  default:
    break;
  }

}/* gpsdiag_XtraEventCb */

/*===========================================================================
FUNCTION gpsdiag_XtraRegCmd

DESCRIPTION
  This procedure handles the Xtra Register Diag command from the XTRA client
  by requesting the appropriate PDAPI command.

DEPENDENCIES
  This client should have first been initialized, registered, and activated
  via the gpsdiag_ApiCmdsDispatch() and gpsdiag_RegCmdsDispatch() functions.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* gpsdiag_XtraRegCmd(PACKED void* p_PktReq, uint16 w_PktLength)
{
  pdsm_client_status_e_type retVal    = PDSM_CLIENT_OK;
  pdsm_client_status_e_type retVal2   = PDSM_CLIENT_OK;
  gpsdiag_eStatusType        e_Status = GPSDIAG_SUCCESS;
  gpsdiag_XtraRegisterReqType * p_Req = (gpsdiag_XtraRegisterReqType*) p_PktReq;
  gpsdiag_XtraRegisterResType* p_Rsp  = NULL;

  gpsdiag_ClientTable[PDSM_CLIENT_TYPE_XTRA].l_ClientId
    = pdsm_client_init(PDSM_CLIENT_TYPE_XTRA);
  gpsdiag_ClientTable[PDSM_CLIENT_TYPE_XTRA].e_ClientType
    = PDSM_CLIENT_TYPE_XTRA;

  /* Verify if Client ID returned is valid */
  if ( gpsdiag_ClientTable[PDSM_CLIENT_TYPE_XTRA].l_ClientId == (pdsm_client_id_type)GPSDIAG_ERROR_VALUE )
  {
     MSG_HIGH("unable to initialize xtra client; exiting id:%d \n",
               gpsdiag_ClientTable[PDSM_CLIENT_TYPE_XTRA].l_ClientId, 0, 0);
     retVal = PDSM_CLIENT_ERR_CLIENT_INIT_FAIL;
  }
  else
  {
    retVal
      = pdsm_client_xtra_reg
        ( gpsdiag_ClientTable[PDSM_CLIENT_TYPE_XTRA].l_ClientId, /* Requesting client */
          NULL,                   /* client data pointer is NULL*/
          gpsdiag_XtraEventCb,    /* cb function for events*/
          PDSM_CLIENT_EVENT_REG,  /* Type of events */
          PDSM_XTRA_EVENT_MASK,   /* Event mask of all events needed */
          NULL /* No need for events of other clients*/);

    if(retVal == PDSM_CLIENT_OK)
    {
      MSG_HIGH("gpsdiag_XtraRegCmd: XTRA Registration succeeded  %d\n",
                gpsdiag_ClientTable[PDSM_CLIENT_TYPE_XTRA].l_ClientId, 0, 0);
    }
    else
    {
      MSG_HIGH("gpsdiag_XtraRegCmd: XTRA Registration Failed  %d retvalReg:%d \n",
                gpsdiag_ClientTable[PDSM_CLIENT_TYPE_XTRA].l_ClientId, retVal, 0);
    }

    retVal2 = pdsm_client_act(gpsdiag_ClientTable[PDSM_CLIENT_TYPE_XTRA].l_ClientId) ;

    if (retVal2 != PDSM_CLIENT_OK)
    {
      MSG_HIGH("gpsdiag_XtraRegCmd: XTRA Client Activation Failed  id:%d retValAct:%d \n",
                gpsdiag_ClientTable[PDSM_CLIENT_TYPE_XTRA].l_ClientId, retVal2, 0);
    }
  }

  e_Status = (retVal == PDSM_CLIENT_OK) ? GPSDIAG_SUCCESS : GPSDIAG_ERROR;

  /* Allocate space through diag for response packet */
  p_Rsp = (gpsdiag_XtraRegisterResType*)
           diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
           CGPS_DIAG_PDAPI_CMD,
           sizeof(gpsdiag_XtraRegisterResType) );

  /* Fill in response packet */
  if(p_Rsp == NULL)
  {
    MSG_ERROR("gpsdiag_RegCmd: Rsp packet is NULL!!", 0, 0, 0);
  }
  else
  {
    p_Rsp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
    p_Rsp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
    p_Rsp->l_ClientId                 = p_Req->l_ClientId;
    p_Rsp->q_Status                   = (uint32)e_Status;
 }

 return p_Rsp;
}/* gpsdiag_XtraRegCmd */


/*===========================================================================
FUNCTION gpsdiag_StartRexTask

DESCRIPTION
  This function is called to start the specified Rex Task.  

DEPENDENCIES
  None.

RETURN VALUE
  Success or failure

SIDE EFFECTS
  None.
===========================================================================*/
int gpsdiag_StartRexTask (rex_tcb_type *p_tcb, 
                          char *p_task_name, 
                          rex_stack_pointer_type p_stack, 
                          rex_stack_size_type stack_size, 
                          rex_priority_type priority, 
                          rex_task_func_type p_task_entry_func)
{
  rex_task_attributes_type gpsdiag_task_attr;  /* GPSDIAG Task Attributes */
  int error;
  
  error = rex_task_attr_init(&gpsdiag_task_attr);
  if(error != REX_SUCCESS)
  {
    MSG_ERROR("Cannot initialize task attributes.  Error = %d", error, 0, 0);
    
    return error;
  }
  
  error = rex_task_attr_set_stack_addr(&gpsdiag_task_attr, (rex_stack_pointer_type)p_stack);
  if(error != REX_SUCCESS)
  {
    MSG_ERROR("Cannot set task Stack Addr.  Error = %d", error, 0, 0);
    return error;
  }
  
  error = rex_task_attr_set_stack_size(&gpsdiag_task_attr, (rex_stack_size_type) stack_size);
  if(error != REX_SUCCESS)
  {
    MSG_ERROR("Cannot set task Stack Size.  Error = %d", error, 0, 0);
    return error;
  }
  
  error = rex_task_attr_set_prio(&gpsdiag_task_attr, (rex_priority_type) priority);
  if(error != REX_SUCCESS)
  {
    MSG_ERROR("Cannot set task Priority.  Error = %d", error, 0, 0);
    return error;
  }
  
  error = rex_task_attr_set_entry_func(&gpsdiag_task_attr, p_task_entry_func, (rex_task_func_param_type)NULL);
  if(error != REX_SUCCESS)
  {
    MSG_ERROR("Cannot set task Entry Function.  Error = %d", error, 0, 0);
    return error;
  }
  
  error = rex_task_attr_set_task_name(&gpsdiag_task_attr, p_task_name);
  if(error != REX_SUCCESS)
  {
    MSG_ERROR("Cannot set task name.  Error = %d", error, 0, 0);
    return error;
  }
  
  /* Start the task with Rex using the defined task attributes. */
  error = rex_common_def_task(p_tcb, &gpsdiag_task_attr);
  if(error != REX_SUCCESS)
  {
    MSG_ERROR("Cannot define task with Rex.  Error = %d", error, 0, 0);
    return error;
  }
  
  return REX_SUCCESS;
}

/*===========================================================================
FUNCTION gpsdiag_StartTask

DESCRIPTION
  This function is called via a QXDM command and results in the gpsdiag_task
  being started. This needs to be called from diag for other commands such
  as pdsm_get_position() to run.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

/*lint -e818 Pointer parameter could be declared as pointing to const */
PACKED void* gpsdiag_StartTask (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_StartTaskRspType* p_StartRsp = NULL;

   /* Allocate space through diag for response packet */
   p_StartRsp = (gpsdiag_StartTaskRspType*)
                diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                      CGPS_DIAG_START_TASK,
                                      sizeof(gpsdiag_StartTaskRspType) );
   diagpkt_commit(p_StartRsp);

   os_MutexLock(p_GpsDiagTaskMutex); /* TASKLOCK(); */
   if(gpsdiag_TaskIsRunning == FALSE)
   {
      /* Create GPSDIAG task */
      MSG_MED("Creating GPSDIAG task", 0, 0, 0);
      if(REX_SUCCESS == gpsdiag_StartRexTask(&gpsdiag_tcb,
                                             "GPSDIAG",
                                             (rex_stack_pointer_type) &gpsdiag_stack,
                                             (rex_stack_size_type) GPSDIAG_STACK_SIZ,
                                             (rex_priority_type) GPSDIAG_PRI,
                                             (rex_task_func_type) gpsdiag_task))
      {
         /* so that we can keep track of how many gpsdiag_task instances were created*/
         gpsdiag_task_count++;
           
         /* remember that we just started a gps diag task*/
         gpsdiag_TaskIsRunning = TRUE;
         MSG_MED("GPSDIAG task started.", 0, 0, 0);
      }
      else
      {
         MSG_ERROR("GPSDIAG task NOT started.", 0, 0, 0);
      }
   }
   os_MutexUnlock(p_GpsDiagTaskMutex); /* TASKFREE(); */

   return NULL;
} /* gpsdiag_StartTask */

/*===========================================================================
FUNCTION gpsdiag_EndTask

DESCRIPTION
  This function is called via a QXDM command and results in the gpsdiag_task
  being ended.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

/*lint -e818 Pointer parameter could be declared as pointing to const */
PACKED void* gpsdiag_EndTask (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_EndTaskRspType* p_EndRsp = NULL;

   /* Allocate space through diag for response packet */
   p_EndRsp = (gpsdiag_EndTaskRspType*)
              diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                    CGPS_DIAG_END_TASK,
                                    sizeof(gpsdiag_EndTaskRspType) );
//  diagpkt_commit(p_EndRsp);

   os_MutexLock(p_GpsDiagTaskMutex); /* TASKLOCK(); */
   if (gpsdiag_TaskIsRunning)
   {
     MSG_MED("Ending GPSDIAG task", 0, 0, 0);
     if(gpsdiag_SendMsg(0, THREAD_ID_GPSDIAG, C_GPSDIAG_END_TASK, NULL))
     {
       MSG_ERROR("GPSDIAG IPC Msg %d Not Sent.", C_GPSDIAG_END_TASK, 0, 0);
     }

     MSG_MED("GPSDIAG task ended", 0, 0, 0);
   }
   os_MutexUnlock(p_GpsDiagTaskMutex); /* TASKFREE(); */

   return p_EndRsp;
} /* gpsdiag_EndTask */

#ifdef FEATURE_CGPS_PDCOMM_TEST_DRIVER
/*===========================================================================
FUNCTION pdcommtd_StartTask

DESCRIPTION
  This function is called via a QXDM command and results in the pdcommtd_StartTask
  being started. This needs to be called from diag for other commands such
  as pdsm_get_position() to run.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

/*lint -e818 Pointer parameter could be declared as pointing to const */
PACKED void* pdcommtd_StartTask (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_StartTaskRspType* p_StartRsp = NULL;

   /* Allocate space through diag for response packet */
   p_StartRsp = (gpsdiag_StartTaskRspType*)
                diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                      CGPS_PDCOMM_TD_START_TASK,
                                      sizeof(gpsdiag_StartTaskRspType) );

   os_MutexLock(p_PdcommTdTaskMutex);

   if(pdcommtd_TaskIsRunning == FALSE)
   {      
      /* Create PDCOMM_TD task */
      MSG_MED("Creating PDCOMM_TD task", 0, 0, 0);
      if(REX_SUCCESS == gpsdiag_StartRexTask(&pdcommtd_tcb,
                                             "PDCOMM_TD",
                                             (rex_stack_pointer_type) &pdcommtd_stack,
                                             (rex_stack_size_type) PDCOMMTD_STACK_SIZ,
                                             (rex_priority_type) PDCOMMTD_PRI,
                                             (rex_task_func_type) pdcomm_test_driver_task))
      {
         /* remember that we just started a GPS PDCOMM task*/
         pdcommtd_TaskIsRunning = TRUE;
         MSG_MED("PDCOMM_TD task started.", 0, 0, 0);
      }
      else
      {
         MSG_ERROR("PDCOMM_TD task NOT started.", 0, 0, 0);
      }
   }
   else
   {
      MSG_ERROR("A instance of PDCOMM_TD task already running",0,0,0);
   }

   os_MutexUnlock(p_PdcommTdTaskMutex);

   return p_StartRsp;
} /* pdcommtd_StartTask */

/*===========================================================================
FUNCTION pdcommtd_EndTask

DESCRIPTION
  This function is called via a QXDM command and results in the pdcommtd_task
  being ended.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

/*lint -e818 Pointer parameter could be declared as pointing to const */
PACKED void* pdcommtd_EndTask (PACKED void* p_PktReq, uint16 w_PktLength)
{
   gpsdiag_EndTaskRspType* p_EndRsp = NULL;

   /* Allocate space through diag for response packet */
   p_EndRsp = (gpsdiag_EndTaskRspType*)
              diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                    CGPS_PDCOMM_TD_END_TASK,
                                    sizeof(gpsdiag_EndTaskRspType) );

   os_MutexLock(p_PdcommTdTaskMutex);
   if(pdcommtd_TaskIsRunning)
   {
      MSG_MED("Ending PDCOMM TD task", 0, 0, 0);
      rex_kill_task(&pdcommtd_tcb);
      pdcommtd_TaskIsRunning = FALSE;
      MSG_MED("PDCOMM TD task ended", 0, 0, 0);
   }
   os_MutexUnlock(p_PdcommTdTaskMutex);

   return p_EndRsp;
} /* pdcommtd_EndTask */
#endif /* FEATURE_CGPS_PDCOMM_TEST_DRIVER */


/*===========================================================================
FUNCTION gpsdiag_InitAppTracking

DESCRIPTION
  This function initializes the application tracking timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

static void gpsdiag_InitAppTracking (void)
{
   uint32 q_ClientType;
   uint32 q_TimerId;

   q_TimerId = (uint32)C_GPSDIAG_TIMER_APP_TRACKING_START;

   /* Initialize the structures that stores state variables for Application
    * Tracking for each client type.
    */
   for(q_ClientType = 0; q_ClientType < (uint32)PDSM_CLIENT_TYPE_MAX; q_ClientType++)
   {
      /* Set Client Type to appropriate enumerated value */
      gpsdiag_ClientTable[q_ClientType].e_ClientType = (pdsm_client_type_e_type)q_ClientType;

      /* Set Client ID to initial unregistered value */
      gpsdiag_ClientTable[q_ClientType].l_ClientId   = (pdsm_client_id_type)GPSDIAG_INIT_VALUE;

      /* Set Application Tracking Flow Control varables to initial reset values */
      gpsdiag_ClientTable[q_ClientType].z_FixFlowCtrl.q_NumFixesLeft            = 0;
      gpsdiag_ClientTable[q_ClientType].z_FixFlowCtrl.q_NumFixesTried           = 0;
      gpsdiag_ClientTable[q_ClientType].z_FixFlowCtrl.q_NumFixesSuccess         = 0;
      gpsdiag_ClientTable[q_ClientType].z_FixFlowCtrl.q_NumFixesFailed          = 0;
      gpsdiag_ClientTable[q_ClientType].z_FixFlowCtrl.e_FixType                 = GPSDIAG_FIX_TYPE_STOP;
      gpsdiag_ClientTable[q_ClientType].z_FixFlowCtrl.u_AppTrackingTimerExpired = FALSE;
      gpsdiag_ClientTable[q_ClientType].z_FixFlowCtrl.u_PdsmGetPosDone          = FALSE;

      /* Create the application tracking timer */
      if(gpsdiag_ClientTable[q_ClientType].p_AppTrackingTimer == NULL)
      {
         gpsdiag_ClientTable[q_ClientType].p_AppTrackingTimer = os_TimerCreateInCgpsTimerGroup(q_TimerId++, (uint32)THREAD_ID_GPSDIAG);
      }

      if(gpsdiag_ClientTable[q_ClientType].p_AppTrackingTimer == NULL)
      {
         MSG_MED("gpsdiag_InitAppTracking: Init Failed.", 0, 0, 0);
      }
   }

   MSG_MED("gpsdiag_InitAppTracking: Init Succeeded", 0, 0, 0);

} /* gpsdiag_InitAppTracking */

/*===========================================================================
FUNCTION gpsdiag_StartAppTracking

DESCRIPTION
  This function triggers pdsm_get_position() during an application tracking
  session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  z_FixFlowCtrl structure updated and app tracking timer is restarted
===========================================================================*/

static void gpsdiag_StartAppTracking (pdsm_client_type_e_type e_ClientType)
{
  /* Client Type Sanity Check */
  if (e_ClientType >= PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == PDSM_CLIENT_TYPE_NONE)
  {
    MSG_ERROR("gpsdiag_StartAppTracking: Client Type Error %d", e_ClientType, 0, 0);
    return;
  }

  /* First check to see if end session has been called previously */
  if (gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesLeft == 0)
  {
    MSG_MED("gpsdiag_StartAppTracking: App Tracking Stopped!!", 0, 0, 0);
    return;
  }

  /* Otherwise, get the position */
  if (!pdsm_get_position(gpsdiag_GetPosFixCmdCb,
                         (void*)&(gpsdiag_ClientTable[e_ClientType].e_ClientType), /* user data pointer */
                         &(gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdOptions),
                         &(gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdQos),
                         gpsdiag_ClientTable[e_ClientType].l_ClientId))
  {
    MSG_ERROR("gpsdiag_StartAppTracking: Unable to call pdsm_get_position",0 ,0, 0);
  }
  else
  {
    /* Reset Application Tracking fix flow control variables */
    gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.u_AppTrackingTimerExpired = FALSE;
    gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.u_PdsmGetPosDone = FALSE;
    gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesLeft--;
    gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesTried++;

    MSG_MED("gpsdiag_StartAppTracking: New Position Fix Request (%d/%d)",
            (gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesTried),
            (gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesLeft + gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesTried),
            0);

    /* Updated algorithm for doing diag based tracking:
     * If TBF <= 1s, trigger next get position immediately when next position event is received.
     * Also, start the timer with TBF - 0.5 seconds instead, to give enough time for info to propagate to MGP.
     */
    if (gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdOptions.fix_rate.time_between_fixes_ms <= 1000)
    {
      /*lint -e506 Constant value boolean */
      gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.u_AppTrackingTimerExpired = TRUE;
    }
    else
    {
      /* Start Application Tracking Timer */
      if (!os_TimerStart(gpsdiag_ClientTable[e_ClientType].p_AppTrackingTimer,
                         (gpsdiag_ClientTable[e_ClientType].z_PdsmGetPosParams.z_PdOptions.fix_rate.time_between_fixes_ms),
                         gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesTried))
      {
        MSG_MED("gpsdiag_StartAppTracking: Timer Start Failed.", 0, 0, 0);
      }
      else
      {
        MSG_MED("gpsdiag_StartAppTracking: Timer Start Succeeded.", 0, 0, 0);
      }
    }
  }
} /* gpsdiag_StartAppTracking */

/*===========================================================================
FUNCTION gpsdiag_RestartAppTracking

DESCRIPTION
  This function is called when the application tracking timer has elapsed.
  It checks the z_FixFlowCtrl structure and decides if it should
  trigger gpsdiag_StartAppTracking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  z_FixFlowCtrl.u_AppTrackingTimerExpired updated to TRUE
===========================================================================*/

static void gpsdiag_RestartAppTracking (pdsm_client_type_e_type e_ClientType)
{
   MSG_MED("gpsdiag_RestartAppTracking: Application Tracking Timer Expired", 0, 0, 0);

   /* Client Type Sanity Check */
   if(e_ClientType >= PDSM_CLIENT_TYPE_MAX ||
      e_ClientType == PDSM_CLIENT_TYPE_NONE)
   {
      MSG_ERROR("gpsdiag_RestartAppTracking: Client Type Error %d", e_ClientType, 0, 0);
      return;
   }

   /* Check if App Tracking has finished yet */
   if(gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.u_PdsmGetPosDone &&
      gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesLeft == 0 &&
      gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.e_FixType == GPSDIAG_FIX_TYPE_APP_TRACKING)
   {
      MSG_HIGH("GPSDIAG Application Tracking Complete", 0, 0, 0);
   }

   /*lint -e506 Constant value boolean */
   gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.u_AppTrackingTimerExpired = TRUE;

   /* When the application tracking timer expires, 3 things need to be checked
    * to see if it should invoke position fix again:
    *   1) PDSM has returned a fix
    *   2) Number of fixes left > 0
    *   3) QXDM didn't send a stop application tracking cmd
    */
   if((gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.u_PdsmGetPosDone) &&
      (gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.q_NumFixesLeft > 0) &&
      (gpsdiag_ClientTable[e_ClientType].z_FixFlowCtrl.e_FixType == GPSDIAG_FIX_TYPE_APP_TRACKING))
   {
      MSG_MED("gpsdiag_RestartAppTracking: Starting Application Tracking Again...", 0, 0, 0);
      gpsdiag_StartAppTracking(e_ClientType);
   }
} /* gpsdiag_RestartAppTracking */

/*===========================================================================
FUNCTION gpsdiag_GetClientType

DESCRIPTION
  This function is called to extract the correct Client Type mapping from the
  input Client Id. This is done by iterating through the gpsdiag_ClientTable
  in search for a matching Client Id, and returning the indexing Client Type.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

static uint32 gpsdiag_GetClientType (pdsm_client_id_type l_ClientId)
{
   uint32 q_ClientType;

   for(q_ClientType = 0; q_ClientType < (uint32)PDSM_CLIENT_TYPE_MAX; q_ClientType++)
   {
      if(gpsdiag_ClientTable[q_ClientType].l_ClientId == l_ClientId)
      {
         return(uint32)q_ClientType; //pdsm_client_type_e_type
      }
   }
   return(uint32)GPSDIAG_ERROR_VALUE;
} /* gpsdiag_GetClientType */


/**
 * Translates gpsdiag delete parameters struct to corresponding
 * PDSM delete params struct
 * @param pDest
 * @param pSrc
 *
 * @return uint32. returns 0 on success, non-zero otherwise.
 */
static uint32 gpsdiag_TranslateGpsDiagToPdsmDeleteParams(pdsm_delete_parms_type *pDest,
                                                         const gpsdiag_PaDelParamsType *pSrc)
{
   uint32 q_retval = 0;
   if(pDest && pSrc)
   {
      pDest->pdsm_delete_parms_flags = 0; /* reset all flags to prevent spurious bit setting */

      /* translate flags. this is done independently here to decouple diag flag and pdapi flag definitions
      this will allow each interface to change flag definitions without forcing changes in the other. */
      if(GPSDIAG_PA_DELETE_EPH & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH;
      }

      if(GPSDIAG_PA_DELETE_ALM & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM;
      }

      if(GPSDIAG_PA_DELETE_POS & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_POS;
      }

      if(GPSDIAG_PA_DELETE_TIME & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_TIME;
      }

      if(GPSDIAG_PA_DELETE_IONO & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_IONO;
      }

      if(GPSDIAG_PA_DELETE_UTC & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_UTC;
      }

      if(GPSDIAG_PA_DELETE_HEALTH & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_HEALTH;
      }

      if(GPSDIAG_PA_DELETE_SVDIR & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR;
      }

      if(GPSDIAG_PA_DELETE_SVSTEER & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER;
      }

      if(GPSDIAG_PA_DELETE_SADATA & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SADATA;
      }

      if(GPSDIAG_PA_DELETE_RTI & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_RTI;
      }

      if(GPSDIAG_PA_TIME_UNC & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_TIME_UNC;
      }

      if(GPSDIAG_PA_POS_UNC & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_POS_UNC;
      }

      if(GPSDIAG_PA_TIME_OFFSET & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_TIME_OFFSET;
      }

      if(GPSDIAG_PA_POS_OFFSET & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_POS_OFFSET;
      }

      if(GPSDIAG_PA_DELETE_CELLDB_INFO & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_CELLDB_INFO;
      }

      if(GPSDIAG_PA_DELETE_ALM_CORR & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_CORR;
      }

      if(GPSDIAG_PA_FREQ_BIAS_EST & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_FREQ_BIAS_EST;
      }

      if(GPSDIAG_PA_DELETE_SV_NO_EXIST & pSrc->q_DelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SV_NO_EXIST;
      }

      //translate newer gnss flags
      if(GNSSDIAG_PA_DELETE_EPH_GLO        & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH_GLO;
      }

      if(GNSSDIAG_PA_DELETE_EPH_SBAS       & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH_SBAS;
      }

      if(GNSSDIAG_PA_DELETE_ALM_GLO        & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_GLO;
      }

      if(GNSSDIAG_PA_DELETE_ALM_SBAS       & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_SBAS;
      }

      if(GNSSDIAG_PA_DELETE_SVDIR_GLO      & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR_GLO;
      }

      if(GNSSDIAG_PA_DELETE_SVDIR_SBAS     & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR_SBAS;
      }

      if(GNSSDIAG_PA_DELETE_SVSTEER_GLO    & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER_GLO;
      }

      if(GNSSDIAG_PA_DELETE_SVSTEER_SBAS   & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER_SBAS;
      }

      if(GNSSDIAG_PA_DELETE_ALM_CORR_GLO   & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_CORR_GLO;
      }

      if(GNSSDIAG_PA_DELETE_CLOCK_INFO   & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_CLOCK_INFO;
      }

      // BDS support
      if(GNSSDIAG_PA_DELETE_EPH_BDS       & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH_BDS;
      }

      if(GNSSDIAG_PA_DELETE_ALM_BDS        & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_BDS;
      }

      if(GNSSDIAG_PA_DELETE_SVDIR_BDS      & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR_BDS;
      }

      if(GNSSDIAG_PA_DELETE_SVSTEER_BDS    & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER_BDS;
      }

      if(GNSSDIAG_PA_DELETE_ALM_CORR_BDS   & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_CORR_BDS;
      }

      // GAL support
      if(GNSSDIAG_PA_DELETE_EPH_GAL       & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH_GAL;
      }

      if(GNSSDIAG_PA_DELETE_ALM_GAL        & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_GAL;
      }

      if(GNSSDIAG_PA_DELETE_SVDIR_GAL      & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR_GAL;
      }

      if(GNSSDIAG_PA_DELETE_SVSTEER_GAL    & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER_GAL;
      }

      if(GNSSDIAG_PA_DELETE_ALM_CORR_GAL   & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_CORR_GAL;
      }

      if(GNSSDIAG_PA_DELETE_GNSS_SV_BLACKLIST_GPS   & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GPS;
      }

      if(GNSSDIAG_PA_DELETE_GNSS_SV_BLACKLIST_GLO   & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GLO;
      }

      if(GNSSDIAG_PA_DELETE_GNSS_SV_BLACKLIST_BDS   & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_GNSS_SV_BLACKLIST_BDS;
      }

      if(GNSSDIAG_PA_DELETE_GNSS_SV_BLACKLIST_GAL   & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GAL;
      }

      if(GNSSDIAG_PA_DELETE_GNSS_SV_BLACKLIST_QZSS   & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_GNSS_SV_BLACKLIST_QZSS;
      }

      if(GNSSDIAG_PA_DELETE_EPH_QZSS       & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_EPH_QZSS;
      }

      if(GNSSDIAG_PA_DELETE_ALM_QZSS        & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_QZSS;
      }

      if(GNSSDIAG_PA_DELETE_SVDIR_QZSS      & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVDIR_QZSS;
      }

      if(GNSSDIAG_PA_DELETE_SVSTEER_QZSS    & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_SVSTEER_QZSS;
      }

      if(GNSSDIAG_PA_DELETE_ALM_CORR_QZSS   & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_ALM_CORR_QZSS;
      }

      if(GNSSDIAG_PA_DELETE_IONO_QZSS   & pSrc->q_GnssDelParamsFlags)
      {
         pDest->pdsm_delete_parms_flags |= PDSM_PA_DELETE_IONO_QZSS;
      }      
          
      // Fill in pdsm_delete_parms_type struct
      pDest->time_unc                = pSrc->q_TimeUnc;
      pDest->pos_unc                 = pSrc->q_PosUnc;
      pDest->time_offset             = pSrc->q_TimeOffset;
      pDest->pos_offset              = pSrc->q_PosOffset;
      pDest->eph_sv_mask             = pSrc->q_EphSvMask;
      pDest->alm_sv_mask             = pSrc->q_AlmSvMask;
      pDest->glo_eph_sv_mask         = pSrc->q_GloEphSvMask;
      pDest->glo_alm_sv_mask         = pSrc->q_GloAlmSvMask;
      pDest->sbas_eph_sv_mask        = pSrc->q_SBASEphSvMask;
      pDest->sbas_alm_sv_mask        = pSrc->q_SBASAlmSvMask;
      pDest->bds_eph_sv_mask         = 0; //pSrc->t_BdsEphSvMask;  // will delete all SVs if it's 0
      pDest->bds_alm_sv_mask         = 0; //pSrc->t_BdsAlmSvMask;  // RH TODO
      pDest->qzss_eph_sv_mask        = 0; //pSrc->t_QzssEphSvMask;  // will delete all SVs if it's 0
      pDest->qzss_alm_sv_mask        = 0; //pSrc->t_QzssAlmSvMask;  // JV TODO
      pDest->gal_eph_sv_mask         = 0; //pSrc->t_GalEphSvMask;  // will delete all SVs if it's 0
      pDest->gal_alm_sv_mask         = 0; //pSrc->t_GalAlmSvMask;  // RH TODO
      pDest->delete_celldb_mask      = pSrc->q_DelCellDBMask;
      pDest->delete_clock_info_mask  = pSrc->q_DeleteClockInfoMask;
      pDest->gps_gnss_sv_blacklist_mask = 0; // the blacklist for all GPS SVs will be deleted
      pDest->glo_gnss_sv_blacklist_mask = 0; // the blacklist for all GLO SVs will be deleted
      pDest->bds_gnss_sv_blacklist_mask = 0; // the blacklist for all BDS SVs will be deleted
      pDest->qzss_gnss_sv_blacklist_mask = 0; // the blacklist for all QZSS SVs will be deleted
      pDest->gal_gnss_sv_blacklist_mask = 0; // the blacklist for all GAL SVs will be deleted
   }
   else
   {
      q_retval = (uint32)GPSDIAG_ERROR_VALUE;
   }

   return q_retval;
} /* end gpsdiag_TranslateGpsDiagToPdsmDeleteParams() */

static uint32 gpsdiag_TranslateGpsDiagToPdsmNmeaType(pdsm_pa_nmea_type *pDest,
                                                     const uint16 w_src_diag_nmea_sentence_mask)
{
   uint32 q_retval = 0;
   if(pDest)
   {
      *pDest = 0; //reset all flags, to prevent spurious settings

      if(GPSDIAG_PA_NMEA_GGA  & w_src_diag_nmea_sentence_mask)
      {
         *pDest |= PDAPI_NMEA_GPGGA;
      }

      if(GPSDIAG_PA_NMEA_RMC  & w_src_diag_nmea_sentence_mask)
      {
         *pDest |= PDAPI_NMEA_GPRMC;
      }

      if(GPSDIAG_PA_NMEA_GSV  & w_src_diag_nmea_sentence_mask)
      {
         *pDest |= PDAPI_NMEA_GPGSV;
      }

      if(GPSDIAG_PA_NMEA_GSA  & w_src_diag_nmea_sentence_mask)
      {
         *pDest |= PDAPI_NMEA_GPGSA;
      }

      if(GPSDIAG_PA_NMEA_VTG  & w_src_diag_nmea_sentence_mask)
      {
         *pDest |= PDAPI_NMEA_GPVTG;
      }

      if(GNSSDIAG_PA_NMEA_GLGSV  & w_src_diag_nmea_sentence_mask)
      {
         *pDest |= PDAPI_NMEA_GLGSV;
      }

      if(GNSSDIAG_PA_NMEA_GNGSA  & w_src_diag_nmea_sentence_mask)
      {
         *pDest |= PDAPI_NMEA_GNGSA;
      }

      if(GNSSDIAG_PA_NMEA_GNGNS  & w_src_diag_nmea_sentence_mask)
      {
         *pDest |= PDAPI_NMEA_GNGNS;
      }

      if(GNSSDIAG_PA_NMEA_PSTIS  & w_src_diag_nmea_sentence_mask)
      {
         *pDest |= PDAPI_NMEA_PSTIS;
      }

      if(GNSSDIAG_PA_NMEA_PQGSX  & w_src_diag_nmea_sentence_mask)
      {
         if(GPSDIAG_PA_NMEA_GSA  & w_src_diag_nmea_sentence_mask)
         {
            *pDest |= PDAPI_NMEA_PQGSA;
         }

         if(GPSDIAG_PA_NMEA_GSV  & w_src_diag_nmea_sentence_mask)
         {
            *pDest |= PDAPI_NMEA_PQGSV;
         }
      }

      if(GNSSDIAG_PA_NMEA_GARMC & w_src_diag_nmea_sentence_mask)
      {
         *pDest |= PDAPI_NMEA_GARMC;
      }

      if(GNSSDIAG_PA_NMEA_GAGSV & w_src_diag_nmea_sentence_mask) 
      {
         *pDest |= PDAPI_NMEA_GAGSV;
      }

      if(GNSSDIAG_PA_NMEA_GAGSA & w_src_diag_nmea_sentence_mask) 
      {
         *pDest |= PDAPI_NMEA_GAGSA;
      }

      if(GNSSDIAG_PA_NMEA_GAVTG & w_src_diag_nmea_sentence_mask) 
      {
         *pDest |= PDAPI_NMEA_GAVTG;
      }

      if(GNSSDIAG_PA_NMEA_GAGGA & w_src_diag_nmea_sentence_mask) 
      {
         *pDest |= PDAPI_NMEA_GAGGA;
      }
   }
   else
   {
      q_retval = (uint32)GPSDIAG_ERROR_VALUE;
   }

   return q_retval;
} /* end gpsdiag_TranslateGpsDiagToPdsmNmeaType() */



/** Queries software SM version
 *
 * @param p_PktReq
 * @param w_PktLength
 *
 * @return PACKED void* pointer to response packet
 * @see gpsdiag_ApiCmdsDispatch(). Client should have first been
 *      initialized by gpsdiag_ApiCmdsDispatch().
 */
PACKED void* gpsdiag_QueryVersionCmd(PACKED void* p_PktReq, uint16 w_PktLength)
{

   PACKED void *p_retval = NULL;
   gpsdiag_QueryStatusType e_status = GPSDIAG_QUERY_STATUS_SUCCESS;
   gpsdiag_QueryVersionRespType *p_Resp = NULL;

   /* using do-while(0) idiom to allow single clean-up and exit points */
   /* think of this as a poor-man's try-catch(...) exception block */
   do
   {
      gpsdiag_QueryVersionReqType *p_Req = NULL;

      if(!p_PktReq)
      {
         e_status = GPSDIAG_QUERY_STATUS_ERROR_BAD_PARAMS;
         break;
      }

      /* Verify that the parameter length is correct */
      if(!w_PktLength)
      {
         e_status = GPSDIAG_QUERY_STATUS_ERROR_BAD_PKT_LENGTH;
         break;
      }

      p_Req = (gpsdiag_QueryVersionReqType*) p_PktReq;

      /* check if we handle this version of the query command */
      switch(p_Req->z_DiagCmdHdr.u_Version)
      {
      case GPSDIAG_QUERY_CMD_VERSION_1:
         /* Verify that the parameter length is correct */
         if(w_PktLength != sizeof(*p_Req)) /* sizeof(gpsdiag_QueryVersionReqType) */
         {
            e_status = GPSDIAG_QUERY_STATUS_ERROR_BAD_PKT_LENGTH;
            break;
         }
         //gpsdiag_HandleQueryVersion_1(gpsdiag_QueryVersionReqType *p_Req);
         p_Resp = gpsdiag_HandleQueryVersion_1(p_Req);
         e_status = p_Resp ? GPSDIAG_QUERY_STATUS_SUCCESS : GPSDIAG_QUERY_STATUS_ERROR_BAD_PARAMS;
         break;

      default:
         /* add handling for other versions above this line */
         e_status = GPSDIAG_QUERY_STATUS_ERROR_BAD_PARAMS;
         break;
      }

      /* assign newly crafted response packet to output */
      p_retval = p_Resp;
   } while(0);

   /* clean ups here */
   switch(e_status)
   {
   case GPSDIAG_QUERY_STATUS_ERROR_BAD_PARAMS:
      if(p_retval)
      {
         diagpkt_free(p_retval);
      }
      /* Return a diag error packet indicating bad parameters were passed */
      p_retval = diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength);
      break;

   case GPSDIAG_QUERY_STATUS_ERROR_BAD_PKT_LENGTH:
      if(p_retval)
      {
         diagpkt_free(p_retval);
      }
      /* indicate that a packet with bad length was passed */
      p_retval = diagpkt_err_rsp(DIAG_BAD_LEN_F,  p_PktReq, w_PktLength);
      break;

      /* add other error handling above this line */
   case GPSDIAG_QUERY_STATUS_SUCCESS: /* intentional fall through */
   default:
      /* do nothing */
      break;
   }

   return p_retval;
} /* end gpsdiag_QueryVersionCmd() */

/**
 * retrieves description string used by diag query command
 *
 * @return const char* pointing to description. Currently,
 *         this is generated by P4.
 */
const char* gpsdiag_GetDescriptionString()
{
   return c_sm_query_file_description;
}

/**
 * Encrypts the description string sent out by the diag command,
 * so that it is limiited to QC use only
 * @param p_Description: buffer to containing description to be
 *                     encrypted. this buffer is encrypted in
 *                     place.
 * @param u_DescriptionLength: number of bytes in description
 *                           string, including null terminator,
 *                           if any
 *
 * @return boolean true on success. false if p_Description ==
 *         NULL or u_Description = 0
 */
boolean gpsdiag_EncryptDescriptionString(uint8 *p_Description, uint8 u_DescriptionLength)
{
   boolean b_retval = (p_Description != NULL);

   if(b_retval)
   {
      while(u_DescriptionLength--)
      {
         p_Description[u_DescriptionLength] ^= 0xff; /* this is same as bit inversion */
      }
   }
   return b_retval;
} /* end gpsdiag_EncryptDescriptionString() */

static gpsdiag_QueryVersionRespType* gpsdiag_HandleQueryVersion_1(gpsdiag_QueryVersionReqType *p_Req)
{
   const char* c_Description = NULL;
   uint8 u_DescriptionLength = 0;
   uint32 q_PacketSize = 0;
   gpsdiag_QueryVersionRespType* p_Resp = NULL;

   if(p_Req)
   {
      /* determine size of diag packet */
      c_Description = gpsdiag_GetDescriptionString();
      u_DescriptionLength = c_Description ? strlen(c_Description) : 0;

      /* minus 1 to accomodate null terminator */
      u_DescriptionLength = MIN(u_DescriptionLength, GPSDIAG_QUERY_DIAG_DESCRIPTION_LENGTH_MAX - 1);

      /* this includes size of null-terminator byte because of
      definition gpsdaig_QueryVersionReqType::u_Description[1]*/
      q_PacketSize = (sizeof(*p_Resp)+u_DescriptionLength);

      /* Allocate space through diag for response packet */
      p_Resp = (gpsdiag_QueryVersionRespType*)
               diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                     CGPS_DIAG_PDAPI_CMD,
                                     q_PacketSize);
      /* Fill in response packet */
      if(p_Resp  == NULL)
      {
         MSG_ERROR("gpsdiag_SetParamCmd: Rsp packet is NULL!!", 0, 0, 0);
      }
      else
      {
         memset(p_Resp, 0, q_PacketSize);

         p_Resp->z_DiagCmdHdr = p_Req->z_DiagCmdHdr;

         /* we always return SM version */
         p_Resp->q_MajorRev = GPSDIAG_QUERY_SM_MAJOR_VERSION;
         p_Resp->q_MinorRev = GPSDIAG_QUERY_SM_MINOR_VERSION;

         p_Resp->q_SubsystemType = p_Req->q_SubsystemType;
         p_Resp->q_Subcommand = p_Req->q_Subcommand;

         p_Resp->q_Status = GPSDIAG_QUERY_STATUS_ERROR_NOT_IMPLEMENTED;
         p_Resp->q_SubCommandVersion = 0;
         switch(p_Resp->q_SubsystemType)
         {
         case GPSDIAG_QUERY_SUBSYSTEM_TYPE_SM:
            switch(p_Resp->q_Subcommand)
            {
            case GPSDIAG_QUERY_SUBCOMMAND_GET_SM_VERSION:
               p_Resp->q_Status = GPSDIAG_QUERY_STATUS_SUCCESS;
               p_Resp->q_SubCommandVersion = 0; /* for this sub-command, 0 is a valid version */
               break;

               /* add handling for other SUBSYSTEM_TYPE_SM subcommands here */
            default:
               p_Resp->q_Status = GPSDIAG_QUERY_STATUS_ERROR_NOT_IMPLEMENTED;
               p_Resp->q_SubCommandVersion = 0;
               break;
            }
            break;  /* end case GPSDIAG_QUERY_SUBSYSTEM_TYPE_SM */

            /*add handling for other sub-systems here */
         default:
            p_Resp->q_Status = GPSDIAG_QUERY_STATUS_ERROR_NOT_IMPLEMENTED;
            p_Resp->q_SubCommandVersion = 0;
            break;

         } /* end switch(p_Resp->q_SubsystemType) */

         p_Resp->u_DescriptionLength = u_DescriptionLength;
         if (NULL != c_Description)
         {
            (void) GNSS_STRLCPY((char*) p_Resp->u_Description, c_Description, u_DescriptionLength);
         }
         (void) gpsdiag_EncryptDescriptionString(p_Resp->u_Description, p_Resp->u_DescriptionLength);

      } /* end else(p_Resp  == NULL)*/


   } /* end if(p_Req) */

   return p_Resp;
} /* end gpsdiag_HandleQueryVersion_1() */

PACKED void* gpsdiag_HandleExternalSPIInjection(PACKED void* p_PktReq, uint16 w_PktLength)
{
   PACKED void* p_retval = NULL;
   gpsdiag_XSpiInjectionCmdStatusType e_status = GPSDIAG_XSPI_CMD_STATUS_ERROR_NONE;
   gpsdiag_XSpiInjectionReqType *p_Req = NULL;
   gpsdiag_XSpiInjectionRespType *p_Resp = NULL;
   pdsm_pd_external_spi_type     z_xspi = {0};

   do
   {
      /* sanity check inputs */
      p_Req = (gpsdiag_XSpiInjectionReqType*) p_PktReq;

      /* Function parameters Sanity Check */
      if(p_Req == NULL)
      {
         /* Return a diag error packet indicating bad parameters were passed */
         return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
      }

      if(!w_PktLength || (sizeof(*p_Req) != w_PktLength))
      {
         e_status = GPSDIAG_XSPI_CMD_STATUS_BAD_PKT_LENGTH;
         break;
      }

      switch(p_Req->q_Mode)
      {
      case GPSDIAG_XSPI_STATIONARY:
         z_xspi.q_state = PDSM_PD_EXTERNAL_SPI_STATE_STATIONARY;
         break;

      case GPSDIAG_XSPI_NON_STATIONARY:
         z_xspi.q_state = PDSM_PD_EXTERNAL_SPI_STATE_NON_STATIONARY;
         break;

      case GPSDIAG_XSPI_UNKNOWN: /* intentional cascade */
      default:
         z_xspi.q_state = PDSM_PD_EXTERNAL_SPI_STATE_UNKNOWN;
         break;

      }

      z_xspi.u_confidence = p_Req->u_Confidence;
      p_Req->q_Reserved[0] = 0;
      p_Req->q_Reserved[1] = 0;

      if(!pdsm_pd_inject_external_spi(
                                     gpsdiag_ExternalSPInjectCb,
                                     p_Req->l_ClientId,
                                     NULL,
                                     &z_xspi))
      {
         MSG_ERROR("failed injecting spi", 0, 0, 0);
         e_status = GPSDIAG_XSPI_CMD_STATUS_PDAPI_ERROR;
      }

   } while(0);

   /* generate appropriate response here */
   /* Allocate space through diag for response packet */
   p_Resp = (gpsdiag_XSpiInjectionRespType*)
            diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                  CGPS_DIAG_PDAPI_CMD,
                                  sizeof(*p_Resp) );
   /* Fill in response packet */
   if(p_Resp == NULL)
   {
      MSG_ERROR("gpsdiag_HandleExternalSPIInjection: Rsp packet is NULL!!", 0, 0, 0);
   }
   else
   {
      p_Resp->z_DiagCmdHdr    = p_Req->z_DiagCmdHdr;
      p_Resp->q_Status        = (uint32)e_status;
      p_Resp->q_Reserved[0]   = 0;
      p_Resp->q_Reserved[1]   = 0;

      p_retval = (void*) p_Resp;
   }

   return p_retval;

} /* end gpsdiag_HandleExternalSPIInjection() */

void  gpsdiag_ExternalSPInjectCb(

    void                      *data_block_ptr,
        /* Pointer to client provided data block.
        **
        ** NOTE! Depending on the client implementation of this function
        ** the data block pointed by this pointer might be accessed from
        ** client, as well as, PDSM context simultaneously */

    pdsm_pd_cmd_e_type        pd_cmd,
        /* Indicate which command is this error status for */

    pdsm_pd_cmd_err_e_type    pd_cmd_err
        /* Command error code, indicating whether command
        ** is rejected and the reason */
)
{
   MSG_HIGH("gpsdiag_ExternalSPInjectCb received cmd_type = %d, error =%d", pd_cmd, pd_cmd_err, 0);
   return;
}



/** OEM DRE Control
*
* @param p_PktReq
* @param w_PktLength
*
* @return PACKED void* pointer to response packet
* @see gpsdiag_ApiCmdsDispatch(). Client should have first been
*      initialized by gpsdiag_ApiCmdsDispatch().
*/
static PACKED void* gpsdiag_OemControl(PACKED void* p_PktReq, uint16 w_PktLength)
{
  PACKED void* p_RspPkt = NULL;
  gpsdiag_OemControlReqType* p_Req = (gpsdiag_OemControlReqType*) p_PktReq;
  diagpkt_cmd_code_type u_DiagCode = DIAG_MAX_F;

  if(w_PktLength == sizeof(gpsdiag_OemControlReqType))
  {
    switch(p_Req->q_OemFeature)
    {
      case GPSDIAG_OEMFEATURE_DRE:
        p_RspPkt = gpsdiag_OemDre(p_Req, w_PktLength);
        break;

      case GPSDIAG_OEMFEATURE_DRSYNC:
        p_RspPkt = gpsdiag_OemDrSync(p_Req, w_PktLength);
        break;

      default:
        u_DiagCode = DIAG_BAD_PARM_F;
        break;
    }
  }
  else
  {
    u_DiagCode = DIAG_BAD_LEN_F;
  }

  if (DIAG_MAX_F != u_DiagCode)
  {
    p_RspPkt = diagpkt_err_rsp(u_DiagCode,  p_PktReq, w_PktLength);
  }

  return p_RspPkt;
}


/** OEM DRE
 *
 * @param p_Req
 * @param w_PktLength
 *
 * @return PACKED void* pointer to response packet
 * @see gpsdiag_ApiCmdsDispatch(). Client should have first been
 *      initialized by gpsdiag_ApiCmdsDispatch().
 */
static PACKED void* gpsdiag_OemDre(gpsdiag_OemControlReqType* p_Req, uint16 w_PktLength)
{
   PACKED void* p_RspPkt;
   gpsdiag_OemControlRespType* p_Resp;
   oemdre_statusType oemdre_Resp = FEATURE_OEMDRE_NONE;

   switch (p_Req->q_OemCommand)
   {
      case GPSDIAG_OEM_DRE_ON:
         oemdre_Resp = oemdreOn();
         break;
      case GPSDIAG_OEM_DRE_OFF:
         oemdre_Resp = oemdreOff();
         break;
   }

   if (FEATURE_OEMDRE_NONE != oemdre_Resp)
   {
      p_RspPkt = diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                        CGPS_DIAG_PDAPI_CMD,
                                        sizeof(gpsdiag_OemControlRespType) );
      if (NULL != p_RspPkt)
      {
        p_Resp = (gpsdiag_OemControlRespType*) p_RspPkt;
        p_Resp->z_DiagCmdHdr = p_Req->z_DiagCmdHdr;
        p_Resp->q_OemFeature = p_Req->q_OemFeature;
        p_Resp->q_OemCommand = p_Req->q_OemCommand;
        p_Resp->q_RespResult = (uint32) oemdre_Resp;
      }
      else
      {
        MSG_ERROR("gpsdiag_OemDre - response packet not allocated!", 0, 0, 0);
      }
   }
   else
   {
      p_RspPkt = diagpkt_err_rsp(DIAG_BAD_PARM_F, (PACKED void*) p_Req, w_PktLength);
   }

   return p_RspPkt;

} /* end gpsdiag_OemDre() */

PACKED void* gpsdiag_OemDrSync(gpsdiag_OemControlReqType *p_Req, uint16 w_PktLength)
{
  PACKED void *p_retval = NULL;
  boolean oemdrsync_Resp = TRUE;
  gpsdiag_OemControlRespType *p_Resp;

  if(GPSDIAG_OEM_DRSYNC_ARM == p_Req->q_OemCommand) {
    oemdrsync_Resp = oemDrSyncArm();
  }
  else if(GPSDIAG_OEM_DRSYNC_GET_CFG == p_Req->q_OemCommand) {
    oemdrsync_Resp = oemDrSyncGetConfig();
  }
  else {
    p_retval = diagpkt_err_rsp(DIAG_BAD_PARM_F, (PACKED void*) p_Req, w_PktLength);
    return p_retval;
  }

  p_Resp = (gpsdiag_OemControlRespType*)
    diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
    CGPS_DIAG_PDAPI_CMD,
    sizeof(gpsdiag_OemControlRespType));

  if(NULL != p_Resp) {
    p_Resp->q_RespResult = (uint32) oemdrsync_Resp;
    p_Resp->q_OemFeature = p_Req->q_OemFeature;
    p_Resp->q_OemCommand = p_Req->q_OemCommand;
    p_retval = p_Resp;
  }
  else {
    /* need to check if a null can sent back to describe error, setting
    * an incorrect error code for now when mem alloc fails from diag
    * subsystem */
    p_retval = diagpkt_err_rsp(DIAG_BAD_PARM_F,  (PACKED void*) p_Req, w_PktLength);
  }

  return p_retval;
} /* end gpsdiag_OemDrSync() */

PACKED void* gpsdiag_OemDrSyncSetCfg(PACKED void* p_PktReq, uint16 w_PktLength)
{
  PACKED void *p_retval = NULL;
  gpsdiag_OemDrSyncSetConfig *p_Req = NULL;
  gnss_DRSyncConfigType drSyncCfg;
  boolean oemdrsync_Resp = TRUE;
  gpsdiag_OemControlRespType* p_Resp;

  if(NULL == p_PktReq)
  {
    p_retval = diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength);
    return p_retval;
  }

  if(w_PktLength != sizeof(gpsdiag_OemDrSyncSetConfig))
  {
    p_retval = diagpkt_err_rsp(DIAG_BAD_LEN_F,  p_PktReq, w_PktLength);
    return p_retval;
  }

  p_Req = (gpsdiag_OemDrSyncSetConfig*) p_PktReq;
  memset(&drSyncCfg, 0, sizeof(gnss_DRSyncConfigType));
  drSyncCfg.e_Mode
    = (gnss_DRSyncModeType) p_Req->z_DrSyncConfig.q_Mode;
  drSyncCfg.e_Polarity
    = (gnss_DRSyncPolarityType) p_Req->z_DrSyncConfig.q_Polarity;
  drSyncCfg.f_BiasNs                 = (FLT) p_Req->z_DrSyncConfig.f_BiasNs;
  drSyncCfg.l_PhaseSec               = p_Req->z_DrSyncConfig.l_PhaseSec;
  drSyncCfg.l_PeriodSec              = p_Req->z_DrSyncConfig.l_PeriodSec;
  drSyncCfg.f_TuncStartThresholdMs   = (FLT) p_Req->z_DrSyncConfig.f_TuncStartThresholdMs;
  drSyncCfg.f_TuncStopThresholdMs    = (FLT) p_Req->z_DrSyncConfig.f_TuncStopThresholdMs;
  drSyncCfg.b_TuncIgnore             = p_Req->z_DrSyncConfig.b_TuncIgnore;
  drSyncCfg.b_Armed                  = p_Req->z_DrSyncConfig.b_Armed;

  oemdrsync_Resp = oemDrSyncSetConfig(&drSyncCfg);

  p_Resp = (gpsdiag_OemControlRespType*)
    diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
    CGPS_DIAG_PDAPI_CMD,
    sizeof(gpsdiag_OemControlRespType));

  if(NULL != p_Resp) {
    p_Resp->q_RespResult = (uint32) oemdrsync_Resp;
    p_Resp->q_OemFeature = GPSDIAG_OEMFEATURE_DRSYNC;
    p_Resp->q_OemCommand = GPSDIAG_OEM_DRSYNC_SET_CFG;
    p_retval = p_Resp;
  }
  else {
    /* need to check if a null can sent back to describe error, setting
    * an incorrect error code for now when mem alloc fails from diag
    * subsystem */
    p_retval = diagpkt_err_rsp(DIAG_BAD_PARM_F,  p_PktReq, w_PktLength);
  }

  return p_retval;
}


/** OEM DRE Geofence Log
 *
 * @param p_Req
 * @param w_PktLength
 *
 * @return PACKED void* pointer to response packet
 * @see gpsdiag_ApiCmdsDispatch(). Client should have first been
 *      initialized by gpsdiag_ApiCmdsDispatch().
 */
static PACKED void* gpsdiag_GeofenceInMemLog(gpsdiag_GeofenceInMemLogCmdReqType* p_Req, uint16 w_PktLength)
{
   PACKED void* p_RspPkt;
   gpsdiag_GeofenceInMemLogCmdRspType* p_Resp;
   boolean b_Resp = TRUE;

   switch (p_Req->q_GeofenceCommand)
   {
      case GPSDIAG_GEOFENCE_IN_MEM_LOG_START:
      {           
        b_Resp = pdsm_gm_in_mem_logging_start(p_Req->z_LogConfig.q_log_size, p_Req->z_LogConfig.q_period_s);
        break;
      }      
      case GPSDIAG_GEOFENCE_IN_MEM_LOG_STOP:
      {
        b_Resp = pdsm_gm_in_mem_logging_stop();
        break;
      }
   }

  /* Allocate the memory for the response packet */
  p_RspPkt = diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                    CGPS_DIAG_PDAPI_CMD,
                                    sizeof(gpsdiag_GeofenceInMemLogCmdRspType) );
  if (NULL != p_RspPkt)
  {
    p_Resp = (gpsdiag_GeofenceInMemLogCmdRspType*) p_RspPkt;
    p_Resp->z_DiagCmdHdr = p_Req->z_DiagCmdHdr;
    p_Resp->l_ClientId = p_Req->l_ClientId;
    p_Resp->q_Status  = (uint32) b_Resp;
  }
  else
  {
    MSG_ERROR("gpsdiag_OemGeofenceLog - response packet not allocated!", 0, 0, 0);
  }
   return p_RspPkt;

} /* end gpsdiag_OemDre() */

/*
 * @param p_Req
 * @param w_PktLength
 *
 * @return PACKED void* pointer to response packet
 * @see gpsdiag_ApiCmdsDispatch(). Client should have first been
 *      initialized by gpsdiag_ApiCmdsDispatch().
 */

PACKED void* gpsdiag_TestForceMemAllocError(PACKED void* p_PktReq, uint16 w_PktLength)
{
  PACKED void *p_retval = NULL;
  gpsdiag_TestForceMemAllocErrorReqType *p_Req   = NULL;
  gpsdiag_TestForceMemAllocErrorRespType *p_Resp = NULL;
 
  if(NULL == p_PktReq)
  {
    p_retval = diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength);
    return p_retval;
  }

  if(w_PktLength != sizeof(gpsdiag_TestForceMemAllocErrorReqType))
  {
    p_retval = diagpkt_err_rsp(DIAG_BAD_LEN_F,  p_PktReq, w_PktLength);
    return p_retval;
  }

  p_Req = (gpsdiag_TestForceMemAllocErrorReqType*) p_PktReq;

  switch ( p_Req->q_Mode )
  {
    case C_HEAP_ALLOC_FAIL_TEST_NONE:
    case C_HEAP_ALLOC_FAIL_TEST_GNSS:
    case C_HEAP_ALLOC_FAIL_TEST_SBAS:
    case C_HEAP_ALLOC_FAIL_TEST_ALL:
      mgp_SetHeapAllocFailTestVolatile( p_Req->q_Mode );
      break;
    default:
      mgp_SetHeapAllocFailTestVolatile( C_HEAP_ALLOC_FAIL_TEST_NONE );
      break;
  }

  p_Resp = (gpsdiag_TestForceMemAllocErrorRespType*)
    diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
    CGPS_DIAG_PDAPI_CMD,
    sizeof(gpsdiag_TestForceMemAllocErrorRespType));

  if (NULL != p_Resp) {
    p_Resp->z_DiagCmdHdr.u_CgpsCmdCode = p_Req->z_DiagCmdHdr.u_CgpsCmdCode;
    p_Resp->z_DiagCmdHdr.u_Version     = GPSDIAG_MSG_VERSION_1;
    p_Resp->q_RespResult = (uint32) GPSDIAG_SUCCESS;
    p_retval = p_Resp;
  }
  else {
    /* need to check if a null can sent back to describe error, setting
    * an incorrect error code for now when mem alloc fails from diag
    * subsystem */
    p_retval = diagpkt_err_rsp(DIAG_BAD_PARM_F,  p_PktReq, w_PktLength);
  }

  return p_retval;
}
