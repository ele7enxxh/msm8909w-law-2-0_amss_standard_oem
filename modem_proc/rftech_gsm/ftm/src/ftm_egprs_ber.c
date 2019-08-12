/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2003 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2007 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2011 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2012 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
 
                           EDIT HISTORY FOR MODULE
                           
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.1

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/ftm/src/ftm_egprs_ber.c#1 $ 
  $DateTime: 2016/12/13 07:59:51 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/21/13   tws     Replace FEATURE_TRITON_MODEM with FEATURE_DUAL_SIM
04/15/13   ec      Featurise geran_test_mode apis to use as_id only for Triton 
04/11/13   ec	     Update geran_test_mode_api calls to use as_id.  
04/11/13   ec	     Fix Offtarget Error
04/03/13   ec	     Retrieve ns_active_device at function entry instead of at multiple points,
                   pass rfm_dev from dispatch layer. Map as_id at geran interface layer in 
                   advance of passing to geran.
02/05/13   ec      Fix KW Errors
01/16/13   ec	     Add framework for multiple devices in Non Signalling       
11/21/12   tws     Remove legacy GERAN API includes.
05/11/12   dej     Featurize ftm_egprs_ber_send_msg_to_gl1 prototype to remove compiler warning
03/22/12   dej     Initializing variable req to zero to avoid storing junk values     
01/05/12   sr      geran api changes for non-signaling.
10/04/11   og      Protect the setting and clearing of rex sigs using a mutex.     
07/12/11   st      Update Timing Advance Value in Slot Reconfiguration     
05/16/11   av      Fix for starting NS EDGE call and vary the power based on gamma value.     
                   Fix is to populate some of the layer1 sys info parameters. 
03/30/11   ems     Support for EGPRS Rx Level
02/10/11   ems     Correct timing loop for Synchronous diagnostic SE BER/BLER report
11/01/10   ems     Support for FTM Sequencer 
08/20/10   tsr     Remove INTLOCK/INTFREE code
08/13/10   tsr     Disable ASRB Loopback in dynamic TBF config
02/04/10   sr      Lint & kw fixes 
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
12/28/09   sr      High/Medium lint errors fix
12/22/09   sr      changed the debug msg macro from legacy to new one.
10/22/07   ad      SE BER support from 6260: Add V2 command for SRB 
                   configuration (more UL/DL slots)
08/06/07   jfc     Featurized code for EDGE-only builds
05-24-06   jac     Fixed LINT high errors.
11-02-05   dp      Add ability to configure gamma parameter for each uplink slot
10-17-05   dp      Add ftm_umts_ber.h include file.
10-15-05   dp      Add multislot (max 2 UL/DL) support.
09-26-05   dp      Add handling of L1 reset IND/CNF.  Add some more comments.
07-20-05   dp      Updated state handling, uplink gamma handling
06-08-05   dp      Created
===========================================================================*/


/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/


#include "rfa_variation.h"
#include "target.h"
#include "msg.h"
#include "customer.h"
#include "ftm.h"
#include "ftm_task.h"
#include "rf_test_task_v.h"
#include "ftmicap.h"
#include "ftm_umts_ber.h"
#include "ftm_gsm_ber.h"
#include "ftm_egprs_ber.h"
#include "ftmdiag.h"
#include "diagpkt.h"
#include "ftm_gsm_rfctl.h"


#include "queue.h"
#include "sys_type.h"
#include "sys_cnst.h"

#include "geran_test_mode_api.h"
#include "ms.h"

#ifdef FEATURE_WCDMA
#include "l1task.h"
#endif /*FTM_HAS_WCDMA*/



#ifdef FEATURE_FTM_GSM_BER
#ifdef FEATURE_GSM_EGPRS
#ifdef FEATURE_FTM_EGPRS_BER

/*===========================================================================

                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/

/* Structure for events that will be reported synchronously */
typedef struct 
{
	uint8 has_updated;
	ftm_egprs_ber_sync_status_type status;

} ftm_egprs_ber_sync_status_struct;

/* array to record the events that will be reported synchronously 
   via FTM_EGPRS_BER_EVENT_STATUS_REQ
*/
ftm_egprs_ber_sync_status_struct EGPRS_BER_Sync_Status[  FTM_EGPRS_BER_SYNC_STATUS_MAX ];

/* State of the FTM EGPRS scenario */

static ftm_egprs_ber_status_info_type  ftm_egprs_ber_status = { FTM_EGPRS_BER_STATE_NULL };
#define FTM_EGPRS_BER_SET_STATE( s)  ( ftm_egprs_ber_status.current_state = s)
#define FTM_EGPRS_BER_GET_STATE()    ( ftm_egprs_ber_status.current_state)

/* New macros to access device-specific status */
#define FTM_EGPRS_BER_SET_DEV_STATE(ftm_vars_p, s)  ( ftm_vars_p->current_egprs_state = s)
#define FTM_EGPRS_BER_GET_DEV_STATE(ftm_vars_p)    ( ftm_vars_p->current_egprs_state )

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

/* Message senders/handlers */
static void ftm_egprs_ber_send_msg_to_gpl1( rfm_device_enum_type rfm_dev, void *msg);
static void ftm_egprs_ber_free_buffer( void *msg);
 

/* Functions to send primitives as MAC would */

static void ftm_egprs_ber_send_MAC_L1_FREQ_CONFIG( rfm_device_enum_type rfm_dev );
static void ftm_egprs_ber_send_MAC_L1_DL_TBF_CONFIG( rfm_device_enum_type rfm_dev );
static void ftm_egprs_ber_send_MAC_L1_RELEASE( rfm_device_enum_type rfm_dev );
static void ftm_egprs_ber_send_MAC_L1_DYNAMIC_TBF_CONFIG( rfm_device_enum_type rfm_dev );
static void ftm_egprs_ber_send_MAC_L1_TS_RECONFIG( rfm_device_enum_type rfm_dev );

/* Functions to send primitives as RR would */

static void ftm_egprs_ber_send_MPH_START_GPRS_IDLE_MODE_REQ( rfm_device_enum_type rfm_dev );


extern void gpl1_l1_test_mode_stop_ind( void );          
extern void gpl1_l1_test_mode_start_ind( void );         

/* Utility functions */

static power_control_gamma_T *ftm_egprs_ber_get_power_control_gamma_ptr( power_control_params_T *pwr_ctrl_params,
                                                                         uint8 slot_number);
void ftm_egprs_ber_clear_sync_status( void);

/*! @brief Used for directing messages from L1 to the correct device instance */
extern rfm_device_enum_type ftm_gsm_ns_active_device;

/*===========================================================================

FUNCTION      FTM_EGPRS_BER_CREATE_GEN_RESPONSE

DESCRIPTION   Creates a generic response packet with a status field.  
              Typically all immediate responses from FTM after sending
              down an EGPRS command packet will use this response
              mechanism.


DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS   
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_egprs_ber_create_gen_response( const ftm_egprs_ber_pkt_type *cmd_ptr,
                                                    uint16 status,
                                                    uint16 cid,
                                                    uint16 scid)
{

  ftm_rsp_pkt_type  gen_rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_egprs_ber_gen_rsp_pkt_type   *gen_rsp_payload;


  gen_rsp = ftmdiag_create_new_pkt( sizeof( ftm_egprs_ber_gen_rsp_pkt_type));

  if ( gen_rsp.pkt_payload != NULL)
  {
    gen_rsp_payload = ( ftm_egprs_ber_gen_rsp_pkt_type *) gen_rsp.pkt_payload;

    gen_rsp_payload->header                  = cmd_ptr->header;
    gen_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_EGPRS_BER_GEN_RESPONSE;
    gen_rsp_payload->ftm_rsp_header.rsp_cid  = cid;
    gen_rsp_payload->ftm_rsp_header.rsp_scid = scid;
    gen_rsp_payload->status                  = status;

  }

  return gen_rsp;
}



/*===========================================================================

FUNCTION     FTM_EGPRS_BER_LOG_GEN_CNF

DESCRIPTION
             Logs a generic confirmation message to the user (PC side).  This
             is used when the indication is that a command was completed,
             but no payload needs to be sent.
               
DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_egprs_ber_log_gen_cnf( ftm_log_egprs_ber_id_type  id)
{

#ifdef FTM_HAS_LOGGING 

  ftm_log_egprs_ber_gen_cnf        *gen_cnf_log;


  gen_cnf_log = ( ftm_log_egprs_ber_gen_cnf *) 
                ftm_log_malloc( (uint16)FTM_LOG_GSM_BER, 
                                sizeof( ftm_log_egprs_ber_gen_cnf) );

  if ( gen_cnf_log != NULL)
  {
    gen_cnf_log->log_id = id;

    log_commit( gen_cnf_log);
  }
#endif

}



/*===========================================================================

FUNCTION     FTM_EGPRS_BER_LOG_GEN_CNF_WITH_STATUS

DESCRIPTION
             Logs a generic confirmation message to the user (PC side).  This
             is used when the indication is that a command was completed, and
             pass a generic status byte to the user.
               
DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_egprs_ber_log_gen_cnf_with_status( ftm_log_egprs_ber_id_type  id, uint8 status)
{

#ifdef FTM_HAS_LOGGING 
  ftm_log_egprs_ber_gen_cnf_with_status        *gen_cnf_log;

  gen_cnf_log = ( ftm_log_egprs_ber_gen_cnf_with_status *) 
                ftm_log_malloc( (uint16)FTM_LOG_GSM_BER, 
                                sizeof( ftm_log_egprs_ber_gen_cnf_with_status) );

  if ( gen_cnf_log != NULL)
  {
    gen_cnf_log->log_id = id;
    gen_cnf_log->status = status;

    log_commit( gen_cnf_log);
  }
#endif

}

/*===========================================================================

FUNCTION         FTM_EGPRS_BER_LOG_TBF_IND   

DESCRIPTION      Logs a TBF Indication, which informs the user that 
                 a particular TBF (UL or DL) has an event (such as TBF Release)
            
               
DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_egprs_ber_log_tbf_ind( ftm_log_egprs_tbf_type   type, ftm_log_egprs_tbf_event_type event )
{

#ifdef FTM_HAS_LOGGING 
  ftm_log_egprs_ber_tbf_ind_type   *egprs_ber_log;

  egprs_ber_log = (ftm_log_egprs_ber_tbf_ind_type *) 
                  ftm_log_malloc( (uint16)FTM_LOG_GSM_BER,
                                  sizeof( ftm_log_egprs_ber_tbf_ind_type));

  if ( egprs_ber_log != NULL)
  {

    egprs_ber_log->log_id    = FTM_LOG_EGPRS_BER_TBF_IND;
    egprs_ber_log->tbf_type  = type;
    egprs_ber_log->tbf_event = event;

    log_commit( egprs_ber_log);
  }
#endif

  switch ( type )
  {
  case FTM_LOG_EGPRS_BER_TBF_UL:
    if ( event == FTM_LOG_EGPRS_TBF_EVENT_ESTABLISHED )
    {
      EGPRS_BER_Sync_Status[ FTM_EGPRS_BER_UL_TBF_ESTABLISHED_SYNC_STATUS ].status = FTM_EGPRS_BER_SYNC_STATUS_SUCCESS;
      EGPRS_BER_Sync_Status[ FTM_EGPRS_BER_UL_TBF_ESTABLISHED_SYNC_STATUS ].has_updated = TRUE;
      ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );
    
    }
    else if ( event == FTM_LOG_EGPRS_TBF_EVENT_RELEASED )
    {
      EGPRS_BER_Sync_Status[ FTM_EGPRS_BER_UL_TBF_RELEASED_SYNC_STATUS ].status = FTM_EGPRS_BER_SYNC_STATUS_SUCCESS;
      EGPRS_BER_Sync_Status[ FTM_EGPRS_BER_UL_TBF_RELEASED_SYNC_STATUS ].has_updated = TRUE;
      ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );    
    
    }

    // Need to handle the error case, but unfotunately the error case is common to both establish & release commands
    break;

  case FTM_LOG_EGPRS_BER_TBF_DL:
    if ( event == FTM_LOG_EGPRS_TBF_EVENT_ESTABLISHED )
    {
      EGPRS_BER_Sync_Status[ FTM_EGPRS_BER_DL_TBF_ESTABLISHED_SYNC_STATUS ].status = FTM_EGPRS_BER_SYNC_STATUS_SUCCESS;
      EGPRS_BER_Sync_Status[ FTM_EGPRS_BER_DL_TBF_ESTABLISHED_SYNC_STATUS ].has_updated = TRUE;
      ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );    
    }
    else if ( event == FTM_LOG_EGPRS_TBF_EVENT_RELEASED )
    {
      EGPRS_BER_Sync_Status[ FTM_EGPRS_BER_DL_TBF_RELEASED_SYNC_STATUS ].status = FTM_EGPRS_BER_SYNC_STATUS_SUCCESS;
      EGPRS_BER_Sync_Status[ FTM_EGPRS_BER_DL_TBF_RELEASED_SYNC_STATUS ].has_updated = TRUE;
      ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );        
    }

    // Need to handle the error case, but unfotunately the error case is common to both establish & release commands
    break;
  } // switch ( type )

}





/*===========================================================================

FUNCTION         FTM_EGPRS_BER_LOG_TBF_IND   

DESCRIPTION      Logs a TBF Indication, which informs the user that 
                 a particular TBF (UL or DL) has an event (such as TBF Release)
            
               
DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_egprs_ber_log_serving_meas_ind( uint8 rxlev_average )
{

#ifdef FTM_HAS_LOGGING 
  ftm_log_egprs_ber_serving_meas_ind_type   *egprs_ber_log;

  egprs_ber_log = (ftm_log_egprs_ber_serving_meas_ind_type *) 
                  ftm_log_malloc( (uint16)FTM_LOG_GSM_BER,
                                  sizeof( ftm_log_egprs_ber_serving_meas_ind_type));

  if ( egprs_ber_log != NULL)
  {

    egprs_ber_log->log_id         = FTM_LOG_EGPRS_BER_SERVING_MEAS_IND;
    egprs_ber_log->rxlev_average  = rxlev_average;

    log_commit( egprs_ber_log);
  }
#endif

}

/*===========================================================================

FUNCTION        FTM_EGPRS_BER_INIT

DESCRIPTION     Initializes resources to be used during FTM EGPRS session.


DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_egprs_ber_init( void)
{

  uint8    buf_count;

  FTM_EGPRS_BER_SET_STATE( FTM_EGPRS_BER_STATE_NULL);


  /* Initialize Qs for sending data from L1 to FTM.  L1 can request a buffer from the
   * free Q, and send a message to FTM.  This will all be handled using static 
   * allocation, as some l1 messages are sent out of an ISR.  The buffers are of type
   * geran_test_mode_api_l1_mac_sig_t  
   */

  (void)q_init( &ftm_egprs_ber_status.ftm_egprs_ber_cmd_q);
  (void)q_init( &ftm_egprs_ber_status.ftm_egprs_ber_cmd_free_q);

  for ( buf_count = 0; buf_count < FTM_EGPRS_BER_NUM_CMD_BUFFERS; buf_count++)
  {

    (void)q_link( (void *) &ftm_egprs_ber_status.ftm_egprs_ber_l1_mac_cmd_buffers[buf_count],
                  &(ftm_egprs_ber_status.ftm_egprs_ber_l1_mac_cmd_buffers[buf_count].link) );

    q_put( &ftm_egprs_ber_status.ftm_egprs_ber_cmd_free_q,
           &(ftm_egprs_ber_status.ftm_egprs_ber_l1_mac_cmd_buffers[buf_count].link)) ;

  }


}



/*===========================================================================

FUNCTION        FTM_EGPRS_BER_HANDLE_MSGS_FROM_GL1

DESCRIPTION     Handles messages from L1 intended for RR during the
                FTM EGPRS session.  
.

DEPENDENCIES    ftm_gsm_ber_init() called
                ftm_egprs_ber_init() called
   

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_egprs_ber_handle_msgs_from_gl1( void *msg)
{

  ftm_gsm_ber_queue_type  *message = (ftm_gsm_ber_queue_type *) msg;
  IMH_T                   *header;
  ftm_gsm_rfctl_vars_s    *ftm_vars_p;

  rfm_device_enum_type rfm_dev = ftm_gsm_ns_active_device;


  if ( rfm_dev >= RFM_MAX_WAN_DEVICES )                                                    
  {                                                                                                        
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"ftm_gsm_ns_active_device invalid! %d", rfm_dev);
    return;                                                                                                
  }                                                                                                        


  /* Use the same workaround as GSM for handling the messages */
  ftm_vars_p= ftm_gsm_get_vars_ptr(rfm_dev);

  header = ( IMH_T *) &message->msg.ftm_pseudo_rr_msg.message_header;

  if ( header->message_set == MS_RR_L1)
  {

    switch ( header->message_id)
    {


    /*************************************************************************
    *              MPH_START_GSM_MODE_CNF                                    *                          *
    *              MPH_STOP_GSM_MODE_CNF                                     *                              *
    *              MPH_SELECT_SPECIFIC_BCCH_CNF                              *
    *              MPH_START_IDLE_MODE_CNF                                   *
    *                                                                        *
    **************************************************************************/
    
    case MPH_START_GSM_MODE_CNF:
    case MPH_STOP_GSM_MODE_CNF:
    case MPH_SELECT_SPECIFIC_BCCH_CNF:
      {

        FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_NULL);

        break;
      }



      /*************************************************************************
      *              MPH_L1_TEST_MODE_START_IND                                *
      *                                                                        *
      **************************************************************************/

    case MPH_L1_TEST_MODE_START_IND:
      {

        break;
      }
    case MPH_L1_TEST_MODE_STOP_IND:
      {
        break;
      }



      /*************************************************************************
      *              MPH_START_IDLE_MODE_CNF                                   *
      *                                                                        *
      **************************************************************************/


    case MPH_START_IDLE_MODE_CNF:
      {

        if ( FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p) == FTM_EGPRS_BER_STATE_GPRS_IDLE_PENDING)
        {

          MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Received GPRS Start IDLE CNF",0);

          FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_GPRS_IDLE);

          ftm_gsm_ber_set_dev_state( rfm_dev, FTM_GSM_BER_STATE_GPRS_IDLE);

          /* Logging not necessary, will be logged as GSM Start IDLE Mode CNF Log code */
        }
        else
        {
          FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_NULL);
        }

        break;
      }



      /*************************************************************************
      *              MPH_SERVING_MEAS_IND                                      *
      *                                                                        *
      **************************************************************************/

    case MPH_SERVING_MEAS_IND:
      {

        uint8   rxlev_average;

        rxlev_average = message->msg.ftm_pseudo_rr_msg.mph_serving_meas_ind.rxlev_average;               
        ftm_egprs_ber_log_serving_meas_ind( rxlev_average);

        break;
      }

      /*************************************************************************
      *              MPH_RESET_IND/MPH_RESET_CNF                               *
      *                                                                        *
      **************************************************************************/

    case MPH_RESET_IND:
    case MPH_RESET_CNF:
      {

        MSG_1(MSG_SSID_FTM, MSG_LEGACY_LOW,"Received Reset IND or CNF from L1",0);
        FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_NULL);
        break;
      }




      /*************************************************************************
      *                                                                        *
      *                                                                        *
      **************************************************************************/

    default:
      {            

        break;
      }

    }
  }


}



/*===========================================================================

FUNCTION        FTM_EGPRS_BER_HANDLE_MSGS_FROM_GPL1

DESCRIPTION     Handles the relevant message types for the FTM EGPRS BER
                scenario.  The typical sequence, assuming we are in GPRS IDLE
                is:
                
                1) Send DL Freq Config (not in this function)
                2) Handle DL Freq Config Assignment Confirmation here.  If 
                   success, send the DL TBF Config.
                3) Handle DL TBF confirmation.  If success, send UL Freq
                   Config
                4) Handle UL Freq Config Assignment Confirmation here.  If
                   success, send the UL DYNAMIC TBF config.
            
                If a DL TBF is already estabished, we start by sending the UL
                TBF config (end of step 3 above).         
                   
                When tearing down the links.
                
                1) Send UL Release Message to L1 (not in this function)
                2) Handle UL Release Confirmation here.  If only tearing
                   down UL, stop here. Else, proceed to send DL Release Message.
                3) Handle DL Release Confirmation.  If success, we are now
                   back to GPRS IDLE.

                Typically, if any message indicates the process had some sort of
                error, the code will simply fall back to the previous state before
                the configuration message was sent.  For example, if we fail at the
                UL TBF release, then we go back to the UL TBF Established state.


DEPENDENCIES    ftm_egprs_ber_init() called.
   

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_egprs_ber_handle_msgs_from_gpl1( void)
{
  geran_test_mode_api_l1_mac_sig_t  *message = NULL;

  ftm_gsm_rfctl_vars_s *ftm_vars_p;

  sys_modem_as_id_e_type as_id;

  rfm_device_enum_type rfm_dev = ftm_gsm_ns_active_device;

  as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

  if ( rfm_dev >= RFM_MAX_WAN_DEVICES )
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,
          "rfm_dev invalid! %d", rfm_dev);
    return;
  }

  ftm_vars_p = ftm_gsm_get_vars_ptr( rfm_dev );


  /* Fetch all the messages off the command Q */

  while ( (message = q_get( &ftm_egprs_ber_status.ftm_egprs_ber_cmd_q)) != NULL)
  {

    /* Verify they are from L1 destined for MAC */

    if ( message->task_id == MS_MAC_L1)
    {
      /* Parse out each message */

      switch ( message->sig_id)
      {

      /* Handle Frequency Assignment confirmations from L1 */


      /*************************************************************************
      *              L1_MAC_FREQ_ASS_CNF                                       *
      *                                                                        *
      **************************************************************************/
      
      case L1_MAC_FREQ_ASS_CNF:
        {

          MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH, 
                "Received L1_MAC_FREQ_ASS_CNF, status = %d state=%d", 
                message->msg.tbf_freq_ass_cnf.band_supported,
                FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p));

          if ( message->msg.tbf_freq_ass_cnf.tbf_freq_cnf   == DL_TBF_FREQ_CONF                          &&
               FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p)                    == FTM_EGPRS_BER_STATE_DL_TBF_ESTABLISHING   )
          {

            if ( message->msg.tbf_freq_ass_cnf.band_supported == TRUE)
            {

              /* Ready to establish the DL TBF */

              ftm_egprs_ber_send_MAC_L1_DL_TBF_CONFIG( rfm_dev );
            }
            else
            {

              FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_GPRS_IDLE);

              ftm_egprs_ber_log_tbf_ind( FTM_LOG_EGPRS_BER_TBF_DL, FTM_LOG_EGPRS_TBF_EVENT_ERROR);
            }
          }
          else if ( message->msg.tbf_freq_ass_cnf.tbf_freq_cnf   == UL_TBF_FREQ_CONF                        &&
                    FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p)                    == FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHING )
          {

            if ( message->msg.tbf_freq_ass_cnf.band_supported == TRUE)
            {

              /* Ready to establish the UL TBF */

              ftm_egprs_ber_send_MAC_L1_DYNAMIC_TBF_CONFIG( rfm_dev );
            }
            else
            {

              FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_DL_TBF_ESTABLISHED);
              ftm_egprs_ber_log_tbf_ind( FTM_LOG_EGPRS_BER_TBF_DL, FTM_LOG_EGPRS_TBF_EVENT_ERROR);
            }
          }
          else if ( message->msg.tbf_freq_ass_cnf.tbf_freq_cnf   == UL_DL_TBF_FREQ_CONF                      &&
                    FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p)                    == FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHED )
          {

            MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH, "UL_DL_TBF_FREQ_CONF Received",0);

            if ( message->msg.tbf_freq_ass_cnf.band_supported == TRUE)
            {

              /* new freq info sent, continue with reconfig */

              ftm_egprs_ber_send_MAC_L1_TS_RECONFIG( rfm_dev );
            }
            else
            {

              ftm_egprs_ber_log_tbf_ind( FTM_LOG_EGPRS_BER_TBF_UL, FTM_LOG_EGPRS_TBF_EVENT_ERROR);
              ftm_egprs_ber_log_tbf_ind( FTM_LOG_EGPRS_BER_TBF_DL, FTM_LOG_EGPRS_TBF_EVENT_ERROR);
            }
          }
          break;
        }


        /*************************************************************************
        *              MAC_PH_CONNECT_IND                                        *
        *                                                                        *
        **************************************************************************/

        /* Handle TBF connection indications (UL or DL or UL_DL) */

      case MAC_PH_CONNECT_IND:
        {

          if ( message->msg.connect_ind.tbf_type_ind == DOWNLINK_TBF_ESTABLISHED && 
               FTM_EGPRS_BER_GET_DEV_STATE(ftm_vars_p) == FTM_EGPRS_BER_STATE_DL_TBF_ESTABLISHING   )
          {

            /* Indicate to GSM that we are in packet transfer state */

            ftm_gsm_ber_set_dev_state( rfm_dev, FTM_GSM_BER_STATE_GPRS_PDTCH);

            /* DL TBF Established */

            FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_DL_TBF_ESTABLISHED);
            ftm_egprs_ber_log_tbf_ind( FTM_LOG_EGPRS_BER_TBF_DL, FTM_LOG_EGPRS_TBF_EVENT_ESTABLISHED);

            /* Jump right to UL TBF establishment */

            FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHING);

            /* Send the Freq config for UL TBF */

            ftm_egprs_ber_send_MAC_L1_FREQ_CONFIG( rfm_dev );

          }
          else if ( message->msg.connect_ind.tbf_type_ind == UPLINK_TBF_ESTABLISHED &&
                    FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p)             == FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHING )
          {

            /* UL TBF Established */

            ftm_egprs_ber_log_tbf_ind( FTM_LOG_EGPRS_BER_TBF_UL, FTM_LOG_EGPRS_TBF_EVENT_ESTABLISHED);                        

            FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHED);

          }
          else if ( message->msg.connect_ind.tbf_type_ind == UPLINK_DOWNLINK_TBF_ESTABLISHED &&
                    FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p)             == FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHED )
          {

            /* Finished a reconfigure, send logs */

            ftm_egprs_ber_log_tbf_ind( FTM_LOG_EGPRS_BER_TBF_DL, FTM_LOG_EGPRS_TBF_EVENT_ESTABLISHED);                        
            ftm_egprs_ber_log_tbf_ind( FTM_LOG_EGPRS_BER_TBF_UL, FTM_LOG_EGPRS_TBF_EVENT_ESTABLISHED);                       
#ifdef FEATURE_DUAL_SIM 
            geran_test_mode_api_restart_srb_loop( as_id ); /* Restart the SRB loop after a TS_RECONFIG */
#else
	    as_id = as_id;
            geran_test_mode_api_restart_srb_loop(); /* Restart the SRB loop after a TS_RECONFIG */
#endif /* FEATURE_DUAL_SIM */
          }


          MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,
                "Received MAC_PH_CONNECT_IND: type=%d state=%d", 
                message->msg.connect_ind.tbf_type_ind, FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p)); 
          break;
        }

        /*************************************************************************
        *              L1_MAC_TBF_REL_CONFIRM                                    *
        *                                                                        *
        **************************************************************************/

        /* Handle TBF release confirmations (UL or DL) */

      case L1_MAC_TBF_REL_CONFIRM:
        {

          if ( message->msg.tbf_released.tbf_released == DL_TBF_RELEASE_CONF &&
               FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p)              == FTM_EGPRS_BER_STATE_DL_TBF_RELEASING)
          {

            ftm_egprs_ber_log_tbf_ind( FTM_LOG_EGPRS_BER_TBF_DL, FTM_LOG_EGPRS_TBF_EVENT_RELEASED);

            /* Seems to automatically go back to GPRS idle after releasing DL TBF */

            FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_GPRS_IDLE);

            /* Indicate to GSM that we are in GPRS Idle */

            ftm_gsm_ber_set_dev_state( rfm_dev, FTM_GSM_BER_STATE_GPRS_IDLE );

          }
          else if ( message->msg.tbf_released.tbf_released == UL_DL_TBF_RELEASE_CONF &&
                    FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p)              == FTM_EGPRS_BER_STATE_UL_DL_TBF_RELEASING)
          {

            ftm_egprs_ber_log_tbf_ind( FTM_LOG_EGPRS_BER_TBF_DL, FTM_LOG_EGPRS_TBF_EVENT_RELEASED);
            ftm_egprs_ber_log_tbf_ind( FTM_LOG_EGPRS_BER_TBF_UL, FTM_LOG_EGPRS_TBF_EVENT_RELEASED);

            /* Seems to automatically go back to GPRS idle after releasing all TBF */

            FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_GPRS_IDLE);

            /* Indicate to GSM that we are in GPRS Idle */

            ftm_gsm_ber_set_dev_state( rfm_dev, FTM_GSM_BER_STATE_GPRS_IDLE );

          }

          MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Received MAC_TBF_REL_CONFIRM: type=%d state=%d", 
                message->msg.tbf_released.tbf_released,FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p));
          break;
        }

        /*************************************************************************
        *              L1_MAC_USF_DETECTED                                       *
        *                                                                        *
        **************************************************************************/

        /* Handle USF Detected indication */

      case L1_MAC_USF_DETECTED:
        {

          MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Received L1_MAC_USF_DETECTED",0);
          break;
        }

        /*************************************************************************
        *              MAC_PH_DATA_IND                                           *
        *                                                                        *
        **************************************************************************/

        /* Handle PH_DATA_IND destined for MAC */

      case MAC_PH_DATA_IND:
        {

          ftm_egprs_ber_log_gen_cnf( FTM_LOG_EGPRS_BER_MAC_PH_DATA_IND);

          MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Received MAC_PH_DATA_IND, FN=%d, TN=%d", 
                message->msg.data_ind.frame_no, message->msg.data_ind.timeslot);

          break;
        }

        /*************************************************************************
        *              DEFAULT                                                   *
        *                                                                        *
        **************************************************************************/

      default:
        {
          MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Received unknown message from GPL1: %d", 
                message->sig_id); 
          break;
        }
      }
    }
    else
    {

      MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Received unknown message %d with task id: %d", 
            message->sig_id, message->task_id);

    }


    /* Return the messages to the Command Free Q */

    ftm_egprs_ber_free_buffer( ( void *) message);
  }


}



/*===========================================================================

FUNCTION        FTM_EGPRS_BER_SEND_MSG_TO_GPL1

DESCRIPTION     Sends a message to GPL1.  Uses the normal MAC->L1 interface.
.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
static void ftm_egprs_ber_send_msg_to_gpl1( rfm_device_enum_type rfm_dev, void *msg)
{
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

#ifdef FEATURE_DUAL_SIM 
  geran_test_mode_api_gprs_l1_mac_acc_func( ( geran_test_mode_api_mac_l1_sig_t *) msg, as_id );
#else
  as_id = as_id;
  geran_test_mode_api_gprs_l1_mac_acc_func( ( geran_test_mode_api_mac_l1_sig_t *) msg );
#endif /* FEATURE_DUAL_SIM */
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
void *ftm_egprs_ber_get_cmd_buf( void)
{

  /* Get a buffer from the Free Q */

  void *msg = NULL;

  msg = q_get( &ftm_egprs_ber_status.ftm_egprs_ber_cmd_free_q);

  return( msg);

}




/*===========================================================================

FUNCTION        FTM_EGPRS_BER_SEND_MSG_TO_FTM

DESCRIPTION     Takes a populated message and sends it to the FTM task via
                the q_xxxx() interfaces (uses command Q).  Sets signal for FTM task to 
                handle the message. Called from GERAN MAC gprs_mac_l1_acc_func()
.

DEPENDENCIES    buffer has been previously "allocated" using 
                ftm_egprs_ber_get_cmd_buf()
   

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_egprs_ber_send_msg_to_ftm( void *msg)
{

  /* Put the message onto the Cmd Q, and set the signal */
  geran_test_mode_api_l1_mac_sig_t  *message = ( geran_test_mode_api_l1_mac_sig_t *) msg;

  RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

  (void)q_link( ( void *) message, &message->link);

  q_put( &ftm_egprs_ber_status.ftm_egprs_ber_cmd_q, &message->link);

  (void)rex_set_sigs( &ftm_tcb, FTM_EGPRS_BER_CMD_Q_SIG);

  RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);

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
static void ftm_egprs_ber_free_buffer( void *msg)
{

  /* Return a buffer to the free Q for future use */
  geran_test_mode_api_l1_mac_sig_t  *message = ( geran_test_mode_api_l1_mac_sig_t *) msg;

  (void)q_link( ( void *) message, &message->link);

  q_put( &ftm_egprs_ber_status.ftm_egprs_ber_cmd_free_q, &message->link);

}




/*===========================================================================

FUNCTION        FTM_EGPRS_BER_SEND_MAC_L1_FREQ_CONFIG

DESCRIPTION     Sends a MAC_L1_FREQ_CONFIG command to configure either the 
                UL or DL TBF frequency info.  In the FTM scenario, only one
                frequency is used (no hopping).
.

DEPENDENCIES    ftm_egprs_ber_init() called
                L1 should be in GPRS IDLE or in transfer mode.
   

RETURN VALUE
   None.

SIDE EFFECTS    Sends a configuration message to L1.
   

===========================================================================*/
static void ftm_egprs_ber_send_MAC_L1_FREQ_CONFIG( rfm_device_enum_type rfm_dev )
{
  geran_test_mode_api_mac_l1_sig_t       config_data;

  ftm_gsm_rfctl_vars_s * ftm_vars_p = ftm_gsm_get_vars_ptr(rfm_dev);

  if ( FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p) == FTM_EGPRS_BER_STATE_DL_TBF_ESTABLISHING)
  {

    config_data.mac_l1_freq_config.tbf_freq_cnf               = DL_FREQ_CONFIG;
    config_data.mac_l1_freq_config.frequency_information.tsc  = ftm_vars_p->current_egprs_dl_tbf_info.dl_tsc;
  }
  else if ( FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p) == FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHING)
  {

    config_data.mac_l1_freq_config.tbf_freq_cnf               = UL_FREQ_CONFIG;
    config_data.mac_l1_freq_config.frequency_information.tsc  = ftm_vars_p->current_egprs_ul_tbf_info.ul_tsc;
  }
  else if ( FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p) == FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHED)
  {

    config_data.mac_l1_freq_config.tbf_freq_cnf               = UL_DL_FREQ_CONFIG;
    config_data.mac_l1_freq_config.frequency_information.tsc  = ftm_vars_p->current_egprs_ul_tbf_info.ul_tsc;
  }


  config_data.mac_l1_freq_config.reuse_sb_frq_alloc                 = FALSE;
  config_data.mac_l1_freq_config.frequency_information.hopping_flag = FALSE;
  config_data.mac_l1_freq_config.frequency_information.hsn          = 0;
  config_data.mac_l1_freq_config.frequency_information.maio         = 0;

  /* Only use channel/band info provided in DL TBF config */

  config_data.mac_l1_freq_config.frequency_information.frequency_list.no_of_items     = 1;
  config_data.mac_l1_freq_config.frequency_information.frequency_list.channel[0].num  = ftm_vars_p->current_egprs_dl_tbf_info.dl_arfcn.channel;      
  config_data.mac_l1_freq_config.frequency_information.frequency_list.channel[0].band = (sys_band_T)ftm_vars_p->current_egprs_dl_tbf_info.dl_arfcn.band;

  config_data.mac_l1_freq_config.message_header.message_id  = (byte)MAC_L1_FREQ_CONFIG;
  config_data.mac_l1_freq_config.message_header.message_set = MS_MAC_L1;

  /* Send function utility will set the message header size */

  MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Sending MAC_L1_FREQ_CONFIG",0);

  ftm_egprs_ber_send_msg_to_gpl1( rfm_dev, ( void *) &config_data);

}







/*===========================================================================

FUNCTION        FTM_EGPRS_BER_SEND_MAC_L1_TS_RECONFIG

DESCRIPTION     
.

DEPENDENCIES    ftm_egprs_ber_init() called
                L1 should be in transfer mode.
   

RETURN VALUE
   None.

SIDE EFFECTS    Sends a configuration message to L1.
   

===========================================================================*/
static void ftm_egprs_ber_send_MAC_L1_TS_RECONFIG( rfm_device_enum_type rfm_dev )
{

  geran_test_mode_api_mac_l1_sig_t       config_data;

  power_control_gamma_T    *gamma_ptr;

  uint8              slot;
  uint8              dl_ts_allocation = 0;
  uint8              usf_index;
  uint8              i;

  ftm_gsm_rfctl_vars_s * ftm_vars_p = ftm_gsm_get_vars_ptr(rfm_dev);

  /* loop over all possible slots.  The V2 command supports more slots in the interface.  Even if the 
   * original command was used, the last 2 slots in dl_slot_config array should be initialized to "inactive"
   */

  for ( i = 0; i < FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2; i++)
  {
    if ( ftm_vars_p->current_egprs_dl_tbf_info.dl_slot_config[i].slot_active == TRUE)
    {

      slot = ftm_vars_p->current_egprs_dl_tbf_info.dl_slot_config[i].slot_number;

      /* Validate DL slot number */

      if ( slot > MAX_TN)
      {
        /* turn off slot if invalid */
        ftm_vars_p->current_egprs_dl_tbf_info.dl_slot_config[i].slot_active = FALSE;
      }
      else
      {
        dl_ts_allocation |= ( 0x80 >> slot);
      }
    }
  }

  config_data.mac_l1_ts_reconfig.message_header.message_set = MS_MAC_L1;
  config_data.mac_l1_ts_reconfig.message_header.message_id  = (byte)MAC_L1_TS_RECONFIG;

  config_data.mac_l1_ts_reconfig.dl_ts_allocation           = dl_ts_allocation;
  config_data.mac_l1_ts_reconfig.dl_tfi_present             = TRUE;
  config_data.mac_l1_ts_reconfig.dl_tfi                     = ftm_vars_p->current_egprs_dl_tbf_info.dl_tfi;
  config_data.mac_l1_ts_reconfig.starting_time_present      = FALSE;
  config_data.mac_l1_ts_reconfig.rlc_mode                   = RLC_UNACK;
  config_data.mac_l1_ts_reconfig.tbf_mode                   = TBF_MODE_EGPRS;

  config_data.mac_l1_ts_reconfig.dl_ctrl_param.params_valid = FALSE;
  config_data.mac_l1_ts_reconfig.dl_pwr_params.params_valid = FALSE;

  config_data.mac_l1_ts_reconfig.global_packet_ta.tav.present = TRUE;
  config_data.mac_l1_ts_reconfig.global_packet_ta.tav.ta_value = 0;

  config_data.mac_l1_ts_reconfig.global_packet_ta.downlink_tai.present = FALSE;

  /* Uplink */

  config_data.mac_l1_ts_reconfig.global_packet_ta.uplink_tai.present   = FALSE;

  config_data.mac_l1_ts_reconfig.power_control_params.present           = TRUE;
  config_data.mac_l1_ts_reconfig.power_control_params.alpha             = 0;
  config_data.mac_l1_ts_reconfig.power_control_params.gamma_TN0.present = FALSE;
  config_data.mac_l1_ts_reconfig.power_control_params.gamma_TN1.present = FALSE;
  config_data.mac_l1_ts_reconfig.power_control_params.gamma_TN2.present = FALSE;
  config_data.mac_l1_ts_reconfig.power_control_params.gamma_TN3.present = FALSE;
  config_data.mac_l1_ts_reconfig.power_control_params.gamma_TN4.present = FALSE;
  config_data.mac_l1_ts_reconfig.power_control_params.gamma_TN5.present = FALSE;
  config_data.mac_l1_ts_reconfig.power_control_params.gamma_TN6.present = FALSE;
  config_data.mac_l1_ts_reconfig.power_control_params.gamma_TN7.present = FALSE;


  for ( i = 0; i < FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2; i++)
  {

    if ( ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[i].slot_active == TRUE)
    {

      slot = ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[i].slot_number;

      /* Validate UL slot number */

      if ( slot > MAX_TN)
      {
        /* turn off slot if invalid */
        ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[i].slot_active = FALSE;
      }
      else
      {

        gamma_ptr = ftm_egprs_ber_get_power_control_gamma_ptr( &config_data.mac_l1_ts_reconfig.power_control_params,
                                                               slot);

        if ( gamma_ptr != NULL)
        {

          gamma_ptr->present = TRUE;
          gamma_ptr->gamma   = ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[i].ul_gamma;
        }

      }
    }
  }

  config_data.mac_l1_ts_reconfig.uplink_tbf_type         = MAC_L1_MAC_MODE_DYNAMIC;
  config_data.mac_l1_ts_reconfig.uplink_ts_alloc_present = FALSE;
  config_data.mac_l1_ts_reconfig.uplink_ts_alloc         = 0;
  config_data.mac_l1_ts_reconfig.usf_granularity         = FALSE;

  for ( usf_index = 0; usf_index < MAX_TN; usf_index++)
  {

    config_data.mac_l1_ts_reconfig.usf[ usf_index].present = FALSE;
  }

  for ( i = 0; i < FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2; i++)
  {

    if ( ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[i].slot_active == TRUE)
    {

      slot = ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[i].slot_number;

      config_data.mac_l1_ts_reconfig.usf[ slot].present   = TRUE;
      config_data.mac_l1_ts_reconfig.usf[ slot].usf_value = ftm_vars_p->current_egprs_ul_tbf_info.usf;
    }
  }


  MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Sending MAC_L1_TS_RECONFIG",0);

  ftm_egprs_ber_send_msg_to_gpl1( rfm_dev, ( void *) &config_data);

}


/*===========================================================================

FUNCTION        FTM_EGPRS_BER_SEND_MAC_L1_DL_TBF_CONFIG

DESCRIPTION     Sends a L1_DL_TBF_CONFIG to establish the DL TBF.  The 
                mode is configured for EGPRS operation only.


DEPENDENCIES    ftm_egprs_ber_init() called.
                MAC_FREQ_CONFIG has been sent prior to this message to 
                set up the frequency info.


RETURN VALUE
   None.

SIDE EFFECTS    Sends configuration message to L1.
  

===========================================================================*/
static void ftm_egprs_ber_send_MAC_L1_DL_TBF_CONFIG( rfm_device_enum_type rfm_dev )
{

  geran_test_mode_api_mac_l1_sig_t       config_data;

  uint8              slot;
  uint8              i;
  uint8              ts_allocation = 0;

  ftm_gsm_rfctl_vars_s * ftm_vars_p = ftm_gsm_get_vars_ptr(rfm_dev);

  /* loop over all possible slots.  The V2 command supports more slots in the interface.  Even if the 
   * original command was used, the last 2 slots in dl_slot_config array should be initialized to "inactive"
   */

  for ( i = 0; i < FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2; i++)
  {

    if ( ftm_vars_p->current_egprs_dl_tbf_info.dl_slot_config[i].slot_active == TRUE)
    {

      slot = ftm_vars_p->current_egprs_dl_tbf_info.dl_slot_config[i].slot_number;

      /* Validate slot number */

      if ( slot > MAX_TN)
      {
        /* turn off slot if invalid */
        ftm_vars_p->current_egprs_dl_tbf_info.dl_slot_config[i].slot_active = FALSE;
      }
      else
      {
        ts_allocation |= ( 0x80 >> slot);
      }
    }
  }

  /* TBD handle more than one slot */

  config_data.mac_l1_dl_tbf_config.message_header.message_set       = MS_MAC_L1;
  config_data.mac_l1_dl_tbf_config.message_header.message_id        = (byte)MAC_L1_DL_TBF_CONFIG;


  config_data.mac_l1_dl_tbf_config.dl_tfi_present                   = TRUE;
  config_data.mac_l1_dl_tbf_config.dl_tfi                           = ftm_vars_p->current_egprs_dl_tbf_info.dl_tfi;
  config_data.mac_l1_dl_tbf_config.tbf_start_time_valid             = FALSE;
  config_data.mac_l1_dl_tbf_config.ts_allocation                    = ts_allocation;  
  config_data.mac_l1_dl_tbf_config.tbf_mode                         = TBF_MODE_EGPRS;


  config_data.mac_l1_dl_tbf_config.bep_period2.present              = FALSE;
  config_data.mac_l1_dl_tbf_config.bep_period2.bep_period2          = 0;   
  config_data.mac_l1_dl_tbf_config.link_qual_meas_mode              = 0;   
  config_data.mac_l1_dl_tbf_config.mac_mode                         = MAC_L1_MAC_MODE_DYNAMIC;   
  config_data.mac_l1_dl_tbf_config.rlc_mode                         = RLC_UNACK;  

  config_data.mac_l1_dl_tbf_config.ta_params.ta_value_ie.present    = TRUE;
  config_data.mac_l1_dl_tbf_config.ta_params.ta_value_ie.ta_value   = 0;      
  config_data.mac_l1_dl_tbf_config.ta_params.cont_ta_ie.present     = FALSE;  
  config_data.mac_l1_dl_tbf_config.ta_params.cont_ta_ie.tai         = 0;
  config_data.mac_l1_dl_tbf_config.ta_params.cont_ta_ie.ta_timeslot = 0;

  config_data.mac_l1_dl_tbf_config.downlink_pwr.params_valid        = FALSE;  
  config_data.mac_l1_dl_tbf_config.downlink_pwr.bts_pwr_ctrl_mode   = 0;
  config_data.mac_l1_dl_tbf_config.downlink_pwr.po                  = 0;
  config_data.mac_l1_dl_tbf_config.downlink_pwr.pr_mode             = 0;

  /* DL power control params necessary??? */

  config_data.mac_l1_dl_tbf_config.power_control.present            = TRUE;   
  config_data.mac_l1_dl_tbf_config.power_control.alpha              = 0;
  config_data.mac_l1_dl_tbf_config.power_control.gamma_TN0.present  = FALSE;
  config_data.mac_l1_dl_tbf_config.power_control.gamma_TN1.present  = FALSE;
  config_data.mac_l1_dl_tbf_config.power_control.gamma_TN2.present  = FALSE;
  config_data.mac_l1_dl_tbf_config.power_control.gamma_TN3.present  = FALSE;
  config_data.mac_l1_dl_tbf_config.power_control.gamma_TN4.present  = FALSE;
  config_data.mac_l1_dl_tbf_config.power_control.gamma_TN5.present  = FALSE;
  config_data.mac_l1_dl_tbf_config.power_control.gamma_TN6.present  = FALSE;
  config_data.mac_l1_dl_tbf_config.power_control.gamma_TN7.present  = FALSE;

  MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Sending MAC_L1_DL_TBF_CONFIG",0);

  /* Buffer the DL TBF config info */

  ftm_vars_p->current_egprs_l1_dl_tbf_info = config_data.mac_l1_dl_tbf_config;

  ftm_egprs_ber_send_msg_to_gpl1( rfm_dev, ( void *) &config_data );
}



/*===========================================================================

FUNCTION        FTM_EGPRS_BER_SEND_MAC_L1_RELEASE

DESCRIPTION     Sends a MAC_L1_RELEASE message to release either the UL
                or DL TBF, depending on the current state of the FTM
                scenario.
.

DEPENDENCIES    ftm_egprs_ber_init() called.
                TBF to be released is currently in operation.  
        

RETURN VALUE
   None.

SIDE EFFECTS    Sends configuration message to L1.
              

===========================================================================*/
static void ftm_egprs_ber_send_MAC_L1_RELEASE( rfm_device_enum_type rfm_dev )
{

  geran_test_mode_api_mac_l1_sig_t       config_data;

  ftm_gsm_rfctl_vars_s * ftm_vars_p = ftm_gsm_get_vars_ptr(rfm_dev);

  if ( FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p) <= FTM_EGPRS_BER_STATE_GPRS_IDLE)
  {

    return;
  }

  config_data.mac_l1_release.message_header.message_set = MS_MAC_L1;
  config_data.mac_l1_release.message_header.message_id  = (byte)MAC_L1_RELEASE;

  if ( FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p) == FTM_EGPRS_BER_STATE_DL_TBF_ESTABLISHED)
  {

    config_data.mac_l1_release.release_tbf = DL_TBF_NORMAL;

    FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_DL_TBF_RELEASING);
  }
  else if ( FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p) == FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHED)
  {

    config_data.mac_l1_release.release_tbf = UL_AND_DL_TBF_NORMAL;

    FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_UL_DL_TBF_RELEASING);
  }

  MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Sending MAC_L1_RELEASE type=%d state=%d", 
        config_data.mac_l1_release.release_tbf, FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p));

  ftm_egprs_ber_send_msg_to_gpl1( rfm_dev, ( void *) &config_data );
}




/*===========================================================================

FUNCTION        FTM_EGPRS_BER_SEND_MPH_START_GPRS_IDLE_MODE_REQ

DESCRIPTION     Sends a fixed message to GL1 to get into GPRS IDLE mode 
                using the 51 multiframe CCCH.
.

DEPENDENCIES    GL1 is synchronized to BCCH already.  
    

RETURN VALUE
   None.

SIDE EFFECTS    Sends configuration message to L1.
              

===========================================================================*/
static void ftm_egprs_ber_send_MPH_START_GPRS_IDLE_MODE_REQ( rfm_device_enum_type rfm_dev)
{
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

#ifdef FEATURE_DUAL_SIM 
  geran_test_mode_api_send_FTM_START_GPRS_IDLE_MODE_REQ( as_id );
#else
  as_id = as_id;
  geran_test_mode_api_send_FTM_START_GPRS_IDLE_MODE_REQ();
#endif /* FEATURE_DUAL_SIM */  

}





/*===========================================================================

FUNCTION        FTM_EGPRS_BER_GET_POWER_CONTROL_GAMMA_PTR

DESCRIPTION     Utility function to get pointer to gamma structure corresponding
                to slot of interest.
.

DEPENDENCIES    
       

SIDE EFFECTS   
   

===========================================================================*/
static power_control_gamma_T *ftm_egprs_ber_get_power_control_gamma_ptr( power_control_params_T *pwr_ctrl_params,
                                                                         uint8 slot_number)
{

  if ( slot_number == 0)
  {
    return &pwr_ctrl_params->gamma_TN0;
  }
  else if ( slot_number == 1)
  {
    return &pwr_ctrl_params->gamma_TN1;        
  }
  else if ( slot_number == 2)
  {
    return &pwr_ctrl_params->gamma_TN2;        
  }
  else if ( slot_number == 3)
  {
    return &pwr_ctrl_params->gamma_TN3;        
  }
  else if ( slot_number == 4)
  {
    return &pwr_ctrl_params->gamma_TN4;        
  }
  else if ( slot_number == 5)
  {
    return &pwr_ctrl_params->gamma_TN5;        
  }
  else if ( slot_number == 6)
  {
    return &pwr_ctrl_params->gamma_TN6;        
  }
  else if ( slot_number == 7)
  {
    return &pwr_ctrl_params->gamma_TN7;        
  }
  else
  {
    return NULL;        
  }
}
/*===========================================================================

FUNCTION        FTM_EGPRS_BER_SEND_MAC_L1_DYNAMIC_TBF_CONFIG

DESCRIPTION     Sets up a dynamic TBF configuration to establish an uplink 
                TBF.  In this FTM scenario, the UL TBF is only established
                after a DL TBF has been established.  This UL configuration
                is set to use the SRB test mode.
.

DEPENDENCIES    DL TBF has been established
                

RETURN VALUE
   None.

SIDE EFFECTS    Sends configuration message to L1.
   

===========================================================================*/
static void ftm_egprs_ber_send_MAC_L1_DYNAMIC_TBF_CONFIG( rfm_device_enum_type rfm_dev )
{
  geran_test_mode_api_mac_l1_sig_t             config_data;

  uint8                    usf_index;
  uint8                    slot;
  uint8                    i;
  power_control_gamma_T    *gamma_ptr;

  ftm_gsm_rfctl_vars_s * ftm_vars_p = ftm_gsm_get_vars_ptr(rfm_dev);

  /* Validate UL slot number */

  for ( i = 0; i < FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2; i++)
  {
    if ( ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[i].slot_active == TRUE)
    {
      if ( ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[i].slot_number > MAX_TN)
      {
        /* turn off slot if invalid */
        ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[i].slot_active = FALSE;
      }
    }
  }

  config_data.mac_l1_dynamic_tbf_config.message_header.message_set                         = MS_MAC_L1;
  config_data.mac_l1_dynamic_tbf_config.message_header.message_id                          = (byte)MAC_L1_DYNAMIC_TBF_CONFIG;

  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.bep_period2.present                    = FALSE;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.bep_period2.bep_period2                = 0;

  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.dl_ctrl_param.params_valid             = FALSE;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.dl_ctrl_param.po                       = 0;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.dl_ctrl_param.pr_mode                  = 0;

  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.power_control_params.present           = TRUE;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.power_control_params.alpha             = 0;

  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.power_control_params.gamma_TN0.present = FALSE;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.power_control_params.gamma_TN1.present = FALSE;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.power_control_params.gamma_TN2.present = FALSE;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.power_control_params.gamma_TN3.present = FALSE;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.power_control_params.gamma_TN4.present = FALSE;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.power_control_params.gamma_TN5.present = FALSE;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.power_control_params.gamma_TN6.present = FALSE;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.power_control_params.gamma_TN7.present = FALSE;


  for ( i = 0; i < FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2; i++)
  {

    if ( ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[i].slot_active == TRUE)
    {

      slot = ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[i].slot_number;
      gamma_ptr = ftm_egprs_ber_get_power_control_gamma_ptr( &config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.power_control_params,
                                                             slot);

      if ( gamma_ptr != NULL)
      {

        gamma_ptr->present = TRUE;
        gamma_ptr->gamma   = ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[i].ul_gamma;
      }
    }
  }

  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.rlc_mode              = RLC_UNACK;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.starting_time         = 0;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.tbf_mode              = TBF_MODE_EGPRS;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.srb_mode              = FALSE;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.tbf_start_time_valid  = FALSE;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.test_mode             = TEST_MODE_SRB;

  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.timing_advance_params.cont_ta_ie.present     = FALSE;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.timing_advance_params.cont_ta_ie.ta_timeslot = 0;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.timing_advance_params.cont_ta_ie.tai         = 0;

  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.timing_advance_params.ta_value_ie.present = TRUE;
  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.timing_advance_params.ta_value_ie.ta_value = 0;

  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.ts_offset = 0;

  config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.usf_granularity = FALSE;

  for ( usf_index = 0; usf_index < MAX_TN; usf_index++)
  {

    config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.usf[ usf_index].present = FALSE;
  }

  for ( i = 0; i < FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2; i++)
  {
    if ( ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[i].slot_active == TRUE)
    {

      slot = ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[i].slot_number;
      config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.usf[ slot].present   = TRUE;
      config_data.mac_l1_dynamic_tbf_config.dynamic_tbf.usf[ slot].usf_value = ftm_vars_p->current_egprs_ul_tbf_info.usf;
    }
  }

  MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Sending MAC_L1_DYNAMIC_TBF_CONFIG", 0);

  /* Buffer the dynamic config info */

  ftm_vars_p->current_egprs_l1_dynamic_tbf_info = config_data.mac_l1_dynamic_tbf_config.dynamic_tbf;

  ftm_egprs_ber_send_msg_to_gpl1( rfm_dev, ( void *) &config_data);
}



/*===========================================================================

FUNCTION        FTM_EGPRS_BER_DISPATCH

DESCRIPTION     Handles external commands sent by PC side.
.

DEPENDENCIES
                ftm_egprs_ber_init called.

RETURN VALUE
                Response packet

SIDE EFFECTS
                Can trigger functions that send primitives to L1.

===========================================================================*/
ftm_rsp_pkt_type ftm_egprs_ber_dispatch(  ftm_egprs_ber_pkt_type *pkt, rfm_device_enum_type rfm_dev)
{
  ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_egprs_ber_sync_status_type cmd_status = FTM_EGPRS_BER_SYNC_STATUS_GEN_FAILURE;

  // Variables for synchronous status
  uint32        start_time     = 0;
  int32         time_to_wait   = 0;
  uint8         TimeOutExpired = FALSE;
  rex_sigs_type ftm_sig_clk_rex_wait_return_sig;

  ftm_gsm_rfctl_vars_s *ftm_vars_p = ftm_gsm_get_vars_ptr(rfm_dev);

  MSG_1(MSG_SSID_FTM, MSG_LEGACY_LOW,"FTM EGPRS BER Dispatch: Received Command: %d", 
        pkt->ftm_cmd_header.cmd_id);

  switch ( pkt->ftm_cmd_header.cmd_id)
  {

  /*************************************************************************
  *              FTM_EGPRS_BER_START_GPRS_IDLE_MODE_REQ                    *
  *                                                                        *
  **************************************************************************/
  
  case FTM_EGPRS_BER_START_GPRS_IDLE_MODE_REQ:
    {

      if ( ftm_gsm_ber_get_dev_state(rfm_dev) != FTM_GSM_BER_STATE_ACQ_BCCH)
      {

        cmd_status = FTM_EGPRS_BER_SYNC_STATUS_GEN_FAILURE;
        break;
      }

      MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Sending START_GPRS_IDLE_MODE_REQ", 0);

      ftm_egprs_ber_send_MPH_START_GPRS_IDLE_MODE_REQ( rfm_dev );

      FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p, FTM_EGPRS_BER_STATE_GPRS_IDLE_PENDING);

      cmd_status = FTM_EGPRS_BER_SYNC_STATUS_SUCCESS;

      break;
    }

    /*************************************************************************
    *              FTM_EGPRS_BER_ESTABLISH_SRB_LOOPBACK                      *
    *                                                                        *
    **************************************************************************/


  case FTM_EGPRS_BER_ESTABLISH_SRB_LOOPBACK:
    {

      if ( FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p) == FTM_EGPRS_BER_STATE_GPRS_IDLE)
      {

        MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Establishing DL/UL TBF, and closing loop", 0);

        FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_DL_TBF_ESTABLISHING);
        ftm_egprs_ber_send_MAC_L1_FREQ_CONFIG( rfm_dev );

        cmd_status = FTM_EGPRS_BER_SYNC_STATUS_SUCCESS;
      }
      else if ( FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p) == FTM_EGPRS_BER_STATE_DL_TBF_ESTABLISHED)
      {

        MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Establishing UL TBF, and closing loop", 0);

        FTM_EGPRS_BER_SET_DEV_STATE( ftm_vars_p,  FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHING);
        ftm_egprs_ber_send_MAC_L1_FREQ_CONFIG( rfm_dev );

        cmd_status = FTM_EGPRS_BER_SYNC_STATUS_SUCCESS;
      }
      else if ( FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p) == FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHED)
      {
        /* Currently in SRB loop...DL and UL established.  Therefore, start the reconfig
         * operation.  This assumes the new configuration has already been buffered.  If not,
         * we will teardown the current config, and re-establish with the same config, which
         * is not useful! 
         */

        /* Kickoff the reconfig by sending a freq config */

        ftm_egprs_ber_send_MAC_L1_FREQ_CONFIG( rfm_dev );

        cmd_status = FTM_EGPRS_BER_SYNC_STATUS_SUCCESS;

      }
      else
      {

        cmd_status = FTM_EGPRS_BER_SYNC_STATUS_GEN_FAILURE;
      }

      break;
    }

    /*************************************************************************
    *              FTM_EGPRS_BER_RELEASE_ALL_TBF                             *
    *                                                                        *
    **************************************************************************/

  case FTM_EGPRS_BER_RELEASE_ALL_TBF:
    {

      if ( FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p) != FTM_EGPRS_BER_STATE_DL_TBF_ESTABLISHED &&
           FTM_EGPRS_BER_GET_DEV_STATE( ftm_vars_p) != FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHED)
      {

        cmd_status = FTM_EGPRS_BER_SYNC_STATUS_GEN_FAILURE;
        break;
      }

      ftm_egprs_ber_send_MAC_L1_RELEASE( rfm_dev );

      cmd_status = FTM_EGPRS_BER_SYNC_STATUS_SUCCESS;
      break;
    }

    /*************************************************************************
    *              FTM_EGPRS_BER_CONFIGURE_DL_TBF                            *
    *                                                                        *
    **************************************************************************/


  case FTM_EGPRS_BER_CONFIGURE_DL_TBF:
  case FTM_EGPRS_BER_CONFIGURE_DL_TBF_V2:
    {

      uint8                                  slot_config_index;
      ftm_egprs_ber_configure_dl_tbf_type    *dl_tbf_pkt = ( ftm_egprs_ber_configure_dl_tbf_type *) pkt;
      uint8                                  num_slots;

      if ( pkt->ftm_cmd_header.cmd_id == FTM_EGPRS_BER_CONFIGURE_DL_TBF)
      {
        num_slots = FTM_EGPRS_BER_MAX_DL_UL_SLOTS;
      }
      else
      {
        num_slots = FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2;
      }

      MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Configuring DL TBF",0);

      ftm_vars_p->current_egprs_dl_tbf_info.dl_tfi         = dl_tbf_pkt->dl_tfi;
      ftm_vars_p->current_egprs_dl_tbf_info.dl_tsc         = dl_tbf_pkt->dl_tsc;
      ftm_vars_p->current_egprs_dl_tbf_info.dl_arfcn       = dl_tbf_pkt->dl_arfcn;

      MSG_2(MSG_SSID_FTM, MSG_LEGACY_LOW,"DL TBF Config: tfi=%d, tsc=%d", 
            dl_tbf_pkt->dl_tfi, dl_tbf_pkt->dl_tsc);

      /* initialize */
      for ( slot_config_index = 0; slot_config_index < FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2; slot_config_index++)
      {
        ftm_vars_p->current_egprs_dl_tbf_info.dl_slot_config[ slot_config_index].slot_active = FALSE;
      }

      for ( slot_config_index = 0; slot_config_index < num_slots; slot_config_index++)
      {

        ftm_vars_p->current_egprs_dl_tbf_info.dl_slot_config[ slot_config_index] = dl_tbf_pkt->dl_slot_config[ slot_config_index];
      }

      MSG_2(MSG_SSID_FTM, MSG_LEGACY_LOW,"  DL TBF Config: chan=%d, band=%d", 
            dl_tbf_pkt->dl_arfcn.channel, 
            dl_tbf_pkt->dl_arfcn.band);
      MSG_2(MSG_SSID_FTM, MSG_LEGACY_LOW,"  DL TBF Config: slot_0: %d, slot_1: %d",  
            dl_tbf_pkt->dl_slot_config[0].slot_number, 
            dl_tbf_pkt->dl_slot_config[1].slot_number);

      cmd_status = FTM_EGPRS_BER_SYNC_STATUS_SUCCESS;

      break;
    }

    /*************************************************************************
    *              FTM_EGPRS_BER_CONFIGURE_UL_TBF                            *
    *                                                                        *
    **************************************************************************/


  case FTM_EGPRS_BER_CONFIGURE_UL_TBF:
  case FTM_EGPRS_BER_CONFIGURE_UL_TBF_V2:
    {

      uint8                                  slot_config_index;
      ftm_egprs_ber_configure_ul_tbf_type    *ul_tbf_pkt = ( ftm_egprs_ber_configure_ul_tbf_type *) pkt;
      uint8                                  num_slots;

      if ( pkt->ftm_cmd_header.cmd_id == FTM_EGPRS_BER_CONFIGURE_UL_TBF)
      {
        num_slots = FTM_EGPRS_BER_MAX_DL_UL_SLOTS;
      }
      else
      {
        num_slots = FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2;
      }

      MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"Configuring UL TBF",0);

      ftm_vars_p->current_egprs_ul_tbf_info.usf      = ul_tbf_pkt->usf;
      ftm_vars_p->current_egprs_ul_tbf_info.ul_tsc   = ul_tbf_pkt->ul_tsc;

      MSG_2(MSG_SSID_FTM, MSG_LEGACY_LOW,"  UL TBF Config: usf=%d, tsc=%d", 
            ul_tbf_pkt->usf, ul_tbf_pkt->ul_tsc);

      /* initialize */
      for ( slot_config_index = 0; slot_config_index < FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2; slot_config_index++)
      {
        ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[ slot_config_index].slot_active = FALSE;
      }

      for ( slot_config_index = 0; slot_config_index < num_slots; slot_config_index++)
      {

        ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[ slot_config_index] = ul_tbf_pkt->ul_slot_config[ slot_config_index];
      }

      MSG_2(MSG_SSID_FTM, MSG_LEGACY_LOW,"  UL TBF Config: slot_0: %d, slot_1: %d", 
            ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[0].slot_number,
            ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[1].slot_number);
      MSG_2(MSG_SSID_FTM, MSG_LEGACY_LOW,"  UL TBF Config: slot_0_gamma: %d, slot_1_gamma: %d",  
            ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[0].ul_gamma, 
            ftm_vars_p->current_egprs_ul_tbf_info.ul_slot_config[1].ul_gamma);

      cmd_status = FTM_EGPRS_BER_SYNC_STATUS_SUCCESS;

      break;

    }
   case FTM_EGPRS_BER_EVENT_STATUS_CLEAR_REQ:
   {
      // call the function to clear the status
      (void) ftm_egprs_ber_clear_sync_status();

      // Setup the response value
      cmd_status = FTM_EGPRS_BER_SYNC_STATUS_SUCCESS;

      FTM_MSG_MED( "Clearing synchronous EGPRS BER events", 0,0,0);

      break;
   }

   case FTM_EGPRS_BER_EVENT_STATUS_REQ:
   {
     // This comand will return the status of events that have occured
     // these events are stored in the egprs_BER_Sync_Status

     ftm_egprs_ber_event_status_req_type *request = (ftm_egprs_ber_event_status_req_type *) pkt;

     // validate the ID of the event_type
     if ( request->event_type < FTM_EGPRS_BER_SYNC_STATUS_MAX)
     {
       // Record the start time
       start_time      = timetick_get();

       // Setup timeout loop
       // If the event has already reported then the timing loop will not execute
       while (	(  EGPRS_BER_Sync_Status[ request->event_type ].has_updated == FALSE )
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
         if( time_to_wait > 0 )
         {
             /* Wait on a request status sig or a timeout. */
              ftm_sig_clk_rex_wait_return_sig = ftm_sig_clk_rex_wait(FTM_SEQ_STATUS_REQ_SIG | FTM_EGPRS_BER_CMD_Q_SIG, time_to_wait);

              if( ftm_sig_clk_rex_wait_return_sig & FTM_EGPRS_BER_CMD_Q_SIG)
              {
                ftm_egprs_ber_handle_msgs_from_gpl1();

                RFA_FTM_GSM_ISR_LOCK(&ftm_gsm_lock);

                /* clear the signal only if the queue is empty */
                if ( q_cnt(&ftm_egprs_ber_status.ftm_egprs_ber_cmd_q) == 0 )
                {
                  (void)rex_clr_sigs( &ftm_tcb, FTM_EGPRS_BER_CMD_Q_SIG);
                }

                RFA_FTM_GSM_ISR_UNLOCK(&ftm_gsm_lock);
              }

           // Check to see if we timed out
           if( (ftm_sig_clk_rex_wait_return_sig & FTM_WAIT_TIMEOUT) == FTM_WAIT_TIMEOUT )
           {
             TimeOutExpired = TRUE;
             cmd_status     = FTM_EGPRS_BER_SYNC_STATUS_TIMEOUT_EXPIRED;
           }
         }
         else
         {
           TimeOutExpired = TRUE;
           cmd_status     = FTM_EGPRS_BER_SYNC_STATUS_TIMEOUT_EXPIRED;
         } //if( time_to_wait > 0 )

       }// End While timing loop 

       // If message received then use the status from that message type
       if(  EGPRS_BER_Sync_Status[ request->event_type ].has_updated == TRUE)
       {
         cmd_status = EGPRS_BER_Sync_Status[ request->event_type ].status;
       }
    }
     else
     {
       // Error, invalid event_type
       cmd_status = FTM_EGPRS_BER_SYNC_STATUS_GEN_FAILURE;

       FTM_MSG_MED( "FTM_egprs_BER_EVENT_STATUS_REQ: Invalid event type %d", request->event_type,0,0);
     }//if ( request->event_type < FTM_egprs_BER_SYNC_STATUS_MAX)

     FTM_MSG_LOW("FTM_EGPRS_BER_EVENT_STATUS_REQ: Event Type=%d cmd_status =%d",request->event_type,
                                                                                cmd_status,
                                                                                0);
   }
   break;


    /*************************************************************************
    *              DEFAULT                                                   *
    *                                                                        *
    **************************************************************************/

  default:
    {

      cmd_status = FTM_EGPRS_BER_SYNC_STATUS_GEN_FAILURE;

      MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"FTM EGPRS: Unknown Cmd: %d", 
            pkt->ftm_cmd_header.cmd_id);

      break;
    }

  }


  if ( cmd_status == FTM_EGPRS_BER_SYNC_STATUS_GEN_FAILURE )
  {

    MSG_3(MSG_SSID_FTM, MSG_LEGACY_HIGH,
          "FTM EGPRS BER, Command Failure: %d, GSM State: %d, EGPRS State: %d", 
          pkt->ftm_cmd_header.cmd_id, 
          ftm_gsm_ber_get_dev_state( rfm_dev ), 
          FTM_EGPRS_BER_GET_DEV_STATE(ftm_vars_p));
  }
  rsp = ftm_egprs_ber_create_gen_response( pkt,
                                           (uint16) cmd_status,   
                                           ( (ftm_egprs_ber_pkt_type *) pkt)->ftm_cmd_header.cmd_id,
                                           0);

  return( rsp);

}
/*===========================================================================

FUNCTION       ftm_egprs_ber_clear_sync_status

DESCRIPTION    This function clears all entries in the synchronous status 
               array.  This behavior is called by the FTM_EGPRS_BER_EVENT_STATUS_CLEAR_REQ
               command.

DEPENDENCIES
  none

===========================================================================*/
void ftm_egprs_ber_clear_sync_status( void)
{
   uint8 Index;

   // Interate through the array that stores the synchronous status items
   for ( Index = 0; Index < FTM_EGPRS_BER_SYNC_STATUS_MAX; Index = Index + 1 )
   {
      // Reset the updated flag
      EGPRS_BER_Sync_Status[ Index ].has_updated = FALSE;

      // Reset the event status 
      EGPRS_BER_Sync_Status[ Index ].status = FTM_EGPRS_BER_SYNC_STATUS_GEN_FAILURE;
   }

}

/*===========================================================================

FUNCTION       FTM_EGPRS_BER_GET_STATE

DESCRIPTION    Returns current state of FTM EGPRS BER operations.


DEPENDENCIES
  none

===========================================================================*/
ftm_egprs_ber_state_type ftm_egprs_ber_get_state( void)
{
  /* Workaround so as not to break DIME */
  return FTM_EGPRS_BER_GET_DEV_STATE( ftm_gsm_get_vars_ptr(ftm_gsm_ns_active_device));
}

ftm_egprs_ber_state_type ftm_egprs_ber_get_dev_state( rfm_device_enum_type rfm_dev )
{
  return FTM_EGPRS_BER_GET_DEV_STATE( ftm_gsm_get_vars_ptr(rfm_dev));
}
#endif /* FEATURE_FTM_EGPRS_BER */
#endif /* FEATURE_GSM_EGPRS */
#endif /* FEATURE_FTM_GSM_BER */
