/* ===========================================================================

FILE:         HALqfprom.c

DESCRIPTION:
    This is the HAL file for the QFPROM functionality on the chip. In it, we
    can perfrom various functions relating to this block. Namely the reading
    and writing to fuse register.  

===========================================================================

                             Edit History

$Header: 
when         who           what, where, why
--------   -------     --------------------------------------------------------
06/11/12     dh           Initial revision for 8974
===========================================================================
             Copyright  2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include QFPROM_HWIO_REG_INCLUDE_H
#include "msmhwio.h"
#include "qfprom.h"
#include QFPROM_TARGET_INCLUDE_H
#include "HALqfprom.h"

/*============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

============================================================================*/

/*=============================================================================
   
                             GLOBAL  FUNCTION DEFINITIONS

============================================================================*/
static uint32 qfprom_sec_ctl_clock_backuped = FALSE;
static uint32 qfprom_sec_ctl_clock_backup_value = 0;

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
void HAL_qfprom_SetBlowTimer(uint32 value)
{
    HWIO_OUT(QFPROM_BLOW_TIMER, value); 
} /*  *** end HAL_qfprom_InitBlowTimer */


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
void HAL_qfprom_SetAccelator(uint32 value)
{
    HWIO_OUT(QFPROM_ACCEL, value);
} /*  *** end HAL_qfprom_EnableAccelator */


/*===========================================================================

**  Function : HAL_qfprom_ReadRawAddress

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
uint32 HAL_qfprom_ReadRawAddress(uint32 address, uint32 *value)
{
    uint32 return_value = QFPROM_ERR_UNKNOWN;
    uint32 test_val = 0x0;

    if( value == NULL)
    {
      return QFPROM_DATA_PTR_NULL_ERR;
    }
    
    test_val = (address & 0xF); /* Check for 32bit alignment */

    if((test_val & ~3) == test_val)
    {
        *value = in_dword(address);
        return_value = QFPROM_NO_ERR;
    }
    else
    {
        *value = 0;
        return_value = QFPROM_ADDRESS_INVALID_ERR;
    }   

    return return_value;   
} /*  *** end HAL_qfprom_ReadRawAddress */


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
uint32 HAL_qfprom_ReadRawAddressRow(uint32 address, uint32 *value)
{
    uint32 return_value = QFPROM_ERR_UNKNOWN;
 
    if( value == NULL)
    {
      return QFPROM_DATA_PTR_NULL_ERR;
    }
    
    do
    {
        return_value = HAL_qfprom_ReadRawAddress( address, value);

        if (return_value != QFPROM_NO_ERR )
        {
            break;
        }                    

        value++;
        return_value = HAL_qfprom_ReadRawAddress( address+4, value);  
    }while(0);

    return return_value;


} /*  *** end HAL_qfprom_ReadRawAddressRow */


/*===========================================================================

**  Function : HAL_qfprom_ReadCorrectedAddress

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
uint32 HAL_qfprom_ReadCorrectedAddress(uint32 address, uint32 *value)
{
    uint32 return_value = QFPROM_ERR_UNKNOWN;
    uint32 test_val = 0x0;
    
    if( value == NULL)
    {
      return QFPROM_DATA_PTR_NULL_ERR;
    }
    
    /* Check for 32bit alignment */
    test_val = (address & 0xF); 

    if((test_val & ~3) == test_val)
    {
        *value = in_dword(address);
        return_value = QFPROM_NO_ERR;        
    }
    else
    {
        *value = 0;
        return_value = QFPROM_ADDRESS_INVALID_ERR;
    }

    return return_value;   
} /*  *** end HAL_qfprom_ReadCorrectedAddress */


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
uint32 HAL_qfprom_ReadCorrectedAddressRow(uint32 address, uint32 *value)
{
    uint32 return_value = QFPROM_ERR_UNKNOWN;

    if( value == NULL)
    {
      return QFPROM_DATA_PTR_NULL_ERR;
    }
    
    return_value = HAL_qfprom_ReadCorrectedAddress( address, value);

    if (return_value == QFPROM_NO_ERR )
    {
       value++;
       return_value = HAL_qfprom_ReadCorrectedAddress( address+4, value); 
     }                    

    return return_value;
} /*  *** end HAL_qfprom_ReadCorrectedAddressRow */

/*===========================================================================

**  Function : HAL_qfprom_WriteRawAddress

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
uint32 HAL_qfprom_WriteRawAddress(uint32 address, uint32 value)
{
    uint32 return_value = QFPROM_ERR_UNKNOWN;
    uint32 test_val = 0x0;
    
    /* Check for 32bit alignment */
    test_val = (address & 0xF);

    if((test_val & ~3) == test_val)
    {
#ifdef FEATURE_QFUSE_PROGRAMMING
        out_dword(address, value);
        return_value = QFPROM_NO_ERR;
#else
        return_value = QFPROM_OPERATION_NOT_ALLOWED_ERR;
#endif
    }
    else
    {
        return_value = QFPROM_ADDRESS_INVALID_ERR;
    }   

    return return_value;   
} /*  *** end HAL_qfprom_WriteRawAddress */


/*===========================================================================

**  Function : HAL_qfprom_ReadBlowStatus

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
void HAL_qfprom_ReadBlowStatus(uint32* value)
{
   if(value != NULL)
    {
       *value = HWIO_IN(QFPROM_BLOW_STATUS);
    }
} /*  *** end HAL_qfprom_ReadBlowStatus */


/*===========================================================================

**  Function : HAL_qfprom_ClearFecErrorStatus

** ==========================================================================
*/
/*!
*   HAL_qfprom_ClearErrorStatus
* 
* @brief
*   EFC_ESR register stores error status for the first error occurred. Once
*   the status register is read then we need to clear tha bit in order to 
*   detect further errors. Hence this write function. 
*   
* @par Dependencies
* 
* @param
*   The value to be written to the error status register. The bits that are 1, 
*   will clear that bit's error.
*   
* @retval
*   
* @par Side Effects
* 
*/
void HAL_qfprom_ClearFecErrorStatus(uint16 value)
{
      HWIO_OUT(FEC_ESR, value);

} /*  *** end HAL_qfprom_ClearErrorStatus */


/*===========================================================================

**  Function : HAL_qfprom_IsFecErrorSeen

** ==========================================================================
*/
/*!
*    HAL_qfprom_IsFecErrorSeen
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
boolean HAL_qfprom_IsFecErrorSeen( void )
{
  uint16 value = HWIO_INF(FEC_ESR, ERR_SEEN);
  
  /* If zero, error not detected */  
  if(value == 0 )
  {
    return FALSE; 
  }
  else
  {
    /* Clear the FEC_ESR register */
    HAL_qfprom_ClearFecErrorStatus(value);
    return TRUE; 
  }
} /* HAL_qfprom_IsErrorSeen */



/*===========================================================================

**  Function : HAL_qfprom_ReadErrorAddress

** ==========================================================================
*/
/*!
*   HAL_qfprom_ReadErrorAddress
* 
* @brief
*   This error address register stores the address at which the first error and
*   correction occur. These are read only bits. Note that the address refers to
*   the row of the QFPROM which was accessed to cause the error/correction.
* 
* @par Dependencies
*   Clearing the related bit in the FEC_ESR register will also clear the 
*   captured address in this register so it is important to not do that
*     before trying to find the error address.
* 
* @param
*   A pointer to the return value.
*   
* @retval
*   This function will return the address of the error.
* 
* @par Side Effects
* 
*/
void HAL_qfprom_ReadErrorAddress(uint16* value)
{
  if(value != NULL)
  {
    *value = HWIO_INF(FEC_EAR, ERR_ADDR);
  }
} /*  *** end HAL_qfprom_ReadErrorAddress */


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
void HAL_qfprom_EnableClock(void) 
{
  /* backup the clock control register value first */
  qfprom_sec_ctl_clock_backup_value = HWIO_IN(GCC_SEC_CTRL_CFG_RCGR);
  qfprom_sec_ctl_clock_backuped = TRUE;
  
   /* set for src=0, divider = 15 (aka div8) */
  HWIO_OUT(GCC_SEC_CTRL_CFG_RCGR,  0xF);
  
  /* start the clock switch */
  HWIO_OUTF(GCC_SEC_CTRL_CMD_RCGR, UPDATE, 1);
  
  /* wait for the clock switch to complete */
  while(HWIO_INF(GCC_SEC_CTRL_CMD_RCGR, UPDATE) == 1); 

} /*  *** end HAL_qfprom_EnableClock */


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
void HAL_qfprom_DisableClock(void) 
{
  /* Restore clock control register if there is a back up */
  if(qfprom_sec_ctl_clock_backuped == TRUE)
  {
    HWIO_OUT(GCC_SEC_CTRL_CFG_RCGR,  qfprom_sec_ctl_clock_backup_value);
    
    /* start the clock switch */
    HWIO_OUTF(GCC_SEC_CTRL_CMD_RCGR, UPDATE, 1);
  
    /*wait for the clock switch to complete */
    while(HWIO_INF(GCC_SEC_CTRL_CMD_RCGR, UPDATE) == 1);
    
    qfprom_sec_ctl_clock_backuped  = FALSE;
  }
} /*  *** end HAL_qfprom_DisableClock */


