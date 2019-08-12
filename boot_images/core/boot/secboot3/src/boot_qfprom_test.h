#ifndef BOOT_QFPROM_TEST_H
#define BOOT_QFPROM_TEST_H


/*=============================================================================

                                boot_qfprom_test.h

GENERAL DESCRIPTION
  This file contains the function header for invoking Qfprom Test Framework.

Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*==============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_qfprom_test.h#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

when       who        what, where, why
--------   ---        ----------------------------------------------------------
06/26/12   dh         Port to badger family
10/19/11   kedara     Initial version.
==============================================================================*/


/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/
#include "boot_comdef.h"

#define QFPROM_BLOW_ARRAY_MAX_LEN  11

struct write_row_type
{
    /* Row address */
    uint32   raw_row_address;

    /* MSB row data */
    uint32   row_data_msb;
    
    /* LSB row data */
    uint32   row_data_lsb;
} ;

struct boot_qfprom_test_data_type
{
  uint32 qfprom_service_api_status;
  uint32 addr_type ;
  uint32 qfprom_fuse_addr;
  uint32 qfprom_fuse_data[2];
  struct write_row_type qfprom_test_fuse_array[QFPROM_BLOW_ARRAY_MAX_LEN];
};


/*==============================================================================

                          PUBLIC FUNCTION DECLARATIONS

==============================================================================*/

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
*   Needs to be invoked before boot_fastcall_tz_2arg  thats makes a 
*  TZ_SBL_END_MILESTONE service request to TZ  (that locks out
* qfprom driver in TZ).
*
*   @par Side Effects
*   None
*
*   @sa
*   None
*/
void boot_qfprom_test(void);


#endif /* BOOT_QFPROM_TEST_H */
