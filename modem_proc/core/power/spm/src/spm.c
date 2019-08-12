/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

               S U B S Y S T E M   P O W E R   M A N A G E R

GENERAL DESCRIPTION

  This module contains routines for the subsystem power manager (SPM).

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

             Copyright (c) 2009-2012 by Qualcomm Technologies, Inc. All Rights Reserved.

============================================================================*/

/*============================================================================
                           INCLUDE FILES FOR MODULE
============================================================================*/

#include "spm_common.h"
#include "spm.h"
#include "HALspm.h"
#include "CoreVerify.h"

/*=============================================================================
 *                     GLOBAL FUNCTION DEFINITIONS
 *===========================================================================*/

/**
 * spm_set_low_power_mode
 */
void spm_set_low_power_mode
( 
  uint32 core_num,
  spm_low_power_mode_type low_power_mode
)
{
  CORE_VERIFY(core_num < SPM_NUM_CORES);
  CORE_VERIFY( low_power_mode < SPM_NUM_LOW_POWER_MODES );
  HAL_spm_SetLowPowerMode( core_num, low_power_mode );
}

/**
 * spm_set_rpm_bypass
 */
void spm_set_rpm_bypass
( 
 uint32 core_num,
 spm_rpm_bypass_type rpm_bypass
)
{
  CORE_VERIFY(core_num < SPM_NUM_CORES);
  HAL_spm_Set_RPM_HandshakeMode( core_num, rpm_bypass );
} 

/**
 * spm_enable
 */
void spm_enable( uint32 core_num, boolean spm_enable )
{
  CORE_VERIFY(core_num < SPM_NUM_CORES);
  HAL_spm_Enable( core_num, spm_enable );
}

/**
 * spm_initialize
 */
void spm_initialize( void )
{
  char **ppszVersion    = NULL;
  
  /* Initialize the HAL layer */
  CORE_VERIFY( TRUE == HAL_spm_Init( SPM_CURR_CORE, ppszVersion ) );
} 

/**
 * spm_program_cmd_seq
 */
spm_result_t spm_program_cmd_seq
(
  uint32 core_num, 
  spm_cmd_t *seq, 
  uint32 seq_length,
  uint32 *start_addr
)
{
  CORE_VERIFY( core_num < SPM_NUM_CORES );

  return ( HAL_spm_ProgramCmdSeq( core_num, seq, seq_length, start_addr ) );
}

/**
 * spm_update_cmd_seq
 */
spm_result_t spm_update_cmd_seq
( 
  uint32 core_num, 
  spm_cmd_t *seq,
  uint32 seq_length,
  uint32 start_addr
)
{
  CORE_VERIFY( core_num < SPM_NUM_CORES );

  return ( HAL_spm_UpdateCmdSeq( core_num, seq, seq_length, start_addr ) );
}

/**
 * spm_update_cmd_in_seq
 */
spm_result_t spm_update_cmd_in_seq
(
  uint32 core_num,
  uint32 start_addr,
  uint32 cmd_offset,
  spm_cmd_t updated_cmd
)
{
  CORE_VERIFY( core_num < SPM_NUM_CORES );

  return (
    HAL_spm_UpdateCmdInSeq( core_num, start_addr, cmd_offset, updated_cmd )
  );
}
