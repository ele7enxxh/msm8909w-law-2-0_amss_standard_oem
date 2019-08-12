#ifndef HDRSRCH_TEST_IDLE_H
#define HDRSRCH_TEST_IDLE_H
/** @mainpage HDRSRCH_TEST_IDLE class
*   @file hdrsrch_test_idle.h
*   @brief This file contains the defintion of class
*          hdrsrch_test_idle. */

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
04/07/09   ljl     Initial Version

===========================================================================*/
#include "hdrsrch_test.h"
#include "hdrsrch_test_sync.h"

/** @class hdrsrch_test_idle
 *  @brief This is  the base class for HDRSRCH idle tests. This
 *         is not a test case by itself.
 *   */
class hdrsrch_test_idle: public hdrsrch_test_sync {

	/**
     * This is the constructor of hdrsrch_test_idle.
	*/
    public:
    hdrsrch_test_idle( const string& type,
              const string& name,
              const string& path,
              const string& file )
    : hdrsrch_test_sync(type, name, path, file) { };

	/**
     * This is the test function for idle state. It calls
     * hdrsrch_test_acq::test_hdrsrch_acq(),
     * hdrsrch_test_sync::test_hdrsrch_sync(), and test_hdrsrch_idle
     * to enter idle state.
	 */
    virtual void Test();

	/**
     * This function receives and prcoesses messages from HDR SRCH
     * and sends back responses at idle state. The subclasses can
     * define their own test_hdrsrch_idle() for different test
     * scenario. In this class, it does nothing.
	 */
    virtual void test_hdrsrch_idle();

};

/** @class hdrsrch_test_idle_ho
 *  @brief This is the subclass of hdrsrch_test_idle. This class defines the
 *         test case for idle handoff.
 *   */
class hdrsrch_test_idle_ho: public hdrsrch_test_idle {

  	/**
     * This is the constructor of hdrsrch_test_idle_ho.
     */
    public:
    hdrsrch_test_idle_ho( const string& type,
              const string& name,
              const string& path,
              const string& file )
    : hdrsrch_test_idle(type, name, path, file) { };


	/**
     * This function receives and prcoesses messages from HDR SRCH
     * and sends back responses at idle state.
     * 
     * - Add a nset pilot.
     * - Advances the time by 30ms and triggers a search request.
     * - Sends back search results that the newly added nset has
     *   very good energy and the current aset has very low energy
     *   which can trigger a handoff.
	 */
    void test_hdrsrch_idle( );
};
#endif /* HDRSRCH_TEST_SYNC_H */
