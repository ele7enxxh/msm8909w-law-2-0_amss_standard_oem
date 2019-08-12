
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               1 X  C A L L  P R O C E S S I N G   T E S T  

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/01/10   pxu	   Fixed script to work with current MOB build
12/11/09   grl     Added FW State Config Support
09/30/09   kss     Updated fw register access.
09/16/09   etv     Updated fw register access for interface change.
12/10/08   pmk     Initial Version

===========================================================================*/

#include "hdrsrch_test_acq.h"

void hdrsrch_test_acq::test_hdrsrch_acq( )
{
  int                   i=0,c=0,f=0;
  errno_enum_type       e_retval;

  /* Send the activate command
  */

  /* Send a activate command from MMOC
  */
  TF_MSG(" Activate HDR CP for acquisition ");

  hdrmc_activate_protocol( PROT_ACT_ONLINE_ACQ );

  /* Set parameters for successful acquisition
  */
  for ( c = 0; c < HDRSRCH_MAX_DEMOD_CARRIERS_CNT; c++ )
  {
    for ( f = 0; f < 8 /*HDR_MAX_FINGS_CNT*/; f++ )
    {
      hdrmdsp_shared_mem_interface.read.srch_dump_ind.srchDumpInfo[c].fing[f].antRssiFilt[0] = 0xFFFFFFFF;
      hdrmdsp_shared_mem_interface.read.srch_dump_ind.srchDumpInfo[c].fing[f].antRssiFilt[1] = 0xFFFFFFFF;
      hdrmdsp_shared_mem_interface.read.srch_dump_ind.srchDumpInfo[c].fing[f].pathRSSIFilt = 0xFFFFFFFF;
      hdrmdsp_shared_mem_interface.read.srch_dump_ind.srchDumpInfo[c].fing[f].rtcOfs = 0xFFFFFFFF;
      hdrmdsp_shared_mem_interface.read.srch_dump_ind.srchDumpInfo[c].fing[f].status = 0xFFFFFFFF;
    }
  }

 
  /* Set up DSP registers
  */

  /* The HDR SRCH procdure on getting an acquisition command
  ** HDR SRCH gets a deactivate command, which HDR SRCH ignores
  ** An activate command follows the deactivate command.
  **
  ** hdrsrchacq_init_acq_state() gets called and HDR moves to 
  ** HDRSRCH_ACQ_STATE
  ** In the process of acquisition 
  ** hdrsrchdrv_request_srch4_mdsp_app() gets called, which in turn calls
  **               |
  **              \/
  ** called srch4_request_mdsp_app() gets called this is one more 
  ** interception point intercept the call and send a message. 
  ** 
  ** Interception point 1
  ** In the test set the signal as HDRSRCH does a busy wait on
  ** HDRSRCH_ADHOC_GP_SIG2 
  *

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if 0
  /* Interception point 1
     currently this message will not be sent since srch4_request_mdsp_app is replaced by a
     dummy stub.
     03/01/10, pxu
  */

  qtf_tmc_msgr_receive( (uint8*)&srchMsg, sizeof(msg), &bytes_rcvd );
  TF_ASSERT( msg.hdr_qtf_stub_msg == HDR_QTF_SRCH4_MDSP_APP );
  TF_ASSERT( msg.payload.srch4_mdsp_app.tech_type == SRCH4_HDR_MASK );
  TF_ASSERT( msg.payload.srch4_mdsp_app.task_sig == HDRSRCH_ADHOC_GP_SIG2 );
  TF_ASSERT( msg.payload.srch4_mdsp_app.task_tcb == &hdrsrch_tcb );

  TF_MSG( " Recieved SRCH4 MDSP APP command ");

  TF_YIELD();

  if ( msg.payload.srch4_mdsp_app.task_tcb != NULL )
  {
    /* Set the task signal to unblock it */
    (void) rex_set_sigs( msg.payload.srch4_mdsp_app.task_tcb, 
                         msg.payload.srch4_mdsp_app.task_sig );
  }

#endif
  /* After getting the signal, hdrsrchacq_init gets called. 
  ** Eventually hdrsrchutil_roll gets called which sets the HDRSRCH_ROLL_SIG.
  ** On getting the Roll sig, 
  ** hdrsrchutil_handle_pn_roll gets called which calls
  **            |
  **           \/
  ** hdrsrchlog_pn_roll_logging gets called.
  ** hdrsrchacq_lock_rf calls 
  **            |
  **           \/
  ** trm_request_and_notify which grants rf by calling
  **            |
  **           \/
  ** hdrsrchrf_grant_callback, 
  **            |
  **           \/
  ** hdrsrch_rf_grant_cb sets the HDRSRCH_PRI_CHAIN_EVENT_SIG.
  **
  ** hdrsrchacq_pri_chain_granted gets called as part of the signal handling
  ** HDR acq moves to HDRSRCHACQ_S1_WAITING_FOR_TCXO_OPEN_LOOP. As part of the 
  ** state change hdrmdsp_enable_hdr_app gets called and does an inline wait
  **
  ** Interception point 2:
  ** mdsp_enable sends a message to test task.
  ** 
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  wait_for_msg( HDR_FW_STATE_CFG_CMD );

  /* Send HDR State Config response */
  msgr_init_hdr( &hdr_state_cfg_rsp_msg.hdr, MSGR_HDR_FW, HDR_FW_STATE_CFG_RSP);
  init_hdr_state_cfg_rsp_msg();
  hdr_state_cfg_rsp_msg.state = HDRFW_STATE_ACTIVE;
  e_retval = msgr_send( &hdr_state_cfg_rsp_msg.hdr, sizeof( hdrfw_state_cfg_rsp_msg_t  ) );
  TF_ASSERT( e_retval                 == E_SUCCESS );


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* after the wait hdrsrchafc_request_open_loop gets called which sends 
  ** HDRSRCHAFC_OPEN_LOOP_CMD. The acq state is 
  ** HDRSRCHACQ_S1_WAITING_FOR_TCXO_OPEN_LOOP, which changes to
  ** HDRSRCHACQ_S1_IDLE. 
  ** hdrsrchacq_start_acq gets called
  **            |
  **           \/
  ** hdrsrchacq_init_on_rf_lock gets called,
  **            |
  **           \/
  ** hdrsrchfing_diversity_init gets called
  **            |
  **           \/
  ** hdrsrchfing_diversity_init gets called 
  **            |
  **           \/
  ** hdrsrchfing_update_diversity gets called
  **            |
  **           \/
  ** hdrsrchfing_program_fings gets called
  **
  ** This sends a finger assignment message to HDR FW
  **
  ** Interception point 3:
  ** Trap the finger assignment message, and send a response back
  */

  /* finger cmd msg no logner gets sent if it's not modified*/
#if 0
  /* Interception point 3
  */
  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  TF_YIELD();
  TF_SLEEP(0);

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_ACQ_STATE );

  /* Send a rsponse back to the HDR fing assignment request
  */
  msgr_init_hdr( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP );
  e_retval = msgr_send( &hdrfw_msg_rsp.hdr, sizeof( hdrfw_msg_rsp_u ) );
  TF_ASSERT( e_retval                 == E_SUCCESS );

#endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* hdrsrchdrv_enable_srch4 gets called which calls
  ** srch4_hdr_register
  **
  ** Interception point 4:
  ** Trap the call to srch4_hdr_register
  **
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  wait_for_qtf_stub_msg( &srch4Msg, HDR_QTF_SRCH4_HDR_REGISTER );
  TF_ASSERT( srch4Msg.payload.hdr_register.priority == SRCH4_PRIO_HDR_REACQ_PAGING );

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_ACQ_STATE );


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* hdrsrchacq_start_timer gets called
  ** hdrsrchacq_start_acq calls hdrsrchrf_set_agc_acq_params and 
  ** hdrsrchrf_tune_to_chan sets the tune done call back to hdrsrchacq_tune
  ** and calls the hdrsrchrf_start_tune which calls sample_server_set_chain_mode
  ** This could be one more interception point.
  **
  ** hdrsrchafc_set_chan gets called which posts a HDRSRCHAFC_SET_CHAN_CMD
  ** which is handled in hdrsrchafc_ol_set_chan_tf
  ** HDRSRCHRF_TUNE_DELAY_US, which is 0 seconds is set
  */

  /* Force the timer to fire by calling TF_SLEEP
  */
  TF_SLEEP(1);
  TF_YIELD();
  /* HDRSRCH_TUNE_SIG gets set after the timer fires
  ** hdrsrchrf_tune_sig gets called
  ** HDRSRCHRF_TUNING_IN_PROGRESS is the RF state
  ** hdrsrchrf_settle_start gets called
  ** rfm_set_agc_mode could be another interception point, but it is called
  ** from many places
  */
  /* hdrsrchrf_settle_start() starts a timer. 11662 uS, exipre the timer
  */

  TF_SLEEP(12);
  TF_YIELD();
  TF_SLEEP(12);
  TF_YIELD();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* After getting the signal hdrsrchrf_tune_sig gets called
  ** state is HDRSRCHRF_LOOPS_SETTLING hdrsrchrf_tune_done gets called
  ** tune_done_func => hdrsrchacq_tune gets called
  */
  /*Step 1: 
  **
  **HDR SRCH calls hdrsrch_acq_tune() which in turn calls 
  **                        |
  **                       \/
  **          hdrsrchacq_start_predetect which in turn calls
  **                        |
  **                       \/
  **           hdrsrchacq_submit_predetect_search which calls
  **                        |
  **                       \/ 
  **              hdrsrchdrv_send_srch_list
  **
  ** hdrsrchdrv_send_srch_list populates the SRCH4 data structure. 
  ** srch4_hdr_add_task is stubbed out.
  ** srch4_hdr_start_search is stubbed out. This could be the point of QTF 
  ** interception, test can wait for this to be called. And then call
  ** the dump call back
  **
  ** Currently the stub populates an sucessful acq parameters and calls the 
  ** dump call back hdrsrchdrv_dump_cb
  **
  **- - - - - - - - - - - - - - - - - - - -  - - - -  - - - - - - - - - - -**
  **
  ** Step2
  **
  ** Dump call back calls hdrsrchacq_dump which calls 
  **                                 |
  **                                \/
  **                      hdrsrchacq_predetect_dump in turn
  **                                 |
  **                                \/ 
  **                      hdrsrchacq_start_detect gets called.
  **
  ** Eventually hdrsrchdrv_send_srch_list gets called. As noted above, this
  ** could be a point of interception, eventually hdrsrchdrv_dump_cb gets 
  ** called
  ** 
  **- - - - - - - - - - - - - - - - - - - -  - - - -  - - - - - - - - - - -**
  **
  ** Step 3
  **
  ** Dump call back calls hdrsrchacq_dump which calls 
  **                                 |
  **                                \/
  **                      hdrsrchacq_detect_dump in turn
  **                                 |
  **                                \/ 
  **                      hdrsrchacq_start_anti_alias gets called.
  **                                 |
  **                                \/ 
  **                      hdrsrchacq_submit_anti_alias_search gets called
  **                                 |
  **                                \/
  **                      eventually hdrsrchacq_submit_anti_alias_search
  ** 
  ** State moves to HDRSRCHACQ_S1_DETECT this inturn calls 
  ** hdrsrchacq_submit_anti_alias_search. Eventually hdrsrchdrv_send_srch_list 
  ** gets called and the dump call back is called.
  **
  ** State now moves to HDRSRCHACQ_S1_ANTI_ALIAS. 
  ** hdrsrchacq_submit_anti_alias_search gets called again
  ** In the dump callback state is still HDRSRCHACQ_S1_ANTI_ALIAS
  **
  ** An internal command to close the loop is posted hdrsrchafc_ol_to_cl_tf 
  ** is called HDRSRCH_AFC_CLOSE_LOOP_CMD is posted. 
  ** In the command handling the state is HDRSRCHACQ_S2_WAITING_FOR_TCXO_CLOSE_LOOP. 
  **
  ** State changes to HDRSRCHACQ_S2_IDLE and HDR begins pull in.
  **
  ** As part of pull-in HDR SRCH programs the fingers and starts a 
  ** 0x384 us inline wait. 
  ** 
  ** After the wait HDR commits the fingers to hw by calling program fings. 
  ** Adds HDRSRCHACQ_S2_POLL_SIG to wait signal and posts 
  ** HDRSRCHAFC_START_ACQ_CMD. hdrsrchafc_ol_start_acq_tf gets called. 
  ** State changes to HDRSRCHAFC_V_ACQ_STATE hdrsrchafc_enter_v_acq is called. 
  ** HDRSRCHAFC_PULLIN_POLL_TIME (10ms)is started.
  **
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* wait for the 4 search messages from: hdrsrch_predetect_dump, 
     hdrsrch_detect_dump, hdrsrch_anti_alias_dump and submit_anti_alias_dump
  */

  for(int i = 0; i < 4; i++ )
  {
    process_srch_msg(&srch4Msg);
    TF_ASSERT(hdrsrch_state_id == HDRSRCH_ACQ_STATE );
  }
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* HDR SRCH programs the fingers
  */

  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_ACQ_STATE );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* In HDRSRCH_AFC_CLOSE_LOOP_CMD handling, hdrsrchacq_start_pullin gets 
  ** called, which programs the fingers
  ** If the fingers are programmed, HDR waits on the HDRSRCHACQ_S2_POLL_SIG 
  ** hdrsrchafc_start_fll_acq gets called which posts HDRSRCHAFC_START_ACQ_CMD
  ** The HDR acq state moves to HDRSRCHACQ_S2_PULLIN
  ** As part of the command handling, hdrsrchafc_ol_start_acq_tf gets called and
  ** hrd srch afc moved to HDRSRCHAFC_V_ACQ_STATE hdrsrchafc_enter_v_acq 
  ** is the entry function HDRSRCHAFC_PULLIN_POLL_TIME is started 10 ms
  ** hdrsrchafc_v_acq_timer_expire_tf gets called pullin status is HDRSRCHAFC_PULLIN_SUCCEEDED
  ** HDRSRCH_AFC_ACQ_SUCCEEDED_CMD is queued, the entry function is
  ** hdrsrchafc_enter_rv_trk, which calls hdrsrchafc_start_rvafc_tracking
  ** HDRSRCHAFC_RVAFC_POLL_TIME timer (27ms) is started.
  ** Contril comes back to HDRSRCH_AFC_ACQ_SUCCEEDED_CMD. hdrsrchacq_start_slam is called
  ** PRogram fingers is called and HDRSRCHACQ_S2_POLL_TIMER timer is started
  ** and HDRSRCH waits for 5ms. acq state moves to HDRSRCHACQ_S2_SLAM

  */

  TF_YIELD();
     
  /* Set up DSP registers
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Send a rsponse back to the HDR fing assignment request
  */
  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP, sizeof( hdrfw_msg_rsp_u ));

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_ACQ_STATE );

  /* Wait for afc acq start msg */
  wait_for_msg( HDR_FW_FLL_START_ACQ_CMD );
  
  /* lock the fingers via shared memory interface */
  hdrmdsp_shared_mem_interface.read.general_status.C[0].fingStatus[0] = 0xFFFFFFFF;
  hdrmdsp_shared_mem_interface.read.general_status.C[0].fingStatus[1] = 0xFFFFFFFF;
}

void hdrsrch_test_acq::test_acq_success( )
{
  test_hdrsrch_acq( );

  /* config the fll locked flag */
	hdrfw_msg_rsp.fll_acq.fllLocked = 0xFFFFFFFF;
  /* Send back afc acq response */
  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FLL_ACQ_RSP, sizeof( hdrfw_msg_rsp_u ));

  TF_YIELD();

  /* Set up DSP registers
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  TF_SLEEP(1);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_ACQ_STATE );

  /* wait for 1 more predetect message */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  process_srch_msg(&srch4Msg);

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_ACQ_STATE );


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set up DSP registers
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   /* Send a rsponse back to the HDR fing assignment request
  */
  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP, sizeof( hdrfw_msg_rsp_u ));

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_ACQ_STATE );

  /* Wait for afc acq start msg */
  wait_for_msg( HDR_FW_MSTR_SLAM_CMD );
  
  /* Send back afc acq response */
  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_SLAM_RSP, sizeof( hdrfw_msg_rsp_u ));

	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Expire the timer */
  TF_SLEEP(10);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  TF_YIELD();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Expire 5 ms timer */
  TF_SLEEP(5);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* After the 5 ms timer expiry, HDRSRCH_GP_TMR0_SIG is set 
   ** this calls hdrsrchacq_s2_poll which calls hdrsrchacq_slam_poll
   ** HDRIND_LMAC_ACQUIRED is sent and hdrsrchsync_init_sync_state is entered
   ** which moves the acq state to HDRSRCH_SYNC_STATE 
   ** In the sync state init, HDRSRCH_SYNC_TIMEOUT_MS (5200ms) is set and HDR
   ** waits on HDRSRCH_TIMEOUT_SIG and HDRSRCH_SCHEDULE_SIG
   ** the sequence noted above is followed schedule timer of 30ms is set
  */
  TF_ASSERT(hdrsrch_state_id == HDRSRCH_SYNC_STATE );
}
