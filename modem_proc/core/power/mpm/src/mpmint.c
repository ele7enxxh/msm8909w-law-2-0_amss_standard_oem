/*===========================================================================
  FILE:         mpmint.c
  
  OVERVIEW: This file contains types, function definitions, and
            wrapper functions needed to support legacy driver functions.
 
  @TODO:  This file will be deprecated following migration of
          outside teams to DAL Driver and/or new driver functions.
          (see vmpm.c)
   
  @NOTE:  After migration by external teams to Sleep2.0 MPM
          API's this file will be deprecated along with the
          following additional files:
          mpmint.c (this file)
          mpmint.h
          mpmint_types.h
          :
          :

  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/mpmint.c#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 
#include "mpmint.h"
#include "vmpm_internal.h"
#include "vmpm_HAL.h"
#include "CoreVerify.h"
#include "CoreIntrinsics.h"
#include "vmpm.h"

/*============================================================================
 *                      TYPEDEF DEFINITIONS 
 *===========================================================================*/
/**
 *  @brief This enum type supports the wrapper function for routing legacy
 *         vmpm driver functions to new vmpm driver functions
 */
typedef enum 
{
  MPM_WRAP_CONFIG_AND_ENABLE = 0,
  MPM_WRAP_CONFIG_ONLY,
  MPM_WRAP_ENABLE,
  MPM_WRAP_DISABLE,
} mpm_wrapper_fnc_type;
/*============================================================================
 *                      STATIC FUNCTION DEFINITIONS
 *===========================================================================*/
/**
 * @brief Converts detection type and polarity to equivalent vMPM trigger type.
 *
 * @param detect: Detection type for the interrupt or gpio (edge/level).
 * @param polarity: Polarity of the interrupt or gpio (positive/negative).
 *
 * @return vMPM trigger type equivalent for input detection and polarity values.
 */
static mpm_trigger_type vmpm_getTriggerType ( mpmint_detect_type detect, 
                                              mpmint_polarity_type polarity )
{
  mpm_trigger_type trigger = MPM_TRIGGER_LEVEL_HIGH;

  if ( ( detect   == MPMINT_LEVEL_DETECT ) && 
       ( polarity == MPMINT_ACTIVE_LOW ) )
  {
    trigger = MPM_TRIGGER_LEVEL_LOW;
  }
  else if ( ( detect   == MPMINT_LEVEL_DETECT ) && 
            ( polarity == MPMINT_ACTIVE_HIGH ) ) 
  {
    trigger = MPM_TRIGGER_LEVEL_HIGH;
  }
  else if ( ( detect   == MPMINT_EDGE_DETECT ) && 
            ( polarity == MPMINT_ACTIVE_LOW ) ) 
  {
    trigger = MPM_TRIGGER_FALLING_EDGE;
  }
  else if ( ( detect   == MPMINT_EDGE_DETECT ) && 
            ( polarity == MPMINT_ACTIVE_HIGH ) ) 
  {
    trigger = MPM_TRIGGER_RISING_EDGE;
  }
  else if ( ( detect   == MPMINT_EDGE_DETECT ) && 
            ( polarity == MPMINT_ACTIVE_DUAL ) )
  {
    trigger = MPM_TRIGGER_DUAL_EDGE;
  }
  else 
  {
    vmpm_LogPrintf(2, "Invalid detect and/or polarity: %d %d", 
                   (int)detect, (int)polarity);
    CORE_VERIFY(0);
  }

  return trigger;
}

/**
 *  vmpm_sleep2pt0_wrapper
 *  
 *  @brief This wrapper function is used to call the new 
 *         Sleep2.0 MPM API functions until extern teams migrate
 *         to new Sleep2.0 MPM API function.
 *  
 * @param vmpmFunctionType 
 *        Wrapper uses this parameter to determine which MPM
 *        Sleep2.0 function to call
 * @param mpmPinNum 
 *        MPM Pin Number (or GPIO number if pinType is MPM_GPIO)
 * @param pinType 
 *        MPM_GPIO or MPM_IRQ 
 * @param triggerType 
 *        TRIGGER type (if applicable) for configuration 
 */
static void vmpm_sleep2pt0_wrapper( uint32 mpmPinNum,
                                    mpm_wrapper_fnc_type vmpmFunctionType,
                                    mpm_pin_type pinType,
                                    mpm_trigger_type triggerType)
{
  mpm_status_type status = MPM_FAIL;
  mpm_pin_config_type pinConfigInfo;

  /* Provide configuration info */
  pinConfigInfo.pin_type = pinType;
  pinConfigInfo.trig_config = triggerType;

  switch (vmpmFunctionType) 
  {
    case MPM_WRAP_CONFIG_AND_ENABLE:
      status = vmpm_configAndEnablePin(mpmPinNum, &pinConfigInfo);
      break;

    case MPM_WRAP_CONFIG_ONLY:
      status = vmpm_configPin(mpmPinNum, &pinConfigInfo);
      break;

    case MPM_WRAP_ENABLE:
      status = vmpm_enablePin(mpmPinNum);
      break;

    case MPM_WRAP_DISABLE:
      status = vmpm_disablePin(mpmPinNum);
      break;

    default:
      vmpm_LogPrintf( 1, "WARNING: Invalid MPM Wrapper function type ( val: %u)",
                         vmpmFunctionType );
      break;
  }

  if (MPM_FAIL == status) 
  { 
    vmpm_LogPrintf( 1, "WARNING: MPM_FAIL in MPM Sleep2.0 Wrapper call ( val: %u)",
                        vmpmFunctionType );
  }
}

/*============================================================================
 *                      EXTERNAL FUNCTION DECLARATIONS
 *===========================================================================*/
/** 
 *  mpmint_config_int 
 *  
 *  @brief Configure the IRQ corresponding to the input MPM pin number 
 */
void mpmint_config_int( uint32 mpmPinNum,
                        mpmint_detect_type detect,
                        mpmint_polarity_type  polarity )
{
  /* New Sleep2.0 will use Trigger type rather than detect & polarity types
   * Get Trigger type from input detect & polarity types */
  mpm_trigger_type triggerType = vmpm_getTriggerType(detect, polarity);

  /* Call Sleep2.0 Wrapper function which will call new Sleeep2.0 API functions */
  vmpm_sleep2pt0_wrapper( mpmPinNum,
                          MPM_WRAP_CONFIG_ONLY,
                          MPM_IRQ,
                          triggerType);
}

/**
 *  vmpm_config_wakeup
 *  
 *  @brief Enable the IRQ corresponding to the inpu MPM pin
 *         number
 *  
 * @NOTE: This function only enabled the input pin 
 *        i.e. function name was misnomer 
 */
void mpmint_config_wakeup ( mpmint_isr_type mpmPinNum, 
                            mpmint_processor_type unused )
{
  /* Trigger type unused for this function */
  mpm_trigger_type triggerType = MPM_TRIGGER_DUAL_EDGE;

  /* Call Sleep2.0 Wrapper function which will call new Sleeep2.0 API functions */
  vmpm_sleep2pt0_wrapper( mpmPinNum,
                          MPM_WRAP_ENABLE,
                          MPM_IRQ,
                          triggerType);
}

/**
 *  vmpm_disable_gpio_wakeup 
 *  
 *  @brief Disables the MPM Pin Number mapped to the input GPIO
 *         number
 */ 
void mpmint_disable_gpio_wakeup ( mpmint_isr_type gpio_num ) 
{
  uint32 mpmPinNum = 0;

  /* Trigger type unused for this function */
  mpm_trigger_type triggerType = MPM_TRIGGER_DUAL_EDGE;
           
  /* Get the MPM Pin Number from the input GPIO number
   * NOTE: Sleep2.0 MPM API's will accept MPM Pin Number for all API
   * functions;  so this translation will only be required 
   * for wrapper */ 
  if ( MPM_SUCCESS == vmpm_queryPinMapping(gpio_num, MPM_GPIO, &mpmPinNum) )
  {
    /* Call Sleep2.0 Wrapper function which will call new Sleeep2.0 API functions */
    vmpm_sleep2pt0_wrapper( mpmPinNum,
                            MPM_WRAP_DISABLE,
                            MPM_GPIO,
                            triggerType);
    vmpm_LogPrintf( 2, "Disabling GPIO (GPIO id: %u) (MPM Pin Number: %u)",
                    gpio_num, mpmPinNum);

  }
  else
  {
    vmpm_LogPrintf( 1, "WARNING: Invalid GPIO disable request (GPIO Pin Num: %u)",
                    mpmPinNum );
  }
}
  
/**
 *  vmpm_disable_wakeup
 *  
 *  @brief Disables the IRQ mapped to the input MPM Pin Number
 */
void mpmint_disable_wakeup( uint32 mpmPinNum )
{
  /* Trigger type unused for this function */
  mpm_trigger_type triggerType = MPM_TRIGGER_DUAL_EDGE;

  /* Call Sleep2.0 Wrapper function which will call new Sleeep2.0 API functions */
  vmpm_sleep2pt0_wrapper( mpmPinNum,
                          MPM_WRAP_DISABLE,
                          MPM_IRQ,
                          triggerType);
}  

/** 
 *  mpmint_config_gpio_wakeup
 *  
 *  @brief Configure and enable the GPIO provided
 */
void mpmint_config_gpio_wakeup( uint32 gpio_num,
                                mpmint_detect_type detect,
                                mpmint_polarity_type  polarity,
                                mpmint_processor_type unused )
{
  /* New Sleep2.0 will use Trigger type rather than detect & polarity types
   * Get Trigger type from input detect & polarity types */
  mpm_trigger_type triggerType = vmpm_getTriggerType(detect, polarity);

  /* Get the MPM Pin Number from the input GPIO number
   * NOTE: Sleep2.0 MPM API's will accept MPM Pin Number for all API
   * functions;  so this translation will only be required 
   * for wrapper */ 
  uint32 mpmPinNum = 0;
                              
  if ( MPM_SUCCESS == vmpm_queryPinMapping(gpio_num, MPM_GPIO, &mpmPinNum) )
  {
    /* Call Sleep2.0 Wrapper function which will call new Sleeep2.0 API functions */
    vmpm_sleep2pt0_wrapper( mpmPinNum,
                            MPM_WRAP_CONFIG_AND_ENABLE,
                            MPM_GPIO,
                            triggerType);

    vmpm_LogPrintf( 3, "Enabling GPIO (Local id: %u) (MPM Pin Num: %u) "
                    "(Trig Type: %d)", 
                    gpio_num,
                    mpmPinNum,
                    (int)triggerType );
  }
  else
  {
    vmpm_LogPrintf( 1, "WARNING: Invalid GPIO config request (MPM Pin Num: %u)",
                    mpmPinNum );
  }
}

