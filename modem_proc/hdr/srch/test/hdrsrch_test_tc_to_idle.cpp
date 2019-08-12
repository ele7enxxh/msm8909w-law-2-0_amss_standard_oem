
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               1 X  C A L L  P R O C E S S I N G   T E S T  

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2008 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/15   vke     Changes to disable TA if TCA is inprogress
03/01/10   pxu     use new function to send msg
05/05/09   lyl     Initial Version

===========================================================================*/

#include "hdrsrch_test_tc.h"
extern "C" {
#include "hdrsrchset.h"
#include "hdrsrchutil.h"
#include "hdrsrch.h"

}


/* Register the test */
TF_REGISTER_TEST_CASE( COMPONENT, hdrsrch_test_tc_to_idle );

void hdrsrch_test_tc_to_idle::Test()
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

  test_hdrsrch_tc_to_idle();
    /* Move to idle state */
}

void hdrsrch_test_tc_to_idle::test_hdrsrch_tc_to_idle()
{
  uint32                bytes_rcvd;
  errno_enum_type       e_retval;
  hdr_qtf_stub_msg_s    msg;

  hdrsrch_idle_ho_cb_type         idle_ho_cb = NULL;
  hdrsrch_suspend_cb_type         suspend_cb = NULL;

  /* Send idle mode cmd and trigger state change to idle */
  hdrsrch_idle_mode( idle_ho_cb, suspend_cb, FALSE );

  /* hdrsrchutil_aset_enter_idle();
     hdrsrchutil_aset_commit_to_dsp();
     hdrsrchfing_program_fings(); */
  wait_for_msg( HDR_FW_HANDOFF_CFG_CMD );

  wait_for_msg( HDR_FW_DRC_CFG_CMD );

  wait_for_msg( HDR_FW_ASP_UPDATE_CMD );

  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP, sizeof( hdrfw_msg_rsp_u ));

  /* hdrsrchtc_done(); 
     hdrsrcheq_deassign_all_eqs(); */
  wait_for_msg( HDR_FW_EQ_CFG_CMD );

  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_EQ_CONFIG_RSP, sizeof( hdrfw_msg_rsp_u ));

  /* hdrsrchmon_init();
     hdrsrchmsg_send_ho_ovrd_msg(); */
  wait_for_msg( HDR_FW_TUNEAWAY_STOP_CMD );

  TF_MSG( "... End of test_tc_to_idle() ...\n" );

}



