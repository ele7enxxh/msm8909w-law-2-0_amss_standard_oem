/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2003 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2007 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2008 - 2014 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
 
                           EDIT HISTORY FOR MODULE
                           
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/ftm/src/ftm_gsm_ber.c#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/24/15   zbz     Fix FTM state when enter GSM NS
11/13/14   ck      add capability to disable/enable gsm diversity 
09/09/14   tsr     DR-DSDS C2/C3 RX calibration support 
07/25/14   par     Stalling FTM task until GL1 sets FTM_GSM_BER_EXIT_CMD_Q_SIG
                   signal so that no FTM process happens  
06/30/14   ec      Remove featurised RxD disable
06/13/14   tsr     Reenable  GSM RX Diversity
05/22/14   tsr     Disable GSM RX Diversity Temporarily  
04/15/14   tsr     Add support for GSM RX Diversity (GL1 Dependant)
09/15/13   jr      Replace ghdi_control_tch_gating_subs with ghdi_control_subs_tch_gating API
08/21/13   tws     Replace FEATURE_TRITON_MODEM with FEATURE_DUAL_SIM
05/07/13   ck      Fix gsm cleanup function to properly stop GSM non-signalling mode
04/16/13   ec      Featurise geran_test_mode apis to use as_id only for Triton 
04/16/13   svi     Replaced memcpy() by safer memscpy().  
04/11/13   ec	     Update geran_test_mode_api calls to use as_id. 
04/03/13   ec	     Pass rfm_dev down through all layers instead of vars_ptr, add 
                   TCH gating for Device 2, ns_override device for Triton NS,
                   remove unused state functions which are not device specific.
                   Map as_id at the geran interface layer in advance of passing in
                   to geran_test_mode_apis.
04/08/13   tws     Use ghdi_control_tch_gating_subs().
02/25/13   tws     Check FTM variables are allocated prior to get/set ber state. 
                   Check all devices for != STATE_STOPPED. 
02/21/13   tws     Fix KW errors.
02/21/13   tws     Allocate the ftm variables in the ber dispatch handler.
02/08/13   tws     Temporarily disable the ghdi_control_tch_gating on DSDA.
02/05/13   ec      Fix Klocwork Errors
02/05/13   tws	   Add vsid parameter to gsdi function call.
02/04/13   ec	     Fix Offtarget Errors
01/29/13   ec	     Add framework for multiple devices in Non Signalling
11/30/12   tws     Remove AS_ID mapping until dependency is fixed.
11/27/12   tws     Add device id & as_id to FTM.
11/23/12   tws     Remove deprecated FEATURE.
11/13/12   ec	     Update to use new geran api for DSDA    
04/13/12   sb      reset RF device to original state after GSM NS stop
03/22/12   dej     Initializing variable req to zero to avoid storing junk values
01/28/12   sar     Feature reduction changes.
01/05/12   sr      geran api changes for non-signaling.
01/05/12   sr      Ported the mem overflow CR fix from 7x30 to NikeL baseline.
11/07/11   ars     Fix for EGPRS Rx Level and SE/BER
06/21/11   aro     Moved ftm_sys_control functionality to ftm_common_control
06/02/11   sr      removed the ftm_rfmode_enter() call during ber start req. 
04/25/11   aro     Renamed FTM RFMODE to FTM State 
03/24/11   aro     Renamed FTM system control filename
03/24/11   aro     Renamed FTM Enter Mode function   
03/02/11   aro     Renamed FTM and FTM-NS mode enums
03/01/11   aro     Moved ftm_gsm_enter() to ftm_sys_ctl.c
11/01/10   ems     Support for FTM Sequencer
08/20/10   tsr     Remove INTLOCK/INTFREE code 
03/19/10   lcl     Pull in fixes from QSC6295
02/04/10   sr      Lint & kw fixes 
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
12/28/09   sr      High/Medium lint errors fix
12/22/09   sr      changed the debug msg macro from legacy to new one.
10/26/09   sr      Compiler warnings fix
12/10/08   kma     Disabled voice activation in FTM mode
03/21/08   dw      Change F3 message priority of GSM BER MSG length = 0 to HIGH
03/21/08   jfc     Include "assert.h"
03/06/08   ad      Removed rr_general.h since there are no references to it 
02/25/08   dw      Fixed high Lint warnings (525)
11/15/07   ad      SE BER support from 6260: Manage Rx data buffer overflows                   
11/06/07   ka      Ensure 1x fw is unloaded to prevent crash.
08/09/07   jfc     Mainlining T_MSM6250 feature
08/08/07   jfc     Mainlined T_MSM6280 feature
08/06/07   jfc     Featurized code for EDGE-only builds
06/25/07   ad      Added support for single ended BER
02/28/07   ycl     Fix compiler warnings.
12/13/06   jfc     Call ftm_send_FTM_STOP_GSM_MODE_REQ()
09/18/06   dp      Handle loopback configuration properly across handover, channel
                   release, and GSM Stop.
05-24-06   jac     Fixed LINT high errors.
04/03/06   dp      Featurize new band defs to MSM6280, so this code can be shared
                   with 6250/6275 MSM.  Formatting changes.
02/21/06  tws      Change WCDMA_1800 to BC3. Add support for BC4 and BC9.
10-17-05   dp      Add ftm_umts_ber.h include file.
10-14-05   dp      If F/S ACQ fails, don't advance state to ACQ_BCCH.
09-26-05   dp      Add handling of L1 reset IND/CNF.  Add some more comments.
07-20-05   dp      Add better state handling.  Add new Channel Assign V2 request
                   type with more parameters.  Can also perform better Handovers.
                   (changing channel on test box, no new slot/frame/multiframe timing)
06-16-05   dp      Add logging of channel release confirmation.
11-18-04   dp      Added ftm_gsm_ber_cleanup function
10/18/04   fpe     Removed FTM_HAS_ASYNC_PKT
09-12-04   dp      Update with more TCH options.  
                   Add event logging.
09-01-04   dp      Created
===========================================================================*/


/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rfa_variation.h"
#include <string.h>
#include <stdio.h>
#include "msg.h"
#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "ftm_task.h"
#include "ftm_gsm_ctl.h"
#include "rf_test_task_v.h"
#include "ftmicap.h"
#include "ftm_umts_ber.h"
#include "ftm_gsm_ber.h"
#include "ftm_common_control.h"
#include "ftmdiag.h"
#include "diagpkt.h"
#include "dog.h"
#include "assert.h"
#include "ftm_gsm_rfctl.h"
#include "stringl.h"

#include "rr_l1.h"
#include "ms.h"

#include "geran_test_mode_api.h"
#include "gl1_hw.h"

#include "queue.h"
#include "gs.h"
#include "sys_type.h"
#include "sys_cnst.h"
#include "ghdi_exp.h"
#ifdef FEATURE_CMI
#include "ghdi_exp_v.h"
#endif /* FEATURE_CMI */

#if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_FTM_EGPRS_BER)
#include "ftm_egprs_ber.h"
#endif

#ifdef FEATURE_MMGPS
#include "lsm.h"
#endif


#ifdef FTM_HAS_SE_BER
#include "ftm_se_ber.h"
#endif

#ifdef FEATURE_FTM_GSM_BER

/* Set this TRUE to test DRX path only */
static boolean ftm_gsm_ber_test_drx = FALSE;

/*! @brief Used for directing messages from L1 to the correct device instance */
extern rfm_device_enum_type ftm_gsm_ns_active_device;
rfm_device_enum_type ftm_gsm_ns_override_device = RFM_INVALID_DEVICE;

extern rfm_device_enum_type device_in_ftm_mode;

/* Structure for events that will be reported synchronously */
typedef struct 
{
  uint8 has_updated;
  ftm_gsm_ber_sync_status_type status;

} ftm_gsm_ber_sync_status_struct;

/* array to record the events that will be reported synchronously 
   via FTM_GSM_BER_EVENT_STATUS_REQ
*/
ftm_gsm_ber_sync_status_struct GSM_BER_Sync_Status[  FTM_GSM_BER_SYNC_STATUS_MAX ];

#ifdef FEATURE_MVS
extern void ghdi_gsm_set_dtx_mode(boolean ); 
#endif

#ifdef FEATURE_CDMA1X
extern void ftm_gsm_enter(void );
#endif

extern uint64 rfcommon_nv_get_rf_bc_config(rf_path_enum_type path);

void ftm_gsm_ber_handle_msgs( void *msg);
void ftm_gsm_ber_handle_rr_msgs( void *msg);
void ftm_gsm_ber_handle_l2_msgs( void *msg);

void ftm_gsm_ber_clear_sync_status( void);

/* Prototype for function to initialise non signalling ftm variables */
boolean ftm_gsm_ber_create_and_init_vars( ftm_gsm_rfctl_vars_s * instance );

LOCAL void ftm_gsm_ber_log_gen_cnf( ftm_log_gsm_ber_id_type  id);
LOCAL void ftm_gsm_ber_log_gen_cnf_with_status( ftm_log_gsm_ber_id_type  id, uint8 status);
LOCAL void ftm_gsm_ber_log_acq_bcch_cnf( boolean status, const byte *sch_data );
LOCAL void ftm_gsm_ber_log_ph_data_ind( l2_channel_type_T channel_type, boolean crc_pass);
LOCAL void ftm_gsm_ber_log_ded_rpt( uint8 rxlev, uint8 rxqual);


static void ftm_gsm_ber_free_rx_data_buffer( void *msg);

/* Function prototypes moved here to access ftm_gsm_rfctl_vars_s */

gs_status_T ftm_gsm_ber_send_FTM_SELECT_SPECIFIC_BCCH_REQ( rfm_device_enum_type rfm_dev,
                                                           uint16 chanNum, 
                                                           sys_band_T band );
gs_status_T ftm_gsm_ber_send_FTM_START_GSM_MODE_REQ( rfm_device_enum_type rfm_dev );
gs_status_T ftm_gsm_ber_send_FTM_CHANNEL_ASSIGN_REQ( rfm_device_enum_type rfm_dev );
gs_status_T ftm_gsm_ber_send_FTM_CHANNEL_RELEASE_REQ( rfm_device_enum_type rfm_dev );
gs_status_T ftm_gsm_ber_send_FTM_START_IDLE_REQ( rfm_device_enum_type rfm_dev );
gs_status_T ftm_gsm_ber_send_FTM_STOP_GSM_MODE_REQ( rfm_device_enum_type rfm_dev );

void ftm_gsm_reset_device ( void );


/* Queues for processing messages sent by L1 */
q_type  ftm_gsm_ber_q;
q_type  ftm_gsm_ber_free_q;


/* static memory for holding Queue'd messages */
ftm_gsm_ber_queue_type  ftm_gsm_ber_msgs[GS_DEFAULT_QUEUE_SIZE];


q_type                                 ftm_gsm_ber_rx_data_q;
q_type                                 ftm_gsm_ber_rx_data_free_q;
ftm_gsm_ber_rx_data_payload_msg_type   ftm_gsm_ber_rx_data_msgs[FTM_GSM_BER_NUM_RX_DATA_MSG_BUFFERS];

/* Old ber status structure - will be removed when it is safe to do so...*/
ftm_gsm_ber_status_type ber_status = { 
  FTM_GSM_BER_STATE_STOPPED,
  {0,SYS_BAND_EGSM_900},
  0,
  {0,SYS_BAND_EGSM_900},
  FTM_TCH_SLOT_INVALID,
  FTM_GSM_BER_TCH_FR,
  FTM_GSM_LOOPBACK_TYPE_NULL,
  FTM_GSM_LOOPBACK_TYPE_NULL,
  {{0, 0}},
  0
};

/* Used to initialise the ftm variables quickly */
static ftm_gsm_ber_status_type ber_status_init = { 
  FTM_GSM_BER_STATE_STOPPED,
  {0,SYS_BAND_EGSM_900},
  0,
  {0,SYS_BAND_EGSM_900},
  FTM_TCH_SLOT_INVALID,
  FTM_GSM_BER_TCH_FR,
  FTM_GSM_LOOPBACK_TYPE_NULL,
  FTM_GSM_LOOPBACK_TYPE_NULL,
  {{0, 0}},
  0
};


/* New macros to update the device-specific status */
#define FTM_GSM_BER_DEV_SET_STATE(p, s)  (p->ftm_gsm_ber_status.state = s)
#define FTM_GSM_BER_DEV_GET_STATE(p)     (p->ftm_gsm_ber_status.state)

/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================
 
                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Checks the GSM BER state. If any devices are not in the stopped state that
  state gets returned;

  @details
  Checks all devices. If they are initialised and BER state is not STOPPED, then
  return that state

  @retval
  FTM_GSM_BER_STATE_STOPPED if devices are uninitialized or all devices are in the
  stopped state OR the state of the first device that is not STOPPED
*/  
ftm_gsm_ber_state_type ftm_gsm_ber_get_state( void )
{
  uint8 i;

  /* If any of the devices are initialized but not stopped, return that state */
  for( i=0; i<RFM_MAX_WAN_DEVICES; i++ )
  {
    if ( ( ftm_gsm_vars_created(i) == TRUE ) &&
         ( FTM_GSM_BER_DEV_GET_STATE(ftm_gsm_get_vars_ptr(i)) != FTM_GSM_BER_STATE_STOPPED ) )
    {
      return(FTM_GSM_BER_DEV_GET_STATE(ftm_gsm_get_vars_ptr(i)));
    }
  }

  /* If all devices are stopped or not initialised, return STATE_STOPPED */
  return FTM_GSM_BER_STATE_STOPPED;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sets the GSM BER state for a given device.
 
  @param rfm_dev - The RFM device to set the state for
 
  @param new_state - the new state for the given device
*/
void ftm_gsm_ber_set_dev_state( rfm_device_enum_type rfm_dev, 
                                ftm_gsm_ber_state_type new_state )
{
  if ( ftm_gsm_vars_created(rfm_dev) )
  {
    FTM_GSM_BER_DEV_SET_STATE( ftm_gsm_get_vars_ptr(rfm_dev), new_state);
  }
  else
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, 
           "Attempting to set BER state %d for unitialized device %d", new_state, rfm_dev );
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Gets the GSM BER state for a given device.
 
  @param rfm_dev - The RFM device to set the state for
 
  @retval ber_state - state of the device
*/
ftm_gsm_ber_state_type ftm_gsm_ber_get_dev_state( rfm_device_enum_type rfm_dev )
{

  if ( ftm_gsm_vars_created(rfm_dev) )
  {
    return( FTM_GSM_BER_DEV_GET_STATE(ftm_gsm_get_vars_ptr(rfm_dev)));
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, 
           "Attempting to read state for unitialized device %d", rfm_dev);

    return FTM_GSM_BER_STATE_STOPPED;
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief Allocates and initialises the global variables for GSM FTM BER 

   @details 

   @param  

   @retval 
   
*/
boolean ftm_gsm_ber_create_and_init_vars( ftm_gsm_rfctl_vars_s * ftm_vars_p )
{

  if( ftm_vars_p != NULL )
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_HIGH, "Initializing GSM NS vars for device %d", ftm_vars_p->rfm_dev);

    //Initialise status here
    ftm_vars_p->ftm_gsm_ber_status = ber_status_init;

  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_HIGH, "NULL pointer passed to FTM BER INIT, %d", ftm_vars_p );

    return FALSE;
  }

  /* Everything went OK */
  return TRUE;
  
}
/*===========================================================================

FUNCTION 

DESCRIPTION
.  
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_gsm_ber_create_gen_response( const ftm_gsm_ber_pkt_type *cmd_ptr, 
                                                  uint16 status, uint16 cid, uint16 scid)
{

  ftm_rsp_pkt_type  gen_rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_gsm_ber_gen_rsp_pkt_type   *gen_rsp_payload;


  gen_rsp = ftmdiag_create_new_pkt( sizeof( ftm_gsm_ber_gen_rsp_pkt_type));

  if ( gen_rsp.pkt_payload != NULL)
  {
    gen_rsp_payload = ( ftm_gsm_ber_gen_rsp_pkt_type *) gen_rsp.pkt_payload;

    gen_rsp_payload->header                  = cmd_ptr->header;
    gen_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_GSM_BER_GEN_RESPONSE;
    gen_rsp_payload->ftm_rsp_header.rsp_cid  = cid;
    gen_rsp_payload->ftm_rsp_header.rsp_scid = scid;
    gen_rsp_payload->status                  = status;

  }

  return gen_rsp;
}


/*===========================================================================

FUNCTION     FTM_GSM_BER_CREATE_RX_LEV_RESPONSE

DESCRIPTION
             This API is used to generate FTM rsp pkt for FTM_LOG_GSM_BER_RX_LEVEL_REQ
DEPENDENCIES
  none

RETURN VALUE
   FTM rsp packet

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_gsm_ber_create_rx_lev_response( const ftm_gsm_ber_pkt_type *cmd_ptr, 
                                                  uint16 status, uint16 cid, uint16 scid, ftm_gsm_ber_rx_level_rpt_type ftm_gsm_ber_rx_lev_rpt)
{
  ftm_rsp_pkt_type  gen_rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  
  ftm_gsm_ber_rx_lev_rsp_pkt_type   *gen_rsp_payload;

  gen_rsp = ftmdiag_create_new_pkt( sizeof( ftm_gsm_ber_rx_lev_rsp_pkt_type));

  if ( gen_rsp.pkt_payload != NULL)
  {
    gen_rsp_payload = ( ftm_gsm_ber_rx_lev_rsp_pkt_type *) gen_rsp.pkt_payload;
    gen_rsp_payload->header                  = cmd_ptr->header;
    gen_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_GSM_BER_GEN_RESPONSE;  // Need to be checked
    gen_rsp_payload->ftm_rsp_header.rsp_cid  = cid;
    gen_rsp_payload->ftm_rsp_header.rsp_scid = scid;
    gen_rsp_payload->status                  = status;
    gen_rsp_payload->rx_lev_rpt              = ftm_gsm_ber_rx_lev_rpt;
  }
  return gen_rsp;
}

/*===========================================================================

FUNCTION     FTM_GSM_BER_LOG_GEN_CNF

DESCRIPTION
             Logs a generic confirmation message to the user (PC side).  This
             is used when the indication is that a command was completed,
             but no payload needs to be sent.
               
DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_gsm_ber_log_gen_cnf( ftm_log_gsm_ber_id_type  id)
{

#ifdef FTM_HAS_LOGGING 
  ftm_log_gsm_ber_gen_cnf        *gen_cnf_log;


  gen_cnf_log = ( ftm_log_gsm_ber_gen_cnf *) 
                ftm_log_malloc( (uint16)FTM_LOG_GSM_BER, 
                                sizeof( ftm_log_gsm_ber_gen_cnf) );

  if ( gen_cnf_log != NULL)
  {
    gen_cnf_log->log_id = id;

    log_commit( gen_cnf_log);
  }
#endif
}



/*===========================================================================

FUNCTION     FTM_GSM_BER_LOG_GEN_CNF_WITH_STATUS

DESCRIPTION
             Logs a generic confirmation message to the user (PC side).  This
             is used when the indication is that a command was completed, and
             pass a generic status byte to the user.
               
DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_gsm_ber_log_gen_cnf_with_status( ftm_log_gsm_ber_id_type  id, uint8 status)
{

#ifdef FTM_HAS_LOGGING 
  ftm_log_gsm_ber_gen_cnf_with_status        *gen_cnf_log;

  gen_cnf_log = ( ftm_log_gsm_ber_gen_cnf_with_status *) 
                ftm_log_malloc( (uint16)FTM_LOG_GSM_BER, 
                                sizeof( ftm_log_gsm_ber_gen_cnf_with_status) );

  if ( gen_cnf_log != NULL)
  {
    gen_cnf_log->log_id = id;
    gen_cnf_log->status = status;

    log_commit( gen_cnf_log);
  }
#endif
}


/*===========================================================================

FUNCTION    FTM_GSM_BER_LOG_ACQ_BCCH_CNF   

DESCRIPTION
            Logs a select specific bcch confirmation message to the user (PC
            side).  Payload is an indication if acquisition was successful, 
            and the SCH data block.  SCH data will only be valid if the
            confirmation indicates the acquisition was successful.
               
DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_gsm_ber_log_acq_bcch_cnf( boolean status, const byte *sch_data )
{

#ifdef FTM_HAS_LOGGING 
  ftm_log_gsm_ber_select_specific_bcch_cnf_type   *gsm_ber_log;

  gsm_ber_log = (ftm_log_gsm_ber_select_specific_bcch_cnf_type *) 
                ftm_log_malloc( (uint16)FTM_LOG_GSM_BER,
                                sizeof( ftm_log_gsm_ber_select_specific_bcch_cnf_type));

  if ( gsm_ber_log != NULL)
  {
    gsm_ber_log->log_id = FTM_LOG_GSM_BER_SELECT_SPECIFIC_BCCH_CNF;
    gsm_ber_log->status = status;

    memscpy( (void *) gsm_ber_log->sch_data,
            SCH_BLOCK_SIZE,
            (void *) sch_data,
            SCH_BLOCK_SIZE);
    log_commit( gsm_ber_log);
  }
#endif
}


/*===========================================================================

FUNCTION     FTM_GSM_BER_LOG_PH_DATA_IND     

DESCRIPTION
             Logs a ph_data_ind received from L1 to the user (PC side)
               
DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_gsm_ber_log_ph_data_ind( l2_channel_type_T channel_type, boolean crc_pass)
{

#ifdef FTM_HAS_LOGGING 
  ftm_log_gsm_ber_ph_data_ind_type          *gsm_ber_log;


  gsm_ber_log = ( ftm_log_gsm_ber_ph_data_ind_type *)
                ftm_log_malloc( (uint16)FTM_LOG_GSM_BER,
                                sizeof( ftm_log_gsm_ber_ph_data_ind_type)); 

  if ( gsm_ber_log != NULL)
  {
    gsm_ber_log->log_id       = FTM_LOG_GSM_BER_PH_DATA_IND;
    gsm_ber_log->channel_type = channel_type;
    gsm_ber_log->crc_pass     = crc_pass;


    log_commit( gsm_ber_log);
  }
#endif
}



/*===========================================================================

FUNCTION     FTM_GSM_BER_LOG_PH_DATA_IND     

DESCRIPTION
             Logs a ph_data_ind received from L1 to the user (PC side)
               
DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_gsm_ber_log_ded_rpt( uint8 rxlev, uint8 rxqual)
{

#ifdef FTM_HAS_LOGGING 

  ftm_log_gsm_ber_ded_rpt_type          *gsm_ber_log;

  gsm_ber_log = ( ftm_log_gsm_ber_ded_rpt_type *)
                ftm_log_malloc( (uint16)FTM_LOG_GSM_BER,
                                sizeof( ftm_log_gsm_ber_ded_rpt_type)); 

  if ( gsm_ber_log != NULL)
  {
    gsm_ber_log->log_id       = FTM_LOG_GSM_BER_DED_RPT;
    gsm_ber_log->RxLev_avg    = rxlev;
    gsm_ber_log->RxQual_avg   = rxqual;


    log_commit( gsm_ber_log);
  }

#endif
}



/*===========================================================================

FUNCTION       FTM_GSM_BER_COMPUTE_BSIC_FROM_SCH_BLOCK

DESCRIPTION
.              
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_ber_compute_BSIC_from_SCH_block( BSIC_T  *BSIC_result_ptr, const byte *SCH_block)
{

  BSIC_result_ptr->PLMN_colour_code = (byte) (((( SCH_block[0] & 0xFC ) >> 2 ) & 0x38 ) >> 3 );
  BSIC_result_ptr->BS_colour_code   = (byte) (((  SCH_block[0] & 0xFC ) >> 2 ) & 0x07 );


  MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,"BSIC: PLMN_Colour_Code = %d, BS_colour_code = %d", 
        BSIC_result_ptr->PLMN_colour_code,BSIC_result_ptr->BS_colour_code);
} 


/*===========================================================================

FUNCTION       FTM_GSM_BER_ENABLE_TCH

DESCRIPTION
.              Starts the TCH data flow by using GHDI interface
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_ber_enable_tch( rfm_device_enum_type rfm_dev
#ifdef FEATURE_DUAL_SIM
                             ,sys_modem_as_id_e_type as_id
#endif 
)
{

  ghdi_tch_control_T       tch_control;
  channel_mode_T           channel_mode;

  ftm_gsm_rfctl_vars_s *ftm_vars_p = ftm_gsm_get_vars_ptr(rfm_dev);

#ifdef FEATURE_MVS
  ghdi_gsm_set_dtx_mode( FALSE);
#endif

  switch ( ftm_vars_p->ftm_gsm_ber_status.tch_mode)
  {
  
  case FTM_GSM_BER_TCH_FR:
    tch_control  = GHDI_TURN_SPEECH_TCH_F_ON;
    channel_mode = SPEECH_V1;

    break;

  case FTM_GSM_BER_TCH_HR:

    tch_control  = GHDI_TURN_SPEECH_TCH_H_0_ON;
    channel_mode = SPEECH_V1;

    break;

  case FTM_GSM_BER_TCH_EFR:

    tch_control  = GHDI_TURN_SPEECH_TCH_F_ON;
    channel_mode = SPEECH_V2;

    break;

  case FTM_GSM_BER_TCH_AMR:

    tch_control  = GHDI_TURN_SPEECH_TCH_F_ON;
    channel_mode = SPEECH_V3;

    break;

  default:

    tch_control  = GHDI_TURN_SPEECH_TCH_F_ON;
    channel_mode = SPEECH_V1;
    break;

  }

  if( rfm_dev == RFM_DEVICE_2 )
  {
#ifdef FEATURE_DUAL_SIM
    (void)ghdi_control_subs_tch_gating( tch_control, channel_mode, (1<<18), as_id );
#else
    (void)ghdi_control_tch_gating_subs( tch_control, channel_mode, (1<<18) );
#endif 
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
    (void)ghdi_control_subs_tch_gating( tch_control, channel_mode, 0 , as_id );
#else
    (void)ghdi_control_tch_gating_subs( tch_control, channel_mode, 0 );
#endif 
  }  

}



/*===========================================================================

FUNCTION     FTM_GSM_BER_DISABLE_TCH

DESCRIPTION
.            Disable the TCH data flow using GHDI interface
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_ber_disable_tch( rfm_device_enum_type rfm_dev
#ifdef FEATURE_DUAL_SIM
                             ,sys_modem_as_id_e_type as_id
#endif 
)
{

  ghdi_tch_control_T       tch_control;
  channel_mode_T           channel_mode;

  ftm_gsm_rfctl_vars_s *ftm_vars_p = ftm_gsm_get_vars_ptr(rfm_dev);

  switch ( ftm_vars_p->ftm_gsm_ber_status.tch_mode)
  {
  
  case FTM_GSM_BER_TCH_FR:
    tch_control  = GHDI_TURN_SPEECH_TCH_F_OFF;
    channel_mode = SPEECH_V1;

    break;

  case FTM_GSM_BER_TCH_HR:

    tch_control  = GHDI_TURN_SPEECH_TCH_H_0_OFF;
    channel_mode = SPEECH_V1;

    break;

  case FTM_GSM_BER_TCH_EFR:

    tch_control  = GHDI_TURN_SPEECH_TCH_F_OFF;
    channel_mode = SPEECH_V2;

    break;

  case FTM_GSM_BER_TCH_AMR:

    tch_control  = GHDI_TURN_SPEECH_TCH_F_OFF;
    channel_mode = SPEECH_V3;

    break;

  default:

    tch_control  = GHDI_TURN_SPEECH_TCH_F_OFF;
    channel_mode = SPEECH_V1;
    break;

  }


  if( rfm_dev == RFM_DEVICE_2 )
  {
#ifdef FEATURE_DUAL_SIM
    (void)ghdi_control_subs_tch_gating( tch_control, channel_mode, (1<<18), as_id  );
#else
    (void)ghdi_control_tch_gating_subs( tch_control, channel_mode, (1<<18) );
#endif 
  }
  else
  {
#ifdef FEATURE_DUAL_SIM 
    (void)ghdi_control_subs_tch_gating( tch_control, channel_mode, 0, as_id );
#else
    (void)ghdi_control_tch_gating_subs( tch_control, channel_mode, 0 );
#endif 
  }  

}


/*===========================================================================

FUNCTION      FTM_GSM_BER_INIT

DESCRIPTION
              Initializes the Queue's and message handlers for the
              FTM GSM BER command set.
               
DEPENDENCIES
   FTM task initialized

===========================================================================*/
void ftm_gsm_ber_init( void)
{

  int i;

  (void)q_init( &ftm_gsm_ber_q);
  (void)q_init( &ftm_gsm_ber_free_q);

  for ( i = 0; i < GS_DEFAULT_QUEUE_SIZE; i++)
  {
    q_put( &ftm_gsm_ber_free_q, q_link( (void *) &ftm_gsm_ber_msgs[i], &(ftm_gsm_ber_msgs[i].link) ));
  }

  if ( gs_init( GS_QUEUE_FTM_GSM, 
                rex_self(),
                FTM_GSM_BER_CMD_Q_SIG,
                &ftm_gsm_ber_q,
                &ftm_gsm_ber_free_q,
                sizeof( ftm_gsm_ber_queue_type),
                GS_DEFAULT_QUEUE_SIZE,
                (gs_get_handler_ptr) ftm_gsm_ber_handle_msgs ) != GS_SUCCESS )
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "FTM GSM BER Q services not initialized!", 0);
  }

  /* initialize the Qs used for the data payload messages for single ended BER (GSM and EGPRS) */

  (void) q_init( &ftm_gsm_ber_rx_data_q);
  (void) q_init( &ftm_gsm_ber_rx_data_free_q);

  for ( i = 0; i < FTM_GSM_BER_NUM_RX_DATA_MSG_BUFFERS; i++)
  {
    q_put( &ftm_gsm_ber_rx_data_free_q, q_link( (void *) &ftm_gsm_ber_rx_data_msgs[i],
                                                &(ftm_gsm_ber_rx_data_msgs[i].link)));
  }
}


/*===========================================================================

FUNCTION      FTM_GSM_BER_CLEANUP

DESCRIPTION
              Sends necessary primitives to Layer 1 to clean up, in case
              we transition to online mode unexpectedly.
 
RETURN VALUE
   status
 
DEPENDENCIES
   FTM task initialized

===========================================================================*/
boolean ftm_gsm_ber_cleanup( rfm_device_enum_type rfm_dev )
{
    sys_modem_as_id_e_type  as_id;
	gs_status_T status = GS_FAILURE;      
	ftm_gsm_rfctl_vars_s *ftm_vars_p = ftm_gsm_get_vars_ptr(rfm_dev);
	if (  ftm_vars_p == NULL )
	{
	  MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "GSM FTM variables are NULL for device %d", rfm_dev);
	  return FALSE;
	}      

	as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

	if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) == FTM_GSM_BER_STATE_STOPPED)
	{
	  return TRUE;
	}

	if ( ftm_vars_p->ftm_gsm_ber_status.loopback_type != FTM_GSM_LOOPBACK_TYPE_NULL)
	{
	  /* clear the loopback on stop GSM */
	  geran_test_mode_api_do_tch_loopback( (gl1_defs_loopback_type) FTM_GSM_LOOPBACK_TYPE_NULL,as_id);
	}

	/* clear the loopback status on stop GSM */
	ftm_vars_p->ftm_gsm_ber_status.loopback_type       = FTM_GSM_LOOPBACK_TYPE_NULL;   
	ftm_vars_p->ftm_gsm_ber_status.saved_loopback_type = FTM_GSM_LOOPBACK_TYPE_NULL;   

	status = ftm_gsm_ber_send_FTM_STOP_GSM_MODE_REQ(rfm_dev);
	FTM_GSM_BER_DEV_SET_STATE( ftm_vars_p,  FTM_GSM_BER_STATE_STOPPED);
	if (status == GS_SUCCESS) 
	{
	  return TRUE;
	}
	else
	{
	  return FALSE;
	}
}

/*===========================================================================

FUNCTION       FTM_GSM_BER_SEND_TO_L1

DESCRIPTION

               
DEPENDENCIES
    
    UE is in FTM mode.
    ftm_gsm_ber_init() has been called

===========================================================================*/
gs_status_T ftm_gsm_ber_send_to_l1( void *msg)
{

  return gs_send_message( GS_QUEUE_LAYER_1, msg, TRUE);
}


/*===========================================================================

FUNCTION      FTM_GSM_BER_HANDLE_MESSAGES  

DESCRIPTION
              Main handler for messages sent to the GS_QUEUE_FTM_GSM.
               
DEPENDENCIES
      UE is in FTM mode.
      ftm_gsm_ber_init() has been called

===========================================================================*/
void ftm_gsm_ber_handle_msgs( void *msg)
{

  IMH_T                              *header;
  ftm_gsm_ber_queue_type             *message;
  uint16                             length;

  message = (ftm_gsm_ber_queue_type *) msg;

  header = (IMH_T *) &message->msg;

  GET_IMH_LEN( length, header);
  if ( length == 0 )
  {
    MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,"GSM BER MSG length = 0, Message Set: %d, Message id: %d",
          header->message_set,header->message_id);
  }

  switch ( header->message_set)
  {
  case MS_FTM_P_RR_L1:

    ftm_gsm_ber_handle_rr_msgs( msg);

    break;

  case MS_FTM_P_L2_L1:

    ftm_gsm_ber_handle_l2_msgs( msg);

    break;

  default:

    MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "Unexpected Message Set Passed into FTM GSM BER Q: MS:%d ID:%d",
          header->message_set,header->message_id);
  }

#if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_FTM_EGPRS_BER) 
  ftm_egprs_ber_handle_msgs_from_gl1( msg );
#endif
}



/*===========================================================================

FUNCTION        FTM_EGPRS_BER_GET_CMD_BUF

DESCRIPTION     "Allocates" a buffer for L1 to send messages to FTM.  This
                FTM scenario uses static allocation, so this simply gets a
                pointer to a buffer off the Free Q.


DEPENDENCIES    ftm_egprs_ber_init() called
  

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void *ftm_gsm_ber_get_rx_data_cmd_buf( void)
{

  ftm_gsm_ber_rx_data_payload_msg_type    *message;

  void *free_msg = NULL;

  if ( q_cnt( &ftm_gsm_ber_rx_data_free_q) <= FTM_GSM_BER_NUM_RX_DATA_MSG_BUFFERS/4)
  {

    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Not enough Free RxData buffers available",0);
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Clearing RxData buffer to avoid underrrun: cnt=%d",
          q_cnt(&ftm_gsm_ber_rx_data_free_q));
    while ( (message = q_get( &ftm_gsm_ber_rx_data_q)) != NULL)
    {
      ftm_gsm_ber_free_rx_data_buffer( (void *) message);
    }

    return NULL;
  }

  /* Get a buffer from the Free Q */

  free_msg = q_get( &ftm_gsm_ber_rx_data_free_q);

  return( free_msg);

}




/*===========================================================================

FUNCTION        FTM_EGPRS_BER_SEND_MSG_TO_FTM

DESCRIPTION     Takes a populated message and sends it to the FTM task via
                the q_xxxx() interfaces (uses command Q).  Sets signal for FTM task to 
                handle the message.
.

DEPENDENCIES    buffer has been previously "allocated" using 
                ftm_egprs_ber_get_cmd_buf()
   

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_ber_send_rx_data_to_ftm( const ftm_gsm_ber_rx_data_ind_type *rx_data_ind)
{
  /* Put the message onto the Cmd Q, and set the signal */

  ftm_gsm_ber_rx_data_payload_msg_type  *message;
  uint16  num_bytes = FTM_GSM_BER_RX_DATA_PAYLOAD_MAX_SIZE;

  rfm_device_enum_type rfm_dev = ftm_gsm_ns_active_device;

  if ( !ftm_mode )
  { //Stops Signalling mode breaking now that we are using status inside ftm_gsm_rfctl_vars
    return;
  }


  if ( rfm_dev >= RFM_MAX_WAN_DEVICES )                                                    
  {                                                                                                        
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"ftm_gsm_ns_active_device invalid! %d", rfm_dev);
    return;                                                                                                
  }                                                                                                        


  /* This is a temporary work-around until we implement a way to get the device from GL1 */
  if ( ftm_gsm_ber_get_dev_state(rfm_dev) != FTM_GSM_BER_STATE_TCH
#ifdef FEATURE_GSM_EGPRS
       && ftm_gsm_ber_get_dev_state(rfm_dev) != FTM_GSM_BER_STATE_GPRS_PDTCH
#endif
     )
  {
    /* must be in TCH or PDTCH non-signaling mode to perform this send Rx data operation */
    return;  
  }

  message = ( ftm_gsm_ber_rx_data_payload_msg_type *) ftm_gsm_ber_get_rx_data_cmd_buf();

  if ( message != NULL)
  {

    message->msg.fn       = rx_data_ind->fn;
    message->msg.bfi      = rx_data_ind->bfi;
    message->msg.num_bits = rx_data_ind->num_bits;
    message->msg.slot_num = rx_data_ind->slot_num;

    /* num_bits size is a nibble short.  So add 4 bits to convert from bits to bytes*/
    num_bytes = (rx_data_ind->num_bits + 4)/8;

    /* check for the max size is not needed anymore. see memscpy.c */
    memscpy( (void *) message->msg.rx_data, sizeof(message->msg.rx_data), rx_data_ind->data, num_bytes);

    (void)q_link( ( void *) message, &message->link);

    q_put( &ftm_gsm_ber_rx_data_q, &message->link);

    (void)rex_set_sigs( &ftm_tcb, FTM_GSM_BER_CMD_Q_SIG);

  }

}



/*===========================================================================

FUNCTION        FTM_EGPRS_BER_FREE_BUFFER

DESCRIPTION     "De-allocates" inter-task message buffers.  The L1->FTM 
                interaction uses static buffer allocation for sending messages
                so the message is freed by placing it back on the Free Q.
.

DEPENDENCIES    ftm_egprs_ber_init() called.
 

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
static void ftm_gsm_ber_free_rx_data_buffer( void *msg)
{

  /* Return a buffer to the free Q for future use */


  ftm_gsm_ber_rx_data_payload_msg_type  *message = ( ftm_gsm_ber_rx_data_payload_msg_type *) msg;

  (void)q_link( ( void *) message, &message->link);

  q_put( &ftm_gsm_ber_rx_data_free_q, &message->link);

}




/*===========================================================================

FUNCTION        ftm_gsm_ber_handle_rx_data_indications

DESCRIPTION     Handles raw data sent by L1 for use with single ended BER
                subsystem.
.

DEPENDENCIES    ftm_gsm_ber_init() called.
 

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_ber_handle_rx_data_indications( void)
{
  ftm_gsm_ber_rx_data_payload_msg_type    *message;

#ifdef FTM_HAS_SE_BER
  ftm_se_ber_config_run_type              config;
#endif

  rfm_device_enum_type rfm_dev = ftm_gsm_ns_active_device;
  ftm_gsm_rfctl_vars_s *ftm_vars_p;

  if ( rfm_dev >= RFM_MAX_WAN_DEVICES )                                                    
  {                                                                                                        
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"rfm_dev invalid! %d", rfm_dev);
    return;                                                                                                
  }

  ftm_vars_p = ftm_gsm_get_vars_ptr(rfm_dev);

  /* Fetch all the messages off the command Q */

  while ( (message = q_get( &ftm_gsm_ber_rx_data_q)) != NULL)
  {

#ifdef FTM_HAS_SE_BER
    MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH," RxDataQ: %d, RxDataFreeQ: %d",
          q_cnt(&ftm_gsm_ber_rx_data_q),q_cnt(&ftm_gsm_ber_rx_data_free_q));
    MSG_3(MSG_SSID_FTM, MSG_LEGACY_MED,"Rx Data: %d bits, tn: %d, crc: %d",
          message->msg.num_bits,message->msg.slot_num, message->msg.bfi);
    MSG_3(MSG_SSID_FTM, MSG_LEGACY_MED," 0x%02x,0x%02x,0x%02x",
          message->msg.rx_data[0],message->msg.rx_data[1], message->msg.rx_data[2]);


    /* Submit the DL data to the SE BER module */

    if ( ftm_vars_p->ftm_gsm_ber_status.loopback_type == FTM_GSM_LOOPBACK_TYPE_C)
    {

      config.bit_ordering     = FTM_SE_BER_BIT_ORDERING_LEFT_ALIGNED;  /* left for loop C */
      config.run_mode         = FTM_SE_RUN_MODE_GSM_LOOP_C;        
      config.bit_def.num_bits = message->msg.num_bits;
      config.slot_num         = message->msg.slot_num;
      config.bfi              = message->msg.bfi;
      config.fn               = message->msg.fn;

      ftm_se_ber_run( &message->msg.rx_data[0], &config);
    }
    else if ( (message->msg.num_bits == 260) &&
              (ftm_vars_p->ftm_gsm_ber_status.loopback_type == FTM_GSM_LOOPBACK_TYPE_A ||
               ftm_vars_p->ftm_gsm_ber_status.loopback_type == FTM_GSM_LOOPBACK_TYPE_B))
    {

      config.bit_ordering     = FTM_SE_BER_BIT_ORDERING_RIGHT_ALIGNED;  /* right aligned for Loop A/B */
      config.run_mode         = FTM_SE_RUN_MODE_GSM_LOOP_AB;   

      /* assuming FR mode */
      config.bit_def.num_bits_gsm_loop_ab.num_class1a_bits = 50;
      config.bit_def.num_bits_gsm_loop_ab.num_class1b_bits = 132;
      config.bit_def.num_bits_gsm_loop_ab.num_class2_bits  = 78;

      config.slot_num         = message->msg.slot_num;
      config.bfi              = message->msg.bfi;
      config.fn               = message->msg.fn;

      ftm_se_ber_run( &message->msg.rx_data[0], &config);
    }
#ifdef FEATURE_GSM_EGPRS
    else if ( ftm_gsm_ber_get_dev_state(rfm_dev) == FTM_GSM_BER_STATE_GPRS_PDTCH)
    {

      config.bit_ordering     = FTM_SE_BER_BIT_ORDERING_LEFT_ALIGNED;  /* left for EGPRS SRB */
      config.run_mode         = FTM_SE_RUN_MODE_EGPRS_SRB;  
      config.bit_def.num_bits = message->msg.num_bits;
      config.slot_num         = message->msg.slot_num;
      config.bfi              = message->msg.bfi;
      config.fn               = message->msg.fn;

      ftm_se_ber_run( &message->msg.rx_data[0], &config);
    }
#endif  /*FEATURE_GSM_EGPRS*/

#endif  /* FTM_HAS_SE_BER */

    ftm_gsm_ber_free_rx_data_buffer( (void *) message);

    dog_report( DOG_FTM_RPT);

    if ( q_cnt( &ftm_gsm_ber_rx_data_q) >= 3*FTM_GSM_BER_NUM_RX_DATA_MSG_BUFFERS/4 )
    {
      MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Clearing RxData buffer to avoid overrun: cnt=%d",
            q_cnt(&ftm_gsm_ber_rx_data_q));
      while ( (message = q_get( &ftm_gsm_ber_rx_data_q)) != NULL)
      {
        ftm_gsm_ber_free_rx_data_buffer( (void *) message);
      }
    }

    MSG_2(MSG_SSID_FTM, MSG_LEGACY_LOW," RxDataQ: %d, RxDataFreeQ: %d",
          q_cnt(&ftm_gsm_ber_rx_data_q),q_cnt(&ftm_gsm_ber_rx_data_free_q)); 
  }      
}

/*===========================================================================

FUNCTION       FTM_GSM_BER_HANDLE_RR_MSGS

DESCRIPTION
              Handles messages sent from L1 that would normally go
              to the RR task
               
DEPENDENCIES
      UE is in FTM mode.
      ftm_gsm_ber_init() has been called

===========================================================================*/
void ftm_gsm_ber_handle_rr_msgs( void *msg)
{

  IMH_T                              *header;
  ftm_gsm_ber_queue_type             *message;
  uint16                             length;

  rfm_device_enum_type rfm_dev = ftm_gsm_ns_active_device;
  ftm_gsm_rfctl_vars_s *ftm_vars_p;

  sys_modem_as_id_e_type  as_id;

  if ( rfm_dev >= RFM_MAX_WAN_DEVICES )                                                    
  {                                                                                                        
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"ftm_gsm_ns_active_device invalid! %d", rfm_dev);
    return;                                                                                                
  }

  as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

  ftm_vars_p = ftm_gsm_get_vars_ptr(rfm_dev);

  message = (ftm_gsm_ber_queue_type *) msg;
  header  = (IMH_T *) &message->msg;

  GET_IMH_LEN( length, header);

  switch ( header->message_id)
  {

  /*************************************************************************
  *              FTM_START_GSM_MODE_CNF                                    *
  *                                                                        *
  **************************************************************************/
  

  case FTM_START_GSM_MODE_CNF:
    /* In GSM mode now */
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH, 
          "RFM_DEV_%d Received START_GSM_MODE_CNF",
          rfm_dev);

    ftm_gsm_ber_log_gen_cnf( FTM_LOG_GSM_BER_START_GSM_MODE_CNF);

    if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) == FTM_GSM_BER_STATE_STOPPED)
    {

      FTM_GSM_BER_DEV_SET_STATE( ftm_vars_p, FTM_GSM_BER_STATE_NULL);
    }

    // Fill out the entry for the synchronous status message
    GSM_BER_Sync_Status[ FTM_GSM_BER_START_GSM_MODE_SYNC_STATUS ].status = FTM_GSM_BER_SYNC_STATUS_SUCCESS;
    GSM_BER_Sync_Status[ FTM_GSM_BER_START_GSM_MODE_SYNC_STATUS ].has_updated = TRUE;
    ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

    device_in_ftm_mode = rfm_dev;

    break;

    /*************************************************************************
    *              FTM_STOP_GSM_MODE_CNF                                     *
    *                                                                        *
    **************************************************************************/


  case FTM_STOP_GSM_MODE_CNF:

    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "RFM_DEV_%d Received STOP_GSM_MODE_CNF",
          rfm_dev);

    ftm_gsm_ber_log_gen_cnf( FTM_LOG_GSM_BER_STOP_GSM_MODE_CNF);

    if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) == FTM_GSM_BER_STATE_TCH)
    {
#ifdef FEATURE_DUAL_SIM
    ftm_gsm_ber_disable_tch( rfm_dev, as_id );
#else
    ftm_gsm_ber_disable_tch( rfm_dev );
#endif       
    }

    FTM_GSM_BER_DEV_SET_STATE( ftm_vars_p, FTM_GSM_BER_STATE_STOPPED);

    /* Now set the NS device invalid */
    ftm_gsm_ns_active_device = RFM_INVALID_DEVICE;

    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH, "FTM GSM active device is now invalid",0);

    ftm_gsm_reset_device(); //reset device on stop NS mode CNF

    // Fill out the entry for the synchronous status message
    GSM_BER_Sync_Status[ FTM_GSM_BER_STOP_GSM_MODE_SYNC_STATUS ].status = FTM_GSM_BER_SYNC_STATUS_SUCCESS;
    GSM_BER_Sync_Status[ FTM_GSM_BER_STOP_GSM_MODE_SYNC_STATUS ].has_updated = TRUE;
    ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

    break;

    /*************************************************************************
    *              FTM_START_IDLE_MODE_CNF                                   *
    *                                                                        *
    **************************************************************************/


  case FTM_START_IDLE_MODE_CNF:

    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "RFM_DEV_%d Received START_IDLE_MODE_CNF",
          rfm_dev);

    ftm_gsm_ber_log_gen_cnf( FTM_LOG_GSM_BER_START_IDLE_MODE_CNF);


    if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) == FTM_GSM_BER_STATE_ACQ_BCCH)
    {

      FTM_GSM_BER_DEV_SET_STATE( ftm_vars_p,  FTM_GSM_BER_STATE_IDLE);
    }

    // Fill out the entry for the synchronous status message
    GSM_BER_Sync_Status[ FTM_GSM_BER_START_IDLE_MODE_SYNC_STATUS ].status = FTM_GSM_BER_SYNC_STATUS_SUCCESS;
    GSM_BER_Sync_Status[ FTM_GSM_BER_START_IDLE_MODE_SYNC_STATUS ].has_updated = TRUE;
    ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

    break;

    /*************************************************************************
    *              FTM_SELECT_SPECIFIC_BCCH_CNF                              *
    *                                                                        *
    **************************************************************************/

  case FTM_SELECT_SPECIFIC_BCCH_CNF:

    /* BCCH confirmation...get SCH data if decoded by L1 */

    if ( message->msg.ftm_pseudo_rr_msg.mph_select_specific_bcch_cnf.BCCH_found == TRUE)
    {

      MSG_3(MSG_SSID_FTM, MSG_LEGACY_HIGH,
            "RFM_DEV_%d BCCH Found: (Chan,Band) = (%d,%d)",
            rfm_dev,
            ftm_vars_p->ftm_gsm_ber_status.arfcn.num,
            ftm_vars_p->ftm_gsm_ber_status.arfcn.band);


      ftm_gsm_ber_compute_BSIC_from_SCH_block( &ftm_vars_p->ftm_gsm_ber_status.network_parms.bsic,
                                               &message->msg.ftm_pseudo_rr_msg.mph_select_specific_bcch_cnf.SCH_block[0]);
    }
    else
    {

      MSG_3(MSG_SSID_FTM, MSG_LEGACY_HIGH,
            "RFM_DEV_%d BCCH Not Found: (Chan, Band) = (%d,%d)",
            rfm_dev,
            ftm_vars_p->ftm_gsm_ber_status.arfcn.num, 
            ftm_vars_p->ftm_gsm_ber_status.arfcn.band);
    }


    ftm_gsm_ber_log_acq_bcch_cnf( message->msg.ftm_pseudo_rr_msg.mph_select_specific_bcch_cnf.BCCH_found,
                                  message->msg.ftm_pseudo_rr_msg.mph_select_specific_bcch_cnf.SCH_block );


    if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) == FTM_GSM_BER_STATE_NULL ||
         FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) == FTM_GSM_BER_STATE_ACQ_BCCH ||
         FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) == FTM_GSM_BER_STATE_IDLE
#ifdef FEATURE_GSM_EGPRS
         || FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) == FTM_GSM_BER_STATE_GPRS_IDLE
#endif                 
       )
    {
      if ( message->msg.ftm_pseudo_rr_msg.mph_select_specific_bcch_cnf.BCCH_found == TRUE)
      {

        FTM_GSM_BER_DEV_SET_STATE( ftm_vars_p,  FTM_GSM_BER_STATE_ACQ_BCCH);
      }
      else
      {
        FTM_GSM_BER_DEV_SET_STATE( ftm_vars_p,  FTM_GSM_BER_STATE_NULL );
      }
    }
    // Fill out the entry for the synchronous status message
    if ( message->msg.ftm_pseudo_rr_msg.mph_select_specific_bcch_cnf.BCCH_found )
      GSM_BER_Sync_Status[ FTM_GSM_BER_SELECT_SPECIFIC_BCCH_CNF_SYNC_STATUS ].status = FTM_GSM_BER_SYNC_STATUS_SUCCESS;
    else
      GSM_BER_Sync_Status[ FTM_GSM_BER_SELECT_SPECIFIC_BCCH_CNF_SYNC_STATUS ].status = FTM_GSM_BER_SYNC_STATUS_EVENT_STATUS_FAILED;
    GSM_BER_Sync_Status[ FTM_GSM_BER_SELECT_SPECIFIC_BCCH_CNF_SYNC_STATUS ].has_updated = TRUE;
    ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

    break;


    /*************************************************************************
    *              FTM_CHANNEL_ASSIGN_CNF                                    *
    *                                                                        *
    **************************************************************************/


  case FTM_CHANNEL_ASSIGN_CNF:

    MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "RFM_DEV_%d CHANNEL_ASSIGN_CNF: Code %d",
          rfm_dev,
          message->msg.ftm_pseudo_rr_msg.mph_channel_assign_cnf.error_code);

    if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) == FTM_GSM_BER_STATE_IDLE)
    {
#ifdef FEATURE_DUAL_SIM
    ftm_gsm_ber_enable_tch( rfm_dev, as_id );
#else
    ftm_gsm_ber_enable_tch( rfm_dev );
#endif  

      FTM_GSM_BER_DEV_SET_STATE( ftm_vars_p,  FTM_GSM_BER_STATE_TCH);
    }

    /* restore the loopback type across handover */

    if ( ftm_vars_p->ftm_gsm_ber_status.saved_loopback_type != FTM_GSM_LOOPBACK_TYPE_NULL)
    {

      geran_test_mode_api_do_tch_loopback( (gl1_defs_loopback_type) ftm_vars_p->ftm_gsm_ber_status.saved_loopback_type, as_id );    
      ftm_vars_p->ftm_gsm_ber_status.loopback_type             = ftm_vars_p->ftm_gsm_ber_status.saved_loopback_type; 
      ftm_vars_p->ftm_gsm_ber_status.saved_loopback_type       = FTM_GSM_LOOPBACK_TYPE_NULL;                   
    }


    ftm_gsm_ber_log_gen_cnf_with_status( FTM_LOG_GSM_BER_CHANNEL_ASSIGN_CNF,
                                         message->msg.ftm_pseudo_rr_msg.mph_channel_assign_cnf.error_code);

    // Fill out the entry for the synchronous status message
    if ( message->msg.ftm_pseudo_rr_msg.mph_channel_assign_cnf.error_code == 0 )
      GSM_BER_Sync_Status[ FTM_GSM_BER_CHANNEL_ASSIGN_SYNC_STATUS ].status = FTM_GSM_BER_SYNC_STATUS_SUCCESS;
    else
      GSM_BER_Sync_Status[ FTM_GSM_BER_CHANNEL_ASSIGN_SYNC_STATUS ].status = FTM_GSM_BER_SYNC_STATUS_EVENT_STATUS_FAILED;

    GSM_BER_Sync_Status[ FTM_GSM_BER_CHANNEL_ASSIGN_SYNC_STATUS ].has_updated = TRUE;
    ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );


    break;

    /*************************************************************************
    *              FTM_CHANNEL_RELEASE_CNF                                   *
    *                                                                        *
    **************************************************************************/


  case FTM_CHANNEL_RELEASE_CNF:

    MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "RFM_DEV_%d CHANNEL_RELEASE_CNF: Code %d",
          rfm_dev,
          message->msg.ftm_pseudo_rr_msg.mph_channel_release_cnf.error_code);

    ftm_gsm_ber_log_gen_cnf_with_status( FTM_LOG_GSM_BER_CHANNEL_RELEASE_CNF,
                                         message->msg.ftm_pseudo_rr_msg.mph_channel_release_cnf.error_code);

    if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) == FTM_GSM_BER_STATE_TCH)
    {

#ifdef FEATURE_DUAL_SIM
    ftm_gsm_ber_disable_tch(rfm_dev, as_id);
#else
    ftm_gsm_ber_disable_tch(rfm_dev);
#endif  
      FTM_GSM_BER_DEV_SET_STATE( ftm_vars_p,  FTM_GSM_BER_STATE_ACQ_BCCH);
    }

    // Fill out the entry for the synchronous status message
    GSM_BER_Sync_Status[ FTM_GSM_BER_CHANNEL_RELEASE_SYNC_STATUS ].status = FTM_GSM_BER_SYNC_STATUS_SUCCESS;
    GSM_BER_Sync_Status[ FTM_GSM_BER_CHANNEL_RELEASE_SYNC_STATUS ].has_updated = TRUE;
    ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

    break;

    /*************************************************************************
    *              FTM_SURROUND_MEAS_IND                                     *
    *                                                                        *
    **************************************************************************/

  case FTM_SURROUND_MEAS_IND:
    MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "RFM_DEV_%d SURROUND_MEAS_IND: %d cells", 
          rfm_dev,
          geran_test_mode_api_get_best_six_info_no_of_entries(message));
    break;


    /*************************************************************************
    *              FTM_SERVING_DED_MEAS_IND                                  *
    *                                                                        *
    **************************************************************************/


  case FTM_SERVING_DED_MEAS_IND:

    MSG_3(MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "RFM_DEV_%d SERVING_DED_MEAS_IND: RXLEV %d,RXQUAL %d ", 
          rfm_dev,
          message->msg.ftm_pseudo_rr_msg.mph_serving_ded_meas_ind.RXLEV_FULL_average,
          message->msg.ftm_pseudo_rr_msg.mph_serving_ded_meas_ind.RXQUAL_FULL_average);


    ftm_gsm_ber_log_ded_rpt( message->msg.ftm_pseudo_rr_msg.mph_serving_ded_meas_ind.RXLEV_FULL_average,
                             message->msg.ftm_pseudo_rr_msg.mph_serving_ded_meas_ind.RXQUAL_FULL_average);

    break;


    /*************************************************************************
    *              DEFAULT                                                   *
    *                                                                        *
    **************************************************************************/

  default:

    MSG_3(MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "RFM_DEV_%d Primitive Received L1->FTM(RR): ID: %d, Length: %d", 
          rfm_dev,
          header->message_id, 
          length);

    break;

  }
}


/*===========================================================================

FUNCTION       FTM_GSM_BER_HANDLE_L2_MSGS

DESCRIPTION
               Message handler for messages sent from L1 that would
               normally go to the L2 task.
               
DEPENDENCIES
      UE is in FTM mode.
      ftm_gsm_ber_init() has been called

===========================================================================*/
void ftm_gsm_ber_handle_l2_msgs( void *msg)
{


  IMH_T                              *header;
  ftm_gsm_ber_queue_type             *message;
  uint16                             length;


  l2_channel_type_T                  chan_type;
  boolean                            crc_pass = FALSE;

  rfm_device_enum_type rfm_dev = ftm_gsm_ns_active_device;

  message = (ftm_gsm_ber_queue_type *) msg;
  header  = (IMH_T *) &message->msg;    

  GET_IMH_LEN( length, header);


  switch ( header->message_id)
  {
  /* phy data indications....collect BLER stats, dump out raw data */
  
  case FTM_PH_DATA_IND:

    chan_type = message->msg.ftm_pseudo_l2_msg.ph_data_ind.l2_channel_type;

    MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "RFM_DEV_%d PH_DATA_IND: Channel Type: %d", 
          rfm_dev,
          chan_type);

    if ( length == sizeof( ph_data_ind_T) - sizeof( IMH_T))
    {

      crc_pass = TRUE;                   
    }

    // If the channel type is SACCH and the CRC is pass, then record the SACCH synchronous status
    if ( 
          (chan_type == SACCH)
      &&  crc_pass
      )
    {
      // Fill out the entry for the synchronous status message
      GSM_BER_Sync_Status[ FTM_GSM_BER_SACCH_SYNC_STATUS ].status = FTM_GSM_BER_SYNC_STATUS_SUCCESS;
      GSM_BER_Sync_Status[ FTM_GSM_BER_SACCH_SYNC_STATUS ].has_updated = TRUE;
      ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );
    
    }


    ftm_gsm_ber_log_ph_data_ind( chan_type, crc_pass); 

    break;


  case FTM_PH_READY_TO_SEND_IND:


    MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "RFM_DEV_%d PH_READY_TO_SEND_IND, Chan Type %d",
          rfm_dev,
          message->msg.ftm_pseudo_l2_msg.ph_ready_to_send_ind.l2_channel_type);

    break;


  default:
    /* Handle messages intended for Layer 2 */
    MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Primitive Received L1->FTM(L2): ID: %d, Length: %d", 
          header->message_id, length);

    break;

  }
}


/*===========================================================================

FUNCTION       FTM_GSM_BER_SEND_FTM_START_GSM_MODE_REQ

DESCRIPTION
               Sends a Start GSM Mode Request from FTM to L1 task
               
DEPENDENCIES
  none

===========================================================================*/
gs_status_T ftm_gsm_ber_send_FTM_START_GSM_MODE_REQ( rfm_device_enum_type rfm_dev )
{

  gs_status_T                              status;

  uint64 rxd_dev_band_mask = 0;

  boolean enable_rxd = FALSE;
  boolean enable_prx = FALSE;
  boolean enable_drx = FALSE;

  ftm_gsm_rfctl_vars_s *vars_ptr = ftm_gsm_get_vars_ptr( rfm_dev );

  /*Retrieve RXD device associated with a PRX Device*/
  rfm_device_enum_type rxd_dev = rfgsm_core_get_drx_dev(rfm_dev);

  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

  MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,
        "RFM_DEV_%d ftm_gsm_ber_send_FTM_START_GSM_MODE_REQ()",
        rfm_dev );

  /*Query BC config NV for DRX to retrieve the band mask information*/
  rxd_dev_band_mask = rfcommon_nv_get_rf_bc_config( (rf_path_enum_type)(rxd_dev) );

  /*Confirm if GSM Bands are supported*/
  if (rxd_dev_band_mask & (SYS_BAND_MASK_GSM_EGSM_900 |
                           SYS_BAND_MASK_GSM_DCS_1800 |
                           SYS_BAND_MASK_GSM_850      |
                           SYS_BAND_MASK_GSM_PCS_1900) )
  {
    enable_rxd = vars_ptr->diversity_on;
    if (ftm_gsm_ber_test_drx == TRUE) 
    {
      enable_prx = FALSE;
      enable_drx = TRUE;
    }
    else
    {
      enable_prx = TRUE;
      enable_drx = TRUE;
    }
  }

  status = geran_test_mode_api_send_FTM_START_GSM_MODE_REQ( as_id,
															enable_rxd,
                                                            enable_drx,
                                                            enable_prx); 


  /* Set the active device so that we can process GL1 messages - This is a workaround until we can fully dereference all
     the GL1 message types and extract the gas_id; when we can do this, the code can be fully re-entrant */
  ftm_gsm_ns_active_device = rfm_dev;

  MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH, "FTM GSM active device is now device %d, using as_id %d", rfm_dev, as_id);

  if ( status == GS_SUCCESS )
  {
   // This will turn SACCH tx power control off
   // Tx power will freeze at the current level
    status = geran_test_mode_api_set_sacch_tx_pwr_control( FALSE, as_id );
  }


  return( status);
}


/*===========================================================================

FUNCTION       FTM_GSM_BER_SEND_FTM_STOP_GSM_MODE_REQ

DESCRIPTION
               Sends a Stop GSM Mode Request from FTM to L1 task
               
DEPENDENCIES
  none

===========================================================================*/
gs_status_T ftm_gsm_ber_send_FTM_STOP_GSM_MODE_REQ( rfm_device_enum_type rfm_dev )
{
  gs_status_T status;

  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

  status = ftm_send_FTM_STOP_GSM_MODE_REQ( rfm_dev );

  // This will turn SACCH tx power control on
  // Tx power control back to normal
  status = geran_test_mode_api_set_sacch_tx_pwr_control(TRUE , as_id );

  ftm_sig_rex_wait_no_clear(FTM_GSM_BER_EXIT_CMD_Q_SIG);
  (void)rex_clr_sigs(TASK_HANDLE(ftm), FTM_GSM_BER_EXIT_CMD_Q_SIG);

  return status;
}


/*===========================================================================

FUNCTION       FTM_GSM_BER_SEND_FTM_SELECT_SPECIFIC_BCCH_REQ

DESCRIPTION
               Sends a Select Specific Bcch Req from FTM task to L1
               
DEPENDENCIES
  none

===========================================================================*/
gs_status_T ftm_gsm_ber_send_FTM_SELECT_SPECIFIC_BCCH_REQ( rfm_device_enum_type rfm_dev,
                                                           uint16 chanNum, 
                                                           sys_band_T band )
{
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

#ifdef FEATURE_DUAL_SIM 
  return geran_test_mode_api_send_FTM_SELECT_SPECIFIC_BCCH_REQ( chanNum, band, as_id );
#else
  as_id = as_id;
  return geran_test_mode_api_send_FTM_SELECT_SPECIFIC_BCCH_REQ( chanNum, band );
#endif /* FEATURE_DUAL_SIM */ 
}


/*===========================================================================

FUNCTION      FTM_GSM_BER_SEND_START_IDLE_REQ       

DESCRIPTION
              Sends a Start IDLE Request from FTM to L1 task
               
DEPENDENCIES
  none

===========================================================================*/
gs_status_T ftm_gsm_ber_send_FTM_START_IDLE_REQ( rfm_device_enum_type rfm_dev )
{
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

#ifdef FEATURE_DUAL_SIM 
  return geran_test_mode_api_send_FTM_START_IDLE_REQ( as_id );
#else
  as_id = as_id;
  return geran_test_mode_api_send_FTM_START_IDLE_REQ();
#endif /* FEATURE_DUAL_SIM */
}


/*===========================================================================

FUNCTION       FTM_GSM_BER_SEND_FTM_CHANNEL_ASSIGN_REQ

DESCRIPTION
               Sends Channel Assign Request from FTM to L1 task
               
DEPENDENCIES
  none

===========================================================================*/
gs_status_T ftm_gsm_ber_send_FTM_CHANNEL_ASSIGN_REQ( rfm_device_enum_type rfm_dev )
{
  geran_test_mode_api_ftm_gsm_ber_status_type copy_ber_status;

  ftm_gsm_rfctl_vars_s *ftm_vars_p = ftm_gsm_get_vars_ptr( rfm_dev );

  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

  copy_ber_status.tsc = ftm_vars_p->ftm_gsm_ber_status.tsc;
  copy_ber_status.tch_arfcn = ftm_vars_p->ftm_gsm_ber_status.tch_arfcn;
  copy_ber_status.tch_slot = (geran_test_mode_api_ftm_gsm_ber_tch_slot_type) ftm_vars_p->ftm_gsm_ber_status.tch_slot;
  copy_ber_status.tch_mode = (geran_test_mode_api_ftm_gsm_ber_tch_mode_type) ftm_vars_p->ftm_gsm_ber_status.tch_mode;
  copy_ber_status.tch_ordered_power_level = ftm_vars_p->ftm_gsm_ber_status.tch_ordered_power_level;
#ifdef FEATURE_DUAL_SIM 
  return geran_test_mode_api_send_FTM_CHANNEL_ASSIGN_REQ( copy_ber_status, as_id );
#else
  as_id = as_id;
  return geran_test_mode_api_send_FTM_CHANNEL_ASSIGN_REQ( copy_ber_status );
#endif /* FEATURE_DUAL_SIM */
}




/*===========================================================================

FUNCTION       FTM_GSM_BER_SEND_FTM_CHANNEL_RELEASE_REQ

DESCRIPTION
               Sends Channel Release Request from FTM to L1 task
               
DEPENDENCIES
  none

===========================================================================*/
gs_status_T ftm_gsm_ber_send_FTM_CHANNEL_RELEASE_REQ( rfm_device_enum_type rfm_dev )
{
  geran_test_mode_api_ftm_gsm_ber_status_type copy_ber_status;

  ftm_gsm_rfctl_vars_s *ftm_vars_p = ftm_gsm_get_vars_ptr( rfm_dev );

  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

  copy_ber_status.arfcn = ftm_vars_p->ftm_gsm_ber_status.arfcn;
  copy_ber_status.network_parms.bsic = ftm_vars_p->ftm_gsm_ber_status.network_parms.bsic;
#ifdef FEATURE_DUAL_SIM 
  return geran_test_mode_api_send_FTM_CHANNEL_RELEASE_REQ( copy_ber_status, as_id );
#else
  as_id = as_id;
  return geran_test_mode_api_send_FTM_CHANNEL_RELEASE_REQ( copy_ber_status );
#endif /* FEATURE_DUAL_SIM */ 
}


/*===========================================================================

FUNCTION FTM_GSM_BER_DISPATCH

DESCRIPTION
       Dispatch function for user commands for FTM GSM BER testmode.
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_gsm_ber_dispatch( rfm_device_enum_type rfm_dev,
                                       ftm_gsm_ber_pkt_type *cmd_ptr)
{

  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

  // Variables for synchronous status
  uint32        start_time     = 0;
  int32         time_to_wait   = 0;
  uint8         TimeOutExpired = FALSE;
  rex_sigs_type ftm_sig_clk_rex_wait_return_sig;

  ftm_gsm_ber_select_specific_bcch_req_type     *select_specific_bcch_req;
  ftm_gsm_ber_config_loopback_req_type          *config_loopback_req;
  ftm_gsm_ber_loopback_type                     loopback_type;
  ftm_gsm_ber_channel_assign_req_type           *channel_assign_req;


  gs_status_T                                   status = GS_FAILURE;
  boolean                                       cmd_fail = TRUE;

  sys_modem_as_id_e_type  as_id; 

  //Variables for Rx_lev Request
  dBx16_T rx_agc_dbm16;
  uint32 rssi;
  dBx16_T rx_agc_dbm16_drx;
  uint32 rssi_drx;

  ftm_gsm_ber_rx_level_rpt_type		 ftm_gsm_ber_rx_lev_rpt;

  ftm_gsm_rfctl_vars_s *ftm_vars_p = NULL;

  if (ftm_gsm_ns_override_device != RFM_INVALID_DEVICE) 
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "GSM NS device overriden to %d",ftm_gsm_ns_override_device );

    rfm_dev = ftm_gsm_ns_override_device;
  }

  as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

  if ( ftm_gsm_ns_active_device != rfm_dev && 
       cmd_ptr->ftm_cmd_header.cmd_id != FTM_GSM_BER_START_GSM_MODE_REQ)
  {
    MSG_1(MSG_SSID_FTM, 
          MSG_LEGACY_ERROR, 
          "New device id (%d) sent to FTM GSM BER without doing START_GSM_MODE_REQ!", 
          rfm_dev);
  }

  /* If the FTM variables have not been initialised, do so now */
  if ( !ftm_gsm_vars_created(rfm_dev) )
  { 
    ftm_vars_p = ftm_gsm_create_and_init_vars(rfm_dev);
  }
  else
  {
    ftm_vars_p = ftm_gsm_get_vars_ptr(rfm_dev);
  }

  if (  ftm_vars_p == NULL )
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "GSM FTM variables are NULL for device %d", rfm_dev);
    return rsp_pkt;
  }

#if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_FTM_EGPRS_BER) 
  if ( cmd_ptr->ftm_cmd_header.cmd_id >= FTM_EGPRS_BER_CMD_BASE)
  {
    rsp_pkt = ftm_egprs_ber_dispatch( (ftm_egprs_ber_pkt_type *) cmd_ptr, rfm_dev ); /*lint !e740*/ 
  }
  else
#endif
  {

    MSG_1(MSG_SSID_FTM, MSG_LEGACY_LOW,"FTM GSM BER Dispatch: Received Command: %d", 
          cmd_ptr->ftm_cmd_header.cmd_id);

    switch ( cmd_ptr->ftm_cmd_header.cmd_id )
    {

    /*************************************************************************
    *             FTM_GSM_BER_START_GSM_MODE_REQ                             *
    *                                                                        *
    **************************************************************************/
    case FTM_GSM_BER_START_GSM_MODE_REQ:

      if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) != FTM_GSM_BER_STATE_STOPPED)
      {

        break;
      }

    /* ftm_rfmode_enter ensures that the previous ftm rfmode is exited before
       sending a start cmd to GL1 for NS operation
       Same as done by WCDMA NS cleanup 
    */

      ftm_set_rf_db_state( FTM_DB_RF_IDLE);
      ftm_rfmode_enter(rfm_dev,FTM_STATE_GSM_NS);

#ifdef FEATURE_MMGPS
      /* Set test mode in LSM task */
      (void)lsm_cmd_pt_test_mode( FALSE );
#endif /* FEATURE_MMGPS */

      ftm_set_rf_db_state( FTM_DB_RF_IDLE);

      /* clear the loopback status at start GSM */
      ftm_vars_p->ftm_gsm_ber_status.loopback_type       = FTM_GSM_LOOPBACK_TYPE_NULL; 
      ftm_vars_p->ftm_gsm_ber_status.saved_loopback_type = FTM_GSM_LOOPBACK_TYPE_NULL; 

      /* Now, activate GSM L1 using RR primitive */

      status = ftm_gsm_ber_send_FTM_START_GSM_MODE_REQ( rfm_dev );

      break;


      /*************************************************************************
      *              FTM_GSM_BER_SELECT_SPECIFIC_BCCH_REQ                      *
      *                                                                        *
      **************************************************************************/
    case FTM_GSM_BER_SELECT_SPECIFIC_BCCH_REQ:


      if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) != FTM_GSM_BER_STATE_NULL  &&
           FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) != FTM_GSM_BER_STATE_ACQ_BCCH  &&
           FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) != FTM_GSM_BER_STATE_IDLE      
#ifdef FEATURE_GSM_EGPRS                    
           && FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) != FTM_GSM_BER_STATE_GPRS_IDLE
#endif                  
         )
      {

        break;
      }

      select_specific_bcch_req = ( ftm_gsm_ber_select_specific_bcch_req_type *) cmd_ptr;/*lint !e740*/

      ftm_vars_p->ftm_gsm_ber_status.arfcn.num  = select_specific_bcch_req->channel;
      ftm_vars_p->ftm_gsm_ber_status.arfcn.band = ( sys_band_T) select_specific_bcch_req->band;

      status = ftm_gsm_ber_send_FTM_SELECT_SPECIFIC_BCCH_REQ( rfm_dev,
                                                              ftm_vars_p->ftm_gsm_ber_status.arfcn.num,
                                                              ftm_vars_p->ftm_gsm_ber_status.arfcn.band );

      break;


      /*************************************************************************
      *             FTM_GSM_BER_START_IDLE_MODE_REQ                            *
      *                                                                        *
      **************************************************************************/
    case FTM_GSM_BER_START_IDLE_MODE_REQ:

      if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) != FTM_GSM_BER_STATE_ACQ_BCCH)
      {

        break;
      }

      status = ftm_gsm_ber_send_FTM_START_IDLE_REQ( rfm_dev );

      break;

      /*************************************************************************
      *             FTM_GSM_BER_CONFIG_LOOPBACK_TYPE_REQ                       *
      *                                                                        *
      **************************************************************************/
    case FTM_GSM_BER_CONFIG_LOOPBACK_TYPE_REQ:

      config_loopback_req = (ftm_gsm_ber_config_loopback_req_type *) cmd_ptr;/*lint !e740*/
      loopback_type = ( ftm_gsm_ber_loopback_type) config_loopback_req->loopback_type;

      if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) != FTM_GSM_BER_STATE_TCH)
      {
        break;
      }


      //Remove comparison because RVCT 2.1 treats enum as uint.
      //if( loopback_type >= FTM_GSM_LOOPBACK_TYPE_NULL &&
      //    loopback_type <  FTM_GSM_NUM_LOOPBACK_TYPES)
      if ( loopback_type <  FTM_GSM_NUM_LOOPBACK_TYPES )
      {

        /* check for change in loopback type */
        if ( loopback_type != ftm_vars_p->ftm_gsm_ber_status.loopback_type)
        {

          /* If we were in loopback C, clear out the loopback type before applying the new type*/
          /* This is because loopback C is in the DSP while A and B are handled in SW */

          if ( ftm_vars_p->ftm_gsm_ber_status.loopback_type == FTM_GSM_LOOPBACK_TYPE_C &&
               loopback_type != FTM_GSM_LOOPBACK_TYPE_NULL)
          {
            geran_test_mode_api_do_tch_loopback( (gl1_defs_loopback_type) FTM_GSM_LOOPBACK_TYPE_NULL, as_id );
          }


          geran_test_mode_api_do_tch_loopback( (gl1_defs_loopback_type) loopback_type, as_id );
          ftm_vars_p->ftm_gsm_ber_status.loopback_type = loopback_type;
        }

        status = GS_SUCCESS;
      }


      break;

      /*************************************************************************
      *             FTM_GSM_BER_CHANNEL_ASSIGN_REQ                             *
      *                                                                        *
      **************************************************************************/
    case FTM_GSM_BER_CHANNEL_ASSIGN_REQ:

      if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) != FTM_GSM_BER_STATE_IDLE &&
           FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) != FTM_GSM_BER_STATE_TCH)
      {

        break;
      }

      channel_assign_req = ( ftm_gsm_ber_channel_assign_req_type *) cmd_ptr;/*lint !e740*/

      ftm_vars_p->ftm_gsm_ber_status.tch_mode                = (ftm_gsm_ber_tch_mode_type) channel_assign_req->tch_mode;
      ftm_vars_p->ftm_gsm_ber_status.tch_slot                = (ftm_gsm_ber_tch_slot_type) channel_assign_req->slot;
      ftm_vars_p->ftm_gsm_ber_status.tsc                     = 0;                  // default TSC
      ftm_vars_p->ftm_gsm_ber_status.tch_arfcn               = ftm_vars_p->ftm_gsm_ber_status.arfcn;   // use bcch arfcn
      ftm_vars_p->ftm_gsm_ber_status.tch_ordered_power_level = 15;                 // default PCL

      /* if we are doing a handover, save the current loopback type, then disable the loopback
       * .  When channel_cnf is received we can restore the loopback 
      */
      if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) == FTM_GSM_BER_STATE_TCH && 
           ftm_vars_p->ftm_gsm_ber_status.loopback_type != FTM_GSM_LOOPBACK_TYPE_NULL)
      {
        ftm_vars_p->ftm_gsm_ber_status.saved_loopback_type = ftm_vars_p->ftm_gsm_ber_status.loopback_type; 
        geran_test_mode_api_do_tch_loopback( (gl1_defs_loopback_type) FTM_GSM_LOOPBACK_TYPE_NULL, as_id );    
        ftm_vars_p->ftm_gsm_ber_status.loopback_type       = FTM_GSM_LOOPBACK_TYPE_NULL;                   
      }
      status = ftm_gsm_ber_send_FTM_CHANNEL_ASSIGN_REQ( rfm_dev );

      break;

      /*************************************************************************
      *            FTM_GSM_BER_CHANNEL_ASSIGN_V2_REQ                           *
      *                                                                        *
      **************************************************************************/
    case FTM_GSM_BER_CHANNEL_ASSIGN_V2_REQ:
      {

        ftm_gsm_ber_channel_assign_req_v2_type *channel_assign_v2_req;

        if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) != FTM_GSM_BER_STATE_IDLE &&
             FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) != FTM_GSM_BER_STATE_TCH)
        {

          break;
        }

        channel_assign_v2_req = ( ftm_gsm_ber_channel_assign_req_v2_type *) cmd_ptr;/*lint !e740*/

        ftm_vars_p->ftm_gsm_ber_status.tsc                     = channel_assign_v2_req->tsc;
        ftm_vars_p->ftm_gsm_ber_status.tch_arfcn.num           = channel_assign_v2_req->channel;
        ftm_vars_p->ftm_gsm_ber_status.tch_arfcn.band          = (sys_band_T)channel_assign_v2_req->band;
        ftm_vars_p->ftm_gsm_ber_status.tch_mode                = (ftm_gsm_ber_tch_mode_type) channel_assign_v2_req->tch_mode;
        ftm_vars_p->ftm_gsm_ber_status.tch_slot                = (ftm_gsm_ber_tch_slot_type) channel_assign_v2_req->slot;
        ftm_vars_p->ftm_gsm_ber_status.tch_ordered_power_level = channel_assign_v2_req->power_level;

        /* if we are doing a handover, save the current loopback type, then disable the loopback
         * .  When channel_cnf is received we can restore the loopback 
         */
        if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) == FTM_GSM_BER_STATE_TCH && 
             ftm_vars_p->ftm_gsm_ber_status.loopback_type != FTM_GSM_LOOPBACK_TYPE_NULL)
        {
          ftm_vars_p->ftm_gsm_ber_status.saved_loopback_type = ftm_vars_p->ftm_gsm_ber_status.loopback_type; 
          geran_test_mode_api_do_tch_loopback( (gl1_defs_loopback_type) FTM_GSM_LOOPBACK_TYPE_NULL, as_id );    
          ftm_vars_p->ftm_gsm_ber_status.loopback_type       = FTM_GSM_LOOPBACK_TYPE_NULL;                   
        }

        status = ftm_gsm_ber_send_FTM_CHANNEL_ASSIGN_REQ( rfm_dev );            

        break;
      }

      /*************************************************************************
      *             FTM_GSM_BER_CHANNEL_RELEASE_REQ                           *
      *                                                                        *
      **************************************************************************/
    case FTM_GSM_BER_CHANNEL_RELEASE_REQ:

      if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) != FTM_GSM_BER_STATE_TCH)
      {

        break;
      }

      /* clear the loopback on channel release */
      if ( ftm_vars_p->ftm_gsm_ber_status.loopback_type != FTM_GSM_LOOPBACK_TYPE_NULL)
      {
        geran_test_mode_api_do_tch_loopback( (gl1_defs_loopback_type) FTM_GSM_LOOPBACK_TYPE_NULL, as_id );
      }


      status = ftm_gsm_ber_send_FTM_CHANNEL_RELEASE_REQ( rfm_dev );

      /* clear the loopback status on channel release */
      ftm_vars_p->ftm_gsm_ber_status.loopback_type       = FTM_GSM_LOOPBACK_TYPE_NULL;  
      ftm_vars_p->ftm_gsm_ber_status.saved_loopback_type = FTM_GSM_LOOPBACK_TYPE_NULL;  

      break;

      /*************************************************************************
      *             FTM_GSM_BER_STOP_GSM_MODE_REQ                              *
      *                                                                        *
      **************************************************************************/
    case FTM_GSM_BER_STOP_GSM_MODE_REQ:

      if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p) == FTM_GSM_BER_STATE_STOPPED)
      {

        break;
      }
      else if ( ftm_vars_p->ftm_gsm_ber_status.loopback_type != FTM_GSM_LOOPBACK_TYPE_NULL)
      {
        /* clear the loopback on stop GSM */
        geran_test_mode_api_do_tch_loopback( (gl1_defs_loopback_type) FTM_GSM_LOOPBACK_TYPE_NULL, as_id );
      }

      /* clear the loopback status on stop GSM */
      ftm_vars_p->ftm_gsm_ber_status.loopback_type       = FTM_GSM_LOOPBACK_TYPE_NULL;   
      ftm_vars_p->ftm_gsm_ber_status.saved_loopback_type = FTM_GSM_LOOPBACK_TYPE_NULL;   

#ifdef FEATURE_MMGPS
      /* Set test mode in LSM task */
      (void)lsm_cmd_pt_test_mode( TRUE );
#endif /* FEATURE_MMGPS */


      status = ftm_gsm_ber_send_FTM_STOP_GSM_MODE_REQ( rfm_dev );

      FTM_GSM_BER_DEV_SET_STATE( ftm_vars_p,  FTM_GSM_BER_STATE_STOPPED);

      break;
    case FTM_GSM_BER_EVENT_STATUS_CLEAR_REQ:

      // call the function to clear the status
      (void) ftm_gsm_ber_clear_sync_status();

      // Setup the response value
      status = GS_SUCCESS;

      FTM_MSG_MED( "Clearing synchronous GSM events", 0,0,0);

      break;

    case FTM_GSM_BER_EVENT_STATUS_REQ:
      {
        // This comand will return the status of events that have occured
        // these events are stored in the GSM_BER_Sync_Status

        ftm_gsm_ber_event_status_req_type *request = (ftm_gsm_ber_event_status_req_type *) cmd_ptr;
        status   = GS_SUCCESS;
        cmd_fail = FALSE;

        // validate the ID of the event_type
        if ( request->event_type < FTM_GSM_BER_SYNC_STATUS_MAX)
        {
          // Record the start time
          start_time      = timetick_get();

          // Setup timeout loop
          // If the event has already reported then the timing loop will not execute
          while (  (  GSM_BER_Sync_Status[ request->event_type ].has_updated == FALSE )
                   &&  (  !TimeOutExpired ))
          {
            // Status messages from L1 may not be the one we are looking for.
            // For this case we will wait again adjusting for the time remaining.
            // This is neccessary because all request status messages are sharing the 
            // same signal.
            // The first time through the loop the time to wait will be the value passed
            // in by from the user
            time_to_wait = request->timeout_ms - timetick_get_elapsed( start_time, T_MSEC);

            // Make sure we have more time remaining
            if ( time_to_wait > 0 )
            {
              /* Wait on a request status sig or a timeout. */
              ftm_sig_clk_rex_wait_return_sig = ftm_sig_clk_rex_wait(FTM_SEQ_STATUS_REQ_SIG | FTM_GSM_BER_CMD_Q_SIG, time_to_wait);

              if( ftm_sig_clk_rex_wait_return_sig & FTM_GSM_BER_CMD_Q_SIG)
              {
                (void)rex_clr_sigs( &ftm_tcb, FTM_GSM_BER_CMD_Q_SIG);
                gs_read_messages( GS_QUEUE_FTM_GSM);   //rex sig cleared in gs services!
                ftm_gsm_ber_handle_rx_data_indications();
              }

              // Check to see if we timed out
              if ( (ftm_sig_clk_rex_wait_return_sig & FTM_WAIT_TIMEOUT) == FTM_WAIT_TIMEOUT )
              {
                TimeOutExpired = TRUE;
                cmd_fail       = FTM_GSM_BER_SYNC_STATUS_TIMEOUT_EXPIRED;
                status         = GS_FAILURE;
              }
            }
            else
            {
              TimeOutExpired = TRUE;
              cmd_fail       = FTM_GSM_BER_SYNC_STATUS_TIMEOUT_EXPIRED;
              status         = GS_FAILURE;
            } //if( time_to_wait > 0 )

          }// End While timing loop 

          // If message received then use the status from that message type
          if (  GSM_BER_Sync_Status[ request->event_type ].has_updated == TRUE)
          {
            cmd_fail = GSM_BER_Sync_Status[ request->event_type ].status;
            if(cmd_fail != FTM_GSM_BER_SYNC_STATUS_SUCCESS)
            {
              status = GS_FAILURE;
            }
          }
        }
        else
        {
          // Error, invalid event_type
          cmd_fail = FTM_GSM_BER_SYNC_STATUS_GEN_FAILURE;
          status   = GS_FAILURE;

          FTM_MSG_HIGH( "FTM_gsm_ber_EVENT_STATUS_REQ: Invalid event type %d", request->event_type,0,0);
        }//if ( request->event_type < FTM_GSM_BER_SYNC_STATUS_MAX)

        FTM_MSG_LOW( "FTM_GSM_BER_EVENT_STATUS_REQ: cmd_fail =%d event_type =%d", cmd_fail,request->event_type,0);
      }
      break;

   
  /***************************************************************************
					FTM_LOG_GSM_BER_RX_LEVEL_REQ
   ****************************************************************************/

    case FTM_LOG_GSM_BER_RX_LEVEL_REQ:
      if ( FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p)   != FTM_GSM_BER_STATE_TCH  &&
           ftm_egprs_ber_get_dev_state(rfm_dev) != FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHED)
      {
        status = GS_FAILURE;
        FTM_MSG_HIGH( "FTM_LOG_GSM_BER_RX_LEVEL_REQ: not In traffic state", 0,0,0);
        break;
      }
      gl1_hw_ftm_get_rx_power_results( &rssi,
                                       &rx_agc_dbm16,
                                       &rssi_drx,
                                       &rx_agc_dbm16_drx,
									   as_id
									  );

      // Fill RSSI and AGC values in rx_lev_rpt structure
      ftm_gsm_ber_rx_lev_rpt.rssi = (uint32)rssi;
      ftm_gsm_ber_rx_lev_rpt.rx_agc_dbm16 = (int16)(rx_agc_dbm16);

	 ftm_gsm_ber_rx_lev_rpt.rssi_drx = (uint32)rssi_drx;
     ftm_gsm_ber_rx_lev_rpt.rx_agc_dbm16_drx = (int16)(rx_agc_dbm16_drx);

    status = GS_SUCCESS;
    break;
      /*************************************************************************
      *             DEFAULT -- should return invalid command                   *
      *                                                                        *
      **************************************************************************/
    default:

      MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Ftm GSM BER, unknown cmd: %d", 
            cmd_ptr->ftm_cmd_header.cmd_id);
      break;
    } /* END of switch ( cmd_ptr->ftm_cmd_header.cmd_id ) */

    if ( status != GS_SUCCESS)
    {
      MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,"FTM GSM BER, Command Failure: %d, State: %d", 
            cmd_ptr->ftm_cmd_header.cmd_id,
           FTM_GSM_BER_DEV_GET_STATE(ftm_vars_p));
    }
    else
    {
      cmd_fail = FALSE;
    }

    switch ( cmd_ptr->ftm_cmd_header.cmd_id )
    {
    case FTM_LOG_GSM_BER_RX_LEVEL_REQ:
      rsp_pkt = ftm_gsm_ber_create_rx_lev_response(cmd_ptr,
                                              cmd_fail,
                                              cmd_ptr->ftm_cmd_header.cmd_id,
                                              0,
						ftm_gsm_ber_rx_lev_rpt);
      break;
    default:	
    rsp_pkt = ftm_gsm_ber_create_gen_response(cmd_ptr,
                                              cmd_fail,
                                              cmd_ptr->ftm_cmd_header.cmd_id,
                                              0);
    }
  }

  return rsp_pkt;
}


/*===========================================================================

FUNCTION       ftm_gsm_ber_clear_sync_status

DESCRIPTION    This function clears all entries in the synchronous status 
               array.  This behavior is called by the FTM_GSM_BER_EVENT_STATUS_CLEAR_REQ
               command.

DEPENDENCIES
  none

===========================================================================*/
void ftm_gsm_ber_clear_sync_status( void )
{
    uint8 Index;

   // Interate through the array that stores the synchronous status items
   for ( Index = 0; Index < FTM_GSM_BER_SYNC_STATUS_MAX; Index = Index + 1 )
   {
      // Reset the updated flag
      GSM_BER_Sync_Status[ Index ].has_updated = FALSE;

      // Reset the event status 
      GSM_BER_Sync_Status[ Index ].status = FTM_GSM_BER_SYNC_STATUS_GEN_FAILURE;
   }
}

/*===========================================================================

FUNCTION       ftm_gsm_set_exit_cmd_sig

DESCRIPTION    This function sets the FTM_GSM_BER_EXIT_CMD_Q_SIG
               signal which is called by GL1 and when complete
               GSM tech exit is done

DEPENDENCIES
  none

===========================================================================*/
void ftm_gsm_set_exit_cmd_sig( void )
{
   ftm_sig_rex_set(FTM_GSM_BER_EXIT_CMD_Q_SIG);
}

#endif /* FEATURE_FTM_GSM_BER */

