/*===========================================================================

                       QFPROM Test Source Code

DESCRIPTION
 Contains defintions and APIs to be used when testing Qfprom
 driver.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014 by Qualcomm Technologies , Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/boot/qfprom/src/qfprom_test.c#1 $

when         who                    what, where, why
--------     ---      ----------------------------------------------------------
03/27/14     ck       Updated test fuse region for Bear family
12/05/12     kedara   Initial creation

============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "qfprom.h"         /* qfprom driver APIs */
#include QFPROM_TARGET_INCLUDE_H  /* qfprom intialization APIs */
#include "qfprom_test.h"    /* qfprom test header file */

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/



/*===========================================================================

FUNCTION QFPROM_TEST

DESCRIPTION
  This function tests the qfprom driver.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qfprom_test ( void )
{
  uint32 ret_code = QFPROM_ERR_UNKNOWN;
  uint32   raw_row_address;
  uint32 row_data[2]={0};
  volatile uint32 test_loop = 1;
  
  raw_row_address = (uint32) 0x000581F8; /*Spare region 19 */
  do
  {
    // Test loop
    while(test_loop == 1);  

    /* Read fuse value */
    ret_code = qfprom_read_row(raw_row_address,
                                QFPROM_ADDR_SPACE_RAW,
                                row_data);

    if (ret_code != QFPROM_NO_ERR )
    {
      break;
    }
    
    /* Blow fuse */
    ret_code = qfprom_write_row(raw_row_address,
                             row_data,
                             0);
                             
    if (ret_code != QFPROM_NO_ERR )
    {
      break;
    }
      
  }while(0);

  // Test loop
  while(test_loop == 1);    
  
} /* qfprom_test */

