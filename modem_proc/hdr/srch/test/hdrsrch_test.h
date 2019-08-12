#ifndef HDRSRCH_TEST_H
#define HDRSRCH_TEST_H

/** @mainpage HDRSRCH_TEST class
    @file hdrsrch_test.h
    @brief This file contains the defintion of class hdrsrch_test. */

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
03/10/10   pxu     Added various functions to accomodate script changes
12/11/09   grl     Added FW state config support
10/01/09   kss     Remove include of obsolete fw header files.
09/14/09   ljl     Supported srch rsp msg.
08/21/09   smd     Moved fs_public.h to extern "c" block.
07/29/09   etv     Included hdrfw_msg.h.
07/15/09   etv     Moved sw module definitions to hdrmsg.h
12/10/08   pmk     Initial Version

===========================================================================*/
#include "qtf.h"
#include "qtf_tmc.h"
#include <qtf_efs.h>

extern "C" {
//#include "qtf_efs.h"
#include <fs_public.h>
#include "rex.h"
#include "hdrmc.h"
#include "task.h"
#include "appmgr.h"
#include "mmoc.h"
#include "hdrsrch.h"
#include "hdrrx.h"
#include "hdrtx.h"
#include "hdrmc_v.h"
#include "hdrbctask.h"
#include "hdrsrchtask.h"
#include "hdrsrchstate.h"
#include "hdrdec.h"
#include "trm.h"
#include "tcxomgr.h"

#include "hdrmsg.h"
#include "hdrfw_msg.h"
#include "srchfw_msg.h"
#include "hdrsrchmsg.h"
#include "hdr_qtf_stubs.h"
#include "hdrsrchdrv.h"
//#define TESTFILE "/hdr"
#include "hdrsrchtypes.h"
#include "hdrmdspmcr.h"
#include "hdrtask.h"
#include "sd_v.h"
#include "fws.h"
#include "fws_msg.h"
}

/**
 *  @defgroup hdrsrchtest HDRSRCH Test */
/*\{*/

/** @def   HDR_STUB_MAX_MSG_SIZE
    @brief The maximum message size of qtf_tmc task */
#define HDR_STUB_MAX_MSG_SIZE   256

/** @def   HDR_STUB_NUM_MSGS
    @brief The number of messages for qtf_tmc task */
#define HDR_STUB_NUM_MSGS       30

/** @def   HDR_STUB_MSG_PRIORITY
    @brief The priority of messages for qtf_tmc task */
#define HDR_STUB_MSG_PRIORITY   10

/** @def   STACK_SIZE
    @brief The stack size of spawned tasks(HDRSRCH, HDRTX, HDRRX, and HDRMC etc */
#define STACK_SIZE 16400
/*\}*/

/** @class hdrsrch_test
 *  @brief This is the base class for
 *  HDRSRCH tests. This is not a test case
 *  */

class hdrsrch_test : public CTestFramework::CTestCase {         
    TF_DECLARE_TEST_CASE( hdrsrch_test );                         

  protected:                                                    
                                                              
    hdr_qtf_stub_msg_s                msg;
       /** messages containing stub messages not from srch4   */
    srch4_qtf_stub_hdr_msg_s          srch4Msg;
      /** message used by srch4 to coummunicate with the firmware   */
    hdrfw_msg_u                       hdrsrch_msg; 
      /** The message received from HDR SRCH */
    msgr_id_t                         hdr_stub_id;
      /** The message queue id of qtf_tmc task */
    hdrfw_shared_memory_interface_t   hdrmdsp_shared_mem_interface;
      /** The shared memory structure between qtf_tmc (which simulates firmware) and software */
    hdrfw_state_cfg_rsp_msg_t         hdr_state_cfg_rsp_msg;
      /** HDR state config response message */
    hdrfw_msg_rsp_u                   hdrfw_msg_rsp;
      /** Reponse message to HDR SRCH */
    srchfw_msg_rsp_u                  srch_rsp_msg;
      /* Srch result response */
    hdrsrch_tran_sys_time_req_msg_t   sys_time_req;
      /* LTE to DO system time requet message*/
  	hdrsrch_lte_pilot_meas_req_msg_t  pilot_meas_req;
     /* LTE to DO pilot measurement requet message*/
    fws_app_config_rsp_t              fws_app_rsp;
     /* response to fws */
    fws_shutdown_spr_t                fws_shutdown;
     /* fws shutdown command */
	hdrsrch_state_enum_type           last_state;
	 /* last state machine state */

  public:           

    /**
    * Set up the test case. This function does the following:
    * Spawn the tasks, register to receive the messages from HDR
    * SRCH, process the messages to enable HDR App and configure
    * indications during AMSS bootup. The subclasses can define
    * their own setup functions.
    */
    virtual void Setup();

    /**
    * This is the test fucntion which performs the tests. The
    * subclasses can define their own test functions. For example,
    * in HDR SRCH acquisition test (class hdrsrch_test_acq), Test()
    * processes the finerg assignments, pullin and slam messages.
    */
    virtual void Test();

    /**
     * This function cleans up the test. The subclasses can define
     * their own TearDown() functions.
     */
    virtual void Teardown();

    /**
     * This function waits for a message with the specific UMID. It
     * discards the messages with other UMIDs until the specified
     * message is received.
     * @param id The UMID to wait for,
     */
    void wait_for_msg( msgr_umid_type id );

    /**
     * This function waits for a message with the specific msg ID. It
     * discards the messages with other msg id until the specified
     * message is received.
     * @param msg The msg to receive,
     *        msg_id The msg ID to wait for
     */
    void wait_for_qtf_stub_msg( hdr_qtf_stub_msg_s *msg, hdr_qtf_msg_e  msg_id );
    /* overload the function to allow srch4 msgs to be passed in */
    void wait_for_qtf_stub_msg( srch4_qtf_stub_hdr_msg_s *msg, srch4_qtf_stub_hdr_msg_e msg_id );

    /**
     * This function fills in search result data into the msg passed
     * in as parameter 
     * @param msg The msg to receive
     */
    void fill_in_srch_data ( srch4_qtf_stub_hdr_msg_s *msg );

    /**
     * This function intializes the HDR state config response
     * message. It sets the firmware version and the address of the
     * shared memory structure. It does not set the enbable flag
     * which should be set individually for the response.
     */
    void init_hdr_state_cfg_rsp_msg( );

    /**
     * This function receives and processes the messages from HDR
     * SRCH when it is deactivated and when sync was the last state to be processed
     */
    void test_hdrsrch_deactivate ();

    /**
     * This function sends srch rsp msg.
     */
    void send_srch_rsp();

    /**
     * This function waits for the start search message, and fills 
     * out fields in the response with a set of default values, then
     * sends a response message back 
     */
    void process_srch_msg( srch4_qtf_stub_hdr_msg_s *msg );

    /**
     * initilizes a msg header, sends it, and checks if it's been 
     * sent successfully 
     */
    void send_msg
    (
      msgr_hdr_struct_type   *hdr,     /*!< Header to initialize */
      msgr_tech_module_type   from,    /*!< Module that will be sending the msg */
      msgr_umid_type          umid,     /*!< UMID of the message to be sent*/
      uint16                  msgSize  /*!< size of the msg to be sent*/
    );
  };                                                  

#endif /* HDRSRCH_TEST_H */
