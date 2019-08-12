/*
=================================================================================

                  T O P  L E V E L  M O D E  M U X  ( T L M M ) 
              H A R D W A R E   A B S T R A C T I O N   L A Y E R
                               F O R   8 9 6 0 

FILE:         HALtlmmCommon.c

DESCRIPTION:  
              This modules contains common functionality to the application
              processor and the master processor.

=================================================================================

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/tlmm/hw/v1/HALtlmm.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------
08/05/10   dcf     Created.

=================================================================================
             Copyright (c) 2011 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=================================================================================
*/

/*===============================================================================
                         INCLUDE FILES FOR MODULE
===============================================================================*/
#include <HALcomdef.h>
#include <HALhwio.h>
#include "HALtlmm.h"
#include "HALtlmmHWIO.h"


/*===============================================================================
                    GLOBAL STRUCTURES FOR THIS MODULE
===============================================================================*/



/* ==============================================================================
   The following macro takes the configuration values from the BSP format used
   by the driver and restructures them to fit the format of the configuration
   registers for this target.
   ----------------------------------------------------------------------------*/

#define HAL_GPIO_CONFIG_MASK(cfg)   \
    ((HAL_PULL_VAL(cfg)   << 0x0) | \
     (HAL_FUNC_VAL(cfg)   << 0x2) | \
     (HAL_DRVSTR_VAL(cfg) << 0x6) | \
     (HAL_DIR_VAL(cfg)    << 0x9) )


static HAL_tlmm_InitType tHALTLMMInit;


/*===============================================================================
  ===============================================================================
                     FUNCTIONS CONTAINED WITHIN THIS MODULE
===============================================================================*/



/*===============================================================================

FUNCTION  
  void HAL_tlmm_Init ( HAL_tlmm_InitType* pInit)

DESCRIPTION
   Does the initializing of the hardware buffers and address structures.

PARAMETERS
   Pointer to the version of this HAL module.

DEPENDENCIES
   None.

RETURN VALUE
   The current version via input parameter.

SIDE EFFECTS
   None.
===============================================================================*/

void HAL_tlmm_Init( HAL_tlmm_InitType* pInit )
{
  /* Initialize the registers and buffers. */
  if( pInit )
  {
    tHALTLMMInit.nNumGpio = pInit->nNumGpio;
    tHALTLMMInit.nBaseAddress = pInit->nBaseAddress;
  }

}/* END HAL_tlmm_Init */


/*===============================================================================

FUNCTION  
  void HAL_tlmm_WriteConfig( uint32 nGpioNumber, uint32 nConfig )

DESCRIPTION
   Programs the GPIOn_PAGE and GPIOn_CFG registers based on the input 
   parameters.  For APPS processor, this function is stubbed out.

PARAMETERS
   nGpioNumber  - GPIO number to configure
   nGroup       - The registe group that this GPIO number belongs to.
   nConfig      - The masked configuration for this GPIO.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===============================================================================*/
void HAL_tlmm_WriteConfig( uint32 nGpioNumber, uint32 nConfig )
{
  if( nGpioNumber < tHALTLMMInit.nNumGpio )
  {
    HWIO_GPIO_CFGn_OUTI(tHALTLMMInit.nBaseAddress, nGpioNumber, nConfig);
  }
}


/*===============================================================================

FUNCTION  
   void HAL_tlmm_Reset( void )

DESCRIPTION
   Will program the reset state of the TLMM block.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===============================================================================*/

void HAL_tlmm_Reset( void )
{
  /* Reserved for future use. */
}


/*===============================================================================

FUNCTION  
  void HAL_tlmm_ConfigGpio ( uint32 nWhichConfig )

DESCRIPTION
   Configures the specified GPIO to the requested configuration at the
   provided index.

PARAMETERS
   Packed configuration data containing GPIO number and configuration info.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===============================================================================*/

void HAL_tlmm_ConfigGpio( uint32 nWhichConfig )
{
  uint32 nWhichGpio = (uint32)HAL_GPIO_NUMBER(nWhichConfig);

  if( nWhichGpio < tHALTLMMInit.nNumGpio )
  {
     /* If an output value was specified. */
    if( HAL_GPIO_OUTVAL(nWhichConfig) == HAL_TLMM_OUTPUT_LOW )
    {
      HAL_tlmm_WriteGpio( nWhichConfig, FALSE );
    }
    else if( HAL_GPIO_OUTVAL(nWhichConfig) == HAL_TLMM_OUTPUT_HIGH )
    {
      HAL_tlmm_WriteGpio( nWhichConfig, TRUE );
    } 
 
    HAL_tlmm_WriteConfig(nWhichGpio, (uint32)HAL_GPIO_CONFIG_MASK(nWhichConfig));

  }
}


/*===============================================================================
FUNCTION  
  void HAL_tlmm_Save ( void )

DESCRIPTION
   Saves the contents of the GPIO registers.

PARAMETERS
   None.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===============================================================================*/
void HAL_tlmm_Save( void )
{
  /* Nothing to do here. */
}


/*===============================================================================
FUNCTION  
  void HAL_tlmm_Restore( void )

DESCRIPTION
   Restores the state of the GPIO_OUT and GPIO_OE registers.

PARAMETERS
   None.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===============================================================================*/
void HAL_tlmm_Restore( void )
{
  /* Nothing to do here. */
}


/*===============================================================================

FUNCTION  
  void HAL_tlmm_WriteGpio( uint32 nWhichConfig, bool bValue )

DESCRIPTION
   Outputs the supplied value to the GPIO register at the GPIO location..

DEPENDENCIES
   The GPIO should be configured to be output before calling this function;
   otherwise, the value written to the GPIO may not take effect.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===============================================================================*/
void HAL_tlmm_WriteGpio( uint32 nWhichConfig, boolean bValue )
{
  uint32 nWhichGpio = (uint32)HAL_GPIO_NUMBER(nWhichConfig);

  if(nWhichGpio < tHALTLMMInit.nNumGpio)
  {
    if(TRUE == bValue)
    {
      HWIO_GPIO_IN_OUTn_OUTMI(tHALTLMMInit.nBaseAddress, nWhichGpio, 0x2, 0x2);
    }
    else
    {
      HWIO_GPIO_IN_OUTn_OUTMI(tHALTLMMInit.nBaseAddress, nWhichGpio, 0x2, 0x0);
    }
  }
}


/*===============================================================================

FUNCTION  
  void HAL_tlmm_ConfigGpioGroup ( uint32 nWhichGpioSet[], 
                                  uint32 nWhichCfgSet[], 
                                  uint32 size )

DESCRIPTION
   Configures a group of GPIOs based on the supplied input parameters.  Each
   array contains a GPIO/Configuration set, but we extract only the GPIO from
   the first array and the configuration from the second.  This allows GPIO
   groups to be set to several different configurations, such as primary,
   sleep, or default group configuration.

PARAMETERS
   nWhichGpioSet[]  - An array of GPIO configurations to extract the GPIO 
                      numbers to configure.
   nWhichCfgSet[]   - An array of GPIO configurations to extract the GPIO
                      configuration information to set the above GPIOs to.
   nWhatSize        - The size of the first array.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===============================================================================*/
void HAL_tlmm_ConfigGpioGroup
(
  uint32 *nWhichGpioSet,
  uint32 nWhatSize
)
{
  uint32 nIdx;

  for( nIdx=0; nIdx<(uint32)nWhatSize; ++nIdx )
    HAL_tlmm_ConfigGpio(nWhichGpioSet[nIdx]);
}


/*=========================================================================

FUNCTION  
  void HAL_tlmm_SetPort(HAL_tlmm_PortType ePort, uint32 mask, uint32 value)

DESCRIPTION
   Selects the specified port if it is defined.

PARAMETERS
   ePort - The requested port to configure.
   mask  - The mask to use for this port.
   value - The value to be written to the port.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/

void HAL_tlmm_SetPort(HAL_tlmm_PortType nPortOffs, uint32 nMask, uint32 nValue)
{
  uint32 inValue = 0;

  inValue = in_dword(tHALTLMMInit.nBaseAddress + nPortOffs);
  inValue &= ~nMask;

  out_dword((tHALTLMMInit.nBaseAddress + nPortOffs), (inValue | (nMask & nValue)));


} /* END HAL_tlmm_SetPort */


/*=============================================================================*/

