/*===========================================================================

                        QM_TIB.C 

DESCRIPTION

 QMI_MMODE source file for CDMA time indication blocking module.

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:
  

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/09/13    aa     Initial version
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "qm_comdef.h"
#include "qm_tib.h"
#include "msg.h"
#include "mcfg_fs.h"
#include "qm_util.h"

#define TIB_EFS_FILE "/nv/item_files/modem/mmode/qmi/tib_timer"

//===========================================================================
// GLOBALS
//===========================================================================
static struct qm_tib_global_s tib_global;

//===========================================================================
// Function declarations
//===========================================================================
static boolean is_timer_running(void);
static boolean is_cdma_time_info_changed( sys_cdma_time_s_type *cdma_info );
static boolean is_hdr_time_info_changed( sys_hdr_time_s_type *hdr_info );

/*===========================================================================
  FUNCTION QM_TIB_INIT()

  DESCRIPTION
    This function initializes the time indication blocking global variables and defines timer

  PARAMETERS
    func : Call back function which is to be called when timer expires

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_tib_init ( timer_t1_cb_type   func )
{
  memset( &tib_global, 0x00, sizeof(struct qm_tib_global_s) );

  if( func != NULL )
  {
    tib_global.timer.interval    = 0xFFFFFFFF;

    timer_def( &tib_global.timer.timer,               // timer object
               NULL,                                  // timer group (needed if timer is critical and needs to expire while device is in sleep)
               NULL,                                  // task to be called when timer expires (needed when signal parameter is provided, not needed with callback function)
               0,                                     // signal to be generated when timer expires
               func,                                  //call back function
               (timer_cb_data_type)QM_TIMER_TIB       // timer id
             );

    QM_MSG_HIGH("qm_tib_init timer defined");
  }
}

/*===========================================================================
  FUNCTION IS_TIMER_RUNNING()

  DESCRIPTION
    This functions determines if the timer is running.

  PARAMETERS
    None

  RETURN VALUE
    TRUE if timer is active, FALSE otherwise (even if timer is paused)

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean is_timer_running(void)
{
  return timer_is_active( &tib_global.timer.timer );
}

/*===========================================================================
  FUNCTION QM_TIB_BLOCK_TIME_IND()

  DESCRIPTION
     This functions starts running a timer and blocks time indications until 
     timer expires.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_tib_block_time_ind (void)
{
  if( tib_global.timer.is_valid )
  {
    if( is_timer_running() )
    {
      // stop timer first, unblock time ind and then start timer again
      timer_clr( &tib_global.timer.timer, T_SEC );
    }

    tib_global.is_time_ind_blocked = TRUE;
    timer_set( &tib_global.timer.timer, tib_global.timer.interval, 0, T_SEC);
    QM_MSG_HIGH_1("starting timer interval %d secs", tib_global.timer.interval);
  }
}

/*===========================================================================
  FUNCTION QM_TIB_UNBLOCK_TIME_IND()

  DESCRIPTION
    This functions stops the running timer and unblocks the time indications.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_tib_unblock_time_ind (void)
{
  if( tib_global.timer.is_valid && is_timer_running() )
  {
    timer_clr( &tib_global.timer.timer, T_SEC); // stops the active timer
    QM_MSG_HIGH_1("stopping timer interval %d secs", tib_global.timer.interval);
  }

  tib_global.is_time_ind_blocked = FALSE;
}

/*===========================================================================
  FUNCTION QM_TIB_UPDATE_TIME_INFO()

  DESCRIPTION
    This function is used to update the 3gpp2 time info received in cm ss event into qmi_nasi_global cache
    
  PARAMETERS
    p_info        : pointer to the latest CM SS EVENT

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_tib_update_time_info( cm_mm_msim_ss_info_s_type *p_info )
{
  enum qm_stack_index_e stack = STACK_INDEX_MAX;
  if(p_info)
  {
    if( qm_util_is_in_service( QM_SUBS_PRI, QM_RAT_1X, p_info, FALSE, &stack ) && p_info->stack_info[stack].mode_info.cdma_info.time_info.sys_mode == SYS_SYS_MODE_CDMA )
    {
      tib_global.cdma_cache.is_valid           = TRUE;
      tib_global.cdma_cache.cdma.lp_sec        = p_info->stack_info[stack].mode_info.cdma_info.time_info.time.cdma_time.lp_sec;
      tib_global.cdma_cache.cdma.daylt_savings = p_info->stack_info[stack].mode_info.cdma_info.time_info.time.cdma_time.daylt_savings;
      tib_global.cdma_cache.cdma.ltm_offset    = p_info->stack_info[stack].mode_info.cdma_info.time_info.time.cdma_time.ltm_offset;
    }
    else if( qm_util_is_in_service( QM_SUBS_PRI, QM_RAT_HDR, p_info, FALSE, &stack ) && p_info->stack_info[stack].mode_info.cdma_info.time_info.sys_mode == SYS_SYS_MODE_HDR )
    {
      tib_global.hdr_cache.is_valid       = TRUE;
      tib_global.hdr_cache.hdr.lp_sec     = p_info->stack_info[stack].mode_info.cdma_info.time_info.time.hdr_time.lp_sec;
      tib_global.hdr_cache.hdr.ltm_offset = p_info->stack_info[stack].mode_info.cdma_info.time_info.time.hdr_time.ltm_offset;
    }
  }
}

/*===========================================================================
  FUNCTION IS_CDMA_TIME_INFO_CHANGED()

  DESCRIPTION
    This function is used to check if CDMA time information received in cm ss event changed from CDMA global cache
    
  PARAMETERS
    p_info        : pointer to the CDMA time info

  RETURN VALUE
    TRUE if CDMA time info received is changed from saved global cache or cache is not initialized, otherwise FALSE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean is_cdma_time_info_changed( sys_cdma_time_s_type *cdma_info )
{
  boolean retval = FALSE;
 
  if( !tib_global.cdma_cache.is_valid )
  {
    retval = TRUE;
  }
  else
  {
    if( cdma_info != NULL )
    { 
      if( tib_global.cdma_cache.cdma.lp_sec != cdma_info->lp_sec )
      { 
        retval = TRUE; 
      }
      else if( tib_global.cdma_cache.cdma.ltm_offset != cdma_info->ltm_offset )
      { 
        retval = TRUE; 
      }
      else if( tib_global.cdma_cache.cdma.daylt_savings != cdma_info->daylt_savings )
      { 
        retval = TRUE; 
      }
    }
  }

  if(retval == TRUE)
  {
  QM_MSG_HIGH_1("is_cdma_time_info_changed %d", retval);
  }
  return retval;
}

/*===========================================================================
  FUNCTION IS_HDR_TIME_INFO_CHANGED()

  DESCRIPTION
    This function is used to check if HDR time information received in cm ss event changed from HDR global cache
    
  PARAMETERS
    hdr_info        : pointer to the HDR time info

  RETURN VALUE
    TRUE if HDR time info received is changed from saved global cache or cache is not initialized,
    otherwise FALSE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean is_hdr_time_info_changed( sys_hdr_time_s_type *hdr_info )
{
  boolean retval = FALSE;
 
  if( !tib_global.hdr_cache.is_valid )
  {
    retval = TRUE;
  }
  else
  {    
    if( hdr_info != NULL )
    {
      if( tib_global.hdr_cache.hdr.lp_sec != hdr_info->lp_sec )
      { 
        retval = TRUE; 
      }
      else if( tib_global.hdr_cache.hdr.ltm_offset != hdr_info->ltm_offset )
      { 
        retval = TRUE; 
      }
    }
  }

  if(retval == TRUE)
  {
  QM_MSG_HIGH_1("is_hdr_time_info_changed %d", retval);
  }
  return retval;
}

/*===========================================================================
  FUNCTION QM_TIB_IS_TIME_INFO_CHANGED()

  DESCRIPTION
    This function is used to determine if 3gpp2 time info received in cm ss event changed from global cache
    
  PARAMETERS
    p_info        : pointer to the latest CM SS EVENT

  RETURN VALUE
    TRUE if 3gpp2 time info received in CM SS EVENT changed from global cache or cache is not initialized,
    otherwise FALSE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_tib_is_time_info_changed( cm_mm_msim_ss_info_s_type *p_info )
{
  boolean retval = FALSE;
  enum qm_stack_index_e stack = STACK_INDEX_MAX;

  if( p_info != NULL)
  {
    if( qm_util_is_in_service( QM_SUBS_PRI, QM_RAT_1X, p_info, FALSE, &stack ) && p_info->stack_info[stack].mode_info.cdma_info.time_info.sys_mode == SYS_SYS_MODE_CDMA )
    {
      retval = is_cdma_time_info_changed( &p_info->stack_info[stack].mode_info.cdma_info.time_info.time.cdma_time );
    }
    else if( qm_util_is_in_service( QM_SUBS_PRI, QM_RAT_1X, p_info, FALSE, &stack ) && p_info->stack_info[stack].mode_info.cdma_info.time_info.sys_mode == SYS_SYS_MODE_HDR )
    {
      retval = is_hdr_time_info_changed( &p_info->stack_info[stack].mode_info.cdma_info.time_info.time.hdr_time );
    }
  }

  if(retval == TRUE)
  {
  QM_MSG_HIGH_1("qm_tib_is_time_info_changed returned %d", retval);
  }
  return retval;
}

/*===========================================================================
  FUNCTION QM_TIB_PROCESS_TIMER()

  DESCRIPTION
    This function is used to set the block_time_ind global to FALSE to block time indications
    
  PARAMETERS
    NONE

  RETURN VALUE
    NONE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_tib_process_timer ( void )
{
  tib_global.is_time_ind_blocked = FALSE;
}

/*===========================================================================
  FUNCTION QM_TIB_IS_TIME_IND_BLOCKED()

  DESCRIPTION
    This function returns the value of block_time_ind global variable
    
  PARAMETERS
    NONE

  RETURN VALUE
    TRUE if time indications are blocked, FALSE otherwise
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_tib_is_time_ind_blocked( void )
{
  return tib_global.is_time_ind_blocked;
}

/*===========================================================================
  FUNCTION QM_TIB_CHECK_AND_INVALIDATE_CDMA_HDR_CACHE()

  DESCRIPTION
    This function is used to check if CDMA or HDR is in service. If any of them is not in service, the corresponding cache
    values are invalidated.
    
  PARAMETERS
    p_info        : pointer to the latest CM SS EVENT

  RETURN VALUE
    NONE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_tib_check_and_invalidate_cdma_hdr_cache( cm_mm_msim_ss_info_s_type *p_info )
{  
  if(p_info)
  {
    if( !qm_util_is_in_service( QM_SUBS_PRI, QM_RAT_1X, p_info, FALSE, NULL ) )
    {
      tib_global.cdma_cache.is_valid = FALSE;
    }
    
    if( !qm_util_is_in_service( QM_SUBS_PRI, QM_RAT_HDR, p_info, FALSE, NULL ) )
    {
      tib_global.hdr_cache.is_valid = FALSE;
    }
  }
}

/*===========================================================================
  FUNCTION QM_TIB_IS_ENABLED()

  DESCRIPTION
    This function determines if time indication blocking module is initialized and valid to use. When called for the first 
    time, this function attempts to read the EFS file to get the valid timer interval.
    
  PARAMETERS
    NONE

  RETURN VALUE
    TRUE if timer is initialized with valid interval, otherwise FALSE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_tib_is_enabled( void )
{
  mcfg_fs_status_e_type status;
  if( !tib_global.is_efs_read ) // we haven't read the timer interval yet, read EFS file and start timer
  {
    // read efs file and mark is_efs_read as TRUE so that EFS file is not attempted to read next time
    status = mcfg_fs_read (TIB_EFS_FILE, (byte *)&tib_global.timer.interval, (uint32) sizeof(uint32), MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );
    tib_global.is_efs_read = TRUE;
 
    if( status == MCFG_FS_STATUS_OK )
    {
      QM_MSG_HIGH_1("efs_get timer read success interval %d secs", tib_global.timer.interval);
 
      if( tib_global.timer.interval != 0xFFFFFFFF ) 
      { 
        tib_global.timer.is_valid = TRUE;
      }
    }
    else
    {
      QM_MSG_ERROR_1("efs_get timer read failed status %d", status);
    }
  }

  return tib_global.timer.is_valid;
}

