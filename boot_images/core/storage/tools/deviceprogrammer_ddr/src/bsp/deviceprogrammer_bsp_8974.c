/*===========================================================================

 DEVICEPROGRAMMER_BSP_8974.c

GENERAL DESCRIPTION
 This file contain the platform dependent code that is SCorpion processor
 specific

  
 Copyright (c) 2009 - 2013 Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/deviceprogrammer_ddr/src/bsp/deviceprogrammer_bsp_8974.c#1 $ 
  $DateTime: 2015/09/01 00:30:35 $ 
  $Author: pwbldsvc $

YYYY-MM-DD   who     what, where, why
----------   ---     ----------------------------------------------------------
2012-12-12   sy      Update Pmic header file
2011-06-15   ah      8960 bringup

===========================================================================*/


#include "msm.h"
#include "deviceprogrammer_bsp.h"
#include "SpmiBus.h"
#include "pm_pon.h"


extern void qhsusb_dci_write_scratch(uint32 core_id,uint8 offset,uint32 data);

/******************************************************************************
*
* Description:
*    This function Resets the target
*
******************************************************************************/
void bsp_target_reset(void)
{
//  HWIO_OUT(SCSS_WDT0_RST, HWIO_FMSK(SCSS_WDT0_RST, STB));
//  HWIO_OUT(TCSR_SMPSS_WDOG_CFG, 
//  		HWIO_FMSK(TCSR_SMPSS_WDOG_CFG, SCSS_WDT0CPU0BITEEXPIRED));
//  HWIO_OUT(SCSS_WDT0_EN, HWIO_FMSK(SCSS_WDT0_EN, EN));

   //qhsusb_dci_write_scratch(0, 0, 0x0);

   pm_pon_ps_hold_cfg(0, PM_PON_RESET_CFG_WARM_RESET);

   // pull down pmic PS_HOLD to force a reset
   HWIO_OUTF( MPM2_MPM_PS_HOLD, PSHOLD, 0x0 );

   for (;;)
   {
     /* let the watch dog timer expire here... */
   }
}

void bsp_target_poweroff(void) {
    pm_pon_ps_hold_cfg(0, PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN);
    HWIO_OUTF( MPM2_MPM_PS_HOLD, PSHOLD, 0x0 );    // pull down pmic PS_HOLD

    for (;;)
    {

    }
}

/******************************************************************************
*
* Description:
*    This function performs the HW Init for eHOSTDL
*    The HW init includes enabling SDRAM, Clocks etc...
*
******************************************************************************/
void bsp_hw_init(void)
{
}

/******************************************************************************
*
* Description:
*    Kick the watchdog timer
*      
******************************************************************************/
void bsp_kick_wdog(void)
{ 
   //HWIO_OUT(APCS_WDOG_CTL, 1);
   //HWIO_OUT(APCS_WDOG_BARK_TIME, 0xFFFFF);
   //HWIO_OUT(APCS_WDOG_RESET, 1);
   
}

/******************************************************************************
*
* Description:
*    Disable the watchdog timer
*      
******************************************************************************/
void bsp_freeze_wdog(void)
{
   //HWIO_OUT(APCS_WDOG_CTL, 0);
}

/******************************************************************************
*
* Description:
*    This function provides a busy wait for the caller.
*
* Arguments:
*    us              [IN] : time in micro seconds
*      
******************************************************************************/
/*
void clk_busy_wait(uint32 us)
{
   uint32 count;
   HWIO_OUT(APCS_GPT0_CNT, 0);
   HWIO_OUT(APCS_GPT0_EN, 1);
   
   count = us * 0.032768;
   while (HWIO_IN(APCS_GPT0_CNT) < count);
   HWIO_OUT(APCS_GPT0_EN, 0);
}
*/

/*
#define HWIO_OUT(hwiosym, val)                                   __msmhwio_out(hwiosym, val)
#define __msmhwio_out(hwiosym, val)                             HWIO_##hwiosym##_OUT(val)

#define HWIO_FMSK(hwio_regsym, hwio_fldsym)              __msmhwio_fmsk(hwio_regsym, hwio_fldsym)
#define __msmhwio_fmsk(hwiosym, hwiofldsym)                     HWIO_##hwiosym##_##hwiofldsym##_BMSK

//////HWIO_OUT(APCS_WDT0_RST, HWIO_FMSK(APCS_WDT0_RST, STB));
////HWIO_APCS_WDT0_RST_OUT(HWIO_APCS_WDT0_RST_STB_BMSK)
//HWIO_APCS_WDOG_RESET_OUT(HWIO_APCS_WDOG_RESET_RESET_BMSK)
HWIO_OUT(APCS_WDOG_RESET, HWIO_FMSK(APCS_WDOG_RESET, RESET))
---
//////HWIO_OUT(TCSR_SMPSS_WDOG_CFG, HWIO_FMSK(TCSR_SMPSS_WDOG_CFG, SCSS_WDT0CPU0BITEEXPIRED));
//HWIO_TCSR_SMPSS_WDOG_CFG_OUT(HWIO_TCSR_SMPSS_WDOG_CFG_SCSS_WDT0CPU0BITEEXPIRED_BMSK)

---
//////HWIO_OUT(APCS_WDT0_EN, HWIO_FMSK(APCS_WDT0_EN, EN));
////HWIO_APCS_WDT0_EN_OUT(HWIO_APCS_WDT0_EN_EN_BMSK)
//HWIO_APCS_WDOG_CTL_OUT(HWIO_APCS_WDOG_CTL_ENABLE_BMSK)
HWIO_OUT(APCS_WDOG_CTL, HWIO_FMSK(APCS_WDOG_CTL, ENABLE))
*/
