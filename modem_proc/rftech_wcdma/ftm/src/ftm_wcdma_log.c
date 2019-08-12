/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              FTM Log

GENERAL DESCRIPTION
  This is the header file for FTM Logging.

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/ftm/src/ftm_wcdma_log.c#1 $
  $DateTime: 2016/12/13 07:59:54 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/14   kr      Fix crash in FTM log packet. 
02/14/14   kai     Add pin/pout logging
02/04/14   kai     Add xpt logging support
12/05/13   bn      Add IQ gain logging support
10/28/13   dw      Add LNA state logging support
09/10/13   dw      Remove DBDC featurization and code clean up.
08/01/13   kr       replace memcpy with memscpy
07/19/13   ac      changes related to mdsp for dbdc
11/14/12   dw      Change logging to get PA bias from FW
08/17/12   tks     Added necessary comments  
04/04/12   jr      Ensure WCDMA AGC logging is not colliding with log disable operation
10/04/11   dw      Change APT logging for backwards compatibility
09/06/11   dw      Added APT logging support
06/22/11   sar     Removed deprecated files rftx.h and rfagc.h.
10/19/10   vb      Removed rfdevice_interface.h
07/19/10   ka      Removed warnings
06/08/10   ka      Klocwork fixes.
03/16/10   kguo    Merged from scmm_final_merge branch
01/28/10   sar     Removed deprecated file msmhwio.h.
01/25/10   vb      Lint error fixes
01/12/10   xw      Included rfwcdma_core_txplim.h to fix compiler warning
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
01/05/10   bn      Bug fix in WCDMA ENH log packet for WCDMA only builds
12/10/09   bn      FTM WCDMA ENH log packet
10/20/09   ckl     Fixed incorrect Rx AGC report in diversity chain
10/15/09   ckl     Fixed wrong LNA gain state report in diversity chain
10/12/09   ckl     Fix crash with WCDMA Rx Div AGC Log request
10/02/09   sar     Removed DDIAdc.h.
09/18/09   sar     Updated ADC interface for CMI conformance.
09/09/09   pl      Remove FEATURE_FTM_HAS_LTE
08/18/09   ka      Moved AGC_CTL_3 access to HAL.
08/11/09   mkv     Merge MODEM_LTE_DEV into rfa2
07/06/09   ckl     Fixed incorrect Rx error value.
05/19/08   ad      Fix fatal error crash on ADS compilers due to overcasting pointers
11/13/07   ycl     Modified temperature/HDET logging to read from buffered values.
08/08/07   jfc     Mainlined T_MSM6280 feature
02/06/07   ycl     Added ftm_log_wcdma_txplim_data() function.
11/16/06   vm/jfc  Allow FTM logging to be mode independent
10/17/06   ycl     Modifications for initial MSM7600 build.
06/26/06   xw      Added rf state checking in FTM AGC LOG.
06/19/06   jac     Fixed LINT error.
06/15/06   ka      Added support for internal thermistor.
01/18/06   dp      Add WCDMA diversity support.
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

#ifdef FTM_HAS_UMTS
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
#include "ftm_wcdma_log.h"
#include "rfwcdma_msm.h"
#include "nv_items.h"
#include "rftxplim.h"
#include "rfumts.h"
#include "rfwcdma_mdsp.h"
#include "rfwcdma_core_temp_comp.h"
#include "ftm_wcdma_dispatch.h"
#include "rfwcdma_core_txplim.h"
#include "rfwcdma_core_util.h"
#include "rfm_wcdma.h"
#include "ftm_common_locks.h"

#ifdef FEATURE_FTM_HWTC
#include "ftm_hwtc_dc.h"
#endif
/* memscpy */
#include "stringl.h"

#define THERM_SCALED_MIN   0
#define THERM_SCALE      255

// duplicate of define in /drivers/rf/rfi.c
#define FTM_LOG_TEMP_BIN_SIZ    (THERM_SCALE / (NV_TEMP_TABLE_SIZ-1))


/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/
//extern uint16 rfi_read_therm_being_used, rfi_read_therm_raw_being_used;
extern rfwcdma_core_temp_comp_value_type rfwcdma_core_temp_comp_value;
extern boolean rfm_is_sleeping( rfcom_device_enum_type device );
/*! @todo (tks) Should be replaced with QPOET device calls */
extern uint16 rfwcdma_msm_get_smps_pa_bias_val( void );
extern int16 rfwcdma_core_txplim_get_hdet(void);
extern uint16 rfwcdma_core_txplim_get_hdet_adc(void);


/*===========================================================================

                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/

ftm_log_wcdma_agc_data_type *ftm_agc_wcdma_data=NULL;
#ifdef FEATURE_WCDMA_RX_DIVERSITY
ftm_log_wcdma_agc_c1_data_type *ftm_agc_wcdma_c1_data=NULL;
#endif
ftm_log_wcdma_agc_enh_data_type *ftm_agc_wcdma_enh_data=NULL;

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
void ftm_log_wcdma_agc_init_func( void)
{

  ftm_log_config_table_entry         *log_config_ptr;


  log_config_ptr = ftm_log_find_log_id( FTM_LOG_WCDMA_AGC);

  ftm_common_enter_critical_section();

  /* must allocate an internal buffer to collect the data */
  if ( log_config_ptr != NULL )
  {
  ftm_agc_wcdma_data =
    (ftm_log_wcdma_agc_data_type *)ftm_malloc (log_config_ptr->log_config.num_buffer
                                               * sizeof (ftm_log_wcdma_agc_data_type)); /*lint !e737 suppress int to unsigned int */
  }
  else
  {
    ftm_agc_wcdma_data = NULL;
  }
  /* check that we have memory to work with */
  if (ftm_agc_wcdma_data == NULL)
  {
    FTM_MSG_ERROR("Unable to allocate AGC data buffer for UMTS",0,0,0);
  }

  ftm_common_leave_critical_section();
}

/*===========================================================================

FUNCTION  FTM_LOG_WCDMA_AGC_ENH_INIT_FUNC

DESCRIPTION
   Function to initalize the wcdma enh log packet

DEPENDENCIES


RETURN VALUE
   None.

SIDE EFFECTS
.

===========================================================================*/
void ftm_log_wcdma_agc_enh_init_func( void)
{

    ftm_log_config_table_entry         *log_config_ptr;


    log_config_ptr = ftm_log_find_log_id( FTM_LOG_WCDMA_AGC_ENH);

    if( log_config_ptr == NULL )
    {
      FTM_MSG_ERROR("NULL pointer obtained for log id %d ",FTM_LOG_WCDMA_AGC_ENH,0,0);
      return;
    }

    ftm_common_enter_critical_section();


    /* must allocate an internal buffer to collect the data */
    ftm_agc_wcdma_enh_data = (ftm_log_wcdma_agc_enh_data_type *)ftm_malloc (log_config_ptr->log_config.num_buffer
                                                    * sizeof (ftm_log_wcdma_agc_enh_data_type)); /*lint !e737 suppress int to unsigned int */
    /* check that we have memory to work with */
    if (ftm_agc_wcdma_enh_data == NULL)
    {
         FTM_MSG_ERROR("Unable to allocate AGC data buffer",0,0,0);
    }

    ftm_common_leave_critical_section();
}

/*===========================================================================

FUNCTION       ftm_log_wcdma_txplim_data

DESCRIPTION


DEPENDENCIES


RETURN VALUE
   None.

SIDE EFFECTS
.

===========================================================================*/
void ftm_log_wcdma_txplim_data( ftm_log_wcdma_txplim_data_type *data)
{

    ftm_log_wcdma_txplim_type         *log_ptr;

    /* must allocate an internal buffer to collect the data */
    log_ptr = (ftm_log_wcdma_txplim_type *)ftm_log_malloc( (uint16)FTM_LOG_WCDMA_TXPLIM,
                                                      sizeof( ftm_log_wcdma_txplim_type));/* check that we have memory to work with */

    if( log_ptr != NULL)
    {
      log_ptr->data = *data;
      log_commit( log_ptr);
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
void ftm_log_wcdma_agc_exit_func( void)
{

  ftm_common_enter_critical_section();

  if( ftm_agc_wcdma_data != NULL)
  {
    ftm_free((void*)ftm_agc_wcdma_data); /* free the internal buffer */
  }

  ftm_common_leave_critical_section();

}

/*===========================================================================

FUNCTION  FTM_LOG_WCDMA_AGC_ENH_EXIT_FUNC

DESCRIPTION
   Function to clear and free memory after the wcdma enh log packet use


DEPENDENCIES


RETURN VALUE
   None.

SIDE EFFECTS
.

===========================================================================*/
void ftm_log_wcdma_agc_enh_exit_func( void)
{
    ftm_common_enter_critical_section();

    if( ftm_agc_wcdma_enh_data != NULL)
    {
        ftm_free((void*)ftm_agc_wcdma_enh_data); /* free the internal buffer */
    }

    ftm_common_leave_critical_section();
}


/*===========================================================================

FUNCTION FTM_LOG_WCDMA_AGC_CB

DESCRIPTION
   This function reads the registers required for AGC logging support
   and returns them in an FTM log packet.

DEPENDENCIES
   Callback function, must be enabled with ftm_log_enable.

RETURN VALUE
   None.

SIDE EFFECTS
   Generates FTM log pkt with results.

===========================================================================*/
void ftm_log_wcdma_agc_cb(int4 interval)
{
  static int buf              = 0;
  ftm_log_wcdma_agc_type  *agc_log  = NULL;
  byte therm_scaled           = 0;

  ftm_log_config_table_entry         *log_config_ptr;

  if ( !rfm_is_sleeping(RFCOM_TRANSCEIVER_0) )
  {
    log_config_ptr = ftm_log_find_log_id( FTM_LOG_WCDMA_AGC);

    if( log_config_ptr == NULL )
    {
      FTM_MSG_ERROR("NULL pointer obtained for log id %d ",FTM_LOG_WCDMA_AGC,0,0);
      return;
    }

    ftm_common_enter_critical_section();

    interval = interval;  /* lint */

    ftm_agc_wcdma_data[buf].rx_agc           =
    rfm_get_receive_agc_val((rfcom_device_enum_type)RFAGC_RECEIVE_CHAIN_0,RFCOM_SERVINGCELL_RXAGC_RAW, RFCOM_RXAGC_IN_DB10);
    ftm_agc_wcdma_data[buf].tx_agc           = rfwcdma_core_util_get_rflm_txagc_dB10();

    ftm_agc_wcdma_data[buf].tx_agc_adj   = (uint16) rfwcdma_core_util_get_rflm_rgi();

    ftm_agc_wcdma_data[buf].lna_state        = rfwcdma_core_util_get_lna_state(RFM_DEVICE_0);

    ftm_agc_wcdma_data[buf].pa_state         = rfwcdma_core_util_get_rflm_pa_range();

    ftm_agc_wcdma_data[buf].hdet_raw         = (uint8) rfwcdma_core_txplim_get_hdet_adc();
    therm_scaled  =  rfwcdma_core_temp_comp_value.last_scaled_temp_value;
    ftm_agc_wcdma_data[buf].therm_raw        =  rfwcdma_core_temp_comp_value.last_raw_temp_value;


    ftm_agc_wcdma_data[buf].therm_scaled     = therm_scaled;
    ftm_agc_wcdma_data[buf].temp_comp_index  = therm_scaled / FTM_LOG_TEMP_BIN_SIZ;
    ftm_agc_wcdma_data[buf].temp_comp_rem    = therm_scaled % FTM_LOG_TEMP_BIN_SIZ;


    if ( ++buf >= log_config_ptr->log_config.num_buffer )
    {
      agc_log = (ftm_log_wcdma_agc_type *)ftm_log_malloc( (uint16)FTM_LOG_WCDMA_AGC,
                                                    sizeof (ftm_log_wcdma_agc_type)+ ( log_config_ptr->log_config.num_buffer - 1 )
                                                    * sizeof( ftm_log_wcdma_agc_data_type) ); /*lint !e737 suppres int to unsigned int*/
      if ( agc_log != NULL )
      {
        /* have to copy the internal buffer to the suite_log packet, we cannot
        simply accumulate the data in the suite_log packet directly since any memory
        allocated with ftm_log_malloc must be passed to log_commit soon afterward
        (where "soon" is apparently less than tens of ms) */
        memscpy( ( void*) &( agc_log->data), log_config_ptr->log_config.num_buffer * sizeof( ftm_log_wcdma_agc_data_type),
                ( void*) ftm_agc_wcdma_data,
                log_config_ptr->log_config.num_buffer * sizeof( ftm_log_wcdma_agc_data_type)); /*lint !e737 suppres int to unsigned int*/

        /* send log */
        log_commit( ( ftm_log_type*) agc_log );
      }

      buf = 0;
    }

    ftm_common_leave_critical_section();

  }
}


/*===========================================================================

FUNCTION FTM_LOG_AGC_ENH_CB

DESCRIPTION
   This function reads the registers required for AGC logging support
   and returns them in an FTM log packet.

DEPENDENCIES
   Callback function, must be enabled with ftm_log_enable.

RETURN VALUE
   None.

SIDE EFFECTS
   Generates FTM log pkt with results.

===========================================================================*/
void ftm_log_wcdma_agc_enh_cb(int4 interval)
{
  static int buf              = 0;
  ftm_log_wcdma_agc_enh_type  *agc_enh_log  = NULL;
  byte therm_scaled           = 0;
  rfwcdma_core_util_txagc_logging_type txagc_data;

  ftm_log_config_table_entry         *log_config_ptr;

  if ( !rfm_is_sleeping(RFCOM_TRANSCEIVER_0) )
  {
    log_config_ptr = ftm_log_find_log_id( FTM_LOG_WCDMA_AGC_ENH);

    if( log_config_ptr == NULL )
    {
      FTM_MSG_ERROR("NULL pointer obtained for log id %d ",FTM_LOG_WCDMA_AGC_ENH,0,0);
      return;
    }

    ftm_common_enter_critical_section();

    rfwcdma_core_util_get_rflm_txagc_logging(&txagc_data);

    interval = interval;  /* lint */
    ftm_agc_wcdma_enh_data[buf].rx_agc           =
    rfm_get_receive_agc_val((rfcom_device_enum_type)RFAGC_RECEIVE_CHAIN_0,RFCOM_SERVINGCELL_RXAGC_RAW, RFCOM_RXAGC_IN_DB10);
    ftm_agc_wcdma_enh_data[buf].tx_agc           = rfwcdma_core_util_get_rflm_txagc_dB10();
    ftm_agc_wcdma_enh_data[buf].tx_pdm           = txagc_data.rgi;
    ftm_agc_wcdma_enh_data[buf].mtpl             = rfwcdma_core_txplim_get_mtpl();
    ftm_agc_wcdma_enh_data[buf].iq_gain          = (uint16) txagc_data.iq_gain;
    ftm_agc_wcdma_enh_data[buf].lna_state        = rfwcdma_core_util_get_lna_state(RFM_DEVICE_0);
    ftm_agc_wcdma_enh_data[buf].pa_state         = txagc_data.pa_range;
    ftm_agc_wcdma_enh_data[buf].hdet_raw         = rfwcdma_core_txplim_get_hdet_adc();
    ftm_agc_wcdma_enh_data[buf].therm_raw        = rfwcdma_core_temp_comp_value.last_raw_temp_value;
    therm_scaled = rfwcdma_core_temp_comp_value.last_scaled_temp_value;
    ftm_agc_wcdma_enh_data[buf].therm_scaled     = therm_scaled;
    ftm_agc_wcdma_enh_data[buf].temp_comp_index  = therm_scaled / FTM_LOG_TEMP_BIN_SIZ;
    ftm_agc_wcdma_enh_data[buf].temp_comp_rem    = therm_scaled % FTM_LOG_TEMP_BIN_SIZ;
    /*! @todo (tks) Should be replaced with QPOET device calls */
    ftm_agc_wcdma_enh_data[buf].hdet             = rfwcdma_core_txplim_get_hdet();
    ftm_agc_wcdma_enh_data[buf].smps_bias        = txagc_data.smps_bias;
    ftm_agc_wcdma_enh_data[buf].icq_current      = txagc_data.icq_current;
    ftm_agc_wcdma_enh_data[buf].env_scale        = txagc_data.env_scale;
    ftm_agc_wcdma_enh_data[buf].xpt_mode          = txagc_data.xpt_mode;
    ftm_agc_wcdma_enh_data[buf].dpd_index         = txagc_data.dpd_index;
    ftm_agc_wcdma_enh_data[buf].pin_comp          = txagc_data.pin_comp;
    ftm_agc_wcdma_enh_data[buf].pout_comp         = txagc_data.pout_comp;
    ftm_agc_wcdma_enh_data[buf].xpt_delay         = txagc_data.xpt_delay;


    if ( ++buf >= log_config_ptr->log_config.num_buffer )
    {
      agc_enh_log = (ftm_log_wcdma_agc_enh_type *)ftm_log_malloc( (uint16)FTM_LOG_WCDMA_AGC_ENH,
                                                    sizeof (ftm_log_wcdma_agc_enh_type)+ ( log_config_ptr->log_config.num_buffer - 1 )
                                                    * sizeof( ftm_log_wcdma_agc_enh_data_type) ); /*lint !e737 suppres int to unsigned int*/
      if ( agc_enh_log != NULL )
      {
        /* have to copy the internal buffer to the suite_log packet, we cannot
        simply accumulate the data in the suite_log packet directly since any memory
        allocated with ftm_log_malloc must be passed to log_commit soon afterward
        (where "soon" is apparently less than tens of ms) */
        memscpy( ( void*) &( agc_enh_log->data),log_config_ptr->log_config.num_buffer * sizeof( ftm_log_wcdma_agc_enh_data_type),
                ( void*) ftm_agc_wcdma_enh_data,
                log_config_ptr->log_config.num_buffer * sizeof( ftm_log_wcdma_agc_enh_data_type)); /*lint !e737 suppres int to unsigned int*/

        /* send log */
        log_commit( ( ftm_log_type*) agc_enh_log );
      }
      buf = 0;
    }

    ftm_common_leave_critical_section();

  }
}



#ifdef FEATURE_WCDMA_RX_DIVERSITY
/*===========================================================================

FUNCTION       FTM_LOG_AGC_C1_INIT_FUNC

DESCRIPTION


DEPENDENCIES


RETURN VALUE
   None.

SIDE EFFECTS
.

===========================================================================*/
void ftm_log_wcdma_agc_c1_init_func( void)
{

  ftm_log_config_table_entry         *log_config_ptr;


  log_config_ptr = ftm_log_find_log_id( FTM_LOG_WCDMA_AGC_C1);

  if( log_config_ptr == NULL )
  {
    FTM_MSG_ERROR("NULL pointer obtained for log id %d ",FTM_LOG_WCDMA_AGC_C1,0,0);
    return;
  }

  ftm_common_enter_critical_section();

  /* must allocate an internal buffer to collect the data */
  ftm_agc_wcdma_c1_data =
    (ftm_log_wcdma_agc_c1_data_type *)ftm_malloc (log_config_ptr->log_config.num_buffer
                                                  * sizeof (ftm_log_wcdma_agc_c1_data_type)); /*lint !e737 suppress int to unsigned int */
  /* check that we have memory to work with */
  if (ftm_agc_wcdma_c1_data == NULL)
  {
    FTM_MSG_ERROR("Unable to allocate AGC data buffer",0,0,0);
  }

  ftm_common_leave_critical_section();

}


/*===========================================================================

FUNCTION     FTM_LOG_AGC_EXIT_FUNC

DESCRIPTION


DEPENDENCIES


RETURN VALUE
   None.

SIDE EFFECTS
.

===========================================================================*/
void ftm_log_wcdma_agc_c1_exit_func( void)
{

  ftm_common_enter_critical_section();

  if( ftm_agc_wcdma_c1_data != NULL)
  {
    ftm_free((void*)ftm_agc_wcdma_c1_data); /* free the internal buffer */
  }

  ftm_common_leave_critical_section();

}

/*===========================================================================

FUNCTION FTM_LOG_AGC_C1_CB

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
void ftm_log_wcdma_agc_c1_cb(int4 interval)
{
  static int buf              = 0;
  ftm_log_wcdma_agc_c1_type  *agc_c1_log  = NULL;
  byte therm_scaled           = 0;


  ftm_log_config_table_entry         *log_config_ptr;

  if (!rfm_is_sleeping(RFCOM_TRANSCEIVER_0))
  {

  log_config_ptr = ftm_log_find_log_id( FTM_LOG_WCDMA_AGC_C1);

  if( log_config_ptr == NULL )
  {
    FTM_MSG_ERROR("NULL pointer obtained for log id %d ",FTM_LOG_WCDMA_AGC_C1,0,0);
    return;
  }

  ftm_common_enter_critical_section();

  interval = interval;  /* lint */

  ftm_agc_wcdma_c1_data[buf].rx_agc           =
  rfm_get_receive_agc_val((rfcom_device_enum_type)RFAGC_RECEIVE_CHAIN_0,RFCOM_SERVINGCELL_RXAGC_RAW, RFCOM_RXAGC_IN_DB10);

  ftm_agc_wcdma_c1_data[buf].tx_agc           =  rfwcdma_core_util_get_rflm_txagc_dB10();

  ftm_agc_wcdma_c1_data[buf].tx_agc_adj     = (uint16) rfwcdma_core_util_get_rflm_rgi();

  ftm_agc_wcdma_c1_data[buf].lna_state        = rfwcdma_core_util_get_lna_state(RFM_DEVICE_1);

  ftm_agc_wcdma_c1_data[buf].pa_state         = rfwcdma_core_util_get_rflm_pa_range();

  ftm_agc_wcdma_c1_data[buf].hdet_raw         = (uint8) rfwcdma_core_txplim_get_hdet_adc();

  therm_scaled  =  rfwcdma_core_temp_comp_value.last_scaled_temp_value;
  ftm_agc_wcdma_c1_data[buf].therm_raw        =  rfwcdma_core_temp_comp_value.last_raw_temp_value;

  ftm_agc_wcdma_c1_data[buf].therm_scaled     = therm_scaled;

  ftm_agc_wcdma_c1_data[buf].temp_comp_index  = therm_scaled / FTM_LOG_TEMP_BIN_SIZ;
  ftm_agc_wcdma_c1_data[buf].temp_comp_rem    = therm_scaled % FTM_LOG_TEMP_BIN_SIZ;

  /* Diversity path parameters */
  ftm_agc_wcdma_c1_data[buf].rx_agc_c1        =
  rfm_get_receive_agc_val((rfcom_device_enum_type)RFAGC_RECEIVE_CHAIN_1 ,RFCOM_SERVINGCELL_RXAGC_RAW, RFCOM_RXAGC_IN_DB10);


  ftm_agc_wcdma_c1_data[buf].lna_state_c1     = rfwcdma_core_util_get_lna_state(RFM_DEVICE_1);

  if (++buf >= log_config_ptr->log_config.num_buffer)
  {

    agc_c1_log = (ftm_log_wcdma_agc_c1_type *)ftm_log_malloc( (uint16) FTM_LOG_WCDMA_AGC_C1,
                                                  sizeof (ftm_log_wcdma_agc_c1_type)+ ( log_config_ptr->log_config.num_buffer - 1 )
                                                  * sizeof( ftm_log_wcdma_agc_c1_data_type) ); /*lint !e737 suppres int to unsigned int*/
    if ( agc_c1_log != NULL)
    {
      /* have to copy the internal buffer to the suite_log packet, we cannot
      simply accumulate the data in the suite_log packet directly since any memory
      allocated with ftm_log_malloc must be passed to log_commit soon afterward
      (where "soon" is apparently less than tens of ms) */
      memscpy( ( void*) &( agc_c1_log->data),log_config_ptr->log_config.num_buffer * sizeof( ftm_log_wcdma_agc_c1_data_type),
              ( void*) ftm_agc_wcdma_c1_data,
              log_config_ptr->log_config.num_buffer * sizeof( ftm_log_wcdma_agc_c1_data_type)); /*lint !e737 suppres int to unsigned int*/

      /* send log */
      log_commit( ( ftm_log_type*) agc_c1_log );
    }

    buf = 0;
  }

  ftm_common_leave_critical_section();
}
}
#endif /* FTM_HAS_WCDMA_RX_DIVERSITY */



/*===========================================================================

FUNCTION FTM_LOG_DC_CB

DESCRIPTION
   This function reads the registers related to the DC loop
   and returns them in an FTM log packet.

DEPENDENCIES
   Callback function, must be enabled with ftm_log_enable.

RETURN VALUE
   None.

SIDE EFFECTS
   Generates FTM log pkt with results.

===========================================================================*/
void ftm_log_dc_cb(int4 interval)
{
  interval = interval; /* lint */
  #ifdef FEATURE_FTM_HWTC
  //To do 7600
  //ftm_hwtc_dc_read_regs(FALSE);
  #endif
}

#endif /* FTM_HAS_LOGGING */
#endif /* FTM_HAS_UMTS */
#endif /* FEATURE_FACTORY_TESTMODE */

