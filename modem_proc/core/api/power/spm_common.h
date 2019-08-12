#ifndef SPM_COMMON_H
#define SPM_COMMON_H

/*==============================================================================
  FILE:         spm_common.h

  OVERVIEW:     This file contains public API for SPM functionalities that can
                be used by multiple drivers.

  DEPENDENCIES: SPM driver must be initialized before calling any APIs.

                Copyright (c) 2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary

  $Header: //components/rel/core.mpss/3.5.c12.3/api/power/spm_common.h#1 $
================================================================================*/

#include "DALStdDef.h"

/*==============================================================================
                           GLOBAL TYPE DEFINITIONS
 =============================================================================*/
/**
 * @brief SPM cmd type that will be written to actual HW register.
 */
typedef uint8 spm_cmd_t;

/**
 * @brief Enumeration for return type for public SPM APIs.
 */
typedef enum
{
  SPM_SUCCESS = 0,                /* SPM API call was successful */
  SPM_INVALID_PARAM,              /* Parameter(s) to API was invalid */
  SPM_NO_SEQ_MEMORY,              /* Not enough memory for SPM sequence */
  SPM_ERROR = 0xFFFFFFFF          /* Generic failure */
}spm_result_t;

/*==============================================================================
                        GLOBAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * @brief Programs input command sequence to SPM HW.
 *
 * @param core_num        : Which core's SPM (0, 1, 2,...)
 * @param seq             : Array of spm commands.
 * @param seq_length      : Length of above array.
 * @param start_addr[out] : Offset in SPM sequence register bank where this
 *                          sequence is programmed - valid only when SPM_SUCCESS
 *                          is returned.
 *
 * @return SPM_SUCCESS if sequence was programmed successfully else error code
 *         - see spm_result_t.
 */
spm_result_t spm_program_cmd_seq
( 
  uint32 core_num, 
  spm_cmd_t *seq, 
  uint32 seq_length,
  uint32* start_addr 
);

/**
 * @brief Updates the existing command sequence in SPM HW.
 *
 * This function should be used when client needs to update its command
 * sequence in SPM depending on run time scenario. This function provides
 * a mechanism to avoid having multiple sequences which are similar
 * in SPM HW. It is useful especially on SPM instances with limited command
 * registers.
 *
 * @Dependency Requires start address from a successful call to 
 *             spm_program_cmd_seq as one of the input arguments.
 *
 * @Note As of now this function does not support subsequent command sequences
 *       to be longer than the one programmed for the first time.
 * 
 * @param core_num   : Which core's SPM (0, 1, 2,...)
 * @param seq        : Updated/New array of spm commands.
 * @param seq_length : Length of above array.
 * @param start_addr : Value obtained from successful call to 
 *                     spm_program_cmd_seq
 *
 * @return SPM_SUCCESS if sequence was updated successfully else error
 *         code - see spm_result_t.
 */
spm_result_t spm_update_cmd_seq
( 
  uint32 core_num, 
  spm_cmd_t *seq, 
  uint32 seq_length, 
  uint32 start_addr 
);

/**
 * @brief Updates a single command in already programmed command sequence.
 *
 * This function is useful if client needs to change only a few commands
 * instead of almost entire sequence at run time.
 *
 * @Dependency Requires start address from a successful call to 
 *             spm_program_cmd_seq as one of the input arguments.
 *
 * @param core_num    : Which core's SPM (0, 1, 2,...)
 * @param start_addr  : Value obtained from successful call to 
 *                      spm_program_cmd_seq
 * @param cmd_offset  : Offset of the command in the sequence which will 
 *                      be updated (must be less than sequence length)
 * @param updated_cmd : Updated command which will be copied to SPM HW.
 *
 * @return SPM_SUCCESS if command was updated correctly else error code
 *         - see spm_result_t
 */
spm_result_t spm_update_cmd_in_seq
(
  uint32 core_num,
  uint32 start_addr,
  uint32 cmd_offset,
  spm_cmd_t updated_cmd
);

#endif
