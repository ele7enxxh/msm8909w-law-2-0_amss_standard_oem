#ifndef HDRSRCH_TEST_TC_H
#define HDRSRCH_TEST_TC_H
/** @mainpage HDRSRCH_TEST_TC class
*   @file hdrsrch_test_tc.h
*   @brief This file contains the defintion of class
*          hdrsrch_test_tc. */

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
05/05/09   lyl     Initial Version

===========================================================================*/
#include "hdrsrch_test.h"
#include "hdrsrch_test_idle.h"

/** @class hdrsrch_test_tc @brief This is the base class for
 *  HDRSRCH TC tests. This
 *         is not a test case by itself.
 *   */

class hdrsrch_test_tc: public hdrsrch_test_idle {

	/**
     * This is the constructor of hdrsrch_test_tc.
	*/
    public:
    hdrsrch_test_tc( const string& type,
              const string& name,
              const string& path,
              const string& file )
    : hdrsrch_test_idle(type, name, path, file) { };

	/**
     * This is the test function for connected state. It calls
     * hdrsrch_test_acq::test_hdrsrch_acq(),
     * hdrsrch_test_sync::test_hdrsrch_sync(),
     * hdrsrch_test_idle::test_hdrsrch_idle(), and test_hdrsrch_tc
     * to enter traffic state.
	 */
    virtual void Test();

	/**
     * This function receives and prcoesses messages from HDR SRCH
     * and sends back responses at traffic state.
     * - Send a TCA cmd to trigger state change to connected state
	 */
    void test_hdrsrch_tc();

};

/** @class hdrsrch_test_tc_ho
 *  @brief This is the subclass of hdrsrch_test_tc. This class
 *         defines the test case for hard handoff in connected
 *         state.
 *   */
class hdrsrch_test_tc_ho: public hdrsrch_test_tc {

  	/**
     * This is the constructor of hdrsrch_test_tc_ho.
     */
    public:
    hdrsrch_test_tc_ho( const string& type,
              const string& name,
              const string& path,
              const string& file )
    : hdrsrch_test_tc(type, name, path, file) { };

	/**
     * This is the test function for handoff in connected state. It
     * calls hdrsrch_test_acq::test_hdrsrch_acq(),
     * hdrsrch_test_sync::test_hdrsrch_sync(),
     * hdrsrch_test_idle::test_hdrsrch_idle(),
     * hdrsrch_test_tc::test_hdrsrch_tc() and
     * test_hdrsrch_tc_ho() to do handoff. */

    virtual void Test();

	/**
     * This function receives and prcoesses messages from HDR SRCH
     * and sends back responses at connected state.
     * 
     * - Send a TCA cmd to trigger hard handoff.
     * - AT does a hard handoff.
     * - AT sends call back after TCA cmd is completed.
	 */
    void test_hdrsrch_tc_ho();

};

/** @class hdrsrch_test_tc_to_idle
 *  @brief This is the subclass of hdrsrch_test_tc. This class
 *         defines the test case for state transition to idle
 *         from connected state.
 *   */
class hdrsrch_test_tc_to_idle: public hdrsrch_test_tc {

  	/**
     * This is the constructor of hdrsrch_test_tc_to_idle.
     */
    public:
    hdrsrch_test_tc_to_idle( const string& type,
              const string& name,
              const string& path,
              const string& file )
    : hdrsrch_test_tc(type, name, path, file) { };

	/**
     * This is the test function for state transition to idle from
     * connected state. It calls
     * hdrsrch_test_acq::test_hdrsrch_acq(),
     * hdrsrch_test_sync::test_hdrsrch_sync(),
     * hdrsrch_test_idle::test_hdrsrch_idle(),
     * hdrsrch_test_tc::test_hdrsrch_tc() and
     * test_hdrsrch_tc_to_idle() to transfer to idle state. */

    virtual void Test();

	/**
     * This function receives and prcoesses messages from HDR SRCH
     * and sends back responses at connected state.
     * 
     * - Send a TCA cmd to trigger hard handoff.
     * - AT does a hard handoff.
     * - AT sends call back after TCA cmd is completed.
	 */
    void test_hdrsrch_tc_to_idle();


};


void tca_complete_cb( void );

void tc_ho_tca_complete_cb( void );

#endif /* HDRSRCH_TEST_TC_H */
