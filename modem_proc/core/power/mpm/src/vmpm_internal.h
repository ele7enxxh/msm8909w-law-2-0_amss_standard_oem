#ifndef VMPM_INTERNAL_H
#define VMPM_INTERNAL_H
/*==============================================================================
  FILE:         vmpm_internal.h
  
  OVERVIEW:     This file contains the definitions for the MPM Interrupt
                Controller. This includes the table of values for each of the
                interrupt sources and the necessary functions to interface the
                MPM IC hardware registers.
 
  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/vmpm_internal.h#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "vmpm.h"
#include "vmpm_HAL.h"

/*=========================================================================
 *                     INTERNAL OS SPECIFIC DEFINITIONS
 *=========================================================================*/
/* The macros defined below are usually OS depedent. New definitions must be
 * added as support for new OS comes or it will result into default value
 * that may not work as expected on all OSes. */

#if defined(USES_QNX) && defined(QNP_POWERMAN_PROC)

  /* QNX powerman process specific implementation */

  #define VMPM_LOCK()
  #define VMPM_UNLOCK()

#else

  /* Default implementation for message ram lock/unlock. */

  #include "DALSys.h"

  extern DALSYSSyncHandle mpmSharedMsgRamLock;

  #define VMPM_LOCK()   DALSYS_SyncEnter(mpmSharedMsgRamLock)
  #define VMPM_UNLOCK() DALSYS_SyncLeave(mpmSharedMsgRamLock)

#endif

/*===========================================================================
 *                         INTERNAL TYPE DEFINITIONS
 *==========================================================================*/


/*===========================================================================
 *                       EXTERNAL VARIABLE DECLARATIONS
 *===========================================================================*/
/**
 * Pointer to the table containing mapping of master interrupt ids to mpm
 * interrupt ids. Depending on particular target, this table may contain
 * intermediate invalid/unmapped entries.
 */

extern mpm_int_pin_mapping_type *vmpm_pinMapTbl;

/**
 * Address in message ram for master's mpm configuration registers base.
 */
extern uint32 vmpm_IrqCfgRegBase_VA;

/**
 * Id for wakeup interrupt to this master. Ideally type of this 
 * variable would be some typedef but those are not allowed in 
 * DevCfg xml files which is one of the ways to obtain this 
 * value. 
 *  
 * @note: supports RPM and AON 
 */
extern uint32 vmpm_wakeupIrq;

/**
 * Flags for rpm wakeup interrupt. As of now this flag just contains
 * trigger type information.
 */
extern uint32 vmpm_wakeupIrqFlags;

/*===========================================================================
 *                            MUTATOR/ACCESSOR FUNCTION PROTOTYPES
 *===========================================================================*/
/**
 * Set & Get MPM IRQ/GPIO Mapping Table Size 
 * @note: vmpm_pinMapTbl size must be first initialized 
 *        before calling this function 
 */
void vmpm_setMapTblSize( void );
uint32 vmpm_getMapTblSize( void );
void vmpm_setNumMappedIrqs( void );
uint32 vmpm_getNumMappedIrqs( void );
void vmpm_setNumMappedGpios( void );
uint32 vmpm_getNumMappedGpios( void );

/*===========================================================================
 *                            FUNCTION PROTOTYPES
 *===========================================================================*/
/**
 * Triggers the GPIO passed as argument on the master. This function usually
 * called by vmpm_TriggerInterrupts while coming out of sleep if it finds
 * particular GPIO occurred at MPM during sleep.
 *
 * Depending on the implementation, validity of gpio_num may or may not be
 * checked. It is meant for vMPM internal use only.
 *
 * @see vmpm_TriggerInterrupts
 *
 * @param gpio_num: Actual gpio number that will be triggered on the master.
 */
void vmpm_RetriggerGpio ( uint32 gpioNum );

/**
 * Trigger the interrupt passed as argument on the master. This function 
 * usually called by vmpm_TriggerInterrupts while coming out of
 * sleep if it finds particular interrupt occurred at MPM during 
 * sleep. 
 *
 * Depending on the implementation, validity of gpio_num may or may not be
 * checked. It is meant for vMPM internal use only.
 *
 * @see vmpm_TriggerInterrupts
 *
 * @param irqNum: Local IRQ number for the interrupt that will 
 *                be triggered on the master.
 */
void vmpm_RetriggerIrq ( uint32 irqNum );

/**
 * Initializes all target specific data for common mpm driver to use. 
 * Depending on the OS, this function can query DevCfg APIs to obtain
 * and then intialize target specific data or can have direct assignment.
 * Currently it is expected to initializ following variables
 *   - vmpm_IrqCfgRegBase_VA
 *   - vmpm_wakeupIrq
 *   - vmpm_wakeupIrqFlags
 *
 * @return none
 */
void vmpm_TargetDataInit( void );

/**
 * Maps the MPM HW to memory and sets a global variable to contain that
 * virtual address.
 *
 * @return none
 */
void vmpm_MapHwRegBase( void );

/**
 * Initializes log for mpm. This function must be called before using
 * any log functionalities for mpm.
 */
void vmpm_LogInit( void );

/**
 * Logs the mpm driver messages in Ulog. A valid ulog handle should be passed
 * as there will not be any checking in this function. For low memory targets
 * such as sensor, this could be a stub function.
 *
 * @Note
 * vmpm_LogInit must be called before calling this function.
 *
 * @param arg_count: Number of parameters that are to be logged
 * @param format: The formate string to print logs. Similar to one used in 
 *                printf statements.
 */
void vmpm_LogPrintf( int arg_count, const char * format, ... );
                                                                                
/**
 * @brief Initialize the MPM interrupt controller subsystem.
 *
 * This function initializes the MPM interrupt controller to a known state (all
 * interrupts masked off).  The MPM interrupt controller is the piece of
 * hardware that remains on in even the deepest sleep modes, allowing the
 * system to wake for interrupts no matter what power saving measures have been
 * taken.  An ISR is also registered, so that this driver can soft-trigger the
 * local equivalents of any interrupt that the MPM catches during a sleep mode
 * (thus removing any requirement to monitor an interrupt in both the MPM and
 * the local interrupt controller).
 */
void vmpm_init( void );

/**
 * @brief Register the wakeup ISR for the master
 */
void vmpm_registerISR( void );

/**
 * @brief De-register the wakeup ISR for the master
 */
void vmpm_deRegisterISR( void );

/**
 * @brief Determine if the mpm pin number is mapped as a GPIO 
 *        pin. 
 *  
 * @param mpmPinNum          mpm pin number  
 * 
 * @return mpm_status_type   Returns MPM_SUCCESS if the pin is 
 *                           mapped as a GPIO pin within the
 *                           target configuration table
 */
mpm_status_type vmpm_IsGpio(uint32 mpmPinNum);

/**
 * @brief Determine if the mpm pin number is mapped as an IRQ 
 *        pin
 * 
 * @param mpmPinNum          mpm pin number  
 * 
 * @return mpm_status_type   Returns MPM_SUCCESS if the pin is 
 *                           mapped as an IRQ pin within the
 *                           target configuration table
 */
mpm_status_type vmpm_IsIrq(uint32 mpmPinNum);

/**
 * This function is used to determine if the MPM Pin interrupt 
 * is supported on this HW platform. 
 *  
 * @brief Determines if the input value corresponds to a valid 
 *        wakeup interrupt line for a this target.
 *
 * It is a valid wake up if the table entry at that index within
 * the mapping table
 *
 *
 * @param nMPM  A value which would be used to index into the 
 *              pin mapping config table.
 * 
 * @return mpm_status_type   Returns MPM_SUCCESS if the pin is 
 *                           mapped within the target
 *                           configuration table
 */
mpm_status_type vmpm_IsPinSupported( uint32 nMPM );

/**
 * This function is used to determine if a particular MPM line is of
 * interrupt type and if it is a wakeup source.
 *
 * @param      nMPM       Index of the entry in to the mappling 
 *                        table for the MPM line we are
 *                        interested in.
 * @param[out] ptr_IrqNum Interrupt id on the 
 *                        master GIC that corresponds to this
 *                        entry in the mapping table if the
 *                        entry is a wakeup source else
 *                        untouched (valid only if this function
 *                        returns TRUE).
 *
 * @return mpm_status_type   Returns MPM_SUCCESS if the pin is 
 *                           mapped as an IRQ pin within the
 *                           target configuration table
 */
mpm_status_type  vmpm_getIrqNum( uint32 nMPM, uint32 *ptr_IrqNum );

/**
 * This function is used to determine if a particular MPM line is of
 * interrupt type and if it is a wakeup source.
 *
 * @param      nMPM         Index of the entry in to mapping 
 *                          table for the MPM line of interest.
 * 
 * @param[out] ptr_GpioNum  GPIO number on the master 
 *                          that corresponds to this entry in
 *                          the mapping table (valid only if
 *                          this function returns TRUE).
 *
 * @return mpm_status_type   Returns MPM_SUCCESS if the pin is 
 *                           mapped as an GPIO pin within the
 *                           target configuration table
 */
mpm_status_type  vmpm_getGpioNum( uint32 nMPM, uint32 *ptr_GpioNum );

#endif  /* VMPM_INTERNAL_H */

