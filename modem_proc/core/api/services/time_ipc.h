/**
@file time_ipc.h
@brief
This module implements the IPC time task.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the time_services group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2009-2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                  T I M E R - I P C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements IPC time task 

  Slow Clock Frequency          Granularity     Timer Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs

EXTERNALIZED FUNCTIONS
  time_ipc_task
    Initializes time Time_IPC task

INITIALIZATION AND SEQUENCING REQUIREMENTS

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/time_ipc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/29/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
10/27/10   EBR     Doxygenated File.
03/11/10   din     Moved efs write to Time IPC task.
02/19/10   din     Moved genoff RPC calls from modem proc to TIME_IPC task.
02/08/10   sri     Made changes for DOG_DYNAMIC_API
05/22/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "rex.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
/** @addtogroup time_services
@{ */

#ifdef DOG_DYNAMIC_API
/*  Dog variables - now we MAY be dynamic       */
/** @name Dog variables 
@{ */
extern uint32  dog_time_ipc_rpt;                 /** Dog report ID. */
extern uint32  dog_time_ipc_rpt_time;            /** Report period. */

#define  DOG_TIME_IPC_RPT       dog_time_ipc_rpt      /** Dog report ID. */ /* rpt_def */ 
#define  DOG_TIME_IPC_RPT_TIME  dog_time_ipc_rpt_time /** Report period. */  /* time_def */ 
/** @} */ /* end_name Dog variables */
#endif /* DOG_DYNAMIC_API */

/** Signal for a resynchronization with TOD. */
#define TIME_TOD_RESYNC_SIG      0x0008

/** Signal for a generic offset update. */
#define TIME_IPC_GENOFF_UPDATE   0x0010

/** Signal for an embedded file system write event. */
#define TIME_IPC_EFS_WRITE       0x0020

/** Signal for the modem-to-modem registration. */
#define TIME_IPC_MM_REGISTRATION 0x0040

/** Signal to send over the tod modem update. */
#define TIME_IPC_SEND_REMOTE_MODEM_UDPATE 0x0080

/*Signal to send over the user modem update*/
#define TIME_IPC_SEND_REMOTE_USER_UDPATE 0x00100

/*Signal to send over the user_tz update*/
#define TIME_IPC_SEND_REMOTE_USER_TZ_DL_UDPATE 0x00200

#define TIME_IPC_QMI_REMOTE_COMM   0x400

#define TIME_IPC_QMI_REMOTE_COMM_TIMEOUT   0x800

#define TIME_IPC_QMI_REMOTE_COMM2 0x1000

/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/


/**
Initalizes the time_ipc task.

@param[in] parm Unused parameter; required to match the rex_task_func_type 
                prototype.

@return
None.

@dependencies
None.
*/
void  time_ipc_task
(
  dword                           parm    /*lint -esym(715,ignored) */
);

/** @} */ /* end_addtogroup time_services */
