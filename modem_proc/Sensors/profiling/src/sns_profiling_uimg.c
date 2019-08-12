/*=============================================================================
  @file sns_profiling.c

  This file contains sensor use case timeline profiling code.

*******************************************************************************
* Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved
* Qualcomm Technologies Confidential and Proprietary.
********************************************************************************/


/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-09-17   ps   Remove AVS component dependency
  2014-07-22   ps   Fix compiler warnings
  2014-07-20   VY   Disabled logging in uImage
  2014-06-11   vy   Created, refactored for uImage
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#if 0
#include <stdio.h>
#include <qurt.h>
#include "qurt_timer.h"
#include "uTimetick.h"

#include "sns_profiling.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_osa.h"
#include "sns_em.h"
#include "sns_smgr_ddf_if.h"
//#include "sns_ddf_common.h"

extern volatile qurt_mutex_t sns_profiling_mutex;
extern unsigned int sns_pwr_prof_loc_count, sns_pwr_wraparound_count; 
extern volatile sns_pwr_prof_struct sns_pwr_profiler[SNS_PWR_PROFILING_BUF_SIZE] __attribute__ ((aligned (256)));
#endif


