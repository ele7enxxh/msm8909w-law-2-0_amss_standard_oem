/*===========================================================================

                    BOOT EXTERN MHI DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external mhi drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_mhi_interface.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/13   kedara  use mhi_boot_get_max_raw_data_size for raw data transfer.
09/16/13   kedara  Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_extern_mhi_interface.h"
#include "mhi_boot.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

**  Function :  boot_mhi_init

** ==========================================================================
*/
/*!
 * 
 * @brief   This function initializes the MHI, PCIe hardware.
 *
 * @details This function initializes the Mhi. PCIe hardware.
 *
 * 
 * @return  None.
 * 
 */
void boot_mhi_init( void )
{
  mhi_boot_init();
}



/*===========================================================================

**  Function :  boot_mhi_poll

** ==========================================================================
*/
/*!
 * 
 * @brief   This function polls the hardware for events.
 *
 * @details This function polls the hardware for events.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
void boot_mhi_poll(void)
{
  mhi_boot_poll();
}


/*===========================================================================

**  Function :  mhi_shutdown

** ==========================================================================
*/
/*!
 * 
 * @brief   This function performs a shutdown of the hardware.
 *
 * @details This function performs a shutdown of the hardware.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
void boot_mhi_shutdown(void)
{
  mhi_boot_shutdown();
}


/*===========================================================================

**  Function :  boot_mhi_receive

** ==========================================================================
*/
/*!
 * 
 * @brief   This function receives a bulk of data upto the input length.
 *
 * @details This function will continue to poll the hardware until the receive
 *          URB is not NULL, upon which the data will be copied to the
 *          input receive buffer.
 * 
 * @param   *rx_buf     -  [IN/OUT] Receive buffer for data, must be 4 byte aligned 
 * @param   len         -  [IN    ] Length of buffer, must be multiples of max packet size
 * @param   rx_cb       -  [IN    ] Return callback function
 * @param   *err_code   -  [IN/OUT] Any error that occurs during transfer
 * @param   type        -  [IN    ] Type of packet
 * 
 * @return  Number of bytes receieved.
 * 
 */
uint32 boot_mhi_receive(byte *rx_buf, 
                        uint32 len, 
                        sahara_rx_cb_type rx_cb, 
                        uint32 * err_code,
                        enum boot_sahara_packet_type type )
{
  return mhi_boot_rx_bulk(rx_buf, len, rx_cb, err_code, type);
}


/*===========================================================================

**  Function :  boot_mhi_transmit

** ==========================================================================
*/
/*!
 * 
 * @brief   This function transmits a bulk of data upto the input length.
 *
 * @details This function will continue to poll the hardware until the transmit
 *          URB transfer is complete.
 * 
 * @param   *tx_buf     -  [IN/OUT] Transmit buffer for data, must be 4 byte aligned 
 * @param   len         -  [IN    ] Length of data to trasmit in bytes
 * @param   tx_cb       -  [IN    ] Transmit complete callback
 * @param   *err_code   -  [IN/OUT] Any error that occurs during transfer
 * 
 * @return  Number of bytes transmitted.
 * 
 */
uint32 boot_mhi_transmit(byte *tx_buf,    
                          uint32 len,       
                          sahara_tx_cb_type tx_cb, 
                          uint32 * err_code)
{
  return mhi_boot_tx_bulk(tx_buf, len, tx_cb, err_code);
}


/*===========================================================================

**  Function :  boot_mhi_get_max_packet_size

** ==========================================================================
*/
/*!
 * 
 * @brief   This function returns whether PCIe spec defined max packet sizes
 *          to be used
 *
 * @details Correct packet sizes are determined internally and will be
 *          returned to external interface
 *          
 * @return  Max packet sizes
 * 
 */
uint32 boot_mhi_get_max_packet_size(void)
{
  return mhi_boot_get_max_packet_size();
}


/*===========================================================================

**  Function :  mhi_get_max_raw_data_size

** ==========================================================================
*/
/*!
 * 
 * @brief   Returns the maximum bulk transfer level size that can be requested
 *          by the client
 *
 * @details Windows only support maximum bulk transfer size of 1MB, whereas
 *          Linux supports a larger maximum 
 *          
 * @return  Max PCIe bulk transfer level size
 * 
 */
uint32 boot_mhi_get_max_raw_data_size(void)
{
  return mhi_boot_get_max_raw_data_size();
}
