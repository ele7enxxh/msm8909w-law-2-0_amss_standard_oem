#ifndef PM_UTILS_H
#define PM_UTILS_H

/*! \file
 *  
 *  \brief  pm_utils.c ----This file contains the implementation of utils functions.
 *  \details This file contains the implementation of utils functions.
 *  
 *    &copy; Copyright 2013 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/utils/src/pm_utils.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/13   umr      Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "com_dtypes.h"

/*===========================================================================

                        GENERIC FUNCTION PROTOTYPES

===========================================================================*/
extern uint32
pm_utils_get_upper_idx(uint32 data_value, uint32 *data_list, uint32 list_count);

#endif // PM_UTILS_H
