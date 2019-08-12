/*=============================================================================

                                boot_qfprom_test.c

GENERAL DESCRIPTION
  This file contains the function definition for invoking Qfprom Test Framework.

Copyright 2012-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*==============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_qfprom_test.c#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

when       who        what, where, why
--------   ---        ----------------------------------------------------------
11/27/12   dh         remove modification of addr_type in read test
10/29/12   dh         Add boot_qfprom_test_data to BOOT_UNCACHED_DDR_ZI_ZONE
06/26/12   dh         Port to badger family
10/15/10   kedara     Initial version.
==============================================================================*/


/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/
#include "boot_qfprom_test.h"
#include "boot_fastcall_tz.h"
#include "IxErrno.h"
#include "boot_error_handler.h"
#include "qfprom.h"

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

  /* These variables are modified by Trace32 */
  
static volatile uint32 qfprom_fastcall_status =E_SUCCESS;
static volatile boolean test_single_fuse_blow = FALSE;
static volatile boolean test_multiple_fuse_blow = FALSE;
static volatile boolean test_read_fuse = FALSE;
static volatile boolean qfprom_test_run_always = FALSE;

#pragma arm section zidata = "BOOT_UNCACHED_DDR_ZI_ZONE"
/*boot_qfprom_test_data must be allocated in DDR since TZ can't address L2 TCM */
static volatile struct boot_qfprom_test_data_type boot_qfprom_test_data; 
#pragma arm section


static volatile  uint32 qfprom_test_array_length = 
            (sizeof(boot_qfprom_test_data.qfprom_test_fuse_array)/sizeof(struct write_row_type));
            
            
/* =============================================================================
**  Function : boot_qfprom_single_test
** =============================================================================
*/
/*!
*   @brief
*   This function invokes Qfprom Test Api's one time.
*
*
*   @return
*   None
*
*   @par Dependencies
*  Needs to be invoked before boot_fastcall_tz_2arg  thats makes a 
*  TZ_SBL_END_MILESTONE service request to TZ  (that locks out
*  qfprom driver in TZ).
*
*   @par Side Effects
*   None
*
*   @sa
*   None
*/
static void boot_qfprom_single_test()
{
  boot_qfprom_test_data.qfprom_service_api_status = QFPROM_NO_ERR;
  /* Blow single  Qfprom fuse */
  if(test_single_fuse_blow) 
  {
    qfprom_fastcall_status= boot_fastcall_tz_4arg (TZ_WRITE_FUSE_CMD, 
                              boot_qfprom_test_data.qfprom_fuse_addr, 
                              &boot_qfprom_test_data.qfprom_fuse_data, 
                              0,
                              &boot_qfprom_test_data.qfprom_service_api_status);
  }
  
  /* Blow array of  Qfprom fuse */
  if(test_multiple_fuse_blow &&
    (qfprom_test_array_length == QFPROM_BLOW_ARRAY_MAX_LEN)) 
  {
    qfprom_fastcall_status= boot_fastcall_tz_4arg (TZ_WRITE_MULT_FUSE_CMD, 
                              &boot_qfprom_test_data.qfprom_test_fuse_array, 
                              sizeof(boot_qfprom_test_data.qfprom_test_fuse_array),
                              0,
                              &boot_qfprom_test_data.qfprom_service_api_status);
  }
  
  /* Read  Qfprom fuse */
  if(test_read_fuse) 
  {
    qfprom_fastcall_status= boot_fastcall_tz_4arg (TZ_READ_FUSE_CMD, 
                              boot_qfprom_test_data.qfprom_fuse_addr, 
                              boot_qfprom_test_data.addr_type, 
                              &boot_qfprom_test_data.qfprom_fuse_data,
                              &boot_qfprom_test_data.qfprom_service_api_status);
  }
} /* boot_qfprom_single_test */


/* =============================================================================
**  Function : boot_qfprom_test
** =============================================================================
*/
/*!
*   @brief
*   This function invokes Qfprom Test Framework.
*
*
*   @return
*   None
*
*   @par Dependencies
*  Needs to be invoked before boot_fastcall_tz_2arg  thats makes a 
*  TZ_SBL_END_MILESTONE service request to TZ  (that locks out
*  qfprom driver in TZ).
*
*   @par Side Effects
*   None
*
*   @sa
*   None
*/
void boot_qfprom_test()
{
  do
  {
    /* loop as long as the test script sets the run always flag to do so */
    boot_qfprom_single_test();
  }while(qfprom_test_run_always == TRUE);

  /* Check if blow operation went ok. */
  BL_VERIFY(((boot_qfprom_test_data.qfprom_service_api_status == QFPROM_NO_ERR) &&
              (qfprom_fastcall_status == E_SUCCESS)),
              BL_ERR_QFPROM_TEST_FRAMEWORK_FAIL );  

}/*boot_qfprom_test*/

