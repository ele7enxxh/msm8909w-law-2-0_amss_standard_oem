/**
 * Copyright (c) 2011 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/ddf/src/sns_ddf_signal.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2015-04-10 AH   Enable GPIO read code
  2015-04-08 hw   Uinsg uGPIOInt_RegisterInterrupt for registering sensor GPIO interrupt
  2015-02-02 jms  Removing TLMM since it's not available for SLPI on 8996
  2014-12-10 MW   Incude sns_ddf_signal_priv.h.
  2014-08-27 pn   Restored overriden changes; removed unused declarations.
  2014-08-15 vy   Moved register and deregister apis to bigImage
  2014-08-08 pn   Configures GPIO to NO_PULL
  2014-07-29 vy   Moved sns_ddf_signal_irq_enabled to uImage
  2014-01-27 ps   Added sns_ddf_signal_gpio_read
  2013-12-17 ps   Branched out from 8974 directory
  2013-11-26 pn   Prevents dispatching signals with same timestamp
  2013-10-31 hw   add QDSS SW events
  2013-08-19 pn   Clears signal bit when deregistering signal
  2013-07-12 lka  Added compile-time configurations
  2013-07-01 hw   Add latency measurement support for DRI mode
  2013-06-12 pn   Added debug logging to catch first time interrupt is triggered
  2013-06-11 ae   Added QDSP SIM playback support
  2013-04-06 ag   Remove APDS interrupt workaround
  2013-01-18 ag   
             pn   APDS workaround
  2012-11-07 ag   Removed debug printf
  2012-10-25 vh   Eliminated compiler warnings
  2012-10-19 ag   Port code to work on ADSP
  2012-09-13 vh   Eliminated compiler warnings
  2012-04-13 br   Fixed racing condition between ISR and sns_ddf_signal_dispatch()
  2012-03-05 br   change the type fo gpio_num from 32 bits to 16 bits
  2012-02-03 sd   moved DRI gpio control to DD
  2011-09-01 sd   Changed MD int detect GPIO bin to 67 to use INT2
==============================================================================*/
#include "sns_ddf_signal.h"
#include "sns_ddf_signal_priv.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_smgr_sensor_config.h"

#if !defined(SNS_PCSIM)
extern sns_ddf_sig_tb_s sns_ddf_sig_tb;
#endif

/*===========================================================================

  FUNCTION:   sns_ddf_signal_gpio_config

===========================================================================*/
/*!
  @brief Configuring GPIO pin

  @detail

  @param gpio_num gpio number
  @return
     gpio configuration
 */
/*=========================================================================*/
static uint32_t sns_ddf_signal_gpio_config (uint32_t  gpio_num)
{
#if !defined(SNS_PCSIM)

    /* who should provide the parameters 0, DAL_GPIO_INPUT, DAL_GPIO_PULL_UP, DAL_GPIO_2MA ? DD or DDF?*/
  return DAL_GPIO_CFG(gpio_num, 0, DAL_GPIO_INPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA);
#else
	return 0;
#endif
}

/*===========================================================================

  FUNCTION:   sns_ddf_signal_gpio_read

===========================================================================*/
/*!
  @brief  Read GPIO pin
  @detail

  @param[i] gpio number
  @param[0] gpio status
  @return
   DAL_SUCCESS -- GPIO was successfully configured as requested.\n
   DAL_ERROR -- Either an invalid GPIO number in the configuration or ownership
   cannot be obtained from a secure root.
 */
/*=========================================================================*/
sns_ddf_status_e sns_ddf_signal_gpio_read(uint16_t gpio_num, uint32_t *gpio_status)
{
#if !defined(SNS_PCSIM)
		uint32_t gpio_config;
		DALGpioValueType gpio_val;
	
		DALSYS_SyncEnter(sns_ddf_sig_tb.sync_handle);
	
		gpio_config = sns_ddf_signal_gpio_config(gpio_num);
	
		if ( DAL_ERROR == DalTlmm_GpioIn(sns_ddf_sig_tb.tlmm_handle,
													gpio_config,
													&gpio_val) )
		{
		  //log error;
		  DALSYS_SyncLeave(sns_ddf_sig_tb.sync_handle);
		  return SNS_DDF_EFAIL;
		}
	
		*gpio_status = (uint32_t)gpio_val;
	
		DALSYS_SyncLeave(sns_ddf_sig_tb.sync_handle);
#endif
		return SNS_DDF_SUCCESS;
}


/**
 * Returns the index of the next free slot in the signal table, or -1 if
 * none.
 */
static int8_t sns_ddf_signal_get_free_slot (void)
{
#if !defined(SNS_PCSIM)
  int8_t i;
  for(i = 0; i < SNS_DDF_SIGNAL_MAX_CNT; i++) {
    if( !sns_ddf_sig_tb.sig_info[i].is_registered &&
        !(sns_ddf_sig_tb.pending_flags & (1 << i)) )
    {
      return i;
    }
  }
#endif
  return -1;
}


/**
 * Initializes the signal service's internal structures.
 */
static void sns_ddf_signal_init(void)
{
#if !defined(SNS_PCSIM)
  uint8_t i;
  sns_ddf_sig_tb.pending_flags = 0;
  for(i = 0; i < SNS_DDF_SIGNAL_MAX_CNT; i++)
  {
    sns_ddf_sig_tb.sig_info[i].is_registered = false;
    sns_ddf_sig_tb.sig_info[i].warning_cnt = 0;
    sns_ddf_sig_tb.sig_info[i].int_fired = false;
  }

  if(DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_TLMM, &sns_ddf_sig_tb.tlmm_handle))
  {
    SNS_ASSERT(0);
  }
  if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                                       &sns_ddf_sig_tb.sync_handle, NULL))
  {
    SNS_ASSERT(0);
  }
  if(DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_GPIOINT, &sns_ddf_sig_tb.gpio_int_handle))
  {
    SNS_ASSERT(0);
  }
#endif
}


/**
 * Returns the index of the signal informaiton registered for a particular GPIO, or -1
 * if none.
 */
static int8_t sns_ddf_signal_find_sig_index (uint16_t gpio_num)
{
#if !defined(SNS_PCSIM)
  int8_t i;
  for(i = 0; i < SNS_DDF_SIGNAL_MAX_CNT; i++) {
    if(sns_ddf_sig_tb.sig_info[i].is_registered && (sns_ddf_sig_tb.sig_info[i].gpio_num == gpio_num))
      return i;
  }
#endif
  return -1;
}


/*===========================================================================

  FUNCTION:   sns_ddf_signal_gpio_enable

===========================================================================*/
/*!
  @brief  Enable/Disable GPIO pin
  @detail

  @param[i] gpio_handle the handle
  @param[i] gpio_pin_cfg gpio config
  @param[i] enable_flag DAL_TLMM_GPIO_ENABLE or DAL_TLMM_GPIO_DISABLE
  @return
   DAL_SUCCESS -- GPIO was successfully configured as requested.\n
   DAL_ERROR -- Either an invalid GPIO number in the configuration or ownership
   cannot be obtained from a secure root.
 */
/*=========================================================================*/
#if !defined(SNS_PCSIM)
static int sns_ddf_signal_gpio_enable (DalDeviceHandle *tlmm_handle, uint32_t gpio_pin_cfg, DALGpioEnableType enable_flag)
{
  return DAL_SUCCESS;
}
#endif


/*===========================================================================

  FUNCTION:   sns_ddf_signal_register

===========================================================================*/
/*!
  @brief This function registers interrupt 

  @detail 

  @param[i]  none
  @return   none
 */
/*=========================================================================*/
sns_ddf_status_e sns_ddf_signal_register (
    uint32_t                      gpio_num,
    sns_ddf_handle_t              dd_handle,
    sns_ddf_driver_if_s*          dd_interface,
    sns_ddf_signal_irq_trigger_e  trigger)
{
#if !defined(SNS_PCSIM)
    int8_t sig_idx;

    int32_t err = 0;
    
    // Lazy initialization of the DDF signal service
    static bool signal_service_initialized = false;
    if(!signal_service_initialized)
    {
      sns_ddf_signal_init();
      signal_service_initialized = true;
    }

    sig_idx = sns_ddf_signal_find_sig_index(gpio_num);
    if(sig_idx == -1)
    {
      sig_idx = sns_ddf_signal_get_free_slot();
      if(sig_idx == -1)
        return SNS_DDF_EFAIL;
    }
    MSG_3(MSG_SSID_SNS, DBG_HIGH_PRIO, "sig_reg - idx(%d) gpio(%d) reg(%d)", 
          sig_idx, gpio_num, sns_ddf_sig_tb.sig_info[sig_idx].is_registered);
    
    DALSYS_SyncEnter(sns_ddf_sig_tb.sync_handle);
  
    sns_ddf_sig_tb.sig_info[sig_idx].gpio_num = gpio_num;
    sns_ddf_sig_tb.sig_info[sig_idx].handle = dd_handle;
    sns_ddf_sig_tb.sig_info[sig_idx].driver = dd_interface;
    sns_ddf_sig_tb.sig_info[sig_idx].trigger = trigger;
    sns_ddf_sig_tb.sig_info[sig_idx].gpio_pin_cfg = sns_ddf_signal_gpio_config(gpio_num);
    sns_ddf_sig_tb.sig_info[sig_idx].is_uimg_refac = sns_ddf_smgr_get_uimg_refac(dd_handle);
    if ( DAL_ERROR ==sns_ddf_signal_gpio_enable(sns_ddf_sig_tb.tlmm_handle,
                                                sns_ddf_sig_tb.sig_info[sig_idx].gpio_pin_cfg, 
                                                DAL_TLMM_GPIO_ENABLE) )
    {
      //log error;
      DALSYS_SyncLeave(sns_ddf_sig_tb.sync_handle);
      return SNS_DDF_EFAIL;
    }

    if(!sns_ddf_sig_tb.sig_info[sig_idx].is_registered)
        {
      {
        err = (int32_t)GPIOInt_RegisterIsr(
            sns_ddf_sig_tb.gpio_int_handle,
            gpio_num,
            (GPIOIntTriggerType)trigger,
            (GPIOINTISR)sns_ddf_smgr_notify_irq,
            sig_idx);
        if (err != -1)
        {
          sns_ddf_sig_tb.sig_info[sig_idx].is_registered = true;
          DDF_MSG_2(MED,"Normal GPIO Int Reg Done [idx(%d) gpio(%d)]",sig_idx,gpio_num);
        }
      }
    }
    DALSYS_SyncLeave(sns_ddf_sig_tb.sync_handle);
#endif
    return SNS_DDF_SUCCESS;
}


/*===========================================================================

  FUNCTION:   sns_ddf_signal_deregister

===========================================================================*/
/*!
  @brief This function de-registers interrupt 

  @detail 

  @param[i]  none
  @return   none
 */
/*=========================================================================*/
sns_ddf_status_e sns_ddf_signal_deregister (uint32_t gpio_num)
{
#if !defined(SNS_PCSIM)
    int8_t sig_idx;
    int32_t err = 0;
    
    sig_idx = sns_ddf_signal_find_sig_index(gpio_num);
    if(sig_idx == -1)
        return SNS_DDF_EINVALID_PARAM;
    
    MSG_3(MSG_SSID_SNS, DBG_HIGH_PRIO, "sig_dereg - idx(%d) gpio(%d) fired(%d)", 
          sig_idx, sns_ddf_sig_tb.sig_info[sig_idx].gpio_num, 
          sns_ddf_sig_tb.sig_info[sig_idx].int_fired); 

    DALSYS_SyncEnter(sns_ddf_sig_tb.sync_handle);
    
    if (sns_ddf_sig_tb.sig_info[sig_idx].is_registered)
    {
      {
        err = (int32_t) GPIOInt_DeregisterIsr(
            sns_ddf_sig_tb.gpio_int_handle,
            gpio_num, 
            (GPIOINTISR)sns_ddf_smgr_notify_irq);
      }
      if (err != -1)
      {
        sns_ddf_sig_tb.sig_info[sig_idx].is_registered = false;
        DDF_MSG_2(MED,"GPIO Int Dereg Done [idx(%d) gpio(%d)]",sig_idx,gpio_num);
      }
      else
      {
        MSG_3(MSG_SSID_SNS, DBG_ERROR_PRIO, "sig_dereg FAILED - idx(%d) gpio(%d) fired(%d)", 
              sig_idx, sns_ddf_sig_tb.sig_info[sig_idx].gpio_num, 
              sns_ddf_sig_tb.sig_info[sig_idx].int_fired); 
      }
    }

    sns_ddf_sig_tb.sig_info[sig_idx].int_fired = false;
    sns_ddf_sig_tb.pending_flags &= ~(1 << sig_idx);
    
    DALSYS_SyncLeave(sns_ddf_sig_tb.sync_handle);
#endif
    return SNS_DDF_SUCCESS;
}


/**
 * Indicates if interrupts are available on the current platform. If interrupt 
 * functionality is not supported, all other functions in this file will return 
 * a failed status.
 *  
 * @return True if interrupts are enabled on this platform. 
 */
bool sns_ddf_signal_irq_enabled (void)
{
    return true;
}


