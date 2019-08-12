/*! \file
*  
*  \brief  pm_malloc.c ----This file contains the implementation of pm_malloc()
*  \details This file contains the implementation of pm_malloc()
*  
*  &copy; Copyright 2012 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/framework/src/pm_utils.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/20/14   aab     Added pm_boot_adc_get_mv() 
06/11/13   hs      Adding settling time for regulators.
06/20/12   hs      Created

===========================================================================*/

/*===========================================================================

INCLUDE FILES 

===========================================================================*/
#include "pm_utils.h"
#include "DALSys.h"
#include "CoreVerify.h"
#include "busywait.h"


void pm_malloc(uint32 dwSize, void **ppMem)
{
    DALResult dalResult = DAL_SUCCESS;

    dalResult = DALSYS_Malloc(dwSize, ppMem);
    CORE_VERIFY(dalResult == DAL_SUCCESS );
    CORE_VERIFY_PTR(*ppMem);

    DALSYS_memset(*ppMem, 0, dwSize);
}

uint64 pm_convert_time_to_timetick(uint64 time_us)
{
    return (time_us*19200000)/1000000;
}

uint64 pm_convert_timetick_to_time(uint64 time_tick)
{

    return (time_tick*1000000)/19200000;
}



pm_err_flag_type
pm_boot_adc_get_mv(const char *pszInputName, uint32 *battery_voltage)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  AdcBootDeviceChannelType channel;
  AdcBootResultType adc_result;
  uint32 error = 0x0;
  DALResult adc_err;

  /*Initialize the ADC*/
  error = AdcBoot_Init();

  if (0 != error)
  {
    err_flag |= PM_ERR_FLAG__ADC_NOT_READY;
  }
  else
  {
    /*Get the channel from where the data is needed, Like ADC_INPUT_VBATT, ADC_INPUT_XO_THERM */
    adc_err = AdcBoot_GetChannel(pszInputName, &channel);
    if (DAL_SUCCESS != adc_err)
    {
      err_flag |= PM_ERR_FLAG__ADC_NOT_READY;
    }
    else
    {
      /*Read the Voltage of the Battery*/
      adc_err = AdcBoot_Read(&channel, &adc_result);
      if (DAL_SUCCESS != error)
      {
        err_flag |= PM_ERR_FLAG__ADC_NOT_READY;
      }
      /*Check for the result*/
      if (0 != adc_result.eStatus)
      {
        *battery_voltage = (uint32)(adc_result.nMicrovolts / 1000);
      }
      else
      {
        err_flag |= PM_ERR_FLAG__ADC_NOT_READY;
      }
    }
  }
  return err_flag;
}


pm_err_flag_type
pm_clk_busy_wait ( uint32 uS )
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  if ( uS > 0 )
  {  
     (void) DALSYS_BusyWait(uS);
  }
  else
  {
     err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
  }

  return err_flag;
}

