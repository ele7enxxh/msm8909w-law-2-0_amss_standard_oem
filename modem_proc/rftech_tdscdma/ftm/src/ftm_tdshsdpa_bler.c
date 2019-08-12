/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2011 by Qualcomm Technologies INCORPORATED.  
  Copyright (c) 2012 by Qualcomm Technologies INCORPORATED.  
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/ftm/src/ftm_tdshsdpa_bler.c#1 $
  $DateTime: 2016/12/13 07:59:50 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/02/13   ms      KW fix
05/23/12   hy      Added support for TDSCDMA HSDPA Non-signaling
===========================================================================*/


/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"

#include "rfa_variation.h"
#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"

#ifdef FTM_HAS_TDSCDMA_BER
#ifdef FTM_HAS_TDS_HSDPA

#include <stdio.h>
#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "rf_test_task_v.h"
#include "ftmdiag.h"
#include "queue.h"
#include "tdsrrccmd.h"
#include "ftm_log.h"
//#include "ftm_umts_ber.h"
#include "ftm_tdscdma_ber.h"
#include "ftm_tdshsdpa_bler.h"
#include "tdsl1rrcif.h"
//#include "wl1api.h"


/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/

static tdsl1_hs_info_struct_type   ftm_tdshsdpa_config_info;
static ftm_tdshsdpa_bler_db_type       ftm_tdshsdpa_bler_db;

/* array to recrod the events that will be reported synchronously via FTM_TDSHSDPA_BLER_EVENT_STATUS_REQ*/
ftm_tdshsdpa_bler_sync_status_struct HSDPA_BLER_Sync_Status[  FTM_TDSHSDPA_BLER_SYNC_STATUS_MAX ];

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

static void    ftm_tdshsdpa_bler_reset_cmd_state( void);

static boolean ftm_tdshsdpa_bler_start_hs_channel( void);
static boolean ftm_tdshsdpa_bler_stop_hs_channel( void);
static boolean ftm_tdshsdpa_bler_reconfig_hs_channel( void *pkt);
static boolean ftm_tdshsdpa_bler_config_hs_scch( void *pkt);
static boolean ftm_tdshsdpa_bler_config_hs_dsch( void *pkt);
static boolean ftm_tdshsdpa_bler_config_hs_pdsch( void *pkt);
static boolean ftm_tdshsdpa_bler_fill_config_db(void);
void ftm_tdshsdpa_bler_clear_sync_status( void);

//Long INT 4 bytes -2147483648 to 2147483647
#define FTM_TDSCDMA_MAX_LONG_INT_DIGITS 11
//UINT8 1 byte 0 ~ 256
#define FTM_TDSCDMA_MAX_UINT8_DIGITS    3

/*===========================================================================

FUNCTION     FTM_TDSHSDPA_BLER_LOG_GEN_CNF

DESCRIPTION
             Logs a generic confirmation message to the user (PC side).  This
             is used when the indication is that a command was completed,
             but no payload needs to be sent.

DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_tdshsdpa_bler_log_gen_cnf( ftm_log_tdshsdpa_bler_id_type  id)
{

#ifdef FTM_HAS_LOGGING

    ftm_log_tdshsdpa_bler_gen_cnf        *gen_cnf_log;


    gen_cnf_log = ( ftm_log_tdshsdpa_bler_gen_cnf *)
                    ftm_log_malloc( (uint16)FTM_LOG_TDSCDMA_BER,
                                    sizeof( ftm_log_tdshsdpa_bler_gen_cnf) );

    if( gen_cnf_log != NULL)
    {
        gen_cnf_log->log_id = id;

        log_commit( gen_cnf_log);
    }
#endif

}

/*===========================================================================

FUNCTION     FTM_TDSHSDPA_BLER_LOG_GEN_CNF_WITH_STATUS

DESCRIPTION
             Logs a generic confirmation message to the user (PC side).  This
             is used when the indication is that a command was completed, and
             pass a generic status byte to the user.

DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_tdshsdpa_bler_log_gen_cnf_with_status( ftm_log_tdshsdpa_bler_id_type  id, uint8 status)
{
    ftm_tdshsdpa_bler_sync_status_type sync_status = FTM_TDSHSDPA_BLER_SYNC_STATUS_GEN_FAILURE;

#ifdef FTM_HAS_LOGGING
    ftm_log_tdshsdpa_bler_gen_cnf_with_status        *gen_cnf_log;

    gen_cnf_log = ( ftm_log_tdshsdpa_bler_gen_cnf_with_status *)
                    ftm_log_malloc( (uint16)FTM_LOG_TDSCDMA_BER,
                                    sizeof( ftm_log_tdshsdpa_bler_gen_cnf_with_status) );

    if( gen_cnf_log != NULL)
    {
        gen_cnf_log->log_id = id;
        gen_cnf_log->status = status;

        log_commit( gen_cnf_log);
    }
#endif
    // Fill out the entry for the synchronous status message
    if ( status )
      sync_status = FTM_TDSHSDPA_BLER_SYNC_STATUS_SUCCESS;
    else
      sync_status = FTM_TDSHSDPA_BLER_SYNC_STATUS_EVENT_STATUS_FAILED;

    switch ( id )
    {
      case FTM_LOG_TDSHSDPA_HS_CHANNEL_START_IND:
      {

        HSDPA_BLER_Sync_Status[ FTM_TDSHSDPA_BLER_HS_CHANNEL_START_SYNC_STATUS ].status = sync_status;
        HSDPA_BLER_Sync_Status[ FTM_TDSHSDPA_BLER_HS_CHANNEL_START_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

        break;
      }
      case FTM_LOG_TDSHSDPA_HS_CHANNEL_STOP_IND:
      {
        HSDPA_BLER_Sync_Status[ FTM_TDSHSDPA_BLER_HS_CHANNEL_STOP_SYNC_STATUS ].status = sync_status;
        HSDPA_BLER_Sync_Status[ FTM_TDSHSDPA_BLER_HS_CHANNEL_STOP_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

        break;
      }
      case FTM_LOG_TDSHSDPA_HS_CHANNEL_RECFG_IND:
      {
      
        HSDPA_BLER_Sync_Status[ FTM_TDSHSDPA_BLER_HS_CHANNEL_RECFG_SYNC_STATUS ].status = sync_status;
        HSDPA_BLER_Sync_Status[ FTM_TDSHSDPA_BLER_HS_CHANNEL_RECFG_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );
        break;
      }
    }

}

/*===========================================================================

FUNCTION        FTM_TDSHSDPA_BLER_INIT

DESCRIPTION
.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdshsdpa_bler_init( void)
{

    ftm_tdshsdpa_bler_db.isInitialized  = TRUE;
    ftm_tdshsdpa_bler_db.ftm_tdshsdpa_info = &ftm_tdshsdpa_config_info;
    ftm_tdshsdpa_bler_db.current_op     = TDSL1_HSDPA_SETUP_OPS_NOOP;

    /* Initialize some default MAC-d info...not sure how this should be handled
     * currently
     */

    ftm_tdshsdpa_config_info.hs_dsch_info.macd_info.num_mac_hs_q = 1;

    ftm_tdshsdpa_config_info.hs_dsch_info.macd_info.mac_hs_q_info[0].mac_hs_q_id      = 0;
    ftm_tdshsdpa_config_info.hs_dsch_info.macd_info.mac_hs_q_info[0].num_mac_d_pdu_sz = 1;

    ftm_tdshsdpa_config_info.hs_dsch_info.macd_info.mac_hs_q_info[0].mac_d_pdu_sz_info[0].pdu_sz_idx = 0;
    ftm_tdshsdpa_config_info.hs_dsch_info.macd_info.mac_hs_q_info[0].mac_d_pdu_sz_info[0].pdu_sz     = 336;


    ftm_tdshsdpa_bler_db.hs_config_to_resume_after_recfg = FALSE;

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
static void ftm_tdshsdpa_bler_reset_cmd_state( void)
{

    ftm_tdshsdpa_bler_db.current_op = TDSL1_HSDPA_SETUP_OPS_NOOP;

}





/*===========================================================================

FUNCTION        FTM_TDSHSDPA_BLER_START_HS_CHANNEL

DESCRIPTION     Commands WL1 to begin processing of an HSDPA channel.


DEPENDENCIES    All HS channels have been configured.


RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
static boolean ftm_tdshsdpa_bler_start_hs_channel( void)
{

    tdsl1_setup_cmd_type                  cmd;


    if( ftm_tdshsdpa_bler_db.isInitialized == FALSE || ftm_tdshsdpa_bler_db.current_op != TDSL1_HSDPA_SETUP_OPS_NOOP)
    {
        FTM_MSG_ERROR("ftm_tdshsdpa_bler_start_hs_channel: unitialized state",0,0,0);

        return FALSE;
    }


    if( ftm_tdscdma_ber_get_state() != FTM_TDSCDMA_BER_STATE_DCH)
    {
        FTM_MSG_ERROR("ftm_tdshsdpa_bler_start_hs_channel: BER isn't in DCH",0,0,0);

        return FALSE;
    }

    /* Make sure hs channel is inactive before starting */

    if( ftm_tdshsdpa_bler_db.current_state != FTM_TDSHSDPA_BLER_HS_CHAN_STATE_INACTIVE)
    {    
        FTM_MSG_ERROR("ftm_tdshsdpa_bler_start_hs_channel: BLER isn't inactive",0,0,0);
        return FALSE;
    }

    
    /* Not currently sending a MAC config along with HS config, so don't set these signals */
    
    ftm_tdscdma_ber_init_cphy_setup_cmd( &cmd, FALSE);

    ftm_tdshsdpa_bler_db.current_op                  = TDSL1_HSDPA_SETUP_OPS_START;
    ftm_tdshsdpa_bler_db.ftm_tdshsdpa_info->hs_req_mask = TDSL1_CPHY_HS_CFG_ALL_INCL; // 0x07
    ftm_tdshsdpa_bler_fill_config_db();

    cmd.call_type_mask = TDSL1_CPHY_VOICE_CALL_ACTIVE | TDSL1_CPHY_HS_CALL_ACTIVE;   /* R4 + HS */
    cmd.req_mask      = TDSL1_CPHY_HS_CHAN_CFG_INCL;
    cmd.hsdpa_l1_ops  = ftm_tdshsdpa_bler_db.current_op;
    cmd.hsdpa_l1_info = ftm_tdshsdpa_bler_db.ftm_tdshsdpa_info;


    FTM_MSG_HIGH("Attempting to Start HS channel",0,0,0);

    ftm_tdscdma_ber_send_CPHY_SETUP_REQ( &cmd);

    return TRUE;
}







/*===========================================================================

FUNCTION      FTM_TDSHSDPA_BLER_STOP_HS_CHANNEL

DESCRIPTION   Stops L1 processing of HS signal


DEPENDENCIES  HSDPA was active

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
static boolean ftm_tdshsdpa_bler_stop_hs_channel( void)
{

    tdsl1_setup_cmd_type                  cmd;

    /* Check if any command in progress before sending another message to WL1 */

    if( ftm_tdshsdpa_bler_db.isInitialized == FALSE || ftm_tdshsdpa_bler_db.current_op != TDSL1_HSDPA_SETUP_OPS_NOOP)
    {
        FTM_MSG_ERROR("ftm_tdshsdpa_bler_start_hs_channel: unitialized state",0,0,0);
 
        return FALSE;
    }

    /* Make sure hs channel active before stopping */

    if( ftm_tdshsdpa_bler_db.current_state != FTM_TDSHSDPA_BLER_HS_CHAN_STATE_ACTIVE)
    {
        FTM_MSG_ERROR("ftm_tdshsdpa_bler_start_hs_channel: BLER isn't active",0,0,0);

        return FALSE;
    }


    ftm_tdscdma_ber_init_cphy_setup_cmd( &cmd, FALSE);

    ftm_tdshsdpa_bler_db.current_op                  = TDSL1_HSDPA_SETUP_OPS_STOP;
    ftm_tdshsdpa_bler_db.ftm_tdshsdpa_info->hs_req_mask = 0;

    cmd.req_mask      = TDSL1_CPHY_HS_CHAN_CFG_INCL;
    cmd.hsdpa_l1_ops  = ftm_tdshsdpa_bler_db.current_op;
    cmd.hsdpa_l1_info = ftm_tdshsdpa_bler_db.ftm_tdshsdpa_info;

    FTM_MSG_HIGH("Attempting to Stop HS Channel",0,0,0);

    ftm_tdscdma_ber_send_CPHY_SETUP_REQ( &cmd);

    return TRUE;
}



/*===========================================================================

FUNCTION      FTM_TDSHSDPA_BLER_REQUEST_STOP_HS_CHANNEL

DESCRIPTION   Function for outside module to call to request stop of HS channel.
              This will check if a HS channel is already active, and add the 
              HS stop command to the tdsl1_setup_cmd_type buffer in progress.


DEPENDENCIES  FTM TDSCDMA BER feature DCH should be active

RETURN VALUE
              Error code for operation

SIDE EFFECTS
   None.

===========================================================================*/
ftm_tdshsdpa_bler_hs_force_stop_error_type ftm_tdshsdpa_bler_request_stop_hs_channel( tdsl1_setup_cmd_type *cmd)
{


    /* Check if any command in progress before sending another message to WL1 */

    if( ftm_tdshsdpa_bler_db.isInitialized == FALSE || ftm_tdshsdpa_bler_db.current_op != TDSL1_HSDPA_SETUP_OPS_NOOP)
    {

        return FTM_TDSHSDPA_BLER_HS_FORCE_STOP__ERROR_OP_IN_PROGRESS;
    }

    /* Make sure hs channel active before stopping */

    if( ftm_tdshsdpa_bler_db.current_state != FTM_TDSHSDPA_BLER_HS_CHAN_STATE_ACTIVE)
    {

        return FTM_TDSHSDPA_BLER_HS_FORCE_STOP__ERROR_HS_INACTIVE;
    }

    ftm_tdshsdpa_bler_db.current_op                  = TDSL1_HSDPA_SETUP_OPS_STOP;
    ftm_tdshsdpa_bler_db.ftm_tdshsdpa_info->hs_req_mask = 0;

    cmd->req_mask      |= TDSL1_CPHY_HS_CHAN_CFG_INCL;
    cmd->hsdpa_l1_ops   = ftm_tdshsdpa_bler_db.current_op;
    cmd->hsdpa_l1_info  = ftm_tdshsdpa_bler_db.ftm_tdshsdpa_info;

    FTM_MSG_LOW("Need to resume HS config after Reconfig success",0,0,0);

    ftm_tdshsdpa_bler_db.hs_config_to_resume_after_recfg = TRUE;

    return FTM_TDSHSDPA_BLER_HS_FORCE_STOP__NO_ERROR;
}




/*===========================================================================

FUNCTION      FTM_TDSHSDPA_BLER_REQUEST_RESUMET_HS_CHANNEL

DESCRIPTION   Function for outside module to call to request resumption
              of a previously configured HS channel.  Function checks if a
              HS config was previously torn down using 
              ftm_tdshsdpa_bler_request_stop_hs_channel() before sending
              a start HS command to L1.


DEPENDENCIES  FTM TDSCDMA BER feature in DCH state.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdshsdpa_bler_request_resume_hs_channel( void)
{

    if( ftm_tdshsdpa_bler_db.hs_config_to_resume_after_recfg == TRUE)
    {

        FTM_MSG_HIGH("Attmepting to Resume HS config after Reconfig",0,0,0);
        ftm_tdshsdpa_bler_db.hs_config_to_resume_after_recfg = FALSE;
        (void)ftm_tdshsdpa_bler_start_hs_channel();
    }
}




/*===========================================================================

FUNCTION       FTM_TDSHSDPA_BLER_RECONFIG_HS_CHANNEL

DESCRIPTION
.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
static boolean ftm_tdshsdpa_bler_reconfig_hs_channel( void *pkt)
{

    tdsl1_setup_cmd_type                  cmd;
    ftm_tdshsdpa_bler_reconfig_hs_cmd_type *hs_reconfig;

    hs_reconfig = ( ftm_tdshsdpa_bler_reconfig_hs_cmd_type *) pkt;


    /* Check if any command in progress before sending another message to WL1 */
    if( ftm_tdshsdpa_bler_db.isInitialized == FALSE || ftm_tdshsdpa_bler_db.current_op != TDSL1_HSDPA_SETUP_OPS_NOOP)
    {

        return FALSE;
    }

    if( ftm_tdscdma_ber_get_state() != FTM_TDSCDMA_BER_STATE_DCH)
    {

        return FALSE;
    }

    if( ftm_tdshsdpa_bler_db.current_state != FTM_TDSHSDPA_BLER_HS_CHAN_STATE_ACTIVE)
    {

        return FALSE;
    }

    /* Make sure req_mask not empty, and make sure no invalid bits are set */

    if( hs_reconfig->req_mask == 0 ||
        ( hs_reconfig->req_mask & ~TDSL1_CPHY_HS_CFG_ALL_INCL) )
    {

        return FALSE;
    }


    ftm_tdscdma_ber_init_cphy_setup_cmd( &cmd, FALSE);

    ftm_tdshsdpa_bler_db.current_op                  = TDSL1_HSDPA_SETUP_OPS_RECFG;
    ftm_tdshsdpa_bler_db.ftm_tdshsdpa_info->hs_req_mask = hs_reconfig->req_mask;


    cmd.req_mask      = TDSL1_CPHY_HS_CHAN_CFG_INCL;
    cmd.hsdpa_l1_ops  = ftm_tdshsdpa_bler_db.current_op;
    cmd.hsdpa_l1_info = ftm_tdshsdpa_bler_db.ftm_tdshsdpa_info;

    FTM_MSG_HIGH("Attempting to Reconfig HS Channel with req_mask=0x%x",hs_reconfig->req_mask,0,0);

    ftm_tdscdma_ber_send_CPHY_SETUP_REQ( &cmd);

    return TRUE;
}

/*===========================================================================

FUNCTION        FTM_TDSHSDPA_BLER_CONFIG_HS_SCCH

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
static boolean ftm_tdshsdpa_bler_config_hs_scch( void *pkt)
{
#if 0

    ftm_tdshsdpa_bler_pkt_type              *cmd_pkt;
    ftm_tdshsdpa_bler_hs_scch_config_type   *hs_config;
    uint8                               *codes;
    uint8                               code_index;

    static char                         buffer[80];
    static char                         *b_ptr;

    cmd_pkt = ( ftm_tdshsdpa_bler_pkt_type *) pkt;

    if( ftm_tdshsdpa_bler_db.isInitialized == FALSE)
    {
        return FALSE;
    }

    hs_config = ( ftm_tdshsdpa_bler_hs_scch_config_type *)  cmd_pkt->data;

    codes = ( uint8 *) &hs_config->channelisation_codes[0];

    /* Validate number of channelisation codes */

    if( hs_config->num_channelisation_codes > TDS_MAX_HS_SCCH_SICH_COUNT)
    {

        return FALSE;
    }

    ftm_tdshsdpa_config_info.hs_scchset_info.num_hs_scch_sich = hs_config->num_channelisation_codes;

    /* Copy channelisation code data */

    for( code_index = 0; code_index < ftm_tdshsdpa_config_info.hs_scchset_info.num_hs_scch_sich;
         code_index++)
    {
        //hyang todo: copy the struct??
        //ftm_tdshsdpa_config_info.hs_scchset_info.ch_code[ code_index]  = codes[ code_index];        
    }

    b_ptr = buffer;

    b_ptr += snprintf( b_ptr, strlen("HS-SCCH Codeset = [")+1, "HS-SCCH Codeset = [");

    for( code_index = 0; code_index < ftm_tdshsdpa_config_info.hs_scchset_info.num_hs_scch_sich;
         code_index++)
    {
        b_ptr += snprintf( b_ptr, FTM_TDSCDMA_MAX_UINT8_DIGITS+2, " %d,", codes[code_index]);
    }
        
    for( code_index = ftm_tdshsdpa_config_info.hs_scchset_info.num_hs_scch_sich; code_index < TDS_MAX_HS_SCCH_SICH_COUNT-1; code_index++)
    {
        
        b_ptr += snprintf( b_ptr, strlen(" X,")+1, " X,");
    }

    for( ; code_index < TDS_MAX_HS_SCCH_SICH_COUNT; code_index++)
    {
        
        b_ptr += snprintf( b_ptr, strlen(" X")+1, " X");
    }

    b_ptr += snprintf( b_ptr, strlen(" ]")+1,  " ]");

    FTM_MSG_LOW(buffer,0,0,0);

#endif

    return TRUE;
}






/*===========================================================================

FUNCTION        FTM_TDSHSDPA_BLER_CONFIG_HS_DSCH

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
static boolean ftm_tdshsdpa_bler_config_hs_dsch( void *pkt)
{
#if 0
    ftm_tdshsdpa_bler_pkt_type              *cmd_pkt;
    ftm_tdshsdpa_bler_hs_dsch_config_type   *hs_config;
    uint8                               proc_index;
    uint8                               num_proc;


    static char                         buffer[80];
    static char                         *b_ptr;


    cmd_pkt = ( ftm_tdshsdpa_bler_pkt_type *) pkt;

    if( ftm_tdshsdpa_bler_db.isInitialized == FALSE)
    {
        return FALSE;
    }

    hs_config = ( ftm_tdshsdpa_bler_hs_dsch_config_type *)  cmd_pkt->data;

    /* Validate user specified number of HARQ processes */
    num_proc = hs_config->harq_info.num_harq_processes;

    if( num_proc > TDSL1_DL_CCTRCHCFG_HS_DSCH_HARQ_MAX_PROC)
    {

        return FALSE;
    }

    ftm_tdshsdpa_config_info.hs_dsch_info.cctrch_id                     = 5;  // arbitrary, 5 or 6

    /* Copy user specified H-RNTI */
    ftm_tdshsdpa_config_info.hs_dsch_info.h_rnti                        = hs_config->h_rnti;

    /* Copy user specified num processes */
    ftm_tdshsdpa_config_info.hs_dsch_info.harq_info.num_proc            = hs_config->harq_info.num_harq_processes;

    /* Default the mac HS or EHS flag to indicate HS. May want to have it user defined in future */
    //ftm_tdshsdpa_config_info.hs_dsch_info.mac_hs_or_ehs                 = MAC_HS;

    FTM_MSG_LOW("H-RNTI=0x%x, NumHARQprocs=%d",hs_config->h_rnti,hs_config->harq_info.num_harq_processes,0);

    /* Copy user specified memory partition info valid flag */
    ftm_tdshsdpa_config_info.hs_dsch_info.harq_info.mem_part_info_valid = hs_config->harq_info.mem_partition_valid;

    /* Copy memory partition information */

    if( ftm_tdshsdpa_config_info.hs_dsch_info.harq_info.mem_part_info_valid == TRUE)
    {

        for( proc_index = 0; proc_index < num_proc; proc_index++)
        {

            ftm_tdshsdpa_config_info.hs_dsch_info.harq_info.mem_size_type[ proc_index] = hs_config->harq_info.mem_size[ proc_index];
        }



        b_ptr = buffer;

        b_ptr += snprintf( b_ptr, strlen("HARQ Memory Partition = [")+1, "HARQ Memory Partition = [");

        for( proc_index = 0; proc_index < num_proc; proc_index++)
        {
            b_ptr += snprintf( b_ptr, FTM_TDSCDMA_MAX_LONG_INT_DIGITS+2, " %ld,", TDSL1DLCCTRCHCFG_HARQ_SZ_ENUM_TO_VAL(hs_config->harq_info.mem_size[ proc_index] ));
        }
            
        for( proc_index = num_proc; proc_index < TDSL1_DL_CCTRCHCFG_HS_DSCH_HARQ_MAX_PROC-1; proc_index++)
        {
            b_ptr += snprintf( b_ptr, strlen(" X,")+1, " X,");
        }

        for( ; proc_index < TDSL1_DL_CCTRCHCFG_HS_DSCH_HARQ_MAX_PROC; proc_index++)
        {
            
            b_ptr += snprintf( b_ptr, strlen(" X")+1, " X");
        }

        b_ptr += snprintf( b_ptr, strlen(" ]")+1, " ]");

        //FTM_MSG_LOW(buffer,0,0,0);
    }
#endif
    return TRUE;
}






/*===========================================================================

FUNCTION      FTM_TDSHSDPA_BLER_CONFIG_HS_PDSCH

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
static boolean ftm_tdshsdpa_bler_config_hs_pdsch( void *pkt)
{

    ftm_tdshsdpa_bler_pkt_type               *cmd_pkt;
    ftm_tdshsdpa_bler_hs_pdsch_config_type   *hs_config;

    cmd_pkt = ( ftm_tdshsdpa_bler_pkt_type *) pkt;

    if( ftm_tdshsdpa_bler_db.isInitialized == FALSE)
    {
        return FALSE;
    }

    hs_config = ( ftm_tdshsdpa_bler_hs_pdsch_config_type *)  cmd_pkt->data;

    /* configure the radio link primary scrambling code */

//hyang todo:     ftm_tdshsdpa_config_info.hs_pdsch_rl_psc = hs_config->hs_pdsch_radio_link_psc;

    FTM_MSG_LOW("HS-PDSCH RL PSC=%d",hs_config->hs_pdsch_radio_link_psc,0,0);

    return TRUE;
}




/*===========================================================================

FUNCTION      FTM_TDSHSDPA_BLER_CREATE_GEN_RESPONSE

DESCRIPTION
.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
static ftm_rsp_pkt_type ftm_tdshsdpa_bler_create_gen_response( ftm_tdshsdpa_bler_pkt_type *cmd_ptr,
                                                    uint16 status,
                                                    uint16 cid,
                                                    uint16 scid)
{

    ftm_rsp_pkt_type  gen_rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    ftm_tdshsdpa_bler_gen_rsp_pkt_type   *gen_rsp_payload;


    gen_rsp = ftmdiag_create_new_pkt( sizeof( ftm_tdshsdpa_bler_gen_rsp_pkt_type));

    if( gen_rsp.pkt_payload != NULL)
    {
        gen_rsp_payload = ( ftm_tdshsdpa_bler_gen_rsp_pkt_type *) gen_rsp.pkt_payload;

        gen_rsp_payload->header                  = cmd_ptr->header;
        gen_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_TDSHSDPA_BLER_GEN_RESPONSE;
        gen_rsp_payload->ftm_rsp_header.rsp_cid  = cid;
        gen_rsp_payload->ftm_rsp_header.rsp_scid = scid;
        gen_rsp_payload->status                  = status;

    }

    return gen_rsp;
}








/*===========================================================================

FUNCTION       FTM_TDSHSDPA_BLER_PROCESS_L1_MESSAGES

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
void ftm_tdshsdpa_bler_process_l1_messages( tdsrrc_cmd_type *msg )
{



    switch( (tdsrrc_l1_cmd_e_type)(msg->cmd_hdr.cmd_id))
    {

/*************************************************************************
*              RRC_CPHY_START_TDSCDMA_MODE_CNF                             *
*                                                                        *
**************************************************************************/        
        
        
        case RRC_CPHY_START_TDS_MODE_CNF:
        {

            ftm_tdshsdpa_bler_db.current_state = FTM_TDSHSDPA_BLER_HS_CHAN_STATE_INACTIVE;
            ftm_tdshsdpa_bler_reset_cmd_state();


            ftm_tdshsdpa_bler_db.hs_config_to_resume_after_recfg = FALSE;
            FTM_MSG_HIGH("Received RRC_CPHY_START_TDSCDMA_MODE_CNF",0,0,0);

            break;

        }

/*************************************************************************
*              RRC_CPHY_STOP_TDSCDMA_MODE_CNF                              *
*                                                                        *
**************************************************************************/

        case RRC_CPHY_STOP_TDS_MODE_CNF:
        {

            if( ftm_tdshsdpa_bler_db.current_state == FTM_TDSHSDPA_BLER_HS_CHAN_STATE_ACTIVE)
            {

                ftm_tdshsdpa_bler_log_gen_cnf( FTM_LOG_TDSHSDPA_HS_CHANNEL_STOP_IND );
            }

            ftm_tdshsdpa_bler_db.current_state = FTM_TDSHSDPA_BLER_HS_CHAN_STATE_INACTIVE;
            ftm_tdshsdpa_bler_reset_cmd_state();

            ftm_tdshsdpa_bler_db.hs_config_to_resume_after_recfg = FALSE;

            FTM_MSG_HIGH("Received RRC_CPHY_STOP_TDSCDMA_MODE_CNF",0,0,0);


            break;

        }

/*************************************************************************
*              RRC_CPHY_IDLE_CNF                                         *
*                                                                        *
**************************************************************************/

        case TDSRRC_CPHY_IDLE_CNF:
        {

            if( ftm_tdshsdpa_bler_db.current_state == FTM_TDSHSDPA_BLER_HS_CHAN_STATE_ACTIVE)
            {

                ftm_tdshsdpa_bler_log_gen_cnf( FTM_LOG_TDSHSDPA_HS_CHANNEL_STOP_IND );
            }


            ftm_tdshsdpa_bler_db.current_state = FTM_TDSHSDPA_BLER_HS_CHAN_STATE_INACTIVE;
            ftm_tdshsdpa_bler_reset_cmd_state();
            ftm_tdshsdpa_bler_db.hs_config_to_resume_after_recfg = FALSE;

            break;
        }

/*************************************************************************
*              RRC_CPHY_SETUP_CNF                                        *
*                                                                        *
**************************************************************************/

        case TDSRRC_CPHY_SETUP_CNF:
        {



            FTM_MSG_HIGH("RRC_CPHY_SETUP_CNF, Status: %d!", msg->cmd.setup_cnf.status, 0, 0);

            switch( ftm_tdshsdpa_bler_db.current_op)
            {

                case TDSL1_HSDPA_SETUP_OPS_NOOP:
                {
                    /* Confirmation not meant for HSDPA */

                    FTM_MSG_HIGH("Received CPHY_SETUP_CNF: current hs op is NOOP",0,0,0);
                    break;

                }
                case TDSL1_HSDPA_SETUP_OPS_START:
                {

                    if( msg->cmd.setup_cnf.status == TRUE)
                    {

                        ftm_tdshsdpa_bler_db.current_state = FTM_TDSHSDPA_BLER_HS_CHAN_STATE_ACTIVE;
                        FTM_MSG_HIGH( "Received CPHY_SETUP_CNF: HSDPA Channel Active", 0, 0, 0);

                    }

                    ftm_tdshsdpa_bler_log_gen_cnf_with_status( FTM_LOG_TDSHSDPA_HS_CHANNEL_START_IND, msg->cmd.setup_cnf.status);

                    break;
                }
                case TDSL1_HSDPA_SETUP_OPS_STOP:
                {

                    if( msg->cmd.setup_cnf.status == TRUE)
                    {

                        ftm_tdshsdpa_bler_db.current_state = FTM_TDSHSDPA_BLER_HS_CHAN_STATE_INACTIVE;
                        FTM_MSG_HIGH( "Received CPHY_SETUP_CNF: HSDPA Channel Inactive", 0, 0, 0);
                    }


                    ftm_tdshsdpa_bler_log_gen_cnf_with_status( FTM_LOG_TDSHSDPA_HS_CHANNEL_STOP_IND, msg->cmd.setup_cnf.status );

                    break;
                }
                case TDSL1_HSDPA_SETUP_OPS_RECFG:
                {

                    if( msg->cmd.setup_cnf.status == TRUE)
                    {

                        FTM_MSG_HIGH("Received CPHY_SETUP_CNF: current hs op is RECFG",0,0,0);
                    }

                    ftm_tdshsdpa_bler_log_gen_cnf_with_status( FTM_LOG_TDSHSDPA_HS_CHANNEL_RECFG_IND, msg->cmd.setup_cnf.status );

                    break;
                }
                default:
                {

                    FTM_MSG_HIGH("Received CPHY_SETUP_CNF: current hs op is UNKNOWN!",0,0,0);

                    break;
                }
            }



            ftm_tdshsdpa_bler_reset_cmd_state();

            break;
        }

/*************************************************************************
*              RRC_CPHY_PHYCHAN_ESTABLISHED_IND                          *
*                                                                        *
**************************************************************************/

        case TDSRRC_CPHY_PHYCHAN_ESTABLISHED_IND:
        {

            FTM_MSG_HIGH( "Received RRC_CPHY_PHYCHAN_ESTABLISHED_IND with status %d",
                      msg->cmd.phy_est_ind.status,0,0);

            break;
        }

/*************************************************************************
*              RRC_CPHY_RL_FAILURE_IND                                   *
*                                                                        *
**************************************************************************/

        case TDSRRC_CPHY_RL_FAILURE_IND:
        {
            /* Automatic teardown of HSDPA channels by L1 */

            if( ftm_tdshsdpa_bler_db.current_state == FTM_TDSHSDPA_BLER_HS_CHAN_STATE_ACTIVE)
            {

                ftm_tdshsdpa_bler_db.current_state = FTM_TDSHSDPA_BLER_HS_CHAN_STATE_INACTIVE;
                ftm_tdshsdpa_bler_log_gen_cnf_with_status( FTM_LOG_TDSHSDPA_HS_CHANNEL_STOP_IND, TRUE );

            }

            FTM_MSG_HIGH( "Received RRC_CPHY_RL_FAILURE_IND",0,0,0);

            ftm_tdshsdpa_bler_reset_cmd_state();

            break;
        }

/*************************************************************************
*              RRC_CPHY_ERROR_IND                                        *
*                                                                        *
**************************************************************************/

        case TDSRRC_CPHY_ERROR_IND:
        {

            FTM_MSG_HIGH( "Received RRC_CPHY_ERROR_IND",0,0,0);


            ftm_tdshsdpa_bler_reset_cmd_state();

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

FUNCTION       FTM_TDSHSDPA_BLER_DISPATCH

DESCRIPTION    Main dispatch function for HSDPA related commands (extension
               of FTM TDSCDMA BER commands)


DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_tdshsdpa_bler_dispatch( ftm_tdshsdpa_bler_pkt_type *pkt)
{

    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    ftm_tdshsdpa_bler_sync_status_type cmd_status = FTM_TDSHSDPA_BLER_SYNC_STATUS_GEN_FAILURE;

    // Variables for synchronous status
    uint32        start_time     = 0;
    int32         time_to_wait   = 0;
    uint8         TimeOutExpired = FALSE;
    rex_sigs_type ftm_sig_clk_rex_wait_return_sig;
    ftm_tdshsdpa_bler_event_status_req_type *request;

    FTM_MSG_HIGH( "FTM TDSHSDPA BLER dispatch, Received cmd id: %d, curent BER state: %d, HSDPA state: %d", 
      pkt->ftm_cmd_header.cmd_id,ftm_tdscdma_ber_get_state(),ftm_tdshsdpa_bler_db.current_state);


    switch( pkt->ftm_cmd_header.cmd_id)
    {

        case FTM_TDSHSDPA_BLER_CMD_START_HS: 

            if ( ftm_tdshsdpa_bler_start_hs_channel() )
               cmd_status = FTM_TDSHSDPA_BLER_SYNC_STATUS_SUCCESS;

            break;


        case FTM_TDSHSDPA_BLER_CMD_STOP_HS:

            if ( ftm_tdshsdpa_bler_stop_hs_channel() )
               cmd_status = FTM_TDSHSDPA_BLER_SYNC_STATUS_SUCCESS;
            break;


        case FTM_TDSHSDPA_BLER_CMD_RECONFIG_HS:

            if ( ftm_tdshsdpa_bler_reconfig_hs_channel( ( void *) pkt) )
               cmd_status = FTM_TDSHSDPA_BLER_SYNC_STATUS_SUCCESS;

            break;

        case FTM_TDSHSDPA_BLER_CMD_CONFIG_HS_SCCH:

            if ( ftm_tdshsdpa_bler_config_hs_scch( ( void *) pkt) )
               cmd_status = FTM_TDSHSDPA_BLER_SYNC_STATUS_SUCCESS;
            break;

        case FTM_TDSHSDPA_BLER_CMD_CONFIG_HS_DSCH:

            if ( ftm_tdshsdpa_bler_config_hs_dsch( ( void *) pkt) )
               cmd_status = FTM_TDSHSDPA_BLER_SYNC_STATUS_SUCCESS;

            break;


        case FTM_TDSHSDPA_BLER_CMD_CONFIG_HS_PDSCH:

            if ( ftm_tdshsdpa_bler_config_hs_pdsch( ( void *) pkt) )
               cmd_status = FTM_TDSHSDPA_BLER_SYNC_STATUS_SUCCESS;

            break;


        case FTM_TDSHSDPA_BLER_EVENT_STATUS_CLEAR_REQ:
             // call the function to clear the status
		     ftm_tdshsdpa_bler_clear_sync_status();

		     // Setup the response value
		     cmd_status = FTM_TDSHSDPA_BLER_SYNC_STATUS_SUCCESS;

           FTM_MSG_MED( "Clearing synchronous HSDPA events", 0,0,0);
         
           break;

         case FTM_TDSHSDPA_BLER_EVENT_STATUS_REQ:
          // This comand will return the status of events that have occured
          // these events are stored in the HSDPA_BLER_Sync_Status

          request = (ftm_tdshsdpa_bler_event_status_req_type *) pkt;

          // validate the ID of the event_type
          if ( request->event_type < FTM_TDSHSDPA_BLER_SYNC_STATUS_MAX)
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
                  cmd_status     = FTM_TDSHSDPA_BLER_SYNC_STATUS_TIMEOUT_EXPIRED;
                }
              }
              else
              {
                TimeOutExpired = TRUE;
                cmd_status     = FTM_TDSHSDPA_BLER_SYNC_STATUS_TIMEOUT_EXPIRED;
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
            cmd_status = FTM_TDSHSDPA_BLER_SYNC_STATUS_GEN_FAILURE;

            FTM_MSG_HIGH( "FTM_TDSHSDPA_BLER_EVENT_STATUS_REQ: Invalid event type %d", request->event_type,0,0);
          }//if ( request->event_type < FTM_TDSHSDPA_BLER_SYNC_STATUS_MAX)

          FTM_MSG_LOW( "FTM_TDSHSDPA_BLER_EVENT_STATUS_REQ: cmd_fail =%d event_type =%d", cmd_status,request->event_type,0);

            break;

        default:

            FTM_MSG_HIGH( "FTM HSDPA BLER, Unknown command: %d", pkt->ftm_cmd_header.cmd_id, 0, 0);

            break;

    }


    if( cmd_status == FTM_TDSHSDPA_BLER_SYNC_STATUS_GEN_FAILURE )
    {

        FTM_MSG_HIGH( "FTM HSDPA BLER, Command Failure: %d, TDSCDMA State: %d, HSDPA State: %d", pkt->ftm_cmd_header.cmd_id,
                                                                                     ftm_tdscdma_ber_get_state(),
                                                                                     ftm_tdshsdpa_bler_db.current_state);
                                                                        
    }
    rsp = ftm_tdshsdpa_bler_create_gen_response( pkt,
                                             (uint16) cmd_status,  
                                             pkt->ftm_cmd_header.cmd_id,
                                             0);

    return( rsp);

}

/*===========================================================================

FUNCTION       ftm_tdshsdpa_bler_clear_sync_status

DESCRIPTION    This function clears all entries in the synchronous status 
               array.  This behavior is called by the FTM_TDSHSDPA_BLER_EVENT_STATUS_CLEAR_REQ
			   command.

DEPENDENCIES
  none

===========================================================================*/
void ftm_tdshsdpa_bler_clear_sync_status( void)
{
    uint8 Index;

	// Interate through the array that stores the synchronous status items
    for ( Index = 0; Index < FTM_TDSHSDPA_BLER_SYNC_STATUS_MAX; Index = Index + 1 )
	{
		// Reset the updated flag
		HSDPA_BLER_Sync_Status[ Index ].has_updated = FALSE;

		// Reset the event status 
		HSDPA_BLER_Sync_Status[ Index ].status = FTM_TDSHSDPA_BLER_SYNC_STATUS_GEN_FAILURE;
	}

}

/*===========================================================================

FUNCTION       ftm_tdshsdpa_bler_fill_config_db

DESCRIPTION    This function sets up TDS HSDPA configuration db

DEPENDENCIES
  none

===========================================================================*/
boolean ftm_tdshsdpa_bler_fill_config_db(void)
{
  uint8 Index;

  ftm_tdshsdpa_config_info.hs_cat = 0x0F;
  ftm_tdshsdpa_config_info.hs_req_mask = 0x07;
  ftm_tdshsdpa_config_info.common_scch_sich_params_present = FALSE;


  /* HS-DSCH */
  ftm_tdshsdpa_config_info.hs_dsch_info.cctrch_id = 4;
  ftm_tdshsdpa_config_info.hs_dsch_info.h_rnti = 0x0;
  ftm_tdshsdpa_config_info.hs_dsch_info.harq_info.num_proc = 4;
  //ftm_tdshsdpa_config_info.hs_dsch_info.mac_hs_or_ehs = MAC_HS;
  ftm_tdshsdpa_config_info.hs_dsch_info.harq_info.mem_part_info_valid = TRUE;

  if( ftm_tdshsdpa_config_info.hs_dsch_info.harq_info.mem_part_info_valid == FALSE)
  {
    for( Index = 0; Index < ftm_tdshsdpa_config_info.hs_dsch_info.harq_info.num_proc; Index++)
    {
      //ftm_tdshsdpa_config_info.hs_dsch_info.harq_info.mem_size_type[Index] = TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS32000;
    }
  }
  else
  {
     ftm_tdshsdpa_config_info.hs_dsch_info.harq_info.mem_size_type[0] = TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS13600;
     ftm_tdshsdpa_config_info.hs_dsch_info.harq_info.mem_size_type[1] = TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS13600;
     ftm_tdshsdpa_config_info.hs_dsch_info.harq_info.mem_size_type[2] = TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS13600;
     ftm_tdshsdpa_config_info.hs_dsch_info.harq_info.mem_size_type[3] = TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS13600;
  }

  ftm_tdshsdpa_config_info.hs_dsch_info.hsdsch_tb_size_alignment_type = TDSL1_DL_CCTRCHCFG_HS_DSCH_TB_TABLE_OCTET_UNALIGNED;

  ftm_tdshsdpa_config_info.hs_dsch_info.macd_info.num_mac_hs_q = 1;     // MAC-D
  ftm_tdshsdpa_config_info.hs_dsch_info.macd_info.mac_hs_q_info[0].mac_hs_q_id = 0;
  ftm_tdshsdpa_config_info.hs_dsch_info.macd_info.mac_hs_q_info[0].num_mac_d_pdu_sz = 1;
  ftm_tdshsdpa_config_info.hs_dsch_info.macd_info.mac_hs_q_info[0].mac_d_pdu_sz_info[0].pdu_sz_idx = 0;
  ftm_tdshsdpa_config_info.hs_dsch_info.macd_info.mac_hs_q_info[0].mac_d_pdu_sz_info[0].pdu_sz = 336;

  /* HS-SCCH set */

  ftm_tdshsdpa_config_info.hs_scchset_info.num_hs_scch_sich = TDS_MAX_HS_SCCH_SICH_COUNT;

  //ftm_tdshsdpa_config_info.ack_nack_powoffset = 0;
  //ftm_tdshsdpa_config_info.prx_sich = -100;
  //ftm_tdshsdpa_config_info.tpc_ssize = TDSL1_TPC_STEP_SIZE_1DB;
  //ftm_tdshsdpa_config_info.pow_control_gap = 32;
  //ftm_tdshsdpa_config_info.do_pathloss_compens = 1;

  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[0].ts_num_scch = 0;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[0].scch_chcode_1st = 4;               // HS-SCCH
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[0].scch_chcode_2nd = 5;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[0].hs_scch_midamble_cfg.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[0].hs_scch_midamble_cfg.config = TDSL1_MIDAMBLE_CONFIG_4;
  //ftm_tdshsdpa_config_info.l1_dl_hs_scch_sich_config[0].hs_scch_midamble_cfg.shift = 255;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[0].bler_target = -60;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[0].l1_sich_config.ts_num_sich = 1;     // HS-SICH
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[0].l1_sich_config.sich_chcode = 10;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[0].l1_sich_config.midamble_config_sich.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[0].l1_sich_config.midamble_config_sich.config = TDSL1_MIDAMBLE_CONFIG_8;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[0].l1_sich_config.ack_nack_powoffset = 0;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[0].l1_sich_config.prx_sich = -112;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[0].l1_sich_config.tpc_ssize = TDSL1_TPC_STEP_SIZE_1DB;

  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[1].ts_num_scch = 0;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[1].scch_chcode_1st = 6;               // HS-SCCH
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[1].scch_chcode_2nd = 7;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[1].hs_scch_midamble_cfg.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[1].hs_scch_midamble_cfg.config = TDSL1_MIDAMBLE_CONFIG_4;
  //ftm_tdshsdpa_config_info.l1_dl_hs_scch_sich_config[1].hs_scch_midamble_cfg.shift = 255;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[1].bler_target = -60;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[1].l1_sich_config.ts_num_sich = 1;     // HS-SICH
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[1].l1_sich_config.sich_chcode = 11;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[1].l1_sich_config.midamble_config_sich.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[1].l1_sich_config.midamble_config_sich.config = TDSL1_MIDAMBLE_CONFIG_8;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[1].l1_sich_config.ack_nack_powoffset = 0;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[1].l1_sich_config.prx_sich = -112;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[1].l1_sich_config.tpc_ssize = TDSL1_TPC_STEP_SIZE_1DB;

  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[2].ts_num_scch = 0;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[2].scch_chcode_1st = 8;               // HS-SCCH
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[2].scch_chcode_2nd = 9;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[2].hs_scch_midamble_cfg.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[2].hs_scch_midamble_cfg.config = TDSL1_MIDAMBLE_CONFIG_4;
  //ftm_tdshsdpa_config_info.l1_dl_hs_scch_sich_config[2].hs_scch_midamble_cfg.shift = 255;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[2].bler_target = -60;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[2].l1_sich_config.ts_num_sich = 1;     // HS-SICH
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[2].l1_sich_config.sich_chcode = 12;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[2].l1_sich_config.midamble_config_sich.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[2].l1_sich_config.midamble_config_sich.config = TDSL1_MIDAMBLE_CONFIG_8;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[2].l1_sich_config.ack_nack_powoffset = 0;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[2].l1_sich_config.prx_sich = -112;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[2].l1_sich_config.tpc_ssize = TDSL1_TPC_STEP_SIZE_1DB;

  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[3].ts_num_scch = 0;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[3].scch_chcode_1st = 10;               // HS-SCCH
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[3].scch_chcode_2nd = 11;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[3].hs_scch_midamble_cfg.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[3].hs_scch_midamble_cfg.config = TDSL1_MIDAMBLE_CONFIG_4;
  //ftm_tdshsdpa_config_info.l1_dl_hs_scch_sich_config[3].hs_scch_midamble_cfg.shift = 255;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[3].bler_target = -60;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[3].l1_sich_config.ts_num_sich = 1;     // HS-SICH
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[3].l1_sich_config.sich_chcode = 13;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[3].l1_sich_config.midamble_config_sich.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[3].l1_sich_config.midamble_config_sich.config = TDSL1_MIDAMBLE_CONFIG_8;  
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[3].l1_sich_config.ack_nack_powoffset = 0;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[3].l1_sich_config.prx_sich = -112;
  ftm_tdshsdpa_config_info.hs_scchset_info.l1_dl_hs_scch_sich_config[3].l1_sich_config.tpc_ssize = TDSL1_TPC_STEP_SIZE_1DB;

  /* HS-PDSCH */
  ftm_tdshsdpa_config_info.hs_pdsch_midamble_cfg.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
  ftm_tdshsdpa_config_info.hs_pdsch_midamble_cfg.config = TDSL1_MIDAMBLE_CONFIG_8;
  ftm_tdshsdpa_config_info.hs_pdsch_midamble_cfg.shift = 255;
  //ftm_tdshsdpa_config_info.hs_pdsch_midamble_cfg.shift = 255;

  return TRUE;

}

/*===========================================================================

FUNCTION       ftm_tdshsdpa_bler_result_update

DESCRIPTION    This function is called by TDSL1 to update HSDPA decoding status

DEPENDENCIES
  none

===========================================================================*/
void ftm_tdshsdpa_bler_result_update(uint32 txRcv, 
                                     uint32 newtxRcv, 
                                     uint32 txCRCpass, 
                                     uint32 newtxCRCpass,
                                     uint16 bitRate/* in Kbps*/)
{
  FTM_MSG_HIGH("TDS HSDPA data packet received: %d, CRC passed: %d",txRcv,txCRCpass,0);
  FTM_MSG_HIGH("TDS HSDPA data packet (new TX) received: %d, CRC passed: %d",newtxRcv,newtxCRCpass,0);

  return;
}
#endif /* FEATURE_TDSHSDPA */
#endif /* FTM_HAS_TDSCDMA_BER */
#endif /* FEATURE_FACTORY_TESTMODE */

