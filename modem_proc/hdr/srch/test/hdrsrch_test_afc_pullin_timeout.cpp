
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               1 X  C A L L  P R O C E S S I N G   T E S T  

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2008 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/01/10   pxu     factored out portions of code and put them in hdrsrch_test_acq.cpp 
12/11/09   grl     Added FW State Config Support
09/30/09   kss     Updated fw register access.
09/16/09   etv     Updated fw register access for interface change.
09/14/09   ljl     Supported srch rsp msg.
04/07/09   ljl     Initial Version

===========================================================================*/

#include "hdrsrch_test_acq.h"

/* Regsiter the test */
TF_REGISTER_TEST_CASE( COMPONENT, hdrsrch_test_afc_pullin_timeout );


void hdrsrch_test_afc_pullin_timeout::Test()
{
  test_hdrsrch_acq( );
  /* config the fll locked flag */
  hdrfw_msg_rsp.fll_acq.fllLocked = 0xFFFFFFFF;

	/* let srch time out */
  for ( int i = 0; i < 1000/10 + 2; i++ ) 
  {
    TF_SLEEP(20);
    TF_YIELD();
  }

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
 
	/* process left over search cmds */
	process_srch_msg(&srch4Msg);
  process_srch_msg(&srch4Msg);
  process_srch_msg(&srch4Msg);
  
  wait_for_msg( HDR_FW_FING_ASSIGN_CMD );

  /* Send a rsponse back to the HDR fing assignment request
  */
  send_msg( &hdrfw_msg_rsp.hdr, MSGR_HDR_FW, HDR_FW_SRCH_FING_ASSIGN_RSP, sizeof( hdrfw_msg_rsp_u ) );

	TF_ASSERT(hdrsrch_state_id == HDRSRCH_ACQ_STATE );
}

