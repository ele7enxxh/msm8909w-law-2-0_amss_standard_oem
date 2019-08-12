/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              FTM Log

GENERAL DESCRIPTION
  This is the header file for FTM Logging.

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_log.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/28/14   zbz     Added support for GSM enhanced AGC logging 
08/26/14   aa      Added support for FBRX QRCT logging
09/04/13   sg      Corrected Featurization for SE BER test logging
07/09/13   aca     ATUNER logging
06/18/13   kai     Stop sending legacy WCDMA FTM log packet ftm_log_wcdma_agc
04/05/13   gvn     FTM IRAT Timing packet support
02/26/13   gvn     Add new LTE FTM log for CA enabled RxAGC
07/16/12   hy      Add TD-SCDMA FTM Rx AGC and Tx AGC logs.
07/09/12   nv      (Ported)Changes to enable GSM FTM logging using sub mask instead of FTM looging ID
02/14/12   sb      Changes to support FTM logging for GSM
02/14/12   sar     Reinstated TDSCDMA features.
12/05/11   whc     LTE SMPS logging / Enhanced TXAGC log support
01/28/12   sar     Feature reduction changes.
12/05/11   whc     LTE SMPS logging / Enhanced TXAGC log support
09/09/11   aro     Renamed CDMA logging functions to reflect primary and
                   secondary chains
09/07/11   aro     Updated intelliceiver logging with new API
09/07/11   aro     Updated HDR logging with new API
08/24/11   aro     Updated 1x logging with new API
07/11/11   hy      Including TD-SCDMA support.
06/07/11   aro     Mainlined FEATURE_RF_SVDO_API
01/25/11   sr      merged cmi code from SCMM to remove the clk.h dependency.
12/13/10   aro     Renamed Intelliciever Logging function
12/07/10   aro     Added Logging APIs for SVDO
10/25/10   ems     Added FTM Sequencer Log
07/28/10   mkv     Intial version of LTE Tx and Rx AGC log packets.
03/19/10   lcl     Pull in fixes from QSC6295
02/09/10   sar     KW fixes.
01/26/10   sar     Lint fixes.
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
12/10/09   bn      FTM WCDMA ENH log packet
10/02/09   zg      Backed out SVDO FTM related changes.
09/17/09   sar     Updated clk interface for CMI conformance.
09/13/09   vb      Added ICI calibration support
09/14/09   sr      Merged from 8650 SVDO dev branch
09/09/09   aro     Removed DateTime and Author + Extra Space Removal
                   + Featurized Code with FEATURE_RF_SVDO_API
07/30/09   aro     Created New files for NV and Logging
03/06/08   jtn     Fix Klocwork errors.
02/22/08   dw      Fixed high Lint warnings (546)
09/19/07   jfc     Wrap DACC log entry with FTM_HAS_WCDMA
08/06/07   jfc     Featurized code for EDGE-only builds
07/25/07   ycl     Lint fixes.
06/25/07   ad      Added support for single ended BER
06/07/07   sar     Featurized the definition for ftm_log_config_table.
05/22/07   sar     Allocated space to ftm_log_config_table.
02/28/07   ycl     Fix compiler warnings.
02/05/07   ycl     Added FTM_LOG_WCDMA_TXPLIM type.
11/16/06   vm/jfc  Allow FTM logging to be mode independent
07/13/06   xw      Type casting -- compilation error fix for 7200.
06/16/06   jac     Fixed LINT errors.
03/09/06   ka      Added FTM_LOG_TX_RX_SWEEP.
07/20/05   dp      Use weakly defined types to handle 1x code.
06/14/05   rsr     Added log support for therm and vbatt
10/12/04   jac     Mainlined ASYNC packet code.
12/15/03   dp      Port from 6100 code
11/18/03   jwh     Substitute include files msm.h and msmhwio.h.
10/17/03   ka      Dereg. callback when changing interval, fixed macro usage,
                   removed expiration parameter.
10/14/03   ka      Added callback-based flexible logging support in DMSS or FTM.
09/16/03   wd      Updated FTM log id to LOG_FTM_VER_2_C.
09/12/03   wd      Initial revision.

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include "customer.h"
#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"

#ifdef FTM_HAS_LOGGING
#include "comdef.h"
#include "target.h"

#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "ctype.h"  /*lint -esym(766,ctype.h)*/

#include "ftm.h"
#include "rf_test_task_v.h"
#include "ftm_log.h"

#ifdef FTM_HAS_UMTS
#include "ftm_wcdma_log.h"
#endif

#ifdef FEATURE_LTE
#include "ftm_lte_log.h"
#endif

#ifdef FTM_HAS_TDSCDMA
#include "ftm_tdscdma_log.h"
#endif /*FTM_HAS_TDSCDMA*/


#ifdef FEATURE_CDMA1X
#include "ftm_1x_log.h"
#include "ftm_hdr_log.h"
#include "ftm_cdma_log.h"
#endif

#ifdef FEATURE_FTM_HWTC
#include "ftm_hwtc_dc.h"
#endif

#include "log.h"
#include "log_codes.h"
#include "time_svc.h"   /* Replaces clk.h*/
#include "time_tod.h"   /* Replaces clk.h*/
#include "timer.h"
#include "DALSys.h"

#include "ftm_common_fbrx.h"
/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/


/*===========================================================================

                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/
#define FTM_LOG_MAX_BUFFERS 100

/*
 *  This table defines the configuration for each log id.  Any id's that use
 *  a periodic callback should be initialized below.  There is no requirement
 *  on the log id's being in a certain order, or numbered in a continous fashion
 *  (can use log id entries of (9,2,32768,7), rather than (1,2,3,4))
 *
 *  Any log id that has a callback function or entry function initialized here needs to have the
 *  active flag initialized to FALSE.  These log id's need to be initialized
 *  thru the ftm_log_enable interface so the callback intervals can be defined
 *  and init functions called.  Functions that have NULL for the func_cb AND
 *  entry/exit functions can be set as active = TRUE here, since they require no dynamic
 *  memory allocaiton.
 */
#ifndef FTM_HAS_UMTS
ftm_log_config_table_entry        ftm_log_config_table[FTM_LOG_MAX] =
#else
ftm_log_config_table_entry        ftm_log_config_table[] =
#endif
{
  /* Log ID                    clk_cb,    func_cb,           active, num_buffers  entry_func, exit_func */
  #ifdef FEATURE_LTE
  { FTM_LOG_LTE_TX_AGC,        { NULL,    ftm_log_lte_tx_agc_cb, FALSE,  0 },   ftm_log_lte_tx_agc_init_func, ftm_log_lte_tx_agc_exit_func },
  { FTM_LOG_LTE_RX_AGC,        { NULL,    ftm_log_lte_rx_agc_cb, FALSE,  0 },   ftm_log_lte_rx_agc_init_func, ftm_log_lte_rx_agc_exit_func },
  { FTM_LOG_LTE_TXPLIM,        { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },
  { FTM_LOG_LTE_TX_AGC_ENH,    { NULL,    ftm_log_lte_tx_agc_enh_cb,  FALSE,  0 },   ftm_log_lte_tx_agc_enh_init_func, ftm_log_lte_tx_agc_enh_exit_func },
  { FTM_LOG_LTE_RX_AGC_ENH,    { NULL,    ftm_log_lte_ca_enabled_rx_agc_cb,  FALSE,  0 },   ftm_log_lte_ca_enabled_rx_agc_init_func, ftm_log_lte_ca_enabled_rx_agc_exit_func },
  #endif /* FEATURE_FTM_HAS_LTE */

  #ifdef FEATURE_WCDMA
  { FTM_LOG_WCDMA_AGC,           { NULL,    NULL,              FALSE,      0 },   NULL,                   NULL                   },
  { FTM_LOG_WCDMA_AGC_ENH,       { NULL,    ftm_log_wcdma_agc_enh_cb,FALSE,     0 },   ftm_log_wcdma_agc_enh_init_func, ftm_log_wcdma_agc_enh_exit_func },
  { FTM_LOG_WCDMA_BER,           { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },
  #endif
  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  { FTM_LOG_SE_BER,              { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },
  #endif

  #ifdef FEATURE_WCDMA
  #ifdef FEATURE_WCDMA_RX_DIVERSITY
  { FTM_LOG_WCDMA_AGC_C1,        { NULL,    ftm_log_wcdma_agc_c1_cb, FALSE,     0 },   ftm_log_wcdma_agc_c1_init_func,  ftm_log_wcdma_agc_c1_exit_func },
  #endif
  { FTM_LOG_WCDMA_TXPLIM,        { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },
  #endif /* FTM_HAS_WCDMA */

  #ifdef FEATURE_GSM
  { FTM_LOG_GSM_BER,             { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },
  { FTM_LOG_AUTOCAL,             { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },
  { FTM_LOG_GSM,                 { NULL,    NULL,              FALSE,       0 },   NULL,                   NULL                   },
  { FTM_LOG_GSM_ENH,             { NULL,    NULL,              FALSE,       0 },   NULL,                   NULL                   },
  #endif /* FTM_HAS_GSM */

  #ifdef FEATURE_WCDMA
  #ifdef FEATURE_FTM_HWTC
  { FTM_LOG_DACC_ACCUM_C0,       { NULL,    ftm_log_dc_cb,    FALSE,      0 },   NULL,                   NULL                   },
  #endif /* FEATURE_FTM_HWTC */
  #endif /* FTM_HAS_WCDMA */

  #ifdef FTM_HAS_TDSCDMA
  { FTM_LOG_TDSCDMA_RX_AGC,           { NULL,    ftm_log_tdscdma_rx_agc_cb, FALSE,  0 },   ftm_log_tdscdma_rx_agc_init_func, ftm_log_tdscdma_rx_agc_exit_func },
  { FTM_LOG_TDSCDMA_TX_AGC,           { NULL,    ftm_log_tdscdma_tx_agc_cb, FALSE,  0 },   ftm_log_tdscdma_tx_agc_init_func, ftm_log_tdscdma_tx_agc_exit_func },
  { FTM_LOG_TDSCDMA_BER,           { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },

  { FTM_LOG_TDSCDMA_TXPLIM,        { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },
  #endif /* FTM_HAS_TDSCDMA */

  #ifdef FEATURE_CDMA1X
  { FTM_LOG_1X_AGC ,             { NULL,    ftm_1x_log_primary_agc_cb, FALSE,     0 },   ftm_1x_log_primary_agc_init_func,    ftm_1x_log_primary_agc_exit_func },
  #ifdef FEATURE_HDR
  { FTM_LOG_HDR_AGC ,            { NULL,    ftm_hdr_log_primary_agc_cb, FALSE,    0 },   ftm_hdr_log_primary_agc_init_func,    ftm_hdr_log_primary_agc_exit_func },
  { FTM_LOG_1X_AGC_C1,           { NULL,    ftm_1x_log_diversity_agc_cb, FALSE,  0 },   ftm_1x_log_diversity_agc_init_func, ftm_1x_log_diversity_agc_exit_func },
  { FTM_LOG_HDR_AGC_C1,          { NULL,    ftm_hdr_log_diveristy_agc_cb, FALSE, 0 },   ftm_hdr_log_diversity_agc_init_func, ftm_hdr_log_diversity_agc_exit_func },
  #endif /* FEATURE_HDR */
  #endif /* FEATURE_CDMA1X*/


  #if defined(FEATURE_CDMA1X)
  { FTM_LOG_INTELLICEIVER,       {NULL,     ftm_cdma_log_c0_intelliceiver_cb,FALSE,0 },  NULL,                   NULL                   },
  #endif
  #ifdef FTM_HAS_COMMON_COMMANDS
  { FTM_LOG_TX_RX_SWEEP,         { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },
  #endif
  { FTM_LOG_IRAT_TIMING,         { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },
  { FTM_LOG_PRINTF,              { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },
  { FTM_LOG_GSM_THERM_VBATT,     { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },
  { FTM_LOG_ICI_CAL_DATA,        { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },

  { FTM_LOG_ATUNER_RL_DATA,      { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },
  { FTM_LOG_ATUNER_SCENARIO_DATA,{ NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },

  { FTM_LOG_SEQUENCER,           { NULL,    NULL,              TRUE,       0 },   NULL,                   NULL                   },
  { FTM_LOG_FBRX_RESULTS,      { NULL,    ftm_common_fbrx_log_results_cb, FALSE,  0 },   ftm_common_fbrx_log_results_init_func, ftm_common_fbrx_log_results_exit_func }
};


/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/




/*===========================================================================

FUNCTION     FTM_LOG_FIND_LOG_ID

DESCRIPTION
             This function finds a particular log id in the log config table

DEPENDENCIES
   None.

RETURN VALUE
             pointer to table entry if log id is found. Else returns
             NULL.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_log_config_table_entry*   ftm_log_find_log_id( ftm_log_id_type   ftm_log_id)
{
  int i;
  ftm_log_config_table_entry     *table_ptr = NULL;

  for( i = 0; i < FTM_LOG_ELEMENT_COUNT( ftm_log_config_table); i++)
  {
    if( ftm_log_config_table[i].log_id == ftm_log_id)
    {
      table_ptr = &ftm_log_config_table[i];
      break;
    }
  }

  return( table_ptr);

}




/*===========================================================================

FUNCTION     FTM_IS_LOG_ENABLED

DESCRIPTION
             This function checks if a particular log id is active

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
boolean ftm_is_log_enabled( ftm_log_id_type  ftm_log_id)
{

  ftm_log_config_table_entry   *table_ptr;

  table_ptr = ftm_log_find_log_id(ftm_log_id);

  if( table_ptr == NULL)
  {
    return FALSE;
  }
  else
  {
    return table_ptr->log_config.active;
  }
}

/*===========================================================================

FUNCTION FTM_LOG_MALLOC

DESCRIPTION
   This function allocates memory for an ftm lok pkt.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
PACKED void * ftm_log_malloc (uint16 ftm_log_id, unsigned int length)
{
  ftm_log_gen_type *ftm_log_pkt = NULL;

  if ( ftm_is_log_enabled( (ftm_log_id_type)ftm_log_id) && log_status( LOG_FTM_VER_2_C ))
  {
    ftm_log_pkt = (ftm_log_gen_type*) log_alloc( LOG_FTM_VER_2_C, length);

    if (ftm_log_pkt != NULL)
    {
      ftm_log_pkt->ftm_log_id = (word)ftm_log_id;
    }
    else
    {
      FTM_MSG_ERROR("Unable to allocate ftm log memory",0,0,0);
    }
  }

  return ( ( void*) ftm_log_pkt);
} /* End ftm_log_malloc */



/*===========================================================================

FUNCTION FTM_LOG_ENABLE

DESCRIPTION
   This function enables FTM logging for a particular log id.
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Allocates and initializes memory for log array structure if necessary.
   Registers callback function for given log id.

===========================================================================*/
void ftm_log_enable(ftm_log_id_type log_id, int4 interval, int num_buffer)
{


  ftm_log_config_table_entry     *log_config_ptr;

  log_config_ptr = ftm_log_find_log_id( log_id);

  if( log_config_ptr == NULL)
  {
      return;
  }


  log_config_ptr->log_config.num_buffer = ((num_buffer > 0 && num_buffer < FTM_LOG_MAX_BUFFERS) ? num_buffer : 1);

  if( log_config_ptr->log_config.active == FALSE)
  {
    /* Make sure this is a log that requires a callback function */

    if( log_config_ptr->log_config.cb_func != NULL)
    {
         /* Enable callback for log_id */
         log_config_ptr->log_config.cb_struct = (timer_type*)ftm_malloc(sizeof(timer_type));
         if (log_config_ptr->log_config.cb_struct == NULL) {
           FTM_MSG_ERROR("Failed to allocate memory for callback structure", 0, 0, 0);
           return;
         }
         timer_def2(log_config_ptr->log_config.cb_struct, NULL);
         timer_reg( log_config_ptr->log_config.cb_struct,
                    (timer_t2_cb_type)log_config_ptr->log_config.cb_func,
                    (timer_cb_data_type)NULL,
                 interval,
                    interval);
    }

    /* Set log to active status */

    log_config_ptr->log_config.active = TRUE;

    /* Call the log startup function, if available */

    if( log_config_ptr->log_init_func != NULL)
    {
        log_config_ptr->log_init_func();
    }
  }
  else
  {
    /* Make sure this is a log that requires a callback function */

    if( log_config_ptr->log_config.cb_func != NULL)
    {
        /* log is already active, dereg then register w/ new interval */
        (void) timer_clr(log_config_ptr->log_config.cb_struct, T_NONE);
        timer_reg( log_config_ptr->log_config.cb_struct,
                   (timer_t2_cb_type)log_config_ptr->log_config.cb_func,
                   (timer_cb_data_type)NULL,
                interval,
                   interval);
    }
  }
} /* End ftm_log_enable */

/*===========================================================================

FUNCTION FTM_LOG_DISABLE

DESCRIPTION
   This function disables FTM logging for a particular log id.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Deregisters the callback for the given log id.
   Deallocates the log array from the heap if no logs remain active.

===========================================================================*/
void ftm_log_disable(ftm_log_id_type log_id)
{

  ftm_log_config_table_entry     *log_config_ptr;

  log_config_ptr = ftm_log_find_log_id( log_id);


  /* Return if invalid log id */
  if( log_config_ptr == NULL)
  {
    return;
  }

  /* Disable log if active */

  if(log_config_ptr->log_config.active)
  {

    if( log_config_ptr->log_config.cb_struct != NULL)
    {
        (void) timer_clr(log_config_ptr->log_config.cb_struct, T_NONE);
        ftm_free(log_config_ptr->log_config.cb_struct);

    }

    log_config_ptr->log_config.active = FALSE;


    /* Call the log's exit function if available */

    if( log_config_ptr->log_exit_func != NULL)
    {
        log_config_ptr->log_exit_func();
    }
  }
} /* End ftm_log_disable */

/*===========================================================================

FUNCTION FTM_LOG_DISPATCH

DESCRIPTION
   This function handles requests to run tests and  other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_log_dispatch(ftm_log_req_pkt_type  *cmd_ptr)
{

  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

  FTM_MSG_MED("FTM Log dispatching packet id: %d",cmd_ptr->ftm_hdr.cmd_id,0,0); /*lint !e778*/

  switch(cmd_ptr->ftm_hdr.cmd_id)
  {
  case FTM_LOG_ENABLE:
    ftm_log_enable((ftm_log_id_type) cmd_ptr->params.log_id,
                   cmd_ptr->params.interval,
                   cmd_ptr->params.num_buffer);
    break;

  case FTM_LOG_DISABLE:
    ftm_log_disable((ftm_log_id_type) cmd_ptr->params.log_id);
    break;

  default:
    FTM_MSG_ERROR("Unknown FTM Log Command",0,0,0);
    rsp_pkt.cmd = FTM_RSP_BAD_CMD;
    break;
  }

  return rsp_pkt;

} /* End ftm_log_dispatch */

#endif /* FTM_HAS_LOGGING */
#endif /* FEATURE_FACTORY_TESTMODE */

