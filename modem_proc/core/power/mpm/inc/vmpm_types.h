#ifndef VMPM_TYPES_H
#define VMPM_TYPES_H

/*==============================================================================
  FILE:         vmpm_types.h
  
  OVERVIEW:     This file provides the Abstract Data Types for
                the Virtual MPM (vMPM) driver API code

  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/inc/vmpm_types.h#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 

/*============================================================================
 *                     INCLUDE FILES FOR MODULE
 *==========================================================================*/
#include <stdint.h>
#include "comdef.h"

/*===========================================================================
 *                         MACRO DEFINITIONS
 *==========================================================================*/

/** 
 * @brief Initial value for Local IC Pin Number for
 *        unmapped interrupts 
 */ 
#define MPM_UNMAPPED_INT  (UINT32_MAX)

/*===========================================================================
 *                         CONST TYPE DECLARATIONS
 *==========================================================================*/
/**
 * @brief Max number of MPM2 interrupts
 */
extern const uint32 MPM_MAX_NUM_INTS;

/**
 * @brief Max value of valid Local IC Pin Number
 */
extern const uint32 MPM_MAX_LOC_IC_VAL;

/*===========================================================================
 *                         vMPM TYPE DEFINITIONS
 *==========================================================================*/
/**
 * mpm_status_type 
 *  
 * @brief MPM SUCCESS/FAIL status type 
 */
typedef enum
{
  MPM_SUCCESS = 0,
  MPM_FAIL =    1
} mpm_status_type;

/**
 * mpm_trigger_type 
 *  
 * @brief Enumeration of possible trigger types for a MPM 
 *        interrupt.
 *
 * @Note
 * Change in this enumeration may require change in
 * and vmpm_HAL_ConfigInfoType.
 *
 * trigger type bit fields
 *  -----------------------------| 
 *    polarity |    detect       |
 *  -----------|-----------------| 
 *      bit2   |   bit1   bit0   |
 *  -----------------------------| 
 * MPM_TRIGGER_LEVEL_LOW     = 0  = b000
 * MPM_TRIGGER_FALLING_EDGE  = 1  = b001
 * MPM_TRIGGER_RISING_EDGE   = 2  = b010
 * MPM_TRIGGER_DUAL_EDGE     = 3  = b011
 * MPM_TRIGGER_LEVEL_HIGH    = 4  = b100 
*/
typedef enum
{
  MPM_TRIGGER_LEVEL_LOW    = 0,
  MPM_TRIGGER_FALLING_EDGE = 1,
  MPM_TRIGGER_RISING_EDGE  = 2,
  MPM_TRIGGER_DUAL_EDGE    = 3,
  MPM_TRIGGER_LEVEL_HIGH   = 4,
}mpm_trigger_type;

/*
 * mpm_pin_type
 *
 * @brief Enumeration of MPM mapping table pin types.
 */
typedef enum
{
  MPM_IRQ  = 0,
  MPM_GPIO = 1,
  MPM_OPEN = 2,
  MPM_EOT = UINT32_MAX,
}mpm_pin_type;

/** 
 * mpm_int_pin_mapping_type 
 *  
 * @brief Mapping of MPM pin numbers to the Master's local 
 *        interrupt controller pins 
 */
typedef struct
{
  mpm_trigger_type  trig_type;
  uint32            local_pin_num;
  mpm_pin_type      mpm_pin_type;
}mpm_int_pin_mapping_type;

/**
 * mpm_pin_config_type
 *
 * @brief Abstract data type used by MPM API for pin type &
 *        trigger type configuration
 */
typedef struct
{
  mpm_pin_type     pin_type;
  mpm_trigger_type trig_config;
}mpm_pin_config_type;

/**
 * mpm_pin_query_info_type
 *
 * @brief Abstract data type used by MPM API for queries to 
 *        determine configuration of a MPM pin in MPM mapping
 *        table.
 */
typedef struct
{
  uint32           local_pin_num;
  mpm_pin_type     pin_type;
  mpm_trigger_type trig_config;
}mpm_pin_query_info_type;

/** 
 * mpm_config_info_type 
 *  
 * @brief Stores various information about an interrupt. This 
 *        structure is mainly.  This type is preserved for
 *        support of legacy vMPM driver functions.  Opportunity
 *        to refactor at later time if needed. Used when there
 *        is a need to configure/enable/disable more than one
 *        mpm mapped interrupts.
 *  
 * @Note
 * First two fields are supposed to be filled in with query/mapping
 * functions like vmpm_MapIrqsWithMaster (see below).
 *
 * For Trigger,
 * Refer to mpm_trigger_type enum.
 *
 * For Status,
 * 0  =  Disable
 * 1  =  Enable
 */
typedef struct
{
  uint32 mpm_hw_int_id;            /* Actual interrupt id at MPM hardware */
  uint32 master_hw_int_id;         /* Interrupt id at master hardware */
  uint8 trigger;                   /* Trigger for the interrupt at mpm */
  uint8 status;                    /* Interrupt status */
}mpm_config_info_type;

#endif  /* VMPM_TYPES_H */

