#ifndef HALSPM_H
#define HALSPM_H
/*==============================================================================
  FILE:         HALspm.h

  OVERVIEW:     This is the hardware abstraction layer interface for the
                SPM block.

  DEPENDENCIES: None
  
                Copyright (c) 2009-2014 Qualcomm Technologies, Inc. (QTI).
                 All Rights Reserved.
                 Qualcomm Technologies Confidential and Proprietary.
===============================================================================

$Header: //components/rel/core.mpss/3.5.c12.3/power/spm/src/hal/HALspm.h#1 $

=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <HALcomdef.h>
#include "CoreMutex.h"
#include "spm_common.h"

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/**
 * @brief Information about programming command sequence in SPM HW.
 */
typedef struct HAL_spm_ProgrammedSeqInfoType
{
  /* Mutex to ensure atomicity while programming a sequence */
  CoreMutex *lock;

  /* Maximum allowed number of commands in SPM HW */
  uint32 maxAllowedCmds;

  /* No. of commands that are programmed at any given point of time */
  uint32 usedCmdCount;

  /* Maximum number of allowed sequences */
  uint32 maxAllowedSeqs;

  /* Number of programmed sequences in seqList below */
  uint32 numProgrammedSeqs;

  /* List containing start addresses */
  uint32 *seqList;

}HAL_spm_ProgrammedSeqInfoType;

/* -----------------------------------------------------------------------
** Interface
** ----------------------------------------------------------------------- */

/**
 * HAL_spm_Init
 *
 * This is the init function for the HAL SPM layer.
 *
 * @param[in] coreNum  Which SPM.
 *
 * @param  None
 *
 * @return TRUE if successful; FALSE otherwise.
 */
bool32 HAL_spm_Init( uint32 coreNum, char **ppszVersion );

/**
 * HAL_spm_GetSawStatus
 *
 * This function returns the contents of the SAW_STS register.  The caller
 * must know how to interpret the data returned.
 *
 * @param coreNum     Which SPM.
 * @param *sawStatus  Output parameter - pointer to the location to put the
 *                    status in.
 *
 * @return None
 */
void HAL_spm_GetSawStatus
(
  uint32 coreNum,
  uint32 *sawStatus
);

/**
 * HAL_spm_Set_RPM_HandshakeMode
 *
 * This function indicates whether to perform the RPM handshaking, or
 * to bypass the RPM.
 *
 * @param coreNum              Which SPM.
 * @param rpmHandshakeMode[in] RPM handshake mode.
 *
 * @return None.
 */
void HAL_spm_Set_RPM_HandshakeMode
(
 uint32 coreNum,
 spm_rpm_bypass_type rpmHandshakeMode
);

/**
 * HAL_spm_SetLowPowerMode
 *
 * This function indicates which low power mode is desired on the
 * next SW DONE signal.
 *
 * @param coreNum  Which SPM.
 * @param mode  Indicates which low power mode to set.
 *              These should align with BSP_spm_LpmType.
 *
 * @return None
 *
 * @see
 */
void HAL_spm_SetLowPowerMode
(
  uint32 coreNum,
  spm_low_power_mode_type mode
);

/**
 * HAL_spm_ConfigVddWakeLevel
 *
 * This function is used to program the VDD value to use when the system
 * is awake (normal operation).  This value may change at run time as the
 * system active voltage changes.
 *
 * @param wake  VDD value to use when awake.
 *
 * @return None
 *
 * @see
 */
void HAL_spm_ConfigVddWakeLevel
(
  uint8 wake
);

/**
 * Enable/Disable the SPM HW.
 *
 * @param coreNum   : Which SPM.
 * @param spmEnable : TRUE to enable, FALSE to disable.
 */
void HAL_spm_Enable( uint32 coreNum, bool32 spmEnable );

/**
 * Updates the contents of the SAW2 SPM register that contains the
 * PMIC data for active voltage level. This function retrieves the
 * current active voltage level from another SAW2 SPM hardware
 * register.
 *
 * @param coreNum[in]   - Which SPM.
 *
 * @return None.
 *
 * @dependencies None.
 */
void HAL_spm_UpdateActiveVoltageLevel
(
  uint32 coreNum
);

/**
 * @brief Programs/copies a *new* command sequence to SPM HW.
 * 
 * @param coreNum        : Which SPM.
 * @param seq            : Command sequence to program
 * @param seqLength      : Length of above command sequence.
 * @param start_addr[out]: Offset in SPM sequence register bank where this
 *                         sequence is programmed - valid only when SPM_SUCCESS
 *                         is returned.
 *
 * @return SPM_SUCCESS if sequence was programmed successfully else error code.
 */
spm_result_t HAL_spm_ProgramCmdSeq
( 
  uint32 coreNum, 
  spm_cmd_t *seq, 
  uint32 seqLength,
  uint32 *startAddr
);

/**
 * @brief Updates the *existing* command sequence in SPM HW.
 * 
 * @param coreNum   : Which SPM.
 * @param seq       : Command sequence to program
 * @param seqLength : Length of above command sequence.
 * @param start_addr: Value obtained from successful call to 
 *                    HAL_spm_ProgramCmdSeq.
 *
 * @return SPM_SUCCESS if sequence was updated successfully else error code.
 */
spm_result_t HAL_spm_UpdateCmdSeq
( 
  uint32 coreNum, 
  spm_cmd_t *seq, 
  uint32 seqLen, 
  uint32 startAddr 
);

/**
 * @brief Updates a single command in existing command sequence at SPM HW.
 *
 * @param coreNum    : Which core's SPM (0, 1, 2,...)
 * @param startAddr  : Value obtained from successful call to 
 *                      spm_program_cmd_seq
 * @param cmdOffset  : Offset of the command in the sequence which will 
 *                      be updated (must be less than sequence length)
 * @param updatedCmd : Updated command which will be copied to SPM HW.
 *
 * @return SPM_SUCCESS if command was updated correctly else error code
 */
spm_result_t HAL_spm_UpdateCmdInSeq
(
  uint32 coreNum,
  uint32 startAddr,
  uint32 cmdOffset,
  spm_cmd_t updatedCmd
);

/*============================================================================
   INTERFACES DOCUMENTATION
=============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* HALSPM_H */
