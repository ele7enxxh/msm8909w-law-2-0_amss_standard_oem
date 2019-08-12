
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
05/10/10   smd     Updated code with new hdrsrch_synchronize().
03/01/10   pxu     Fixed Code for compatibility issues
09/14/09   ljl     Supported srch rsp msg.
04/07/09   ljl     Initial Version

===========================================================================*/

#include "hdrsrch_test_sync.h"

/* Regsiter the test */
TF_REGISTER_TEST_CASE( COMPONENT, hdrsrch_test_sync );

void hdrsrch_test_sync::Test()
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  test_acq_success();
    /* Acquisition */

  test_hdrsrch_sync();
    /* Sync */

}

void hdrsrch_test_sync::test_hdrsrch_sync()
{
  int                   i=0,c=0,f=0;
  errno_enum_type       e_retval;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* As part of sync init 
  ** hdrsrchsync_init gets called 
  **         |
  **        \/
  ** hdrsrchsync_sched
  **         |
  **        \/
  ** hdrsrchutil_program_search_set
  **         |
  **        \/
  ** hdrsrchutil_submit_search_list
  **         |
  **        \/
  ** hdrsrchdrv_send_srch_list
  **         |
  **        \/
  ** srch4_hdr_start_search    
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* The first search has been received by hdrsrch_test_acq::test_hdrsrch_acq() 
     after acq succeeded and entered sync state */

  process_srch_msg(&srch4Msg);

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_SYNC_STATE );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* hdrsrchsync_dump is called which calls hdrsrchutil_process_search_set
  ** which calls hdrsrchutil_process_search_triage hdrsrchtri_triage, 
  ** eventually hdrsrchtri_wrapup calls hdrsrchfing_program_fings
  ** after this hdrsrchutil_update_win_center gets called
  */

  /* HDR SRCH programs the fingers
  */

  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_SYNC_STATE );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Send a response back to the HDR fing assignment request
  */
  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP, sizeof( hdrfw_msg_rsp_u ) );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Send synchronization cmd */
  qword                           sync_time;
    /* Time from Synchronous Control Channel (SCC) message */

  sys_pilot_pn_type               sync_pn;
    /* Pilot PN from SCC msg */

  uint16                          sync_hstr;
    /* The hardware half-slot value at the SCC message capsule start */

  sync_time[0] = 0x0100;
  sync_time[1] = 0x0100;
  sync_pn = 104;
  sync_hstr = 0x20;

  hdrsrch_synchronize( sync_time, sync_pn, sync_hstr, NULL );

  /* 
      The followimg is the message sequence of synchronization:

      - Assign fingers with the updated PN offset

        - Send finger assignment response

      - Start slam

        - Send slam response

      - Set hyperspace flag

      - Update hstr offset

      - Enter Idle state

   */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP, sizeof( hdrfw_msg_rsp_u ) );

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_SYNC_STATE );

  wait_for_msg( HDR_FW_MSTR_SLAM_CMD );

  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_SLAM_RSP, sizeof( hdrfw_msg_rsp_u ) );

  wait_for_msg( HDR_FW_HYPERSPACE_CFG_CMD );

  /* To move timer in hdrsrchsync */
  TF_YIELD();
  TF_SLEEP(10);

  wait_for_msg( HDR_FW_HSTR_OFFSET_CFG_CMD );

  /* Expire timers
  */

  TF_YIELD();
  TF_SLEEP(30);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* hdrsrchafc_rv_trk_timer_expire_tf gets called
  ** HDRSRCH_SCHEDULE_SIG gets set hdrsrchsync_sched gets called 
  ** hdrsrchutil_program_search_set gets called which sends the srch4 command
  */

  process_srch_msg(&srch4Msg);

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_IDLE_STATE );


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* HDR SRCH programs the fingers
  */

  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_IDLE_STATE );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Send a rsponse back to the HDR fing assignment request
  */
  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP, sizeof( hdrfw_msg_rsp_u ) );

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_IDLE_STATE );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Expire timers
  */
  TF_SLEEP(30);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* hdrsrchsync_dump gets called
  */
  /* Schedule sig gets set and srch is programmed again
  */

  process_srch_msg(&srch4Msg);

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_IDLE_STATE );

  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* hdrsrchsync_dump is called which calls hdrsrchutil_process_search_set
  ** which calls hdrsrchutil_process_search_triage hdrsrchtri_triage, 
  ** eventually hdrsrchtri_wrapup calls hdrsrchfing_program_fings
  ** after this hdrsrchutil_update_win_center gets called
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* HDR SRCH programs the fingers
  */
  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_IDLE_STATE );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Send a rsponse back to the HDR fing assignment request
  */
  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP, sizeof( hdrfw_msg_rsp_u ) );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();
  TF_ASSERT(hdrsrch_state_id == HDRSRCH_IDLE_STATE );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Increment time ans deactivate the HDR stack
  */

  TF_SLEEP(5);
  TF_YIELD();

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_IDLE_STATE );
}

