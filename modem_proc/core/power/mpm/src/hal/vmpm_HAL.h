#ifndef VMPM_HAL_H
#define VMPM_HAL_H

/*===========================================================================

  FILE:         vmpm_HAL.h

  DESCRIPTION:  This is the hardware abstraction layer interface for the MPM 
                interrupt controller block.

  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/hal/vmpm_HAL.h#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 

#ifdef __cplusplus
extern "C" {
#endif

#include <HALcomdef.h>
#include "vmpm_internal.h"
/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/**
 * Binary Compatibility Notes:
 * If any of the following macros and enums need change for a target within
 * same family, it may affect binary compatibility of vMPM driver.
 */

/* 
 * HAL_TT_EXTRACT_*_BIT 
 *
 * These MACROS are used to convert mpm_trigger_type values into
 * bit values for vmpm based Polarity,Detect3 and Detect1 registers
 */

#define HAL_TT_EXTRACT_POL_BIT(hal_tt)  ((hal_tt & 0x4) >> 2)
#define HAL_TT_EXTRACT_DET3_BIT(hal_tt) ((hal_tt & 0x2) >> 1)
#define HAL_TT_EXTRACT_DET1_BIT(hal_tt) ((hal_tt & 0x1) >> 0)

/*
 * vmpm_HAL_StatusType
 *
 * Enumeration of possible interrupt status
 *
 * @Note
 * Change in this enumeration may require change in mpm_config_info_type
 * and vmpm_HAL_ConfigInfoType.
 */
typedef enum
{
  VMPM_HAL_STATUS_DISABLE = 0,
  VMPM_HAL_STATUS_ENABLE  = 1,
}vmpm_HAL_StatusType;

/*
 * vmpm_HAL_ConfigInfoType
 *
 * Stores various information about an interrupt. This structure is mainly
 * used when there is a need to configure/enable/disable more than one
 * mpm mapped interrupts.
 *
 * @Note
 * This structure must be same as the one defined in mpm header file as
 * main purpose for decoupling them is avoid including HAL mpm files
 * outside mpm code and also avoid including any driver level file at
 * HW level.
 */
typedef struct vmpm_HAL_ConfigInfoType
{
  uint32 mpm_hw_int_id;            /* Actual interrupt id at MPM hardware */
  uint32 master_hw_int_id;         /* Interrupt id at master hardware 
                                      (could be Tramp if available) */
  uint8 trigger;                   /* Trigger for the interrupt at mpm */
  uint8 status;                    /* Interrupt status */
}vmpm_HAL_ConfigInfoType;

/* -----------------------------------------------------------------------
** Variable Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Interface
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** HAL standard APIs
** ----------------------------------------------------------------------- */
                                 
/**
 * This is the init function for the MPM interrupt controller HAL layer.
 *
 * It should be noted that in case of OS where we cannot directly write into
 * some physical memory address, the memory must be mapped while passing
 * the second parameter as virtual address.
 *
 * @param ppszVersion  The HAL version string for this HAL.
 * @param master_msg_ram_base_ptr: Base address (physical/virtual based on OS)
 *                                 of master's shared message ram with rpm for 
 *                                 MPM.
 * @param init_vmpm_regs: If true, initializes the vmpm registers that rpm
 *                        uses else keep it the way they were. It can be false
 *                        for the OSes in which mpm driver is used in multiple
 *                        process space and once initialized, we don't want to
 *                        disturb vmpm configuration registers.
 * @return None
 */
void vmpm_HAL_Init ( char **ppszVersion, 
                     uint8 *master_msg_ram_base_ptr, 
                     bool32 init_vmpm_regs );

/**
 * This is the restore function for the MPM interrupt controller HAL layer.  It
 * does nothing of particular value, since the MPM hardware is always on.
 * 
 * @return None
 */
void vmpm_HAL_Restore( void );
                                 

/* -----------------------------------------------------------------------
** Single interrupt APIs
** ----------------------------------------------------------------------- */

/**
 * This function is used to enable a specific interrupt in the MPM interrupt
 * controller HW.  If the interrupt is not supported on this platform, this
 * function does nothing.
 *
 * @param nMPM  A valid index in to mapping table to enable interrupt/gpio 
 *              corresponding to that index.
 * 
 * @return None
 */
void vmpm_HAL_Enable( uint32 nMPM );

/**
 * This function is used to disable a specific interrupt in the MPM interrupt
 * controller HW.  If the interrupt is not supported on this platform, this
 * function does nothing.
 *
 * @param nMPM  A valid index in to mapping table to disable 
 *              interrupt/gpio corresponding to that index.
 * 
 * @return None
 */
void vmpm_HAL_Disable( uint32 nMPM ); 

/**
 * This function is used to clear a specific interrupt in the MPM interrupt
 * controller HW.  If the interrupt is not supported on this platform, this
 * function does nothing.
 *
 * @param nMPM  A valid index in to mapping table to clear 
 *              interrupt/gpio represented by that index.
 * 
 * @return None
 */
void vmpm_HAL_Clear( uint32 nMPM );


/* -----------------------------------------------------------------------
** Mass interrupt APIs
** ----------------------------------------------------------------------- */

/**
 * This function enables every interrupt supported by this platform in the MPM
 * interrupt controller HW.
 * 
 * @return None
 */
void vmpm_HAL_All_Enable( void );

/**
 * This function disables every interrupt supported by this platform in the MPM
 * interrupt controller HW.
 * 
 * @return None
 */
void vmpm_HAL_All_Disable( void ); 

/**
 * This function clears every interrupt supported by this platform in the MPM
 * interrupt controller HW.
 * 
 * @return None
 */
void vmpm_HAL_All_Clear( void );


/* -----------------------------------------------------------------------
** Masked interrupt APIs
** ----------------------------------------------------------------------- */

/**
 * This function is used to determine the number of 32-bit masks needed to
 * represent all of the physical interrupts supported by the MPM interrupt
 * controller on this platform.
 *
 * @param[out] pnNumber  The number of 32-bit masks needed to represent all
 *                       interrupts supported by this platform.
 * 
 * @return None
 */
void vmpm_HAL_GetNumberMasks( uint32 *pnNumber );

/**
 * This function is used to enable multiple interrupts at once.
 * 
 * @param nMaskIndex  Which 32-bit mask is being set.  The mask that houses
 *                    physical interrupt number 'n' is floor(n / 32).
 * @param nMask       The bits of the mask to be enabled in HW.  The bit to set
 *                    for physical interrupt 'n' is (1 << (n % 32)).
 *
 * @return None
 *
 * @see vmpm_HAL_GetNumberMasks
 */
void vmpm_HAL_Mask_Enable( uint32 nMaskIndex, uint32 nMask );

/**
 * This function is used to disable multiple interrupts at once.
 * 
 * @param nMaskIndex  Which 32-bit mask is being set.  The mask that houses
 *                    physical interrupt number 'n' is floor(n / 32).
 * @param nMask       The bits of the mask to be disabled in HW.  The bit to set
 *                    for physical interrupt 'n' is (1 << (n % 32)).
 *
 * @return None
 *
 * @see vmpm_HAL_GetNumberMasks
 */
void vmpm_HAL_Mask_Disable( uint32 nMaskIndex, uint32 nMask );

/**
 * This function is used to clear multiple interrupts at once.
 * 
 * @param nMaskIndex  Which 32-bit mask is being set.  The mask that houses
 *                    physical interrupt number 'n' is floor(n / 32).
 * @param nMask       The bits of the mask to be cleared in HW.  The bit to set
 *                    for physical interrupt 'n' is (1 << (n % 32)).
 *
 * @return None
 *
 * @see vmpm_HAL_GetNumberMasks
 */
void vmpm_HAL_Mask_Clear( uint32 nMaskIndex, uint32 nMask );

/**
 * This function is used to retrieve a mask of pending interrupts.
 * 
 * @param nMaskIndex   Which 32-bit mask to retrieve.  The mask that houses
 *                     physical interrupt number 'n' is floor(n / 32).
 * @param pnMask[out]  The mask representing pending interrupts.  The bit that
 *                     that will be set for physical interrupt 'n' is
 *                     (1 << (n % 32)).
 *
 * @return None
 *
 * @see vmpm_HAL_GetNumberMasks
 */
void vmpm_HAL_GetPending( uint32 nMaskIndex, uint32 *pnMask );


/* -----------------------------------------------------------------------
** Interrupt configuration APIs
** ----------------------------------------------------------------------- */

/**
 * This function is used retrieve the current configuration of an interrupt as
 * it is programmed into the MPM HW.
 *
 * @param      nMPM       The interrupt which needs its configuration queried.
 * @param[out] peTrigger  The configuration of the interrupt requested.
 * 
 * @return None
 */
void vmpm_HAL_GetTrigger( uint32 nMPM, mpm_trigger_type *peTrigger );

/**
 * This function is used set the current configuration of an interrupt in the
 * MPM interrupt controller HW.
 *
 * @param nMPM      The interrupt which should be reconfigured.
 * @param eTrigger  The new configuration for the interrupt requested.
 * 
 * @return None
 */
void vmpm_HAL_SetTrigger( uint32 nMPM, mpm_trigger_type eTrigger );

/**
 * This function is used to configure and enable/disable multiple interrupts
 * at once in the MPM interrupt controller HW.
 *
 * @param irqArray:  Array containing interrupts' 
 *                   information like id, trigger type and
 *                   status.
 * @param irqCount:  Size of the above array.
 *
 * @return none
 */
void vmpm_HAL_SetupIrqs ( vmpm_HAL_ConfigInfoType *irqArray, 
                          uint32 irqCount );

/* -----------------------------------------------------------------------
** Query APIs
** ----------------------------------------------------------------------- */

/**
 * This function is used to determine if a particular MPM interrupt is pending
 * in HW.
 *
 * @param nMPM  Index in to HAL config table for the interrupt to be checked
 *              for pending status.
 * 
 * @return TRUE if the HW indicates this interrupt is pending, otherwise FALSE.
 */
bool32 vmpm_HAL_IsPending( uint32 nMPM );

/**
 * This function is used to determine if a particular MPM interrupt is enabled
 * in HW.
 *
 * @param nMPM  Index in to HAL config table for the interrupt to be checked
 *              if enabled.
 * 
 * @return TRUE if the HW indicates this interrupt is currently enabled in HW,
 *         otherwise FALSE.
 */
bool32 vmpm_HAL_IsEnabled( uint32 nMPM );

/**
 * This function is used to determine if a particular MPM interrupt is cleared
 * before its associated ISR is called or not.  If it is not a 'clear before'
 * interrupt it is assumed the interrupt is cleared after the ISR is called.
 *
 * @param nMPM  Index in to HAL config table for the interrupt to be checked
 *              if Clear Before.
 * 
 * @return TRUE if the this interrupt is 'clear before', FALSE if it is 'clear
 *         after.'
 */
bool32 vmpm_HAL_IsClearBefore( uint32 nMPM );

#ifdef __cplusplus
}
#endif

#endif /* VMPM_HAL_H */


