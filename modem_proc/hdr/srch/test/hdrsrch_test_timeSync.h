
#ifndef HDRSRCH_TEST_SYNC_H
#define HDRSRCH_TEST_SYNC_H
/** @mainpage HDRSRCH_TEST_SYNC* class
*   @file hdrsrch_test_timeSync.h
*   @brief This file contains the defintion of class
*          hdrsrch_test_timeSync.c */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               1 X  C A L L  P R O C E S S I N G   T E S T  

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/20/10   pxu     Initial Version

===========================================================================*/
#include "hdrsrch_test.h"

/** @class hdrsrch_test_timeSync
 *  @brief This is the subclass of hdrsrch_test and the base
 *         class for HDRSRCH timeSync tests. This class defines
 *         the test case for successful timeSync state.
 *   */
class hdrsrch_test_timeSync: public hdrsrch_test {

	/**
     * This is the constructor of hdrsrch_test_timeSync.
	*/
    public:
    hdrsrch_test_timeSync( const string& type,
              const string& name,
              const string& path,
              const string& file )
    : hdrsrch_test(type, name, path, file) { };

	/**
     * This is the test function for sync state. It calls
     * hdrsrch_test_timeSync::test_hdrsrch_timeSync() to enter 
     * timeSync state. Then it calls test_hdrsrch_timeSync() for 
     * timeSync state tests. 
	 */
    virtual void Test();

	/**
     * This function receives and prcoesses messages from HDR SRCH
     * and sends back responses. The responses include slam response
     * and finger assignment response. In the succcessful sync test
     * case, HDR SRCH enters idle state after this function returns.
	 */
    virtual void test_hdrsrch_timeSync();

};

#endif /* HDRSRCH_TEST_TIMESYNC_H */
