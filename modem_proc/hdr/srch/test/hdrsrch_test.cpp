
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               1 X  C A L L  P R O C E S S I N G   T E S T  

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2008 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/test/hdrsrch_test.cpp#1 $ $Author: mplcsds1 $ $DateTime: 2016/12/13 07:58:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/01/10   pxu     Updated script for compatibility/enhanced functionaility
12/11/09   grl     Added FW state config support
11/04/09   etv     Included mcs_time.h
11/03/09   etv     Updated for changes to timer API
08/12/09   wsh     Fixed compile error on genesis.2h09 MOB
08/05/09   lyl     Updated for decoder deactivation.
07/16/09   lyl     Supported CC config MSGR interface.
06/25/09   wsh     Replace mcs_efs_closedir with efs_closedir
12/10/08   pmk     Initial Version

===========================================================================*/

#include "hdr_variation.h"
#include "hdrsrch_test.h"

extern "C" {
#include "mcs_time.h"
#include "task.h"
}

/* UMID registered in MSGR */
static msgr_umid_type regsitered_umids[] =  
  {
    /* HDR State Config */
    HDR_FW_STATE_CFG_CMD,

    /* AFC */
    HDR_FW_FLL_START_ACQ_CMD,
    HDR_FW_FLL_ACCUM_LOAD_CMD,
    HDR_FW_DLL_CFG_CMD,
    HDR_FW_FLL_GAIN_CFG_CMD,
    HDR_FW_FLL_SLEW_CFG_CMD,
    HDR_FW_PLL_CFG_CMD,
    HDR_FW_FLL_ROT_CFG_CMD,
    HDR_FW_FLL_ROT_OVERRIDE_CMD,
    HDR_FW_FLL_PDM_OVERRIDE_CMD,
    HDR_FW_FLL_ENABLE_CMD,

    /* Drv */
    HDR_FW_HSTR_OFFSET_CFG_CMD,
    HDR_FW_MSTR_SLAM_CMD,
    HDR_FW_SRCH_SHUTDOWN_CMD,
    HDR_FW_HYPERSPACE_CFG_CMD,

    /* Fing */
    HDR_FW_FING_ASSIGN_CMD,
    HDR_FW_FING_LOCK_THRESH_CMD,

    /* Misc msg */
    HDR_FW_RXAGC_UNLOCKED_MODE_CMD,
    HDR_FW_MIN_BEST_ASP_SINR_CMD,

    HDR_FW_INDICATION_CFG_CMD,

    HDR_FW_FPD_CMD,
    HDR_FW_QPCH_CMD,
    HDR_FW_DIVERSITY_CFG_CMD,
    HDR_FW_FORCE_COMB_MODE_CMD,

    /* ASP */
    HDR_FW_ASP_UPDATE_CMD,

    /* Handoff */
    HDR_FW_HANDOFF_CFG_CMD,
    HDR_FW_HANDOFF_OVERRIDE_CMD,

    /* DRC */
    HDR_FW_DRC_CFG_CMD,
    HDR_FW_DRC_FILT_RESET_CMD,

    /* EQ */
    HDR_FW_EQ_CFG_CMD,
    HDR_FW_EQ_OVERRIDE_CMD,

    /* Tuneaway */
    HDR_FW_TUNEAWAY_START_CMD,
    HDR_FW_TUNEAWAY_STOP_CMD,

    /* Sleep */
    HDR_FW_MPC_CFG_CMD,
    HDR_FW_SLEEP_ADJ_CMD,

    /* Dec */
    HDR_FW_DECOB_IND_RATE_CMD,
    HDR_FW_DECOB_UPDATE_CMD,
    HDR_FW_DECODE_CTL_CMD,
    HDR_FW_MIN_PREAM_THRESH_CFG_CMD,

    /* MAC */
    HDR_FW_CC_DEMOD_CFG_CMD,
    HDR_FW_FMAC_HANDOFF_CFG_CMD,
    HDR_FW_SCC_CMD,
    
    HDR_FW_RESET_SEQ_NUM_CMD,

	/* LTE to HDR */
	HDR_FW_PILOT_MEAS_CFG_RSP,
	HDR_SRCH_LTE_PILOT_MEAS_RSP,

    /* FWS */
    MCS_FWS_APP_CONFIG_CMD
  };

void hdrsrch_test::init_hdr_state_cfg_rsp_msg( void )
{

  /* Set up the payload for hdr enable reply msg */
  hdr_state_cfg_rsp_msg.hdrVersion = 0;
  hdr_state_cfg_rsp_msg.sharedMemoryAddr  = (uint32) ( &hdrmdsp_shared_mem_interface );

}

void hdrsrch_test::wait_for_msg( msgr_umid_type id )
{
  errno_enum_type        e_retval;
  uint32                 bytes_rcvd;

  do {

      e_retval = msgr_receive( &m_test_client, (uint8*)&hdrsrch_msg, sizeof(hdrfw_msg_u), &bytes_rcvd );
      TF_ASSERT( e_retval == E_SUCCESS );
      TF_MSG("QTF received msg %s umid 0x%8x from src 0x%4x\n",
             msgr_get_umid_name(hdrsrch_msg.hdr.id),
             hdrsrch_msg.hdr.id, 
             hdrsrch_msg.hdr.src );

  } while (  hdrsrch_msg.hdr.id         != id  );

  TF_ASSERT( e_retval                               == E_SUCCESS );
  TF_ASSERT( hdrsrch_msg.hdr.id         == id );
  TF_ASSERT( hdrsrch_msg.hdr.num_attach == 0 );
  TF_ASSERT( hdrsrch_msg.hdr.priority   == MSGR_PRIORITY_HIGH );
}

void hdrsrch_test::fill_in_srch_data ( srch4_qtf_stub_hdr_msg_s *msg )
{
  ( (hdrsrchdrv_srch_res_struct_type *) (msg->payload.start_srch_param.task[0]->result_ptr) )->eng[0] = 0x01CD;
  ( (hdrsrchdrv_srch_res_struct_type *) (msg->payload.start_srch_param.task[0]->result_ptr) )->posx2[0] = 0x4C7D;
  ( (hdrsrchdrv_srch_res_struct_type *) (msg->payload.start_srch_param.task[0]->result_ptr) )->eng_int[0] = 0x01D1;
  ( (hdrsrchdrv_srch_res_struct_type *) (msg->payload.start_srch_param.task[0]->result_ptr) )->posx8_int[0] = 0x000131F3;
}

void hdrsrch_test::process_srch_msg( srch4_qtf_stub_hdr_msg_s *msg )
{
  wait_for_qtf_stub_msg(msg, HDR_QTF_SRCH4_HDR_START_SEARCH_MSG);
  TF_SLEEP(1);
  TF_YIELD();
  TF_MSG(" Test thread recieved HDR_QTF_SRCH4_HDR_START_SEARCH_MSG");
  
  fill_in_srch_data(msg);

  send_msg( &srch_rsp_msg.hdr, MSGR_HDR_FW, CDMA_SRCH_FW_HDR_SRCH_RESULTS_RSP, sizeof(srchfw_msg_rsp_u) );
}

void hdrsrch_test::send_msg( msgr_hdr_struct_type *hdr, msgr_tech_module_type type, msgr_umid_type umid, uint16 msgSize )
{
  errno_enum_type        e_retval;
  msgr_init_hdr( hdr, type, umid );
  e_retval = msgr_send( hdr, msgSize );
  TF_ASSERT( e_retval                 == E_SUCCESS );
}

void hdrsrch_test::wait_for_qtf_stub_msg( hdr_qtf_stub_msg_s *msg, hdr_qtf_msg_e  msg_id )
{
  uint32                bytes_rcvd;

  do
  {
    qtf_tmc_msgr_receive( (uint8*)msg, sizeof( hdr_qtf_stub_msg_s ), &bytes_rcvd );
    TF_MSG( "--- Test script received stub msg: %d --- ", msg->hdr_qtf_stub_msg );
 
  }while( msg->hdr_qtf_stub_msg != msg_id );

  TF_ASSERT( msg->hdr_qtf_stub_msg == msg_id );

}

void hdrsrch_test::wait_for_qtf_stub_msg( srch4_qtf_stub_hdr_msg_s *msg, srch4_qtf_stub_hdr_msg_e  msg_id )
{
  uint32                bytes_rcvd;

  do
  {
    qtf_tmc_msgr_receive( (uint8*)msg, sizeof( srch4_qtf_stub_hdr_msg_s ), &bytes_rcvd );
    TF_MSG( "--- Test script received srch4 stub msg: %d --- ", msg->hdr_qtf_stub_msg );
 
  }while( msg->hdr_qtf_stub_msg != msg_id );

  TF_ASSERT( msg->hdr_qtf_stub_msg == msg_id );

}

void hdrsrch_test::Setup()
{

  static const appmgr_service_init_entry_s  services[] = 
  {
    { "ATS", mcs_timer_init, QTHREAD_MAX_PRIORITY, mcs_timer_teardown },
  };

  prot_trans_type             trans_id = 0;
  int                         fd;
  struct fs_stat              sbuf;
  EFSDIR                      *fs_dirp;
  int                         stat_result;
  hdr_qtf_stub_msg_s          msg;
  errno_enum_type             e_status;
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qtf_efs_initialize("HDR_srch_test");

  /* Initialize EFS
  */
  /* Check if the directory exists
  */
  fs_dirp = efs_opendir("/hdr");

  if(!fs_dirp )
  {

    TF_ASSERT (efs_mkdir ("/hdr",0777)== 0);
  }
  else
  {
    efs_closedir( fs_dirp );
  }

  stat_result = efs_stat ("/hdr/hdr_protocol.cfq", &sbuf);

  if(stat_result)
  {

    fd = efs_open("/hdr/hdr_protocol.cfq", O_RDWR | O_CREAT, 0666);  
    /*TF_ASSERT( fd > 0 );*/

    if ( fd > 0 )
    {
      efs_close( fd );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  // Make sure the timer task is running
  appmgr_test_init( MSGR_MCS_ATS,
                    NULL,       0,
                    services,   ARR_SIZE(services) );

  InitTestResources();

  /* Setup timeticks so we can call TF_SLEEP later
   */
  timetick_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Register for messages
  */
  e_status = msgr_client_add_mq( "hdr_test_queue",
                                &m_test_client, 
                                HDR_STUB_MSG_PRIORITY, 
                                HDR_STUB_NUM_MSGS, 
                                HDR_STUB_MAX_MSG_SIZE,
                                &hdr_stub_id );
  TF_ASSERT( e_status == E_SUCCESS );

  e_status = msgr_register_block( MSGR_MCS_EXT_TEST,
                            &m_test_client,
                            hdr_stub_id,
                            regsitered_umids,
                            sizeof( regsitered_umids ) / 4 );
  TF_ASSERT( e_status == E_SUCCESS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  TF_YIELD();
  TF_MSG("Spawning Tasks");

  /* Initialize TMC and Start libraries
  */

  qtf_tmc_initialize();

  /* Manually start fws
     TODO: fws team should provide stubs to ensure this script works */
  fws_task_init(100);

  #ifdef FEATURE_TMC_TRM
  /* Initialize TRM.  This call must be made after rf_init which is also
    done in tmc_libraries_init_after_service_tasks_start. Note that no
    TRM clients can call TRM until after they first call task_start. */
  //trm_init(rex_self(), HDRMC_NV_CMD_SIG, (trm_wait_callback_t)hdrmc_wait);
  trm_init(rex_self(), 0, NULL);
  #endif

  /* Start the HDRMC task
  */
  qtf_tmc_spawn_rex_task( "HDR CP Task",
                          &hdrmc_tcb,                                   
                          (unsigned char *)hdrmc_stack,
                          (STACK_SIZE/sizeof(rex_stack_size_type)),    
                          (rex_priority_type) HDRMC_PRI,              
                          hdrmc_task,
                          0L );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Start the HDRTX task
  */
  qtf_tmc_spawn_rex_task( "HDR TX Task",
                          &hdrtx_tcb,                                 
                          (unsigned char *)hdrtx_stack,
                          (STACK_SIZE/sizeof(rex_stack_size_type)),   
                          (rex_priority_type) HDRTX_PRI,                
                          hdrtx_task,
                          0L );



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start the HDRRX task
  */
  qtf_tmc_spawn_rex_task( "HDR RX Task",
                          &hdrrx_tcb,                                 
                          (unsigned char *)hdrrx_stack,
                          (STACK_SIZE/sizeof(rex_stack_size_type)),   
                          (rex_priority_type) HDRRX_PRI,              
                          hdrrx_task,
                          0L );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start the HDRDEC task
  */
  qtf_tmc_spawn_rex_task( "HDR DEC Task",
                          &hdrdec_tcb,                                 
                          (unsigned char *)hdrdec_stack,
                          (STACK_SIZE/sizeof(rex_stack_size_type)),   
                          (rex_priority_type) HDRDEC_PRI,              
                          hdrdec_task,
                          0L );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start the HDRSRCH task
  */
  qtf_tmc_spawn_rex_task( "HDR SRCH Task",
                          &hdrsrch_tcb,                              
                          (unsigned char *)hdrsrch_stack,
                          (STACK_SIZE/sizeof(rex_stack_size_type)),  
                          (rex_priority_type) HDRSRCH_PRI,           
                          hdrsrch_task,
                          0L );

  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start the HDR BC task
  */
  qtf_tmc_spawn_rex_task( "HDR BC Task",
                          &hdrbc_tcb,                                  
                          (unsigned char *)hdrbc_stack,
                          (STACK_SIZE/sizeof(rex_stack_size_type)),   
                          (rex_priority_type) HDRBC_PRI,               
                          hdrbc_task,
                          0L );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qtf_tmc_start_rex_task( &hdrmc_tcb );

  qtf_tmc_start_rex_task( &hdrtx_tcb );

  qtf_tmc_start_rex_task( &hdrrx_tcb );

  qtf_tmc_start_rex_task( &hdrdec_tcb );

  qtf_tmc_start_rex_task( &hdrsrch_tcb );

  qtf_tmc_start_rex_task( &hdrbc_tcb );


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* wait for mcs app config cmd*/
  wait_for_msg(MCS_FWS_APP_CONFIG_CMD);

  /* Send a response back to fws */
  send_msg( &fws_app_rsp.hdr, MSGR_MCS_FWS, MCS_FWS_APP_CONFIG_RSP, sizeof( fws_app_config_rsp_t ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* HDR Srch calls SRCH4_HDR_DEREGISTER, wait for that
  */
  wait_for_qtf_stub_msg( &srch4Msg, HDR_QTF_SRCH4_HDR_DEREGISTER );

  TF_YIELD();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  TF_MSG( "HDR tasks started" );

  srch4_qtf_stub_hdr_cbs_s_type* qtf_srch_cbs = hdr_qtf_srch4_get_hdr_search_cbs();

  TF_MSG(" Activate HDR CP ");

  hdrmc_ph_status_chgd_cmd( PROT_PH_STAT_CHG_OPRT_MODE,
                            0,
                            TRUE,
                            PROT_OPRT_MODE_ONLINE,
                            trans_id,
                            PROT_SUBSC_CHG_CDMA
                        );

  /* Wait for 1X CP to ack. MMOC
  */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* HDR MC sends MMOC RPT wait for that
  */
  wait_for_qtf_stub_msg( &msg, HDR_QTF_MMOC_RPT );

  TF_ASSERT( msg.payload.mmoc_rpt.rpt_name == HDR_QTF_MMOC_RPT_PH_STAT_CHGD_CNF );
  TF_ASSERT( msg.payload.mmoc_rpt.prot == SD_MODE_HDR );
  TF_ASSERT( msg.payload.mmoc_rpt.trans_id == trans_id );

  trans_id++;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set the SD action
  */
  sd_ss_act_s_type* sd_act = hdr_qtf_get_sd_ss_act_stub();

  TF_ASSERT( sd_act != NULL );

  sd_act->act = SD_SS_ACT_ACQ_HDR;
  sd_act->prm.acq_hdr.band = SD_BAND_BC0;
  sd_act->prm.acq_hdr.chan = 283;
  sd_act->prm.acq_hdr.acq_mode = SD_HDR_ACQ_MODE_FULL;

}



void hdrsrch_test::Test()
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


}


void hdrsrch_test::test_hdrsrch_deactivate ()
{
  int                   i=0,c=0,f=0;
  uint32                bytes_rcvd;
  errno_enum_type       e_retval;

  /* Deactivate ends up in hdrsrchsync_cmd which HDRSRCH_DEACTIVATE_CMD which calls
  **  and state changes to INACTIVE
  ** as part of inactive enter hdrsrchdrv_shutdown_proc is called which programs the
  ** fingers and starts
  ** HDRSRCHFING_ASSIGNMENT_DONE_WAIT_US 1 ms timer
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* wait for mcs app config cmd*/
  wait_for_msg(MCS_FWS_APP_CONFIG_CMD);

  /* Send a response back to fws */
  send_msg( &fws_app_rsp.hdr, MSGR_MCS_FWS, MCS_FWS_APP_CONFIG_RSP, sizeof( fws_app_config_rsp_t ) );
  
  /* HDR SRCH programs the fingers
  */

  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  /* Send a rsponse back to the HDR fing assignment request
  */
  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP, sizeof( hdrfw_msg_rsp_u ) );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Expire timers
  */

  wait_for_msg( HDR_FW_FLL_ENABLE_CMD ); // wait for fll disable command
  wait_for_msg( HDR_FW_FLL_ENABLE_CMD );
  wait_for_msg( HDR_FW_FLL_ROT_OVERRIDE_CMD );

  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD(); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* HDRSRCH gets the HDRSRCH_DEACTIVATE_SIG */

  /* hdrsrchst_powerdown is called which calls
  ** hdrsrchst_release_rf
  ** hdrsrchutil_dec_deactivate
  ** hdrmdsp_disable_hdr_app */

  wait_for_msg( HDR_FW_DECODE_CTL_CMD );

  /* Send back the cancel decoding response */
  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_CANCEL_ACTIVE_PACKET_RSP, sizeof( hdrfw_cancel_active_packet_rsp_msg_t ) );

  TF_YIELD();


  wait_for_msg( HDR_FW_STATE_CFG_CMD );
  
  /* Send back hdr state config response */
  msgr_init_hdr( &hdr_state_cfg_rsp_msg.hdr, MSGR_HDR_FW, HDR_FW_STATE_CFG_RSP);
  init_hdr_state_cfg_rsp_msg();
  hdr_state_cfg_rsp_msg.state = HDRFW_STATE_ACTIVE;
  e_retval = msgr_send( &hdr_state_cfg_rsp_msg.hdr, sizeof( hdrfw_state_cfg_rsp_msg_t ) );
  TF_ASSERT( e_retval                 == E_SUCCESS );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */



  /* Expire timers
  */
  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();
  TF_YIELD();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* deactivate_done_cb is called which sends HDRIND_LMAC_DEACTIVATE_DONE
  ** srch4_hdr_deregister is called
  */
  /*wait_for_srch4_msg( HDR_QTF_SRCH4_HDR_START_SEARCH_MSG );*/
  
  wait_for_qtf_stub_msg( &srch4Msg, HDR_QTF_SRCH4_HDR_DEREGISTER );

}

void hdrsrch_test::Teardown( )
{

  prot_trans_type               trans_id = 0;
  errno_enum_type               e_status;
  hdr_qtf_stub_msg_s            msg;

  appmgr_service_init_entry_s  teardowns[] = {
     { "ATS", mcs_timer_init, QTHREAD_MAX_PRIORITY, mcs_timer_teardown },
  };

  TF_MSG(" In Teardown ");

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Shut down the HDR stack
  */
  hdrmc_deactivate_protocol(PROT_DEACT_PWR_DOWN,trans_id);
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* process new msgs from acq state
  */
  if ( hdrsrch_state_id == HDRSRCH_ACQ_STATE )
  {
    process_srch_msg(&srch4Msg);
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* perform deactivate depend on state,
     for now, time sync and ofs measurement state will not 
	 take on the normal deactivate route
  */

  if ( hdrsrch_state_id != HDRSRCH_TIME_SYNC_STATE && hdrsrch_state_id != HDRSRCH_OFS_MEAS_STATE 
       && last_state != HDRSRCH_OFS_MEAS_STATE)
  {
	test_hdrsrch_deactivate ();
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Wait for 1X CP to ack. MMOC
  */


  wait_for_qtf_stub_msg( &msg, HDR_QTF_MMOC_RPT );

  TF_ASSERT( msg.payload.mmoc_rpt.rpt_name == HDR_QTF_MMOC_RPT_PROT_DEACTD_CNF );
  TF_ASSERT( msg.payload.mmoc_rpt.trans_id == trans_id );

  trans_id++;

  TF_YIELD();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  hdrmc_ph_status_chgd_cmd( PROT_PH_STAT_CHG_OPRT_MODE,
                            0,
                            TRUE,
                            PROT_OPRT_MODE_PWROFF,
                            trans_id,
                            PROT_SUBSC_CHG_CDMA
                        );

  /* Wait for 1X CP to ack. MMOC
  */

  
  wait_for_qtf_stub_msg( &msg, HDR_QTF_MMOC_RPT );

  TF_ASSERT( msg.payload.mmoc_rpt.rpt_name == HDR_QTF_MMOC_RPT_PH_STAT_CHGD_CNF );
  TF_ASSERT( msg.payload.mmoc_rpt.prot == SD_MODE_HDR );
  TF_ASSERT( msg.payload.mmoc_rpt.trans_id == trans_id );

  trans_id++;
  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  TF_MSG(" MMOC Recieved MMOC_RPT_PH_STAT_CHGD_CNF ");

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  TF_YIELD();

  /* shut down mcs fws */

  send_msg( &fws_shutdown.hdr, MSGR_MCS_FWS, MCS_FWS_SHUTDOWN_SPR, sizeof( fws_shutdown_spr_t ) );

  /* stop all hdr threads */

  qtf_tmc_teardown();

  e_status = msgr_deregister_block( MSGR_MCS_EXT_TEST,
                            &m_test_client,
                            regsitered_umids,
                            sizeof( regsitered_umids ) / 4 );

  TF_ASSERT( e_status == E_SUCCESS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  appmgr_test_teardown( 0,
                        teardowns, ARR_SIZE(teardowns) );

  TeardownTestResources();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  printf("\nTest Successful!\n\n");

}

