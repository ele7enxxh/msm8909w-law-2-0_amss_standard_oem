#ifndef SLEEP_TARGET_H
#define SLEEP_TARGET_H
/*==============================================================================
  FILE:         sleep_target.h
  
  OVERVIEW:     This file provides the externs and declarations needed for
                target specific code

  DEPENDENCIES: None

                Copyright (c) 2012-2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/sleep_target.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "sleepi.h"
#include "DALStdDef.h"
#include "DDIChipInfo.h"

/**
 * @brief Used to enumerate l2 lprm configuration
 */
typedef enum{
  L2_TCM_RET,       /* Config processor SS & L2 for L2 ret & TCM ret          */
  L2_TCM_NORET,     /* Config processor SS & L2 for L2 non-ret & TCM non-ret  */
  L2_NORET,         /* Config processor SS & L2 for L2 non-ret & TCM ret      */
  L2_NUM_RET_MODES  /* Total number of retention modes                        */
} l2_lprm;

/**
 *  @brief target config data returned by sleep_target_init()
 */
typedef struct 
{
  uint32                  entry_ticks;    /* Overhead Entering Sleep, without
                                             lpr entry time*/
  uint32                  exit_ticks;     /* Overhead Exiting Sleep, without 
                                             lpr exit time*/
  uint32                  init_delay_ms;  /* Delay before sleep_perform_lpm in 
                                             milliseconds*/
  DalChipInfoFamilyType   chip_family;    /* Chip family architecture*/

  DalChipInfoVersionType  chip_ver;       /* Chip architecture version */

  uint32                  l2_mask;        /* Mask for L2 Cache Power Ctl */

  uint32                  tcm_mask;       /* Mask for TCM Power Ctl */
} sleep_target_config;

/**
 * @brief sleep_target_init
 * 
 * Perform target-specific initialization of the sleep subsystem.
 */
sleep_target_config * sleep_target_init( void );

/**
 * @brief sleep_target_lpr_init 
 *  
 * Do target-specific initialization for the /sleep/lpr node.
 * This initialization will happen before the node is available
 * to the outside world.
 */
void sleep_target_lpr_init( void );

/**
 * @brief sleep_target_enable_sleep_timer
 *  
 * Set the timer match value hardware register to expire after
 * ticks amount of time. 
 * 
 * Function implementaion is defined by type of unerlying processor
 * Q6    : Converts ticks to sclks and programs IDLT
 * Others: Programs the SLEEP QTimer
 *
 * Any software overhead that is required before and after the actual 
 * sleep timer expires will have to be taken into account.
 *
 * @param wakeup_ticks : Absolute time in 19.2MHz ticks when wakeup 
 *                       should be programmed.
 *  
 * @return Actual wakeup time programed
 */
uint64 sleep_target_enable_sleep_timer( uint64 wakeup_ticks );

/**
 * @brief sleep_target_disable_sleep_timer
 *  
 * Disables any Timer Hardware that was programmed to wake-up
 * the processor.
 * 
 * Function implementaion is defined by type of unerlying processor
 * Q6    : Converts ticks to sclks and programs IDLT
 * Others: Programs the SLEEP QTimer
 *
 * 
 */
void sleep_target_disable_sleep_timer( void );

/**
 * @brief sleep function to disable deferrable timers
 *
 */
void sleep_target_defer_timers(void);

/**
 * @brief sleep function to enable deferrable timers
 *
 */
void sleep_target_undefer_timers(void);

/**
 * @brief sleep function to enable DOG AUTOKICK
 *
 */
void sleep_target_enable_dog(void);

/**
 * @brief sleep function to disable DOG AUTOKICK
 *
 */
void sleep_target_disable_dog(void);


/**
 * @brief Gets the current CPU family & version
 *  
 * @param chipFamily: Return value with family of chip
 * @param chipVer: Return value with version of chip
 */
void sleep_target_get_CPU_arch(DalChipInfoFamilyType *chip_family, 
                               DalChipInfoVersionType *chip_ver);

/**
 * sleep_target_set_rpm_assisted_mode
 *
 * @brief Sets an indicator if the RPM LPR enter/exit functions have been 
 * executed as part of the current synth mode 
 *  
 * This function should only be called from the RPM LPR functions. 
 *  
 * @param value: TRUE or FALSE to indicate if in RPM assisted mode 
 */
void sleep_target_set_rpm_assisted_mode( boolean value );

/**
 * sleep_target_get_rpm_assisted_mode
 *
 * @brief Retreives the value previously set by the "set" function that 
 * indicates if an RPM synth mode was selected for entry. 
 *
 * This function is mainly useful to the CPU VDD LPR to determine if we are 
 * in an RPM assised synth mode 
 *  
 * @return: TRUE or FALSE to indicate if we have entered RPM assisted mode
 */
boolean sleep_target_get_rpm_assisted_mode( void );

/**
 * sleep_target_get_rpm_stat_ptr
 *
 * @brief Gets the memory pointer to the shared memory location
 * of the RPM statistics data 
 *  
 * Note: This function has a dependency on MPM initialization. 
 *       The MPM must be initialized first in order to receive
 *       the MPM virtual address used by this function. 
 *   
 * @return: Pointer to data
 */
void* sleep_target_get_rpm_statistics_addr(void);

#endif


