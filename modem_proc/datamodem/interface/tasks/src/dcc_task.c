/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              D C C _ T A S K . C

DESCRIPTION
  This is the source file for the Data Common Control Task. Contained herein
  are the functions needed to initialize all the modules that execute in DCC
  task context and the main task processing loop.


EXTERNALIZED FUNCTIONS
  dcc_task()
    DataCommon Control Task entry point and main processing loop.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the DCC services can be used unless MC has created the DCC task
  and dcc_task() has been invoked.

Copyright (c) 2009 - 2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/dcc_task.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/18/15    skc    Fixed compiler warning
11/18/14    skc    Removing FEATURE_UW_FMC
10/21/14    sk     QMI-DMS Service QCSI migration.
11/01/13    rk     Removed feature mobileap.
03/02/13    pkp    Dog heart beat implementation.
09/01/12    ss     Added TFTP ALG init call.
02/01/12    am     Added QCMAP service signal handling.
10/01/11   bvd     Added rcinit featurization 
11/03/11    sm     Added RTSP ALG client init call. 
08/12/11    ss     Added SIP ALG client init call.
08/12/11    cp     Added the support for PPTP ALG.
08/10/11    wc     Data aggregation based on CPU utilization
06/24/11    scb    Port changes for FMC into DCC task
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
09/27/10    ss     DYNAMIC WDOG support for DATA tasks.
08/27/10    sy     Moving QMH init before DS profile init in dcci_init().
08/26/10    pp     Enable IPSec initialization.
07/116/10   asn    Fix compilation issues
07/14/10    asn    Address initialization of DS Profile and UMTS Profile Reg
03/16/10    sy     Changed task_* api to tmc_task* api.
11/05/09    ar     Adjust order of QMI/RmNET init functions.
09/07/09    am     Moved ipsec_hdlr_init to Apps side.
05/22/09    am     Make sure ps_mem_init is called before ps_mem_pool_init.
02/19/09    am     Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#include "task.h"
#include "rex.h"
#include "queue.h"
#include "msg.h"
#include "err.h"
#include "dog.h"
#include "dog_hb_rex.h"
#include "dcc_task.h"
#include "dcc_taski.h"
#include "dcc_task_defs.h"
#include "rcinit_rex.h"

#ifdef FEATURE_DATA_RM_NET
#include "ds_rmnet_sm_ext.h"
#endif /* FEATURE_DATA_RM_NET */

#ifdef FEATURE_DATA_QMH
#include "ds_qmh.h"
#endif /* FEATURE_DATA_QMH */

#ifdef FEATURE_DATA_PS_IWLAN
#include "iwlani_iface_ext.h"
#endif /* FEATURE_DATA_PS_IWLAN */

#ifdef FEATURE_DATA_PS_MIPV6
#include "ps_mip6.h"
#include "ps_mip6_msm.h"
#endif /* FEATURE_DATA_PS_MIPV6 */

#ifdef FEATURE_DATA_PS_UICC
#include "ps_uicc.h"
#endif /* FEATURE_DATA_PS_UICC */

#ifdef FEATURE_DATA_PS_SLIP
#include "ps_slip_iface_hdlr.h"
#endif /* FEATURE_DATA_PS_SLIP */

#ifdef FEATURE_SEC_IPSEC
#include "ipsec_iface_hdlr.h"
#endif /* FEATURE_SEC_IPSEC */

#include "ps_mem.h"

#include "ds_profile.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_profile_remote_client.h"
#include "ds_rmnet_utils.h"
/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

rex_tcb_type  * rex_dcc_tcb;

/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

                           LOCAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================
FUNCTION DCCI_INIT()

DESCRIPTION
  Initialization function for the data common ctrl task. This function
  performs the functions needed for the data task to exit disabled state.

DEPENDENCIES
  DCC task should have been started.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dcci_init
(
  void
)
{
#ifdef FEATURE_DATA_PS_MIPV6
  int32  ret_val;
#endif /* FEATURE_DATA_PS_MIPV6 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_SEC_IPSEC
  /*-------------------------------------------------------------------------
    Initialize the IPSec Mode Handler.
    Note: IPSec is intialized prior to iWLAN/MIP6.
  -------------------------------------------------------------------------*/
  ipsec_iface_hdlr_iface_init();
#endif /* FEATURE_SEC_IPSEC */

#ifndef IMAGE_MODEM_PROC
#ifdef FEATURE_DATA_PS_IWLAN
  /*-------------------------------------------------------------------------
    Initialize the IWLAN mode controller.
  -------------------------------------------------------------------------*/
  iwlan_init();
#endif /* FEATURE_DATA_PS_IWLAN */

#endif /* !(IMAGE_MODEM_PROC) */

#ifndef TEST_FRAMEWORK
#ifdef FEATURE_DATA_QMH
  /*-------------------------------------------------------------------------
    Initialize QMI Mode Handler interface
   -------------------------------------------------------------------------*/
  dsqmh_handler_init();
#endif /* FEATURE_DATA_QMH */
#endif /* TEST_FRAMEWORK */

#if ! defined(IMAGE_APPS_PROC)

#ifdef FEATURE_DATA_PS_MIPV6
  ret_val = ps_mip6_init();
  if (ret_val < 0)
  {
    LOG_MSG_ERROR_0("dcci_init(): "
                    "Failed to init MIP6 module.");
  }
#endif /* FEATURE_DATA_PS_MIPV6 */

#ifdef FEATURE_DATA_PS_UICC
  ps_uicc_powerup_init();
#endif /* FEATURE_DATA_PS_UICC */

#ifdef FEATURE_DATA_PS_SLIP
  /*-------------------------------------------------------------------------
    Initialize SLIP
  -------------------------------------------------------------------------*/
  slip_sm_init();
#endif /* FEATURE_SLIP */

#endif /* !(FEATURE_DATA_PS_CMI_REARCH && IMAGE_APPS_PROC) */

  /*--------------------------------------------------------------------------- 
   Call the subroutine to register wds service up indiation
    on remote modem
  ---------------------------------------------------------------------------*/
  ds_profile_remote_client_init();

  /*--------------------------------------------------------------------------- 
    Initialize rmnet utils module
  ---------------------------------------------------------------------------*/
  rmnet_utils_init();  

} /* dcci_init() */


/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/


/*===========================================================================
FUNCTION DCC_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the Data
  Common Control task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dcc_task
(
  uint32 ignored    /* Parameter received from Main Control task - ignored */
)
{
  rex_sigs_type     loop_sigs;      /* Signals to be processed within a task
                                       loop */
  rex_sigs_type     set_sigs = 0;   /* Currently set signals               */
  uint8             loop_cnt;       /* # of times task loop is iterated
                                       on a signal mask                    */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  rex_dcc_tcb = rex_self();

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  rcinit_handshake_startup();

#ifndef TEST_FRAMEWORK
  (void)rcevt_wait_name("DS:READY");
#endif

  dcc_dog_rpt_id = dog_hb_register_rex(
                  (rex_sigs_type)1 << (rex_sigs_type)DCC_DOG_HB_REPORT_SIGNAL);

  dcci_init();

  /*-------------------------------------------------------------------------
    Main task loop. Never exit!
  -------------------------------------------------------------------------*/
  for( ;;)
  {
    /*-----------------------------------------------------------------------
      Wait for work to do. No return from wait until one of the enabled
      signals is set. Even if we come out of wait, process only those
      signals which are currently enabled.
    -----------------------------------------------------------------------*/

    set_sigs |= rex_get_sigs( rex_dcc_tcb );

    set_sigs &= dcci_enabled_sig_mask;

    /*-----------------------------------------------------------------------
      Enter into the Wait only if no requested signal events have occurred
    -----------------------------------------------------------------------*/
    if( set_sigs == 0 )
    {
      set_sigs = rex_wait( dcci_enabled_sig_mask );
      set_sigs &= dcci_enabled_sig_mask;
    }

    (void)rex_clr_sigs( rex_dcc_tcb, set_sigs );

    loop_sigs = set_sigs & ~( (rex_sigs_type) 1
                             << (rex_sigs_type) DCC_SIG_MASK_CHANGE_SIGNAL );
    set_sigs = 0;

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than DCC_MAX_SIGNALS times, some signal
      is not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    while( loop_sigs && loop_cnt++ < (rex_sigs_type) DCC_MAX_SIGNALS )
    {
      /*---------------------------------------------------------------------
        If dog heart beat is received, ack the beat. Play ping<->pong.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_DOG_HB_REPORT_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type)DCC_DOG_HB_REPORT_SIGNAL );
        dog_hb_report( dcc_dog_rpt_id);
      }
      /*---------------------------------------------------------------------
        PS Task command queue signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) DCC_CMD_Q_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_CMD_Q_SIGNAL );

        if ( dcci_process_cmd() == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_CMD_Q_SIGNAL );
        }
      }

#ifdef FEATURE_8960_SGLTE_FUSION
      /*---------------------------------------------------------------------
        Process DCC_DS_PROFILE_REMOTE_NOTIFY_CLIENT_SIGNAL signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) DCC_DS_PROFILE_REMOTE_NOTIFY_CLIENT_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_DS_PROFILE_REMOTE_NOTIFY_CLIENT_SIGNAL);
        if (FALSE == dcci_sig_handler[DCC_DS_PROFILE_REMOTE_NOTIFY_CLIENT_SIGNAL].sig_handler
                       (
                         DCC_DS_PROFILE_REMOTE_NOTIFY_CLIENT_SIGNAL,
                         dcci_sig_handler[DCC_DS_PROFILE_REMOTE_NOTIFY_CLIENT_SIGNAL].user_data_ptr
                        ))
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_DS_PROFILE_REMOTE_NOTIFY_CLIENT_SIGNAL);
        }
      }
#endif /* FEATURE_8960_SGLTE_FUSION */
      /*---------------------------------------------------------------------
        Process other signals with registered handlers
      ---------------------------------------------------------------------*/
#if ! defined(IMAGE_APPS_PROC)


#endif /* !(FEATURE_DATA_PS_CMI_REARCH && IMAGE_APPS_PROC) */

#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */

#if defined(FEATURE_RMNET_DATA_AGG_TIMER) && defined(FEATURE_CPU_BASED_FLOW_CONTROL)
      /*---------------------------------------------------------------------
        Process FC signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) DCC_RMNET_FC_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_RMNET_FC_SIGNAL);
        if (FALSE == dcci_sig_handler[DCC_RMNET_FC_SIGNAL].sig_handler
                       (
                         DCC_RMNET_FC_SIGNAL,
                         dcci_sig_handler[DCC_RMNET_FC_SIGNAL].user_data_ptr
                        ))
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_RMNET_FC_SIGNAL);
        }
      }
#endif /* FEATURE_RMNET_DATA_AGG_TIMER && FEATURE_CPU_BASED_FLOW_CONTROL */

      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) DCC_DSD_RDR_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_DSD_RDR_SIGNAL);
        if (FALSE == dcci_sig_handler[DCC_DSD_RDR_SIGNAL].sig_handler
                       (
                         DCC_DSD_RDR_SIGNAL,
                         dcci_sig_handler[DCC_DSD_RDR_SIGNAL].user_data_ptr
                        ))
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_DSD_RDR_SIGNAL);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) DCC_DMS_RDR_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_DMS_RDR_SIGNAL);
        if (FALSE == dcci_sig_handler[DCC_DMS_RDR_SIGNAL].sig_handler
                       (
                         DCC_DMS_RDR_SIGNAL,
                         dcci_sig_handler[DCC_DMS_RDR_SIGNAL].user_data_ptr
                        ))
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_DMS_RDR_SIGNAL);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) DCC_DFS_RDR_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_DFS_RDR_SIGNAL);
        if (FALSE == dcci_sig_handler[DCC_DFS_RDR_SIGNAL].sig_handler
                       (
                         DCC_DFS_RDR_SIGNAL,
                         dcci_sig_handler[DCC_DFS_RDR_SIGNAL].user_data_ptr
                        ))
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_DFS_RDR_SIGNAL);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) DCC_ATP_RDR_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_ATP_RDR_SIGNAL);
        if (FALSE == dcci_sig_handler[DCC_ATP_RDR_SIGNAL].sig_handler
                       (
                         DCC_ATP_RDR_SIGNAL,
                         dcci_sig_handler[DCC_ATP_RDR_SIGNAL].user_data_ptr
                        ))
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_ATP_RDR_SIGNAL);
        }
      }
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      /*---------------------------------------------------------------------
        Check if offline command signal was set.  If set then clear signal,
        and process task offline procedures.
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) DCC_TASK_OFFLINE_SIGNAL ) )
      {
        LOG_MSG_INFO2_0("dcc_task(): "
                        "DCC_TASK_OFFLINE_SIGNAL received");

        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) DCC_TASK_OFFLINE_SIGNAL );
      }
    }

    ASSERT(0 == loop_sigs);
  } /* forever */

} /* dcc_task() */
