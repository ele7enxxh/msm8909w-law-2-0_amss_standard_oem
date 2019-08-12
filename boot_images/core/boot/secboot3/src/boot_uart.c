/*===========================================================================

                    BOOT UART DRIVER 

DESCRIPTION
  Contains wrapper definition for external uart drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: 
$DateTime: 
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/20/13   lm      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_uart.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 


/*===========================================================================

**  Function :  boot_uart_init

** ==========================================================================
*/
/*!
* 
* @brief
* This wrapper funcion will call the uart driver function which will
* initiate the uart driver for Tx/Rx operation.
*
*@param[in] handle handler
*
*@param[in] id Port ID
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
UartResult boot_uart_init(UartHandle* handle,UartPortID id)
{
  return Uart_init(handle, id);
}


/* ========================================================================
**  Function : boot_uart_deinit
** ======================================================================*/
/*!
* 
* @brief
* This wrapper funcion will call the uart driver function which will
* deinit the uart driver.
*
*@param[in] handle handler
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
UartResult boot_uart_deinit(UartHandle handle)
{
  return Uart_deinit(handle);
}

/* ========================================================================
**  Function : boot_uart_receive
** ======================================================================*/
/*!
* 
* @brief
* This wrapper funcion will call the uart driver receive function which will
* receive the data comming from the uart port.
*
*@param[in] handle - handler
*
*@param[in] buf - receive buffer
*
* @param[in] bytes_to_rx - receive data size

* @par Dependencies
*   None
*   
* @retval
*   Received Bytes
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_uart_rx(UartHandle handle, char* buf,uint32 bytes_to_rx)
{
  return Uart_receive(handle,buf,bytes_to_rx);
}

/* ============================================================================
**  Function : boot_uart_transmit
** ============================================================================
*/
/*!
* 
* @brief
* This wrapper funcion will call the uart driver transmit function which will
* transmit the data from the uart port.
*
*@param[in] handle - handler
*
*@param[in] buf - transmit buffer
*
* @param[in] bytes_to_tx - transmit data size

* @par Dependencies
*   None
*   
* @retval
*   Transmitted Bytes
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_uart_tx(UartHandle handle, char* buf, uint32 bytes_to_tx)
{
	return Uart_transmit(handle,buf,bytes_to_tx); 
}
