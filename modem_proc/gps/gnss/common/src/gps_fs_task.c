/*==============================================================================

                          GPS FILE SYSTEM MODULE

GENERAL DESCRIPTION
   gps_fs_task.c contains all the basic functions associated with the task.

 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/src/gps_fs_task.c#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 

==============================================================================*/


/* Include files:
*/

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cgps_api.h"
#include "gps_common.h"
#include "mgp_api.h"
#include "gnss_gdt_protected.h"


#include "err.h"        /* Interface to error log services */
#include "msg.h"        /* Interface to F3 messages services */

#include "fs_public.h"
#include "fs_errno.h"

#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "aries_ipc_diag_log.h"

#include "gps_fs_api.h"
#define GPS_FS_DATA_DECLARE
#include "gps_fs_task.h"
#undef  GPS_FS_DATA_DECLARE

/*
 * Constant definitions
*/
#define GPS_FS_TASK_NV_TIMER_ID        10
#define GPS_FS_TASK_NV_TIMER_COUNT     60000

/* See defintion of MSG_SPRINTF in msg_diag_service.h.  This also checks if the proper debug
** level is enabled and only executes the print statement if it is. */
#define GPSFS_MSG_SPRINTF_1(level, fmt, arg1) \
        do { \
          if( msg_status( MSG_SSID_MGPPE, level ) ) \
          { \
            MSG_SPRINTF_1(MSG_SSID_MGPPE, \
                          level, \
                          fmt, \
                          arg1 \
                         ); \
          } \
        } while(0)

#define GPSFS_MSG_SPRINTF_2(level, fmt, arg1, arg2) \
        do { \
          if( msg_status( MSG_SSID_MGPPE, level ) ) \
          { \
            MSG_SPRINTF_2(MSG_SSID_MGPPE, \
                          level, \
                          fmt, \
                          arg1, \
                          arg2 \
                         ); \
          } \
        } while(0)
          
#define GPSFS_MSG_SPRINTF_3(level, fmt, arg1, arg2, arg3) \
        do { \
          if( msg_status( MSG_SSID_MGPPE, level ) ) \
          { \
            MSG_SPRINTF_3(MSG_SSID_MGPPE, \
                          level, \
                          fmt, \
                          arg1, \
                          arg2, \
                          arg3 \
                         ); \
          } \
        } while(0)

/*
 * Local data declarations.
*/
#ifndef FEATURE_GNSS_NO_NV_WRITES
static os_TimerMsgType* p_NvLoaderTimer;
#endif /* FEATURE_GNSS_NO_NV_WRITES */

static uint32 l_efsPutStatus[GPS_FS_TASK_MAX_MESSAGE];

static volatile enum { FS_TASK_STATE_RUN, FS_TASK_STATE_STOP } e_fs_task_state;


/*
 ******************************************************************************
 * temp
 ******************************************************************************
*/
void gps_fs_AliveMsg( void )
{
  MSG_HIGH("RECEIVE GPS FS TASK ALIVE ********",0,0,0);
}

/*
 ******************************************************************************
 * gps_fs_Shutdown
 *
 * Function description:
 *
 *  gps_fs_Shutdown saves information used by GPS FS prior to system shutdown
 *
 * Parameters: 
 *
 *  None
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void gps_fs_Shutdown( void )
{

}

/*
 ******************************************************************************
 * Function GPS_FS_HANDLE_DOG
 *
 * Description:
 *
 *  This function is used to handle the Watchdog IPC
 *
 * Parameters: 
 *
 *  None
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void gps_fs_HandleDog( void )
{
  ERR_FATAL("Failed GPS FS dog report timer!", 0, 0, 0);
}

/*
 ******************************************************************************
 * Function gps_fs_HandleStop
 *
 * Description:
 *
 *  This function is used to handle Stop IPC
 *
 * Parameters: 
 *
 *  None
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void gps_fs_HandleStop( void )
{

  MSG_MED(" TASK_STOP_IPC received by GPS FS",0,0,0);
  
  gps_fs_Shutdown();

  os_TaskStopAck();
}

/*
 ******************************************************************************
 * Function gps_fs_HandleOffline
 *
 * Description:
 *
 *  This function is used to handle TASK_OFFLINE IPC
 *
 * Parameters: 
 *
 *  None
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void gps_fs_HandleOffline( void )
{
  MSG_MED(" TASK_OFFLINE_IPC received by GPS FS",0,0,0);
}

/*
 ******************************************************************************
 * FillMgpNotificationData
 *
 * Function description:
 *
 *  FillMgpNotificationData fill the notification data  from the EFS commands
 *
 * Parameters: 
 *
 *  pz_Msg - output notification data
 *  pz_GpsFs - input FS command information
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
static void FillMgpNotificationData(mgp_GpsFsTaskNotificationDataType * pz_Msg,
                                 const gps_fs_u_cmd_type *pz_GpsFs)
{
  if (NULL == pz_Msg || NULL == pz_GpsFs)
  {
    GPSFS_MSG_SPRINTF_2(MSG_LEGACY_ERROR,
      "FillMgpNotificationData: null pointer %u, %u",
      NULL != pz_Msg, NULL != pz_GpsFs);

    return;
  }
  memset(pz_Msg, 0, sizeof(*pz_Msg));

  pz_Msg->u_FileOp = pz_GpsFs->u_FileOP;
  pz_Msg->w_RecId = pz_GpsFs->u_Reserved[1] << 8 | pz_GpsFs->u_Reserved[0];

  pz_Msg->t_Mask =  (pz_GpsFs->u_FileOP == GPS_FS_DELETE_FILE_LIST) ? 
    pz_GpsFs->t_DeleteMask : 0;
}

/*
 ******************************************************************************
 * gps_fs_ProcEfsCmd
 *
 * Function description:
 *
 *  gps_fs_ProcEfsCmd processes EFS commands sent to the GPS FS task
 *
 * Parameters: 
 *
 *  None
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void gps_fs_ProcEfsCmd( gps_fs_u_cmd_type *p_gpsfsPtr, uint8 u_msgTask )
{
  int32  l_efsRet;
  int32  l_efsSyncRet = 0;
  struct fs_dirent *dirent = NULL;
  struct fs_stat    f_stat;
  uint32 fmode;
  EFSDIR *dirp             = NULL;

  #ifdef FEATURE_CGPS_PLAYBACK
  PE_MSG_HIGH_1("Received fileop %d", p_gpsfsPtr->u_FileOP);
  #endif

  if ( p_gpsfsPtr->u_FileOP == GPS_FS_CREATE_WRITE_FILE )
  {
    if( p_gpsfsPtr->q_DataSize > GPS_FS_TASK_EFS_PUT_SIZE )
    {
      l_efsRet = efs_put( (const char *) p_gpsfsPtr->p_FileName, (void *) p_gpsfsPtr->p_DataPtr,
                             ( fs_size_t) p_gpsfsPtr->q_DataSize, O_CREAT | O_AUTODIR,
                            0777 );
      if ( l_efsRet < 0 )
      {
        l_efsPutStatus[u_msgTask] = efs_errno;
      }
      else
      {
        l_efsPutStatus[u_msgTask] = 0;
      }
      GPSFS_MSG_SPRINTF_3(MSG_LEGACY_HIGH, "gps_fs_task:%s EFS_PUT %lu  Error Code: %d\n", 
                     (char *)(p_gpsfsPtr->p_FileName), p_gpsfsPtr->q_DataSize, efs_errno);
    }
    else
    {
      fs_ssize_t       bytes_written=0;
      fs_ssize_t  bytes_written_temp = 0;
      int   fd;
    
      /* Open existing file. If file does not exist create new 
         file. Also set access permissions to R/W */
      fd = efs_open ((char * )p_gpsfsPtr->p_FileName, O_RDWR | O_CREAT | O_TRUNC, S_IWRITE | S_IREAD);

      if( fd < 0 )
      {
        GPSFS_MSG_SPRINTF_2(MSG_LEGACY_HIGH, "gps_fs_task: FILE OPEN ERR: %s Code: %d\n", 
                            (char *)(p_gpsfsPtr->p_FileName), efs_errno);
        l_efsPutStatus[u_msgTask] = efs_errno;

        /* Diag Task: return status in Call back function */
        if ( u_msgTask == GPS_FS_TASK_DIAG_MESSAGE )
        {
          p_gpsfsPtr->gps_fs_efs_status_get(l_efsPutStatus[u_msgTask]);
        }

        /* Send the Mesasge to CD task, GPS FS Task has finished writing message */
        if ( u_msgTask == GPS_FS_TASK_MGP_MESSAGE )
        {
          mgp_GpsFsTaskNotificationDataType z_NotificationData;
          FillMgpNotificationData(&z_NotificationData, p_gpsfsPtr);
          mgp_GpsFsTaskNotification( &z_NotificationData );

          GPSFS_MSG_SPRINTF_1(MSG_LEGACY_HIGH, "gps_fs_task: GPS_FS_TASK_DONE Sent: %s \n", 
                              (char *)(p_gpsfsPtr->p_FileName));
        }

        return ;
      }

      while ( bytes_written < p_gpsfsPtr->q_DataSize )
      {
          /* Write the required number of bytes to the file */
          bytes_written_temp = efs_write (fd, ( void *)( (uint8 *)p_gpsfsPtr->p_DataPtr + bytes_written ), 
                                p_gpsfsPtr->q_DataSize - bytes_written);

          if ( bytes_written_temp < 0 )
          {
            GPSFS_MSG_SPRINTF_1(MSG_LEGACY_HIGH, "gps_fs_task: FILE WRITE ERR: %s\n", 
                                (char *)(p_gpsfsPtr->p_FileName));
            l_efsPutStatus[u_msgTask] = efs_errno;
            break;
          }

          if ( bytes_written_temp == 0 )
            break;

          bytes_written += bytes_written_temp;
      }
      if(bytes_written == p_gpsfsPtr->q_DataSize )
      {
          l_efsPutStatus[u_msgTask] = 0;
      }
      GPSFS_MSG_SPRINTF_3(MSG_LEGACY_HIGH, "gps_fs_task: EFS_PUT: %s %lu  bytes written : %ld\n", 
                          (char *)(p_gpsfsPtr->p_FileName), p_gpsfsPtr->q_DataSize, bytes_written);
      (void) efs_close(fd);
      /* if we want the file to be committed immediately to EFS instead of shutdown,
      call EFS sync. u_Reserved[3] is used to flag if this is needed */ 
      if ( p_gpsfsPtr->u_Reserved[3] == 1 )
      {
  #ifdef EFS_HAVE_SYNC
        l_efsSyncRet = efs_sync((char *)(p_gpsfsPtr->p_FileName));
        MSG_HIGH("gps_fs_task: EFS_PUT. Efs sync called. ret %ld\n", 
                        l_efsSyncRet, 0, 0);
        if ( l_efsSyncRet !=0  )
        {
          l_efsPutStatus[u_msgTask] = efs_errno;
        }
  #endif
      }
    }
  }   
  else if ( p_gpsfsPtr->u_FileOP == GPS_FS_APPEND_FILE )
  {
    fs_ssize_t       bytes_written=0;
    fs_ssize_t  bytes_written_temp = 0;
    int fd;
    
    /* Open existing file. If file does not exist create new 
       file. Also set access permissions to R/W */
    fd = efs_open ((char * )p_gpsfsPtr->p_FileName, O_RDWR | O_CREAT | O_APPEND, S_IWRITE | S_IREAD);

    if( fd < 0 )
    {
      GPSFS_MSG_SPRINTF_2(MSG_LEGACY_HIGH, "gps_fs_task: FILE OPEN ERR: %s Code: %d\n", 
                          (char *)(p_gpsfsPtr->p_FileName), efs_errno);
      l_efsPutStatus[u_msgTask] = efs_errno;

      /* Diag Task: return status in Call back function */
      if ( u_msgTask == GPS_FS_TASK_DIAG_MESSAGE )
      {
        p_gpsfsPtr->gps_fs_efs_status_get(l_efsPutStatus[u_msgTask]);
      }

      /* Send the Mesasge to CD task, GPS FS Task has finished writing message */
      if ( u_msgTask == GPS_FS_TASK_MGP_MESSAGE )
      {
        mgp_GpsFsTaskNotificationDataType z_NotificationData;
        FillMgpNotificationData(&z_NotificationData, p_gpsfsPtr);
        mgp_GpsFsTaskNotification( &z_NotificationData );

        GPSFS_MSG_SPRINTF_1(MSG_LEGACY_HIGH, "gps_fs_task: GPS_FS_TASK_DONE Sent: %s \n", 
                            (char *)(p_gpsfsPtr->p_FileName));
      }

      return ;
    }

    while ( bytes_written < p_gpsfsPtr->q_DataSize )
    {
      /* Write the required number of bytes to the file */
      bytes_written_temp = efs_write (fd, ( void *)( (uint8 *)p_gpsfsPtr->p_DataPtr + bytes_written ), 
                                p_gpsfsPtr->q_DataSize - bytes_written);

      if ( bytes_written_temp < 0 )
      {
        GPSFS_MSG_SPRINTF_1(MSG_LEGACY_HIGH, "gps_fs_task: FILE WRITE ERR: %s\n", 
                            (char *)(p_gpsfsPtr->p_FileName));
        l_efsPutStatus[u_msgTask] = efs_errno;
        break;
      }

      if ( bytes_written_temp == 0 )
        break;

      bytes_written += bytes_written_temp;
    }
    if(bytes_written == p_gpsfsPtr->q_DataSize )
    {
      l_efsPutStatus[u_msgTask] = 0;
    }
    GPSFS_MSG_SPRINTF_3(MSG_LEGACY_HIGH, "gps_fs_task: EFS_PUT: %s %lu  bytes written : %ld\n", 
                        (char *)(p_gpsfsPtr->p_FileName), p_gpsfsPtr->q_DataSize, bytes_written);
    (void)efs_close(fd);
  }
  else if ( p_gpsfsPtr->u_FileOP == GPS_FS_DELETE_FILE )
  {
    if( efs_unlink ((char *) p_gpsfsPtr->p_FileName ) != 0 )
    {
        l_efsPutStatus[u_msgTask] = efs_errno;
    }
    else
    {
          l_efsPutStatus[u_msgTask] = 0;
    }
    GPSFS_MSG_SPRINTF_2(MSG_LEGACY_HIGH, "gps_fs_task: DELETE: %s status: %lu\n", 
                        (char *)(p_gpsfsPtr->p_FileName), l_efsPutStatus[u_msgTask]);
  }
  /* Delete all regular files (not links, subdirs etc.) from given EFS dir */
  else if ( p_gpsfsPtr->u_FileOP == GPS_FS_PURGE_DIR )
  {
    int32  chars_written      = 0;
    int32  chars_written_temp = 0;
    static char s_Buf[256];
    dirp = efs_opendir ((char*)p_gpsfsPtr->p_FileName);
    /* recursively read all the directory entries */
    while ((dirent = efs_readdir (dirp)) != 0)
    {
      /* first generate the full path of the file. Print the dir name to a buf */
      chars_written = snprintf( s_Buf,
                                sizeof(s_Buf), 
                                "%s", 
                                p_gpsfsPtr->p_FileName);

      /* Catch error and overflow in snprintf */
      if( chars_written < 0 || chars_written >= sizeof(s_Buf))
      {
        MSG_ERROR("Could not generate dir path. err %d", chars_written, 0, 0);
        l_efsPutStatus[u_msgTask] = ENAMETOOLONG;
        break;
      }
      /* Try to add a trailing '/' if not already present. 
       * chars_written holds the number of chars written excluding the trailing
       * '\0'. Check that the buffer is sufficient to hold additional chars 
       * '/' and '\0' */
      if( chars_written > 0 && s_Buf[chars_written - 1] != '/')
      {
        if(chars_written + 2 <= sizeof(s_Buf))
        {
          s_Buf[chars_written] = '/';
          s_Buf[chars_written+1] = '\0';
          chars_written++;
        }
        else
        {
          /* buffer not sufficient to hold path */
          GPSFS_MSG_SPRINTF_1(MSG_LEGACY_HIGH, "Not enough memory to hold file path %s", s_Buf);
          l_efsPutStatus[u_msgTask] = ENAMETOOLONG;
          break;
        }
      }
      /* now add the file name at the end */
      chars_written_temp = snprintf( s_Buf + chars_written, 
                                     sizeof(s_Buf) - chars_written,
                                     "%s",
                                     dirent->d_name);

      /* Catch error and overflow in snprintf */
      if( (chars_written_temp < 0) || 
          (chars_written_temp >= sizeof(s_Buf) - chars_written))
      {
        /* Some error in snprintf */
        MSG_ERROR("Could not generate dir path. err %d", chars_written_temp, 0, 0);
        l_efsPutStatus[u_msgTask] = ENAMETOOLONG;
        break;
      }

      /* Check if mode is returned, otherwise call stat to get mode */
      if (FS_MASK_CHECK (dirent->d_stats_present, FS_DIRENT_HAS_ST_MODE))
      {
        fmode = dirent->d_stat.st_mode;
      }
      else
      {
        if (efs_stat (s_Buf, &f_stat) != 0)
        {
          /* something wrong with this file, check the next one */
          GPSFS_MSG_SPRINTF_1(MSG_LEGACY_HIGH, "could not stat file %s", s_Buf);
          continue;
        }
        fmode = f_stat.st_mode;
      }

      /* check if mode indicates that this is a regular file */
      if (S_ISREG(fmode))
      {
        /* delete file if its a regular file */
        if( efs_unlink ((char *) s_Buf ) != 0 )
        {
            MSG_ERROR("Could not delete file errno = %d", efs_errno, 0, 0);
            l_efsPutStatus[u_msgTask] = efs_errno;
            break;
        }
        else
        {
          GPSFS_MSG_SPRINTF_1(MSG_LEGACY_MED, "Deleted file %s", s_Buf);
        }
      }
    } /* while ((dirent = efs_readdir (dirp)) != 0) */
  } /* if ( p_gpsfsPtr->u_FileOP == GPS_FS_PURGE_DIR ) */
  else if ( p_gpsfsPtr->u_FileOP == GPS_FS_DELETE_FILE_LIST )
  {
    static char s_Buf[256];
    uint64 t_Mask = p_gpsfsPtr->t_DeleteMask;
    uint64 t_FailedDeleteMask = 0LL, t_MaskBit = 1;
    char**  p_Cur = p_gpsfsPtr->p_DataPtr;
    uint32 q_NmbrOfFiles = 0;

    /* q_DataSize is holding max number of files in file list in this case */
    while( t_Mask > 0 && q_NmbrOfFiles < p_gpsfsPtr->q_DataSize)
    {
      if ( t_Mask & (U64)1)
      {
        /* Assumes the file separator / is included in p_FileName */
        (void)snprintf(s_Buf, sizeof(s_Buf), "%s%s",
                     (char*) p_gpsfsPtr->p_FileName, 
                     (char*) (*p_Cur));

        if( efs_unlink( (char *) s_Buf ) != 0 )
        {
          l_efsPutStatus[u_msgTask] = efs_errno;
          t_FailedDeleteMask |= t_MaskBit;
        }
        else
        {
          l_efsPutStatus[u_msgTask] = 0;
        }
      }
      t_Mask >>= 1;
      t_MaskBit <<= 1;
      p_Cur++;
      q_NmbrOfFiles++;
    }

    (void)snprintf(s_Buf,sizeof(s_Buf),"gps_fs_task: DELETE_LIST status: %lu delete mask %llx failed mask %llx\n",
                   l_efsPutStatus[u_msgTask], p_gpsfsPtr->t_DeleteMask, t_FailedDeleteMask );
    MSG_SPRINTF_1(MSG_SSID_MGPPE, MSG_LEGACY_HIGH, "%s", s_Buf);
  } /* if ( p_gpsfsPtr->u_FileOP == GPS_FS_DELETE_FILE_LIST ) */
  else
  {
    MSG_ERROR("Unknown FS API cmd", 0, 0, 0);
  }
  
  
  
  /* Diag Task: return status in Call back function */
  if ( u_msgTask == GPS_FS_TASK_DIAG_MESSAGE )
  {
    p_gpsfsPtr->gps_fs_efs_status_get(l_efsPutStatus[u_msgTask]);
  }
  
  /* Send the Mesasge to CD task, GPS FS Task has finished writing message */
  if ( u_msgTask == GPS_FS_TASK_MGP_MESSAGE )
  {
    mgp_GpsFsTaskNotificationDataType z_NotificationData;
    FillMgpNotificationData(&z_NotificationData, p_gpsfsPtr);
    mgp_GpsFsTaskNotification( &z_NotificationData );
   
    GPSFS_MSG_SPRINTF_1(MSG_LEGACY_HIGH, "gps_fs_task: GPS_FS_TASK_DONE Sent: %s \n", 
                  (char *)(p_gpsfsPtr->p_FileName) );
  }
} 

/*
 ******************************************************************************
 * gps_fs_CellDBEfsRecordRead
 *
 * Function description:
 *
 * This function reads the Cell DB record from the EFS.
 *
 * Parameters:
 *   p_Data:  Pointer to the array that holds the record read from NVS
 *   q_MaxBytes: Maximum number of bytes read from NVS
 *
 * Return value: Number of bytes read; 0 if reading fails
 *
 ******************************************************************************
*/

static uint32 gps_fs_CellDBEfsRecordRead ( uint8 *p_Data, uint32 q_MaxBytes)
{
  int fd;
  char pathname[25];
  uint32 q_BytesRead = 0;
  
  (void) snprintf(pathname, sizeof(pathname), "/CGPS_ME/%s", "CGPSCellDBFile");    

  fd = efs_open (pathname,O_RDONLY);

  if( fd < 0 )
  {
    GPSFS_MSG_SPRINTF_2(MSG_LEGACY_HIGH, "gps_fs_task: FILE OPEN ERR: %s Code: %d\n", 
                        (char *)(pathname), efs_errno);
    return 0;
  } 

  while( q_BytesRead < q_MaxBytes )
  {
  fs_ssize_t bytes_read_temp;
  
    bytes_read_temp = efs_read( fd, p_Data+q_BytesRead, q_MaxBytes-q_BytesRead );
    
    if( bytes_read_temp <= 0 )
    {
      GPSFS_MSG_SPRINTF_2(MSG_LEGACY_HIGH, "gps_fs_task: Unable to Read File: %s Code: %d\n", 
                          (char *)(pathname), efs_errno);
      return 0;
    }
    
    q_BytesRead += (uint32)bytes_read_temp;
  }

  /* Close file */
  if(efs_close(fd) !=0 )
  {
    GPSFS_MSG_SPRINTF_2(MSG_LEGACY_HIGH, "gps_fs_task: FILE CLOSE ERR: %s Code: %d\n", 
                        (char *)(pathname), efs_errno);
  }

  return(q_BytesRead);
}



/*
 ******************************************************************************
 * gps_fs_CellDBEfsRecordWrite
 *
 * Function description:
 *
 * This function writes the Cell Database record to the EFS.
 *
 * Parameters:
 *   p_Data:  Pointer to the array that holds the record read from NVS
 *   q_MaxBytes: Number of bytes to write to NVS
 *
 * Return value: Number of bytes written; 0 if writing fails
 *
 ******************************************************************************
*/

static uint32 gps_fs_CellDBEfsRecordWrite ( const uint8 *p_Data, uint32 q_MaxBytes )
{
  uint32 q_BytesWritten = 0;
  int fd;
  char pathname[25];

  (void) snprintf(pathname,sizeof(pathname),"/CGPS_ME/%s", "CGPSCellDBFile");   

  /* Open already existing file. If file does not exist create new 
      file. Also set access permissions to R/W 
  */
  fd = efs_open (pathname, O_RDWR | O_CREAT, S_IWRITE | S_IREAD);

  if( fd < 0 )
  {
    GPSFS_MSG_SPRINTF_2(MSG_LEGACY_HIGH, "gps_fs_task: File OPEN ERR: %s Code: %d\n", 
                        (char *)(pathname), efs_errno);
    return 0;
  }

  /* Write the required number of bytes to the file */
  while( q_BytesWritten < q_MaxBytes )
  {
  fs_ssize_t bytes_written_temp;
  
    bytes_written_temp = efs_write (fd, p_Data+q_BytesWritten, q_MaxBytes-q_BytesWritten);
    
    if (bytes_written_temp <= 0)
    {
      GPSFS_MSG_SPRINTF_2(MSG_LEGACY_HIGH, "gps_fs_task: File Write Error: %s Code: %d\n", 
                          (char *)(pathname), efs_errno);
      return 0;
    }
    
    q_BytesWritten += (uint32)bytes_written_temp;
  }

  if(q_BytesWritten != q_MaxBytes)
  {
    GPSFS_MSG_SPRINTF_3(MSG_LEGACY_HIGH, "gps_fs_task: EFS WR, Size mismatch: %s %lu %lu\n", 
                        (char *)(pathname), q_BytesWritten, q_MaxBytes);
    return q_BytesWritten;
  }

  /* Close file */
  if(efs_close(fd) !=0 )
  {
    GPSFS_MSG_SPRINTF_2(MSG_LEGACY_HIGH, "gps_fs_task: FILE CLOSE ERR: %s Code: %d\n", 
                        (char *)(pathname), efs_errno);
  }

  return q_MaxBytes;
}

/*
 ******************************************************************************
 * gps_fs_ProcCellDbUpdate
 *
 * Function description:
 *
 * Function to deletes the specified information from Cell DB
 *
 * Parameters:
 *   void
 *
 * Return value: None
 *
 ******************************************************************************
*/

void gps_fs_ProcCellDbUpdate ( gps_fs_celldb_update_type *p_CellDbUpdate )
{
  cgps_CellDBType *p_CellDb, *p_Cell;
  boolean u_CellDbRecordUpdated = FALSE;
  uint32 q_I;

  /* Function to delete the specified information from Cell DB */
  MSG_4( MSG_SSID_MGPPE, MSG_LEGACY_HIGH, "GPS_FS ProcCellDBUpdate %08lX, %d, %u %lu\n", 
         p_CellDbUpdate->q_DeleteCellDBMask,
         p_CellDbUpdate->u_DeleteOldTimetagInfo,
         p_CellDbUpdate->z_MinGpsTimeAllowed.w_GpsWeek,
         p_CellDbUpdate->z_MinGpsTimeAllowed.q_GpsMsec );

  /* First read the Cell DB into a local buffer */
  /* Use os_MemAlloc to obtain memory to read the Cell DB */
  p_CellDb = p_Cell = (cgps_CellDBType*)os_MemAlloc((sizeof(cgps_CellDBType) * C_CELLDB_MAX_CELL_INDEX),OS_MEM_SCOPE_TASK);

  if( p_CellDb != NULL )
  {
    /* Read the Cell DB */
    if( gps_fs_CellDBEfsRecordRead( (uint8 *)p_CellDb, sizeof(cgps_CellDBType) * C_CELLDB_MAX_CELL_INDEX ) )
    {
      /* If Position and Time-tag information is deleted, delete the entire cell database.
         else, delete specific information (as specified). 
      */
      if( (p_CellDbUpdate->q_DeleteCellDBMask & C_DELETE_INFO_CELLDB_POS) &&
          (p_CellDbUpdate->q_DeleteCellDBMask & C_DELETE_INFO_CELLDB_TIMETAG)  
        )
      {
        /* both position and time-tag information is deleted. delete entire cell db */
        memset(p_CellDb, 0, sizeof(cgps_CellDBType) * C_CELLDB_MAX_CELL_INDEX);
        u_CellDbRecordUpdated = TRUE;
      }
      else
      {
        for( q_I = C_CELLDB_MAX_CELL_INDEX; q_I; q_I--, p_Cell++ )
        {
          /* Delete Cell DB Position Info */
          if( p_CellDbUpdate->q_DeleteCellDBMask & C_DELETE_INFO_CELLDB_POS )
          {
            memset( &p_Cell->z_CellDBPos, 0, sizeof(p_Cell->z_CellDBPos) );
            u_CellDbRecordUpdated = TRUE;
          }
    
          /* Delete Cell DB CellID Info */
          if( p_CellDbUpdate->q_DeleteCellDBMask & C_DELETE_INFO_CELLDB_CELLID )
          {
            /* When Cell ID of the cell is deleted, the cell is not longer useful
               Delete the Global cell information and GPS time associated with the cell.
            */
            p_Cell->u_CellGlobalInfoValid = FALSE;
            p_Cell->u_GpsTimeValid = FALSE;
            p_Cell->q_GpsMsec = 0;
            p_Cell->w_GpsWeek = C_GPS_WEEK_UNKNOWN;
            memset( &p_Cell->z_CellInfoGlobal, 0, sizeof(p_Cell->z_CellInfoGlobal) );
            u_CellDbRecordUpdated = TRUE;
          }
    
          /* Delete Cell DB Time-tag Info */
          if( p_CellDbUpdate->q_DeleteCellDBMask & C_DELETE_INFO_CELLDB_TIMETAG )
          {
            #ifdef FEATURE_CGPS_TIME_TAGGING
            memset( &p_Cell->z_CellDBTimeTag, 0, sizeof(p_Cell->z_CellDBTimeTag) );
            u_CellDbRecordUpdated = TRUE;
            #endif /* FEATURE_CGPS_TIME_TAGGING */
          }
          else
          {
            #ifdef FEATURE_CGPS_TIME_TAGGING
            /* Delete time-tag information older than a specified GPS time */
            if( p_CellDbUpdate->u_DeleteOldTimetagInfo && 
                (p_CellDbUpdate->z_MinGpsTimeAllowed.w_GpsWeek != C_GPS_WEEK_UNKNOWN) )
            {
              if( p_Cell->z_CellDBTimeTag.u_Valid && p_Cell->z_CellDBTimeTag.u_GpsTimeValid &&
                  ( (p_Cell->z_CellDBTimeTag.z_GpsTime.w_GpsWeek < p_CellDbUpdate->z_MinGpsTimeAllowed.w_GpsWeek) ||
                    ( (p_Cell->z_CellDBTimeTag.z_GpsTime.w_GpsWeek == p_CellDbUpdate->z_MinGpsTimeAllowed.w_GpsWeek) &&
                      (p_Cell->z_CellDBTimeTag.z_GpsTime.q_GpsMsec < p_CellDbUpdate->z_MinGpsTimeAllowed.q_GpsMsec)
                    )
                  ) 
                )
              {
                /* Old timetag information. Delete now */
                memset( &p_Cell->z_CellDBTimeTag, 0, sizeof(p_Cell->z_CellDBTimeTag) );
                u_CellDbRecordUpdated = TRUE;
              }
            }
            #endif /* FEATURE_CGPS_TIME_TAGGING */
          }
      
          /* Delete Cell DB Neighbor List Info */
          if( p_CellDbUpdate->q_DeleteCellDBMask & C_DELETE_INFO_CELLDB_NEIGHBOR_INFO )
          {
            /* Initialize the Neighbor List info */
            memset( &p_Cell->q_NeighborList[0], 0, ((C_CELLDB_MAX_CELL_INDEX + 31) >> 5) << 2 );
            u_CellDbRecordUpdated = TRUE;
          }
        }
      }

      /* If the Cell DB record has been updated, commit it to the EFS */
      if( u_CellDbRecordUpdated )
      {
        gps_fs_CellDBEfsRecordWrite( (uint8 *)p_CellDb, sizeof(cgps_CellDBType) * C_CELLDB_MAX_CELL_INDEX );
      }
    }
  }

  /* All operations are now complete. os_MemFree up the memory used */
  os_MemFree((void**)&p_CellDb);

}

/******************************************************************************
 * Function description:
 *
 *  gps_fs_Process() dispatches control based upon the flags that are received.
 *
 * Parameters:    q_Flags - type of message.
 *
 * Return value:  none
 *
 ******************************************************************************
*/

void gps_fs_Process( os_IpcMsgType *p_Msg )
{
  switch ( p_Msg->q_MsgId )
  {
  case C_GPS_FS_MSG_TASK_ALIVE:
  {
    gps_fs_AliveMsg();
    break;
  }

  case C_GPS_FS_MSG_DIAG_CMD:
  {
    gps_fs_ProcEfsCmd( (gps_fs_u_cmd_type *)(p_Msg->p_Data), GPS_FS_TASK_DIAG_MESSAGE);
    break;
  }

  case C_GPS_FS_MSG_MGP_CMD:
  {
    gps_fs_ProcEfsCmd( (gps_fs_u_cmd_type *)(p_Msg->p_Data), GPS_FS_TASK_MGP_MESSAGE);
    break;
  }

  case C_GPS_FS_MSG_CELLDB_UPDATE:
  {
    gps_fs_ProcCellDbUpdate( (gps_fs_celldb_update_type *)(p_Msg->p_Data) );
    break;
  }

  case C_GPS_FS_MSG_GDT_EXTENDED_SEND:
  {
    /* This send is expected to have a latency on the order of seconds and, due to
           this, will be executed in the current (FS) task context */
    gdt_ProtectedExtendedSend( (gdt_SendBlobType *)(p_Msg->p_Data) );
    break;
  }

  case C_OS_MSG_ID_TIMER_EXPIRY:
  {
    os_MsgIdEnumType q_MsgId = (os_MsgIdEnumType)p_Msg->q_MsgId;
    if (C_OS_MSG_ID_TIMER_EXPIRY == p_Msg->q_MsgId)
    {
      /* Timer ID is stored in IPC payload */
      q_MsgId = (os_MsgIdEnumType)
                 ((os_TimerExpiryType*)p_Msg->p_Data)->q_TimerId;
    }

    if (C_OS_MSG_ID_DOG_RPT == q_MsgId)
    {
      gps_fs_HandleDog();
    }

    if (GPS_FS_TASK_NV_TIMER_ID == q_MsgId)
    {
      gps_NvInitDefaultWrites();
    }

    break;
  }

  case C_OS_MSG_ID_OFFLINE:
  {
    gps_fs_HandleOffline();
    break;
  }

  case C_OS_MSG_ID_STOP:
  {
    gps_fs_HandleStop();

    break;
  }

  case C_OS_MSG_ID_NV_WRITE:
  {
    gps_NvWriteDone();
    break;
  }

  default:
    break;
  }

  (void)os_IpcDelete(p_Msg);
}

/******************************************************************************
 * Function description:
 *  gps_fs_Init() is called by gps_fs_StartUp to initialize variables in the 
 *  GPS File System task.
 * 
 * Parameters:    None
 *
 * Return value:  None
 *    
 ******************************************************************************
*/

void gps_fs_Init( void )
{
#ifndef FEATURE_GNSS_NO_NV_WRITES
  p_NvLoaderTimer = os_TimerCreate((uint32)GPS_FS_TASK_NV_TIMER_ID,(uint32)THREAD_ID_GPS_FS);
  if (os_TimerStart(p_NvLoaderTimer, GPS_FS_TASK_NV_TIMER_COUNT, 0) == FALSE)
  {
    ERR_FATAL("Failed Default NV Timer Create", 0, 0, 0);
  }
#endif /* FEATURE_GNSS_NO_NV_WRITES */
}

/******************************************************************************
 * Function description:
 *  gps_fs_StartUp() is called to initialize the GPS File System task. 
 * 
 * Parameters:    None
 *          
 * Return value:  None
 *    
 ******************************************************************************
*/

void gps_fs_StartUp( void )
{
  /* Initialize IPC scheme */
  (void)os_IpcStart(THREAD_ID_GPS_FS);

  os_TaskReadyAck();
  os_TaskStopRegister();

  gps_fs_Init();
}

/*=============================================================================

  FUNCTION      gps_fs_task_stop()

  DESCRIPTION 	Exit fs task loop

  DEPENDENCIES  None

  PARAMETERS    None

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void gps_fs_task_stop( void )
{
    e_fs_task_state = FS_TASK_STATE_STOP;
}

/******************************************************************************
 * Function description:
 *  gps_fs_task() is the entry point for the GPS File System task. This 
 *  function implements a DO FOREVER loop simply suspends on an OS flag
 *  event. When a flag event occurs, gps_fs_Process()is dispatched. Performing the
 *  begin / end processing in an isolated function lends itself to RT 
 *  performance analysis with a SPA - This maybe a feature that we would choose
 *  to make use of in the future.
 * 
 * Parameters: 
 *    None
 *
 * Return value: 
 *    gps_fs_Task will never return
 ******************************************************************************
*/

void gps_fs_task( dword dummy )
{
  /* For MSM targets, xx_StartUp() is not called from SystemInit
     because REX initialization is different. 
     So xx_StartUp() needs to be called from this place
  */
  gps_fs_StartUp();

  /* Resort to a suspend forever loop. Task servicing is controlled by 
     flags. */

  e_fs_task_state = FS_TASK_STATE_RUN;

  while ( e_fs_task_state == FS_TASK_STATE_RUN )
  {
    os_IpcMsgType *p_Msg;

    p_Msg = os_IpcReceive();
    
    if(NULL != p_Msg) {
      gps_fs_Process( p_Msg );
    }
  }
}
/*lint !e715 */ 

/* End of gps_fs_task.c */
