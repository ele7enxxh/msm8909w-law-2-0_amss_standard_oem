#include "hdrsrch_test_acq.h"

/* Regsiter the test */
TF_REGISTER_TEST_CASE( COMPONENT, hdrsrch_test_acq_success );

void hdrsrch_test_acq_success::Test()
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  test_acq_success( );
  /* Successful Acquisition */

  /* Enetr sync state, process the search msg in sync state */
  process_srch_msg(&srch4Msg);

  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  /* Send a rsponse back to the HDR fing assignment request
  */
  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP, sizeof( hdrfw_msg_rsp_u ) );
}
