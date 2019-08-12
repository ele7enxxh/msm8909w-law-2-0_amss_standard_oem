
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
03/05/10   pxu     Updated script to function with new script APIs 
09/14/09   ljl     Supported srch rsp msg.
04/07/09   ljl     Initial Version

===========================================================================*/

#include "hdrsrch_test.h"
#include "hdrsrch_test_idle.h"
extern "C" {
#include "hdrsrchset.h"
#include "hdrsrchutil.h"

}

/* Regsiter the test */
TF_REGISTER_TEST_CASE( COMPONENT, hdrsrch_test_idle_ho );

void hdrsrch_test_idle::Test()
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  test_acq_success( );
    /* Acquisition */

  test_hdrsrch_sync();
    /* Sync */

  test_hdrsrch_idle();
    /* Idle */

}

void hdrsrch_test_idle::test_hdrsrch_idle()
{
}

void hdrsrch_test_idle_ho::test_hdrsrch_idle()
{
  errno_enum_type       e_retval;
  hdr_qtf_stub_msg_s    msg;

  boolean               transition;
  hdrsrch_nset_update_params_type     params;

  memset( &params, 0, sizeof( hdrsrch_nset_update_params_type ) );

  TF_MSG( "subaset cnt:%d, ASET cnt:%d, pn_offset:%d, chan: %d/%d\n",
          hdrsrchset.subaset_cnt,
          hdrsrchset.subaset[0].aset_cnt,
          hdrsrchset.subaset[0].aset[0]->pn_offset,
          hdrsrchset.subaset[0].aset[0]->chan.band,
          hdrsrchset.subaset[0].aset[0]->chan.chan_num );

  params.num_pilots = 1;
  params.update_type = HDRSRCH_NSET_UPDATE_BROADCAST;
  params.nset_pilots[0].link.mode = SYS_HDR_CHAN_REC_MODE_IS856;
  params.nset_pilots[0].link.channel.band = SYS_BAND_BC0;
  params.nset_pilots[0].link.channel.chan_num = 283;
  params.nset_pilots[0].link.pilot_pn = 116;
  params.num_channels = 1;
  params.channel_list[0].band = SYS_BAND_BC0;
  params.channel_list[0].chan_num = 283;
  params.serving_pilot_gid = 254;
  params.hdr_subtype = SYS_ACTIVE_PROT_HDR_REL0;

  transition = hdrsrchutil_update_nset_pilots( params.nset_pilots,
                                               params.num_pilots,
                                               params.update_type,
                                               params.channel_list,
                                               params.num_channels,
                                               params.avoided_channel_list,
                                               params.num_avoided_channels,
                                               params.serving_pilot_gid,
                                               params.hdr_subtype );

  TF_MSG( "NSET cnt:%d, pn:%d, ch:%d/%d\n\n",
          hdrsrchset.nset_cnt,
          hdrsrchset.nset[0]->pn_offset,
          hdrsrchset.nset[0]->chan.band,
          hdrsrchset.nset[0]->chan.chan_num );

  TF_YIELD();
  TF_SLEEP(30);  
    /* 1st search */

  wait_for_qtf_stub_msg( &srch4Msg, HDR_QTF_SRCH4_HDR_START_SEARCH_MSG );

  /* fill out search fields*/

  ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[0]->result_ptr) )->eng[0] = 0x0040;

  ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[0]->result_ptr) )->posx2[0] = 0x4C7D;

  ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[0]->result_ptr) )->eng_int[0] = 0x0064;

  ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[0]->result_ptr) )->posx8_int[0] = 0x000131F3;

  ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[2]->result_ptr) )->eng[0] = 0x01EF;

  ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[2]->result_ptr) )->posx2[0] = 0x4C89;

  ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[2]->result_ptr) )->eng_int[0] = 0x01D1;

  ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[2]->result_ptr) )->posx8_int[0] = 0x00013210;

  msgr_init_hdr( &srch_rsp_msg.hdr, MSGR_HDR_FW, CDMA_SRCH_FW_HDR_SRCH_RESULTS_RSP );
  e_retval = msgr_send( &srch_rsp_msg.hdr, sizeof( srch_rsp_msg ) );
  TF_ASSERT( e_retval                 == E_SUCCESS );

  /* hdrsrchutil_process_search_triage(); */
  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  msgr_init_hdr( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP );
  e_retval = msgr_send( &hdrfw_msg_rsp.hdr, sizeof( hdrfw_msg_rsp_u ) );
  TF_ASSERT( e_retval                 == E_SUCCESS );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();
  TF_ASSERT(hdrsrch_state_id == HDRSRCH_IDLE_STATE );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for ( uint8 i = 0; i < 7; i++ )
  {
    TF_MSG( " ... searched for %d times ... ", i+1 );
    TF_YIELD();
    TF_SLEEP(30);

    wait_for_qtf_stub_msg( &srch4Msg, HDR_QTF_SRCH4_HDR_START_SEARCH_MSG );

    /* fill out search response data*/

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[0]->result_ptr) )->eng[0] = 0x0040;

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[0]->result_ptr) )->posx2[0] = 0x4C7D;

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[0]->result_ptr) )->eng_int[0] = 0x0064;

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[0]->result_ptr) )->posx8_int[0] = 0x000131F3;

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[2]->result_ptr) )->eng[0] = 0x01EF;

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[2]->result_ptr) )->posx2[0] = 0x4C89;

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[2]->result_ptr) )->eng_int[0] = 0x01D1;

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[2]->result_ptr) )->posx8_int[0] = 0x00013210;

    /* send back response*/
    msgr_init_hdr( &srch_rsp_msg.hdr, MSGR_HDR_FW, CDMA_SRCH_FW_HDR_SRCH_RESULTS_RSP );
    e_retval = msgr_send( &srch_rsp_msg.hdr, sizeof( srch_rsp_msg ) );
    TF_ASSERT( e_retval                 == E_SUCCESS );

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* hdrsrchutil_process_search_triage(); */
    wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

    msgr_init_hdr( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP );
    e_retval = msgr_send( &hdrfw_msg_rsp.hdr, sizeof( hdrfw_msg_rsp_u ) );
    TF_ASSERT( e_retval                 == E_SUCCESS );

    TF_YIELD();
    TF_SLEEP(1);
    TF_YIELD();
    TF_ASSERT(hdrsrch_state_id == HDRSRCH_IDLE_STATE );

  }  /* for i */

    TF_MSG( " ... searched for 8 times ... " );
    TF_YIELD();
    TF_SLEEP(30);

    wait_for_qtf_stub_msg( &srch4Msg, HDR_QTF_SRCH4_HDR_START_SEARCH_MSG );

    /* fill in search response data*/

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[0]->result_ptr) )->eng[0] = 0x0040;

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[0]->result_ptr) )->posx2[0] = 0x4C7D;

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[0]->result_ptr) )->eng_int[0] = 0x0064;

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[0]->result_ptr) )->posx8_int[0] = 0x000131F3;

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[2]->result_ptr) )->eng[0] = 0x01EF;

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[2]->result_ptr) )->posx2[0] = 0x4C89;

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[2]->result_ptr) )->eng_int[0] = 0x01D1;

    ( (hdrsrchdrv_srch_res_struct_type *) (srch4Msg.payload.start_srch_param.task[2]->result_ptr) )->posx8_int[0] = 0x00013210;

    /* send back response */
    msgr_init_hdr( &srch_rsp_msg.hdr, MSGR_HDR_FW, CDMA_SRCH_FW_HDR_SRCH_RESULTS_RSP );
    e_retval = msgr_send( &srch_rsp_msg.hdr, sizeof( srch_rsp_msg ) );
    TF_ASSERT( e_retval                 == E_SUCCESS );

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* hdrsrchutil_aset_commit_to_dsp() */
  wait_for_msg( HDR_FW_HANDOFF_CFG_CMD );

  wait_for_msg( HDR_FW_DRC_CFG_CMD );

  /* hdrsrchfing_program_fings() */
  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  msgr_init_hdr( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP );
  e_retval = msgr_send( &hdrfw_msg_rsp.hdr, sizeof( hdrfw_msg_rsp_u ) );
  TF_ASSERT( e_retval                 == E_SUCCESS );

  wait_for_msg( HDR_FW_HANDOFF_OVERRIDE_CMD );

  /* hdrsrch_drop_from_aset() */
  wait_for_msg( HDR_FW_ASP_UPDATE_CMD );

  /* hdrsrchutil_process_search_triage(); */
  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  msgr_init_hdr( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP );
  e_retval = msgr_send( &hdrfw_msg_rsp.hdr, sizeof( hdrfw_msg_rsp_u ) );
  TF_ASSERT( e_retval                 == E_SUCCESS );

  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();


  TF_ASSERT( hdrsrchset.subaset[0].aset[0]->pn_offset == 116 );
  TF_MSG( "... Idle Handoff to PN116 (from 104) sucessfully! ..." );

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_IDLE_STATE );
  TF_MSG( ".... End of test_hdrsrch_idle_ho()....\n" );


}

