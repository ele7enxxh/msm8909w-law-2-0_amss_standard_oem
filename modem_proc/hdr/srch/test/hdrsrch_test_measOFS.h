#ifndef HDRSRCH_TEST_MEASOFS_H
#define HDRSRCH_TEST_MEASOFS_H
/** @mainpage HDRSRCH_TEST_SYNC* class
*   @file hdrsrch_test_sync.h
*   @brief This file contains the defintion of class
*          hdrsrch_test_sync and hdrsrch_test_sync_slam_timeout. */

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
03/01/10   pxu     Initial Version

===========================================================================*/
#include "hdrsrch_test_timeSync.h"

/** @class hdrsrch_test_sync
 *  @brief This is the subclass of hdrsrch_test_acq and the base
 *         class for HDRSRCH sync tests. This class defines the
 *         test case for successful sync state.
 *   */
class hdrsrch_test_measOFS: public hdrsrch_test_timeSync {

	/**
     * This is the constructor of hdrsrch_test_sync.
	*/
    public:
    hdrsrch_test_measOFS( const string& type,
              const string& name,
              const string& path,
              const string& file )
    : hdrsrch_test_timeSync(type, name, path, file) { };

	/**
     * This is the test function for sync state. It calls
     * hdrsrch_test_acq::test_hdrsrch_acq() to enter sync state.
     * Then it calls test_hdrsrch_sync() for sync state tests.
	 */
    virtual void Test();

	/**
     * This function receives and prcoesses messages from HDR SRCH
     * and sends back responses. The responses include slam response
     * and finger assignment response. In the succcessful sync test
     * case, HDR SRCH enters idle state after this function returns.
	 */
    virtual void test_hdrsrch_measOFS();

  /** 
   * This function fills default data into the lte pilot 
   * measurement request. If anything needs to be changed later, 
   * we can manuallay assign individual fields. 
	 */	
	void prepare_lte_pilot_request();

};

class hdrsrch_test_measOFS_short_gap: public hdrsrch_test_measOFS {

	/**
     * This is the constructor of hdrsrch_test_sync.
	*/
    public:
    hdrsrch_test_measOFS_short_gap( const string& type,
              const string& name,
              const string& path,
              const string& file )
    : hdrsrch_test_measOFS(type, name, path, file) { };

	/**
     * This function receives and prcoesses messages from HDR SRCH
     * and sends back responses. The responses include slam response
     * and finger assignment response. In the succcessful sync test
     * case, HDR SRCH enters idle state after this function returns.
	 */
    virtual void test_hdrsrch_measOFS();
};

#endif
