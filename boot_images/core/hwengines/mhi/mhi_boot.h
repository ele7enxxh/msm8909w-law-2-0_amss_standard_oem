/** 
  @file mhi_boot.c
  @brief
  This header file contains the boot interface implementation layer
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/22/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi.h" 

/** 
 * @brief      mhi_boot_init
 *
 *             Initialize the boot wrapper
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             BULK_SUCCESS if successful
 *             BULK_ERR_ENUMERATION if channel could not be opened 
 *             BULK_ERR_UNKNOWN for all other errors
 * 
 * @sideeffect None 
 */
uint32 mhi_boot_init(void);

/** 
 * @brief      mhi_boot_shutdown
 *
 *             Shutdown MHI boot. This involves any clean-up such as saving 
 *             event ring/command rings to HOST memory to allow hand-over to 
 *             AMSS
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_boot_shutdown(void);

/** 
 * @brief      mhi_boot_poll
 *
 *             Poll for any new data
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_boot_poll(void);

/** 
 * @brief      mhi_boot_rx_bulk
 *
 *             Read bulk data over SAHARA RX channel
 *              
 * 
 * @param[in]  *rx_buf    - Pointer to RX buffer
 * @param[in]  len        - Length of buffer in bytes
 * @param[in]  rx_cb      - CB to be called when operation is completed 
 * @param[out]  *err_code - Set if any error occurred
 * @param[in]  tyoe       - Specifies the packet type
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Bytes read if no CB is specified
 * 
 * @sideeffect None 
 */
uint32 mhi_boot_rx_bulk(uint8 *rx_buf,
      uint32 len,
      sahara_rx_cb_type rx_cb,
      uint32 * err_code,
      enum boot_sahara_packet_type type);

/** 
 * @brief      mhi_boot_tx_bulk
 *
 *             Transmits bulk data over SAHARA TX channel
 *              
 * 
 * @param[in]  *tx_buf   - Pointer to tx buffer
 * @param[in]  len       - Length of data in bytes
 * @param[in]  tx_cb     - CB to be called when operation is completed
 * @param[out] *err_code - Set if any error occurred 
 *
 * @dependencies
 *             mhi_boot_init must be called first
 *              
 * 
 * @return       
 *             Number of bytes written if CB is not specified
 * 
 * @sideeffect None 
 */
uint32 mhi_boot_tx_bulk(uint8 *tx_buf,
      uint32 len,
      sahara_tx_cb_type tx_cb,
      uint32 * err_code);

/** 
 * @brief      mhi_boot_get_max_packet_size
 *
 *             Return MAX packet size in bytes 
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Max packet size
 * 
 * @sideeffect None 
 */
uint32 mhi_boot_get_max_packet_size(void);

/** 
 * @brief      mhi_boot_get_max_raw_data_size
 *
 *             Return max raw data size. This is used to determine max raw binary 
 *             data size that can be read at one time 
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
uint32 mhi_boot_get_max_raw_data_size(void);
