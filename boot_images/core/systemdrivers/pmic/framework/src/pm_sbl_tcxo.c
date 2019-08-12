/*! \file pm_sbl_tcxo.c
*  \n
*  \brief 
*  \n
*  \n &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/framework/src/pm_sbl_tcxo.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/13   umr     Add support for TCXO config. 
========================================================================== */

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include "com_dtypes.h"
#include "AdcInputs.h"
#include "pm_utils.h"
#include "pm_sbl_boot.h"

#define PM_TCXO_THERM_MV   40

/* Temp function to check and configure TCXO */

pm_err_flag_type
pm_boot_tcxo_config (void)
{
   uint32                 xo_therm_mv = 0;
   pm_err_flag_type err = PM_ERR_FLAG__SUCCESS;
   
   err |= pm_boot_adc_get_mv(ADC_INPUT_XO_THERM_MV, &xo_therm_mv);

   if (PM_ERR_FLAG__SUCCESS == err)
   {
     if ((xo_therm_mv <= PM_TCXO_THERM_MV) && xo_therm_mv)
     {
       /* enables the 32KHz crystal oscillator */
       err |= pm_clk_tcxo_config ();
     }
   }
   else
   {
     err = PM_ERR_FLAG__ADC_SIG_NOT_SUPPORTED;
   }

   return err;
}
