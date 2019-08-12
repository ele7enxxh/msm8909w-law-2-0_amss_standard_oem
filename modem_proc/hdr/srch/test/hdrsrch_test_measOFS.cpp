

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

#include "hdrsrch_test_measOFS.h"

/* Regsiter the test */
TF_REGISTER_TEST_CASE( COMPONENT, hdrsrch_test_measOFS );

void hdrsrch_test_measOFS::Test()
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  test_hdrsrch_timeSync();
    /* timeSync */
  test_hdrsrch_measOFS();
    /* measOFS */
}

void hdrsrch_test_measOFS::test_hdrsrch_measOFS()
{
  errno_enum_type       e_retval;

  /* set up pilot measurement request parameters*/
  prepare_lte_pilot_request();
  
  send_msg( &pilot_meas_req.hdr, MSGR_HDR_SRCH, HDR_SRCH_LTE_PILOT_MEAS_REQ, sizeof( hdrsrch_lte_pilot_meas_req_msg_t ) );

  wait_for_msg( HDR_FW_STATE_CFG_CMD );

  /* Send HDR State Config response */
  msgr_init_hdr( &hdr_state_cfg_rsp_msg.hdr, MSGR_HDR_FW, HDR_FW_STATE_CFG_RSP);
  init_hdr_state_cfg_rsp_msg();
  hdr_state_cfg_rsp_msg.state = HDRFW_STATE_ACTIVE;   //TODO: should be in meas state
  e_retval = msgr_send( &hdr_state_cfg_rsp_msg.hdr, sizeof( hdrfw_state_cfg_rsp_msg_t  ) );
  TF_ASSERT( e_retval                 == E_SUCCESS );

  TF_YIELD();
  TF_SLEEP(1);
  TF_YIELD();
  TF_SLEEP(12);

  //wait_for_msg( HDR_SRCH_LTE_PILOT_MEAS_RSP );
  TF_YIELD(); // give state machine time to update the state id
  TF_SLEEP(1);

  TF_ASSERT(hdrsrch_state_id == HDRSRCH_OFS_MEAS_STATE );
}

void hdrsrch_test_measOFS::prepare_lte_pilot_request()
{
  pilot_meas_req.rot_error = 17;
  pilot_meas_req.rf_device = RFCOM_TRANSCEIVER_0;
  pilot_meas_req.rf_cleanup_script_idx = 2;
  pilot_meas_req.channel.band = SYS_BAND_CLASS_PCS;
  pilot_meas_req.channel.chan_num = 500;
  pilot_meas_req.sector_count = 3;
  pilot_meas_req.pn_list[0] = 104;
  pilot_meas_req.pn_list[1] = 108;
  pilot_meas_req.pn_list[2] = 112;
  pilot_meas_req.gap_start = HDRSRCH_PILOT_MEAS_START_IMMEDIATE;
  pilot_meas_req.gap_length = 1000;
  pilot_meas_req.window_size = HDRSRCH_WINSIZE_4_CHIPS; 
  pilot_meas_req.online_srch = false;
}

