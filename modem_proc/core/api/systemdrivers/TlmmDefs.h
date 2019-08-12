#ifndef __TLMMDEFS_H__
#define __TLMMDEFS_H__
/**
  @file TlmmDefs.h 
  
  Public interface include file for accessing the TLMM DAL device.
  
  The TlmmDefs.h file is the public interface to the Top Level Mode Multiplexor 
  (TLMM) Device Access Library (DAL) configurations.
*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/TlmmDefs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/10   sw      (Tech Pubs) edited Doxygen comments and markup; modified
                   Doxygen markup to meet Living Docs standards.
06/17/09   dcf     Fix for embedded output.
02/23/09   dcf     Added comments and versioning.
=============================================================================
Copyright (c) 2010 - 2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.  
QUALCOMM Proprietary and Confidential. 
===========================================================================*/



/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

/*==========================================================================

                   PUBLIC DEFINITIONS FOR MODULE

==========================================================================*/

/** 
  @addtogroup const_macros
  @{ 
*/ 

/**
  Macro that defines the TLMM DAL version currently in use. 
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DALTLMM_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)
#else
#define DALTLMM_INTERFACE_VERSION
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */


/** 
  Configuration version 1.1 support.
  To satisfy the demand for backward compatibility with regard to configuration 
  bitfield lengths, the DAL_GPIO_VERSION bit was introduced. This bit specifies 
  that the GPIO configuration conforms to interface version 1.1, which supports 
  wider bitfields. 
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_VERSION 0x20000000
#else
#define DAL_GPIO_VERSION
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/** 
  Main configuration macro. 
    
  This main configuration macro is used when configuring a GPIO. It 
  utilizes the DAL_GPIO_VERSION definition to ensure that configurations conform 
  to the wider field size.\n
  
  This macro creates a GPIO configuration that is passed into APIs requiring a 
  DALGpioSignalType parameter. 
    
  @param gpio  [in] -- GPIO number to program.
  @param func  [in] -- Function selection (0 for generic I/O).
  @param dir   [in] -- Direction (input or output).
  @param pull  [in] -- Pull value (up, down, or none).
  @param drive [in] -- Drive strength to program.
   
  @sa
  DalTlmm_ConfigGpio
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_CFG(gpio, func, dir, pull, drive) \
         (((gpio) & 0x3FF)<< 4 | \
          ((func) & 0xF)|        \
          ((dir)  & 0x1) << 14|  \
          ((pull) & 0x3) << 15|  \
          ((drive)& 0xF) << 17| DAL_GPIO_VERSION)
#else
#define DAL_GPIO_CFG(gpio, func, dir, pull, drive)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/**
  GPIO number extraction macro.
    
  This extraction macro is used to retrieve the GPIO number from a DAL_GPIO_CFG
  formatted configuration.
 
  @param config [in] -- DAL_GPIO_CFG format configuration.
 
  @sa
  DAL_GPIO_CFG
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_NUMBER(config)    (((config) >> 4) & 0x3FF)
#else
#define DAL_GPIO_NUMBER(config)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/** 
  Function select extraction macro. 
    
  Extraction macro used to retrieve the function select from a DAL_GPIO_CFG 
  formatted configuration. 
    
  @param config [in] -- DAL_GPIO_CFG format configuration.
    
  @sa
  DAL_GPIO_CFG
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_FUNCTION(config)   ((config) & 0xF)
#else
#define DAL_GPIO_FUNCTION(config)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */
/** 
  Direction value extraction macro.
    
  Extraction macro used to retrieve the direction from a DAL_GPIO_CFG formatted 
  configuration. 
    
  @param config [in] -- DAL_GPIO_CFG format configuration.
    
  @sa
  DAL_GPIO_CFG 
*/ 
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_DIRECTION(config) (((config) >> 14) & 0x1)
#else
#define DAL_GPIO_DIRECTION(config)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/** 
  Pull value extraction macro.
    
  This extraction macro is used to retrieve the pull value from a DAL_GPIO_CFG 
  formatted configuration. 
    
  @param config [in] -- DAL_GPIO_CFG format configuration.
    
  @sa
  DAL_GPIO_CFG
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_PULL(config)      (((config) >> 15) & 0x3)
#else
#define DAL_GPIO_PULL(config)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/** 
  Drive strength extraction macro.
    
  Extraction macro used to retrieve the drive strength from a DAL_GPIO_CFG 
  formatted configuration. 
    
  @param config [in] -- DAL_GPIO_CFG format configuration.
    
  @sa
  DAL_GPIO_CFG 
*/ 
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_DRIVE(config)     (((config) >> 17) & 0xF)
#else
#define DAL_GPIO_DRIVE(config)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/** 
  Invalid GPIO number marker.
    
  This marker specifies a GPIO as invalid. It can be useful to mark a GPIO as 
  invalid in the sleep table so that low-power reconfiguration of a particular 
  GPIO is bypassed during MSM sleep.
  
  This definition can be used in place of the GPIO number in the 
  DAL_GPIO_CFG macro. 
    
  @param config [in] -- DAL_GPIO_CFG format configuration.
    
  @sa
  DAL_GPIO_NUMBER, DAL_GPIO_CFG 
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_INVALID_PIN  0x3FF
#else
#define DAL_GPIO_INVALID_PIN
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */


/** 
  Special case configuration macro with an embedded output specification. 
    
  This is a special case GPIO macro that allows an output value to be embedded 
  in the configuration. The specified output value is written directly after 
  the GPIO is configured. This is typically used when specifying an inactive GPIO 
  configuration where the low-power output value must be stored.
    
  @param gpio   [in] -- GPIO number to program.
  @param func   [in] -- Function select (0 for generic I/O).
  @param dir    [in] -- Direction (input or output).
  @param pull   [in] -- Pull value (up, down, or none).
  @param drive  [in] -- Drive strength to program.
  @param outval [in] -- DALGpioValueType value (HIGH or LOW).
    
  @sa
  DALGpioValueType, DAL_GPIO_CFG
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_CFG_OUT(gpio, func, dir, pull, drive, outval) \
          (DAL_GPIO_CFG((gpio), (func), (dir), (pull), (drive))\
          | (((outval)|0x2) << 0x15))
#else
#define DAL_GPIO_CFG_OUT(gpio, func, dir, pull, drive, outval)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/** 
  Extraction macro to retrieve the output value of a GPIO.
    
  This macro returns the output value of a particular GPIO configuration when 
  either internally set or set by DAL_GPIO_CFG_OUT. This macro is not associated 
  with any value written to the hardware. Its sole purpose is to extract an 
  output value from a configuration. It is typically used when modifying an 
  inactive GPIO configuration. 
    
  @param config [in] -- DAL_GPIO_CFG format configuration,
   
  @sa
  DAL_GPIO_CFG_OUT 
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_GET_OUTVAL(config) (((config) >> 0x15) & 0x1)
#else
#define DAL_GPIO_GET_OUTVAL(config)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/** 
  @}
*/ /* end_group const_macro */

/*==========================================================================

                      PUBLIC DATA STRUCTURES

==========================================================================*/



/** 
  @addtogroup dir_enums 
  @{ 
*/ 

/** 
  Mode of a GPIO.
    
  Enumeration allowing a particular GPIO ID to be toggled 
  between its primary function and an I/O mode. 
*/

typedef enum
{
   DAL_GPIO_MODE_PRIMARY,
   DAL_GPIO_MODE_IO,
   DAL_PLACEHOLDER_DALGpioModeType = 0x7fffffff
}DalGpioModeType;


/** 
  State of a GPIO.
    
  The DALGpioStatusType enumerations are used to tell if a particular GPIO is 
  configured to an active or inactive state. 
*/

typedef enum
{
  DAL_GPIO_INACTIVE = 0,  /**< -- Inactive state. */
  DAL_GPIO_ACTIVE   = 1,  /**< -- Active state. */
/** @cond */
  DAL_PLACEHOLDER_DALGpioStatusType = 0x7fffffff
/** @endcond */
}DALGpioStatusType;


/** 
  GPIO ownership value.
    
  The DALGpioOwnerType enumerations are used to get the current processor owner 
  of a particular GPIO. 
*/
typedef enum
{
  DAL_GPIO_OWNER_MASTER,  /**< -- Secure Root Of Trust (sROT). */
  DAL_GPIO_OWNER_APPS1,    /**< -- Applications procesor 1. */
/** @cond */
  DAL_PLACEHOLDER_DALGpioOwnerType = 0x7fffffff
/** @endcond */
}DALGpioOwnerType;


/** 
  Locking values for GPIO.
    
  The DALGpioLockType enumerations are rudimentary mechanisms provided for 
  debugging to ensure that a GPIO does not get changed, either by another driver 
  or by the internal TLMM driver until it is unlocked.
  
  @note This mechanism is strictly <i>use at your own risk</i>. Use it only for 
  debugging in cases where it is necessary. Do not use it as an overall 
  solution. 
*/

typedef enum
{
  DAL_GPIO_UNLOCKED = 0,    /**< -- Allow GPIO configuration overwrites. */
  DAL_GPIO_LOCKED   = 1,    /**< -- Deny configuration overwrites. */

/** @cond */
  DAL_PLACEHOLDER_DALGpioLockType = 0x7fffffff
/** @endcond */

}DALGpioLockType;


/** 
  Locking mechanism clients.
    
  The DALGpioClientType is used in conjunction with the DALGpioLockType and 
  associated APIs to manipulate the locking mechanism of a GPIO.\n
  
  @note Locking a GPIO does not ensure that it cannot be unlocked by another 
  driver, so use it with extreme caution and only for debugging purposes.
*/
typedef enum
{
  DAL_GPIO_CLIENT_PWRDB = 0,   /**< -- Power debug client. */

/** @cond */
  DAL_PLACEHOLDER_DALGpioClientType = 0x7fffffff
/** @endcond */

}DALGpioClientType;

/** 
  @}
*/ /*end_group dir_enums */


/*==========================================================================

                      PUBLIC GPIO CONFIGURATION MACROS

==========================================================================*/

/** 
  @addtogroup config_enums 
  @{ 
*/

/** 
  Direction enumerations for configuration. 
    
  The DALGpioDirectionType enumeration specifies the direction to use in the 
  configuration of a GPIO: an INPUT to the MSM or an OUTPUT from the MSM. 
    
  @sa
  DAL_GPIO_CFG
*/

typedef enum
{
  DAL_GPIO_INPUT  = 0,     /**< -- Set to INPUT. */
  DAL_GPIO_OUTPUT = 1,     /**< -- Set to OUTPUT. */

/** @cond */
  DAL_PLACEHOLDER_DALGpioDirectionType = 0x7fffffff
/**@endcond */

}DALGpioDirectionType;



/** 
  Pull value for a GPIO configuration.
    
  The DALGpioPullType specifies what type of pull (if any) to use in the 
  configuration of a particular GPIO. 
    
  @sa
  DAL_GPIO_CFG 
*/

typedef enum
{
  DAL_GPIO_NO_PULL    = 0,   /**< -- Do not specify a pull. */
  DAL_GPIO_PULL_DOWN  = 0x1, /**< -- Pull the GPIO down. */
  DAL_GPIO_KEEPER     = 0x2, /**< -- Designate as a Keeper. */
  DAL_GPIO_PULL_UP    = 0x3, /**< -- Pull the GPIO up. */

/** @cond */
  DAL_PLACEHOLDER_DALGpioPullType = 0x7fffffff
/** @endcond */

}DALGpioPullType;


/** 
  Drive strength values 
    
  The DALGpioDriveType enumeration specifies the drive strength to use in the 
  configuration of a GPIO. 
    
  @sa
  DAL_GPIO_CFG 
*/

typedef enum
{
  DAL_GPIO_2MA     = 0,    /**< -- Specify a 2 mA drive. */
  DAL_GPIO_4MA     = 0x1,  /**< -- Specify a 4 mA drive. */
  DAL_GPIO_6MA     = 0x2,  /**< -- Specify a 6 mA drive. */
  DAL_GPIO_8MA     = 0x3,  /**< -- Specify an 8 mA drive. */
  DAL_GPIO_10MA    = 0x4,  /**< -- Specify a 10 mA drive. */
  DAL_GPIO_12MA    = 0x5,  /**< -- Specify a 12 mA drive. */
  DAL_GPIO_14MA    = 0x6,  /**< -- Specify a 14 mA drive. */
  DAL_GPIO_16MA    = 0x7,  /**< -- Specify a 16 mA drive. */

/** @cond */
  DAL_PLACEHOLDER_DALGpioDriveType = 0x7fffffff
/** @endcond */

}DALGpioDriveType;


/**
  Output value specification for general purpose I/O.
    
  The DALGpioValueType specifies the value to write to GPIO lines configured as 
  OUTPUT with function select 0 (also known as <i>bit banging</i>).
    
  @sa
  DAL_GPIO_CFG_OUT, DalTlmm_GpioOut, DalTlmm_GpioOutGroup
*/

typedef enum
{
  DAL_GPIO_LOW_VALUE,     /**< -- Drive the output LOW. */
  DAL_GPIO_HIGH_VALUE,    /**< -- Drive the output HIGH. */

/** @cond */
  DAL_PLACEHOLDER_DALGpioValueType = 0x7fffffff
/** @endcond */

}DALGpioValueType;


/** 
  Signal Type alias.
    
  The DALGpioSignalType is an alias for a 32-bit configuration value. The type 
  name can be used by any calling drivers, but the underlying configuration is 
  used as an unsigned 32-bit integer.
    
  @sa
  DAL_GPIO_CFG 
*/


/** @cond */
#define DAL_PLACEHOLDER_DALGpioSignalType 0x7fffffff
/** @endcond */

typedef uint32 DALGpioSignalType;


/** 
  GPIO Identifier type.
    
  The DALGpioIdType identifies a particular GPIO configuration 
  pertaining to a GPIO string name. 
    
  @sa
  DalTlmm_GetGpioId, DalTlmm_ConfigGpioId, 
  DalTlmm_ConfigGpioIdInactive 
*/
typedef uint32 DALGpioIdType;

/** 
  Low-power subset enumerations.
    
  The DALGpioSleepConfigType defines the subset of GPIOs to configure to 
  low-power states during power collapse or TCXO shutdown.
  
  @note These enumerations must be used only by the proper power drivers. They 
  are not for generic use because they can affect every GPIO in the system.
    
  @sa
  DalTlmm_ConfigGpiosForSleep, DalTlmm_RestoreGpiosFromSleep
*/

typedef enum
{
  DAL_GPIO_SLEEP_CONFIG_ALL,   /**< -- Handle all GPIOs. */
  DAL_GPIO_SLEEP_CONFIG_APPS,  /**< -- Handle the application processor-owned GPIO. */
  DAL_GPIO_SLEEP_CONFIG_NUM_MODES, /**< -- Number of sleep mode GPIO manipulators. */
/** @cond */
  DAL_PLACEHOLDER_DALGpioSleepConfigType = 0x7fffffff
/** @endcond */
}DALGpioSleepConfigType;


/** 
  Enable or disable a GPIO.
    
  The DALGpioEnableType enumerations specify whether a GPIO is to be configured 
  to an active (enable) or inactive (disable) state. 
    
  @sa
  DalTlmm_ConfigGpio, DalTlmm_ConfigGpioGroup 
*/

typedef enum 
{
  DAL_TLMM_GPIO_DISABLE,  /**< -- Use the internal inactive configuration. */
  DAL_TLMM_GPIO_ENABLE,   /**< -- Use the configuration passed as parameter. */

/** @cond */
  DAL_PLACEHOLDER_DALGpioEnableType = 0x7fffffff
/** @endcond */

}DALGpioEnableType;

/** 
  @}
*/ /* end_group config_enums */


/** 
  @addtogroup block_configs 
  @{ 
*/

/** 
  TLMM port selection enumerations.
    
  The DALGpioPortType is a port configuration enumeration for publicly available 
  TLMM ports, which abstract miscellaneous TLMM registers and are used with the 
  DalTlmm_SetPort API.
  
  @note Additions to this enumeration and associated functionality are added 
  on a needed basis, and not every enumeration is available or defined on every 
  target). 
    
  @sa
  DalTlmm_SetPort
*/

/* ====================================================================
   New Ports must be added to the end of this enumeration (just before 
   DAL_TLMM_NUM_PORTS).
=====================================================================*/
typedef enum
{
  DAL_TLMM_USB_PORT_SEL,         /**< -- USB port selection */
  DAL_TLMM_PORT_CONFIG_USB,      /**< -- Configuration port for USB. */
  DAL_TLMM_PORT_TSIF,            /**< -- Transport Stream InterFace port 
                                         selection. */
  DAL_TLMM_AUD_PCM,              /**< -- Audio configuration selector. */
  DAL_TLMM_UART1,                /**< -- UART 1 configuration selector. */
  DAL_TLMM_UART3,                /**< -- UART 3 configuration selector. */
  DAL_TLMM_LCDC_CFG,             /**< -- Display configuration for LCD. */
  DAL_TLMM_KEYSENSE_SC_IRQ,      /**< -- Keysense visibility on the Scorpion 
                                         processor. */
  DAL_TLMM_KEYSENSE_A9_IRQ,      /**< -- Keysense visibility on the 
                                         ARM9&reg;&nbsp; processor. */
  DAL_TLMM_TCXO_EN,              /**< -- TCXO enable functionality. */
  DAL_TLMM_SSBI_PMIC,            /**< -- PMIC SSBI pin configuration. */
  DAL_TLMM_PA_RANGE1,            /**< -- Power Amplifier Range 1 configuration. */
  DAL_TLMM_SPARE_WR_UART_SEL,    /**< -- UART selector configuration. */
  DAL_TLMM_PAD_ALT_FUNC_SDIO_EN, /**< -- SDIO function selector. */
  DAL_TLMM_SPARE_WR_TCXO_EN,     /**< -- Spare TCXO enable. */
  DAL_TLMM_SPARE_WR_PA_ON,       /**< -- Power Amplifier On selection. */
  DAL_TLMM_SDC3_CTL,             /**< -- Slot 3 for the Secure Digital Card. */
  DAL_TLMM_SDC4_CTL,             /**< -- Slot 4 for the Secure Digital Card.  */
  DAL_TLMM_UIM1_PAD_CTL,         /**< -- UIM1 PAD Control support. */
  DAL_TLMM_UIM2_PAD_CTL,         /**< -- UIM2 PAD Control support. */
  DAL_TLMM_RFFE_CTL,             /**< -- RFFE Control register support. */  

/** @cond */
  DAL_TLMM_NUM_PORTS,
  DAL_PLACEHOLDER_DALGpioPortType = 0x7fffffff
/** @endcond */
}DALGpioPortType;

/** 
  TLMM GPIO Config ID Type.
    
  This type defines the configurable portions of a GPIO whose 
  pin number, and function select are derived at runtime through
  device config using the API DalTlmm_GetGpioId(). 
   
  @sa
  DalTlmm_GetGpioId, DalTlmm_ConfigGpioId
*/

typedef struct
{
  DALGpioDirectionType eDirection;
  DALGpioPullType ePull;
  DALGpioDriveType eDriveStrength;

}DalTlmm_GpioConfigIdType;


/**
  TLMM GPIO settings type

  This structure defines the parameters which can be read from the 
  TLMM HW for testing and other purposes.
*/

typedef struct
{
  uint32                   nGpioNumber;
  uint32                   nFunctionSelect;
  DalTlmm_GpioConfigIdType Settings;
  DALGpioValueType         eOutputDrive;
}DalTlmm_GpioIdSettingsType;

/** 
  @}
*/ /* end_group block_configs */


#endif

