/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2003 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2013 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2014 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2015 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/ftm/src/ftm_hsdpa_bler.c#1 $
  $DateTime: 2016/12/13 07:59:54 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/02/15   ag      Initialize the parameters in cmd properly by first setting them all to NULL  
04/23/13   nv/kai  Fix HS call failure due to change in number of HARQ buffers
04/10/13   gh      Remove feature for Triton
04/03/13   kcj     Feature out previous changes for Triton. 
04/03/13   kcj     Updated from RRC/L1 interface change. 
06/07/12   nv      Fixed multiple issues in HSDPA Non-Signalling
                   a) Removed CPHY_DL_PHYCHAN_CFG_INCL in 
                      ftm_hsdpa_bler_request_stop_hs_channel() as it is not 
                      required.
                   b) Added parameter L1_DL_PHYCHAN_DPCH in
                      ftm_hsdpa_bler_start_hs_channel() as we are sending CPHY_DL_PHYCHAN_CFG_INCL
                      in request mask to L1. 
02/16/12   gr/ars  Fix compilation issues 
02/13/12   swb     Added API to update L1 parameters for HSDPA reconfigure during HO 
10/20/11    gh     Fix non-signaling start call
04/26/11    dw     CMI compliance changes
04/18/11   dw      KW fixes
10/21/10   ems     Factory features Rx Lvl, non-signaling events and SE/BER
08/13/10    dw     Added mac_hs_or_ehs config parameter for REL7 HSDPA non-signaling
03/16/10   kguo    Merged from scmm_final_merge branch
02/17/10    vb     Klocwork error fixes
01/25/10    vb     Lint error fixes
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
07/25/07   ycl     Lint Fixes.
12/08/06   dp      Use call_type_mask properly for CPHY_SETUP
05/04/06   xw      Removed the feature wrapped around #include "hscfg.h".
04/20/06   mc      Added support for HSDPA performance optimization.
10-17-05   dp      Add ftm_umts_ber.h include file.
09/26/05   dp      Update F3 messages.
07/11/05   dp      Add more debug messages for configuration commands.
                   Better tracking of states.  Ability to resume HS config
                   after an interfreq HHO on RMC DCH connection.
06/08/05   dp      Add status field to FTM HSDPA logs.
05/19/05   dp      Add state specific logging.
03-21-05   dp      Created
===========================================================================*/


/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include <stdio.h>
#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "rf_test_task_v.h"
#include "ftmdiag.h"
#include "queue.h"
#include "rrccmd.h"
#include "ftm_log.h"
#include "ftm_umts_ber.h"
#include "ftm_wcdma_ber.h"
#include "ftm_hsdpa_bler.h"
#include "wl1api.h"

#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_FTM_WCDMA_BER
#ifdef FEATURE_HSDPA

/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/

static l1_hsdpa_info_struct_type   ftm_hsdpa_config_info;
static ftm_hsdpa_bler_db_type       ftm_hsdpa_bler_db;

/* Structure for events that will be reported synchronously */
typedef struct 
{
	uint8 has_updated;
	ftm_hsdpa_bler_sync_status_type status;
} ftm_hsdpa_bler_sync_status_struct;

/* array to recrod the events that will be reported synchronously via FTM_HSDPA_BLER_EVENT_STATUS_REQ*/
ftm_hsdpa_bler_sync_status_struct HSDPA_BLER_Sync_Status[  FTM_HSDPA_BLER_SYNC_STATUS_MAX ];

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

static void    ftm_hsdpa_bler_reset_cmd_state( void);

static boolean ftm_hsdpa_bler_start_hs_channel( void);
static boolean ftm_hsdpa_bler_stop_hs_channel( void);
static boolean ftm_hsdpa_bler_reconfig_hs_channel( void *pkt);
static boolean ftm_hsdpa_bler_config_hs_dpcch( void *pkt);
static boolean ftm_hsdpa_bler_config_hs_scch( void *pkt);
static boolean ftm_hsdpa_bler_config_hs_dsch( void *pkt);
static boolean ftm_hsdpa_bler_config_hs_pdsch( void *pkt);
void ftm_hsdpa_bler_clear_sync_status( void);

//Long INT 4 bytes -2147483648 to 2147483647
#define MAX_LONG_INT_DIGITS 11
//UINT8 1 byte 0 ~ 256
#define MAX_UINT8_DIGITS    3

/*===========================================================================

FUNCTION     FTM_HSDPA_BLER_LOG_GEN_CNF

DESCRIPTION
             Logs a generic confirmation message to the user (PC side).  This
             is used when the indication is that a command was completed,
             but no payload needs to be sent.

DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_hsdpa_bler_log_gen_cnf( ftm_log_hsdpa_bler_id_type  id)
{

#ifdef FTM_HAS_LOGGING

    ftm_log_hsdpa_bler_gen_cnf        *gen_cnf_log;


    gen_cnf_log = ( ftm_log_hsdpa_bler_gen_cnf *)
                    ftm_log_malloc( (uint16)FTM_LOG_WCDMA_BER,
                                    sizeof( ftm_log_hsdpa_bler_gen_cnf) );

    if( gen_cnf_log != NULL)
    {
        gen_cnf_log->log_id = id;

        log_commit( gen_cnf_log);
    }
#endif

}

/*===========================================================================

FUNCTION     FTM_HSDPA_BLER_LOG_GEN_CNF_WITH_STATUS

DESCRIPTION
             Logs a generic confirmation message to the user (PC side).  This
             is used when the indication is that a command was completed, and
             pass a generic status byte to the user.

DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_hsdpa_bler_log_gen_cnf_with_status( ftm_log_hsdpa_bler_id_type  id, uint8 status)
{
    ftm_hsdpa_bler_sync_status_type sync_status = FTM_HSDPA_BLER_SYNC_STATUS_GEN_FAILURE;

#ifdef FTM_HAS_LOGGING
    ftm_log_hsdpa_bler_gen_cnf_with_status        *gen_cnf_log;

    gen_cnf_log = ( ftm_log_hsdpa_bler_gen_cnf_with_status *)
                    ftm_log_malloc( (uint16)FTM_LOG_WCDMA_BER,
                                    sizeof( ftm_log_hsdpa_bler_gen_cnf_with_status) );

    if( gen_cnf_log != NULL)
    {
        gen_cnf_log->log_id = id;
        gen_cnf_log->status = status;

        log_commit( gen_cnf_log);
    }
#endif
    // Fill out the entry for the synchronous status message
    if ( status )
      sync_status = FTM_HSDPA_BLER_SYNC_STATUS_SUCCESS;
    else
      sync_status = FTM_HSDPA_BLER_SYNC_STATUS_EVENT_STATUS_FAILED;

    switch ( id )
    {
      case FTM_LOG_HSDPA_HS_CHANNEL_START_IND:
      {

        HSDPA_BLER_Sync_Status[ FTM_HSDPA_BLER_HS_CHANNEL_START_SYNC_STATUS ].status = sync_status;
        HSDPA_BLER_Sync_Status[ FTM_HSDPA_BLER_HS_CHANNEL_START_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

        break;
      }
      case FTM_LOG_HSDPA_HS_CHANNEL_STOP_IND:
      {
        HSDPA_BLER_Sync_Status[ FTM_HSDPA_BLER_HS_CHANNEL_STOP_SYNC_STATUS ].status = sync_status;
        HSDPA_BLER_Sync_Status[ FTM_HSDPA_BLER_HS_CHANNEL_STOP_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

        break;
      }
      case FTM_LOG_HSDPA_HS_CHANNEL_RECFG_IND:
      {
      
        HSDPA_BLER_Sync_Status[ FTM_HSDPA_BLER_HS_CHANNEL_RECFG_SYNC_STATUS ].status = sync_status;
        HSDPA_BLER_Sync_Status[ FTM_HSDPA_BLER_HS_CHANNEL_RECFG_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );
        break;
      }
    }

}

/*===========================================================================

FUNCTION        FTM_HSDPA_BLER_INIT

DESCRIPTION
.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_hsdpa_bler_init( void)
{

    ftm_hsdpa_bler_db.isInitialized  = TRUE;
    ftm_hsdpa_bler_db.ftm_hsdpa_info = &ftm_hsdpa_config_info;
    ftm_hsdpa_bler_db.current_op     = HSDPA_SETUP_OPS_NOOP;

    /* Initialize some default MAC-d info...not sure how this should be handled
     * currently
     */

    ftm_hsdpa_config_info.hs_dsch_info.macd_info.num_mac_hs_q = 1;

    ftm_hsdpa_config_info.hs_dsch_info.macd_info.mac_hs_q_info[0].mac_hs_q_id      = 0;
    ftm_hsdpa_config_info.hs_dsch_info.macd_info.mac_hs_q_info[0].num_mac_d_pdu_sz = 1;

    ftm_hsdpa_config_info.hs_dsch_info.macd_info.mac_hs_q_info[0].mac_d_pdu_sz_info[0].pdu_sz_idx = 0;
    ftm_hsdpa_config_info.hs_dsch_info.macd_info.mac_hs_q_info[0].mac_d_pdu_sz_info[0].pdu_sz     = 336;

    /* No support here for sec. scrambling code, although interface supports it */

    ftm_hsdpa_config_info.hs_scch_info.sec_scr_code_valid = FALSE;

    ftm_hsdpa_bler_db.isInitialized  = TRUE;
    ftm_hsdpa_bler_db.hs_config_to_resume_after_recfg = FALSE;

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
static void ftm_hsdpa_bler_reset_cmd_state( void)
{

    ftm_hsdpa_bler_db.current_op = HSDPA_SETUP_OPS_NOOP;

}





/*===========================================================================

FUNCTION        FTM_HSDPA_BLER_START_HS_CHANNEL

DESCRIPTION     Commands WL1 to begin processing of an HSDPA channel.


DEPENDENCIES    All HS channels have been configured.


RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
static boolean ftm_hsdpa_bler_start_hs_channel( void)
{

    l1_setup_cmd_type                  cmd;
	memset(&cmd, 0, sizeof(l1_setup_cmd_type));


    if( ftm_hsdpa_bler_db.isInitialized == FALSE || ftm_hsdpa_bler_db.current_op != HSDPA_SETUP_OPS_NOOP)
    {

        return FALSE;
    }


    if( ftm_wcdma_ber_get_state() != FTM_WCDMA_BER_STATE_DCH)
    {

        return FALSE;
    }

    /* Make sure hs channel is inactive before starting */

    if( ftm_hsdpa_bler_db.current_state != FTM_HSDPA_BLER_HS_CHAN_STATE_INACTIVE)
    {
        return FALSE;
    }

    
    /* Not currently sending a MAC config along with HS config, so don't set these signals */
    
    ftm_wcdma_ber_init_cphy_setup_cmd( &cmd, FALSE);

    ftm_hsdpa_bler_db.current_op                  = HSDPA_SETUP_OPS_START;
    ftm_hsdpa_bler_db.ftm_hsdpa_info->hs_req_mask = CPHY_HS_CFG_ALL_INCL;

    cmd.call_type_mask = CPHY_VOICE_CALL_ACTIVE | CPHY_HS_CALL_ACTIVE;   /* R99 + HS */
    cmd.req_mask      = (CPHY_HS_CHAN_CFG_INCL | CPHY_DL_PHYCHAN_CFG_INCL);
    cmd.hsdpa_l1_ops  = ftm_hsdpa_bler_db.current_op;
    cmd.hsdpa_l1_info = ftm_hsdpa_bler_db.ftm_hsdpa_info;
    cmd.dl_phychan = L1_DL_PHYCHAN_DPCH;

    FTM_MSG_HIGH("Attempting to Start HS channel",0,0,0);

    ftm_wcdma_ber_send_CPHY_SETUP_REQ( &cmd);

    return TRUE;
}







/*===========================================================================

FUNCTION      FTM_HSDPA_BLER_STOP_HS_CHANNEL

DESCRIPTION   Stops L1 processing of HS signal


DEPENDENCIES  HSDPA was active

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
static boolean ftm_hsdpa_bler_stop_hs_channel( void)
{

    l1_setup_cmd_type                  cmd;
	memset(&cmd, 0, sizeof(l1_setup_cmd_type));

    /* Check if any command in progress before sending another message to WL1 */

    if( ftm_hsdpa_bler_db.isInitialized == FALSE || ftm_hsdpa_bler_db.current_op != HSDPA_SETUP_OPS_NOOP)
    {

        return FALSE;
    }

    /* Make sure hs channel active before stopping */

    if( ftm_hsdpa_bler_db.current_state != FTM_HSDPA_BLER_HS_CHAN_STATE_ACTIVE)
    {

        return FALSE;
    }


    ftm_wcdma_ber_init_cphy_setup_cmd( &cmd, FALSE);

    ftm_hsdpa_bler_db.current_op                  = HSDPA_SETUP_OPS_STOP;
    ftm_hsdpa_bler_db.ftm_hsdpa_info->hs_req_mask = 0;

    cmd.req_mask      = (CPHY_HS_CHAN_CFG_INCL | CPHY_DL_PHYCHAN_CFG_INCL);
    cmd.hsdpa_l1_ops  = ftm_hsdpa_bler_db.current_op;
    cmd.hsdpa_l1_info = ftm_hsdpa_bler_db.ftm_hsdpa_info;

    FTM_MSG_HIGH("Attempting to Stop HS Channel",0,0,0);

    ftm_wcdma_ber_send_CPHY_SETUP_REQ( &cmd);

    return TRUE;
}



/*===========================================================================

FUNCTION      FTM_HSDPA_BLER_REQUEST_STOP_HS_CHANNEL

DESCRIPTION   Function for outside module to call to request stop of HS channel.
              This will check if a HS channel is already active, and add the 
              HS stop command to the l1_setup_cmd_type buffer in progress.


DEPENDENCIES  FTM WCDMA BER feature DCH should be active

RETURN VALUE
              Error code for operation

SIDE EFFECTS
   None.

===========================================================================*/
ftm_hsdpa_bler_hs_force_stop_error_type ftm_hsdpa_bler_request_stop_hs_channel( l1_setup_cmd_type *cmd)
{


    /* Check if any command in progress before sending another message to WL1 */

    if( ftm_hsdpa_bler_db.isInitialized == FALSE || ftm_hsdpa_bler_db.current_op != HSDPA_SETUP_OPS_NOOP)
    {

        return FTM_HSDPA_BLER_HS_FORCE_STOP__ERROR_OP_IN_PROGRESS;
    }

    /* Make sure hs channel active before stopping */

    if( ftm_hsdpa_bler_db.current_state != FTM_HSDPA_BLER_HS_CHAN_STATE_ACTIVE)
    {

        return FTM_HSDPA_BLER_HS_FORCE_STOP__ERROR_HS_INACTIVE;
    }

    ftm_hsdpa_bler_db.current_op                  = HSDPA_SETUP_OPS_STOP;
    ftm_hsdpa_bler_db.ftm_hsdpa_info->hs_req_mask = 0;

    cmd->req_mask      |= (CPHY_HS_CHAN_CFG_INCL );
    cmd->hsdpa_l1_ops   = ftm_hsdpa_bler_db.current_op;
    cmd->hsdpa_l1_info  = ftm_hsdpa_bler_db.ftm_hsdpa_info;

    FTM_MSG_HIGH("Need to resume HS config after Reconfig success",0,0,0);

    ftm_hsdpa_bler_db.hs_config_to_resume_after_recfg = TRUE;

    return FTM_HSDPA_BLER_HS_FORCE_STOP__NO_ERROR;
}




/*===========================================================================

FUNCTION      FTM_HSDPA_BLER_REQUEST_RESUMET_HS_CHANNEL

DESCRIPTION   Function for outside module to call to request resumption
              of a previously configured HS channel.  Function checks if a
              HS config was previously torn down using 
              ftm_hsdpa_bler_request_stop_hs_channel() before sending
              a start HS command to L1.


DEPENDENCIES  FTM WCDMA BER feature in DCH state.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_hsdpa_bler_request_resume_hs_channel( void)
{

    if( ftm_hsdpa_bler_db.hs_config_to_resume_after_recfg == TRUE)
    {

        FTM_MSG_HIGH("Attmepting to Resume HS config after Reconfig",0,0,0);
        ftm_hsdpa_bler_db.hs_config_to_resume_after_recfg = FALSE;
        (void)ftm_hsdpa_bler_start_hs_channel();
    }
}




/*===========================================================================

FUNCTION       FTM_HSDPA_BLER_RECONFIG_HS_CHANNEL

DESCRIPTION
.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
static boolean ftm_hsdpa_bler_reconfig_hs_channel( void *pkt)
{

    l1_setup_cmd_type                  cmd;
    ftm_hsdpa_bler_reconfig_hs_cmd_type *hs_reconfig;

    hs_reconfig = ( ftm_hsdpa_bler_reconfig_hs_cmd_type *) pkt;
    memset(&cmd, 0, sizeof(l1_setup_cmd_type));

    /* Check if any command in progress before sending another message to WL1 */
    if( ftm_hsdpa_bler_db.isInitialized == FALSE || ftm_hsdpa_bler_db.current_op != HSDPA_SETUP_OPS_NOOP)
    {

        return FALSE;
    }

    if( ftm_wcdma_ber_get_state() != FTM_WCDMA_BER_STATE_DCH)
    {

        return FALSE;
    }

    if( ftm_hsdpa_bler_db.current_state != FTM_HSDPA_BLER_HS_CHAN_STATE_ACTIVE)
    {

        return FALSE;
    }

    /* Make sure req_mask not empty, and make sure no invalid bits are set */

    if( hs_reconfig->req_mask == 0 ||
        ( hs_reconfig->req_mask & ~CPHY_HS_CFG_ALL_INCL) )
    {

        return FALSE;
    }


    ftm_wcdma_ber_init_cphy_setup_cmd( &cmd, FALSE);

    FTM_MSG_MED( "Reconfiguring DCH from DCH state",0,0,0);

	/* Update the CMD parameters*/
    ftm_hsdpa_bler_update_reconfig_hs_channel_parameters(&cmd);

    FTM_MSG_HIGH("Attempting to Reconfig HS Channel with req_mask=0x%x",hs_reconfig->req_mask,0,0);

    ftm_wcdma_ber_send_CPHY_SETUP_REQ( &cmd);

    return TRUE;
}

/*===========================================================================

FUNCTION        FTM_HSDPA_BLER_CONFIG_HS_DPCCH

DESCRIPTION     Configures the HS-DPCCH channel parameters.  Minimal validation
                is done here.  WL1 will completely validate all the params
                during a CPHY_SETUP_REQ.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
static boolean ftm_hsdpa_bler_config_hs_dpcch( void *pkt)
{

    ftm_hsdpa_bler_pkt_type              *cmd_pkt;
    ftm_hsdpa_bler_hs_dpcch_config_type  *hs_config;


    cmd_pkt = ( ftm_hsdpa_bler_pkt_type *) pkt;

    if( ftm_hsdpa_bler_db.isInitialized == FALSE)
    {

        return FALSE;
    }

    hs_config = ( ftm_hsdpa_bler_hs_dpcch_config_type *)  cmd_pkt->data;

    /*
     *  Copy user specified data:
     *     - ACK/NACK Repetition Factor
     *     - CQI Feedback Cycle
     *     - CQI Repetition Factor
     *     - Delta Ack
     *     - Delta Nack
     *     - DSCH power offset relative to CPICH
     */

    #ifdef FEATURE_HSDPA_PERFORMANCE_OPTIMIZATION
     ftm_hsdpa_config_info.hs_dpcch_info.info_present_bitmask =
       HS_DPCCH_BETA_HS_INFO_PRESENT_BITMASK |
       HS_DPCCH_MEAS_FEEDBACK_INFO_PRESENT_BITMASK;
    #endif
    
    HS_DPCCH_GET_MEAS_FEEDBACK_INFO(&(ftm_hsdpa_config_info.hs_dpcch_info), ack_nack_rep_factor) = hs_config->ack_nack_repetition_factor;
    HS_DPCCH_GET_MEAS_FEEDBACK_INFO(&(ftm_hsdpa_config_info.hs_dpcch_info), cqi_feedback_cycle)  = hs_config->cqi_feedback_cycle;
    HS_DPCCH_GET_MEAS_FEEDBACK_INFO(&(ftm_hsdpa_config_info.hs_dpcch_info), cqi_rep_factor)      = hs_config->cqi_repetition_factor;
    HS_DPCCH_GET_BETA_HS_INFO(&(ftm_hsdpa_config_info.hs_dpcch_info), delta_ack)                 = hs_config->delta_ack;
    HS_DPCCH_GET_BETA_HS_INFO(&(ftm_hsdpa_config_info.hs_dpcch_info), delta_cqi)                 = hs_config->delta_cqi;
    HS_DPCCH_GET_BETA_HS_INFO(&(ftm_hsdpa_config_info.hs_dpcch_info), delta_nack)                = hs_config->delta_nack;
    HS_DPCCH_GET_MEAS_FEEDBACK_INFO(&(ftm_hsdpa_config_info.hs_dpcch_info), po_hs_dsch)          = hs_config->cpich_power_offset;


    FTM_MSG_HIGH("deltaACK=%d, deltaNACK=%d, deltaCQI=%d", hs_config->delta_ack,
                                                          hs_config->delta_nack,
                                                          hs_config->delta_cqi);
    FTM_MSG_HIGH("AckNackRepFactor=%d, cqiRepFactor=%d, cqiFeedbackCycle=%d",
                 hs_config->ack_nack_repetition_factor,
                 hs_config->cqi_repetition_factor,
                 hs_config->cqi_feedback_cycle);
    FTM_MSG_HIGH("CpichPowerOffset=%d", hs_config->cpich_power_offset,0,0);


    return TRUE;

}





/*===========================================================================

FUNCTION        FTM_HSDPA_BLER_CONFIG_HS_SCCH

DESCRIPTION     Configures the HS-SCCH channel parameters.  Minimal validation
                is done here.  WL1 will completely validate all the params
                during a CPHY_SETUP_REQ.


DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
static boolean ftm_hsdpa_bler_config_hs_scch( void *pkt)
{

    ftm_hsdpa_bler_pkt_type              *cmd_pkt;
    ftm_hsdpa_bler_hs_scch_config_type   *hs_config;
    uint8                               *codes;
    uint8                               code_index;

    static char                         buffer[80];
    static char                         *b_ptr;


    cmd_pkt = ( ftm_hsdpa_bler_pkt_type *) pkt;

    if( ftm_hsdpa_bler_db.isInitialized == FALSE)
    {
        return FALSE;
    }

    hs_config = ( ftm_hsdpa_bler_hs_scch_config_type *)  cmd_pkt->data;

    codes = ( uint8 *) &hs_config->channelisation_codes[0];

    /* Validate number of channelisation codes */

    if( hs_config->num_channelisation_codes > L1_DL_HS_SCCH_CODES_MAX)
    {

        return FALSE;
    }

    ftm_hsdpa_config_info.hs_scch_info.num_ch_code = hs_config->num_channelisation_codes;

    /* Copy channelisation code data */

    for( code_index = 0; code_index < ftm_hsdpa_config_info.hs_scch_info.num_ch_code;
         code_index++)
    {
        ftm_hsdpa_config_info.hs_scch_info.ch_code[ code_index]  = codes[ code_index];        
    }




    b_ptr = buffer;

    b_ptr += snprintf( b_ptr, strlen("HS-SCCH Codeset = [")+1, "HS-SCCH Codeset = [");

    for( code_index = 0; code_index < ftm_hsdpa_config_info.hs_scch_info.num_ch_code;
         code_index++)
    {
        b_ptr += snprintf( b_ptr, MAX_UINT8_DIGITS+2, " %d,", codes[code_index]);
    }
        
    for( code_index = ftm_hsdpa_config_info.hs_scch_info.num_ch_code; code_index < L1_DL_HS_SCCH_CODES_MAX-1; code_index++)
    {
        
        b_ptr += snprintf( b_ptr, strlen(" X,")+1, " X,");
    }

    for( ; code_index < L1_DL_HS_SCCH_CODES_MAX; code_index++)
    {
        
        b_ptr += snprintf( b_ptr, strlen(" X")+1, " X");
    }

    b_ptr += snprintf( b_ptr, strlen(" ]")+1,  " ]");

    FTM_MSG_LOW("%s",buffer,0,0);


    return TRUE;
}






/*===========================================================================

FUNCTION        FTM_HSDPA_BLER_CONFIG_HS_DSCH

DESCRIPTION     Configures the HS-DSCH channel parameters.  Minimal validation
                is done here.  WL1 will completely validate all the params
                during a CPHY_SETUP_REQ.
.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
static boolean ftm_hsdpa_bler_config_hs_dsch( void *pkt)
{

    ftm_hsdpa_bler_pkt_type              *cmd_pkt;
    ftm_hsdpa_bler_hs_dsch_config_type   *hs_config;
    uint8                               proc_index;
    uint8                               num_proc;


    static char                         buffer[80];
    static char                         *b_ptr;


    cmd_pkt = ( ftm_hsdpa_bler_pkt_type *) pkt;

    if( ftm_hsdpa_bler_db.isInitialized == FALSE)
    {
        return FALSE;
    }

    hs_config = ( ftm_hsdpa_bler_hs_dsch_config_type *)  cmd_pkt->data;


    /* Validate user specified number of HARQ processes */
    num_proc = hs_config->harq_info.num_harq_processes;

    if( num_proc > L1_DL_HS_DSCH_HARQ_MAX_PROC_INTERNAL)
    {

        return FALSE;
    }

    ftm_hsdpa_config_info.hs_dsch_info.cctrch_id                     = 5;  // arbitrary, 5 or 6

    /* Copy user specified H-RNTI */
    ftm_hsdpa_config_info.hs_dsch_info.h_rnti                        = hs_config->h_rnti;

    /* Copy user specified num processes */
    ftm_hsdpa_config_info.hs_dsch_info.harq_info.num_proc            = hs_config->harq_info.num_harq_processes;

    /* Default the mac HS or EHS flag to indicate HS. May want to have it user defined in future */
    ftm_hsdpa_config_info.hs_dsch_info.mac_hs_or_ehs                 = MAC_HS;

    FTM_MSG_HIGH("H-RNTI=0x%x, NumHARQprocs=%d",hs_config->h_rnti,hs_config->harq_info.num_harq_processes,0);

    /* Copy user specified memory partition info valid flag */
    ftm_hsdpa_config_info.hs_dsch_info.harq_info.mem_part_info_valid = hs_config->harq_info.mem_partition_valid;

    /* Copy memory partition information */

    if( ftm_hsdpa_config_info.hs_dsch_info.harq_info.mem_part_info_valid == TRUE)
    {

        for( proc_index = 0; proc_index < num_proc; proc_index++)
        {

            ftm_hsdpa_config_info.hs_dsch_info.harq_info.mem_size_type[ proc_index] = hs_config->harq_info.mem_size[ proc_index];
        }



        b_ptr = buffer;

        b_ptr += snprintf( b_ptr, strlen("HARQ Memory Partition = [")+1, "HARQ Memory Partition = [");

        for( proc_index = 0; proc_index < num_proc; proc_index++)
        {
            b_ptr += snprintf( b_ptr, MAX_LONG_INT_DIGITS+2, " %ld,", HARQ_SZ_ENUM_TO_VAL(hs_config->harq_info.mem_size[ proc_index] ));
        }
            
        for( proc_index = num_proc; proc_index < L1_DL_HS_DSCH_HARQ_MAX_PROC_INTERNAL-1; proc_index++)
        {
            b_ptr += snprintf( b_ptr, strlen(" X,")+1, " X,");
        }

        for( ; proc_index < L1_DL_HS_DSCH_HARQ_MAX_PROC_INTERNAL; proc_index++)
        {
            
            b_ptr += snprintf( b_ptr, strlen(" X")+1, " X");
        }

        b_ptr += snprintf( b_ptr, strlen(" ]")+1, " ]");

        FTM_MSG_LOW("%s",buffer,0,0);



    }

    return TRUE;
}






/*===========================================================================

FUNCTION      FTM_HSDPA_BLER_CONFIG_HS_PDSCH

DESCRIPTION   Configures the HS-PDSCH channel parameters.  Minimal validation
              is done here.  WL1 will completely validate all the params
              during a CPHY_SETUP_REQ.
.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
static boolean ftm_hsdpa_bler_config_hs_pdsch( void *pkt)
{

    ftm_hsdpa_bler_pkt_type               *cmd_pkt;
    ftm_hsdpa_bler_hs_pdsch_config_type   *hs_config;

    cmd_pkt = ( ftm_hsdpa_bler_pkt_type *) pkt;

    if( ftm_hsdpa_bler_db.isInitialized == FALSE)
    {
        return FALSE;
    }

    hs_config = ( ftm_hsdpa_bler_hs_pdsch_config_type *)  cmd_pkt->data;

    /* configure the radio link primary scrambling code */

    ftm_hsdpa_config_info.hs_pdsch_rl_psc = hs_config->hs_pdsch_radio_link_psc;

    FTM_MSG_HIGH("HS-PDSCH RL PSC=%d",hs_config->hs_pdsch_radio_link_psc,0,0);

    return TRUE;
}




/*===========================================================================

FUNCTION      FTM_HSDPA_BLER_CREATE_GEN_RESPONSE

DESCRIPTION
.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
static ftm_rsp_pkt_type ftm_hsdpa_bler_create_gen_response( ftm_hsdpa_bler_pkt_type *cmd_ptr,
                                                    uint16 status,
                                                    uint16 cid,
                                                    uint16 scid)
{

    ftm_rsp_pkt_type  gen_rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    ftm_hsdpa_bler_gen_rsp_pkt_type   *gen_rsp_payload;


    gen_rsp = ftmdiag_create_new_pkt( sizeof( ftm_hsdpa_bler_gen_rsp_pkt_type));

    if( gen_rsp.pkt_payload != NULL)
    {
        gen_rsp_payload = ( ftm_hsdpa_bler_gen_rsp_pkt_type *) gen_rsp.pkt_payload;

        gen_rsp_payload->header                  = cmd_ptr->header;
        gen_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_HSDPA_BLER_GEN_RESPONSE;
        gen_rsp_payload->ftm_rsp_header.rsp_cid  = cid;
        gen_rsp_payload->ftm_rsp_header.rsp_scid = scid;
        gen_rsp_payload->status                  = status;

    }

    return gen_rsp;
}








/*===========================================================================

FUNCTION       FTM_HSDPA_BLER_PROCESS_L1_MESSAGES

DESCRIPTION    Processes messages that L1 would normally send to RRC
               (Confirmations and Indications).  Here, we use these messages
               to update the internal state of the FTM HSDPA processing, and
               send appropriate FTM asyncrhonous log messages.


DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_hsdpa_bler_process_l1_messages( rrc_cmd_type *msg )
{



    switch( msg->cmd_hdr.cmd_id)
    {

/*************************************************************************
*              RRC_CPHY_START_WCDMA_MODE_CNF                             *
*                                                                        *
**************************************************************************/        
        
        
        case RRC_CPHY_START_WCDMA_MODE_CNF:
        {

            ftm_hsdpa_bler_db.current_state = FTM_HSDPA_BLER_HS_CHAN_STATE_INACTIVE;
            ftm_hsdpa_bler_reset_cmd_state();


            ftm_hsdpa_bler_db.hs_config_to_resume_after_recfg = FALSE;
            FTM_MSG_HIGH("Received RRC_CPHY_START_WCDMA_MODE_CNF",0,0,0);

            break;

        }

/*************************************************************************
*              RRC_CPHY_STOP_WCDMA_MODE_CNF                              *
*                                                                        *
**************************************************************************/

        case RRC_CPHY_STOP_WCDMA_MODE_CNF:
        {

            if( ftm_hsdpa_bler_db.current_state == FTM_HSDPA_BLER_HS_CHAN_STATE_ACTIVE)
            {

                ftm_hsdpa_bler_log_gen_cnf( FTM_LOG_HSDPA_HS_CHANNEL_STOP_IND );
            }

            ftm_hsdpa_bler_db.current_state = FTM_HSDPA_BLER_HS_CHAN_STATE_INACTIVE;
            ftm_hsdpa_bler_reset_cmd_state();

            ftm_hsdpa_bler_db.hs_config_to_resume_after_recfg = FALSE;

            FTM_MSG_HIGH("Received RRC_CPHY_STOP_WCDMA_MODE_CNF",0,0,0);


            break;

        }

/*************************************************************************
*              RRC_CPHY_IDLE_CNF                                         *
*                                                                        *
**************************************************************************/

        case RRC_CPHY_IDLE_CNF:
        {

            if( ftm_hsdpa_bler_db.current_state == FTM_HSDPA_BLER_HS_CHAN_STATE_ACTIVE)
            {

                ftm_hsdpa_bler_log_gen_cnf( FTM_LOG_HSDPA_HS_CHANNEL_STOP_IND );
            }


            ftm_hsdpa_bler_db.current_state = FTM_HSDPA_BLER_HS_CHAN_STATE_INACTIVE;
            ftm_hsdpa_bler_reset_cmd_state();
            ftm_hsdpa_bler_db.hs_config_to_resume_after_recfg = FALSE;

            break;
        }

/*************************************************************************
*              RRC_CPHY_SETUP_CNF                                        *
*                                                                        *
**************************************************************************/

        case RRC_CPHY_SETUP_CNF:
        {



            FTM_MSG_HIGH("RRC_CPHY_SETUP_CNF, Status: %d!", msg->cmd.setup_cnf.status, 0, 0);

            switch( ftm_hsdpa_bler_db.current_op)
            {

                case HSDPA_SETUP_OPS_NOOP:
                {
                    /* Confirmation not meant for HSDPA */

                    FTM_MSG_HIGH("Received CPHY_SETUP_CNF: current hs op is NOOP",0,0,0);
                    break;

                }
                case HSDPA_SETUP_OPS_START:
                {

                    if( msg->cmd.setup_cnf.status == TRUE)
                    {

                        ftm_hsdpa_bler_db.current_state = FTM_HSDPA_BLER_HS_CHAN_STATE_ACTIVE;
                        FTM_MSG_HIGH( "Received CPHY_SETUP_CNF: HSDPA Channel Active", 0, 0, 0);

                    }

                    ftm_hsdpa_bler_log_gen_cnf_with_status( FTM_LOG_HSDPA_HS_CHANNEL_START_IND, msg->cmd.setup_cnf.status);

                    break;
                }
                case HSDPA_SETUP_OPS_STOP:
                {

                    if( msg->cmd.setup_cnf.status == TRUE)
                    {

                        ftm_hsdpa_bler_db.current_state = FTM_HSDPA_BLER_HS_CHAN_STATE_INACTIVE;
                        FTM_MSG_HIGH( "Received CPHY_SETUP_CNF: HSDPA Channel Inactive", 0, 0, 0);
                    }


                    ftm_hsdpa_bler_log_gen_cnf_with_status( FTM_LOG_HSDPA_HS_CHANNEL_STOP_IND, msg->cmd.setup_cnf.status );

                    break;
                }
                case HSDPA_SETUP_OPS_RECFG:
                {

                    if( msg->cmd.setup_cnf.status == TRUE)
                    {

                        FTM_MSG_HIGH("Received CPHY_SETUP_CNF: current hs op is RECFG",0,0,0);
                    }

                    ftm_hsdpa_bler_log_gen_cnf_with_status( FTM_LOG_HSDPA_HS_CHANNEL_RECFG_IND, msg->cmd.setup_cnf.status );

                    break;
                }
                default:
                {

                    FTM_MSG_HIGH("Received CPHY_SETUP_CNF: current hs op is UNKNOWN!",0,0,0);

                    break;
                }
            }



            ftm_hsdpa_bler_reset_cmd_state();

            break;
        }

/*************************************************************************
*              RRC_CPHY_PHYCHAN_ESTABLISHED_IND                          *
*                                                                        *
**************************************************************************/

        case RRC_CPHY_PHYCHAN_ESTABLISHED_IND:
        {

            FTM_MSG_HIGH( "Received RRC_CPHY_PHYCHAN_ESTABLISHED_IND with status %d",
                      msg->cmd.phy_est_ind.status,0,0);

            break;
        }

/*************************************************************************
*              RRC_CPHY_RL_FAILURE_IND                                   *
*                                                                        *
**************************************************************************/

        case RRC_CPHY_RL_FAILURE_IND:
        {
            /* Automatic teardown of HSDPA channels by L1 */

            if( ftm_hsdpa_bler_db.current_state == FTM_HSDPA_BLER_HS_CHAN_STATE_ACTIVE)
            {

                ftm_hsdpa_bler_db.current_state = FTM_HSDPA_BLER_HS_CHAN_STATE_INACTIVE;
                ftm_hsdpa_bler_log_gen_cnf_with_status( FTM_LOG_HSDPA_HS_CHANNEL_STOP_IND, TRUE );

            }

            FTM_MSG_HIGH( "Received RRC_CPHY_RL_FAILURE_IND",0,0,0);

            ftm_hsdpa_bler_reset_cmd_state();

            break;
        }

/*************************************************************************
*              RRC_CPHY_ERROR_IND                                        *
*                                                                        *
**************************************************************************/

        case RRC_CPHY_ERROR_IND:
        {

            FTM_MSG_HIGH( "Received RRC_CPHY_ERROR_IND",0,0,0);


            ftm_hsdpa_bler_reset_cmd_state();

            break;
        }
        default:
        {

            FTM_MSG_HIGH( "Received Unknown message from L1: %d", msg->cmd_hdr.cmd_id,0,0);
            
            break;
        }
    }


}







/*===========================================================================

FUNCTION       FTM_HSDPA_BLER_DISPATCH

DESCRIPTION    Main dispatch function for HSDPA related commands (extension
               of FTM WCDMA BER commands)


DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_hsdpa_bler_dispatch( ftm_hsdpa_bler_pkt_type *pkt)
{

    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    ftm_hsdpa_bler_sync_status_type cmd_status = FTM_HSDPA_BLER_SYNC_STATUS_GEN_FAILURE;

    // Variables for synchronous status
    uint32        start_time     = 0;
    int32         time_to_wait   = 0;
    uint8         TimeOutExpired = FALSE;
    rex_sigs_type ftm_sig_clk_rex_wait_return_sig;
    ftm_hsdpa_bler_event_status_req_type *request;

    if( pkt->ftm_cmd_header.cmd_id >= FTM_HSDPA_BLER_CMD_START_HS &&
        pkt->ftm_cmd_header.cmd_id <= FTM_HSDPA_BLER_CMD_CONFIG_HS_DPCCH)
    {
      FTM_MSG_MED( "FTM WCDMA BER dispatch, Command Received", 0,0,0);
    }


    switch( pkt->ftm_cmd_header.cmd_id)
    {

        case FTM_HSDPA_BLER_CMD_START_HS: 

            if ( ftm_hsdpa_bler_start_hs_channel() )
               cmd_status = FTM_HSDPA_BLER_SYNC_STATUS_SUCCESS;

            break;


        case FTM_HSDPA_BLER_CMD_STOP_HS:

            if ( ftm_hsdpa_bler_stop_hs_channel() )
               cmd_status = FTM_HSDPA_BLER_SYNC_STATUS_SUCCESS;
            break;


        case FTM_HSDPA_BLER_CMD_RECONFIG_HS:

            if ( ftm_hsdpa_bler_reconfig_hs_channel( ( void *) pkt) )
               cmd_status = FTM_HSDPA_BLER_SYNC_STATUS_SUCCESS;

            break;


        case FTM_HSDPA_BLER_CMD_CONFIG_HS_PDSCH:

            if ( ftm_hsdpa_bler_config_hs_pdsch( ( void *) pkt) )
               cmd_status = FTM_HSDPA_BLER_SYNC_STATUS_SUCCESS;

            break;


        case FTM_HSDPA_BLER_CMD_CONFIG_HS_SCCH:

            if ( ftm_hsdpa_bler_config_hs_scch( ( void *) pkt) )
               cmd_status = FTM_HSDPA_BLER_SYNC_STATUS_SUCCESS;
            break;

        case FTM_HSDPA_BLER_CMD_CONFIG_HS_DSCH:

            if ( ftm_hsdpa_bler_config_hs_dsch( ( void *) pkt) )
               cmd_status = FTM_HSDPA_BLER_SYNC_STATUS_SUCCESS;

            break;


        case FTM_HSDPA_BLER_CMD_CONFIG_HS_DPCCH:

            if ( ftm_hsdpa_bler_config_hs_dpcch( ( void *) pkt) )
               cmd_status = FTM_HSDPA_BLER_SYNC_STATUS_SUCCESS;
            break;

        case FTM_HSDPA_BLER_EVENT_STATUS_CLEAR_REQ:
             // call the function to clear the status
		     ftm_hsdpa_bler_clear_sync_status();

		     // Setup the response value
		     cmd_status = FTM_HSDPA_BLER_SYNC_STATUS_SUCCESS;

           FTM_MSG_MED( "Clearing synchronous HSDPA events", 0,0,0);
         
           break;

         case FTM_HSDPA_BLER_EVENT_STATUS_REQ:
          // This comand will return the status of events that have occured
          // these events are stored in the HSDPA_BLER_Sync_Status

          request = (ftm_hsdpa_bler_event_status_req_type *) pkt;

          // validate the ID of the event_type
          if ( request->event_type < FTM_HSDPA_BLER_SYNC_STATUS_MAX)
          {
            // Record the start time
            start_time      = timetick_get();

            // Setup timeout loop
            // If the event has already reported then the timing loop will not execute
            while (	(  HSDPA_BLER_Sync_Status[ request->event_type ].has_updated == FALSE )
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
                // Wait on a request status sig or a timeout.
                ftm_sig_clk_rex_wait_return_sig = ftm_sig_clk_rex_wait(FTM_SEQ_STATUS_REQ_SIG,time_to_wait);

                // Check to see if we timed out
                if( (ftm_sig_clk_rex_wait_return_sig & FTM_WAIT_TIMEOUT) == FTM_WAIT_TIMEOUT )
                {
                  TimeOutExpired = TRUE;
                  cmd_status     = FTM_HSDPA_BLER_SYNC_STATUS_TIMEOUT_EXPIRED;
                }
              }
              else
              {
                TimeOutExpired = TRUE;
                cmd_status     = FTM_HSDPA_BLER_SYNC_STATUS_TIMEOUT_EXPIRED;
              } //if( time_to_wait > 0 )

            }// End While timing loop 

            // If message received then use the status from that message type
            if(  HSDPA_BLER_Sync_Status[ request->event_type ].has_updated == TRUE)
            {
              cmd_status = HSDPA_BLER_Sync_Status[ request->event_type ].status;
            }
          }
          else
          {
            // Error, invalid event_type
            cmd_status = FTM_HSDPA_BLER_SYNC_STATUS_GEN_FAILURE;

            FTM_MSG_HIGH( "FTM_HSDPA_BLER_EVENT_STATUS_REQ: Invalid event type %d", request->event_type,0,0);
          }//if ( request->event_type < FTM_HSDPA_BLER_SYNC_STATUS_MAX)

          FTM_MSG_HIGH( "FTM_HSDPA_BLER_EVENT_STATUS_REQ: cmd_fail =%d event_type =%d", cmd_status,request->event_type,0);

            break;

        default:

            FTM_MSG_HIGH( "FTM HSDPA BLER, Unknown command: %d", pkt->ftm_cmd_header.cmd_id, 0, 0);

            break;

    }


    if( cmd_status == FTM_HSDPA_BLER_SYNC_STATUS_GEN_FAILURE )
    {

        FTM_MSG_HIGH( "FTM HSDPA BLER, Command Failure: %d, WCDMA State: %d, HSDPA State: %d", pkt->ftm_cmd_header.cmd_id,
                                                                                     ftm_wcdma_ber_get_state(),
                                                                                     ftm_hsdpa_bler_db.current_state);
                                                                        
    }
    rsp = ftm_hsdpa_bler_create_gen_response( pkt,
                                             (uint16) cmd_status,  
                                             pkt->ftm_cmd_header.cmd_id,
                                             0);

    return( rsp);

}

/*===========================================================================

FUNCTION       ftm_hsdpa_bler_clear_sync_status

DESCRIPTION    This function clears all entries in the synchronous status 
               array.  This behavior is called by the FTM_HSDPA_BLER_EVENT_STATUS_CLEAR_REQ
			   command.

DEPENDENCIES
  none

===========================================================================*/
void ftm_hsdpa_bler_clear_sync_status( void)
{
    uint8 Index;

	// Interate through the array that stores the synchronous status items
    for ( Index = 0; Index < FTM_HSDPA_BLER_SYNC_STATUS_MAX; Index = Index + 1 )
	{
		// Reset the updated flag
		HSDPA_BLER_Sync_Status[ Index ].has_updated = FALSE;

		// Reset the event status 
		HSDPA_BLER_Sync_Status[ Index ].status = FTM_HSDPA_BLER_SYNC_STATUS_GEN_FAILURE;
	}

}

/*===========================================================================

FUNCTION       ftm_hsdpa_bler_update_reconfig_hs_channel_parameters

DESCRIPTION    This function updates L1 CMD parameters

DEPENDENCIES
  none

===========================================================================*/
void ftm_hsdpa_bler_update_reconfig_hs_channel_parameters(l1_setup_cmd_type *cmd)
{
    /* assign here so MAC and L1 both get updated cctrch id */
    ftm_wcdma_ber_assign_cctrch_id();
						  
    /* Send CPHY_SETUP_REQ with Drop (old config) and Add (new config) commands */
						  
    ftm_wcdma_ber_drop_dl_cphy_setup_cmd(cmd);
    ftm_wcdma_ber_drop_ul_cphy_setup_cmd(cmd);

    /* Add the new configuration */

    ftm_wcdma_ber_add_dl_cphy_setup_cmd(cmd);
    ftm_wcdma_ber_add_ul_cphy_setup_cmd(cmd);

     /* Reconfigure  the Call type for HSDPA  */

    cmd->call_type_mask = CPHY_HS_CALL_ACTIVE | CPHY_R99_PS_CALL_ACTIVE;     

    ftm_hsdpa_bler_db.current_op                  = HSDPA_SETUP_OPS_RECFG;
    //	ftm_hsdpa_bler_db.ftm_hsdpa_info->hs_req_mask = hs_reconfig->req_mask;

    cmd->req_mask      |= CPHY_HS_CHAN_CFG_INCL;
    cmd->hsdpa_l1_ops   = ftm_hsdpa_bler_db.current_op;
    cmd->hsdpa_l1_info  = ftm_hsdpa_bler_db.ftm_hsdpa_info;
}


#endif /* FEATURE_HSDPA */
#endif /* FEATURE_FTM_WCDMA_BER */
#endif /* FEATURE_FACTORY_TESTMODE */

