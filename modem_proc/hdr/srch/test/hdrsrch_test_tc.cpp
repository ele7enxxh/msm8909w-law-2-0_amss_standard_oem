
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
03/01/10   pxu     removed some expected msgs that won't be sent
09/15/09   ljl     Supported srch rsp msg.
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
TF_REGISTER_TEST_CASE( COMPONENT, hdrsrch_test_tc );

void hdrsrch_test_tc::Test()
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
}

void hdrsrch_test_tc::test_hdrsrch_tc()
{
  errno_enum_type       e_retval;
 
  sys_active_prot_e_type               hdr_subtype  = SYS_ACTIVE_PROT_HDR_REL0;
  hdrsrch_traffic_tune_away_enum_type  tc_tune_away = HDRSRCH_TUNE_AWAY_IS2000;
  uint8                                num_subaset  = 1;
  hdrsrch_subactive_set_type           subaset_list[1];
  uint8                                cc_subaset_idx = 0;
  uint8                                frame_offset   = 0;
  hdrsrch_tca_cb_type                  tca_report     = tca_complete_cb;
  hdrsrch_suspend_cb_type              suspend_cb     = NULL;
  hdrsrch_schdgrp_rpt_cb_type          schdgrp_rpt_cb = NULL;

  memset( &subaset_list, 0, sizeof( hdrsrch_subactive_set_type ) );

  subaset_list[0].channel.band = SYS_BAND_BC0;
  subaset_list[0].channel.chan_num = 283;
  subaset_list[0].systype = SYS_HDR_CHAN_REC_MODE_IS856;
  subaset_list[0].not_reportable = FALSE;
  subaset_list[0].num_aset_pilots = 1;
  subaset_list[0].demod_idx = 0;
  subaset_list[0].aset_list[0].pilot_pn = 104;
  subaset_list[0].aset_list[0].softer_handoff = 0;
  subaset_list[0].aset_list[0].mac_index = 69;
  subaset_list[0].aset_list[0].drc_cover = 1;
  subaset_list[0].aset_list[0].pilot_group_id = 0;
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
  /* hdrsrchtc_init()
     hdrsrchtc_init_drc_filter() */
  wait_for_msg( HDR_FW_DRC_FILT_RESET_CMD );

  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_RESET_DRC_FILT_RSP, sizeof(hdrfw_msg_rsp_u) );

  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();
  TF_ASSERT(hdrsrch_state_id == HDRSRCH_CONNECTED_STATE );

  /* hdrsrchtc_from_idle_init()
     hdrsrchtc_init_drc() */
  wait_for_msg( HDR_FW_ASP_UPDATE_CMD );
  wait_for_msg( HDR_FW_HANDOFF_OVERRIDE_CMD );
  wait_for_msg( HDR_FW_DRC_CFG_CMD );



  /* hdrsrchtc_tca_tune()*/

#if 0
   /* tuning is not needed here , these messages will not be received */

  wait_for_msg ( HDR_FW_FING_ASSIGN_CMD );
  msgr_init_hdr( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP );
  e_retval = msgr_send( &hdrfw_msg_rsp.hdr, sizeof( hdrfw_msg_rsp_u ) );
  TF_ASSERT( e_retval                 == E_SUCCESS );

  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();
  TF_ASSERT(hdrsrch_state_id == HDRSRCH_CONNECTED_STATE );

  wait_for_msg( HDR_FW_DECODE_CTL_CMD );

  /* Send back the cancel decoding response */
  msgr_init_hdr( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_CANCEL_ACTIVE_PACKET_RSP );
  e_retval = msgr_send( &hdrfw_msg_rsp.hdr, sizeof( hdrfw_cancel_active_packet_rsp_msg_t ) );
  TF_ASSERT( e_retval  == E_SUCCESS );
  
  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();
  TF_ASSERT(hdrsrch_state_id == HDRSRCH_CONNECTED_STATE );
#endif


  /* hdrsrchtc_tca_tune_done() */
  /* hdrsrchutil_update_aset_pilots_revb() */

#if 0
  /* finger is not modified, no cmd will be sent */
  wait_for_msg ( HDR_FW_FING_ASSIGN_CMD );
  msgr_init_hdr( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP );
  e_retval = msgr_send( &hdrfw_msg_rsp.hdr, sizeof( hdrfw_msg_rsp_u ) );
  TF_ASSERT( e_retval                 == E_SUCCESS );
#endif

  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();

  /* hdrsrchutil_aset_commit_to_dsp() */
  wait_for_msg( HDR_FW_HANDOFF_CFG_CMD );
  wait_for_msg( HDR_FW_DRC_CFG_CMD );
  wait_for_msg( HDR_FW_ASP_UPDATE_CMD );

  /* hdrsrcheq_assign_eqs();
     hdrsrcheq_program_eqs(); */
  wait_for_msg( HDR_FW_EQ_CFG_CMD );

  msgr_init_hdr( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_EQ_CONFIG_RSP );
  e_retval = msgr_send( &hdrfw_msg_rsp.hdr, sizeof( hdrfw_msg_rsp_u ) );
  TF_ASSERT( e_retval                 == E_SUCCESS );

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

  TF_MSG( "... End of hdrsrch_test_tc() ...\n" );
}

void tca_complete_cb( void )
{
  TF_ASSERT( hdrsrch_state_id == HDRSRCH_CONNECTED_STATE );
  TF_ASSERT( hdrsrchset.subaset_cnt == 1 );
  TF_ASSERT( hdrsrchset.subaset[0].aset[0]->pn_offset == 104 );
  TF_ASSERT( hdrsrchset.subaset[0].channel.chan_num == 283 );
  TF_ASSERT( hdrsrchset.subaset[0].channel.band == SYS_BAND_BC0 );

  TF_MSG( "... TCA cmd (band0/chan283/pn104) complete!..." );
}

