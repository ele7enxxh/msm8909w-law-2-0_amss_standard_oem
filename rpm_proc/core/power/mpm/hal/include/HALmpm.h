#ifndef HALMPM_H
#define HALMPM_H
/*
===========================================================================

FILE:         HALmpm.h

DESCRIPTION:  
  This is the hardware abstraction layer interface for the MPM block.

===========================================================================

                             Edit History

$Header: 

when       who     what, where, why
--------   ---     --------------------------------------------------------
04/09/12   mda     MPMv2 for 8974

===========================================================================
             Copyright © 2008-2009 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>
#include "HALcomdef.h"

/* -----------------------------------------------------------------------
** Constants
** ----------------------------------------------------------------------- */

#define HAL_MPM_REGFILE_SIZE 9


/* -----------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */

#define CEILING_DIV(a, b)  (((a)+((b)-1))/(b))

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

typedef enum
{
  HAL_MPM_VOLTAGE_RAIL_VDD_DIG = 0,
  HAL_MPM_VOLTAGE_RAIL_VDD_MEM = 1,

  /* internal definitions */
  HAL_MPM_VOLTAGE_RAIL_COUNT,
  HAL_MPM_VOLTAGE_RAIL_TYPE_SIZE = 0x7fffffff /* force enum to 32 bits */
} HAL_mpm_VoltageRailType;

typedef enum
{
  HAL_MPM_SLEEP_MODE_VDD_LOW = 0,
  HAL_MPM_SLEEP_MODE_VDD_MIN = 1,
  HAL_MPM_SLEEP_MODE_DEBUG = 2,
  HAL_MPM_SLEEP_MODE_COUNT,
  HAL_MPM_SLEEP_MODE_TYPE_SIZE = 0x7fffffff /* force enum to 32 bits */
} HAL_mpm_SleepModeType;


typedef enum
{
  HAL_MPM_PEN_DEBOUNCE_DELAY_0400US = 0x0,
  HAL_MPM_PEN_DEBOUNCE_DELAY_0800US = 0x1,
  HAL_MPM_PEN_DEBOUNCE_DELAY_1200US = 0x2,
  HAL_MPM_PEN_DEBOUNCE_DELAY_1600US = 0x3,
  HAL_MPM_PEN_DEBOUNCE_DELAY_2000US = 0x4,
  HAL_MPM_PEN_DEBOUNCE_DELAY_3000US = 0x5,
  HAL_MPM_PEN_DEBOUNCE_DELAY_4000US = 0x6,
  HAL_MPM_PEN_DEBOUNCE_DELAY_6000US = 0x7
} HAL_mpm_PenDebounceDelayType;


/* -----------------------------------------------------------------------
** Interface
** ----------------------------------------------------------------------- */

// FIXME not tested
void   HAL_mpm_Init                   ( char **ppszVersion );
void   HAL_mpm_Init_Xtal_Latency           ( void );
void   HAL_mpm_Reset                  ( void );
void   HAL_mpm_Save                   ( void );
void   HAL_mpm_Restore                ( void );
                                      
void   HAL_mpm_GetCurrentSclkCount    ( uint64_t *pSclkCount );
void   HAL_mpm_SetWakeUpTime          ( uint64_t sclkDuration );
                                      
void   HAL_mpm_HwRestored             ( void );
bool32 HAL_mpm_IsHwRestored           ( void );

void   HAL_mpm_PenDebounceCtl         ( bool32 enable );
void   HAL_mpm_ConfigPenDebounceDelay ( HAL_mpm_PenDebounceDelayType delay );
       
void   HAL_mpm_CfgJTAGEfuseState      ( uint32_t  value );
void   HAL_mpm_GetJTAGEfuseState      ( uint32_t  *valuePtr );
void   HAL_mpm_DebugThroughPC         ( bool32 enable );

void   HAL_mpm_CfgEbi1SwCtl           (bool enable);
                                      
void HAL_mpm_SetCXOWarmup              ( uint16_t warmupSclks );

void HAL_mpm_CfgVoltageRailSleep(uint32_t offMxMicrovolts, uint32_t onMxMicrovolts, 
                                 uint32_t offCxMicrovolts, uint32_t onCxMicrovolts);

void HAL_mpm_CfgCXOSleep              ( void );
void HAL_mpm_CfgFreezeIOs              ( bool enable );
void HAL_mpm_ClearSleepModes          ( void );

void HAL_mpm_EnableCXO                 ( void );

void HAL_mpm_SWDone                   (  uint64_t sleep_duration );

void HAL_mpm_CfgVoltageCommands (uint32_t offMxMicrovolts, uint32_t onMxMicrovolts,
                                 uint32_t offCxMicrovolts, uint32_t onCxMicrovolts);
										 
void HAL_mpm_GetWakeupInts      (uint32 mask_index, uint32 *status);
uint32 HAL_mpm_GetDeepSleepEnterLatency(void);
uint32 HAL_mpm_GetDeepSleepExitLatency(void);
uint16 HAL_mpm_GetDelayFromMicrovolts(uint32 offMicrovolts, uint32 onMicrovolts);
uint32_t HAL_mpm_GetTCXOLatency(void);

/*
===============================================================================
  INTERFACES DOCUMENTATION
===============================================================================*/
/*==============================================================================
Function: HAL_mpm_Init()
   This function initializes the MPM hardware. This API has to be call once at power up.

Parameters: 
   ppszVersion: MPM HAL version

Return value: None

Dependencies: None

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_Reset()
   MPM hardware could be reset through Clock regime hardware, hence would be 
   supported by Clock regime HAL. Refer to the clock regime HAL APIs for such 
   support.

Parameters: None

Return value: None

Dependencies: None

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_Save()
   Currently not supported

Parameters: 

Return value: None

Dependencies: None

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_Restore()
   Currently not supported

Parameters: 

Return value: None

Dependencies: None

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_CfgWakeUpDelays()
   This procedure configures MPM hardware with TCXO warmup and voltage rampup
   delay durations.

Parameters:
   warmupSclks: TCXO warmup delay in sleep clock cycles
   rampupSclks: Voltage rampup delay in sleep clock cycles
   
Return value: None

Dependencies: None

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_GetCurrentSclkCount()
   This procedure returns the current value of free running sleep clock counter.

Parameters:

Return value:
   pSclkCount: pointer to current sclk counter

Dependencies: None

Side effects: None

See Also: HAL_mpm_SetWakeUpTime
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_SetWakeUpTime()
   This procedure sets the duration after which MPM wake up interrupt is generated.

Parameters:
   sclkDuration: Duration in sleep clock cycles after which wakeup interrupt
                 is generated

Return value: None

Dependencies: MPM interrupt registers should be configured to trigger wakeup
              interrupt

Side effects: None

See Also: HAL_mpmint_Init, HAL_mpmint_Enable
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_CfgMpmSlpProc()
   This procedure programs MPM hardware to invoke TCXO shutdown, Vdd min or 
   power collapse procedure during awake-to-sleep and sleep-to-awake
   transitions.

Parameters:
   mpmSleepCfg: MPM Sleep configuration type

   typedef enum 
   {
	   HAL_MPM_PC_CFG_TCXO_SHUTDOWN,
	   HAL_MPM_PC_CFG_VDD_MINIMIZATION,
	   HAL_MPM_PC_CFG_POWER_COLLAPSE,
	   HAL_MPM_PC_CFG_LIMITED,
	   HAL_MPM_PC_CFG_DISABLE
   } HAL_mpm_MpmSleepCfgType;

Return value: None

Dependencies: None

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_HwRestored()
   This procedure indicates the MPM hardware that hardware restoration is complete.
   This would result in un-freeze and un-clamp of any IO signals that are in
   freeze and clamp states.

Parameters:

Return value: None

Dependencies: 

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_IsHwRestored()
   This procedure checks if hardware is restored.

Parameters: None

Return value: 
   TRUE: Hardware is restored
   FALSE: Hardware not restored

Dependencies: 

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_PenDebounceCtl()
   This procedure enables touch-screen driver configure debounce control
   settings which is part of MPM (always-on) hardware.

Parameters:
   enable:
     TRUE:  Enable Pen Debounce Control
	 FALSE: Disable Pen Debounce Control

Return value: None

Dependencies: HAL_mpm_ConfigPenDebounceDelay() should be called to configure
              debounce delay when enabled.

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_ConfigPenDebounceDelay()
   This procedure confugures the pen debounce delay.

Parameters:
   delay: delay (us)

   typedef enum
   {
	   HAL_MPM_PEN_DEBOUNCE_DELAY_0400US = 0x0,
	   HAL_MPM_PEN_DEBOUNCE_DELAY_0800US = 0x1,
	   HAL_MPM_PEN_DEBOUNCE_DELAY_1200US = 0x2,
	   HAL_MPM_PEN_DEBOUNCE_DELAY_1600US = 0x3,
	   HAL_MPM_PEN_DEBOUNCE_DELAY_2000US = 0x4,
	   HAL_MPM_PEN_DEBOUNCE_DELAY_3000US = 0x5,
	   HAL_MPM_PEN_DEBOUNCE_DELAY_4000US = 0x6,
	   HAL_MPM_PEN_DEBOUNCE_DELAY_6000US = 0x7
   } HAL_mpm_PenDebounceDelayType;
   
Return value: 

Dependencies:

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_CfgJTAGEfuseState()
   This procedure is used to save efuse shadow register state after power up. 
   This should be called only once after power up.

Parameters: value: Efuse state value to be saved.

Return value: 
   
Dependencies:

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_GetJTAGEfuseState()
   This procedure returns the saved Efuse state value.

Parameters:

Return value: 

Dependencies:

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_CfgPmicSbiPort()
   This procedure configures the SBI/SSBI port for PMIC

Parameters:
   pmicPort:   Port to used for PMIC:

               typedef enum
               {
                 HAL_MPM_PMIC_PORT_SSBI,
                 HAL_MPM_PMIC_PORT_SBI0,
                 HAL_MPM_PMIC_PORT_SBI1,
                 HAL_MPM_PMIC_PORT_SBI2
               } HAL_mpm_PmicPortType;

Return value: None

Dependencies: HAL_mpm_CfgPmicSbiAddr() should be called to complete PMIC i/f
              configuration

Side effects: None

See Also: HAL_mpm_CfgPmicSbiAddr()
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_CfgPmicSsbiPort()
   This function is used to configure the SSBI version and enabled port for the
   PMIC.  When this function is called, the SSBI_MODE is automatically enabled 
   in the MPM_PMIC_SBI_SEL register.  This function is empty for targets
   that do not support configuring for SSBI2.0.

Parameters:
   ssbiVersion: Choose SSBI 1.0 (default) or SSBI 2.0 according to this enum:

                typedef enum
                {
                  HAL_MPM_SSBI_1_0,
                  HAL_MPM_SSBI_2_0
                } HAL_mpm_SsbiVersionType;

   ssbiPortNum: SSBI port number to use when MPM drives SSBI.  When MPM does
                not drive SSBI, MPM lets the SSBI from the core pass through
                regardless of this setting.  Port number can be anywhere
                from 0 to HAL_MPM_MAX_SSBI_PORT_NUM.

Return value: None

Dependencies: HAL_mpm_CfgPmicSbiAddr() should be called to complete PMIC i/f
              configuration

Side effects: SSBI_MODE is automatically enabled in the MPM_PMIC_SBI_SEL 
              register.

See Also: HAL_mpm_CfgPmicSbiAddr()
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_CfgPmicOnOffVal()
   This procedure configures the PMIC On and Off Values used during Vdd min 
   and/or Power Collapse procedures (depending on the target).

Parameters:
   dataOn:  PMIC raw value corresponding to the voltage to use when coming out
            of VDD min and/or power collapse.
   dataOff: PMIC raw value corresponding to the voltage to use when going into
            VDD min and/or power collapse.

Return value: None

Dependencies: SBI port and Addr should be configured using 
              HAL_mpm_CfgPmicSbiAddr() HAL_mpm_CfgPmicSbiPort() APIs

Side effects: None

See Also: HAL_mpm_CfgPmicSbiAddr(), HAL_mpm_CfgPmicSbiPort()
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_CfgPmicOnOffVal_PC()
   This procedure configures the PMIC On and Off Values used during power
   collapse.  This function is empty on targets that don't have a different
   bit field for power collapse vs. VDD min.

Parameters:
   dataOn:  PMIC raw value corresponding to the voltage to use when coming out
            of power collapse.
   dataOff: PMIC raw value corresponding to the voltage to use when going into
            power collapse.

Return value: None

Dependencies: SBI port and Addr should be configured using 
              HAL_mpm_CfgPmicSbiAddr() HAL_mpm_CfgPmicSbiPort() APIs

Side effects: None

See Also: HAL_mpm_CfgPmicSbiAddr(), HAL_mpm_CfgPmicSbiPort()
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_CfgPmicOnVal()
   This procedure is used to configure the voltage that is used during active
   modes when coming out of VDD minimization and/or power collapse (depending
   on the target).

Parameters:
   dataOn:  PMIC raw value corresponding to the voltage to use when coming out
            of VDD min and/or power collapse.

Return value: None 

Dependencies: SBI port and Addr should be configured using 
              HAL_mpm_CfgPmicSbiAddr() HAL_mpm_CfgPmicSbiPort() APIs

Side effects: None

See Also: HAL_mpm_CfgPmicSbiAddr(), HAL_mpm_CfgPmicSbiPort()
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_CfgPmicOnVal_PC()
   This procedure is used to configure the voltage that is used during active
   modes when coming out of power collapse  This function is empty on targets 
   that don't have a different bit field for power collapse vs. VDD min.

Parameters:
   dataOn:  PMIC raw value corresponding to the voltage to use when coming out
            of power collapse.

Return value: None 

Dependencies: SBI port and Addr should be configured using 
              HAL_mpm_CfgPmicSbiAddr() HAL_mpm_CfgPmicSbiPort() APIs

Side effects: None

See Also: HAL_mpm_CfgPmicSbiAddr(), HAL_mpm_CfgPmicSbiPort()
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_AccessRegFile()
   This procedure provides access to MPM register file used to save EBI1/SMI 
   memory configuration.

Parameters:
   access: Read or Write access

           typedef enum
           {
        	   HAL_MPM_REGFILE_READ,
        	   HAL_MPM_REGFILE_WRITE
           } HAL_mpm_RegFileAccessType;

   regFilePtr: Pointer to Access Register File to/from which data has to be 
               read/written.

Return value: None

Dependencies: None

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_AccessRegFileIndex()
   This procedure provides access to MPM register file used to save EBI1/SMI 
   memory configuration.  This provides access to a specific index of the 
   register file.

Parameters:
   access: Read or Write access

           typedef enum
           {
	           HAL_MPM_REGFILE_READ,
	           HAL_MPM_REGFILE_WRITE
           } HAL_mpm_RegFileAccessType;

   regFilePtr: Pointer to Access Register File to/from which data has to be 
               read/written.

   index: Index in the reg file to read or write.

Return value: None

Dependencies: None

Side effects: None

See Also:
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* HAL_MPM_H */

