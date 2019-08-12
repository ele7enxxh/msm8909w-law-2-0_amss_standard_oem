#ifndef __HALAVS_H__
#define __HALAVS_H__

/*===============================================================================

                A D A P T I V E   V O L T A G E   S C A L I N G
              H A R D W A R E   A B S T R A C T I O N   L A Y E R

FILE:         HALavs.h

DESCRIPTION:  
              This file is the public (to associated driver only) API header
              file for use of the HAL AVS system.

=================================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/avs/inc/HALavs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------- 
03/31/11   dcf     Ported to 8x60. 
05/12/10   dcf     Created 

=================================================================================
                 Copyright (c) 2010 - 2011 Qualcomm Technologies Incorporated.
                              All Rights Reserved.
                           QUALCOMM Proprietary/GTDR
=================================================================================
*/


/*===============================================================================
                        INCLUDES
  =============================================================================*/


#include "HALcomdef.h"



/*===============================================================================
                        STRUCTURE DEFINITIONS
  =============================================================================*/

/*
 * HAL AVS Core type enumerations.  These enumerations are used for multicore 
 * systems as indicators to what core the code is running on in order to ensure 
 * the correct AVS hardware is configured.  These enumerations are ignored on 
 * non-multicore systems. 
*/
typedef enum
{
  HAL_AVS_CORE_SELF,   /* Current executing core. */
  HAL_AVS_CORE_APC0,   /* Core 0 on Applications Processor Subsystem */
  HAL_AVS_CORE_APC1,   /* Core 1 on Applications Processor Subsystem */
  HAL_AVS_CORE_QDSPSW, /* QDSP Software */
  HAL_AVS_CORE_QDSPFW, /* QDSP Firmware */
  HAL_AVS_NUM_CORES,

  HAL_ENUM_32BITS(HALAVSCORE)

}HAL_avs_CoreType;


/*
 * HAL AVS Step Size enumerations.  This enumeration is used for to specify 
 * the step size to use when stepping up or down the voltage in HW. 
*/
typedef enum
{
  HAL_AVS_STEP_SIZE_25MV, /* Step size of 25 mV */

  HAL_AVS_NUM_STEP_SIZE
}HAL_avs_StepSizeType;


/*
 * HAL AVS interrupt request type.  These types are used to program
 * the AVS hardware to generate various interrupts used by the AVS system when
 * implemented in SW.
 */
typedef enum
{

  HAL_AVS_INT_REQUEST_SW_DONE = 0,        /* HW ACK of SW write adjust Vdd */
  HAL_AVS_INT_REQUEST_MAX_LVL_EXCEEDED,   /* Vdd request above maximum level */
  HAL_AVS_INT_REQUEST_MIN_LVL_EXCEEDED,   /* Vdd request below minimum level */

}HAL_avs_IntRequestType;


/*
 * HAL AVS enumeration of supported PMIC types.  Each AVS enabled core will use 
 * one of the below supported PMIC types.  This value should be set as part of 
 * the initialization parameters for each core. 
 */
typedef enum
{
  HAL_AVS_PM8058_SUPPORT,         /* Used to specify core support for PM8058. */
  HAL_AVS_PM8901_SUPPORT,         /* Used to specify core support for PM8901. */
  HAL_AVS_PM8921_SUPPORT,         /* Used to specify core support for PM8921. */
  HAL_AVS_PM8018_SUPPORT,         /* Used to specify core support for PM8018. */

  HAL_AVS_NUM_PM_SUPPORT,

  HAL_ENUM_32BITS(HALAVSPMIC)

}HAL_avs_PmicType;


/*
 * HAL_avs_CoreDataType
 *
 * HAL AVS Core data type structure which describes how a core should
 * be initialized in the HAL.
 *
 *  ePMIC    -  The supported PMIC for the required core.
 *             
 *  nBaseAddr - The base address used by this core to acces HW.
 */
typedef struct
{
  HAL_avs_PmicType ePMIC;
  uint32           nBaseAddr;

}HAL_avs_CoreDataType;



/*
 * HAL_avs_Context
 *
 * HAL AVS driver context data.
 *
 *  BusyWait -  Pointer to busy wait function.
 *              Assumption: Will be implemented by the top level driver
 *              whether in DAL or via clk_busy_wait. If this function
 *              pointer is NULL then the HAL AVS will not wait between
 *              status register reads after programmming a voltage.
 *             
 *  pCoreData - A pointer to an array of Core data structures to use to
 *              initialize the core.
 */
typedef struct
{
  void (*BusyWait) ( uint32 uSeconds );
  HAL_avs_CoreDataType *pCoreData;

} HAL_avs_ContextType;



/*===============================================================================
                        PUBLIC API
  =============================================================================*/


void HAL_avs_Init                          ( HAL_avs_ContextType* pCtxt );
void HAL_avs_Reset                         ( void );
void HAL_avs_Save                          ( void );
void HAL_avs_Restore                       ( void );
boolean HAL_avs_SetCurrentVoltage          ( HAL_avs_CoreType eCore, uint32 nVdd );
void HAL_avs_SetMaxVoltage                 ( HAL_avs_CoreType eCore, uint32 nMaxVdd );
void HAL_avs_SetMinVoltage                 ( HAL_avs_CoreType eCore, uint32 nMinVdd );
void HAL_avs_SetDelay                      ( HAL_avs_CoreType eCore, uint32 nDelay );
void HAL_avs_SetClockDivider               ( HAL_avs_CoreType eCore, uint32 nDivider );
void HAL_avs_SetVoltageStepSize            ( HAL_avs_CoreType eCore, HAL_avs_StepSizeType nStep );
void HAL_avs_EnableInterrupt               ( HAL_avs_CoreType eCore, HAL_avs_IntRequestType 
                                             eRequest, boolean bEnable );
void HAL_avs_SetVoltageWidth               ( HAL_avs_CoreType eCore, HAL_avs_PmicType ePMIC );
void HAL_avs_Enable                        ( HAL_avs_CoreType eCore, boolean bEnable );
boolean HAL_avs_IsCoreSupported            ( HAL_avs_CoreType eCore );

/*
=================================================================================
  INTERFACES DOCUMENTATION
=================================================================================

FUNCTION  
  void HAL_avs_Init ( char ** ppszVersion )

DESCRIPTION
   Does any initialization of the AVS hardware required to use the AVS system. This
   function must be called before using any other HAL AVS APIs.

PARAMETERS
  pCtxt       - Pointer to a HAL_avs_ContextType data structure.  This field is
                reserved for future use.  

DEPENDENCIES
  Driver must first populate the HAL AVS Context structure by calling
  HAL_avs_GetInitContext() and populating its fields.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=================================================================================

FUNCTION  
  void HAL_avs_Reset( void )

DESCRIPTION
   Required API to handle HW reset.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=================================================================================

FUNCTION  
  void HAL_avs_Save ( void )

DESCRIPTION
   Required API to save HW states for the AVS system if applicable.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=================================================================================

FUNCTION  
  void HAL_avs_Restore ( void )

DESCRIPTION
   Required API to restore a HW state for the AVS system if applicable after a 
   save has been performed (if applicable).

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=================================================================================

FUNCTION  
  boolean HAL_avs_SetCurrentVoltage ( HAL_avs_CoreType eCore, uint32 nVoltage )

DESCRIPTION
  Sets the hardware digital voltage to value specified in the parameter.

PARAMETERS 
  eCore    - The current core.  For single cores, this enumeration is ignored. 
  nVoltage - The current voltage value to be set to HW.  This value must be
             in PMIC_DATA format, which conversion should be obtained via PMIC 
             API.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Voltage was successfully set.
  FALSE - Voltage was not successfully set.

SIDE EFFECTS
  None.

=================================================================================

FUNCTION  
  void HAL_avs_SetMaxVoltage ( HAL_avs_CoreType eCore, uint32 nVoltage )

DESCRIPTION
  Sets the upper voltage limit.  AVS will not request a voltage higher than
  the value specified by the voltage parameter.  HW interrupt may be enable
  using the HAL_avs_EnableInterrupt() API to request an interrupt generation
  if HW determines the voltage needs to be raised above this level.

PARAMETERS 
  eCore    - The current core.  For single cores, this enumeration is ignored. 
  nVoltage - The voltage in PMIC format to use to set the high end of the
             allowable voltage range.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=================================================================================

FUNCTION  
  void HAL_avs_SetMinVoltage ( HAL_avs_CoreType eCore, uint32 nVoltage )

DESCRIPTION
  Sets the lower voltage limit.  AVS will not request a voltage lower than
  the value specified by the voltage parameter.  HW interrupt may be enable
  using the HAL_avs_EnableInterrupt() API to request an interrupt generation
  if HW determines the voltage may be lowered beyond this point.

PARAMETERS 
  eCore    - The current core.  For single cores, this enumeration is ignored. 
  nVoltage - The voltage in PMIC format to use to set the low end of the 
             allowable voltage range.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=================================================================================

FUNCTION  
  void HAL_avs_SetDelay( HAL_avs_CoreType eCore, uint32 nDelay )

DESCRIPTION
  Sets the delay between hardware voltage change requests.  Increments are
  defined by the values used in HAL_avs_SetCockDivider() API.

  Example (From Scorpion Power Management):  Setting this value to 0x3FF with 
  clock divider value of 0x14, AVS will delay for 0x3FF*(0x14*reference-clock-
  period).  So, for TCXO at 19.2 MHz, delay is (1.042 us)*0x3FF = 1.066 ms.

PARAMETERS 
  eCore  - The current core.  For single cores, this enumeration is ignored. 
  nDelay - The amount of delay to use between requests.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=================================================================================

FUNCTION  
  void HAL_avs_SetClockDivider( HAL_avs_CoreType eCore, uint32 nDivider )

DESCRIPTION
  Sets the SAW timer ticks to the value passed in.

PARAMETERS 
  eCore    - The current core.  For single cores, this enumeration is ignored. 
  nDivider - The supported clock divider to use with the AVS system.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=================================================================================

FUNCTION  
  void HAL_avs_SetVoltageStepSize( HAL_avs_CoreType eCore, HAL_avs_StepSizeType nStep )

DESCRIPTION
  Sets the step to adjust the voltage in.  Voltage will be increased or decreased
  in steps of the size passed in.

DEPENDENCIES
  None.

PARAMETERS 
  eCore - The current core.  For single cores, this enumeration is ignored. 
  nStep - The supported step size to use when stepping up or down the voltage.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=================================================================================

FUNCTION  
  void HAL_avs_EnableInterrupt( HAL_avs_CoreType eCore,
                                HAL_avs_IntRequestType eRequest,
                                boolean bEnable )

DESCRIPTION
  API to enable/disable various AVS-related HW interrupts.

PARAMETERS 
  eCore    - The current core.  For single cores, this enumeration is ignored. 
  eRequest - The type of AVS HW interrupt to select.
  bEnable  - TRUE: Enable the interrupt, FALSE: Disable the interrupt.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=================================================================================

FUNCTION  
  void void HAL_avs_SetVoltageWidth( HAL_avs_CoreType eCore, uint32 nWidth )

DESCRIPTION
  API to select the current voltage level width (PMIC VREG) used by AVS.

PARAMETERS 
  eCore  - The current core.  For single cores, this enumeration is ignored. 
  nWidth - The value required to select the associate voltage width.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=================================================================================

FUNCTION  
  void HAL_avs_Enable( HAL_avs_CoreType eCore, boolean bEnable )

DESCRIPTION
  API to allow/disallow HW to control voltage level requests.

PARAMETERS 
  eCore   - The current core.  For single cores, this enumeration is ignored.
  bEnable - TRUE:  Enable AVS
            FALSE: Disable AVS

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
 
=================================================================================

FUNCTION  
  boolean HAL_avs_IsCoreSupported( HAL_avs_CoreType eCore )

DESCRIPTION
  Determines whether a particular core or subsystem is supported or not on the
  current image.

PARAMETERS 
  eCore   - The current core.  For single core systems, HAL_AVS_CORE_SELF should
            be used.
  bEnable - TRUE:  The current image supports the core/subsystem.
            FALSE: The current image does not support the core/subsystem.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
 
===============================================================================*/

#endif /* __HALAVS_H__ */

