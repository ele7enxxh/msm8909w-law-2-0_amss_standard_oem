/*============================================================================
  @file sns_init_dsps.c

  @brief
  This implements the initialization of the sensors subsystem required for
  DSPS builds.

  Provides an "sns_init()" function which is called from the OSIdleHook as
  registered with the uCOS idle thread.

  <br><br>

  DEPENDENCIES: This uses OS services defined in sns_osa.h.
  It initializes all of the modules on the apps processor, so it depends on each
  module's init function.

  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dsps/src/sns_init_dsps.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who    what, where, why
  ---------- --- -----------------------------------------------------------
  2015-08-13 gju Remove smr_utils
  2015-03-19 hw  Remove sensors' usage of smsm
  2014-09-17 ps  Remove AVS component dependency
  2014-05-24 vy  Fixed sns_init_once bug in init flag gating
  2013-07-26 vh  Eliminated compiler warnings
  2013-03-27 hw  Fix sensors F3 trace buffer issue for MultiPD
  2013-01-29 gju Include different file for SMR utlity functions.
  2012-11-15 ps  Added sns_heap_init()
  2012-10-02 ag  SMR/QMI cleanup
  2011-11-18 sc  Fix compilation warnings
  2011-08-23 sc  Fix compilation error for PCSIM.
  2011-08-23 jtl Init the error handler and SMSM driver.
  2011-08-23 agk Conditional compilation of timetick_offset_init() for non PCSIM targets.
  2011-08-10 agk Added timetick_offset_init() in order to init offset between SLP timer and MPM timer.
  2011-02-24 br  Changed variable "init" to "sns_init_done_flag", and make it golbal for profiling
  2010-12-20 pg  Changed DALSYS_InitMod() param to be compatible with new
                 DAL release.
  2010-11-10 pg  Changes for using sensors mem manager for DAL Heap.
  2010-10-19 pg  Compiler warning fix.
  2010-09-17 pg  Added featurization for test builds.
  2010-08-30 jtl Adding DSPS specfic init.
  2010-08-27 jtl Initial revision

  ============================================================================*/

/*============================================================================

  INCLUDE FILES

  ============================================================================*/

#include "sensor1.h"
#include "sns_init.h"
#include "sns_osa.h"
#ifdef FEATURE_SMDL
#include "smd_lite.h"
#include "smem.h"
#include "err.h"
#endif
#include "sns_memmgr.h"
#include "sns_pm.h"

#include "sns_profiling.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ============================================================================*/

#define SNS_MODULE SNS_MODULE_DSPS_INIT

#define SNS_INIT_FLAG_NAME "init"

#define SNS_INIT_FLAG_DONE 0x0001

#define SNS_F3_TRACE_SIZE 16*1024

/*============================================================================
  Type Declarations
  ============================================================================*/

/*============================================================================
 * Global Data Definitions
 ============================================================================*/

/*============================================================================
  Static Variable Definitions
  ============================================================================*/

/** Flag group used for synchronizing initialization. Each "init" function
    posts to the flag group when its initialization is complete by calling
    sns_init_done() */
static OS_FLAG_GRP *sns_init_flag_grp = NULL;
extern boolean diag_lsm_f3_trace_init(uint8 *address,unsigned int length);

/*============================================================================
  Static Function Definitions and Documentation
  ============================================================================*/

/*===========================================================================

  FUNCTION:   sns_init_once

  ===========================================================================*/
/*!
  @brief Performs all one-time initialization.

  This function is called via "sns_init()" to insure that it can only be
  called once.

  This function will call all of the modules' initialization functions.


  @sideeffects
  None directly -- although each module's init function may have side effects.

*/
/*=========================================================================*/
static void sns_init_once( void )
{
  int      i;
  INT8U    err;
  OS_FLAGS flags = 0;

  const sns_init_fcn init_ptrs[] = SNS_INIT_FUNCTIONS;

  if ( SNS_SUCCESS != sns_heap_init()) {
      MSG(MSG_SSID_SNS, DBG_ERROR_PRIO, "Sensors Heap Init failed\n");
  }

  sns_init_flag_grp = sns_os_sigs_create( SNS_INIT_FLAG_DONE, &err );

  SNS_ASSERT(NULL != sns_init_flag_grp);

  for( i = 0; NULL != init_ptrs[i]; i++ ) {
    //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sensors Init : %d", i);
    if( SNS_SUCCESS != init_ptrs[i]() ) {
      /* Handle error */
      //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sensors Init FAIL: %d", i);
      sns_init_done();
    }

    while( !(SNS_INIT_FLAG_DONE & flags) ) {
      /* Continue polling for the flag until module init is done */
      flags = sns_os_sigs_pend( sns_init_flag_grp,
                                SNS_INIT_FLAG_DONE,
                                OS_FLAG_WAIT_SET_ANY,
                                0,
                                &err );
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sensors Init : waiting(%x)", flags);
    }
    flags = 0;
  }

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sensors Init : ///////////init once completed///////////");
}

/*============================================================================
  Externalized Function Definitions
  ============================================================================*/

/*===========================================================================

  FUNCTION:   sns_init_dsps

  ===========================================================================*/
/*!
  @brief Performs init of various DSPS services which are required to be ready
  before the SNS modules are initialized.

  @sideeffects

*/
/*=========================================================================*/
sns_err_code_e sns_init_dsps( void )
{
  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "DSPS INIT");
  sns_err_code_e rv = SNS_SUCCESS; // Return Value
#ifdef DALSYS_UCOS_SHIM
  DALSYS_InitMod(NULL);
  OS_CPU_INT_FREE(); // Make sure interrupts are enabled
#endif
#ifdef FEATURE_SMDL
  smem_init();
  err_init();
  smd_init();
  if( 0 != smdl_init( 0 ) ) {
    rv = SNS_ERR_FAILED;
  }
#endif
#if !defined (SNS_PCSIM) && !defined (QDSP6)
  timetick_offset_init();
#endif
  sns_init_done();
  return rv;
}

/*===========================================================================

  FUNCTION:   sns_init_done
  - Documented in sns_init.h

  ===========================================================================*/
void sns_init_done( void )
{
  INT8U err;
  if( NULL != sns_init_flag_grp ) {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "INIT DONE");
    (void) sns_os_sigs_post( sns_init_flag_grp,
                             SNS_INIT_FLAG_DONE,
                             OS_FLAG_SET,
                            &err );
  }
}


/*===========================================================================

  FUNCTION:   sensor1_init
  - Documented in sns_init.h

  ===========================================================================*/
#ifdef SNS_DSPS_PROFILE_ON
int8_t sns_init_done_flag = FALSE;
#else
static int8_t sns_init_done_flag = FALSE;
#endif
sns_err_code_e sns_init( void )
{
  uint8_t *snsF3TraceBufPtr;

  /*Initialize profiling instrumentation*/
  sns_profiling_init();

  if( FALSE == sns_init_done_flag )
  {
    sns_init_once();

    //Allocate a buffer from heap for DIAG messages
    if( NULL != (snsF3TraceBufPtr = SNS_OS_MALLOC( SNS_DBG_MOD_DSPS_SMGR, SNS_F3_TRACE_SIZE )) )
    {
        //if( TRUE == diag_lsm_f3_trace_init( snsF3TraceBufPtr, SNS_F3_TRACE_SIZE ) )
        {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sensors DIAG F3 Trace Buffer Initialized");
        }
        //else
        {
            //MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Can not initialize DIAG F3 Trace Buffer - this is okay, RAM dump will not have Sensors messages");
        }
    }
    else
    {
        MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Unable to allocate memory for DIAG F3 Trace Buffer - this is okay, RAM dump will not have Sensors messages");
    }
    sns_init_done_flag = TRUE;
    return SNS_SUCCESS;
  }
  return SNS_ERR_NOTALLOWED;
}
