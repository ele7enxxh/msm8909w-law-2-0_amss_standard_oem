#ifndef SECCRYPTARM_H
#define SECCRYPTARM_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S

         C R Y P T O G R A P H I C   S E R V I C E S   L I B R A R Y

                     I N T E R N A L    F U N C T I O N S

DESCRIPTION
  This file contains the definitions of internal functions used in the
  cryptographic services library.

  NOTE: Applications outside of DMSS shall use the API defined
        by secapi.h !


EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  seccrypt_init() has to be called before using any of the other routines.
  This is done at task startup automatically.

 Copyright (c) 2000-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCSPath: L:/src/asw/COMMON/vcs/seccryptarm.h_v   1.2   22 Oct 2003 11:58:44   omichael  $
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/seccryptarm.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/13   nk      AES256 support in secapi interface
07/18/10   nk      Shared Library Framework - Removed FEATURE_SEC
06/10/09   ejt     Removed seccrypt.h
10/21/03   om      Added arm platform cleanup
04/04/03   om      Created module.

===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "dsm.h"
#include "msg.h"
#include "err.h"

#include "secerrno.h"
#include "secapi.h"
#include "seccrypti.h"


/*===========================================================================

                            TYPE DECLARATIONS

===========================================================================*/

/* Common data for instances executing on the ARM */
struct seccryptarm_common_data_s
{
  void*  dummy;
};


/*===========================================================================

                             DATA DEFINITIONS

===========================================================================*/

/* Queue of commands for the crypto layer */
extern const seccrypt_cmd_q_type   seccryptarm_cmd_q;


/*===========================================================================

                    ARM CRYPTO TASK RELATED DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Security Service ARM Crunching Task Context
---------------------------------------------------------------------------*/
#define SECCRYPTARM_TCB_PTR           &seccryptarm_tcb

/*--------------------------------------------------------------------------
  Security Service ARM Crunching Signals
---------------------------------------------------------------------------*/
/* The next 3 sigs are never used explicitly in the security services code.  
 * They are here as place-holders, so that when adding signals, the user does 
 * not re-defined signals already defined system-wide in task.h.  These 
 * signals are referred by their proper name (i.e., TASK_STOP_SIG) in this 
 * Security services code.
 */

#define SECCRYPTARM_START_SIG         0x8000    /* Rsvd for TASK_START_SIG     */

#define SECCRYPTARM_STOP_SIG          0x4000    /* Rsvd for TASK_STOP_SIG      */

#define SECCRYPTARM_OFFLINE_SIG       0x2000    /* Rsvd for TASK_OFFLINE_SIG   */

/* The following sigs are all okay, and can be allocated freely
 */
#define SECCRYPTARM_DOG_RPT_TIMER_SIG 0x0001    /* watchdog signal */

#define SECCRYPTARM_CMD_Q_SIG         0x0002    /* CMD queue signal */

#define SECCRYPTARM_LOG_TIMER_SIG     0x0004    /* periodic logging */

/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION SECCRYPTARM_POWERUP_INIT()

DESCRIPTION
  One-time initialization of ARM platform-specific data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void seccryptarm_powerup_init( void );


/*===========================================================================

FUNCTION SECCRYPTARM_PLATFORM_INIT()

DESCRIPTION
  Re-initialization of ARM platform-specific data after switching
  platforms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void seccryptarm_platform_init
(
  seccrypt_instance_type*      instance_ptr
);

/*===========================================================================

FUNCTION SECCRYPTARM_PLATFORM_RELEASE()

DESCRIPTION
  Cleanup and release of ARM platform-specific data after switching
  platforms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void seccryptarm_platform_release
(
  seccrypt_instance_type*      instance_ptr
);

secerrno_enum_type seccryptarm_schedule_op
(
  seccrypt_cmd_enum_type   cmd,
  seccrypt_instance_type*  instance_ptr
);

void seccryptarm_process_cmd_sig( void );

secerrno_enum_type seccryptarm_process_cmd
(
  seccrypt_cmd_type*  cmd_ptr
);


/*==========================================================================
 FUNCTION SECCRYPTARM_WAIT
  
 DESCRIPTION 
   This function performs the SEC task waiting function. This function
   will not return until a signal from the requested mask is set.  To 
   ensure that the watchdog stays happy, this function internally makes
   sure that the watchdog is petted periodically.
   
   Note that if the task just uses rex_wait instead of sec_wait, then
   the task could wait on a signal for a long time, without ever petting
   the watchdog.  The watchdog expects all tasks to report in periodically
   (indicating good software operation); failure for this task to pet the
   watchdog will cause the phone to reset.  So each task must make its own
   specific wait routine, so that it takes care to pet the watchdog
   periodically.
   
 DEPENDENCIES 
   CAN ONLY BE CALLED FROM THE SEC TASK CONTEXT.  IF CALLED FROM 
   ANOTHER TASK, THEN THAT TASK WILL HANG FOREVER...

 RETURN VALUE 
   A signal mask respresenting the signals which are currently set.  At
   least one signal in the requested mask will be set.  Any other signals
   which are set, which may or may not be spec'd in the signal mask, are
   also returned.
  
 SIDE EFFECTS
   None
===========================================================================*/
extern rex_sigs_type seccryptarm_wait
(
  rex_sigs_type requested_mask              /* mask of signals to wait for */
);


/*===========================================================================

FUNCTION SECCRYPTARM_TASK

DESCRIPTION
  This function is the entry point and main processing loop for the security
  services task.  This function contains the infinite while loop.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void seccryptarm_task
( 
  dword dummy                            /* unused dummy var               */
);

#endif  /* SECCRYPTARM_H */
