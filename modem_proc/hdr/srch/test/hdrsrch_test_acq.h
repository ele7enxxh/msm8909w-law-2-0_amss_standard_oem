#ifndef HDRSRCH_TEST_ACQ_H
#define HDRSRCH_TEST_ACQ_H
/** @mainpage HDRSRCH_TEST_ACQ* class
*   @file hdrsrch_test_acq.h
*   @brief This file contains the defintion of class
*          hdrsrch_test_acq, hdrsrch_test_afc_pullin_timeout, and
*          hdrsrch_test_afc_pullin_fail. */

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
03/10/10   pxu     Modified functions to accodmodate script change
04/07/09   ljl     Initial Version

===========================================================================*/

#include "hdrsrch_test.h"

/** @class hdrsrch_test_acq
 *  @brief This is the subclass of hdrsrch_test and the base
 *         class for HDRSRCH acquisition tests. This is not a
 *         test case by itself.
 *   */
class hdrsrch_test_acq: public hdrsrch_test {
    public:

    /**
     * This is the constructor of hdrsrch_test_acq class. */
    hdrsrch_test_acq( const string& type,
              const string& name,
              const string& path,
              const string& file )
    : hdrsrch_test(type, name, path, file) { };

    /**
     * This is the generic test function for acquisition state. It
     * receives and processes the messages from HDR SRCH and sends
     * the responses. It is called by the test case of successful
     * acquisition where HDR SRCH will succeed acquisition and enter
     * sync state. Currently, the response messages are finger
     * assignment and HDR App enable responeses. It also processes
     * the search tasks submitted by HDR SRCH and sends back the
     * search dump. This will be changed when srch4 APIs are
     * available. The results of this acqusition will vary depending 
     * on which test calls it. 
     *  
     */
    virtual void test_hdrsrch_acq( );

    /**
     * This is the test function for a successful acqusition 
     *  
     */
    virtual void test_acq_success( );
};

/** @class hdrsrch_test_acq_success
 *  @brief This is class for HDRSRCH acquisition success test.
 *         It defines the test case of the successful
 *         acquisition.
 *  */
class hdrsrch_test_acq_success: public hdrsrch_test_acq {
  public:

    /**
     * This is the constructor.
     *  */
    hdrsrch_test_acq_success( const string& type,
                              const string& name,
                              const string& path,
                              const string& file )
      : hdrsrch_test_acq(type, name, path, file) { };

   /**
     * This is the test function. It calls the function
     * test_hdrsrch_acq(). 
     */
    virtual void Test();

    /**
     * This is the test function the for successful aquisition test
     * case. It calls the member function test_hdrsrch_acq() of the
     * class hdrsrch_test_acq and processes the search requests when
     * HDR SRCH enters sync state after acquisition succeeds.
     */

};

/** @class hdrsrch_test_afc_pullin_fail
 *  @brief This is the subclass of hdrsrch_test_acq. It defines
 *         the test case of the afc pullin failure during
 *         acquisition.
 *  */
class hdrsrch_test_afc_pullin_fail: public hdrsrch_test_acq {

    /**
     * This is the constructor of hdrsrch_test_afc_pullin_fail.
     */
    public:
    hdrsrch_test_afc_pullin_fail( const string& type,
              const string& name,
              const string& path,
              const string& file )
    : hdrsrch_test_acq(type, name, path, file) { };

    /**
     * This is the test function. It calls the function
     * test_hdrsrch_acq(). 
     */
    virtual void Test();

    /**
     * This fucntion provides the test procedure for the afc pullin
     * failure test case. It is called by Test(). It receives and
     * processes the messages from HDR SRCH and sends back response.
     * When it receives acq start message from HDR SRCH AFC, it
     * sends back acq failure response. This causes HDR SRCH to fail
     * pullin.
     */


};

/** @class hdrsrch_test_afc_pullin_timeout
 *  @brief This is the subclass of hdrsrch_test_acq. It defines
 *         the test case of the afc pullin timeout during
 *         acquisition.
 *  */
class hdrsrch_test_afc_pullin_timeout: public hdrsrch_test_acq {

    /**
     * This is the constructor of hdrsrch_test_afc_pullin_timeout.
     */
    public:
    hdrsrch_test_afc_pullin_timeout( const string& type,
              const string& name,
              const string& path,
              const string& file )
    : hdrsrch_test_acq(type, name, path, file) { };

    /**
     * This is the test function. It calls the function
     * test_hdrsrch_acq(). 
     */
    virtual void Test();

    /**
     * This fucntion provides the test procedure for the afc pullin
     * failure test case. It is called by Test(). It receives and
     * processes the messages from HDR SRCH and sends back response.
     * When it receives acq start message from HDR SRCH AFC, it does
     * not send back the afc acq response. This causes HDR SRCH AFC
     * to timeout and fail pullin.
     */

};

class hdrsrch_test_acq_slam_timeout: public hdrsrch_test_acq {
    /*TF_DECLARE_TEST_CASE( hdrsrch_test_afc_pullin_fail ); */
    public:
    hdrsrch_test_acq_slam_timeout( const string& type,
              const string& name,
              const string& path,
              const string& file )
    : hdrsrch_test_acq(type, name, path, file) { };

    /**
     * This is the test function. It calls the function
     * test_hdrsrch_acq(). 
     */
    virtual void Test();
};

#endif /* HDRSRCH_TEST_ACQ_H */
