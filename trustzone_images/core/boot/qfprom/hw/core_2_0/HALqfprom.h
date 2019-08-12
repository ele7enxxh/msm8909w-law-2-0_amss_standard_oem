#ifndef HAL_QFPROM_H
#define HAL_QFPROM_H

/* ===========================================================================

FILE:         HALqfprom.h

DESCRIPTION:
    This is the HAL file for the QFPROM functionality on the chip. In it, we
    can perfrom verious functions relating to this block. Namely the reading
    and writing to fuse register.  

             Copyright  2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================

                             Edit History

$Header: 

when        who           what, where, why
--------   ------       --------------------------------------------------------
06/11/12     dh           Initial revision for 8974


=========================================================================== */

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "qfprom.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* HAL version */
#define HAL_QFPROM_VERSION_STRING "QFPROM Version 00.01.00"


/*===========================================================================

                      GLOBAL CONSTANT DECLARATIONS

===========================================================================*/



/*=============================================================================
   
                              FUNCTION DEFINITIONS

============================================================================*/


/*===========================================================================

**  Function :

** ==========================================================================
*/
/*!
*   HAL_qfprom_ReadRawAddress
* 
* @brief
*   This function will read either the lsb or msb 32 bit values from the QFPROM
*   memory. When we read from raw address we get to see the data as it is along
*   with FEC bits. The row address space is read/write. Hence HAL provides 
*   ability to read that address space. But Driver should really not use it. 
*   Sometimes like the issue of SEC_HW_KEY Driver have no choice but to use 
*   that ability.
* 
* @par Dependencies
* 
* @param
*   Which address is to be read from and a pointer to the return value.
* 
* @retval
*   The raw value read from the qfuse address
* 
* @par Side Effects
* 
*/
uint32 HAL_qfprom_ReadRawAddress(uint32 address, uint32 *value);

/*===========================================================================

**  Function : HAL_qfprom_ReadRawAddressRow

** ==========================================================================
*/
/*!
*   HAL_qfprom_ReadRawAddressRow
* 
* @brief
*   This function will read the raw  address space. The corrected address
*   space is the without FEC and if detected 1 bit error; it will correct it. 
*   
* @par Dependencies
* 
* @param
*   Which address is to be read from and a pointer to the return value.
* 
* @retval
*   The corrected value read from the qfuse address.
* 
* @par Side Effects
* 
*/
uint32 HAL_qfprom_ReadRawAddressRow(uint32 address, uint32 *value);

/*===========================================================================

**  Function :

** ==========================================================================
*/
/*!
*   HAL_qfprom_ReadCorrectedAddress
* 
* @brief
*   This function will read the corrected address space. The corrected address
*   space is the without FEC and if detected 1 bit error; it will correct it. 
*   
* @par Dependencies
* 
* @param
*   Which address is to be read from and a pointer to the return value.
* 
* @retval
*   The corrected value read from the qfuse address.
* 
* @par Side Effects
* 
*/
uint32 HAL_qfprom_ReadCorrectedAddress(uint32 address, uint32 *value);

/*===========================================================================

**  Function :HAL_qfprom_ReadCorrectedAddressRow

** ==========================================================================
*/
/*!
*   HAL_qfprom_ReadCorrectedAddressRow
* 
* @brief
*   This function will read the corrected address space. The corrected address
*   space is the without FEC and if detected 1 bit error; it will correct it. 
*   
* @par Dependencies
* 
* @param
*   Which address is to be read from and a pointer to the return value.
* 
* @retval
*   The corrected value read from the qfuse address.
* 
* @par Side Effects
* 
*/
uint32 HAL_qfprom_ReadCorrectedAddressRow(uint32 address, uint32 *value);

/*===========================================================================

**  Function :

** ==========================================================================
*/
/*!
*   HAL_qfprom_WriteRawAddress
* 
* @brief
*   This function will allow you to write 32 bits values to the given address.
*   QFPROM software is capable of writing or reading 64 bits value; we are
*   limited by AHB bus. AHB bus has data width and address width 32 bits.
* 
* @par Dependencies
*   The fuse blow voltage must be set for the blow operation to take place. 
*   Also,the blow timer must be programmed to a value such that the blow time 
*   is more than 10 uS. Otherwise, it could be that the fuse was not blown 
*   properly and you can have an unknown value.
* 
* @param
*   The address that you wish to blow, as well as the value to be written to 
*   that address.
* 
* @retval
* 
* @par Side Effects
*   Once the fuse is blown (e.g., set to a 1, it cannot be changed. Zero 
*   values, however can we written to a 1, later on.
* 
*/

uint32 HAL_qfprom_WriteRawAddress(uint32 address, uint32 value);


/*===========================================================================

**  Function :

** ==========================================================================
*/
/*!
*   HAL_qfprom_ReadBlowStatus
* 
* @brief
*   This function will tell the user if it is currently in the middle of a 
*   blow operation.
*   
* @par Dependencies
* 
* @param
*   A pointer to the return value.
* 
* @retval
*   It will be a high if the QFPROM is in the middle of a write operaion.
* 
* @par Side Effects
* 
*/
void HAL_qfprom_ReadBlowStatus(uint32* value);


/*===========================================================================

**  Function :

** ==========================================================================
*/
/*!
*    HAL_qfprom_IsErrorSeen
*   
* @brief
*   The function checks for the ERR_SEEN bit of the FEC_ESR register to determine
*   if there was an error.
* 
* @par Dependencies
* 
* @param
*   None
* 
* @retval
*   TRUE if error detected, FALSE otherwise
*   
* @par Side Effects
*   Clears the register if error detected
* 
*/
boolean HAL_qfprom_IsFecErrorSeen( void );

/*===========================================================================

**  Function : HAL_qfprom_SetBlowTimer

** ==========================================================================
*/
/*!
*   HAL_qfprom_SetBlowTimer    
* 
* @brief
*   This will initialize the QFPROM for the Read and write of the Fuse.
*   Program the QFPROM_BLOW_TIMER(). The value of the blow timer is
*   dependant on the clock frequency. We want to have the timer programmed to
*   10 microseconds. 
* 
* @par Dependencies
*   The value being written to the timer must be atleast 10 uS.
* 
* @param
*   The value to be written to the timer. 
* @retval
* 
*   
* @par Side Effects
*     None
* 
*/
void HAL_qfprom_SetBlowTimer(uint32 value);


/*===========================================================================

**  Function :HAL_qfprom_SetAccelator

** ==========================================================================
*/
/*!
*   HAL_qfprom_SetAccelator
* 
* @brief
*   This function allows to write to QFPROM_ACCEL register in order to
*   accelarate the timing if needed for QFPROM. This is also external function. 
*   Because it won't be used very often but driver can use it if tweaking is
*     needed for some target.  
* 
* @par Dependencies
* 
* @param
*   The value being written to the QFPROM_ACCEL register. This register 
*   controls the accelerator bits for the QFPROM. These bits can be used 
*   to tweak the timing of the QFPROM if necessary, though they are unlikely 
*   to be needed.
* 
* @retval
* 
* @par Side Effects
* 
*/
void HAL_qfprom_SetAccelator(uint32 value);


/*===========================================================================

**  Function : HAL_qfprom_EnableClock

** ==========================================================================
*/
/*!
*   HAL_qfprom_EnableClock
* 
* @brief
*   This function configures the sec control clock to 2.4Mhz for fuse blow. 
*   
* @par Dependencies
* 
* @param
* 
* @retval
* 
* @par Side Effects
* 
*/
void HAL_qfprom_EnableClock(void);


/*===========================================================================

**  Function : HAL_qfprom_DisableClock

** ==========================================================================
*/
/*!
*   HAL_qfprom_DisableClock
* 
* @brief
*   This function restores the clock to the QFPROM
*   
* @retval
* 
* @par Side Effects
* 
*/
void HAL_qfprom_DisableClock(void);


#endif /* HAL_QFPROM_H */
