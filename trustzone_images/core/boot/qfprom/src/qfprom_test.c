/*=============================================================================

                                qfprom_test.c

GENERAL DESCRIPTION
  This file contains the function definition for invoking Qfprom Test Framework.

Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*==============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who        what, where, why
--------   ---        ----------------------------------------------------------
06/14/14   lm         Added mapping & unmapping of security control core memory region
03/20/14   ck         Initial version for Bear family
==============================================================================*/


/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/
#include "qfprom_test.h"
#include "qfprom.h"
#include QFPROM_TARGET_INCLUDE_H
#include QFPROM_HWIO_REG_INCLUDE_H
#include "tzbsp_mem.h"

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

  /* These variables are modified by Trace32 */
  
static volatile boolean test_single_fuse_blow   = FALSE;
static volatile boolean test_multiple_fuse_blow = FALSE;
static volatile boolean test_read_fuse          = FALSE;
static volatile boolean qfprom_test_run_always  = FALSE;

static volatile struct boot_qfprom_test_data_type boot_qfprom_test_data; 

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
*   Needs to be involded before TZ_SBL_END_MILESTONE service request
*   (that locks out qfprom driver in TZ).
*
*   @par Side Effects
*   None
*
*   @sa
*   None
*/
static void boot_qfprom_single_test()
{
  int i=0;
  boot_qfprom_test_data.qfprom_service_api_status = QFPROM_NO_ERR;
  
  
  /* Blow single  Qfprom fuse */
  if(test_single_fuse_blow) 
  {
    boot_qfprom_test_data.qfprom_service_api_status =
      qfprom_write_row(boot_qfprom_test_data.qfprom_fuse_addr + QFPROM_SANDBOX_RELOCATE_OFFSET, 
                       (uint32 *)boot_qfprom_test_data.qfprom_fuse_data,
                       0);
  }
  
  /* Blow array of  Qfprom fuse */
  if(test_multiple_fuse_blow &&
    (qfprom_test_array_length == QFPROM_BLOW_ARRAY_MAX_LEN)) 
  {
    /* adding the offset to correct the address temporary */ 
    for(i = 0; i< QFPROM_BLOW_ARRAY_MAX_LEN ;i++)
	{
	   if( boot_qfprom_test_data.qfprom_test_fuse_array[i].raw_row_address != NULL)
	   {
	     boot_qfprom_test_data.qfprom_test_fuse_array[i].raw_row_address += QFPROM_SANDBOX_RELOCATE_OFFSET;
	   }
	}
    boot_qfprom_test_data.qfprom_service_api_status =
      qfprom_write_multiple_rows(&boot_qfprom_test_data.qfprom_test_fuse_array,
                                 0);
	 for(i = 0; i< QFPROM_BLOW_ARRAY_MAX_LEN ;i++)
	{
	   if( boot_qfprom_test_data.qfprom_test_fuse_array[i].raw_row_address != NULL)
	   {
	     boot_qfprom_test_data.qfprom_test_fuse_array[i].raw_row_address -= QFPROM_SANDBOX_RELOCATE_OFFSET;
	   }
	}			
  }
  
  /* Read  Qfprom fuse */
  if(test_read_fuse) 
  {
    boot_qfprom_test_data.qfprom_service_api_status =
      qfprom_read_row(boot_qfprom_test_data.qfprom_fuse_addr + QFPROM_SANDBOX_RELOCATE_OFFSET, 
                      boot_qfprom_test_data.addr_type, 
                      (uint32 *)boot_qfprom_test_data.qfprom_fuse_data);
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
*   Needs to be involded before TZ_SBL_END_MILESTONE service request
*   (that locks out qfprom driver in TZ).
*
*   @par Side Effects
*   None
*
*   @sa
*   None
*/
void boot_qfprom_test()
{
  tzbsp_mem_map_all_devices();
  do
  {
    /* loop as long as the test script sets the run always flag to do so */
    boot_qfprom_single_test();
  }while(qfprom_test_run_always == TRUE);
  tzbsp_mem_unmap_all_devices();
}  /* qfprom_test */

