/*==============================================================================

FILE:      vmpm.c

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2010-2011 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/src/vmpm.c#1 $
$Date: 2015/02/25 $

==============================================================================*/

#include <stdbool.h>
#include <DALSys.h>
#include <DALDeviceId.h>
#include <DDIInterruptController.h>
#include <err.h>

#include "vmpm.h"
#include "vmpmi.h"
#include <HALmpmint.h>
#include "HALhwio.h"
#include "tlmm_hwio.h"
#include "cortex-m3.h"
#include "../hal/source/common/HALmpmintInternal.h"
#include "HALmpm.h"

static struct vmpm_masters_cache_s
{
  //0 = apps, 1 = modem, 2 = qdsp, 3 = riva, 4 = qdsp
  vmpm_master_cache_type master[VMPM_MAX_MASTERS];
} vmpm_masters_cache;

bool lookupDirConn(uint32 master, uint32 gpio)
{
  int i = 0;
  bool gpio_found = FALSE;
  uint32 gpio_num = 0;
  switch (master)
  {
    case (0):
      for (i = 0; i<2; i++)
      {
        gpio_num = HWIO_INFI(TLMM_DIR_CONN_INTRn_CFG_WCSS, i, GPIO_SEL);
        if (gpio_num == gpio)
        {
          gpio_found = TRUE;
          break;
        }
      }
      break;
    case (1):
      for (i = 0; i<10; i++)
      {
        gpio_num = HWIO_INFI(TLMM_DIR_CONN_INTRn_CFG_SENSORS, i, GPIO_SEL);
        if (gpio_num == gpio)
        {
          gpio_found = TRUE;
          break;
        }
      }
      break;
    case (2):
      for (i = 0; i<6; i++)
      {
        gpio_num = HWIO_INFI(TLMM_DIR_CONN_INTRn_CFG_LPA_DSP, i, GPIO_SEL);
        if (gpio_num == gpio)
        {
          gpio_found = TRUE;
          break;
        }
      }
      break;
    case (3):
      for (i = 0; i<1; i++)
      {
        gpio_num = HWIO_INFI(TLMM_DIR_CONN_INTRn_CFG_RPM, i, GPIO_SEL);
        if (gpio_num == gpio)
        {
          gpio_found = TRUE;
          break;
        }
      }
      break;
    case (4):
      for (i = 0; i<8; i++)
      {
        gpio_num = HWIO_INFI(TLMM_DIR_CONN_INTRn_CFG_APSS, i, GPIO_SEL);
        if (gpio_num == gpio)
        {
          gpio_found = TRUE;
          break;
        }
      }
      break;
    case (5):
      for (i = 0; i<2; i++)
      {
        gpio_num = HWIO_INFI(TLMM_DIR_CONN_INTRn_CFG_MSS, i, GPIO_SEL);
        if (gpio_num == gpio)
        {
          gpio_found = TRUE;
          break;
        }
      }
      break;
  }
  return gpio_found;
}

/* the purpose of this function is to make sure that a master's requested
 * interrupt configuration is in place at all times.
 *
 * if the interrupt corresponds to a GPIO, then we should only reconfigure it
 * if the requesting processor is the target of that GPIO.
 *
 * if it isn't a GPIO, we have no way to identify the 'owning' master, so we
 * only use the data in the BSP table and never reconfigure */
void vmpm_reconfigure(uint32 phys_int_num, uint32 master_isr, HAL_mpmint_TriggerType trigger)
{
  uint32                 mpm_int_id, gpio_num;
  bool                   direct_connect;

  /* check to see if this physical interrupt number is supported */
  if(!HAL_mpmint_GetEnumNumber(phys_int_num, &mpm_int_id))
    return;
  
  /* before reconfiguring, check to see if this is a GPIO--if not we leave the
   * interrupt configured to whatever is in the BSP table (as the interrupt is
   * a fixed-function interrupt that should never change). */
  if(!HAL_mpmint_IsGpio(mpm_int_id, &gpio_num))
    return;

  /* in order to check the permissions of this master to change this interrupt,
   * we first need to see if this gpio is routed to a summary interrupt or a
   * direct connect interrupt */

  direct_connect = HWIO_INFI(TLMM_GPIO_INTR_CFGn, gpio_num, DIR_CONN_EN);

  /* now go look at the appropriate configuration source to figure out which
   * trigger we should be using */

  if(!direct_connect)
  {
    /* this is a summary interrupt, which means we can look up its destination
     * mapping quite easily */
    uint32 target = HWIO_INFI(TLMM_GPIO_INTR_CFGn, gpio_num, TARGET_PROC);
    if(vmpm_masters_info[master_isr].target_id == target)
    {
      /* configure MPM with the correct information */
      HAL_mpmint_SetTrigger(mpm_int_id, trigger);
    }
  }
  else
  {
    bool gpio_found = lookupDirConn(vmpm_masters_info[master_isr].target_id, gpio_num); 
    /* for direct connect, we have to loop through the direct connect registers
     * and find the one that is connecting this GPIO */
    if((gpio_found))
    {
      /* configure MPM with the correct information */
      HAL_mpmint_SetTrigger(mpm_int_id, trigger);
    }
  }
}

void vmpm_enable_interrupt(uint32 phys_int_num)
{
  uint32 mpm_int_id;

  /* check to see if this physical interrupt number is supported */
  if(!HAL_mpmint_GetEnumNumber(phys_int_num, &mpm_int_id))
    return;

  /* enable the interrupt */
  HAL_mpmint_Enable(mpm_int_id);
}

void vmpm_disable_interrupt(uint32 phys_int_num)
{
  uint32 i, num_masks, mpm_int_id;
  bool   in_use = false;

  /* check to see if this physical interrupt number is supported */
  if(!HAL_mpmint_GetEnumNumber(phys_int_num, &mpm_int_id))
    return;

  /* before we can disable an interrupt, we need to loop through all the
   * masters and make sure nobody is requesting that interrupt be enabled */
  HAL_mpmint_GetNumberMasks(&num_masks);
  for(i = 0; i < VMPM_MAX_MASTERS; i++)
  {
    uint32 masterEnable = vmpm_masters_info[i].enableRegAddr[phys_int_num / 32];
    if((1 << (phys_int_num % 32)) & masterEnable)
    {
      in_use = true;
      break;
    }
  }

  if(!in_use)
    HAL_mpmint_Disable(mpm_int_id);
}

/** Interrupt service routine for an incoming master interrupt.
 *
 * Masters send interrupts after they have written their 'clear' register, so
 * that we can be notified of the write and process it accordingly.  So--check
 * their clear register and do the operation now.
 */
void vmpm_isr(void) __irq
{
  //vmpm_master_info_type *master = (vmpm_master_info_type *)param;
  DALBOOL still_pending = FALSE;
  uint32 i, num_masks, master_isr;
  unsigned curr_isr = interrupt_current_isr();

  switch(curr_isr)
  {
    case APPS_IRQ:
      master_isr = 0;
      break;

    case MPSS_IRQ:
      master_isr = 1;
      break;

    case LPASS_IRQ:
      master_isr = 4;
      break;

    case PRONTO_IRQ:
      master_isr = 2;
      break;

    default:
      // shouldn't reach here...
      abort();
  }
  
  /* the clear operation clears from status any bits written '1' to clear */
  HAL_mpmint_GetNumberMasks(&num_masks);
  for(i = 0; i < num_masks; i++)
  {
    uint32 masterEnable         = vmpm_masters_info[master_isr].enableRegAddr[i];
    uint32 masterEnableCache    = vmpm_masters_cache.master[master_isr].vmpm_enable[i];
    uint32 masterDetectCtl      = vmpm_masters_info[master_isr].detectCtlRegAddr[i];
    uint32 masterDetectCtl3      = vmpm_masters_info[master_isr].detectCtl3RegAddr[i];
    uint32 masterDetectCtlCache = vmpm_masters_cache.master[master_isr].vmpm_detect_ctl[i];
    uint32 masterDetectCtl3Cache = vmpm_masters_cache.master[master_isr].vmpm_detect_ctl3[i];
    uint32 masterPolarity       = vmpm_masters_info[master_isr].polarityRegAddr[i];
    uint32 masterPolarityCache  = vmpm_masters_cache.master[master_isr].vmpm_polarity[i];
    uint32 masterStatus         = vmpm_masters_info[master_isr].statusRegAddr[i];
    uint32 temp, int_num;

    /* check for any newly reconfigured interrupts, or any newly enabled
     * interrupts, and make sure the triggers are configured correctly */
    temp = masterDetectCtl ^ masterDetectCtlCache;
    temp |= masterDetectCtl3 ^ masterDetectCtl3Cache;
    temp |= masterPolarity ^ masterPolarityCache;
    temp |= masterEnable & ~masterEnableCache;
    while(temp)
    {
      uint32                 detect, detect3, polarity;
      HAL_mpmint_TriggerType trigger;

      int_num = (31 - __clz(temp)) + 32 * i;

      detect = masterDetectCtl & (1 << (int_num % 32));
      detect3 = masterDetectCtl3 & (1 << (int_num % 32));
      polarity = masterPolarity & (1 << (int_num % 32));

      HAL_mpmint_GetIndex(detect, detect3, polarity, (1 << (int_num % 32)), &trigger);

      vmpm_reconfigure(int_num, master_isr, trigger);

      temp &= ~(1 << (int_num % 32));
    }
    vmpm_masters_cache.master[master_isr].vmpm_detect_ctl[i] = masterDetectCtl;
    vmpm_masters_cache.master[master_isr].vmpm_detect_ctl3[i] = masterDetectCtl3;
    vmpm_masters_cache.master[master_isr].vmpm_polarity[i] = masterPolarity;

    /* check for any newly enabled interrupts */
    temp = masterEnable & ~masterEnableCache;
    while(temp)
    {
      int_num = (31 - __clz(temp)) + 32 * i;
      vmpm_enable_interrupt(int_num);
      temp &= ~(1 << (int_num % 32));
    }

    /* check for any newly disabled interrupts */
    temp = masterEnableCache & ~masterEnable;
    while(temp)
    {
      int_num = (31 - __clz(temp)) + 32 * i;
      vmpm_disable_interrupt(int_num);
      temp &= ~(1 << (int_num % 32));
    }
    vmpm_masters_cache.master[master_isr].vmpm_enable[i] = masterEnable;

    /* if any 'clear' bits are set, remove them from status */
    vmpm_masters_info[master_isr].statusRegAddr[i] = masterStatus;

    /* check if there's still an int pending */
    if(masterStatus & masterEnable)
      still_pending = TRUE;
  }

  /* if there was still an interrupt pending, re-send the virtual mpm int */
  if(still_pending)
      vmpm_masters_info[master_isr].pfnSendInterrupt();
}

void vmpm_init(void)
{
  uint32 i, j, num_masks;
  char **ppszVersionInt = NULL;
  volatile unsigned isrNum = interrupt_current_isr();

  HAL_mpmint_Init( ppszVersionInt );

  /* walk the master list and initialize for each */
  HAL_mpmint_GetNumberMasks(&num_masks);
  for(i = 0; i < VMPM_MAX_MASTERS; i++)
  {
    for(j = 0; j < num_masks; j++)
    {
      vmpm_masters_info[i].enableRegAddr[j]      = 0;
      vmpm_masters_info[i].detectCtlRegAddr[j]   = 0;
      vmpm_masters_info[i].polarityRegAddr[j]    = 0;
      vmpm_masters_info[i].statusRegAddr[j]      = 0;

      vmpm_masters_cache.master[i].vmpm_enable[j]        = 0;
      vmpm_masters_cache.master[i].vmpm_detect_ctl[j]    = 0;
      vmpm_masters_cache.master[i].vmpm_polarity[j]      = 0;
    }
    interrupt_set_isr(vmpm_masters_info[i].vmpm_interrupt, vmpm_isr);
  }
}

void vmpm_prepare_for_sleep(void)
{
  /* first clear any stale interrupts (from previous sleep / active period) */
  HAL_mpmint_All_Clear();

  /* no matter what interrupts are enabled by masters, we always need to
   * enable the timetick interrupt (for wakeup-on-timer) and the processor
   * wakeup interrupts (to allow actual wakeups) */
  HAL_mpmint_Enable(HAL_MPMINT_QTIMER_ISR);
}

void vmpm_propogate_interrupts(void)
{
  uint32 i, num_masks, master;

  /* disable the MPM interrupt in the RPM QGIC because this basically services
   * the interrupt before interrupts are ever unlocked */
  interrupt_disable(MPM_IRQ);

  /* iterate over the hardware's masks, collecting pending ints */
  HAL_mpmint_GetNumberMasks(&num_masks);
  for(i = 0; i < num_masks; i++)
  {
    uint32 newStatus;
    HAL_mpm_GetWakeupInts(i, &newStatus);

    /* install the new pending interrupts in each master */
    for(master = 0; master < VMPM_MAX_MASTERS; master++)
      vmpm_masters_info[master].statusRegAddr[i] |= newStatus;
  }

  /* now send interrupts to masters if they have pending interrupts */
  for(master = 0; master < VMPM_MAX_MASTERS; master++)
  {
    for(i = 0; i < num_masks; i++)
    {
      uint32 enabled = vmpm_masters_info[master].enableRegAddr[i];
      uint32 pending = vmpm_masters_info[master].statusRegAddr[i];

      if(enabled & pending)
      {
        /* this master does need an interrupt--send it and move on */
       /* Send both vmpm and the wakeup_ind, per agreement with the sleep team */
        vmpm_masters_info[master].pfnSendWakeup();
        vmpm_masters_info[master].pfnSendInterrupt ();
        break;
      }
    }
  }
}

