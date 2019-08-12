#ifndef __PMAPP_PWR_H__
#define __PMAPP_PWR_H__
/*===========================================================================


               P M A P P_ P W R  H E A D E R    F I L E

\details
    This file contains functions prototypes and variable/type/constant 
  declarations to support conversions between voltage values and PMIC
  register values
  
Copyright (c) 2013            by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pmapp_pwr.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/20/14   rk      Provide PMIC API in MPSS to set voltage for Vdd_MSS Rail (CR - 668036)
10/07/13   rh      File created
===========================================================================*/

#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/** @addtogroup pmapp_pwr
@{ */

/*=========================================================================== */
/*                     pmapp_pwr_set_vdd_mss                                  */
/*=========================================================================== */

/**
  This function sets the MSS rail to the voltage specified by the voltage input
  parameter (in uV)

  PARAMETERS
 * @param[in]  voltage: Voltage to set MSS (in uV)

  @return      pm_err_flag_type - PM_ERR_FLAG__SUCCESS if successful, otherwise
                                  appropriate PMIC error flag is returned

*/
pm_err_flag_type pmapp_pwr_set_vdd_mss(uint32 voltage);

/** @} */ /* end_addtogroup pmapp_pwr */

#endif /* __PMAPP_VSET_H__ */
