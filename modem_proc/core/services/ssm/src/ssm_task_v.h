/**
@file ssm_task_v.h
@brief
This module implements the IPC time task.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The SSM_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      SSM_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the SSM group
      description in the SSM_mainpage.dox file.
=============================================================================*/

/*=============================================================================
  Copyright (c) 2011 - 2014 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                 S E C U R E   S E R V I C E S  M O D U L E

GENERAL DESCRIPTION
  Implements SSM task

EXTERNALIZED FUNCTIONS
  ssm_task
    Initializes ssm task

INITIALIZATION AND SEQUENCING REQUIREMENTS

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_task_v.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/20/14   din     Fixed concurreny issue seen due to retry of sfs read operations.
04/29/13   din     Changes pertaining to ADSP were done.
11/05/12   din     Addition of Key handler and override Command handler.
08/24/12   din     Removed warnings pertaining to insertion of task.h
07/20/12   din     Removal key exchange code from SSM.
09/03/11   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "ssm_v.h"
#ifdef FEATURE_SSM_REX_API_PRESENT
#include "rcinit_rex.h"
#endif /* FEATURE_SSM_REX_API_PRESENT */


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
/** @addtogroup ssm
@{ */

/** Signals USed by SSM TASK */
#define SSM_SMDL_READ_SIG                   0x0001
#define SSM_SMDL_WRITE_SIG                  0x0002
#define SSM_RPT_TIMER_SIG                   0x0004
#define SSM_PARSER_RETRY_SFS_READ_SIG       0x0008
#define SSM_PARSE_PERM_FILE_SIG             0x0010
#define SSM_KEY_HANDLER_PROCESS_SIG         0x0020
#define SSM_OCH_PROCESS_SIG                 0x0040
#define SSM_ACH_PROCESS_SIG                 0x0080

/* Other definitions */
/* Kick the watchdog */
#define  SSM_DOG_RPT() \
  dog_report(DOG_SSM_RPT); \
  (void) rex_set_timer ( &ssm_rpt_timer, DOG_SSM_RPT_TIME )



/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/


/**
Initalizes the ssm task.

@param[in] parm Unused parameter; required to match the rex_task_func_type
                prototype.

@return
None.

@dependencies
None.
*/
void  ssm_task
(
  void
);

/** @} */ /* end_addtogroup ssm */
