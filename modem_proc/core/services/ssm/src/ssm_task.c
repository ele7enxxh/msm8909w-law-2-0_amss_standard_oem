/*=============================================================================

             S E C U R E  S E R V I C E S   M O D U L E

GENERAL DESCRIPTION
  Implements SSM task

EXTERNALIZED FUNCTIONS
  ssm_task
    Initializes ssm task

  INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2011 - 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_task.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/20/14   din     Fixed concurreny issue seen due to retry of sfs read operations.
04/03/14   din     Added new apis to get mode and to write to permission file. 
                   Also added diag command to get ssm mode.
04/29/13   din     Changes pertaining to ADSP were done.
11/05/12   din     Addition of Key handler and override Command handler.
08/24/12   din     Removed warnings pertaining to insertion of task.h
08/09/12   din     Added Secure Channel Initialization .
07/20/12   din     Removed key exchange code from SSM.
07/19/12   din     Changes done to switch from efs to sfs based permission file.
09/01/12   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#ifdef FEATURE_SSM_REX_API_PRESENT
#include "rex.h"
#else /*  FEATURE_SSM_REX_API_PRESENT */
#include "qurt.h"
#endif /* FEATURE_SSM_REX_API_PRESENT */
#include "ssm_task_v.h"
#include "secapi.h"
#include "ssm_v.h"
#include "ssm_smdl_v.h"
#include "ssm_diag_v.h"
#include "ssm_test_v.h"
#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
#include "fs_diag_access.h"
#include "sfs_api.h"
#include "ssm_diag_och_v.h"
#include "fs_errno.h"
#include "ssm_key_handler_v.h"
#include "ssm_diag_key_handler_v.h"
#include "ssm_diag_ach_v.h"
#include "ssm_och_v.h"
#include "ssm_ach_v.h"
#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
#include "rcinit.h"



/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

#ifdef FEATURE_SSM_REX_API_PRESENT
/* Look up pointer for ssm tcb */
static rex_tcb_type* ssm_tcb_ptr=NULL;
#else /* FEATURE_SSM_REX_API_PRESENT */
qurt_anysignal_t  ssm_sig_qurt;
#endif /* FEATURE_SSM_REX_API_PRESENT */



/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/


/**
This procedure is called  by timer IPC when it is ready to start its task.
It will wait on TASK_START_SIG after this call.


@param[in]
None

@return
None

@dependencies
None.
*/

static void ssm_ready_start_task (void)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rcinit_handshake_startup();  /* Handshake allows startup to progress */

} /* ssm_ready_start_task */



#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
static const char *ssm_access_dirs[] = {
"ssm"
};


/*==================================================================
FUNCTION SSM_CHECK_REMOTE_ACCESS

DESCRIPTION
   This functions returns access permissions for directories specified
   by input parameter "path".

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==================================================================*/
static boolean ssm_check_remote_access
(
  char *path,
  uint32 op_type
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  (void) path;
  (void) op_type;
  return FALSE;

} /* ssm_check_remote_access  */

#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */



/*=============================================================================

FUNCTION SSM_TASK

DESCRIPTION
  Initializes the SSM task and listens to messages

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void  ssm_task
(
  void
)
{

  #ifdef FEATURE_SSM_REX_API_PRESENT
  /* Signals returned from rex wait procedure */
  rex_sigs_type   sigs;
  #else /* FEATURE_SSM_REX_API_PRESENT */
  unsigned int    sigs;
  #endif /* FEATURE_SSM_REX_API_PRESENT */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_SSM_REX_API_PRESENT
    qurt_anysignal_init( &ssm_sig_qurt );
  #endif /* FEATURE_SSM_REX_API_PRESENT */

  /* Ready to start task */
  ssm_ready_start_task();

  #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
  /* Intialize SSM Key Handler */
  ssm_key_handler_init();
  #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

  #ifndef FEATURE_SSM_SEC_CHANNEL_NOT_AVAILABLE
  /* Setup Secure Channel */
  (void)secapi_secure_channel_init();
  #endif /* FEATURE_SSM_SEC_CHANNEL_NOT_AVAILABLE*/

  /* Initialize SSM smdl */
  ssm_smdl_init();

  #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
  /* Stop Access permission to "/ssm" */
  diag_fs_register_access_check (ssm_access_dirs, 1, ssm_check_remote_access);
  #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

  #ifdef FEATURE_SSM_REX_API_PRESENT
  ssm_tcb_ptr = rcinit_lookup_rextask("ssm");
  if ( ssm_tcb_ptr == NULL )
  {
    SSM_ERR_FATAL(" SSM task tcb not found", 0 ,0, 0 );
  }
  #endif /* FEATURE_SSM_REX_API_PRESENT */
 
  #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
  /* Initialize SSM ACH */
  ssm_ach_init();
  ssm_diag_ach_init();
  
  /* Initialize SSM Diag key handler */
  ssm_diag_key_handler_init();
  
  /* Initialize SSM OCH */
  ssm_och_init();
  ssm_diag_och_init();
  
  #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

  ssm_diag_init();

  #ifdef FEATURE_SSM_TEST_DEBUG
  /* Initialize SSM test */
  #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
  ssm_test_init();
  ssm_diag_och_test_init();
  #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
  #endif /* FEATURE_SSM_TEST_DEBUG */
 
  for(;;)
  {
    #ifdef FEATURE_SSM_REX_API_PRESENT
	  
    /* Wait for some task signal */
    sigs = rex_wait( SSM_RPT_TIMER_SIG /* Watchdog timer elapsed */
                        | SSM_SMDL_READ_SIG
			| SSM_SMDL_WRITE_SIG
			| SSM_PARSER_RETRY_SFS_READ_SIG
			| SSM_PARSE_PERM_FILE_SIG
                        | SSM_KEY_HANDLER_PROCESS_SIG
			| SSM_OCH_PROCESS_SIG
			| SSM_ACH_PROCESS_SIG);

    #else /* FEATURE_SSM_REX_API_PRESENT */
    
    /* Wait for some task signal */
    sigs = qurt_anysignal_wait( &ssm_sig_qurt,
		          SSM_RPT_TIMER_SIG /* Watchdog timer elapsed */
                        | SSM_SMDL_READ_SIG
			| SSM_SMDL_WRITE_SIG
			| SSM_PARSE_PERM_FILE_SIG
                   );

    #endif /* FEATURE_SSM_REX_API_PRESENT */

    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                        SSM_SMDL_READ_SIG SIGNAL                           */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & SSM_SMDL_READ_SIG )
    {
      #ifdef FEATURE_SSM_REX_API_PRESENT
      (void)rex_clr_sigs ( ssm_tcb_ptr, SSM_SMDL_READ_SIG );
      #else /* FEATURE_SSM_REX_API_PRESENT */
        qurt_anysignal_clear( &ssm_sig_qurt, SSM_SMDL_READ_SIG );
      #endif /* FEATURE_SSM_REX_API_PRESENT */

      /* Process the ssm mode change */
      ssm_smdl_handle_rx();
    }

    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                        SSM_SMDL_WRITE_SIG SIGNAL                           */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & SSM_SMDL_WRITE_SIG )
    {
      #ifdef FEATURE_SSM_REX_API_PRESENT
      (void)rex_clr_sigs ( ssm_tcb_ptr, SSM_SMDL_WRITE_SIG );
      #else /* FEATURE_SSM_REX_API_PRESENT */
        qurt_anysignal_clear( &ssm_sig_qurt, SSM_SMDL_WRITE_SIG );
      #endif /* FEATURE_SSM_REX_API_PRESENT */
      /* Process the ssm mode change */
      //ssm_smdl_handle_tx();
    }


    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                     SSM_PARSER_RETRY_SFS_READ_SIG SIGNAL                */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & SSM_PARSER_RETRY_SFS_READ_SIG )
    {
      #ifdef FEATURE_SSM_REX_API_PRESENT
      (void)rex_clr_sigs ( ssm_tcb_ptr, SSM_PARSER_RETRY_SFS_READ_SIG );
      #else /* FEATURE_SSM_REX_API_PRESENT */
        qurt_anysignal_clear( &ssm_sig_qurt, SSM_PARSER_RETRY_SFS_READ_SIG );
      #endif /* FEATURE_SSM_REX_API_PRESENT */
      /* Process the retry read */
      ssm_parser_retry_sfs_read();

    }
    
    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                     SSM_PARSE_PERM_FILE_SIG SIGNAL                      */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & SSM_PARSE_PERM_FILE_SIG )
    {
      #ifdef FEATURE_SSM_REX_API_PRESENT
      (void)rex_clr_sigs ( ssm_tcb_ptr, SSM_PARSE_PERM_FILE_SIG );
      #else /* FEATURE_SSM_REX_API_PRESENT */
        qurt_anysignal_clear( &ssm_sig_qurt, SSM_PARSE_PERM_FILE_SIG );
      #endif /* FEATURE_SSM_REX_API_PRESENT */
      /* Process the ssm mode change */
      ssm_parse_perm_file();

    }

    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                 SSM_KEY_HANDLER_PROCESS_SIG SIGNAL                      */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & SSM_KEY_HANDLER_PROCESS_SIG )
    {
      #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
      (void)rex_clr_sigs ( ssm_tcb_ptr, SSM_KEY_HANDLER_PROCESS_SIG );
      /* Process the key handler request */
      ssm_key_handler_process_request();
      #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
      
    }

    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                      SSM_OCH_PROCESS_SIG SIGNAL                         */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & SSM_OCH_PROCESS_SIG )
    {
      #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
      (void)rex_clr_sigs ( ssm_tcb_ptr, SSM_OCH_PROCESS_SIG );
      /* Process the OCH request */
      ssm_och_process_request();
      #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
      
    }

    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                      SSM_ACH_PROCESS_SIG SIGNAL                         */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & SSM_ACH_PROCESS_SIG )
    {
      #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
      (void)rex_clr_sigs ( ssm_tcb_ptr, SSM_ACH_PROCESS_SIG );
      /* Process the OCH request */
      ssm_ach_process_request();
      #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
    }

  } /* for (;;) */

} /* ssm_task */

