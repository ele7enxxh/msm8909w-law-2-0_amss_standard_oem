/*! \file
*  
*  \brief  pm_utils.c ----This file contains the implementation of utils functions.
*  \details This file contains the implementation of utils functions.
*  
*  &copy; Copyright 2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/utils/src/pm_utils.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/13   umr      Created

===========================================================================*/

/*===========================================================================

INCLUDE FILES 

===========================================================================*/
#include "pm_utils.h"

uint32
pm_utils_get_upper_idx(uint32 data_value, uint32 *data_list, uint32 list_count)
{
  uint32 idx = list_count - 1, i;

  /* Calc timer id */
  if (data_value == 0)
  {
    idx = 0;
  }
  else if (data_value >= data_list[list_count - 1])
  {
    idx = list_count - 1;
  }
  else
  {
    /* Room for improvement if the list gets too long */
    for (i = 0; i < list_count - 1; i++)
    {
      if (data_value > data_list[i] && data_value <= data_list[i+1])
      {
        idx = i+1;
        break;
      }
    }
  }

  return idx;
}

