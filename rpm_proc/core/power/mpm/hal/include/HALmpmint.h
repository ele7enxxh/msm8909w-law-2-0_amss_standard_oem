#ifndef HALMPMINT_H
#define HALMPMINT_H
/*
===========================================================================

FILE:         HALmpmint.h

DESCRIPTION:  
  This is the hardware abstraction layer interface for the MPM interrupt
  controller block.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/include/HALmpmint.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
          Copyright © 2008-2010 Qualcomm Technologies Incorporated.
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

#include "comdef.h"
#include "HALmpm.h"

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

#include HAL_MPMINT_TARGET_H

/*                                                       
 * HAL_MPMINT_NOT_DEFINED                                
 *                                                       
 * Used to specify an invalid MPM interrupt.             
 */                                                      
#define HAL_MPMINT_NOT_DEFINED  HAL_MPMINT_NUM

/*
 * HAL_MPMINT_INVALID_GPIO
 *
 * Used to specify an invalid GPIO interrupt.
 */
#define HAL_MPMINT_INVALID_GPIO 0xFF

/*
 * HAL_MPMINT_INVALID_BIT_POS
 *
 * Used to specify an invalid GPIO interrupt.
 */
#define HAL_MPMINT_INVALID_BIT_POS 0xFF

/*
 * HAL_MPMINT_NONE
 *
 * Value returned by GetPending to indicate no more interrupts pending.
 */
#define HAL_MPMINT_NONE  0xFFFF


/*
 * HAL_mpmint_TriggerType
 *
 * Enumeration of possible trigger types for a MPM interrupt.
 *
 * HAL_MPM_TRIGGER_LOW:     Interrupt is level sensitive, negative polarity.
 * HAL_MPM_TRIGGER_FALLING: Interrupt is falling edge sensitive, negative polarity.
 * HAL_MPM_TRIGGER_RISING:  Interrupt is rising edge sensitive, negative polarity.
 * HAL_MPM_TRIGGER_DUAL:    Interrupt is edge sensitive 
 * HAL_MPM_TRIGGER_HIGH:    Interrupt is level sensitive, positive polarity.
 */
typedef enum
{
  HAL_MPMINT_TRIGGER_LOW      = 0,
  HAL_MPMINT_TRIGGER_FALLING  = 1,
  HAL_MPMINT_TRIGGER_RISING   = 2,
  HAL_MPMINT_TRIGGER_DUAL     = 3,
  HAL_MPMINT_TRIGGER_HIGH     = 4,
  HAL_MPMINT_TRIGGER_INVALID
} HAL_mpmint_TriggerType;

extern const HAL_mpmint_TriggerType mpmint_trigger_lookup[];

/*
 * HAL_mpmint_GroupType
 */
typedef enum
{
  HAL_MPMINT_GROUP1 = 0,
  HAL_MPMINT_GROUP2 = 1,
} HAL_mpmint_GroupType;


/* -----------------------------------------------------------------------
** Interface
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** HAL standard APIs
** ----------------------------------------------------------------------- */
                                 
/**
 * This is the init function for the MPM interrupt controller HAL layer.
 * 
 * @param ppszVersion  The HAL version string for this HAL.
 * 
 * @return None
 */
void HAL_mpmint_Init              ( char **ppszVersion );

/**
 * This is the reset function for the MPM interrupt controller HAL layer.
 * 
 * @return None
 */
void HAL_mpmint_Reset             ( void );

/**
 * This is the save function for the MPM interrupt controller HAL layer.  It
 * does nothing of particular value, since the MPM hardware is always on.
 * 
 * @return None
 */
void HAL_mpmint_Save              ( void );

/**
 * This is the restore function for the MPM interrupt controller HAL layer.  It
 * does nothing of particular value, since the MPM hardware is always on.
 * 
 * @return None
 */
void HAL_mpmint_Restore           ( void );
                                 

/* -----------------------------------------------------------------------
** Single interrupt APIs
** ----------------------------------------------------------------------- */

/**
 * This function is used to enable a specific interrupt in the MPM interrupt
 * controller HW.  If the interrupt is not supported on this platform, this
 * function does nothing.
 *
 * @param nMPM  The value from HAL_mpmint_IsrType representing the interrupt to
 *              enable.
 * 
 * @return None
 */
void HAL_mpmint_Enable            ( uint32 nMPM );

/**
 * This function is used to disable a specific interrupt in the MPM interrupt
 * controller HW.  If the interrupt is not supported on this platform, this
 * function does nothing.
 *
 * @param nMPM  The value from HAL_mpmint_IsrType representing the interrupt to
 *              disable.
 * 
 * @return None
 */
void HAL_mpmint_Disable           ( uint32 nMPM ); 

/**
 * This function is used to clear a specific interrupt in the MPM interrupt
 * controller HW.  If the interrupt is not supported on this platform, this
 * function does nothing.
 *
 * @param nMPM  The value from HAL_mpmint_IsrType representing the interrupt to
 *              clear.
 * 
 * @return None
 */
void HAL_mpmint_Clear             ( uint32 nMPM );


/* -----------------------------------------------------------------------
** Mass interrupt APIs
** ----------------------------------------------------------------------- */

/**
 * This function enables every interrupt supported by this platform in the MPM
 * interrupt controller HW.
 * 
 * @return None
 */
void HAL_mpmint_All_Enable        ( void );

/**
 * This function disables every interrupt supported by this platform in the MPM
 * interrupt controller HW.
 * 
 * @return None
 */
void HAL_mpmint_All_Disable       ( void ); 

/**
 * This function clears every interrupt supported by this platform in the MPM
 * interrupt controller HW.
 * 
 * @return None
 */
void HAL_mpmint_All_Clear         ( void );


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
void HAL_mpmint_GetNumberMasks    ( uint32 *pnNumber );

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
 * @see HAL_mpmint_GetNumberMasks
 * @see HAL_mpmint_GetPhysNumber
 */
void HAL_mpmint_Mask_Enable       ( uint32 nMaskIndex, uint32 nMask );

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
 * @see HAL_mpmint_GetNumberMasks
 * @see HAL_mpmint_GetPhysNumber
 */
void HAL_mpmint_Mask_Disable      ( uint32 nMaskIndex, uint32 nMask );

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
 * @see HAL_mpmint_GetNumberMasks
 * @see HAL_mpmint_GetPhysNumber
 */
void HAL_mpmint_Mask_Clear        ( uint32 nMaskIndex, uint32 nMask );

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
 * @see HAL_mpmint_GetNumberMasks
 * @see HAL_mpmint_GetPhysNumber
 */
void HAL_mpmint_GetPending        ( uint32 nMaskIndex, uint32 *pnMask );


/* -----------------------------------------------------------------------
** Interrupt configuration APIs
** ----------------------------------------------------------------------- */


/**
 * This function is used retrieve the current configuration of an interrupt
 * based on the detect and polarity bits, and the interrupt mask.
 *
 * @param      nDet       Detect data bit 1
 * @param      nDet3      Detect data bit 2
 * @param      nPol       Polarity data
 * @param      nMask      The interrupt mask
 * @param[out] peTrigger  The configuration of the interrupt requested.
 * 
 * @return None
 */
void HAL_mpmint_GetIndex        ( uint32 nDet, uint32 nDet3, uint32 nPol, uint32 nMask, 
                                    HAL_mpmint_TriggerType *peTrigger );

/**
 * This function is used retrieve the current configuration of an interrupt as
 * it is programmed into the MPM HW.
 *
 * @param      nMPM       The interrupt which needs its configuration queried.
 * @param[out] peTrigger  The configuration of the interrupt requested.
 * 
 * @return None
 */
void HAL_mpmint_GetTrigger        ( uint32 nMPM, 
                                    HAL_mpmint_TriggerType *peTrigger );

/**
 * This function is used set the current configuration of an interrupt in the
 * MPM interrupt controller HW.
 *
 * @param nMPM      The interrupt which should be reconfigured.
 * @param eTrigger  The new configuration for the interrupt requested.
 * 
 * @return None
 */
void HAL_mpmint_SetTrigger        ( uint32 nMPM, 
                                    HAL_mpmint_TriggerType eTrigger );


/* -----------------------------------------------------------------------
** Query APIs
** ----------------------------------------------------------------------- */

/**
 * This function is used to determine the number of interrupts supported by
 * this version of the MPM HAL.  This is basically equivalent to the count of
 * all items in HAL_mpmint_IsrType.
 *
 * @note Not all of the interrupts listed in the enumeration are necessarily
 *       supported by this particular hardware platform.
 *
 * @param[out] pnNumber  The number interrupts supported by this version of the
 *                       MPM HAL.
 * 
 * @return None
 *
 * @see HAL_mpmint_IsSupported
 */
void HAL_mpmint_GetNumber         ( uint32 *pnNumber );

/**
 * This function is used to determine the number of physical interrupts
 * supported by this HW platform.
 *
 * @param[out] pnNumber  The number of physical interrupts supported by this HW
 *                       platform.
 * 
 * @return None
 */
void HAL_mpmint_GetNumberPhysical ( uint32 *pnNumber );

/**
 * This function is used to determine the physical assignment of an interrupt
 * defined in HAL_mpmint_IsrType for this HW platform.
 *
 * @param      nMPM    The HAL_mpmint_IsrType value to retrive the physical
 *                     assignment for.
 * @param[out] pnPhys  The physical interrupt number of nMPM if it is supported
 *                     by this HW platform, otherwise untouched.
 * 
 * @return FALSE if nMPM is not supported on this HW platform, otherwise TRUE.
 *
 * @see HAL_mpmint_IsSupported
 */
bool32 HAL_mpmint_GetPhysNumber   ( uint32 nMPM, uint32 *pnPhys );

/**
 * This function is used to determine which HAL_mpmint_IsrType interrupt is
 * assigned to a given physical interrupt number on this HW platform.
 *
 * @param      nPhys  The physical interrupt number to retrive the
 *                    HAL_mpmint_IsrType for.
 * @param[out] pnMPM  The HAL_mpmint_IsrType value if a valid physical
 *                    interrupt number is provided, otherwise untouched.
 * 
 * @return FALSE if nPhys is not a valid physical interrupt number for this HW
 *         platform, otherwise TRUE.
 *
 * @see HAL_mpmint_GetNumberPhysical
 */
bool32 HAL_mpmint_GetEnumNumber   ( uint32 nPhys, uint32 *pnMPM );

/**
 * This function is used to determine if a HAL_mpmint_IsrType interrupt is
 * supported on this HW platform.
 *
 * @param nMPM  The HAL_mpmint_IsrType interrupt value to query for support.
 * 
 * @return TRUE if this HW platform supports nMPM, otherwise FALSE.
 *
 * @see HAL_mpmint_GetNumber
 */
bool32 HAL_mpmint_IsSupported     ( uint32 nMPM );

/**
 * This function is used to determine if a particular GPIO is supported as an
 * MPM wakeup interrupt.
 *
 * @param      nGPIO  The GPIO number to check for wakeup interrupt support.
 * @param[out] pnMPM  The HAL_mpmint_IsrType interrupt value if the GPIO is
 *                    supported by the MPM on this HW platform, otherwise
 *                    untouched.
 * 
 * @return TRUE if this HW platform supports nGPIO as a wakeup interrupt,
 *         otherwise FALSE.
 */
bool32 HAL_mpmint_IsGpioSupported ( uint32 nGPIO, uint32 *pnMPM );

/**
 * This function is used to determine if a particular MPM interrupt is mapped
 * to a GPIO on this HW platform.
 *
 * @param      nMPM    The HAL_mpmint_IsrType value to be checked for a GPIO
 *                     assignment.
 * @param[out] pnGPIO  The GPIO number of the interrupt line if it is mapped to
 *                     a GPIO on this HW platform, otherwise untouched.
 * 
 * @return TRUE if this HW platform supports this wakeup via a GPIO, otherwise
 *         FALSE.
 */
bool32 HAL_mpmint_IsGpio          ( uint32 nMPM, uint32 *pnGPIO );

/**
 * This function is used to determine if a particular MPM interrupt is pending
 * in HW.
 *
 * @param nMPM  The HAL_mpmint_IsrType value to be checked.
 * 
 * @return TRUE if the HW indicates this interrupt is pending, otherwise FALSE.
 */
bool32 HAL_mpmint_IsPending       ( uint32 nMPM );

/**
 * This function is used to determine if a particular MPM interrupt is enabled
 * in HW.
 *
 * @param nMPM  The HAL_mpmint_IsrType value to be checked.
 * 
 * @return TRUE if the HW indicates this interrupt is currently enabled in HW,
 *         otherwise FALSE.
 */
bool32 HAL_mpmint_IsEnabled       ( uint32 nMPM );

#ifdef __cplusplus
}
#endif

#endif /* HAL_MPMINT_H */


