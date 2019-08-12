
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
04/27/11   rmg     Added HDR MCPM support changes.
03/01/10   pxu     removed some expected msgs that won't be sent
08/05/09   lyl     Updated for decoder deactivation.
07/22/09   ljl     Added frame offset.
05/05/09   lyl     Initial Version

===========================================================================*/

#include "hdrsrch_test_tc.h"
extern "C" {
#include "hdrsrchset.h"
#include "hdrsrchutil.h"
#include "hdrsrch.h"

}

/* Register the test */
TF_REGISTER_TEST_CASE( COMPONENT, hdrsrch_test_tc_ho );

void hdrsrch_test_tc_ho::Test()
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  test_acq_success( );
    /* Acquisition */

  test_hdrsrch_sync();
    /* Sync */

  test_hdrsrch_idle();
    /* Idle */

  test_hdrsrch_tc();
    /* Traffic */

  test_hdrsrch_tc_ho();
    /* Handoff */
}

void hdrsrch_test_tc_ho::test_hdrsrch_tc_ho()
{

 errno_enum_type       e_retval;
 hdr_qtf_stub_msg_s    msg;

 TF_YIELD();
 TF_SLEEP(1);
 TF_YIELD();

 sys_active_prot_e_type               hdr_subtype  = SYS_ACTIVE_PROT_HDR_REL0;
 hdrsrch_traffic_tune_away_enum_type  tc_tune_away = HDRSRCH_TUNE_AWAY_IS2000;
 uint8                                num_subaset  = 1;
 hdrsrch_subactive_set_type           subaset_list[1];
 uint8                                cc_subaset_idx = 0;
 uint8                                frame_offset   = 0;
 hdrsrch_tca_cb_type                  tca_report     = tc_ho_tca_complete_cb;
 hdrsrch_suspend_cb_type              suspend_cb     = NULL;
 hdrsrch_schdgrp_rpt_cb_type          schdgrp_rpt_cb = NULL;

 memset( subaset_list, 0, sizeof( subaset_list ) );

 subaset_list[0].channel.band = SYS_BAND_BC0;
 subaset_list[0].channel.chan_num = 500;
 subaset_list[0].systype = SYS_HDR_CHAN_REC_MODE_IS856;
 subaset_list[0].not_reportable = FALSE;
 subaset_list[0].num_aset_pilots = 1;
 subaset_list[0].demod_idx = 0;
 subaset_list[0].aset_list[0].pilot_pn = 108;
 subaset_list[0].aset_list[0].softer_handoff = 0;
 subaset_list[0].aset_list[0].mac_index = 81;
 subaset_list[0].aset_list[0].drc_cover = 2;
 subaset_list[0].aset_list[0].pilot_group_id = 1;
 subaset_list[0].aset_list[0].dsc = 1;

 hdrsrch_chan_and_aset_assign_revb( hdr_subtype,
                                    tc_tune_away,
                                    num_subaset,
                                    subaset_list,
                                    cc_subaset_idx,
                                    frame_offset,
                                    tca_report,
                                    suspend_cb,
                                    schdgrp_rpt_cb );

  /* hdrsrchtc_tca_tune(); 
     hdrsrchfing_deassign_all_fings(); 
     hdrsrchutil_dec_deactivate() */

  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP, sizeof( hdrfw_msg_rsp_u ));

  wait_for_msg( HDR_FW_DECODE_CTL_CMD );

  /* Send back the cancel decoding response */
  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_CANCEL_ACTIVE_PACKET_RSP, sizeof( hdrfw_cancel_active_packet_rsp_msg_t ));

  wait_for_msg( HDR_FW_FLL_GAIN_CFG_CMD );

  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP, sizeof( hdrfw_msg_rsp_u ));


  /* hdrsrchrf_settle_start();
     hdrsrchrf_start_timer(); - 14ms */
  TF_SLEEP(14); 


  /* hdrsrchutil_update_aset_pilots_revb(); 
     hdrsrchfing_program_fings(); */

  /* hdrsrchutil_aset_commit_to_dsp() */
  wait_for_msg( HDR_FW_HANDOFF_CFG_CMD );
  wait_for_msg( HDR_FW_DRC_CFG_CMD );
  wait_for_msg( HDR_FW_ASP_UPDATE_CMD );

  /* hdrsrcheq_assign_eqs();
     hdrsrcheq_program_eqs(); */

  /* hdrsrcheq_set_dev_sel_and_override_mode(); */
  wait_for_msg( HDR_FW_EQ_OVERRIDE_CMD );

  /* hdrsrch_on_searcher_idle( hdrsrchtc_aset_update_search ) */
  process_srch_msg(&srch4Msg);

  /* hdrsrchtc_default_dump();
     hdrsrcheq_update_com(); */
  wait_for_msg( HDR_FW_ASP_UPDATE_CMD );

  /* hdrsrchutil_process_search_triage(); */
  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP, sizeof( hdrfw_msg_rsp_u ));


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();

  /* hdrsrchutil_set_handoff_control(); */
  wait_for_msg( HDR_FW_HANDOFF_OVERRIDE_CMD );

  TF_MSG( ".... End of test_hdrsrch_tc_ho() ...." );
}


void tc_ho_tca_complete_cb( void )
{
  TF_ASSERT( hdrsrch_state_id == HDRSRCH_CONNECTED_STATE );
  TF_ASSERT( hdrsrchset.subaset_cnt == 1 );
  TF_ASSERT( hdrsrchset.subaset[0].aset[0]->pn_offset == 108 );
  TF_ASSERT( hdrsrchset.subaset[0].channel.chan_num == 500 );
  TF_ASSERT( hdrsrchset.subaset[0].channel.band == SYS_BAND_BC0 );

  TF_MSG( "... TCA cmd (band0/chan500/pn108) complete! ... \n" );
}
