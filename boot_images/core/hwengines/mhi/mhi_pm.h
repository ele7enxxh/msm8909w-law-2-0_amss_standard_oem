#ifndef MHI_PM_H
#define MHI_PM_H

/** 
  @file mhi_pm.h
  @brief
  This header file defines data structures and public apis associated with mhi power management. 
    

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

#include "mhi_sm.h"
#include "mhi_link.h"
#include "mhi_osal.h"

#ifndef MHI_BOOT
#include "DDIGPIOInt.h"
#include "DDITlmm.h"
#endif


/** Device power state notification received from the MHI link */
typedef enum _mhi_link_d_state
{
  MHI_LINK_D0=0,
  MHI_LINK_D1=1,
  MHI_LINK_D2=2,
  MHI_LINK_D3hot=3,
  MHI_LINK_D3cold=4,
  MHI_LINK_INVALID_D_STATE
} mhi_link_d_state_t;

/** MHI power management related information */
typedef struct _mhi_pm_info
{
  mhi_link_d_state_t current_d_state; /**< Current D state as last informed by the MHI link */

  volatile uint32 device_wake_gpio_state; /**< State of device wake GPIO */

  boolean mhi_sleep_vote_cast; /**< Status of MHI's vote for sleep */
} mhi_pm_info_t;
  
#ifndef MHI_BOOT
/** GPIO handle */
typedef struct _mhi_gpio_handle
{
  DalDeviceHandle    *h_mhi_tlmm;
  DalDeviceHandle    *h_mhi_gpio_int;
  DALGpioSignalType   mhi_gpio_pin;
} mhi_gpio_handle_t;
#endif

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
uint32 mhi_pm_init(mhi_pm_info_t *pm_info);

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
void mhi_pm_reset(void);

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
void mhi_pm_hw_acc_wakeup(void);

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
void mhi_pm_process_d_state_change(mhi_link_d_state_t d_state);

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
void mhi_pm_wakeup_host(void);

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
void mhi_pm_wakeup_device_req_fromHost(void);

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
void mhi_pm_vote_for_sleep(void);

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
void mhi_pm_vote_against_sleep(void);

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
void mhi_pm_set_d_state(mhi_link_d_state_t d_state);

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
mhi_link_d_state_t mhi_pm_get_d_state(void);

/**
 * @brief       mhi_pm_get_device_wake_gpio_state
 *
 *              Get the current state of the device_wake gpio.
 * 
 * @dependencies
 *              Should be called only after mhi_pm_init. 
 * 
 * @return       
 *              TRUE\FALSE depending on ASSERTION\DEASSERTION of device_wake.  
 * 
 * @sideeffect  None 
 */
boolean mhi_pm_get_device_wake_gpio_state(void);

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
boolean mhi_pm_is_sleep_voted(void);

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
void mhi_trigger_device_wake_intr(void);

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
void mhi_pm_core_wakeup(void);

#endif
