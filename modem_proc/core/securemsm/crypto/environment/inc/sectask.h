#ifndef SECTASK_H
#define SECTASK_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   T A S K
                             
GENERAL DESCRIPTION
  This is the task level file for the security services. It defines the
  timers, signals, queues and other necessary handlings for the security
  services task.

EXTERNALIZED FUNCTIONS
  sec_wait() - SEC task wait until a requested signal is set.
  sec_get_cmd_buf_else_err_fatal() - Get a command buffer from SEC task.
  sec_cmd()  - send a command to SEC task.
  sec_task() - entry point for the SEC task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  MC must initialize the task and set the start signal for this task, 
  before any other functions can be called.

  Copyright (c) 2001-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/environment/inc/sectask.h#1 $
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/07/10   ejt     removed DSP signal info 
07/18/10   nk      Shared Library Framework - DeWrapped FEATURE_HW_CRYPTO 
                   and removed QDSP6 dependency
06/18/09   bm      added support for Crypto HW engine
06/10/09   ejt     removed FEATURE_SEC_IPC_SVC
11/12/07   df      add support for HW Crypto Engine
09/03/07   df      changes for SEC_DSP_EVENT_SIG
10/02/06   gr      Added a new type
03/22/06   df      changes to support FEATURE_SECRND
10/03/05   df      replace xx_CSPRNG_TIMER_SIG with xx_CSPRNG_SIG
                     Fixed CSPRNG and IPC signal defs: collision with 
                     STOP and OFFLINE sigs
09/16/05   df      Added support for csprng timer
02/25/05   sb      Added support for IPC based SEC services
09/21/04   df      added dsp_req and gp timer signals to support dsp-arm
                    failover
01/06/04   jay     Added a new signal for the new Pri Q
10/21/03   jay     Added signals for timers
02/20/03   rwh     Use the PROC_DONE_SIG instead of DNLD_SIG.
02/17/03   om      Split the SEC task into SEC and SSL task
06/04/02   om      Added SECSSL_ABORT_SIG definition.
03/22/02   kvd     Added enum, SEC_SSL_CLOSE_NETWORK  sec_cmd_name_type.
02/27/02   rwh     Featurized for FEATURE_SEC_SSL (QCase 25659)
02/07/02   om      Added support for user action for security related events.
01/30/02   om      Renamed SSL specific signals to SECSSL_*.
06/01/01   om      Added logging support.
05/31/01   om      Added DNS support.
05/24/01   yll     Added the ret_code and execute mode as part of the
                   sec_cmd_hdr_type.
05/07/01   yll     Added support for RSA encryption and signature 
                   verification.
04/06/01   yll     Added the SEC_DSP_DNLD_SIG and SEC_DSP_PROC_DONE_SIG.
02/12/01   yll     Removed the test commands.
01/26/01   yll     Created Initial Version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "rex.h" 
#include "secerrno.h" 

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================

                 INTLOCK definition 

===========================================================================*/
#include "ixlock.h"
extern strLockInfo  g_seclock; /* Rex based critical section structure */
#define SEC_LOCK_INIT()         IxLockInit(&g_seclock, LOCK_TASK_ONLY)
#define SEC_LOCK()	        IxLock(&g_seclock);
#define SEC_UNLOCK()	        IxUnlock(&g_seclock);	
#define SEC_LOCK_SAV()	        IxLock(&g_seclock);
#define SEC_UNLOCK_SAV()	IxUnlock(&g_seclock);	
#define SEC_TASK_LOCK()	        IxLock(&g_seclock);
#define SEC_TASK_UNLOCK()	IxUnlock(&g_seclock);	
#define SEC_INT_TASK_LOCK()	IxLock(&g_seclock);
#define SEC_INT_TASK_UNLOCK()	IxUnlock(&g_seclock);

/*--------------------------------------------------------------------------
  Security Service Signals
---------------------------------------------------------------------------*/
/* The next 3 sigs are never used explicitly in the security services code.  
 * They are here as place-holders, so that when adding signals, the user does 
 * not re-defined signals already defined system-wide in task.h.  These 
 * signals are referred by their proper name (i.e., TASK_STOP_SIG) in this 
 * Security services code.
 */

#define SEC_START_SIG          0x8000   /* Reserved for TASK_START_SIG     */

#define SEC_STOP_SIG           0x4000   /* Reserved for TASK_STOP_SIG      */

#define SEC_OFFLINE_SIG        0x2000   /* Reserved for TASK_OFFLINE_SIG   */

/* The following sigs are all okay, and can be allocated freely
 */
#define SEC_DOG_RPT_TIMER_SIG   0x0001   /* signal set when watchdog expires*/

#define SEC_NV_CMD_SIG          0x0004   /* signal when NV interaction done */

#define SECCRYPT_CMD_Q_SIG      0x0008   /*signal for cmds in SECCRYPT_CMD_Q*/

#define SECCRYPT_OP_WAIT_SIG    0x0010   /* inline wait signal              */

#define SEC_LOG_TIMER_SIG       0x0800   /* signal to periodically log stuff*/

#define SECCRYPT_PRI_CMD_Q_SIG  0x1000   /* signal for cmds in SECCRYPT_PRI_CMD_Q  */

#ifndef FEATURE_SECRND
#define SEC_CSPRNG_SIG         0x020000   /* signal for CSPRNG */
#endif

#define SECCRYPTHW_CMD_Q_SIG   0x080000   /*sig for cmds in SECCRYPTHW_CMD_Q*/
#define SEC_HW_EXEC_TIMER_SIG  0x100000   /* crypto-HW CE time expiry */
#define SEC_HW_USER_TIMER_SIG  0x200000  /* HW CE user timer for disable */
#define SEC_HW_OP_COMPLETE_SIG 0x400000  /* HW CE op complete sig */

#define SEC_CEV2_OP_COMPLETE   0x800000

#define SEC_NULL_SIG            0x0000   /* NULL Signal */


/*---------------------------------------------------------------------------
  External function declarations.
---------------------------------------------------------------------------*/


/*==========================================================================
 FUNCTION SEC_WAIT
  
 DESCRIPTION 
   this function is deprecated  
 DEPENDENCIES 
   CAN ONLY BE CALLED FROM THE SEC TASK CONTEXT.  IF CALLED FROM 
   ANOTHER TASK, THEN THAT TASK WILL HANG FOREVER...

 RETURN VALUE 
  
 SIDE EFFECTS
   None
===========================================================================*/
extern rex_sigs_type sec_wait
(
rex_sigs_type requested_mask              /* mask of signals to wait for */
);


/*===========================================================================

FUNCTION SEC_TASK

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
extern void sec_task
(
dword dummy                            /* unused dummy var               */
);


#endif /* SECTASK_H */
