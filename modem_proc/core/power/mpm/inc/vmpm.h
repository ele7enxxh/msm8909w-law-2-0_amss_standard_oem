#ifndef VMPM_H
#define VMPM_H

/*==============================================================================
  FILE:         vmpm.h
  
  OVERVIEW:     This file provides the libraries for the Virtual MPM (vMPM)
                driver API code

  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/inc/vmpm.h#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 

/*============================================================================
 *                     INCLUDE FILES FOR MODULE
 *==========================================================================*/
#include <stdint.h>
#include <HALcomdef.h> /* bool32 typedef */
#include "comdef.h"
#include "vmpm_types.h"

/*===========================================================================   
                      PUBLIC FUNCTION DECLARATIONS                              
===========================================================================*/
/**
 *  vmpm_configAndEnablePin
 *  
 * @brief  Configures the trigger type and enables the MPM pin
 *         provided as input
 *
 * This function configures and enables a MPM pin with the 
 * configuration information provided.
 * Since fixed-function interrupts generally have only a single configuration
 * that makes sense, this driver populates each one with a default setting at
 * init time.  In the unlikely event you want to override the default you can
 * call this function--but it is not expected to be used in general.
 *
 *  
 * @param mpmPinNum       Unsigned integer represents 
 *                        MPM2 Pin number in valid range 
 * @param ptrConfigInfo MPM configuration includes pin type
 *                        (GPIO or IRQ) and trigger type.
 * @return mpm_status_type 
 */
mpm_status_type vmpm_configAndEnablePin( uint32 mpmPinNum,
                                         mpm_pin_config_type *ptrConfigInfo );

/**
 *  vmpm_configPin
 *  
 * @brief  Configures the trigger type of the MPM pin provided 
 *         as input
 *
 * This function only configures a MPM pin (pin is not enabled) 
 * with the configuation information provided. Since 
 * fixed-function interrupts generally have only a single 
 * configuration that makes sense, this driver populates each 
 * one with a default setting at init time.  In the unlikely 
 * event you want to override the default you can call this 
 * function--but it is not expected to be used in general. 
 *
 *  
 * @param mpmPinNum       Unsigned integer represents MPM2 Pin
 *                        number in valid range
 * @param ptrConfigInfo MPM configuration includes pin type
 *                        (GPIO or IRQ) and trigger type.
 * @return mpm_status_type 
 */
mpm_status_type vmpm_configPin( uint32 mpmPinNum,
                                mpm_pin_config_type *ptrConfigInfo );

/**
 * vmpm_enablePin 
 *  
 * @brief Enables IRQ or GPIO monitoring at a given MPM Pin on 
 *        MPM HW
 * 
 * @param mpmPinNum 
 * @return mpm_status_type 
 */
mpm_status_type vmpm_enablePin( uint32 mpmPinNum );

/**
 * vmpm_disablePin 
 *  
 * @brief Disables IRQ or GPIO monitoring at a given MPM Pin on 
 *        MPM HW
 * 
 * @param mpmPinNum 
 * @return mpm_status_type 
 */
mpm_status_type vmpm_disablePin( uint32 mpmPinNum );

/** 
 *  vmpm_queryPinMapping
 *  
 * @brief  Query routine provides the MPM pin number (if mapped)
 *         from the local pin number if configured
 *
 * @param local_pin_num     Local Controller pin number from 
 *                          interrupt ID at local interrupt
 *                          controller.
 * @param pinType           pin type (IRQ or GPIO) to query
 * @param ptr_mpmPinNum     pointer to 32-bit unsigned integer 
 *                          for MPM Pin number
 * @return                  Returns status MPM_SUCCESS or 
 *                          MPM_FAIL, If MPM pin is configured
 *                          for GPIO or IRQ and pin number is
 *                          valid MPM_SUCCESS is returned.
 */
mpm_status_type vmpm_queryPinMapping( uint32 local_pin_num,
                                      mpm_pin_type pinType,
                                      uint32 *ptr_mpmPinNum );
                             
/** 
 *  vmpm_query_pin_config_info
 *  
 * @brief  Query routine provides the trigger configuration 
 *         informatio, pin type, and local pin number given the
 *         MPM pin number as input
 *
 * @param mpmPinNum         MPM pin number 
 * @param ptr_mpm_pin_info  Pin configuration info as follows:
 *                          - local controller pin number
 *                          - pin type (e.g. GPIO or IRQ)
 *                          - pin trigger config type (e.g.
 *                            MPM_TRIGGER_RISING_EDGE)
 * @return                  Returns status MPM_SUCCESS or 
 *                          MPM_FAIL. Returns MPM_SUCCESS if MPM
 *                          pin is configured for GPIO or IRQ
 *                          and pin number is valid.
 */
mpm_status_type vmpm_queryPinConfigInfo( uint32 mpmPinNum,
                                         mpm_pin_query_info_type *ptr_mpm_pin_info);

/**
 * vmpm_addIrqPinToMappingTbl 
 *  
 * @brief Checks MPM Pin Num is valid and request is to 
 *        configure a pin in mapping table to be an IRQ pin.
 *        Also check that legitimate value for Irq ID value is
 *        requested.
 *  
 * @param mpmPinNum          MPM Pin Number value (range: 0-63). 
 *                           This value is the index into the
 *                           mapping table
 * @param newMapTblEntryInfo New Mapping table entry information 
 *                           includes:
 *                           - Trigger Type
 *                           - Local ID of Pin
 *                           - MPM_IRQ (assumed) returns
 *                             MPM_FAIL otherwise.
 * @return mpm_status_type  returns MPM_FAIL if input conditions 
 *                          are not met
 * 
 */
mpm_status_type vmpm_addIrqPinToMappingTbl( uint32 mpmPinNum,
                                            mpm_int_pin_mapping_type *pNewMapTblEntryInfo);

/**
 * @brief Fills in the input arrays with the mapping of mpm interrupt id
 *        and corresponding master interrupt id.
 *
 * It should be noted that the first two fields of 
 * mpm_config_info_type must not be changed by caller or it 
 * can result into configuring other interrupts which are not 
 * meant to be. 
 *
 * @param irqArray: Array in which above mapping will be filled in.
 * @param irqCount: Size of the array (Usually it should be value returned
 *                  in numMapped)
 *
 * @param numMapped: Number of mappings actually filled in 
 *                   (should be same as irqCount in normal
 *                   scenario).
 * 
 * @return mpm_status_type   Returns MPM_SUCCESS if the pin is 
 *                           configured as a IRQ pin within the
 *                           target configuration table
 */
mpm_status_type vmpm_MapIrqsWithMaster ( mpm_config_info_type *irqArray, 
                                         uint32 irqCount,
                                         uint32 *numMapped);

/**
 * @brief Sets up various fields of more than one interrupts at MPM level
 *        like trigger type and status.
 *
 * @note  As of now this function is intended to use for interrupts only 
 *        (no GPIO). 
 *
 * @param irqArray: Input array containing various interrupt information 
 * @param irqCount: Number of interrupts in the above array.
 * 
 * @return mpm_status_type   Returns MPM_SUCCESS if the pin is 
 *                           configured as a IRQ pin within the
 *                           target configuration table
 */
mpm_status_type vmpm_SetupIrqs ( mpm_config_info_type *irqArray, 
                                 uint32 irqCount );

/** 
 *  vmpm_TriggerInterrupts 
 *  
 * @brief A function that loops through all the latched MPM interrupts and
 *        performs an associated soft-trigger operation so that it looks like
 *        the interrupt was caught in the normal mechanism, rather than by a
 *        secondary interrupt controller during deep sleep.
 * @ Note: Required by sleep lpr while exiting sleep 
 */
void vmpm_TriggerInterrupts( void );

/** 
 * vmpm_getProcMpmBaseVA 
 *  
 * @brief Returns the virtual address for the start of the 
 *        processor's MPM configuration area in shared RAM
 *        corresponding to wakeup_time. This virtual address
 *        value is determined at runtime and is target specific
 *  
 * @note  This function has dependency on prior initialization 
 *        of MPM driver as the virtual address is determined by
 *        MPM driver during runtime and target specific. 
 *  
 * @return Returns the virtual address for the processor's MPM 
 *         RAM.  Returns zero if not vMPM driver is not properly
 *         initialized before calling.
 */
uint32 vmpm_getProcMpmBaseVA( void );
#endif  /* VMPM_H */
