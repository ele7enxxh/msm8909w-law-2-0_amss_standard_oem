#ifndef HDRDEC_TEST_H
#define HDRDEC_TEST_H
/** @mainpage HDRDEC_TEST class
    @file hdrdec_test.h
    @brief This file contains the defintion of class hdrdec_test. */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  H D R   D E C O D E R   T E S T   M O D U L E

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/04/09   etv     Initial Version

===========================================================================*/

#include "qtf.h"
#include "qtf_tmc.h"
#include <qtf_efs.h>

extern "C" {
#include "rex.h"
#include "task.h"
#include "appmgr.h"
#include "hdrdec.h"
#include "hdrmsg.h"
#include "hdrfw_msg.h"
#include "hdrmdspmcr.h"
#include "hdrtask.h"
}

/**
    @defgroup hdrdectest HDR Decoder Test Module */
/*\{*/

/** @class hdrdec_test
    @brief This is the class for all HDR Decoder tests. */

class hdrdec_test : public CTestFramework::CTestCase {         

  TF_DECLARE_TEST_CASE( hdrdec_test );                         

  protected:                                                    
                                                              
  hdrfw_msg_u fw_cmd;
    /**< The message received from HDR Module */

  msgr_id_t m_queue_id;
    /**< The message queue id of qtf_tmc task */

  hdrfw_shared_memory_interface_t shared_mem;
    /**< The shared memory structure between qtf_tmc (which simulates
         firmware) and software */

  public:           

  /** Set up the test case. This function does the following:
      Spawn the tasks, register to receive the messages from
      HDR Module, process the messages to enable HDR App and
      configure indications during AMSS bootup. The subclasses
      can define their own setup functions. */
  void Setup ( void );

  /** This is the test fucntion which performs the tests. The
      subclasses can define their own test functions. For example,
      in HDR SRCH acquisition test (class hdrsrch_test_acq), Test()
      processes the finerg assignments, pullin and slam messages. */
  void Test ( void );

  /** This function cleans up the test. The subclasses can define
      their own TearDown() functions. */
  void Teardown ( void );

  /** This function is used to wait for a specified message */
  void wait_for_msg
  ( 
    msgr_umid_type id 
      /**< UMID of the message to wait for */
  );

};                                                  

/*\}*/

#endif /* HDRDEC_TEST_H */
