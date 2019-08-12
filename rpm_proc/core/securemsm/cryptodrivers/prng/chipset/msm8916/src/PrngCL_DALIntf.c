/**
@file PrngCL_DALIntf.c 
@brief Prng Engine Core Library source file 
*/

/*===========================================================================

                       P R N G E n g i n e D r i v e r

                       S o u r c e  F i l e (e x t e r n a l)

DESCRIPTION
  This header file contains HW PRNG DAL interface specific declarations.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.1/core/securemsm/cryptodrivers/prng/chipset/msm8916/src/PrngCL_DALIntf.c#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $ 

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2011-09-05   nk      Initial version
============================================================================*/

#include "comdef.h"
#include "DALDeviceId.h"
#include "Clock.h"
#include "PrngCL_DALIntf.h"
#include "PrngCL.h"

uint32 prng_clk_init = NULL;
ClockIdType      nClockID;
/**
 * @brief   This function enables the PRNG Engine Clock 
 *          The function uses DAL interface to enable clock.
 *
 * @param None
 *
 * @return Prng_Result_Type
 *
 * @see PrngCL_DAL_Clock_Disable
 */

PrngCL_Result_Type PrngCL_DAL_Clock_Enable( void )
{

  DALResult           eResult;
  PrngCL_Result_Type    ePrngStatus = PRNGCL_ERROR_NONE;
  
  if (prng_clk_init == 0)
  {
    /* Get the clock ID */
    eResult = Clock_GetClockId("gcc_prng_ahb_clk", &nClockID);
    if(eResult != DAL_SUCCESS)
    {
      ePrngStatus = PRNGCL_ERROR_FAILED;
     return ePrngStatus;
    }

    eResult = Clock_EnableClock(nClockID);
    if(eResult != DAL_SUCCESS)
    {
      ePrngStatus = PRNGCL_ERROR_FAILED;
      return ePrngStatus;
    }
  }
  prng_clk_init++;

  return ePrngStatus;
}

/**
 * @brief   This function disables the PRNG Engine Clock 
 *          The function uses DAL interface to disable clock.
 *
 * @param None
 *
 * @return Prng_Result_Type
 *
 * @see PrngEL_DAL_Clock_Enable
 */
PrngCL_Result_Type PrngCL_DAL_Clock_Disable( void )
{
  DALResult           eResult;
  PrngCL_Result_Type    ePrngStatus = PRNGCL_ERROR_NONE;
  
  if(prng_clk_init!=0)
  {
    prng_clk_init--;
  }

  if (prng_clk_init == 0)
  {
    eResult = Clock_DisableClock(nClockID);
    if(eResult != DAL_SUCCESS)
    {
      ePrngStatus = PRNGCL_ERROR_FAILED;
      return ePrngStatus;
    }
  }

  return ePrngStatus;
}

