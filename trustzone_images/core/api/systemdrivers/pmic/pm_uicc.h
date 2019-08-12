#ifndef __PM_UICC_H__
#define __PM_UICC_H__
/*===========================================================================


               P M _ U I C C  H E A D E R    F I L E

\details
    This file contains functions prototypes and variable/type/constant 
  declarations to support the HOTSWAP (UICC) feature inside the Qualcomm 
  PMIC chips.
  
Copyright (c) 2011            by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_uicc.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/24/11   dy      Add API to set hotswap polarity
09/14/11   dy      Created.
===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

typedef enum
{
  PM_UICC_POLARITY_ACTIVE_LOW,
  PM_UICC_POLARITY_ACTIVE_HIGH,
  PM_UICC_POLARITY_INVALID,
} pm_uicc_polarity_type;

/*===========================================================================

                 UICC DRIVER FUNCTION PROTOTYPES

===========================================================================*/

/*=========================================================================== */
/*                        pm_uicc_cntrl_enable                                */
/*=========================================================================== */

/**
  This function is used to enable/disable the UICC Hotswap module

  PARAMETERS
  externalResourceIndex - PM_UICC_1 or PM_UICC_2 for the two slots  
  boolean enable        - TRUE to enable, FALSE to disable

  @return      pm_err_flag_type - PM_ERR_FLAG__SUCCESS on success

*/
pm_err_flag_type pm_uicc_cntrl_enable(int externalResourceIndex, boolean enable);

/**
  This function is used to set the UICC Hotswap polarity

  PARAMETERS
  externalResourceIndex - PM_UICC_1 or PM_UICC_2 for the two slots  
  polarity -            - PM_UICC_POLARITY_ACTIVE_LOW or PM_UICC_POLARITY_ACTIVE_HIGH

  @return      pm_err_flag_type - PM_ERR_FLAG__SUCCESS on success

*/
pm_err_flag_type pm_uicc_set_polarity(int externalResourceIndex, pm_uicc_polarity_type polarity);

#endif /* __PM_UICC_H__ */
