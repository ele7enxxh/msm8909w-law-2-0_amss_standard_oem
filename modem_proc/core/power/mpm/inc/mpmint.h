#ifndef MPMINT_H
#define MPMINT_H
/*==============================================================================
  FILE:         mpmint.h
  
  OVERVIEW:     This file contains original MPM driver API calls which
                are obsoleted by new API functions. These empty
                function calls now only re-route to new MPM driver
                API functions with support of wrapper function.
 
  NOTE:         This file will be deprecated following migration of
                outside teams to DAL Driver
 
  DEPENDENCIES: None

                Copyright (c) 2011-2014 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/inc/mpmint.h#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "vmpm.h"

/*===========================================================================   
                      TYPE DECLARATIONS
===========================================================================*/   
/**
 *  Originally from the following deprecated files:
 *    mpmint_types.h
 *    mpmint_target.h
 */
typedef uint32 mpmint_isr_type;


/* The PIC hardware can support the setting of polarity
 * triggering on some interrupts.
 */
typedef enum
{
  MPMINT_LEVEL_DETECT = 0,
  MPMINT_EDGE_DETECT
} mpmint_detect_type;

/* The PIC hardware can support the setting of edge or level
 * triggering on some interrupts. 
 */
typedef enum
{
  MPMINT_ACTIVE_LOW     = 0,
  MPMINT_ACTIVE_FALLING = 0,
  MPMINT_ACTIVE_HIGH    = 1,
  MPMINT_ACTIVE_RISING  = 1,
  MPMINT_ACTIVE_DUAL    = 2,
} mpmint_polarity_type;

/* For dynamically configured interrupts which processor 
 * handles normally during active time.
 */
typedef enum
{
  MPMINT_TRIGGER_LEVEL_LOW      = 0,
  MPMINT_TRIGGER_EDGE_FALLING   = 1,
  MPMINT_TRIGGER_EDGE_RISING    = 2,
  MPMINT_TRIGGER_EDGE_DUAL      = 3,
  MPMINT_TRIGGER_LEVEL_HIGH     = 4,
}mpmint_trigger_type;

typedef enum
{
  MPMINT_RPM = 0,
  MPMINT_APPS,
  MPMINT_MODEM,
  MPMINT_LPASS
} mpmint_processor_type;

/* Pointer to an isr function that is passed within tramp services */
typedef void (*mpm_isr_ptr_type) (void);

/*===========================================================================   
                      PUBLIC FUNCTION DECLARATIONS                              
===========================================================================*/   
                             

/** 
 * @brief Configure the trigger type (RISING/FALLING/EDGE/LEVEL)
 *        of an MPM fixed-function interrupt.
 *
 * @TODO: 
 * OBSOLETE: Replaced with vmpm_configPin() (see vmpm.h) 
 *  
 * If you want to configure an MPM-routed GPIO interrupt, you probably want to
 * use mpmint_config_gpio_wakeup instead.  Also, if you simply want to enable
 * an interrupt as a wakeup source, see vmpm_config_wakeup.
 *
 * Since fixed-function interrupts generally have only a single configuration
 * that makes sense, this driver populates each one with a default setting at
 * init time.  In the unlikely event you want to override the default you can
 * call this function--but it is not expected to be used in general.
 *
 * @param mpmPinNum   The interrupt that you wish to 
 *                      configure.
 * @param detect        detection type 
 * @param polarity      polarity type 
 *
 * @see vmpm_configPin
 */
void mpmint_config_int( uint32 mpmPinNum,
                        mpmint_detect_type detect,
                        mpmint_polarity_type  polarity );
/**
 * @brief Enable an MPM-routed interrupt as a wakeup source during deep sleep.
 *
 * @TODO: 
 * OBSOLETE: Replaced with vmpm_configPin() (see vmpm.h) 
 *  
 * If the interrupt occurs during deep sleep, the MPM will wake the system and
 * this driver will soft-trigger the interrupt into the local interrupt
 * controller (that is, you do not have to handle the MPM interrupt seperate
 * from the interrupt's regular incarnation).
 *
 * @param mpmPinNum Which interrupt to enable.
 * @param ignored     No longer used--reatined for legacy 
 *                    compatibility.
 *  
 * @see vmpm_configPin
 */
void mpmint_config_wakeup ( mpmint_isr_type mpmPinNum, 
                            mpmint_processor_type unused );
/**
 * @brief Disable an MPM-routed interrupt as a wakeup source during deep sleep.
 *
 * @TODO: 
 * OBSOLETE: Replaced with vmpm_disablePin() (see vmpm.h) 
 *  
 * @note When an interrupt is disabled in the MPM, that interrupt may be
 *       completely discarded during deep sleep (not even latched for later
 *       processing).  However, it can still cause a wakeup from "shallower"
 *       forms of sleep (like simple halt, etc.).
 *
 * @param int_num Which interrupt to disable.
 */
void mpmint_disable_wakeup ( mpmint_isr_type int_num );

/**
 * @brief Configure a GPIO interrupt as a wakeup source.
 *
 * @TODO: 
 * OBSOLETE using vmpm_configPin() (see vmpm.h) 
 *  
 * @note If the GPIO is not supported by the MPM hardware this function does
 *       nothing and returns.
 *
 * @param gpio_num      GPIO ID number
 * @param detect        detection type 
 * @param polarity      polarity type 
 * @param unused        unused parameter
 */
void mpmint_config_gpio_wakeup( uint32 gpio_num,
                                mpmint_detect_type detect,
                                mpmint_polarity_type  polarity,
                                mpmint_processor_type unused );

/**
 * @brief Disable an MPM-routed GPIO as a wakeup source during deep sleep. 
 *  
 * @TODO: 
 * OBSOLETE: Replaced with vmpm_disablePin() (see vmpm.h) 
 *  
 *
 * @note When an interrupt is disabled in the MPM, that interrupt may be
 *       completely discarded during deep sleep (not even latched for later
 *       processing).  However, it can still cause a wakeup from "shallower"
 *       forms of sleep (like simple halt, etc.).
 *
 * @param which_gpio The number of the GPIO to disable.
 */
void mpmint_disable_gpio_wakeup ( uint32 which_gpio );

void mpmint_disable_gpio ( uint32 mpmPinNum ); 
#endif  /* MPMINT_H */

