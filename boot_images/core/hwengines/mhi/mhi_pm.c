/**
  @file mhi_pm.c
  @brief
  This file contains the MHI power management implementation.
    

*/
/*
===============================================================================
   
                             Edit History
 
 $Header: 
 
when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/15/13   am      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi_pm.h"
#include "mhi_core.h"
#include "mhi_hwacc.h"


/* GPIO pin# for device wake in the TLMM */
#define MHI_GPIO_DEVICE_WAKE_PINOUT  60

#ifndef MHI_BOOT
/* GPIO handle */
mhi_gpio_handle_t mhi_gpio_device_wake = {NULL};                                           
#endif

#ifndef MHI_BOOT

/** 
 * @brief      mhi_pm_device_wake_isr
 *
 *             Device Wake ISR
 *              
 * 
 * @param[in]  data - user data
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_pm_device_wake_isr(uint32 data)
{
#ifndef MHI_PM_STATE_TEST 
   DALResult dal_result;
#endif

#ifdef VIRTIO  
   GPIOIntTriggerType trigger;
#endif

   /* pm test loop directly manipulates mhiCtx.mhi_pm_info.device_wake_gpio_state for now */ 
#ifndef MHI_PM_STATE_TEST 
   dal_result = DalTlmm_GpioIn(mhi_gpio_device_wake.h_mhi_tlmm,
         mhi_gpio_device_wake.mhi_gpio_pin,(DALGpioValueType *)&mhiCtx.mhi_pm_info.device_wake_gpio_state);

   MHI_ASSERT(DAL_SUCCESS == dal_result && mhiCtx.mhi_pm_info.device_wake_gpio_state <= DAL_GPIO_HIGH_VALUE);
#endif
   mhi_osal_debug(MSG_HIGH,"mhi_pm_device_wake_isr: %d",mhiCtx.mhi_pm_info.device_wake_gpio_state);

#ifdef VIRTIO 
   if (LOW == mhiCtx.mhi_pm_info.device_wake_gpio_state)
   {
      trigger = GPIOINT_TRIGGER_RISING;

      /* Arm the link inactivity timer interrupt */
      mhi_link_enable_inactivity_intr();
   }
   else 
   {
      trigger = GPIOINT_TRIGGER_FALLING;

      /* Disarm the link inactivity timer interrupt */
      mhi_link_disable_inactivity_intr();
   }

   dal_result = GPIOInt_SetTrigger(
         mhi_gpio_device_wake.h_mhi_gpio_int,
         MHI_GPIO_DEVICE_WAKE_PINOUT, 
         trigger); 

   MHI_ASSERT(DAL_SUCCESS == dal_result);
#endif  

   mhi_sm_set_trig_pending(MHI_SM_TRIG_DEVICE_WAKE_SHIFT,TRUE);
}
#endif /* #ifndef MHI_BOOT */


/**
 * @brief       mhi_pm_init
 *
 *              Initialize the MHI power management software module.
 *
 * @param[in]   *pm_info - pointer to the MHI power module context.
 *
 * @dependencies
 *              Device must be in D0.
 * 
 * @return       
 *              SUCCESS\ERROR  
 * 
 * @sideeffect  None 
 */
uint32 mhi_pm_init(mhi_pm_info_t *pm_info)
{
#ifndef MHI_BOOT
   DALResult dal_result;
   GPIOIntTriggerType trigger = GPIOINT_TRIGGER_DUAL_EDGE;
#endif

   mhi_osal_debug(MSG_HIGH,"mhi_pm_init");

   /* Device must be in D0 during init */
   mhiCtx.mhi_pm_info.current_d_state = MHI_LINK_D0;

   /* Sleep vote should not have been cast until init is complete */
   mhiCtx.mhi_pm_info.mhi_sleep_vote_cast = FALSE;

#ifndef MHI_BOOT
   dal_result = DAL_DeviceAttach(DALDEVICEID_TLMM,&mhi_gpio_device_wake.h_mhi_tlmm);

   if(DAL_SUCCESS != dal_result || !mhi_gpio_device_wake.h_mhi_tlmm)
   {
      return ERROR;
   }

   dal_result = DAL_DeviceAttach(DALDEVICEID_GPIOINT,&mhi_gpio_device_wake.h_mhi_gpio_int) ;

   if(DAL_SUCCESS != dal_result || !mhi_gpio_device_wake.h_mhi_gpio_int)
   {
      return ERROR;
   }    

   mhi_gpio_device_wake.mhi_gpio_pin = DAL_GPIO_CFG(
         MHI_GPIO_DEVICE_WAKE_PINOUT,
         0,
         DAL_GPIO_INPUT,
         DAL_GPIO_PULL_DOWN,
         DAL_GPIO_2MA); 

   dal_result = DalTlmm_ConfigGpio(
         mhi_gpio_device_wake.h_mhi_tlmm, 
         mhi_gpio_device_wake.mhi_gpio_pin,
         DAL_TLMM_GPIO_ENABLE);

   if(DAL_SUCCESS != dal_result)
   {
      return ERROR;
   }

#ifndef MHI_PM_STATE_TEST
   dal_result = DalTlmm_GpioIn(mhi_gpio_device_wake.h_mhi_tlmm,
         mhi_gpio_device_wake.mhi_gpio_pin,(DALGpioValueType *)&mhiCtx.mhi_pm_info.device_wake_gpio_state);
#else
   mhiCtx.mhi_pm_info.device_wake_gpio_state = TRUE;
#endif

   if (LOW == mhiCtx.mhi_pm_info.device_wake_gpio_state)
   {
#ifdef VIRTIO 
      trigger = GPIOINT_TRIGGER_RISING;
#endif
      mhi_link_enable_inactivity_intr();
   }
   else 
   {
#ifdef VIRTIO 
      trigger = GPIOINT_TRIGGER_FALLING;
#endif
      mhi_link_disable_inactivity_intr();
   }    

   /* Register ISR for device_wake signal */
   dal_result = GPIOInt_RegisterIsr(
         mhi_gpio_device_wake.h_mhi_gpio_int,
         MHI_GPIO_DEVICE_WAKE_PINOUT,
         trigger,
         (GPIOINTISR)mhi_pm_device_wake_isr,
         NULL) ;
   if(DAL_SUCCESS != dal_result)
   {
      return ERROR;
   }

   /* Trigger whenever the state of device_wake gpio changes */
   dal_result = GPIOInt_SetTrigger(
         mhi_gpio_device_wake.h_mhi_gpio_int,
         MHI_GPIO_DEVICE_WAKE_PINOUT, 
         trigger); 
   if(DAL_SUCCESS != dal_result)
   {
      return ERROR;
   }
#else /* #ifndef MHI_BOOT */
   mhiCtx.mhi_pm_info.device_wake_gpio_state = TRUE; /* default init value */
#endif /* #ifndef MHI_BOOT */

   return SUCCESS;

}

/**
 * @brief       mhi_pm_reset
 *
 *              Reset the MHI power manaagment software module.
 *
 * @dependencies
 *              Device must be in D0. 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */  
void mhi_pm_reset(void)
{
   /* Device must be in D0 when MHI is reset  */
   mhiCtx.mhi_pm_info.current_d_state = MHI_LINK_D0;

  /* Sleep vote should be revoked in reset */
  if (mhiCtx.mhi_pm_info.mhi_sleep_vote_cast)
  {
    mhi_pm_vote_against_sleep();
  }
}

/**
 * @brief       mhi_pm_core_wakeup
 *
 *              API called by MHI core to wake up MHI from low power state.
 *
 * @dependencies
 *              Can be called only after mhi_init 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_pm_core_wakeup(void)
{
  mhi_osal_debug(MSG_HIGH,"mhi_pm_core_wakeup");
  mhi_sm_set_trig_pending(MHI_SM_TRIG_CORE_WAKEUP_SHIFT,FALSE);
}  

/**
 * @brief       mhi_pm_hw_acc_wakeup
 *
 *              API called by the hardware accelerator to wake up from
 *              low power state.
 *              
 * @dependencies
 *              Can be called only after mhi_init  
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_pm_hw_acc_wakeup(void)
{
   mhi_osal_debug(MSG_HIGH,"mhi_pm_hw_acc_wakeup");
   mhi_sm_set_trig_pending(MHI_SM_TRIG_HW_ACC_WAKEUP_SHIFT,TRUE);
}

/**
 * @brief       mhi_pm_process_d_state_change
 *
 *              Processes a change of state device power (D states).
 *              
 * @dependencies
 *              Cached D state in mhi_pm.c must be up to date. 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_pm_process_d_state_change(mhi_link_d_state_t d_state)
{
   switch (d_state)
   {
      case MHI_LINK_D0:
         /* nothing to do */
         mhi_osal_debug(MSG_HIGH,"mhi_pm_process_d_state_change: Change to MHI_LINK_D0 complete");
         break;
      case MHI_LINK_D1:
         /* No trigger defined at this time */
         /* nothing to do */
         mhi_osal_debug(MSG_HIGH,"mhi_pm_process_d_state_change: Change to MHI_LINK_D1 complete");
         break;
      case MHI_LINK_D2:
         /* No trigger defined at this time */
         /* nothing to do */
         mhi_osal_debug(MSG_HIGH,"mhi_pm_process_d_state_change: Change to MHI_LINK_D2 complete");
         break;
      case MHI_LINK_D3hot:
         mhi_osal_debug(MSG_HIGH,"mhi_pm_process_d_state_change: Change to MHI_LINK_D3hot complete");
         /* Check if wake up is pending */
         if (TRUE == mhiCtx.mhi_sm_info.wakeup_pending)
         {
            mhi_pm_wakeup_host();
         }
         break;
      case MHI_LINK_D3cold:
         /* If wake up is pending, don't vote for sleep. Note that if host ignored
          * the wake up in D3hot and entered D3cold anyway, device will attempt to
          * wake host up again in D3cold */
         if (TRUE == mhiCtx.mhi_sm_info.wakeup_pending)
         {
            mhi_pm_wakeup_host();
            /* Wake up procedure completed, clear wake up pending flag */
            mhiCtx.mhi_sm_info.wakeup_pending = FALSE;
            return;
         }
         /* Synchronously cause hw accelerator to trigger its own power management */
         mhi_hw_acc_enable_pm();
         mhi_pm_vote_for_sleep();
         mhi_osal_debug(MSG_HIGH,"mhi_pm_process_d_state_change: Change to MHI_LINK_D3cold complete");
         break;
      default:
         mhi_osal_debug(MSG_FATAL,"mhi_pm_process_d_state_change: Invalid D state received");
   }
}

/**
 * @brief       mhi_pm_wakeup_host
 *
 *              Calls the link power management API to send a wake up to the 
 *              host. The wake up may involve an interrupt or toggling a GPIO.
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_pm_wakeup_host(void)
{
   /* If device is in low power state, issue wake up */
   if (MHI_LINK_D3cold == mhiCtx.mhi_pm_info.current_d_state)
   {
      /* Issue a link wakeup request */  
      mhi_link_wakeup_req();
      mhi_osal_debug(MSG_HIGH,"mhi_pm_wakeup_host: Wake up issued in D3cold");
   }
   else
   {
      mhi_osal_debug(MSG_HIGH,"mhi_pm_wakeup_host: Wake up request ignored. D state = %d",mhiCtx.mhi_pm_info.current_d_state);
   }
}

/**
 * @brief       mhi_pm_wakeup_device_req_fromHost
 *
 *              Received a request from the host to make the device exit low
 *              power mode.
 * 
 * @dependencies
 *              Should be called only after mhi_pm_init. 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_pm_wakeup_device_req_fromHost(void)
{
  /* Vote against sleep/HW ACC power management only if it has been voted for */
  if (mhi_pm_is_sleep_voted())
  {
    /* According to the current state machine design, there is no circumstance 
     * where MHI votes for sleep, but does not vote for HW ACC power management */
    mhi_pm_vote_against_sleep();

    /* Request IPA resources */
    mhi_hw_acc_disable_pm();
  }

  /* Re-establish the link with the host */
  mhi_osal_debug(MSG_HIGH,"mhi_pm_wakeup_device_req_fromHost: Enabling link hardware");
  
  mhi_link_enable();
}

/**
 * @brief       mhi_pm_vote_for_sleep
 *
 *              Vote for sleep (XO shutdown). Explicit voting by MHI for the XO 
 *              is not required. The vote for XO is dependent on drivers that 
 *              directly mainpulate resources sourced from the XO. This function 
 *              is retained for MHI spec compliance and future use, if MHI requires
 *              it's own XO vote.
 *              
 * @dependencies
 *              mhi_pm_init must be called. Caller should make sure that this is
 *              protected by the mhiSync mutex (i.e. mutex from the mhi context).
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_pm_vote_for_sleep(void)
{
   if (!mhiCtx.mhi_pm_info.mhi_sleep_vote_cast)
   {
      /* Vote for XOShutdown */
      mhiCtx.mhi_pm_info.mhi_sleep_vote_cast=TRUE;
      mhi_osal_debug(MSG_HIGH, "mhi_pm_vote_for_sleep");
   }
}

/**
 * @brief       mhi_pm_vote_against_sleep
 *
 *              Vote against sleep (XO shutdown). Explicit voting by MHI for the XO 
 *              is not required. The vote for XO is dependent on drivers that 
 *              directly mainpulate resources sourced from the XO. This function 
 *              is retained for MHI spec compliance and future use, if MHI requires
 *              it's own XO vote.
 *
 * @dependencies
 *              mhi_pm_init must be called. Caller should make sure that this is
 *              protected by the mhiSync mutex (i.e. mutex from the mhi context). 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_pm_vote_against_sleep(void)
{
   if (mhiCtx.mhi_pm_info.mhi_sleep_vote_cast)
   {
      /* Vote against xo shutdown */
      mhiCtx.mhi_pm_info.mhi_sleep_vote_cast=FALSE;
      mhi_osal_debug(MSG_HIGH, "mhi_pm_vote_against_sleep");
   }
}

/**
 * @brief       mhi_pm_set_d_state
 *
 *              Set the current D state in the mhi_pm context.
 * 
 * @dependencies
 *              Should be called only after mhi_pm_init. 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_pm_set_d_state(mhi_link_d_state_t d_state)
{
   mhiCtx.mhi_pm_info.current_d_state = d_state;
   mhi_osal_debug(MSG_HIGH,"mhi_pm_set_d_state: %d",d_state);
}

/**
 * @brief       mhi_pm_get_d_state
 *
 *              Get the current D state according to mhi_pm
 * 
 * @dependencies
 *              Should be called only after mhi_pm_init. 
 * 
 * @return       
 *              Current D state as cached by mhi_pm.  
 * 
 * @sideeffect  None 
 */
mhi_link_d_state_t mhi_pm_get_d_state(void)
{
   mhi_osal_debug(MSG_HIGH,"mhi_pm_get_d_state: %d",mhiCtx.mhi_pm_info.current_d_state);
   return mhiCtx.mhi_pm_info.current_d_state;
}

/**
 * @brief       mhi_pm_get_device_wake_gpio_state
 *
 *              Get the current state of the device_wake gpio.
 * 
 * @dependencies
 *              Should be called only after mhi_pm_init. 
 * 
 * @return       
 *              HIGH\LOW depending on ASSERTION\DEASSERTION of device_wake.  
 * 
 * @sideeffect  None 
 */
boolean mhi_pm_get_device_wake_gpio_state(void)
{
   mhi_osal_debug(MSG_HIGH,"mhi_pm_get_device_wake_gpio_state: %d",mhiCtx.mhi_pm_info.device_wake_gpio_state);
   return mhiCtx.mhi_pm_info.device_wake_gpio_state;
}

/**
 * @brief       mhi_pm_is_sleep_voted
 *
 *              Return whether the sleep vote has been cast yet.
 * 
 * @dependencies
 *              Should be called only after mhi_pm_init. 
 * 
 * @return       
 *              TRUE\FALSE depending on whether the vote for sleep has been cast.  
 * 
 * @sideeffect  None 
 */
boolean mhi_pm_is_sleep_voted(void)
{
   return mhiCtx.mhi_pm_info.mhi_sleep_vote_cast;
}

#ifndef MHI_BOOT

/** 
 * @brief      mhi_trigger_device_wake_intr
 *
 *             Artificially trigger device wake for testing
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_trigger_device_wake_intr(void)
{
   DALResult           dal_result;

   dal_result = GPIOInt_TriggerInterrupt(mhi_gpio_device_wake.h_mhi_gpio_int,MHI_GPIO_DEVICE_WAKE_PINOUT);
   MHI_ASSERT(DAL_SUCCESS == dal_result);
}
#endif /* #ifndef MHI_BOOT */

















