/** @file ClockTlmmSpare.c
   
  This file is used for clearing control of TLMM_SPARE over GPIO 93.

  Copyright (c) 2014, Qualcomm Technologies Inc. All rights reserved.
  
**/
/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/clock/hw/msm8909w/src/ClockTLMMSpare.c#1 $ 
  $DateTime: 2015/10/01 02:54:06 $ 
  $Author: pwbldsvc $ 

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 18/02/14   mkumari     Initial version

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DDIChipInfo.h"
#include "ClockHWIO.h"
#include "DDITlmm.h"
#include "DDIChipInfo.h"


/* ========================================================================
**  Function : Clock_GetChipId()
** ======================================================================*/
/*
    Description: Get chip id type

    @param   None
    @return
    TRUE  -- Configuration was success.
    FALSE -- Configuration failed.

    @dependencies
    None.

    @sa None
*/

DalChipInfoIdType Clock_GetChipId(void)
{
    return DalChipInfo_ChipId();
}
/* ========================================================================
**  Function : Clock_ExtBuck_GPIO_Misc
** ======================================================================*/
/*
    Description: Configure GPIO 111 to OUTPUT PULL UP HIGH
                 and set the TLMM_SPARE register for 8916 and 8936

    @param   None
    @return
    TRUE  -- Configuration was success.
    FALSE -- Configuration failed.

    @dependencies
    None.

    @sa None
*/

boolean Clock_ExtBuckGPIOMisc(void)
{
  DALResult eResult = DAL_ERROR;
  uint32 cfg_gpio, nVal = 0;
  volatile uint32 dummy_read;
  DalDeviceHandle* hTlmmDriver = NULL;
  
  eResult = DAL_DeviceAttach(DALDEVICEID_TLMM, &hTlmmDriver);
  if (DAL_SUCCESS == eResult)
  {
    /*Set TLMM_SPARE register*/
    nVal = HWIO_IN(TLMM_SPARE);
    nVal |= 0x1;
    HWIO_OUT(TLMM_SPARE, nVal);
      
    /*
     * Put GPIO111 in low power config.
     */
    cfg_gpio  = DAL_GPIO_CFG_OUT( 0x5D,0x0,0x1,0x3, 0x0,0x1 );
  
    eResult = DalTlmm_ConfigGpio(hTlmmDriver, cfg_gpio, DAL_TLMM_GPIO_DISABLE);

    DAL_DeviceDetach(hTlmmDriver);

    if(DAL_SUCCESS == eResult)
    {
      return(TRUE);
    }
    return(FALSE);
#if 0
    /*
     * To avoid glitches while switching from SPARE Control to GPIO OUT 
     * control configuring the pad pull to PULL UP.
     * Configuring GPIOs 111 to OUTPUT, PULL UP, HIGH
     */
    cfg_111  = DAL_GPIO_CFG_OUT( 0x6F,0x0,0x1,0x3,0x0,0x1 );
  
    eResult = DalTlmm_ConfigGpio(hTlmmDriver, cfg_111, DAL_TLMM_GPIO_ENABLE);
    
    if(DAL_SUCCESS == eResult)
    {
      /*Set TLMM_SPARE register*/
      nVal = HWIO_IN(TLMM_SPARE);
      nVal |= 0x1;
      HWIO_OUT(TLMM_SPARE, nVal);
      
      /* Making sure of register write go through */
      dummy_read = HWIO_IN(TLMM_SPARE);

      /*
       * Changin the pad pull to NO PULL to avoid leakage during sleep.
       * Congiuring GPIOs 111 to OUTPUT, NO PULL
       */
      cfg_111  = DAL_GPIO_CFG( 0x6F,0x0,0x1,0x0,0x0 );

      eResult = DalTlmm_ConfigGpio(hTlmmDriver, cfg_111, DAL_TLMM_GPIO_ENABLE);
      if(DAL_SUCCESS == eResult)
      {
      /* 
       * Only need to initialize TLMM internal structures.  No need to
       * maintain a handle.
       */
        eResult = DAL_DeviceDetach(hTlmmDriver);
      }
    }

    if(DAL_SUCCESS == eResult)
    {
      return(TRUE);
    }
    return(FALSE);
#endif
  }
  return FALSE;
  
}
