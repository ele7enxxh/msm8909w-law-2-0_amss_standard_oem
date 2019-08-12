/** @mainpage HDRDEC_TEST class
    @file hdrdec_test.c
    @brief This file contains the defintion of class hdrdec_test */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  H D R   D E C O D E R   T E S T   M O D U L E

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/drivers/test/hdrdec_test.cpp#1 $ $Author: mplcsds1 $ $DateTime: 2016/12/13 07:58:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/09   etv     Included mcs_time.h
11/03/09   etv     Updated for changes to timer API
10/04/09   etv     Initial Version

===========================================================================*/


#include "hdrdec_test.h"

extern "C" {
#include "mcs_time.h"
}

/**
    @defgroup hdrdec_test HDRDec Test */
/*\{*/

/** @def   HDRDEC_QTF_MAX_MSG_SIZE
    @brief The maximum message size of hdrdec_test task */
#define HDRDEC_QTF_MAX_MSG_SIZE   sizeof(hdrfw_msg_u)

/** @def   HDR_STUB_NUM_MSGS
    @brief The number of messages for qtf_tmc task */
#define HDRDEC_QTF_NUM_MSGS       4

/** @def   HDR_STUB_MSG_PRIORITY
    @brief The priority of messages for qtf_tmc task */
#define HDRDEC_QTF_MSG_PRIORITY   10

/** @def   HDRDEC_QTF_STACK_SIZE
    @brief The stack size of spawned tasks */
#define HDRDEC_QTF_STACK_SIZE 16400

/*\}*/

/* UMID registered in MSGR */
static msgr_umid_type hdrdec_test_registered_umids[] =  
{
  /* Dec */
  HDR_FW_DECOB_IND_RATE_CMD,
  HDR_FW_DECOB_UPDATE_CMD,
  HDR_FW_DECODE_CTL_CMD,
  HDR_FW_MIN_PREAM_THRESH_CFG_CMD,
};

/* Regsiter the test */
TF_REGISTER_TEST_CASE( COMPONENT, hdrdec_test );


void hdrdec_test_shutdown_complete_cb( void )
{

  TF_MSG ( "Decoder Shutdown Complete" );

}

void hdrdec_test::wait_for_msg( msgr_umid_type id )
{
  errno_enum_type        e_retval;
  uint32                 bytes_rcvd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do {

      e_retval = msgr_receive( &m_test_client, 
                               (uint8*)&fw_cmd, 
                               sizeof(hdrfw_msg_u), 
                               &bytes_rcvd );

      TF_ASSERT( e_retval == E_SUCCESS );

      TF_MSG ( "QTF received msg %s umid 0x%8x from src 0x%4x\n",
               msgr_get_umid_name ( fw_cmd.hdr.id ),
               fw_cmd.hdr.id, 
               fw_cmd.hdr.src );

  } while ( fw_cmd.hdr.id != id );

  TF_ASSERT( e_retval == E_SUCCESS );
  TF_ASSERT( fw_cmd.hdr.id == id );

} /* wait_for_msg */


void hdrdec_test::Setup()
{

  static const appmgr_service_init_entry_s  services[] = 
  {
    { "ATS", mcs_timer_init, QTHREAD_MAX_PRIORITY, mcs_timer_teardown },
  };

  errno_enum_type             e_status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the timer task is running */
  appmgr_test_init( MSGR_MCS_ATS,
                    NULL,       
                    0,
                    services,   
                    ARR_SIZE(services) );

  InitTestResources();

  /* Setup timeticks so we can call TF_SLEEP later */
  timetick_init();

  /* Register for messages */
  e_status = msgr_client_add_mq( "hdrdec_test_queue",
                                 &m_test_client,
                                 HDRDEC_QTF_MSG_PRIORITY, 
                                 HDRDEC_QTF_NUM_MSGS, 
                                 HDRDEC_QTF_MAX_MSG_SIZE,
                                 &m_queue_id );
  TF_ASSERT ( e_status == E_SUCCESS );

  e_status = msgr_register_block( MSGR_MCS_EXT_TEST,
                                  &m_test_client, 
                                  m_queue_id, 
                                  hdrdec_test_registered_umids, 
                                  sizeof( hdrdec_test_registered_umids ) / 4 );
  TF_ASSERT ( e_status == E_SUCCESS );

  TF_YIELD();
  TF_MSG ( "Spawning Tasks" );

  /* Initialize TMC and Start libraries */
  qtf_tmc_initialize();

  /* Spawn the HDRDEC task */
  qtf_tmc_spawn_rex_task ( "HDRDEC Task",
                           &hdrdec_tcb,
                           (unsigned char *)hdrdec_stack,
                           (HDRDEC_QTF_STACK_SIZE / sizeof(rex_stack_size_type)),
                           /* Why not use HDRDEC_STACK_SIZ from task.h? */
                           (rex_priority_type) HDRDEC_PRI,
                           hdrdec_task,
                           0L );

  /* Start the spawned threads */
  qtf_tmc_start_rex_task( &hdrdec_tcb );

  /* Initialize the shared memory pointer */
  hdrmdsp_shared_mem_interface = &shared_mem;

} /* Setup */

void hdrdec_test::Test()
{

  errno_enum_type        e_retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  TF_MSG ( "Enter Online Mode" );

  /*---------------------------------------------------------------------
     Enter HDR Online Mode */

  /* Set signal to HDR decoder task to enter HDR online mode */
  ( void ) rex_set_sigs ( &hdrdec_tcb, HDRDEC_ENTER_HDR_SIG );

  /* Yield to let HDR Decoder task to enter Online Mode */
  TF_YIELD();

  TF_MSG ( "Activate Decoder" );

  /* Activate decoder */
  hdrdec_activate();

  /*---------------------------------------------------------------------
     HDRDEC_ACTIVATE: Activate function sends the following two messages
        1. HDR_FW_DECOB_IND_RATE_CMD
        2. HDR_FW_DECOB_UPDATE_CMD */

  /* HDRDEC calls HDR_FW_DECOB_IND_RATE_CMD, wait for it  */
  wait_for_msg ( HDR_FW_DECOB_IND_RATE_CMD );

  TF_ASSERT ( 
    fw_cmd.decob_ind_rate.decobIndMsgRate == HDRDEC_IDLE_DECOB_MSG_RATE_V );

  /* HDRDEC calls HDR_FW_DECOB_UPDATE_CMD, wait for it  */
  wait_for_msg ( HDR_FW_DECOB_UPDATE_CMD );

  TF_ASSERT ( fw_cmd.decob_update.decobClear == 0x00FFFFFF );

  /* Decoder is in Idle Mode. Capable of decoding Control channel packets */

  /*---------------------------------------------------------------------
    ENTER TRAFFIC: Enters traffic mode
        1. HDR_FW_DECOB_IND_RATE_CMD
        2. HDR_FW_MIN_PREAM_THRESH_CFG_CMD */

  TF_MSG ( "Enter Traffic" );

  /* Enter Traffic Mode */
  hdrdec_enter_mode ( HDRDEC_TRAFFIC_MODE );

  /* HDRDEC calls HDR_FW_DECOB_IND_RATE_CMD, wait for it  */
  wait_for_msg ( HDR_FW_DECOB_IND_RATE_CMD );

  TF_ASSERT ( 
    fw_cmd.decob_ind_rate.decobIndMsgRate == HDRDEC_TRAFFIC_DECOB_MSG_RATE_V );

  /* HDRDEC calls HDR_FW_DECOB_IND_RATE_CMD, wait for it  */
  wait_for_msg ( HDR_FW_MIN_PREAM_THRESH_CFG_CMD );
  TF_ASSERT ( fw_cmd.min_pream_thresh_cfg.minPreambleThresh == 
              HDRDEC_MIN_PREAMBLE_THRESH_TRAFFIC );

  /*---------------------------------------------------------------------
     HDRDEC_DEACTIVATE: Deactivate function sends the following two 
     messages
        1. HDR_FW_DECODE_CTL_CMD
        2. Waits for HDR_FW_CANCEL_ACTIVE_PACKET_RSP and calls callback */

  TF_MSG ( "Deactivate Decoder" );

  hdrdec_deactivate ( hdrdec_test_shutdown_complete_cb );

  wait_for_msg ( HDR_FW_DECODE_CTL_CMD );
  TF_ASSERT ( fw_cmd.decode_ctl.cancelActivePackets == 1 );

  /* Send back Cancel active packet response */
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_FW, HDR_FW_CANCEL_ACTIVE_PACKET_RSP );
  e_retval = msgr_send ( &fw_cmd.hdr, sizeof( hdrfw_msg_rsp_u ) );
  TF_ASSERT ( e_retval == E_SUCCESS );


  TF_YIELD();
  /*---------------------------------------------------------------------
     Exit HDR Online Mode */

  TF_MSG ( "Exit Online Mode" );

  /* Set signal to HDR decoder task to exit HDR online mode */
  ( void ) rex_set_sigs ( &hdrdec_tcb, HDRDEC_EXIT_HDR_SIG );

} /* Test */


void hdrdec_test::Teardown()
{

  errno_enum_type               e_status;

  appmgr_service_init_entry_s  teardowns[] = {
     { "ATS", mcs_timer_init, QTHREAD_MAX_PRIORITY, mcs_timer_teardown },
  };

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  TF_MSG ( " In Teardown " );

  e_status = msgr_deregister_block( MSGR_MCS_EXT_TEST, 
                                    &m_test_client, 
                                    hdrdec_test_registered_umids, 
                                    sizeof( hdrdec_test_registered_umids ) / 4 );

  TF_ASSERT ( e_status == E_SUCCESS );


  appmgr_test_teardown ( 0, teardowns, ARR_SIZE(teardowns) );

  qtf_tmc_teardown();

  TF_YIELD();

  TeardownTestResources();

  TF_MSG ( " Test Successful" );

}
