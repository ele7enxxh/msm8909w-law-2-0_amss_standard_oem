

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
02/20/10   pxu     Initial Version

===========================================================================*/

#include "hdrsrch_test_timeSync.h"

/* Regsiter the test */
TF_REGISTER_TEST_CASE( COMPONENT, hdrsrch_test_timeSync );

void hdrsrch_test_timeSync::Test()
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  test_hdrsrch_timeSync();
    /* timeSync */

}

void hdrsrch_test_timeSync::test_hdrsrch_timeSync()
{
  sys_time_req.cdma_sys_time = 1000;
  sys_time_req.lte_ostmr = 2000;
  send_msg( &sys_time_req.hdr, MSGR_HDR_SRCH, HDR_SRCH_TRAN_SYS_TIME_REQ, sizeof( hdrsrch_tran_sys_time_req_msg_t ) );

  /* allow timer to expire and state be updated */
  TF_YIELD();
  TF_SLEEP(2);  

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_TIME_SYNC_STATE );
}

