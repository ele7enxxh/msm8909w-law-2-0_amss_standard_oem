/*==============================================================================
  FILE:         uim_logging.c

  OVERVIEW:     File contains the functions for APDU and EFS logging.

  DEPENDENCIES: N/A

                Copyright (c) 2016 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_efs_logging_stubs.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       --------------------------------------------------------- 
04/27/16    na         Draft created for EFS logging function
==============================================================================*/
#include "uim_p.h"
#include "uimi.h"
#include "uimglobals.h"
#include "uim_logging.h"



/**
 * DECLARATIONS OF INTERNAL FUNCTIONS
 */
static boolean uim_efslog_purge_check( uim_instance_global_type *uim_ptr );


/**
 * DEFINITIONS OF EXTERNAL FUNTIONS
 *
 */

/**
 * This function will simply retunrs the FALSE
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
boolean uim_efslog_init( uim_instance_global_type *uim_ptr )
{
  (void)uim_ptr;
  /* Just returning FALSE without doing anything */
  return FALSE;
} /* uim_efslog_init */


/**
 * This Function would now become dummy and simply returns the control
 *
 * @param attribute Attribute of logging data
 * @param character Data to be logged.
 * @param uim_ptr Pointer to the global data common.
 */
void uim_efslog_apdu_to_buffer(
  uim_instance_global_type *uim_ptr
)
{
  (void)uim_ptr;
  /* do nothing */
  return;

} /* uim_efslog_apdu_to_buffer */


/**
 * This Function is being stub out and just return the conrtol.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_efslog_save_buffer_to_efs( uim_instance_global_type *uim_ptr )
{
  (void)uim_ptr;
  /* Do nothing and just return the control*/
  return;
} /* uim_efslog_save_buffer_to_efs */


/**
 * Cleans up EFSLOG when complete
 *
 * @param uim_ptr
 */
void uim_efslog_clean_up( uim_instance_global_type *uim_ptr )
{
  (void)uim_ptr;
  /* Do nothing and just return the control */
  return;
} /* uim_efslog_clean_up */


/**
 * DEFINITIONS OF INTERNAL FUNTIONS
 *
 */

/**
 * This file checks if a purge is necessary for any EFS Buffers.
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean True if EFS logging feature is enabled.
 */
static boolean uim_efslog_purge_check(
  uim_instance_global_type *uim_ptr
)
{
  (void)uim_ptr;
  /* Do nothing and simply return FALSE */
  return FALSE;
} /* uim_efslog_purge_check */


/**
 * This file handle the processing of purge signal.
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return none.
 */
void uim_efslog_process_purge_sig(
  uim_instance_global_type *uim_ptr
)
{
  (void)uim_ptr;
  return;
} /* uim_efslog_process_purge_sig */