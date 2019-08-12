#ifndef __FLASH_TEST_BENCHMARK_H__
#define __FLASH_TEST_BENCHMARK_H__

/**********************************************************************
 * flash_test_benchmark.h
 *
 * Public interface declaration for the Flash Drivers.
 *
 * This file describes the Flash external interface for the Flash Test 
 * Framework 
 *
 *       Copyright (c) 2009 Qualcomm Technologies Incorporated. 
 *               All Rights Reserved.
 *         Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 11/09/09     mc      Initial version
 *=======================================================================*/

/* This function is called by the fs_benchmark to begin the flash tests */
int flash_test_start(int32 *);

#endif
