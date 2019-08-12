/*****************************************************************************
***
*** TITLE
***
***  GPRS LAYER 2 SERVICES CONTROL TASK FUNCTIONS
***
***
*** DESCRIPTION
***
***  REX task based scheduling framework for GPRS LLC functionality.
***
***
*** EXTERNALIZED FUNCTIONS
***
***  gllc_task()
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  REX initialisation of the task control blocks and stacks is the
***  function of the TMC Task Mode Controller.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gllc/src/gllctask.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10/12/13   ra      CR563511
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "geran_tasks.h"
#include "gllci.h"
#include "gllclient.h"

#include "comdef.h"
#include "gmutex.h"
#include "dsm.h"
#include "err.h"
#include "msg.h"
#include "rex.h"
#include "task.h"
#include "rcinit.h"

#include "dog_hb_rex.h"

#include "gllclog.h"

#include "gsndcp_v_g.h"


/* LLC critical section to protect accesses to LLC messages and internal vars
*/

rex_crit_sect_type gllc_crit_sec[ NUM_GERAN_DATA_SPACES ];
rex_crit_sect_type gllc_ul_msg_crit_sec[ NUM_GERAN_DATA_SPACES ];
rex_crit_sect_type gllc_dl_msg_crit_sec[ NUM_GERAN_DATA_SPACES ];
rex_crit_sect_type gllc_cipher_crit_sec;


/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/* This feature is defined to support two input parameters into call back
   functions.*/

#define FEATURE_DSM_WM_CB


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

static void gllc_initialise( gas_id_t gas_id );


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/

rex_sigs_type     gllc_task_sigs[ NUM_GERAN_DATA_SPACES ]   = { 0 };
boolean           gllc_initialised[ NUM_GERAN_DATA_SPACES ] = { FALSE };
rex_timer_type    gllc_gmm_ready_timer[ NUM_GERAN_DATA_SPACES ];


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

/* LLC UL Watermark Event Notification Functions */

static void gllc_ll_1_ul_ack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_SET_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_1_GMM, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_1_ul_ack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_CLR_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_1_GMM, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_1_ul_unack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_SET_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_1_GMM, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}


static void gllc_ll_1_ul_unack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_CLR_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_1_GMM, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}

/* ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */


static void gllc_ll_3_ul_ack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_SET_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_3_LL3, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_3_ul_ack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_CLR_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_3_LL3, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_3_ul_unack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_SET_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_3_LL3, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}


static void gllc_ll_3_ul_unack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_CLR_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_3_LL3, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}

/* ------------------------------------------------------------------------ */


static void gllc_ll_5_ul_ack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_SET_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_5_LL5, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_5_ul_ack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_CLR_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_5_LL5, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_5_ul_unack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_SET_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_5_LL5, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}


static void gllc_ll_5_ul_unack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_CLR_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_5_LL5, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}

/* ------------------------------------------------------------------------ */


static void gllc_ll_7_ul_ack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_SET_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_7_SMS, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_7_ul_ack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_CLR_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_7_SMS, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_7_ul_unack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_SET_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_7_SMS, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}


static void gllc_ll_7_ul_unack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_CLR_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_7_SMS, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}

/* ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */


static void gllc_ll_9_ul_ack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_SET_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_9_LL9, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_9_ul_ack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_CLR_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_9_LL9, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_9_ul_unack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_SET_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_9_LL9, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}


static void gllc_ll_9_ul_unack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_CLR_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_9_LL9, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}

/* ------------------------------------------------------------------------ */


static void gllc_ll_11_ul_ack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_SET_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_11_LL11, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_11_ul_ack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_CLR_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_11_LL11, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_11_ul_unack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_SET_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_11_LL11, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}


static void gllc_ll_11_ul_unack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
  gas_id_t gas_id = ( gas_id_t )context;
  GLLC_CLR_LLE_UL_EVT( gas_id, GLLC_LL_SAPI_11_LL11, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}

/*===========================================================================
===
===  FUNCTION      GLLC_INITIALISE()
===
===  DESCRIPTION
===
===    For use by LLC control task to initialise the LLC Process.
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

static void gllc_initialise( gas_id_t gas_id )
{
  uint32  lle;
  uint32  llme_ul_scheme;
  uint32  lle_ul_scheme;

  /* --------------------------------------------------------------------- */

  if( gas_id == GERAN_ACCESS_STRATUM_ID_1 )
  {
    geran_tcb_set(GERAN_TASK_ID_GLLC_1, rex_self());
  }

#if (defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM))

  else if( gas_id == GERAN_ACCESS_STRATUM_ID_2 )
  {
    geran_tcb_set(GERAN_TASK_ID_GLLC_2, rex_self());
  }

#endif /* (FEATURE_DUAL_SIM) || (FEATURE_TRIPLE_SIM) */


#ifdef FEATURE_TRIPLE_SIM

  else if( gas_id == GERAN_ACCESS_STRATUM_ID_3 )
  {
    geran_tcb_set(GERAN_TASK_ID_GLLC_3, rex_self());
  }

#endif /* FEATURE_TRIPLE_SIM */

  /* --------------------------------------------------------------------- */

  /* LLE initialisations. */

  for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
  {
    /* Initialise LL SAP Client Records. */

    gllc_ll_client_record[ gas_id ][lle].ll_dl_sig_get_mes_buf_fnc_ptr = NULL;
    gllc_ll_client_record[ gas_id ][lle].ll_dl_sig_put_mes_buf_fnc_ptr = NULL;
    gllc_ll_client_record[ gas_id ][lle].ll_dl_dat_put_mes_buf_fnc_ptr = NULL;

    /* ................................................................... */

    /* Initialise LLE queues and associated parameters. */

    (void)q_init( &gllc_cr_ul_q[gas_id][lle] );
    (void)q_init( &gllc_ll_ul_sig_q[gas_id][lle] );

    gllc_lle[gas_id][lle].ack.retrx_first_p = NULL;
    gllc_lle[gas_id][lle].ack.retrx_last_p  = NULL;

    gllc_lle[gas_id][lle].ack.reseq_first_p = NULL;
    gllc_lle[gas_id][lle].ack.reseq_last_p  = NULL;

    /* ................................................................... */

    /* Initialise LLE acknowledged data watermarks. */

    gllc_ll_ul_ack_wm[gas_id][lle].lo_watermark             = 0;
    gllc_ll_ul_ack_wm[gas_id][lle].hi_watermark             = 0;
    gllc_ll_ul_ack_wm[gas_id][lle].current_cnt              = 0;

#ifdef FEATURE_DSM_MEM_CHK
    gllc_ll_ul_ack_wm[gas_id][lle].highest_cnt              = 0;
    gllc_ll_ul_ack_wm[gas_id][lle].total_rcvd_cnt           = 0;
#endif

    gllc_ll_ul_ack_wm[gas_id][lle].each_enqueue_func_ptr    = NULL;
    gllc_ll_ul_ack_wm[gas_id][lle].gone_empty_func_ptr      = NULL;
    gllc_ll_ul_ack_wm[gas_id][lle].non_empty_func_ptr       = NULL;
    gllc_ll_ul_ack_wm[gas_id][lle].lowater_func_ptr         = NULL;
    gllc_ll_ul_ack_wm[gas_id][lle].hiwater_func_ptr         = NULL;

    /* Initialise the ack watermark queue. Don't Exceed count is set to zero
       via the function below. Ensure that Don't Exceed count is set to
       appropriate value. */

    dsm_queue_init ( &gllc_ll_ul_ack_wm[gas_id][lle], 0, &gllc_ll_ul_ack_q[gas_id][lle]);

    /* ................................................................... */

    /* Initialise LLE unacknowledged data watermarks. */

    gllc_ll_ul_unack_wm[gas_id][lle].lo_watermark           = 0;
    gllc_ll_ul_unack_wm[gas_id][lle].hi_watermark           = 0;
    gllc_ll_ul_unack_wm[gas_id][lle].current_cnt            = 0;

#ifdef FEATURE_DSM_MEM_CHK
    gllc_ll_ul_unack_wm[gas_id][lle].highest_cnt            = 0;
    gllc_ll_ul_unack_wm[gas_id][lle].total_rcvd_cnt         = 0;
#endif

    gllc_ll_ul_unack_wm[gas_id][lle].each_enqueue_func_ptr  = NULL;
    gllc_ll_ul_unack_wm[gas_id][lle].gone_empty_func_ptr    = NULL;
    gllc_ll_ul_unack_wm[gas_id][lle].non_empty_func_ptr     = NULL;
    gllc_ll_ul_unack_wm[gas_id][lle].lowater_func_ptr       = NULL;
    gllc_ll_ul_unack_wm[gas_id][lle].hiwater_func_ptr       = NULL;

    /* Initialise the unack watermark queue. Don't Exceed count is set to zero
       via the function below. Ensure that Don't Exceed count is set to
       appropriate value. */

    dsm_queue_init ( &gllc_ll_ul_unack_wm[gas_id][lle], 0, &gllc_ll_ul_unack_q[gas_id][lle]);


    /* ................................................................... */

    /* Initialise LLE timers. */

    if ( gas_id == GERAN_ACCESS_STRATUM_ID_1 )
    {
      rex_def_timer_ex
      (
        &gllc_timer_t200[gas_id][lle],
        gllc_timer_callback_1,
        (uint32)( (lle << 16) | GLLC_LLE_UL_TIMER_T200_EVT )
      );

      rex_def_timer_ex
      (
        &gllc_timer_t201[gas_id][lle],
        gllc_timer_callback_1,
        (uint32)( (lle << 16) | GLLC_LLE_UL_TIMER_T201_EVT )
      );
    }
#ifdef FEATURE_DUAL_SIM
    else if ( gas_id == GERAN_ACCESS_STRATUM_ID_2 )
    {
      rex_def_timer_ex
      (
        &gllc_timer_t200[gas_id][lle],
        gllc_timer_callback_2,
        (uint32)( (lle << 16) | GLLC_LLE_UL_TIMER_T200_EVT )
      );

      rex_def_timer_ex
      (
        &gllc_timer_t201[gas_id][lle],
        gllc_timer_callback_2,
        (uint32)( (lle << 16) | GLLC_LLE_UL_TIMER_T201_EVT )
      );
    }
#endif
#ifdef FEATURE_TRIPLE_SIM

    else if ( gas_id == GERAN_ACCESS_STRATUM_ID_3 )
    {
      rex_def_timer_ex
      (
        &gllc_timer_t200[gas_id][lle],
        gllc_timer_callback_3,
        (uint32)( (lle << 16) | GLLC_LLE_UL_TIMER_T200_EVT )
      );

      rex_def_timer_ex
      (
        &gllc_timer_t201[gas_id][lle],
        gllc_timer_callback_3,
        (uint32)( (lle << 16) | GLLC_LLE_UL_TIMER_T201_EVT )
      );
    }

#endif /* FEATURE_TRIPLE_SIM */

    /* ................................................................... */

    /* Initialise LLE event masking scheme record flags. */

    for
    (
      lle_ul_scheme = 0;
      lle_ul_scheme < (uint32)GLLC_LLE_NUM_SCHEMES;
      lle_ul_scheme++
    )
    {
      gllc_lle_ul_evt_mask_scheme_flags[gas_id][lle_ul_scheme][lle] = FALSE;
    }

    /* ................................................................... */

    /* Intialise LLE active event and mask bitmaps. */

    gllc_lle_ul_event_map[gas_id][lle] = GLLC_LLE_UL_NO_EVT;
    gllc_lle_ul_mask_map[gas_id][lle]  = (uint16)~GLLC_LLE_UL_NO_EVT;

    /* ................................................................... */

    /* Finally perform a full initialisation of the LLE. */

    gllc_lle_reset( gas_id, (gllc_ll_sapi_t)lle, GLLC_LLE_TLLI_UNASSIGNED );
  }

  /* --------------------------------------------------------------------- */

  /* Non LLE specific event masking scheme flags initialisations. */

  for
  (
    llme_ul_scheme = 0;
    llme_ul_scheme < (uint32)GLLC_LLME_NUM_SCHEMES;
    llme_ul_scheme++
  )
  {
    gllc_llme_ul_evt_mask_scheme_flags[gas_id][llme_ul_scheme] = FALSE;
  }

  /* --------------------------------------------------------------------- */

  /* LLE Watermark event notification, high, low, and dne initialisations. */

  /* SAPI-1 */
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_1_GMM].gone_empty_func_ptr     = gllc_ll_1_ul_ack_wm_clr_evt;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_1_GMM].gone_empty_func_data    = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_1_GMM].non_empty_func_ptr      = gllc_ll_1_ul_ack_wm_set_evt;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_1_GMM].non_empty_func_data     = (void *)gas_id;

  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_1_GMM].gone_empty_func_ptr   = gllc_ll_1_ul_unack_wm_clr_evt;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_1_GMM].gone_empty_func_data  = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_1_GMM].non_empty_func_ptr    = gllc_ll_1_ul_unack_wm_set_evt;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_1_GMM].non_empty_func_data   = (void *)gas_id;

  /* Flow control is not supported for GMM but set a maximum limit of the unacknowledged
     watermark occupancy of N maximal length frames. */

  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_1_GMM].dont_exceed_cnt       =
    (uint16)(gllc_max_N201_U_octets[ GLLC_LL_SAPI_1_GMM ] * 5);



  /* SAPI-3 */
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_3_LL3].gone_empty_func_ptr     = gllc_ll_3_ul_ack_wm_clr_evt;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_3_LL3].gone_empty_func_data    = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_3_LL3].non_empty_func_ptr      = gllc_ll_3_ul_ack_wm_set_evt;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_3_LL3].non_empty_func_data     = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_3_LL3].hiwater_func_ptr        = gsn_ll3_ul_ack_hiwater;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_3_LL3].hiwater_func_data       = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_3_LL3].lowater_func_ptr        = gsn_ll3_ul_ack_lowater;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_3_LL3].lowater_func_data       = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_3_LL3].hi_watermark            = GSN_LL3_ACK_HIWATER_CNT;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_3_LL3].lo_watermark            = GSN_LL3_ACK_LOWATER_CNT;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_3_LL3].dont_exceed_cnt         = GSN_LL3_ACK_DISCARD_CNT;


  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_3_LL3].gone_empty_func_ptr   = gllc_ll_3_ul_unack_wm_clr_evt;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_3_LL3].gone_empty_func_data  = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_3_LL3].non_empty_func_ptr    = gllc_ll_3_ul_unack_wm_set_evt;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_3_LL3].non_empty_func_data   = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_3_LL3].hiwater_func_ptr      = gsn_ll3_ul_unack_hiwater;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_3_LL3].hiwater_func_data     = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_3_LL3].lowater_func_ptr      = gsn_ll3_ul_unack_lowater;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_3_LL3].lowater_func_data     = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_3_LL3].hi_watermark          = GSN_LL3_UNACK_HIWATER_CNT;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_3_LL3].lo_watermark          = GSN_LL3_UNACK_LOWATER_CNT;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_3_LL3].dont_exceed_cnt       = GSN_LL3_UNACK_DISCARD_CNT;


  /* SAPI-5 */
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_5_LL5].gone_empty_func_ptr     = gllc_ll_5_ul_ack_wm_clr_evt;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_5_LL5].gone_empty_func_data    = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_5_LL5].non_empty_func_ptr      = gllc_ll_5_ul_ack_wm_set_evt;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_5_LL5].non_empty_func_data     = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_5_LL5].hiwater_func_ptr        = gsn_ll5_ul_ack_hiwater;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_5_LL5].hiwater_func_data       = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_5_LL5].lowater_func_ptr        = gsn_ll5_ul_ack_lowater;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_5_LL5].lowater_func_data       = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_5_LL5].hi_watermark            = GSN_LL5_ACK_HIWATER_CNT;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_5_LL5].lo_watermark            = GSN_LL5_ACK_LOWATER_CNT;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_5_LL5].dont_exceed_cnt         = GSN_LL5_ACK_DISCARD_CNT;


  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_5_LL5].gone_empty_func_ptr   = gllc_ll_5_ul_unack_wm_clr_evt;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_5_LL5].gone_empty_func_data  = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_5_LL5].non_empty_func_ptr    = gllc_ll_5_ul_unack_wm_set_evt;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_5_LL5].non_empty_func_data   = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_5_LL5].hiwater_func_ptr      = gsn_ll5_ul_unack_hiwater;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_5_LL5].hiwater_func_data     = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_5_LL5].lowater_func_ptr      = gsn_ll5_ul_unack_lowater;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_5_LL5].lowater_func_data     = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_5_LL5].hi_watermark          = GSN_LL5_UNACK_HIWATER_CNT;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_5_LL5].lo_watermark          = GSN_LL5_UNACK_LOWATER_CNT;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_5_LL5].dont_exceed_cnt       = GSN_LL5_UNACK_DISCARD_CNT;


  /* SAPI-7 */
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_7_SMS].gone_empty_func_ptr     = gllc_ll_7_ul_ack_wm_clr_evt;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_7_SMS].gone_empty_func_data    = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_7_SMS].non_empty_func_ptr      = gllc_ll_7_ul_ack_wm_set_evt;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_7_SMS].non_empty_func_data     = (void *)gas_id;

  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_7_SMS].gone_empty_func_ptr   = gllc_ll_7_ul_unack_wm_clr_evt;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_7_SMS].gone_empty_func_data  = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_7_SMS].non_empty_func_ptr    = gllc_ll_7_ul_unack_wm_set_evt;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_7_SMS].non_empty_func_data   = (void *)gas_id;

  /* Flow control is not supported for GSMS but set a maximum limit on the unacknowledged
     watermark occupancy of N maximal length frames. */

  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_7_SMS].dont_exceed_cnt =
      (uint16)(gllc_max_N201_U_octets[ GLLC_LL_SAPI_7_SMS ] * 5);



  /* SAPI-9 */
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_9_LL9].gone_empty_func_ptr     = gllc_ll_9_ul_ack_wm_clr_evt;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_9_LL9].gone_empty_func_data    = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_9_LL9].non_empty_func_ptr      = gllc_ll_9_ul_ack_wm_set_evt;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_9_LL9].non_empty_func_data     = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_9_LL9].hiwater_func_ptr        = gsn_ll9_ul_ack_hiwater;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_9_LL9].hiwater_func_data       = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_9_LL9].lowater_func_ptr        = gsn_ll9_ul_ack_lowater;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_9_LL9].lowater_func_data       = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_9_LL9].hi_watermark            = GSN_LL9_ACK_HIWATER_CNT;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_9_LL9].lo_watermark            = GSN_LL9_ACK_LOWATER_CNT;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_9_LL9].dont_exceed_cnt         = GSN_LL9_ACK_DISCARD_CNT;

  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_9_LL9].gone_empty_func_ptr   = gllc_ll_9_ul_unack_wm_clr_evt;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_9_LL9].gone_empty_func_data  = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_9_LL9].non_empty_func_ptr    = gllc_ll_9_ul_unack_wm_set_evt;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_9_LL9].non_empty_func_data   = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_9_LL9].hiwater_func_ptr      = gsn_ll9_ul_unack_hiwater;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_9_LL9].hiwater_func_data     = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_9_LL9].lowater_func_ptr      = gsn_ll9_ul_unack_lowater;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_9_LL9].lowater_func_data     = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_9_LL9].hi_watermark          = GSN_LL9_UNACK_HIWATER_CNT;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_9_LL9].lo_watermark          = GSN_LL9_UNACK_LOWATER_CNT;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_9_LL9].dont_exceed_cnt       = GSN_LL9_UNACK_DISCARD_CNT;


  /* SAPI-11 */
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_11_LL11].gone_empty_func_ptr   = gllc_ll_11_ul_ack_wm_clr_evt;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_11_LL11].gone_empty_func_data  = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_11_LL11].non_empty_func_ptr    = gllc_ll_11_ul_ack_wm_set_evt;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_11_LL11].non_empty_func_data   = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_11_LL11].hiwater_func_ptr      = gsn_ll11_ul_ack_hiwater;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_11_LL11].hiwater_func_data     = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_11_LL11].lowater_func_ptr      = gsn_ll11_ul_ack_lowater;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_11_LL11].lowater_func_data     = (void *)gas_id;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_11_LL11].hi_watermark          = GSN_LL11_ACK_HIWATER_CNT;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_11_LL11].lo_watermark          = GSN_LL11_ACK_LOWATER_CNT;
  gllc_ll_ul_ack_wm[gas_id][GLLC_LL_SAPI_11_LL11].dont_exceed_cnt       = GSN_LL11_ACK_DISCARD_CNT;

  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_11_LL11].gone_empty_func_ptr = gllc_ll_11_ul_unack_wm_clr_evt;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_11_LL11].gone_empty_func_data= (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_11_LL11].non_empty_func_ptr  = gllc_ll_11_ul_unack_wm_set_evt;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_11_LL11].non_empty_func_data = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_11_LL11].hiwater_func_ptr    = gsn_ll11_ul_unack_hiwater;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_11_LL11].hiwater_func_data   = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_11_LL11].lowater_func_ptr    = gsn_ll11_ul_unack_lowater;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_11_LL11].lowater_func_data   = (void *)gas_id;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_11_LL11].hi_watermark        = GSN_LL11_UNACK_HIWATER_CNT;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_11_LL11].lo_watermark        = GSN_LL11_UNACK_LOWATER_CNT;
  gllc_ll_ul_unack_wm[gas_id][GLLC_LL_SAPI_11_LL11].dont_exceed_cnt     = GSN_LL11_UNACK_DISCARD_CNT;

  /* --------------------------------------------------------------------- */

  /* LLME initialisations. */

  gllc_llme_reset(gas_id);

  /* TLLI Last Received, GEA, Cipher Key and IOV-UI must traverse general
     LLME Reset operations, and so must be reset individually */

  gllc_llme[gas_id].tlli_last_rxd       = GLLC_TLLI_UNASSIGNED;
  gllc_llme[gas_id].gea                 = GMMLLC_GEA_NONE;
  gllc_llme[gas_id].key_Kc[0]           = 0;
  gllc_llme[gas_id].key_Kc[1]           = 0;
  gllc_llme[gas_id].key_Kc128[0]        = 0;
  gllc_llme[gas_id].key_Kc128[1]        = 0;
  gllc_llme[gas_id].key_Kc128[2]        = 0;
  gllc_llme[gas_id].key_Kc128[3]        = 0;
  gllc_llme[gas_id].input_offset_IOV_UI = 0;

#ifdef FEATURE_DUAL_SIM

  /* Access Stratum ID is reset */

  gllc_llme[gas_id].gas_id              = gas_id;

#endif /* FEATURE_DUAL_SIM */

  /* --------------------------------------------------------------------- */

  /* Other initialisations. */

  /* Host the GMM Ready Timer on behalf of GMM for restart efficiency
     purposes. */

  rex_def_timer( &gllc_gmm_ready_timer[ gas_id], rex_self(), GLLC_GMM_RDY_TMR_SIG );

  /* --------------------------------------------------------------------- */

  MSG_GERAN_LOW_0_G( "GLC ME: Initialised");
  gllc_initialised[ gas_id ] = TRUE;

  /* --------------------------------------------------------------------- */
} /* end of gllc_initialise() */


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GSM_LLC_TASK_X_FN_INIT()
===
===  DESCRIPTION
===
===  Only use this mechanism to perform memory init, hardware init, or other
===  startup init to satisfy dependency that follow. Avoid all HLOS mechanisms
===  that will block.
===
===  Code in this function will add to startup latency. Do not use large loops
===  that can be avoided.
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

void gsm_llc_task_x_fn_init( gas_id_t gas_id )
{

  /* --------------------------------------------------------------------- */


  /* Initialise LLC critical section */

  rex_init_crit_sect(&gllc_crit_sec[ gas_id ]);

  rex_init_crit_sect(&gllc_ul_msg_crit_sec[ gas_id ]);

  rex_init_crit_sect(&gllc_dl_msg_crit_sec[ gas_id ]);

  /* This is a mutex common to all GLLC tasks, only initialise in association with task 1 */
  if( GERAN_ACCESS_STRATUM_ID_1 == gas_id )
  {
    rex_init_crit_sect(&gllc_cipher_crit_sec);
  }


  /* --------------------------------------------------------------------- */

  /* Initialise non-LLE queues. */

  (void)q_init( &gllc_gmm_ul_q[ gas_id ] );
  (void)q_init( &gllc_grr_dl_q[ gas_id ] );

  /* --------------------------------------------------------------------- */

}

/*===========================================================================
===
===  FUNCTION      GSM_LLC_TASK_1_FN_INIT()
===
===  DESCRIPTION
===
===  Only use this mechanism to perform memory init, hardware init, or other
===  startup init to satisfy dependency that follow. Avoid all HLOS mechanisms
===  that will block.
===
===  Code in this function will add to startup latency. Do not use large loops
===  that can be avoided.
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

void gsm_llc_task_1_fn_init( void )
{
  gsm_llc_task_x_fn_init( GERAN_ACCESS_STRATUM_ID_1 );
}

#if (defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM))

/*===========================================================================
===
===  FUNCTION      GSM_LLC_TASK_2_FN_INIT()
===
===  DESCRIPTION
===
===  Only use this mechanism to perform memory init, hardware init, or other
===  startup init to satisfy dependency that follow. Avoid all HLOS mechanisms
===  that will block.
===
===  Code in this function will add to startup latency. Do not use large loops
===  that can be avoided.
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

void gsm_llc_task_2_fn_init( void )
{
  gsm_llc_task_x_fn_init( GERAN_ACCESS_STRATUM_ID_2 );
}

#endif /* (FEATURE_DUAL_SIM) || (FEATURE_TRIPLE_SIM) */


#ifdef FEATURE_TRIPLE_SIM

/*===========================================================================
===
===  FUNCTION      GSM_LLC_TASK_3_FN_INIT()
===
===  DESCRIPTION
===
===  Only use this mechanism to perform memory init, hardware init, or other
===  startup init to satisfy dependency that follow. Avoid all HLOS mechanisms
===  that will block.
===
===  Code in this function will add to startup latency. Do not use large loops
===  that can be avoided.
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

void gsm_llc_task_3_fn_init( void )
{
  gsm_llc_task_x_fn_init( GERAN_ACCESS_STRATUM_ID_3 );
}

#endif /* FEATURE_TRIPLE_SIM */


/*===========================================================================
===
===  FUNCTION      GLLC_TASK_X()
===
===  DESCRIPTION
===
===    REX task based scheduling framework for GPRS LLC functionality for
===    subscriptions.
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

void gllc_task_x
(
  gas_id_t gas_id
)
{
  dog_report_type     gllc_dog_rpt;

  /* Perform startup init here. Satisfy dependency of other tasks
     provided by this task. Do not linger here, startup latency is
     impacted. Higher priority tasks in the same group will preempt
     this task at this point. Do not rely upon priority alone as the
     sole mechanism to arbitrate resource contention with other tasks. */

  rcinit_handshake_startup(); /* required rcinit handshake */

  gllc_initialise(gas_id);

  gllc_dog_rpt = dog_hb_register_rex((rex_sigs_type)GLLC_DOG_RPT_TMR_SIG);

  /* Perform remaining init here. This is sequenced by a start
     signal delivered at the end of the sequence group processing. */



  /* Register GMM, GSMS, GSNDCP proxy clients on appropriate LL SAPs. */

  gllclient_reg_gmm_gsms_proxy(gas_id);



  /* Initialise logging. */

  llc_log_init(gas_id);



  /* Begin event based processing via REX scheduling. */

  for (;;)
  {
    /* Recind processor context until a relevent signal(s) becomes active. */

    gllc_task_sigs[gas_id] =
                      rex_wait
                      (
                        (
                          TASK_START_SIG       |
                          TASK_STOP_SIG        |
                          TASK_OFFLINE_SIG     |
                          GLLC_DOG_RPT_TMR_SIG |
                          GLLC_UL_MASTER_SIG   |
                          GLLC_DL_MASTER_SIG   |
                          GLLC_GMM_RDY_TMR_SIG |
                          GLLC_TEST_MODE_SIG
                        )
                      );

    /* Process any active signals with the following priority. */

    if ( gllc_task_sigs[gas_id] & TASK_START_SIG )
    {
      (void)rex_clr_sigs( rex_self(), TASK_START_SIG );
    }

    if ( gllc_task_sigs[gas_id] & TASK_STOP_SIG )
    {
      /* Used in QTF. Clear down the signal and stop task to fix KW error. */
      (void)rex_clr_sigs( rex_self(), TASK_STOP_SIG );

      rcinit_handshake_term();

      break;
    }

    if ( gllc_task_sigs[gas_id] & TASK_OFFLINE_SIG )
    {
      /* Not used at present so just clear down the signal. */
      (void)rex_clr_sigs( rex_self(), TASK_OFFLINE_SIG );
    }

    if ( gllc_task_sigs[gas_id] & GLLC_DOG_RPT_TMR_SIG )
    {
      (void)rex_clr_sigs( rex_self(), GLLC_DOG_RPT_TMR_SIG );
      dog_hb_report( gllc_dog_rpt );
    }

    if ( gllc_task_sigs[gas_id] & GLLC_TEST_MODE_SIG )
    {
      gllc_test_mode_handler(gas_id);
    }

    if ( gllc_task_sigs[gas_id] & GLLC_GMM_RDY_TMR_SIG )
    {
      gllc_gmm_ready_timer_handler( gas_id );
    }

    if ( gllc_task_sigs[gas_id] & (GLLC_UL_MASTER_SIG | GLLC_DL_MASTER_SIG) )
    {
      gllc_event_handler( gas_id );
    }
  }

  dog_hb_deregister( gllc_dog_rpt );

  /*UNREACHED*/
}

/*===========================================================================
===
===  FUNCTION      GLLC_TASK_1()
===
===  DESCRIPTION
===
===    REX task based scheduling framework for GPRS LLC functionality for 1st
===    subscription.
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
void gllc_task_1
(
  /* Required for REX, ignore. */

  /*lint -esym(715,unused) Prevent lint from complaining about the unused
    parameter 'dummy' which is required to conform to the rex_def_task()
    definition. */

  dword  unused
)
{
  NOTUSED(unused); /* silence compiler warnings of unused parameter */
  gllc_task_x(GERAN_ACCESS_STRATUM_ID_1);
}

#if (defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM))

/*===========================================================================
===
===  FUNCTION      GLLC_TASK_2()
===
===  DESCRIPTION
===
===    REX task based scheduling framework for GPRS LLC functionality for 2nd
===    subscription.
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

void gllc_task_2
(
  /* Required for REX, ignore. */

  /*lint -esym(715,unused) Prevent lint from complaining about the unused
    parameter 'dummy' which is required to conform to the rex_def_task()
    definition. */

  dword  unused
)
{
  NOTUSED(unused); /* silence compiler warnings of unused parameter */
  gllc_task_x(GERAN_ACCESS_STRATUM_ID_2);
}

#endif /* (FEATURE_DUAL_SIM) || (FEATURE_TRIPLE_SIM) */


#ifdef FEATURE_TRIPLE_SIM

/*===========================================================================
===
===  FUNCTION      GLLC_TASK_3()
===
===  DESCRIPTION
===
===    REX task based scheduling framework for GPRS LLC functionality for 3rd
===    subscription.
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

void gllc_task_3
(
  /* Required for REX, ignore. */

  /*lint -esym(715,unused) Prevent lint from complaining about the unused
    parameter 'dummy' which is required to conform to the rex_def_task()
    definition. */

  dword  unused
)
{
  NOTUSED(unused); /* silence compiler warnings of unused parameter */
  gllc_task_x(GERAN_ACCESS_STRATUM_ID_3);
}

#endif /* FEATURE_TRIPLE_SIM */



/*** EOF: don't remove! ***/

