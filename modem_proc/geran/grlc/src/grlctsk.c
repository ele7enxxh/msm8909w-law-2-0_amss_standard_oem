/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC LAYER 2 TASK CONTROL FUNCTIONS
***
***
*** DESCRIPTION
***
***  Provides a REX framework for the scheduling of RLC Uplink Task and
***  RLC Downlink Task processing.
***
***
*** EXTERNALIZED FUNCTIONS
***
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlctsk.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/15/01    hv     Defined RLC UL & DL task control functions
*** 08/03/04    hv     Added PL1_EPDAN_REQ signal and handler for EGPRS
*** 03/08/12    hv     Corrected priority of uplink message handler
*** 09/18/12    hv     Added dual data space functionality
*** 03/05/13    hv     CR462174
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "comdef.h"
#include "geran_tasks.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlctsk.h"
#include "grlcutil.h"
#include "queue.h"
#include "rex.h"
#include "task.h"
#include "rcinit.h"

#include "grlcul.h"
#include "grlcdl.h"

#include "dog_hb_rex.h"

#include "grlclog.h"


/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/

rex_sigs_type      grlc_ul_task_sigs[NUM_GERAN_DATA_SPACES];
rex_sigs_type      grlc_dl_task_sigs[NUM_GERAN_DATA_SPACES];

/*------------------------------
** Declare the 5 uplink queues
**------------------------------
**
*/
q_type  grlc_ul_llc_pdu_q[NUM_GERAN_DATA_SPACES];
q_type  grlc_ul_llc_tmp_pdu_q[NUM_GERAN_DATA_SPACES];
q_type  grlc_ul_llc_msg_q[NUM_GERAN_DATA_SPACES];
q_type  grlc_ul_mac_q[NUM_GERAN_DATA_SPACES];
q_type  grlc_ul_tmr_q[NUM_GERAN_DATA_SPACES];


/*--------------------------------
** Declare the 3 downlink queues
**--------------------------------
*/
q_type  grlc_dl_mac_q[NUM_GERAN_DATA_SPACES];
q_type  grlc_dl_tmr_q[NUM_GERAN_DATA_SPACES];
q_type  grlc_dl_pl1_q[NUM_GERAN_DATA_SPACES];


/*--------------------------------------------------------------------------
** Temporary declaration of system information.
** This structure will be created and maintained by modules other than RLC.
** To be removed at integration time
**--------------------------------------------------------------------------
*/
gprs_sysinfo_t  sysinfo[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/***************************
** BEGIN Single SIM section
****************************
*/

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GRLC_UL_TASK_X()
===
===  DESCRIPTION
===
===    REX task based scheduling framework for GPRS RLC Generic Uplink Task
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/
void grlc_ul_task_x
(
  gas_id_t gas_id,
  geran_task_id_t task_id
)
{
  /* Watchdog HB for uplink task */
  dog_report_type   grlc_ul_dog_rpt;
  boolean task_stopped = FALSE;

  geran_tcb_set(task_id, rex_self());

  rcinit_handshake_startup(); /* required rcinit handshake */
  /* 2nd phase of init */
  grlc_ul_init(gas_id);
  rlc_log_init(gas_id);

  /* Register with the Watchdog Heartbeat - once ONLY */
  grlc_ul_dog_rpt = dog_hb_register_rex((rex_sigs_type)GRLC_UL_DOG_RPT_TMR_SIG);

  RLC_UL_CLR_PL1_SIG(gas_id);
  RLC_UL_CLR_MAC_SIG(gas_id);


  RLC_UL_CLR_LLC_PDU_SIG(gas_id);
  RLC_UL_CLR_LLC_TMP_PDU_SIG(gas_id);
  RLC_UL_CLR_LLC_MSG_SIG(gas_id);
  RLC_UL_CLR_TMR_SIG(gas_id);


  /* Begin event based processing via REX scheduling. */

  while (!task_stopped)
  {
    /* Recind processor context until a relevent signal(s) becomes active. */

    grlc_ul_task_sigs[gas_id]
      = rex_wait
                        (
                          (
                            TASK_START_SIG          |
                            TASK_STOP_SIG           |
                            TASK_OFFLINE_SIG        |
                            GRLC_UL_DOG_RPT_TMR_SIG |
                            GRLC_UL_MASTER_SIG
                          )
                        );

    while ( grlc_ul_task_sigs[gas_id] )
    {
      /* Process any active signals with the following priority. */

      if ( grlc_ul_task_sigs[gas_id] & GRLC_UL_PL1_SIG )
      {
        /* Handle L1 signal */
        grlc_ul_handle_pl1_sig(gas_id);
      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_ul_task_sigs[gas_id] & GRLC_UL_DOG_RPT_TMR_SIG )
      {
        /* Report to the Dog HB */
        dog_hb_report( grlc_ul_dog_rpt );

        (void)rex_clr_sigs( geran_tcb_read(task_id), GRLC_UL_DOG_RPT_TMR_SIG );
        grlc_ul_task_sigs[gas_id] = rex_get_sigs( geran_tcb_read(task_id) );

      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_ul_task_sigs[gas_id] & GRLC_UL_MAC_SIG )
      {
        /* Handle MAC signal */
        grlc_ul_handle_mac_sig(gas_id);
      }


      /* ------------------------------------------------------------------ */

      else if ( grlc_ul_task_sigs[gas_id] & GRLC_UL_LLC_MSG_SIG )
      {
        /* Handle LLC MSG signal */
        grlc_ul_handle_llc_msg_sig(gas_id);
      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_ul_task_sigs[gas_id] & GRLC_UL_LLC_TMP_PDU_SIG )
      {
        /* Handle LLC PDU signal */
        grlc_ul_handle_llc_tmp_pdu_sig(gas_id);
      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_ul_task_sigs[gas_id] & GRLC_UL_LLC_PDU_SIG )
      {
        /* Handle LLC PDU signal */
        grlc_ul_handle_llc_pdu_sig(gas_id);
      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_ul_task_sigs[gas_id] & GRLC_UL_TMR_SIG )
      {
        /* Handle timer signal */
        grlc_ul_handle_tmr_sig(gas_id);
      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_ul_task_sigs[gas_id] & GRLC_UL_ITN_SIG )
      {
        /* Handle internal uplink signal */
        grlc_ul_handle_internal_sig(gas_id);
      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_ul_task_sigs[gas_id] & GRLC_UL_LOG_SIG )
      {
        grlc_ul_handle_log_sig(gas_id);
      }

      /* ------------------------------------------------------------------ */

    else if ( grlc_ul_task_sigs[gas_id] & TASK_START_SIG )
      {
        (void)rex_clr_sigs( geran_tcb_read(task_id), TASK_START_SIG );
        grlc_ul_task_sigs[gas_id] = rex_get_sigs( geran_tcb_read(task_id) );
      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_ul_task_sigs[gas_id] & TASK_STOP_SIG )
      {
        /* Not used at present so just clear down the signal. */

        (void)rex_clr_sigs( geran_tcb_read(task_id), TASK_STOP_SIG );
        grlc_ul_task_sigs[gas_id] = rex_get_sigs( geran_tcb_read(task_id) );
        task_stopped = TRUE;
      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_ul_task_sigs[gas_id] & TASK_OFFLINE_SIG )
      {
        /* Not used at present so just clear down the signal. */

        (void)rex_clr_sigs( geran_tcb_read(task_id), TASK_OFFLINE_SIG );
        grlc_ul_task_sigs[gas_id] = rex_get_sigs( geran_tcb_read(task_id) );
      }

      /* ------------------------------------------------------------------ */
    }
  }
  /* Deregister if leaving the task so that we don't ERR_FATAL */
  dog_hb_deregister(grlc_ul_dog_rpt);

  /*UNREACHED*/
} /* end of grlc_ul_task() */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_TASK
===
===  DESCRIPTION
===
===    RLC Uplink task One
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/
void grlc_ul_task
(
  /* Required for REX, ignore */
  dword unused
)
{
  NOTUSED(unused);
  grlc_ul_task_x (GERAN_ACCESS_STRATUM_ID_1, GERAN_TASK_ID_GRLC_UL);
}

/* Normalised naming
*/
void gsm_rlc_ul_task (dword dummy)
{
   grlc_ul_task(dummy);
}

/* Wrapper function for init uplink task one initialisation
*/
void gsm_rlc_ul_fn_init(void)
{
  grlc_ul_one_time_init(GERAN_ACCESS_STRATUM_ID_1);
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_TASK_X()
===
===  DESCRIPTION
===
===    REX task based scheduling framework for GPRS RLC Generic downlink Task
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/
void grlc_dl_task_x
(
  gas_id_t gas_id,
  geran_task_id_t task_id
)
{
  /* Watchdog HB for downlink task */
  dog_report_type   grlc_dl_dog_rpt;
  boolean task_stopped = FALSE;

  geran_tcb_set(task_id, rex_self());

  rcinit_handshake_startup(); /* required rcinit handshake */
  /* 2nd phase of init */
  grlc_dl_init(gas_id);

  /* Register with the Watchdog Heartbeat - once ONLY */
  grlc_dl_dog_rpt = dog_hb_register_rex((rex_sigs_type)GRLC_DL_DOG_RPT_TMR_SIG);

  RLC_DL_CLR_PL1_SIG(gas_id);
  RLC_DL_CLR_PL1_EPDAN_REQ_SIG(gas_id);
  RLC_DL_CLR_MAC_SIG(gas_id);
  RLC_DL_CLR_TMR_SIG(gas_id);


  /* Begin event based processing via REX scheduling. */

  while (!task_stopped)
  {
    /* Recind processor context until a relevent signal(s) becomes active. */

    grlc_dl_task_sigs[gas_id]
    = rex_wait
                        (
                          (
                            TASK_START_SIG          |
                            TASK_STOP_SIG           |
                            TASK_OFFLINE_SIG        |
                            GRLC_DL_DOG_RPT_TMR_SIG |
                            GRLC_DL_MASTER_SIG
                          )
                        );

    while ( grlc_dl_task_sigs[gas_id] )
    {
      /* Process any active signals with the following priority. */

      /* ------------------------------------------------------------------ */

      if ( grlc_dl_task_sigs[gas_id] & TASK_START_SIG )
      {
        (void)rex_clr_sigs( geran_tcb_read(task_id), TASK_START_SIG );
        grlc_dl_task_sigs[gas_id] = rex_get_sigs( geran_tcb_read(task_id) );
      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_dl_task_sigs[gas_id] & TASK_STOP_SIG )
      {
        /* Not used at present so just clear down the signal. */

        (void)rex_clr_sigs( geran_tcb_read(task_id), TASK_STOP_SIG );
        grlc_dl_task_sigs[gas_id] = rex_get_sigs( geran_tcb_read(task_id) );
        task_stopped = TRUE;
      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_dl_task_sigs[gas_id] & TASK_OFFLINE_SIG )
      {
        /* Not used at present so just clear down the signal. */

        (void)rex_clr_sigs( geran_tcb_read(task_id), TASK_OFFLINE_SIG );
        grlc_dl_task_sigs[gas_id] = rex_get_sigs( geran_tcb_read(task_id) );
      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_dl_task_sigs[gas_id] & GRLC_DL_DOG_RPT_TMR_SIG )
      {
        /* Report to the Dog HB */
        dog_hb_report( grlc_dl_dog_rpt );

        (void)rex_clr_sigs( geran_tcb_read(task_id), GRLC_DL_DOG_RPT_TMR_SIG );
        grlc_dl_task_sigs[gas_id] = rex_get_sigs( geran_tcb_read(task_id) );
      }

      /* ------------------------------------------------------------------ */
      else if ( grlc_dl_task_sigs[gas_id] & GRLC_DL_PL1_EPDAN_REQ_SIG )
      {
        /* Handle PL1 EPDAN Request signal.
        ** Note that this signal has priority over the GRLC_DL_PL1_SIG which
        ** handles processing of the DL FIFO
        */
        grlc_dl_handle_pl1_epdan_req_sig(gas_id);
      }
      /* ------------------------------------------------------------------ */

      else if ( grlc_dl_task_sigs[gas_id] & GRLC_DL_PL1_SIG )
      {
        /* Handle PL1 signal */
        grlc_dl_handle_pl1_sig(gas_id);
      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_dl_task_sigs[gas_id] & GRLC_DL_MAC_SIG )
      {
        /* Handle MAC signal */
        grlc_dl_handle_mac_sig(gas_id);
      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_dl_task_sigs[gas_id] & GRLC_DL_TMR_SIG )
      {
        /* Handle timer signal */
        grlc_dl_handle_tmr_sig(gas_id);
      }

      /* ------------------------------------------------------------------ */

      else if ( grlc_dl_task_sigs[gas_id] & GRLC_DL_LOG_SIG )
      {
        grlc_dl_handle_log_sig(gas_id);
      }

      /* ------------------------------------------------------------------ */
    }
  }

  /* Deregister if leaving the task so that we don't ERR_FATAL */
  dog_hb_deregister(grlc_dl_dog_rpt);

  /*UNREACHED*/
} /* end of grlc_dl_task() */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_TASK
===
===  DESCRIPTION
===
===    RLC Downlink task One
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/
void grlc_dl_task
(
  /* Required for REX, ignore */
  dword unused
)
{
  NOTUSED(unused);
  grlc_dl_task_x(GERAN_ACCESS_STRATUM_ID_1, GERAN_TASK_ID_GRLC_DL);
}

/* Normalise naming
*/
void gsm_rlc_dl_task (dword dummy)
{
  grlc_dl_task(dummy);
}

/* Wrapper function for init downlink task one initialisation
*/
void gsm_rlc_dl_fn_init(void)
{
  grlc_dl_one_time_init(GERAN_ACCESS_STRATUM_ID_1);
}

/***************************
** END Single SIM section
****************************
*/

/********************************************************************
** BEGIN Dual SIM section which also includes the TRIPLE SIM section
*********************************************************************
*/

#if (defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM))
/*===========================================================================
===
===  FUNCTION      GRLC_UL_2_TASK()
===
===  DESCRIPTION
===
===    REX task based scheduling framework for GPRS RLC Uplink functionality.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void grlc_ul_2_task
(
  /* Required for REX, ignore */
  dword unused
)
{
  NOTUSED(unused);
  grlc_ul_task_x (GERAN_ACCESS_STRATUM_ID_2, GERAN_TASK_ID_GRLC_UL_2);
} /* end of grlc_ul_2_task() */

/* Normalised naming
*/
void gsm_rlc_ul_2_task (dword dummy)
{
   grlc_ul_2_task(dummy);
}

/* Wrapper function for init uplink task two initialisation
*/
void gsm_rlc_ul_2_fn_init(void)
{
  grlc_ul_one_time_init(GERAN_ACCESS_STRATUM_ID_2);
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_2_TASK()
===
===  DESCRIPTION
===
===    REX task based scheduling framework for GPRS RLC Downlink
===    functionality.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void grlc_dl_2_task
(
  /* Required for REX, ignore.
     lint -esym(715,unused)
     Prevent lint from complaining about the unused parameter 'dummy' which
     is required to conform to the rex_def_task() definition. */

  dword unused
)
{
  NOTUSED(unused);
  grlc_dl_task_x(GERAN_ACCESS_STRATUM_ID_2, GERAN_TASK_ID_GRLC_DL_2);
} /* end of grlc_dl_2_task() */

/* Normalised naming
*/
void gsm_rlc_dl_2_task (dword dummy)
{
   grlc_dl_2_task(dummy);
}

/* Wrapper function for init downlink task two initialisation
*/
void gsm_rlc_dl_2_fn_init(void)
{
  grlc_dl_one_time_init(GERAN_ACCESS_STRATUM_ID_2);
}

#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
/******************************************************************
** END Dual SIM section which also includes the TRIPLE SIM section
*******************************************************************
*/

/***************************
** BEGIN Triple SIM section
****************************
*/
#ifdef FEATURE_TRIPLE_SIM
/*===========================================================================
===
===  FUNCTION      GRLC_UL_3_TASK()
===
===  DESCRIPTION
===
===    REX task based scheduling framework for GPRS RLC Uplink functionality.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void grlc_ul_3_task
(
  /* Required for REX, ignore */
  dword unused
)
{
  NOTUSED(unused);
  grlc_ul_task_x (GERAN_ACCESS_STRATUM_ID_3, GERAN_TASK_ID_GRLC_UL_3);
} /* end of grlc_ul_3_task() */

/* Normalised naming
*/
void gsm_rlc_ul_3_task (dword dummy)
{
   grlc_ul_3_task(dummy);
}

/* Wrapper function for init uplink task 3 initialisation
*/
void gsm_rlc_ul_3_fn_init(void)
{
  grlc_ul_one_time_init(GERAN_ACCESS_STRATUM_ID_3);
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_3_TASK()
===
===  DESCRIPTION
===
===    REX task based scheduling framework for GPRS RLC Downlink
===    functionality.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void grlc_dl_3_task
(
  /* Required for REX, ignore.
     lint -esym(715,unused)
     Prevent lint from complaining about the unused parameter 'dummy' which
     is required to conform to the rex_def_task() definition. */

  dword unused
)
{
  NOTUSED(unused);
  grlc_dl_task_x(GERAN_ACCESS_STRATUM_ID_3, GERAN_TASK_ID_GRLC_DL_3);
} /* end of grlc_dl_3_task() */

/* Normalised naming
*/
void gsm_rlc_dl_3_task (dword dummy)
{
   grlc_dl_3_task(dummy);
}

/* Wrapper function for init downlink task 3 initialisation
*/
void gsm_rlc_dl_3_fn_init(void)
{
  grlc_dl_one_time_init(GERAN_ACCESS_STRATUM_ID_3);
}

#endif /* FEATURE_TRIPLE_SIM */
/***************************
** END Triple SIM section
****************************
*/


/*** EOF: don't remove! ***/

