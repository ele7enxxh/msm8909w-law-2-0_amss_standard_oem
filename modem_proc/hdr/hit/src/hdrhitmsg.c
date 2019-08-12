/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        HIT MESSAGE SERVICE MODULE

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
    
REGIONAL FUNCTIONS
    None

INITIALIZATION AND SEQUENCING REQUIREMENTS
    None
    
    Copyright (c) 2011 - 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE


This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/hit/src/hdrhitmsg.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when         who      what, where, why
----------   ---      --------------------------------------------------------
05/05/2016   wsh      Fixed DIV mode not set properly for NS mode 
01/09/2015   sat      Starting TX clocks while starting RTC (RUMI)
12/23/2014   sat      API to start Flex capture for RUMI Tx
12/17/2014   wsh      Fixed compile warnings 
10/14/2014   wsh      Changes to support Jolokia bringup
07/29/2014   tnp      FR22375: Fix Medium compiler warnings in HDR
04/18/2014   mbs      Increased wait time for completion of RF TX START
04/18/2014   mbs      Ensure FW is set to revB mode to generate MC waveform
04/09/2014   mbs      Increased wait time for revB RFRetune operation
03/14/2014   vko/dsp  FR19338:Reduce call setup time by skipping RF Tx shutdown.
12/19/2013   sat      Changed memcpy() to assignment or memscpy() (safer version).
11/27/2013   dsp/cnx  FR17482 for Page blocking management.
11/26/2013   rmg      Supported enabling carriers in FW without a frame delay.
11/08/2013   mbs      Added changes to support revB calls in RUMI
10/24/2013   wsh      Changes to support BOLT bring up
10/08/2013   mbs      Added support for BOLT interface changes
09/04/2013   wsh      Changes to support BOLT bring up
08/06/2013   rmg      Added support to skip DAC toggle in modulator ctl cmd.
06/24/2013   arm      Passed default band mask while enabling diversity.
05/06/2013   mbs      Fixed compiler errors for BOLT build
03/12/2013   smd      Removed featurization for hdr acquisition.
02/25/2013   dsp      Fixed low Tx pwr issue in FTM mode.
02/11/2013   vko      Fixed compiler warnings
01/25/2013   dsp      Enabled Tx clock in hdrhitmsg_process_mod_rtc().
01/17/2013   rmg      Removed references to hdrmod_setup_tx_timed_enable_mc.
01/08/2013   dsp      Moved programming of pn_code from hdrhitmsg_process_mod_acc() 
                      to hdramac_process_start_attempt().
12/19/2012   smd      Added RUMI Tx support.
12/20/2012   vko      Fixed compiler warnings 
12/04/2012   dsp      Added a wait of 26.6ms between Tx shutdown and Rx shutdown 
                      in process_deassign() 
11/20/2012   mbs      Fixed compilation errors for Triton build
07/27/2012   rmg      Added support for configuring RL funnel mode.
07/20/2012   smd      Used HHO for sleep (for DIME RUMI).
06/15/2012   dsp/rmg  Changed hdrhitmsg_process_enable_modulator() 
                      to hdrhitmsg_process_modulator_ctl().       
06/07/2012   ljl      Supported sync and traffic on Dime RUMI.
05/13/2012   dsp/rmg  Added hdrhitmsg_get_fwd_lnk_stats(),
                      Added hdrhitmsg_process_reset_fwd_link_stats(),
                      Added hdrhitmsg_process_get_fwd_link_stats(),
                      Added hdrhitmsg_process_modulator_ctl().
04/06/2012   dsp      Added hdrhitmsg_process_hard_handoff()
04/06/2012   dsp      Added hdrhitmag_hdrsrch_tca_cb()
03/16/2012   arm      Resolve TCB errors for Dime. 
03/08/2012   smd      Put hiapp_tcb under feature_hit.
02/27/2012   smd      Added updates of FTM msgr interface changes.
10/25/2011   vlc      Replaced obsolete clk APIs with DAL APIs. 
09/28/2011   smd      Fixed compiler errors.
06/15/2011   smd      Created

===========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "hdr_variation.h"
#include "customer.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "msgr_rex.h"
#include "task.h"
#include "sys.h"
#include "msgr.h"
#include "rex.h"
#include "ftm.h"
#include "DALSys.h" 
#include "hdrhitmsg.h"

#include "hitapp.h"
#include "rfm_hdr.h"

#include "hdramac.h"
#include "hdrenc.h"
#include "hdrfmac.h"
#include "hdrfmaci.h"
#include "hdrhai.h"

#include "hdrhitcmd.h"
#include "hdrlmac.h"
#include "hdrmdspmcr.h"
#include "hdrmod.h"
#include "hdrrmac.h"
#include "hdrrmac0.h"
#include "hdrsrch.h"
#include "hdrsrchset.h"
#include "hdrsrchutil.h"
#include "hdrsrchstates.h"
#include "hdrsrchmcpm.h"
#include "hdrsrchdrv.h"

#include "hdrts.h"

#include "DALSys.h"

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

/*==========================================================================

                     GLOBAL VARIABLES FOR MODULE

==========================================================================*/

hdrhitmsg_queue_type                hdrhitmsg_rcvd_msg_q;           /* Message queue for received messages */

msgr_client_t                       hdrhitmsg_msgr_client;          /* MSGR client for received messages */

hdrhitmsg_hdr_ftm_state_t           hdrhitmsg_hdr_ftm_state;

hdrhitmsg_ftm_intf_t                hdrhitmsg_ftm_intf;

/* Temp variables to help initialization in hdrhitmsg_init_global_vars() */
LOCAL uint8                         hdrhitmsg_t2p_hicap_revb_init[2][13]
                                    = {{0, 3, 15, 28, 35, 40, 46, 52, 57, 62, 68, 74, 85},
                                       {0, 3, 15, 28, 35, 40, 46, 52, 57, 62, 68, 74, 85}};     /* [pre/post][rri] */
LOCAL uint8                         hdrhitmsg_t2p_lolat_revb_init[2][13]
                                    = {{0, 28, 40, 52, 59, 65, 73, 77, 77, 82, 87, 93, 105},
                                       {0,  3, 15, 28, 35, 40, 46, 52, 57, 62, 68, 74,  85}};   /* [pre/post][rri] */
LOCAL int8                          hdrhitmsg_rri_gain_revb_init[2][4]
                                    = {{ 0, 0,-6,-6},
                                       {-6,-6,-6,-6}};                                          /*[pre/post][gain trans point]*/

LOCAL hdrhit_msg_status_type hdrhitmsg_response_status = HDRHIT_SUCCESS;

LOCAL    hdrrup_tca_msg_type         hdrhitmsg_ftm_tca_msg;

LOCAL qword                       hdrhitmsg_acc_i_mask;
LOCAL qword                       hdrhitmsg_acc_q_mask;

#ifdef FEATURE_HDR_RUMI
extern volatile boolean hdrsrchmcpm_bypass_down_state_transition;
  /* needed to bypass MCPM transition to sleep */

volatile hdrhit_rumi_cfg_type hdrhit_rumi_cfg = {
  TRUE,         /* y1y2_support */
  FALSE,        /* tx_only_support */

  TRUE,         /* force_mcpm_cfg */
  TRUE,         /* clock_on_during_sleep */
  TRUE,         /* force_trm_grant */
  TRUE,         /* verbose_debug */
  FALSE,        /* sram_check */

  0,            /* sync_pn_offset */
  9,            /* slot_cycle */
  0,            /* eq_ctrl */
  0xc1,         /* div_ctrl */
  1,            /* div_on_hack */
  0,            /* afc_override */
  0,            /* pri_chain_override */
  1,            /* div_chain_override */

#ifdef FEATURE_HDR_REVB_SUPPORTED
  0,            /* revb_carriers */
  { 0, 0, 0 },  /* revb_drc[3] */
#endif /* FEATURE_HDR_REVB_SUPPORTED */

  0,            /* cc_pkt_hstr */
  0             /* div_enabled */
};

LOCAL uint64   hdrhitmsg_cdma_sys_time = 0x9757dbbb;
  /* Force CDMA system time to this value in TX only RUMI */
typedef struct {
 qword    i_mask;
 qword    q_mask;
} hdrhitmsg_rumi_tx_only_type;

hdrhitmsg_rumi_tx_only_type       hdrhitmsg_tx_only;

uint8                             hdrhitmsg_data_pkt[30]= {
   0x8f,0x58,0x68,0xe5,0xe5,0x17,0x38,0xde,0x2f,0xab,
   0xa4,0x70,0xc1,0x78,0xd7,0x48,0xb6,0x38,0xaf,0x75,
   0x62,0x36,0x56,0x24,0x4d,0x29,0xba,0x41,0xe0,0x00
};

typedef enum {
    HDRHITMSG_TX_ACC_REV0,
    HDRHITMSG_TX_ACC_REVA,
    HDRHITMSG_TX_RTC_REV0,
    HDRHITMSG_TX_RTC_REVA,
    HDRHITMSG_TX_RTC_REVB
} hdrhitmsg_tx_action_enum_type ;

hdrhitmsg_tx_action_enum_type     hdrhitmsg_tx_action;

hdrhitmsg_s_type                  hdrhitmsg_msg_buf;
#endif /* FEATURE_HDR_RUMI */


/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/
 
msgr_umid_type hdrhitmsg_reg_msgs[] =
{
    HDR_HIT_CMD_PIL_ACQ,
    HDR_HIT_CMD_SYNC,
    HDR_HIT_CMD_DEMOD_FTC,
    HDR_HIT_CMD_MOD_ACC,
    HDR_HIT_CMD_MOD_RTC,
    HDR_HIT_CMD_DEASSIGN,
    HDR_HIT_CMD_IDLE_STATE,
    HDR_HIT_CMD_DEMOD_FW_LINK_REV0,
    HDR_HIT_CMD_RELA_MAC_CONFIG,
    HDR_HIT_CMD_DEMOD_FW_LINK_REVA,
    HDR_HIT_CMD_MOD_ACC_REVA,
    HDR_HIT_CMD_MOD_RTC_REVA,
    HDR_HIT_CMD_MOD_RTC_REVB,
    HDR_HIT_CMD_CONFIG_RTC_REVB,
    HDR_HIT_CMD_HARD_HANDOFF,
    HDR_HIT_CMD_GET_FWD_LNK_STATS,
    HDR_HIT_CMD_RESET_FWD_LNK_STATS,
    HDR_HIT_CMD_MODULATOR_CTL,
    HDR_HIT_CMD_CONFIG_FUNNEL_MODE
};

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/ 

/*=========================================================================
]
FUNCTION     : HDRHITMSG_INIT_GLOBAL_VARS

DESCRIPTION  : This function initializes the appropriate fields of
               hdrhitmsg_ftm_intf with their corresponding values.

DEPENDENCIES : None

INPUTS       : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrhitmsg_init_global_vars( void )
{
    hdrhitmsg_rmac_params_t         *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    int                             i,j;            /* Loop variables */
/*-----------------------------------------------------------------------*/

    hdrhitmsg_ftm_intf.fwd_link_info_rev0.mac_index     = 63;
    hdrhitmsg_ftm_intf.fwd_link_info_rev0.rablength     = 0;
    hdrhitmsg_ftm_intf.fwd_link_info_rev0.raboffset     = 0;
    hdrhitmsg_ftm_intf.fwd_link_info_rev0.drclocklength = 1;
    hdrhitmsg_ftm_intf.fwd_link_info_rev0.drclockperiod = 1;

    hdrhitmsg_ftm_intf.fwd_link_info_reva.mac_index     = 63;
    hdrhitmsg_ftm_intf.fwd_link_info_reva.rablength     = 0;
    hdrhitmsg_ftm_intf.fwd_link_info_reva.raboffset     = 0;
    hdrhitmsg_ftm_intf.fwd_link_info_reva.drclocklength = 1;
    hdrhitmsg_ftm_intf.fwd_link_info_reva.drclockperiod = 1;

    hdrhitmsg_ftm_intf.fwd_link_info_revb.mac_index     = 63;
    hdrhitmsg_ftm_intf.fwd_link_info_revb.rablength     = 0;
    hdrhitmsg_ftm_intf.fwd_link_info_revb.raboffset     = 0;
    hdrhitmsg_ftm_intf.fwd_link_info_revb.drclocklength = 1;
    hdrhitmsg_ftm_intf.fwd_link_info_revb.drclockperiod = 1;
    
    hdrhitmsg_ftm_intf.mode_type_rev0           = 0;
    hdrhitmsg_ftm_intf.mode_type_reva           = 0;
    hdrhitmsg_ftm_intf.mode_type_revb           = 0;

    hdrhitmsg_ftm_intf.first_time_rev_link_rev0 = 0;
    hdrhitmsg_ftm_intf.first_time_rev_link_reva = 0;
    hdrhitmsg_ftm_intf.first_time_rev_link_revb = 0;

    hdrhitmsg_ftm_intf.ftm_not_doing_hho = TRUE;

    rmac_params_ptr->gain_transition_point      = 3;
    rmac_params_ptr->modem_fwd_rev_link_type    = 0;
    rmac_params_ptr->revb_gain_transition_point = 3;

    for( i = 0; i < 2; i++)
        for( j = 0; j< 13; j++)
        {
            rmac_params_ptr->t2p_hicap_revb[i][j]   = hdrhitmsg_t2p_hicap_revb_init[i][j];
            rmac_params_ptr->t2p_lolat_revb[i][j]   = hdrhitmsg_t2p_lolat_revb_init[i][j];
        }
    for( i = 0; i < 2; i++)
        for( j = 0; j< 4; j++)
        {
            rmac_params_ptr->rri_gain_revb[i][j]    = hdrhitmsg_rri_gain_revb_init[i][j];
        }

    rmac_params_ptr->revb_single_fwd_link_mode  = 1;
    rmac_params_ptr->revb_drc_gating            = 0;
    rmac_params_ptr->revb_drc_length            = 0;
    rmac_params_ptr->revb_drc_cover             = 1;
    rmac_params_ptr->revb_arq_mode              = 0;
    rmac_params_ptr->revb_ack_mode              = (uint8) IS890_MODE_1;
    rmac_params_ptr->revb_auxpil_payload_thresh = 7;
    rmac_params_ptr->revb_dsc_length            = 8;
    rmac_params_ptr->revb_dsc_value             = 1;
    rmac_params_ptr->revb_frame_offset          = 0;
    rmac_params_ptr->revb_rtc_pattern           = 0xaaaaaaaa;
    rmac_params_ptr->revb_rtclc_mask_i_hi       = 0x3ff;
    rmac_params_ptr->revb_rtclc_mask_i_lo       = 0x0;
    rmac_params_ptr->revb_rtclc_mask_q_hi       = 0x3fe;
    rmac_params_ptr->revb_rtclc_mask_q_lo       = 0x1;
    rmac_params_ptr->hicap_lolatency            = 0;
    rmac_params_ptr->sht_pkt_enabled            = 0;
    rmac_params_ptr->mup_enabled                = 0;

    hdrhitmsg_hdr_ftm_state.fwd_curr_state = HDRHITMSG_HDR_FTM_ENTER;
    hdrhitmsg_hdr_ftm_state.rev_curr_state = HDRHITMSG_HDR_FTM_ENTER;
} /* hdrhitmsg_init_global_vars */

/*=========================================================================

FUNCTION     : HDRHITMSG_INIT

DESCRIPTION  : This function creates MSGR clients and queues and 
               registers the messages with MSGR.

DEPENDENCIES : None

INPUTS       : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrhitmsg_init( void )
{

  int                        i;
    /* loop index */

  errno_enum_type            estatus;
    /* Status */

/*-------------------------------------------------------------------------*/

  /* Initialize global variables */
  ( void ) hdrhitmsg_init_global_vars();

  /* Initialize the queues for received messages */
  ( void ) q_init( &hdrhitmsg_rcvd_msg_q.msg_q );
  ( void ) q_init( &hdrhitmsg_rcvd_msg_q.free_q );

  /* Link the queue for received messages */
  for ( i = 0; i < HDRHITMSG_QUEUE_SIZE; i++ )
  {
    q_put( &hdrhitmsg_rcvd_msg_q.free_q,
           q_link( &hdrhitmsg_rcvd_msg_q.q_items[i], 
                   &hdrhitmsg_rcvd_msg_q.q_items[i].qlink)
         );
  }

  estatus = msgr_client_create( &hdrhitmsg_msgr_client );
    /* Create the MSGR Client */

  ASSERT( estatus == E_SUCCESS );
#ifdef FEATURE_HIT
  estatus = msgr_client_set_rex_q ( &hdrhitmsg_msgr_client,
                                    hdrhitapp_tcb_ptr,
                                    HITAPP_MSG_Q_SIG,
                                    &hdrhitmsg_rcvd_msg_q.msg_q,
                                    &hdrhitmsg_rcvd_msg_q.free_q,
                                    MSGR_OFFSETOF( hdrhitmsg_s_type, msg ),
                                    MSGR_NO_OFFSET,
                                    (uint16)sizeof( hdrhitmsg_s_type ) );
    /* Set queue and signal for response */

  ASSERT( estatus == E_SUCCESS );
#endif /* FEATURE_HIT */

  /* Register messages for message receiving queue */
  estatus = msgr_register_block( 
                MSGR_HDR_HIT,
                &hdrhitmsg_msgr_client,
                MSGR_ID_REX,
                hdrhitmsg_reg_msgs,
                sizeof( hdrhitmsg_reg_msgs )/sizeof( msgr_umid_type ) );

  ASSERT( estatus == E_SUCCESS );

} /* hdrhitmsg_init() */


/*========================================================================
FUNCTION HDRHITMSG_PROCESS_RCVD_MSGS

DESCRIPTION
  This function processes messages received by hit.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrhitmsg_process_rcvd_msgs( void )
{

    hdrhitmsg_s_type         *msg;
    /* Message */

/*-----------------------------------------------------------------------*/

  msg = ( hdrhitmsg_s_type * ) q_get( &hdrhitmsg_rcvd_msg_q.msg_q );
    /* get a message from receiving queue */

  while(  msg != NULL )
  {
    /* Process the message */

    MSG_1( MSG_SSID_HIT, MSG_LEGACY_HIGH, 
           "hit rcvd msg: 0x%8x", msg->msg.hdr.id );

    switch( msg->msg.hdr.id )
    {
        case HDR_HIT_CMD_PIL_ACQ :
            hdrhitmsg_process_pil_acq( msg );
            break;

        case HDR_HIT_CMD_SYNC :
            hdrhitmsg_process_sync(msg);
            break;

        case HDR_HIT_CMD_DEMOD_FTC :
            hdrhitmsg_process_demod_ftc(msg);
            break;

        case HDR_HIT_CMD_MOD_ACC :
            hdrhitmsg_process_mod_acc(msg);
            break;

        case HDR_HIT_CMD_MOD_RTC :
            hdrhitmsg_process_mod_rtc(msg);
            break;

        case HDR_HIT_CMD_DEASSIGN :
            hdrhitmsg_process_deassign(msg);
            break;

        case HDR_HIT_CMD_IDLE_STATE :
            hdrhitmsg_process_idle_state(msg);
            break;

        case HDR_HIT_CMD_DEMOD_FW_LINK_REV0 :
            hdrhitmsg_process_demod_fw_link_rev0(msg);
            break;

        case HDR_HIT_CMD_RELA_MAC_CONFIG :
            hdrhitmsg_process_rela_mac_config(msg);
            break;

        case HDR_HIT_CMD_DEMOD_FW_LINK_REVA :
            hdrhitmsg_process_demod_fw_link_reva(msg);
            break;

        case HDR_HIT_CMD_MOD_ACC_REVA :
            hdrhitmsg_process_mod_acc_reva(msg);
            break;

        case HDR_HIT_CMD_MOD_RTC_REVA :
            hdrhitmsg_process_mod_rtc_reva(msg);
            break;

        case HDR_HIT_CMD_MOD_RTC_REVB :
            hdrhitmsg_process_mod_rtc_revb(msg);
            break;

        case HDR_HIT_CMD_CONFIG_RTC_REVB :
            hdrhitmsg_process_config_rtc_revb(msg);
            break;

        case HDR_HIT_CMD_HARD_HANDOFF:
            hdrhitmsg_process_hard_handoff(msg);
            break;

        case HDR_HIT_CMD_GET_FWD_LNK_STATS:
            hdrhitmsg_process_get_fwd_lnk_stats(msg);
            break;

        case HDR_HIT_CMD_RESET_FWD_LNK_STATS:
            hdrhitmsg_process_reset_fwd_lnk_stats(msg);
            break;

        case HDR_HIT_CMD_MODULATOR_CTL:
            hdrhitmsg_process_modulator_ctl(msg);
            break;

        case HDR_HIT_CMD_CONFIG_FUNNEL_MODE:
            hdrhitmsg_process_config_funnel_mode(msg);
            break;

        default:
            HDR_MSG_HIT_1( MSG_LEGACY_HIGH, "hit ignored rcvd msg: 0x%8x",
                                            msg->msg.hdr.id );
        
        break;
    } /* switch */

    (void) q_link( msg, &msg->qlink );
    q_put( &hdrhitmsg_rcvd_msg_q.free_q, &msg->qlink );
      /* return the buffer to free queue */

    msg = ( hdrhitmsg_s_type * ) q_get( &hdrhitmsg_rcvd_msg_q.msg_q );
      /* get next message from receiving message queue */
  } /* while */

} /* hitmsg_process_rcvd_msgs() */


/*=========================================================================

FUNCTION HDRHITMSG_RESET_HDR_MODE_REV_LINK  

DESCRIPTION
  This function sets the values of mode_type and first_time_rev_link to 0.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrhitmsg_reset_hdr_mode_rev_link ( void )
{

    hdrhitmsg_ftm_intf.mode_type_rev0  = 0;
    hdrhitmsg_ftm_intf.mode_type_reva  = 0;
    hdrhitmsg_ftm_intf.mode_type_revb  = 0;
    hdrhitmsg_ftm_intf.first_time_rev_link_rev0  = 0;
    hdrhitmsg_ftm_intf.first_time_rev_link_reva  = 0;
    hdrhitmsg_ftm_intf.first_time_rev_link_revb  = 0;

    return;
}
/*===========================================================================

FUNCTION HDRHITMSG_INIT_MSG

DESCRIPTION
  Initialize message.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/ 
void hdrhitmsg_init_msg
(
    hdrhit_msg_rsp_u *            p_hdrhit_rsp_msg,                     /* Pointer to message */
    msgr_umid_type                msg_id                                /* Message ID */
)
{
   memset(  p_hdrhit_rsp_msg, 0, sizeof( hdrhit_msg_rsp_u ) );          /* Clear the message */
   msgr_init_hdr( &( p_hdrhit_rsp_msg->hdr ), MSGR_HDR_HIT, msg_id );   /* Create message header */
}

/*===========================================================================

FUNCTION HDRHITMSG_GET_RESPONSE_STATUS

DESCRIPTION
  Provide response status.

DEPENDENCIES

RETURN VALUE
   Response message status
SIDE EFFECTS
  None.

===========================================================================*/ 
hdrhit_msg_status_type hdrhitmsg_get_response_status( void )
{
   return hdrhitmsg_response_status;
}/* hdrhitmsg_get_response_status */

/*===========================================================================

FUNCTION HDRHITMSG_SEND_MSG

DESCRIPTION
  Send message.

DEPENDENCIES

RETURN VALUE
   
SIDE EFFECTS
  None.

===========================================================================*/ 
void hdrhitmsg_send_msg
(
    hdrhit_msg_rsp_u*            rsp_msg,      /* message to be sent */
    uint32                       len        /* message length */
)
{
    errno_enum_type              e_retval = E_SUCCESS;     /* error code */

    rex_tcb_type                 *ftm_tcb_ptr;             /* temp ptr */
/*-----------------------------------------------------------------------*/

    hdrhitmsg_response_status = rsp_msg->common_rsp.cmd_status;
   
    ftm_tcb_ptr = rcinit_lookup_rextask("ftm");

    (void) rex_set_sigs( ftm_tcb_ptr,  FTM_CMD_RSP_SIG );
          /* fire a signal for pilot acquisition. This is a temp solution now */
    return;

    /* FTM does not have queue to receive message, currently the code below
       won't be executes
    */
    e_retval = msgr_send( (msgr_hdr_struct_type *) rsp_msg, len ); 


    if ( e_retval != E_SUCCESS )
    {
        ERR_FATAL( "HDRHIT Send msg to RF error: 0x%8x retval %d", 
                   ( ( msgr_hdr_struct_type *) rsp_msg)->id, e_retval, 0 );
    }
    else
    {
        HDR_MSG_HIT_1( MSG_LEGACY_HIGH, "HDRHIT Send MSG to RF: 0x%8x",
                       ( ( msgr_hdr_struct_type *) rsp_msg)->id );
    }

}


/*===========================================================================

FUNCTION HDRHITMSG_ACQUIRE_CALLBACK

DESCRIPTION
  HDR Search callback funtion. Update FTM state and send task
  signal indicating successful acquisition.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/ 
void hdrhitmsg_acquire_callback
(
  const sys_channel_type *channel,
    /* Channel acquisition was attempted on */

  hdrsrch_acquire_enum_type     acq_result
    /* Acquisition success or failure status */
)
{
    hdrhit_msg_rsp_u            hdrhit_msg_rsp;             /* Response Message */
/*-------------------------------------------------------------------------*/
    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_PIL_ACQ );

    if( acq_result == HDRSRCH_ACQUIRED )
    {    
      HDR_MSG_HIT_2( MSG_LEGACY_HIGH,
                    "HDR Pilot Acquisition successful on band %d and channel %d",
                    channel->band, channel->chan_num );
      hdrhit_msg_rsp.pil_acq.cmd_status = HDRHIT_SUCCESS;
      hdrhitmsg_hdr_ftm_state.fwd_curr_state = HDRHITMSG_HDR_FTM_PIL_ACQ;          
    }
    else
    {
      HDR_MSG_HIT_2( MSG_LEGACY_HIGH,
                    "HDR Pilot Acquisition failure on band %d and channel %d",
                    channel->band,channel->chan_num );
      hdrhit_msg_rsp.pil_acq.cmd_status = HDRHIT_NO_SUCCESS;      
    }    

    hdrhit_msg_rsp.pil_acq.hdr_chan = *channel;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_pil_acq_rsp_msg_t ) );
 
    return;
} /* hdrhitmsg_acquire_callback */


/*===========================================================================

FUNCTION HDRHITMSG_SYSTEM_LOST_CALLBACK

DESCRIPTION
  HDR Search callback funtion.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/ 

void hdrhitmsg_system_lost_callback
(
  hdrsrch_system_lost_enum_type   reason
    /* System lost explanation */
)
{   

    HDR_MSG_HIT  ( MSG_LEGACY_HIGH, "FTM HDR system lost");
    return;
} /* hdrhitmsg_system_lost_callback */

/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_PIL_ACQ

DESCRIPTION
  This function attempts to acquire a pilot.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrhitmsg_process_pil_acq
(
    hdrhitmsg_s_type         *msg
)
{
    hdrhit_pil_acq_cmd_msg_t      *pil_acq_cmd;
    hdrhit_msg_rsp_u              hdrhit_msg_rsp;
    sys_channel_type              channel;
    hdrsrch_acquire_params_type   acq_params;                 /* Acquisition parameters passed to SRCH */
    uint8                         div_ctrl;
/*-------------------------------------------------------------------------*/
    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_PIL_ACQ );
    pil_acq_cmd = &(msg->msg.hdrhit_cmd_msg.pil_acq);

    if (hdrhitmsg_hdr_ftm_state.fwd_curr_state != HDRHITMSG_HDR_FTM_ENTER)
    {
        hdrhit_msg_rsp.pil_acq.cmd_status = HDRHIT_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_pil_acq_rsp_msg_t ) );  
        return;
    }

    hdrsrch_deactivate(NULL);
    channel.band                           = pil_acq_cmd->band_class;
    channel.chan_num                       = pil_acq_cmd->chan_num;
    hdrhitmsg_hdr_ftm_state.fwd_curr_state = HDRHITMSG_HDR_FTM_ENTER;
    hdrhitmsg_hdr_ftm_state.rev_curr_state = HDRHITMSG_HDR_FTM_ENTER;

    /* Activae hdrrx and hdrtx task. Now we are ready to receive the 
       control channel    */
    ( void ) rex_set_sigs( HDRRX_TASK_PTR, HDRRX_ENTER_HDR_SIG );
    ( void ) rex_set_sigs( HDRTX_TASK_PTR, HDRTX_ENTER_HDR_SIG );

#ifdef FEATURE_HDR_RUMI
    if ( hdrhit_rumi_cfg.clock_on_during_sleep )
    {
      hdrsrchmcpm_bypass_down_state_transition = 1;
      /* skip MCPM set to sleep */
    }

    hdrsrch_set_user_equalizer_pref( hdrhit_rumi_cfg.eq_ctrl );

    if ( ( pil_acq_cmd->diversity_mode != 0 ) &&
         ( hdrhit_rumi_cfg.div_ctrl != 0 ) )
    {
      /* if DIV is enabled from QRCT and from RUMI cfg,
         use the RUMI cfg */
      div_ctrl = hdrhit_rumi_cfg.div_ctrl;
    }
    else
#endif /* FEATURE_HDR_RUMI */
    {
      /* if DIV is disabled from QRCT or is 0 from RUMI cfg,
         leave it disabled */
      div_ctrl = pil_acq_cmd->diversity_mode;
    }

    /* Enable/Disable Rx diversity */
    hdrsrch_set_user_diversity_pref( div_ctrl, 0xFFFFFFFF );

    /* Send Command to Search task */
    acq_params.channel                  = channel;
    acq_params.acq_cb                   = hdrhitmsg_acquire_callback;
    acq_params.mode                     = SYS_HDR_ACQ_MODE_FULL;
    acq_params.system_lost_cb           = hdrhitmsg_system_lost_callback;
    acq_params.usability_cb             = NULL;
    acq_params.num_of_pn                = 0;
    acq_params.overall_time_allowed_ms  = 0;
    hdrsrch_acquire( &acq_params );

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "Wait for HDR SRCH ACQ rpt");

    return;
} /* hdrhitmsg_process_pil_acq */


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_SYNC

DESCRIPTION
  This function attempts to acquire a pilot.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void hdrhitmsg_process_sync
(
    hdrhitmsg_s_type *msg
)
{
    hdrhit_msg_rsp_u            hdrhit_msg_rsp;

#ifdef FEATURE_HDR_RUMI
    uint16 hstr_count = 0; /* store this in an internal structure if used at
                        multiple places in hdrhitcmd.c */

    qword sync_time;
#endif /* FEATURE_HDR_RUMI */

/*-------------------------------------------------------------------------*/

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_SYNC );
    HDR_MSG_HIT( MSG_LEGACY_HIGH, "HDR_FTM: Got the HDR SYNC COMMAND" );

    if (hdrhitmsg_hdr_ftm_state.fwd_curr_state != HDRHITMSG_HDR_FTM_PIL_ACQ)
    {
        hdrhit_msg_rsp.sync.cmd_status = HDRHIT_NO_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_sync_rsp_msg_t ) ); 
        return;
    }

    hdrhitmsg_hdr_ftm_state.fwd_curr_state = HDRHITMSG_HDR_FTM_SYNC_STATE;

#ifdef FEATURE_HDR_RUMI
    qw_set(sync_time, 0, 6);

    //hdrcmac_get_sync_caps_timestamp(hdrhit_rumi_cfg.cc_pkt_hstr, &hstr_count);

    hstr_count = hdrhit_rumi_cfg.cc_pkt_hstr;

    HDR_MSG_HIT_1( MSG_LEGACY_MED, \
      "HDR synchronizing.., hstr=%d", hstr_count);

    hdrsrch_synchronize(sync_time, hdrhit_rumi_cfg.sync_pn_offset, hstr_count, NULL);
#endif /* FEATURE_HDR_RUMI */


    hdramac_activate(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

    hdrhit_msg_rsp.sync.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_sync_rsp_msg_t ) );

    return;
} /* hitmsg_process_ftmi_sync */


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_DEMOD_FTC

DESCRIPTION
  This function attempts to acquire a pilot.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_demod_ftc
(
    hdrhitmsg_s_type *msg
)
{  
    hdrhitmsg_rmac_params_t     *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_demod_ftc_cmd_msg_t  *demod_ftc_cmd;
    hdrhit_msg_rsp_u            hdrhit_msg_rsp;
    hdrsrch_sect_struct_type    *ref_sect_ptr;
    sys_link_type               link_info;
    uint8                       drclocklength_slots = 1;
    uint8                       i;
    hdrsrch_subactive_set_type  subaset;

/*-----------------------------------------------------------------------*/

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_DEMOD_FTC );
    demod_ftc_cmd   = &(msg->msg.hdrhit_cmd_msg.demod_ftc);
    ref_sect_ptr    = hdrsrchutil_get_ref_sector_ptr();

    subaset.channel = demod_ftc_cmd->hdr_chan;
    subaset.systype = SYS_HDR_CHAN_REC_MODE_IS856;
    subaset.not_reportable = FALSE;
    subaset.num_aset_pilots = 1;
    subaset.demod_idx = HDRMDSP_DEMOD_CARRIER_0;

    if (hdrhitmsg_hdr_ftm_state.rev_curr_state != HDRHITMSG_HDR_FTM_REV_TRAFFIC)
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH,
                    "Cannot change the parameters requested for forward link.\
                     Move to reverse traffic start state");

        hdrhit_msg_rsp.demod_ftc.cmd_status = HDRHIT_NO_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_demod_ftc_rsp_msg_t ) );

        return;
    }

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "Demodulating HDR forward traffic ...");

    if( rmac_params_ptr->modem_fwd_rev_link_type == 0 )
    {
        hdrhitmsg_ftm_intf.fwd_link_info_rev0.drclocklength = demod_ftc_cmd->drc_locklength;
        hdrhitmsg_ftm_intf.fwd_link_info_rev0.drclockperiod = demod_ftc_cmd->drc_lockperiod;
        hdrhitmsg_ftm_intf.fwd_link_info_rev0.mac_index     = demod_ftc_cmd->mac_index;
        hdrhitmsg_ftm_intf.fwd_link_info_rev0.rablength     = demod_ftc_cmd->rab_length;
        hdrhitmsg_ftm_intf.fwd_link_info_rev0.raboffset     = demod_ftc_cmd->rab_offset;
    }

    if( rmac_params_ptr->modem_fwd_rev_link_type == 1 )
    {
        hdrhitmsg_ftm_intf.fwd_link_info_reva.drclocklength = demod_ftc_cmd->drc_locklength;
        hdrhitmsg_ftm_intf.fwd_link_info_reva.drclockperiod = demod_ftc_cmd->drc_lockperiod;
        hdrhitmsg_ftm_intf.fwd_link_info_reva.mac_index     = demod_ftc_cmd->mac_index;
        hdrhitmsg_ftm_intf.fwd_link_info_reva.rablength     = demod_ftc_cmd->rab_length;
        hdrhitmsg_ftm_intf.fwd_link_info_reva.raboffset     = demod_ftc_cmd->rab_offset;
    }

    if( rmac_params_ptr->modem_fwd_rev_link_type == 2 )
    {
        hdrhitmsg_ftm_intf.fwd_link_info_revb.drclocklength = demod_ftc_cmd->drc_locklength;
        hdrhitmsg_ftm_intf.fwd_link_info_revb.drclockperiod = demod_ftc_cmd->drc_lockperiod;
        hdrhitmsg_ftm_intf.fwd_link_info_revb.mac_index     = demod_ftc_cmd->mac_index;
        hdrhitmsg_ftm_intf.fwd_link_info_revb.rablength     = demod_ftc_cmd->rab_length;
        hdrhitmsg_ftm_intf.fwd_link_info_revb.raboffset     = demod_ftc_cmd->rab_offset;
    }

    if( hdrsrch_get_link_info(ref_sect_ptr->link_id, &link_info ) != E_SUCCESS )
    {
        ERR("Unknown link ID: %d!",0, 0, 0);
    }
    else
    {
        HDR_MSG_HIT_2( MSG_LEGACY_HIGH, "Chan:%d PN:%d", link_info.channel.chan_num, link_info.pilot_pn);
        subaset.aset_list[0].pilot_pn = link_info.pilot_pn;
    }
    for (i=0; i < demod_ftc_cmd->drc_locklength + 1;i++)
    {
        drclocklength_slots = drclocklength_slots*2;
    }

    (void) hdrhitcmd_fmac_activate_msg( rmac_params_ptr->modem_fwd_rev_link_type,
                                       drclocklength_slots * 2 * ( rmac_params_ptr->modem_fwd_rev_link_type + 1 ),
                                       ( demod_ftc_cmd->drc_lockperiod + 1 ) * 8,
                                       14);

    subaset.aset_list[0].softer_handoff = 0;
    subaset.aset_list[0].mac_index      = demod_ftc_cmd->mac_index;
    subaset.aset_list[0].drc_cover      = hdrsrch_get_drc_cover( ref_sect_ptr->link_id );
    subaset.aset_list[0].rab_length     = demod_ftc_cmd->rab_length;
    subaset.aset_list[0].rab_offset     = demod_ftc_cmd->rab_offset;

    hdrsrch_aset_update_revb( SYS_ACTIVE_PROT_HDR_REL0,          /* RUP Subtype               */
                             1,                                   /* Number of subactive sets  */
                             &subaset,                            /* Subactive set list        */
                             HDRMDSP_DEMOD_CARRIER_0,             /* Subactive set carrying CC */
                             0);
  
    (void) hdrhitcmd_disable_early_decode(!(demod_ftc_cmd->arq_enable));

    hdrhitmsg_hdr_ftm_state.fwd_curr_state = HDRHITMSG_HDR_FTM_TRAFFIC;

    hdrhit_msg_rsp.demod_ftc.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_demod_ftc_rsp_msg_t ) );

    return;
}/* hdrhitmsg_process_demod_ftc */


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_MOD_ACC

DESCRIPTION
  This function starts the access probes for Rev0.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_mod_acc
(
    hdrhitmsg_s_type *msg
)
{
    hdrhitmsg_access_params_t   *access_params_ptr  = & ( hdrhitmsg_ftm_intf.access_params );
    hdrhitmsg_rmac_params_t     *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_mod_acc_cmd_msg_t    *mod_acc_cmd;
    hdrhit_msg_rsp_u            hdrhit_msg_rsp;
#ifdef FEATURE_HDR_RUMI
    uint32                       hdr_rtc_cx8;
    uint32                       hstr;
    uint32                       rtc_offset;
#endif /* FEATURE_HDR_RUMI */

/*-----------------------------------------------------------------------*/

#ifdef FEATURE_HDR_RUMI
    if ( !hdrhit_rumi_cfg.tx_only_support )
    {
#endif /* FEATURE_HDR_RUMI */
    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_MOD_ACC );
    mod_acc_cmd     = &(msg->msg.hdrhit_cmd_msg.mod_acc);

    if (hdrhitmsg_hdr_ftm_state.rev_curr_state == HDRHITMSG_HDR_FTM_REV_TRAFFIC)
    {
        hdrhit_msg_rsp.mod_acc.cmd_status = HDRHIT_NO_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_acc_rsp_msg_t ) );
        return;
    }

    if ( ( hdrhitmsg_ftm_intf.mode_type_reva != 0 ) || ( hdrhitmsg_ftm_intf.mode_type_revb != 0 ))
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, 
          "Cannot do Rev0 access in RevA/B mode. Move to idle state");
                                    
        
        hdrhit_msg_rsp.mod_acc.cmd_status = HDRHIT_NO_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_acc_rsp_msg_t ) );

        return;
    }

    if ((hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_ENTER)||
        (hdrhitmsg_hdr_ftm_state.fwd_curr_state ==  HDRHITMSG_HDR_FTM_PIL_ACQ))
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "In FTM_ENTER OR FTM_PIL_ACQ state, cannot do access");

        hdrhit_msg_rsp.mod_acc.cmd_status = HDRHIT_NO_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_acc_rsp_msg_t ) );

        return;
    }

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "Got the HDR MOD ACC COMMAND");

    hdrhitmsg_hdr_ftm_state.rev_curr_state      = HDRHITMSG_HDR_FTM_SYS_ACC;
    rmac_params_ptr->modem_fwd_rev_link_type    = 0;
    access_params_ptr->open_loop_adjust         = mod_acc_cmd->open_loop_adjust;
    access_params_ptr->probe_initial_adjust     = mod_acc_cmd->probe_initial_adjust;
    access_params_ptr->access_cycle_duration    = mod_acc_cmd->access_cycle_duration;
    access_params_ptr->power_step               = mod_acc_cmd->power_step;
    access_params_ptr->probe_num_step           = mod_acc_cmd->probe_num_step;
    access_params_ptr->preamble_len             = mod_acc_cmd->preamble_len;

    /* Set silence duration to zero */
    hdramac_set_silence_params(0,1);

    qw_set(hdrhitmsg_acc_i_mask, mod_acc_cmd->aclc_mask_i_hi, mod_acc_cmd->aclc_mask_i_lo);
    qw_set(hdrhitmsg_acc_q_mask, mod_acc_cmd->aclc_mask_q_hi, mod_acc_cmd->aclc_mask_q_lo);

    hdramac_start_attempt(HDRHAI_PHYSICAL_LAYER_PROTOCOL);
    hdrmod_tx_start();
      /* Prepare modulator to start tx */
    DALSYS_BusyWait(27000);  
      /* one frame-delay - 26.6 ms */
    hdrhitmsg_ftm_intf.mode_type_rev0 = 1;

    hdrhit_msg_rsp.mod_acc.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_acc_rsp_msg_t ) );
#ifdef FEATURE_HDR_RUMI
    }
    else
    {
        mod_acc_cmd     = &(msg->msg.hdrhit_cmd_msg.mod_acc);
        if (hdrhitmsg_hdr_ftm_state.fwd_curr_state != HDRHITMSG_HDR_FTM_ENTER)
        {
            HDR_MSG_HIT(MSG_LEGACY_HIGH, "RUMI TX only: must in FTM_ENTER state");

            hdrhit_msg_rsp.mod_acc.cmd_status = HDRHIT_NO_SUCCESS;
            hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_acc_rsp_msg_t ) );

            return;
        }

        hdrhitmsg_hdr_ftm_state.rev_curr_state      = HDRHITMSG_HDR_FTM_SYS_ACC;
        hdrhitmsg_hdr_ftm_state.fwd_curr_state      = HDRHITMSG_HDR_FTM_IDLE;

        HDR_MSG_HIT(MSG_LEGACY_HIGH, "FTM TX only: Got the HDR MOD ACC COMMAND");

        ( void ) rex_set_sigs( HDRTX_TASK_PTR, HDRTX_ENTER_HDR_SIG );
        hdrsrchmcpm_set_state( HDRSRCHMCPM_WAKE_UP_REQ );
          /* Tell MCPM to turn on clocks grouped under wake-up */

        hdrmdsp_fws_enable_app();
          /* Enable fws hdr app */

        hdrmdsp_config_hdr_app( HDRFW_STATE_ACTIVE, FALSE, FALSE );

        DALSYS_BusyWait(60000);  
          /* wait 30 ms for response */

#ifdef FEATURE_HDR_BOLT_MODEM
        hdr_rtc_cx8 = hdrts_get_rtc_cx8();
          /* read the current hdr rtc */
#else
        hdr_rtc_cx8 = ( HWIO_IN( HDR_RTC_CNT )) << 2;
          /* read the current hdr rtc */
#endif /* FEATURE_HDR_BOLT_MODEM */
          
        hdrhitmsg_cdma_sys_time =  (hdr_rtc_cx8+5000) >> 3;
          /* Set SYS CDMA time to 625 chips ahead of RTC */      
        hstr = hdrhitmsg_cdma_sys_time & 0x7ffff;
        hstr /= 1024;
          /* Find HSTR at CDMA SYS time, hstr = (cdma_sys_time%0x80000)/1024 */
        
        rtc_offset = hdr_rtc_cx8 + 0x40000 - ( ( hdrhitmsg_cdma_sys_time << 3 ) & 0x3ffff );
        /* HDR RTC is in chip ratex8. It wraps around at 32768 chip
           36768*8= 0x40000. */
        rtc_offset &= 0x3ffff;

        hdrhitmsg_tx_action = HDRHITMSG_TX_ACC_REV0;

        /* Read RTC again to be as current as possible */
#ifdef FEATURE_HDR_BOLT_MODEM
        hdr_rtc_cx8 = hdrts_get_rtc_cx8();
#else
        hdr_rtc_cx8 = ( HWIO_IN( HDR_RTC_CNT )) << 2;
#endif /* FEATURE_HDR_BOLT_MODEM */

        hdrsrchdrv_start_slam( TRUE, hstr, rtc_offset,
                               hdrhitmsg_cdma_sys_time,hdr_rtc_cx8 );
          /* Pass timing to firmware */

        HDR_MSG_SRCH_4( MSG_LEGACY_HIGH, 
             "RUMI TX only access:  rtc_offset=0x%x hstr=0x%x rtc_cx8=0x%x cdma_sys_time=0x%x",
              rtc_offset, hstr, hdr_rtc_cx8, hdrhitmsg_cdma_sys_time );

        rmac_params_ptr->modem_fwd_rev_link_type    = 0;
        access_params_ptr->open_loop_adjust         = mod_acc_cmd->open_loop_adjust;
        access_params_ptr->probe_initial_adjust     = mod_acc_cmd->probe_initial_adjust;
        access_params_ptr->access_cycle_duration    = mod_acc_cmd->access_cycle_duration;
        access_params_ptr->power_step               = mod_acc_cmd->power_step;
        access_params_ptr->probe_num_step           = mod_acc_cmd->probe_num_step;
        access_params_ptr->preamble_len             = mod_acc_cmd->preamble_len;

        qw_set(hdrhitmsg_tx_only.i_mask, mod_acc_cmd->aclc_mask_i_hi, mod_acc_cmd->aclc_mask_i_lo);
        qw_set(hdrhitmsg_tx_only.q_mask, mod_acc_cmd->aclc_mask_q_hi, mod_acc_cmd->aclc_mask_q_lo);

        hdramac_activate(HDRHAI_PHYSICAL_LAYER_PROTOCOL);
    }
#endif /* FEATURE_HDR_RUMI */
    return;
}/* hdrhitmsg_process_mod_acc */

/*===========================================================================

FUNCTION HDRHITMSG_SET_PN_LONG_CODE_MASK

DESCRIPTION
  Load the 42-bit I and Q long code masks into the modulator.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void hdrhitmsg_set_pn_long_code_mask( void )
{
#ifdef FEATURE_HDR_RUMI
  hdrmod_set_pn_long_code_mask( hdrhitmsg_tx_only.i_mask, hdrhitmsg_tx_only.q_mask
#else /* FEATURE_HDR_RUMI_TX_ONLY */
  hdrmod_set_pn_long_code_mask( hdrhitmsg_acc_i_mask, hdrhitmsg_acc_q_mask
#endif /* FEATURE_HDR_RUMI */
#ifdef FEATURE_HDR_BOLT_MODEM
    , 7    /* Enable PN generator for all three carriers */
#endif /* FEATURE_HDR_BOLT_MODEM */
    );

} /* hdrhitmsg_set_pn_long_code_mask */

#ifdef FEATURE_HDR_RUMI_TX_ONLY
/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_MOD_RTC

DESCRIPTION
  This function starts the reverse line traffic channel for Rev0.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_mod_rtc
(
    hdrhitmsg_s_type *msg
)
{
    hdrhit_msg_rsp_u            hdrhit_msg_rsp;
    hdrhitmsg_rmac_params_t     *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_mod_rtc_cmd_msg_t    *mod_rtc_cmd;
    uint32                      hdr_rtc_cx8;
    uint32                      hstr;
    uint32                      rtc_offset;
/*-------------------------------------------------------------------------*/   

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "FTM TX only: Got the HDR MOD RTC COMMAND");

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_MOD_RTC );
    mod_rtc_cmd     = &(msg->msg.hdrhit_cmd_msg.mod_rtc);

    if (hdrhitmsg_hdr_ftm_state.rev_curr_state != HDRHITMSG_HDR_FTM_ENTER)
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "RUMI TX only: must in FTM_ENTER state");

        hdrhit_msg_rsp.mod_rtc.cmd_status = HDRHIT_NO_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_rsp_msg_t ) );

        return;
    }

    hdrts_init();
      /* Initialize timeline. The function is safe to call multiple times */

    hdrhitmsg_hdr_ftm_state.rev_curr_state      = HDRHITMSG_HDR_FTM_REV_TRAFFIC;
    hdrhitmsg_hdr_ftm_state.fwd_curr_state      = HDRHITMSG_HDR_FTM_IDLE;

    hdrhitmsg_ftm_intf.mode_type_rev0 = 1;

    rmac_params_ptr->power_params[0]   =   mod_rtc_cmd->dataoffset_nom;
    rmac_params_ptr->power_params[1]   =   mod_rtc_cmd->dataoffset_9k6;
    rmac_params_ptr->power_params[2]   =   mod_rtc_cmd->dataoffset_19k2;
    rmac_params_ptr->power_params[3]   =   mod_rtc_cmd->dataoffset_38k4;
    rmac_params_ptr->power_params[4]   =   mod_rtc_cmd->dataoffset_76k8;
    rmac_params_ptr->power_params[5]   =   mod_rtc_cmd->dataoffset_153k6;

    rmac_params_ptr->tx_duty_cycle  =  mod_rtc_cmd->rtc_txduty_cycle;
    rmac_params_ptr->tx_rri         =  mod_rtc_cmd->rri;
    rmac_params_ptr->tx_payload     =  hdrenc_get_rri_sizes( (int) mod_rtc_cmd->rri );
    rmac_params_ptr->rpc_enable     =  mod_rtc_cmd->rpc_enable;

    qw_set(hdrhitmsg_tx_only.i_mask, mod_rtc_cmd->rtclc_mask_i_hi, mod_rtc_cmd->rtclc_mask_i_lo);
    qw_set(hdrhitmsg_tx_only.q_mask, mod_rtc_cmd->rtclc_mask_q_hi, mod_rtc_cmd->rtclc_mask_q_lo);

#ifdef FEATURE_MEMCPY_REMOVAL
    hdrhitmsg_msg_buf = *msg;
#else
    memcpy(&hdrhitmsg_msg_buf, msg, sizeof(hdrhitmsg_s_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

    hdrhit_msg_rsp.mod_rtc.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_rsp_msg_t ) );

    ( void ) rex_set_sigs( HDRTX_TASK_PTR, HDRTX_ENTER_HDR_SIG );
    hdrsrchmcpm_set_state( HDRSRCHMCPM_WAKE_UP_REQ );
      /* Tell MCPM to turn on clocks grouped under wake-up */
    hdrsrchmcpm_set_state( HDRSRCHMCPM_START_TX_REQ );
      /* Start the Tx clock, so that we can write to TX HWIO regs */

    hdrmdsp_fws_enable_app();
      /* Enable fws hdr app */

    hdrmdsp_config_hdr_app( HDRFW_STATE_ACTIVE, FALSE,FALSE );

    DALSYS_BusyWait(60000);  
      /* wait 30 ms for response */
#ifdef FEATURE_HDR_BOLT_MODEM
    hdr_rtc_cx8 = hdrts_get_rtc_cx8();
#else
    hdr_rtc_cx8 = ( HWIO_IN( HDR_RTC_CNT )) << 2;
#endif /* FEATURE_HDR_BOLT_MODEM */

      /* read the current hdr rtc */
    hdrhitmsg_cdma_sys_time =  (hdr_rtc_cx8+5000) >> 3;
      /* Set CDMA SYS time to 625 chips ahead of RTC */   

    hstr = hdrhitmsg_cdma_sys_time & 0x7ffff;
    hstr /= 1024;
      /* Get hstr at CDMA SYS time, hstr = (cdma_sys_time%0x80000)/1024 */
    
    rtc_offset = hdr_rtc_cx8 + 0x40000 - 
                 ( ( hdrhitmsg_cdma_sys_time << 3 ) & 0x3ffff );
      /* HDR RTC is in chip ratex8. It wraps around at 32768 chip
         36768*8= 0x40000. */
    rtc_offset &= 0x3ffff;

    /* Read RTC again to be as current as possible */
#ifdef FEATURE_HDR_BOLT_MODEM
    hdr_rtc_cx8 = hdrts_get_rtc_cx8();
#else
    hdr_rtc_cx8 = ( HWIO_IN( HDR_RTC_CNT )) << 2;
#endif /* FEATURE_HDR_BOLT_MODEM */

    hdrsrchdrv_start_slam( TRUE, hstr, rtc_offset,
                           hdrhitmsg_cdma_sys_time,hdr_rtc_cx8 );
      /* Pass timing to firmware */

    HDR_MSG_SRCH_4( MSG_LEGACY_HIGH, 
         "RUMI TX only RTC:  rtc_offset=0x%x hstr=0x%x rtc_cx8=0x%x cdma_sys_time=0x%x",
          rtc_offset, hstr, hdr_rtc_cx8, hdrhitmsg_cdma_sys_time );

    hdrhitmsg_tx_action = HDRHITMSG_TX_RTC_REV0;                

    return;

} /* hdrhitmsg_process_mode_rtc */

#else /* FEATURE_HDR_RUMI_TX_ONLY */

/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_MOD_RTC

DESCRIPTION
  This function starts the reverse line traffic channel for Rev0.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_mod_rtc
(
    hdrhitmsg_s_type *msg
)
{
    hdrhitmsg_access_params_t   *access_params_ptr  = & ( hdrhitmsg_ftm_intf.access_params );
    hdrhitmsg_rmac_params_t     *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_mod_rtc_cmd_msg_t    *mod_rtc_cmd;
    hdrhit_msg_rsp_u            hdrhit_msg_rsp;
    hdrsrch_sect_struct_type*   ref_sect_ptr;
    sys_link_type               link_info;
    qword                       i_mask;
    qword                       q_mask;
    int16                       total_pwr;
    int16                       pilot_pwr;
    int32                      *ptr;
    uint8                       drclocklength_slots_rev0 = 1;
    uint8                       hdr_ack_mode;
    hdrfmac_state_enum_type     hdr_ftm_fmac_state;
    uint8                       i;
/*-------------------------------------------------------------------------*/   

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_MOD_RTC );
    mod_rtc_cmd     = &(msg->msg.hdrhit_cmd_msg.mod_rtc);
    ref_sect_ptr    = hdrsrchutil_get_ref_sector_ptr();

    hdrhitmsg_ftm_intf.fwd_link_info_rev0.drc_cover = mod_rtc_cmd->drc_cover;

    if ((hdrhitmsg_ftm_intf.mode_type_reva != 0) || (hdrhitmsg_ftm_intf.mode_type_revb != 0))
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, 
                    "Cannot do Rev0 mod_rtc in RevA/B mode");

        hdrhit_msg_rsp.mod_rtc.cmd_status = HDRHIT_NO_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_rsp_msg_t ) );

        return;
    }

    if ((hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_ENTER)||
        (hdrhitmsg_hdr_ftm_state.fwd_curr_state ==  HDRHITMSG_HDR_FTM_PIL_ACQ))
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "In FTM_ENTER OR FTM_PIL_ACQ state, cannot do mod_rtc.");

        hdrhit_msg_rsp.mod_rtc.cmd_status = HDRHIT_NO_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_rsp_msg_t ) );

        return;
    }
    if ( hdrhitmsg_ftm_intf.ftm_not_doing_hho )
    {
        if ( hdrhitmsg_hdr_ftm_state.rev_curr_state == HDRHITMSG_HDR_FTM_REV_TRAFFIC )
        {
            HDR_MSG_HIT( MSG_LEGACY_HIGH, "Modulate RTC: off" );
            hdrrmac_deactivate( HDRHAI_PHYSICAL_LAYER_PROTOCOL );
            hdrlmac_idle_mode( HDRHAI_PHYSICAL_LAYER_PROTOCOL );
            hdrhitmsg_hdr_ftm_state.rev_curr_state = HDRHITMSG_HDR_FTM_SYS_ACC;
            hdramac_activate( HDRHAI_PHYSICAL_LAYER_PROTOCOL );

            hdrhit_msg_rsp.mod_rtc.cmd_status = HDRHIT_SUCCESS;
            hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_rsp_msg_t ) );

            return;
        }
        else
        {
            if ( hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_FWD_TRAFFIC_ONLY
                 && hdrhitmsg_ftm_intf.first_time_rev_link_rev0 != 0 )
            {
                hdrhitmsg_ftm_intf.first_time_rev_link_rev0 = 0;
                hdrlmac_idle_mode( HDRHAI_PHYSICAL_LAYER_PROTOCOL );  
                DALSYS_BusyWait( 500 );

            }
        }
        if (hdrhitmsg_hdr_ftm_state.rev_curr_state != HDRHITMSG_HDR_FTM_SYS_ACC)
        {
            hdrhit_msg_rsp.mod_rtc.cmd_status = HDRHIT_INVALID_STATE;
            hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_rsp_msg_t ) );  
            return;
        }
    }

    /* Deactivate AC MAC */
    hdramac_deactivate(HDRHAI_PHYSICAL_LAYER_PROTOCOL, FALSE);

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "Modulate RTC command: on");

    rmac_params_ptr->modem_fwd_rev_link_type = 0;


    if ( hdrsrch_get_link_info( ref_sect_ptr->link_id, &link_info ) != E_SUCCESS )
    {
        HDR_MSG_HIT(MSG_LEGACY_ERROR, "Unknown link ID" );
    }
    else
    {
        HDR_MSG_HIT_2(MSG_LEGACY_HIGH, " Chan:%d PN:%d", link_info.channel.chan_num, link_info.pilot_pn);
    }

    (void) hdrhitcmd_enable_drc_gating(mod_rtc_cmd->drc_gating);

    rmac_params_ptr->power_params[0]   =   mod_rtc_cmd->dataoffset_nom;
    rmac_params_ptr->power_params[1]   =   mod_rtc_cmd->dataoffset_9k6;
    rmac_params_ptr->power_params[2]   =   mod_rtc_cmd->dataoffset_19k2;
    rmac_params_ptr->power_params[3]   =   mod_rtc_cmd->dataoffset_38k4;
    rmac_params_ptr->power_params[4]   =   mod_rtc_cmd->dataoffset_76k8;
    rmac_params_ptr->power_params[5]   =   mod_rtc_cmd->dataoffset_153k6;

    ptr = (int32 *)hdrrmac0_get_hdrrmac_pkt();

    for (i=0;i<(512/4);i++)
    {
        *ptr  =   mod_rtc_cmd->rtc_txpattern;
        ptr++;
    }

    rmac_params_ptr->tx_duty_cycle  =  mod_rtc_cmd->rtc_txduty_cycle;
    rmac_params_ptr->tx_rri         =  mod_rtc_cmd->rri;
    rmac_params_ptr->tx_payload     =  hdrenc_get_rri_sizes( (int) mod_rtc_cmd->rri );
    rmac_params_ptr->rpc_enable     =  mod_rtc_cmd->rpc_enable;

    qw_set(i_mask, mod_rtc_cmd->rtclc_mask_i_hi, mod_rtc_cmd->rtclc_mask_i_lo);
    qw_set(q_mask, mod_rtc_cmd->rtclc_mask_q_hi, mod_rtc_cmd->rtclc_mask_q_lo);

    rfm_hdr_set_nominal_pwr( RFM_DEVICE_0, access_params_ptr->open_loop_adjust );
    (void) rfm_hdr_set_tx_power_adjust( RFM_DEVICE_0, access_params_ptr->probe_initial_adjust );  

    for ( i = 0; i < hdrhitmsg_ftm_intf.fwd_link_info_rev0.drclocklength + 1; i++ )
    {
        drclocklength_slots_rev0 *= 2;
    }

    (void) hdrhitcmd_fmac_activate_msg( 
             HDRHITCMD_REV_0, 
             drclocklength_slots_rev0*2,
             ( hdrhitmsg_ftm_intf.fwd_link_info_rev0.drclockperiod + 1 )*8,
             12 
           );

    /* Start the Tx clock, so that we can write to TX HWIO regs */
    hdrsrchmcpm_set_state( HDRSRCHMCPM_START_TX_REQ );

    hdrrmac_activate( HDRHAI_PHYSICAL_LAYER_PROTOCOL );
    /* Program the User Long Code Mask into the modulator. */
    hdrmod_set_pn_long_code_mask( i_mask, q_mask
#ifdef FEATURE_HDR_BOLT_MODEM
      , 1                 /* Enable carrier 0 PN generator */
#endif /* FEATURE_HDR_BOLT_MODEM */
      );

    /* Make the mobile ACK, NAK, or not ACK every slot */
    hdr_ftm_fmac_state = hdrfmacsm_get_state();
    hdr_ack_mode = mod_rtc_cmd->hdr_ack_mode;

    /* Sanity check; plus make this code backward compatible */
    if (hdr_ack_mode > (byte) IS890A_MODE_NAK_OOK)
    {
        hdr_ack_mode = (byte) IS856_COMPLIANT;
    }

    if (hdr_ftm_fmac_state == HDRFMAC_INACTIVE_STATE)
    {
        hdrfmacsm_change_state( HDRFMAC_VAR_RATE_STATE );
    }

    hdrfmac_set_ack_mode ((hdrfmac_ack_mode_enum_type) hdr_ack_mode);

    if (hdr_ftm_fmac_state == HDRFMAC_INACTIVE_STATE)
    {
        hdrfmacsm_change_state( HDRFMAC_INACTIVE_STATE );
    }

    memset(&hdrhitmsg_ftm_tca_msg, 0, sizeof(hdrrup_tca_msg_type));
    hdrhitmsg_ftm_tca_msg.msg_id = 0;
    hdrhitmsg_ftm_tca_msg.msg_seq = 0;
    hdrhitmsg_ftm_tca_msg.chan_incl = 0;
    hdrhitmsg_ftm_tca_msg.frame_offset = mod_rtc_cmd->frame_offset;
    hdrhitmsg_ftm_tca_msg.dsc_channel_gain = 0;
    hdrhitmsg_ftm_tca_msg.link_id = ref_sect_ptr->link_id;
    hdrhitmsg_ftm_tca_msg.num_sectors = 1;
    hdrhitmsg_ftm_tca_msg.num_sub_asets = 1;
    hdrhitmsg_ftm_tca_msg.scheduler_tag_incl = 0;
    hdrhitmsg_ftm_tca_msg.feedback_mux_enabled = 0;
    hdrhitmsg_ftm_tca_msg.sector_info[0].drc_cover = mod_rtc_cmd->drc_cover;
    hdrhitmsg_ftm_tca_msg.sector_info[0].pilot_pn = link_info.pilot_pn;
    hdrhitmsg_ftm_tca_msg.sector_info[0].rab_length = hdrhitmsg_ftm_intf.fwd_link_info_rev0.rablength;
    hdrhitmsg_ftm_tca_msg.sector_info[0].rab_offset = hdrhitmsg_ftm_intf.fwd_link_info_rev0.raboffset;
    hdrhitmsg_ftm_tca_msg.sector_info[0].softer_handoff = FALSE;
    hdrhitmsg_ftm_tca_msg.sector_info[0].ra_channel_gain = 0;
    hdrhitmsg_ftm_tca_msg.sector_info[0].dsc = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].pilot_included = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.drc_length = mod_rtc_cmd->drc_length;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.drc_channel_gain = mod_rtc_cmd->drc_gain;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.ack_channel_gain = mod_rtc_cmd->ack_gain;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.feedback_rev_ch_id = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].fwd_channel[0].mode = SYS_HDR_CHAN_REC_MODE_IS856;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].fwd_channel[0].channel = mod_rtc_cmd->hdr_chan;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].ftc_params[0].ftc_mac_index = 
        hdrhitmsg_ftm_intf.fwd_link_info_rev0.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].aux_drc_cover = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].rev_mac_index =
         hdrhitmsg_ftm_intf.fwd_link_info_rev0.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].rab_mac_index = 
        hdrhitmsg_ftm_intf.fwd_link_info_rev0.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].delta_t2p = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].pilot_group_id = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].scheduler_tag = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_rev_channels_incl = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_rev_channels = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].channel = mod_rtc_cmd->hdr_chan;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].a_order = 0; 
       /* Should be same as feedback rev chan id */
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].ch_config = 1; 

    hdrlmac_tc_assign( TRUE,                          /* Initial assignment  */
                       0,                             /* Link id             */
                       HDRSRCH_TUNE_AWAY_NONE,        /* Tuneaway disallowed */
                       HDRMDSP_DEMOD_CARRIER_0,       /* CC Subaset index    */
                       &hdrhitmsg_ftm_tca_msg,                      /* TCA msg ptr         */
                       HDRHAI_PHYSICAL_LAYER_PROTOCOL,/* Sender              */
                       HDRSCP_DEFAULT_RUP             /* RUP subtype         */    
                     );

     /* This is a critical delay needed so that due to different task priorities 
     RF TX START happens before L1 TX DAC START. From an experiement, 3 ms yields
     ok Tx intermittently, 5 ms was consistently OK. Using 10 ms to be safe. */
     DALSYS_BusyWait(20000);

     hdrmod_tx_start();  
     DALSYS_BusyWait(27000);  
      /* one frame-delay - 26.6 ms */

    /* Set RPC step size: Macro takes units of 1/2 dB.
       In RPC_step field, 0 = .5 dB steps
                        1 = 1.0 dB steps. 
       (Thus 1 is added to convert)  */
    (void) hdrhitcmd_modulator_cfg(HDRMDSP_DEMOD_CARRIER_0, FALSE, FALSE, 
                                   (mod_rtc_cmd->rpc_step + 1), TRUE);

    /* Disable throttling */
    hdrrmac_disable_throttling( HDRRMAC_FTM_ID, TRUE );

    total_pwr = rfm_hdr_get_tx_total_power(RFM_DEVICE_0,
                                          RFM_HDR_CARRIER_ID__0,
                                          RFM_CDMA_AGC_FORMAT__DBM256) /256;
  
    pilot_pwr = rfm_hdr_get_tx_pilot_power(RFM_DEVICE_0,
                                          RFM_HDR_CARRIER_ID__0,
                                          RFM_CDMA_AGC_FORMAT__DBM256) /256;

    hdrsrch_at_set_drc(HDRMDSP_DEMOD_CARRIER_0, mod_rtc_cmd->drc_value);

    HDR_MSG_HIT_1(MSG_LEGACY_HIGH, "Total TX Power is :%d", total_pwr);
    HDR_MSG_HIT_1(MSG_LEGACY_HIGH, "Pilot Power is :%d", pilot_pwr);

    hdrhitmsg_hdr_ftm_state.rev_curr_state    =   HDRHITMSG_HDR_FTM_REV_TRAFFIC;

    hdrhitmsg_ftm_intf.mode_type_rev0 = 1;

    hdrhit_msg_rsp.mod_rtc.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_rsp_msg_t ) );

    return;
}

#endif /* FEATURE_HDR_RUMI_TX_ONLY */
/*===========================================================================

FUNCTION HDRHITMSG_DEACTIVATE_DONE_CB

DESCRIPTION
  Callback function for L1.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_deactivate_done_cb( void )
{
#ifdef FEATURE_HDR_RUMI
  HDR_MSG_HIT( MSG_LEGACY_MED, "hit deact done cb");
#endif /* FEATURE_HDR_RUMI */

  (void) rex_set_sigs( HDRTX_TASK_PTR,  HDRTX_EXIT_HDR_SIG);
  (void) rex_set_sigs( HDRRX_TASK_PTR,  HDRRX_EXIT_HDR_SIG);
}/* hdrhitmsg_deactivate_done_cb */


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_DEASSIGN

DESCRIPTION
  This function turns off HDR traffic channel .

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_deassign
(
    hdrhitmsg_s_type *msg
)
{
    hdrhit_msg_rsp_u          hdrhit_msg_rsp;
    hdrfmac_state_enum_type   hdr_ftm_fmac_state;
/*---------------------------------------------------------------------*/

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_DEASSIGN );
    HDR_MSG_HIT( MSG_LEGACY_HIGH, "Got the HDR DEASSIGN COMMAND" );

    if (( hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_ENTER ) &&
        ( hdrhitmsg_hdr_ftm_state.rev_curr_state == HDRHITMSG_HDR_FTM_ENTER ))
    {
        HDR_MSG_HIT( MSG_LEGACY_HIGH, "Already deassigned,no-op" );
        hdrhit_msg_rsp.deassign.cmd_status = HDRHIT_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_deassign_rsp_msg_t ) );

        return;
    }

    /* Restore the previous value of ACK mode: that before going into the non-signaling mode */
    hdr_ftm_fmac_state = hdrfmacsm_get_state();

    if ( hdr_ftm_fmac_state == HDRFMAC_INACTIVE_STATE )
    {
        hdrfmacsm_change_state( HDRFMAC_VAR_RATE_STATE );
    }

    /* Restore the default ACK mode */
    hdrfmac_set_ack_mode ( IS856_COMPLIANT );

    if ( hdr_ftm_fmac_state == HDRFMAC_INACTIVE_STATE )
    {
        hdrfmacsm_change_state( HDRFMAC_INACTIVE_STATE );
    }

    if ( hdrhitmsg_hdr_ftm_state.rev_curr_state == HDRHITMSG_HDR_FTM_REV_TRAFFIC )
    {
        hdrrmac_deactivate( HDRHAI_PHYSICAL_LAYER_PROTOCOL );
    }
    else
    {
        hdramac_deactivate( HDRHAI_PHYSICAL_LAYER_PROTOCOL, FALSE );
    }

    DALSYS_BusyWait(27000);  
      /* one frame-delay - 26.6 ms */

    hdrsrch_deactivate( hdrhitmsg_deactivate_done_cb );

    hdrhitmsg_hdr_ftm_state.fwd_curr_state  = HDRHITMSG_HDR_FTM_ENTER;
    hdrhitmsg_hdr_ftm_state.rev_curr_state  = HDRHITMSG_HDR_FTM_ENTER;
    hdrhitmsg_reset_hdr_mode_rev_link();

    hdrhit_msg_rsp.deassign.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_deassign_rsp_msg_t ) );

    return;
} /* hdrhitmsg_process_deassign */


/*===========================================================================

FUNCTION HDRHITMSG_FTM_GET_AP_MSG

DESCRIPTION
  This function sets the required value for the fields of ap_msg.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_ftm_get_ap_msg
( 
  hdrovhd_ap_msg_type *ap_msg
)
{
    hdrhitmsg_access_params_t   *access_params_ptr  = & ( hdrhitmsg_ftm_intf.access_params );
/*----------------------------------------------------------------------------------------------*/

    ap_msg->access_cycle_duration               = access_params_ptr->access_cycle_duration;
    ap_msg->probe_init_adjust                   = access_params_ptr->probe_initial_adjust;
    ap_msg->nominal_pwr                         = access_params_ptr->open_loop_adjust;
    ap_msg->power_step                          = access_params_ptr->power_step;
    ap_msg->capsule_length_max                  = 8;
    ap_msg->preamble_len                        = access_params_ptr->preamble_len;
    ap_msg->probe_num_step                      = access_params_ptr->probe_num_step;
    ap_msg->apersistence[0]                     = 0;
    ap_msg->enhanced_access_parameters_included = access_params_ptr->enhanced_acc_enable;
    ap_msg->preamble_length_slots               = access_params_ptr->preamble_len_slots;  
            /* PreambleLengthSlots */
    ap_msg->access_offset                       = access_params_ptr->access_offset;         
           /* AccessOffset */
    ap_msg->sector_access_max_rate              = access_params_ptr->sector_max_rate;     
           /* SectorAccessMaxRate */
    ap_msg->probe_time_out_adjust               = 0;                                       
           /* ProbeTimeOutAdjust */
    ap_msg->pilot_strength_nominal              = 0;                                        
           /* PilotStrengthNominal */
    ap_msg->pilot_strength_correction_min       = 0;                                      
           /* PilotStrengthCorrectionMin */
    ap_msg->pilot_strength_correction_max       = 0;                                      
          /* PilotStrengthCorrectionMax */
} /* hdrhitmsg_ftm_get_ap_msg */


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_IDLE_STATE

DESCRIPTION
  This function configures the MAC for Rev A.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_idle_state
(
    hdrhitmsg_s_type *msg
)
{
    hdrhit_msg_rsp_u            hdrhit_msg_rsp;
/*-------------------------------------------------------------------------*/

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_IDLE_STATE );
    HDR_MSG_HIT(MSG_LEGACY_HIGH, "Got the FTM Idle state command"); 

    if (hdrhitmsg_hdr_ftm_state.rev_curr_state == HDRHITMSG_HDR_FTM_REV_TRAFFIC)
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "Modulate RTC: off");
        hdrrmac_deactivate(HDRHAI_PHYSICAL_LAYER_PROTOCOL);
        hdrlmac_idle_mode(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

        hdrhitmsg_hdr_ftm_state.rev_curr_state = HDRHITMSG_HDR_FTM_SYS_ACC;
        hdrhitmsg_hdr_ftm_state.fwd_curr_state = HDRHITMSG_HDR_FTM_IDLE;
        hdrhitmsg_reset_hdr_mode_rev_link();

        hdramac_activate(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

        hdrhit_msg_rsp.idle_state.cmd_status = HDRHIT_SUCCESS;

    }
    else
    {
        if (hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_FWD_TRAFFIC_ONLY)
        {
            HDR_MSG_HIT(MSG_LEGACY_HIGH, "Demodulate FTC only off:");
            hdrlmac_idle_mode(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

            hdrhitmsg_hdr_ftm_state.rev_curr_state = HDRHITMSG_HDR_FTM_SYS_ACC;
            hdrhitmsg_hdr_ftm_state.fwd_curr_state = HDRHITMSG_HDR_FTM_IDLE;
            hdrhitmsg_reset_hdr_mode_rev_link();

            hdramac_activate(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

            hdrhit_msg_rsp.idle_state.cmd_status = HDRHIT_SUCCESS;

        }
        else if (hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_IDLE)
        {
            HDR_MSG_HIT(MSG_LEGACY_HIGH, "Already in IDLE state:");

            hdrhitmsg_hdr_ftm_state.rev_curr_state = HDRHITMSG_HDR_FTM_SYS_ACC;
            hdrhitmsg_reset_hdr_mode_rev_link();

            hdramac_activate(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

            hdrhit_msg_rsp.idle_state.cmd_status = HDRHIT_SUCCESS;

        }
        else
        {
            HDR_MSG_HIT(MSG_LEGACY_HIGH, "Cannot move to IDLE state:");

            hdrhit_msg_rsp.idle_state.cmd_status = HDRHIT_NO_SUCCESS;

        }
    }

    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_idle_state_rsp_msg_t ) );

} /* hdrhitmsg_process_idle_state */



/*===========================================================================

FUNCTION HDRHITMSG_TC_ASSIGN_COMPLETE_CB

DESCRIPTION
  Callback function passed to HDR TC assignment command.

DEPENDENCIES

RETURN VALUE
  Will be called when HDR traffic channel is assigned.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrhitmsg_tc_assign_complete_cb ( hdrsrch_tca_result_type result )
{
  uint8 carriers;
  uint8 drc[1];
  uint8 i;
/*-------------------------------------------------------------------------*/

#if defined(FEATURE_HDR_RUMI) && defined(FEATURE_HDR_REVB_SUPPORTED)
  if ( hdrhit_rumi_cfg.revb_carriers != 0 )
  {    
    carriers = hdrhit_rumi_cfg.revb_carriers;
    for (i = 0; i < carriers; i++ )
    {
      if ( hdrhit_rumi_cfg.revb_drc[i] != 0 )
      {
        /* if hdrhitmsg_revb_drc is unset, use hdrhit_rumi_cfg.drc_override */
        drc[i] = hdrhit_rumi_cfg.revb_drc[i];
      }
      else 
      {
        drc[i] = hdrhitmsg_ftm_intf.drc_value;
      }
    }
  }
  else
#endif /* FEATURE_HDR_RUMI && FEATURE_HDR_REVB_SUPPORTED */
  {
    drc[0] = hdrhitmsg_ftm_intf.drc_value;
    carriers = 1;
  }

  HDR_MSG_HIT(MSG_LEGACY_HIGH, "TC Assign Callback");


  (void) hdrhitcmd_modulator_cfg(HDRMDSP_DEMOD_CARRIER_0, TRUE, TRUE, 0, FALSE);    
  (void) hdrhitcmd_handoff_override(carriers, 1);
  (void) hdrhitcmd_hyperspace_cfg (TRUE);

  (void) hdrhitcmd_drc_cfg(TRUE,
                           TRUE,
                           drc,
                           TRUE,
                           TRUE,
                           carriers);
  return;
} /* hdrlmac_tc_assign_complete_cb */

/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_DEMOD_FW_LINK_REV0

DESCRIPTION
  This function satrts the demodulation of forward link for Rev0 without 
  reverse link transmitting.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_demod_fw_link_rev0
(
    hdrhitmsg_s_type *msg
)
{
    hdrhitmsg_rmac_params_t     *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );  
    hdrhit_demod_fw_link_rev0_cmd_msg_t *demod_fw_link_rev0_cmd;
    hdrhit_msg_rsp_u                    hdrhit_msg_rsp;
    hdrsrch_sect_struct_type*           ref_sect_ptr;
    sys_link_type                       link_info;
    uint8                               drclocklength_slots = 1;
    uint8                               i;
    hdrsrch_subactive_set_type          subaset;
/*-------------------------------------------------------------------------*/ 

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_DEMOD_FW_LINK_REV0 );
    demod_fw_link_rev0_cmd  = &(msg->msg.hdrhit_cmd_msg.demod_fw_link_rev0);
    ref_sect_ptr            = hdrsrchutil_get_ref_sector_ptr();

    if ((hdrhitmsg_ftm_intf.mode_type_reva != 0) || (hdrhitmsg_ftm_intf.mode_type_revb != 0))
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, 
              "Cannot change the parameters of forward traffic channel only for Rev0");
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "in RevA/B mode. Move to idle state");

        hdrhit_msg_rsp.demod_fw_link_rev0.cmd_status = HDRHIT_INVALID_STATE;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_demod_fw_link_rev0_rsp_msg_t ) );
        return;
    }

    if (hdrhitmsg_hdr_ftm_state.rev_curr_state == HDRHITMSG_HDR_FTM_REV_TRAFFIC)
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH,
            "Cannot change the parameters requested for forward link with reverse link on");
         HDR_MSG_HIT(MSG_LEGACY_HIGH, "Move to idle Mode");

        hdrhit_msg_rsp.demod_fw_link_rev0.cmd_status = HDRHIT_INVALID_STATE;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_demod_fw_link_rev0_rsp_msg_t ) );
        return;
    }

    if (hdrhitmsg_hdr_ftm_state.fwd_curr_state !=   HDRHITMSG_HDR_FTM_IDLE)
    {
        if (hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_FWD_TRAFFIC_ONLY)
        {
            hdrlmac_idle_mode(HDRHAI_PHYSICAL_LAYER_PROTOCOL);
            DALSYS_BusyWait(1000);
        }
        else
        {
#ifndef FEATURE_HDR_RUMI
            HDR_MSG_HIT(MSG_LEGACY_HIGH,
              "Cannot change the parameters requested for forward link. Move to idle mode");
            hdrhit_msg_rsp.demod_fw_link_rev0.cmd_status = HDRHIT_INVALID_STATE;
            hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_demod_fw_link_rev0_rsp_msg_t ) );
            return;
#endif /* FEATURE_HDR_RUMI */
        }
        
    }

    subaset.channel         = demod_fw_link_rev0_cmd->hdr_chan;
    subaset.systype         = SYS_HDR_CHAN_REC_MODE_IS856;
    subaset.not_reportable  = FALSE;
    subaset.num_aset_pilots = 1;
    subaset.demod_idx       = HDRMDSP_DEMOD_CARRIER_0;

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "Demodulating HDR forward traffic with no reverse link on for Rev0 ...");

    hdrhitmsg_ftm_intf.fwd_link_info_rev0.drclockperiod  = demod_fw_link_rev0_cmd->drc_lockperiod;
    hdrhitmsg_ftm_intf.fwd_link_info_rev0.drclocklength  = demod_fw_link_rev0_cmd->drc_locklength;
    hdrhitmsg_ftm_intf.fwd_link_info_rev0.mac_index      = demod_fw_link_rev0_cmd->mac_index;
    hdrhitmsg_ftm_intf.fwd_link_info_rev0.rablength      = demod_fw_link_rev0_cmd->rab_length;
    hdrhitmsg_ftm_intf.fwd_link_info_rev0.raboffset      = demod_fw_link_rev0_cmd->rab_offset;

    hdrhitmsg_ftm_intf.drc_value  = demod_fw_link_rev0_cmd->drc_value;

    rmac_params_ptr->modem_fwd_rev_link_type = 0;

    if (hdrsrch_get_link_info(ref_sect_ptr->link_id, &link_info) != E_SUCCESS)
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "Unknown link ID: %d!");
    }
    else
    {
        HDR_MSG_HIT_2(MSG_LEGACY_HIGH, "Chan:%d PN:%d", link_info.channel.chan_num, link_info.pilot_pn);
        subaset.aset_list[0].pilot_pn = link_info.pilot_pn;
    }

    for ( i = 0; i < hdrhitmsg_ftm_intf.fwd_link_info_rev0.drclocklength + 1; i++ )
    {
        drclocklength_slots = drclocklength_slots*2;
    }

    (void) hdrhitcmd_fmac_activate_msg( 
              HDRHITCMD_REV_0, 
              drclocklength_slots * 2, 
              ( hdrhitmsg_ftm_intf.fwd_link_info_rev0.drclockperiod + 1 ) * 8, 
              12 );
    (void) hdrhitcmd_disable_early_decode( !( demod_fw_link_rev0_cmd->arq_enable ) );

    subaset.aset_list[0].softer_handoff = 0;
    subaset.aset_list[0].mac_index  = hdrhitmsg_ftm_intf.fwd_link_info_rev0.mac_index;
    subaset.aset_list[0].drc_cover  = demod_fw_link_rev0_cmd->drc_cover;
    subaset.aset_list[0].rab_length = hdrhitmsg_ftm_intf.fwd_link_info_rev0.rablength;
    subaset.aset_list[0].rab_offset = hdrhitmsg_ftm_intf.fwd_link_info_rev0.raboffset;

    hdrsrch_chan_and_aset_assign_revb(
         SYS_ACTIVE_PROT_HDR_REL0,            /* RUP Subtype                  */
         HDRSRCH_TUNE_AWAY_NONE,               /* 1x Tuneaway disallowed       */
         1,                                    /* Number of subactive sets     */
         &subaset,                             /* Subactive set list           */
         HDRMDSP_DEMOD_CARRIER_0,              /* Subactive set carrying CC    */
         0,                                     
         hdrhitmsg_tc_assign_complete_cb,      /* TC assigned call back        */
         NULL,                                 /* Add/Delete scheduler grps cb */
         NULL
         );
        
    hdrhitmsg_hdr_ftm_state.fwd_curr_state      = HDRHITMSG_HDR_FTM_FWD_TRAFFIC_ONLY;
    hdrhitmsg_ftm_intf.first_time_rev_link_rev0 = 1;
    hdrhitmsg_ftm_intf.mode_type_rev0           = 1;

    hdrhit_msg_rsp.demod_fw_link_rev0.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_demod_fw_link_rev0_rsp_msg_t ) );

    return;
} /* hdrhitmsg_process_demod_fw_link_rev0 */


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_RELA_MAC_CONFIG

DESCRIPTION
  This function configures the MAC for Rev A.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_rela_mac_config
(
    hdrhitmsg_s_type *msg
)
{  
    hdrhitmsg_rmac_params_t             *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_rela_mac_config_cmd_msg_t    *rela_mac_config_cmd;
    hdrhit_msg_rsp_u                    hdrhit_msg_rsp;
/*-------------------------------------------------------------------------*/
    rela_mac_config_cmd     = &(msg->msg.hdrhit_cmd_msg.rela_mac_config);
    HDR_MSG_HIT(MSG_LEGACY_HIGH, "Got the HDR Rev A MAC Config COMMAND");   

    HDR_MSG_HIT_3(MSG_LEGACY_HIGH, "HDR Rel A MAC Config Command MAC=%d param0=%d param1=%d",
                                   0,
                                   rela_mac_config_cmd->sht_pkt_enabled,
                                   rela_mac_config_cmd->mup_enabled);

    rmac_params_ptr->mup_enabled     = rela_mac_config_cmd->mup_enabled;
    rmac_params_ptr->sht_pkt_enabled = rela_mac_config_cmd->sht_pkt_enabled;

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_RELA_MAC_CONFIG );
    hdrhit_msg_rsp.rela_mac_config.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_rela_mac_config_rsp_msg_t ) );

    return;
} /* hdrhitmsg_process_rela_mac_config */


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_DEMOD_FW_LINK_REVA

DESCRIPTION
  This function satrts the demodulation of forward link without reverse 
  link transmitting for RevA.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_demod_fw_link_reva
(
    hdrhitmsg_s_type *msg
)
{  
    hdrhitmsg_rmac_params_t             *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_demod_fw_link_reva_cmd_msg_t *demod_fw_link_reva_cmd;
    hdrhit_msg_rsp_u                    hdrhit_msg_rsp;
    hdrsrch_sect_struct_type*           ref_sect_ptr;
    sys_link_type                       link_info;
    uint8                               drclocklength_slots = 1;
    uint8                               i;
    hdrhitcmd_system_mode_type          mode;
    uint8                               num_subaset;
    sys_active_prot_e_type              prot_type; 
    uint8                               max_drc;
    hdrsrch_subactive_set_type          subaset[3];
/*-------------------------------------------------------------------------*/

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_DEMOD_FW_LINK_REVA );
    demod_fw_link_reva_cmd  = &(msg->msg.hdrhit_cmd_msg.demod_fw_link_reva);
    ref_sect_ptr            = hdrsrchutil_get_ref_sector_ptr();

 
    if ( hdrhitmsg_ftm_intf.mode_type_rev0 != 0 )
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, 
              "Cannot change the parameters of forward traffic channel only for RevA/B");
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "in RevA/B mode. Move to idle state");

        hdrhit_msg_rsp.demod_fw_link_reva.cmd_status = HDRHIT_INVALID_STATE;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_demod_fw_link_reva_rsp_msg_t ) );

        return;
    }

    if (hdrhitmsg_hdr_ftm_state.rev_curr_state == HDRHITMSG_HDR_FTM_REV_TRAFFIC)
    {

        HDR_MSG_HIT(MSG_LEGACY_HIGH,
            "Cannot change the parameters requested for forward link with reverse link on");
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "Move to idle Mode");

        hdrhit_msg_rsp.demod_fw_link_reva.cmd_status = HDRHIT_INVALID_STATE;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_demod_fw_link_reva_rsp_msg_t ) );

        return;
    }

    if (hdrhitmsg_hdr_ftm_state.fwd_curr_state != HDRHITMSG_HDR_FTM_IDLE)
    {
        if (hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_FWD_TRAFFIC_ONLY)
        {
            hdrlmac_idle_mode(HDRHAI_PHYSICAL_LAYER_PROTOCOL);
            DALSYS_BusyWait(1000);
        }
        else
        {
#ifndef FEATURE_HDR_RUMI
           HDR_MSG_HIT(MSG_LEGACY_HIGH,
              "Cannot change the parameters requested for forward link. Move to idle mode");
            hdrhit_msg_rsp.demod_fw_link_reva.cmd_status = HDRHIT_INVALID_STATE;
            hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_demod_fw_link_reva_rsp_msg_t ) );

            return;
#endif /* FEATURE_HDR_RUMI */
        }
    }

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "Demodulating HDR forward traffic with no reverse link on for RevA/B ...");

    hdrhitmsg_ftm_intf.fwd_link_info_reva.drclockperiod   =   demod_fw_link_reva_cmd->drc_lockperiod;
    hdrhitmsg_ftm_intf.fwd_link_info_reva.drclocklength   =   demod_fw_link_reva_cmd->drc_locklength;
    hdrhitmsg_ftm_intf.fwd_link_info_reva.mac_index       =   demod_fw_link_reva_cmd->mac_index;
    hdrhitmsg_ftm_intf.fwd_link_info_reva.rablength       =   demod_fw_link_reva_cmd->rab_length;
    hdrhitmsg_ftm_intf.fwd_link_info_reva.raboffset       =   demod_fw_link_reva_cmd->rab_offset;
    hdrhitmsg_ftm_intf.drc_value                          =   demod_fw_link_reva_cmd->drc_value;

#if defined(FEATURE_HDR_RUMI) && defined(FEATURE_HDR_REVB_SUPPORTED)
    if ( hdrhit_rumi_cfg.revb_carriers != 0 )
    {
        hdrhitmsg_ftm_intf.mode_type_revb = 1;
        rmac_params_ptr->modem_fwd_rev_link_type = 2;
        mode = 2;
        num_subaset = hdrhit_rumi_cfg.revb_carriers;
        prot_type = SYS_ACTIVE_PROT_HDR_RELB;
        max_drc = 27;
    }
    else
#endif /* FEATURE_HDR_RUMI && FEATURE_HDR_REVB_SUPPORTED */
    {
        hdrhitmsg_ftm_intf.mode_type_reva = 1;
        rmac_params_ptr->modem_fwd_rev_link_type = 1;
        mode = 1;
        num_subaset = 1;
        prot_type = SYS_ACTIVE_PROT_HDR_RELA;
        max_drc = 14;
    }

    subaset[0].channel         = demod_fw_link_reva_cmd->hdr_chan;
    subaset[0].systype         = SYS_HDR_CHAN_REC_MODE_IS856;
    subaset[0].not_reportable  = FALSE;
    subaset[0].num_aset_pilots = 1;
    subaset[0].demod_idx       = HDRMDSP_DEMOD_CARRIER_0;

    if ( hdrsrch_get_link_info( ref_sect_ptr->link_id, &link_info ) != E_SUCCESS )
    {
        HDR_MSG_HIT_1( MSG_LEGACY_HIGH, "Unknown link ID: %d!", 0 );
    }
    else
    {
        HDR_MSG_HIT_2( MSG_LEGACY_HIGH, "Chan:%d PN:%d", link_info.channel.chan_num, link_info.pilot_pn );
        subaset[0].aset_list[0].pilot_pn = link_info.pilot_pn;
    }
    for ( i=0; i < hdrhitmsg_ftm_intf.fwd_link_info_reva.drclockperiod + 1; i++ )
    {
        drclocklength_slots *= 2;
    }

    (void) hdrhitcmd_fmac_activate_msg(
               mode,
               drclocklength_slots*4,
               (hdrhitmsg_ftm_intf.fwd_link_info_reva.drclockperiod + 1) * 8,
               max_drc);
    (void) hdrhitcmd_disable_early_decode(!(demod_fw_link_reva_cmd->arq_enable));
  
    subaset[0].aset_list[0].softer_handoff  = 0;
    subaset[0].aset_list[0].mac_index       = hdrhitmsg_ftm_intf.fwd_link_info_reva.mac_index;
    subaset[0].aset_list[0].drc_cover       = demod_fw_link_reva_cmd->drc_cover;
    subaset[0].aset_list[0].rab_length      = hdrhitmsg_ftm_intf.fwd_link_info_reva.rablength;
    subaset[0].aset_list[0].rab_offset      = hdrhitmsg_ftm_intf.fwd_link_info_reva.raboffset;
    subaset[0].aset_list[0].dsc             = demod_fw_link_reva_cmd->dsc_value;
    subaset[0].aset_list[0].ra_channel_gain = 0;    

#if defined(FEATURE_HDR_RUMI) && defined(FEATURE_HDR_REVB_SUPPORTED)
    if ( hdrhit_rumi_cfg.revb_carriers != 0)
    {
        subaset[0].aset_list[0].aux_drc_cover   = 1;

        memcpy( &subaset[1], &subaset[0], sizeof(subaset[0]) );
        memcpy( &subaset[2], &subaset[0], sizeof(subaset[0]) );

        subaset[1].demod_idx = 1;
        subaset[2].demod_idx = 2;

        subaset[1].channel.chan_num = subaset[0].channel.chan_num + 1;
        subaset[2].channel.chan_num = subaset[0].channel.chan_num + 2;
    }
#endif /* FEATURE_HDR_RUMI && FEATURE_HDR_REVB_SUPPORTED */

    hdrsrch_chan_and_aset_assign_revb(
        prot_type,                       /* RUP Subtype                  */
        HDRSRCH_TUNE_AWAY_NONE,          /* 1x Tuneaway disallowed       */
        num_subaset,                     /* Number of subactive sets     */
        subaset,                         /* Subactive set list           */
        HDRMDSP_DEMOD_CARRIER_0,         /* Subactive set carrying CC    */
        0,   
        hdrhitmsg_tc_assign_complete_cb, /* TC assigned call back        */
        NULL,                            /* Add/Delete scheduler grps cb */
        NULL
    );

#ifdef FEATURE_HDR_RUMI
    hdrhitmsg_hdr_ftm_state.fwd_curr_state      = HDRHITMSG_HDR_FTM_FWD_TRAFFIC_ONLY;
    hdrhitmsg_ftm_intf.first_time_rev_link_reva = 1;
    hdrhitmsg_ftm_intf.mode_type_reva           = 1;
#endif /* FEATURE_HDR_RUMI */

    hdrhit_msg_rsp.demod_fw_link_reva.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_demod_fw_link_reva_rsp_msg_t ) );

    return;
} /* hdrhitmsg_process_demod_fw_link_reva */

#ifdef FEATURE_HDR_RUMI_TX_ONLY
/*===========================================================================

FUNCTION  HDRHITMSG_PROCESS_MOD_ACC_REVA

DESCRIPTION
  This function satrts the access probe sequence for RevA

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_mod_acc_reva
(
    hdrhitmsg_s_type *msg
)
{  
    hdrhitmsg_access_params_t       *access_params_ptr  = & ( hdrhitmsg_ftm_intf.access_params );
    hdrhitmsg_rmac_params_t         *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_mod_acc_reva_cmd_msg_t   *mod_acc_reva_cmd;
    hdrhit_msg_rsp_u                hdrhit_msg_rsp;

    uint32                       hdr_rtc_cx8;
    uint32                       hstr;
    uint32                       rtc_offset;
/*-------------------------------------------------------------------------*/

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_MOD_ACC_REVA );
    mod_acc_reva_cmd = &( msg->msg.hdrhit_cmd_msg.mod_acc_reva );

    if (hdrhitmsg_hdr_ftm_state.fwd_curr_state != HDRHITMSG_HDR_FTM_ENTER)
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "RUMI TX only: must in FTM_ENTER state");

        hdrhit_msg_rsp.mod_acc_reva.cmd_status = HDRHIT_NO_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_acc_reva_rsp_msg_t ) );

        return;
    }

    hdrhitmsg_hdr_ftm_state.rev_curr_state      = HDRHITMSG_HDR_FTM_SYS_ACC;
    hdrhitmsg_hdr_ftm_state.fwd_curr_state      = HDRHITMSG_HDR_FTM_IDLE;

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "FTM TX only: Got the HDR MOD ACC REVA COMMAND");

    ( void ) rex_set_sigs( HDRTX_TASK_PTR, HDRTX_ENTER_HDR_SIG );
    hdrsrchmcpm_set_state( HDRSRCHMCPM_WAKE_UP_REQ );
      /* Tell MCPM to turn on clocks grouped under wake-up */

    hdrmdsp_fws_enable_app();
      /* Enable fws hdr app */

    hdrmdsp_config_hdr_app( HDRFW_STATE_ACTIVE, FALSE,FALSE );

    DALSYS_BusyWait(60000);  
      /* wait 30 ms for response */
#ifdef FEATURE_HDR_BOLT_MODEM
    hdr_rtc_cx8 = hdrts_get_rtc_cx8();
#else
    hdr_rtc_cx8 = ( HWIO_IN( HDR_RTC_CNT )) << 2;
#endif /* FEATURE_HDR_BOLT_MODEM */

      /* read the current hdr rtc */
    hdrhitmsg_cdma_sys_time =  (hdr_rtc_cx8 + 5000) >> 3;
      /* force rtc_offset to be 0 */
    hstr = hdrhitmsg_cdma_sys_time & 0x7ffff;
    hstr /= 1024;
      /* hstr = (cdma_sys_time%0x80000)/1024 */
    
    rtc_offset = hdr_rtc_cx8 + 0x40000 - 
                 ( ( hdrhitmsg_cdma_sys_time << 3 ) & 0x3ffff );
    /* HDR RTC is in chip ratex8. It wraps around at 32768 chip
       36768*8= 0x40000. */
    rtc_offset &= 0x3ffff;

    /* Read RTC again to be as current as possible */
#ifdef FEATURE_HDR_BOLT_MODEM
    hdr_rtc_cx8 = hdrts_get_rtc_cx8();
#else
    hdr_rtc_cx8 = ( HWIO_IN( HDR_RTC_CNT )) << 2;
#endif /* FEATURE_HDR_BOLT_MODEM */

    hdrsrchdrv_start_slam( TRUE, hstr, rtc_offset,
                           hdrhitmsg_cdma_sys_time,hdr_rtc_cx8 );
      /* Pass timing to firmware */

    HDR_MSG_SRCH_4( MSG_LEGACY_HIGH, 
         "RUMI TX only access reva:  rtc_offset=0x%x hstr=0x%x rtc_cx8=0x%x cdma_sys_time=0x%x",
          rtc_offset, hstr, hdr_rtc_cx8, hdrhitmsg_cdma_sys_time );

    hdrhitmsg_tx_action = HDRHITMSG_TX_ACC_REVA;                    

    rmac_params_ptr->modem_fwd_rev_link_type = 1;

    access_params_ptr->open_loop_adjust      = mod_acc_reva_cmd->open_loop_adjust;
    access_params_ptr->probe_initial_adjust  = mod_acc_reva_cmd->probe_initial_adjust;
    access_params_ptr->access_cycle_duration = mod_acc_reva_cmd->access_cycle_duration;
    access_params_ptr->power_step            = mod_acc_reva_cmd->power_step;
    access_params_ptr->probe_num_step        = mod_acc_reva_cmd->probe_num_step;
    access_params_ptr->preamble_len          = mod_acc_reva_cmd->preamble_len;

    access_params_ptr->enhanced_acc_enable  = mod_acc_reva_cmd->enhanced_access_enable;  
    access_params_ptr->preamble_len_slots   = mod_acc_reva_cmd->preamble_length_slots;
    access_params_ptr->access_offset        = mod_acc_reva_cmd->access_offset;
    access_params_ptr->sector_max_rate      = mod_acc_reva_cmd->sector_access_rate_max;
    access_params_ptr->terminal_max_rate    = mod_acc_reva_cmd->terminal_access_rate_max;  


    if ( access_params_ptr->sector_max_rate >= access_params_ptr->terminal_max_rate )
    {
        access_params_ptr->access_max_rate = access_params_ptr->sector_max_rate;
    }
    else
    {
        access_params_ptr->access_max_rate = access_params_ptr->terminal_max_rate;
    }

    qw_set( hdrhitmsg_tx_only.i_mask, mod_acc_reva_cmd->aclc_mask_i_hi, mod_acc_reva_cmd->aclc_mask_i_lo );
    qw_set( hdrhitmsg_tx_only.q_mask, mod_acc_reva_cmd->aclc_mask_q_hi, mod_acc_reva_cmd->aclc_mask_q_lo );

    hdramac_activate(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

    HDR_MSG_HIT( MSG_LEGACY_HIGH, "RUMI TX only,sending hit_mode_acc_reva_rsp"); 
    hdrhit_msg_rsp.mod_acc_reva.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_acc_reva_rsp_msg_t ) );

    return;

} /* hdrhitmsg_process_mod_acc_reva */
#else
/*===========================================================================

FUNCTION  HDRHITMSG_PROCESS_MOD_ACC_REVA

DESCRIPTION
  This function satrts the access probe sequence for RevA

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_mod_acc_reva
(
    hdrhitmsg_s_type *msg
)
{  
    hdrhitmsg_access_params_t       *access_params_ptr  = & ( hdrhitmsg_ftm_intf.access_params );
    hdrhitmsg_rmac_params_t         *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_mod_acc_reva_cmd_msg_t   *mod_acc_reva_cmd;
    hdrhit_msg_rsp_u                hdrhit_msg_rsp;
/*-------------------------------------------------------------------------*/

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_MOD_ACC_REVA );
    mod_acc_reva_cmd = &( msg->msg.hdrhit_cmd_msg.mod_acc_reva );

    if ( hdrhitmsg_hdr_ftm_state.rev_curr_state == HDRHITMSG_HDR_FTM_REV_TRAFFIC )
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "In reverse traffic state, no-op");
        hdrhit_msg_rsp.mod_acc_reva.cmd_status = HDRHIT_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_acc_reva_rsp_msg_t ) );
            
        return;
    }

    /* Post enter signal to HDRTX task for it to process signals set 
     by hdrmod_tx_start() */
    ( void ) rex_set_sigs( HDRTX_TASK_PTR, HDRTX_ENTER_HDR_SIG );

    if ( hdrhitmsg_ftm_intf.mode_type_rev0 != 0 )
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, 
                "Cannot do REVA access in Rev0 mode. Move to idle state");

        hdrhit_msg_rsp.mod_acc_reva.cmd_status = HDRHIT_INVALID_STATE;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_acc_reva_rsp_msg_t ) );

        return;
    }

    if ( ( hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_ENTER )
       ||( hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_PIL_ACQ ))
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "In FTM_ENTER OR FTM_PIL_ACQ state, cannot do REVA access");

        hdrhit_msg_rsp.mod_acc_reva.cmd_status = HDRHIT_INVALID_STATE;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_acc_reva_rsp_msg_t ) );

        return;
    }

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "Got the HDR MOD ACC COMMAND.");

    hdrhitmsg_hdr_ftm_state.rev_curr_state   = HDRHITMSG_HDR_FTM_SYS_ACC;
    rmac_params_ptr->modem_fwd_rev_link_type = 1;
    access_params_ptr->open_loop_adjust      = mod_acc_reva_cmd->open_loop_adjust;
    access_params_ptr->probe_initial_adjust  = mod_acc_reva_cmd->probe_initial_adjust;
    access_params_ptr->access_cycle_duration = mod_acc_reva_cmd->access_cycle_duration;
    access_params_ptr->power_step            = mod_acc_reva_cmd->power_step;
    access_params_ptr->probe_num_step        = mod_acc_reva_cmd->probe_num_step;
    access_params_ptr->preamble_len          = mod_acc_reva_cmd->preamble_len;

    access_params_ptr->enhanced_acc_enable  = mod_acc_reva_cmd->enhanced_access_enable;  
    access_params_ptr->preamble_len_slots   = mod_acc_reva_cmd->preamble_length_slots;
    access_params_ptr->access_offset        = mod_acc_reva_cmd->access_offset;
    access_params_ptr->sector_max_rate      = mod_acc_reva_cmd->sector_access_rate_max;
    access_params_ptr->terminal_max_rate    = mod_acc_reva_cmd->terminal_access_rate_max;  

    if ( access_params_ptr->sector_max_rate >= access_params_ptr->terminal_max_rate )
    {
        access_params_ptr->access_max_rate = access_params_ptr->sector_max_rate;
    }
    else
    {
        access_params_ptr->access_max_rate = access_params_ptr->terminal_max_rate;
    }

    /* Set silence duration to zero */
    hdramac_set_silence_params(0,1);

    qw_set( hdrhitmsg_acc_i_mask, mod_acc_reva_cmd->aclc_mask_i_hi, mod_acc_reva_cmd->aclc_mask_i_lo );
    qw_set( hdrhitmsg_acc_q_mask, mod_acc_reva_cmd->aclc_mask_q_hi, mod_acc_reva_cmd->aclc_mask_q_lo );

    hdramac_start_attempt(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

    /* Prepare modulator to start tx */
    hdrmod_tx_start();
    DALSYS_BusyWait(27000);  
     /* one frame-delay - 26.6 ms */

    hdrhitmsg_ftm_intf.mode_type_reva = 1;
    hdrhit_msg_rsp.mod_acc_reva.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_acc_reva_rsp_msg_t ) );

    return;

} /* hdrhitmsg_process_mod_acc_reva */

#endif /* FEATURE_HDR_RUMI_TX_ONLY */

#ifdef FEATURE_HDR_RUMI_TX_ONLY
/*===========================================================================

FUNCTION  HDRHITMSG_PROCESS_MOD_RTC_REVA

DESCRIPTION
  This function satrts the reverse traffic channel for RevA

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_mod_rtc_reva
(
    hdrhitmsg_s_type *msg
)
{
    hdrhitmsg_rmac_params_t         *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_mod_rtc_reva_cmd_msg_t   *mod_rtc_reva_cmd;

    hdrhit_msg_rsp_u            hdrhit_msg_rsp;

    uint32                       hdr_rtc_cx8;
    uint32                       hstr;
    uint32                       rtc_offset;
/*-------------------------------------------------------------------------*/   

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "FTM TX only: Got the HDR MOD RTC REVA COMMAND");
    mod_rtc_reva_cmd = &(msg->msg.hdrhit_cmd_msg.mod_rtc_reva);

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_MOD_RTC_REVA );

    if (hdrhitmsg_hdr_ftm_state.rev_curr_state != HDRHITMSG_HDR_FTM_ENTER)
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "RUMI TX only: must in FTM_ENTER state");

        hdrhit_msg_rsp.mod_rtc_reva.cmd_status = HDRHIT_NO_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_reva_rsp_msg_t ) );

        return;
    }

    hdrts_init();
      /* Initialize timeline. The function is safe to call multiple times */

    hdrhitmsg_hdr_ftm_state.rev_curr_state      = HDRHITMSG_HDR_FTM_REV_TRAFFIC;
    hdrhitmsg_hdr_ftm_state.fwd_curr_state      = HDRHITMSG_HDR_FTM_IDLE;

    hdrhitmsg_ftm_intf.mode_type_reva = 1;

    rmac_params_ptr->hicap_lolatency             = mod_rtc_reva_cmd->hicap_lolatency;
    rmac_params_ptr->gain_transition_point       = mod_rtc_reva_cmd->gain_transition_point;
    rmac_params_ptr->t2p_pre_transition          = mod_rtc_reva_cmd->t2p_pre_transition;
    rmac_params_ptr->t2p_post_transition         = mod_rtc_reva_cmd->t2p_post_transition;
    rmac_params_ptr->arq_mode                    = mod_rtc_reva_cmd->arq_mode;
    rmac_params_ptr->auxpil_payload_thresh       = mod_rtc_reva_cmd->auxpil_min_payload;
    rmac_params_ptr->auxpil_chan_gain            = mod_rtc_reva_cmd->auxpil_gain;
    rmac_params_ptr->rri_ch_gain_pre_transition  = mod_rtc_reva_cmd->rri_gain_pre_transition;
    rmac_params_ptr->rri_ch_gain_post_transition = mod_rtc_reva_cmd->rri_gain_post_transition;
    rmac_params_ptr->dsc_length                  = mod_rtc_reva_cmd->dsc_length;
    rmac_params_ptr->rtc_pattern                 = mod_rtc_reva_cmd->rtc_txpattern;
    rmac_params_ptr->tx_rri_reva                 = mod_rtc_reva_cmd->rri;
    rmac_params_ptr->rpc_enable                  = mod_rtc_reva_cmd->rpc_enable;
    rmac_params_ptr->rpc_step                    = mod_rtc_reva_cmd->rpc_step;    
      /*should not add 1 here because hdrrmac3.c adds 1*/
    rmac_params_ptr->tx_payload_reva             = 
                     hdrenc_get_rri_reva_sizes( (int) mod_rtc_reva_cmd->rri );

    qw_set( hdrhitmsg_tx_only.i_mask, mod_rtc_reva_cmd->rtclc_mask_i_hi, mod_rtc_reva_cmd->rtclc_mask_i_lo );
    qw_set( hdrhitmsg_tx_only.q_mask, mod_rtc_reva_cmd->rtclc_mask_q_hi, mod_rtc_reva_cmd->rtclc_mask_q_lo );

#ifdef FEATURE_MEMCPY_REMOVAL
    hdrhitmsg_msg_buf = *msg;
#else
    memcpy(&hdrhitmsg_msg_buf, msg, sizeof(hdrhitmsg_s_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

    hdrhit_msg_rsp.mod_rtc_reva.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_reva_rsp_msg_t ) );

    ( void ) rex_set_sigs( HDRTX_TASK_PTR, HDRTX_ENTER_HDR_SIG );
    hdrsrchmcpm_set_state( HDRSRCHMCPM_WAKE_UP_REQ );
      /* Tell MCPM to turn on clocks grouped under wake-up */
    hdrsrchmcpm_set_state( HDRSRCHMCPM_START_TX_REQ );
      /* Start the Tx clock, so that we can write to TX HWIO regs */

    hdrmdsp_fws_enable_app();
      /* Enable fws hdr app */

    hdrmdsp_config_hdr_app( HDRFW_STATE_ACTIVE, FALSE,FALSE );

    DALSYS_BusyWait(60000);  
      /* wait 30 ms for response */
#ifdef FEATURE_HDR_BOLT_MODEM
    hdr_rtc_cx8 = hdrts_get_rtc_cx8();
#else
    hdr_rtc_cx8 = ( HWIO_IN( HDR_RTC_CNT )) << 2;
#endif /* FEATURE_HDR_BOLT_MODEM */

      /* read the current hdr rtc */
    hdrhitmsg_cdma_sys_time =  (hdr_rtc_cx8+5000) >> 3;
      /* force rtc_offset to be 0 */
    hstr = hdrhitmsg_cdma_sys_time & 0x7ffff;

    hstr /= 1024;
      /* hstr = (cdma_sys_time%0x80000)/1024 */
    
    rtc_offset = ( hdr_rtc_cx8 + 0x40000 ) - 
                 ( ( hdrhitmsg_cdma_sys_time << 3 ) & 0x3ffff );
    /* HDR RTC is in chip ratex8. It wraps around at 32768 chip
       36768*8= 0x40000. */
    rtc_offset &= 0x3ffff;

#ifdef FEATURE_HDR_BOLT_MODEM
    hdr_rtc_cx8 = hdrts_get_rtc_cx8();
#else
    hdr_rtc_cx8 = ( HWIO_IN( HDR_RTC_CNT )) << 2;
#endif /* FEATURE_HDR_BOLT_MODEM */

    hdrsrchdrv_start_slam( TRUE, hstr, rtc_offset,
                           hdrhitmsg_cdma_sys_time,hdr_rtc_cx8 );
      /* Pass timing to firmware */

    HDR_MSG_SRCH_4( MSG_LEGACY_HIGH, 
         "RUMI TX only RTC RevA:  rtc_offset=0x%x hstr=0x%x rtc_cx8=0x%x cdma_sys_time=0x%x",
          rtc_offset, hstr, hdr_rtc_cx8, hdrhitmsg_cdma_sys_time );

    hdrhitmsg_tx_action = HDRHITMSG_TX_RTC_REVA;

    return;

} /* hdrhitmsg_process_mod_rtc_reva */
#else
/*===========================================================================

FUNCTION  HDRHITMSG_PROCESS_MOD_RTC_REVA

DESCRIPTION
  This function satrts the reverse traffic channel for RevA

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_mod_rtc_reva
(
    hdrhitmsg_s_type *msg
)
{  
    hdrhitmsg_access_params_t       *access_params_ptr  = & ( hdrhitmsg_ftm_intf.access_params );
    hdrhitmsg_rmac_params_t         *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_mod_rtc_reva_cmd_msg_t   *mod_rtc_reva_cmd;
    hdrhit_msg_rsp_u                hdrhit_msg_rsp;
    hdrsrch_sect_struct_type*       ref_sect_ptr;
    sys_link_type                   link_info;
    qword                           i_mask;
    qword                           q_mask;
    int16                           total_pwr;
    int16                           pilot_pwr;
    int16                           i;
    uint8                           hdr_ack_mode;
    uint8                           drclocklength_slots_reva = 1;
    hdrfmac_state_enum_type         hdr_ftm_fmac_state;
/*--------------------------------------------------------------------------*/

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_MOD_RTC_REVA );
    mod_rtc_reva_cmd = &(msg->msg.hdrhit_cmd_msg.mod_rtc_reva);
    ref_sect_ptr     = hdrsrchutil_get_ref_sector_ptr();

    hdrhitmsg_ftm_intf.fwd_link_info_reva.drc_cover = mod_rtc_reva_cmd->drc_cover;
    hdrhitmsg_ftm_intf.fwd_link_info_reva.dsc_value = mod_rtc_reva_cmd->dsc_value;

    if ( hdrhitmsg_ftm_intf.mode_type_rev0 != 0 )
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, 
                    "Cannot change the parameters of reverse traffic channel for RevA in Rev0 mode.");
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "Move to idle state");

        hdrhit_msg_rsp.mod_rtc_reva.cmd_status = HDRHIT_INVALID_STATE;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_reva_rsp_msg_t ) );

        return;
    }

    if ((hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_ENTER)||
        (hdrhitmsg_hdr_ftm_state.fwd_curr_state ==  HDRHITMSG_HDR_FTM_PIL_ACQ))
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "Not in right state.");
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "Cannot change the parameters of reverse traffic channel for RevA");

        hdrhit_msg_rsp.mod_rtc_reva.cmd_status = HDRHIT_INVALID_STATE;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_reva_rsp_msg_t ) );

        return;
    }

    if ( hdrhitmsg_ftm_intf.ftm_not_doing_hho )
    {
        if (hdrhitmsg_hdr_ftm_state.rev_curr_state == HDRHITMSG_HDR_FTM_REV_TRAFFIC)
        {
            HDR_MSG_HIT(MSG_LEGACY_HIGH, "Modulate RTC: off");
            hdrrmac_deactivate(HDRHAI_PHYSICAL_LAYER_PROTOCOL);
            hdrlmac_idle_mode(HDRHAI_PHYSICAL_LAYER_PROTOCOL);
            hdrhitmsg_hdr_ftm_state.rev_curr_state = HDRHITMSG_HDR_FTM_SYS_ACC;
            hdramac_activate(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

            hdrhit_msg_rsp.mod_rtc_reva.cmd_status = HDRHIT_SUCCESS;
            hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_reva_rsp_msg_t ) );

            return;
        }

        else
        {
            if (hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_FWD_TRAFFIC_ONLY
                && hdrhitmsg_ftm_intf.first_time_rev_link_reva != 0)
            {
                hdrhitmsg_ftm_intf.first_time_rev_link_reva = 0;
                hdrlmac_idle_mode(HDRHAI_PHYSICAL_LAYER_PROTOCOL);  
                DALSYS_BusyWait(500);

            }
        }

        if (hdrhitmsg_hdr_ftm_state.rev_curr_state != HDRHITMSG_HDR_FTM_SYS_ACC)
        {
            hdrhit_msg_rsp.mod_rtc_reva.cmd_status = HDRHIT_INVALID_STATE;
            hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_reva_rsp_msg_t ) );
            
            return;
        }
    }
    /* Deactivate AC MAC */
    hdramac_deactivate(HDRHAI_PHYSICAL_LAYER_PROTOCOL, FALSE);

    rmac_params_ptr->modem_fwd_rev_link_type = 1;

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "Modulate RTC command for RevA: on");

    if (hdrsrch_get_link_info(ref_sect_ptr->link_id, &link_info ) != E_SUCCESS)
    {
        ERR("Unknown link ID: %d!",0, 0, 0);
    }
    else
    {
        HDR_MSG_HIT_2(MSG_LEGACY_HIGH, "Chan:%d PN:%d", link_info.channel.chan_num, link_info.pilot_pn);
    }

   (void) hdrhitcmd_enable_drc_gating(mod_rtc_reva_cmd->drc_gating);
   
    rmac_params_ptr->hicap_lolatency             = mod_rtc_reva_cmd->hicap_lolatency;
    rmac_params_ptr->gain_transition_point       = mod_rtc_reva_cmd->gain_transition_point;
    rmac_params_ptr->t2p_pre_transition          = mod_rtc_reva_cmd->t2p_pre_transition;
    rmac_params_ptr->t2p_post_transition         = mod_rtc_reva_cmd->t2p_post_transition;
    rmac_params_ptr->arq_mode                    = mod_rtc_reva_cmd->arq_mode;
    rmac_params_ptr->auxpil_payload_thresh       = mod_rtc_reva_cmd->auxpil_min_payload;
    rmac_params_ptr->auxpil_chan_gain            = mod_rtc_reva_cmd->auxpil_gain;
    rmac_params_ptr->rri_ch_gain_pre_transition  = mod_rtc_reva_cmd->rri_gain_pre_transition;
    rmac_params_ptr->rri_ch_gain_post_transition = mod_rtc_reva_cmd->rri_gain_post_transition;
    rmac_params_ptr->dsc_length                  = mod_rtc_reva_cmd->dsc_length;
    rmac_params_ptr->rtc_pattern                 = mod_rtc_reva_cmd->rtc_txpattern;
    rmac_params_ptr->tx_rri_reva                 = mod_rtc_reva_cmd->rri;
    rmac_params_ptr->rpc_enable                  = mod_rtc_reva_cmd->rpc_enable;
    rmac_params_ptr->rpc_step                    = mod_rtc_reva_cmd->rpc_step;
      /*should not add 1 here because hdrrmac3.c adds 1*/
    rmac_params_ptr->tx_payload_reva             = 
                     hdrenc_get_rri_reva_sizes( (int) mod_rtc_reva_cmd->rri );

    qw_set( i_mask, mod_rtc_reva_cmd->rtclc_mask_i_hi, mod_rtc_reva_cmd->rtclc_mask_i_lo );
    qw_set( q_mask, mod_rtc_reva_cmd->rtclc_mask_q_hi, mod_rtc_reva_cmd->rtclc_mask_q_lo );

    rfm_hdr_set_nominal_pwr( RFM_DEVICE_0, access_params_ptr->open_loop_adjust );
    (void) rfm_hdr_set_tx_power_adjust( RFM_DEVICE_0, access_params_ptr->probe_initial_adjust );  


    for (i=0; i < hdrhitmsg_ftm_intf.fwd_link_info_reva.drclocklength + 1; i++)
    {
        drclocklength_slots_reva *= 2;
    }

    (void) hdrhitcmd_fmac_activate_msg( 
              1,
              drclocklength_slots_reva * 4,
              (hdrhitmsg_ftm_intf.fwd_link_info_reva.drclockperiod + 1)*8,
              14 );

    /* Start the Tx clock, so that we can write to TX HWIO regs */
    hdrsrchmcpm_set_state( HDRSRCHMCPM_START_TX_REQ );

    hdrrmac_activate(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

    /* Program the User Long Code Mask into the modulator. */
    hdrmod_set_pn_long_code_mask( i_mask, q_mask
#ifdef FEATURE_HDR_BOLT_MODEM
      , 1                 /* Enable carrier 0 PN generator */
#endif /* FEATURE_HDR_BOLT_MODEM */
      );

    /* Make the mobile ACK, NAK, or not ACK every slot */
    hdr_ftm_fmac_state = hdrfmacsm_get_state();
    hdr_ack_mode       = mod_rtc_reva_cmd->hdr_ack_mode;

/* Sanity check; plus make this code backward compatible */
    if (hdr_ack_mode > (byte) IS890A_MODE_NAK_OOK)
    {
        hdr_ack_mode = (byte) IS856_COMPLIANT;
    }

    if (hdr_ftm_fmac_state == HDRFMAC_INACTIVE_STATE)
    {
        hdrfmacsm_change_state( HDRFMAC_VAR_RATE_STATE );
    }

    hdrfmac_set_ack_mode ((hdrfmac_ack_mode_enum_type) hdr_ack_mode);

    if (hdr_ftm_fmac_state == HDRFMAC_INACTIVE_STATE)
    {
        hdrfmacsm_change_state( HDRFMAC_INACTIVE_STATE );
    }

    memset(&hdrhitmsg_ftm_tca_msg, 0, sizeof(hdrrup_tca_msg_type));
    hdrhitmsg_ftm_tca_msg.msg_id = 0;
    hdrhitmsg_ftm_tca_msg.msg_seq = 0;
    hdrhitmsg_ftm_tca_msg.chan_incl = 0;
    hdrhitmsg_ftm_tca_msg.frame_offset = mod_rtc_reva_cmd->frame_offset;
    hdrhitmsg_ftm_tca_msg.dsc_channel_gain = mod_rtc_reva_cmd->dsc_gain;
    hdrhitmsg_ftm_tca_msg.link_id = ref_sect_ptr->link_id;
    hdrhitmsg_ftm_tca_msg.num_sectors = 1;
    hdrhitmsg_ftm_tca_msg.num_sub_asets = 1;
    hdrhitmsg_ftm_tca_msg.scheduler_tag_incl = 0;
    hdrhitmsg_ftm_tca_msg.feedback_mux_enabled = 0;
    hdrhitmsg_ftm_tca_msg.sector_info[0].drc_cover = mod_rtc_reva_cmd->drc_cover;
    hdrhitmsg_ftm_tca_msg.sector_info[0].pilot_pn = link_info.pilot_pn;
    hdrhitmsg_ftm_tca_msg.sector_info[0].rab_length = hdrhitmsg_ftm_intf.fwd_link_info_reva.rablength;
    hdrhitmsg_ftm_tca_msg.sector_info[0].rab_offset = hdrhitmsg_ftm_intf.fwd_link_info_reva.raboffset;
    hdrhitmsg_ftm_tca_msg.sector_info[0].softer_handoff = FALSE;
    hdrhitmsg_ftm_tca_msg.sector_info[0].ra_channel_gain = 0;
    hdrhitmsg_ftm_tca_msg.sector_info[0].dsc = mod_rtc_reva_cmd->dsc_value;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].pilot_included = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.feedback_rev_ch_id = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.drc_length = mod_rtc_reva_cmd->drc_length;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.drc_channel_gain = mod_rtc_reva_cmd->drc_gain;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.ack_channel_gain = mod_rtc_reva_cmd->ack_gain;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].fwd_channel[0].mode = SYS_HDR_CHAN_REC_MODE_IS856;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].fwd_channel[0].channel = mod_rtc_reva_cmd->hdr_chan;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].ftc_params[0].ftc_mac_index = 
                         hdrhitmsg_ftm_intf.fwd_link_info_reva.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].aux_drc_cover = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].rev_mac_index = 
                         hdrhitmsg_ftm_intf.fwd_link_info_reva.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].rab_mac_index = 
                         hdrhitmsg_ftm_intf.fwd_link_info_reva.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].delta_t2p = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].pilot_group_id = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].scheduler_tag = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_rev_channels_incl = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_rev_channels = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_fwd_channels = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].channel = mod_rtc_reva_cmd->hdr_chan;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].a_order = 0; 
      /* Should be same as feedback rev chan id */
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].ch_config = 1; 

    hdrlmac_tc_assign( TRUE,                          /* Initial assignment  */
                       0,                             /* Link id             */
                       HDRSRCH_TUNE_AWAY_NONE,        /* Tuneaway disallowed */
                       HDRMDSP_DEMOD_CARRIER_0,       /* CC Subaset index    */
                       &hdrhitmsg_ftm_tca_msg,                      /* TCA msg ptr         */
                       HDRHAI_PHYSICAL_LAYER_PROTOCOL,/* Sender              */
                       HDRSCP_DEFAULT_RUP             /* RUP subtype         */    
                     );

    (void) hdrhitcmd_modulator_cfg(HDRMDSP_DEMOD_CARRIER_0, FALSE, FALSE, 
                                   (mod_rtc_reva_cmd->rpc_step + 1), TRUE);
    hdrsrch_at_set_drc(HDRMDSP_DEMOD_CARRIER_0, mod_rtc_reva_cmd->drc_value);

    // disable throttling
    hdrrmac_disable_throttling( HDRRMAC_FTM_ID, TRUE );

    total_pwr = rfm_hdr_get_tx_total_power(RFM_DEVICE_0,
                                          RFM_HDR_CARRIER_ID__0,
                                          RFM_CDMA_AGC_FORMAT__DBM256) /256;
  
    pilot_pwr = rfm_hdr_get_tx_pilot_power(RFM_DEVICE_0,
                                          RFM_HDR_CARRIER_ID__0,
                                          RFM_CDMA_AGC_FORMAT__DBM256) /256;

    HDR_MSG_HIT_1(MSG_LEGACY_HIGH, "Total TX Power is :%d dbm", total_pwr);
    HDR_MSG_HIT_1(MSG_LEGACY_HIGH, "Pilot Power is :%d dbm", pilot_pwr);

    hdrhitmsg_hdr_ftm_state.rev_curr_state  = HDRHITMSG_HDR_FTM_REV_TRAFFIC;
    hdrhitmsg_ftm_intf.mode_type_reva       = 1;

    hdrhit_msg_rsp.mod_rtc_reva.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_reva_rsp_msg_t ) );

    return;

} /* hdrhitmsg_process_mod_rtc_reva */
#endif /* FEATURE_HDR_RUMI_TX_ONLY */

#ifdef FEATURE_HDR_RUMI_TX_ONLY 
/*===========================================================================

FUNCTION  HDRHITMSG_PROCESS_MOD_RTC_REVB

DESCRIPTION
  This function satrts the reverse traffic channel for RevB

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_mod_rtc_revb
(
    hdrhitmsg_s_type *msg
)
{

    hdrhit_msg_rsp_u            hdrhit_msg_rsp;

    uint32                       hdr_rtc_cx8;
    uint32                       hstr;
    uint32                       rtc_offset;
/*-------------------------------------------------------------------------*/   

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "RUMI TX only: Got the HDR MOD RTC REVB COMMAND");

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_MOD_RTC_REVB );

    if (hdrhitmsg_hdr_ftm_state.rev_curr_state != HDRHITMSG_HDR_FTM_ENTER)
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "RUMI TX only: must in FTM_ENTER state");

        hdrhit_msg_rsp.mod_rtc_revb.cmd_status = HDRHIT_NO_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_revb_rsp_msg_t ) );

        return;
    }

    hdrts_init();
      /* Initialize timeline. The function is safe to call multiple times */

    hdrhitmsg_hdr_ftm_state.rev_curr_state      = HDRHITMSG_HDR_FTM_REV_TRAFFIC;
    hdrhitmsg_hdr_ftm_state.fwd_curr_state      = HDRHITMSG_HDR_FTM_IDLE;

    hdrhitmsg_ftm_intf.mode_type_revb = 1;

#ifdef FEATURE_MEMCPY_REMOVAL
    hdrhitmsg_msg_buf = *msg;
#else
    memcpy(&hdrhitmsg_msg_buf, msg, sizeof(hdrhitmsg_s_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

    hdrhit_msg_rsp.mod_rtc_revb.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_revb_rsp_msg_t ) );

    ( void ) rex_set_sigs( HDRTX_TASK_PTR, HDRTX_ENTER_HDR_SIG );
    hdrsrchmcpm_set_state( HDRSRCHMCPM_WAKE_UP_REQ );
      /* Tell MCPM to turn on clocks grouped under wake-up */
    hdrsrchmcpm_set_state( HDRSRCHMCPM_START_TX_REQ );
      /* Start the Tx clock, so that we can write to TX HWIO regs */

    hdrmdsp_fws_enable_app();
      /* Enable fws hdr app */

    hdrmdsp_config_hdr_app( HDRFW_STATE_ACTIVE, FALSE,FALSE );

    DALSYS_BusyWait(60000);  
      /* wait 30 ms for response */
#ifdef FEATURE_HDR_BOLT_MODEM
    hdr_rtc_cx8 = hdrts_get_rtc_cx8();
#else
    hdr_rtc_cx8 = ( HWIO_IN( HDR_RTC_CNT )) << 2;
#endif /* FEATURE_HDR_BOLT_MODEM */

      /* read the current hdr rtc */
    hdrhitmsg_cdma_sys_time =  (hdr_rtc_cx8+5000) >> 3;
      /* force rtc_offset to be 0 */
    hstr = hdrhitmsg_cdma_sys_time & 0x7ffff;

    hstr /= 1024;
      /* hstr = (cdma_sys_time%0x80000)/1024 */
    
    rtc_offset = hdr_rtc_cx8 + 0x40000 - 
                 ( ( hdrhitmsg_cdma_sys_time << 3 ) & 0x3ffff );
    /* HDR RTC is in chip ratex8. It wraps around at 32768 chip
       36768*8= 0x40000. */

    rtc_offset &= 0x3ffff;

#ifdef FEATURE_HDR_BOLT_MODEM
    hdr_rtc_cx8 = hdrts_get_rtc_cx8();
#else
    hdr_rtc_cx8 = ( HWIO_IN( HDR_RTC_CNT )) << 2;
#endif /* FEATURE_HDR_BOLT_MODEM */

    hdrsrchdrv_start_slam( TRUE, hstr, rtc_offset,
                           hdrhitmsg_cdma_sys_time,hdr_rtc_cx8 );
      /* Pass timing to firmware */

    HDR_MSG_SRCH_4( MSG_LEGACY_HIGH, 
         "RUMI TX only RTC RevA:  rtc_offset=0x%x hstr=0x%x rtc_cx8=0x%x cdma_sys_time=0x%x",
          rtc_offset, hstr, hdr_rtc_cx8, hdrhitmsg_cdma_sys_time );

    hdrhitmsg_tx_action = HDRHITMSG_TX_RTC_REVB;                
          
}/* hdrhitmsg_process_mod_rtc_revb */
#else

/*===========================================================================

FUNCTION  HDRHITMSG_PROCESS_MOD_RTC_REVB

DESCRIPTION
  This function satrts the reverse traffic channel for RevB

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_mod_rtc_revb
(
    hdrhitmsg_s_type *msg
)
{
    hdrhitmsg_access_params_t       *access_params_ptr  = & ( hdrhitmsg_ftm_intf.access_params );
    hdrhitmsg_rmac_params_t         *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_mod_rtc_revb_cmd_msg_t   *mod_rtc_revb_cmd;
    hdrhit_msg_rsp_u                hdrhit_msg_rsp;
    hdrsrch_sect_struct_type*       ref_sect_ptr;
    sys_link_type                   link_info;  
    sys_channel_type                band_chan_local[3];
    qword                           i_mask;
    qword                           q_mask;
    int16                           total_pwr[MAX_NUM_REV_PHY_CHANS];
    int16                           pilot_pwr[MAX_NUM_REV_PHY_CHANS];
    int8                            i;
    hdrfmac_state_enum_type         hdr_ftm_fmac_state;
    uint8                           rpc_enable;
    uint8                           single_fwd_link_mode, hicap_lolat;
    uint8                           rri_c0;
    uint8                           rri_c1;
    uint8                           rri_c2;
    uint8                           drclocklength_slots_revb = 1;
/*------------------------------------------------------------------------*/

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_MOD_RTC_REVB );
    mod_rtc_revb_cmd     = &(msg->msg.hdrhit_cmd_msg.mod_rtc_revb);
    ref_sect_ptr         = hdrsrchutil_get_ref_sector_ptr();

    single_fwd_link_mode = (mod_rtc_revb_cmd->single_fwd_link_mode == 0)? 0:1;
    hicap_lolat          = (mod_rtc_revb_cmd->hicap_lolatency == 0)? 0:1;
    rpc_enable           = (mod_rtc_revb_cmd->rpc_enable ==0)? 0:1;
    rri_c0               =  mod_rtc_revb_cmd->rri_c0;
    rri_c1               =  mod_rtc_revb_cmd->rri_c1;
    rri_c2               =  mod_rtc_revb_cmd->rri_c2; 

    /* Sanity check params */
    if (single_fwd_link_mode == 0)
    {
        HDR_MSG_HIT( MSG_LEGACY_HIGH, "Multiple fwd link mode not supported" );

        hdrhit_msg_rsp.mod_rtc_revb.cmd_status = HDRHIT_INVALID_STATE;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_revb_rsp_msg_t ) );
        
        return;
    }
    if ((mod_rtc_revb_cmd->num_carriers < 1)||
        (mod_rtc_revb_cmd->num_carriers >3))
    {
        HDR_MSG_HIT_1(MSG_LEGACY_HIGH, 
               "num_of_carriers = %d not supported", mod_rtc_revb_cmd->num_carriers);

        hdrhit_msg_rsp.mod_rtc_revb.cmd_status = HDRHIT_INVALID_STATE;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_revb_rsp_msg_t ) );
        
        return;
    }

    if (rri_c0 > 12)
        rri_c0 = 12;

    if (rri_c1 > 12)
        rri_c1 = 12;

    if (rri_c2 > 12)
        rri_c2 = 12;

    /* Check channels */
    /* Assign main channel */
    band_chan_local[0] = mod_rtc_revb_cmd->hdr_chan;
    /* Check 2nd channel */
    band_chan_local[1].band = mod_rtc_revb_cmd->hdr_chan.band;
    band_chan_local[1].chan_num = mod_rtc_revb_cmd->chan_c1;
    /* Check 3rd channel */
    band_chan_local[2].band = mod_rtc_revb_cmd->hdr_chan.band;
    band_chan_local[2].chan_num = mod_rtc_revb_cmd->chan_c2;

    HDR_MSG_HIT_4(MSG_LEGACY_HIGH,
      "hdrhitmsg_process_mod_rtc_revb: Chan1=%d Chan2=%d Chan3=%d lchan_num=%d",
      mod_rtc_revb_cmd->hdr_chan.chan_num,
      mod_rtc_revb_cmd->chan_c1,
      mod_rtc_revb_cmd->chan_c2,
      band_chan_local[0].chan_num);

    /* Mode states sanity check */
    if ( hdrhitmsg_ftm_intf.mode_type_rev0 != 0 )
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, 
           "Cannot change the parameters of reverse traffic channel for RevB in Rev0 mode.");

        hdrhit_msg_rsp.mod_rtc_revb.cmd_status = HDRHIT_INVALID_STATE;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_revb_rsp_msg_t ) );
        
        return;
    }

    if ( (hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_ENTER) || 
         (hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_PIL_ACQ) )
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH,
             "Invalid state. Cannot change the parameters of reverse traffic channel for RevB.");

        hdrhit_msg_rsp.mod_rtc_revb.cmd_status = HDRHIT_INVALID_STATE;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_revb_rsp_msg_t ) );
        
        return;
    }



    if ( hdrhitmsg_ftm_intf.ftm_not_doing_hho )
    {
       if ( hdrhitmsg_hdr_ftm_state.rev_curr_state == HDRHITMSG_HDR_FTM_REV_TRAFFIC )
       {
          HDR_MSG_HIT(MSG_LEGACY_HIGH, "Modulate RTC REVB: off");
          hdrrmac_deactivate(HDRHAI_PHYSICAL_LAYER_PROTOCOL);
          hdrlmac_idle_mode(HDRHAI_PHYSICAL_LAYER_PROTOCOL);
          hdrhitmsg_hdr_ftm_state.rev_curr_state = HDRHITMSG_HDR_FTM_SYS_ACC;
          hdramac_activate(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

          hdrhit_msg_rsp.mod_rtc_revb.cmd_status = HDRHIT_SUCCESS;
          hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_revb_rsp_msg_t ) );

          return;
       }
       else
       {
          if ( ( hdrhitmsg_hdr_ftm_state.fwd_curr_state == HDRHITMSG_HDR_FTM_FWD_TRAFFIC_ONLY ) &&
             ( hdrhitmsg_ftm_intf.first_time_rev_link_revb != 0 ) )
          {
             hdrhitmsg_ftm_intf.first_time_rev_link_revb = 0;
             hdrlmac_idle_mode(HDRHAI_PHYSICAL_LAYER_PROTOCOL);  
             DALSYS_BusyWait(500);

          }
       }

       if (hdrhitmsg_hdr_ftm_state.rev_curr_state != HDRHITMSG_HDR_FTM_SYS_ACC)
       {
          HDR_MSG_HIT(MSG_LEGACY_HIGH,
               "Cannot change the parameters of reverse traffic channel for RevB");
          HDR_MSG_HIT(MSG_LEGACY_HIGH, "Modulate Revserse Access Channel is not called.");

          hdrhit_msg_rsp.mod_rtc_revb.cmd_status = HDRHIT_INVALID_STATE;
          hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_revb_rsp_msg_t ) );

          return;
       }
    }

    /* Deactivate AC MAC */
    hdramac_deactivate(HDRHAI_PHYSICAL_LAYER_PROTOCOL, FALSE);

    /* Set forward link type to RevB */
    rmac_params_ptr->modem_fwd_rev_link_type = 2;

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "Modulate RTC command for RevB: on");

    if ( hdrsrch_get_link_info(ref_sect_ptr->link_id, &link_info ) != E_SUCCESS )
    {
        ERR("Unknown link ID: %d!",0, 0, 0 );
    }
    else
    {
        HDR_MSG_HIT_2(MSG_LEGACY_HIGH, " Chan:%d PN:%d", 
                      link_info.channel.chan_num, link_info.pilot_pn);
    }

   (void) hdrhitcmd_enable_drc_gating(FALSE);

    rmac_params_ptr->revb_single_fwd_link_mode = single_fwd_link_mode;
    rmac_params_ptr->revb_num_carriers      = mod_rtc_revb_cmd->num_carriers;
    rmac_params_ptr->hicap_lolatency        = hicap_lolat;

    rmac_params_ptr->tx_rri_revb_c[0]    =   rri_c0;
    rmac_params_ptr->tx_rri_revb_c[1]    =   rri_c1;
    rmac_params_ptr->tx_rri_revb_c[2]    =   rri_c2;

    rmac_params_ptr->gain_transition_point = rmac_params_ptr->revb_gain_transition_point;

    if (hicap_lolat == 0) /* 0 means hicap */
    {
        rmac_params_ptr->t2p_pre_transition_revb_c[0]  = rmac_params_ptr->t2p_hicap_revb[0][rri_c0];
        rmac_params_ptr->t2p_pre_transition_revb_c[1]  = rmac_params_ptr->t2p_hicap_revb[0][rri_c1];
        rmac_params_ptr->t2p_pre_transition_revb_c[2]  = rmac_params_ptr->t2p_hicap_revb[0][rri_c2];
        rmac_params_ptr->t2p_post_transition_revb_c[0] = rmac_params_ptr->t2p_hicap_revb[1][rri_c0];
        rmac_params_ptr->t2p_post_transition_revb_c[1] = rmac_params_ptr->t2p_hicap_revb[1][rri_c1];
        rmac_params_ptr->t2p_post_transition_revb_c[2] = rmac_params_ptr->t2p_hicap_revb[1][rri_c2];
    }
    else                  /* lolat */
    {
        rmac_params_ptr->t2p_pre_transition_revb_c[0]  = rmac_params_ptr->t2p_lolat_revb[0][rri_c0];
        rmac_params_ptr->t2p_pre_transition_revb_c[1]  = rmac_params_ptr->t2p_lolat_revb[0][rri_c1];
        rmac_params_ptr->t2p_pre_transition_revb_c[2]  = rmac_params_ptr->t2p_lolat_revb[0][rri_c2];
        rmac_params_ptr->t2p_post_transition_revb_c[0] = rmac_params_ptr->t2p_lolat_revb[1][rri_c0];
        rmac_params_ptr->t2p_post_transition_revb_c[1] = rmac_params_ptr->t2p_lolat_revb[1][rri_c1];
        rmac_params_ptr->t2p_post_transition_revb_c[2] = rmac_params_ptr->t2p_lolat_revb[1][rri_c2];
    }

    rmac_params_ptr->arq_mode                    = rmac_params_ptr->revb_arq_mode;
    rmac_params_ptr->auxpil_payload_thresh       = rmac_params_ptr->revb_auxpil_payload_thresh;
    rmac_params_ptr->auxpil_chan_gain            = mod_rtc_revb_cmd->auxpil_gain;
    rmac_params_ptr->rri_ch_gain_pre_transition  = rmac_params_ptr->rri_gain_revb[0][rmac_params_ptr->revb_gain_transition_point];
    rmac_params_ptr->rri_ch_gain_post_transition = rmac_params_ptr->rri_gain_revb[1][rmac_params_ptr->revb_gain_transition_point];
    rmac_params_ptr->dsc_length                  = rmac_params_ptr->revb_dsc_length;
    rmac_params_ptr->rtc_pattern                 = rmac_params_ptr->revb_rtc_pattern;


    for ( i=0; i< MAX_NUM_REV_PHY_CHANS; i++ )
    {
        /* RevA and RevB have common RL packet sizes.  
           Hence, hdrenc_rri_reva_sizes[] is used as the lookup table 
           for finding RL payload size. */
        rmac_params_ptr->tx_payload_revb_c[i] = 
            hdrenc_get_rri_reva_sizes( (int) rmac_params_ptr->tx_rri_revb_c[i] );
    }

    rmac_params_ptr->revb_closed_loop_c[0] = 
        ( (uint16) mod_rtc_revb_cmd->power_offset_c0 )*128; 
      /* unit from 1/4 dB to 1/512 dB*/
    rmac_params_ptr->revb_closed_loop_c[1] = 
        ( (uint16) mod_rtc_revb_cmd->power_offset_c1 )*128; 
      /* unit from 1/4 dB to 1/512 dB*/
    rmac_params_ptr->revb_closed_loop_c[2] = 
        ( (uint16) mod_rtc_revb_cmd->power_offset_c2 )*128; 
      /* unit from 1/4 dB to 1/512 dB*/
    rmac_params_ptr->rpc_enable            =  rpc_enable;
    rmac_params_ptr->rpc_step              =  mod_rtc_revb_cmd->rpc_step;

    qw_set( i_mask, rmac_params_ptr->revb_rtclc_mask_i_hi, rmac_params_ptr->revb_rtclc_mask_i_lo );
    qw_set( q_mask, rmac_params_ptr->revb_rtclc_mask_q_hi, rmac_params_ptr->revb_rtclc_mask_q_lo );

    rfm_hdr_set_nominal_pwr( RFM_DEVICE_0, access_params_ptr->open_loop_adjust );
    (void) rfm_hdr_set_tx_power_adjust( RFM_DEVICE_0, access_params_ptr->probe_initial_adjust );  

    for ( i=0; i < hdrhitmsg_ftm_intf.fwd_link_info_revb.drclocklength + 1; i++ )
    {
        drclocklength_slots_revb = drclocklength_slots_revb * 2;
    }

    (void) hdrhitcmd_fmac_activate_msg(HDRHITCMD_REV_B,
                                drclocklength_slots_revb*4,
                                (hdrhitmsg_ftm_intf.fwd_link_info_revb.drclockperiod+1)*8,
                                14);

    /* Start the Tx clock, so that we can write to TX HWIO regs */
    hdrsrchmcpm_set_state( HDRSRCHMCPM_START_TX_REQ );

    hdrrmac_activate( HDRHAI_PHYSICAL_LAYER_PROTOCOL );

    /* Program the User Long Code Mask into the modulator. */
    hdrmod_set_pn_long_code_mask( i_mask, q_mask 
#ifdef FEATURE_HDR_BOLT_MODEM
      , 7    /* Enable PN generator for all three carriers */
#endif /* FEATURE_HDR_BOLT_MODEM */
      );

    /* Make the mobile ACK, NAK, or not ACK every slot */
    hdr_ftm_fmac_state = hdrfmacsm_get_state();

/* Sanity check; plus make this code backward compatible */
    if ( rmac_params_ptr->revb_ack_mode > (byte) IS890A_MODE_NAK_OOK )
    {
        rmac_params_ptr->revb_ack_mode = (byte) IS890_MODE_1; 
         /* IS890_MODE_1 is the preferred mode */
    }

    if (hdr_ftm_fmac_state == HDRFMAC_INACTIVE_STATE)
    {
        hdrfmacsm_change_state( HDRFMAC_VAR_RATE_STATE );
    }

    hdrfmac_set_ack_mode (( hdrfmac_ack_mode_enum_type ) rmac_params_ptr->revb_ack_mode);

    if (hdr_ftm_fmac_state == HDRFMAC_INACTIVE_STATE)
    {
        hdrfmacsm_change_state( HDRFMAC_INACTIVE_STATE );
    }

    /* Construct TCA msg */
    memset(&hdrhitmsg_ftm_tca_msg, 0, sizeof(hdrrup_tca_msg_type));

    hdrhitmsg_ftm_tca_msg.msg_id = 0;
    hdrhitmsg_ftm_tca_msg.msg_seq = 0;
    hdrhitmsg_ftm_tca_msg.chan_incl = 0;
    hdrhitmsg_ftm_tca_msg.frame_offset = rmac_params_ptr->revb_frame_offset;
    hdrhitmsg_ftm_tca_msg.dsc_channel_gain = mod_rtc_revb_cmd->dsc_gain;
    hdrhitmsg_ftm_tca_msg.link_id = ref_sect_ptr->link_id;
    hdrhitmsg_ftm_tca_msg.num_sectors = 1;
    hdrhitmsg_ftm_tca_msg.num_sub_asets = rmac_params_ptr->revb_num_carriers;
    hdrhitmsg_ftm_tca_msg.scheduler_tag_incl = 0;
    hdrhitmsg_ftm_tca_msg.feedback_mux_enabled = 0;
    hdrhitmsg_ftm_tca_msg.sector_info[0].drc_cover = rmac_params_ptr->revb_drc_cover;
    hdrhitmsg_ftm_tca_msg.sector_info[0].pilot_pn = link_info.pilot_pn;
    hdrhitmsg_ftm_tca_msg.sector_info[0].rab_length = hdrhitmsg_ftm_intf.fwd_link_info_revb.rablength;
    hdrhitmsg_ftm_tca_msg.sector_info[0].rab_offset = hdrhitmsg_ftm_intf.fwd_link_info_revb.raboffset;
    hdrhitmsg_ftm_tca_msg.sector_info[0].softer_handoff = FALSE;
    hdrhitmsg_ftm_tca_msg.sector_info[0].ra_channel_gain = 0;
    hdrhitmsg_ftm_tca_msg.sector_info[0].dsc = rmac_params_ptr->revb_dsc_value;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].pilot_included = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.feedback_rev_ch_id = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.drc_length = rmac_params_ptr->revb_drc_length;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.drc_channel_gain = mod_rtc_revb_cmd->drc_gain;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.ack_channel_gain = mod_rtc_revb_cmd->ack_gain;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].fwd_channel[0].mode = SYS_HDR_CHAN_REC_MODE_IS856;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].fwd_channel[0].channel = mod_rtc_revb_cmd->hdr_chan;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].ftc_params[0].ftc_mac_index = 
        hdrhitmsg_ftm_intf.fwd_link_info_revb.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].aux_drc_cover = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].rev_mac_index = 
        hdrhitmsg_ftm_intf.fwd_link_info_revb.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].rab_mac_index = 
        hdrhitmsg_ftm_intf.fwd_link_info_revb.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].delta_t2p = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].pilot_group_id = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].scheduler_tag = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_rev_channels_incl = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_rev_channels = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_fwd_channels = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].channel = mod_rtc_revb_cmd->hdr_chan;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].ch_config = 1; 

    /* Replicate repeating parameters across subactive sets and 
       sectors */
    for (i = 1; i< MAX_NUM_REV_PHY_CHANS; i++)
    {
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrhitmsg_ftm_tca_msg.sub_aset[i] = 
                 hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0];
        hdrhitmsg_ftm_tca_msg.sector_info[i] = 
                 hdrhitmsg_ftm_tca_msg.sector_info[HDRMDSP_DEMOD_CARRIER_0];
#else
        memcpy(&(hdrhitmsg_ftm_tca_msg.sub_aset[i]), 
               &(hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0]), 
               sizeof(hdrrup_tca_sub_aset_type));
        memcpy(&(hdrhitmsg_ftm_tca_msg.sector_info[i]), 
               &(hdrhitmsg_ftm_tca_msg.sector_info[HDRMDSP_DEMOD_CARRIER_0]),
                sizeof(hdrrup_tca_sector_info_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    }

    /* Configure channel numbers for other carriers.  All carriers
       must be in the same band. */
    hdrhitmsg_ftm_tca_msg.sub_aset[1].fwd_channel[0].channel.chan_num = mod_rtc_revb_cmd->chan_c1;
    hdrhitmsg_ftm_tca_msg.sub_aset[1].rev_channel[0].channel.chan_num = mod_rtc_revb_cmd->chan_c1;

    hdrhitmsg_ftm_tca_msg.sub_aset[2].fwd_channel[0].channel.chan_num = mod_rtc_revb_cmd->chan_c2;
    hdrhitmsg_ftm_tca_msg.sub_aset[2].rev_channel[0].channel.chan_num = mod_rtc_revb_cmd->chan_c2;

    // disable throttling
    hdrrmac_disable_throttling( HDRRMAC_FTM_ID, TRUE );

    hdrlmac_tc_assign( TRUE,                          /* Initial assignment  */
                       ref_sect_ptr->link_id,         /* Link id             */
                       HDRSRCH_TUNE_AWAY_NONE,        /* Tuneaway disallowed */
                       HDRMDSP_DEMOD_CARRIER_0,       /* CC Subaset index    */
                       &hdrhitmsg_ftm_tca_msg,                      /* TCA msg ptr         */
                       HDRHAI_PHYSICAL_LAYER_PROTOCOL,/* Sender              */
                       HDRSCP_MC_RUP                  /* MultiCarrier RUP subtype */
                     );

    DALSYS_BusyWait(60000);  
     /* two frame-delay - 2 * 26.6 ms */

    memset(total_pwr, 0x0, sizeof(total_pwr));
    memset(pilot_pwr, 0x0, sizeof(pilot_pwr));

    /* Set per-carrier DRC value and compute pilot power */
    for ( i=0; i < rmac_params_ptr->revb_num_carriers; i++ )
    {

       (void) hdrhitcmd_modulator_cfg(i, TRUE, TRUE, 
                         (mod_rtc_revb_cmd->rpc_step + 1), TRUE);

        /* Set DRC value - same DRC is requested on all carriers */
        hdrsrch_at_set_drc( i, mod_rtc_revb_cmd->drc_value);


        /* Compute pilot power */  
        total_pwr[i] = rfm_hdr_get_tx_total_power(RFM_DEVICE_0,
                                            (rfm_cdma_carrier_type) i,
                                            RFM_CDMA_AGC_FORMAT__DBM256) /256;
    
        pilot_pwr[i] = rfm_hdr_get_tx_pilot_power(RFM_DEVICE_0,
                                             (rfm_cdma_carrier_type) i,
                                            RFM_CDMA_AGC_FORMAT__DBM256) /256;
   }

    HDR_MSG_HIT_3(MSG_LEGACY_HIGH, "Total TX Power is :c0=%d, c1=%d, c2=%d dBm",
                  total_pwr[0], total_pwr[1], total_pwr[2]);
    HDR_MSG_HIT_3(MSG_LEGACY_HIGH, "Carrier pilot powers: %d, %d, %d",
                   pilot_pwr[0], pilot_pwr[1], pilot_pwr[2]);

    hdrhitmsg_hdr_ftm_state.rev_curr_state = HDRHITMSG_HDR_FTM_REV_TRAFFIC;
    hdrhitmsg_ftm_intf.mode_type_revb      = 1;

    hdrhit_msg_rsp.mod_rtc_revb.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_rtc_revb_rsp_msg_t ) );

    return;
} /* hdrhitmsg_process_mod_rtc_revb */

#endif /* FEATURE_HDR_RUMI_TX_ONLY */

/*===========================================================================

FUNCTION  HDRHITMSG_PROCESS_CONFIG_RTC_REVB

DESCRIPTION
  This function satrts the reverse traffic channel for RevB

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_config_rtc_revb
(
    hdrhitmsg_s_type *msg
)
{
    hdrhitmsg_rmac_params_t             *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_config_rtc_revb_cmd_msg_t    *config_rtc_revb_cmd;
    hdrhit_msg_rsp_u                    hdrhit_msg_rsp;
    uint8                               arq_mode;
    uint8                               hdr_ack_mode;
    uint8                               auxpil_min_payload; 
    uint8                               rri;
    uint8                               gain_transition_point;
/*------------------------------------------------------------------------*/

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_CONFIG_RTC_REVB );
    config_rtc_revb_cmd = &(msg->msg.hdrhit_cmd_msg.config_rtc_revb);

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "Entering hdrhitmsg_process_config_rtc_revb");
    if (hdrhitmsg_hdr_ftm_state.rev_curr_state == HDRHITMSG_HDR_FTM_REV_TRAFFIC)
    {
        HDR_MSG_HIT(MSG_LEGACY_HIGH, "Cannot configure RTC Rev. B when current state is HDRHITMSG_HDR_FTM_REV_TRAFFIC");

        hdrhit_msg_rsp.config_rtc_revb.cmd_status = HDRHIT_INVALID_STATE;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_config_rtc_revb_rsp_msg_t ) );

        return;
    }

    arq_mode              = config_rtc_revb_cmd->arq_mode;
    hdr_ack_mode          = config_rtc_revb_cmd->hdr_ack_mode;
    auxpil_min_payload    = config_rtc_revb_cmd->auxpil_min_payload; 
    rri                   = config_rtc_revb_cmd->rri;
    gain_transition_point = config_rtc_revb_cmd->gain_transition_point;

    /* Sanity check */
    if (rri > 12)
    {   
        rri = 12;
    }
    if (auxpil_min_payload > 12)
    {   
        auxpil_min_payload = 12;
    }

    if (gain_transition_point > 3)
    {   
        gain_transition_point = 3;
    }


    /* Sanity check. Plus make this code backward compatible */
    if ( hdr_ack_mode > (byte) IS890A_MODE_NAK_OOK )
    {
        hdr_ack_mode = (byte) IS856_COMPLIANT;
    }

    //set values.
    rmac_params_ptr->revb_ack_mode               = hdr_ack_mode;
    rmac_params_ptr->revb_arq_mode               = arq_mode;
    rmac_params_ptr->revb_auxpil_payload_thresh  = auxpil_min_payload;
    rmac_params_ptr->revb_drc_cover              = config_rtc_revb_cmd->drc_cover;
    rmac_params_ptr->revb_drc_gating             = config_rtc_revb_cmd->drc_gating;
    rmac_params_ptr->revb_drc_length             = config_rtc_revb_cmd->drc_length;
    rmac_params_ptr->revb_dsc_value              = config_rtc_revb_cmd->dsc_value;
    rmac_params_ptr->revb_dsc_length             = config_rtc_revb_cmd->dsc_length;
    rmac_params_ptr->revb_frame_offset           = config_rtc_revb_cmd->frame_offset;
    rmac_params_ptr->revb_gain_transition_point  = gain_transition_point;
    rmac_params_ptr->revb_rtclc_mask_i_hi        = config_rtc_revb_cmd->rtclc_mask_i_hi;
    rmac_params_ptr->revb_rtclc_mask_i_lo        = config_rtc_revb_cmd->rtclc_mask_i_lo;
    rmac_params_ptr->revb_rtclc_mask_q_hi        = config_rtc_revb_cmd->rtclc_mask_q_hi;
    rmac_params_ptr->revb_rtclc_mask_q_lo        = config_rtc_revb_cmd->rtclc_mask_q_lo;
    rmac_params_ptr->revb_rtc_pattern            = config_rtc_revb_cmd->rtc_txpattern;

    if ( config_rtc_revb_cmd->hicap_lolatency == 0 ) /* 0 means hicap */
    {
        rmac_params_ptr->t2p_hicap_revb[0][rri] = config_rtc_revb_cmd->t2p_pre_transition;
        rmac_params_ptr->t2p_hicap_revb[1][rri] = config_rtc_revb_cmd->t2p_post_transition;
    }
    else /* lolat */
    {
        rmac_params_ptr->t2p_lolat_revb[0][rri] = config_rtc_revb_cmd->t2p_pre_transition;
        rmac_params_ptr->t2p_lolat_revb[1][rri] = config_rtc_revb_cmd->t2p_post_transition;
    }

    rmac_params_ptr->rri_gain_revb[0][gain_transition_point] = config_rtc_revb_cmd->rri_gain_pre_transition;
    rmac_params_ptr->rri_gain_revb[1][gain_transition_point] = config_rtc_revb_cmd->rri_gain_post_transition;

    hdrhit_msg_rsp.config_rtc_revb.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_config_rtc_revb_rsp_msg_t ) );

    return;

} /* hdrhitmsg_process_config_rtc_revb */

#ifndef FEATURE_HDR_RUMI
/*===========================================================================

FUNCTION  HDRHITMSG_HHO_TCA_CB

DESCRIPTION 
    Wrapper function which calls hdrmac_start_tx()

DEPENDENCIES
    None
 
RETURN VALUE
    None
 
SIDE EFFECTS
    None.

===========================================================================*/

static void hdrhitmsg_hho_tca_cb ( hdrsrch_tca_result_type dummy )
{
    hdrrmac_start_tx( HDRHAI_LINK_MANAGEMENT_AND_CONTROL_PROTOCOL );
} /* hdrhitmsg_hdrsrch_tca_cb */
#endif /* FEATURE_HDR_RUMI */

/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_HARD_HANDOFF

DESCRIPTION
  This function performs hard handoff.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrhitmsg_process_hard_handoff
(
    hdrhitmsg_s_type         *msg
)
{
    hdrhit_msg_rsp_u                    hdrhit_msg_rsp;

#ifdef FEATURE_HDR_RUMI
    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_HARD_HANDOFF );
    HDR_MSG_HIT(MSG_LEGACY_HIGH, "HDR_FTM:USE HDR HARD HANDOFF COMMAND for Sleep (RUMI bringup)");    

    hdrsrch_sleep( 
#ifdef FEATURE_HDR_RUMI
                   hdrhit_rumi_cfg.slot_cycle, /*  slot cycle */
#else
                   9, /*  slot cycle */
#endif /* FEATURE_HDR_RUMI */
                   9, /* negotiated_sci */
                   0, /* qpch_enabled */
                   NULL, /* wakeup_cb */
                   0, /* intra_scc_valid */
                   0 /* cc_start*/
#ifdef FEATURE_HDR_TO_LTE
                   ,
                   0 /*other_rat_tx_cycle */
#endif /* FEATURE_HDR_TO_LTE*/
#ifdef FEATURE_HDR_PAGE_BLOCKING_MGMT
                   ,0 /* session_is_open */
                   ,0 /* ovhd_is_updated */
#endif /* FEATURE_HDR_PAGE_BLOCKING_MGMT */
                 );
    hdrhit_msg_rsp.hard_handoff.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_hard_handoff_rsp_msg_t ) );

#else /* FEATURE_HDR_RUMI */
    hdrhit_hard_handoff_cmd_msg_t      *hard_handoff_cmd;
    sys_link_type                       link_info;
    hdrsrch_subactive_set_type          subaset;
    hdrsrch_sect_struct_type*           ref_sect_ptr;

    hdrsrch_tca_cb_type hdrsrch_tca_cb = hdrhitmsg_hho_tca_cb; 
    ref_sect_ptr    = hdrsrchutil_get_ref_sector_ptr();

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_HARD_HANDOFF );
    HDR_MSG_HIT(MSG_LEGACY_HIGH, "HDR_FTM:Got the HDR HARD HANDOFF COMMAND");

    hard_handoff_cmd = &(msg->msg.hdrhit_cmd_msg.hard_handoff);

    if (hdrhitmsg_hdr_ftm_state.rev_curr_state == HDRHITMSG_HDR_FTM_REV_TRAFFIC)
    {
        hdrhitmsg_ftm_intf.ftm_not_doing_hho = FALSE;

        /************** Rev 0 code ***********************************************/
        if( hdrhitmsg_ftm_intf.mode_type_rev0 )
        {
           subaset.channel = hard_handoff_cmd->target_hdr_chan;
           subaset.systype = SYS_HDR_CHAN_REC_MODE_IS856;
           subaset.not_reportable = FALSE;
           subaset.num_aset_pilots = 1;
           subaset.demod_idx = HDRMDSP_DEMOD_CARRIER_0;

           HDR_MSG_HIT(MSG_LEGACY_HIGH, "Starting HDR FTM HHO for Rev0");

           if ( hdrsrch_get_link_info( ref_sect_ptr->link_id, &link_info ) != E_SUCCESS )
           {
               HDR_MSG_HIT (MSG_LEGACY_HIGH, "Unknown link ID: 0");
           }
           else
           {
               HDR_MSG_HIT_2(MSG_LEGACY_HIGH, "Chan:%d PN:%d", link_info.channel.chan_num,
                         link_info.pilot_pn);
              subaset.aset_list[0].pilot_pn = link_info.pilot_pn;
           }

           subaset.aset_list[0].softer_handoff = 0;
           subaset.aset_list[0].mac_index = hdrhitmsg_ftm_intf.fwd_link_info_rev0.mac_index;
           subaset.aset_list[0].drc_cover = hdrhitmsg_ftm_intf.fwd_link_info_rev0.drc_cover; 
           subaset.aset_list[0].rab_length = hdrhitmsg_ftm_intf.fwd_link_info_rev0.rablength;
           subaset.aset_list[0].rab_offset = hdrhitmsg_ftm_intf.fwd_link_info_rev0.raboffset;

           hdrrmac_stop_tx( HDRHAI_LINK_MANAGEMENT_AND_CONTROL_PROTOCOL );

           DALSYS_BusyWait(5000); /* wait for 100 us ) */

           hdrsrch_chan_and_aset_assign_revb( 
             SYS_ACTIVE_PROT_HDR_REL0,           /* RUP Subtype                  */
             HDRSRCH_TUNE_AWAY_NONE,              /* 1x Tuneaway disallowed       */
             1,                                   /* Number of subactive sets     */
             &subaset,                            /* Subactive set list           */
             HDRMDSP_DEMOD_CARRIER_0,             /* Subactive set carrying CC    */
             0,                                     
             hdrsrch_tca_cb,                      /* TC assigned call back        */
             NULL,                                /* Add/Delete scheduler grps cb */
             NULL
             );
        }
        /************** Rev A code ***********************************************/
        if ( hdrhitmsg_ftm_intf.mode_type_reva)
        {
            subaset.channel = hard_handoff_cmd->target_hdr_chan;
            subaset.systype = SYS_HDR_CHAN_REC_MODE_IS856;
            subaset.not_reportable = FALSE;
            subaset.num_aset_pilots = 1;
            subaset.demod_idx = HDRMDSP_DEMOD_CARRIER_0;

            HDR_MSG_HIT(MSG_LEGACY_HIGH, "Starting HDR FTM HHO for Reva");

           if ( hdrsrch_get_link_info( ref_sect_ptr->link_id, &link_info ) != E_SUCCESS )
           {
               HDR_MSG_HIT (MSG_LEGACY_HIGH, "Unknown link ID: 0");
           }
           else
           {
              HDR_MSG_HIT_2(MSG_LEGACY_HIGH, "Chan:%d PN:%d", link_info.channel.chan_num,
               link_info.pilot_pn);

              subaset.aset_list[0].pilot_pn = link_info.pilot_pn;
           }
          
           subaset.aset_list[0].softer_handoff = 0;
           subaset.aset_list[0].mac_index = hdrhitmsg_ftm_intf.fwd_link_info_reva.mac_index;
           subaset.aset_list[0].drc_cover = hdrhitmsg_ftm_intf.fwd_link_info_reva.drc_cover;
           subaset.aset_list[0].rab_length = hdrhitmsg_ftm_intf.fwd_link_info_reva.rablength;
           subaset.aset_list[0].rab_offset = hdrhitmsg_ftm_intf.fwd_link_info_reva.raboffset;
           subaset.aset_list[0].dsc = hdrhitmsg_ftm_intf.fwd_link_info_reva.dsc_value;
           subaset.aset_list[0].ra_channel_gain = 0;

           hdrrmac_stop_tx( HDRHAI_LINK_MANAGEMENT_AND_CONTROL_PROTOCOL );

           DALSYS_BusyWait(5000); /* wait for 100 us ) */

           hdrsrch_chan_and_aset_assign_revb( 
             SYS_ACTIVE_PROT_HDR_REL0,           /* RUP Subtype                  */
             HDRSRCH_TUNE_AWAY_NONE,              /* 1x Tuneaway disallowed       */
             1,                                   /* Number of subactive sets     */
             &subaset,                            /* Subactive set list           */
             HDRMDSP_DEMOD_CARRIER_0,             /* Subactive set carrying CC    */
             0,   
             hdrsrch_tca_cb,                      /* TC assigned call back        */
             NULL,                                /* Add/Delete scheduler grps cb */
             NULL
             );
        }
              
        hdrhitmsg_ftm_intf.ftm_not_doing_hho = TRUE;
        hdrhit_msg_rsp.hard_handoff.cmd_status = HDRHIT_SUCCESS;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_hard_handoff_rsp_msg_t ) );
       
    }
    else
    {
        HDR_MSG_HIT_1(MSG_LEGACY_ERROR, "Invalid HDR traffic state %d, must be in HDRHITMSG_HDR_FTM_REV_TRAFFIC",
                      hdrhitmsg_hdr_ftm_state.rev_curr_state);
        hdrhit_msg_rsp.hard_handoff.cmd_status = HDRHIT_INVALID_STATE;
        hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_hard_handoff_rsp_msg_t ) );
    }
#endif /* FEATURE_HDR_RUMI */

} /* hdrhitmsg_process_hard_handoff */


/*===========================================================================

FUNCTION  HDRHITMSG_GET_FWD_CURR_STATE

DESCRIPTION
  This function gives the value of the current HDR FTM state on the forward link.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

hdrhitmsg_hdr_ftm_state_enum_type hdrhitmsg_get_fwd_curr_state ( void )
{
    return hdrhitmsg_hdr_ftm_state.fwd_curr_state;
} /* hdrhitmsg_get_fwd_curr_state */


/*===========================================================================

FUNCTION  HDRHITMSG_SET_FWD_CURR_STATE

DESCRIPTION
  This function sets the value of the current HDR FTM state on the forward link.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_set_fwd_curr_state
(
    hdrhitmsg_hdr_ftm_state_enum_type fwd_curr_state
)
{
    hdrhitmsg_hdr_ftm_state.fwd_curr_state = fwd_curr_state;
} /* hdrhitmsg_get_fwd_curr_state */

/*===========================================================================

FUNCTION  HDRHITMSG_GET_RMAC_PARAMS_PTR

DESCRIPTION 
    Returns pointer rmac_params field of hdrhitmsg_ftm_intf. 

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

hdrhitmsg_rmac_params_t * hdrhitmsg_get_rmac_params_ptr ( void )
{
    return &( hdrhitmsg_ftm_intf.rmac_params );
} /* hdrhitmsg_get_rmac_params_ptr */


/*===========================================================================

FUNCTION  HDRHITMSG_GET_FTM_INTF_PTR

DESCRIPTION 
    Returns pointer to hdrhitmsg_ftm_intf global variable. 

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

hdrhitmsg_ftm_intf_t * hdrhitmsg_get_ftm_intf_ptr ( void )
{
    return &hdrhitmsg_ftm_intf;
} /* hdrhitmsg_get_ftm_intf_ptr */


/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_GET_FWD_LNK_STATS

DESCRIPTION
  This function performs gets fwd lnk stats

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 

=========================================================================*/

void hdrhitmsg_process_get_fwd_lnk_stats
(
    hdrhitmsg_s_type         *msg
)
{
    uint32                               total_tc_packets = 0;
    uint32                               total_tc_errors  = 0;

    hdrhit_msg_rsp_u                     hdrhit_msg_rsp;

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "HDR_FTM:Got the HDR GET FWD LNK STATS COMMAND");

    hdrrx_get_connected_state_per_data( &total_tc_packets, &total_tc_errors);

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_GET_FWD_LNK_STATS );
    hdrhit_msg_rsp.get_fwd_lnk_stats.total_tc_errors = total_tc_errors;
    hdrhit_msg_rsp.get_fwd_lnk_stats.total_tc_packets = total_tc_packets;
    hdrhit_msg_rsp.get_fwd_lnk_stats.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_get_fwd_lnk_stats_rsp_msg_t ));

} /* hdrhitmsg_process_get_fwd_lnk_stats */

/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_RESET_FWD_LNK_STATS

DESCRIPTION
  This function performs resets fwd lnk stats

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrhitmsg_process_reset_fwd_lnk_stats
(
    hdrhitmsg_s_type         *msg
)
{
    hdrhit_msg_rsp_u                    hdrhit_msg_rsp;

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "HDR_FTM:Got the HDR RESET FWD LNK STATS COMMAND");

    hdrrx_reset_connected_state_per ();

    hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_RESET_FWD_LNK_STATS );
    hdrhit_msg_rsp.reset_fwd_lnk_stats.cmd_status = HDRHIT_SUCCESS;
    hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_reset_fwd_lnk_stats_rsp_msg_t ) );

} /* hdrhitmsg_process_reset_fwd_lnk_stats */

/*===========================================================================

FUNCTION HDRHITMSG_GET_FWD_LNK_STATS

DESCRIPTION
  Provides the forward link stats.

DEPENDENCIES

RETURN VALUE
   None
 
SIDE EFFECTS
  None.

===========================================================================*/ 
void hdrhitmsg_get_fwd_lnk_stats
( 
   uint32 * total_tc_packets,
   uint32 * total_tc_errors 
)
{
   hdrrx_get_connected_state_per_data(total_tc_packets,total_tc_errors);

}/* hdrhitmsg_get_fwd_lnk_stats */

/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_MODULATOR_CTL

DESCRIPTION
  This function enables/disable tx on carriers as specified by the input.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrhitmsg_process_modulator_ctl
(
    hdrhitmsg_s_type         *msg
)
{
  hdrhit_modulator_ctl_cmd_msg_t          *modulator_ctl_cmd;
  hdrhit_msg_rsp_u                        hdrhit_msg_rsp;
  uint8                                   carrier_mask, c;
  static boolean                          tx_enabled = FALSE;
  qword                                   i_mask,q_mask;
  uint32                                  i_shift, i_mask_32, xor_result; 
  hdrfw_msg_u                             fw_cmd_enable_carrier; /* Firmware commands */
  hdrfw_msg_u                             fw_cmd_rmac_modulator_cfg; /* Firmware command */
  errno_enum_type                         estatus;  /* Error status */
  int                                     i;
  const byte hdrhitcmd_q_xor_array[] =
  {
    0, 1, 2, 4, 5, 6, 9, 15, 16, 17, 18, 20, 21, 24, 25, 26,
    30, 32, 34, 41
  };

  /* Initialize firmware message */
  memset ( &fw_cmd_enable_carrier, 0, sizeof( hdrfw_msg_u ) );
  msgr_init_hdr ( &fw_cmd_enable_carrier.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_ENABLE_CARRIER_CMD );


  modulator_ctl_cmd = &( msg->msg.hdrhit_cmd_msg.modulator_ctl );
  carrier_mask = modulator_ctl_cmd->carrier_mask;

  HDR_MSG_HIT(MSG_LEGACY_HIGH, "HDR_FTM:Got the HDR ENABLE MODULATOR COMMAND");

  if( ( carrier_mask != 0 ) && !tx_enabled )
  {

    ( void ) rex_set_sigs( HDRTX_TASK_PTR, HDRTX_ENTER_HDR_SIG );

    if ( !(modulator_ctl_cmd->skip_dac_toggle) )
    { 
      /* Reset the modulator when the pilot channel is turned on. */
      hdrmod_reset();

      /* Set traffic mode to RevA/B */
      hdrmod_set_traffic_mode( HDRMOD_TRAFFIC_MODE_REVB );

      /* Initialize firmware modulator configuration message */
      memset ( &fw_cmd_rmac_modulator_cfg, 0, sizeof(hdrfw_msg_u) );
      msgr_init_hdr ( &fw_cmd_rmac_modulator_cfg.hdr, MSGR_HDR_RMAC, HDR_FW_RMAC_MODULATOR_CFG_CMD );
      fw_cmd_rmac_modulator_cfg.rmac_modulator_cfg.txGainAdjStep = (1 * 128); /* Default gain step */
      fw_cmd_rmac_modulator_cfg.rmac_modulator_cfg.hdrModulatorModeConfig = 2; /* 2=RevB */
      /* Program transmit pilot filter time constant */
      fw_cmd_rmac_modulator_cfg.rmac_modulator_cfg.txPilotPowerFiltCoeff = 0x6D; /* Default filtCoeff */
      /* Disable forced termination test mode */
      fw_cmd_rmac_modulator_cfg.rmac_modulator_cfg.rlForceEarlyTerminate.force = 0;
      /* Send modulator config command to firmware */
      estatus = msgr_send ( &fw_cmd_rmac_modulator_cfg.hdr, sizeof(hdrfw_msg_u) );
      ASSERT ( estatus == E_SUCCESS );

      /* Generate and set I/Q mask */

      qw_set( i_mask, 0, 0 );

      /* Construct Q mask ... First figure out what bit 0 should be */
      xor_result = 0;

      for( i=0; i<ARR_SIZE( hdrhitcmd_q_xor_array ); i++ )
      {
          i_shift = hdrhitcmd_q_xor_array[i];

          if( i_shift > 31 )
          {
              i_mask_32 = qw_hi( i_mask );
              i_shift-=32;
          }
          else
          {
              i_mask_32 = qw_lo( i_mask );
          }

          xor_result ^= ( ( i_mask_32 >> i_shift ) & 0x00000001 );
      }

      /* Set Q to I, shifted by one bit to the left. */
      qw_set( q_mask, qw_hi( i_mask ), qw_lo( i_mask ) );
      qw_shift( q_mask, 1 );

      /* Add in last bit. */
      qw_set( q_mask, qw_hi( q_mask ) & 0x3ff, 
        ( qw_lo( q_mask ) | xor_result ) );

      HDR_MSG_HIT_2( MSG_LEGACY_MED, 
        "I_MASK_U = %x, I_MASK_L = %x", i_mask[1], i_mask[0] );
      HDR_MSG_HIT_2( MSG_LEGACY_MED, 
        "Q_MASK_U = %x, Q_MASK_L = %x", q_mask[1], q_mask[0] );

      /* Program the user long code mask into the modulator */
      hdrmod_set_pn_long_code_mask( i_mask, q_mask
#ifdef FEATURE_HDR_BOLT_MODEM
        , 7         /* Enable PN generator for all carriers */
#endif /* FEATURE_HDR_BOLT_MODEM */
        );

      /* Trigger DAC */
      hdrmod_tx_start( );

      /* Wait for about a frame for DAC to get triggered.  
         One frame is 26.66 ms */
      DALSYS_BusyWait( 27000 );
    }

    /* Set tx enabled flag */
    tx_enabled = TRUE;

  } /* If tx needs to be enabled */


  /* Program carriers in modulator and FW */
  for( c=0; c<MAX_NUM_REV_PHY_CHANS; c++)
  {
    fw_cmd_enable_carrier.rmac_enable_carrier.carrierMask |= ( 1 << c );
    fw_cmd_enable_carrier.rmac_enable_carrier.modulatorConfig[c].
      carrierMask = ( 1 << c );

    if( carrier_mask & ( 1<<c ) )
    {
      /* Enable carrier c */
      hdrmod_config_carrier( c, TRUE, 1<<c );
      fw_cmd_enable_carrier.rmac_enable_carrier.modulatorConfig[c].
        enable = 1;
    }
    else
    {
      /* Disable carrier c */
      hdrmod_config_carrier( c, FALSE, 1<<c );
      fw_cmd_enable_carrier.rmac_enable_carrier.modulatorConfig[c].
        enable = 0;
    }
  }

  estatus = msgr_send ( &fw_cmd_enable_carrier.hdr, sizeof( hdrfw_msg_u ) );
  ASSERT ( estatus == E_SUCCESS );

  /* Turn on/off PA */
  if( carrier_mask != 0 )
  {
    /* Turn on PA along with carriers specified by carrier mask */
    hdrmod_setup_tx_timed_enable( TRUE, 0 );
  }
  else
  {
    /* Turn off PA and all carriers */
    hdrmod_setup_tx_timed_disable( TRUE, 0 );
  }

  /* If all carriers are being disabled, stop DAC and put 
     modulator to Rel0 mode */
  if( carrier_mask == 0 )
  {
    if ( !(modulator_ctl_cmd->skip_dac_toggle) )
    { 
      hdrmod_tx_stop( );
      /* Wait for ~slot for DAC to be stopped */
      DALSYS_BusyWait( 4000 );

      hdrmod_set_traffic_mode( 0 );    
    }
   
    /* Reset tx enabled flag */
    tx_enabled = FALSE; 
  }

  /* Send command processed response */
  hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_MODULATOR_CTL );
  hdrhit_msg_rsp.modulator_ctl.cmd_status = HDRHIT_SUCCESS;
  hdrhitmsg_send_msg( &hdrhit_msg_rsp, 
    sizeof( hdrhit_modulator_ctl_rsp_msg_t ) );

} /* hdrhitmsg_process_modulator_ctl */


/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_CONFIG_FUNNEL_MODE

DESCRIPTION
  This function configures reverse link funnel mode as specified by 
  the input.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrhitmsg_process_config_funnel_mode
(
    hdrhitmsg_s_type         *msg
)
{
  hdrhit_config_funnel_mode_cmd_msg_t     *config_funnel_mode_cmd;
  hdrhit_msg_rsp_u                        hdrhit_msg_rsp;

  config_funnel_mode_cmd = &( msg->msg.hdrhit_cmd_msg.config_funnel_mode );

  /* Configure funnel mode */
  hdrrmac_config_funnel_mode( 
    config_funnel_mode_cmd->enable, 
    config_funnel_mode_cmd->data_active_carrier_idx );

  /* Send command processed response */
  hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_CONFIG_FUNNEL_MODE );
  hdrhit_msg_rsp.config_funnel_mode.cmd_status = HDRHIT_SUCCESS;
  hdrhitmsg_send_msg( &hdrhit_msg_rsp, 
    sizeof( hdrhit_config_funnel_mode_rsp_msg_t ) );

} /* hdrhitmsg_process_config_funnel_mode */

#ifdef FEATURE_HDR_RUMI_TX_ONLY
/*===========================================================================

FUNCTION HDRHITMSG_START_ACC

DESCRIPTION 
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  There will be a time discontinuity.
  The ISR auto uninstalls itself.

===========================================================================*/

void hdrhitmsg_start_acc( void )
{

  hdrhit_msg_rsp_u            hdrhit_msg_rsp;


  uint16 raw_hstr;
/*---------------------------------------------------------------------------------------------*/

  raw_hstr = HDRHSTR_GET_COUNT();

  HDR_MSG_HIT_1( MSG_LEGACY_HIGH, "RUMI TX only,time_valid, update time service time, curr_raw_hstr=%d",
                 raw_hstr );


  hdrts_update_ts_time();
    /* Synchronize Time of Day to HDR System Time */
  HDR_MSG_HIT( MSG_LEGACY_HIGH, "RUMI TX only,synchronized time of day "); 

  DALSYS_BusyWait(20000); 
  HDR_MSG_HIT( MSG_LEGACY_HIGH, "RUMI TX only,busy wait for 20ms"); 

  HDR_MSG_HIT( MSG_LEGACY_HIGH, "RUMI TX only,Activate amac"); 

  /* Set silence duration to zero */
  hdramac_set_silence_params(0,1);

  /* Program the User Long Code Mask into the modulator. */
  hdrmod_set_pn_long_code_mask( hdrhitmsg_tx_only.i_mask, hdrhitmsg_tx_only.q_mask
#ifdef FEATURE_HDR_BOLT_MODEM
    , 1                 /* Enable carrier 0 PN generator */
#endif /* FEATURE_HDR_BOLT_MODEM */
    );

  hdramac_start_attempt(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

  DALSYS_BusyWait(27000);  
    /* one frame-delay - 26.6 ms */
  hdrhitmsg_ftm_intf.mode_type_rev0 = 1;

  HDR_MSG_HIT( MSG_LEGACY_HIGH, "RUMI TX only,sending hit_mode_acc_rsp"); 

  hdrhitmsg_init_msg( &hdrhit_msg_rsp, HDR_HIT_RSP_MOD_ACC );
  hdrhit_msg_rsp.mod_acc.cmd_status = HDRHIT_SUCCESS;
  hdrhitmsg_send_msg( &hdrhit_msg_rsp, sizeof( hdrhit_mod_acc_rsp_msg_t ) );

  HDR_MSG_HIT( MSG_LEGACY_HIGH, "RUMI TX only,sent hit_mode_acc_rsp"); 
}/* hdrhitmsg_start_acc */

/*===========================================================================

FUNCTION HDRHITMSG_START_ACC_REVA

DESCRIPTION 
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  There will be a time discontinuity.
  The ISR auto uninstalls itself.

===========================================================================*/

void hdrhitmsg_start_acc_reva( void )
{
  uint16 raw_hstr;

/*---------------------------------------------------------------------------------------------*/

  raw_hstr = HDRHSTR_GET_COUNT();

  HDR_MSG_HIT_1( MSG_LEGACY_HIGH, "RUMI TX only,time_valid, update time service time, curr_raw_hstr=%d",
                 raw_hstr );


  hdrts_update_ts_time();
    /* Synchronize Time of Day to HDR System Time */

  HDR_MSG_HIT( MSG_LEGACY_HIGH, "RUMI TX only,synchronized time of day "); 

  DALSYS_BusyWait(20000); 
  HDR_MSG_HIT( MSG_LEGACY_HIGH, "RUMI TX only,busy wait for 20ms"); 

  HDR_MSG_HIT( MSG_LEGACY_HIGH, "RUMI TX only,Activate amac"); 

  /* Set silence duration to zero */
  hdramac_set_silence_params(0,1);

  /* Program the User Long Code Mask into the modulator. */
  hdrmod_set_pn_long_code_mask( hdrhitmsg_tx_only.i_mask, hdrhitmsg_tx_only.q_mask
#ifdef FEATURE_HDR_BOLT_MODEM
    , 1                 /* Enable carrier 0 PN generator */
#endif /* FEATURE_HDR_BOLT_MODEM */
    );

  hdramac_start_attempt(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

  /* Prepare modulator to start tx */
  //hdrmod_tx_start();
  DALSYS_BusyWait(27000);  
   /* one frame-delay - 26.6 ms */

  hdrhitmsg_ftm_intf.mode_type_reva = 1;

}/* hdrhitmsg_start_acc_reva */

/*===========================================================================

FUNCTION HDRHITMSG_FRAME_ISR

DESCRIPTION
  This Interrupt Service Routine completes the time synchronization.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  There will be a time discontinuity.
  The ISR auto uninstalls itself.

===========================================================================*/

void hdrhitmsg_frame_isr( void )
{
  uint64 slam_frame;
  uint16 raw_hstr;

  raw_hstr = HDRHSTR_GET_COUNT();
    /* Get raw HSTR value: 0 .. 0xffff half slots */
  slam_frame = hdrhitmsg_cdma_sys_time >> 15;
    /* frame number when we send slam message */


  hdrts_deregister_roll_callback( hdrhitmsg_frame_isr );

  HDR_MSG_HIT_3( MSG_LEGACY_HIGH, "RUMI TX only: hdrhitmsg_frame_isr(), slam_frame=0x%x,curr raw_hstr=0x%x, hstr=0x%x ",
                slam_frame, raw_hstr, raw_hstr&0x1FF );
  hdrts_set( slam_frame + 1, hdrhitmsg_time_valid );
    /* Set HDR time (# of 26.7ms periods from UTC=0), and call
       hdrhitmsg_time_value when time has been successfully set.
       Usually the current time is one frame after the time when slam message is sent. That's why
       we pass slam_frame+1. We skip all the calculations in FTM mode.
    */
}

/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_SLAM_RSP

DESCRIPTION
  This function process slam response.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrhitmsg_process_slam_rsp( uint16 hstr_at_slam )
{

  uint16 raw_hstr;

  raw_hstr = HDRHSTR_GET_COUNT();

  HDR_MSG_HIT_2( MSG_LEGACY_HIGH, "RUMI TX Only: received slam response hstr_at_slam %d, curr_raw_hstr=%d",
                hstr_at_slam, raw_hstr
               );
  hdrts_abort_sleep();
    /* Re-enable time stamp service's demod roll isr */
   HDR_MSG_HIT( MSG_LEGACY_HIGH, "RUMI TX only,register hdrhitmsg_fram_isr"); 
  hdrts_register_roll_callback( hdrhitmsg_frame_isr );
    /* Install PN roll handler*/
}


/*=========================================================================

FUNCTION HDRHITMSG_GET_DATA_PKT

DESCRIPTION
  This function returns the pointer to data packet

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
uint8 * hdrhitmsg_get_data_pkt( void )
{
  return &hdrhitmsg_data_pkt[0];
}

#ifdef FEATURE_HDR_RUMI
/*=========================================================================

FUNCTION HDRHITMSG_GET_DATA_PKT_AT_POS

DESCRIPTION
  This function returns the data at given position

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
uint8 hdrhitmsg_get_data_pkt_at_pos
( 
  uint16 pos
)
{
  uint8 data = hdrhitmsg_data_pkt[pos % sizeof(hdrhitmsg_data_pkt)];

  if ( hdrhit_rumi_cfg.verbose_debug )
  {
      HDR_MSG_PROT_2( MSG_LEGACY_MED, "Hit Get Data: %x@%x", 
                      data, pos );
  }

  return data;
}
#endif /* FEATURE_HDR_RUMI */

/*===========================================================================

FUNCTION HDRHITMSG_START_RTC

DESCRIPTION
  This function starts the reverse line traffic channel for Rev0.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_start_rtc
(
    void
)
{
    hdrhitmsg_access_params_t   *access_params_ptr  = & ( hdrhitmsg_ftm_intf.access_params );
    hdrhitmsg_rmac_params_t     *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_mod_rtc_cmd_msg_t    *mod_rtc_cmd;
//    hdrsrch_sect_struct_type*   ref_sect_ptr;
    sys_link_type               link_info;
    qword                       i_mask;
    qword                       q_mask;
    int16                       total_pwr;
    int16                       pilot_pwr;
    uint8                       *ptr_byte;
 // uint8                       drclocklength_slots_rev0 = 1;
    uint8                       hdr_ack_mode;
    hdrfmac_state_enum_type     hdr_ftm_fmac_state;
    uint8                       i;
/*-------------------------------------------------------------------------*/   

    mod_rtc_cmd     = &(hdrhitmsg_msg_buf.msg.hdrhit_cmd_msg.mod_rtc);
//    ref_sect_ptr    = hdrsrchutil_get_ref_sector_ptr();

    hdrhitmsg_ftm_intf.fwd_link_info_rev0.drc_cover = mod_rtc_cmd->drc_cover;

    hdrhitmsg_ftm_intf.fwd_link_info_rev0.drc_cover = mod_rtc_cmd->drc_cover;

    rmac_params_ptr->modem_fwd_rev_link_type = 0;


    /* hard code band/channel/PN */
    link_info.channel.band = 0;
    link_info.channel.chan_num = 500;
    link_info.pilot_pn = 0;
    HDR_MSG_HIT_2(MSG_LEGACY_HIGH, "RUMI TX only: Chan:%d PN:%d", link_info.channel.chan_num, link_info.pilot_pn);

    (void) hdrhitcmd_enable_drc_gating(mod_rtc_cmd->drc_gating);

    hdrmod_set_traffic_mode(HDRMOD_TRAFFIC_MODE_REV0);
      
    rmac_params_ptr->power_params[0]   =   mod_rtc_cmd->dataoffset_nom;
    rmac_params_ptr->power_params[1]   =   mod_rtc_cmd->dataoffset_9k6;
    rmac_params_ptr->power_params[2]   =   mod_rtc_cmd->dataoffset_19k2;
    rmac_params_ptr->power_params[3]   =   mod_rtc_cmd->dataoffset_38k4;
    rmac_params_ptr->power_params[4]   =   mod_rtc_cmd->dataoffset_76k8;
    rmac_params_ptr->power_params[5]   =   mod_rtc_cmd->dataoffset_153k6;

    ptr_byte = hdrrmac0_get_hdrrmac_pkt();
    for (i=0;i<30;i++)
    {
        *ptr_byte  = hdrhitmsg_data_pkt[i];
        ptr_byte++;
    }

    rmac_params_ptr->tx_duty_cycle  =  mod_rtc_cmd->rtc_txduty_cycle;
    rmac_params_ptr->tx_rri         =  mod_rtc_cmd->rri;
    rmac_params_ptr->tx_payload     =  hdrenc_get_rri_sizes( (int) mod_rtc_cmd->rri )+1;
    rmac_params_ptr->rpc_enable     =  mod_rtc_cmd->rpc_enable;

    qw_set(i_mask, mod_rtc_cmd->rtclc_mask_i_hi, mod_rtc_cmd->rtclc_mask_i_lo);
    qw_set(q_mask, mod_rtc_cmd->rtclc_mask_q_hi, mod_rtc_cmd->rtclc_mask_q_lo);

    rfm_hdr_set_nominal_pwr( RFM_DEVICE_0, access_params_ptr->open_loop_adjust );
    (void) rfm_hdr_set_tx_power_adjust( RFM_DEVICE_0, access_params_ptr->probe_initial_adjust );  

    /*
    for ( i = 0; i < hdrhitmsg_ftm_intf.fwd_link_info_rev0.drclocklength + 1; i++ )
    {
        drclocklength_slots_rev0 *= 2;
    }

    (void) hdrhitcmd_fmac_activate_msg( 
             HDRHITCMD_REV_0, 
             drclocklength_slots_rev0*2,
             ( hdrhitmsg_ftm_intf.fwd_link_info_rev0.drclockperiod + 1 )*8,
             12 
           );
    */
    hdrrmac_activate( HDRHAI_PHYSICAL_LAYER_PROTOCOL );

    /* Program the User Long Code Mask into the modulator. */
    hdrmod_set_pn_long_code_mask( i_mask, q_mask 
#ifdef FEATURE_HDR_BOLT_MODEM
      , 1                 /* Enable carrier 0 PN generator */
#endif /* FEATURE_HDR_BOLT_MODEM */
      );

    /* Make the mobile ACK, NAK, or not ACK every slot */
    hdr_ftm_fmac_state = hdrfmacsm_get_state();
    hdr_ack_mode = mod_rtc_cmd->hdr_ack_mode;

    /* Sanity check; plus make this code backward compatible */
    if (hdr_ack_mode > (byte) IS890A_MODE_NAK_OOK)
    {
        hdr_ack_mode = (byte) IS856_COMPLIANT;
    }

    if (hdr_ftm_fmac_state == HDRFMAC_INACTIVE_STATE)
    {
        hdrfmacsm_change_state( HDRFMAC_VAR_RATE_STATE );
    }

    HDR_MSG_HIT_1(MSG_LEGACY_HIGH, "RUMI TX only: ack_mode %d", hdr_ack_mode);
    hdrfmac_set_ack_mode ((hdrfmac_ack_mode_enum_type) hdr_ack_mode);

    if (hdr_ftm_fmac_state == HDRFMAC_INACTIVE_STATE)
    {
        hdrfmacsm_change_state( HDRFMAC_INACTIVE_STATE );
    }

    memset(&hdrhitmsg_ftm_tca_msg, 0, sizeof(hdrrup_tca_msg_type));
    hdrhitmsg_ftm_tca_msg.msg_id = 0;
    hdrhitmsg_ftm_tca_msg.msg_seq = 0;
    hdrhitmsg_ftm_tca_msg.chan_incl = 0;
    hdrhitmsg_ftm_tca_msg.frame_offset = mod_rtc_cmd->frame_offset;
    hdrhitmsg_ftm_tca_msg.dsc_channel_gain = 0;
    hdrhitmsg_ftm_tca_msg.link_id = 0;             //ref_sect_ptr->link_id;
    hdrhitmsg_ftm_tca_msg.num_sectors = 1;
    hdrhitmsg_ftm_tca_msg.num_sub_asets = 1;
    hdrhitmsg_ftm_tca_msg.scheduler_tag_incl = 0;
    hdrhitmsg_ftm_tca_msg.feedback_mux_enabled = 0;
    hdrhitmsg_ftm_tca_msg.sector_info[0].drc_cover = mod_rtc_cmd->drc_cover;
    hdrhitmsg_ftm_tca_msg.sector_info[0].pilot_pn = link_info.pilot_pn;
    hdrhitmsg_ftm_tca_msg.sector_info[0].rab_length = hdrhitmsg_ftm_intf.fwd_link_info_rev0.rablength;
    hdrhitmsg_ftm_tca_msg.sector_info[0].rab_offset = hdrhitmsg_ftm_intf.fwd_link_info_rev0.raboffset;
    hdrhitmsg_ftm_tca_msg.sector_info[0].softer_handoff = FALSE;
    hdrhitmsg_ftm_tca_msg.sector_info[0].ra_channel_gain = 0;
    hdrhitmsg_ftm_tca_msg.sector_info[0].dsc = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].pilot_included = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.drc_length = mod_rtc_cmd->drc_length;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.drc_channel_gain = mod_rtc_cmd->drc_gain;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.ack_channel_gain = mod_rtc_cmd->ack_gain;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.feedback_rev_ch_id = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].fwd_channel[0].mode = SYS_HDR_CHAN_REC_MODE_IS856;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].fwd_channel[0].channel = mod_rtc_cmd->hdr_chan;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].ftc_params[0].ftc_mac_index = 
        hdrhitmsg_ftm_intf.fwd_link_info_rev0.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].aux_drc_cover = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].rev_mac_index =
         hdrhitmsg_ftm_intf.fwd_link_info_rev0.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].rab_mac_index = 
        hdrhitmsg_ftm_intf.fwd_link_info_rev0.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].delta_t2p = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].pilot_group_id = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].scheduler_tag = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_rev_channels_incl = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_rev_channels = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].channel = mod_rtc_cmd->hdr_chan;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].a_order = 0; 
       /* Should be same as feedback rev chan id */
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].ch_config = 1; 

    hdrlmac_tc_assign( TRUE,                          /* Initial assignment  */
                       0,                             /* Link id             */
                       HDRSRCH_TUNE_AWAY_NONE,        /* Tuneaway disallowed */
                       HDRMDSP_DEMOD_CARRIER_0,       /* CC Subaset index    */
                       &hdrhitmsg_ftm_tca_msg,                      /* TCA msg ptr         */
                       HDRHAI_PHYSICAL_LAYER_PROTOCOL,/* Sender              */
                       HDRSCP_DEFAULT_RUP             /* RUP subtype         */    
                     );

     HDR_MSG_HIT(MSG_LEGACY_ERROR, "RUMI TX only: wait 10ms after lmac_tc_assign");
     /* This is a critical delay needed so that due to different task priorities 
     RF TX START happens before L1 TX DAC START. From an experiement, 3 ms yields
     ok Tx intermittently, 5 ms was consistently OK. Using 10 ms to be safe. */
     DALSYS_BusyWait(10000);
  
     HDR_MSG_HIT(MSG_LEGACY_ERROR, "RUMI TX only: Don't call hdrmod_tx_start() here");
     hdrmod_tx_start();  
     HDR_MSG_HIT(MSG_LEGACY_ERROR, "RUMI TX only: wait 27ms after hdrmod_tx_start");
     DALSYS_BusyWait(27000);  
      /* one frame-delay - 26.6 ms */

    /* Set RPC step size: Macro takes units of 1/2 dB.
       In RPC_step field, 0 = .5 dB steps
                        1 = 1.0 dB steps. 
       (Thus 1 is added to convert)  */
    (void) hdrhitcmd_modulator_cfg(HDRMDSP_DEMOD_CARRIER_0, FALSE, FALSE, 
                                   (mod_rtc_cmd->rpc_step + 1), TRUE);

    /* Disable throttling */
    hdrrmac_disable_throttling( HDRRMAC_FTM_ID, TRUE );

    total_pwr = rfm_hdr_get_tx_total_power(RFM_DEVICE_0,
                                          RFM_HDR_CARRIER_ID__0,
                                          RFM_CDMA_AGC_FORMAT__DBM256) /256;
  
    pilot_pwr = rfm_hdr_get_tx_pilot_power(RFM_DEVICE_0,
                                          RFM_HDR_CARRIER_ID__0,
                                          RFM_CDMA_AGC_FORMAT__DBM256) /256;

    hdrsrch_at_set_drc(HDRMDSP_DEMOD_CARRIER_0, mod_rtc_cmd->drc_value);
    hdrsrchtc_drc_param_ctrl(HDRMDSP_DEMOD_CARRIER_0);

    HDR_MSG_HIT_1(MSG_LEGACY_HIGH, "Total TX Power is :%d", total_pwr);
    HDR_MSG_HIT_1(MSG_LEGACY_HIGH, "Pilot Power is :%d", pilot_pwr);


    return;

} /* hdrhitmsg_process_mode_rtc */

/*===========================================================================

FUNCTION HDRHITMSG_START_RTC_REVA

DESCRIPTION
  This function starts the reverse line traffic channel for RevA.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_start_rtc_reva
(
    void
)
{
   
    hdrhitmsg_access_params_t       *access_params_ptr  = & ( hdrhitmsg_ftm_intf.access_params );
    hdrhitmsg_rmac_params_t         *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_mod_rtc_reva_cmd_msg_t   *mod_rtc_reva_cmd;
    //hdrsrch_sect_struct_type*       ref_sect_ptr;
    sys_link_type                   link_info;
    qword                           i_mask;
    qword                           q_mask;
    uint8                           hdr_ack_mode;
#ifndef FEATURE_HDR_BOLT_MODEM
    int16                           total_pwr;
    int16                           pilot_pwr;
    int16                           i;
    uint8                           hdr_ack_mode;
    uint8                           drclocklength_slots_reva = 1;
#endif /* FEATURE_HDR_BOLT_MODEM */
    hdrfmac_state_enum_type         hdr_ftm_fmac_state;
/*--------------------------------------------------------------------------*/

    mod_rtc_reva_cmd     = &(hdrhitmsg_msg_buf.msg.hdrhit_cmd_msg.mod_rtc_reva);

    hdrhitmsg_ftm_intf.fwd_link_info_reva.drc_cover = mod_rtc_reva_cmd->drc_cover;
    hdrhitmsg_ftm_intf.fwd_link_info_reva.dsc_value = mod_rtc_reva_cmd->dsc_value;

    rmac_params_ptr->modem_fwd_rev_link_type = 1;

    /* hard code band/channel/PN */
    link_info.channel.band = 0;
    link_info.channel.chan_num = 500;
    link_info.pilot_pn = 0;
    HDR_MSG_HIT_2(MSG_LEGACY_HIGH, "RUMI TX only: Chan:%d PN:%d", link_info.channel.chan_num, link_info.pilot_pn);



   (void) hdrhitcmd_enable_drc_gating(mod_rtc_reva_cmd->drc_gating);
    HDR_MSG_HIT_1(MSG_LEGACY_HIGH, "RUMI TX only: send FTCMAC_CHANNEL_CFG drc_gating %d",
                  mod_rtc_reva_cmd->drc_gating );

    hdrmod_set_traffic_mode(HDRMOD_TRAFFIC_MODE_REVA);

    rmac_params_ptr->modem_fwd_rev_link_type = 1;

    rmac_params_ptr->hicap_lolatency             = mod_rtc_reva_cmd->hicap_lolatency;
    rmac_params_ptr->gain_transition_point       = mod_rtc_reva_cmd->gain_transition_point;
    rmac_params_ptr->t2p_pre_transition          = mod_rtc_reva_cmd->t2p_pre_transition;
    rmac_params_ptr->t2p_post_transition         = mod_rtc_reva_cmd->t2p_post_transition;
    rmac_params_ptr->arq_mode                    = mod_rtc_reva_cmd->arq_mode;
    rmac_params_ptr->auxpil_payload_thresh       = mod_rtc_reva_cmd->auxpil_min_payload;
    rmac_params_ptr->auxpil_chan_gain            = mod_rtc_reva_cmd->auxpil_gain;
    rmac_params_ptr->rri_ch_gain_pre_transition  = mod_rtc_reva_cmd->rri_gain_pre_transition;
    rmac_params_ptr->rri_ch_gain_post_transition = mod_rtc_reva_cmd->rri_gain_post_transition;
    rmac_params_ptr->dsc_length                  = mod_rtc_reva_cmd->dsc_length;
    rmac_params_ptr->rtc_pattern                 = mod_rtc_reva_cmd->rtc_txpattern;
    rmac_params_ptr->tx_rri_reva                 = mod_rtc_reva_cmd->rri;
    rmac_params_ptr->rpc_enable                  = mod_rtc_reva_cmd->rpc_enable;
    rmac_params_ptr->rpc_step                    = mod_rtc_reva_cmd->rpc_step;
      /*should not add 1 here because hdrrmac3.c adds 1*/
    rmac_params_ptr->tx_payload_reva             = 
                     hdrenc_get_rri_reva_sizes( (int) mod_rtc_reva_cmd->rri );


    qw_set( i_mask, mod_rtc_reva_cmd->rtclc_mask_i_hi, mod_rtc_reva_cmd->rtclc_mask_i_lo );
    qw_set( q_mask, mod_rtc_reva_cmd->rtclc_mask_q_hi, mod_rtc_reva_cmd->rtclc_mask_q_lo );

    rfm_hdr_set_nominal_pwr( RFM_DEVICE_0, access_params_ptr->open_loop_adjust );
    (void) rfm_hdr_set_tx_power_adjust( RFM_DEVICE_0, access_params_ptr->probe_initial_adjust );  

    /*
    for (i=0; i < hdrhitmsg_ftm_intf.fwd_link_info_reva.drclocklength + 1; i++)
    {
        drclocklength_slots_reva *= 2;
    }

    (void) hdrhitcmd_fmac_activate_msg( 
              1,
              drclocklength_slots_reva * 4,
              (hdrhitmsg_ftm_intf.fwd_link_info_reva.drclockperiod + 1)*8,
              14 );
     */


    hdrrmac_activate(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

    /* Program the User Long Code Mask into the modulator. */
    hdrmod_set_pn_long_code_mask( i_mask, q_mask 
#ifdef FEATURE_HDR_BOLT_MODEM
      , 1                 /* Enable carrier 0 PN generator */
#endif /* FEATURE_HDR_BOLT_MODEM */
      );

    /* Make the mobile ACK, NAK, or not ACK every slot */
    hdr_ftm_fmac_state = hdrfmacsm_get_state();
    hdr_ack_mode       = mod_rtc_reva_cmd->hdr_ack_mode;

/* Sanity check; plus make this code backward compatible */
    if (hdr_ack_mode > (byte) IS890A_MODE_NAK_OOK)
    {
        hdr_ack_mode = (byte) IS856_COMPLIANT;
    }

    if (hdr_ftm_fmac_state == HDRFMAC_INACTIVE_STATE)
    {
        hdrfmacsm_change_state( HDRFMAC_VAR_RATE_STATE );
    }

    hdrfmac_set_ack_mode ((hdrfmac_ack_mode_enum_type) hdr_ack_mode);

    if (hdr_ftm_fmac_state == HDRFMAC_INACTIVE_STATE)
    {
        hdrfmacsm_change_state( HDRFMAC_INACTIVE_STATE );
    }


    memset(&hdrhitmsg_ftm_tca_msg, 0, sizeof(hdrrup_tca_msg_type));
    hdrhitmsg_ftm_tca_msg.msg_id = 0;
    hdrhitmsg_ftm_tca_msg.msg_seq = 0;
    hdrhitmsg_ftm_tca_msg.chan_incl = 0;
    hdrhitmsg_ftm_tca_msg.frame_offset = mod_rtc_reva_cmd->frame_offset;
    hdrhitmsg_ftm_tca_msg.dsc_channel_gain = mod_rtc_reva_cmd->dsc_gain;
    hdrhitmsg_ftm_tca_msg.link_id = 0; //ref_sect_ptr->link_id;
    hdrhitmsg_ftm_tca_msg.num_sectors = 1;
    hdrhitmsg_ftm_tca_msg.num_sub_asets = 1;
    hdrhitmsg_ftm_tca_msg.scheduler_tag_incl = 0;
    hdrhitmsg_ftm_tca_msg.feedback_mux_enabled = 0;
    hdrhitmsg_ftm_tca_msg.sector_info[0].drc_cover = mod_rtc_reva_cmd->drc_cover;
    hdrhitmsg_ftm_tca_msg.sector_info[0].pilot_pn = link_info.pilot_pn;
    hdrhitmsg_ftm_tca_msg.sector_info[0].rab_length = hdrhitmsg_ftm_intf.fwd_link_info_reva.rablength;
    hdrhitmsg_ftm_tca_msg.sector_info[0].rab_offset = hdrhitmsg_ftm_intf.fwd_link_info_reva.raboffset;
    hdrhitmsg_ftm_tca_msg.sector_info[0].softer_handoff = FALSE;
    hdrhitmsg_ftm_tca_msg.sector_info[0].ra_channel_gain = 0;
    hdrhitmsg_ftm_tca_msg.sector_info[0].dsc = mod_rtc_reva_cmd->dsc_value;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].pilot_included = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.feedback_rev_ch_id = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.drc_length = mod_rtc_reva_cmd->drc_length;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.drc_channel_gain = mod_rtc_reva_cmd->drc_gain;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.ack_channel_gain = mod_rtc_reva_cmd->ack_gain;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].fwd_channel[0].mode = SYS_HDR_CHAN_REC_MODE_IS856;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].fwd_channel[0].channel = mod_rtc_reva_cmd->hdr_chan;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].ftc_params[0].ftc_mac_index = 
                         hdrhitmsg_ftm_intf.fwd_link_info_reva.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].aux_drc_cover = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].rev_mac_index = 
                         hdrhitmsg_ftm_intf.fwd_link_info_reva.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].rab_mac_index = 
                         hdrhitmsg_ftm_intf.fwd_link_info_reva.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].delta_t2p = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].pilot_group_id = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].scheduler_tag = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_rev_channels_incl = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_rev_channels = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_fwd_channels = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].channel = 
        link_info.channel;
        //mod_rtc_reva_cmd->hdr_chan;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].a_order = 0; 
      /* Should be same as feedback rev chan id */
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].ch_config = 1; 


    hdrlmac_tc_assign( TRUE,                          /* Initial assignment  */
                       0,                             /* Link id             */
                       HDRSRCH_TUNE_AWAY_NONE,        /* Tuneaway disallowed */
                       HDRMDSP_DEMOD_CARRIER_0,       /* CC Subaset index    */
                       &hdrhitmsg_ftm_tca_msg,                      /* TCA msg ptr         */
                       HDRHAI_PHYSICAL_LAYER_PROTOCOL,/* Sender              */
                       HDRSCP_DEFAULT_RUP             /* RUP subtype         */    
                     );

    (void) hdrhitcmd_modulator_cfg(HDRMDSP_DEMOD_CARRIER_0, FALSE, FALSE, 
                                   (mod_rtc_reva_cmd->rpc_step + 1), TRUE);
    HDR_MSG_HIT(MSG_LEGACY_HIGH, "RUMI TX: Send REL0_RL_CFG message???????");

    hdrsrch_at_set_drc(HDRMDSP_DEMOD_CARRIER_0, mod_rtc_reva_cmd->drc_value);

    // disable throttling
    hdrrmac_disable_throttling( HDRRMAC_FTM_ID, TRUE );

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "RUMI TX: Start_rtc_reva() is done");

    return;

} /* hdrhitmsg_start_rtc_reva */
/*===========================================================================

FUNCTION HDRHITMSG_START_RTC_REVB

DESCRIPTION
  This function starts the reverse line traffic channel for RevB.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_start_rtc_revb
(
    void
)
{
    hdrhitmsg_access_params_t       *access_params_ptr  = & ( hdrhitmsg_ftm_intf.access_params );
    hdrhitmsg_rmac_params_t         *rmac_params_ptr    = & ( hdrhitmsg_ftm_intf.rmac_params );
    hdrhit_mod_rtc_revb_cmd_msg_t   *mod_rtc_revb_cmd;
//    hdrsrch_sect_struct_type*       ref_sect_ptr;
    sys_link_type                   link_info;  
    sys_channel_type                band_chan_local[3];
    qword                           i_mask;
    qword                           q_mask;
    int16                           total_pwr[MAX_NUM_REV_PHY_CHANS];
    int16                           pilot_pwr[MAX_NUM_REV_PHY_CHANS];
    int8                            i;
    hdrfmac_state_enum_type         hdr_ftm_fmac_state;
    uint8                           rpc_enable;
    uint8                           single_fwd_link_mode, hicap_lolat;
    uint8                           rri_c0;
    uint8                           rri_c1;
    uint8                           rri_c2;
#ifndef FEATURE_HDR_BOLT_MODEM
    uint8                           drclocklength_slots_revb = 1;
#endif /* FEATURE_HDR_BOLT_MODEM */
/*------------------------------------------------------------------------*/

    mod_rtc_revb_cmd     = &(hdrhitmsg_msg_buf.msg.hdrhit_cmd_msg.mod_rtc_revb);

    //hdrhitmsg_ftm_intf.fwd_link_info_revb.drc_cover = mod_rtc_revb_cmd->drc_cover;
    //hdrhitmsg_ftm_intf.fwd_link_info_revb.dsc_value = mod_rtc_revb_cmd->dsc_value;

    /* hard code band/channel/PN */
    link_info.channel.band = 0;
    link_info.channel.chan_num = 500;
    link_info.pilot_pn = 0;
    HDR_MSG_HIT_2(MSG_LEGACY_HIGH, "RUMI TX only: Chan:%d PN:%d", link_info.channel.chan_num, link_info.pilot_pn);


    single_fwd_link_mode = (mod_rtc_revb_cmd->single_fwd_link_mode == 0)? 0:1;
    hicap_lolat          = (mod_rtc_revb_cmd->hicap_lolatency == 0)? 0:1;
    rpc_enable           = (mod_rtc_revb_cmd->rpc_enable ==0)? 0:1;
    rri_c0               =  mod_rtc_revb_cmd->rri_c0;
    rri_c1               =  mod_rtc_revb_cmd->rri_c1;
    rri_c2               =  mod_rtc_revb_cmd->rri_c2; 

    if (rri_c0 > 12)
        rri_c0 = 12;

    if (rri_c1 > 12)
        rri_c1 = 12;

    if (rri_c2 > 12)
        rri_c2 = 12;

    /* Check channels */
    /* Assign main channel */
    band_chan_local[0] = mod_rtc_revb_cmd->hdr_chan;
    /* Check 2nd channel */
    band_chan_local[1].band = mod_rtc_revb_cmd->hdr_chan.band;
    band_chan_local[1].chan_num = mod_rtc_revb_cmd->chan_c1;
    /* Check 3rd channel */
    band_chan_local[2].band = mod_rtc_revb_cmd->hdr_chan.band;
    band_chan_local[2].chan_num = mod_rtc_revb_cmd->chan_c2;

    HDR_MSG_HIT_4(MSG_LEGACY_HIGH,
        "RUMI TX only mode_rtc_revb: Chan1=%d Chan2=%d Chan3=%d lchan_num=%d",
        mod_rtc_revb_cmd->hdr_chan.chan_num,
        mod_rtc_revb_cmd->chan_c1,
        mod_rtc_revb_cmd->chan_c2,
        band_chan_local[0].chan_num);


    /* hard code band/channel/PN */
    link_info.channel.band = 0;
    link_info.channel.chan_num = 500;
    link_info.pilot_pn = 0;
    HDR_MSG_HIT_2(MSG_LEGACY_HIGH, "RUMI TX only: Chan:%d PN:%d", link_info.channel.chan_num, link_info.pilot_pn);


    /* Set forward link type to RevB */
    rmac_params_ptr->modem_fwd_rev_link_type = 2;

   (void) hdrhitcmd_enable_drc_gating(FALSE);

    HDR_MSG_HIT_1(MSG_LEGACY_HIGH, "RUMI TX only: send FTCMAC_CHANNEL_CFG drc_gating %d",
                 0 );

    hdrmod_set_traffic_mode(HDRMOD_TRAFFIC_MODE_REVB);

    rmac_params_ptr->revb_single_fwd_link_mode = single_fwd_link_mode;
    rmac_params_ptr->revb_num_carriers      = mod_rtc_revb_cmd->num_carriers;
    rmac_params_ptr->hicap_lolatency        = hicap_lolat;

    rmac_params_ptr->tx_rri_revb_c[0]    =   rri_c0;
    rmac_params_ptr->tx_rri_revb_c[1]    =   rri_c1;
    rmac_params_ptr->tx_rri_revb_c[2]    =   rri_c2;

    rmac_params_ptr->gain_transition_point = rmac_params_ptr->revb_gain_transition_point;

    if (hicap_lolat == 0) /* 0 means hicap */
    {
        rmac_params_ptr->t2p_pre_transition_revb_c[0]  = rmac_params_ptr->t2p_hicap_revb[0][rri_c0];
        rmac_params_ptr->t2p_pre_transition_revb_c[1]  = rmac_params_ptr->t2p_hicap_revb[0][rri_c1];
        rmac_params_ptr->t2p_pre_transition_revb_c[2]  = rmac_params_ptr->t2p_hicap_revb[0][rri_c2];
        rmac_params_ptr->t2p_post_transition_revb_c[0] = rmac_params_ptr->t2p_hicap_revb[1][rri_c0];
        rmac_params_ptr->t2p_post_transition_revb_c[1] = rmac_params_ptr->t2p_hicap_revb[1][rri_c1];
        rmac_params_ptr->t2p_post_transition_revb_c[2] = rmac_params_ptr->t2p_hicap_revb[1][rri_c2];
    }
    else                  /* lolat */
    {
        rmac_params_ptr->t2p_pre_transition_revb_c[0]  = rmac_params_ptr->t2p_lolat_revb[0][rri_c0];
        rmac_params_ptr->t2p_pre_transition_revb_c[1]  = rmac_params_ptr->t2p_lolat_revb[0][rri_c1];
        rmac_params_ptr->t2p_pre_transition_revb_c[2]  = rmac_params_ptr->t2p_lolat_revb[0][rri_c2];
        rmac_params_ptr->t2p_post_transition_revb_c[0] = rmac_params_ptr->t2p_lolat_revb[1][rri_c0];
        rmac_params_ptr->t2p_post_transition_revb_c[1] = rmac_params_ptr->t2p_lolat_revb[1][rri_c1];
        rmac_params_ptr->t2p_post_transition_revb_c[2] = rmac_params_ptr->t2p_lolat_revb[1][rri_c2];
    }

    rmac_params_ptr->arq_mode                    = rmac_params_ptr->revb_arq_mode;
    rmac_params_ptr->auxpil_payload_thresh       = rmac_params_ptr->revb_auxpil_payload_thresh;
    rmac_params_ptr->auxpil_chan_gain            = mod_rtc_revb_cmd->auxpil_gain;
    rmac_params_ptr->rri_ch_gain_pre_transition  = rmac_params_ptr->rri_gain_revb[0][rmac_params_ptr->revb_gain_transition_point];
    rmac_params_ptr->rri_ch_gain_post_transition = rmac_params_ptr->rri_gain_revb[1][rmac_params_ptr->revb_gain_transition_point];
    rmac_params_ptr->dsc_length                  = rmac_params_ptr->revb_dsc_length;
    rmac_params_ptr->rtc_pattern                 = rmac_params_ptr->revb_rtc_pattern;


    for ( i=0; i< MAX_NUM_REV_PHY_CHANS; i++ )
    {
        /* RevA and RevB have common RL packet sizes.  
           Hence, hdrenc_rri_reva_sizes[] is used as the lookup table 
           for finding RL payload size. */
        rmac_params_ptr->tx_payload_revb_c[i] = 
            hdrenc_get_rri_reva_sizes( (int) rmac_params_ptr->tx_rri_revb_c[i] );
    }

    rmac_params_ptr->revb_closed_loop_c[0] = 
        ( (uint16) mod_rtc_revb_cmd->power_offset_c0 )*128; 
      /* unit from 1/4 dB to 1/512 dB*/
    rmac_params_ptr->revb_closed_loop_c[1] = 
        ( (uint16) mod_rtc_revb_cmd->power_offset_c1 )*128; 
      /* unit from 1/4 dB to 1/512 dB*/
    rmac_params_ptr->revb_closed_loop_c[2] = 
        ( (uint16) mod_rtc_revb_cmd->power_offset_c2 )*128; 
      /* unit from 1/4 dB to 1/512 dB*/
    rmac_params_ptr->rpc_enable            =  rpc_enable;
    rmac_params_ptr->rpc_step              =  mod_rtc_revb_cmd->rpc_step;

    qw_set( i_mask, rmac_params_ptr->revb_rtclc_mask_i_hi, rmac_params_ptr->revb_rtclc_mask_i_lo );
    qw_set( q_mask, rmac_params_ptr->revb_rtclc_mask_q_hi, rmac_params_ptr->revb_rtclc_mask_q_lo );

    rfm_hdr_set_nominal_pwr( RFM_DEVICE_0, access_params_ptr->open_loop_adjust );
    (void) rfm_hdr_set_tx_power_adjust( RFM_DEVICE_0, access_params_ptr->probe_initial_adjust );  

    /*
    for ( i=0; i < hdrhitmsg_ftm_intf.fwd_link_info_revb.drclocklength + 1; i++ )
    {
        drclocklength_slots_revb = drclocklength_slots_revb * 2;
    }

    (void) hdrhitcmd_fmac_activate_msg(HDRHITCMD_REV_B,
                                drclocklength_slots_revb*4,
                                (hdrhitmsg_ftm_intf.fwd_link_info_revb.drclockperiod+1)*8,
                                14);
     */
    hdrrmac_activate( HDRHAI_PHYSICAL_LAYER_PROTOCOL );
    /* Program the User Long Code Mask into the modulator. */
    hdrmod_set_pn_long_code_mask( i_mask, q_mask 
#ifdef FEATURE_HDR_BOLT_MODEM
      , 7    /* Enable PN generator for all three carriers */
#endif /* FEATURE_HDR_BOLT_MODEM */
      );

    /* Make the mobile ACK, NAK, or not ACK every slot */
    hdr_ftm_fmac_state = hdrfmacsm_get_state();

/* Sanity check; plus make this code backward compatible */
    if ( rmac_params_ptr->revb_ack_mode > (byte) IS890A_MODE_NAK_OOK )
    {
        rmac_params_ptr->revb_ack_mode = (byte) IS890_MODE_1; 
         /* IS890_MODE_1 is the preferred mode */
    }

    if (hdr_ftm_fmac_state == HDRFMAC_INACTIVE_STATE)
    {
        hdrfmacsm_change_state( HDRFMAC_VAR_RATE_STATE );
    }

    hdrfmac_set_ack_mode (( hdrfmac_ack_mode_enum_type ) rmac_params_ptr->revb_ack_mode);

    if (hdr_ftm_fmac_state == HDRFMAC_INACTIVE_STATE)
    {
        hdrfmacsm_change_state( HDRFMAC_INACTIVE_STATE );
    }

    /* Construct TCA msg */
    memset(&hdrhitmsg_ftm_tca_msg, 0, sizeof(hdrrup_tca_msg_type));

    hdrhitmsg_ftm_tca_msg.msg_id = 0;
    hdrhitmsg_ftm_tca_msg.msg_seq = 0;
    hdrhitmsg_ftm_tca_msg.chan_incl = 0;
    hdrhitmsg_ftm_tca_msg.frame_offset = rmac_params_ptr->revb_frame_offset;
    hdrhitmsg_ftm_tca_msg.dsc_channel_gain = mod_rtc_revb_cmd->dsc_gain;
    hdrhitmsg_ftm_tca_msg.link_id = 0; //ref_sect_ptr->link_id;
    hdrhitmsg_ftm_tca_msg.num_sectors = 1;
    hdrhitmsg_ftm_tca_msg.num_sub_asets = rmac_params_ptr->revb_num_carriers;
    hdrhitmsg_ftm_tca_msg.scheduler_tag_incl = 0;
    hdrhitmsg_ftm_tca_msg.feedback_mux_enabled = 0;
    hdrhitmsg_ftm_tca_msg.sector_info[0].drc_cover = rmac_params_ptr->revb_drc_cover;
    hdrhitmsg_ftm_tca_msg.sector_info[0].pilot_pn = link_info.pilot_pn;
    hdrhitmsg_ftm_tca_msg.sector_info[0].rab_length = hdrhitmsg_ftm_intf.fwd_link_info_revb.rablength;
    hdrhitmsg_ftm_tca_msg.sector_info[0].rab_offset = hdrhitmsg_ftm_intf.fwd_link_info_revb.raboffset;
    hdrhitmsg_ftm_tca_msg.sector_info[0].softer_handoff = FALSE;
    hdrhitmsg_ftm_tca_msg.sector_info[0].ra_channel_gain = 0;
    hdrhitmsg_ftm_tca_msg.sector_info[0].dsc = rmac_params_ptr->revb_dsc_value;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].pilot_included = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.feedback_rev_ch_id = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.drc_length = rmac_params_ptr->revb_drc_length;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.drc_channel_gain = mod_rtc_revb_cmd->drc_gain;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].ftc_params.ack_channel_gain = mod_rtc_revb_cmd->ack_gain;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].fwd_channel[0].mode = SYS_HDR_CHAN_REC_MODE_IS856;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].fwd_channel[0].channel = mod_rtc_revb_cmd->hdr_chan;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].ftc_params[0].ftc_mac_index = 
        hdrhitmsg_ftm_intf.fwd_link_info_revb.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].aux_drc_cover = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].rev_mac_index = 
        hdrhitmsg_ftm_intf.fwd_link_info_revb.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].rab_mac_index = 
        hdrhitmsg_ftm_intf.fwd_link_info_revb.mac_index;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].rtc_params[0].delta_t2p = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].pilot_group_id = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].sector_params[0].scheduler_tag = 0;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_rev_channels_incl = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_rev_channels = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].num_fwd_channels = 1;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].channel = mod_rtc_revb_cmd->hdr_chan;
    hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0].rev_channel[0].ch_config = 1; 

    /* Replicate repeating parameters across subactive sets and 
       sectors */
    for (i = 1; i< MAX_NUM_REV_PHY_CHANS; i++)
    {
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrhitmsg_ftm_tca_msg.sub_aset[i] = 
                hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0];
        hdrhitmsg_ftm_tca_msg.sector_info[i] = 
                hdrhitmsg_ftm_tca_msg.sector_info[HDRMDSP_DEMOD_CARRIER_0];
#else
        memcpy(&(hdrhitmsg_ftm_tca_msg.sub_aset[i]), 
               &(hdrhitmsg_ftm_tca_msg.sub_aset[HDRMDSP_DEMOD_CARRIER_0]), 
               sizeof(hdrrup_tca_sub_aset_type));
        memcpy(&(hdrhitmsg_ftm_tca_msg.sector_info[i]), 
               &(hdrhitmsg_ftm_tca_msg.sector_info[HDRMDSP_DEMOD_CARRIER_0]),
                sizeof(hdrrup_tca_sector_info_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    }

    /* Configure channel numbers for other carriers.  All carriers
       must be in the same band. */
    hdrhitmsg_ftm_tca_msg.sub_aset[1].fwd_channel[0].channel.chan_num = mod_rtc_revb_cmd->chan_c1;
    hdrhitmsg_ftm_tca_msg.sub_aset[1].rev_channel[0].channel.chan_num = mod_rtc_revb_cmd->chan_c1;

    hdrhitmsg_ftm_tca_msg.sub_aset[2].fwd_channel[0].channel.chan_num = mod_rtc_revb_cmd->chan_c2;
    hdrhitmsg_ftm_tca_msg.sub_aset[2].rev_channel[0].channel.chan_num = mod_rtc_revb_cmd->chan_c2;

    // disable throttling
    hdrrmac_disable_throttling( HDRRMAC_FTM_ID, TRUE );

    hdrlmac_tc_assign( TRUE,                          /* Initial assignment  */
                       0,  //ref_sect_ptr->link_id,         /* Link id             */
                       HDRSRCH_TUNE_AWAY_NONE,        /* Tuneaway disallowed */
                       HDRMDSP_DEMOD_CARRIER_0,       /* CC Subaset index    */
                       &hdrhitmsg_ftm_tca_msg,                      /* TCA msg ptr         */
                       HDRHAI_PHYSICAL_LAYER_PROTOCOL,/* Sender              */
                       HDRSCP_MC_RUP                  /* MultiCarrier RUP subtype */
                     );

    DALSYS_BusyWait(30000);  
     /* one frame-delay - 26.6 ms */

    memset(total_pwr, 0x0, sizeof(total_pwr));
    memset(pilot_pwr, 0x0, sizeof(pilot_pwr));

    /* Set per-carrier DRC value and compute pilot power */
    for ( i=0; i < rmac_params_ptr->revb_num_carriers; i++ )
    {

       (void) hdrhitcmd_modulator_cfg(i, TRUE, TRUE, 
                         (mod_rtc_revb_cmd->rpc_step + 1), TRUE);

        /* Set DRC value - same DRC is requested on all carriers */
        hdrsrch_at_set_drc( i, mod_rtc_revb_cmd->drc_value);


        /* Compute pilot power */  
        total_pwr[i] = rfm_hdr_get_tx_total_power(RFM_DEVICE_0,
                                            (rfm_cdma_carrier_type) i,
                                            RFM_CDMA_AGC_FORMAT__DBM256) /256;
    
        pilot_pwr[i] = rfm_hdr_get_tx_pilot_power(RFM_DEVICE_0,
                                             (rfm_cdma_carrier_type) i,
                                            RFM_CDMA_AGC_FORMAT__DBM256) /256;
   }

    HDR_MSG_HIT_3(MSG_LEGACY_HIGH, "Total TX Power is :c0=%d, c1=%d, c2=%d dBm",
                  total_pwr[0], total_pwr[1], total_pwr[2]);
    HDR_MSG_HIT_3(MSG_LEGACY_HIGH, "Carrier pilot powers: %d, %d, %d",
                   pilot_pwr[0], pilot_pwr[1], pilot_pwr[2]);

    HDR_MSG_HIT(MSG_LEGACY_HIGH, "RUMI TX only: mod_rtc_revb is done");

    return;
} /* hdrhitmsg_start_rtc_reva */
/*===========================================================================

FUNCTION HDRHITMSG_TIME_VALID

DESCRIPTION
  This Interrupt Service Routine completes the time synchronization.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  There will be a time discontinuity.
  The ISR auto uninstalls itself.

===========================================================================*/
void hdrhitmsg_time_valid( void )
{

/*---------------------------------------------------------------------------------------------*/
    switch ( hdrhitmsg_tx_action )
    {
       case HDRHITMSG_TX_ACC_REV0 : 
         hdrhitmsg_start_acc();
         break;

       case HDRHITMSG_TX_ACC_REVA : 
         hdrhitmsg_start_acc_reva();
         break;

       case HDRHITMSG_TX_RTC_REV0 : 
         hdrhitmsg_start_rtc();
         break;

       case HDRHITMSG_TX_RTC_REVA : 
         hdrhitmsg_start_rtc_reva();
         break;

       case HDRHITMSG_TX_RTC_REVB : 
         hdrhitmsg_start_rtc_revb();
         break;

       default :
         break;
    }
}

/*===========================================================================

FUNCTION HDRHITMSG_START_FLEX_CAPTURE

DESCRIPTION
 This function is used to trigger flex capture.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void hdrhitmsg_start_flex_capture( void )
{
  static boolean tx_capture_triggered = FALSE;
/*-----------------------------------------------------------------------*/

  if( tx_capture_triggered == FALSE )
  {
    volatile uint32 *flex_addr;
    HDR_MSG_DRIVERS(MSG_LEGACY_HIGH, "Triggering Flex capture" );

    flex_addr = (volatile uint32 *)HDRHITMSG_FLEX_ADDRESS;
    /* Trigger flex to start recording */
    *flex_addr = 0x100000;
    tx_capture_triggered = TRUE;
  }
}

#endif /* FEATURE_HDR_RUMI_TX_ONLY */
#endif /* FEATURE_FACTORY_TESTMODE */

