/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              FTM Log

GENERAL DESCRIPTION
  This is the header file for FTM Logging.

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/ftm/src/ftm_tdscdma_log.c#1 $
  $DateTime: 2016/12/13 07:59:50 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/30/13   qzh     Replace memcpy() with safer memscpy().
02/27/13   ms      Temporary change in tx and rx agc cb till merge is done
07/16/12   hy      Add TD-SCDMA FTM Rx AGC and Tx AGC logs. 

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include "customer.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"

#ifdef FTM_HAS_TDSCDMA
#ifdef FTM_HAS_LOGGING
#include "comdef.h"
#include "target.h"

#include "string.h"
#include <stringl/stringl.h>
#include "stdarg.h"
#include "stdio.h"
#include "ctype.h"  /*lint -esym(766,ctype.h)*/
#include "ftm.h"
#include "rf_test_task_v.h"
#include "ftm_log.h"
#include "ftm_tdscdma_log.h"
#include "msm.h"
#include "rf_tdscdma_msm.h"
#include "rfnv_items.h"
#include "rftxplim.h"
#include "rf_tdscdma_mdsp.h"
#include "rf_tdscdma_core_temp_comp.h"
#include "ftm_tdscdma_dispatch.h"
#include "rf_tdscdma_core_txplim.h"
#include "rf_tdscdma_mc.h"
#include "ftm_tdscdma_ctl.h"
#include "rf_tdscdma_mdsp_types.h"
#include "rf_tdscdma_mdsp_smem_intf.h"
#include "rf_tdscdma_mdsp_util.h"

/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/


/*===========================================================================

                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/

static ftm_log_tdscdma_rx_agc_data_type *ftm_rx_agc_tdscdma_data=NULL;
static ftm_log_tdscdma_tx_agc_data_type *ftm_tx_agc_tdscdma_data=NULL;

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/
extern ftm_log_config_table_entry*   ftm_log_find_log_id( ftm_log_id_type   ftm_log_id);
/*===========================================================================

FUNCTION

DESCRIPTION


DEPENDENCIES


RETURN VALUE
   None.

SIDE EFFECTS
.

===========================================================================*/
void ftm_log_tdscdma_rx_agc_init_func( void)
{

  ftm_log_config_table_entry         *log_config_ptr;


  log_config_ptr = ftm_log_find_log_id( FTM_LOG_TDSCDMA_RX_AGC);

  /* must allocate an internal buffer to collect the data */
  if ( log_config_ptr != NULL )
  {
     ftm_rx_agc_tdscdma_data =
          (ftm_log_tdscdma_rx_agc_data_type *)ftm_malloc (log_config_ptr->log_config.num_buffer
                                               * sizeof (ftm_log_tdscdma_rx_agc_data_type)); /*lint !e737 suppress int to unsigned int */
  }
  else
  {
    ftm_rx_agc_tdscdma_data = NULL;
  }
  /* check that we have memory to work with */
  if (ftm_rx_agc_tdscdma_data == NULL)
  {
    FTM_MSG_ERROR("Unable to allocate Rx AGC data buffer for TDSCDMA",0,0,0);
  }
  else
   {
     memset(ftm_rx_agc_tdscdma_data, 0, log_config_ptr->log_config.num_buffer
                                               * sizeof (ftm_log_tdscdma_rx_agc_data_type));

   }
}


/*===========================================================================

FUNCTION

DESCRIPTION


DEPENDENCIES


RETURN VALUE
   None.

SIDE EFFECTS
.

===========================================================================*/
void ftm_log_tdscdma_rx_agc_exit_func( void)
{
  if( ftm_rx_agc_tdscdma_data != NULL)
  {
    ftm_free((void*)ftm_rx_agc_tdscdma_data); /* free the internal buffer */
    ftm_rx_agc_tdscdma_data = NULL;
  }
}


/*===========================================================================

FUNCTION FTM_LOG_TDSCDMA_RX_AGC_CB

DESCRIPTION
   This function reads the registers required for Rx AGC logging support
   and returns them in an FTM log packet.

DEPENDENCIES
   Callback function, must be enabled with ftm_log_enable.

RETURN VALUE
   None.

SIDE EFFECTS
   Generates FTM log pkt with results.

===========================================================================*/
void ftm_log_tdscdma_rx_agc_cb(int4 interval)
{
  static int buf              = 0;
  ftm_log_tdscdma_rx_agc_type  *rx_agc_log  = NULL;
  ftm_log_config_table_entry         *log_config_ptr;

  if ( (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX)
       || (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RX) )
  {
    log_config_ptr = ftm_log_find_log_id( FTM_LOG_TDSCDMA_RX_AGC);

    if( log_config_ptr == NULL )
    {
      FTM_MSG_ERROR("NULL pointer obtained for log id %d ",FTM_LOG_TDSCDMA_RX_AGC,0,0);
      return;
    }

    /* Get log from FW and to have them with right data format */
    rf_tdscdma_mdsp_get_rf_rx_agc_log( &(ftm_rx_agc_tdscdma_data[buf]) );

    if ( ++buf >= log_config_ptr->log_config.num_buffer )
    {
      rx_agc_log = (ftm_log_tdscdma_rx_agc_type *)ftm_log_malloc( (uint16)FTM_LOG_TDSCDMA_RX_AGC, 
                                                    sizeof (ftm_log_tdscdma_rx_agc_type)+ ( log_config_ptr->log_config.num_buffer - 1 )
                                                    * sizeof( ftm_log_tdscdma_rx_agc_data_type) ); /*lint !e737 suppres int to unsigned int*/
      if ( rx_agc_log != NULL )
      {
        /* have to copy the internal buffer to the suite_log packet, we cannot
        simply accumulate the data in the suite_log packet directly since any memory
        allocated with ftm_log_malloc must be passed to log_commit soon afterward
        (where "soon" is apparently less than tens of ms) */
        memscpy( ( void*) &( rx_agc_log->data), 
                sizeof(ftm_log_tdscdma_rx_agc_data_type),
                ( void*) ftm_rx_agc_tdscdma_data, 
                log_config_ptr->log_config.num_buffer * sizeof( ftm_log_tdscdma_rx_agc_data_type)); /*lint !e737 suppres int to unsigned int*/

        /* send log */
        log_commit( ( ftm_log_type*) rx_agc_log );
      }
      buf = 0;
    }
  }
}




/*===========================================================================

FUNCTION       FTM_LOG_TX_AGC_INIT_FUNC

DESCRIPTION


DEPENDENCIES


RETURN VALUE
   None.

SIDE EFFECTS
.

===========================================================================*/
void ftm_log_tdscdma_tx_agc_init_func( void)
{

  ftm_log_config_table_entry         *log_config_ptr;


  log_config_ptr = ftm_log_find_log_id( FTM_LOG_TDSCDMA_TX_AGC);

  if( log_config_ptr == NULL )
  {
    FTM_MSG_ERROR("NULL pointer obtained for log id %d ",FTM_LOG_TDSCDMA_TX_AGC,0,0);
    return;
  }

  /* must allocate an internal buffer to collect the data */
  ftm_tx_agc_tdscdma_data =
    (ftm_log_tdscdma_tx_agc_data_type *)ftm_malloc (log_config_ptr->log_config.num_buffer
                                                  * sizeof (ftm_log_tdscdma_tx_agc_data_type)); /*lint !e737 suppress int to unsigned int */
  
  /* check that we have memory to work with */
  if (ftm_tx_agc_tdscdma_data == NULL)
  {
    FTM_MSG_ERROR("Unable to allocate Tx AGC data buffer",0,0,0);
  }
  else
  {
     memset(ftm_tx_agc_tdscdma_data, 0, log_config_ptr->log_config.num_buffer
                                               * sizeof (ftm_log_tdscdma_tx_agc_data_type));

  }
}


/*===========================================================================

FUNCTION     FTM_LOG_TX_AGC_EXIT_FUNC

DESCRIPTION


DEPENDENCIES


RETURN VALUE
   None.

SIDE EFFECTS
.

===========================================================================*/
void ftm_log_tdscdma_tx_agc_exit_func( void)
{
  if( ftm_tx_agc_tdscdma_data != NULL)
  {
    ftm_free((void*)ftm_tx_agc_tdscdma_data); /* free the internal buffer */
    ftm_tx_agc_tdscdma_data = NULL;
  }
}

/*===========================================================================

FUNCTION FTM_LOG_TX_AGC_CB

DESCRIPTION
   This function reads the registers required for AGC C1 logging support
   and returns them in an FTM log packet.

DEPENDENCIES
   Callback function, must be enabled with ftm_log_enable.

RETURN VALUE
   None.

SIDE EFFECTS
   Generates FTM log pkt with results.

===========================================================================*/
void ftm_log_tdscdma_tx_agc_cb(int4 interval)
{
  static int buf              = 0;
  ftm_log_tdscdma_tx_agc_type  *tx_agc_log  = NULL;

  ftm_log_config_table_entry         *log_config_ptr;

  log_config_ptr = ftm_log_find_log_id( FTM_LOG_TDSCDMA_TX_AGC);

  if( log_config_ptr == NULL )
  {
    FTM_MSG_ERROR("NULL pointer obtained for log id %d ", FTM_LOG_TDSCDMA_TX_AGC,0,0);
    return;
  }

  if (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX)
  {
     /* Get log from FW and to have them with right data format */
     rf_tdscdma_mdsp_get_rf_tx_agc_log( &(ftm_tx_agc_tdscdma_data[buf]) );

     if (++buf >= log_config_ptr->log_config.num_buffer)
     {

        tx_agc_log = (ftm_log_tdscdma_tx_agc_type *)
           ftm_log_malloc( (uint16) FTM_LOG_TDSCDMA_TX_AGC,
                           sizeof (ftm_log_tdscdma_tx_agc_type)
                           + ( log_config_ptr->log_config.num_buffer - 1 )
                           * sizeof( ftm_log_tdscdma_tx_agc_data_type) ); /*lint !e737 suppres int to unsigned int*/
        if ( tx_agc_log != NULL)
        {
           /* have to copy the internal buffer to the suite_log packet, we cannot
              simply accumulate the data in the suite_log packet directly since any memory
              allocated with ftm_log_malloc must be passed to log_commit soon afterward
              (where "soon" is apparently less than tens of ms) */
           memscpy( ( void*) &( tx_agc_log->data),
                   sizeof( ftm_log_tdscdma_tx_agc_data_type),
                   ( void*) ftm_tx_agc_tdscdma_data,
                   log_config_ptr->log_config.num_buffer * sizeof( ftm_log_tdscdma_tx_agc_data_type)); /*lint !e737 suppres int to unsigned int*/

           /* send log */
           log_commit( ( ftm_log_type*) tx_agc_log );
        }

        buf = 0;
     }
  }
}


/*===========================================================================

FUNCTION       ftm_log_tdscdma_txplim_data

DESCRIPTION


DEPENDENCIES


RETURN VALUE
   None.

SIDE EFFECTS
.

===========================================================================*/
void ftm_log_tdscdma_txplim_data( ftm_log_tdscdma_txplim_data_type *data)
{

    ftm_log_tdscdma_txplim_type         *log_ptr;

    /* must allocate an internal buffer to collect the data */
    log_ptr = (ftm_log_tdscdma_txplim_type *)ftm_log_malloc( (uint16)FTM_LOG_TDSCDMA_TXPLIM,
                                                      sizeof( ftm_log_tdscdma_txplim_type));/* check that we have memory to work with */

    if( log_ptr != NULL)
    {
      log_ptr->data = *data;
      log_commit( log_ptr);
    }
}

#endif /* FTM_HAS_LOGGING */
#endif /* FTM_HAS_TDSCDMA */
#endif /* FEATURE_FACTORY_TESTMODE */

