#ifndef PM_VS_APP_H
#define PM_VS_APP_H

/*! \file
 *  \n
 *  \brief  pm_vs.h : VS - Voltage Switcher
 *  \details  
 *  \n &copy; Copyright 2010 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_vs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
06/04/10   hw      Creation of Voltage Switch (VS) Module
========================================================================== */
#include "pm_lib_err.h"
#include "comdef.h"
#include "pm_resources_and_types.h"

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * Enum used for setting the pin control mode
 */
typedef enum
{
  
  PM_VS_NORMAL_MODE,
 
  PM_VS_LPM_MODE,
  
  PM_VS_LPM_MODE_VIA_SLEEPB,
  
  PM_VS_AUTO_SLEEP_MODE,
  
  PM_VS_SWITCH_MODE_ID__INVALID,

}pm_vs_switch_mode_type;

typedef enum
{
  
  PM_VS_SSPGM_4uA_CURRENT,
 
  PM_VS_SSPGM_12uA_CURRENT,
  
  PM_VS_SSPGM_20uA_CURRENT,
  
  PM_VS_SSPGM_28uA_CURRENT,
  
  PM_VS_SSPGM_CURRENT_ID__INVALID,
}
pm_vs_sspgm_current_type;
/**
 * Enum used for setting the pin control mode
 */
typedef enum
{
  /**
   * sleep_b pin control
   */
  PM_VS_PIN_CTRL_ID__SLEEP_B,
  /**
   * TCXO_EN0 pin control
   */
  PM_VS_PIN_CTRL_ID__TCXO_EN0,
  /**
   * TCXO_EN1 pin control
   */
  PM_VS_PIN_CTRL_ID__TCXO_EN1,
  /**
   * TCXO_EN2 pin control
   */
  PM_VS_PIN_CTRL_ID__TCXO_EN2,
  /**
   * TCXO_EN3 pin control
   */
  PM_VS_PIN_CTRL_ID__TCXO_EN3,
  /**
   * Invalid pin control ID
   */
  PM_VS_PIN_CTRL_ID__INVALID,
}pm_vs_pin_ctrl_id_type;

/*
 * Enum used to set sleep_b pin in the control register.
 */
typedef enum
{
  /* */
  PM_VS_SLEEP_B_PIN_FOLLOW_SLEEP_B,
  /* */
  PM_VS_SLEEP_B_PIN_IGNORE,
} pm_vs_sleep_b_pin_type;

/*
 * Enum used to set force sleep mode in the control register.
 */
typedef enum
{
  /* */
  PM_VS_FORCE_SLEEP_PIN_CONTROLLED,
  /* */
  PM_VS_FORCE_SLEEP_ENTER_SLEEP_MODE,
} pm_vs_force_sleep_type;

/**
 * @brief This function enables the switch mode of the VS.
 * 
 * @details
 *   This function enables the switch mode of the VS.
 *   
 * @param externalResourceIndex The external VS ID.
 *
 * @param pm_vs_switch_mode_type the command used to enable the switch mode
 *        
 * @return     pm_err_flag_type 
 *
 * @note This API is used to set the SWMODE:  Switch mode
 */
pm_err_flag_type pm_vs_switch_mode_switch(int externalResourceIndex, 
                                          pm_vs_switch_mode_type mode);

/**
 * @brief This function sets the soft start pull down current
 * 
 * @details
 *   This function sets the soft start pull down current
 *   
 * @param externalResourceIndex The external VS ID.
 *
 *
 * @param pm_sspgm_current_type current  
 * 
 * @return     pm_err_flag_type 
 */
pm_err_flag_type pm_vs_set_sspgm(int externalResourceIndex, 
                                 pm_vs_sspgm_current_type current);

/**
 * @brief : Over Current indicator
 * 
 * @details
 *   : Over Current indicator
 *   
 * @param externalResourceIndex The external VS ID.
 *
 *
 * @return : boolean Over Current indicator
 *   FALSE: No over current condition
 *   TRUE : Over Current has occurred
 *
 * 
 */
pm_err_flag_type  pm_vs_get_over_current_indicator(int externalResourceIndex, 
                                                   boolean *status);
/**
 * @brief This function returns the Output voltage status
 * 
 * @details
 *   This function returns the Output voltage status
 *   
 * @param externalResourceIndex The external VS ID.
 *
 *
 * @return : boolean Output voltage status
 FALSE = output voltage less than switch threshold value (switched Vout below ~10% of switch input level)
 TRUE = output voltage has crossed the switch threshold value (switch Vout within  ~10% of switch input level)
 * 
 * @return     boolean 
 */
pm_err_flag_type pm_vs_get_output_voltage_status(int externalResourceIndex, 
                                                 boolean *status);
/**
 * @brief This function overrides the softstart
 * 
 * @details
 *   This function overrides the softstart
 *   
 * @param externalResourceIndex The external VS ID.
 * @ param boolean ss_override
 * FALSE = No effect
 * TRUE = Disable soft start; enabling the switch results in hard switching with no current limit
 * 
 * @return     pm_err_flag_type 
 */
pm_err_flag_type pm_vs_soft_start_override(int externalResourceIndex, 
                                           boolean ss_override );
/**
 * @brief This function overrides the overcurrent protection
 * 
 * @details
 *   This function overrides the overcurrent protection
 *   
 * @param externalResourceIndex The external VS ID.
 * @ boolean   over_current_protection
 * FALSE = No effect
 * TRUE = Disable over current protection 

 *  
 * 
 * @return     pm_err_flag_type 
 */
pm_err_flag_type pm_vs_over_current_protection_override(int externalResourceIndex, 
                                                        boolean over_current_protection);

/**
 * @brief This function sets how the sleep mode is controlled.
 * 
 * @details
 *   This function can set the sleep mode to be controlled by pin mode or force
 *   sleep mode to enable immediately.
 *   
 * @param 
 * externalResourceIndex
 *
 * pm_vs_force_sleep_type mode
 *   PM_VS_FORCE_SLEEP_PIN_CONTROLLED
 *   PM_VS_FORCE_SLEEP_ENTER_SLEEP_MODE
 * 
 * @return pm_err_flag_type 
 */
pm_err_flag_type pm_vs_force_sleep_mode(int externalResourceIndex, 
                                        pm_vs_force_sleep_type mode);

/**
 * @brief This function could have sleep/normal mode follows sleep_b pin.
 * 
 * @details
 * This function could set the sleep/normal mode to folow sleep_b pin.
 *   
 * @param 
 * externalResourceIndex
 *
 * pm_vs_force_sleep_type mode
 *   PM_VS_SLEEP_B_PIN_IGNORE
 *   PM_VS_SLEEP_B_PIN_FOLLOW_SLEEP_B
 * 
 * @return pm_err_flag_type 
 */
pm_err_flag_type pm_vs_sleep_b_pin_control(int externalResourceIndex, 
                                           pm_vs_sleep_b_pin_type control);
#ifdef __cplusplus
 }
#endif

#endif /* PM_VS_APP_H */
