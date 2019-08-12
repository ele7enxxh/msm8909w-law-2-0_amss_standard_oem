#ifndef _QM_TIB_H
#define _QM_TIB_H

/*===========================================================================

                         QM_TIB.H

DESCRIPTION

  QMI_MMODE header file for CDMA time indication blocking module.

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
01/09/13    aa     Created file
===========================================================================*/

#include "timer.h"
#include "cm.h"

/*--------------------------------------------------------------------------- 
  data types
---------------------------------------------------------------------------*/

struct qm_tib_global_s
{
  boolean is_time_ind_blocked;
  boolean is_efs_read;

  struct
  {
    boolean       is_valid;
    timer_type    timer;
    timetick_type interval;
  } timer;

  struct
  {
    boolean              is_valid;
    sys_cdma_time_s_type cdma;
  } cdma_cache;

  struct
  {
    boolean             is_valid;
    sys_hdr_time_s_type hdr;
  } hdr_cache;
};

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
void qm_tib_init ( timer_t1_cb_type   func );

/*===========================================================================
  FUNCTION QM_TIB_BLOCK_TIME_IND()

  DESCRIPTION
    This functions starts running a timer and blocks time indications until timer expires.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_tib_block_time_ind ( void );

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
void qm_tib_unblock_time_ind( void );

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
void qm_tib_update_time_info( cm_mm_msim_ss_info_s_type *p_info );

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
boolean qm_tib_is_time_info_changed( cm_mm_msim_ss_info_s_type *p_info );

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
void qm_tib_process_timer ( void );

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
boolean qm_tib_is_time_ind_blocked( void );

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
void qm_tib_check_and_invalidate_cdma_hdr_cache( cm_mm_msim_ss_info_s_type *p_info );

/*===========================================================================
  FUNCTION QM_TIB_IS_ENABLED()

  DESCRIPTION
    This function determines if time indication blocking module is initialized and valid to use. When called for the first 
    time, this function attempts to read the EFS file to get the valid timer interval. 
    
  PARAMETERS
    NONE

  RETURN VALUE
    TRUE if time indication module is valid to use, otherwise FALSE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_tib_is_enabled( void );

#endif // !_QM_TIB_H

